#include "bsp_pwm.h"
#include  "main.h"
TIM_HandleTypeDef    Tim3Handle;
TIM_HandleTypeDef    Tim13Handle;
TIM_HandleTypeDef    Tim10Handle;
//TIM_OC_InitTypeDef sConfig;
int BSP_Pwm1Start(void);
int BSP_Pwm2Start(void);
int BSP_Pwm3Start(void);
/*PWM1*/
/********************************************************************************************************
 * @brief  初始化PWM1定时器(TIM3)
 * @note   配置TIM3的预分频器、周期、时钟分频和计数模式，并初始化PWM
 * @retval HAL_OK    初始化成功
 * @retval HAL_ERROR 初始化失败
 *******************************************************************************************************/
int BSP_Pwm1Init(void)
{
	Tim3Handle.Instance = TIM3;
	Tim3Handle.Init.Prescaler = 11;
	Tim3Handle.Init.Period = 1399;
	//tim2_period =TIM2_COUNTER_CLOCK/Freq - 1;
	//tim2_pulse  = (tim2_period + 1)*Dutycycle / 100;
	Tim3Handle.Init.ClockDivision = 0;
	Tim3Handle.Init.CounterMode = 0x00;
	if(HAL_TIM_PWM_Init(&Tim3Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

/********************************************************************************************************
 * @brief 设置PWM1通道的占空比
 * @param DutyCycles 占空比数值 (0-100)
 * @retval HAL_OK 设置成功
 * @retval HAL_ERROR 参数错误或配置失败
//  *******************************************************************************************************/
int BSP_SetPwm1Duty(uint8_t DutyCycles)
{
	TIM_OC_InitTypeDef sConfig;

	memset(&sConfig, 0, sizeof(sConfig));
	if(DutyCycles>100)
	{
	    return HAL_ERROR;
	}
	sConfig.OCMode = 0x60;
	sConfig.OCFastMode = 0x00;	


	if (gEvChargeInfo.u8PWMPolarityCover)        //如果PWM极性反转   则低电平有效
	{
		sConfig.OCPolarity = 0x02;           //高电平有效

	}
	else
	{
		sConfig.OCPolarity = 0x00;         //低电平有效
	}

//	sConfig.Pulse = ((uint32_t)DutyCycles*14);
	
	if(DutyCycles == 0) 
		{
    		sConfig.Pulse = 0;
    		HAL_TIM_PWM_Stop(&Tim3Handle, TIM_CHANNEL_3); // 明确停止PWM
		} 
		else 
		{
   			 sConfig.Pulse = ((uint32_t)DutyCycles*14);
   			 HAL_TIM_PWM_Start(&Tim3Handle, TIM_CHANNEL_3);
		}

	if(HAL_TIM_PWM_ConfigChannel(&Tim3Handle, &sConfig, 0x08) != HAL_OK)
	{
	  return HAL_ERROR;
	}
	BSP_Pwm1Start();
	return HAL_OK;
}

/********************************************************************************************************
 * @brief  启动PWM1输出
 * @retval HAL_OK    启动成功
 * @retval HAL_ERROR 启动失败
 *******************************************************************************************************/
int BSP_Pwm1Start(void)
{
	if (HAL_TIM_PWM_Start(&Tim3Handle, 0x08) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}
/*PWM2*/
/********************************************************************************************************
 * @brief 初始化PWM2模块
 * @note  配置TIM13定时器作为PWM输出，设置预分频器和周期值
 * @retval HAL_OK    初始化成功
 * @retval HAL_ERROR 初始化失败
 *******************************************************************************************************/
int BSP_Pwm2Init(void)
{
	Tim13Handle.Instance = TIM13;
	Tim13Handle.Init.Prescaler = 1;	//2分频
	Tim13Handle.Init.Period = 419;	//调节频率

	Tim13Handle.Init.ClockDivision = 0;
	Tim13Handle.Init.CounterMode = 0x00;
	if(HAL_TIM_PWM_Init(&Tim13Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

/********************************************************************************************************
 * @brief  初始化PWM3模块
 * @note   配置TIM10定时器参数并初始化PWM
 * @param  无
 * @retval HAL_OK    初始化成功
 * @retval HAL_ERROR 初始化失败
 *******************************************************************************************************/
int BSP_Pwm3Init(void)
{
	Tim10Handle.Instance = TIM10;
	Tim10Handle.Init.Prescaler = 119;//59; //1K 频率
	Tim10Handle.Init.Period = 1399;
	Tim10Handle.Init.ClockDivision = 0;
	Tim10Handle.Init.CounterMode = 0x00;
	if(HAL_TIM_PWM_Init(&Tim10Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}
/********************************************************************************************************
 * @brief 设置PWM2通道的占空比
 * @param DutyCycles 占空比值 (0-100)
 * @retval HAL_OK 设置成功
 * @retval HAL_ERROR 参数错误或配置失败
 * @note 占空比计算方式为 DutyCycles*4.2，极性由gEvChargeInfo.u8PWMPolarityCover决定
 *******************************************************************************************************/
int BSP_SetPwm2Duty(uint8_t DutyCycles)
{
	TIM_OC_InitTypeDef sConfig;

	memset(&sConfig, 0, sizeof(sConfig));
	
	if(DutyCycles>100)
	{
		return HAL_ERROR;
	}
	sConfig.OCMode = 0x60;
	sConfig.OCFastMode = 0x00;
	
		if (gEvChargeInfo.u8PWMPolarityCover)
	{
		sConfig.OCPolarity = 0x02;
	}
	else
	{
		sConfig.OCPolarity = 0x00;//0x00;	0x02脉冲反向
	}

	sConfig.OCFastMode = 0x00;
	//sConfig.Pulse = ((uint32_t)DutyCycles*1);
	//sConfig.Pulse = ((uint32_t)699);
	sConfig.Pulse = (uint32_t)((uint32_t)DutyCycles*4.2);//调节占空比
	if(HAL_TIM_PWM_ConfigChannel(&Tim13Handle, &sConfig, 0x00) != HAL_OK)
	{
		return HAL_ERROR;
	}
	BSP_Pwm2Start();
	return HAL_OK;
}

/********************************************************************************************************
 * @brief  设置PWM3通道的占空比
 * @param  DutyCycles 占空比值，范围取决于具体硬件实现
 * @retval HAL_OK 设置成功
 * @retval HAL_ERROR 设置失败
 *******************************************************************************************************/
int BSP_SetPwm3Duty(uint32_t DutyCycles)
{
	TIM_OC_InitTypeDef sConfig;

	memset(&sConfig, 0, sizeof(sConfig));
	sConfig.OCMode = 0x60;

	//if (gs_Cfg.SetData.NameData.u8PWMPolarityCover)
	//{
		//sConfig.OCPolarity = 0x02;
	//}
	//else
	{
		sConfig.OCPolarity = 0x00;//0x00;	0x02脉冲反向
	}

	sConfig.OCFastMode = 0x00;

	//sConfig.Pulse = ((uint32_t)DutyCycles * 14);
	sConfig.Pulse = DutyCycles;
	if(HAL_TIM_PWM_ConfigChannel(&Tim10Handle, &sConfig, 0x00) != HAL_OK)
	{
		return HAL_ERROR;
	}
	BSP_Pwm3Start();
	return HAL_OK;
}


/********************************************************************************************************
 * @brief 启动PWM2通道
 * @retval HAL_OK 启动成功
 * @retval HAL_ERROR 启动失败
 *******************************************************************************************************/
int BSP_Pwm2Start(void)
{
	if(HAL_TIM_PWM_Start(&Tim13Handle, 0x00) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}


/********************************************************************************************************
 * @brief  启动PWM3输出
 * @retval HAL_OK 启动成功
 * @retval HAL_ERROR 启动失败
 *******************************************************************************************************/
int BSP_Pwm3Start(void)
{
	if (HAL_TIM_PWM_Start(&Tim10Handle, 0x00) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}
