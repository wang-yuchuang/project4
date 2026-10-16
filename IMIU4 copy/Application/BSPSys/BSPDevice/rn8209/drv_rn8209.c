/**
 ******************************************************************************
 * Copyright(c) Infy Power 2023-2023
 * @file    drv_rn8209.c
 * @author  ZZC955
 * @version V1.0
 * @date    2023-9-9
 * @brief   rn8209读写操作
 * @history 2023-9-9 ZZC955 创建
 ******************************************************************************
 */

/* 头文件 */
#include "BSPTypeDef.h"
#include "BSPKA84917_async.h"
#include "spi_async.h"  /* ucSPI_WaitAllDone */
#include "SEGGER_RTT.h"  /* RTT diagnostic */
// #include "BSPKA84917.h"
#include "drv_rn8209.h"
#include "drv_rn8209_type.h"
#include "drv_rn8209_interface.h"
#include "BSPTIM.h"
#include "BSPIRQ.h"
#include "SystemData.h"
#include "SystemDiag.h"
/* 全局变量定义 */
DRV_RN8209_STATIC uint8 g_u8Rn8209Init = 0;

/* 内部函数声明 */
DRV_RN8209_STATIC uint8 Rn8209SpecialCmdProc(uint8 cmdData);
DRV_RN8209_STATIC uint8 Rn8209Write2ByteCmdProc(uint8 cmd, uint16 data);
DRV_RN8209_STATIC uint8 Rn8209Read3ByteCmdProc(uint8 cmd, uint32 *data);
DRV_RN8209_STATIC uint8 Rn8209Read2ByteCmdProc(uint8 cmd, uint16 *data);
// DRV_RN8209_STATIC uint8 Rn8209Read1ByteCmdProc(uint8 cmd, uint8 *data);
DRV_RN8209_STATIC uint8 Rn8209SoftReset(TickType_t xLastWakeTime);

/* 内部函数定义 */
DRV_RN8209_STATIC uint8 Rn8209SpecialCmdProc(uint8 cmdData)
{
    uint8 u8Temp;
    uint8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    RN8209_ENABLE();
    if (SPI2_Send_Byte(SPECIAL_CMD_RN8209, &u8Temp) == SPI_STATUS_OK) {
        if (SPI2_Send_Byte(cmdData, &u8Temp) == SPI_STATUS_OK) {
            u8RetVal = RN8209_STATUS_ACCESS_OK;
        }
    }
    RN8209_DISABLE();
    return u8RetVal;
}

DRV_RN8209_STATIC uint8 Rn8209Write2ByteCmdProc(uint8 cmd, uint16 data)
{
    uint8 u8Temp;
    uint8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    RN8209_ENABLE();
    if (SPI2_Send_Byte((cmd | RN8209_WRITE_BIT7), &u8Temp) == SPI_STATUS_OK) {
        if (SPI2_Send_Byte(((uint8)(data >> 8U)), &u8Temp) == SPI_STATUS_OK) {
            if (SPI2_Send_Byte(((uint8)(data & 0xFF)), &u8Temp) == SPI_STATUS_OK) {
                u8RetVal = RN8209_STATUS_ACCESS_OK;
            }
        }
    }
    RN8209_DISABLE();
    return u8RetVal;
}

DRV_RN8209_STATIC uint8 Rn8209Read3ByteCmdProc(uint8 cmd, uint32* data)
{
    uint8 u8Temp, u8Temp1, u8Temp2;
    uint8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    RN8209_ENABLE();
    if (SPI2_Send_Byte(cmd, &u8Temp) == SPI_STATUS_OK) {
        if (SPI2_Send_Byte(RN8209_DUMMY_BYTE, &u8Temp) == SPI_STATUS_OK) {
            if (SPI2_Send_Byte(RN8209_DUMMY_BYTE, &u8Temp1) == SPI_STATUS_OK) {
                if (SPI2_Send_Byte(RN8209_DUMMY_BYTE, &u8Temp2) == SPI_STATUS_OK) {
                    *data = ((uint32)u8Temp << 16) | ((uint32)u8Temp1 << 8) | u8Temp2;
                    u8RetVal = RN8209_STATUS_ACCESS_OK;
                }
            }
        }
    }
    RN8209_DISABLE();
    return u8RetVal;
}

DRV_RN8209_STATIC uint8 Rn8209Read2ByteCmdProc(uint8 cmd, uint16* data)
{
    uint8 u8Temp, u8Temp1;
    uint8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    RN8209_ENABLE();
    if (SPI2_Send_Byte(cmd, &u8Temp) == SPI_STATUS_OK) {
        if (SPI2_Send_Byte(RN8209_DUMMY_BYTE, &u8Temp) == SPI_STATUS_OK) {
            if (SPI2_Send_Byte(RN8209_DUMMY_BYTE, &u8Temp1) == SPI_STATUS_OK) {
                *data = ((uint16)u8Temp << 8) | u8Temp1;
                u8RetVal = RN8209_STATUS_ACCESS_OK;
            }
        }
    }
    RN8209_DISABLE();
    return u8RetVal;
}

/* DRV_RN8209_STATIC uint8 Rn8209Read1ByteCmdProc(uint8 cmd, uint8* data)
{
    uint8 u8Temp;
    uint8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    RN8209_ENABLE();
    if (SPI2_Send_Byte(cmd, &u8Temp) == SPI_STATUS_OK) {
        if (SPI2_Send_Byte(RN8209_DUMMY_BYTE, &u8Temp) == SPI_STATUS_OK) {
            *data = u8Temp;
            u8RetVal = RN8209_STATUS_ACCESS_OK;
        }
    }
    RN8209_DISABLE();
    return u8RetVal;
} */

DRV_RN8209_STATIC uint8 Rn8209SoftReset(TickType_t xLastWakeTime)
{
    if (Rn8209SpecialCmdProc(DATA_RN8209_WRITE_ENABLE) == RN8209_STATUS_ACCESS_OK) {
        if (Rn8209SpecialCmdProc(DATA_RN8209_RESRT) == RN8209_STATUS_ACCESS_OK) {
            // vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));
           // delayms(1);

            return RN8209_STATUS_ACCESS_OK;
        }
    }
    return RN8209_STATUS_ACCESS_FAIL;
}

#if 1
/* 外部函数定义 */
u8 RN8209_DRV_Init(TickType_t xLastWakeTime)
{
    /* 1. 定义所有局部变量 */
    uint8 u8Ret, u8Ret1;
    uint16 u16RdData, u16RdData1;
    uint16 u16Temp = (1U << 14U) | (1U << 5U) | (1U << 6U); // 14, 5, 6分别代表各通道高通滤波设置位

    /* 2. 初始化结果 */
    g_u8Rn8209Init = 0;

    do
    {
        if(Rn8209SoftReset(xLastWakeTime) != RN8209_STATUS_ACCESS_OK){
            break;
        }
            if(Rn8209SpecialCmdProc(DATA_RN8209_WRITE_ENABLE) != RN8209_STATUS_ACCESS_OK) {
                break;
            }
                if(Rn8209Write2ByteCmdProc(CMD_SYSCON, 0x0000) != RN8209_STATUS_ACCESS_OK) {
                    break;
                }
                    if(Rn8209Write2ByteCmdProc(CMD_EMUCON, u16Temp) != RN8209_STATUS_ACCESS_OK) {
                        break;
                    }
                        u8Ret = Rn8209Read2ByteCmdProc(CMD_SYSCON, &u16RdData);
                        u8Ret1 = Rn8209Read2ByteCmdProc(CMD_EMUCON, &u16RdData1);
    
                        if ((u8Ret == RN8209_STATUS_ACCESS_OK) 
                        && (u8Ret1 == RN8209_STATUS_ACCESS_OK) 
                        &&(u16RdData == 0x0000) 
                        && (u16RdData1 == u16Temp)) 
                        {
                            g_u8Rn8209Init = 1;
                        }
                     
            
        
    } while (0);
    
    /* 3. 告警检测与上报 */
    if(g_u8Rn8209Init == 1)//rn8209激活成功告警检测
    {
        DiagSetErrNowStateAPI(eERR_ID_820_COM,eErr_A);
    }else
    {
        DiagSetErrNowStateAPI(eERR_ID_820_COM,eErr_B);
    }

    /* 4. 关闭写使能，保护寄存器 */
    (void)Rn8209SpecialCmdProc(DATA_RN8209_WRITE_PROTECT);

    return g_u8Rn8209Init;
  
}
#endif

#if 0 
/* 外部函数定义 */
void DRV_RN8209_Init(void)
{
    uint8 u8Ret, u8Ret1;
    uint16 u16RdData, u16RdData1;
    if (Rn8209SoftReset() == RN8209_STATUS_ACCESS_OK) {
        if (Rn8209SpecialCmdProc(DATA_RN8209_WRITE_ENABLE) == RN8209_STATUS_ACCESS_OK) {
            if (Rn8209Write2ByteCmdProc(CMD_SYSCON, 0x0000) == RN8209_STATUS_ACCESS_OK) {
                uint16 u16Temp = (1U << 14U) | (1U << 5U) | (1U << 6U); // 14, 5, 6分别代表各通道高通滤波设置位
                if (Rn8209Write2ByteCmdProc(CMD_EMUCON, u16Temp) == RN8209_STATUS_ACCESS_OK) {
                    u8Ret = Rn8209Read2ByteCmdProc(CMD_SYSCON, &u16RdData);
                    u8Ret1 = Rn8209Read2ByteCmdProc(CMD_EMUCON, &u16RdData1);
                    if ((u8Ret == RN8209_STATUS_ACCESS_OK) && (u8Ret1 == RN8209_STATUS_ACCESS_OK) &&
                        (u16RdData == 0x0000) && (u16RdData1 == u16Temp)) {
                        g_u8Rn8209Init = 1;
                       
                    }
                    else
                    {
                        g_u8Rn8209Init = 0;
                       
                    }
                    
                }
            }
        }
    }
    (void)Rn8209SpecialCmdProc(DATA_RN8209_WRITE_PROTECT);
}
#endif
//读取负载端分流器电流
uint8 DRV_RN8209_ReadAdcIA(uint32* data)
{
    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    return Rn8209Read3ByteCmdProc(CMD_SPL_IA, data);
}



/*=============================================================================================
** 优化版RN8209 ADC读取 — 配置GPSPI一次, 保持CS使能, 连续发4字节后读3次RX
** 原版SPI2_Send_Byte每字节需5次菊花链SPI: write_Mask(CFG) + 2×read + write(DATA) + write_Mask(TALK) + read(RX)
** 优化后: 1×write_Mask(CFG) + 1×write_Mask(CS_EN) + 4×(write(DATA)+write_Mask(TALK)+read(RX)) + 1×write_Mask(CS_DIS)
** 从 ~22次菊花链SPI 减少到 ~14次
**===========================================================================================*/
uint8 DRV_RN8209_ReadAdcIA_Fast(uint32* data)
{
    u8 u8Temp, u8Temp1, u8Temp2;
    u8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 1. 配置GPSPI: 8bit帧, CS1负极性, 1Mbps — 只做一次 */
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_35,
            CMIC_BIT_LENGTH_1_7|CMIC_BIT_GPSPI_CS1_N|CMIC_BIT_GPSPI_RATE2, 0x7FF)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 2. 使能RN8209 CS — 保持使能直到所有字节发完 */
    RN8209_ENABLE();

    /* 3. 发送命令字节 + 读取(命令字节无有效返回, 但必须触发TALK) */
    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, (u16)(CMD_SPL_IA << 8) | 0xAA)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    // [优化] 跳过命令字节的回读 (原代码: vSPI_2byte_read(...) 结果被丢弃, 省~0.7ms)

    /* 4. 发送3个dummy字节, 每次读回1字节有效数据 */
    /* Dummy 1 → 读回第1字节 */
    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, (u16)(RN8209_DUMMY_BYTE << 8) | 0xAA)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    u8Temp = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* Dummy 2 → 读回第2字节 */
    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, (u16)(RN8209_DUMMY_BYTE << 8) | 0xAA)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    u8Temp1 = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* Dummy 3 → 读回第3字节 */
    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, (u16)(RN8209_DUMMY_BYTE << 8) | 0xAA)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    u8Temp2 = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* 5. 禁用RN8209 CS */
    RN8209_DISABLE();

    *data = ((uint32)u8Temp << 16) | ((uint32)u8Temp1 << 8) | u8Temp2;
    u8RetVal = RN8209_STATUS_ACCESS_OK;

    return u8RetVal;
}





//读取负载端电压 
uint8 DRV_RN8209_ReadAdcU(uint32* data)
{
    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    return Rn8209Read3ByteCmdProc(CMD_SPL_U, data);
}
//
uint8 DRV_RN8209_ReadAdcU_Fast(uint32* data)
{
    u8 u8Temp, u8Temp1, u8Temp2;
    u8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 配置GPSPI */
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_35,
            CMIC_BIT_LENGTH_1_7|CMIC_BIT_GPSPI_CS1_N|CMIC_BIT_GPSPI_RATE2, 0x7FF)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    RN8209_ENABLE();

    /* 发送CMD_SPL_U命令字节 */
    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, (u16)(CMD_SPL_U << 8) | 0xAA)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);

    /* Dummy 1 */
    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, (u16)(RN8209_DUMMY_BYTE << 8) | 0xAA)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    u8Temp = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* Dummy 2 */
    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, (u16)(RN8209_DUMMY_BYTE << 8) | 0xAA)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    u8Temp1 = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* Dummy 3 */
    if (TRUE != vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, (u16)(RN8209_DUMMY_BYTE << 8) | 0xAA)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    u8Temp2 = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    RN8209_DISABLE();

    *data = ((uint32)u8Temp << 16) | ((uint32)u8Temp1 << 8) | u8Temp2;
    u8RetVal = RN8209_STATUS_ACCESS_OK;

    return u8RetVal;
}


/*=============================================================================================
** 优化版RN8209 IA ADC读取 — 32-bit GPSPI帧
**   一次TALK触发完成CMD(8bit)+DATA(24bit)完整传输, 替代原逐字节Fast版
**   GPSPI速率: 1Mbps (符合RN8209手册≤1.7MHz)
**   Daisy操作数: ~8次 (vs 原Fast版~14次, 降43%)
**===========================================================================================*/
uint8 DRV_RN8209_ReadAdcIA_Fast_V2(uint32* data)
{
    uint16 u16Rx34, u16Rx33;
    uint32 u32FullRx;

    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 1. 配置GPSPI: 32-bit帧, 1Mbps, CS负极性 */
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_35,
            CMIC_BIT_LENGTH_1_32|CMIC_BIT_GPSPI_CS1_N|CMIC_BIT_GPSPI_RATE2, 0x7FF)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 2. 使能RN8209 CS */
    RN8209_ENABLE();

    /* 3. TX: CMD_SPL_IA(0x30) + 3字节dummy → 32-bit = 0x30AAAAAA */
    vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, 0x30AAu);
    vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX1500_31, 0xAAAAu);
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 4. RX: 32-bit捕获 → 丢弃首字节(CMD阶段) → 保留后24-bit */
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX3116_34);
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    u16Rx34 = usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX3116_34];
    u16Rx33 = usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33];
    u32FullRx = ((uint32)u16Rx34 << 16u) | (uint32)u16Rx33;
    /* RX移位方向: MSB先入→[31:24], 故 [31:24]=垃圾(CMD回显), [23:16]=MSB, [15:8]=MID, [7:0]=LSB */
    *data = u32FullRx & 0x00FFFFFFu;

    /* 5. 释放RN8209 CS */
    RN8209_DISABLE();

    return RN8209_STATUS_ACCESS_OK;
}


/*=============================================================================================
** 优化版RN8209 U ADC读取 — 32-bit GPSPI帧
**   一次TALK触发完成CMD(8bit)+DATA(24bit)完整传输, 替代原逐字节Fast版
**   GPSPI速率: 1Mbps (符合RN8209手册≤1.7MHz)
**   Daisy操作数: ~8次 (vs 原Fast版~14次, 降43%)
**===========================================================================================*/
uint8 DRV_RN8209_ReadAdcU_Fast_V2(uint32* data)
{
    uint16 u16Rx34, u16Rx33;
    uint32 u32FullRx;

    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 1. 配置GPSPI: 32-bit帧, 1Mbps, CS负极性 */
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_35,
            CMIC_BIT_LENGTH_1_32|CMIC_BIT_GPSPI_CS1_N|CMIC_BIT_GPSPI_RATE2, 0x7FF)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 2. 使能RN8209 CS */
    RN8209_ENABLE();

    /* 3. TX: CMD_SPL_U(0x32) + 3字节dummy → 32-bit = 0x32AAAAAA */
    vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX3116_32, 0x32AAu);
    vSPI_2byte_write(BMIC_ADDRESS_917, CMIC_TX1500_31, 0xAAAAu);
    if (TRUE != vSPI_2byte_write_Mask(BMIC_ADDRESS_917, CMIC_CNTADR_30,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
            CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON)) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* 4. RX: 32-bit捕获 → 丢弃首字节(CMD阶段) → 保留后24-bit */
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX3116_34);
    vSPI_2byte_read(BMIC_ADDRESS_917, CMIC_RX1500_33);
    u16Rx34 = usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX3116_34];
    u16Rx33 = usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33];
    u32FullRx = ((uint32)u16Rx34 << 16u) | (uint32)u16Rx33;
    /* RX移位方向: MSB先入→[31:24], 故 [31:24]=垃圾(CMD回显), [23:16]=MSB, [15:8]=MID, [7:0]=LSB */
    *data = u32FullRx & 0x00FFFFFFu;

    /* 5. 释放RN8209 CS */
    RN8209_DISABLE();

    return RN8209_STATUS_ACCESS_OK;
}

/*=============================================================================================
** V4: 8-bit GPSPI帧 + NoWait批量入队
**   协议: 与_Fast版完全相同(逐字节8-bit), 已验证数据正确
**   优化: 将串行等待改为3段批量入队 + WaitAllDone
**   WaitAllDone次数: 3次 (vs 原Fast版13次), 预计耗时从~4ms降至~1.5ms
**===========================================================================================*/

/*=============================================================================================
** @Name      : DRV_RN8209_ReadAdcIA_Fast_V3
** @Function  : RN8209 IA ADC读取 — 8-bit帧 + 3段批量入队
**   Phase A: 配置 + CS使能 + CMD发送 + Dummy1发送 + 读RX → MSB
**   Phase B: Dummy2发送 + 读RX → MID
**   Phase C: Dummy3发送 + 读RX → LSB + CS释放
**===========================================================================================*/
uint8 DRV_RN8209_ReadAdcIA_Fast_V3(uint32* data)
{
    u8 u8Temp, u8Temp1, u8Temp2;
    u8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* Phase A: 配置GPSPI(8bit) + CS使能 + CMD发送 + Dummy1发送 + 读RX, 全部NoWait入队 */
    if (TRUE != vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_35,
            CMIC_BIT_LENGTH_1_7|CMIC_BIT_GPSPI_CS1_N|CMIC_BIT_GPSPI_RATE2, 0x7FF)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    /* CS使能 */
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON, CMIC_CSON_1_ON|CMIC_MAN_CS_ON);
    /* CMD字节: CMD_SPL_IA(0x30) */
    vSPI_2byte_write_NoWait(BMIC_ADDRESS_917, CMIC_TX3116_32,
        (u16)(CMD_SPL_IA << 8) | RN8209_DUMMY_BYTE);
    /* TALK触发 */
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON);
    /* Dummy1字节 */
    vSPI_2byte_write_NoWait(BMIC_ADDRESS_917, CMIC_TX3116_32,
        (u16)(RN8209_DUMMY_BYTE << 8) | RN8209_DUMMY_BYTE);
    /* TALK触发 */
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON);
    /* 读回Dummy1数据(MSB) — NoWait入队, 状态机会在TALK完成后读 */
    vSPI_2byte_read_NoWait(BMIC_ADDRESS_917, CMIC_RX1500_33);
    if (ucSPI_WaitAllDone(10u) != SPI_NOERR) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    u8Temp = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* Phase B: Dummy2发送 + 读RX(MID) */
    vSPI_2byte_write_NoWait(BMIC_ADDRESS_917, CMIC_TX3116_32,
        (u16)(RN8209_DUMMY_BYTE << 8) | RN8209_DUMMY_BYTE);
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON);
    vSPI_2byte_read_NoWait(BMIC_ADDRESS_917, CMIC_RX1500_33);
    if (ucSPI_WaitAllDone(10u) != SPI_NOERR) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    u8Temp1 = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* Phase C: Dummy3发送 + 读RX(LSB) + CS释放 */
    vSPI_2byte_write_NoWait(BMIC_ADDRESS_917, CMIC_TX3116_32,
        (u16)(RN8209_DUMMY_BYTE << 8) | RN8209_DUMMY_BYTE);
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON);
    vSPI_2byte_read_NoWait(BMIC_ADDRESS_917, CMIC_RX1500_33);
    /* CS释放 — 在TALK和读RX之后 */
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_OFF|CMIC_MAN_CS_OFF, CMIC_CSON_1_ON|CMIC_MAN_CS_OFF);
    if (ucSPI_WaitAllDone(10u) != SPI_NOERR) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    u8Temp2 = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    *data = ((uint32)u8Temp << 16) | ((uint32)u8Temp1 << 8) | u8Temp2;
    u8RetVal = RN8209_STATUS_ACCESS_OK;

    return u8RetVal;
}


/*=============================================================================================
** @Name      : DRV_RN8209_ReadAdcU_Fast_V3
** @Function  : RN8209 U ADC读取 — 8-bit帧 + 3段批量入队
**===========================================================================================*/
uint8 DRV_RN8209_ReadAdcU_Fast_V3(uint32* data)
{
    u8 u8Temp, u8Temp1, u8Temp2;
    u8 u8RetVal = RN8209_STATUS_ACCESS_FAIL;

    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }

    /* Phase A: 配置GPSPI(8bit) + CS使能 + CMD发送 + Dummy1发送 + 读RX */
    if (TRUE != vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_35,
            CMIC_BIT_LENGTH_1_7|CMIC_BIT_GPSPI_CS1_N|CMIC_BIT_GPSPI_RATE2, 0x7FF)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON, CMIC_CSON_1_ON|CMIC_MAN_CS_ON);
    /* CMD字节: CMD_SPL_U(0x32) */
    vSPI_2byte_write_NoWait(BMIC_ADDRESS_917, CMIC_TX3116_32,
        (u16)(CMD_SPL_U << 8) | RN8209_DUMMY_BYTE);
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON);
    vSPI_2byte_write_NoWait(BMIC_ADDRESS_917, CMIC_TX3116_32,
        (u16)(RN8209_DUMMY_BYTE << 8) | RN8209_DUMMY_BYTE);
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON);
    vSPI_2byte_read_NoWait(BMIC_ADDRESS_917, CMIC_RX1500_33);
    if (ucSPI_WaitAllDone(10u) != SPI_NOERR) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    u8Temp = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* Phase B: Dummy2 */
    vSPI_2byte_write_NoWait(BMIC_ADDRESS_917, CMIC_TX3116_32,
        (u16)(RN8209_DUMMY_BYTE << 8) | RN8209_DUMMY_BYTE);
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON);
    vSPI_2byte_read_NoWait(BMIC_ADDRESS_917, CMIC_RX1500_33);
    if (ucSPI_WaitAllDone(10u) != SPI_NOERR) {
        RN8209_DISABLE();
        return RN8209_STATUS_ACCESS_FAIL;
    }
    u8Temp1 = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    /* Phase C: Dummy3 + CS释放 */
    vSPI_2byte_write_NoWait(BMIC_ADDRESS_917, CMIC_TX3116_32,
        (u16)(RN8209_DUMMY_BYTE << 8) | RN8209_DUMMY_BYTE);
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON,
        CMIC_CSON_1_ON|CMIC_MAN_CS_ON|CMIC_TALK_GPSPI1ON);
    vSPI_2byte_read_NoWait(BMIC_ADDRESS_917, CMIC_RX1500_33);
    vSPI_2byte_write_Mask_NoWait(BMIC_ADDRESS_917, CMIC_CNTADR_30,
        CMIC_CSON_1_OFF|CMIC_MAN_CS_OFF, CMIC_CSON_1_ON|CMIC_MAN_CS_OFF);
    if (ucSPI_WaitAllDone(10u) != SPI_NOERR) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    u8Temp2 = (u8)(usSPI_Reg_ReadData[KA84917_ARRAY_INDEX][CMIC_RX1500_33] & 0xFF);

    *data = ((uint32)u8Temp << 16) | ((uint32)u8Temp1 << 8) | u8Temp2;
    u8RetVal = RN8209_STATUS_ACCESS_OK;

    return u8RetVal;
}




uint8 DRV_RN8209_ReadRmsIA(uint32* data)
{
    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    return Rn8209Read3ByteCmdProc(CMD_IARMS, data);
}

uint8 DRV_RN8209_ReadRmsU(uint32* data)
{
    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    return Rn8209Read3ByteCmdProc(CMD_URMS, data);
}

uint8 DRV_RN8209_WriteRmsIAOffset(uint32 u32Rms)
{
    uint16 u16High16Data = (u32Rms >> 8U) & 0xFFFF;
    uint16 u16DclData;
    uint8 u8Ret, u8Ret1, u8Ret2;

    u8Ret2 = RN8209_STATUS_ACCESS_FAIL;
    if (Rn8209SpecialCmdProc(DATA_RN8209_WRITE_ENABLE) == RN8209_STATUS_ACCESS_OK) {
        if (Rn8209Read2ByteCmdProc(CMD_DCL, &u16DclData) == RN8209_STATUS_ACCESS_OK) {
            u16DclData = (u16DclData & 0xFFF0U )| ((u32Rms >> 4U) & 0x000FU); // BIT[7:4]写入DCL的BIT[3:0]
            u8Ret = Rn8209Write2ByteCmdProc(CMD_DCIAH, u16High16Data);
            u8Ret1 = Rn8209Write2ByteCmdProc(CMD_DCL, u16DclData);
            if ((u8Ret == RN8209_STATUS_ACCESS_OK) && (u8Ret1 == RN8209_STATUS_ACCESS_OK)) {
                u8Ret2 = RN8209_STATUS_ACCESS_OK;
            }
        }
    }
    (void)Rn8209SpecialCmdProc(DATA_RN8209_WRITE_PROTECT);
    return u8Ret2;
}

uint8 DRV_RN8209_WriteRmsUOffset(uint32 u32Rms)
{
    uint16 u16High16Data = (u32Rms >> 8U) & 0xFFFF;
    uint16 u16DclData;
    uint8 u8Ret, u8Ret1, u8Ret2;

    u8Ret2 = RN8209_STATUS_ACCESS_FAIL;
    if (Rn8209SpecialCmdProc(DATA_RN8209_WRITE_ENABLE) == RN8209_STATUS_ACCESS_OK) {
        if (Rn8209Read2ByteCmdProc(CMD_DCL, &u16DclData) == RN8209_STATUS_ACCESS_OK) {
            u16DclData = (u16DclData & 0xF0FF) | ((u32Rms << 4U) & 0x0F00U); // BIT[7:4]写入DCL的BIT[11:8]
            u8Ret = Rn8209Write2ByteCmdProc(CMD_DCUH, u16High16Data);
            u8Ret1 = Rn8209Write2ByteCmdProc(CMD_DCL, u16DclData);
            if ((u8Ret == RN8209_STATUS_ACCESS_OK) && (u8Ret1 == RN8209_STATUS_ACCESS_OK)) {
                u8Ret2 = RN8209_STATUS_ACCESS_OK;
            }
        }
    }
    (void)Rn8209SpecialCmdProc(DATA_RN8209_WRITE_PROTECT);
    return u8Ret2;
}



uint8 Rn8209InitSuccessFlag = 0;

DRV_RN8209_STATIC uint8 DRV_RN8209_ReadID(uint32* data)
{
    if ((data == NULL_PTR) || (g_u8Rn8209Init == 0)) {
        return RN8209_STATUS_ACCESS_FAIL;
    }
    return Rn8209Read3ByteCmdProc(CMD_DEVICE_ID,data);
}

void RN8209ReadCMD_DEVICE_ID(void)
{
    //初始化RN8209成功的话,读取DeviceID寄存器内数据
    uint8 u8Ret;
    uint32 u32RdData;
    
    if (g_u8Rn8209Init == 1)
    {
        /* ===== 8-bit GPSPI读取DEVICE_ID (原方法) ===== */
        u8Ret = DRV_RN8209_ReadID(&u32RdData);
        
        /* 判断8-bit结果 */
        if ((u8Ret == RN8209_STATUS_ACCESS_OK) && (u32RdData == 0x820900))
        {
            Rn8209InitSuccessFlag = 1;
        }
        else
        {
            Rn8209InitSuccessFlag = 0;
        }
    }
}


// 校准前确保通道接地, maintunction间隔5毫秒,不可重入
// static uint8 AdjustHighVolt(void)
// {
//     static uint16 s_u16AdjustCount = 0;
//     static uint32 s_u32RmsIASum = 0, s_u32RmsUSum = 0;
//     static uint32 s_u32RmsIASum2 = 0, s_u32RmsUSum2 = 0;

//     if (g_u8AdjustFlag == 0) {
//         return ADJUST_END;
//     }

//     if (s_u16AdjustCount >= ADJUST_END_COUNT) {
//         return ADJUST_END;
//     }
//     if (s_u16AdjustCount < 10U) { // 读IA、U的有效值10次
//         if (CalcRmsSum(&s_u32RmsIASum, &s_u32RmsUSum) == ADJUST_FAIL) {
//             s_u16AdjustCount = ADJUST_END_COUNT;
//             return ADJUST_FAIL;
//         }
//     } else if (s_u16AdjustCount == 10U) { // 第10次计算平均值
//         s_u32RmsIASum /= 10U;
//         s_u32RmsUSum /= 10U;
//         if ((DRV_RN8209_WriteRmsIAOffset(s_u32RmsIASum) != RN8209_STATUS_ACCESS_OK) ||
//             (DRV_RN8209_WriteRmsUOffset(s_u32RmsUSum) != RN8209_STATUS_ACCESS_OK)) {
//                 s_u16AdjustCount = ADJUST_END_COUNT;
//                 return ADJUST_FAIL;
//         }
//     } else if (s_u16AdjustCount < (400U + 10U)) { // 等待400次
//         // nothing
//     } else if (s_u16AdjustCount < (400U + 10U + 10U)) { // 读IA、U的有效值10次
//         if (CalcRmsSum(&s_u32RmsIASum2, &s_u32RmsUSum2) == ADJUST_FAIL) {
//             s_u16AdjustCount = ADJUST_END_COUNT;
//             return ADJUST_FAIL;
//         }
//     } else if (s_u16AdjustCount == (400U + 10U + 10U)) { // 第10次计算平均值
//         s_u32RmsIASum2 /= 10U;
//         s_u32RmsUSum2 /= 10U;
//         if (s_u32RmsIASum2 > s_u32RmsIASum) { // 大于则取反校正
//             s_u32RmsIASum = ~(s_u32RmsIASum);
//             if (DRV_RN8209_WriteRmsIAOffset(s_u32RmsIASum) != RN8209_STATUS_ACCESS_OK) {
//                 s_u16AdjustCount = ADJUST_END_COUNT;
//                 return ADJUST_FAIL;
//             }
//         }
//         if (s_u32RmsUSum2 > s_u32RmsUSum) { // 大于则取反校正
//             s_u32RmsUSum = ~(s_u32RmsUSum);
//             if (DRV_RN8209_WriteRmsUOffset(s_u32RmsUSum) != RN8209_STATUS_ACCESS_OK) {
//                 s_u16AdjustCount = ADJUST_END_COUNT;
//                 return ADJUST_FAIL;
//             }
//         }
//         s_u16AdjustCount = ADJUST_END_COUNT;
//         SetLongSetData(CFG_LONG_u32RmsIACoef, &s_u32RmsIASum);
//         SetLongSetData(CFG_LONG_u32RmsUCoef, &s_u32RmsUSum);
//         return ADJUST_OK;
//     } else {
//         // nothing
//     }
//     if (s_u16AdjustCount < (400U + 10U + 10U)) {
//         s_u16AdjustCount++;
//         return ADJUST_PROCESS;
//     } else {
//         s_u16AdjustCount = ADJUST_END_COUNT;
//         return ADJUST_END;
//     }
// }

// static uint8 CalcRmsSum(uint32 *pu32RmsIA, uint32 *pu32RmsU)
// {
//     uint32 u32Rms;

//     if (DRV_RN8209_ReadRmsIA(&u32Rms) == RN8209_STATUS_ACCESS_OK) {
//         *pu32RmsIA += u32Rms;
//     } else {
//         return ADJUST_FAIL;
//     }

//     if (DRV_RN8209_ReadRmsU(&u32Rms) == RN8209_STATUS_ACCESS_OK) {
//         *pu32RmsU += u32Rms;
//     } else {
//         return ADJUST_FAIL;
//     }
//     return ADJUST_OK;
// }