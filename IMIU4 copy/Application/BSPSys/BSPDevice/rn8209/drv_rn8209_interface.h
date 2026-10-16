/**
  ******************************************************************************
  * @file    drv_rn8209_interface.h
  * @author  ZZC955
  * @version V1.0
  * @date    2023-9-9
  * @brief   gd25qxx芯片读写等操作接口说明
  * @history 2023-9-9 ZZC955 创建
  ******************************************************************************
*/

#ifndef DRV_RN9209_INTERFACE_H
#define DRV_RN9209_INTERFACE_H

#include "drv_rn8209_type.h"

#ifdef __cplusplus
extern "C" {
#endif


extern  uint8 Rn8209InitSuccessFlag; //8209读取ID值标志
/*=============================================================================
* Function      : RN8209ReadCMD_DEVICE_ID
* reentrant     : 
* syn/asyn      : 
* Input         : none
* Output        : none
* Returns       : none
* Description   : 工装检测8209读取ID功能
*=============================================================================*/
void RN8209ReadCMD_DEVICE_ID(void);

/*=============================================================================
* Function      : RN8209_DRV_Init
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : none
* Output        : none
* Returns       : none
* Description   : 初始化RN8209软件模块，此接口需放在SPI初始化之后
*=============================================================================*/
u8 RN8209_DRV_Init(TickType_t xLastWakeTime);

/*=============================================================================
* Function      : DRV_RN8209_ReadAdcIA
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : data-存放20位AD值
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 读RN8209 IA通道20位AD值，补码形式
*=============================================================================*/
uint8 DRV_RN8209_ReadAdcIA(uint32* data);

uint8 DRV_RN8209_ReadAdcIA_Fast(uint32* data);

/*=============================================================================
* Function      : DRV_RN8209_ReadAdcU
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : data-存放20位AD值
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 读RN8209 U通道20位AD值，补码形式
*=============================================================================*/
uint8 DRV_RN8209_ReadAdcU(uint32* data);
uint8 DRV_RN8209_ReadAdcU_Fast(uint32* data);

/*=============================================================================
* Function      : DRV_RN8209_ReadAdcIA_Fast_V2
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : data-存放IA通道20位AD值(补码)
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 读IA电流ADC — 32-bit GPSPI帧, @1Mbps, Daisy~8次 (v2优化版)
*=============================================================================*/
uint8 DRV_RN8209_ReadAdcIA_Fast_V2(uint32* data);

/*=============================================================================
* Function      : DRV_RN8209_ReadAdcU_Fast_V2
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : data-存放U通道20位AD值(补码)
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 读U电压ADC — 32-bit GPSPI帧, @1Mbps, Daisy~8次 (v2优化版)
*=============================================================================*/
uint8 DRV_RN8209_ReadAdcU_Fast_V2(uint32* data);

/*=============================================================================
* Function      : DRV_RN8209_ReadAdcIA_Fast_V3
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : data-存放IA通道20位AD值(补码)
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 读IA电流ADC — 批量入队版, SPI状态机连续处理, 耗时~1.5ms (v3优化版)
*=============================================================================*/
uint8 DRV_RN8209_ReadAdcIA_Fast_V3(uint32* data);

/*=============================================================================
* Function      : DRV_RN8209_ReadAdcU_Fast_V3
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : data-存放U通道20位AD值(补码)
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 读U电压ADC — 批量入队版, SPI状态机连续处理, 耗时~1.5ms (v3优化版)
*=============================================================================*/
uint8 DRV_RN8209_ReadAdcU_Fast_V3(uint32* data);

/*=============================================================================
* Function      : DRV_RN8209_ReadRmsIA
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : data-存放24位有效值
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 读RN8209 IA通道24位有效值，补码形式
*=============================================================================*/
uint8 DRV_RN8209_ReadRmsIA(uint32* data);

/*=============================================================================
* Function      : DRV_RN8209_ReadRmsU
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : data-存放24位有效值
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 读RN8209 U通道24位有效值，补码形式
*=============================================================================*/
uint8 DRV_RN8209_ReadRmsU(uint32* data);

/*=============================================================================
* Function      : DRV_RN8209_WriteRmsIAOffset
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : u32Rms-存放24位有效值,注意传入的数据不需要移位
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 根据读取的有效值设置RN8209 IA通道校正寄存器，包括DCIAH和DCL
*=============================================================================*/
uint8 DRV_RN8209_WriteRmsIAOffset(uint32 u32Rms);

/*=============================================================================
* Function      : DRV_RN8209_WriteRmsUOffset
* reentrant     : 不可重入
* syn/asyn      : 同步
* Input         : u32Rms-存放24位有效值,注意传入的数据不需要移位
* Output        : none
* Returns       : DRV_RN8209_StatusType
* Description   : 根据读取的有效值设置RN8209 U通道校正寄存器，包括DCUH和DCL
*=============================================================================*/
uint8 DRV_RN8209_WriteRmsUOffset(uint32 u32Rms);





#ifdef __cplusplus
}
#endif
#endif