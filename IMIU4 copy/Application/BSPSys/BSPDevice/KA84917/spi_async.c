/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : spi_async.c
** @Founder      : WYP40505
** @Date         : 2026.05.06
** @Function     : 异步SPI通信状态机 — 定时器中断+DMA菊花链通信驱动
** @Instructions : 移植自Nuvoton spi.c, 适配STM32G0xx HAL + FreeRTOS
**                 核心架构: 请求队列 → TIM14中断驱动状态机 → DMA TX/RX → HAL回调
**===========================================================================================*/
#if 1
/*=============================================================================================
** 头文件
**===========================================================================================*/
#include "spi_async.h"
#include "BSPSPI.h"
#include "BSPKA84917_async.h"
// #include "BSPKA84917.h"
#include "crc.h"
#include "spi.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "BSPDIDO.h"
#include "SEGGER_RTT.h"
#include "spi_async.h"
/*=============================================================================================
** 外部变量
**===========================================================================================*/
extern SPI_HandleTypeDef hspi3;
static const ULONG ulFixedBit_COMMIC[]={
	M_MASK0((ULONG)0x3FC0u),	M_MASK0((ULONG)0xFFF0u),	ALLMASK,					NOMASK,						/* 0x00~ 0x03 */
	NOMASK,						NOMASK,						ALLMASK,					ALLMASK,					/* 0x04~ 0x07 */
	ALLMASK,					ALLMASK,					ALLMASK,					M_MASK0((ULONG)0x84FEu),	/* 0x08~ 0x0B */
	M_MASK0((ULONG)0xBFFFu),	M_MASK0((ULONG)0x276Fu),	ALLMASK,					ALLMASK,					/* 0x0C~ 0x0F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x10~ 0x13 */
	ALLMASK,					ALLMASK,					M_MASK0((ULONG)0x9CFFu),	ALLMASK,					/* 0x14~ 0x17 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x18~ 0x1B */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x1C~ 0x1F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x20~ 0x23 */
	ALLMASK,					NOMASK,						ALLMASK,					ALLMASK,					/* 0x24~ 0x27 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x28~ 0x2B */
	NOMASK,						M_MASK0((ULONG)0xFE00u),	ALLMASK,					ALLMASK,					/* 0x2C~ 0x2F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x30~ 0x33 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x34~ 0x37 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x38~ 0x3B */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x3C~ 0x3F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x40~ 0x43 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x44~ 0x47 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x48~ 0x4B */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x4C~ 0x4F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x50~ 0x53 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x54~ 0x57 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x58~ 0x5B */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x5C~ 0x5F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x60~ 0x63 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x64~ 0x67 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x68~ 0x6B */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x6C~ 0x6F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x70~ 0x73 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x74~ 0x77 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x78~ 0x7B */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x7C~ 0x7F */
	ALLMASK,					ALLMASK,					ALLMASK,					NOMASK,						/* 0x80~ 0x83 */
	ALLMASK,					NOMASK,						NOMASK,						NOMASK,						/* 0x84~ 0x87 */
	ALLMASK,					ALLMASK,					ALLMASK,					NOMASK,						/* 0x88~ 0x8B */
	NOMASK,						NOMASK,						NOMASK,						ALLMASK,					/* 0x8C~ 0x8F */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x90~ 0x93 */
	NOMASK,						ALLMASK,					ALLMASK,					M_MASK0((ULONG)0xFFF8u),	/* 0x94~ 0x97 */
	M_MASK0((ULONG)0xE318u),	ALLMASK,					ALLMASK,					ALLMASK,					/* 0x98~ 0x9B */
	ALLMASK,					M_MASK0((ULONG)0x8FFFu),	ALLMASK,					M_MASK0((ULONG)0x000Eu),	/* 0x9C~ 0x9F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0xA0~ 0xA3 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0xA4~ 0xA7 */
	ALLMASK,					ALLMASK,					ALLMASK,					M_MASK0((ULONG)0xFF00u),	/* 0xA8~ 0xAB */
	ALLMASK,					ALLMASK,					ALLMASK,					M_MASK0((ULONG)0xFFC0u),	/* 0xAC~ 0xAF */
	ALLMASK,					ALLMASK,					ALLMASK,					M_MASK0((ULONG)0x00FFu),	/* 0xB0~ 0xB3 */
};
static const ULONG ulFixedBit_CMIC[]={
	M_MASK0((ULONG)0x3FC0u),	M_MASK0((ULONG)0xFFF3u),	ALLMASK,					ALLMASK,					/* 0x00~ 0x03 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x04~ 0x07 */
	ALLMASK,					M_MASK0((ULONG)0xF9DCu),	M_MASK0((ULONG)0xFC00u),	M_MASK0((ULONG)0xFC1Eu),	/* 0x08~ 0x0B */
	M_MASK0((ULONG)0xF80Fu),	M_MASK0((ULONG)0x1FFFu),	M_MASK0((ULONG)0xFFF8u),	ALLMASK,					/* 0x0C~ 0x0F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x10~ 0x13 */
	M_MASK0((ULONG)0xF80Fu),	ALLMASK,					M_MASK0((ULONG)0x01FFu),	M_MASK0((ULONG)0xFFF8u),	/* 0x14~ 0x17 */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x18~ 0x1B */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x1C~ 0x1F */
	ALLMASK,					ALLMASK,					ALLMASK,					ALLMASK,					/* 0x20~ 0x23 */
	ALLMASK,					NOMASK,						NOMASK,						NOMASK,						/* 0x24~ 0x27 */
	NOMASK,						ALLMASK,					ALLMASK,					ALLMASK,					/* 0x28~ 0x2B */
	M_MASK0((ULONG)0x5403u),	NOMASK,						M_MASK0((ULONG)0x7079u),	M_MASK0((ULONG)0xC009u),	/* 0x2C~ 0x2F */
	M_MASK0((ULONG)0x0200u),	NOMASK,						NOMASK,						NOMASK,						/* 0x30~ 0x33 */
	NOMASK,						M_MASK0((ULONG)0x8000u),	M_MASK0((ULONG)0xFFCCu),	NOMASK,						/* 0x34~ 0x37 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x38~ 0x3B */
	NOMASK,						NOMASK,						NOMASK,						M_MASK0((ULONG)0xFE00u),	/* 0x3C~ 0x3F */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x40~ 0x43 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x44~ 0x47 */
	M_MASK0((ULONG)0x8000u),	M_MASK0((ULONG)0xFFCCu),	NOMASK,						NOMASK,						/* 0x48~ 0x4B */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x4C~ 0x4F */
	NOMASK,						NOMASK,						M_MASK0((ULONG)0xFE00u),	NOMASK,						/* 0x50~ 0x53 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x54~ 0x57 */
	NOMASK,						NOMASK,						NOMASK,						M_MASK0((ULONG)0xFFF8u),	/* 0x58~ 0x5B */
	M_MASK0((ULONG)0x0002u),	M_MASK0((ULONG)0x0010u),	NOMASK,						M_MASK0((ULONG)0x0400u),	/* 0x5C~ 0x5F */
	NOMASK,						NOMASK,						NOMASK,						M_MASK0((ULONG)0x000Fu),	/* 0x60~ 0x63 */
	M_MASK0((ULONG)0x000Fu),	M_MASK0((ULONG)0x0100u),	NOMASK,						NOMASK,						/* 0x64~ 0x67 */
	M_MASK0((ULONG)0x0FFFu),	M_MASK0((ULONG)0x0303u),	M_MASK0((ULONG)0x0308u),	M_MASK0((ULONG)0x0001u),	/* 0x68~ 0x6B */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x6C~ 0x6F */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x70~ 0x73 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x74~ 0x77 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x78~ 0x7B */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x7C~ 0x7F */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x80~ 0x83 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x84~ 0x87 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x88~ 0x8B */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x8C~ 0x8F */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0x90~ 0x93 */
	NOMASK,						NOMASK,						NOMASK,						M_MASK0((ULONG)0x8000u),	/* 0x94~ 0x97 */
	M_MASK0((ULONG)0xA108u),	M_MASK0((ULONG)0xA108u),	M_MASK0((ULONG)0xA108u),	M_MASK0((ULONG)0xA108u),	/* 0x98~ 0x9B */
	M_MASK0((ULONG)0xA108u),	M_MASK0((ULONG)0x83FFu),	ALLMASK,					M_MASK0((ULONG)0xFFFEu),	/* 0x9C~ 0x9F */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0xA0~ 0xA3 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0xA4~ 0xA7 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0xA8~ 0xAB */
	ALLMASK,					ALLMASK,					M_MASK0((ULONG)0xFF00u),	M_MASK0((ULONG)0xFFC0u),	/* 0xAC~ 0xAF */
	M_MASK0((ULONG)0xE000u),	M_MASK0((ULONG)0xFFE7u),	M_MASK0((ULONG)0xFC7Fu),	ALLMASK,					/* 0xB0~ 0xB3 */
	M_MASK0((ULONG)0xFFFEu),	NOMASK,						M_MASK0((ULONG)0xFFFEu),	M_MASK0((ULONG)0xFFF0u),	/* 0xB4~ 0xB7 */
	M_MASK0((ULONG)0xFFFEu),	M_MASK0((ULONG)0x7FF0u),	M_MASK0((ULONG)0x0100u),	M_MASK0((ULONG)0xE100u),	/* 0xB8~ 0xBB */
	M_MASK0((ULONG)0xE100u),	M_MASK0((ULONG)0xE100u),	M_MASK0((ULONG)0xE100u),	M_MASK0((ULONG)0xE100u),	/* 0xBC~ 0xBF */
	M_MASK0((ULONG)0x0F00u),	M_MASK0((ULONG)0x0F00u),	M_MASK0((ULONG)0x0F00u),	M_MASK0((ULONG)0x0F00u),	/* 0xC0~ 0xC3 */
	M_MASK0((ULONG)0x0F00u),	M_MASK0((ULONG)0x0F00u),	M_MASK0((ULONG)0xFFFCu),	NOMASK,						/* 0xC4~ 0xC7 */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0xC8~ 0xCB */
	NOMASK,						NOMASK,						NOMASK,						NOMASK,						/* 0xCC~ 0xCF */
};
/*=============================================================================================
** 全局数据缓冲区
**===========================================================================================*/
USHORT usSPI_Reg_ReadData[DEVICEADDRESS_MAX][BMIC_REG_ADR_MAX_COMMON] = {0u};
ULONG  usSPI_RegFifo_Data[3][32] = {0u};

u8 SDO_H = 0;//test SDO在读操作时的拉高次数
/*=============================================================================================
** 模块内部变量
**===========================================================================================*/

/* --- 请求队列 --- */
static SPIREQ ucSPI_req[SPI_REQ_SIZE];     /* 请求环形缓冲区 */
static UCHAR  ucSPI_req_write_ptr = 0u;     /* 写指针 */
static UCHAR  ucSPI_req_read_ptr = 0u;      /* 读指针 */

/* --- 完成队列 --- */
static SPIREQ ucSPI_end[SPI_REQ_SIZE];     /* 完成环形缓冲区 */
static UCHAR  ucSPI_end_write_ptr = 0u;    /* 完成写指针 */
// static UCHAR  ucSPI_end_read_ptr = 0u;     /* 完成读指针 */

/* --- 当前处理的请求 --- */
static SPIREQ ucSPI_req_current = {0u};

/* --- TX/RX缓冲区 --- */
static UCHAR  ucSPI_Tx_buf[5u] = {0u};     /* 发送缓冲区 (最大5字节: 设备地址、寄存器、数据高位、数据低位、CRC) */
static UCHAR  ucSPI_Rx_buf[66u] = {0u};    /* 接收缓冲区 (最大(0x20+1)*2=66字节) */
// static UCHAR  ucSPI_Tx_DummyData = 0xFFu;  /* RX阶段dummy数据 */
static UCHAR  uc_rx_cnt = 0u;               /* 接收字节计数(用于CRC) */
// static UCHAR  uclength = 0u;                /* 连续读取长度 */

/* --- 状态机 --- */
static volatile eSPI_State eSPI_state = SPI_STATE_IDLE;
static UCHAR ucSPI_Rx_mode = RX_NORMAL;    /* 接收模式 */
static UCHAR ucSPI_communication_error = SPI_NOERR;

/* --- SDO等待 --- */
static UCHAR ucSDO_H_wait = 0u;            /* 1: 等待SDO=H */
static UCHAR ucSDO_L_HoldCnt_write = 0u;   /* SDO=L 超时计数(写) */
static UCHAR ucSDO_L_HoldCnt_read = 0u;    /* SDO=L 超时计数(读) */

/* --- 等待计时 --- */
static USHORT usSPI_wait = 0u;             /* CS释放等待计时(100μs单位) */
static USHORT usSPI_time = 0u;             /* 等待累计 */

/* --- 无SDO模式: 固定延时计数 --- */
#if !SPI_USE_SDO_POLLING
static UCHAR ucFixedDelay_cnt = 0u;        /* 固定延时倒计数 */
static UCHAR ucFixedDelay_target = 0u;     /* 目标延时值 */
#endif

/* --- 同步信号量 --- */
static SemaphoreHandle_t xSPI_DoneSem = NULL;      /* 全局完成信号量 */
static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

/* --- 统计 --- */
static volatile USHORT usSPI_PendingCount = 0u;  /* 待处理请求计数 */


/*=============================================================================================
** 内部函数声明
**===========================================================================================*/
static void vSPI_SndDataSet(void);
static void vSPI_RcvDataSet(void);
static void vSPI_ErrorInit(void);
static UCHAR ucSPICheckSdo(void);
// static void vSPI_Wait(void);

/* DMA异步传输接口 */
static UCHAR ucSPI_DmaTxStart(const UCHAR *pucSrc, USHORT usLen, UCHAR ucSrcInc);
static UCHAR ucSPI_DmaRxStart(UCHAR *pucDst, USHORT usLen);

/*=============================================================================================
** SDO读取 (根据硬件配置)
**===========================================================================================*/
#if SPI_USE_SDO_POLLING
static UCHAR ucSPICheckSdo(void)
{
    /* SDO与SPI3_MISO复用PC11 — 在SPI时钟空闲时读取
     * IDR在任何引脚模式(含AF复用)下始终可读
     * 仅在TX完成(时钟停止)后、RX启动前调用，此时MISO线上无SPI数据
     * SDO=HIGH: 菊花链数据就绪, 可以启动RX
     * SDO=LOW:  菊花链仍在准备, 继续等待 */
    if ((BSPGPIOGetPortState(SPI3_MISO_PORT,SPI3_MISO_PIN)) != 0u) {
        return 1u;  /* HIGH — 数据就绪 */
    }
    return 0u;  /* LOW — 未就绪 */
}
#else
static UCHAR ucSPICheckSdo(void)
{
    /* 无SDO硬件 — 始终返回HIGH以跳过SDO等待 */
    return 1u;
}
#endif

/*=============================================================================================
** CS控制
**===========================================================================================*/
/* 使用BSPKA84917.c中已有的 SPI_CS_Enable()/SPI_CS_Disable() */
/* 这些函数在BSPDIDO.c中定义, 通过BSPKA84917.h的包含链可访问 */

/*=============================================================================================
** 短延时 (仅500ns, 用于CS拉高后等SPI稳定)
**===========================================================================================*/
// static void vSPI_Wait(void)
// {
//     /* STM32G0 @64MHz: 约32个NOP = 500ns */
//     __NOP(); __NOP(); __NOP(); __NOP();
//     __NOP(); __NOP(); __NOP(); __NOP();
//     __NOP(); __NOP(); __NOP(); __NOP();
//     __NOP(); __NOP(); __NOP(); __NOP();
//     __NOP(); __NOP(); __NOP(); __NOP();
//     __NOP(); __NOP(); __NOP(); __NOP();
//     __NOP(); __NOP(); __NOP(); __NOP();
//     __NOP(); __NOP(); __NOP(); __NOP();
// }

/*=============================================================================================
** DMA TX启动 — 异步发送
**===========================================================================================*/
static UCHAR ucSPI_DmaTxStart(const UCHAR *pucSrc, USHORT usLen, UCHAR ucSrcInc)
{
    HAL_StatusTypeDef status;

    /* 配置DMA源地址增量模式 */
    if (ucSrcInc == DMA_SRCADR_FIXED) {
        /* 固定源地址 — 发送dummy数据(0xFF) */
        /* HAL DMA不支持固定源, 但对于0xFF dummy我们可以:
           方案1: 用polling (数据量小, 几个字节)
           方案2: 填充缓冲区为0xFF后用DMA增量
           这里用方案1: RX前的dummy TX用polling, 因为只有2-3字节 */
        status = HAL_SPI_Transmit(&hspi3, (UCHAR*)pucSrc, usLen, 100);
        if (status != HAL_OK) {
            return SPI_ERR;
        }
        /* TX完成即认为RX也准备好(全双工同时完成) */
        vSPI_DmaTxCpltCallback();
        return SPI_NOERR;
    }


    // SEGGER_RTT_printf(0, "MEM: Rx_buf=%p Dummy=%p InsMng=%p gSysData=%p\r\n",
    //     (void*)ucSPI_Rx_buf, (void*)ucDummyBuf, 
    //     (void*)&gs_InsulationDetectMng, (void*)&gSystemData); 
    

    /* 增量源地址 — 正常DMA发送 */
    status = HAL_SPI_Transmit_DMA(&hspi3, (UCHAR*)pucSrc, usLen);
    if (status != HAL_OK) {
        return SPI_ERR;
    }
    return SPI_NOERR;
}

/*=============================================================================================
** DMA RX启动 — 异步接收
**===========================================================================================*/
static UCHAR ucSPI_DmaRxStart(UCHAR *pucDst, USHORT usLen)
{
    HAL_StatusTypeDef status;

    // if(hspi3.State != HAL_SPI_STATE_READY){
    //     HAL_SPI_Abort(&hspi3);
    // }
    /* 全双工模式下, HAL_SPI_Receive_DMA内部会发送0x00 dummy
     * 我们改用 HAL_SPI_TransmitReceive_DMA 发送0xFF dummy */
    /* 先填充dummy缓冲区 */
     static UCHAR ucDummyBuf[66u];
     USHORT i;
     for (i = 0u; i < usLen; i++) {
         ucDummyBuf[i] = 0xFFu;
    }

    //清空rx_buffer
    memset(pucDst, 0, usLen);
    //status = HAL_SPI_Receive_DMA(&hspi3, pucDst, usLen);
    status = HAL_SPI_TransmitReceive_DMA(&hspi3, ucDummyBuf, pucDst, usLen);
    if (status != HAL_OK) {
        return SPI_ERR;
    }
    return SPI_NOERR;
}

/*=============================================================================================
** 发送数据构造 (从请求队列取出请求, 构造TX帧, 启动DMA)
**===========================================================================================*/
static void vSPI_SndDataSet(void)
{
    UCHAR result = SPI_ERR;
    UCHAR ucdev;
    UCHAR ucdev_tx;
    UCHAR ucreg;

    ucdev = ucSPI_req[ucSPI_req_read_ptr].m_ucdev;
    ucreg = ucSPI_req[ucSPI_req_read_ptr].m_ucreg;

    /* 设备方向处理 — STM32单环拓扑, 不需要upper/lower切换 */
    ucdev_tx = ucdev;

    switch (ucSPI_req[ucSPI_req_read_ptr].m_ucreq) {
    case SPI_REQ_WRITE_INIT:
    case SPI_REQ_2BYTE_WRITE:
    {
        USHORT us_tmp;
        USHORT usMask = ucSPI_req[ucSPI_req_read_ptr].m_usMask;
        USHORT usdata = ucSPI_req[ucSPI_req_read_ptr].m_usdata;

        us_tmp = usdata;
        ucSPI_Tx_buf[0u] = ucdev_tx;
        ucSPI_Tx_buf[1u] = ucreg;

        /* 掩码处理 */
        if (usMask != NO_MASK) {
            us_tmp = usSPI_Reg_ReadData[ucdev - 1u][ucreg];
            us_tmp = us_tmp & (usMask ^ 0xFFFFu);
            us_tmp = us_tmp | (usdata & usMask);
            /* 有效位过滤 */
            if (ucdev == BMIC_ADDRESS_922 || ucdev == BMIC_GRPID_922) {
                us_tmp = us_tmp & (USHORT)ulFixedBit_COMMIC[ucreg];
            } else if (ucdev == BMIC_ADDRESS_917 || ucdev == BMIC_GRPID_917) {
                us_tmp = us_tmp & (USHORT)ulFixedBit_CMIC[ucreg];
            }
        }
        ucSPI_Tx_buf[2u] = (UCHAR)(us_tmp >> 8u);
        ucSPI_Tx_buf[3u] = (UCHAR)(us_tmp & 0xFFu);
        ucSPI_Tx_buf[4u] = ucCRC_Calc8BIT(4u, &ucSPI_Tx_buf[0u]);

        /* CS Low + DMA TX */
        SPI_CS_Enable();

        //u8 state = 0;
        //state = BSPGPIOGetPortState(SPI3_MISO_PORT,SPI3_MISO_PIN);

        result = ucSPI_DmaTxStart(&ucSPI_Tx_buf[0u], 5u, DMA_SRCADR_INCREMENT);

       
        //state = BSPGPIOGetPortState(SPI3_MISO_PORT,SPI3_MISO_PIN);

        /* 写操作: 设置CS释放延时 */
        usSPI_wait = SPI_DAISY_DELAY_WRITE;  /* 100μs */
    }
    break;

    case SPI_REQ_2BYTE_READ:
        ucSPI_Tx_buf[0u] = ucdev_tx + SPI_READ_BIT;
        ucSPI_Tx_buf[1u] = ucreg;
        ucSPI_Tx_buf[2u] = ucCRC_Calc8BIT(2u, &ucSPI_Tx_buf[0u]);

        //u8 state = 0;
        //state = BSPGPIOGetPortState(SPI3_MISO_PORT,SPI3_MISO_PIN);

        SPI_CS_Enable();
        result = ucSPI_DmaTxStart(&ucSPI_Tx_buf[0u], 3u, DMA_SRCADR_INCREMENT);
     
        //state = BSPGPIOGetPortState(SPI3_MISO_PORT,SPI3_MISO_PIN);
        break;

    case SPI_REQ_CONTINUE_READ:
    case SPI_REQ_FIFO_CONT_READ:
        ucSPI_Tx_buf[0u] = ucdev_tx + (SPI_READ_BIT | CONT_BIT);
        ucSPI_Tx_buf[1u] = ucreg;
        ucSPI_Tx_buf[2u] = (UCHAR)(((ucSPI_req[ucSPI_req_read_ptr].m_usdata) - 1u) & 0x1Fu);
        ucSPI_Tx_buf[3u] = ucCRC_Calc8BIT(3u, &ucSPI_Tx_buf[0u]);

        SPI_CS_Enable();
        result = ucSPI_DmaTxStart(&ucSPI_Tx_buf[0u], 4u, DMA_SRCADR_INCREMENT);
        break;

    case SPI_REQ_WAIT:
        usSPI_wait = ucSPI_req[ucSPI_req_read_ptr].m_usdata;
        ucSPI_req_read_ptr = (ucSPI_req_read_ptr + 1u) & SPI_REQ_NO_MASK;
        usSPI_PendingCount--;
        return;  /* WAIT请求不入当前请求, 直接返回 */

    default:
        result = SPI_ERR;
        break;
    }

    if (result == SPI_ERR) {
        vSPI_ErrorInit();
        ucSPI_communication_error = SPI_ERR;
    } else {
        /* 保存当前请求信息 */
        ucSPI_req_current.m_ucdev = ucdev;
        ucSPI_req_current.m_ucreq = ucSPI_req[ucSPI_req_read_ptr].m_ucreq;
        ucSPI_req_current.m_ucreg = ucreg;
        ucSPI_req_current.m_usdata = ucSPI_req[ucSPI_req_read_ptr].m_usdata;
        ucSPI_req_current.m_usMask = ucSPI_req[ucSPI_req_read_ptr].m_usMask;
        ucSPI_req_read_ptr = (ucSPI_req_read_ptr + 1u) & SPI_REQ_NO_MASK;

        /* 状态切换到TX_CMD */
        eSPI_state = SPI_STATE_TX_CMD;

        // /* 发送第1字节 (命令字节) — 直接写入DR启动传输 */
        // vSPI_Wait();
        // HAL_SPI_Transmit(&hspi3, &ucSPI_Tx_buf[0u], 1u, 10);
    }
}

/*=============================================================================================
** 接收数据解析 (DMA RX完成后, CRC校验+数据填充)
**===========================================================================================*/
static void vSPI_RcvDataSet(void)
{
    USHORT i;
    UCHAR ucStartIndex;
    // USHORT uslength;
    UCHAR ucreg_idx;

    switch (ucSPI_req_current.m_ucreq) {
    case SPI_REQ_2BYTE_READ:
    {
        USHORT registerdata;
        USHORT crc_result;

        /* CRC8校验 */
        crc_result = (USHORT)ucCRC_Calc8BIT(2u, ucSPI_Rx_buf);
        if (crc_result != ucSPI_Rx_buf[2u]) {
            ucSPI_communication_error = SPI_CRCERR;
            break;
        }

        /* 数据填充 */
        registerdata = (USHORT)((USHORT)ucSPI_Rx_buf[0u] << 8u) + ucSPI_Rx_buf[1u];
        /* 有效位过滤 */
        if (ucSPI_req_current.m_ucdev == BMIC_ADDRESS_922 || ucSPI_req_current.m_ucdev == BMIC_GRPID_922) {
            registerdata = registerdata & (USHORT)ulFixedBit_COMMIC[ucSPI_req_current.m_ucreg];
        } else if (ucSPI_req_current.m_ucdev == BMIC_ADDRESS_917 || ucSPI_req_current.m_ucdev == BMIC_GRPID_917) {
            registerdata = registerdata & (USHORT)ulFixedBit_CMIC[ucSPI_req_current.m_ucreg];
        }
        usSPI_Reg_ReadData[ucSPI_req_current.m_ucdev - 1u][ucSPI_req_current.m_ucreg] = registerdata;
    }
    break;

    case SPI_REQ_CONTINUE_READ:
    {
        USHORT registerdata;
        USHORT u16CRC_calc, u16CRC_rcev;

        ucStartIndex = ucSPI_req_current.m_ucreg;
        // uslength = (ucSPI_req_current.m_usdata + 1u) * 2u;;

        /* CRC16校验 */
        u16CRC_calc = usCRC_Calc16BIT(((ucSPI_req_current.m_usdata + 1u) * 2u -2u), ucSPI_Rx_buf);
        u16CRC_rcev = (USHORT)((USHORT)ucSPI_Rx_buf[(ucSPI_req_current.m_usdata + 1u) * 2u -2u] << 8u) + ucSPI_Rx_buf[(ucSPI_req_current.m_usdata + 1u) * 2u-1u];

        if (u16CRC_calc != u16CRC_rcev) {
            ucSPI_communication_error = SPI_CRCERR;
            break;
        }

        /* 数据填充 */
        for (i = 0u; i < ucSPI_req_current.m_usdata; i++) {
            registerdata = (USHORT)((USHORT)ucSPI_Rx_buf[i * 2u] << 8u) + ucSPI_Rx_buf[i * 2u + 1u];
            if (ucSPI_req_current.m_ucdev == BMIC_ADDRESS_922 || ucSPI_req_current.m_ucdev == BMIC_GRPID_922) {
                registerdata = registerdata & (USHORT)ulFixedBit_COMMIC[ucStartIndex + i];
            } else if (ucSPI_req_current.m_ucdev == BMIC_ADDRESS_917 || ucSPI_req_current.m_ucdev == BMIC_GRPID_917) {
                registerdata = registerdata & (USHORT)ulFixedBit_CMIC[ucStartIndex + i];
            }
            usSPI_Reg_ReadData[ucSPI_req_current.m_ucdev - 1u][ucStartIndex + i] = registerdata;
        }
    }
    break;

    case SPI_REQ_FIFO_CONT_READ:
    {
        ULONG registerdata;
        USHORT u16CRC_calc, u16CRC_rcev;

        // uslength = (ucSPI_req_current.m_usdata + 1u) * 2u;;

        /* CRC16校验 */
        u16CRC_calc = usCRC_Calc16BIT(((ucSPI_req_current.m_usdata + 1u) * 2u -2u), ucSPI_Rx_buf);
        u16CRC_rcev = (USHORT)((USHORT)ucSPI_Rx_buf[(ucSPI_req_current.m_usdata + 1u) * 2u -2u] << 8u) + ucSPI_Rx_buf[(ucSPI_req_current.m_usdata + 1u) * 2u-1u];

        if (u16CRC_calc != u16CRC_rcev) {
            ucSPI_communication_error = SPI_CRCERR;
            break;
        }

        /* FIFO数据填充 */
        if ((CMIC_FIFO_C_P_C7 <= ucSPI_req_current.m_ucreg) && (ucSPI_req_current.m_ucreg <= CMIC_FIFO_C_S_C9)) {
            ucreg_idx = ucSPI_req_current.m_ucreg - CMIC_FIFO_C_P_C7;
        } else {
            ucreg_idx = 0u;
        }
        for (i = 0u; i < ucSPI_req_current.m_usdata/2; i++) {
            registerdata = (ULONG)((ULONG)ucSPI_Rx_buf[i * 4u] << 24u)
                         + (ULONG)((ULONG)ucSPI_Rx_buf[i * 4u + 1u] << 16u)
                         + (ULONG)((ULONG)ucSPI_Rx_buf[i * 4u + 2u] << 8u)
                         + ucSPI_Rx_buf[i * 4u + 3u];
            usSPI_RegFifo_Data[ucreg_idx][i] = registerdata;
        }
    }
    break;

    default:
        /* do nothing */
        break;
    }
}

/*=============================================================================================
** 错误初始化
**===========================================================================================*/
static void vSPI_ErrorInit(void)
{
    /* 中止DMA传输 */
    HAL_SPI_Abort(&hspi3);

    /* CS释放 */
    SPI_CS_Disable();

    /* 状态复位 */
    eSPI_state = SPI_STATE_IDLE;
    ucSPI_Rx_mode = RX_NORMAL;
    ucSDO_H_wait = 0u;
    // ucSPI_Tx_DummyData = 0xFFu;

     /* 补充: 错误时减计数，避免 PendingCount 泄漏 */
     if (usSPI_PendingCount > 0u) {
        usSPI_PendingCount--;
    }
    if (xSPI_DoneSem != NULL) {
        xSemaphoreGiveFromISR(xSPI_DoneSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*=============================================================================================
** 定时器中断处理 — 状态机核心
** 从TIM6 100μs中断调用
**===========================================================================================*/
void vSpiTimer_ISR(void)
{
    /* --- 处理SDO等待 --- */
    if (ucSDO_H_wait == 1u) {
        if (ucSPICheckSdo() == 1u) {
            /* SDO=H, 菊花链数据就绪 */
            ucSDO_H_wait = 0u;
            SDO_H++;

        } else {
            /* SDO=L, 增加超时计数 */
            if (ucSPI_Rx_mode == RX_2BYTE_READ_CMD || ucSPI_Rx_mode == RX_CONTINUE_READ_CMD) {
                ucSDO_L_HoldCnt_read++;
                if (ucSDO_L_HoldCnt_read > SDO_H_WAIT_READ) {
                    ucSPI_communication_error = SPI_TIME_OUT_ERR;
                    vSPI_ErrorInit();
                    return;
                }
            } else {
                ucSDO_L_HoldCnt_write++;
                if (ucSDO_L_HoldCnt_write > SDO_H_WAIT_WRITE) {
                    ucSPI_communication_error = SPI_TIME_OUT_ERR;
                    vSPI_ErrorInit();
                    return;
                }
            }
        }
    }

    /* --- 无SDO模式: 固定延时倒计数 --- */
#if !SPI_USE_SDO_POLLING
    if (ucFixedDelay_cnt > 0u) {
        ucFixedDelay_cnt--;
        if (ucFixedDelay_cnt == 0u) {
            /* 延时到期, 模拟SDO=H */
            ucSDO_H_wait = 0u;
        }
    }
#endif

    /* --- 状态机主体 --- */
    switch (eSPI_state) {
    case SPI_STATE_IDLE:
    {
        /* 检查等待计时 */
        if ((usSPI_wait != 0u) && (usSPI_time < usSPI_wait)) {
            usSPI_time++;
            return;
        }

        /* 检查SDO等待 */
        if (ucSDO_H_wait == 1u) {
            return;
        }

        /* 检查请求队列 */
        if (ucSPI_req_read_ptr != ucSPI_req_write_ptr) {
            ucSPI_communication_error = SPI_NOERR;
            ucSDO_L_HoldCnt_read = 0u;
            ucSDO_L_HoldCnt_write = 0u;
            ucSPI_Rx_mode = RX_NORMAL;
            vSPI_SndDataSet();
        }
    }
    break;

    case SPI_STATE_TX_CMD:
        /* TX DMA正在进行, 等待完成回调 */
        break;

    case SPI_STATE_WAIT_SDO:
    {
        /* SDO等待中 — 当SDO=H时启动RX DMA */
        if (ucSDO_H_wait == 0u) {
            UCHAR result;

            if (ucSPI_Rx_mode == RX_2BYTE_READ_CMD) {
                ucSPI_Rx_mode = RX_2BYTE_READ;
                uc_rx_cnt = 2u;
                result = ucSPI_DmaRxStart(ucSPI_Rx_buf, 3u);  /* 2字节数据+1字节CRC8 */
                if (result == SPI_ERR) {
                    vSPI_ErrorInit();
                    ucSPI_communication_error = SPI_ERR;
                } else {
                    eSPI_state = SPI_STATE_RX_DATA;
                }
            }
            else if (ucSPI_Rx_mode == RX_CONTINUE_READ_CMD) {
                ucSPI_Rx_mode = RX_CONTINUE_READ;
                // result = ucSPI_DmaRxStart(ucSPI_Rx_buf, (USHORT)(uclength + 2u));
                result = ucSPI_DmaRxStart(ucSPI_Rx_buf, (USHORT)((ucSPI_req_current.m_usdata + 1u) * 2u));
                if (result == SPI_ERR) {
                    vSPI_ErrorInit();
                    ucSPI_communication_error = SPI_ERR;
                } else {
                    eSPI_state = SPI_STATE_RX_DATA;
                }
            }
        }
    }
    break;

    case SPI_STATE_WAIT_CS_OFF:
    {
        /* 写操作后等待计时 */
        if (usSPI_wait == 0u || usSPI_time >= usSPI_wait) {
            SPI_CS_Disable();
            /* 入完成队列 */
            ucSPI_end[ucSPI_end_write_ptr] = ucSPI_req_current;
            ucSPI_end_write_ptr = (ucSPI_end_write_ptr + 1u) & SPI_REQ_NO_MASK;
            usSPI_PendingCount--;
            eSPI_state = SPI_STATE_IDLE;
            usSPI_wait = 0u;
            usSPI_time = 0u;

            /* 释放信号量 (FromISR) */
            if (xSPI_DoneSem != NULL) {
                xSemaphoreGiveFromISR(xSPI_DoneSem, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        } else {
            usSPI_time++;
        }
    }
    break;

    case SPI_STATE_RX_DATA:
        /* RX DMA正在进行, 等待完成回调 */
        break;

    case SPI_STATE_ERROR:
        /* 错误状态, 等待外部清除 */
        break;

    default:
        break;
    }
}

void VSPI_TimeOut_Abort(void)
{
    HAL_SPI_Abort(&hspi3);
    HAL_DMA_Abort(hspi3.hdmatx);
    HAL_DMA_Abort(hspi3.hdmarx);
    vSPI_Async_Init();  // 重建信号量
    ucSPI_communication_error = SPI_NOERR;
    eSPI_state = SPI_STATE_IDLE;
    SEGGER_RTT_printf(0, "SPI recovery");
}


/*=============================================================================================
** DMA TX完成回调 — 从HAL_SPI_TxCpltCallback调用
**===========================================================================================*/
void vSPI_DmaTxCpltCallback(void)
{
    if (eSPI_state != SPI_STATE_TX_CMD) {
        return;  /* 非预期回调, 忽略 */
    }

    /* TX完成, 判断是读还是写 */
    if (ucSPI_req_current.m_ucreq == SPI_REQ_2BYTE_WRITE || ucSPI_req_current.m_ucreq == SPI_REQ_WRITE_INIT) {
        /* 写操作: 不需要RX, 进入CS释放等待 */
        eSPI_state = SPI_STATE_WAIT_CS_OFF;
        usSPI_time = 0u;
    }
    else if (ucSPI_req_current.m_ucreq == SPI_REQ_2BYTE_READ) {
        /* 2字节读: 切换到SDO等待 */
        ucSPI_Rx_mode = RX_2BYTE_READ_CMD;
        ucSDO_H_wait = 1u;
        ucSDO_L_HoldCnt_read = 0u;
        eSPI_state = SPI_STATE_WAIT_SDO;

#if !SPI_USE_SDO_POLLING
        /* 固定延时模式: 设定延时计数 */
        ucFixedDelay_cnt = SPI_DAISY_DELAY_2BYTE;
#endif
    }
    else if (ucSPI_req_current.m_ucreq == SPI_REQ_CONTINUE_READ || ucSPI_req_current.m_ucreq == SPI_REQ_FIFO_CONT_READ) {
        /* 连续读: 切换到SDO等待 */
        ucSPI_Rx_mode = RX_CONTINUE_READ_CMD;
        ucSDO_H_wait = 1u;
        ucSDO_L_HoldCnt_read = 0u;
        eSPI_state = SPI_STATE_WAIT_SDO;

        /* 计算连续读取长度 */
        // if ((ucSPI_Tx_buf[0u] & CONT_BIT) != 0u) {
            // uclength = (ucSPI_req_current.m_usdata + 1u) * 2u;
            // uclength = (ucSPI_Tx_buf[2u] + 1u) * 2u;
        // }
        

#if !SPI_USE_SDO_POLLING
        if (ucSPI_req_current.m_ucreq == SPI_REQ_FIFO_CONT_READ) {
            ucFixedDelay_cnt = SPI_DAISY_DELAY_FIFO;
        } else {
            ucFixedDelay_cnt = SPI_DAISY_DELAY_CONT;
        }
#endif
    }
}

/*=============================================================================================
** DMA RX完成回调 — 从HAL_SPI_RxCpltCallback调用
**===========================================================================================*/
void vSPI_DmaRxCpltCallback(void)
{
    if (eSPI_state != SPI_STATE_RX_DATA) {
        return;  /* 非预期回调, 忽略 */
    }

    /* RX完成: CRC校验 + 数据填充 */
    vSPI_RcvDataSet();
     /* CS释放 */
     SPI_CS_Disable();

    //  u8 state = 0;
    // state = BSPGPIOGetPortState(SPI3_MISO_PORT,SPI3_MISO_PIN);

    /* 入完成队列 */
    ucSPI_end[ucSPI_end_write_ptr] = ucSPI_req_current;
    ucSPI_end_write_ptr = (ucSPI_end_write_ptr + 1u) & SPI_REQ_NO_MASK;
    usSPI_PendingCount--;

    /* 状态回到IDLE */
    eSPI_state = SPI_STATE_IDLE;
    usSPI_wait = 0u;
    usSPI_time = 0u;
    ucSPI_Rx_mode = RX_NORMAL;
    ucSDO_H_wait = 0u;

    /* 释放信号量 (FromISR) */
    if (xSPI_DoneSem != NULL) {
        xSemaphoreGiveFromISR(xSPI_DoneSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*=============================================================================================
** 初始化
**===========================================================================================*/
void vSPI_Async_Init(void)
{
    /* 清除请求队列 */
    ucSPI_req_write_ptr = 0u;
    ucSPI_req_read_ptr = 0u;
    ucSPI_end_write_ptr = 0u;
    // ucSPI_end_read_ptr = 0u;

    /* 清除状态 */
    eSPI_state = SPI_STATE_IDLE;
    ucSPI_Rx_mode = RX_NORMAL;
    ucSPI_communication_error = SPI_NOERR;
    ucSDO_H_wait = 0u;
    ucSDO_L_HoldCnt_write = 0u;
    ucSDO_L_HoldCnt_read = 0u;
    uc_rx_cnt = 0u;
    // uclength = 0u;
    usSPI_wait = 0u;
    usSPI_time = 0u;
    usSPI_PendingCount = 0u;

#if !SPI_USE_SDO_POLLING
    ucFixedDelay_cnt = 0u;
    ucFixedDelay_target = 0u;
#endif

    /* 创建完成信号量 */
    if (xSPI_DoneSem == NULL) {
        xSPI_DoneSem = xSemaphoreCreateCounting(32u, 0u);
    }
}

// /*=============================================================================================
// ** 启动定时器节拍
// **===========================================================================================*/
// void vSPI_Async_Start(void)
// {
//     /* TIM14: 100μs周期中断
//      * 已在STM32CubeMX中配置, 这里只需启动 */
//     HAL_TIM_Base_Start_IT(&htim14);
// }

/*=============================================================================================
** 请求入队 (从FreeRTOS任务调用, 非阻塞)
**===========================================================================================*/
void vSPI_ReqSet(UCHAR ucdev, UCHAR ucreq, UCHAR ucreg, USHORT usdata, USHORT usMask)
{
    UCHAR next_write_ptr;

    /* 关中断保护环形缓冲区写操作 */
    taskENTER_CRITICAL();

    next_write_ptr = (ucSPI_req_write_ptr + 1u) & SPI_REQ_NO_MASK;
    if (next_write_ptr == ucSPI_req_read_ptr) {
        /* 队列满 — 丢弃请求(与Nuvoton行为一致) */
        taskEXIT_CRITICAL();
        return;
    }

    ucSPI_req[ucSPI_req_write_ptr].m_ucdev = ucdev;
    ucSPI_req[ucSPI_req_write_ptr].m_ucreq = ucreq;
    ucSPI_req[ucSPI_req_write_ptr].m_ucreg = ucreg;
    ucSPI_req[ucSPI_req_write_ptr].m_usdata = usdata;
    ucSPI_req[ucSPI_req_write_ptr].m_usMask = usMask;
    ucSPI_req_write_ptr = next_write_ptr;
    usSPI_PendingCount++;

    taskEXIT_CRITICAL();
}

/*=============================================================================================
** 获取通信错误
**===========================================================================================*/
UCHAR ucSPI_GetComError(void)
{
    return ucSPI_communication_error;
}

/*=============================================================================================
** 清除通信错误
**===========================================================================================*/
void vSPI_ErrorClear(void)
{
    ucSPI_communication_error = SPI_NOERR;
    eSPI_state = SPI_STATE_IDLE;
}

/*=============================================================================================
** 获取状态机状态
**===========================================================================================*/
eSPI_State eSPI_GetState(void)
{
    return eSPI_state;
}

/*=============================================================================================
** 等待所有请求完成
**===========================================================================================*/
UCHAR ucSPI_WaitAllDone(ULONG ulTimeoutMs)
{
    ULONG ulStartTick;
    ULONG ulElapsed;

    ulStartTick = xTaskGetTickCount();

    while (usSPI_PendingCount > 0u) {
        ulElapsed = (xTaskGetTickCount() - ulStartTick) * portTICK_PERIOD_MS;
        if (ulElapsed >= ulTimeoutMs) {
            return SPI_ERR;  /* 超时 */
        }
        /* 等待信号量(带超时) */
        if (xSemaphoreTake(xSPI_DoneSem, pdMS_TO_TICKS(10)) == pdTRUE) {
            /* 一个请求完成, 继续检查是否全部完成 */
        }
    }

    return ucSPI_GetComError();
}

/*=============================================================================================
** 等待特定请求完成 (简化版, 等待队列清空)
**===========================================================================================*/
UCHAR ucSPI_WaitReqDone(USHORT usReqMask, ULONG ulTimeoutMs)
{
    (void)usReqMask;  /* 未使用, 保留接口 */
    return ucSPI_WaitAllDone(ulTimeoutMs);
}
#endif