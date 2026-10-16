
#include "main.h"
#include "HistoryEvent.h"
#include "bsp_pwm.h"

#define FAN_ALM_SPEED       8
#define FAN_ALM_TM          6
#define FAN_RUNNING_OK_TM   2
#define FAN_START_DELAY     24
#define mSTOP 0
#define mRUN  1
#define ABS(a)	(((a) < 0) ? (-(a)) : (a))
FanMng_TypeDef gFANMngInfo;
UINT8 gu8TempProbeFilerTimes = 0;
UINT8 gu8TempDeltaFilerTimes = 0;
UINT8 gu8PSDeltaFilerTimes = 0;


//by lxr01202

// #include "stm32f4xx_hal_gpio.h"
// #include "stm32f4xx_hal_tim.h"
// #include "stm32f4xx_hal.h"

// // 定义PWM的引脚
// #define PWM_PIN GPIO_PIN_8  // 选择TIM1_CH1，通常映射在PA8引脚
// #define PWM_PORT GPIOC     // 选择GPIOC端口

// // 定义PWM频率和占空比
// #define PWM_FREQUENCY 1000  // PWM频率1kHz
// #define PWM_DUTY_CYCLE 50   // 占空比50%

// // 全局变量
// static TIM_HandleTypeDef htim1;
// static GPIO_InitTypeDef GPIO_InitStruct;

// // 初始化PWM输出
// void PWM_Init(void)
// {
//     // 1. 初始化HAL库
//     HAL_Init();

//     // 2. 启用GPIOA时钟（PA8引脚作为TIM1的PWM输出）
//     __GPIOA_CLK_ENABLE();


//     // 3. 配置PC8为复用功能（AF1，TIM1_CH1）
//     GPIO_InitStruct.Pin = PWM_PIN;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;  // 推挽输出
//     GPIO_InitStruct.Pull = GPIO_PULLUP;      // 拉高电阻
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 高速
//     HAL_GPIO_Init(PWM_PORT, &GPIO_InitStruct);

//     // 4. 启用TIM1时钟
//     __TIM1_CLK_ENABLE();

//     // 5. 配置TIM1为PWM模式
//     htim1.Instance = TIM1;
//     htim1.Init.Prescaler = 84 - 1;   // 定时器预分频（84用于1kHz，假设系统时钟为84MHz）
//     htim1.Init.CounterMode = TIM_COUNTERMODE_UP; // 向上计数模式
//     htim1.Init.Period = (84000000 / PWM_FREQUENCY) - 1; // 计算周期（84MHz / 1kHz）
//     htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//     HAL_TIM_PWM_Init(&htim1);

//     // 6. 配置TIM1 Channel1作为PWM输出
//     TIM_OC_InitTypeDef sConfigOC;
//     sConfigOC.OCMode = TIM_OCMODE_PWM1;        // PWM模式1
//     sConfigOC.Pulse = (htim1.Init.Period + 1) * PWM_DUTY_CYCLE / 100; // 设置占空比
//     sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH; // 输出高电平有效
//     sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//     sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
//     sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
//     sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
//     HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);

//     // 7. 启动PWM输出
//     HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
// }


// void Set_Fan_Speed(uint8_t duty_cycle)
// {
//     if (duty_cycle > 100) duty_cycle = 100; // 限制最大值为100%

//     // 计算占空比对应的脉冲值
//     uint32_t pulse = (htim1.Init.Period + 1) * duty_cycle / 100;

//     // 更新PWM占空比值
//     TIM_OC_InitTypeDef sConfigOC;
//     sConfigOC.OCMode = TIM_OCMODE_PWM1;
//     sConfigOC.Pulse = pulse;
//     sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//     sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

//     HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);

//     // 重新启动PWM以应用新配置
//     HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
// }



 void init_gFANMngInfo(void)
{
	UCHAR i,j;
	for(i=0; i < mTOTALFANNUM; i++)
	{
		for (j = 0; j < mTOTAL_SAMP_FILTER_WIDTH; j++)
		{
			gFANMngInfo.u32FanPulseInput[i][j] = 0;
		}	
	}
}

//只接一个风扇 六个FANALM里有一个有脉冲就不算告警！
 UINT8 u8GetFanAlm(void);
UINT8 u8GetFanAlm(void)
{
        return 0;
}

#define POWER_PWM_GO() do {\
    dv_Capture_PWM.fOutputFanDriver(hPOWER_PWM, 50);\
} while (0);


void AC_Fan_Ctrl(unsigned char bOn)
{
	static unsigned char s_u8OnStatBak = 0xFF;

	struct tm  tmNow;	
	static UINT8 su8Sec = 0xff;
	static UINT8 su8SecDelay = 0;

	if(s_u8OnStatBak != bOn)
	{
		s_u8OnStatBak = bOn;

		if(bOn)
		{
			gEvChargeInfo.u8CTRLInterDOCMD_B = 0x01;
		}
		else
		{
			gEvChargeInfo.u8CTRLInterDOCMD_B = 0x00;
		}

		gEvChargeInfo.u8CTRLInterDOCMDFlag_B = TRUE;

		su8SecDelay = 0;
	}
	else
	{
		gRTCData.fGetLocalTime(&tmNow);	
		if (su8Sec != tmNow.tm_sec)
		{				
			su8Sec = tmNow.tm_sec;
			su8SecDelay++;
		}
		if(su8SecDelay > 10)
		{
			su8SecDelay = 0;
			s_u8OnStatBak = 0xff;
		}
	}
}



float GetMaxTemparatureForFan(void)
{
	UINT8 i;
	float fTemp;

	fTemp = gEvChargeInfo.fEV_InterTemp;
	if(gs_CANComm.bRectTemperatureIsValid == 1)
	{
		if(gs_CANComm.fMaxRectTemperature > fTemp)
		{
			fTemp = gs_CANComm.fMaxRectTemperature;
		}
	}

	//20220726 增加几个温度
	if((gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 1)
		&& (gIMEU_Info[0].u8CommError == FALSE))
	{
		if(gIMEU_Info[0].fTP1000_1_Val < 125)
		{
			if(fTemp < gIMEU_Info[0].fTP1000_1_Val)
			{
				fTemp = gIMEU_Info[0].fTP1000_1_Val;
			}
		}

		if(gIMEU_Info[0].fTP1000_2_Val < 125)
		{
			if(fTemp < gIMEU_Info[0].fTP1000_2_Val)
			{
				fTemp = gIMEU_Info[0].fTP1000_2_Val;
			}
		}

		if(gIMEU_Info[0].fTP1000_3_Val < 125)
		{
			if(fTemp < gIMEU_Info[0].fTP1000_3_Val)
			{
				fTemp = gIMEU_Info[0].fTP1000_3_Val;
			}
		}
	}

	/*for(i = 0; i < LCCU_NUMBER_RING; i++)
	{
		if(g_u8IMMU_LCCU_COMM_Addr != (i+1))
		{
			if(gs_LccuExchangeInfo[i].u8CommAlmStat == 0)
			{
				if(fTemp < gs_LccuExchangeInfo[i].i16Temparature)
				{
					fTemp = gs_LccuExchangeInfo[i].i16Temparature;
				}
			}
		}
	}*/

	return fTemp;

}

void Fan_Imeu2_Ctrl_Deal(void)
{
	float fSystemTemp;

	static UINT8 u8STOPDelayTimes = 101;
	static UINT8 u8STARTDelayTimes = 0;
	static UINT8 s_ucIsFanStarted = 0;
	UINT8 ucDuty = 0;

	if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 0)
	{
		return;
	}

	fSystemTemp = 0;
	if(gIMEU_Info[0].u8CommError == FALSE)
	{
		if(gIMEU_Info[0].fTP1000_1_Val < 125)
		{
			if(fSystemTemp < gIMEU_Info[0].fTP1000_1_Val)
			{
				fSystemTemp = gIMEU_Info[0].fTP1000_1_Val;
			}
		}

		if(gIMEU_Info[0].fTP1000_2_Val < 125)
		{
			if(fSystemTemp < gIMEU_Info[0].fTP1000_2_Val)
			{
				fSystemTemp = gIMEU_Info[0].fTP1000_2_Val;
			}
		}

		if(gIMEU_Info[0].fTP1000_3_Val < 125)
		{
			if(fSystemTemp < gIMEU_Info[0].fTP1000_3_Val)
			{
				fSystemTemp = gIMEU_Info[0].fTP1000_3_Val;
			}
		}
	}

	if ((fSystemTemp < gEvChargeInfo.u8EV_StartTemp - 5))	//5度回差
	{
		u8STARTDelayTimes = 0;
		if (u8STOPDelayTimes < 100)
		{
			u8STOPDelayTimes++;
		}
		else
		{
			gIMEU_Info[0].u8PWMCtrlSpeed[0] = 0;
			
			if(s_ucIsFanStarted == 1)
			{
				RecordEvent(HISTORY_EVENT_CTRL_FAN_ON_IMEU2, gIMEU_Info[0].u8PWMCtrlSpeed[0], 0, (int)fSystemTemp);					
			}

			s_ucIsFanStarted = 0;
		}				
	}
	else if (fSystemTemp >= gEvChargeInfo.u8EV_StartTemp)//40-50
	{				
		u8STOPDelayTimes = 0;
		if (u8STARTDelayTimes < 10)
		{
			u8STARTDelayTimes++;
		}
		else
		{
			if (fSystemTemp <= gEvChargeInfo.u8EV_Ultra_Temp && (gEvChargeInfo.u8EV_Ultra_Temp - gEvChargeInfo.u8EV_StartTemp)>0)
			{
				ucDuty = (UINT8)(50*((fSystemTemp - gEvChargeInfo.u8EV_StartTemp)/(gEvChargeInfo.u8EV_Ultra_Temp - gEvChargeInfo.u8EV_StartTemp)));
				if (ucDuty >50)
				{
					ucDuty = 50;
				}
				gIMEU_Info[0].u8PWMCtrlSpeed[0] = 50 + ucDuty;
			}
			else
			{
				gIMEU_Info[0].u8PWMCtrlSpeed[0] = 100;						//全速
			}
			

			if(s_ucIsFanStarted == 0)
			{
				RecordEvent(HISTORY_EVENT_CTRL_FAN_ON_IMEU2, gIMEU_Info[0].u8PWMCtrlSpeed[0], 1, (int)fSystemTemp);					
			}
			s_ucIsFanStarted = 1;
		}
	}
	else if(s_ucIsFanStarted == 1)
	{
		ucDuty = (UINT8)(25*((fSystemTemp - (gEvChargeInfo.u8EV_StartTemp-5))/5));
		if (ucDuty >25)
		{
			ucDuty = 25;
		}
		gIMEU_Info[0].u8PWMCtrlSpeed[0] = 25 + ucDuty;
	}
	else
	{
		gIMEU_Info[0].u8PWMCtrlSpeed[0] = 0;
	}
}


//#define	FAN_FULL_ON_CHGING_En		1  //使能 当执行充电时风扇全速转
void TAST_FanProc(void* pdata)
{
	pdata = pdata;
	UINT8 u8STOPDelayTimes = 101;
	UINT8 u8STARTDelayTimes = 0;
	UINT8 ucDuty = 0;
	float fSystemTemp;
	static UINT8 s_ucIsFanStarted = 0;
	
	gSelfTest.fSetTaskID(TASK_ID_FAN);

	OSTimeDly(500);

//	BSP_SetPwm1Duty(0);

	POWER_PWM_GO();

	__FAN_ON();
	AC_Fan_Ctrl(TRUE);
//test
//        BSP_SetPwm1Duty(100);
 //      BSP_SetPwm1Duty(80);
 //     BSP_SetPwm1Duty(60);
 //      BSP_SetPwm1Duty(40);
 //   BSP_SetPwm1Duty(20);
 //      BSP_SetPwm1Duty(0);
	//__ZTE_FAN_ON();
//	dv_Capture_PWM.fOutputFanDriver(mFAN1,99); //风机调速
	dv_Capture_PWM.fOutputFanDriver(mFAN1,0); //风机调速
	for (u8STOPDelayTimes = 0; u8STOPDelayTimes < 1; u8STOPDelayTimes++)
	{
		gSelfTest.fSetTaskID(TASK_ID_FAN);
		OSTimeDly(500);
	}
	//__ZTE_FAN_OFF();
	__FAN_OFF();
	AC_Fan_Ctrl(FALSE);

	dv_Capture_PWM.fOutputFanDriver(mFAN1,45);			//风机调速
	for (u8STOPDelayTimes = 0; u8STOPDelayTimes < 1; u8STOPDelayTimes++)
	{
		gSelfTest.fSetTaskID(TASK_ID_FAN);
		OSTimeDly(500);
	}
	
	dv_Capture_PWM.fOutputFanDriver(mFAN1,10);			//风机调速
	POWER_PWM_GO();
	for (u8STOPDelayTimes = 0; u8STOPDelayTimes < 1; u8STOPDelayTimes++)
	{
		gSelfTest.fSetTaskID(TASK_ID_FAN);
		OSTimeDly(500);
	}

	u8STOPDelayTimes = 101;
	float fVolt, fCurr;
    fVolt = 0;
    fCurr = 0;
    UNUSED(fVolt);
    UNUSED(fCurr);
	gEvChargeInfo.u16FanCotrol = 0;
	struct tm  tmNow;
	//PlugInChargingDeal(TRUE);
	for(;;)
	{
		
		gSelfTest.fSetTaskID(TASK_ID_FAN);
		OSTimeDly(10);

		//continue;

		if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
		{
			OSTimeDly(100);
			continue;
		}

		
		POWER_PWM_GO();

		gSelfTest.fSetTaskID(TASK_ID_FAN);
		if (gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)
		{
			// s_ucIsFanStarted = 0;
			
			if(gEvChargeInfo.u8ManualCtrlFANDelay < 120)
			{
				continue;
			}
		}		
		
		gRTCData.fGetLocalTime(&tmNow);

		UINT8 u8IsDirectCoolSystem = FALSE;
		if ((gEvChargeInfo.u8_ParamTOPFanRunTimer == 0) &&  (gEvChargeInfo.u8_ParamTOPFanStopTimer == 0))
		{
			u8IsDirectCoolSystem = TRUE;	//是直通风系统！
		}
		else
		{
			if (gEvChargeInfo.u16FanCotrol >= (gEvChargeInfo.u8_ParamTOPFanRunTimer + gEvChargeInfo.u8_ParamTOPFanStopTimer))
			{
				gEvChargeInfo.u16FanCotrol = 0;
			}
		}

#ifdef	FAN_FULL_ON_CHGING_En
		//开机运行就全速！！
		if (((FALSE == u8IsDirectCoolSystem) && (gEvChargeInfo.u16FanCotrol <= gEvChargeInfo.u8_ParamTOPFanRunTimer)))
		{
			__FAN_ON();
			AC_Fan_Ctrl(TRUE);
			dv_Capture_PWM.fOutputFanDriver(mFAN1,100);						//风机全速
			s_ucIsFanStarted = 1;
			OSTimeDly(100); 
			gSelfTest.fSetTaskID(TASK_ID_FAN);
			u8GetFanAlm();													//必须有风扇转的地方再调用该函数
		}
		else if ((FALSE == u8IsDirectCoolSystem))//水冷
		{
			__FAN_OFF();
			AC_Fan_Ctrl(FALSE);
			s_ucIsFanStarted = 0;
			dv_Capture_PWM.fOutputFanDriver(mFAN1,0);//停转			
		}
		else
#endif
		{
			OSTimeDly(50);
			gSelfTest.fSetTaskID(TASK_ID_FAN);	

			fSystemTemp = GetMaxTemparatureForFan();

			if ((fSystemTemp < gEvChargeInfo.u8EV_StartTemp - 5))	//5度回差
			{
				u8STARTDelayTimes = 0;
				if (u8STOPDelayTimes < 100)
				{
					u8STOPDelayTimes++;
				}
				else
				{
					dv_Capture_PWM.fOutputFanDriver(mFAN1,0);//停转
					gEvChargeInfo.u8SysFanALM = FALSE;
					__FAN_OFF();
					AC_Fan_Ctrl(FALSE);

					if(s_ucIsFanStarted == 1)
					{
						RecordEvent(HISTORY_EVENT_CTRL_FAN_ON, dv_Capture_PWM.Rate[mFAN1], 0, (int)fSystemTemp);					
					}

					s_ucIsFanStarted = 0;
				}				
			}
			else if (fSystemTemp >= gEvChargeInfo.u8EV_StartTemp)//40-50
			{				
				u8STOPDelayTimes = 0;
				if (u8STARTDelayTimes < 10)
				{
					u8STARTDelayTimes++;
				}
				else
				{
					__FAN_ON();
					AC_Fan_Ctrl(TRUE);

					if (fSystemTemp <= gEvChargeInfo.u8EV_Ultra_Temp && (gEvChargeInfo.u8EV_Ultra_Temp - gEvChargeInfo.u8EV_StartTemp)>0)
					{
						ucDuty = (UINT8)(50*((fSystemTemp - gEvChargeInfo.u8EV_StartTemp)/(gEvChargeInfo.u8EV_Ultra_Temp - gEvChargeInfo.u8EV_StartTemp)));
						if (ucDuty >50)
						{
							ucDuty = 50;
						}
						dv_Capture_PWM.fOutputFanDriver(mFAN1,50 + ucDuty);
					}
					else
					{
						dv_Capture_PWM.fOutputFanDriver(mFAN1, 100);							//全速
					}
					u8GetFanAlm();																//必须有风扇转的地方再调用该函数

					

					if(s_ucIsFanStarted == 0)
					{
						RecordEvent(HISTORY_EVENT_CTRL_FAN_ON, dv_Capture_PWM.Rate[mFAN1], 1, (int)fSystemTemp);					
					}
					s_ucIsFanStarted = 1;
				}
			}
			else if(s_ucIsFanStarted == 1)
			{
				__FAN_ON();
				AC_Fan_Ctrl(TRUE);

				ucDuty = (UINT8)(25*((fSystemTemp - (gEvChargeInfo.u8EV_StartTemp-5))/5));
				if (ucDuty >25)
				{
					ucDuty = 25;
				}
				dv_Capture_PWM.fOutputFanDriver(mFAN1,25 + ucDuty);
			}
			else
			{
				dv_Capture_PWM.fOutputFanDriver(mFAN1,0);//停转
				gEvChargeInfo.u8SysFanALM = FALSE;
				__FAN_OFF();
				AC_Fan_Ctrl(FALSE);
			}
		}

		//20220818 LKF 增加IMEU2风机控制功能
		Fan_Imeu2_Ctrl_Deal();

		OSTimeDly(150); 
		gSelfTest.fSetTaskID(TASK_ID_FAN);
	}
}