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

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPIRQ.h"
#include "BSPGPIO.h"
#include "BSPIIC.h"
#include "i2c.h"
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
** @Name      : void BSPIICInit(e_SimI2cChann Channel)
** @Input     : Chan:选择IIC通道
** @Output    : void
** @Function  : IIC通信初始化
** @The notes :
**===========================================================================================*/
void BSPIICInit(e_I2cChann Channel)
{
    if(1 != gBSPFunCfg.Bits.IIC2En)
    {
        return;
    }
    if(eIIC2 == Channel)
    {
        MX_I2C2_Init();
    }
}

/*=============================================================================================
** @Name      : u8 BSPIICMasterSendMsg(e_I2cChann Channel, u16 device,u8 *data, u16 size)
** @Input     : Channel:选择IIC通道 device:设备地址 data:数据指针 size:数据大小
** @Output    : 
** @Function  : IIC通信从机寻址方式发送数据
** @The notes :
**===========================================================================================*/
u8 BSPIICMasterSendMsg(e_I2cChann Channel, u16 device,u8 *data, u16 size)
{
    if(1 != gBSPFunCfg.Bits.IIC2En)
    {
        return(FALSE);
    }

	if(Channel == eIIC2)
    {	
		if(HAL_OK != HAL_I2C_Master_Transmit(&hi2c2,device,data,size,1000))
        {
            return(FALSE);
        }
			
    }
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPIICMasterRcvMsg(e_I2cChann Channel, u16 device,u8 *data, u16 size)
** @Input     : Channel:选择IIC通道 device:设备地址 data:数据指针 size:数据大小
** @Output    : 
** @Function  : IIC通信从机寻址方式接收数据
** @The notes :
**===========================================================================================*/
u8 BSPIICMasterRcvMsg(e_I2cChann Channel, u16 device,u8 *data, u16 size)
{
    if(1 != gBSPFunCfg.Bits.IIC2En)
    {
        return(FALSE);
    }
    if(Channel == eIIC2)
    {	
		if(HAL_OK != HAL_I2C_Master_Receive(&hi2c2,device,data,size,1000))
        {
            return(FALSE);
        }
			
    }
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPIICMasterSendMsg(e_I2cChann Channel, u16 device, u16 add, u16 addsize, u8 *data, u16 size)
** @Input     : Channel:选择IIC通道 device:设备地址 add:从机寄存器地址 addsize:从机寄存器地址长度 data:数据指针 size:数据大小
** @Output    : 
** @Function  : IIC通信从机寄存器寻址方式发送数据
** @The notes :
**===========================================================================================*/
u8 BSPIICMemSendMsg(e_I2cChann Channel, u16 device, u16 add, u16 addsize, u8 *data, u16 datasize)
{
    if(1 != gBSPFunCfg.Bits.IIC2En)
    {
        return(FALSE);
    }
    if(Channel == eIIC2)
    {	
		if(HAL_OK != HAL_I2C_Mem_Write(&hi2c2,device,add,addsize,data,datasize,1000))
        {
            return(FALSE);
        }
			
    }
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPIICMasterRcvMsg(e_I2cChann Channel, u16 device, u16 add, u16 addsize, u8 *data, u16 size)
** @Input     : Channel:选择IIC通道 device:设备地址 add:从机寄存器地址 addsize:从机寄存器地址长度 data:接收数据指针 size:数据大小
** @Output    : 
** @Function  : IIC通信从机寄存器寻址方式接收数据
** @The notes :
**===========================================================================================*/
u8 BSPIICMemRcvMsg(e_I2cChann Channel, u16 device, u16 add, u16 addsize, u8 *data, u16 datasize)
{
    if(1 != gBSPFunCfg.Bits.IIC2En)
    {
        return(FALSE);
    }

	if(Channel == eIIC2)
    {	
		if(HAL_OK != HAL_I2C_Mem_Read(&hi2c2,device,add,addsize,data,datasize,1000))
        {
            return(FALSE);
        }
			
    }
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPGetIICState(void)
** @Input     : void
** @Output    : TURE OR FALSE
** @Function  : 
** @The notes :
**===========================================================================================*/
u8 BSPGetIICState(void)
{
    if(1 != gBSPFunCfg.Bits.IIC2En)
    {
        return(FALSE);
    }
    if(HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
    {
        return(FALSE);
    }
			
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPGetIsDeviceReady(u16 DevAddress)
** @Input     : DevAddress:设备地址
** @Output    : TURE OR FALSE
** @Function  : 
** @The notes :
**===========================================================================================*/
u8 BSPGetIsDeviceReady(u16 DevAddress)
{
    if(1 != gBSPFunCfg.Bits.IIC2En)
    {
        return(FALSE);
    }
    if(HAL_I2C_IsDeviceReady(&hi2c2, DevAddress, EEPROM_MAX_TRIALS, 1000) != HAL_OK)
    {
        return(FALSE);
    }
			
    return(TRUE);
}