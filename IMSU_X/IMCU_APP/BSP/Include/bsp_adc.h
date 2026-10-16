#ifndef __BSP_ADC_H
#define __BSP_ADC_H
#include "stm32f4xx_hal.h"

#define ADCx_DMA_IRQHandler	DMA2_Stream0_IRQHandler
#define  AC_GND      0x28
#define  AC_VREF     0x29
#define  AC_VA1      0x2A 
#define  AC_VB1      0x2B 
#define  AC_VC1      0x2C
#define  AC_IA1      0x2D
#define  AC_IB1      0x2E
#define  AC_IC1      0x2F 

#define  DC_I1_GND   0x40
#define  DC_I1_IREF  0x41
#define  PA3_4051     0x42
#define  DC_I1_2     0x43
#define  DC_I1_3     0x44



#define  LOAD_FUSE1  0x18
#define  LOAD_FUSE2  0x19
#define  LOAD_FUSE3  0x1A  
#define  LOAD_FUSE4  0x1B
#define  LOAD_FUSE5  0x1C
#define  LOAD_FUSE6  0x1D
#define  LOAD_FUSE7  0x1E
#define  BATT_FUSE4  0x1F

#define  BATT_FUSE1  0x20
#define  BATT_FUSE2  0x21
#define  BATT_FUSE3  0x22
#define  BATT_MID1   0x23
#define  BATT_MID2   0x24
#define  BATT_MID3   0x25
#define  BATT_MID4   0x26
#define  BATT_MID5   0x27

#define  BATT_MID6   0x10
#define  BATT_MID7   0x11
#define  BATT_MID8   0x12
#define  BATT_MID9   0x13
#define  TEMP1       0x14
#define  TEMP2       0x15
#define  TEMP3       0x16

#define  GND         0x00
#define  VREF        0x08
#define  VREF_CAL    0x37

#define  PA4_4051		0xC0
#define  PA5_4051		0xC1
#define  PA6_4051		0xC2
#define  PC2_4051		0xC3 
#define  PB0_4051		0xC4   
#define  PC0_4051		0xC5   
#define  PC3_4051		0xC6   
#define  PA0_4051		0xC7  

#define AD_PIN_PA3	PA3_4051			//PA3_4051
#define AD_PIN_PA4	PA4_4051
#define AD_PIN_PA6	PA6_4051
#define AD_PIN_PB0	PB0_4051
#define AD_PIN_PC0	PC0_4051
#define AD_PIN_PC3	PC3_4051
#define AD_PIN_PA0	PA0_4051
#define AD_PIN_PA5  PA5_4051
#define AD_PIN_PC2  PC2_4051			


#define AD_PIN_PF9		0x00

typedef enum 
{
  PA3_ADC_SW_GUN_1_NO_0 = 0,
  PA4_ADC_SW_TEMP_NO_1 = 1,
  PA6_ADC_SW_GUN_2_oldBUS_2_NO_2 = 2,
  PB0_ADC_SW_oldHOLL_TP1000_NO_3  = 3, 
  PC0_ADC_SW_oldCP_CC_OF_AC_NO_4 = 4,
  PC3_ADC_SW_TP1000_NO_5 = 5,
  PA0_ADC_SW_oldGUN_2_None_NO_6 = 6,
  PA5_ADC_SW_oldBUS_1_None_NO_7 = 7,
  PC2_ADC_SW_oldNone_PS_NO_8 = 8,
  MAX_CHANEL_NUM,
  ADC_NO_USE      = 0xff,
}AdcChannel_TypeDef;

int BSP_AdcInit(void);
int BSP_ADC3_Init(void);
int BSP_AdcChannelSelect(ADC_HandleTypeDef *ADC_hande, uint8_t AdcMuxOffset);
int BSP_AdcStart(ADC_HandleTypeDef* HandADC, uint32_t* pDataBuff, uint32_t Length);
int BSP_GetAdcVal(uint16_t* u16AdcVal);
void BSP_vAdcMuxSelect(uint8_t u8ChannelVal);
void BSP_AdcStop(ADC_HandleTypeDef* HandADC);
#endif