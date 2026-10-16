/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : spi_async.h
** @Founder      : WYP40505
** @Date         : 2026.05.06
** @Function     : 异步SPI通信状态机 — 基于定时器中断+DMA的菊花链通信驱动
** @Instructions : 移植自Nuvoton spi.c的vSpiTimer+DMA架构，适配STM32G0xx+FreeRTOS
**===========================================================================================*/
#if 1
#ifndef __SPI_ASYNC_H
#define __SPI_ASYNC_H

#include "BSPTypeDef.h"
#include "main.h"
#include "BSPKA84917_async.h"
// #include "BSPKA84917.h"
// #include "BSPKA84917.h"
/*=============================================================================================
** 宏定义
**===========================================================================================*/

/* SDO轮询使能 — 如果硬件连接了KA84920的SDO引脚到MCU GPIO，则设为1 */
#define SPI_USE_SDO_POLLING     (1u)

/* SDO引脚 = SPI3_MISO (PC11) — KA84920的SDO与MISO复用同一引脚
 * TX完成后、RX启动前，SPI时钟空闲，此时MISO线上电平 = SDO状态
 * LOW = 菊花链未就绪, HIGH = 数据就绪可以开始读
 * GPIO IDR在任何引脚模式下(含AF)始终可读 — STM32G0参考手册 */
#define SPI_SDO_GPIO_PORT       GPIOC
#define SPI_SDO_GPIO_PIN        GPIO_PIN_11  /* PC11 = SPI3_MISO = KA84920 SDO */

/* 请求队列大小 (必须是2的幂) */
#define SPI_REQ_NO_MASK         (0x7Fu)  /* 128 entries */
#define SPI_REQ_SIZE            (SPI_REQ_NO_MASK + 1u)

/* 错误码 */
#define SPI_NOERR               (UCHAR)(0xA0u)
#define SPI_ERR                 (UCHAR)(0xA1u)
#define SPI_CONTINUE            (UCHAR)(0xA2u)
#define SPI_CRCERR              (UCHAR)(0xA3u)
#define SPI_TIME_OUT_ERR        (UCHAR)(0xA4u)
#define SPI_DAISY_ERR           (UCHAR)(0xA5u)

/* 请求类型 */
#define SPI_REQ_2BYTE_WRITE     (UCHAR)(3u)
#define SPI_REQ_2BYTE_READ      (UCHAR)(4u)
#define SPI_REQ_WRITE_INIT      (UCHAR)(5u)
#define SPI_REQ_CONTINUE_READ   (UCHAR)(6u)
#define SPI_REQ_WAIT            (UCHAR)(8u)
#define SPI_REQ_FIFO_CONT_READ  (UCHAR)(15u)

/* RX模式 */
#define RX_NORMAL               (0x00u)
#define RX_2BYTE_READ           (0x02u)
#define RX_CONTINUE_READ        (0x03u)
#define RX_2BYTE_READ_CMD       (0x06u)
#define RX_CONTINUE_READ_CMD    (0x07u)

/* R/W和Continuous位 */
#define SPI_READ_BIT                (0x80u)
#define CONT_BIT                (0x40u)

/* SDO超时计数 (单位: 100μs) */
#define SDO_H_WAIT_READ         (20u)   /* 2000μs */
#define SDO_H_WAIT_WRITE        (20u)   /* 2000μs */

/* DMA源地址模式 */
#define DMA_SRCADR_FIXED        (0u)
#define DMA_SRCADR_INCREMENT    (1u)

/* 菊花链延时 (无SDO时使用, 单位: 100μs) */
#define SPI_DAISY_DELAY_2BYTE   (3u)    /* 300μs — 2字节读等待 */
#define SPI_DAISY_DELAY_CONT    (7u)    /* 700μs — 连续读等待 */
#define SPI_DAISY_DELAY_FIFO    (7u)    /* 700μs — FIFO读等待 */
#define SPI_DAISY_DELAY_WRITE   (1u)    /* 100μs — 写操作后等待 */

/* FIFO数组最大值 */
#define SPI_FIFO_ARRAY_MAX      (256u)

/*=============================================================================================
** 数据类型定义
**===========================================================================================*/

/* SPI请求结构体 */
typedef struct {
    UCHAR m_ucdev;      /* 设备地址 */
    UCHAR m_ucreq;      /* 通信请求类型 */
    UCHAR m_ucreg;      /* 寄存器地址 */
    USHORT m_usdata;    /* 写入数据 或 读取数量 */
    USHORT m_usMask;    /* 掩码数据 */
} SPIREQ;

/* SPI状态机状态 */
typedef enum {
    SPI_STATE_IDLE = 0,     /* 空闲，等待请求 */
    SPI_STATE_TX_CMD,       /* 正在发送命令(DMA TX) */
    SPI_STATE_WAIT_SDO,     /* 等待SDO=H(菊花链数据就绪) */
    SPI_STATE_RX_DATA,      /* 正在接收数据(DMA RX) */
    SPI_STATE_WAIT_CS_OFF,  /* 写操作后等待CS释放时机 */
    SPI_STATE_ERROR         /* 错误状态 */
} eSPI_State;

/*=============================================================================================
** 外部变量声明
**===========================================================================================*/

/* 接收数据缓冲区 — 供BSPKA84917.c直接访问 */
extern USHORT usSPI_Reg_ReadData[DEVICEADDRESS_MAX][BMIC_REG_ADR_MAX_COMMON];


/*=============================================================================================
** 函数声明
**===========================================================================================*/
void VSPI_TimeOut_Abort(void);//超时复位
/* 初始化 */
void vSPI_Async_Init(void);

/* 启动定时器节拍 (在FreeRTOS启动后调用) */
// void vSPI_Async_Start(void);

/* 请求入队 (非阻塞，从任务调用) */
void vSPI_ReqSet(UCHAR ucdev, UCHAR ucreq, UCHAR ucreg, USHORT usdata, USHORT usMask);

/* 定时器中断回调 (从TIM14中断调用) */
void vSpiTimer_ISR(void);

/* DMA完成回调 (从HAL回调调用) */
void vSPI_DmaTxCpltCallback(void);
void vSPI_DmaRxCpltCallback(void);

/* 获取通信错误状态 */
UCHAR ucSPI_GetComError(void);

/* 清除通信错误 */
void vSPI_ErrorClear(void);

/* 获取当前状态机状态 (调试用) */
eSPI_State eSPI_GetState(void);

/* 等待所有请求完成 (阻塞，带超时) */
UCHAR ucSPI_WaitAllDone(ULONG ulTimeoutMs);

/* 等待特定操作完成 (阻塞，带超时) */
UCHAR ucSPI_WaitReqDone(USHORT usReqMask, ULONG ulTimeoutMs);


#endif /* __SPI_ASYNC_H */
#endif
