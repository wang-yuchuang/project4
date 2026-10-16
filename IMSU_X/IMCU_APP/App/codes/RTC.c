
#include "main.h"
//#include "TelaidianProto.h"
#include "Uart6_new.h"
#include "EtherNet.h"
#include "Upgrade.h"
#include "secondary_upgrade.h"
#include "base_def.h"
#include "can_ring.h"
#include "ABU.h"
coTime HeartBeat_Timer;
#define mMs_X(x) (x)
extern INT32 ReadTime_Try(void);
//初始化GMT时间
static void InitGMTTime(void)
{
	struct tm * ptmNow;
	ptmNow = &gRTCData.RTCset;
	ptmNow->tm_year -= 1900;
	ptmNow->tm_mon -= 1;
	dv_RTC.fSetGMTSeconds(mktime_r(ptmNow));
}
//刷新GMT时间
static void  RefreshGMTtime(struct tm * ptmNow)
{
	gRTCData.refresh = TRUE;
	gRTCData.GMTtime.tm_sec = ptmNow->tm_sec;
	gRTCData.GMTtime.tm_min = ptmNow->tm_min;
	gRTCData.GMTtime.tm_hour = ptmNow->tm_hour;
	gRTCData.GMTtime.tm_mday = ptmNow->tm_mday;
	gRTCData.GMTtime.tm_mon = ptmNow->tm_mon + 1;
	gRTCData.GMTtime.tm_year = ptmNow->tm_year + 1900;
	gRTCData.GMTtime.tm_wday = ptmNow->tm_wday;
	gRTCData.GMTtime.tm_yday = ptmNow->tm_yday;
	gRTCData.GMTtime.tm_isdst = ptmNow->tm_isdst;
}
//刷新本地时间
static void  RefreshLocalTime(struct tm * ptmNow)
{
	gRTCData.localtime.tm_sec = ptmNow->tm_sec;
	gRTCData.localtime.tm_min = ptmNow->tm_min;
	gRTCData.localtime.tm_hour = ptmNow->tm_hour;
	gRTCData.localtime.tm_mday = ptmNow->tm_mday;
	gRTCData.localtime.tm_mon = ptmNow->tm_mon + 1;
	gRTCData.localtime.tm_year = ptmNow->tm_year + 1900;
	gRTCData.localtime.tm_wday = ptmNow->tm_wday;
	gRTCData.localtime.tm_yday = ptmNow->tm_yday;
	gRTCData.localtime.tm_isdst = ptmNow->tm_isdst;
}

//读取本地时间
static void  GetLocalTime(struct tm * tmNow)
{
	tmNow->tm_sec = gRTCData.localtime.tm_sec;
	tmNow->tm_min = gRTCData.localtime.tm_min;
	tmNow->tm_hour = gRTCData.localtime.tm_hour;
	tmNow->tm_mday = gRTCData.localtime.tm_mday;
	tmNow->tm_mon = gRTCData.localtime.tm_mon;
	tmNow->tm_year = gRTCData.localtime.tm_year;
	return;
}

//设定本地时间
static void  SetLocalTime(struct tm * tmNow)
{
	gRTCData.RTCset.tm_sec = tmNow->tm_sec;
	gRTCData.RTCset.tm_min = tmNow->tm_min;
	gRTCData.RTCset.tm_hour = tmNow->tm_hour;
	gRTCData.RTCset.tm_mday = tmNow->tm_mday;
	gRTCData.RTCset.tm_mon = tmNow->tm_mon;
	gRTCData.RTCset.tm_year = tmNow->tm_year;
	gRTCData.settimezone = gRTCData.localtimezone;
	gRTCData.set = TRUE;
}

//获取本地时区
static float  GetLocalTimeZone(void)
{
	GetFloatSetData(ADDR_LocalTimeZone, &gRTCData.localtimezone);
	gRTCData.localtimezone = 480;
	return  gRTCData.localtimezone;
}

//设定时区
static void  SetLocalTimeZone(void)
{
	gRTCData.localtimezone = 480;
	SetFloatSetData(ADDR_LocalTimeZone, &gRTCData.localtimezone);
}
//设本地时间
static void  SetGMTTime(struct tm * ptmNow)
{
	gRTCData.set = FALSE;
	gRTCData.refresh = TRUE;
	gRTCData.localtimezone = gRTCData.settimezone;
	ptmNow = &gRTCData.RTCset;
	ptmNow->tm_year -= 1900;
	ptmNow->tm_mon -= 1;
	dv_RTC.fSetGMTSeconds(ULONG_ADD_SLONG( mktime_r(ptmNow), -(long)(gRTCData.localtimezone * MIN_SECONDS)));
}

//获取GMT 秒
static UCHAR  GetGMTOldSecond(void)
{
  return  (UCHAR)gRTCData.GMTtime.tm_sec;
}
//获取驱动GMT秒
static time_t  GetDvGMTSeconds(void)
{
	gRTCData.RTCGMTSec = dv_RTC.fGetGMTSeconds();//get GMT seconds
	return  gRTCData.RTCGMTSec;
}

//GMT秒换算到本地秒
static time_t  GetSecondsGMT2LOCAL(void)
{
  return  ULONG_SUB_SLONG( gRTCData.RTCGMTSec, -(long)(gRTCData.localtimezone * MIN_SECONDS));
}

//初始化RTC
static void  DataInitRTC(void)
{
	struct tm  tmNow;
	time_t now;
	dv_RTC.fReadTime(1);
	now = gRTCData.fGetDvGMTSeconds();//get GMT seconds
	gRTCData.fGetLocalTimeZone();//Get_LocalTimeZone();
	gmtime_r(&now, &tmNow);
	gRTCData.fRefreshGMTTime(&tmNow);//RefreshGMTtime(ptmNow);
	now = gRTCData.fGetSecondsGMT2LOCAL();
	gmtime_r(&now, &tmNow);
	gRTCData.fRefreshLocalTime(&tmNow);//RefreshLocalTime(ptmNow);
}

static void AddSecondsToTimenow(UINT32 ui32Seconds)
{
	dv_RTC.RTC_Counter += ui32Seconds;
	gmtime_r((const time_t *)(&dv_RTC.RTC_Counter), &dv_RTC.tmNow);
}

RTCData_Type gRTCData =
{
	0,
	{0,0,0,1,1,2009,0,0,0},
	{0,0,8,1,1,2009,0,0,0},
	480,
	0,
	{0,0,0,1,1,2009,0,0,0},
	480,
	0,
	0,
	DataInitRTC,
	GetLocalTime,
	SetLocalTime,
	RefreshLocalTime,
	GetLocalTimeZone,
	SetLocalTimeZone,
	InitGMTTime,
	RefreshGMTtime,
	SetGMTTime,
	GetGMTOldSecond,
	GetDvGMTSeconds,
	GetSecondsGMT2LOCAL,
};



/**********************************************************************************************
	RTC 时钟处理线程
**********************************************************************************************/
void TASK_RTCProc (void* pdata)
{
	struct tm  tmNow;
	time_t now;
	UINT8 i;
	//INT32 in32RTC_Counter = 0;
	//INT32 in32RTC_CounterBak = 0;
	UINT8  isRefresh = 0;
	static UCHAR  s_Sec = 0, s_Min = 0;
	UNUSED(pdata);
	__XXX_UNION_VALUE unVal;
	ExGetSigInfo(E_TYPE_SYS, ID_u32PowerOnTimes, &unVal, NULL, BY_E2PROM);
	now = gRTCData.fGetSecondsGMT2LOCAL();
	if(((unVal.u32Value)%2) == 0)
	{ 	 
		dv_E2PROM.fWriteE2PROM(E2PROMRPowerON_2_Time, (0xff&(now>>24)));
		dv_E2PROM.fWriteE2PROM(E2PROMRPowerON_2_Time+1, (0xff&(now>>16)));
		dv_E2PROM.fWriteE2PROM(E2PROMRPowerON_2_Time+2, (0xff&(now>>8)));
		dv_E2PROM.fWriteE2PROM(E2PROMRPowerON_2_Time+3, (0xff&(now)));
	}
	else
	{
		dv_E2PROM.fWriteE2PROM(E2PROMRPowerON_1_Time, (0xff&(now>>24)));
		dv_E2PROM.fWriteE2PROM(E2PROMRPowerON_1_Time+1, (0xff&(now>>16)));
		dv_E2PROM.fWriteE2PROM(E2PROMRPowerON_1_Time+2, (0xff&(now>>8)));
		dv_E2PROM.fWriteE2PROM(E2PROMRPowerON_1_Time+3, (0xff&(now)));
	}

	for(;;)
	{
		
		if (coTimeout(&HeartBeat_Timer, mMs_X(500u)))                //by lxr01202  达到500ms  心跳翻转电平  PG6   ————>  RUN——LED
		{
			BSP_u1_heartbeat_led_toggle();
			BSP_Alm_LED_ON();                                        //如果超出500ms,则反转RUN_LED电平，使得RUN_LED灭掉，然后告警灯常亮
		}
		//RTC
		isRefresh = 0;
		gSelfTest.fSetTaskID(TASK_ID_RTC);
		gRTCData.fGetLocalTime(&tmNow);
		if ((UCHAR)tmNow.tm_sec != s_Sec)
		{
			if(gEvChargeInfo.u8Power_on_Delay < 255)
			gEvChargeInfo.u8Power_on_Delay++;
			//gEvChargeInfo.u16USBInterValTimer++;
			//gEvChargeInfo.u8USB_AppDogTimer++;
			if(gEvChargeInfo.u32_Delay_2_GUN< 100)
			gEvChargeInfo.u32_Delay_2_GUN++;
			if(gEvChargeInfo.u32_Delay_1_GUN< 100)
			gEvChargeInfo.u32_Delay_1_GUN++;

			gs_UpgradePowerModuleCtrl.u16TimerCount++;

			for(i = 0; i < All_GUN_NUM; i++)
			{
				

				//gEvChargeInfo.u16_MachineStateKeepTimeSec[i]++;
			}
			

			

		
			


						
			

			

			if (gEvChargeInfo.i32Uart6_RCV_YND23_DogCount >= UART_DOG_COUNT_START)
			{
				gEvChargeInfo.i32Uart6_RCV_YND23_DogCount++;
			}

			if(gEvChargeInfo.i32Send_Init_Uart6_DogCount >= UART_DOG_COUNT_START)
			{
				gEvChargeInfo.i32Send_Init_Uart6_DogCount++;
			}

			if (gEvChargeInfo.iACFinishCheckCount >= 1)
			{
				gEvChargeInfo.iACFinishCheckCount++;
			}

			isRefresh = 1;
			s_Sec = (UCHAR)tmNow.tm_sec;

			gEvChargeInfo.iACStartTimeOutCount++;


			if(s_Min != (UCHAR)tmNow.tm_min)
			{	
				s_Min = (UCHAR)tmNow.tm_min;
				gEvChargeInfo.u16FanCotrol++;

				
				gEvChargeInfo.u8USB_AppDogTimer++;
				
				gs_CANComm.u16RectCommPeriodResetDelay++;

				if(gEvChargeInfo.u16ACInputContactorDisconCnt > 0)
				{
					gEvChargeInfo.u16ACInputContactorDisconCnt++;
				}
		
			}
			
			if((gBrushInfo.u8BrushWaitHostConfirmTimeout > 0) && (gBrushInfo.u8BrushWaitHostConfirmTimeout < 255))
			{
				gBrushInfo.u8BrushWaitHostConfirmTimeout++;
			}
			
			if(gEvChargeInfo.u32OffLineTimeCount > 0)
			{
				gEvChargeInfo.u32OffLineTimeCount++;
			}

			if(gs_DisplayCtrl.u8WaitSpearOkTimeout > 0)
			{
				gs_DisplayCtrl.u8WaitSpearOkTimeout--;
			}

			if(gs_DisplayCtrl.u8WaitChgModeSelTimeout > 0)
			{
				gs_DisplayCtrl.u8WaitChgModeSelTimeout--;
			}

			if(gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[0]> 0)
			{
				gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[0]--;
			}
			if(gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[1]> 0)
			{
				gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[1]--;
			}

			if(gBrushInfo.i32BrushInterval > 0)
			{
				gBrushInfo.i32BrushInterval--;
			}

			//gNET_TCP_Info.u32BreakCount++;

			if(gs_DisplayCtrl.u8AdminChgKeyDelay[0] > 0 && gs_DisplayCtrl.u8AdminChgKeyDelay[0] < 255)
			{
				gs_DisplayCtrl.u8AdminChgKeyDelay[0]++;
				/*if(gs_DisplayCtrl.u8AdminChgKeyDelay[0] > ADMIN_CHG_KEY_OVERTIME)
				{
					gs_DisplayCtrl.u8AdminChgKeyDelay[0] = 0;
				}*/
			}
			if(gs_DisplayCtrl.u8AdminChgKeyDelay[1] > 0 && gs_DisplayCtrl.u8AdminChgKeyDelay[1] < 255)
			{
				gs_DisplayCtrl.u8AdminChgKeyDelay[1]++;
				/*if(gs_DisplayCtrl.u8AdminChgKeyDelay[1] > ADMIN_CHG_KEY_OVERTIME)
				{
					gs_DisplayCtrl.u8AdminChgKeyDelay[1] = 0;
				}*/
			}

			if(gs_DisplayCtrl.u8ChkPageFlagTimeOut > 0)
			{
				gs_DisplayCtrl.u8ChkPageFlagTimeOut++;
				if(gs_DisplayCtrl.u8ChkPageFlagTimeOut > CHK_PAGE_FLAG_OVERTIM)
				{
					gs_DisplayCtrl.u8ChkPageFlagTimeOut = 0;
					gs_DisplayCtrl.u8ChkPageFlag = 0;
				}
			}
	
			if(gs_DisplayCtrl.u8PasswordPageOverTimer > 0 && gs_DisplayCtrl.u8PasswordPageOverTimer < 255)
			{
				gs_DisplayCtrl.u8PasswordPageOverTimer++;				
			}

			gs_DisplayCtrl.u16ScreemSaverDelay++;	

			gs_DisplayCtrl.u8SysTimeReflashPeriod++;	

			if(gs_EthAppCommMng.u16HeardBeatCount > 0)
			{
				gs_EthAppCommMng.u16HeardBeatCount--;
			}

			
			if(gEvChargeInfo.u8GunSelectTimeOut > 0)
			{
				gEvChargeInfo.u8GunSelectTimeOut++;
			}

			g_RingAllInfo.u32RingTimeSec++;



			//网络任务 计时处理 start
			if(gNET_TCP_Info.u32BreakCount<65535)
			{
				gNET_TCP_Info.u32BreakCount++;
			}
			if(gNET_TCP_Info.u16ReDHCPCount<65535)
			{
				gNET_TCP_Info.u16ReDHCPCount++;
			}

			

			for(i = 0; i < 2; i++)
			{
				if (gs_EthAppCommMng.i32OrderTimeOUTCount[i] >0)
				{
					gs_EthAppCommMng.i32OrderTimeOUTCount[i]--;
					if(gs_EthAppCommMng.i32OrderTimeOUTCount[i] == 0)
					{
						gs_EthAppCommMng.u8OrderChargeCMD[i] = FALSE;
					}
				}

				
				if(gEvChargeInfo.u16GunNoCurrEndDelay[i] > 0)
				{
					gEvChargeInfo.u16GunNoCurrEndDelay[i]++;
				}

				

// 				if(gs_IMMU2_LCCU_Mng.u8LccuSendBMSInofDelay[i] > 0)
// 				{
// 					gs_IMMU2_LCCU_Mng.u8LccuSendBMSInofDelay[i]--;
// 				}

				
				if(gs_EnergyOutputMng.ConverterOutCtrlReq_Delay[i][ConverterDevType_DCDC] > 0) 
				{
					gs_EnergyOutputMng.ConverterOutCtrlReq_Delay[i][ConverterDevType_DCDC]--;
				}

				if((gs_EnergyOutputMng.u8ConverterNoProectDelay[i] > 0)
					&& (gs_EnergyOutputMng.u8ConverterNoProectDelay[i] < 255))
				{
					gs_EnergyOutputMng.u8ConverterNoProectDelay[i]++;
				}

			}

			if(gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_Delay > 0)
			{
				gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_Delay--;
			}

			if(gs_IMMU2_LCCU_Mng.u8NeedRptSubDevVersion_Delay > 0)
			{
				gs_IMMU2_LCCU_Mng.u8NeedRptSubDevVersion_Delay--;
			}

			if (gs_IMMU2_LCCU_Mng.u8NeedRptABUInfo_Delay > 0)
			{
				gs_IMMU2_LCCU_Mng.u8NeedRptABUInfo_Delay--;
			}

			gs_EthAppCommMng.ui16DHCPReconfigTimeOUT++;

			gEvChargeInfo.i32APP_PipeWTDOG++;
			//End: 网络任务 计时处理
			gs_CANComm.u16AllCommFailResetDelay++;


			//20190722
			/*if(gs_EnergyOutputMng.ParallelRelayDisconnectDelay > 0)
			{
				gs_EnergyOutputMng.ParallelRelayDisconnectDelay--;
				if(gs_EnergyOutputMng.ParallelRelayDisconnectDelay == 0)
				{					
					__PARALLEL_OFF();
				}
			}
			else if(gs_EnergyOutputMng.ParallelRelayConnectDelay > 0)
			{
				gs_EnergyOutputMng.ParallelRelayConnectDelay--;
				if(gs_EnergyOutputMng.ParallelRelayConnectDelay == 0)
				{					
					__PARALLEL_ON();
				}
			}*/

			for(i = 0; i < ENJI_M_S_COMM_RCV_BUF_NUM; i++)
			{
				if(gs_IMMU2_LCCU_Mng.RcvBuf[i].RcvOverTimeCnt > 0)
				{
					gs_IMMU2_LCCU_Mng.RcvBuf[i].RcvOverTimeCnt++;
					if(gs_IMMU2_LCCU_Mng.RcvBuf[i].RcvOverTimeCnt > ENJI_RCV_OVERTIME_MAX)
					{
						gs_IMMU2_LCCU_Mng.RcvBuf[i].RcvOverTimeCnt = 0;
						gs_IMMU2_LCCU_Mng.RcvBuf[i].u16RcvPos = 0;
					}
				}
			}
			

			if(gs_IMMU2_LCCU_Mng.u8MasterRcvdSlaveOverTime < MASTER_SLAVE_COMM_OVER_TIME)
			{
				gs_IMMU2_LCCU_Mng.u8MasterRcvdSlaveOverTime++;
			}

			if(gEvChargeInfo.u8ACInputContactorCtrlStat == DO_ON)
			{
				if(gEvChargeInfo.u8ACInputContactorCloseSampDelay > 0)
				{
					gEvChargeInfo.u8ACInputContactorCloseSampDelay--;
				}
				else
				{
					gEvChargeInfo.u8ACInputContactorCloseAlmDelayFlag = FALSE;
				}
			}

			if(gEvChargeInfo.u8DCDCPwrSupplyStat == DO_ON)
			{
				if(gEvChargeInfo.u8DCDCPwrOnSampDelay > 0)
				{
					gEvChargeInfo.u8DCDCPwrOnSampDelay--;
				}
				else
				{
					gEvChargeInfo.u8DCDCPwrOnAlmDelayFlag = FALSE;
				}
			}

			if(gs_UpGradeCtrl.u8UpgradeNeedReset > 1)
			{
				gs_UpGradeCtrl.u8UpgradeNeedReset--;
			}
			

			for(i = 0; i < LCCU_NUMBER_RING; i++)
			{
				if(g_u8IMMU_LCCU_COMM_Addr != (i+1))
				{
					if(gs_LccuExchangeInfo[i].u8CommAlmDelay > LCCU_EXCHANGE_COMM_ALM_DELAY)
					{
						gs_LccuExchangeInfo[i].u8CommAlmStat = 1;
					}
					else
					{
						gs_LccuExchangeInfo[i].u8CommAlmDelay++;
					}
				}
				else
				{
					gs_LccuExchangeInfo[i].u8CommAlmStat = 0;
				}				
			}

			if((gs_EsSampleInfo.SmokeDI_AlmDelay > 0) && (gs_EsSampleInfo.SmokeDI_AlmDelay < 255))
			{
				gs_EsSampleInfo.SmokeDI_AlmDelay++;
			}
			
			for(i = 0; i < BATT_GRP_MAX_NUM; i++)
			{
				if(gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.u8ExistStat == 1)
				{
					gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.u8CommAlmDelay++;
				}
			}

			if((gs_IMMU2_LCCU_Mng.u8IsTrigger == TRUE) && (gs_IMMU2_LCCU_Mng.u8CANRawSendCount > 0))
			{
				gs_IMMU2_LCCU_Mng.u8CANRawSendCount--;
			}

			if((gs_IMMU2_LCCU_Mng.u8IsTrigger == TRUE) && (gs_IMMU2_LCCU_Mng.u16CANRawSendTimeOUTCount > 0))
			{
				gs_IMMU2_LCCU_Mng.u16CANRawSendTimeOUTCount--;
			}

			if (!gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.ABU1Exist)
			{
				ABU->u8ConfigDelay++;
			}

			//这里为了降低CAN负载率增加延时
			if(gEvChargeInfo.u8QueryMpptNumDelay > 0)
			{
				gEvChargeInfo.u8QueryMpptNumDelay--;
			}

			if(gEvChargeInfo.u8GetSingleInfoDelay > 0)
			{
				gEvChargeInfo.u8GetSingleInfoDelay--;
			}

			if(gEvChargeInfo.u8GetABUInfoDelay > 0)
			{
				gEvChargeInfo.u8GetABUInfoDelay--;
			}

			if(gEvChargeInfo.u8SetAdjVoltCurrDelay > 0)
			{
				gEvChargeInfo.u8SetAdjVoltCurrDelay--;
			}

			if(gEvChargeInfo.u8SetOnOffDelay > 0)
			{
				gEvChargeInfo.u8SetOnOffDelay--;
			}

			if(gEvChargeInfo.u8GetVoltCurrDelay > 0)
			{
				gEvChargeInfo.u8GetVoltCurrDelay--;
			}

			if(gEvChargeInfo.u8GetPowerInfoDelay > 0)
			{
				gEvChargeInfo.u8GetPowerInfoDelay--;
			}

			if(gEvChargeInfo.u8GetSingleEveryTimeDelay > 0)
			{
				gEvChargeInfo.u8GetSingleEveryTimeDelay--;
			}

			if(gEvChargeInfo.u8SetWorkModeDelay > 0)
			{
				gEvChargeInfo.u8SetWorkModeDelay--;
			}

			if(gEvChargeInfo.u8SetWorkModeParaDelay > 0)
			{
				gEvChargeInfo.u8SetWorkModeParaDelay--;
			}

			//手动控制风扇延时
			if(gEvChargeInfo.u8ManualCtrlFANDelay < 255)
			{
				gEvChargeInfo.u8ManualCtrlFANDelay++;
			}

			//IMEU2升级计时
			for(i = 0; i < 2; i++)
			{
				if(g_stImeu2UpgradeInfo[i].u8UpgradeFlag == TRUE)
				{
					if(g_stImeu2UpgradeInfo[i].u16UpdateTimeOutCount < 65535)
					{
						g_stImeu2UpgradeInfo[i].u16UpdateTimeOutCount++;
					}
				}
				else
				{
					g_stImeu2UpgradeInfo[i].u16UpdateTimeOutCount = 0;
				}
			}
		}  //END:  if ((UCHAR)tmNow.tm_sec != s_Sec)


		

		gSelfTest.fSetTaskID(TASK_ID_RTC);
		if (isRefresh && (s_Min == 28) && (s_Sec == 0))
		{
			//in32RTC_Counter = ReadTime_Try();
			////判断时间合理性
			//if (in32RTC_Counter != in32RTC_CounterBak
			//	&& in32RTC_Counter <= GMT_TIME_MAX
			//	&& in32RTC_Counter >= GMT_TIME_MIN
			//	&& in32RTC_Counter != -1)
			//{
			//	in32RTC_CounterBak = in32RTC_Counter;
			//	dv_RTC.fReadTime(0);
			//}
			dv_RTC.fReadTime(0);
		}
		else
		{
			if (gRTCData.ucSecondChangeFlag == mTRUE)
			{
				AddSecondsToTimenow(1);
				gRTCData.ucSecondChangeFlag = mFALSE;
			}
		}
		now = gRTCData.fGetDvGMTSeconds();
		//timezone
		gRTCData.fGetLocalTimeZone();
		gmtime_r(&now, &tmNow);
		if(gRTCData.fGetGMTOldSecond() != tmNow.tm_sec)
		{
			gRTCData.fRefreshGMTTime(&tmNow);
			now = gRTCData.fGetSecondsGMT2LOCAL();
			gmtime_r(&now, &tmNow);
			gRTCData.fRefreshLocalTime(&tmNow);
		}
		gSelfTest.fSetTaskID(TASK_ID_RTC);
		if(gRTCData.set == TRUE)
		{
			gRTCData.fSetGMTTime(&tmNow);
			gRTCData.fSetLocalTimeZone();
		}

extern void AddLogSosVoltCurr(void);
		//AddLogSosVoltCurr();

		OSTimeDly(3);
		gSelfTest.fSetTaskID(TASK_ID_RTC);
	}
}
