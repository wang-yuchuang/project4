/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : UsartHandle.c
** @Founder      : WYP40505
** @Date         : 20260228
** @Function     : usart信息接收模块
** @Instructions : 
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "UsartHandle.h"
#include "Modbus.h"
#include "SystemInfo.h"


/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=================================================================================================
** @Name      : void Usart1RcvMsgHandle_API(t_USARTMSG *msg)
** @Input     : msg:usart报文信息
** @Output    : void
** @Function  : usart1通讯信息接收处理函数
** @The notes : 放在usart接收处理hook中
**===============================================================================================*/
void Usart1RcvMsgHandle_API(t_USARTMSG *msg)
{

}

/*=================================================================================================
** @Name      : void Usart2RcvMsgHandle_API(t_USARTMSG *msg)
** @Input     : msg:usart报文信息
** @Output    : void
** @Function  : usart2通讯信息接收处理函数
** @The notes : 放在usart接收处理hook中
**===============================================================================================*/
void Usart2RcvMsgHandle_API(t_USARTMSG *msg)
{



}

/*=================================================================================================
** @Name      : void Usart3RcvMsgHandle_API(t_USARTMSG *msg)
** @Input     : msg:usart报文信息
** @Output    : void
** @Function  : usart3通讯信息接收处理函数
** @The notes : 放在usart接收处理hook中
**===============================================================================================*/
void Usart3RcvMsgHandle_API(t_USARTMSG *msg)
{


}

/*=================================================================================================
** @Name      : void Usart4RcvMsgHandle_API(t_USARTMSG *msg)
** @Input     : msg:usart报文信息
** @Output    : void
** @Function  : usart4通讯信息接收处理函数
** @The notes : 放在usart接收处理hook中
**===============================================================================================*/
void Usart4RcvMsgHandle_API(t_USARTMSG *msg)
{

}

/*=================================================================================================
** @Name      : void Usart6RcvMsgHandle_API(t_USARTMSG *msg)
** @Input     : msg:usart报文信息
** @Output    : void
** @Function  : usart6通讯信息接收处理函数
** @The notes : 放在usart接收处理hook中
**===============================================================================================*/
void Usart6RcvMsgHandle_API(t_USARTMSG *msg)
{
	Modbus_ToolingInterface(msg);
}


