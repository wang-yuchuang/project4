/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Insulation.c
** @Founder      : WYC834
** @Date         : 2025.10.21
** @Function     : 绝缘检测策略
** @Instructions : 
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "Insulation.h"
#include "SystemData.h"
#include "BSPTIM.h"
#include "BSPKA84917_async.h"
// #include "BSPKA84917.h"
#include "SystemData.h"
//RTOS延时必须头文件
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "BSPDIDO.h"
#include "SystemDiag.h"
#include "SystemInfo.h"
#include "SystemPara.h"
#include "CANTabOper.h"
#include "CANUser.h"
#include "SEGGER_RTT.h"
#ifdef INSULATION_TEST
#include "TEST.h"
#endif
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/

InsulationDetectMng gs_InsulationDetectMng;

static u8 s_u8PeriodResetReq = 0;/*绝缘检测周期计时基准重置标志*/

u16 UserCtrlCmd_55[UserCtrlCmd_len] =
{
    0x0                             /*用户控制命令*/
};
u16 CtrlCmdFeedback_56[CtrlFeedback_len] = 
{
	0x0   
};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CtrlAfterInsuPeRlyState(void)
** @Input     : void
** @Output    : void
** @Function  : 控制绝缘检测后PE继电器\IMD继电器状态
** @The notes : 0：断开 1:闭合
**===========================================================================================*/
static void CtrlAfterInsuPeRlyState(void);
/*=============================================================================================
** @Name      : u8 CheckInsulationTaskWithDynamicPeriod(void);
** @Input     : void
** @Output    : void
** @Function  : 绝缘检测采样周期控制 (秒s)
** @The notes : 
**===========================================================================================*/
static u8 CheckInsulationTaskWithDynamicPeriod(void);
static void CheckInsulationTask_ResetPeriod(void);
static void Insulation_Calculate(double fUN1,double fUP1,double fUN2, double fUP2, double fRN1,double fRP1, double fRN2, double fRP2,double* pfRN,double* pfRP);


static u8 Insulation_DetectProcess(void);
static u8 Insulation_StartBatVoltCheck(void);
static u8 Insulation_BatVoltCheck(void);
static u8 Insulation_CheckVoltStatble(void);
static void Insulation_RestoreSwitchAndTimer(void);
static u8  Insluation_STEP_1_ConnectPE_GetVolt(void);
static u8  Insluation_STEP_2_ChangeResist_GetN2GVoltage(void);
static UINT8  Insluation_STEP_3_CalcResist(void);
static void Insulation_GetN2GandP2GVoltage(void);
static UINT8 Insulation_CheckN2GVoltStable(void);
static void Insulation_RecordN2GandP2GVoltage(u8 time);


#ifdef INSULATION_TEST
static void check_insulation(const char *tag);
#endif

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void ControlRLYSwitchTime(void);
** @Input     : void
** @Output    : void
** @Function  : 板上继电器切换至少间隔时间（ms）
** @The notes : 
**===========================================================================================*/
void ControlRLYSwitchTime(void)
{
    u16 time = stParameter.u16ArraySet[RlySwiftInterTime];
    osDelay((u32)time);
}
/*=============================================================================================
** @Name      : void InsulationProcess(void);
** @Input     : void
** @Output    : void
** @Function  : 绝缘检测流程
** @The notes :
**===========================================================================================*/
void InsulationProcess(void)
{
#ifdef INSULATION_TEST
    static u8 dump_once = 1;
    if (dump_once) { check_insulation("InsulationProcess_FIRST"); dump_once = 0; }
#endif

	u8 u8RTN;
	u8 u8InsulationProcessRTN;
	static u32 u32StartTimer = 0;
	static u8 s_u8StartTimerInit = 0;
	static u32 s_TermHoldTimer = 0;  /*终态锁存计时器*/
	static u8 s_u8TermHoldInit = 0;

	//芯片初始化、高压采样就绪检查
	if (!((wakeUpState == 1) && (FirstSampleFinishedFlag == 1) && (GetfAdjustBatVolt()>= mInsulationStartMINVoltage )))
	{
		return;
	}



	
	 if (gs_InsulationDetectMng.ucInsulationMode == AutoDetect)
	{
		// 周期控制：仅在检测起始或停止状态时生效，不阻塞检测过程中的状态切换(无绝缘检测命令)
		if ((gs_InsulationDetectMng.ucInsulationState == eInsulation_Start)
		|| (gs_InsulationDetectMng.ucInsulationState == eInsulation_Stop))
		{
			if (0 == CheckInsulationTaskWithDynamicPeriod())
			{
				return;
			}
		}
	}


		if(gs_InsulationDetectMng.ucInsulationMode == StopDetect)//停止检测
		{
			if (gs_InsulationDetectMng.ucInsulationState != eInsulation_Stop)
			{
				CtrlAfterInsuPeRlyState();//控制绝缘检测结束后PE继电器状态
			}
			gs_InsulationDetectMng.ucInsulationState = eInsulation_Stop;
			CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Stop;
		}
		//添加绝缘检测采样周期计时判断
		else if(gs_InsulationDetectMng.ucInsulationMode == AutoDetect 
			|| gs_InsulationDetectMng.ucInsulationMode == StartDetect)//开始绝缘检测
		{

			if(gs_InsulationDetectMng.ucInsulationState == eInsulation_Start)
			{
				u8RTN = Insulation_StartBatVoltCheck();//检测是否符合启动检测电压
				if(u8RTN == TRUE)
				{
					DiagSetErrNowStateAPI(eERR_ID_INS_LOW,eErr_A);
					u32StartTimer = 0;
					gs_InsulationDetectMng.ucVoltStable = 1;   
					//下一阶段的电压稳定超时初始化，赋值1时为了保证启动时检测一次电压稳定性
					gs_InsulationDetectMng.ucGetVoltDelay = 0;//绝缘检测构成初始标志
					gs_InsulationDetectMng.ucInsulationState = eInsulation_Process;
					CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Process;
					s_u8StartTimerInit = 0;
					gs_InsulationDetectMng.ucInsulationCtrlStep = INSULATION_STEP_1;
				}
				else
				{
					if(s_u8StartTimerInit == 0)
					{
						u32StartTimer = GetRunTimers();
						s_u8StartTimerInit = 1;
					}
					else if (((GetRunTimers() + RunTimersOverflow - u32StartTimer)%RunTimersOverflow) > stParameter.u16ArraySet[InsuTimeLimit])//超时电压未起来
					//电压如果一直小于启动电压，则停止运行
					{
						gs_InsulationDetectMng.ucInsulationState = eInsulation_OverTime;
						CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_OverTime;
						DiagSetErrNowStateAPI(eERR_ID_INS_LOW,eErr_B);
					}
				}
				
			}
			else if(gs_InsulationDetectMng.ucInsulationState == eInsulation_Process)
			{
					u8RTN = Insulation_BatVoltCheck();
					if(u8RTN == TRUE)//电压稳定，则进入绝缘处理
					{
						DiagSetErrNowStateAPI(eERR_ID_INS_STA,eErr_A);
						u8InsulationProcessRTN = Insulation_DetectProcess();	
						if(u8InsulationProcessRTN == FALSE)
						{
							/*STEP_3返回FALSE可能是因为阻值为负(已设Fault)，也可能是STEP_1/2超时(未设状态)*/
							/*只在状态仍为Process时才设OverTime，避免覆盖STEP_3已设置的Fault*/
							if(gs_InsulationDetectMng.ucInsulationState == eInsulation_Process)
							{
								gs_InsulationDetectMng.ucInsulationState = eInsulation_OverTime;
								CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_OverTime;
							}
						}
						else
						{
							/*STEP_1/2返回TRUE时状态仍为Process；STEP_3返回TRUE时状态已被设为End*/
							if(gs_InsulationDetectMng.ucInsulationState == eInsulation_Process)
								CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Process;
						}
					}
					else //电压如果不稳定，则报错
					{
						DiagSetErrNowStateAPI(eERR_ID_INS_STA,eErr_B);//认为绝缘检测电压不稳定
						gs_InsulationDetectMng.ucInsulationState = eInsulation_OverTime;	
						CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_OverTime;
					}
			}
			else if((gs_InsulationDetectMng.ucInsulationState == eInsulation_End)
					||(gs_InsulationDetectMng.ucInsulationState == eInsulation_OverTime)
					||(gs_InsulationDetectMng.ucInsulationState == eInsulation_Fault)
					||(gs_InsulationDetectMng.ucInsulationState == eInsulation_Stop))
			{
				/*Stop 状态由用户命令维持，已持续上传，可直接切换*/
				if(gs_InsulationDetectMng.ucInsulationState == eInsulation_Stop)
				{
					Insulation_RestoreSwitchAndTimer();
					u32StartTimer = 0;
					s_u8StartTimerInit = 0;
					s_TermHoldTimer = 0;
					s_u8TermHoldInit = 0;
					gs_InsulationDetectMng.ucInsulationState = eInsulation_Start;
					CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Start;
				}
				else /*End/OverTime/Fault: 终态锁存，确保CAN 1s周期上传*/
				{
					/*首次进入终态: 恢复开关 + 记录时刻*/
					if(s_u8TermHoldInit == 0)
					{
						s_TermHoldTimer = GetRunTimers();
						s_u8TermHoldInit = 1;
						Insulation_RestoreSwitchAndTimer();
						u32StartTimer = 0;
						s_u8StartTimerInit = 0;
                                                CtrlAfterInsuPeRlyState();//控制绝缘检测结束后PE继电器状态
						/*feedback保留终态，不改动*/
					}
					/*锁存时间到: 切回自动检测并重启*/
					else if((((GetRunTimers() + RunTimersOverflow - s_TermHoldTimer) % RunTimersOverflow) >= stParameter.u16ArraySet[InsuSampPeriod]) && (gs_InsulationDetectMng.ucInsulationMode == AutoDetect))
					{
						s_TermHoldTimer = 0;
						s_u8TermHoldInit = 0;
					CheckInsulationTask_ResetPeriod();
						gs_InsulationDetectMng.ucInsulationState = eInsulation_Start;
						CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Start;
					}
					/*未到锁存时间: 保留终态，不改动*/
				}
			}
			else 
			{
				Insulation_InitInsulationData();
			}	
		}
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : u8 CheckInsulationTaskWithDynamicPeriod(void);
** @Input     : void
** @Output    : void
** @Function  : 绝缘检测采样周期控制 (秒s)
** @The notes : 
**===========================================================================================*/
static u8 CheckInsulationTaskWithDynamicPeriod(void)
{
	static TickType_t xLastWakeTime = 0;
	static uint32_t Period = 0;
	static u8 s_u8PeriodInit = 0;
        TickType_t xCurrentTime;
        uint32_t u32CurrentPeriod;
    
    // 获取当前采样周期
    // 秒直接转 ticks，避免 pdMS_TO_TICKS 内部乘法溢出 (21600s*1000ms*1000Hz > uint32_max)
    u32CurrentPeriod = (uint32_t)stParameter.u16ArraySet[InsuSampPeriod] * (uint32_t)configTICK_RATE_HZ;
    
    // 首次调用
    if (s_u8PeriodInit == 0)
    {
        xLastWakeTime = xTaskGetTickCount();
        Period = u32CurrentPeriod;
        s_u8PeriodInit = 1;
        return 0;  //首次调用:开始计时,不立即触发
    }
    
    // 外部请求重置周期基准
    if (s_u8PeriodResetReq == 1)
    {
        s_u8PeriodResetReq = 0;
        xLastWakeTime = xTaskGetTickCount();
        return 0;  //重置后不立即触发
    }
    
    xCurrentTime = xTaskGetTickCount();
    
    // 检查周期是否发生变化
    if (u32CurrentPeriod != Period)
    {
        // 周期变化，重新初始化时间基准
        Period = u32CurrentPeriod;
        xLastWakeTime = xCurrentTime;
        return 1;  // 周期变化时立即执行一次
    }
    
    // 检查是否达到采样周期
    if ((xCurrentTime - xLastWakeTime) >= u32CurrentPeriod)
    {
        // 更新时间基准
        xLastWakeTime += u32CurrentPeriod;//500
        
        // 防止因长时间阻塞导致的时间漂移
        if ((xCurrentTime - xLastWakeTime) > u32CurrentPeriod)
        {
            xLastWakeTime = xCurrentTime;
        }
#ifdef INSULATION_TEST

		static TickType_t s_LastTriggerTime = 0;
		TickType_t now  = xTaskGetTickCount();
		if (s_LastTriggerTime != 0)
		{
			SEGGER_RTT_printf(0, "[Test Insulation cycle] dt= %dms\r\n", 
            (int)((now -s_LastTriggerTime)* portTICK_PERIOD_MS));
		}
		s_LastTriggerTime = now;
#endif
        return 1;  // 达到时间
		
    }
    
    return 0;  // 未达到时间
}

static void CheckInsulationTask_ResetPeriod(void)
{
	s_u8PeriodResetReq = 1;
}

/*=============================================================================================
** @Name      : static void CtrlAfterInsuPeRlyState(void)
** @Input     : void
** @Output    : void
** @Function  : 控制绝缘检测后PE继电器\IMD继电器状态
** @The notes : 0：断开 1:闭合
**===========================================================================================*/
static void CtrlAfterInsuPeRlyState(void)
{
	u16 state = stParameter.u16ArraySet[AfterInsuPeRlyState];

	if (state == 0)//断开
	{
		PE_RLY_Contr_Open();
		IMD_RLY_Open();
		//添加板上继电器切换至少间隔时间
		ControlRLYSwitchTime();
	}
	else//闭合
	{
		PE_RLY_Contr_Close();
		IMD_RLY_Open();
		//添加板上继电器切换至少间隔时间
		ControlRLYSwitchTime();
	}
	
}
//绝缘检测过程
static u8 Insulation_DetectProcess(void)
{
	u8 u8RTN = FALSE;
	if(gs_InsulationDetectMng.ucInsulationCtrlStep == INSULATION_STEP_1)
	{
		u8RTN = Insluation_STEP_1_ConnectPE_GetVolt();
	}
	else if(gs_InsulationDetectMng.ucInsulationCtrlStep == INSULATION_STEP_2)
	{
		u8RTN = Insluation_STEP_2_ChangeResist_GetN2GVoltage();
	}
	else if(gs_InsulationDetectMng.ucInsulationCtrlStep == INSULATION_STEP_3)
	{
		u8RTN = Insluation_STEP_3_CalcResist();
	}
	return u8RTN;
}


//检测是否符合启动检测电压
static u8 Insulation_StartBatVoltCheck(void)
{
	u8 u8RTN = FALSE;
#ifdef INSULATION_TEST
	/* 电压注入hook: 测试时使用注入值替代实际采样 */
	if (Test_IsInjectEnabled())
	{
		float bat_temp, n2g_temp;
		Test_GetInjectedVoltage(&bat_temp, &n2g_temp);
		gs_InsulationDetectMng.fBatVoltage = bat_temp;
	}
	else
	{
		gs_InsulationDetectMng.fBatVoltage = GetfAdjustBatVolt();
	}
#endif
	gs_InsulationDetectMng.fBatVoltage = GetfAdjustBatVolt();
	gs_InsulationDetectMng.fBatVoltageBack = gs_InsulationDetectMng.fBatVoltage;
	if(gs_InsulationDetectMng.fBatVoltage > mInsulationStartMINVoltage)
	{
		u8RTN = TRUE;
	}else
	{
		u8RTN = FALSE;
	}
	return u8RTN;

}

/*比较本次采样电压和上次电压，如果不一致，则启动计时器计时
如果一致，则停止计时，并且更新母排电压备份值*/
static u8 Insulation_BatVoltCheck(void)
{
	u8 u8CompareRTN = FALSE,u8RTN = FALSE;

#ifdef INSULATION_TEST
	/* 获取电压 */
	if (Test_IsInjectEnabled())
	{
		float bat_temp, n2g_temp;
		Test_GetInjectedVoltage(&bat_temp, &n2g_temp);
		gs_InsulationDetectMng.fBatVoltage = bat_temp;
	}
	else
	{
		gs_InsulationDetectMng.fBatVoltage = GetfAdjustBatVolt();
	}
#endif
	gs_InsulationDetectMng.fBatVoltage = GetfAdjustBatVolt();
	u8CompareRTN = Insulation_CheckVoltStatble();
	if(u8CompareRTN == TRUE)
	{
		gs_InsulationDetectMng.fBatVoltageBack = gs_InsulationDetectMng.fBatVoltage;
		u8RTN = TRUE;
	}
	else
	{
		u8RTN = FALSE;
	}	
	
return u8RTN;
}

//检测电池电压是否稳定，检测周期两次电压的差值是否小于最小差值
static u8 Insulation_CheckVoltStatble(void)
{
	u8 u8RTN = FALSE;
	float fDelta1 = 0;

	fDelta1 = gs_InsulationDetectMng.fBatVoltage - gs_InsulationDetectMng.fBatVoltageBack;

	if ((FABS(fDelta1) < mInsulationVoltStableLimit) 
	&&(gs_InsulationDetectMng.fBatVoltage > mInsulationStartMINVoltage))
	{
		u8RTN = TRUE;
	}

	return u8RTN;
}

static void Insulation_RestoreSwitchAndTimer(void)
{
	//Insulation_Ctrl_Connect_PE(u8Channel,OpenSwitch);	
	//Insulation_Change_Resistance(u8Channel,OpenSwitch);	
	gs_InsulationDetectMng.ucInsulationCtrlStep =INSULATION_STEP_1;
	gs_InsulationDetectMng.ucGetVoltDelay = 0;
	gs_InsulationDetectMng.ucVoltStable= 0;
	// gs_InsulationDetectMng.SwitchRelays = FALSE;
}


//绝缘检测初始化
void Insulation_InitInsulationData(void)
{
	//test
	//gs_InsulationDetectMng.ucInsulationMode = StartDetect;
  gs_InsulationDetectMng.ucInsulationMode = AutoDetect;
	gs_InsulationDetectMng.ucInsulationState = eInsulation_Start;
	gs_InsulationDetectMng.fBatVoltage = 0;
	gs_InsulationDetectMng.fN2GVoltage = 0;
	gs_InsulationDetectMng.fP2GVoltage = 0;
	gs_InsulationDetectMng.fBatVoltageBack = 0;
	gs_InsulationDetectMng.fN2GVoltageBack = 0;
	gs_InsulationDetectMng.fP2GVoltageBack = 0;
	gs_InsulationDetectMng.fN2GVoltageCalc[First_Time] = 0;
	gs_InsulationDetectMng.fP2GVoltageCalc[First_Time]= 0;
	gs_InsulationDetectMng.fN2GVoltageCalc[Second_Time] = 0;
	gs_InsulationDetectMng.fP2GVoltageCalc[Second_Time] = 0;
	gs_InsulationDetectMng.fN2GResistance = 0;
	gs_InsulationDetectMng.fP2GResistance = 0;
	gs_InsulationDetectMng.fN2GResistanceLast = 0;
	gs_InsulationDetectMng.fP2GResistanceLast = 0;
	gs_InsulationDetectMng.ucInsulationCtrlStep =INSULATION_STEP_1;
	gs_InsulationDetectMng.ucGetVoltDelay= 0;
	//gs_InsulationDetectMng.SwitchRelays[u8Channel] = FALSE;

#ifdef INSULATION_TEST
    check_insulation("InitInsulationData_DONE");
#endif


	CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Start;
}



static u8  Insluation_STEP_1_ConnectPE_GetVolt(void)
{
	u8 ucRTN;
	static u32 u32Step1StartTimer = 0;
	if(gs_InsulationDetectMng.ucGetVoltDelay == 0)
	{
		PE_RLY_Contr_Close();//获取母排电压后，接入PE
		IMD_RLY_Open();
		//添加板上继电器切换至少间隔时间
		ControlRLYSwitchTime();

		osDelay(100);//等待电路稳定，确保Measure任务已更新gSystemData.fPeGndVolt
		
		Insulation_GetN2GandP2GVoltage();//获取负母排对地电压
		gs_InsulationDetectMng.ucGetVoltDelay = 1; //接通继电器后，需要等待测量完成才能进入
		gs_InsulationDetectMng.fN2GVoltageBack = gs_InsulationDetectMng.fN2GVoltage;
        gs_InsulationDetectMng.fP2GVoltageBack = gs_InsulationDetectMng.fP2GVoltage;
		osDelay(1000);
                u32Step1StartTimer = GetRunTimers();
	}
	else if (gs_InsulationDetectMng.ucGetVoltDelay > 0) //到达检测周期
	{ 
		if(gs_InsulationDetectMng.ucGetVoltDelay > 0)
		 {
			 Insulation_GetN2GandP2GVoltage();//获取正负母排对地电压
			 ucRTN =Insulation_CheckN2GVoltStable();//负对地电压是否稳定

			if(ucRTN == TRUE)
			{
			gs_InsulationDetectMng.ucGetVoltDelay++;
			}
			else
			{
			gs_InsulationDetectMng.ucGetVoltDelay= 1;
			gs_InsulationDetectMng.fN2GVoltageBack = gs_InsulationDetectMng.fN2GVoltage;
			gs_InsulationDetectMng.fP2GVoltageBack = gs_InsulationDetectMng.fP2GVoltage;
			}
			 osDelay(200);
		 }
		if(gs_InsulationDetectMng.ucGetVoltDelay > 3) //连续三次稳定则记录第一组数据
		{
			Insulation_RecordN2GandP2GVoltage(First_Time);//获取正负母排对地电压
			//如果电压太小，直接判断为绝缘故障
			if((gs_InsulationDetectMng.fN2GVoltageCalc[First_Time] < SHORT_VOLT_LIMIT)
			||(gs_InsulationDetectMng.fP2GVoltageCalc[First_Time]< SHORT_VOLT_LIMIT))
			{ 
				gs_InsulationDetectMng.ucInsulationCtrlStep = INSULATION_STEP_3;
			}
			else //如果电压值正常，继续投切计算绝缘阻值
			{
				gs_InsulationDetectMng.ucInsulationCtrlStep = INSULATION_STEP_2;//延时到达进入下一步
			}
			
			gs_InsulationDetectMng.ucGetVoltDelay = 0; //初始化计数器
		}
	}
	if (((GetRunTimers() + RunTimersOverflow - u32Step1StartTimer)%RunTimersOverflow) > 50)//pe测量不稳则超时返回
	{
			return FALSE;
	}
	return TRUE;	
}

static u8  Insluation_STEP_2_ChangeResist_GetN2GVoltage(void)
{
	u8 ucRTN;
	static u32 u32Step1StartTimer = 0;
	if(gs_InsulationDetectMng.ucGetVoltDelay == 0)
	{
		IMD_RLY_Close(); //闭合继电器，改变分压电阻
		PE_RLY_Contr_Close();	 //接入PE
		//添加板上继电器切换至少间隔时间
		ControlRLYSwitchTime();

		osDelay(200);//等待电路稳定，确保Measure任务已更新gSystemData.fPeGndVolt
		
		Insulation_GetN2GandP2GVoltage();//获取正负母排对地电压
		gs_InsulationDetectMng.ucGetVoltDelay= 1;  //接通继电器后，需要等待测量完成才能进入
		gs_InsulationDetectMng.fN2GVoltageBack = gs_InsulationDetectMng.fN2GVoltage;
		gs_InsulationDetectMng.fP2GVoltageBack = gs_InsulationDetectMng.fP2GVoltage;	
		osDelay(1000);
                u32Step1StartTimer = GetRunTimers();
	}
	else if (gs_InsulationDetectMng.ucGetVoltDelay > 0)
	{ 
		if(gs_InsulationDetectMng.ucGetVoltDelay >0)
		{
			Insulation_GetN2GandP2GVoltage();//获取正负母排对地电压
			ucRTN =Insulation_CheckN2GVoltStable();//负对地电压是否稳定
			if(ucRTN == TRUE)
			{
			gs_InsulationDetectMng.ucGetVoltDelay++;
			}
			else
			{
			gs_InsulationDetectMng.ucGetVoltDelay= 1;
			gs_InsulationDetectMng.fN2GVoltageBack = gs_InsulationDetectMng.fN2GVoltage;
			gs_InsulationDetectMng.fP2GVoltageBack = gs_InsulationDetectMng.fP2GVoltage;
			}
			osDelay(200);
		}
		if(gs_InsulationDetectMng.ucGetVoltDelay>3) //连续三次稳定则记录第一组数据
		{
			Insulation_RecordN2GandP2GVoltage(Second_Time);//获取正负母排对地电压
			gs_InsulationDetectMng.ucInsulationCtrlStep = INSULATION_STEP_3;
			gs_InsulationDetectMng.ucGetVoltDelay = 0; //初始化计数器
		}
	}	
	if (((GetRunTimers() + RunTimersOverflow - u32Step1StartTimer)%RunTimersOverflow) > 50)//pe测量不稳则超时返回
	{
			return FALSE;
	}
	return TRUE;	
}
//接地检测第三步 计算电压值 正负极对pe绝缘输入电阻值
static UINT8  Insluation_STEP_3_CalcResist(void)
{
	f64 fRN = NORMAL_VALUE;
	f64 fRP = NORMAL_VALUE;
	float fAlarmLimit,fFaultLimit;

	fAlarmLimit = ALARM_LIMIT_RATE*gs_InsulationDetectMng.fBatVoltage;
	fFaultLimit = FAULT_LIMIT_RATE*gs_InsulationDetectMng.fBatVoltage;
	
	if(gs_InsulationDetectMng.fN2GVoltageCalc[First_Time] < SHORT_VOLT_LIMIT)
	{
		gs_InsulationDetectMng.fN2GResistance = 0;
		gs_InsulationDetectMng.fP2GResistance = NORMAL_VALUE;
		gs_InsulationDetectMng.ucInsulationState = eInsulation_Fault;//电压负值，异常
		CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Fault;
		
	}
	else if(gs_InsulationDetectMng.fP2GVoltageCalc[First_Time]  < SHORT_VOLT_LIMIT)
	{
		gs_InsulationDetectMng.fP2GResistance = 0;
		gs_InsulationDetectMng.fN2GResistance = NORMAL_VALUE;
		gs_InsulationDetectMng.ucInsulationState = eInsulation_Fault;//电压负值，异常
		CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Fault;
	}
	else
	{
		 Insulation_Calculate(gs_InsulationDetectMng.fN2GVoltageCalc[First_Time],
				  gs_InsulationDetectMng.fP2GVoltageCalc[First_Time],
				  gs_InsulationDetectMng.fN2GVoltageCalc[Second_Time],
				  gs_InsulationDetectMng.fP2GVoltageCalc[Second_Time],
				  FIRST_STEP_R_N,FIRST_STEP_R_P,
				  SECOND_STEP_R_N,SECOND_STEP_R_P,&fRN,&fRP);
		SEGGER_RTT_printf(0, "[Insu STEP3] Bat=%d.%d UN1=%d.%d UP1=%d.%d UN2=%d.%d UP2=%d.%d\r\n",
			(int)gs_InsulationDetectMng.fBatVoltage, (int)(gs_InsulationDetectMng.fBatVoltage*10)%10,
			(int)gs_InsulationDetectMng.fN2GVoltageCalc[First_Time], (int)(gs_InsulationDetectMng.fN2GVoltageCalc[First_Time]*10)%10,
			(int)gs_InsulationDetectMng.fP2GVoltageCalc[First_Time], (int)(gs_InsulationDetectMng.fP2GVoltageCalc[First_Time]*10)%10,
			(int)gs_InsulationDetectMng.fN2GVoltageCalc[Second_Time], (int)(gs_InsulationDetectMng.fN2GVoltageCalc[Second_Time]*10)%10,
			(int)gs_InsulationDetectMng.fP2GVoltageCalc[Second_Time], (int)(gs_InsulationDetectMng.fP2GVoltageCalc[Second_Time]*10)%10);
		SEGGER_RTT_printf(0, "[Insu STEP3] raw fRN=%d raw fRP=%d (x100)\r\n",
			(int)(fRN*100), (int)(fRP*100));

				  
	if(!(fRN >= 0 || fRP >= 0))//拦截NaN(NaN>=0为false)和负值
		{
			gs_InsulationDetectMng.ucInsulationState = eInsulation_Fault;//电阻负值，异常
			CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_Fault;
			return FALSE;
		}
		if(fRN < 0||fRN >= NORMAL_VALUE)
		{
			fRN = NORMAL_VALUE;
		}
		
		if(fRP < 0||fRP >= NORMAL_VALUE)
		{
			fRP = NORMAL_VALUE;
		}
        
		
		if((fRN<fFaultLimit)||(fRP<fFaultLimit))
		{
			gs_InsulationDetectMng.ucInsulationState = eInsulation_End;
			CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_End;
		}
		else if((fRN<fAlarmLimit)||(fRP<fAlarmLimit))
		{
			gs_InsulationDetectMng.ucInsulationState = eInsulation_End;
			CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_End;
		}
		else
		{
			gs_InsulationDetectMng.ucInsulationState = eInsulation_End;
			CtrlCmdFeedback_56[CtrlFeedback] = eInsulation_End;
		}

		//保存上一次的计算值
		gs_InsulationDetectMng.fP2GResistanceLast = gs_InsulationDetectMng.fP2GResistance;
		gs_InsulationDetectMng.fN2GResistanceLast = gs_InsulationDetectMng.fN2GResistance;


		gs_InsulationDetectMng.fP2GResistance = fRP;
		gs_InsulationDetectMng.fN2GResistance = fRN;

	}

	SEGGER_RTT_printf(0, "[Insu STEP3] >>> N2G_R=%d P2G_R=%d State=%d\r\n",
		(int)gs_InsulationDetectMng.fN2GResistance, (int)gs_InsulationDetectMng.fP2GResistance,
		gs_InsulationDetectMng.ucInsulationState);
	InPutSystemDataArray(BatPeResistance,(s16)(gs_InsulationDetectMng.fP2GResistance));//正极对pe绝缘输入
	InPutSystemDataArray(GndPeResistance,(s16)(gs_InsulationDetectMng.fN2GResistance));//负极对pe绝缘输入
	return TRUE;
}
static void Insulation_GetN2GandP2GVoltage(void)
{
	float ftemp = 0;

#ifdef INSULATION_TEST
	/* 电压注入hook: 测试时使用注入值替代实际采样 */
	if (Test_IsInjectEnabled())
	{
		float bat_temp, n2g_temp;
		Test_GetInjectedVoltage(&bat_temp, &n2g_temp);
		ftemp = n2g_temp;
		gs_InsulationDetectMng.fBatVoltage = bat_temp;  /* 同步更新母线电压 */
	}
	else
	{
		ftemp = GetfAdjustPeGndVolt();
	}
#endif
	ftemp = GetfAdjustPeGndVolt();
	gs_InsulationDetectMng.fN2GVoltage = ftemp;
	gs_InsulationDetectMng.fP2GVoltage = gs_InsulationDetectMng.fBatVoltage - ftemp;
    
}

static UINT8 Insulation_CheckN2GVoltStable(void)
{
	INT8U u8RTN = FALSE;
	float fDelta1 = 0;

	fDelta1 = gs_InsulationDetectMng.fN2GVoltage - gs_InsulationDetectMng.fN2GVoltageBack;
	if (fDelta1 < 0)
	{
		fDelta1 = -fDelta1;
	}
	if (fDelta1 < mInsulationVoltStableLimit) 
	{
		u8RTN = TRUE;
	}
	else
	{
		gs_InsulationDetectMng.fN2GVoltageBack = gs_InsulationDetectMng.fN2GVoltage;
		u8RTN = FALSE;
	}

	return u8RTN;
}

//记录用于计算的绝缘电阻的正负对地电压
static void Insulation_RecordN2GandP2GVoltage(u8 time)
{
	if(time < ALL_GET_VOLT_TIME)
	{
		gs_InsulationDetectMng.fN2GVoltageCalc[time] = gs_InsulationDetectMng.fN2GVoltage;
		gs_InsulationDetectMng.fP2GVoltageCalc[time] = gs_InsulationDetectMng.fP2GVoltage;
	}

}

static void Insulation_Calculate(double fUN1,double fUP1,double fUN2, double fUP2, double fRN1,double fRP1, double fRN2, double fRP2,double* pfRN,double* pfRP)
{
	double fRate1,fRate2;
	double ftemp,fdata;
	fRate1 = fUP1/fUN1;
	fRate2 = fUP2/fUN2;
	
	if(FABS(fUN1) <= 0.001f)
	{
		fRate1 = 0;
	}
	if((FABS(fUN2) <= 0.001f) || (fUN2 <= 20)) 
	{
		fRate2 = 0;
	}
	 if( (FABS(fRate2) > 1e-6f)
		&& (FABS(fRate1) > 1e-6f))
	{
		ftemp = fRate2*fRP1*fRN2 - fRate1*fRP1*fRN1- fRate1*fRate2*fRN1*fRN2;
		if((ftemp>0.001)||(ftemp<-0.001))
		{
				fdata = (fRate1*fRP1*fRN1*fRN2 - fRate2*fRP1*fRN1*fRN2)/ftemp;
				*pfRN = fdata;
				*pfRP =  fRate2*fRN2*fdata/(fRN2+fdata);
		}
		else
		{
				*pfRN = -1;
				*pfRP = -1;
		}
	}else if(fRate2 == 0 && fRate1!=0)//只有pe对电池负极有绝缘电阻
	{
		fdata = (fRP1*fRN1)/(fRN1*fRate1-fRP1);
		*pfRN = fdata;
		*pfRP = -1;
	}
	else{
				*pfRN = -1;
				*pfRP = -1;
	}
}

#ifdef INSULATION_TEST
static void check_insulation(const char *tag)
{
    u32 *p = (u32 *)&gs_InsulationDetectMng;
    SEGGER_RTT_printf(0, "[%s] BatVolt=0x%08X N2G=0x%08X P2G=0x%08X Mode=%d State=%d\r\n",
        tag,
        *(u32*)&gs_InsulationDetectMng.fBatVoltage,
        *(u32*)&gs_InsulationDetectMng.fN2GVoltage,
        *(u32*)&gs_InsulationDetectMng.fP2GVoltage,
        gs_InsulationDetectMng.ucInsulationMode,
        gs_InsulationDetectMng.ucInsulationState);
}
#endif