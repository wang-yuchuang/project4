/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPISRHook.h
** @Founder      : ZSY342
** @Date         : 2025.8.19
** @Function     : ADC驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPADC_H
#define _BSPADC_H

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

//ADC模块类型定义
typedef enum
{
    eADC_MOD1 = 0,    //ADC模块1

    eADC_MODNUM
}e_ADCModule;

// typedef struct adcSampleValue
// {  
//     u32 Ch8Value;
//     u32 Ch9Value;
//     u32 Ch10Value;
//     u32 Ch11Value;
//     u32 Ch15Value;

// }AdcSampleValue_TypeDef;
// extern AdcSampleValue_TypeDef adc1;

typedef enum {
    Ch8Value = 0,
    Ch9Value,
    Ch10Value,
    Ch11Value,
    Ch15Value,
    buffer_num
}AdcSampleValue_TypeDef;

extern volatile u16 adc_value_buffer[buffer_num];
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPADCInit(e_ADCModule module)
** @Input     : module: ADC模块 
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : ADC驱动模块初始化(只初始化ADC模块，不初始化AD通道)
** @The notes : 用户调用
**===========================================================================================*/
void BSPADCInit(e_ADCModule module);

/*=============================================================================================
** @Name      : u32 BSPADCGetADCState(void)
** @Input     : void
** @Output    : 0:空闲 1:忙
** @Function  : 获取ADC采样通道工作状态
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPADCGetWorkState(void);

/*=============================================================================================
** @Name      : u16 BSPADCGetSampData(void)
** @Input     : void
** @Output    : 从adc结构体中读取
** @Function  : 获取ADC采样AD值
** @The notes : 用户调用，该芯片由于使用了adc1的in15通道，不能使用指定通道读取的方式，只能轮询读取
**===========================================================================================*/
void BSPADCGetSampData(void);

#endif
