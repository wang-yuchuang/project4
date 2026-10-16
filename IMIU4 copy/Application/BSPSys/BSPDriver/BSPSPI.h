/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPSPI.h
** @Founder      : WYC834
** @Date         : 2025.09.08
** @Function     : SPI通讯驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPSPI_H
#define _BSPSPI_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "main.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
//新增
#define SPI1_MULT_PIN         1                //SPI1管脚选择:0-使用PA5/PA6/PA7,1-使用PB3/PB4/PB5
#define SPI3_MULT_PIN         1                //SPI3管脚选择:0-使用PB3/PB4/PB5,1-使用PC10/PC11/PC12
#define SPI0_MASTER_MODE_EN   1				/*SPI0主机模式使能，0：从机模式    1：主机模式*/
#define SPI1_MASTER_MODE_EN   1				/*SPI1主机模式使能，0：从机模式    1：主机模式*/
#define SPI_TIMEOUT 0x5U
#define DRV_GD25Q16_IDENTIFICATION ((uint32)(0x00C84015))
#define DRV_GD25Q32_IDENTIFICATION ((uint32)(0x00C84016))
#define DRV_GD25Q32_PAGE_SIZE 256U
#define DRV_GD25Q16_BLOCK_NUM 32U
#define DRV_GD25Q16_SIZE ((uint32)(0x200000U))
#define DRV_GD25Q32_BLOCK_NUM 64U
#define DRV_GD25Q32_SIZE ((uint32)(0x400000U))
/*=============================================================================================
** @Data type definition
**===========================================================================================*/
//SPI通道定义
typedef enum 
{
    eSPI1 = 0,
    eSPI3 = 2,
    eSPINUM
}e_SPINum;

typedef enum {
    FLASH_STATUS_ACCESS_OK,
    FLASH_STATUS_ACCESS_FAIL,
    FLASH_STATUS_INVALID_ADD,
    FLASH_STATUS_WAIT,
    FLASH_NUM_STATUS,
} DRV_GD25QXX_FlashStatus;

typedef struct
{
//	u32 spi;
//    u32 dma;
//	dma_channel_enum dmaTxCh;
//	dma_channel_enum dmaRxCh;
	u8 dmaTxDone;
	u8 dmaRxDone;
}M_SPI_t;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPSPIInit(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : SPI驱动模块初始化
** @The notes : BSP调用
**===========================================================================================*/
void BSPSPIInit(e_SPINum spiNum);

/*=============================================================================================
** @Name      : u8 BSPSPIGetWorkState(e_SPINum spiNum)
** @Input     : sspiNum:SPI通道号
** @Output    : SPI工作状态 0:空闲 1:忙
** @Function  : 获取SPI工作状态
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPSPIGetWorkState(e_SPINum spiNum);

/*=============================================================================================
** @Name      : u8 BSPSPIDmaSendRcvData(e_SPINum spiNum, u8 * tdata, u8 *rdata, u8 size)
** @Input     : sspiNum:SPI通道号 *tdata:需要发送的数据 *rdata:需要接收的数据 size: 数据的大小
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SPI通过DMA发送和接收数据
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPSPIDmaSendRcvData(e_SPINum spiNum, u8 *tdata, u8 *rdata, u8 size);

/*=============================================================================================
** @Name      : u8 BSPSPISendRcvData(e_SPINum spiNum, u8 *tdata, u8 *rdata, u8 size)
** @Input     : sspiNum:SPI通道号 *tdata 需要发送的数据 *rdata 接收的数据 size:数据大小
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SPI直接发送和接收数据
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPSPISendRcvData(e_SPINum spiNum, u8 *tdata, u8 *rdata, u8 size);

/*=============================================================================================
** @Name      : u16 BSPSPIGetRegData(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : void
** @Function  : 获得SPI接收到寄存器的数据
** @The notes : 用户调用
**===========================================================================================*/
u16 BSPSPIGetRegData(e_SPINum spiNum);

/*=============================================================================================
** @Name      : u8 BSPSPIRcvData(e_SPINum spiNum, u8 *data, u8 size)
** @Input     : spiNum:SPI通道号 data:接收的数据 size:数据大小
** @Output    : 是否接收成功
** @Function  : 获取SPI接收数据
** @The notes :
**===========================================================================================*/
u8 BSPSPIRcvData(e_SPINum spiNum, u8 *data, u8 size);

/*=============================================================================================
** @Name      : u8 BSPSPISendData(e_SPINum spiNum, u8 *data ,u8 size)
** @Input     : spiNum:SPI通道号  data:需要发送的数据 size:数据大小
** @Output    : 发送是否成功
** @Function  : SPI发送数据
** @The notes :
**===========================================================================================*/
u8 BSPSPISendData(e_SPINum spiNum, u8 *data ,u8 size);


uint8 DRV_SPI2_SendByteBurst(uint8 *u8SendByte, uint8 *u8ReadByte, uint16 u16Size);
uint8 DRV_SPI2_SendByte(uint8 u8Byte, uint8 *u8ReadByte);

#endif

