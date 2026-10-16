/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPDIDO.c
** @Founder      : WYC834
** @Date         : 2025.8.21
** @Function     : DIDO驱动函数模块
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPIIC.h"
#include "i2c.h"
#include "BSPTypeDef.h"
#include "EEPROM.h"
#include "BSPWDOG.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
** @Input     : pBuffer:缓冲区指针 WriteAddr:写地址 NumByteToWrite:写的字节数
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 将缓冲区中的数据写到I2C EEPROM中
** @The notes :该函数会自动判断是否超页，如超页自动换页
**===========================================================================================*/
u8 I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
  Addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
  NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      if(TRUE != I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle))
      {
        return FALSE;
      }
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        if(TRUE != I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE))
        {
          return FALSE;
        }
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        if(TRUE != I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle))
        {
          return FALSE;
        }
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
        if(TRUE != I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle))
        {
          return FALSE;
        }
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
      NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;	
      
      if(count != 0)
      {  
        if(TRUE != I2C_EE_PageWrite(pBuffer, WriteAddr, count))
        {
          return FALSE;
        }
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        
        if(TRUE != I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE))
        {
          return FALSE;
        }
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        if(TRUE != I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle))
        {
          return FALSE;
        }
      }
    }
  }  
  return TRUE;
}


/*=============================================================================================
** @Name      : u8 I2C_EE_ByteWrite(u8* pBuffer, u16 WriteAddr)
** @Input     : pBuffer:缓冲区指针 WriteAddr:写地址 
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 写一个字节到I2C EEPROM中
** @The notes :
**===========================================================================================*/
u8 I2C_EE_ByteWrite(u8* pBuffer, u16 WriteAddr)
{
	u8 status = HAL_OK;
	u32 timeout_cnt = 0;

	status = BSPIICMemSendMsg(eIIC2, (u16)EEPROM_ADDRESS, (u16)WriteAddr, (u16)I2C_MEMADD_SIZE_8BIT, pBuffer, 0x01); 

	/* Check the communication status */
	if(status != HAL_OK)
	{
	/* Execute user timeout callback */
	}
	timeout_cnt = 0;
	while (BSPGetIICState() != TRUE)
	{
		if (++timeout_cnt > 100000) { return FALSE; }
	}

	/* Check if the EEPROM is ready for a new operation */
	timeout_cnt = 0;
	while (BSPGetIsDeviceReady(EEPROM_ADDRESS) != TRUE)
	{
		if (++timeout_cnt > 1000) { return FALSE; }
	}

	/* Wait for the end of the transfer */
	timeout_cnt = 0;
	while (BSPGetIICState() != TRUE)
	{
		if (++timeout_cnt > 100000) { return FALSE; }
	}
	return status;
}

/*=============================================================================================
** @Name      : u8 I2C_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite)
** @Input     : pBuffer:缓冲区指针 WriteAddr:写地址 NumByteToWrite:写的字节数
** @Output    : TRUE:写成功 FALSE:写失败
** @Function  : 在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数不能超过EEPROM页的大小
** @The notes : 已添加超时退出机制，超时后返回FALSE，避免死循环占用MCU资源
**===========================================================================================*/
u8 I2C_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite)
{
	u8 status = FALSE;
    if(NumByteToWrite > EEPROM_PAGESIZE)
    {
        return FALSE;
    }
	/* Write EEPROM_PAGESIZE */
	status = BSPIICMemSendMsg(eIIC2, EEPROM_ADDRESS, WriteAddr, I2C_MEMADD_SIZE_8BIT, (u8*)(pBuffer),NumByteToWrite);

	u32 timeout_cnt = 0;
	while (BSPGetIICState() != TRUE)
	{
		if (++timeout_cnt > 100000) { return FALSE; }
	}

	/* Check if the EEPROM is ready for a new operation */
	timeout_cnt = 0;
	while (BSPGetIsDeviceReady(EEPROM_ADDRESS) != TRUE)
	{
		if (++timeout_cnt > 1000) { return FALSE; }
	}

	/* Wait for the end of the transfer */
	timeout_cnt = 0;
	while (BSPGetIICState() != TRUE)
	{
		if (++timeout_cnt > 100000) { return FALSE; }
	}

	return status;
}

/*=============================================================================================
** @Name      : u8 I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead)
** @Input     : pBuffer:存放从EEPROM读取的数据的缓冲区指针 WriteAddr:接收数据的EEPROM的地址 NumByteToWrite:要从EEPROM读取的字节数
** @Output    : TRUE:读成功 FALSE:读失败
** @Function  : 从EEPROM里面读取一块数据 
** @The notes :
**===========================================================================================*/
u8 I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead)
{
	u8 status = FALSE;
	
	status = BSPIICMemRcvMsg(eIIC2,EEPROM_ADDRESS,ReadAddr, I2C_MEMADD_SIZE_8BIT, (u8 *)pBuffer, NumByteToRead);

	return status;
}

