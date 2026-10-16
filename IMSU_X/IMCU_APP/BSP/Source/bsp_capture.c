
#include "bsp_capture.h"
#include "main.h"

/* Timer handler declaration */
TIM_HandleTypeDef      Tim2Handle;
TIM_HandleTypeDef      Tim8Handle;
TIM_HandleTypeDef      Tim3CaptureHandle;
TIM_HandleTypeDef      Tim9Handle;
//TIM_HandleTypeDef      Tim10Handle;
TIM_HandleTypeDef      Tim11Handle;
TIM_HandleTypeDef      Tim14Handle;
TIM_HandleTypeDef      Tim5Handle;
TIM_HandleTypeDef      Tim7Handle;

/* Timer Input Capture Configuration Structure declaration */
TIM_IC_InitTypeDef     sICConfig;
TIM_ClockConfigTypeDef	sTIMClockConfig;

uint32_t u32PulseCnt[CAPTURE_CHANNEL6 +1];

/* Private function prototypes -----------------------------------------------*/
int bsp_VF_Init(void);
int bsp_CaptureByTIM9_CH1_PE5(void);

/* Private functions ---------------------------------------------------------*/

/*bsp_CaptureInit*/
/*************************************************************************************************************************
 * @brief  初始化捕获模块
 * @note   该函数用于初始化所有捕获通道计数器，并配置VF模块和TIM9_CH1_PE5捕获功能
 * @retval HAL_OK    初始化成功
 * @retval HAL_ERROR 初始化失败
 ************************************************************************************************************************/
int BSP_CaptureInit(void)
{
  uint8_t i=0;
  
  for(i=0; i<(CAPTURE_CHANNEL6 +1);i++)
  {
		u32PulseCnt[i] =0;
  }
  
  if(bsp_VF_Init() !=  HAL_OK)
  {
    return HAL_ERROR;
  }
  
  if(bsp_CaptureByTIM9_CH1_PE5() !=  HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/*************************************************************************************************************************
 * @brief  初始化VF模块的硬件定时器
 * @note   该函数初始化TIM2和TIM8定时器，配置为外部时钟模式2(ETRMODE2)，
 *         上升沿触发，不分频，并启动定时器。
 * @return HAL_OK 初始化成功
 ************************************************************************************************************************/
int bsp_VF_Init(void)
{

	Tim2Handle.Instance = TIM2;

	Tim2Handle.Init.Period        = 0xffff;
	Tim2Handle.Init.Prescaler     = 0;
	Tim2Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	Tim2Handle.Init.CounterMode   = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&Tim2Handle);
   
	sTIMClockConfig.ClockFilter = 0;
	sTIMClockConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
	sTIMClockConfig.ClockPolarity = TIM_CLOCKPOLARITY_RISING;
	sTIMClockConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
	HAL_TIM_ConfigClockSource(&Tim2Handle,&sTIMClockConfig);
	HAL_TIM_Base_Start(&Tim2Handle);


	Tim8Handle.Instance = TIM8;

	Tim8Handle.Init.Period        = 0xffff;
	Tim8Handle.Init.Prescaler     = 0;
	Tim8Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	Tim8Handle.Init.CounterMode   = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&Tim8Handle);
	sTIMClockConfig.ClockFilter = 0;
	sTIMClockConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
	sTIMClockConfig.ClockPolarity = TIM_CLOCKPOLARITY_RISING;
	sTIMClockConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
	HAL_TIM_ConfigClockSource(&Tim8Handle,&sTIMClockConfig);
	HAL_TIM_Base_Start(&Tim8Handle);
	return HAL_OK;
}

/*bsp_CaptureByTIM9_CH1_PE5*/
/********************************************************************************************************************************
 * @brief  配置TIM9通道1(PE5引脚)作为输入捕获模式
 * @note   用于AC交流PWM波形频率回采
 * @param  无
 * @retval HAL状态: HAL_OK成功, HAL_ERROR失败
 ********************************************************************************************************************************/
int bsp_CaptureByTIM9_CH1_PE5(void)
{
	// PE5    	 （输入捕捉）	 TIME9_CH1			AC交流		PWM		波形频率回采
	Tim9Handle.Instance = TIM9;
	Tim9Handle.Init.Period        = 0xFFFF;
	Tim9Handle.Init.Prescaler     = 0;				// Specifies the prescaler value used to divide the TIM clock, This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF
	Tim9Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	Tim9Handle.Init.CounterMode   = TIM_COUNTERMODE_UP;  
	if(HAL_TIM_IC_Init(&Tim9Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	sICConfig.ICPolarity  = TIM_ICPOLARITY_RISING;
	sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sICConfig.ICPrescaler = TIM_ICPSC_DIV4;
	sICConfig.ICFilter    = 0;														//This parameter can be a number between Min_Data = 0x0 and Max_Data = 0xF
	if(HAL_TIM_IC_ConfigChannel(&Tim9Handle, &sICConfig, TIM_CHANNEL_1) != HAL_OK)
	{
		return HAL_ERROR;
	}


	
	if(HAL_TIM_IC_Start_IT(&Tim9Handle, TIM_CHANNEL_1) != HAL_OK)
	{
		return HAL_ERROR;
	}



	return HAL_OK;
}

/********************************************************************************************************************************
 * @brief  初始化TIM5定时器基础配置
 * @note   配置TIM5的周期、预分频器、时钟分频和计数模式，并启动定时器中断
 * @param  无
 * @retval 无
 ********************************************************************************************************************************/
void BSP_TIM_BaseInit(void)
{
	Tim5Handle.Instance = TIM5;
	Tim5Handle.Init.Period        = (PERIOD_TIME_5 - 1);
	Tim5Handle.Init.Prescaler     = (PRESCALER_TIME_5 - 1);
	Tim5Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	Tim5Handle.Init.CounterMode   = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&Tim5Handle);
	HAL_TIM_Base_Start_IT(&Tim5Handle);
}

/********************************************************************************************************************************
 * @brief  初始化TIM7作为VF(电压频率)测量的基础定时器
 * @note   配置TIM7的周期、预分频器等参数，并启动定时器中断
 *         当前版本注释掉了硬件版本检查逻辑
 ********************************************************************************************************************************/
void BSP_TIM_BaseForVFInit(void)
{
	Tim7Handle.Instance = TIM7;
	Tim7Handle.Init.Period        = (PERIOD_TIME_7 - 1);
	Tim7Handle.Init.Prescaler     = (PRESCALER_TIME_7 - 1);
	Tim7Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	Tim7Handle.Init.CounterMode   = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&Tim7Handle);
	HAL_TIM_Base_Start_IT(&Tim7Handle);
}

/********************************************************************************************************************************
 * @brief 获取指定捕获通道的脉冲计数
 * @param Channel 捕获通道号，范围 CAPTURE_CHANNEL0~CAPTURE_CHANNEL6
 * @return 返回指定通道的脉冲计数值，若通道号无效则返回0
 ********************************************************************************************************************************/
uint32_t GetCaptureChannelCnt(CaptureChannel_TypeDef Channel)
{
	if (Channel > CAPTURE_CHANNEL6)
	{
		return 0;
	}
	return u32PulseCnt[Channel];
}

UINT8  u8gTestIdx = 0;
UINT32 capture_time3_ch1_value_1 = 0;
UINT32 capture_time3_ch1_value_2 = 0;
UINT32 CaptureValue;
volatile UINT8 u8Capture_time3_Flag = 0;
UINT8 u8gAC_PWM_PulseTimes = 0;
/********************************************************************************************************************************
 * @brief  TIM输入捕获中断回调函数
 * @param  htim TIM句柄指针
 * @note   当TIM9通道1捕获到信号时，增加PWM脉冲计数和频率计数
 *         仅当风扇脉冲输入达到输出阈值时执行特定操作
 ********************************************************************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if ((htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) && (htim->Instance == TIM9))
	{
		u8gAC_PWM_PulseTimes++;
		++u32PulseCnt[AC_PWM_FREQUENCY];
		if(gFANMngInfo.u32FanPulseInput[AC_PWM_FREQUENCY][0] >= (AC_PWM_IS_OUTPUT))
		{
			
		}
	}
}

/********************************************************************************************************************************
 * @brief  TIM周期中断回调函数
 * @param  htim TIM句柄指针
 * @note   当TIM计数器溢出时自动调用，执行BSP基础时间处理
 ********************************************************************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	BSP_Base_TIME_Deal();
}