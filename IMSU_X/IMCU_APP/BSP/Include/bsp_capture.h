#ifndef __BSP_CAPTURE_H
#define __BSP_CAPTURE_H
#include "stm32f4xx_hal.h"
typedef enum 
{
  FAN_ALM1_BY_FREQ = 0,
  AC_PWM_FREQUENCY = 1,
  BUS_VOLT_1_SQC_NO = 2,
  BUS_VOLT_2_SQC_NO = 3,
  CAPTURE_CHANNEL5 = 4,
  CAPTURE_CHANNEL6 = 5,
}CaptureChannel_TypeDef;

void BSP_Base_TIME_Deal(void);
int BSP_CaptureInit(void);
void BSP_TIM_BaseInit(void);
void BSP_TIM_BaseForVFInit(void);


uint32_t GetCaptureChannelCnt(CaptureChannel_TypeDef Channel);
#endif
