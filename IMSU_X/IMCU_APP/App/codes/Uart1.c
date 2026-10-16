#include "main.h"
#include "Uart6_new.h"
//#define DEBUT_BRUSH_NO_ALM 0

#include "bsp_dido.h"

#define ABS(a)	(((a) < 0) ? (-(a)) : (a))
NOINIT UINT8 WIFI_Buf[UART1_SEND_MAX_NUM];
NOINIT UINT8 DebugBuff[UART1_SEND_MAX_NUM];
#define NO_NEED_BALANCE				0
#define NEED_BALANCE_LAST_TIME		1
#define NEED_BALANCE_THIS_TIME		2
#define NOT_THE_SAME_CARD			3
#define MONEYNOTENOUGH			4

NOINIT UINT8 gUART1SendYDN23Data[UART1_SEND_MAX_NUM];
NOINIT UINT8 gUART1SendData[UART1_SEND_MAX_NUM];
NOINIT UINT8 gUART1SendBuff[UART1_SEND_MAX_NUM];
NOINIT UINT8 gUART1RecvData[UART1_RECV_MAX_NUM];
NOINIT Rs232DebufInfo gDebugInfoBuf[DEBUG_BUF_ARRY_MAX_NUM +1];
NOINIT UINT8 gu8_R_post;
NOINIT UINT8 gu8_W_post;
NOINIT UINT8 gTempAnalyseData[UART1_RECV_MAX_NUM];
NOINIT UINT8 gSection_0_Banck_0[32];
NOINIT UINT8 gLastSection_0_Banck_0[32];
NOINIT UINT8 gLastSection_5_Banck_0[32];
NOINIT UINT8 gSection_1_Banck_0[32];
NOINIT UINT8 gSection_5_Banck_0[32];
NOINIT UINT8 gSection_3_Banck_0[32];
NOINIT BRUSH_INFO gBrushInfo;
NOINIT UINT16 gWifiReceiveFlag;
NOINIT UINT16 gWifiReceive_2_Flag;

#define WIFI_IS_REGIST	1
#define WIFI_ISNT_REGIST	0

UINT8 u8CheckCARD_No(void);



void ClrS_R_Buff(UINT8 uIsRcvFlag);
UINT8 WaitReceiveOK(void);
UINT8 CRT603_ReadCardStat(UINT8* byStat);
BOOL CRT603_CheckBCC(UINT8* byRcvBuf, int iLength,UINT8* byOutBCC);
BOOL CRT603_CheckRecvBCC(UINT8* byRcvBuf, int iLength,UINT8* byOutBCC);
BOOL CRT603_S50_ReadMoney(float* pfVal,int SectorNo,int BlankNo);
BOOL CRT603_Beep(int iTime,UINT8* byStat);
//UINT8 CRT603_S50_WriteMoney(float fMoney,UINT8 SectorNo,UINT8 BlankNo);
UINT8 WaitReceiveTESTOK(void);
UINT8 CRT603_S50_WriteBank(UINT8 *pBuff, UINT8 SectorNo, UINT8 BlankNo);
BOOL CRT603_S50_ReadBank(UINT8* pBuff,int SectorNo,int BlankNo);

UINT8 MT_CRT_ReadCardStat(UINT8* byStat);
UINT8 MT_CRT_S50_WriteBank(UINT8 *pBuff, UINT8 SectorNo, UINT8 BlankNo);
BOOL MT_CRT_S50_ReadBank(UINT8* pBuff,int SectorNo,int BlankNo);

BOOL MT_S50_ReadBank(UINT8* pBuff,int SectorNo,int BlankNo);
UINT8 MT_S50_WriteBank(UINT8 *pBuff, UINT8 SectorNo, UINT8 BlankNo);
UINT8 MT_ReadCardStat(UINT8* byStat);
BOOL MT_CheckBCC(UINT8* byRcvBuf, int iLength,UINT8* byOutBCC);
UINT8 MT_WaitReceiveOK(void);
BOOL MT_CheckRecvBCC(UINT8* byRcvBuf, int iLength,UINT8* byOutBCC);
static UINT8 MT_Check_KeyA(UINT8* pBuff,int SectorNo,int BlankNo);
void ExtractYDN23DataFromBuf(UINT8 u8RArrIdx);
//UINT8 Analyse_2_YdnData(UINT8 uNo, UINT8 * u8PData);
//void ChargingAuthorization_CMD42(UINT8 uNo,UINT8 u8GunNo);
union _unfloat
{
	float fValue;
	UINT8 byData[4];
};
typedef union _unfloat UnFloat;

#define BRUSH_MONEY_L	0x04
//static void RefreshUart1Time(void)
//{
//	struct tm  tmNow;
//	static UCHAR  Sec = 0;
//	gRTCData.fGetLocalTime(&tmNow);
//	if((UCHAR)tmNow.tm_sec != Sec)/*1秒钟*/
//	{
//		Sec = (UCHAR)tmNow.tm_sec;
//		if(gBrushInfo.i32BrushInterval > 0)
//		{
//			gEvChargeInfo.uCardInterval--;
//		}
//	}
//}

void ClrS_R_Buff(UINT8 uIsRcvFlag)
{	
	UINT16 uIdx = 0;
	if (uIsRcvFlag)
	{
		dv_Uart1.pData = 0;
		for (uIdx = 0; uIdx < UART1_RECV_MAX_NUM; uIdx++)
		{
			gUART1RecvData[uIdx] = 0;
		}
	}
	else
	{
	}
}
BOOL CRT603_CheckBCC(UINT8* byRcvBuf, int iLength,UINT8* byOutBCC)
{
	UINT8	i = 0;
	UINT8	byBCC = 0;

	byBCC = ((*(byRcvBuf)));

	for (i = 1; i < iLength; i++)
	{
		byBCC ^= (*(byRcvBuf + i));
	}

	(*byOutBCC) = byBCC;
	if ((*(byRcvBuf + iLength)) == byBCC)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CRT603_CheckRecvBCC(UINT8* byRcvBuf, int iLength,UINT8* byOutBCC)
{
	UINT8	i = 0;
	UINT8	byBCC = 0;

	byBCC = ((*(byRcvBuf)));

	for (i = 1; i < iLength; i++)
	{
		byBCC ^= (*(byRcvBuf + i));
	}

	(*byOutBCC) = byBCC;
	if ((*(byRcvBuf + iLength)) == byBCC)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT8 WaitReceiveOK(void)
{
	UINT8 uIdx = 0;
	for (uIdx = 0; uIdx < 30;uIdx++)
	{
		OSTimeDly(15);
		gSelfTest.fSetTaskID(Task_ID_Uart1);
		if (gUART1RecvData[0] > 0)
		{
			if (dv_Uart1.pData >= gUART1RecvData[0] + 1)
			{
				return TRUE;
			}
		}
	}
	return FALSE;					//time OUT!!
}

UINT8 WaitReceiveTESTOK(void)
{
	UINT8 uIdx = 0;
	for (uIdx = 0; uIdx < 30;uIdx++)
	{
		OSTimeDly(15);
		gSelfTest.fSetTaskID(Task_ID_Uart1);
		if (dv_Uart1.pData >= 3)
		{
			return TRUE;
		}
	}
	return FALSE;					//time OUT!!
}

UINT8 CRT603_ReadCardStat(UINT8* byStat)
{
	UINT8	BCC = 0;
	gUART1SendData[0] = 0x03;
	gUART1SendData[1] = 0x20;
	gUART1SendData[2] = 0x00;
	CRT603_CheckBCC(gUART1SendData, 3, &BCC);
	gUART1SendData[3] = BCC;
	ClrS_R_Buff(TRUE);
	dv_Uart1.fSendDataTXY(gUART1SendData, 4);

	if (WaitReceiveOK())
	{
		if (CRT603_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
		{
			if (gUART1SendData[1] == gUART1RecvData[1])
			{
				(*byStat) = 'Y';
				return TRUE;
			}
			else if (gUART1SendData[1] == ((~gUART1RecvData[1])&0xff) )
			{
				(*byStat) = 'N';
				return TRUE;
			}
			else
			{
				(*byStat) = 'Z';
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}



BOOL CRT603_S50_ReadMoney(float* pfVal,int SectorNo,int BlankNo)
{
	int	i = 0;
	UnFloat	unFloat;
	BYTE	BCC = 0;
	int	iBlockNo = 0;
	int	iDataLength = 0;
	int	iKey_A1 = 0x5a;
	int	iKey_A2 = 0xa5;
	int	iKey_A3 = 0x5a;
	int	iKey_A4 = 0xa5;
	int	iKey_A5 = 0x5a;
	int	iKey_A6 = 0xa5;
	iDataLength = 0;
	gUART1SendData[0] = 0x0a;
	iDataLength++;
	gUART1SendData[1] = 0x21;
	iDataLength++;
	gUART1SendData[2] = 0x00;//密钥ID0  KEYA
	iDataLength++;
	iBlockNo = SectorNo * 4 + BlankNo;
	gUART1SendData[3] = iBlockNo;//块号！
	iDataLength++;
	gUART1SendData[4] = iKey_A1;
	iDataLength++;
	gUART1SendData[5] = iKey_A2;
	iDataLength++;
	gUART1SendData[6] = iKey_A3;
	iDataLength++;
	gUART1SendData[7] = iKey_A4;
	iDataLength++;
	gUART1SendData[8] = iKey_A5;
	iDataLength++;
	gUART1SendData[9] = iKey_A6;
	iDataLength++;
	CRT603_CheckBCC(gUART1SendData,iDataLength,&BCC);
	gUART1SendData[10] = BCC;
	iDataLength++;
	ClrS_R_Buff(TRUE);
	dv_Uart1.fSendDataTXY(gUART1SendData, iDataLength);
	if (WaitReceiveOK())
	{
		if (CRT603_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
		{
			if (gUART1SendData[1] == gUART1RecvData[1])
			{
				for (i = 0; i < BRUSH_MONEY_L; i++)
				{
					unFloat.byData[BRUSH_MONEY_L -1 - i] = gUART1RecvData[2+i];
				}
				*pfVal = unFloat.fValue;
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
	return FALSE;
}


BOOL CRT603_S50_ReadBank(UINT8* pBuff,int SectorNo,int BlankNo)
{
	int	i = 0;
	BYTE	BCC = 0;
	int	iBlockNo = 0;
	int	iDataLength = 0;
	
	int	iKey_A1 = 0;
	int	iKey_A2 = 0;
	int	iKey_A3 = 0;
	int	iKey_A4 = 0;
	int	iKey_A5 = 0;
	int	iKey_A6 = 0;

	if (0 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT0;
		iKey_A2 = gBrushInfo.iKey_A2_SECT0;
		iKey_A3 = gBrushInfo.iKey_A3_SECT0;
		iKey_A4 = gBrushInfo.iKey_A4_SECT0;
		iKey_A5 = gBrushInfo.iKey_A5_SECT0;
		iKey_A6 = gBrushInfo.iKey_A6_SECT0;
	}
	else if (3 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT3;
		iKey_A2 = gBrushInfo.iKey_A2_SECT3;
		iKey_A3 = gBrushInfo.iKey_A3_SECT3;
		iKey_A4 = gBrushInfo.iKey_A4_SECT3;
		iKey_A5 = gBrushInfo.iKey_A5_SECT3;
		iKey_A6 = gBrushInfo.iKey_A6_SECT3;
	}
	else if (1 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT1;
		iKey_A2 = gBrushInfo.iKey_A2_SECT1;
		iKey_A3 = gBrushInfo.iKey_A3_SECT1;
		iKey_A4 = gBrushInfo.iKey_A4_SECT1;
		iKey_A5 = gBrushInfo.iKey_A5_SECT1;
		iKey_A6 = gBrushInfo.iKey_A6_SECT1;
	}
	else if (5 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT5;
		iKey_A2 = gBrushInfo.iKey_A2_SECT5;
		iKey_A3 = gBrushInfo.iKey_A3_SECT5;
		iKey_A4 = gBrushInfo.iKey_A4_SECT5;
		iKey_A5 = gBrushInfo.iKey_A5_SECT5;
		iKey_A6 = gBrushInfo.iKey_A6_SECT5;
	}

	iDataLength = 0;
	gUART1SendData[0] = 0x0a;
	iDataLength++;
	gUART1SendData[1] = 0x21;
	iDataLength++;
	gUART1SendData[2] = 0x00;//密钥ID0  KEYA
	iDataLength++;
	iBlockNo = SectorNo * 4 + BlankNo;
	gUART1SendData[3] = iBlockNo;//块号！
	iDataLength++;
	gUART1SendData[4] = iKey_A1;
	iDataLength++;
	gUART1SendData[5] = iKey_A2;
	iDataLength++;
	gUART1SendData[6] = iKey_A3;
	iDataLength++;
	gUART1SendData[7] = iKey_A4;
	iDataLength++;
	gUART1SendData[8] = iKey_A5;
	iDataLength++;
	gUART1SendData[9] = iKey_A6;
	iDataLength++;
	CRT603_CheckBCC(gUART1SendData,iDataLength,&BCC);
	gUART1SendData[10] = BCC;
	iDataLength++;
	ClrS_R_Buff(TRUE);
	dv_Uart1.fSendDataTXY(gUART1SendData, iDataLength);
	if (WaitReceiveOK())
	{
		if (CRT603_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
		{
			if (gUART1SendData[1] == gUART1RecvData[1])
			{
				for (i = 0; i < 16; i++)
				{
					pBuff[i] = gUART1RecvData[2 + i];
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
	return FALSE;
}

UINT8 CRT603_S50_WriteMoney(float fMoney,UINT8 SectorNo,UINT8 BlankNo)
{
	int	i = 0;
	UnFloat	unFloat;
	BYTE	BCC = 0;
	//UINT8	uRstResult = 0;
	UINT8	iDataLength = 0;
	UINT8	iKey_A1 = 0x5a;
	UINT8	iKey_A2 = 0xa5;
	UINT8	iKey_A3 = 0x5a;
	UINT8	iKey_A4 = 0xa5;
	UINT8	iKey_A5 = 0x5a;
	UINT8	iKey_A6 = 0xa5;
	UNUSED(fMoney);

	UINT8 u8MoneyBuf[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	if (gBrushInfo.fCARDLeaveMoney > gBrushInfo.fCostMoney)
	{
		//CARDLeaveMoney  读和写是顺序执行，所以不需要互斥不用考虑
		unFloat.fValue = gBrushInfo.fCARDLeaveMoney - gBrushInfo.fCostMoney;
	}
	else
	{
		return FALSE;
	}

	for (i = 0; i < BRUSH_MONEY_L; i++)
	{
		u8MoneyBuf[i] = unFloat.byData[BRUSH_MONEY_L - 1 - i];
	}

	iDataLength = 0;
	gUART1SendData[0] = 0x1a;
	iDataLength++;
	gUART1SendData[1] = 0x22;
	iDataLength++;
	gUART1SendData[2] = 0x00;					//密钥ID
	iDataLength++;
	gUART1SendData[3] = (SectorNo * 4) + BlankNo;			//块号
	iDataLength++;
	gUART1SendData[4] = iKey_A1;
	iDataLength++;
	gUART1SendData[5] = iKey_A2;
	iDataLength++;
	gUART1SendData[6] = iKey_A3;
	iDataLength++;
	gUART1SendData[7] = iKey_A4;
	iDataLength++;
	gUART1SendData[8] = iKey_A5;
	iDataLength++;
	gUART1SendData[9] = iKey_A6;
	iDataLength++;
	for (i = 0; i < 16; i++)
	{
		gUART1SendData[10 + i] = u8MoneyBuf[i];
		iDataLength++;
	}
	CRT603_CheckBCC(gUART1SendData, iDataLength, &BCC);
	gUART1SendData[iDataLength] = BCC;
	iDataLength++;

	ClrS_R_Buff(TRUE);
	dv_Uart1.fSendDataTXY(gUART1SendData, iDataLength);
	if (WaitReceiveOK())
	{
		if (CRT603_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
		{
			if (gUART1SendData[1] == gUART1RecvData[1])
			{
				//CRT603_Beep(100, &uRstResult);
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
	return FALSE;
}

UINT8 CRT603_S50_WriteBank(UINT8 *pBuff, UINT8 SectorNo, UINT8 BlankNo)
{
	int	i = 0;
	//UnFloat unFloat;
	BYTE	BCC = 0;
	UINT8	iDataLength = 0;
	UINT8	iKey_A1 = 0xff;
	UINT8	iKey_A2 = 0xff;
	UINT8	iKey_A3 = 0xff;
	UINT8	iKey_A4 = 0xff;
	UINT8	iKey_A5 = 0xff;
	UINT8	iKey_A6 = 0xff;

	if (0 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT0;
		iKey_A2 = gBrushInfo.iKey_A2_SECT0;
		iKey_A3 = gBrushInfo.iKey_A3_SECT0;
		iKey_A4 = gBrushInfo.iKey_A4_SECT0;
		iKey_A5 = gBrushInfo.iKey_A5_SECT0;
		iKey_A6 = gBrushInfo.iKey_A6_SECT0;
	}
	else if (3 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT3;
		iKey_A2 = gBrushInfo.iKey_A2_SECT3;
		iKey_A3 = gBrushInfo.iKey_A3_SECT3;
		iKey_A4 = gBrushInfo.iKey_A4_SECT3;
		iKey_A5 = gBrushInfo.iKey_A5_SECT3;
		iKey_A6 = gBrushInfo.iKey_A6_SECT3;
	}
	else if (5 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT5;
		iKey_A2 = gBrushInfo.iKey_A2_SECT5;
		iKey_A3 = gBrushInfo.iKey_A3_SECT5;
		iKey_A4 = gBrushInfo.iKey_A4_SECT5;
		iKey_A5 = gBrushInfo.iKey_A5_SECT5;
		iKey_A6 = gBrushInfo.iKey_A6_SECT5;
	}

	UINT8 u8MoneyBuf[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	for (i = 0; i < 16; i++)
	{
		u8MoneyBuf[i] = pBuff[i];
	}

	iDataLength = 0;
	gUART1SendData[0] = 0x1a;
	iDataLength++;
	gUART1SendData[1] = 0x22;
	iDataLength++;
	gUART1SendData[2] = 0x00;					//密钥ID
	iDataLength++;
	gUART1SendData[3] = (SectorNo * 4) + BlankNo;			//块号
	iDataLength++;
	gUART1SendData[4] = iKey_A1;
	iDataLength++;
	gUART1SendData[5] = iKey_A2;
	iDataLength++;
	gUART1SendData[6] = iKey_A3;
	iDataLength++;
	gUART1SendData[7] = iKey_A4;
	iDataLength++;
	gUART1SendData[8] = iKey_A5;
	iDataLength++;
	gUART1SendData[9] = iKey_A6;
	iDataLength++;
	for (i = 0; i < 16; i++)
	{
		gUART1SendData[10 + i] = u8MoneyBuf[i];
		iDataLength++;
	}
	CRT603_CheckBCC(gUART1SendData, iDataLength, &BCC);
	gUART1SendData[iDataLength] = BCC;
	iDataLength++;

	ClrS_R_Buff(TRUE);
	dv_Uart1.fSendDataTXY(gUART1SendData, iDataLength);
	if (WaitReceiveOK())
	{
		if (CRT603_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
		{
			if (gUART1SendData[1] == gUART1RecvData[1])
			{
				//CRT603_Beep(100, &uRstResult);
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
	return FALSE;
}
#define NO_NEED_PROCESS   0
#define NO_NEED_WRITE	  1
#define NEED_WRITE	    2
//static UINT8 u8NeedCostMoney(void)
//{
//	//if (gEvChargeInfo.u8CardNeedCostFlag == CARD_NEED_WRITE)
//	//{
//	//	return NEED_WRITE;
//	//}
//	//else
//	//{
//	//	return NO_NEED_WRITE;
//	//}
//	
//}

BOOL CRT603_Beep(int iTime,UINT8* byStat)//iTime 10MS
{
	BYTE	BCC = 0;
	UINT8	iLength = 0;
	BCC = 0;
	iLength = 0;

	gUART1SendData[0] = 0x03;
	iLength++;
	gUART1SendData[1] = 0x14;
	iLength++;
	gUART1SendData[2] = iTime;					//模式
	iLength++;
	CRT603_CheckBCC(gUART1SendData,iLength,&BCC);
	gUART1SendData[3] = BCC;
	iLength++;
	ClrS_R_Buff(TRUE);
	dv_Uart1.fSendDataTXY(gUART1SendData, 4);

	if (WaitReceiveOK())
	{
		if (CRT603_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
		{
			if (gUART1SendData[1] == gUART1RecvData[1])
			{
				(*byStat) = 'Y';
				return TRUE;
			}
			else if (gUART1SendData[1] == ((~gUART1RecvData[1])&0xff) )
			{
				(*byStat) = 'N';
				return TRUE;
			}
			else
			{
				(*byStat) = 'Z';
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}

UINT8 TEST_Mode_Uart_Proc(UINT8 uUartNo)
{
	return FALSE;
}


#define MT_STX_VAL 0x02
#define MT_ETX_VAL 0x03
BOOL MT_CheckBCC(UINT8* byRcvBuf, int iLength,UINT8* byOutBCC)
{
	UINT8	i = 0;
	UINT8	byBCC = 0;

	byBCC = ((*(byRcvBuf)));

	for (i = 1; i < iLength; i++)
	{
		byBCC ^= (*(byRcvBuf + i));
	}

	(*byOutBCC) = byBCC;
	if ((*(byRcvBuf + iLength)) == byBCC)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


UINT8 MT_ReadCardStat(UINT8* byStat)
{
	UINT8	BCC = 0;
	gUART1SendData[0] = MT_STX_VAL;
	gUART1SendData[1] = 0x00;
	gUART1SendData[2] = 0x02;
	gUART1SendData[3] = 0x34;
	gUART1SendData[4] = 0x30;
	gUART1SendData[5] = MT_ETX_VAL;
	MT_CheckBCC(gUART1SendData, 6, &BCC);
	gUART1SendData[6] = BCC;
	ClrS_R_Buff(TRUE);
	dv_Uart1.fSendDataTXY(gUART1SendData, 7);

	if (MT_WaitReceiveOK())
	{
		if ((dv_Uart1.pData - 1) >= UART1_RECV_MAX_NUM)
		{
			dv_Uart1.pData = UART1_RECV_MAX_NUM -1;
		}
		if (MT_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
		{
			if (0x59 == gUART1RecvData[5])
			{
				(*byStat) = 'Y';
				return TRUE;
			}
			else if (0x4e == gUART1RecvData[5])
			{
				(*byStat) = 'N';
				return TRUE;
			}
			else
			{
				(*byStat) = 'Z';
				return FALSE;
			}
		}
		else
		{
			OSTimeDly(2);
			return FALSE;
		}
	}
	OSTimeDly(3);
	return FALSE;
}

UINT8 MT_WaitReceiveOK(void)
{
	UINT8 uIdx = 0;
	for (uIdx = 0; uIdx < 30;uIdx++)
	{
		OSTimeDly(25);
		gSelfTest.fSetTaskID(Task_ID_Uart1);
		if (gUART1RecvData[0] == MT_STX_VAL)
		{
			if (dv_Uart1.pData >= 3)
			{
				if (dv_Uart1.pData >= (((gUART1RecvData[1] << 8) + gUART1RecvData[2]) + 4))
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;					//time OUT!!
}

BOOL MT_CheckRecvBCC(UINT8* byRcvBuf, int iLength,UINT8* byOutBCC)
{
	UINT8	i = 0;
	UINT8	byBCC = 0;

	byBCC = ((*(byRcvBuf)));

	for (i = 1; i < iLength; i++)
	{
		byBCC ^= (*(byRcvBuf + i));
	}

	(*byOutBCC) = byBCC;
	if ((*(byRcvBuf + iLength)) == byBCC)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


UINT8 MT_CRT_S50_WriteBank(UINT8 *pBuff, UINT8 SectorNo, UINT8 BlankNo)
{
	if (0 == gEvChargeInfo.u8BrushFactorType)	//CRT603
	{
		return CRT603_S50_WriteBank(pBuff, SectorNo, BlankNo);
	}
	else
	{
		return MT_S50_WriteBank(pBuff, SectorNo, BlankNo);
	}
}

BOOL MT_CRT_S50_ReadBank(UINT8* pBuff,int SectorNo,int BlankNo)
{
	if (0 == gEvChargeInfo.u8BrushFactorType)	//CRT603
	{
		return CRT603_S50_ReadBank(pBuff, SectorNo, BlankNo);
	}
	else
	{
		return MT_S50_ReadBank(pBuff, SectorNo, BlankNo);
	}
}


UINT8 MT_CRT_ReadCardStat(UINT8* byStat)
{
	UINT8 uRst = 0;
	UINT8 u8Stat;
	if (0 == gEvChargeInfo.u8BrushFactorType)	//CRT603
	{
		uRst = CRT603_ReadCardStat(&u8Stat);
	}
	else
	{
		uRst = MT_ReadCardStat(&u8Stat);
	}
	(*byStat) = u8Stat;
	return uRst;
}

static UINT8 MT_Check_KeyA(UINT8* pBuff,int SectorNo,int BlankNo)
{
	//int	i = 0;
	BYTE	BCC = 0;
	//int	iBlockNo = 0;
	int	iDataLength = 0;

	int	iKey_A1 = 0;
	int	iKey_A2 = 0;
	int	iKey_A3 = 0;
	int	iKey_A4 = 0;
	int	iKey_A5 = 0;
	int	iKey_A6 = 0;

	if (0 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT0;
		iKey_A2 = gBrushInfo.iKey_A2_SECT0;
		iKey_A3 = gBrushInfo.iKey_A3_SECT0;
		iKey_A4 = gBrushInfo.iKey_A4_SECT0;
		iKey_A5 = gBrushInfo.iKey_A5_SECT0;
		iKey_A6 = gBrushInfo.iKey_A6_SECT0;
	}
	else if (3 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT3;
		iKey_A2 = gBrushInfo.iKey_A2_SECT3;
		iKey_A3 = gBrushInfo.iKey_A3_SECT3;
		iKey_A4 = gBrushInfo.iKey_A4_SECT3;
		iKey_A5 = gBrushInfo.iKey_A5_SECT3;
		iKey_A6 = gBrushInfo.iKey_A6_SECT3;
	}
	else if (1 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT1;
		iKey_A2 = gBrushInfo.iKey_A2_SECT1;
		iKey_A3 = gBrushInfo.iKey_A3_SECT1;
		iKey_A4 = gBrushInfo.iKey_A4_SECT1;
		iKey_A5 = gBrushInfo.iKey_A5_SECT1;
		iKey_A6 = gBrushInfo.iKey_A6_SECT1;
	}
	else if (5 == SectorNo)
	{
		iKey_A1 = gBrushInfo.iKey_A1_SECT5;
		iKey_A2 = gBrushInfo.iKey_A2_SECT5;
		iKey_A3 = gBrushInfo.iKey_A3_SECT5;
		iKey_A4 = gBrushInfo.iKey_A4_SECT5;
		iKey_A5 = gBrushInfo.iKey_A5_SECT5;
		iKey_A6 = gBrushInfo.iKey_A6_SECT5;
	}

	iDataLength = 0;
	gUART1SendData[0] = MT_STX_VAL;
	iDataLength++;
	gUART1SendData[1] = 0x00;
	iDataLength++;
	gUART1SendData[2] = 0x09;
	iDataLength++;
	gUART1SendData[3] = 0x34;
	iDataLength++;
	gUART1SendData[4] = 0x32;
	iDataLength++;
	gUART1SendData[5] = SectorNo;//块号！
	iDataLength++;
	gUART1SendData[6] = iKey_A1;
	iDataLength++;
	gUART1SendData[7] = iKey_A2;
	iDataLength++;
	gUART1SendData[8] = iKey_A3;
	iDataLength++;
	gUART1SendData[9] = iKey_A4;
	iDataLength++;
	gUART1SendData[10] = iKey_A5;
	iDataLength++;
	gUART1SendData[11] = iKey_A6;
	iDataLength++;
	gUART1SendData[12] = MT_ETX_VAL;
	iDataLength++;
	MT_CheckBCC(gUART1SendData,iDataLength,&BCC);
	gUART1SendData[13] = BCC;
	iDataLength++;
	ClrS_R_Buff(TRUE);
	dv_Uart1.fSendDataTXY(gUART1SendData, iDataLength);
	if (MT_WaitReceiveOK())
	{
		if ((dv_Uart1.pData - 1) >= UART1_RECV_MAX_NUM)
		{
			dv_Uart1.pData = UART1_RECV_MAX_NUM -1;
		}
		if (MT_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
		{
			if (0x59 == gUART1RecvData[6])
			{
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
	return FALSE;
}


BOOL MT_S50_ReadBank(UINT8* pBuff,int SectorNo,int BlankNo)
{
	int	i = 0;
	BYTE	BCC = 0;
	UINT8	iDataLength = 0;
	if (MT_Check_KeyA(pBuff, SectorNo, BlankNo))
	{
		iDataLength = 0;
		gUART1SendData[0] = MT_STX_VAL;
		iDataLength++;
		gUART1SendData[1] = 0x00;
		iDataLength++;
		gUART1SendData[2] = 0x04;
		iDataLength++;
		gUART1SendData[3] = 0x34;
		iDataLength++;
		gUART1SendData[4] = 0x33;
		iDataLength++;
		gUART1SendData[5] = SectorNo;	//扇区！
		iDataLength++;
		gUART1SendData[6] = BlankNo;
		iDataLength++;
		gUART1SendData[7] = MT_ETX_VAL;
		iDataLength++;
		MT_CheckBCC(gUART1SendData,iDataLength,&BCC);
		gUART1SendData[8] = BCC;
		iDataLength++;
		ClrS_R_Buff(TRUE);
		dv_Uart1.fSendDataTXY(gUART1SendData, iDataLength);
		if (MT_WaitReceiveOK())
		{
			if ((dv_Uart1.pData - 1) >= UART1_RECV_MAX_NUM)
			{
				dv_Uart1.pData = UART1_RECV_MAX_NUM -1;
			}
			if (MT_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
			{
				if (SectorNo == gUART1RecvData[5]
					&& BlankNo == gUART1RecvData[6]
					&& 0x59 == gUART1RecvData[7])
				{
					for (i = 0; i < 16; i++)
					{
						pBuff[i] = gUART1RecvData[8 + i];
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
		return FALSE;
	}
        return FALSE;
}

UINT8 MT_S50_WriteBank(UINT8 *pBuff, UINT8 SectorNo, UINT8 BlankNo)
{
	int	i = 0;
	BYTE	BCC = 0;
	UINT8	iDataLength = 0;
	UINT8	u8MoneyBuf[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	for (i = 0; i < 16; i++)
	{
		u8MoneyBuf[i] = pBuff[i];
	}

	if (MT_Check_KeyA(pBuff, SectorNo, BlankNo))
	{
		iDataLength = 0;
		gUART1SendData[0] = MT_STX_VAL;
		iDataLength++;
		gUART1SendData[1] = 0x00;
		iDataLength++;
		gUART1SendData[2] = 0x14;	
		iDataLength++;
		gUART1SendData[3] = 0x34;		
		iDataLength++;
		gUART1SendData[4] = 0x34;		
		iDataLength++;
		gUART1SendData[5] = SectorNo;		
		iDataLength++;
		gUART1SendData[6] = BlankNo;		
		iDataLength++;
		for (i = 0; i < 16; i++)
		{
			gUART1SendData[7 + i] = u8MoneyBuf[i];
			iDataLength++;
		}
		gUART1SendData[7 + 16] = MT_ETX_VAL;
		iDataLength++;
		MT_CheckBCC(gUART1SendData, iDataLength, &BCC);
		gUART1SendData[iDataLength] = BCC;
		iDataLength++;
		ClrS_R_Buff(TRUE);
		dv_Uart1.fSendDataTXY(gUART1SendData, iDataLength);
		if (MT_WaitReceiveOK())
		{
			if ((dv_Uart1.pData - 1) >= UART1_RECV_MAX_NUM)
			{
				dv_Uart1.pData = UART1_RECV_MAX_NUM -1;
			}
			if (MT_CheckRecvBCC(gUART1RecvData, (dv_Uart1.pData - 1), &BCC))
			{
				if (SectorNo == gUART1RecvData[5]
					&& BlankNo == gUART1RecvData[6]
					&& 0x59 == gUART1RecvData[7])
				{
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
		return FALSE;
	}
	return FALSE;
}

//static void SetCardInfo(void);
//static void SetCardInfo(void)
//{
//	UINT8 u8Idx = 0;
//	struct tm  tmNow;
//	gRTCData.fGetLocalTime(&tmNow);
//	//gEvChargeInfo.CostStartTime = tmNow;
//	/*for (u8Idx = 0; u8Idx < CARD_INFO_BYTES; u8Idx++)
//	{
//		gBrushInfo.u8CARDInfo[GUN_NO_A][u8Idx] = '\0';
//	}*/
//	for (u8Idx = 0; u8Idx < CARD_NO_BYTES; u8Idx++)
//	{
//		gBrushInfo.u8TempCARDInfo[u8Idx] = '\0';
//	}
//	
//	//卡信息 1扇区
//	for (u8Idx = 0; u8Idx < 16 && (u8Idx < CARD_INFO_BYTES/2); u8Idx++)
//	{
//		gBrushInfo.u8TempCARDInfo[u8Idx*2] = Hex2Ascii((gSection_1_Banck_0[u8Idx]>>4)&0x0f);		
//		gBrushInfo.u8TempCARDInfo[(u8Idx*2)+1] = Hex2Ascii((gSection_1_Banck_0[u8Idx])&0x0f);
//
//		gLastSection_5_Banck_0[u8Idx] = gSection_5_Banck_0[u8Idx];
//	}
//	//卡号 0扇区
//	for (u8Idx = 0; u8Idx < 16 && (u8Idx < CARD_NO_BYTES/2); u8Idx++)
//	{
//		//gLastSection_0_Banck_0[u8Idx] = gSection_0_Banck_0[u8Idx];
//		gBrushInfo.u8TempCARDInfo[u8Idx*2] = Hex2Ascii((gSection_0_Banck_0[u8Idx]>>4)&0x0f);
//		gBrushInfo.u8TempCARDInfo[(u8Idx*2)+1] = Hex2Ascii((gSection_0_Banck_0[u8Idx])&0x0f);
//	}
//	return;
//}

UINT8 u8CheckCARD_No(void)
{
	UINT8 u8Idx  = 0;
	for (u8Idx = 0; u8Idx < 16; u8Idx++)
	{
		//if (gLastSection_0_Banck_0[u8Idx] != gSection_0_Banck_0[u8Idx])
		if (gLastSection_5_Banck_0[u8Idx] != gSection_5_Banck_0[u8Idx])
		{
			return FALSE;
		}
	}
	return TRUE;
}

void InitBrushInfo(void)
{
	unsigned char i;

	//gBrushInfo.iKey_A1_SECT0 = 0xff;
	//gBrushInfo.iKey_A2_SECT0 = 0xff;
	//gBrushInfo.iKey_A3_SECT0 = 0xff;
	//gBrushInfo.iKey_A4_SECT0 = 0xff;
	//gBrushInfo.iKey_A5_SECT0 = 0xff;
	//gBrushInfo.iKey_A6_SECT0 = 0xff;

	//gBrushInfo.iKey_A1_SECT1 = 0x5A;
	//gBrushInfo.iKey_A2_SECT1 = 0x2e;
	//gBrushInfo.iKey_A3_SECT1 = 0x0f;
	//gBrushInfo.iKey_A4_SECT1 = 0xab;
	//gBrushInfo.iKey_A5_SECT1 = 0xb6;
	//gBrushInfo.iKey_A6_SECT1 = 0x39;

	////  密码A：0x 8A 72 64 53 4F 3C
	////  密码B：0x 83 7D 62 59 4E 32
	//gBrushInfo.iKey_A1_SECT3 = 0x8A;
	//gBrushInfo.iKey_A2_SECT3 = 0x72;
	//gBrushInfo.iKey_A3_SECT3 = 0x64;
	//gBrushInfo.iKey_A4_SECT3 = 0x53;
	//gBrushInfo.iKey_A5_SECT3 = 0x4F;
	//gBrushInfo.iKey_A6_SECT3 = 0x3C;

	////   第5扇区密码：
	////密码A：0x3E 7F 28 AD 8C 22
	////密码B：0x27 AA 93 5D 35 09
	//gBrushInfo.iKey_A1_SECT5 = 0x3e;
	//gBrushInfo.iKey_A2_SECT5 = 0x7f;
	//gBrushInfo.iKey_A3_SECT5 = 0x28;
	//gBrushInfo.iKey_A4_SECT5 = 0xad;
	//gBrushInfo.iKey_A5_SECT5 = 0x8c;
	//gBrushInfo.iKey_A6_SECT5 = 0x22;

	gBrushInfo.iKey_A1_SECT0 = 0xff;
	gBrushInfo.iKey_A2_SECT0 = 0xff;
	gBrushInfo.iKey_A3_SECT0 = 0xff;
	gBrushInfo.iKey_A4_SECT0 = 0xff;
	gBrushInfo.iKey_A5_SECT0 = 0xff;
	gBrushInfo.iKey_A6_SECT0 = 0xff;

	gBrushInfo.iKey_A1_SECT1 = 0x5A;
	gBrushInfo.iKey_A2_SECT1 = 0x2e;
	gBrushInfo.iKey_A3_SECT1 = 0x0f;
	gBrushInfo.iKey_A4_SECT1 = 0xab;
	gBrushInfo.iKey_A5_SECT1 = 0xb6;
	gBrushInfo.iKey_A6_SECT1 = 0x39;

	//  密码A：0x 8A 72 64 53 4F 3C
	//  密码B：0x 83 7D 62 59 4E 32
	gBrushInfo.iKey_A1_SECT3 = 0x8A;
	gBrushInfo.iKey_A2_SECT3 = 0x72;
	gBrushInfo.iKey_A3_SECT3 = 0x64;
	gBrushInfo.iKey_A4_SECT3 = 0x53;
	gBrushInfo.iKey_A5_SECT3 = 0x4F;
	gBrushInfo.iKey_A6_SECT3 = 0x3C;

	//   第5扇区密码：
	//密码A：0x3E 7F 28 AD 8C 22
	//密码B：0x27 AA 93 5D 35 09
	gBrushInfo.iKey_A1_SECT5 = 0x3e;
	gBrushInfo.iKey_A2_SECT5 = 0x7f;
	gBrushInfo.iKey_A3_SECT5 = 0x28;
	gBrushInfo.iKey_A4_SECT5 = 0xad;
	gBrushInfo.iKey_A5_SECT5 = 0x8c;
	gBrushInfo.iKey_A6_SECT5 = 0x22;


	gBrushInfo.uBrushExist = FALSE;
	gBrushInfo.uBrushCommAlm = FALSE;
	gBrushInfo.fCostMoney = 0;
	//gBrushInfo.fFirstMoney = 0;
	gBrushInfo.uCARDExist = FALSE;
	gBrushInfo.i32BrushInterval = 0;

	gBrushInfo.u8AccountRequestStep = 0;
	gBrushInfo.u8BrushWaitHostConfirmTimeout = 0;

	//gBrushInfo.ucCardInfoProcessStep = Step0_No_Card_Read;
	gBrushInfo.ucCardInfoConfirmResult = CardNoProcess;
	gBrushInfo.u8CardNeedCostFlag = NO_CARD_EXIST;

	gBrushInfo.u16CardInfoIndentifyCount = 0;
	gBrushInfo.fCARDLeaveMoney =0;
	gBrushInfo.fCardMoneyFormNet = 0;

	gBrushInfo.ucCardMoneyErrIsCorrected = 0;

	for(i=0; (i<32) && (i<CARD_NO_BYTES) ;i++)
	{
		//gBrushInfo.ucUserCardNoLastTime[i] = 0; 
		gBrushInfo.u8TempCARDInfo[i]= 0; 
		gBrushInfo.u8CARDInfo[GUN_NO_A][i] = 0;
		gBrushInfo.u8CARDInfo[GUN_NO_B][i] = 0;
	}

}
static UCHAR WriteCardMoney(float fWriteMoneyValue)
{
	UINT32 u32MoneyVal = 0;
	UCHAR uctemp;
	u32MoneyVal = (UINT32)(fWriteMoneyValue*100);
	gSection_3_Banck_0[0] = u32MoneyVal & 0xff;
	gSection_3_Banck_0[1] = (u32MoneyVal >> 8)& 0xff;
	gSection_3_Banck_0[2] = (u32MoneyVal >> 16)& 0xff;
	gSection_3_Banck_0[3] = (u32MoneyVal >> 24)& 0xff;

	if (MT_CRT_S50_WriteBank(gSection_3_Banck_0,3,0))
	{
		OSTimeDly(175);
		//GetCharSetData(SETDATA_CardReaderType_NUM,&uctemp);
		if (0 == gEvChargeInfo.u8BrushFactorType)	//CRT603
		{
			CRT603_Beep(230, &uctemp);
		}
		return CARD_WRITE_OK;
	}
	else
	{
		return  CARD_WRITE_FAIL;
	}
}
//static void MoneyInCardProcess(UCHAR State)
//{
	/*UCHAR RTN;
	float ftemp;

	if(State == NEED_BALANCE_THIS_TIME)
	{
		RTN = u8NeedCostMoney();
		if (RTN == NEED_WRITE)
		{
			OSTimeDly(125);
			if (gBrushInfo.fCARDLeaveMoney > gBrushInfo.fCostMoney)
			{
				ftemp = gBrushInfo.fCARDLeaveMoney - gBrushInfo.fCostMoney;
				gBrushInfo.u8CardNeedCostFlag = WriteCardMoney(ftemp);
			}
			else
			{
				gBrushInfo.u8CardNeedCostFlag = CARD_WRITE_FAIL;
				gBrushInfo.ucCardInfoProcessStep = Step7_Card_Balance_Again;
			}
		}
		else if (RTN == NO_NEED_WRITE)
		{
			gBrushInfo.u8CardNeedCostFlag = CARD_WRITE_OK;
		}
	}
	else if(State == NEED_BALANCE_LAST_TIME)
	{
		gBrushInfo.u8CardNeedCostFlag = WriteCardMoney(gBrushInfo.fCardMoneyFormNet);
		if(gBrushInfo.u8CardNeedCostFlag == CARD_WRITE_OK)
		{
			gBrushInfo.ucCardInfoProcessStep = Step1_Read_Card_First;
		}
	}
	*/
//}

//static UCHAR ForbiddenBrushCard(void)
//{
//	if((gEvChargeInfo.u8OrderChargeCMD == TRUE)//预约不能刷卡
//		||((gEvChargeInfo.ucStartMode == START_RECEIVE_BY_NET)&&(gEvChargeInfo.uUIXXXCmd == START_CHARGE_VAL))//网络启动不能刷卡
//		||((gEvChargeInfo.ucNetALarm== TRUE)&&(gEvChargeInfo.ucCardInfoProcessStep == Step0_No_Card_Read))//有网络告警且不再刷卡充电中 
//		||((gEvChargeInfo.ucStartMode == START_FROM_UI)&&(gEvChargeInfo.uUIXXXCmd == START_CHARGE_VAL)))  //密码启动不能刷卡
//	{
//		return TRUE;
//	}
//	else
//	{
//		return FALSE;
//	}
//}


unsigned char IsNumber(unsigned char Input);
unsigned char IsNumber(unsigned char Input)
{
	if ((Input >= '0') && (Input <= '9'))
	{
		return 1;
	}
	return 0;
}

void V_printf(char* fmt,...)  
{  
	UINT16 iLen = 0; 
	va_list ap; 
	va_start(ap,fmt);
	memset(gUART1SendData, 0, UART1_SEND_MAX_NUM);
	vsprintf((char*)gUART1SendData, fmt, ap);
	va_end(ap);
	iLen=strlen((const char*)gUART1SendData);		//此次发送数据的长度
	dv_Uart1.pData = 0;
	dv_Uart1.fSendDataTXY(gUART1SendData, iLen); 
	gSelfTest.fSetTaskID(Task_ID_Uart1);
}


void atk_BW226_Check_AT_cmd(UINT8 *cmd);
void atk_BW226_Check_AT_cmd(UINT8 *cmd)
{
	V_printf("%s\r\n", cmd);
}





void BrushDeal(void);
void BrushDeal(void)
{
	UINT8 uCommResult = FALSE;
	UINT8 uRstResult = FALSE;
	UINT8 u8Rrst_1 = FALSE;
	UINT8 u8Rrst_2 = FALSE;
	static UINT8 su8AlmDelay = 0;
	static UINT8 su8CARDInExistDelay = 0;
	static UINT8 su8WriteMoneyFlag = 0;
	UINT8 u8Idx = 0;
	UNUSED(uCommResult);
	UNUSED(uRstResult);
	UNUSED(u8Rrst_1);
	UNUSED(u8Rrst_2);

	//su8AlmDelay = 0;
	uRstResult = FALSE;
	unsigned char u8Rst, u8DoneFlag, RTN;
	float ftemp;

	uCommResult = FALSE;
	su8CARDInExistDelay = 0;

	
		uCommResult = MT_CRT_ReadCardStat(&uRstResult);
		//OSTimeDly(5);
		if (TRUE == uCommResult)
		{
			//网络断，且没有刷卡充电
			/*if(ForbiddenBrushCard()== TRUE)
			{
				uRstResult = 'N';
			}*/

			su8AlmDelay = 0;
			gBrushInfo.uBrushCommAlm = FALSE;
			
			if ('Y' == uRstResult) 
			{
				su8CARDInExistDelay = 0;
				gBrushInfo.uCARDExist = TRUE;

				if(gBrushInfo.i32BrushInterval ==0)
				{
					//OSTimeDly(80);
					//做卡有效性校验！！
					u8Rrst_2 = FALSE;
					u8Rrst_2 = MT_CRT_S50_ReadBank(gSection_5_Banck_0, 5, 0);
					OSTimeDly(5);
					if (TRUE == u8Rrst_2)
					{
						//OSTimeDly(20);
						u8Rrst_1 = FALSE;
						u8Rrst_1 = MT_CRT_S50_ReadBank(gSection_0_Banck_0, 0, 0);
						if (TRUE == u8Rrst_1)
						{
							if((gSection_5_Banck_0[0] == (gSection_5_Banck_0[4]^gSection_0_Banck_0[0]))
							&& (gSection_5_Banck_0[1] == (gSection_5_Banck_0[5]^gSection_0_Banck_0[1]))
							&& (gSection_5_Banck_0[2] == (gSection_5_Banck_0[6]^gSection_0_Banck_0[2]))
							&& (gSection_5_Banck_0[3] == (gSection_5_Banck_0[7]^gSection_0_Banck_0[3])))
							{
								for (u8Idx = 0; u8Idx < 8; u8Idx++)
								{
									gBrushInfo.u8TempCARDInfo[u8Idx * 2] = Hex2Ascii((gSection_5_Banck_0[8+u8Idx]>>4)&0x0f);
									gBrushInfo.u8TempCARDInfo[u8Idx * 2 + 1] = Hex2Ascii((gSection_5_Banck_0[8+u8Idx])&0x0f);
								}
								RTN = MT_CRT_S50_ReadBank(gSection_3_Banck_0, 3, 0);
								if (TRUE == RTN)  //必须读到钱才操作?
								{
									gBrushInfo.fCARDLeaveMoney = 0.01* (gSection_3_Banck_0[0] + (gSection_3_Banck_0[1]<<8) + (gSection_3_Banck_0[2]<<16) + (gSection_3_Banck_0[3]<<24));


									//1.禁止网络模式
									if(gEvChargeInfo.u8NET_Mask == TRUE)
									{
										//gBrushInfo.ucIsCardStartThroughNet = 0;			//直接刷卡启动	

										//扣过钱后必须移开过卡后才能继续刷卡
										if(su8WriteMoneyFlag == 0)
										{
											u8DoneFlag = FALSE;
											//A等待扣钱状态
											if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_Stop) && (gs_DisplayCtrl.u8NeedBrushJiesuan[0] == 1))
											{
												u8Rst = TRUE;
												for (u8Idx = 0; u8Idx < 16; u8Idx++)
												{

													if (gBrushInfo.u8CARDInfo[GUN_NO_A][u8Idx] != gBrushInfo.u8TempCARDInfo[u8Idx])
													{
														u8Rst = FALSE;
														break;
													}
												}
												if (u8Rst)  //卡号相同，扣钱
												{
													//if (gBrushInfo.fCARDLeaveMoney > gEvChargeInfo.fCostMoney[0])
													{
														ftemp = gBrushInfo.fCARDLeaveMoney - gEvChargeInfo.fCostMoney[0];

														if(WriteCardMoney(ftemp) == CARD_WRITE_OK)
														{
															gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 0;
															//扣过钱，必须移开卡后才能继续刷卡，此处置标志
															su8WriteMoneyFlag = 1;
														}
													}									

													u8DoneFlag = TRUE;
												}
											}
											//B等待扣钱状态
											if((u8DoneFlag == FALSE) 
												&& (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_Stop) && (gs_DisplayCtrl.u8NeedBrushJiesuan[1] == 1))
											{
												//
												u8Rst = TRUE;
												for (u8Idx = 0; u8Idx < 16; u8Idx++)
												{

													if (gBrushInfo.u8CARDInfo[GUN_NO_B][u8Idx] != gBrushInfo.u8TempCARDInfo[u8Idx])
													{
														u8Rst = FALSE;
														break;
													}
												}
												if (u8Rst)  //卡号相同，扣钱
												{
													//if (gBrushInfo.fCARDLeaveMoney > gEvChargeInfo.fCostMoney[1])
													{
														ftemp = gBrushInfo.fCARDLeaveMoney - gEvChargeInfo.fCostMoney[1];			

														if(WriteCardMoney(ftemp) == CARD_WRITE_OK)
														{
															gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 0;
															//扣过钱，必须移开卡后才能继续刷卡，此处置标志
															su8WriteMoneyFlag = 1;
														}
													}						
													u8DoneFlag = TRUE;
												}
											}

											//没扣过钱，则发给其它任务处理
											if(u8DoneFlag == FALSE) 
											{
												//Put_BrushSem();
											}									

											//RTN =SwipeCardStepProcess();
											//MoneyInCardProcess(RTN);

											if(gBrushInfo.i32BrushInterval == 0)
												gBrushInfo.i32BrushInterval = 5;				//5S后才允许再刷

										} //end: if(su8WriteMoneyFlag == 0)
									} // end:  if(gEvChargeInfo.u8NET_Mask == TRUE) 禁止网络模式

									//2. 联网方式
									else
									{
										//卡上余额错误，需要把服务器上的余额写入卡
										if(gBrushInfo.u8AccountRequestStep == ACCOUNT_REQUEST_STEP_2)  
										{
											if(WriteCardMoney(gBrushInfo.fCardMoneyFormNet) == CARD_WRITE_OK)
											{
												//更新金额
												gBrushInfo.fCARDLeaveMoney = gBrushInfo.fCardMoneyFormNet;
												//gs_DisplayCtrl.fCardLeaveMoney = gBrushInfo.fCARDLeaveMoney;

												gBrushInfo.ucCardMoneyErrIsCorrected = 1;
												gBrushInfo.u8AccountRequestStep = ACCOUNT_REQUEST_STEP_1;  //重新上报
											}
										}
										else 
										{
											//Put_BrushSem();
										}
									}							
								
								}  //if (TRUE == RTN)  //必须读到钱才操作?
							}
						}  //end: if (TRUE == u8Rrst_1)

						if(gBrushInfo.i32BrushInterval == 0)
							gBrushInfo.i32BrushInterval = 3;				//5S后才允许再刷

					}  //end: if(gBrushInfo.i32BrushInterval ==0)
				}  //if(gBrushInfo.i32BrushInterval ==0)
			}// end: if ('Y' == uRstResult) 
			else//卡不存在
			{
				su8CARDInExistDelay++;
				if (su8CARDInExistDelay > 10)
				{
					gBrushInfo.uCARDExist = FALSE;
				}

				//扣过钱后必须移开过卡后才能继续刷卡，此处复位标志
				su8WriteMoneyFlag = 0;				
			}

			//清除刷卡机故障标志
			su8AlmDelay = 0;
			gBrushInfo.uBrushCommAlm = FALSE;
		}  // end:  if (TRUE == uCommResult)
		else
		{
			if (su8AlmDelay <= BRUSH_COMFAIL_NUM)
			{
				su8AlmDelay++;
			}
			else
			{
				su8AlmDelay = 0;
				if (gEvChargeInfo.u8BRUSH_ALM_Mask == TRUE)
				{
					gBrushInfo.uBrushCommAlm = FALSE;
				}
				else
				{
					gBrushInfo.uBrushCommAlm = TRUE;
				}
				//dv_Uart1.fInitUart1();
				//dv_Uart1.fUart1EnableRx();
			}
		}

}



void InitDebugInfo(void);
void InitDebugInfo(void)
{
	UINT8 u8Idx = 0;
	for(u8Idx = 0; u8Idx < DEBUG_BUF_ARRY_MAX_NUM; u8Idx++)
	{
		gDebugInfoBuf[u8Idx].u8SendFlag = FALSE;
		memset(gDebugInfoBuf[u8Idx].uBuff, 0, DEBUG_BUF_BYTES_MAX_NUM);
	}
	gu8_R_post = 0;
	gu8_W_post = 0;
}


void Debug_printf(char* fmt,...);
void Debug_printf(char* fmt,...)  
{  
	UINT16 iLen = 0; 
	UNUSED(iLen);
	va_list ap; 
	va_start(ap,fmt);
	memset(URecrodBuff, 0, DEBUG_BUF_BYTES_MAX_NUM);
	vsprintf((char*)URecrodBuff, fmt, ap);
	va_end(ap);
	//iLen=strlen((const char*)URecrodBuff);		//此次发送数据的长度
}

void  XX_Stuff_cmd(UINT8 *cmd, UINT8 *ack, UINT16 *u16Len);
void  XX_Stuff_cmd(UINT8 *cmd, UINT8 *ack, UINT16 *u16Len)
{
	UINT16 u16temp;
	//memset(URecrodBuff, 0, DEBUG_BUF_BYTES_MAX_NUM);
	u16temp = strnlen((const char*)cmd, DEBUG_BUF_BYTES_MAX_NUM);	
	memcpy(URecrodBuff, cmd, MIN(u16temp, DEBUG_BUF_BYTES_MAX_NUM));

	if(u16temp > DEBUG_BUF_BYTES_MAX_NUM-2)
	{
		u16temp = DEBUG_BUF_BYTES_MAX_NUM-2;
	}
	URecrodBuff[u16temp] = '\r';
	URecrodBuff[u16temp+1] = '\n';

	u16temp += 2;

	*u16Len = u16temp;

	//vsprintf((char*)URecrodBuff, fmt, ap);
	//Debug_printf("%s\r\n", cmd);															//发送命令
	//*u16Len = strlen((const char*)URecrodBuff);		
	return;
}

UINT8 u8CheckNeedRecord(UINT16* uPLen)
{
	UINT16 uLen= 0;
	*uPLen = 0;
	if(gu8_W_post == gu8_R_post)
	{
		return  FALSE;
	}
	else
	{
		memset(DebugBuff, 0, MIN(UART1_SEND_MAX_NUM, DEBUG_BUF_BYTES_MAX_NUM));
		memcpy(DebugBuff, gDebugInfoBuf[gu8_R_post].uBuff, MIN(UART1_SEND_MAX_NUM, DEBUG_BUF_BYTES_MAX_NUM));
		//memset(gDebugInfoBuf[gu8_R_post].uBuff, 0, DEBUG_BUF_BYTES_MAX_NUM);
		gDebugInfoBuf[gu8_R_post].u8SendFlag = FALSE;
		gu8_R_post++;
		if(gu8_R_post >= DEBUG_BUF_ARRY_MAX_NUM)
		{
			gu8_R_post = 0;
		}
		XX_Stuff_cmd(DebugBuff, NULL, &uLen);//最终压到URecrodBuff
		*uPLen = uLen;
		OSTimeDly(3);
		return TRUE;
	}
}
//#ifdef	Debug_Print_En
UINT8 WriteDebugInfo(UINT8 * pSendBuf, UINT8 u8Len)
{
	MutexLock(gEvChargeInfo.hMetex_WriteDebugBuf);

	//INT32 i32Idx = -1;
	//i32Idx = u8GetEmptBuff();
	u8Len = MIN(u8Len, DEBUG_BUF_BYTES_MAX_NUM);
	memcpy(gDebugInfoBuf[gu8_W_post].uBuff, pSendBuf, u8Len);
	if(u8Len <= DEBUG_BUF_BYTES_MAX_NUM)
	{
		gDebugInfoBuf[gu8_W_post].uBuff[u8Len] = 0;
	}
	gu8_W_post++;
	if(gu8_W_post >= DEBUG_BUF_ARRY_MAX_NUM)
	{
		gu8_W_post = 0;
	}

	MutexUnlock(gEvChargeInfo.hMetex_WriteDebugBuf);

	return FALSE;
}
//#endif
//UART1		J23

UINT8 ConsoleDebugPrint(void)
{
	UINT16 length;

	if(u8CheckNeedRecord(&length))
	{
		dv_Uart1.fSendDataTXY(URecrodBuff, length);
		return TRUE;
	}
	return FALSE;
}

//#define	CONSOLE_DEBUG_PRINT_EN
void TACK_Uart1Proc (void* pdata)
{
	
	OSTimeDly(500);
	OSTimeDly(500);
	dv_Uart1.fInitUart1();					//19200
	dv_Uart1.fUart1EnableRx();

	gUART1SendData[0] = 0x01;
	gUART1SendData[1] = 0x02;
	gUART1SendData[2] = 0x03;
	gUART1SendData[3] = 0x04;
	gUART1SendData[4] = 0x05;

	
	InitBrushInfo();

	for(;;)
	{
		
		gSelfTest.fSetTaskID(Task_ID_Uart1);
		OSTimeDly(5);
		if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
		{
			OSTimeDly(50);
			continue;
		}
		if (gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)//线程不运行
		{
			OSTimeDly(50);
			continue;
		}

#ifdef	CONSOLE_DEBUG_PRINT_EN
		if(g_u8ChgingLogFileOpenFlag == 0 && gs_HistAlmWriteToUdisc.u8HistAlmFileOpenFlag == 0)   //没有插U盘打印到串口上
		{
			if(ConsoleDebugPrint() == TRUE)
			{
				OSTimeDly(50);
			}
		}
		continue;
#endif

		
		gSelfTest.fSetTaskID(Task_ID_Uart1);
		BrushDeal();
	}//END FOR!
}




