
#include  <main.h>
#include "EtherNet.h"
#include <cm_backtrace.h>
#include "SEGGER_RTT.h"
UINT8 u8DebugVal = 0;

static  OS_STK  STK_AppTask[APP_CFG_TASK_START_STK_SIZE];
//OS_STK	STK_DisplayTask[TASK_STK_SIZE_DISPLAY];
OS_STK	STK_UART4Task[TASK_STK_SIZE_UART4];
OS_STK	STK_HuaShangCtrlTask[TASK_STK_SIZE_UART1];
OS_STK	STK_E2PROMTask[TASK_STK_SIZE_EEPROM];
OS_STK	STK_PWMTask[TASK_STK_SIZE_PWM];

OS_STK	STK_CAN1Task[TASK_STK_SIZE_CAN1];
OS_STK	STK_ReadRTCTask[TASK_STK_SIZE_RTC];
OS_STK	STK_TestSelfTask[TASK_STK_SIZE_SELF_TEST];
OS_STK	STK_CAN3Task[TASK_STK_SIZE_CAN3];
OS_STK	STK_CAN3TxTask[TASK_STK_SIZE_CAN3_TX];
OS_STK	STK_USBTask[TASK_STK_SIZE_USB];
OS_STK	STK_ModemTask[TASK_STK_SIZE_MODEM];
OS_STK	STK_HostTask[TASK_STK_SIZE_UART5];
OS_STK	STK_AlmTask[TASK_STK_SIZE_ALARM];
OS_STK	STK_MeasureTask[TASK_STK_SIZE_MEASURE];
OS_STK	STK_CAN2Task[TASK_STK_SIZE_CAN2];
OS_STK  STK_EthernetTask[TASK_STK_SIZE_ETHERNET];
OS_STK	STK_NetZhiXinTask[TASK_STK_NET_ZHIXIN];
OS_STK	STK_FlashTask[TASK_STK_SIZE_FLASH];

static  void  TASK_AppStartProc(void  *p_arg);
static void   TASK_Create(void);

CPU_INT32U  AppProbe_Slider;
float gADCC1Val = 0;
float gADCC2Val = 0;
float gADTestFVal1 = 0;
float gADVoltFVal1 = 0;
float gADTestFVal2 = 0;
float gTestFVal3 = 0;
uint32_t gTestEXID = 0;
UCHAR gOutPutSwitchState =0;
UCHAR gAllRectCtrlState =0;												//预留接口
UCHAR gFanFullCtrlState =0; 
UCHAR gContactorCtrlState =0;
UINT32 u32g_A_OverVoltDelayTimes = 0;
__CHARGE_ARGS gTempChargeArgs;

int ACFILTER_count;
UINT8 ACFILTER_IDX;
float AC_DMAValue[mFIELT_NUM];
NOINIT EV_CHARGE_INFO gEvChargeInfo;
NOINIT __ZTE_FAULT_LOG gZTEFaultLogView[FAULT_LOG_VIEW_NUM + 2];
NOINIT __ST_ZTE_FAULT_LOG_RAM gZTEFaultLog[FAULT_LOG_RAM_NUM];				//中兴故障日志
//0x84d1   MAX_FAULT_LOG_NUM 存储这么多条
NOINIT __ZTE_FAULT_LOG_IDX gZTEFaultLogIdx;
__VIEW_RESULT_INFO gA_Result;
__VIEW_RESULT_INFO gB_Result;
TASK_XXXInfo sTaskArray[TASK_MAX_NUM] =
{
	{ &TASK_RTCProc,		TASK_STK_SIZE_RTC,				PRIO_TASK_START+1,		TASK_START_ID+1,	STK_ReadRTCTask},		//RTC
	{ &TASK_SelfTestProc,	        TASK_STK_SIZE_SELF_TEST,	                PRIO_TASK_START+2,		TASK_START_ID+2,	STK_TestSelfTask},		//自检
	{ &TASK_CAN2Proc,		TASK_STK_SIZE_CAN2,				PRIO_TASK_START+3,		TASK_START_ID+3,	STK_CAN2Task},            
	{ &TASK_CAN1Proc,		TASK_STK_SIZE_CAN1,				PRIO_TASK_START+4,		TASK_START_ID+4,	STK_CAN1Task},            //BMS 
	{ &TASK_CAN3_Proc,		TASK_STK_SIZE_CAN3,				PRIO_TASK_START+5,		TASK_START_ID+5,	STK_CAN3Task},            //模块
	{ &TASK_Ethernet,		TASK_STK_SIZE_ETHERNET,			        PRIO_TASK_START+6,		TASK_START_ID+6,	STK_EthernetTask},	
	{ &TASK_MeasureProc,            TASK_STK_SIZE_MEASURE,			        PRIO_TASK_START+7,		TASK_START_ID+7,	STK_MeasureTask},		//测量
	
	{ &TASK_Uart5Proc,		TASK_STK_SIZE_UART5,			        PRIO_TASK_START+8,		TASK_START_ID+8,	STK_HostTask},			//UART5 电表 等
	{ &TASK_Uart4Proc,		TASK_STK_SIZE_UART4,			        PRIO_TASK_START+9,		TASK_START_ID+9,	STK_UART4Task},			//UART4 2枪 电表
	
	{ &TASK_NET_ZhixinProc,	        TASK_STK_NET_ZHIXIN,		                PRIO_TASK_START+10,		TASK_START_ID+10,	STK_NetZhiXinTask},		//智芯应用层	
	{ &TASK_Uart6Proc,		TASK_STK_SIZE_MODEM,			        PRIO_TASK_START+11,		TASK_START_ID+11,	STK_ModemTask},				//UART6 后台
	{ &TACK_Uart1Proc,		TASK_STK_SIZE_UART1,			        PRIO_TASK_START+12,		TASK_START_ID+12,	STK_HuaShangCtrlTask},		//UART1 刷卡机
	{ &TASK_AlmProc,		TASK_STK_SIZE_ALARM,			        PRIO_TASK_START+13,		TASK_START_ID+13,	STK_AlmTask},				//告警处理
	{ &Task_USB_Proc,		TASK_STK_SIZE_USB,				PRIO_TASK_START+14,		TASK_START_ID+14,	STK_USBTask},
	{ &TAST_FanProc,		TASK_STK_SIZE_PWM,				PRIO_TASK_START+15,		TASK_START_ID+15,	STK_PWMTask},				//FAN
	{ &TASK_E2promProc,		TASK_STK_SIZE_EEPROM,			        PRIO_TASK_START+16,		TASK_START_ID+16,	STK_E2PROMTask},			//E2PROM
	{ &TASK_FlashProc,		TASK_STK_SIZE_FLASH,			        PRIO_TASK_START+17,		TASK_START_ID+17,	STK_FlashTask},				//Flash 
	//{ &TAST_FanProc,		TASK_STK_SIZE_PWM,			        PRIO_TASK_START+14,		TASK_START_ID+14,	STK_PWMTask},				//FAN
	//{ &TASK_E2promProc,		TASK_STK_SIZE_EEPROM,			        PRIO_TASK_START+15,		TASK_START_ID+15,	STK_E2PROMTask},			//E2PROM
	//{ &TASK_FlashProc,		TASK_STK_SIZE_FLASH,			        PRIO_TASK_START+16,		TASK_START_ID+16,	STK_FlashTask},				//Flash 
	//{ &Task_USB_Proc,		TASK_STK_SIZE_USB,				PRIO_TASK_START+17,		TASK_START_ID+17,	STK_USBTask},
	{ &TASK_CAN3_Tx,		TASK_STK_SIZE_CAN3_TX,			        PRIO_TASK_START+18,		TASK_START_ID+18,	STK_CAN3TxTask},
};

/***********************************************************************************************************************
	增加线程	需要更改三个地方：
	1）TASK_MAX_NUM
	2）#define TASK_ID_UART4				13
	3）#define CHECKTASKNUM					14
*******************************************************************************************************************/
void InitChageInfo(void);
void InitParasSramVariables(void);
void InitChargeInfo_E2prom(void);
void InitEVInfo(void);

void FaultLogIdxRefresh(void)
{
		UINT8 ucValueH = dv_E2PROM.fReadE2PROM(FAULT_LOG_NUM_ADDR_H);
		UINT8 ucValueL = dv_E2PROM.fReadE2PROM(FAULT_LOG_NUM_ADDR_L);
		gZTEFaultLogIdx.u16TotalLOGNum = (ucValueH<<8) + ucValueL;
		ucValueH = dv_E2PROM.fReadE2PROM(FAULT_LOG_PST_ADDR_H);
		ucValueL = dv_E2PROM.fReadE2PROM(FAULT_LOG_PST_ADDR_L);
		gZTEFaultLogIdx.u16BytesPst = (ucValueH<<8) + ucValueL;
		return;
}


void HMIKeyClean(void)
{
	UINT8 u8Idx = 0;
	BRUSH_INFO *PBrushInfo = GetgEquipInfo(E_TYPE_BRUSH);
	for (u8Idx = 0; u8Idx < HMI_KEY_MAX_NUM; u8Idx++)
	{
		gEvChargeInfo.u8InputKEY[u8Idx] = '\0';
	}
	gEvChargeInfo.u8InputKEY_Idx = 0;
	//PBrushInfo->u8CARD_Info_Result = 10;
}

void HMIKeyBack(void)
{
	UINT8 u8Idx = 0;
	BRUSH_INFO *PBrushInfo = GetgEquipInfo(E_TYPE_BRUSH);
	if (gEvChargeInfo.u8InputKEY_Idx > 0)
	{
		gEvChargeInfo.u8InputKEY_Idx = gEvChargeInfo.u8InputKEY_Idx -1;
	}

	for (u8Idx = gEvChargeInfo.u8InputKEY_Idx; u8Idx < HMI_KEY_MAX_NUM; u8Idx++)
	{
		gEvChargeInfo.u8InputKEY[u8Idx] = '\0';
	}
}

void InitParasSramVariables(void)
{
	//gEvChargeInfo.u8ONE_BUTTON = FALSE;
	
	//gEvChargeInfo.u8ISLT_Type = 0;//IMIU
	//gEvChargeInfo.u16SysCfgType = 0;
	gEvChargeInfo.u8HasACGun = FALSE;
	gEvChargeInfo.u8TYPE_BAURD = 0;
	gEvChargeInfo.u8IsSupportWIFI = 0;
	
	gEvChargeInfo.fMaxOutCurr = 240;
	gEvChargeInfo.fMaxOutVolt = 1000;
	gEvChargeInfo.fMinOutVolt = 50;
	gEvChargeInfo.fBattVolt_1CoefA = 1;
	gEvChargeInfo.fBattVolt_1CoefB = 0;

	gEvChargeInfo.fBattVolt_2CoefA = 1;
	gEvChargeInfo.fBattVolt_2CoefB = 0;

	gEvChargeInfo.fCC1_1Coef_A = 1;
	gEvChargeInfo.fCC1_1Coef_B = 0;
	gEvChargeInfo.fCC1_2Coef_A = 1;
	gEvChargeInfo.fCC1_2Coef_B = 0;
	gEvChargeInfo.fTP1000_1_CoefK = 1;
	gEvChargeInfo.fTP1000_1_CoefB = 0;

	gEvChargeInfo.fTP1000_2_CoefK = 1;
	gEvChargeInfo.fTP1000_2_CoefB = 0;

	gEvChargeInfo.fSysSetOverVolt = 1100;

	gEvChargeInfo.u8PWMPolarityCover = FALSE;

	gEvChargeInfo.fRectLoadStartTime = 1;
	gEvChargeInfo.u8_A_LOCKType = 0;								//电子枪控制为 广州知崇新能源 电子枪，给电平吸合，不给电平断开。 0 新贵电子枪，需要方向
	gEvChargeInfo.u8_B_LOCKType = 0;
	gEvChargeInfo.u8_C_LOCKType = 0;
	gEvChargeInfo.u8EV_StartTemp = 45;
	gEvChargeInfo.u8EV_Ultra_Temp = 55;
	
	gEvChargeInfo.u32PowerOnTimes = 0;

	int i = 0;
	for ( i = 0; i < EV_STATION_CODE_NUM; i++)
	{
		gEvChargeInfo.u16EVCode[i] = 0;
	}

	//for (i = 0; i < EV_OFF_LINE_PSWD; i++)
	//{
	//	gEvChargeInfo.u16OffLinePSWD[i] = 0;
	//}

	gEvChargeInfo.u32NET_PORT = 4460;
	gEvChargeInfo.u8CommMode = 1;			//0应答模式  1上报模式

	/*环网*/
	gEvChargeInfo.bySelfAddress = 0; //本桩地址/**/
	gEvChargeInfo.nTotalRingStub = 0;//所有环网桩数/**/
	gEvChargeInfo.u8IsRingEnabled = 0;
	/*环网 end*/


}

void InitChageInfo(void)
{					
	UINT8 i;

	gEvChargeInfo.u8Earth_DetectEnable = FALSE;
	gEvChargeInfo.u8TEST_MODE_FLAG = FALSE;
	gEvChargeInfo.u8RcvYdn23Flag = RcvYdn23Flag_Init;
	gEvChargeInfo.u8RcvYdn23Flag_UART1 = RcvYdn23Flag_Init;

	gEvChargeInfo.u8CTRLEx2DOCMD = 0;
	gEvChargeInfo.u8CTRLEx2DOCMDFlag = FALSE;
	gEvChargeInfo.u8CTRLInterDOCMDFlag_2 = FALSE;
	gEvChargeInfo.u8CTRLEx1DOCMDFlag = FALSE;
	gEvChargeInfo.u8CTRLEx1DOCMD = 0;
	gEvChargeInfo.u8CTRLInterDOCMD = 0;
	gEvChargeInfo.u8CTRLInterDOCMDFlag = FALSE;
	gEvChargeInfo.u8CTRLInterDOCMD_B = 0;
	gEvChargeInfo.u8CTRLInterDOCMDFlag_B = FALSE;


	gEvChargeInfo.u8EV_InterDI_COM = 0;
	gEvChargeInfo.u8EV_Ex1DI = 0;
	gEvChargeInfo.u8EV_Ex2DI = 0;
	gEvChargeInfo.u8EV_SpearPst[GUN_NO_A] = SPEAR_NO_ON_LINE;							//0 不在位
	gEvChargeInfo.u8EV_SpearPst[GUN_NO_B] = SPEAR_NO_ON_LINE;							//0 不在位
	gEvChargeInfo.u8EV_InterDO = 0;												//0
	gEvChargeInfo.u8EV_DO9_16  = 0;
	gEvChargeInfo.u16EV_Ex2DO = 0;
	gEvChargeInfo.fEV_InterTemp = 25.0;
	gEvChargeInfo.fEV_Ex2Temp = 25.0;
	gEvChargeInfo.fEV_BattVolt_x[GUN_NO_A] = 699;
	gEvChargeInfo.fEV_BattVolt_x[GUN_NO_B] = 688;
	
	gEvChargeInfo.fBMS_ChargeTime[GUN_NO_A] = 0;
	gEvChargeInfo.fBMS_ChargeTime[GUN_NO_B] = 0;

	gEvChargeInfo.u8Gun_Alm[GUN_NO_A] = 0;
	gEvChargeInfo.u8Gun_Alm[GUN_NO_B] = 0;
	gEvChargeInfo.u8SysAlm = FALSE;
	gEvChargeInfo.u8GunSelect = 0;
	gEvChargeInfo.u8GunSelectTimeOut = 0;

	gEvChargeInfo.u8NetCommAlm = FALSE;
	gEvChargeInfo.u8TelaidianHostCommAlm = FALSE;

	gEvChargeInfo.u8FloatParamUpDataCount = 0;
	gEvChargeInfo.u8CharParamUpDataCount = 0;
	gEvChargeInfo.u8CC1_1Set_A_Flag = FALSE;
	gEvChargeInfo.u8CC1_1Set_B_Flag = FALSE;
	gEvChargeInfo.u8CC1_2Set_A_Flag = FALSE;
	gEvChargeInfo.u8CC1_2Set_B_Flag = FALSE;
	gEvChargeInfo.u8CC1_CH_No = 0;

	gEvChargeInfo.u8NET_Mask = TRUE;

	
	gEvChargeInfo.u8Batt_1VoltSet_A_Flag = FALSE;
	gEvChargeInfo.u8Batt_1VoltSet_B_Flag = FALSE;
	
	
	gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] = STOP_CHARGE_VAL;
	gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] = STOP_CHARGE_VAL;
	gEvChargeInfo.u8IsOptionA = FALSE;
	gEvChargeInfo.u8IsOptionB = FALSE;
	gEvChargeInfo.A_IsParallel = FALSE;
	gEvChargeInfo.B_IsParallel = FALSE;


	
	gEvChargeInfo.u8CLRFaultLog = FALSE;
	gEvChargeInfo.fBattSet_A_Volt = 0;
	gEvChargeInfo.fBattSet_B_Volt = 0;
	gEvChargeInfo.u8Holl_CH_No = 0;
	gEvChargeInfo.u8CC1_CH_No = 0;

	gEvChargeInfo.u8A_NetStartSem = 0;
	gEvChargeInfo.u8B_NetStartSem = 0;

	gEvChargeInfo.fOutEnergyByCal[0] = 0;
	gEvChargeInfo.fOutEnergyByCal[1] = 0;

	gEvChargeInfo.ucPreChgOkFlag[0] = 0;
	gEvChargeInfo.ucPreChgOkFlag[1] = 0;

	//gEvChargeInfo.fOutEnergyByCalc = 0;						//累计计算充入电量
	gEvChargeInfo.u8SPDAlm = FALSE;							//交流防雷器告警
	gEvChargeInfo.u8EPOAlm = FALSE;
	gEvChargeInfo.u8DoorAlm = FALSE;						//门禁告警  打开就不能充电，并且告警。 NC
	gEvChargeInfo.u8DoorAlm_MultiCabinet = FALSE;
	gEvChargeInfo.u8SPDAlm_MultiCabinet = FALSE;
	gEvChargeInfo.u8ElecLock_AAlm = FALSE;					//A 电子锁告警
	gEvChargeInfo.u8ElecLock_BAlm = FALSE;					//B 电子锁告警

	gEvChargeInfo.u8Polarity_A_Alm = FALSE;					//A 极性告警
	gEvChargeInfo.u8Polarity_B_Alm = FALSE;					//B 极性告警

	gEvChargeInfo.fEV_InterTemp = 25;						//IMCU1外接温度

	gEvChargeInfo.u8SysFanALM = 0;							//系统风扇告警！
	gEvChargeInfo.u8SysFanSpeed = 0;						//系统风扇转速
	gEvChargeInfo.u8BrushSem = FALSE;
	gEvChargeInfo.u8A_NoCardStopSem = FALSE;
	gEvChargeInfo.u8B_NoCardStopSem = FALSE;

	gEvChargeInfo.u8_A_TestMode = FALSE;
	gEvChargeInfo.u8_B_TestMode = FALSE;

	gTempChargeArgs.u8ChargeMode = BMS_AUTO;
	gTempChargeArgs.fSOCArgs = 0;
	gTempChargeArgs.fTimeArgs = 0;
	gTempChargeArgs.fEnergeArgs = 0;
	gEvChargeInfo.u8ChargeMode[GUN_NO_A] = BMS_AUTO;
	gEvChargeInfo.u8ChargeMode[GUN_NO_B] = BMS_AUTO;
	gEvChargeInfo.u8ChargeMode[GUN_NO_B] = 0;


	
	HMIKeyClean();
	gEvChargeInfo.u8PSWD_0x70_Flag = FALSE;

	gEvChargeInfo.u8_AutoTestStep = 0;
	gEvChargeInfo.u8_ManualTestStep = 0;

	gEvChargeInfo.ucTimePhase[0] = 0;
	gEvChargeInfo.ucTimePhase[1] = 0;
	gEvChargeInfo.ucTimePhase_InitFlag[0] = TRUE;
	gEvChargeInfo.ucTimePhase_InitFlag[1] = TRUE;
	
	gEvChargeInfo.ucRingCostDelay[0] = 0;
	gEvChargeInfo.ucRingCostDelay[1] = 0;

	

	//{4, "865748"},
	gEvChargeInfo.u16UserPassword[3][0] = '8';
	gEvChargeInfo.u16UserPassword[3][1] = '6';
	gEvChargeInfo.u16UserPassword[3][2] = '5';
	gEvChargeInfo.u16UserPassword[3][3] = '7';
	gEvChargeInfo.u16UserPassword[3][4] = '4';
	gEvChargeInfo.u16UserPassword[3][5] = '8';


	for(i = 0; i < DEBUG_SIG_MAX; i++)
	{
		gEvChargeInfo.u8DebugSig[i] = 0;
	}
	for(i = 0; i < DEBUG_SIG_MAX_2; i++)
	{
		gEvChargeInfo.u8DebugSig_2[i] = 0;
	}

	gEvChargeInfo.u8IMMUAlm = 0;
	
	gEvChargeInfo.u16ACInputContactorDisconCnt = 0;
	gEvChargeInfo.u8ACInputContactorCtrlStat = DO_ON;
	gEvChargeInfo.u8ACInputContactorCloseSampDelay = ACInputContactorCloseSampDelay;
	gEvChargeInfo.u8ACInputContactorCloseAlmDelayFlag = TRUE;

	gEvChargeInfo.u8DebugModeSelectChgSource = 0;

	gEvChargeInfo.u8DCDCPwrOnAlmDelayFlag = TRUE;
	gEvChargeInfo.u8DCDCPwrOnSampDelay = DCDCPwrOnSampDelay;
	gEvChargeInfo.u8DCDCPwrSupplyStat = DO_OFF;


	gEvChargeInfo.fDischargeC10FromEV = 0.1;
	gEvChargeInfo.fDischargeLmtVoltFromEV = 300;
	gEvChargeInfo.fDischargeVoltCoeffStopVsRatedFromEV = 0.9;
	gEvChargeInfo.u8DischargeLmtSocFromEV = 10;


	gEvChargeInfo.u8ACInputContactorCloseAlmDelayFlag_FromMaster = TRUE;
	gEvChargeInfo.u8DCDCPwrOnAlmDelayFlag_FromMaster = TRUE;

	gEvChargeInfo.u8ACInputContactorCtrlStat_FromMaster = DO_ON;
	gEvChargeInfo.u8DCDCPwrSupplyStat_FromMaster = DO_OFF;
	gEvChargeInfo.u8BusVoltageError = 0;
	gEvChargeInfo.u8_EvMaxVoltLevel[0] = 0;
	gEvChargeInfo.u8_EvMaxVoltLevel[1] = 0;

	gEvChargeInfo.u8QueryMpptNumDelay = 0;
	gEvChargeInfo.u8QueryMpptNumInterval = 5;

	gEvChargeInfo.u8GetSingleInfoDelay = 0;
	gEvChargeInfo.u8GetSingleInfoInterval = 1;
	gEvChargeInfo.u8GetABUInfoDelay = 0;
	gEvChargeInfo.u8GetABUInfoInterval = 2;
	gEvChargeInfo.u8SetAdjVoltCurrDelay = 0;
	gEvChargeInfo.u8SetAdjVoltCurrInterval = 1;//快速响应电压电流
	gEvChargeInfo.u8SetOnOffDelay = 0;
	gEvChargeInfo.u8SetOnOffInterval = 5;
	gEvChargeInfo.u8GetVoltCurrDelay = 0;
	gEvChargeInfo.u8GetVoltCurrInterval = 3;
	gEvChargeInfo.u8GetPowerInfoDelay = 0;
	gEvChargeInfo.u8GetPowerInfoInterval = 3;
	gEvChargeInfo.u8GetSingleEveryTimeDelay = 0;
	gEvChargeInfo.u8GetSingleEveryTimeInterval = 1;
	gEvChargeInfo.u8SetWorkModeDelay = 0;
	gEvChargeInfo.u8SetWorkModeInterval = 5;
	gEvChargeInfo.u8SetWorkModeParaDelay = 0;
	gEvChargeInfo.u8SetWorkModeParaInterval = 5;
}


void InitChargeInfo_E2prom(void)
{	
	UINT16 uIdx = 0;
	__XXX_UNION_VALUE unVal;
	
	ExGetSigInfo(E_TYPE_SYS, ID_u8GROUP_Detect_Enbl, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_SYS, ID_fCC1_1Coef_A, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fCC1_1Coef_B, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fCC1_2Coef_A, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fCC1_2Coef_B, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fSysSetOverVolt, &unVal, NULL, BY_E2PROM);	
	ExGetSigInfo(E_TYPE_SYS, ID_fRectLoadStartTime, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fMaxOutCurr, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fMaxOutVolt, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fMinOutVolt, &unVal, NULL, BY_E2PROM);
	OSTimeDly(5);
	ExGetSigInfo(E_TYPE_SYS, ID_fBattVolt_1CoefA, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fBattVolt_1CoefB, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fBattVolt_2CoefA, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fBattVolt_2CoefB, &unVal, NULL, BY_E2PROM);

	OSTimeDly(5);
	ExGetSigInfo(E_TYPE_SYS, ID_TP1000_1_CoefA, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_TP1000_1_CoefB, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_TP1000_2_CoefA, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_TP1000_2_CoefB, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_SYS, ID_fSetLimitMaxOutCurr_1, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fSetLimitMaxOutCurr_2, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8PolarityAlmMask_1, &unVal, NULL, BY_E2PROM);
	OSTimeDly(5);
	ExGetSigInfo(E_TYPE_SYS, ID_u8PolarityAlmMask_2, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fMaxOffLineTime, &unVal, NULL, BY_E2PROM);

	unVal.chtemp[0] = TRUE;										//压差小于10V使能
	ExSetSigInfo(E_TYPE_SYS, ID_u8MCCBOnCheckEnable_1, unVal, NULL, BY_E2PROM);
	OSTimeDly(2);

	ExGetSigInfo(E_TYPE_SYS, ID_u8MCCBOnCheckEnable_1, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8MCCBOnCheckEnable_2, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8OutOverVoltEnable_1, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8OutOverVoltEnable_2, &unVal, NULL, BY_E2PROM);
	OSTimeDly(5);
	ExGetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_1, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_2, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8PWMPolarityCover, &unVal, NULL, BY_E2PROM);

	OSTimeDly(5);
	ExGetSigInfo(E_TYPE_SYS, ID_u8EPOAlmVltLEVEL, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8LowVoltAlmVltLEVEL, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8NLINEAlmVltLEVEL, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8BrushFactorType, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8System_Start_Temp, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8System_Ultra_Temp, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8NET_OffLine_Charge_Enable, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u32PowerOnTimes, &unVal, NULL, BY_E2PROM);
	unVal.u32Value += 1;//上电开机次数加1
	ExSetSigInfo(E_TYPE_SYS, ID_u32PowerOnTimes, unVal, NULL, BY_E2PROM);
	OSTimeDly(5);
	ExGetSigInfo(E_TYPE_SYS, ID_u8SPDAlmMask, &unVal, NULL, BY_E2PROM);

	//ExGetSigInfo(E_TYPE_SYS, ID_ParallelMode, &unVal, NULL, BY_E2PROM);

	OSTimeDly(5);
	for (uIdx = 0; uIdx < 6; uIdx++)
	{
		ExGetSigInfo(E_TYPE_SYS, ID_u8IP_1 + uIdx, &unVal, NULL, BY_E2PROM);
		ExGetSigInfo(E_TYPE_SYS, ID_u8Mac_1 + uIdx, &unVal, NULL, BY_E2PROM);
		ExGetSigInfo(E_TYPE_SYS, ID_u8Gateway_1 + uIdx, &unVal, NULL, BY_E2PROM);
		ExGetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_1 + uIdx, &unVal, NULL, BY_E2PROM);
		ExGetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_1 + uIdx, &unVal, NULL, BY_E2PROM);
	}
	ExGetSigInfo(E_TYPE_SYS, ID_u32NET_PORT, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u32NET_PORT = unVal.u32Value;

	ExGetSigInfo(E_TYPE_SYS, ID_NetFrameHead_NUM, &unVal, NULL, BY_E2PROM);
	gs_EthAppCommMng.u16NetFrameHead = unVal.u32Value;
	OSTimeDly(2);
	//for ( uIdx = 0; uIdx < EV_STATION_CODE_NUM; uIdx++)
	for ( uIdx = 0; uIdx < 60; uIdx++)
	{
		ExGetSigInfo(E_TYPE_SYS, ID_u8StationCode_No1 + uIdx, &unVal, NULL, BY_E2PROM);
	}

	for ( uIdx = 0; uIdx < 6; uIdx++)
	{
		ExGetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1 + uIdx, &unVal, NULL, BY_E2PROM);
		ExGetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1 + uIdx, &unVal, NULL, BY_E2PROM);
		ExGetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1 + uIdx, &unVal, NULL, BY_E2PROM);
	}

	for (uIdx = 0; uIdx < 4; uIdx++)
	{
		ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_DNSServerIP_NUM_1 + uIdx, &unVal, NULL, BY_E2PROM);
	}
	OSTimeDly(2);
	ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_RemoteServerIsDomainName_NUM, &unVal, NULL, BY_E2PROM);
	for (uIdx = 0; uIdx < 32; uIdx++)
	{
		ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_RemoteServerDomainName_NUM_1 + uIdx, &unVal, NULL, BY_E2PROM);
	}
	ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_MASK_NET_NUM, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8BRUSH_ALM_Mask, &unVal, NULL, BY_E2PROM);
	OSTimeDly(2);

	ExGetSigInfo(E_TYPE_SYS, ID_u8IsRingEnabled, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_SYS, ID_bySelfAddress, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_nTotalRingStub, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_AllRingsPwrLmt, &unVal, NULL, BY_E2PROM);

	OSTimeDly(2);

	ExGetSigInfo(E_TYPE_SYS, ID_fContactorShortThreshold, &unVal, NULL, BY_E2PROM);
	

	ExGetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed, &unVal, NULL, BY_E2PROM);


	ExGetSigInfo(E_TYPE_SYS, ID_WH_TYPE_BAURD, &unVal, NULL, BY_E2PROM);

	OSTimeDly(5);


	for(uIdx = 0; uIdx < 3; uIdx++)
	{
		if((gEvChargeInfo.u16UserPassword[uIdx][0] == 0)
			|| (gEvChargeInfo.u16UserPassword[uIdx][0] == 0xff))
		{
			gEvChargeInfo.u16UserPassword[uIdx][0] = '1';
			gEvChargeInfo.u16UserPassword[uIdx][1] = '5';
			gEvChargeInfo.u16UserPassword[uIdx][2] = '1';
			gEvChargeInfo.u16UserPassword[uIdx][3] = '7';
			gEvChargeInfo.u16UserPassword[uIdx][4] = '1';
			gEvChargeInfo.u16UserPassword[uIdx][5] = '9';
		}
	}
	

	if((gEvChargeInfo.u8FanMaxSpeed > 100) || (gEvChargeInfo.u8FanMaxSpeed < 1))
	{
		gEvChargeInfo.u8FanMaxSpeed = 100;
	}

	ExGetSigInfo(E_TYPE_SYS, ID_fDCDCInputAlowMaxVolt, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_SYS, ID_u8DischargeLmtSocFromEV, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fDischargeC10FromEV, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fDischargeLmtVoltFromEV, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_fDischargeVoltCoeffStopVsRatedFromEV, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8ACInputContactorOpenDelay, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_SYS, ID_U1_BYTES_NUM, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_U2_BYTES_NUM, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_SYS, ID_u8LeakageSensorEn, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8ModuleType, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8TaskIsAcDispatchMode, &unVal, NULL, BY_E2PROM);

	for(uIdx = 0; uIdx < 12; uIdx++)
	{
		ExGetSigInfo(E_TYPE_SYS, ID_u8GRP1_MODULE_NUM+uIdx, &unVal, NULL, BY_E2PROM);
	}

	ExGetSigInfo(E_TYPE_SYS, ID_u8ACGridInputBreak_En, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8ACLoadOutputBreak_En, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8BattGrp1AlowChgOrDischgDI_En, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8BattGrp2AlowChgOrDischgDI_En, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8BattGrp3AlowChgOrDischgDI_En, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_SYS, ID_u8BattGrp4AlowChgOrDischgDI_En, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 0, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[1] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 1, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[0] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 2, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[1] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 3, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[0] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 4, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[1] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 5, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[0] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 6, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[1] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 7, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[0] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 8, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[1] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 9, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[0] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 10, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[1] = unVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 11, &unVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[0] = unVal.chtemp[0];

	if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU1Exist == 1)
	{
		gIMIU_Info_UART4[0].u8JYMAlmStatMask = FALSE;
	}
	else
	{
		gIMIU_Info_UART4[0].u8JYMAlmStatMask = TRUE;
	}
	if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU2Exist == 1)
	{
		gIMIU_Info_UART4[1].u8JYMAlmStatMask = FALSE;
	}
	else
	{
		gIMIU_Info_UART4[1].u8JYMAlmStatMask = TRUE;
	}
	if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU3Exist == 1)
	{
		gIMIU_Info_UART4[2].u8JYMAlmStatMask = FALSE;
	}
	else
	{
		gIMIU_Info_UART4[2].u8JYMAlmStatMask = TRUE;
	}
	if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU4Exist == 1)
	{
		gIMIU_Info_UART4[3].u8JYMAlmStatMask = FALSE;
	}
	else
	{
		gIMIU_Info_UART4[3].u8JYMAlmStatMask = TRUE;
	}

	if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU5Exist == 1)
	{
		gIMIU_Info_UART5[0].u8JYMAlmStatMask = FALSE;
	}
	else
	{
		gIMIU_Info_UART5[0].u8JYMAlmStatMask = TRUE;
	}
	if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU6Exist == 1)
	{
		gIMIU_Info_UART5[1].u8JYMAlmStatMask = FALSE;
	}
	else
	{
		gIMIU_Info_UART5[1].u8JYMAlmStatMask = TRUE;
	}
	if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU7Exist == 1)
	{
		gIMIU_Info_UART5[2].u8JYMAlmStatMask = FALSE;
	}
	else
	{
		gIMIU_Info_UART5[2].u8JYMAlmStatMask = TRUE;
	}
	if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU8Exist == 1)
	{
		gIMIU_Info_UART5[3].u8JYMAlmStatMask = FALSE;
	}
	else
	{
		gIMIU_Info_UART5[3].u8JYMAlmStatMask = TRUE;
	}

	

	gEvChargeInfo.u8IP_1[0] = 192;
	gEvChargeInfo.u8IP_1[1] = 168;
	gEvChargeInfo.u8IP_1[2] = 100;
	gEvChargeInfo.u8IP_1[3] = 200;
	gEvChargeInfo.u8Gateway[0] = 192;
	gEvChargeInfo.u8Gateway[1] = 168;
	gEvChargeInfo.u8Gateway[2] = 100;
	gEvChargeInfo.u8Gateway[3] = 100;	
	

	gEvChargeInfo.u8Subnetmask[0] = 255;
	gEvChargeInfo.u8Subnetmask[1] = 255;
	gEvChargeInfo.u8Subnetmask[2] = 255;
	gEvChargeInfo.u8Subnetmask[3] = 0;	


	gEvChargeInfo.u8PowerStarIP[0] = 192;
	gEvChargeInfo.u8PowerStarIP[1] = 168;
	gEvChargeInfo.u8PowerStarIP[2] = 100;
	gEvChargeInfo.u8PowerStarIP[3] = 100;
	gEvChargeInfo.u8NET_Mask = FALSE;

	gEvChargeInfo.u32NET_PORT = 6690;

	gEvChargeInfo.u8DHCPClientEn = 0;


}

void InitEVInfo(void)
{
	
	InitBrushInfo();

	InitIMIUInfo_Uart5();
	InitIMIUInfo_Uart4();

	InitWHInfo();
	

	InitChageInfo();

	
	__STOP_CHARGE_CMD();											//开机容错  停止充电！

	InitIMIU_Uart4_E2prom();
	InitIMIU_Uart5_E2prom();

	InitWH_E2prom();

	InitChargeInfo_E2prom();

	gNET_TCP_Info.iNetCreateSocketFlag = FALSE;						//目的是否 是用于 WAIT ARP 总线数据！

	gZTEFaultLogIdx.u16TotalLOGNum = 0;
	gZTEFaultLogIdx.u16BytesPst = 0;

	gZTEFaultLogIdx.u16ViewLogNumPerFive = 1;

	gAlmDeal.uBrushCommAlm = FALSE;
	gAlmDeal.u8PolarityAlm = FALSE;
	gAlmDeal.u8WHCommAlm = FALSE;
	gAlmDeal.u8SPDAlm = FALSE;
	gAlmDeal.u8DoorAlm = FALSE;
	
	gAlmDeal.u8ElecLock_AAlm = FALSE;
	gAlmDeal.u8BUSVoltAbortALM = FALSE;
	gAlmDeal.u8RelayAdhesionALM = FALSE;
	gAlmDeal.u8BMSAbortStopALM = FALSE;
	gAlmDeal.u8CellVoltAbortStopALM = FALSE;
	gAlmDeal.uALMRectSTOP = FALSE;
	gAlmDeal.u8InsltCommAlm = FALSE;
	gAlmDeal.u8CC1ConnectALM = FALSE;
	gAlmDeal.u8EPOALM = FALSE;
		
	dv_Output.fOpenBeep();
	OSTimeDly(500);
	dv_Output.fCloseBeep();
	gEvChargeInfo.i32_A_CreateRecordDelay = INVALID_DELAY;
	gEvChargeInfo.i32_B_CreateRecordDelay = INVALID_DELAY;
	//gEvChargeInfo.u8ManualMode = FALSE;
	

	gEvChargeInfo.u8RectifierUpgrateMode = FALSE;
	gEvChargeInfo.u8TP1000_Chnl_No = 1;
	gEvChargeInfo.u8CTL_TP1000_Pioint_1_Flag = FALSE;
	gEvChargeInfo.u8CTL_TP1000_Pioint_2_Flag = FALSE;
	
	gEvChargeInfo.u32_UART6_DogTimer = 0;

	gEvChargeInfo.u8MeterBaud = 0;
}


/************************************************/
static void  TASK_Create(void)
{
	UINT8 uIdx;
	static const TASK_XXXInfo *sTaskPtr;
	for(uIdx = 0; uIdx < TASK_MAX_NUM ; uIdx++)
	{
		sTaskPtr =  &sTaskArray[uIdx];
		OSTaskCreateExt(sTaskPtr->fTaskProc,
			(void*)0,
			(void*)&(sTaskPtr->StkPtr[sTaskPtr->u16Stk_Size - 1]),
			sTaskPtr->u8Priority,
			sTaskPtr->u8Id,
			(void *)&(sTaskPtr->StkPtr[0]),
			sTaskPtr->u16Stk_Size,
			(void*)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	}
}

static  void  TASK_AppStartProc(void *p_arg)
{
	(void)p_arg;
	BSP_IrqDisbl();
	BSP_Init();
	BSP_Tick_Init();
	
	WTDOG_FEED();

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
	App_SerialInit();
#endif
	
	gEvChargeInfo.u8_Clean_U1_Flash = FALSE;
	gEvChargeInfo.u8_Clean_U2_Flash = FALSE;
	gFlashOptInfo.u8CleanBulkFlag = FALSE;
	gFlashOptInfo.u8CleanRecordsFlag = FALSE;
	gEvChargeInfo.u8RectUpgradeFlag = FALSE;

	gEvChargeInfo.u8ManualClearPCSFaltCmd = 0;

	InitOBJ();
	WTDOG_FEED();
	
	InitHIHardware();
	WTDOG_FEED();
	
	start();
	WTDOG_FEED();
	
	InitParasSramVariables();
	WTDOG_FEED();

	InitRunArgs();
	WTDOG_FEED();
	
	InitMutex();
	WTDOG_FEED();
	
	AppProbe_Slider = 200u;

	WTDOG_FEED();
	BSP_IrqEnable();

	WTDOG_FEED();
	InitEVInfo();

	WTDOG_FEED();
	OSTimeDly(5);

	TASK_Create();

	WTDOG_FEED();

	//掉电充电记录恢复处理
	EepromRecord_Init();

	WTDOG_FEED();


	__ENERGY_BATT_CHG_AC_ON();
	__ACLOAD_MCCB_ON();
	
	OSTimeDly(1000);
	OSTimeDly(1000);

	
	gEvChargeInfo.nTotalRingStub = 4;
	gEvChargeInfo.u8IsRingEnabled = TRUE;

	g_RingAllInfo.emRingMergeMode = RING_MODE_ONECAR_MULTIGUN;

extern void InitDebugInfo(void);
	InitDebugInfo();
#ifdef	Debug_Print_En	
	UINT8 u8Idx = 0;
	UINT8* p = "POWER ON, VERSION is!!";
	memset(u8gTempDebugBuf_System, 0, DEBUG_BUF_BYTES_MAX_NUM);
	u8Idx += sprintf((char*)u8gTempDebugBuf_System,"%s",p);
//	WriteDebugInfo(u8gTempDebugBuf_System, u8Idx);
#endif	

	while (DEF_TRUE) 
	{
		gSelfTest.fSetTaskID(TASK_ID_START);

		//中断出错，重新初始化。
		if (1 == dv_Uart4.u8NeedReInit)
		{
			dv_Uart4.fInitUart(m9600, UARTX_PARITY_EVEN);
			dv_Uart4.fEnableRx();										//使能收中断
			dv_Uart4.u8NeedReInit = 0;
		}

		//中断出错，重新初始化。
		if (1 == dv_Uart5.u8NeedReInit)
		{
			dv_Uart5.fInitUart(m9600, UARTX_PARITY_EVEN);
			dv_Uart5.fEnableRx();										//使能收中断
			dv_Uart5.u8NeedReInit = 0;
		}

		OSTimeDlyHMSM(0u, 0u, 0u, (AppProbe_Slider));
	}
}


int main(void)
{
	unsigned char uctemp;
	uctemp = sizeof(__CHARGE_RECORD);
	UNUSED(uctemp);
//	cm_backtrace_init("IMCU","A02","3.11");
	CPU_Init();
	Mem_Init();
	Math_Init();
        
	OSInit();
	
	OSTaskCreateExt((void (*)(void *))TASK_AppStartProc,                                                           // Create the start task
			(void *) 0,
			(OS_STK *)&STK_AppTask[APP_CFG_TASK_START_STK_SIZE - 1],
			(INT8U) APP_CFG_TASK_START_PRIO,
			(INT16U) APP_CFG_TASK_START_PRIO,
			(OS_STK*)&STK_AppTask[0],
			(INT32U) APP_CFG_TASK_START_STK_SIZE,
			(void*) 0,
			(INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
        
	OSStart();// Start multitasking (i.e. give control to uC/OS-III).

	while (DEF_ON)
	{
		// Should Never Get Here.
	};
}



/***************************************************************************************
	可以把512条充电记录理解为一个滑动窗口（占两个扇区）
	在这如下三个扇区里滚动
	RECORD_SECTION_MAX_NUM * PAGE_NUM_PER_SECTION
	u8UnResponseIdx  0标识已经上传   1标识未上传！
				______________  
				|	      |
				|_____________|
i16CurrPageIdx----->|           |
				|_____________|
				|	      |
				|_____________|


	??è?1ì?¨?÷òy????μ?éè??o?  oí ??ò?á?
***************************************************************************************/
