/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPI2c.h
** @Founder      : WYC834
** @Date         : 2025.09.08
** @Function     : 模拟IIC驱动模块
** @Instructions :
**===========================================================================================*/
#ifndef BSPSYS_DRIVER_BSPSIMI2C_H_
#define BSPSYS_DRIVER_BSPSIMI2C_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPGPIO.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

#define ACK        0
#define NO_ACK     1

#define IIC_DATA_LEN    20
#define EEPROM_MAX_TRIALS 1
/*=============================================================================================
** @Data type definition
**===========================================================================================*/
/*IIC模拟通道*/
typedef enum
{
	eIIC2 = 1,
	eIIC_NUM
}e_I2cChann;

/*模拟IIC方向定义*/
typedef enum
{
	INPUT = 0,		/*初始化IO口为输入*/
	OUTPUT =1		/*初始化IO口为输出*/

}e_SimI2cDirt;


typedef struct
{
    u8  devID;
    u8  regLen;
    u16 regAddr;
    u8  dataLen;
    u8  dataBuf[IIC_DATA_LEN];
}STR_IICDATA;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPIICInit(e_I2cChann Channel)
** @Input     : Chan:选择IIC通道
** @Output    : void
** @Function  : IIC通信初始化
** @The notes :
**===========================================================================================*/
void BSPIICInit(e_I2cChann Channel);

/*=============================================================================================
** @Name      : u8 BSPIICMasterSendMsg(e_I2cChann Channel, u16 device,u8 *data, u16 size)
** @Input     : Channel:选择IIC通道 device:设备地址 data:数据指针 size:数据大小
** @Output    : 
** @Function  : IIC通信从机寻址方式发送数据
** @The notes :
**===========================================================================================*/
u8 BSPIICMasterSendMsg(e_I2cChann Channel, u16 device,u8 *data, u16 size);

/*=============================================================================================
** @Name      : u8 BSPIICMasterRcvMsg(e_I2cChann Channel, u16 device,u8 *data, u16 size)
** @Input     : Channel:选择IIC通道 device:设备地址 data:数据指针 size:数据大小
** @Output    : 
** @Function  : IIC通信从机寻址方式接收数据
** @The notes :
**===========================================================================================*/
u8 BSPIICMasterRcvMsg(e_I2cChann Channel, u16 device,u8 *data, u16 size);

/*=============================================================================================
** @Name      : u8 BSPIICMasterSendMsg(e_I2cChann Channel, u16 device, u16 add, u16 addsize, u8 *data, u16 size)
** @Input     : Channel:选择IIC通道 device:设备地址 add:从机寄存器地址 addsize:从机寄存器地址长度 data:数据指针 size:数据大小
** @Output    : 
** @Function  : IIC通信从机寄存器寻址方式发送数据
** @The notes :
**===========================================================================================*/
u8 BSPIICMemSendMsg(e_I2cChann Channel, u16 device, u16 add, u16 addsize, u8 *data, u16 datasize);

/*=============================================================================================
** @Name      : u8 BSPIICMasterRcvMsg(e_I2cChann Channel, u16 device, u16 add, u16 addsize, u8 *data, u16 size)
** @Input     : Channel:选择IIC通道 device:设备地址 add:从机寄存器地址 addsize:从机寄存器地址长度 data:接收数据指针 size:数据大小
** @Output    : 
** @Function  : IIC通信从机寄存器寻址方式接收数据
** @The notes :
**===========================================================================================*/
u8 BSPIICMemRcvMsg(e_I2cChann Channel, u16 device, u16 add, u16 addsize, u8 *data, u16 datasize);

/*=============================================================================================
** @Name      : u8 BSPGetIICState(void)
** @Input     : void
** @Output    : TURE OR FALSE
** @Function  : 
** @The notes :
**===========================================================================================*/
u8 BSPGetIICState(void);

/*=============================================================================================
** @Name      : u8 BSPGetIsDeviceReady(u16 DevAddress)
** @Input     : DevAddress:设备地址
** @Output    : TURE OR FALSE
** @Function  : 
** @The notes :
**===========================================================================================*/
u8 BSPGetIsDeviceReady(u16 DevAddress);
#endif
