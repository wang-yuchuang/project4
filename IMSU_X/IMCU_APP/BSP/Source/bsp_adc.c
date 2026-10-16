/************************************************************************************************************************************************

*******************************************************************************************************************************************************************/
#include "bsp_adc.h"
#include "bsp_fsmc.h"
//#include "interface.h"
/* ADC handler declaration */
ADC_HandleTypeDef    AdcHandle;
ADC_HandleTypeDef    ADC_3_Handle;
extern uint8_t g_u8LvdStatusBuk;

int BSP_AdcInit(void)
{
  AdcHandle.Instance                   = ADC1;
  AdcHandle.Init.ClockPrescaler = ADC_CCR_ADCPRE_1;				// 0x020000; //6分频
  AdcHandle.Init.Resolution            = 0;					//分辨率  12位！
  AdcHandle.Init.ScanConvMode          = 0;					//SCAN	扫描模式， 0 禁止扫描模式
  AdcHandle.Init.ContinuousConvMode = ENABLE;					//CONT	1连续转换
  AdcHandle.Init.DiscontinuousConvMode = 0;					//DISCEN 0 禁止注入通道的不连续采样模式
  AdcHandle.Init.NbrOfDiscConversion   = 0;					//DISCNUM 不连续采样模式通道计数
  AdcHandle.Init.ExternalTrigConvEdge  = 0;					//EXTEN  规则通道的外部触发使能  0禁止触发使能
  AdcHandle.Init.ExternalTrigConv      = 0;					//EXTSEL 为规则组选择外部事件
  AdcHandle.Init.DataAlign             = 0;					//ALIGN 数据对齐， 0：左对齐
  AdcHandle.Init.NbrOfConversion       = 1;					//L 规则通道序列长度		1： 1次转换
  AdcHandle.Init.DMAContinuousRequests = ENABLE;				//DDS  DMA禁止选择（对于单一ADC模式）  0：最后一次传输后不发出新的DMA请求  1：只要发生数据转换且DMA=1，便会发出DAM请求
  AdcHandle.Init.EOCSelection          = 0;					//EOCS 结束转换选择  0：在每个规则转换 序列 结束时将EOC置1.溢出检测仅在DMA=1时使能；	1：在每个转化结束时将EOC置1，使能溢出检测
      
  return (HAL_ADC_Init(&AdcHandle)); 
}  

int BSP_ADC3_Init(void)
{
	extern unsigned char  u8Is1000VHardwareA00(void);
	if(!u8Is1000VHardwareA00())
	{
		return 0;
	}
	ADC_3_Handle.Instance                   = ADC3;
	ADC_3_Handle.Init.ClockPrescaler = ADC_CCR_ADCPRE_1;			// 0x020000; //6分频
	ADC_3_Handle.Init.Resolution            = 0;					//分辨率  12位！
	ADC_3_Handle.Init.ScanConvMode          = 0;					//SCAN	扫描模式， 0 禁止扫描模式
	ADC_3_Handle.Init.ContinuousConvMode = ENABLE;					//CONT	1连续转换
	ADC_3_Handle.Init.DiscontinuousConvMode = 0;					//DISCEN 0 禁止注入通道的不连续采样模式
	ADC_3_Handle.Init.NbrOfDiscConversion   = 0;					//DISCNUM 不连续采样模式通道计数
	ADC_3_Handle.Init.ExternalTrigConvEdge  = 0;					//EXTEN  规则通道的外部触发使能  0禁止触发使能
	ADC_3_Handle.Init.ExternalTrigConv      = 0;					//EXTSEL 为规则组选择外部事件
	ADC_3_Handle.Init.DataAlign             = 0;					//ALIGN 数据对齐， 0：左对齐
	ADC_3_Handle.Init.NbrOfConversion       = 1;					//L 规则通道序列长度		1： 1次转换
	ADC_3_Handle.Init.DMAContinuousRequests = ENABLE;				//DDS  DMA禁止选择（对于单一ADC模式）  0：最后一次传输后不发出新的DMA请求  1：只要发生数据转换且DMA=1，便会发出DAM请求
	ADC_3_Handle.Init.EOCSelection          = 0;					//EOCS 结束转换选择  0：在每个规则转换 序列 结束时将EOC置1.溢出检测仅在DMA=1时使能；	1：在每个转化结束时将EOC置1，使能溢出检测

	return (HAL_ADC_Init(&ADC_3_Handle)); 
}  

int BSP_AdcChannelSelect(ADC_HandleTypeDef *ADC_hande, uint8_t AdcMuxOffset)          
{  
  ADC_ChannelConfTypeDef sConfig;  
  AdcChannel_TypeDef AdcChannel=ADC_NO_USE;
  if (ADC1 == ADC_hande->Instance)
  {
	  if(AdcMuxOffset == PA3_4051)
	  {
		  AdcChannel = PA3_ADC_SW_GUN_1_NO_0;
	  }
	  else if(AdcMuxOffset == PA4_4051)
	  {
		  AdcChannel = PA4_ADC_SW_TEMP_NO_1;  
	  }  
	  else if(AdcMuxOffset == PA6_4051)
	  {
		  AdcChannel = PA6_ADC_SW_GUN_2_oldBUS_2_NO_2;  
	  } 
	  else if(AdcMuxOffset == PB0_4051)
	  {
		  AdcChannel = PB0_ADC_SW_oldHOLL_TP1000_NO_3;  
	  }   
	  else if (AdcMuxOffset == PC0_4051)
	  {
		  AdcChannel = PC0_ADC_SW_oldCP_CC_OF_AC_NO_4;
	  }
	  else if (AdcMuxOffset == PC3_4051)
	  {
		 AdcChannel =  PC3_ADC_SW_TP1000_NO_5;
	  }
	  else if(AD_PIN_PA0 == AdcMuxOffset)
	  {
		   AdcChannel = PA0_ADC_SW_oldGUN_2_None_NO_6;
	  }
	  else if(AD_PIN_PA5 == AdcMuxOffset)
	  {
			AdcChannel = PA5_ADC_SW_oldBUS_1_None_NO_7;
	  }
	  else if(AD_PIN_PC2 == AdcMuxOffset)
	  {
		  AdcChannel = PC2_ADC_SW_oldNone_PS_NO_8;
	  }

	  switch(AdcChannel)
	  {
	  case PA3_ADC_SW_GUN_1_NO_0:
		  sConfig.Channel = ADC_CHANNEL_3;				//PA3  这个需要4051片选  s1 s2 s3
		  break;
	  case PA4_ADC_SW_TEMP_NO_1:
		  sConfig.Channel = ADC_CHANNEL_4;				//PA4
		  break;       
	  case PA6_ADC_SW_GUN_2_oldBUS_2_NO_2:
		  sConfig.Channel = ADC_CHANNEL_6;				
		  break;   
	  case PB0_ADC_SW_oldHOLL_TP1000_NO_3:
		  sConfig.Channel = ADC_CHANNEL_8;				//PB0 
		  break;               
	  case PC0_ADC_SW_oldCP_CC_OF_AC_NO_4:
		  sConfig.Channel = ADC_CHANNEL_10;
		  break;
	  case PC3_ADC_SW_TP1000_NO_5:
		   sConfig.Channel = ADC_CHANNEL_13;
		  break;
	  case PA0_ADC_SW_oldGUN_2_None_NO_6:
			sConfig.Channel = ADC_CHANNEL_0;
		  break;
	  case PA5_ADC_SW_oldBUS_1_None_NO_7:
		  sConfig.Channel = ADC_CHANNEL_5;
		  break;
	  case PC2_ADC_SW_oldNone_PS_NO_8:
		  sConfig.Channel = ADC_CHANNEL_12;
		  break;
	  default:
		  return HAL_ERROR;
		  break;
	  }
	  sConfig.Rank         = 1;
	  sConfig.SamplingTime = 7;
	  sConfig.Offset       = 0;

	 // return (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig));
	  return (HAL_ADC_ConfigChannel(ADC_hande, &sConfig));
  }
  else
  {
	  if(AdcMuxOffset == AD_PIN_PF9)
	  {
		 sConfig.Channel = ADC_CHANNEL_7;		//PF9	
	  }
      sConfig.Rank         = 1;
	  sConfig.SamplingTime = 7;
	  sConfig.Offset       = 0;
	 // return (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig));
	  return (HAL_ADC_ConfigChannel(ADC_hande, &sConfig));
  }
}  

int BSP_AdcStart(ADC_HandleTypeDef* HandADC, uint32_t* pDataBuff, uint32_t Length)
{
  return  (HAL_ADC_Start_DMA(HandADC, pDataBuff, Length));
}
void BSP_AdcStop(ADC_HandleTypeDef* HandADC)
{
	HAL_ADC_Stop_DMA(HandADC);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{

}
int BSP_GetAdcVal( uint16_t* u16AdcVal)
{
  *u16AdcVal=0;
  HAL_ADC_PollForConversion(&AdcHandle, 10);
  
  if(HAL_ADC_GetState(&AdcHandle) == HAL_ADC_STATE_EOC_REG)
  {
    *u16AdcVal = (uint16_t)HAL_ADC_GetValue(&AdcHandle);
    return HAL_OK;
  }
  return HAL_ERROR;
}

void BSP_vAdcMuxSelect(uint8_t u8ChannelVal)
{
	if(u8ChannelVal >= 0xc0)
	{
		return;
	}
	u8ChannelVal &=0x3f;
	u8ChannelVal = u8ChannelVal | g_u8LvdStatusBuk;
	//TXY-AD  通道选了也不起作用电路没接
	CSDO2 = u8ChannelVal;
}
