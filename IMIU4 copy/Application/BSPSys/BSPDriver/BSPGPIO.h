/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPGPIO.h
** @Founder      : WYC834
** @Date         : 2025.08.21
** @Function     : 输入输出IO驱动函数模块
** @Instructions : 用户完善
**===========================================================================================*/
#ifndef _BSPGPIO_H
#define _BSPGPIO_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
//I/O端口类型名称
typedef enum
{
    ePOR_A = 0,
    ePOR_B,
    ePOR_C,
    ePOR_D,
    ePOR_E,
    ePOR_F,
    ePOR_G,
    ePOR_H,
    ePOR_I,
    ePOR_NUM
}eGPIOPORT;

//I/O端口管脚序号(某一类型下)
typedef enum
{
    ePIN_0 = 0,
    ePIN_1,
    ePIN_2,
    ePIN_3,
    ePIN_4,
    ePIN_5,
    ePIN_6,
    ePIN_7,
    ePIN_8,
    ePIN_9,
    ePIN_10,
    ePIN_11,
    ePIN_12,
    ePIN_13,
    ePIN_14,
    ePIN_15,
    ePIN_NUM,
    ePIN_ALL = 0xff
}eGPIOPIN;
//I/O状态
typedef enum
{
    eIOLOW = 0,             //低电平
    eIOHIG = 1,             //高电平
    
    eIOSTATE                //IO状态数目
}e_IOState;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPALLGPIOInit(void)
** @Input     : 
** @Output    : TRUE:初始化成功 FALSE:初始化失败
** @Function  : 整体GPIO初始化
** @The notes : 用户调用
**===========================================================================================*/
void BSPALLGPIOInit(void);

/*=============================================================================================
** @Name      : u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data)
** @Input     : port:端口 pin:第几个引脚 data:端口值(0低电平 1高电平)
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : 设置GPIO端口输出电平值
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data);

/*=============================================================================================
** @Name      : u8 BSPGPIOGetInPortState(eGPIOPORT port, eGPIOPIN pin)
** @Input     : port:端口 pin:第几个引脚
** @Output    : GPIO端口当前电平值[0低电平,1高电平] 0xff:参数输入错误
** @Function  : 查询GPIO端口当前电平值
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOGetPortState(eGPIOPORT port, eGPIOPIN pin);

/*=============================================================================================
** @Name      : u8 BSPGPIOTogglePortData(eGPIOPORT port, eGPIOPIN pin)
** @Input     : pinNum: GPIO索引
** @Output    : void
** @Function  : 翻转指定IO口的电平
** @The notes :
**===========================================================================================*/
u8 BSPGPIOTogglePortData(eGPIOPORT port, eGPIOPIN pin);

#endif

