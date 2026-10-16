#include "main.h"
#include "Upgrade.h"
#include "HistoryEvent.h"

UINT8 u8IsEVFloatUpData(void);
void UpFloatEVData(void);
UINT8 u8IsEVUint8UpData(void);
void UpUint8EVData(void);
void SaveZTEChargeLog(void);

__ZTE_FAULT_LOG gE2promFaultLog;
#define ABS(a)	(((a) < 0) ? (-(a)) : (a))

static void set_defaultupdate(void)
{
  gs_E2PROM.DefaultUpdate++;
}


static void clear_defaultupdate(void)
{
  if(gs_E2PROM.DefaultUpdate > 1)
  {
    gs_E2PROM.DefaultUpdate = 1;
  }
  else
  {
    gs_E2PROM.DefaultUpdate = 0;
  }
}

static UCHAR is_defaultupdate(void)
{
  if(gs_E2PROM.DefaultUpdate > 0)
  {
    return  TRUE;
  }
  return  FALSE;
}


static void set_setdataupdate(void)
{
  gs_E2PROM.SetDataUpdate++;
}

static void clear_setdataupdate(void)
{
  //gs_E2PROM.SetDataUpdate--;
  if(gs_E2PROM.SetDataUpdate > 1)
  {
    gs_E2PROM.SetDataUpdate = 1;
  }
  else
  {
    gs_E2PROM.SetDataUpdate = 0;
  }
}

static UCHAR is_setdataupdate(void)
{
  if(gs_E2PROM.SetDataUpdate > 0)
  {
    return  TRUE;
  }
  return  FALSE;
}

static void set_batttestupdate(void)
{
  gs_E2PROM.BattTestUpdate++;
}

static void clear_batttestupdate(void)
{
  gs_E2PROM.BattTestUpdate--;
}

static UCHAR is_batttestupdate(void)
{
  if(gs_E2PROM.BattTestUpdate > 0)
    return  TRUE;
  return  FALSE;
}

static void set_statisticupdate(void)
{
	//if(gs_E2PROM.StatisticUpdate < 65535)
		gs_E2PROM.StatisticUpdate++;
}

static void clear_statisticupdate(void)
{
  if(gs_E2PROM.StatisticUpdate > 0)
	  gs_E2PROM.StatisticUpdate--;
}


static UCHAR is_statisticupdate(void)
{
  if(gs_E2PROM.StatisticUpdate > 0)
    return  TRUE;
  return  FALSE;
}

static void set_statistictempupdate(void)
{
	gs_E2PROM.StatisticTempUpdate++;
}

static void clear_statistictempupdate(void)
{
  if(gs_E2PROM.StatisticTempUpdate > 0)
	  gs_E2PROM.StatisticTempUpdate--;
}


static UCHAR is_statistictempupdate(void)
{
  if(gs_E2PROM.StatisticTempUpdate > 0)
    return  TRUE;
  return  FALSE;
}

static void set_battrecordupdate(void)
{
  gs_E2PROM.BattRecordUpdate++;
}

static void clear_battrecordupdate(void)
{
  gs_E2PROM.BattRecordUpdate--;
}


static UCHAR is_battrecordupdate(void)
{
  if(gs_E2PROM.BattRecordUpdate > 0)
    return  TRUE;
  return  FALSE;
}

static UCHAR is_clearsetflag(void)
{
  if(gs_E2PROM.ClearSetFlag > 0)
    return  TRUE;
  return  FALSE;
}

static void clear_clearsetflag(void)
{
  gs_E2PROM.ClearSetFlag = 0;
}

static UCHAR is_cleareventflag(void)
{
  if(gs_E2PROM.ClearEventFlag > 0)
    return  TRUE;
  return  FALSE;
}

static void clear_cleareventflag(void)
{
  gs_E2PROM.ClearEventFlag = 0;
}


static UCHAR is_clearallflag(void)
{
  if(gs_E2PROM.ClearAllFlag > 0)
    return  TRUE;
  return  FALSE;
}

static void clear_clearallflag(void)
{
  gs_E2PROM.ClearAllFlag = 0;
}


static void set_clearsetflag(void)
{
  gs_E2PROM.ClearSetFlag = TRUE;
}

static void set_cleareventflag(void)
{
  gs_E2PROM.ClearEventFlag = TRUE;
}


static void set_clearallflag(void)
{
  gs_E2PROM.ClearAllFlag = TRUE;
}


static void set_activealarmupdate(void)
{
  gs_E2PROM.ActiveAlarmUpdate = TRUE;
}


static void clear_activealarmupdate(void)
{
  gs_E2PROM.ActiveAlarmUpdate = FALSE;
}


static UCHAR is_activealarmupdate(void)
{
  if(gs_E2PROM.ActiveAlarmUpdate > 0)
    return  TRUE;
  return  FALSE;
}


static void set_historyalarmupdate(void)
{
  gs_E2PROM.HistoryAlarmUpdate++;
}


static void clear_historyalarmupdate(void)
{
  //gs_E2PROM.HistoryAlarmUpdate--;
  if(gs_E2PROM.HistoryAlarmUpdate > 1)
  {
    gs_E2PROM.HistoryAlarmUpdate = 1;
  }
  else
  {
    gs_E2PROM.HistoryAlarmUpdate = 0;
  }
}

static UCHAR is_historyalarmupdate(void)
{
  if(gs_E2PROM.HistoryAlarmUpdate > 0)
    return  TRUE;
  return  FALSE;
}


static void set_historyeventupdate(void)
{
  gs_E2PROM.HistoryEventUpdate++;
}

static void clear_historyeventupdate(void)
{
  //gs_E2PROM.HistoryEventUpdate--;
  if(gs_E2PROM.HistoryEventUpdate > 1)
  {
    gs_E2PROM.HistoryEventUpdate = 1;
  }
  else
  {
    gs_E2PROM.HistoryEventUpdate = 0;
  }
}


static UCHAR is_historyeventupdate(void)
{
  if(gs_E2PROM.HistoryEventUpdate > 0)
    return  TRUE;
  return  FALSE;
}


static void set_splashupdate(void)
{
  gs_E2PROM.SplashUpdate++;
}

static void clear_splashupdate(void)
{
  if(gs_E2PROM.SplashUpdate > 1)
  {
    gs_E2PROM.SplashUpdate = 1;
  }
  else
  {
    gs_E2PROM.SplashUpdate = 0;
  }
}

static UCHAR is_splashupdate(void)
{
  if(gs_E2PROM.SplashUpdate > 0)
    return  TRUE;
  return  FALSE;
}


static void set_splashflag(UCHAR data1, UCHAR data2)
{
  gs_E2PROM.SplashFlag1 = data1;
  gs_E2PROM.SplashFlag2 = data2;
}


static UCHAR set_splash(UINT16 ii, void * pDataBuf)
{
  //UCHAR  i, *pChar, *pData;

  /*pChar = &gs_E2PROM.Splash[ii * 16];
  pData = (UCHAR *)pDataBuf;

  for(i = 0; i < 16; i++)
  {
    *pChar++ = *pData++;
  }*/

  return  TRUE;
}


static void set_splashopenflagupdate(void)
{
  gs_E2PROM.SplashOpenFlagUpdate = TRUE;
}

static void clear_splashopenflagupdate(void)
{
  //dv_E2PROM.fWriteE2PROM(E2PROMOpenFlag1, gs_E2PROM.SplashFlag1);		/*标志写E2PROM*/
  //dv_E2PROM.fWriteE2PROM(E2PROMOpenFlag2, gs_E2PROM.SplashFlag2);
  gs_E2PROM.SplashOpenFlagUpdate = FALSE;
}


static UCHAR is_splashopenflagupdate(void)
{
  if(gs_E2PROM.SplashOpenFlagUpdate > 0)
    return  TRUE;
  return  FALSE;
}

static void set_cleardefaultflag(void)
{
  gs_E2PROM.ClearDefaultFlag = TRUE;
}

static void clear_cleardefaultflag(void)
{
  gs_E2PROM.ClearDefaultFlag = 0;
}


static UCHAR is_cleardefaultflag(void)
{
  if(gs_E2PROM.ClearDefaultFlag > 0)
    return  TRUE;
  return  FALSE;
}

static void set_DGdataupdate(void)
{
	gs_E2PROM.DGdataUpdate++;
}

static void clear_DGdataupdate(void)
{
  if(gs_E2PROM.DGdataUpdate > 0)
	  gs_E2PROM.DGdataUpdate--;
}


static UCHAR is_DGdataupdate(void)
{
  if(gs_E2PROM.DGdataUpdate > 0)
    return  TRUE;
  return  FALSE;
}


E2PROM_TypeDef  gs_E2PROM;


void init_gs_E2PROM(void)
{
  unsigned char i;
  gs_E2PROM.DefaultUpdate = 0;
  gs_E2PROM.SetDataUpdate = 0;
  gs_E2PROM.BattTestUpdate = 0;
  gs_E2PROM.BattRecordUpdate = 0;
  gs_E2PROM.ClearDefaultFlag = 0;
  gs_E2PROM.ClearSetFlag = 0;
  gs_E2PROM.ClearEventFlag = 0;
  gs_E2PROM.ClearAllFlag = 0;
  gs_E2PROM.ActiveAlarmUpdate = 0;
  gs_E2PROM.HistoryAlarmUpdate = 0;
  gs_E2PROM.HistoryEventUpdate = 0;
  gs_E2PROM.MenuSystemNameUpdate = 0;
  gs_E2PROM.MenuAliasNameUpdate = 0;
  gs_E2PROM.MenuLineConfigUpdate = 0;
  gs_E2PROM.MenuScreenConfigUpdate = 0;
  gs_E2PROM.MenuAlarmNameUpdate = 0;
  gs_E2PROM.MenuAnalogAlarmNameUpdate = 0;
  gs_E2PROM.SplashUpdate = 0;
  gs_E2PROM.SplashOpenFlagUpdate = 0;
  gs_E2PROM.SplashFlag1 = 0;
  gs_E2PROM.SplashFlag2 = 0;
  gs_E2PROM.ResetReasonRecordPos = 0;
  for(i = 0; i < 3; i++)
  {
	  gs_E2PROM.ResetReasonRecord[i] = 0;
	  gs_E2PROM.u32ResetReasonDogBit[i] = 0;
  }

  gs_E2PROM.fSetDefaultUpdate = set_defaultupdate;
  gs_E2PROM.fClearDefaultUpdate = clear_defaultupdate;
  gs_E2PROM.fIsDefaultUpdate = is_defaultupdate;
  gs_E2PROM.fSetSetDataUpdate = set_setdataupdate;
  gs_E2PROM.fClearSetDataUpdate = clear_setdataupdate;
  gs_E2PROM.fIsSetDataUpdate = is_setdataupdate;
  gs_E2PROM.fSetBattTestUpdate = set_batttestupdate;
  gs_E2PROM.fClearBattTestUpdate = clear_batttestupdate;  
  gs_E2PROM.fIsBattTestUpdate = is_batttestupdate;
  
  
  gs_E2PROM.fSetBattRecordUpdate = set_battrecordupdate;
  gs_E2PROM.fClearBattRecordUpdate = clear_battrecordupdate;
  gs_E2PROM.fIsBattRecordUpdate = is_battrecordupdate;
  
  gs_E2PROM.fSetClearDefaultFlag = set_cleardefaultflag;
  gs_E2PROM.fClearClearDefaultFlag = clear_cleardefaultflag;
  gs_E2PROM.fIsClearDefaultFlag = is_cleardefaultflag;
  gs_E2PROM.fSetClearSetFlag = set_clearsetflag;
  gs_E2PROM.fClearClearSetFlag = clear_clearsetflag;
  gs_E2PROM.fIsClearSetFlag = is_clearsetflag;
  gs_E2PROM.fSetClearEventFlag = set_cleareventflag;
  gs_E2PROM.fClearClearEventFlag = clear_cleareventflag;
  gs_E2PROM.fIsClearEventFlag = is_cleareventflag;
  gs_E2PROM.fSetClearAllFlag = set_clearallflag;
  gs_E2PROM.fClearClearAllFlag = clear_clearallflag;
  gs_E2PROM.fIsClearAllFlag = is_clearallflag;
  gs_E2PROM.fSetActiveAlarmUpdate = set_activealarmupdate;
  gs_E2PROM.fClearActiveAlarmUpdate = clear_activealarmupdate;
  gs_E2PROM.fIsActiveAlarmUpdate = is_activealarmupdate;
  gs_E2PROM.fSetHistoryAlarmUpdate = set_historyalarmupdate;
  gs_E2PROM.fClearHistoryAlarmUpdate = clear_historyalarmupdate;
  gs_E2PROM.fIsHistoryAlarmUpdate = is_historyalarmupdate;
  gs_E2PROM.fSetHistoryEventUpdate = set_historyeventupdate;
  gs_E2PROM.fClearHistoryEventUpdate = clear_historyeventupdate;
  gs_E2PROM.fIsHistoryEventUpdate = is_historyeventupdate;
  gs_E2PROM.fSetSplashFlag = set_splashflag;
  gs_E2PROM.fSetSplash = set_splash;
  gs_E2PROM.fSetSplashOpenFlagUpdate = set_splashopenflagupdate;
  gs_E2PROM.fClearSplashOpenFlagUpdate = clear_splashopenflagupdate;
  gs_E2PROM.fIsSplashOpenFlagUpdate = is_splashopenflagupdate;
  gs_E2PROM.fSetSplashUpdate = set_splashupdate;
  gs_E2PROM.fClearSplashUpdate = clear_splashupdate;
  gs_E2PROM.fIsSplashUpdate = is_splashupdate;
  gs_E2PROM.StatisticUpdate = 0;
  gs_E2PROM.fSetStatisticUpdate = set_statisticupdate;
  gs_E2PROM.fClearStatisticUpdate = clear_statisticupdate;
  gs_E2PROM.fIsStatisticUpdate = is_statisticupdate;
  gs_E2PROM.DGdataUpdate = 0;
  gs_E2PROM.fSetDGdataUpdate = set_DGdataupdate;
  gs_E2PROM.fClearDGdataUpdate = clear_DGdataupdate;
  gs_E2PROM.fIsDGdataUpdate = is_DGdataupdate;
  
  gs_E2PROM.StatisticTempUpdate = 0;
  gs_E2PROM.fSetStatisticTempUpdate = set_statistictempupdate;
  gs_E2PROM.fClearStatisticTempUpdate = clear_statistictempupdate;
  gs_E2PROM.fIsStatisticTempUpdate = is_statistictempupdate;

}

UINT8 u8IsEVUint8UpData(void)
{
	if (gEvChargeInfo.u8CharParamUpDataCount > 0)
	{
		gEvChargeInfo.u8CharParamUpDataCount--;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void UpUint8EVData(void)
{

}


UINT8 u8IsEVFloatUpData(void)
{
	if (gEvChargeInfo.u8FloatParamUpDataCount > 0)
	{
		gEvChargeInfo.u8FloatParamUpDataCount--;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


void UpFloatEVData(void)
{

}

/********************************************************************************************************
										------------
										|			|
										|			|
--->u16BytesPst当前在此区域的字节偏移	|			|
										|			|
									B-->|			|
										|			|
										|			|
										------------
*********************************************************************************************************/
void RefreshFaultLogVievBuf(void);
void RefreshFaultLogVievBuf(void)
{
	//gZTEFaultLogIdx.u16ViewLogNumPerFive= 1 刚一进这个界面View的位置一定是最后5条！		点击一下 下一页就会+1
	//每一屏幕 显示5条
	INT32 iMidLogNumPst = 0;
	UINT16 u16ViewRAddr = 0;
	UINT16 u16Idx = 0;
	UINT8 u8LastLOGNum = 0;
	UINT8 u8StartLOGNum = 0;
	UINT8* pData = NULL;
	FaultLogIdxRefresh();
	//实际可以考虑双向链表，但是EEPROM空间太小了。算了
	
	//证明这个窗口写满了，所以可以任意取数据
	if (gZTEFaultLogIdx.u16TotalLOGNum > MAX_FAULT_LOG_NUM)																	
	{
		iMidLogNumPst = gZTEFaultLogIdx.u16TotalLOGNum - gZTEFaultLogIdx.u16ViewLogNumPerFive * VIEW_FAULT_PER_FIVE;			//往前减5个记录数
		if (iMidLogNumPst >= 0)
		{
			iMidLogNumPst = iMidLogNumPst % MAX_FAULT_LOG_NUM;
			if (iMidLogNumPst < MAX_FAULT_LOG_NUM - VIEW_FAULT_PER_FIVE)														//可以直接读取5组数据
			{
					pData = &gZTEFaultLogView[0].u8AlmVal;
					for (u16Idx = 0; u16Idx < (VIEW_FAULT_PER_FIVE * sizeof(__ZTE_FAULT_LOG));u16Idx++)
					{
							pData[u16Idx] = dv_E2PROM.fReadE2PROM(FAULT_LOG_VAL_ADDR + (iMidLogNumPst * sizeof(__ZTE_FAULT_LOG)/sizeof(UINT8)) + u16Idx);
					}
			}
			else
			{
					//变到了上图中的B 位置， 离窗口最后只有 3 个数据, 又得考虑掉头
					u8LastLOGNum = (MAX_FAULT_LOG_NUM - iMidLogNumPst);														//150- 刚才起始位置，可能只有 u8LastLOGNum = 3
					//小于5 大于0  容错
					if (u8LastLOGNum < (VIEW_FAULT_PER_FIVE) && u8LastLOGNum > 0)				
					{
						pData = &gZTEFaultLogView[0].u8AlmVal;
						for (u16Idx = 0; u16Idx < (u8LastLOGNum * sizeof(__ZTE_FAULT_LOG));u16Idx++)
						{
							pData[u16Idx] = dv_E2PROM.fReadE2PROM(FAULT_LOG_VAL_ADDR + (iMidLogNumPst * sizeof(__ZTE_FAULT_LOG)/sizeof(UINT8)) + u16Idx);
						}

						u8StartLOGNum = VIEW_FAULT_PER_FIVE - u8LastLOGNum;													//比如B的位置，则从0开始再来两组数据	u8StartLOGNum=2
						pData = &gZTEFaultLogView[u8LastLOGNum].u8AlmVal;
						for (u16Idx = 0; u16Idx < (u8StartLOGNum * sizeof(__ZTE_FAULT_LOG));u16Idx++)
						{
							pData[u16Idx] = dv_E2PROM.fReadE2PROM(FAULT_LOG_VAL_ADDR + u16Idx);								//掉头从0开始取得！
						}
					}
			}
		}
		else
		{
			u16ViewRAddr = 0;
			pData = &gZTEFaultLogView[0].u8AlmVal;
			for (u16Idx = 0; u16Idx < (VIEW_FAULT_PER_FIVE * sizeof(__ZTE_FAULT_LOG)); u16Idx++)
			{
				pData[u16Idx] = dv_E2PROM.fReadE2PROM(FAULT_LOG_VAL_ADDR + u16Idx);
			}
		}
	}
	else
	{
			if (gZTEFaultLogIdx.u16TotalLOGNum > 0)
			{
				//也就是说大于 5 条记录
				if (gZTEFaultLogIdx.u16BytesPst >= ((sizeof(__ZTE_FAULT_LOG)/sizeof(UINT8)) * VIEW_FAULT_PER_FIVE) * gZTEFaultLogIdx.u16ViewLogNumPerFive)	
				{
					u16ViewRAddr = gZTEFaultLogIdx.u16BytesPst - ((sizeof(__ZTE_FAULT_LOG)/sizeof(UINT8)) * VIEW_FAULT_PER_FIVE) * gZTEFaultLogIdx.u16ViewLogNumPerFive;
					pData = &gZTEFaultLogView[0].u8AlmVal;
					for (u16Idx = 0; u16Idx < (VIEW_FAULT_PER_FIVE * sizeof(__ZTE_FAULT_LOG));u16Idx++)
					{
						pData[u16Idx] = dv_E2PROM.fReadE2PROM(FAULT_LOG_VAL_ADDR + u16ViewRAddr + u16Idx);
					}
				}
				else
				{
					u16ViewRAddr = 0;
					pData = &gZTEFaultLogView[0].u8AlmVal;
					for (u16Idx = 0; u16Idx < gZTEFaultLogIdx.u16BytesPst;u16Idx++)
					{
						pData[u16Idx] = dv_E2PROM.fReadE2PROM(FAULT_LOG_VAL_ADDR + u16Idx);
					}
				}
			}
			else
			{
				for (u16Idx = 0; u16Idx < VIEW_FAULT_PER_FIVE;u16Idx++)
				{
					gZTEFaultLogView[u16Idx].u8AlmVal = 0;								//无告警！
					gZTEFaultLogView[u16Idx].FaultTime.tm_year[0] = 0;	
					gZTEFaultLogView[u16Idx].FaultTime.tm_year[1] = 0;
					gZTEFaultLogView[u16Idx].FaultTime.tm_mon = 0;	
					gZTEFaultLogView[u16Idx].FaultTime.tm_hour = 0;	
					gZTEFaultLogView[u16Idx].FaultTime.tm_mday = 0;	
					gZTEFaultLogView[u16Idx].FaultTime.tm_sec = 0;	
					gZTEFaultLogView[u16Idx].FaultTime.tm_min = 0;	
					gZTEFaultLogView[u16Idx].u8RectNo = 50;
				}
			}
	}
}

void ClrFaultLOG(void)
{
	gZTEFaultLogIdx.u16BytesPst = 0;
	gZTEFaultLogIdx.u16TotalLOGNum = 0;
	UINT8 u8ValH = 0;
	UINT8 u8ValL = 0;
	UINT16 u16Idx = 0;
	UINT16 u16Idx2 = 0;
	u8ValH = 0xff&(gZTEFaultLogIdx.u16BytesPst >> 8);
	u8ValL =  0xff&(gZTEFaultLogIdx.u16BytesPst);
	dv_E2PROM.fWriteE2PROM(FAULT_LOG_PST_ADDR_H, u8ValH);
	dv_E2PROM.fWriteE2PROM(FAULT_LOG_PST_ADDR_L, u8ValL);
	u8ValH = 0xff&(gZTEFaultLogIdx.u16TotalLOGNum >> 8);
	u8ValL =  0xff&(gZTEFaultLogIdx.u16TotalLOGNum);
	dv_E2PROM.fWriteE2PROM(FAULT_LOG_NUM_ADDR_H, u8ValH);
	dv_E2PROM.fWriteE2PROM(FAULT_LOG_NUM_ADDR_L, u8ValL);
	UINT8* pBuf = NULL;
	gE2promFaultLog.u8AlmVal = 0;
	gE2promFaultLog.FaultTime.tm_hour = 0;
	gE2promFaultLog.FaultTime.tm_sec = 0;
	gE2promFaultLog.FaultTime.tm_min = 0;
	gE2promFaultLog.FaultTime.tm_mday = 1;
	gE2promFaultLog.FaultTime.tm_mon = 1;
	gE2promFaultLog.FaultTime.tm_year[1] = (1970/256);
	gE2promFaultLog.FaultTime.tm_year[0] = (1970%256);
	gE2promFaultLog.u8RectNo = 50;
	UINT16 u16WAddr = FAULT_LOG_VAL_ADDR;
	for (u16Idx = 0; u16Idx < MAX_FAULT_LOG_NUM; u16Idx++)
	{
		pBuf = &gE2promFaultLog.u8AlmVal;
		u16WAddr = FAULT_LOG_VAL_ADDR + (sizeof(__ZTE_FAULT_LOG)/sizeof(UINT8)) * u16Idx;
		for(u16Idx2 = 0; u16Idx2 < (sizeof(__ZTE_FAULT_LOG)/sizeof(UINT8)); u16Idx2++)
		{
			dv_E2PROM.fWriteE2PROM(u16WAddr + u16Idx2, pBuf[u16Idx2]);
		}
		OSTimeDly(3);
		gSelfTest.fSetTaskID(Task_ID_E2prom);
	}
}

/***************************************************************
	充电历史记录：
	如果之前连着网，上报不成功，则保存到EEPROM中
	当网络OK之后，通过 NET_LastUnRptRecord0X78Deal进行上报！
***************************************************************/
void TASK_E2promProc(void* pdata)
{
	struct tm  tmNow;
	UNUSED(pdata);
#define MAX_BUFFER_SIZE  16
	UCHAR * pTest;
	UINT16  i;
	UCHAR j;
	UCHAR  E2PROMPageRead[MAX_BUFFER_SIZE+1];
	UCHAR  E2PROMPageWrite[MAX_BUFFER_SIZE+1];
	dv_E2PROM.fSetE2PROMTaskRun();							//目的与RTC互斥！
	
	for(;;)
	{
		
		gSelfTest.fSetTaskID(Task_ID_E2prom);
		OSTimeDly(10);
		gSelfTest.fSetTaskID(Task_ID_E2prom);
		gRTCData.fGetLocalTime(&tmNow);
		if(gs_E2PROM.fIsSetDataUpdate())
		{
			gSelfTest.fSetTaskID(Task_ID_E2prom);


			//E2PROMPageRead[0] = dv_E2PROM.fReadE2PROM(E2PROMCharData + SETDATA_BaudRate_NUM);

			/*系统工作模式变化需进行重启*/
			//E2PROMPageRead[0] = dv_E2PROM.fReadE2PROM(E2PROMCharData + SETDATA_SysWorkMode_NUM);	

			/*系统工作模式变化需进行重启*/
			//E2PROMPageRead[0] = dv_E2PROM.fReadE2PROM(E2PROMCharData + ADDR_IsSingleGunMode);	

			//if(E2PROMPageRead[0] == 0xA1)
			//{
			//	//读取错误
			//}
			//else if(E2PROMPageRead[0] != gEvChargeInfo.u8IsSingleGunMode)
			//{
			//		gSetDataInfo.SetData.ArrayData.CfgCharData[ADDR_IsSingleGunMode] = gEvChargeInfo.u8IsSingleGunMode;
			//	//E2PROMPageRead[0] = dv_E2PROM.fReadE2PROM(E2PROMCharData1 + ADDR_IsSingleGunMode);	
			//	//if(E2PROMPageRead[0] != gEvChargeInfo.u8IsSingleGunMode)
			//	{
			//		if(gSelfTest.StopFeedDogReason == 0)
			//		{
			//			gSelfTest.StopFeedDogReason = 2;
			//			gSelfTest.u32DogBit_Bak = ((UINT32)E2PROMPageRead[0]<<8) + ((UINT32)gSelfTest.fLastSetDataValue & 0xff) + ((UINT32)gSelfTest.LastSetDataID<<16);
			//			gs_E2PROM.fSetHistoryEventUpdate();
			//		}
			//		RecordEvent(HISTORY_EVENT_SETDATA_CHG, ADDR_IsSingleGunMode, gEvChargeInfo.u8IsSingleGunMode, 0);	
			//		dv_Output.fOpenBeep();
			//		dv_Output.fOpenBeep();
			//		OSTimeDly(700);
			//		OSTimeDly(700);
			//		OSTimeDly(700);
			//		OSTimeDly(700);
			//		OSTimeDly(700);
			//		Trap();
			//	}
			//}
			//else
			//{
			//	gSelfTest.fLastSetDataValue = 0xff;
			//	gSelfTest.LastSetDataID = 0xffff;
			//}
                        gSelfTest.fSetTaskID(Task_ID_E2prom);
                        
			//枪类型发生变化
			//E2PROMPageRead[0] = dv_E2PROM.fReadE2PROM(E2PROMCharData + ADDR_1_GUN_2_GUN_MODE);

			//if(E2PROMPageRead[0] == 0xA1)
			//{
			//	//读取错误
			//}
			//else if(E2PROMPageRead[0] != gEvChargeInfo.u16SysCfgType)
			//{
			//	gSetDataInfo.SetData.ArrayData.CfgCharData[ADDR_1_GUN_2_GUN_MODE] = gEvChargeInfo.u16SysCfgType;

			//	if(gSelfTest.StopFeedDogReason == 0)
			//	{
			//			gSelfTest.StopFeedDogReason = 2;
			//			gSelfTest.u32DogBit_Bak = ((UINT32)E2PROMPageRead[0]<<8) + ((UINT32)gSelfTest.fLastSetDataValue & 0xff) + ((UINT32)gSelfTest.LastSetDataID<<16);
			//			gs_E2PROM.fSetHistoryEventUpdate();
			//	}		

			//	RecordEvent(HISTORY_EVENT_SETDATA_CHG, ADDR_1_GUN_2_GUN_MODE, gEvChargeInfo.u16SysCfgType, 0);	

			//	Trap();
			//}

			gSelfTest.fSetTaskID(Task_ID_E2prom);
			pTest = &gSetDataInfo.SetData.NameData.SysType;	/*设置数据开始地址*/
			for(i = 0; i < mSETDATADIV16; i++)
			{
				gSelfTest.fSetTaskID(Task_ID_E2prom);
				dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, E2PROMCharData + i * 16, 16);
				if(CompareSetData(pTest, E2PROMPageRead, 16) == FALSE)
				{
					WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
					dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData + i * 16, 16);
					dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData1 + i * 16, 16);
					dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData2 + i * 16, 16);
				}
				pTest += 16;
			}
			gs_E2PROM.fClearSetDataUpdate();
			gSelfTest.fSetTaskID(Task_ID_E2prom);
			//E2PROMPageRead[0] = dv_E2PROM.fReadE2PROM(E2PROMCharData + SETDATA_SystemMode_NUM);
		}

		if(gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[0] == 1)
		{
			WriteE2PROMBuffer((UINT8 *)gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade, E2PROMPageWrite, 8);
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROM_Rect_Upgade_BinFileName_U1, 8);

			gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[0] = 0;

			gSelfTest.fSetTaskID(Task_ID_E2prom);
		}
		if(gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[1] == 1)
		{
			WriteE2PROMBuffer((UINT8 *)gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade, E2PROMPageWrite, 8);
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROM_Rect_Upgade_BinFileName_U2, 8);

			gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[1] = 0;

			gSelfTest.fSetTaskID(Task_ID_E2prom);
		}

		if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
		{
			continue;
		}
		//float fTempValue = 0;
		//GetFloatSetData(SETDATA_PeakFlatValleyFare + 1,&fTempValue);

		//if((tmNow.tm_sec %10 == 0) && (gSelfTest.fGetStopFeedDog() == 0))		/*5S钟更新一下*/
		//{
		//	if (u8IsEVFloatUpData())
		//	{
		//		UpFloatEVData();
		//		gSelfTest.fSetTaskID(Task_ID_E2prom);
		//	}

		//	if (u8IsEVUint8UpData())
		//	{
		//		UpUint8EVData();
		//		gSelfTest.fSetTaskID(Task_ID_E2prom);
		//	}
		//	
		//	gSelfTest.fSetTaskID(Task_ID_E2prom);
		//}

		//掉电记录 起始信息
		for(j = 0; j < 2; j++)
		{
			if(gs_EepromRecordMng.u8WriteStartDataFlag[j])
			{
				gs_EepromRecordMng.u8WriteStartDataFlag[j] = 0;

				gSelfTest.fSetTaskID(Task_ID_E2prom);
				pTest = (UCHAR *)&gs_EepromRecordMng.StartData[j];	/*设置数据开始地址*/
				for(i = 0; i < mEEPROM_RECORD_START_DATA_DIV16; i++)
				{
					gSelfTest.fSetTaskID(Task_ID_E2prom);
					dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, RECORD_EEPROM_GUN_A_START_DATA+mRecordEepromStartDataSize*j  + i * 16, 16);
					if(CompareSetData(pTest, E2PROMPageRead, 16) == FALSE)
					{
						WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
						dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, RECORD_EEPROM_GUN_A_START_DATA+mRecordEepromStartDataSize*j + i * 16, 16);
					}
					pTest += 16;
				}
				
				gSelfTest.fSetTaskID(Task_ID_E2prom);
				pTest = (UCHAR *)&gs_EepromRecordMng.u8EndTimePhase[j];	
				WriteE2PROMBuffer(pTest, E2PROMPageWrite, 3);
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, RECORD_EEPROM_GUN_A_END_TIMEPHASE+mRecordEepromStartEndTimePhaseSize*j, 3);

				gSelfTest.fSetTaskID(Task_ID_E2prom);

				dv_E2PROM.fWriteE2PROMFloat_2((RECORD_EEPROM_GUN_A_PHASE_KWH+mRecordEepromPhaseKwhSize*j+gs_EepromRecordMng.u8LastTimePhase[j]*4), gs_EepromRecordMng.fStepKwh[j]);
				
				gSelfTest.fSetTaskID(Task_ID_E2prom);


				gSelfTest.fSetTaskID(Task_ID_E2prom);

				pTest = (UCHAR *)&gs_EepromRecordMng.u8StartTimePhase[j];	
				WriteE2PROMBuffer(pTest, E2PROMPageWrite, 3);
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, RECORD_EEPROM_GUN_A_START_TIMEPHASE+mRecordEepromStartEndTimePhaseSize*j, 3);

				gSelfTest.fSetTaskID(Task_ID_E2prom);

				OSTimeDly(10);

			}
		}

		//掉电记录 半点信息
		for(j = 0; j < 2; j++)
		{
			if(gs_EepromRecordMng.u8WriteTimeDataFlag[j])
			{
				gs_EepromRecordMng.u8WriteTimeDataFlag[j] = 0;

				gSelfTest.fSetTaskID(Task_ID_E2prom);
				pTest = (UCHAR *)&gs_EepromRecordMng.TimeData[j];	/*设置数据开始地址*/
				for(i = 0; i < mEEPROM_RECORD_TIME_DATA_DIV16; i++)
				{
					gSelfTest.fSetTaskID(Task_ID_E2prom);
					dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, RECORD_EEPROM_GUN_A_TIME_DATA+mRecordEepromTimeDataSize*j  + i * 16, 16);
					if(CompareSetData(pTest, E2PROMPageRead, 16) == FALSE)
					{
						WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
						dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, RECORD_EEPROM_GUN_A_TIME_DATA+mRecordEepromTimeDataSize*j + i * 16, 16);
					}
					pTest += 16;
				}
			

				gSelfTest.fSetTaskID(Task_ID_E2prom);

				dv_E2PROM.fWriteE2PROMFloat_2((RECORD_EEPROM_GUN_A_PHASE_KWH+mRecordEepromPhaseKwhSize*j+gs_EepromRecordMng.u8LastTimePhase[j]*4), gs_EepromRecordMng.fStepKwh[j]);

				gSelfTest.fSetTaskID(Task_ID_E2prom);				

				pTest = (UCHAR *)&gs_EepromRecordMng.u8EndTimePhase[j];	
				WriteE2PROMBuffer(pTest, E2PROMPageWrite, 3);
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, RECORD_EEPROM_GUN_A_END_TIMEPHASE+mRecordEepromStartEndTimePhaseSize*j, 3);

				gSelfTest.fSetTaskID(Task_ID_E2prom);
				OSTimeDly(10);
			}
		}

		//掉电记录 存完FLASH， 清除EEPROM记录
		for(j = 0; j < 2; j++)
		{
			if(gs_EepromRecordMng.u8ClearDataFlag[j])
			{
				gs_EepromRecordMng.u8ClearDataFlag[j] = 0;


				//pTest = (UCHAR *)&gs_EepromRecordMng.u8StartTimePhase[j];	
				//WriteE2PROMBuffer(pTest, E2PROMPageWrite, 3);
				E2PROMPageWrite[0] = 0xff;
				E2PROMPageWrite[1] = 0xff;
				E2PROMPageWrite[2] = 0xff;
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, RECORD_EEPROM_GUN_A_START_TIMEPHASE+mRecordEepromStartEndTimePhaseSize*j, 3);
				gSelfTest.fSetTaskID(Task_ID_E2prom);
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, RECORD_EEPROM_GUN_A_END_TIMEPHASE+mRecordEepromStartEndTimePhaseSize*j, 3);
				gSelfTest.fSetTaskID(Task_ID_E2prom);
			}
		}

		

		gSelfTest.fSetTaskID(Task_ID_E2prom);

		if(gs_UpGradeCtrl.u8UpgradeWriteFlag == 1)
		{
			//写升级标志
			if(gEvChargeInfo.u8BootVersion >= BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k)
			{
				E2PROMPageWrite[0] = EX_FLASH_UPDATA_1_SIZEOVER512K;
				E2PROMPageWrite[1] = EX_FLASH_UPDATA_2_SIZEOVER512K;
			}
			else
			{
				E2PROMPageWrite[0] = EX_FLASH_UPDATA_1;
				E2PROMPageWrite[1] = EX_FLASH_UPDATA_2;
			}
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMFLAGADD1, 2);

			gSelfTest.fSetTaskID(Task_ID_E2prom);
			gs_UpGradeCtrl.u8UpgradeWriteFlag = 0;
		}

		//if((tmNow.tm_sec %3 == 0) && (gSelfTest.fGetStopFeedDog() == 0))
		//{
		//	RefreshFaultLogVievBuf();
		//}

		//if (gEvChargeInfo.u8CLRFaultLog == TRUE)
		//{
		//	gEvChargeInfo.u8CLRFaultLog = FALSE;
		//	ClrFaultLOG();
		//}

		if(gs_E2PROM.fIsHistoryEventUpdate())
		{
			if(gs_E2PROM.ResetReasonRecordPos < 3)
			{			
				dv_E2PROM.fWriteE2PROM(E2PROMRResetReason1 + gs_E2PROM.ResetReasonRecordPos*5, gSelfTest.StopFeedDogReason);
				gSelfTest.fSetTaskID(Task_ID_E2prom);
				pTest = (UCHAR *)&gSelfTest.u32DogBit_Bak;	
				WriteE2PROMBuffer(pTest, E2PROMPageWrite, 4);
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite,E2PROMRResetReason1 + gs_E2PROM.ResetReasonRecordPos*5 + 1, 4);
				gSelfTest.fSetTaskID(Task_ID_E2prom);
				gs_E2PROM.ResetReasonRecordPos++;
				if(gs_E2PROM.ResetReasonRecordPos > 2)
				{
					gs_E2PROM.ResetReasonRecordPos = 0;
				}
				dv_E2PROM.fWriteE2PROM(E2PROMRResetReasonPos, gs_E2PROM.ResetReasonRecordPos);
				gSelfTest.fSetTaskID(Task_ID_E2prom);
			}

			gs_E2PROM.fClearHistoryEventUpdate();
		}

		 if(gs_E2PROM.fIsClearEventFlag())
		 {			 
			 dv_E2PROM.fWriteE2PROM(E2PROMRResetReasonPos, 0);
			 dv_E2PROM.fWriteE2PROM(E2PROMRResetReason1, 0);
			 gSelfTest.fSetTaskID(Task_ID_E2prom);
			 dv_E2PROM.fWriteE2PROM(E2PROMRResetReason2, 0);
			 dv_E2PROM.fWriteE2PROM(E2PROMRResetReason3, 0);
			 gSelfTest.fSetTaskID(Task_ID_E2prom);

			 gs_E2PROM.ResetReasonRecordPos = 0;
			 for(i = 0; i < 3; i++)
			 {
				 gs_E2PROM.ResetReasonRecord[i] = 0;
				 gs_E2PROM.u32ResetReasonDogBit[i] = 0;
			 }

			 gs_E2PROM.fClearClearEventFlag();
		 }


		if((tmNow.tm_sec != 0) && (gSelfTest.fGetStopFeedDog() == 0))	/*1分钟到?*/
		{
			OSTimeDly(160);
			gSelfTest.fSetTaskID(Task_ID_E2prom);
			continue;
		}
		else
		{
			OSTimeDly(10);
			gSelfTest.fSetTaskID(Task_ID_E2prom);
		}
		//gRTCData.fGetLocalTime(&tmNow);

		//if(tmNow.tm_sec != 0)/*1秒钟过?*/
		//{
		//	OSTimeDly(160);
		//	gSelfTest.fSetTaskID(Task_ID_E2prom);
		//}
		//else
		//{
		//	OSTimeDly(100); // Delay 1200ms
		//	gSelfTest.fSetTaskID(Task_ID_E2prom);
		//}
	}
}
