/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPADC.c
** @Founder      : WYC834
** @Date         : 2025.8.19
** @Function     : ADC驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "main.h"
#include "BSPConfig.h"
#include "BSPADC.h"
#include "adc.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
// AdcSampleValue_TypeDef adc1 = {
//     .Ch8Value = 0,
//     .Ch9Value = 0,
//     .Ch10Value = 0,
//     .Ch11Value = 0,
//     .Ch15Value = 0
// };
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPADCInit(e_ADCModule module)
** @Input     : module: ADC模块 
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : ADC驱动模块初始化(只初始化ADC模块，不初始化AD通道)
** @The notes : 用户调用
**===========================================================================================*/
void BSPADCInit(e_ADCModule module)
{
    if(1 != gBSPFunCfg.Bits.ADCEn)
    {
        return;
    }
    if(eADC_MOD1 == module)
    {
        MX_ADC1_Init();
    }
}

/*=============================================================================================
** @Name      : u32 BSPADCGetADCState(void)
** @Input     : void
** @Output    : 0:空闲 1:忙
** @Function  : 获取ADC采样通道工作状态
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPADCGetWorkState(void)
{   
    if(1 != gBSPFunCfg.Bits.ADCEn)
    {
        return false;
    }
    u32 ret = 0;
    if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_REG_BUSY) != 0UL)
    {
        ret = 1;
    }
    return ret;
}

/*=============================================================================================
** @Name      : u16 BSPADCGetSampData(void)
** @Input     : void
** @Output    : 从adc结构体中读取
** @Function  : 获取ADC采样AD值
** @The notes : 用户循环调用，该芯片由于使用了adc1的in15通道，不能使用指定通道读取的方式，只能轮询读取
**===========================================================================================*/
#if 0 
void BSPADCGetSampData(void)
{
    if(1 != gBSPFunCfg.Bits.ADCEn)
    {
        return;
    }
    HAL_ADCEx_Calibration_Start(&hadc1);//adc校准
    HAL_ADC_Start(&hadc1);//开始adc检测
    if(HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch8Value = HAL_ADC_GetValue(&hadc1);
    }   

    if(HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch9Value = HAL_ADC_GetValue(&hadc1);
    }
    if(HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch10Value = HAL_ADC_GetValue(&hadc1);
    }
    if(HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch11Value = HAL_ADC_GetValue(&hadc1);
    }
    if(HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch15Value = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);//停止adc检测
}



void BSPADCGetSampData(void)
{

    if (1 != gBSPFunCfg.Bits.ADCEn)
    {
       return;
    }

    
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch8Value = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);

    
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch9Value = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);

    
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch10Value = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);


    
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch11Value = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);


    
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY) == HAL_OK)
    {
        adc1.Ch15Value = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);

}
#endif


volatile u16 adc_value_buffer[buffer_num];

void BSPADCGetSampData(void)
{
    if (1 != gBSPFunCfg.Bits.ADCEn)
        return;
    
    
    // 启动ADC DMA转换
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_value_buffer, buffer_num);
    
    
}