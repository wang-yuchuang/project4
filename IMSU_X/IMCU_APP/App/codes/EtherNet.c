#include "main.h"
#include "EtherNet.h"
#include "Uart6_new.h"
#include "resolv.h"
#include "can_ring.h"
#include "EnergyOutputMng.h"

void NetPipeWHDOG(void);
#define NET_MAX_BUF  1500
ChargeLogTable UnRptLogTable;
 UINT8 gNetBuf[NET_MAX_BUF];
 UINT8 gNetR_Buf[NET_MAX_BUF];
 UINT8 gNetS_Buf[NET_MAX_BUF];
NOINIT __NET_TCP_INFO gNET_TCP_Info;
extern NOINIT ChargeLogTable   gChargeLogTable;

EthAppCommMng gs_EthAppCommMng;

UINT8 g8Mac[16];
UINT8 gNetSeqNo = 0;  //报文的流水号

INT32 GetValidYunShanData_2(UINT16 u16CmdID);
UINT8 GetOnePkg(UINT16 u16TotalLen, UINT16* pNextStartPST, UINT16* StepLen);
UINT8 NetSendData(void);

//static UINT8 u8UnPackXXXCmd(UINT16 u16StepL, UINT16 u16CmdID);

/*static void RefreshEtherNetTime(void)
{
	struct tm  tmNow;
	static UCHAR  Sec = 0;
	unsigned char i;

	gRTCData.fGetLocalTime(&tmNow);
	if((UCHAR)tmNow.tm_sec != Sec)//秒钟
	{
		Sec = (UCHAR)tmNow.tm_sec;
		if(gNET_TCP_Info.u32BreakCount<65535)
		{
			gNET_TCP_Info.u32BreakCount++;
		}
		if(gNET_TCP_Info.u16ReDHCPCount<65535)
		{
			gNET_TCP_Info.u16ReDHCPCount++;
		}
		
        if (gs_EthAppCommMng.i32RegistCount >= 0)
        {
            gs_EthAppCommMng.i32RegistCount--;
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
			


		}
	}
}*/
//static void initSendBuffer(void)
//{
//	UINT16 i;
//	for (i = 0; i< NET_BUF_DATA_L; i++)
//	{
//		gNetS_Buf[i] = '\0';
//	}
//}

/************************************************************************************************************************
	-1  没有读到数据; 
	0  找到所需要的数值
	>0  则认为是有数值  但是没找到所需要的数据
*************************************************************************************************************************/
//static INT16 NetReadData(UINT16 u16TimeOut10ms,UINT16 u16CmdID)
//{
//	UINT16 u16Idx = 0;
//	INT16 i16Result = BUF_NO_DATA;
//	INT8 iRst = BUF_NO_DATA;
//	u16TimeOut10ms = MAX(u16TimeOut10ms,1);
//	if(gNET_TCP_Info.u32BreakCount>180)
//	{
//		u16TimeOut10ms = 1;
//	}
//
//	for (u16Idx = 0; u16Idx < u16TimeOut10ms; u16Idx++)
//	{
//		gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
//		iRst = GetValidYunShanData_2(u16CmdID);
//		if (iRst == BUF_HAS_DADA)
//		{
//			//gNET_TCP_Info.u32BreakCount =0;
//			NetPipeWHDOG();
//			i16Result = BUF_HAS_DADA;
//		}
//		if (iRst == FIND_SPECIFIED_CMD)
//		{
//			//gNET_TCP_Info.u32BreakCount =0;
//			NetPipeWHDOG();
//			i16Result = FIND_SPECIFIED_CMD;
//			break;
//		}
//		OSTimeDly(10);
//	}
//
//	//if(gNET_TCP_Info.u32BreakCount>180)
//	//{
//	//	CLR_CONN_STAT(gNET_TCP_Info.u8MachineStat);		//标志没有连接
//	//	CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);	
//	//}
//	return i16Result;
//}

UINT8 NetSendData(void)
{
	/*UINT16 u16Idx = 0;

	for (u16Idx = 0; u16Idx< 1000;u16Idx++)
	{
		if (CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))
		{
			OSTimeDly(5);
		}
		else
		{
			break;
		}
	}
	SET_SEND_STAT(gNET_TCP_Info.u8MachineStat);
	for (u16Idx = 0; u16Idx < 3000; u16Idx++)
	{
		OSTimeDly(20);
		gSelfTest.fSetTaskID(TaskofNetZhixinlID);
		if (!CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))
		{
			return GET_EEM_OUTPUT_DATA_SUCCESS;
		}
	}
	return GET_EEM_OUTPUT_DATA_FAIL;*/
	return 0;
}


static void InitNetData(void)
{
	UINT8 u8Idx = 0;
	struct tm  tmNow;
	gRTCData.fGetLocalTime(&tmNow);
	gs_EthAppCommMng.PowerOnTime = tmNow;
	gs_EthAppCommMng.u8RegistResult = FALSE;
	gs_EthAppCommMng.u8TickFlag = FALSE;


	gs_EthAppCommMng.i16CommFailTimes = 0;
	
	gs_EthAppCommMng.i16OrderTimeOUT[0] = 0;			//预约超时时间 用于回传 不用于记数
	gs_EthAppCommMng.u8OrderChargeCMD[0] = 0;			//0-无预约（无效）	1-预约有效
	gs_EthAppCommMng.i16OrderTimeOUT[1] = 0;			//预约超时时间 用于回传 不用于记数
	gs_EthAppCommMng.u8OrderChargeCMD[1] = 0;			//0-无预约（无效）	1-预约有效
	gs_EthAppCommMng.i32OrderTimeOUTCount[0] = 0;
	gs_EthAppCommMng.i32OrderTimeOUTCount[1] = 0;


	gs_EthAppCommMng.u8CMD202Flag[0] = FALSE;
	gs_EthAppCommMng.u8CMD202Flag[1] = FALSE;



	for (u8Idx = 0; u8Idx < 16; u8Idx++)
	{
		g8Mac[u8Idx]= '\0';
	}

	g8Mac[0] = '0';
	g8Mac[1] = '0';
	g8Mac[2] = '0';
	g8Mac[3] = '9';
	g8Mac[4] = 'f';
	g8Mac[5] = '5';
	g8Mac[6] = '1';
	g8Mac[7] = '2';
	g8Mac[8] = '8';
	g8Mac[9] = 'a';
	g8Mac[10] = 'b';
	g8Mac[11] = 'c';
	g8Mac[12] = '\0';




	for(u8Idx=0;u8Idx<6;u8Idx++)
	{
		g8Mac[u8Idx] = gEvChargeInfo.u8Mac[u8Idx];
	}
	gs_EthAppCommMng.i32RegistCount = 0;
	

	gs_EthAppCommMng.i32EV_FLASH_RecordDelay = 1 * 60;

	gs_EthAppCommMng.ui16DHCPReconfigTimeOUT = 0;

	memset(gs_EthAppCommMng.ucSetCodeByHost[0], 0, sizeof(gs_EthAppCommMng.ucSetCodeByHost));
	memset(gs_EthAppCommMng.ucSetIdentify[0], 0, sizeof(gs_EthAppCommMng.ucSetIdentify));
	
	gNET_TCP_Info.u16SendNo = 0;
	for(u8Idx = 0; u8Idx < NET_S_NUM; u8Idx++)
	{
		gNET_TCP_Info.u8SendBuf[u8Idx].u16SendNoBak = 0;
	}
}

//按照包的格式  找一个完整的数据包
UINT8 GetOnePkg(UINT16 u16TotalLen, UINT16* pNextStartPST, UINT16* StepLen)
{
	UINT16 uIdx1 = 0;
	UINT16 uIdx2 = 0;
	UINT16 StepMaxLen = 0;
	UINT16 u16temp;
	UINT16 u16RcvContentLen;
	UINT16 s_u16EnjiChkSum = 0;
	UINT16 u8Rst = FALSE;
	UINT16 u16StepStart = 0;
	UINT8 i, BufNo;
//	UINT8 RTN = FALSE;

	if (*pNextStartPST >= NET_MAX_BUF - 1 || *pNextStartPST > u16TotalLen)
	{
		return FALSE;
	}

	for (uIdx1 = *pNextStartPST; uIdx1 < u16TotalLen && uIdx1 < NET_MAX_BUF; uIdx1++)
	{
		if ((gNetR_Buf[uIdx1+ 0] == ENJI_SOI_1) && (gNetR_Buf[uIdx1+ 1] == ENJI_SOI_2)
				&& (gNetR_Buf[uIdx1+ 2] == ENJI_SOI_3))	
		{
			u16RcvContentLen = (UINT16)gNetR_Buf[uIdx1 + 5] + ((UINT16)gNetR_Buf[uIdx1 + 4] << 8);
			StepMaxLen = u16RcvContentLen + 6 + 3;

			if(((StepMaxLen + uIdx1) <= u16TotalLen)
			   && (gNetR_Buf[uIdx1+ StepMaxLen - 1] == ENJI_EOI_1))
			{
				*StepLen = StepMaxLen;
				u16StepStart = uIdx1;
				u8Rst = TRUE;
				*pNextStartPST = uIdx1 + StepMaxLen;
				break;
			}			
		}
	}

	if (u8Rst)
	{		
		for (uIdx2 = u16StepStart; (uIdx2 < (u16StepStart+StepMaxLen)) && (uIdx2 < NET_MAX_BUF); uIdx2++)
		{
			gNetBuf[uIdx2 - u16StepStart] = gNetR_Buf[uIdx2];			
		}
		for(uIdx2 = 3; uIdx2 < (StepMaxLen-3); uIdx2++)
		{
			s_u16EnjiChkSum += gNetBuf[uIdx2];
		}	
		u16temp = ((UINT16)gNetR_Buf[uIdx2]<<8) + gNetR_Buf[uIdx2+1];

		if(s_u16EnjiChkSum == u16temp) //校验和正确
		{			
			BufNo = ENJI_M_S_COMM_RCV_BUF_NUM;
			for(i = 0; i < ENJI_M_S_COMM_RCV_BUF_NUM; i++)
			{		
				if(gs_IMMU2_LCCU_Mng.RcvBuf[i].RcvFlag == ENJI_RCV_FLAG_INIT) 
				{
					BufNo = i;
					break;
				}
			}					
			if(BufNo < ENJI_M_S_COMM_RCV_BUF_NUM)
			{
				for(uIdx2 = 0; uIdx2 < StepMaxLen && uIdx2 < UART6_ENJI_RECV_MAX_NUM; uIdx2++)
				{
					gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[uIdx2] = gNetBuf[uIdx2];
				}	
				gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].u16RcvPos = StepMaxLen; 
				gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].u16RcvContentLen = u16RcvContentLen;
				gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RcvOverTimeCnt = 0;
				gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RcvFlag = ENJI_RCV_FLAG_OK;
#ifdef DEBUG_LCCU_IMMU2_COMM
				if(gEvChargeInfo.u8RcvYdn23Flag_UART1 == RcvYdn23Flag_Init)
				{
					gEvChargeInfo.u8RcvYdn23Flag_UART1 = RcvYdn23Flag_NET;
				}
#else
				/*if(gEvChargeInfo.u8RcvYdn23Flag == RcvYdn23Flag_Init)
				{
					gEvChargeInfo.u8RcvYdn23Flag = RcvYdn23Flag_NET;
				}*/
#endif
			}

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}


/************************************************************************************************************************
	-1  没有读到数据; 
	0  找到所需要的数值
	>0  则认为是有数值  但是没找到所需要的数据
*************************************************************************************************************************/
INT32 GetValidYunShanData_2(UINT16 u16CmdID)
{
	UINT16 u8GetTimes = 0;
	UINT16 u16TotalLength = 0;
	UINT16 u16NextStartIdx = 0;
	UINT8 u8Rst = FALSE, i;
	UINT16 u16StepL = 0;
	INT8 i8Rst = BUF_NO_DATA;
	//INT8 i, BufNo;

	for (i = 0; i < 1; i++)
	{
		memset(gNetR_Buf, 0, NET_MAX_BUF/2);
		u16TotalLength = NET_Read(&gNetR_Buf);

		gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
		if (u16TotalLength >= 9)
		{
			//gNET_TCP_Info.u32BreakCount =0;
			
			if(i8Rst != FIND_SPECIFIED_CMD)
			{
				i8Rst = BUF_HAS_DADA;//大于0  则认为是有数值的
			}

			for (u8GetTimes = 0; u8GetTimes < 3; u8GetTimes++)					//不可能有 4 包数据没收
			{
				u8Rst = GetOnePkg(u16TotalLength, &u16NextStartIdx, &u16StepL);
				if (TRUE == u8Rst && (u16StepL >= 9))
				{
					gs_EthAppCommMng.i16CommFailTimes = 0;
					//RTN = u8UnPackXXXCmd(u16StepL, u16CmdID);				
					
					//if (TRUE == RTN )
					{
						i8Rst = FIND_SPECIFIED_CMD;//0 找到所需要的数值;
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			//缓存中无数据！！！
			if(i8Rst != FIND_SPECIFIED_CMD)
			{
				i8Rst = BUF_NO_DATA;
			}
			break;
		}

		//OSTimeDly(3);
	}

	return i8Rst;
}


/***************************************************************************************************************
	增加该函数的原因为，发现有SOCKET建立OK，但是应用程序就收不到数据，
	而底层还能收到>0的数值，所以要在APP端加个狗！
*****************************************************************************************************************/
void NetPipeWHDOG(void)
{
	gEvChargeInfo.i32APP_PipeWTDOG = 0;
	gEvChargeInfo.u8TelaidianHostCommAlm = FALSE;
	return;
}

extern UCHAR g_ucDHCPAddressPrepared;
static INT8 PipeIsOk(void)
{
	static UINT16 su8Delay = 0;
	if (!CHK_CONN_STAT(gNET_TCP_Info.u8MachineStat))               
	{
		
		gNET_TCP_Info.u8NetIsBreakFlag = TRUE;
		//Put_Ether_3CSem(POWER_ON_3C);
		//if ( (gEvChargeInfo.i32OffLineTimeCount <= 0 && (gEvChargeInfo.u8CARD_OffLineFlag != TRUE))	//进来的时候是网络正常 刷卡进来
		//	&& (gEvChargeInfo.u8NET_OffLine_Charge_Enable == FALSE))								//离线充电使能的情况下可以继续充电！！
		//{
		//	if (START_CHARGE_VAL == gEvChargeInfo.uUIXXXCmd
		//		&& 2 == gEvChargeInfo.uRunPhase
		//		&& (gEvChargeInfo.u8ChargeLogInMode == EV_ENTER_APP
		//		|| gEvChargeInfo.u8ChargeLogInMode == EV_ENTER_IC_CARD))
		//	{
		//		SetChargeSTOP_Cmd(NET_BREAK_STOP);												//网络中断 停机
		//		gEvChargeInfo.u8EV_Charge_RecordTimes = 0;
		//		gEvChargeInfo.u8SendChargeRecordFlag = FALSE;									//断网
		//		gEvChargeInfo.u8SendStepKwh0X77Ok = FALSE;
		//	}
		//}
		//离线禁止充电处理
		if(gEvChargeInfo.u8NET_OffLine_Charge_Enable == FALSE)
		{
			if(gEvChargeInfo.u32OffLineTimeCount == 0)
			{
				gEvChargeInfo.u32OffLineTimeCount = 1;
			}
			else if(gEvChargeInfo.u32OffLineTimeCount > gEvChargeInfo.fMaxOffLineTime)
			{
				if ((gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == START_CHARGE_VAL)
					&& (gEvChargeInfo.u8_StartReason[GUN_NO_A] == EV_ENTER_APP /*|| gEvChargeInfo.u8_StartReason[GUN_NO_A] == EV_ENTER_IC_CARD*/ ))
				{
					/*A_SetChargeSTOP_Cmd(NET_BREAK_STOP);*/
				}

				if ((gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == START_CHARGE_VAL)
					&& (gEvChargeInfo.u8_StartReason[GUN_NO_B] == EV_ENTER_APP /*|| gEvChargeInfo.u8_StartReason[GUN_NO_B] == EV_ENTER_IC_CARD*/))
				{
					/*B_SetChargeSTOP_Cmd(NET_BREAK_STOP);*/
				}
			}
		}
		else
		{	
			
		}
		gEvChargeInfo.u8NetCommAlm = TRUE;

		su8Delay++;
		if (su8Delay > 100)
		{
			//NetPipeWHDOG();
			NET_Clean_SBuf();
			CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);	
			gNET_TCP_Info.iNetCreateSocketFlag = TRUE;	//需要重置，让底层线程重新建立链接！
			g_ucRemoteServerIpIsOk = 0;
			resolv_init();
			su8Delay = 0;
			
		}
		//网络心跳 周期 相关参数

		//重新申请DHCP地址
		if((g_ucDHCPAddressPrepared == mDHCP_CONFIGURED)
			&& (gs_EthAppCommMng.ui16DHCPReconfigTimeOUT > 600))   //10分钟
		{
			gNET_TCP_Info.u8DHCPResult = FALSE;
			g_ucDHCPAddressPrepared = mDHCP_NEED_INIT;
			gs_EthAppCommMng.ui16DHCPReconfigTimeOUT  = 0;
		}

		OSTimeDly(100);

		return FALSE;
	}
	else
	{
		gEvChargeInfo.u8NetCommAlm = FALSE;
		gEvChargeInfo.u32OffLineTimeCount = 0;
		OSTimeDly(2);
		su8Delay = 0;
		gs_EthAppCommMng.ui16DHCPReconfigTimeOUT  = 0;
		return TRUE;
	}
}
/***************************************************************************************************************
	增加该函数的原因为，发现有SOCKET建立OK，但是应用程序就收不到数据，
	而底层还能收到>0的数值，所以要在APP端加个狗！
*****************************************************************************************************************/
void NetPipeCheckWHDOG(void)
{
	//网口 软看门狗。	如果EN久没有收到网络数据则 需要复位网络 重新链接！！！
	//gEvChargeInfo.i32APP_PipeWTDOG++;
	if (gEvChargeInfo.i32APP_PipeWTDOG >= 60)  //3分钟

	{
		gNET_TCP_Info.iNetCreateSocketFlag = TRUE;
		gEvChargeInfo.i32APP_PipeWTDOG = 0;
		CLR_CONN_STAT(gNET_TCP_Info.u8MachineStat);
		CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);

		gEvChargeInfo.u8TelaidianHostCommAlm = TRUE;
	}
	return;
}


//static UINT8 u8UnPackXXXCmd(UINT16 u16StepL, UINT16 u16CmdID)
//{
//	return 0;
//}

void TASK_NET_ZhixinProc(void* pdata)
{
	int iRst;
	UINT8 u8SocktIsCreat = FALSE;

	gNET_TCP_Info.u8DHCPResult = FALSE;
	
	OSTimeDly(500);
	OSTimeDly(500);
	OSTimeDly(500);
	InitNetData();

	Init_IMMU2_LCCU();

	gEvChargeInfo.u8RcvYdn23Flag_UART1 = RcvYdn23Flag_NET;

	for(;;)
	{
	

		//gEvChargeInfo.u8BootVersion = BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k;

		gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
		//RefreshEtherNetTime();

		if(gMeasureInfo.ucAddressDelay <5)
		{
			OSTimeDly(100);
			continue;
		}

		NetPipeCheckWHDOG();

		if (gEvChargeInfo.u8NET_Mask == TRUE)
		{
			gs_EthAppCommMng.ui16DHCPReconfigTimeOUT = 0;

			gEvChargeInfo.u8TelaidianHostCommAlm = FALSE;
			OSTimeDly(100);
			continue;
		}

		if (!PipeIsOk())													//管道是否建立OK
		{
//			gs_EthAppCommMng.i32EV_FLASH_RecordDelay = 1 * 60;
			u8SocktIsCreat = FALSE;
			continue;

		}
		else
		{
			if (FALSE == u8SocktIsCreat)
			{
				u8SocktIsCreat = TRUE;
				gs_EthAppCommMng.u8TickFlag = FALSE;

				//RegistS106R105();				//立即发送登录！
				gs_EthAppCommMng.i32RegistCount = 30;
			}	
		}

		gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
		iRst = GetValidYunShanData_2(0);
		if (iRst == BUF_HAS_DADA)
		{
			NetPipeWHDOG();
		}
		else if (iRst == FIND_SPECIFIED_CMD)
		{
			NetPipeWHDOG();			
		}

		Task_Immu2LccuCommDeal();


		OSTimeDly(10);
    }
}
