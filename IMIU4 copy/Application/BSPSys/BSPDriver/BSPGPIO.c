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

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "main.h"
#include "BSPConfig.h"
#include "BSPGPIO.h"
#include "gpio.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern volatile u_BSPFunCfg gBSPFunCfg;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static GPIO_T* GPIOChangeToGPIOType(eGPIOPORT port)
** @Input     : port:端口
** @Output    : 端口类型
** @Function  : 转换端口对应的类型
** @The notes :
**===========================================================================================*/
static GPIO_TypeDef* GPIOChangeToGPIOType(eGPIOPORT port);

/*=============================================================================================
** @Name      : static u32 GPIOPINChangeToPinType(eGPIOPIN pin)
** @Input     : pin:引脚号
** @Output    : 端口类型
** @Function  : 转换端口对应的引脚类型
** @The notes :
**===========================================================================================*/
static u32 GPIOPINChangeToPinType(eGPIOPIN pin);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPALLGPIOInit(void)
** @Input     : 
** @Output    : TRUE:初始化成功 FALSE:初始化失败
** @Function  : 整体GPIO初始化
** @The notes : 用户调用
**===========================================================================================*/
void BSPALLGPIOInit(void)
{
    MX_GPIO_Init();//GPIO口初始化
}

/*=============================================================================================
** @Name      : u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data)
** @Input     : port:端口 pin:第几个引脚 data:端口值(0低电平 1高电平)
** @Output    : TRUE:设置成功 FALSE:设置失败
** @Function  : 设置GPIO端口输出电平值
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOSetPortState(eGPIOPORT port, eGPIOPIN pin, e_IOState data)
{
    GPIO_TypeDef* GPIOx;
    u32 PINx = 0;
    
    if(1 != gBSPFunCfg.Bits.GPIOEn)
    {
        return(FALSE);
    }
    
   if((port >= ePOR_NUM) || ((pin >= ePIN_NUM) && (pin != ePIN_ALL)) || (data >= eIOSTATE))
   {
       return(FALSE);
   }


    /*转换为对应的PORT类型*/
    GPIOx = GPIOChangeToGPIOType(port);
    /*转换对应引脚号*/
    PINx = GPIOPINChangeToPinType(pin);
    
    /*设置高or低电平*/
        /*设置高电平*/
    if(eIOHIG == data)
    {
    	HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_SET);
    }
    else
    {
    	HAL_GPIO_WritePin(GPIOx, PINx, GPIO_PIN_RESET);
    }

    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPGPIOGetOutPortState(eGPIOPORT port, eGPIOPIN pin)
** @Input     : port:端口 pin:第几个引脚
** @Output    : 0：低电平 1：高电平
** @Function  : 获取GPIO端口输出电平值
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPGPIOGetPortState(eGPIOPORT port, eGPIOPIN pin)
{
    GPIO_TypeDef* GPIOx;
    u32 PINx = 0;
    u8 state = 0;
    if(1 != gBSPFunCfg.Bits.GPIOEn)
    {
        return(FALSE);
    }
    
    if((port >= ePOR_NUM) || ((pin >= ePIN_NUM) && (pin != ePIN_ALL)))
    {
        return(FALSE);
    }

    /*转换为对应的PORT类型*/
    GPIOx = GPIOChangeToGPIOType(port);
    
    /*转换对应引脚号*/
    PINx = GPIOPINChangeToPinType(pin);
    
	state = HAL_GPIO_ReadPin(GPIOx,PINx);
 
    return(state);
}

/*=============================================================================================
** @Name      : u8 BSPGPIOTogglePortData(eGPIOPORT port, eGPIOPIN pin)
** @Input     : pinNum: GPIO索引
** @Output    : void
** @Function  : 翻转指定IO口的电平
** @The notes :
**===========================================================================================*/
u8 BSPGPIOTogglePortData(eGPIOPORT port, eGPIOPIN pin)
{
    GPIO_TypeDef* GPIOx;
    u32 PINx = 0;
    /*转换为对应的PORT类型*/
    GPIOx = GPIOChangeToGPIOType(port);
    /*转换对应引脚号*/
    PINx = GPIOPINChangeToPinType(pin);
    HAL_GPIO_TogglePin(GPIOx,PINx);
    
    return(TRUE);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static GPIO_T* GPIOChangeToGPIOType(eGPIOPORT port)
** @Input     : port:端口
** @Output    : 端口类型
** @Function  : 转换端口对应的类型
** @The notes :
**===========================================================================================*/
static GPIO_TypeDef* GPIOChangeToGPIOType(eGPIOPORT port)
{
	GPIO_TypeDef* data = (void *)0;

    switch(port)
    {
        case ePOR_A:
            data = GPIOA;
            break;

        case ePOR_B:
            data = GPIOB;
            break;

        case ePOR_C:
            data = GPIOC;
            break;

        case ePOR_D:
            data = GPIOD;
            break;

        case ePOR_E:
            data = GPIOE;
            break;

        case ePOR_F:
            data = GPIOF;
            break;
            
        default:
            data = (void *)0;
            break;
    }

    return(data);
}

/*=============================================================================================
** @Name      : static u32 GPIOPINChangeToPinType(eGPIOPIN pin)
** @Input     : pin:引脚号
** @Output    : 端口类型
** @Function  : 转换端口对应的引脚类型
** @The notes :
**===========================================================================================*/
static u32 GPIOPINChangeToPinType(eGPIOPIN pin)
{
	u32 data = 0;

    switch(pin)
    {
        case ePIN_0:
            data  = GPIO_PIN_0;
            break;
            
        case ePIN_1:
            data  = GPIO_PIN_1;
            break;

        case ePIN_2:
            data  = GPIO_PIN_2;
            break;
            
        case ePIN_3:
            data  = GPIO_PIN_3;
            break;

        case ePIN_4:
            data  = GPIO_PIN_4;
            break;
            
        case ePIN_5:
            data  = GPIO_PIN_5;
            break;

        case ePIN_6:
            data  = GPIO_PIN_6;
            break;
            
        case ePIN_7:
            data  = GPIO_PIN_7;
            break;

        case ePIN_8:
            data  = GPIO_PIN_8;
            break;
            
        case ePIN_9:
            data  = GPIO_PIN_9;
            break;
            
        case ePIN_10:
            data  = GPIO_PIN_10;
            break;
            
        case ePIN_11:
            data  = GPIO_PIN_11;
            break;
            
        case ePIN_12:
            data  = GPIO_PIN_12;
            break;
            
        case ePIN_13:
            data  = GPIO_PIN_13;
            break;
            
        case ePIN_14:
            data  = GPIO_PIN_14;
            break;
            
        case ePIN_15:
            data  = GPIO_PIN_15;
            break;

        default:
            data = GPIO_PIN_0;
            break;
    }

    return(data);
}