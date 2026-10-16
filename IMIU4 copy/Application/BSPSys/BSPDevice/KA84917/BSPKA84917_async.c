/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPKA84917_async.c
** @Founder      : WYP40505
** @Date         : 2026.05.06
** @Function     : KA84917驱动模块 — 异步SPI版本
** @Instructions : 基于spi_async.h定时器中断+DMA状态机, 非阻塞通信
**                 与原BSPKA84917.c的区别:
**                 - vSPI_2byte_read/write/Continue_Read/FifoContRead 改为入队+等待
**                 - KA84917_maintask() 不再阻塞等待SPI完成
**                 - 采样周期: 主循环10ms, 分片轮转(4片×2周期=80ms完整一轮)
**===========================================================================================*/
#if 1
#include "main.h"
#include "BSPKA84917_async.h"
// #include "BSPKA84917.h"
#include "BSPDIDO.h"
#include "BSPSPI.h"
#include "spi_async.h"
#include "crc.h"
#include "drv_rn8209_interface.h"
#include "BSPIRQ.h"
#include "SystemDiag.h"
#include "ToolingCheck.h"
#include "BSPTIM.h"
#include "tim.h"
#include "TEST.h"
#include "Measure.h"

/*=============================================================================================
** 全局变量
**===========================================================================================*/
static u8 IMD_RLY_STATE  = 0;
UCHAR wakeUpState = 0;
u8 gWakeUpStateInitFlag922;
u8 gWakeUpStateInitFlag917;
u32 Rn8209AdData[2];
u8 FirstSampleFinishedFlag = 0;

/*===============================================================================================
** 主任务状态机 — 采样周期: 10ms主循环 + 分片轮转 — 宏定义及静态变量
**=============================================================================================*/
#define MAX_WAKE_RERTY      10
#define MAX_INIT917_RERTY   10
#define MAX_INIT8209_RERTY  10

static u8 VoltSampStep = 0xaa;
static u8 WakeRetryCnt = 0;
static u8 Init917RetryCnt = 0;
static u8 Init8209RetryCnt = 0;
static u8 gChunkDivider = 0;            /* chunk divider (1 per 2 cycles) */
static TickType_t xLastWakeTime = 0;     /* vTaskDelayUntil abs time base */
static u8 AfterWakeupFirstSample = 0;    /* first full round flag: 0=init phase, 1=data valid */
static u8 gSampleChunkIdx = 0;          /* chunk index 0-3 */


/*=============================================================================================
** ===  异步SPI (入队+等待) ===
**===========================================================================================*/

/* 2字节写 — 异步入队 */
u8 vSPI_2byte_write(u8 ucdev, u8 ucreg, u16 usdata)
{
    if (ucdev > BMIC_ADDRESS_922 && ucdev != BMIC_GRPID_917) {
        return FALSE;
    }
    vSPI_ReqSet(ucdev, SPI_REQ_2BYTE_WRITE, ucreg, usdata, NO_MASK);
    return TRUE;
}

/* 带掩码写 — 异步入队 */
u8 vSPI_2byte_write_Mask(u8 ucdev, u8 ucreg, u16 usdata, u16 usMask)
{
    if (ucdev > BMIC_ADDRESS_922 && ucdev != BMIC_GRPID_917) {
        return FALSE;
    }
    vSPI_ReqSet(ucdev, SPI_REQ_2BYTE_WRITE, ucreg, usdata, usMask);
    return TRUE;
}

/* 2字节读 — 异步入队+等待完成 */
u8 vSPI_2byte_read(u8 ucdev, u8 ucreg)
{
    if (ucdev > BMIC_ADDRESS_922 && ucdev != BMIC_GRPID_917) {
        return FALSE;
    }
    vSPI_ReqSet(ucdev, SPI_REQ_2BYTE_READ, ucreg, 0x1u, NO_MASK);
    /* 等待此请求完成 (超时5ms) */
    if (ucSPI_WaitAllDone(5u) != SPI_NOERR) {
        // SEGGER_RTT_printf(0, "READ_FAIL dev=%02x reg=%02x err=%02x\r\n", 
        //     ucdev, ucreg, ucSPI_GetComError());
        return FALSE;
    }
    return TRUE;
}

/* 连续读 — 异步入队+等待完成 */
u8 vSPI_Continue_Read(u8 ucdev, u8 ucreg, u8 size)
{
    if (ucdev > BMIC_ADDRESS_922 && ucdev != BMIC_GRPID_917) {
        return FALSE;
    }
    if (size > 0x20) {
        return FALSE;
    }
    vSPI_ReqSet(ucdev, SPI_REQ_CONTINUE_READ, ucreg, (USHORT)size, NO_MASK);
    /* 等待此请求完成 (超时10ms) */
    if (ucSPI_WaitAllDone(10u) != SPI_NOERR) {
        return FALSE;
    }
    return TRUE;
}

/* FIFO连续读 — 异步入队+等待完成 */
u8 vSPI_FifoContRead(u8 ucdev, u8 ucreg, u8 size)
{
    if (ucdev > BMIC_ADDRESS_922 && ucdev != BMIC_GRPID_917) {
        return FALSE;
    }
    if (size > 0x20) {
        return FALSE;
    }
    vSPI_ReqSet(ucdev, SPI_REQ_FIFO_CONT_READ, ucreg, (USHORT)size, NO_MASK);
    /* 等待此请求完成 (超时10ms) */
    if (ucSPI_WaitAllDone(10u) != SPI_NOERR) {
        // SEGGER_RTT_printf(0, "READ_FAIL dev=%02x reg=%02x err=%02x\r\n", 
        //     ucdev, ucreg, ucSPI_GetComError());
        return FALSE;
    }
    return TRUE;
}

/*=============================================================================================
** ===  异步SPI (只入队不等待) — 用于批量提交后统一等待 ===
**===========================================================================================*/

/* 2字节写 — 只入队不等待 */
u8 vSPI_2byte_write_NoWait(u8 ucdev, u8 ucreg, u16 usdata)
{
    if (ucdev > BMIC_ADDRESS_922 && ucdev != BMIC_GRPID_917) {
        return FALSE;
    }
    vSPI_ReqSet(ucdev, SPI_REQ_2BYTE_WRITE, ucreg, usdata, NO_MASK);
    return TRUE;
}

/* 带掩码写 — 只入队不等待 */
u8 vSPI_2byte_write_Mask_NoWait(u8 ucdev, u8 ucreg, u16 usdata, u16 usMask)
{
    if (ucdev > BMIC_ADDRESS_922 && ucdev != BMIC_GRPID_917) {
        return FALSE;
    }
    vSPI_ReqSet(ucdev, SPI_REQ_2BYTE_WRITE, ucreg, usdata, usMask);
    return TRUE;
}

/* 2字节读 — 只入队不等待 */
u8 vSPI_2byte_read_NoWait(u8 ucdev, u8 ucreg)
{
    if (ucdev > BMIC_ADDRESS_922 && ucdev != BMIC_GRPID_917) {
        return FALSE;
    }
    vSPI_ReqSet(ucdev, SPI_REQ_2BYTE_READ, ucreg, 0x1u, NO_MASK);
    return TRUE;
}

/*=============================================================================================
** SPI2_Send_Byte
**===========================================================================================*/
u8 SPI2_Send_Byte(u8 u8Byte, u8 *u8ReadByte)
{
    u16 datatmp;
    datatmp = (u16)(u8Byte << 8) | 0xAA;

    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_35,
            CMIC_BIT_LENGTH_1_7|CMIC_BIT_GPSPI_CS1_N|CMIC_BIT_GPSPI_RATE2, 0x7FF)) {
        return SPI_STATUS_FAIL;
    }
    vSPI_2byte_read(BMIC_ADDRESS_917, 0x30);
    vSPI_2byte_read(BMIC_ADDRESS_917, 0x35);

    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, datatmp)) {
        return SPI_STATUS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        return SPI_STATUS_FAIL;
    }

    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    *u8ReadByte = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);
    return SPI_STATUS_OK;
}

/*=============================================================================================
** 继电器控制
**===========================================================================================*/
void IMD_RLY_Open(void)  { IMD_RLY_STATE = 0; }
void IMD_RLY_Close(void) { IMD_RLY_STATE = 1; }
u8 GetIMD_RLY_State(void) { return IMD_RLY_STATE; }

/*=============================================================================================
** KA84917初始化配置 (阻塞方式, 使用vSPI_2byte_write)
**===========================================================================================*/
u8 KA84917_DRV_Init(TickType_t xLastWakeTime)
{
   
    vSPI_2byte_write(BMIC_GRPID_922, BMIC_CNTADR_0B, BMIC_BST_MODE01); /* 通过922控制BST_MODE  决定连续读取时SDO电平变化  必须fDAISY> fSCK*/
 
    vSPI_2byte_write(BMIC_GRPID_917, CMIC_CNTADR_5C,
    (CMIC_NPD_C_P_ON | CMIC_NPD_V_P_ON | CMIC_NPD_C_S_ON | CMIC_NPD_OCD_P_ON |
        CMIC_NPD_OCD_S_ON | CMIC_NPD_AUX_ON | CMIC_AUXMODE_01 | CMIC_DSP_C_P_FR |
        CMIC_DSP_V_P_FR | CMIC_DSP_C_S_FR));   /* ADC ON 启动测量电路电源, Free on mode 设置测量模式*/
    
    vSPI_2byte_write(BMIC_GRPID_917, CMIC_CNTADR_5D,
    (CMIC_CC_INTVL_P_850 | CMIC_CC_INTVL_S_850 | CMIC_REG_5D_RESERVED1)); /* 850us meas time 设置转换时间*/
   
    vSPI_2byte_write(BMIC_GRPID_917, CMIC_CNTADR_60,
    (CMIC_OCNF_P_1000 | CMIC_OCTH_EN_P_PAN | CMIC_OCTHP_P_25 | CMIC_OCTHN_P_25)); /* NF:1000us, OC threshold:+/-25mV 设置过流参数*/
    vSPI_2byte_write(BMIC_GRPID_917, CMIC_CNTADR_61,
    (CMIC_OCNF_S_1000 | CMIC_OCTH_EN_S_PAN | CMIC_OCTHP_S_25 | CMIC_OCTHN_S_25));
    
    vSPI_2byte_write(BMIC_GRPID_917, CMIC_CNTADR_5E,
    (CMIC_GAIN_PROG_P_4 | CMIC_GAIN_PROG_S_4 | CMIC_EXOSC_CHDRV_5MA)); /* Gain x4, XOdrv:5mA 设置电流ADC增益*/
    
    vSPI_2byte_write(BMIC_GRPID_917, CMIC_CNTADR_68,
    (CMIC_FR_ON_ON | CMIC_AUX_ON_ON | CMIC_OCD_ON_P_ON | CMIC_OCD_ON_S_ON));/* Start OC1, OC2, VBAT, AUX, OCD1, OCD2 measurement 启动测量*/
  
    vSPI_2byte_write(BMIC_GRPID_917, BMIC_CNTADR_16,
    (BMIC_ALM_TMOUT_20 | BMIC_INTTIM_1 | BMIC_ALM_SHDN_NOSHIFT |
        BMIC_CALM_OE_ON | BMIC_CALM_DIR_1 | BMIC_CALM_IE_ON));  /*COM-alarm output*/

    //寄存器配置后等待生效时间
    delayms(12);
   // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));


    vSPI_2byte_write(BMIC_GRPID_917, CMIC_CNTADR_17, (CMIC_PLL_NPD_ON | CMIC_EXOSC_NPD_ON));/* PLL ON */
    //PLL上电等待时间
    delayms(1);
    // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));

 
    vSPI_2byte_write(BMIC_GRPID_917, CMIC_CNTADR_17, (CMIC_PLLCKSEL_PLL | CMIC_PLL_NPD_ON | CMIC_EXOSC_NPD_ON));   /* PLL connect */
    //PLL锁定等待时间
    delayms(1);
    // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));


    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_CNTADR_0C, CMIC_PWM3_SET_GPIO)) {
        return FALSE;
    }
    ucSPI_WaitAllDone(5u);

    vSPI_2byte_write(BMIC_ADDRESS_917, BMIC_CNTGPIO98, BMIC_GPIO1_3_OEN_DISABLE);
    vSPI_2byte_write(BMIC_ADDRESS_917, BMIC_CNTGPIO99, BMIC_GPIO4_6_OEN_DISABLE);
    ucSPI_WaitAllDone(5u);

    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, BMIC_MCELL_ADR66, CMIC_GPIO1_5_ENABLE_ADC)) {
        return FALSE;
    }
    ucSPI_WaitAllDone(5u);
    // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(15));

    return TRUE;
}

/*=============================================================================================
** 主任务状态机 — 采样周期: 10ms主循环 + 分片轮转 */

void KA84917_maintask(void)
{
    if (xLastWakeTime == 0) {
        xLastWakeTime = xTaskGetTickCount();
    }

/* 初始化 创建完成信号量 仅执行一次*/
    static u8 init_flag = 0;
    if(!init_flag){
        vSPI_Async_Init();
        //初始化TIM6        
        MX_TIM6_Init();
        HAL_TIM_Base_Start_IT(&htim6);
        HAL_TIM_Base_Start(&htim6);
        init_flag = 1;
    }

/* 继电器控制 — 初始化成功后可控 */
    if (wakeUpState == 1)
    {
        if (IMD_RLY_STATE == 0) {
            vSPI_2byte_write(BMIC_ADDRESS_917, BMIC_CNTGPIO9B, BMIC_GPIO10_OEN_DISABLE);
        } else {
            vSPI_2byte_write(BMIC_ADDRESS_917, BMIC_CNTGPIO9B, BMIC_GPIO10_OUT_HIGH|BMIC_GPIO10_OEN_ENABLE);
        }
    }
    
/* IC初始化与采样 */
    switch (VoltSampStep) {
    case 0xaa:
        wakeUpState = 0;
        gWakeUpStateInitFlag922 = 0;
        gWakeUpStateInitFlag917 = 0;
        gChunkDivider = 0;
        VoltSampStep = EvtWake;
        osDelay(100);
        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(103));
        break;

    case EvtWake:
        vSPI_2byte_write(BMIC_COMMONSEND, BMIC_CNTWAKE, BMIC_WAKEUP);
        osDelay(12);
        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(15));
        vSPI_2byte_write(BMIC_COMMONSEND, BMIC_CNTADR_00, (BMIC_DCI_RATE_2560|0x0002));
        osDelay(12);
        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(15));

        vSPI_2byte_read(BMIC_ADDRESS_917, BMIC_CNTADR_00);
        vSPI_2byte_read(BMIC_ADDRESS_917, BMIC_CNTGRP_ID);
        vSPI_2byte_read(BMIC_ADDRESS_922, BMIC_CNTADR_00);

        if ((usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][BMIC_CNTADR_00] == (BMIC_DCI_RATE_2560|BMIC_ADDRESS_917))
            && (usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][BMIC_CNTGRP_ID] == BMIC_GRPID_917)
            && (usSPI_Reg_ReadData[KA84922_ARRAY_INDEX][BMIC_CNTADR_00] == (BMIC_DCI_RATE_2560|BMIC_ADDRESS_922)))
        {
            WakeRetryCnt = 0;
            wakeUpState = 1;
            AfterWakeupFirstSample = 0;//初始化成功后 完整采集一轮数据前 上传报文为默认值
            gWakeUpStateInitFlag922 = 1;
            gWakeUpStateInitFlag917 = 1;
            DiagSetErrNowStateAPI(eERR_ID_917_INT, eErr_A);
            VoltSampStep = EvtInit917;
            // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3));
        } else {
            WakeRetryCnt++;
            if (WakeRetryCnt >= MAX_WAKE_RERTY) {
                WakeRetryCnt = 0;
                wakeUpState = 0;
                gWakeUpStateInitFlag922 = 0;
                gWakeUpStateInitFlag917 = 0;
                DiagSetErrNowStateAPI(eERR_ID_917_INT, eErr_B);
                VoltSampStep = EvtError;
                // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3));
            } else {
                VoltSampStep = EvtWake;
                osDelay(1);
                // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1));
            }
        }
        
        break;

    case EvtInit917:
        if (1 == wakeUpState) {
            if (TRUE == KA84917_DRV_Init(xLastWakeTime)) {
                Init917RetryCnt = 0;
                DiagSetErrNowStateAPI(eERR_ID_917_INT, eErr_A);
                VoltSampStep = EvtInit8209;
                // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3));
            } else {
                Init917RetryCnt++;
                if (Init917RetryCnt >= MAX_INIT917_RERTY) {
                    Init917RetryCnt = 0;
                    DiagSetErrNowStateAPI(eERR_ID_917_INT, eErr_B);
                    VoltSampStep = EvtError;
                    // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3));
                } else {
                    VoltSampStep = EvtInit917;
                    //vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3));
                }
            }
        } else {
            VoltSampStep = EvtWake;
            // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3));
        }
        break;

    case EvtInit8209:
        if (1 == RN8209_DRV_Init(xLastWakeTime)) {
            Init8209RetryCnt = 0;
            DiagSetErrNowStateAPI(eERR_ID_820_COM, eErr_A);
            RN8209ReadCMD_DEVICE_ID();
            VoltSampStep = EvtSys;
            // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(15));
            
        } else {
            Init8209RetryCnt++;
            if (Init8209RetryCnt >= MAX_INIT8209_RERTY) {
                Init8209RetryCnt = 0;
                DiagSetErrNowStateAPI(eERR_ID_820_COM, eErr_B);
                VoltSampStep = EvtError;
                // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(15));
            } else {
                VoltSampStep = EvtInit8209;
                // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(15));
            }
        }
        break;

    case EvtSys:
    {
        /* 系统配置寄存器读取 — 仅在初始化后执行一次 */
        vSPI_Continue_Read(BMIC_ADDRESS_917, CMIC_CNTADR_5C, ((CMIC_CNTADR_68 - CMIC_CNTADR_5C) + 1u));
        vSPI_Continue_Read(BMIC_ADDRESS_917, BMIC_CNTGPIO98, ((BMIC_REG_A1 - BMIC_CNTGPIO98) + 1u));
        vSPI_Continue_Read(BMIC_ADDRESS_917, BMIC_CNTGRP_ID, ((BMIC_REG_BF - BMIC_CNTGRP_ID) + 1u));

        /* 清除初始化期间可能产生的通信错误标志, 为主循环提供干净的0x2C起点 */
        vSPI_2byte_write(BMIC_ADDRESS_917, BMIC_CNTADR_09, BMIC_CLR_FLAG);
        ucSPI_WaitAllDone(5u);
        VoltSampStep = EvtFastSample;  /* 初始化完成后进入主采样循环 */
        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));
    }
    break;

    /*=================================================================================================
    ** 主采样循环 (周期=10ms, 由StartTask02的vTaskDelayUntil保证)
    **   每周期必执行: AD锁存 + 3个分流器电流(FIFO C7/C9 + RN8209 IA)
    **   分片轮转(每2周期采集1个分片, 共4分片, 80ms完成完整一轮):
    **     Chunk0→BatVolt/HvP5v/HvN5v/ShuntTemp
    **     Chunk1→NPE FIFO
    **     Chunk2→RN8209电压通道
    **     Chunk3→通信健康检查 + 设置完整轮转就绪标志
    **=================================================================================================*/
    case EvtFastSample:
    {
        u8 bSkipTrack = 0;  /* Comm失败时跳过 */
        TickType_t fast_start, fast_end;
        fast_start = xTaskGetTickCount();  /* 超时保护: 采样时间>50ms则复位SPI+DMA */
        
#ifdef async_TEST_ENABLE
static u8 initTimeDone = 0;
if (initTimeDone != 1)
{
 initTimeDone = 1;

 SEGGER_RTT_printf(0,"[TEST1] All IC init success, dt=%dms from scheduler start\r\n",
        (fast_start - 0) * portTICK_PERIOD_MS);
    
}
#endif

        /* === 1. 主循环核心(每10ms): 3个分流器电流 === */
        vSPI_FifoContRead(BMIC_ADDRESS_917, CMIC_FIFO_C_P_C7, 0x20u);  /* 电池端分流器电流1 */
        vSPI_FifoContRead(BMIC_ADDRESS_917, CMIC_FIFO_C_S_C9, 0x20u);  /* 电池端分流器电流2 */
        if (DRV_RN8209_ReadAdcIA_Fast_V3(&Rn8209AdData[1]) != RN8209_STATUS_ACCESS_OK) {/* 负载端分流器电流 */
            DiagSetErrNowStateAPI(eERR_ID_820_COM, eErr_B);
        }
        else
        {
            DiagSetErrNowStateAPI(eERR_ID_820_COM, eErr_A);
        }                                                            

        /* === 1.1. 采样后立即计算3路分流器电流 === */
        CalcFast3Results();

        /* === 2. 分片轮转: 每2个主循环采集1个分片 === */
        gChunkDivider++;
        if (gChunkDivider >= 2u)
        {
            gChunkDivider = 0;
            switch (gSampleChunkIdx)
            {
            case 0: /* Chunk0: BatVolt, HvP5v, HvN5v, ShuntTemp 寄存器读取 */
                vSPI_2byte_write(BMIC_ADDRESS_917, BMIC_CNTADR_14, CMIC_AUX_LATCH);//连续读取 AD锁存
                vSPI_Continue_Read(BMIC_ADDRESS_917, CMIC_VDD50_ADR6D, ((CMIC_GPIO_ADR88 - CMIC_VDD50_ADR6D) + 1u));
                break;

            case 1: /* Chunk1: NPE FIFO读取 */
                vSPI_FifoContRead(BMIC_ADDRESS_917, CMIC_FIFO_V_P_C8, 0x20u);
                break;

            case 2: /* Chunk2: RN8209电压通道读取 */
                if (DRV_RN8209_ReadAdcU_Fast_V3(&Rn8209AdData[0]) != RN8209_STATUS_ACCESS_OK) {
                    DiagSetErrNowStateAPI(eERR_ID_820_COM, eErr_B);
                }
                else
                {
                    DiagSetErrNowStateAPI(eERR_ID_820_COM, eErr_A);
#ifdef async_TEST_ENABLE
 SEGGER_RTT_printf(0,"[Chunk2], dt=%dms \r\n",
(fast_start - 0) * portTICK_PERIOD_MS);
#endif                   
                }

                break;

            case 3: /* Chunk3: 通信健康检查 + 完整轮转完成 */
                {
                    u8 comm_ok = 1;

                    /*  MCU→922→Daisy→917 */
                    usSPI_Reg_ReadData[KA84922_ARRAY_INDEX][BMIC_STSADR_2C] = 0;
                    vSPI_2byte_read(BMIC_ADDRESS_922, BMIC_STSADR_2C);
                    usSPI_Reg_ReadData[KA84922_ARRAY_INDEX][BMIC_CNTADR_00] = 0;
                    vSPI_2byte_read(BMIC_ADDRESS_922, BMIC_CNTADR_00);

                    //922通信状态判断: 状态寄存器错误标志 + 地址寄存器回读验证
                    if ((usSPI_Reg_ReadData[KA84922_ARRAY_INDEX][BMIC_STSADR_2C] & BMIC_COM_ERR_ALL_MASK)
                        || (usSPI_Reg_ReadData[KA84922_ARRAY_INDEX][BMIC_CNTADR_00] != (BMIC_DCI_RATE_2560|BMIC_ADDRESS_922)))
                    {
                        /* 922异常 — 清除922错误标志后回到唤醒流程 */
                        vSPI_2byte_write(BMIC_ADDRESS_922, BMIC_CNTADR_09, BMIC_CLR_FLAG);
                        DiagSetErrNowStateAPI(eERR_ID_922_COM, eErr_B);
                        DiagSetErrNowStateAPI(eERR_ID_917_COM, eErr_A);/* 917不可达，不报故障 */
                        wakeUpState = 0;
                        gWakeUpStateInitFlag922 = 0;
                        gWakeUpStateInitFlag917 = 0;
                        VoltSampStep = EvtWake;
                        bSkipTrack = 1;
                        comm_ok = 0;
                    }
                    else
                    {
                        /* 922正常 */
                        DiagSetErrNowStateAPI(eERR_ID_922_COM, eErr_A);

                        /* === 2. 再检测917(远端/测量芯片) === */
                        usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][BMIC_STSADR_2C] = 0;
                        vSPI_2byte_read(BMIC_ADDRESS_917, BMIC_STSADR_2C);
                        usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][BMIC_CNTADR_00] = 0;
                        vSPI_2byte_read(BMIC_ADDRESS_917, BMIC_CNTADR_00);

                        //917通信状态判断: 状态寄存器错误标志 + 地址寄存器回读验证
                        if ((usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][BMIC_STSADR_2C] & BMIC_COM_ERR_ALL_MASK)
                            || (usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][BMIC_CNTADR_00] != (BMIC_DCI_RATE_2560|BMIC_ADDRESS_917)))
                        {
                            /* 917异常 — 清除917错误标志后回到唤醒流程 */
                            vSPI_2byte_write(BMIC_ADDRESS_917, BMIC_CNTADR_09, BMIC_CLR_FLAG);
                            DiagSetErrNowStateAPI(eERR_ID_917_COM, eErr_B);
                            wakeUpState = 0;
                            gWakeUpStateInitFlag922 = 0;
                            gWakeUpStateInitFlag917 = 0;
                            VoltSampStep = EvtWake;
                            bSkipTrack = 1;
                            comm_ok = 0;
                        }
                        else
                        {
                            /* 917正常 */
                            DiagSetErrNowStateAPI(eERR_ID_917_COM, eErr_A);
                        }
                    }
                    
                    
                   

                    if (comm_ok)
                    {
                        /* 初始化后首次采样完整一轮 */
                        if (!AfterWakeupFirstSample)
                        {
                            AfterWakeupFirstSample = 1;
                            FirstSampleFinishedFlag = 1;
                        }
#ifdef async_TEST_ENABLE
                        Track_Sample(TRACK_FULL_ROUND, "FULL");
#endif

                        /* 全量计算 (每80ms) */
                        Ka84917VoltCurrentCalculate();
                        Rn8209VoltCurrentCalculate();

#ifdef async_TEST_ENABLE
                        Track_Sample(TRACK_CALC_DONE, "CALC");
#endif
                    }
                }
                break;

            default:
                break;
            }
            gSampleChunkIdx = (gSampleChunkIdx + 1u) & 0x03u;  /* 0→1→2→3→0 */
        }

        /* === 3. 主循环完成 === */
        if (!bSkipTrack)
        {

#ifdef async_TEST_ENABLE
            Track_Sample(TRACK_MAIN_LOOP, "MAIN");
            SEGGER_RTT_printf(0, "[BODY] dt=%dms tick=%08X\r\n",
                (int)((xTaskGetTickCount() - fast_start) * portTICK_PERIOD_MS),
                (u32)xTaskGetTickCount());
#endif

            /* 超时保护: 采样时间>50ms则复位SPI+DMA */
            fast_end = xTaskGetTickCount();
            if ((fast_end - fast_start) > 50)
            {
                VSPI_TimeOut_Abort();
            }
            VoltSampStep = EvtFastSample;  /* 继续主采样循环 */
        }
    }
    break;

    case EvtError:
    {
        static u16 ErrorHoldTime = 0;
        if (++ErrorHoldTime >= 10) {
            ErrorHoldTime = 0;
            VoltSampStep = 0xaa;
        }
        osDelay(1);
        // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(3));
      
        break;
    }

    default:
        break;
    }
}
#endif
