/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : EEPROM.h
** @Founder      : WYC834
** @Date         : 2025.09.08
** @Function     : 模拟IIC驱动模块
** @Instructions :
**===========================================================================================*/
#ifndef EEPROM_H_
#define EEPROM_H_

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPGPIO.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

#define EEPROM_READ_ADDRESS        0xA1
#define EEPROM_WRITE_ADDRESS       0xA0
#define EEPROM_ADDRESS       0xA0
#define EEPROM_PAGESIZE 16


//地址分配
#define Eeprom_TestAddress_Start             (0x7F0)  /*0X7F0~0X7FF*/
#define Eeprom_TestAddress_End               (0x7F0)  
#define Eeprom_ParaAddress_Start             0x00   /*暂定0X00~0X7EF*/
#define Eeprom_SerialNum_Add                 0x01
#define Eeprom_Parameter_Add                 0x20   /*暂定0X20~0X3f*/
#define Eeprom_Calibration_Add               0x40   /*暂定0X40~0XA7*/
#define Eeprom_ParaAddress_End               (0X7EF) 
/*=============================================================================================
** @Data type definition
**===========================================================================================*/


/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 I2C_EE_ByteWrite(u8* pBuffer, u16 WriteAddr)
** @Input     : pBuffer:缓冲区指针 WriteAddr:写地址 
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 写一个字节到I2C EEPROM中
** @The notes :
**===========================================================================================*/
u8 I2C_EE_ByteWrite(u8* pBuffer, u16 WriteAddr);

/*=============================================================================================
** @Name      : u8 I2C_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite)
** @Input     : pBuffer:缓冲区指针 WriteAddr:写地址 NumByteToWrite:写的字节数
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数不能超过EEPROM页的大小
** @The notes :
**===========================================================================================*/
u8 I2C_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite);

/*=============================================================================================
** @Name      : u8 I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead)
** @Input     : pBuffer:存放从EEPROM读取的数据的缓冲区指针 WriteAddr:接收数据的EEPROM的地址 NumByteToWrite:要从EEPROM读取的字节数
** @Output    : TRUE:读成功 FALSE:读失败
** @Function  : 从EEPROM里面读取一块数据 
** @The notes :
**===========================================================================================*/
u8 I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead);

/*=============================================================================================
** @Name      : u8 I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
** @Input     : pBuffer:缓冲区指针 WriteAddr:写地址 NumByteToWrite:写的字节数
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 将缓冲区中的数据写到I2C EEPROM中
** @The notes :该函数会自动判断是否超页，如超页自动换页
**===========================================================================================*/
u8 I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite);

#endif
