#include "main.h"


#define	WH_IS_DC	0
#define	WH_IS_AC	1
#define	WH_IS_DC_3	2


#define	UARTX_RX_BUF_MAX_WHMeter	RS485_UART4_5_RX_MAX_NUM
#define	UARTX_TX_BUF_MAX_WHMeter	RS485_UART4_5_TX_MAX_NUM

extern NOINIT UINT8 gUart4RecvBuff[RS485_UART4_RX_MAX_NUM];
extern NOINIT UINT8 gUart4TXBuff[RS485_UART4_TX_MAX_NUM];

extern NOINIT UINT8 gUart5RecvBuff[RS485_UART5_RX_MAX_NUM];
extern NOINIT UINT8 gUart5TXBuff[RS485_UART5_TX_MAX_NUM];

#define ABS(a)	(((a) < 0) ? (-(a)) : (a))

#define  HW_SOI1				0x68 
#define  HW_ADDR_A0				0xAA 
#define  HW_ADDR_A1				0xAA 
#define  HW_ADDR_A2				0xAA 
#define  HW_ADDR_A3				0xAA 
#define  HW_ADDR_A4				0xAA 
#define  HW_ADDR_A5				0xAA 
#define  HW_SOI2				0x68 
#define  HW_EOI					0x16 
#define  HW_TRANSFERS_ADD		0x33 


//唤醒
#define  HW_AROUSE_DATA_L			4
#define  HW_AROUSE_DATA				0xfe

/*********************控制码********************************/
#define  HW_SOI2_OFFSET				7
#define  HW_CTRL_CODE_OFFSET		0x08 
#define  HW_DATA_LENGTH_OFFSET		0x09 
#define  HW_DATA_CHECKCODE_OFFSET	0x0A
#define	 HW_PROCESS_OK				1
#define	 HW_PROCESS_ERROR			0




//更改宏定义，一定注意和数组的IDX是否关联上！
static stFrameInfo stDTSD3366ElectricSig1[] =
{
	{0x00+HW_TRANSFERS_ADD,0x00+HW_TRANSFERS_ADD,(0x00+HW_TRANSFERS_ADD),0x00+HW_TRANSFERS_ADD, 2,4, MeterSig_P_Energy_Total, 4, 1, 0, 0},		// 当前尖峰平谷有功正向电能。
	//{0x00+HW_TRANSFERS_ADD,0x02+HW_TRANSFERS_ADD,(0xff+HW_TRANSFERS_ADD),0x00+HW_TRANSFERS_ADD, 2,20, MeterSig_N_Energy_Total, 4, 5, 0, 0},		// 当前尖峰平谷有功反向电能。
	{0x02+HW_TRANSFERS_ADD,0x01+HW_TRANSFERS_ADD,(0xff+HW_TRANSFERS_ADD),0x00+HW_TRANSFERS_ADD, 1,6,  MeterSig_Volt_PhaseA,	   2, 3, 1, 1},				// ABC三相电压。
	{0x02+HW_TRANSFERS_ADD,0x02+HW_TRANSFERS_ADD,(0xff+HW_TRANSFERS_ADD),0x00+HW_TRANSFERS_ADD, 3,9,  MeterSig_Curr_PhaseA,	   3, 3, 1, 2},				// ABC三相电流。
	//{0x02+HW_TRANSFERS_ADD,0x03+HW_TRANSFERS_ADD,(0xff+HW_TRANSFERS_ADD),0x00+HW_TRANSFERS_ADD, 4,12, MeterSig_PowerKW_Total,  3, 4, 1, 3},				// ABC三相功率。
};
static unsigned char stDTSD3366ElectricSig1_Num = sizeof(stDTSD3366ElectricSig1) / sizeof(stFrameInfo); 
static unsigned char suIntervalSampleNoMin = 1;
static unsigned char suIntervalSampleNoMax = 1;


NOINIT stWHInfo gWH_Info[WH_Meter_NUM];

WHMeter_COMM_MNG gs_WHMeterCommMng[WH_Meter_NUM] = {0};


static UINT8 WHGL_CheckSum_1(UINT8 *byData,INT32 iCheckL);
static void WHGL_WakeUp_1(WHMeter_COMM_MNG *pWHMeterCommMng);





void GetIntervalSampleNo(void)
{
	unsigned char i, u8MaxNo, u8MinNo;

	u8MaxNo = 0x00;
	u8MinNo = 0xff;

	for(i = 0; i < stDTSD3366ElectricSig1_Num; i++)
	{		
		if(stDTSD3366ElectricSig1[i].bySampleCycle > 0)
		{
			if(u8MaxNo < stDTSD3366ElectricSig1[i].bySampleCycle)
			{
				u8MaxNo = stDTSD3366ElectricSig1[i].bySampleCycle;
				suIntervalSampleNoMax = u8MaxNo;
			}
			if(u8MinNo > stDTSD3366ElectricSig1[i].bySampleCycle)
			{
				u8MinNo = stDTSD3366ElectricSig1[i].bySampleCycle;
				suIntervalSampleNoMin = u8MinNo;
			}
		}
	}
}

static void WHGL_WakeUp_1(WHMeter_COMM_MNG *pWHMeterCommMng)
{
	INT32	i;
	UINT8	bySendBuff[HW_AROUSE_DATA_L];
	for (i = 0; i < HW_AROUSE_DATA_L; i++ )
	{
		bySendBuff[i] = HW_AROUSE_DATA;
	}
	pWHMeterCommMng->pUartDev->fSendDataTXY(bySendBuff, HW_AROUSE_DATA_L);
	gSelfTest.fSetTaskID(pWHMeterCommMng->FeedDogTaskID);
	OSTimeDly(100/2);
	gSelfTest.fSetTaskID(pWHMeterCommMng->FeedDogTaskID);
	return;
}

static UINT8 WHGL_CheckSum_1(UINT8 *byData,INT32 iCheckL)//未判断溢出 需要在外层判断
{
	UINT8 byTemp = 0;
	INT32  i;
	for (i = 0; i < iCheckL; i++)
	{
		byTemp += byData[i];
	}

	return byTemp;
}

static UINT8 WHGL_U8CheckSum(UINT8 *byData,INT32 iCheckL)			//未判断溢出 需要在外层判断
{
	UINT8 byTemp = 0;
	INT32  i;
	for (i = 0; i < iCheckL && i < UARTX_TX_BUF_MAX_WHMeter; i++)
	{
		byTemp += byData[i];
	}

	return byTemp;
}

//static int DCM3366Q_ReadAddr_1(void)
//{
//	UINT16 uIdx = 0;
//	BYTE byFrameCtrlCode = 0x13;
//	gUart4TXBuff[0] = HW_SOI1;
//	gUart4TXBuff[1] = HW_ADDR_A0;						//99广播地址;
//	gUart4TXBuff[2] = HW_ADDR_A1;
//	gUart4TXBuff[3] = HW_ADDR_A2;
//	gUart4TXBuff[4] = HW_ADDR_A3;
//	gUart4TXBuff[5] = HW_ADDR_A4;
//	gUart4TXBuff[6] = HW_ADDR_A5;
//	gUart4TXBuff[7] = HW_SOI2;
//	gUart4TXBuff[8] = byFrameCtrlCode;
//	gUart4TXBuff[9] = 0x00;							//数据长度L
//	gUart4TXBuff[10] = WHGL_U8CheckSum(gUart4TXBuff, 10);
//	gUart4TXBuff[11] = HW_EOI;
//	dv_Uart4.fSendDataTXY(gUart4TXBuff, 12);
//	gSelfTest.fSetTaskID(TASK_ID_UART4);
//	OSTimeDly(500/2);
//	if (dv_Uart4.pData > 8)
//	{
//		for (uIdx = 0; uIdx < dv_Uart4.pData && uIdx < RS485_UART4_RX_MAX_NUM - 10; uIdx++)
//		{
//			if (HW_SOI1 == gUart4RecvBuff[uIdx])
//			{
//				break;
//			}
//		}
//
//		if (0x93 == gUart4RecvBuff[uIdx + 8] && 0x06 == gUart4RecvBuff[uIdx + 9])	//68H A0 … A5 68H 8AH 00H CS 16H
//		{
//			gEvChargeInfo.u8MeterAddress[0][0] = (UINT8)gUart4RecvBuff[uIdx + 1];
//			gEvChargeInfo.u8MeterAddress[0][1] = (UINT8)gUart4RecvBuff[uIdx + 2];
//			gEvChargeInfo.u8MeterAddress[0][2] = (UINT8)gUart4RecvBuff[uIdx + 3];
//			gEvChargeInfo.u8MeterAddress[0][3] = (UINT8)gUart4RecvBuff[uIdx + 4];
//			gEvChargeInfo.u8MeterAddress[0][4] = (UINT8)gUart4RecvBuff[uIdx + 5];
//			gEvChargeInfo.u8MeterAddress[0][5] = (UINT8)gUart4RecvBuff[uIdx + 6];
//			return TRUE;
//		}
//		else
//		{
//			return FALSE;
//		}
//	}
//	return FALSE;
//}
void InitWHInfo(void)
{
	UCHAR i,j;

	for(i = 0; i < 2; i++)
	{
		gWH_Info[i].fWHRealValue = 0;
		gWH_Info[i].fWHRealValue = 0;
		gWH_Info[i].fWH_A_Volt = 0;
		gWH_Info[i].fWH_B_Volt = 0;
		gWH_Info[i].fWH_C_Volt = 0;
		gWH_Info[i].fWH_A_Curr = 0;
		gWH_Info[i].fWH_B_Curr = 0;
		gWH_Info[i].fWH_C_Curr = 0;

		gWH_Info[i].u8WHCommAlm = 0;
		gWH_Info[i].fWHStartValue = 0;				//电表开始充电数值！
		gWH_Info[i].fWHStartTempValue = 0;

		//gWH_Info[i].fMaxWHRealValue_P = 0;
		//gWH_Info[i].fMaxWHRealValue_N = 0;

		for(j = 0; j < MeterSig_Max; j++)
			gWH_Info[i].fWhValue[j] = 0;
	}
}
void InitWH_E2prom(void)
{
	__XXX_UNION_VALUE unVal;
	gWH_Info[0].u8WH_TYPE = 0;			//WH_IS_DC
	gWH_Info[0].u8WH_ALM_Mask = 0;
	gWH_Info[0].fACWhCoef = 1;			//交流表 互感器  系数
	ExGetSigInfo(E_TYPE_WH1, ID_fACWhCoef, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_WH1, ID_u8WH_TYPE, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_WH1, ID_u8WH_ALM_Mask, &unVal, NULL, BY_E2PROM);

	/*ExGetSigInfo(E_TYPE_WH1, ID_fMaxKwhRange, &unVal, NULL, BY_E2PROM);
	if(gWH_Info[GUN_NO_A].fMaxKwhRange < 999)
	{
	gWH_Info[GUN_NO_A].fMaxKwhRange = WH_MAX_RANGE_DEFAULT;
	}*/

	gWH_Info[1].u8WH_TYPE = 0;			//WH_IS_DC
	gWH_Info[1].fACWhCoef = 1;			//交流表 互感器  系数
	gWH_Info[1].u8WH_ALM_Mask = 0;
	ExGetSigInfo(E_TYPE_WH2, ID_fACWhCoef, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_WH2, ID_u8WH_TYPE, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_WH2, ID_u8WH_ALM_Mask, &unVal, NULL, BY_E2PROM);

}


double _String2FloatVal_1(UINT8* byDataInfo, stFrameInfo* pStrucFramInfo)
{
	int i = 0;
	int j = 0;
	int iDocNum;
	int iSigByteNum;
	double  fTotal = 0;
	UCHAR uctemp;
	unsigned long  dTemp;
	unsigned long fTemp[20];

	iDocNum = pStrucFramInfo->bySigForm;
	iSigByteNum = pStrucFramInfo->bySigSize;

	/*
	例如传输过来的数据是  0X78 0X56 0X34 0X12
	则，
	首先将0X78转换为78 由于0X78是第i = 0字节所以 不满足条件不用乘
	其次 0X56 变为56 因为是第i = 1个字节 
	所以 56 * 100
	再次 0X34 变为34 因为是第i = 2个字节
	所以 34 *100 *100
	最后0X12 i = 3字节
	所以 12 *100 * 100 *100
	也由于经过和雷经理电话确认，电压、电流、功率 是有符号的，除了电能是无符号其他都有符号。
	*/
	float fSign = 1.00;
	unsigned int iTempValue = 0;
	for (i = 0 ; i < iSigByteNum && i < 4; i++)
	{
		fTemp[i] = 0;
		uctemp = ((UCHAR)byDataInfo[i]-HW_TRANSFERS_ADD);
		iTempValue = uctemp;

		//最后一个字节  符号处理
		if ((iSigByteNum - 1) == i)
		{
			if (pStrucFramInfo->byIsSigned == 1)//除了电能都有符号
			{
				fTemp[i] = (unsigned long)( iTempValue& 0x0f) + (unsigned long)(((iTempValue>>4)&0x07)*10);
				if (((iTempValue >> 7)&0x01) == 0x01)			//符号位，0正	1负
				{
					fSign = (-1.00);
				}
				else
				{
					fSign = (1.00);
				}
			}
			else
			{
				fTemp[i] = (unsigned long)(iTempValue & 0x0f) + (unsigned long)(((iTempValue>>4)&0x0f)*10);
			}
		}
		else
		{
			fTemp[i] = (unsigned long)(iTempValue & 0x0f) + (unsigned long)(((iTempValue>>4)&0x0f)*10);
		}
		dTemp = fTemp[i];
		for (j = 0; j < i; j++)
		{
			dTemp *= 100;
		}
		fTotal += dTemp;
	}

	if (iDocNum == 0)
	{
		//printf("\n	The function can't parse the Sig OR Check the stFrameInfo member!!	\n");
		//
	}
	else if (1 == iDocNum)
	{
		fTotal *= 0.1;
	}
	else if (2 == iDocNum)
	{
		fTotal *= 0.01;
	}
	else if (3 == iDocNum)
	{
		fTotal *= 0.001;
	}
	else if (4 == iDocNum)
	{
		fTotal *= 0.0001;
	}
	else
	{
		//printf(" The doc is Error	\n");
		//(*byStat) = HW_PROCESS_ERROR;
		//return FALSE;
	}

	//(*byStat) = HW_PROCESS_OK;
	return (fSign * fTotal);
}

//读取操作
UCHAR ReadMeterEnergy(WHMeter_COMM_MNG *pWHMeterCommMng, UCHAR u8SigTypeNo)
{
	UINT8 u8CurrBaud, uctemp, u8SetSeccess1, i;
	UINT16	u16TxIdx, u16StartIdx, u16RcvLenth;
	UINT16 u16WatiResponsTimeMs;
	UINT8 *pRxBuf;

	
	if(u8SigTypeNo >= stDTSD3366ElectricSig1_Num)
	{
		return FALSE;
	}

	if(gEvChargeInfo.u8MeterBaud == 0)  //9600
	{
		u8CurrBaud = m9600;
		u16WatiResponsTimeMs = 150; //ms
	}
	else if(gEvChargeInfo.u8MeterBaud == 1)  //19200
	{
		u8CurrBaud = m19200;
		u16WatiResponsTimeMs = 120; //ms
	}
	else //2400
	{
		u8CurrBaud = m2400;
		u16WatiResponsTimeMs = 300; //ms
	}

	pWHMeterCommMng->pUartDev->fInitUart(u8CurrBaud, UARTX_PARITY_EVEN);				//InitUart5
	pWHMeterCommMng->pUartDev->fEnableRx();							//使能收中断
	OSTimeDly(5);
	WHGL_WakeUp_1(pWHMeterCommMng);

	pWHMeterCommMng->pUartXTXBuff[0] = HW_SOI1;
	pWHMeterCommMng->pUartXTXBuff[1] =pWHMeterCommMng->pWHMeterInfo->Addr[0];
	pWHMeterCommMng->pUartXTXBuff[2] =pWHMeterCommMng->pWHMeterInfo->Addr[1];
	pWHMeterCommMng->pUartXTXBuff[3] =pWHMeterCommMng->pWHMeterInfo->Addr[2];
	pWHMeterCommMng->pUartXTXBuff[4] =pWHMeterCommMng->pWHMeterInfo->Addr[3];
	pWHMeterCommMng->pUartXTXBuff[5] =pWHMeterCommMng->pWHMeterInfo->Addr[4];
	pWHMeterCommMng->pUartXTXBuff[6] =pWHMeterCommMng->pWHMeterInfo->Addr[5];
	pWHMeterCommMng->pUartXTXBuff[7] = HW_SOI2;
	pWHMeterCommMng->pUartXTXBuff[8] = 0x11;
	pWHMeterCommMng->pUartXTXBuff[9] = 4;							//数据长度L	

	pWHMeterCommMng->pUartXTXBuff[10] = stDTSD3366ElectricSig1[u8SigTypeNo].byLabelDI0;
	pWHMeterCommMng->pUartXTXBuff[11] = stDTSD3366ElectricSig1[u8SigTypeNo].byLabelDI1;
	pWHMeterCommMng->pUartXTXBuff[12] = stDTSD3366ElectricSig1[u8SigTypeNo].byLabelDI2;
	pWHMeterCommMng->pUartXTXBuff[13] = stDTSD3366ElectricSig1[u8SigTypeNo].byLabelDI3;
	pWHMeterCommMng->pUartXTXBuff[14] = WHGL_U8CheckSum(pWHMeterCommMng->pUartXTXBuff, 14);
	pWHMeterCommMng->pUartXTXBuff[15] = HW_EOI;
	pWHMeterCommMng->pUartDev->fSendDataTXY(pWHMeterCommMng->pUartXTXBuff, 16);
	gSelfTest.fSetTaskID(pWHMeterCommMng->FeedDogTaskID);

	OSTimeDly(u16WatiResponsTimeMs/2);

	u8SetSeccess1 = 0;
	if (dv_Uart4.pData > 8)
	{
		uctemp = 0;
		for (u16TxIdx = 0; u16TxIdx < pWHMeterCommMng->pUartDev->pData && u16TxIdx < UARTX_RX_BUF_MAX_WHMeter - 10; u16TxIdx++)
		{
			if (HW_SOI1 == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx])
			{
				u16StartIdx = u16TxIdx;
				uctemp = 1;
				break;
			}
		}

		if((uctemp == 1) && (pWHMeterCommMng->pUartDev->pData >= (u16TxIdx+12+4+stDTSD3366ElectricSig1[u8SigTypeNo].byRcvLength)))
		{
			if ((0x91 == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx + 8]) && ((stDTSD3366ElectricSig1[u8SigTypeNo].byRcvLength+4) == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx + 9]))	
			{				
				for(i = 0; i < 6; i++)
				{
					if(pWHMeterCommMng->pUartXRecvBuff[u16StartIdx+1+i] != pWHMeterCommMng->pWHMeterInfo->Addr[i])
					{
						return FALSE;
					}
				}			
				if(pWHMeterCommMng->pUartXRecvBuff[u16StartIdx+stDTSD3366ElectricSig1[u8SigTypeNo].byRcvLength+14] == WHGL_CheckSum_1(&pWHMeterCommMng->pUartXRecvBuff[u16StartIdx], stDTSD3366ElectricSig1[u8SigTypeNo].byRcvLength+14))  //检查校验
				{
					if((pWHMeterCommMng->pUartXRecvBuff[u16StartIdx+10] == stDTSD3366ElectricSig1[u8SigTypeNo].byLabelDI0)
						&& (pWHMeterCommMng->pUartXRecvBuff[u16StartIdx+11] == stDTSD3366ElectricSig1[u8SigTypeNo].byLabelDI1)
						&& (pWHMeterCommMng->pUartXRecvBuff[u16StartIdx+12] == stDTSD3366ElectricSig1[u8SigTypeNo].byLabelDI2)
						&& (pWHMeterCommMng->pUartXRecvBuff[u16StartIdx+13] == stDTSD3366ElectricSig1[u8SigTypeNo].byLabelDI3)
						)
					{
						pRxBuf = &pWHMeterCommMng->pUartXRecvBuff[u16StartIdx+14];
						u16RcvLenth = 0;
						for(i = 0; i < stDTSD3366ElectricSig1[u8SigTypeNo].bySigNum; i++)
						{
							pWHMeterCommMng->pWHMeterInfo->fWhValue[stDTSD3366ElectricSig1[u8SigTypeNo].iRoughNo+i] = _String2FloatVal_1(pRxBuf, &stDTSD3366ElectricSig1[u8SigTypeNo]);
							pRxBuf += stDTSD3366ElectricSig1[u8SigTypeNo].bySigSize;
							u16RcvLenth += stDTSD3366ElectricSig1[u8SigTypeNo].bySigSize;
							if(u16RcvLenth >= stDTSD3366ElectricSig1[u8SigTypeNo].byRcvLength)
							{
								break;
							}
						}
						u8SetSeccess1 = 1;
					}
				}				
			}			
		}		
	} //end: if (dv_Uart4.pData > 8)

	//if((u8SetSeccess1 == 1)
	//	&& (u8SetSeccess2 == 1))
	if(u8SetSeccess1 == 1)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}


UCHAR Routine_Read_Meter_Data_Deal(WHMeter_COMM_MNG *pWHMeterCommMng)
{
	unsigned char i;
	float ftemp;

		
	pWHMeterCommMng->u8SampleCycleCount++; 
	if(pWHMeterCommMng->u8SampleCycleCount > suIntervalSampleNoMax)
	{
		pWHMeterCommMng->u8SampleCycleCount = suIntervalSampleNoMin;
	}

	//读取
	for(i = 0; i < stDTSD3366ElectricSig1_Num; i++)
	{		
		if((stDTSD3366ElectricSig1[i].bySampleCycle == 0)
			|| (stDTSD3366ElectricSig1[i].bySampleCycle == pWHMeterCommMng->u8SampleCycleCount))
		{
			if(ReadMeterEnergy(pWHMeterCommMng, i) == TRUE)
			{
				//suCommFailCount[u8MeterNo] = 0;
				pWHMeterCommMng->u8CommFailCount = 0;
				//gWH_Info[u8MeterNo].u8WHCommAlm = FALSE;
				pWHMeterCommMng->u8CommAlm = FALSE;
				pWHMeterCommMng->pWHMeterInfo->u8WHCommAlm = FALSE;
			}
			else
			{
				if (pWHMeterCommMng->u8CommFailCount < 15)
				{
					pWHMeterCommMng->u8CommFailCount++;
				}
				else
				{
					if (pWHMeterCommMng->pWHMeterInfo->u8WH_ALM_Mask == TRUE)	
					{
						pWHMeterCommMng->pWHMeterInfo->u8WHCommAlm = FALSE;
					}
					else
					{
						pWHMeterCommMng->pWHMeterInfo->u8WHCommAlm = TRUE;						
					}
					pWHMeterCommMng->u8CommAlm = TRUE;
				}	
			}
		}
	} // end: for(i = 0; i < stDTSD3366ElectricSig1_Num; i++)

	if(pWHMeterCommMng->u8CommFailCount == 0)
	{		
		pWHMeterCommMng->pWHMeterInfo->fWHRealValue = pWHMeterCommMng->pWHMeterInfo->fWhValue[MeterSig_P_Energy_Total]; //+gWH_Info[u8MeterNo].fWhValue[MeterSig_N_Energy_Total];
		pWHMeterCommMng->pWHMeterInfo->fWH_A_Volt = pWHMeterCommMng->pWHMeterInfo->fWhValue[MeterSig_Volt_PhaseA];
		pWHMeterCommMng->pWHMeterInfo->fWH_B_Volt = pWHMeterCommMng->pWHMeterInfo->fWhValue[MeterSig_Volt_PhaseB];
		pWHMeterCommMng->pWHMeterInfo->fWH_C_Volt = pWHMeterCommMng->pWHMeterInfo->fWhValue[MeterSig_Volt_PhaseC];

		pWHMeterCommMng->pWHMeterInfo->fWH_A_Curr = pWHMeterCommMng->pWHMeterInfo->fWhValue[MeterSig_Curr_PhaseA];
		pWHMeterCommMng->pWHMeterInfo->fWH_B_Curr = pWHMeterCommMng->pWHMeterInfo->fWhValue[MeterSig_Curr_PhaseB];
		pWHMeterCommMng->pWHMeterInfo->fWH_C_Curr = pWHMeterCommMng->pWHMeterInfo->fWhValue[MeterSig_Curr_PhaseC];

		return TRUE;		
	}// end: if(suCommFailCount[u8MeterNo] == 0)


	if (pWHMeterCommMng->pWHMeterInfo->u8WH_ALM_Mask == TRUE)
		return TRUE;	

	return FALSE;

}


//日时段数
//串口发送:FE FE FE FE 68 01 00 83 09 12 19 68 14 0D 36 35 33 37 35 33 33 33 44 44 44 44 47 A3 16 
//接收:FE FE FE FE 68 01 00 83 09 12 19 68 94 00 1C 16 
//UCHAR CfgTimePriceMode(UCHAR u8MeterNo)
//{
//	UINT8 u8CurrBaud, u8DataLen, i, uctemp, u8SetSeccess1, u8SetSeccess2;
//	UINT16	u16TxIdx, u16temp;
//
//	if(u8MeterNo > 1)
//	{
//		return FALSE;
//	}
//
//	if(gEvChargeInfo.u8MeterBaud == 0)  //9600
//	{
//		u8CurrBaud = m9600;
//	}
//	else if(gEvChargeInfo.u8MeterBaud == 1)  //19200
//	{
//		u8CurrBaud = m19200;
//	}
//	else //2400
//	{
//		u8CurrBaud = m2400;
//	}
//
//	u8SetSeccess1 = 0;
//	u8SetSeccess2 = 0;
//	//for(u8MeterNo = 0; u8MeterNo < 1; u8MeterNo++)
//	{
//		dv_Uart4.fInitUart(u8CurrBaud, UARTX_PARITY_EVEN);				//InitUart5
//		dv_Uart4.fEnableRx();							//使能收中断
//		OSTimeDly(5);
//		WHGL_WakeUp_1();
//
//		gUart4TXBuff[0] = HW_SOI1;
//		gUart4TXBuff[1] =gWH_Info[u8MeterNo].Addr[0];
//		gUart4TXBuff[2] =gWH_Info[u8MeterNo].Addr[1];
//		gUart4TXBuff[3] =gWH_Info[u8MeterNo].Addr[2];
//		gUart4TXBuff[4] =gWH_Info[u8MeterNo].Addr[3];
//		gUart4TXBuff[5] =gWH_Info[u8MeterNo].Addr[4];
//		gUart4TXBuff[6] =gWH_Info[u8MeterNo].Addr[5];
//		gUart4TXBuff[7] = HW_SOI2;
//		gUart4TXBuff[8] = 0x14;
//		gUart4TXBuff[9] = 4+4+4+1;							//数据长度L		
//		gUart4TXBuff[10] = 0x03+HW_TRANSFERS_ADD;
//		gUart4TXBuff[11] = 0x02+HW_TRANSFERS_ADD;
//		gUart4TXBuff[12] = 0x00+HW_TRANSFERS_ADD;
//		gUart4TXBuff[13] = 0x04+HW_TRANSFERS_ADD;
//		gUart4TXBuff[14] = 0x02+HW_TRANSFERS_ADD;
//		gUart4TXBuff[15] = 0x00+HW_TRANSFERS_ADD;
//		gUart4TXBuff[16] = 0x00+HW_TRANSFERS_ADD;
//		gUart4TXBuff[17] = 0x00+HW_TRANSFERS_ADD;
//		gUart4TXBuff[18] = 0x99;
//		gUart4TXBuff[19] = 0x99;
//		gUart4TXBuff[20] = 0x99;
//		gUart4TXBuff[21] = 0x99;
//		gUart4TXBuff[22] = gs_ChgDischgPlanMng.u8ValidTimePriceSegNum+HW_TRANSFERS_ADD;		
//		gUart4TXBuff[23] = WHGL_U8CheckSum(gUart4TXBuff, 23);
//		gUart4TXBuff[24] = HW_EOI;
//		dv_Uart4.fSendDataTXY(gUart4TXBuff, 25);
//		gSelfTest.fSetTaskID(TASK_ID_UART4);
//
//		OSTimeDly(150/2);
//
//		u8SetSeccess1 = 0;
//		if (dv_Uart4.pData > 8)
//		{
//			uctemp = 0;
//			for (u16TxIdx = 0; u16TxIdx < dv_Uart4.pData && u16TxIdx < RS485_UART4_RX_MAX_NUM - 10; u16TxIdx++)
//			{
//				if (HW_SOI1 == gUart4RecvBuff[u16TxIdx])
//				{
//					uctemp = 1;
//					break;
//				}
//			}
//
//			if((uctemp == 1) && (dv_Uart4.pData >= (u16TxIdx+12)))
//			{
//				if ((0x94 == gUart4RecvBuff[u16TxIdx + 8]) && (0 == gUart4RecvBuff[u16TxIdx + 9]))	
//				{				
//					u8SetSeccess1 = 1;
//				}	
//			}
//		}
//
//
//		WHGL_WakeUp_1();
//
//		gUart4TXBuff[0] = HW_SOI1;
//		gUart4TXBuff[1] =gWH_Info[u8MeterNo].Addr[0];
//		gUart4TXBuff[2] =gWH_Info[u8MeterNo].Addr[1];
//		gUart4TXBuff[3] =gWH_Info[u8MeterNo].Addr[2];
//		gUart4TXBuff[4] =gWH_Info[u8MeterNo].Addr[3];
//		gUart4TXBuff[5] =gWH_Info[u8MeterNo].Addr[4];
//		gUart4TXBuff[6] =gWH_Info[u8MeterNo].Addr[5];
//		gUart4TXBuff[7] = HW_SOI2;
//		gUart4TXBuff[8] = 0x14;
//		//gUart4TXBuff[9] = 0x06;							//数据长度L		
//		gUart4TXBuff[10] = 0x01+HW_TRANSFERS_ADD;
//		gUart4TXBuff[11] = 0x00+HW_TRANSFERS_ADD;
//		gUart4TXBuff[12] = 0x01+HW_TRANSFERS_ADD;
//		gUart4TXBuff[13] = 0x04+HW_TRANSFERS_ADD;
//
//		gUart4TXBuff[14] = 0x02+HW_TRANSFERS_ADD;
//		gUart4TXBuff[15] = 0x00+HW_TRANSFERS_ADD;
//		gUart4TXBuff[16] = 0x00+HW_TRANSFERS_ADD;
//		gUart4TXBuff[17] = 0x00+HW_TRANSFERS_ADD;
//		gUart4TXBuff[18] = 0x99;
//		gUart4TXBuff[19] = 0x99;
//		gUart4TXBuff[20] = 0x99;
//		gUart4TXBuff[21] = 0x99;
//		u8DataLen = 12;
//		u16TxIdx = 22;
//		for(i = 0; i < gs_ChgDischgPlanMng.u8ValidTimePriceSegNum && i < 14; i++)
//		{
//			gUart4TXBuff[u16TxIdx++] = gs_ChgDischgPlanMng.TimePrice[i].u8PriceMode + HW_TRANSFERS_ADD;
//			gUart4TXBuff[u16TxIdx++] = HexToBCD(gs_ChgDischgPlanMng.TimePrice[i].u8Min) + HW_TRANSFERS_ADD;
//			gUart4TXBuff[u16TxIdx++] = HexToBCD(gs_ChgDischgPlanMng.TimePrice[i].u8Hour) + HW_TRANSFERS_ADD;
//			u8DataLen +=  3;
//		}
//		gUart4TXBuff[9] = u8DataLen;							//数据长度L	
//		u16temp = u16TxIdx;
//		gUart4TXBuff[u16TxIdx++] = WHGL_U8CheckSum(gUart4TXBuff, u16temp);
//		gUart4TXBuff[u16TxIdx++] = HW_EOI;
//
//		dv_Uart4.fSendDataTXY(gUart4TXBuff, u16TxIdx);
//		gSelfTest.fSetTaskID(TASK_ID_UART4);
//
//		OSTimeDly(200/2);
//
//		u8SetSeccess2 = 0;
//		if (dv_Uart4.pData > 8)
//		{
//			uctemp = 0;
//			for (u16TxIdx = 0; u16TxIdx < dv_Uart4.pData && u16TxIdx < RS485_UART4_RX_MAX_NUM - 10; u16TxIdx++)
//			{
//				if (HW_SOI1 == gUart4RecvBuff[u16TxIdx])
//				{
//					uctemp = 1;
//					break;
//				}
//			}
//
//			if((uctemp == 1) && (dv_Uart4.pData >= (u16TxIdx+12)))
//			{
//				if ((0x94 == gUart4RecvBuff[u16TxIdx + 8]) && (0 == gUart4RecvBuff[u16TxIdx + 9]))	
//				{				
//					u8SetSeccess2 = 1;
//				}	
//			}
//		}
//	}
//
//	if((u8SetSeccess1 == 1)
//		&& (u8SetSeccess2 == 1))
//	{
//		return TRUE;
//	}
//	else
//	{
//		return FALSE;
//	}
//
//}

/*
04	00	01	01	YYMMDDWW	4		*	*	日期及星期
04	00	01	02	hhmmss	3		*	*	时间
*/
UCHAR CfgSigleMeterTime(WHMeter_COMM_MNG *pWHMeterCommMng)
{
	UINT8 u8CurrBaud, uctemp, u8SetSeccess1, u8SetSeccess2;
	UINT16	u16TxIdx;

	struct tm tmNow;

	gRTCData.fGetLocalTime(&tmNow);

	if(gEvChargeInfo.u8MeterBaud == 0)  //9600
	{
		u8CurrBaud = m9600;
	}
	else if(gEvChargeInfo.u8MeterBaud == 1)  //19200
	{
		u8CurrBaud = m19200;
	}
	else //2400
	{
		u8CurrBaud = m2400;
	}

	u8SetSeccess1 = 0;
	u8SetSeccess2 = 0;
	//for(u8MeterNo = 0; u8MeterNo < 1; u8MeterNo++)
	{
		pWHMeterCommMng->pUartDev->fInitUart(u8CurrBaud, UARTX_PARITY_EVEN);				//InitUart5
		pWHMeterCommMng->pUartDev->fEnableRx();							//使能收中断
		OSTimeDly(5);
		WHGL_WakeUp_1(pWHMeterCommMng);

		//秒分时
		pWHMeterCommMng->pUartXTXBuff[0] = HW_SOI1;
		pWHMeterCommMng->pUartXTXBuff[1] =pWHMeterCommMng->pWHMeterInfo->Addr[0];
		pWHMeterCommMng->pUartXTXBuff[2] =pWHMeterCommMng->pWHMeterInfo->Addr[1];
		pWHMeterCommMng->pUartXTXBuff[3] =pWHMeterCommMng->pWHMeterInfo->Addr[2];
		pWHMeterCommMng->pUartXTXBuff[4] =pWHMeterCommMng->pWHMeterInfo->Addr[3];
		pWHMeterCommMng->pUartXTXBuff[5] =pWHMeterCommMng->pWHMeterInfo->Addr[4];
		pWHMeterCommMng->pUartXTXBuff[6] =pWHMeterCommMng->pWHMeterInfo->Addr[5];
		pWHMeterCommMng->pUartXTXBuff[7] = HW_SOI2;
		pWHMeterCommMng->pUartXTXBuff[8] = 0x14;
		pWHMeterCommMng->pUartXTXBuff[9] = 4+4+4+3;							//数据长度L		
		pWHMeterCommMng->pUartXTXBuff[10] = 0x02+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[11] = 0x01+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[12] = 0x00+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[13] = 0x04+HW_TRANSFERS_ADD;

		pWHMeterCommMng->pUartXTXBuff[14] = 0x02+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[15] = 0x00+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[16] = 0x00+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[17] = 0x00+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[18] = 0x99;
		pWHMeterCommMng->pUartXTXBuff[19] = 0x99;
		pWHMeterCommMng->pUartXTXBuff[20] = 0x99;
		pWHMeterCommMng->pUartXTXBuff[21] = 0x99;
		pWHMeterCommMng->pUartXTXBuff[22] = HexToBCD(tmNow.tm_sec) + HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[23] = HexToBCD(tmNow.tm_min) + HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[24] = HexToBCD(tmNow.tm_hour) + HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[25] = WHGL_U8CheckSum(pWHMeterCommMng->pUartXTXBuff, 25);
		pWHMeterCommMng->pUartXTXBuff[26] = HW_EOI;
		pWHMeterCommMng->pUartDev->fSendDataTXY(pWHMeterCommMng->pUartXTXBuff, 27);
		gSelfTest.fSetTaskID(pWHMeterCommMng->FeedDogTaskID);

		OSTimeDly(150/2);

		u8SetSeccess1 = 0;
		if (pWHMeterCommMng->pUartDev->pData > 8)
		{
			uctemp = 0;
			for (u16TxIdx = 0; u16TxIdx < pWHMeterCommMng->pUartDev->pData && u16TxIdx < UARTX_RX_BUF_MAX_WHMeter - 10; u16TxIdx++)
			{
				if (HW_SOI1 == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx])
				{
					uctemp = 1;
					break;
				}
			}

			if((uctemp == 1) && (pWHMeterCommMng->pUartDev->pData >= (u16TxIdx+12)))
			{
				if ((0x94 == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx + 8]) && (0 == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx + 9]))	
				{				
					u8SetSeccess1 = 1;
				}	
			}
		}


		WHGL_WakeUp_1(pWHMeterCommMng);

		//周日月年
		pWHMeterCommMng->pUartXTXBuff[0] = HW_SOI1;
		pWHMeterCommMng->pUartXTXBuff[1] =pWHMeterCommMng->pWHMeterInfo->Addr[0];
		pWHMeterCommMng->pUartXTXBuff[2] =pWHMeterCommMng->pWHMeterInfo->Addr[1];
		pWHMeterCommMng->pUartXTXBuff[3] =pWHMeterCommMng->pWHMeterInfo->Addr[2];
		pWHMeterCommMng->pUartXTXBuff[4] =pWHMeterCommMng->pWHMeterInfo->Addr[3];
		pWHMeterCommMng->pUartXTXBuff[5] =pWHMeterCommMng->pWHMeterInfo->Addr[4];
		pWHMeterCommMng->pUartXTXBuff[6] =pWHMeterCommMng->pWHMeterInfo->Addr[5];
		pWHMeterCommMng->pUartXTXBuff[7] = HW_SOI2;
		pWHMeterCommMng->pUartXTXBuff[8] = 0x14;
		pWHMeterCommMng->pUartXTXBuff[9] = 4+4+4+4;							//数据长度L		
		pWHMeterCommMng->pUartXTXBuff[10] = 0x01+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[11] = 0x01+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[12] = 0x00+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[13] = 0x04+HW_TRANSFERS_ADD;

		pWHMeterCommMng->pUartXTXBuff[14] = 0x02+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[15] = 0x00+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[16] = 0x00+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[17] = 0x00+HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[18] = 0x99;
		pWHMeterCommMng->pUartXTXBuff[19] = 0x99;
		pWHMeterCommMng->pUartXTXBuff[20] = 0x99;
		pWHMeterCommMng->pUartXTXBuff[21] = 0x99;
		pWHMeterCommMng->pUartXTXBuff[22] = HexToBCD(tmNow.tm_wday) + HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[23] = HexToBCD(tmNow.tm_mday) + HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[24] = HexToBCD(tmNow.tm_mon) + HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[25] = HexToBCD(tmNow.tm_year-2000) + HW_TRANSFERS_ADD;
		pWHMeterCommMng->pUartXTXBuff[26] = WHGL_U8CheckSum(pWHMeterCommMng->pUartXTXBuff, 26);
		pWHMeterCommMng->pUartXTXBuff[27] = HW_EOI;
		pWHMeterCommMng->pUartDev->fSendDataTXY(pWHMeterCommMng->pUartXTXBuff, 28);
		gSelfTest.fSetTaskID(pWHMeterCommMng->FeedDogTaskID);

		OSTimeDly(150/2);

		u8SetSeccess2 = 0;
		if (pWHMeterCommMng->pUartDev->pData > 8)
		{
			uctemp = 0;
			for (u16TxIdx = 0; u16TxIdx < pWHMeterCommMng->pUartDev->pData && u16TxIdx < UARTX_RX_BUF_MAX_WHMeter - 10; u16TxIdx++)
			{
				if (HW_SOI1 == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx])
				{
					uctemp = 1;
					break;
				}
			}

			if((uctemp == 1) && (pWHMeterCommMng->pUartDev->pData >= (u16TxIdx+12)))
			{
				if ((0x94 == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx + 8]) && (0 == pWHMeterCommMng->pUartXRecvBuff[u16TxIdx + 9]))	
				{				
					u8SetSeccess2 = 1;
				}	
			}
		}
	}

	if((u8SetSeccess1 == 1)
		&& (u8SetSeccess2 == 1))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}


void BroadMeterTime(WHMeter_COMM_MNG *pWHMeterCommMng)
{
	UINT8 u8CurrBaud;

	struct tm tmNow;

	if(gEvChargeInfo.u8MeterBaud == 0)  //9600
	{
		u8CurrBaud = m9600;
	}
	else if(gEvChargeInfo.u8MeterBaud == 1)  //19200
	{
		u8CurrBaud = m19200;
	}
	else //2400
	{
		u8CurrBaud = m2400;
	}

	gRTCData.fGetLocalTime(&tmNow);

	pWHMeterCommMng->pUartDev->fInitUart(u8CurrBaud, UARTX_PARITY_EVEN);				//InitUart5
	pWHMeterCommMng->pUartDev->fEnableRx();							//使能收中断
	OSTimeDly(5);
	WHGL_WakeUp_1(pWHMeterCommMng);

	pWHMeterCommMng->pUartXTXBuff[0] = HW_SOI1;
	pWHMeterCommMng->pUartXTXBuff[1] = 0x99;
	pWHMeterCommMng->pUartXTXBuff[2] = 0x99;
	pWHMeterCommMng->pUartXTXBuff[3] = 0x99;
	pWHMeterCommMng->pUartXTXBuff[4] = 0x99;
	pWHMeterCommMng->pUartXTXBuff[5] = 0x99;
	pWHMeterCommMng->pUartXTXBuff[6] = 0x99;
	pWHMeterCommMng->pUartXTXBuff[7] = HW_SOI2;
	pWHMeterCommMng->pUartXTXBuff[8] = 0x08;
	pWHMeterCommMng->pUartXTXBuff[9] = 0x06;							//数据长度L

	pWHMeterCommMng->pUartXTXBuff[10] = HexToBCD(tmNow.tm_sec) + HW_TRANSFERS_ADD;
	pWHMeterCommMng->pUartXTXBuff[11] = HexToBCD(tmNow.tm_min) + HW_TRANSFERS_ADD;
	pWHMeterCommMng->pUartXTXBuff[12] = HexToBCD(tmNow.tm_hour) + HW_TRANSFERS_ADD;
	pWHMeterCommMng->pUartXTXBuff[13] = HexToBCD(tmNow.tm_mday) + HW_TRANSFERS_ADD;
	pWHMeterCommMng->pUartXTXBuff[14] = HexToBCD(tmNow.tm_mon) + HW_TRANSFERS_ADD;
	pWHMeterCommMng->pUartXTXBuff[15] = HexToBCD(tmNow.tm_year-2000) + HW_TRANSFERS_ADD;
	pWHMeterCommMng->pUartXTXBuff[16] = WHGL_U8CheckSum(pWHMeterCommMng->pUartXTXBuff, 16);
	pWHMeterCommMng->pUartXTXBuff[17] = HW_EOI;

	pWHMeterCommMng->pUartDev->fSendDataTXY(pWHMeterCommMng->pUartXTXBuff, 18);
	gSelfTest.fSetTaskID(pWHMeterCommMng->FeedDogTaskID);
	OSTimeDly(50);

}

//
/*
通讯速率特征字：
通信速率特征字（调制型、接触式、通信口1、通信口2、通信口3）
Bit7	Bit6	   Bit5   	Bit4	    Bit3	Bit2	Bit1	Bit0
保留	19200bps	9600bps	4800bps 	2400bps	1200bps	600bps	保留
DCM直流表 -- 0x0C 19200-0XB3  9600-0X73   2400-0X43
交流表 -- 0x17 19200-0x73 9600-0X53   2400-0X3B
*/
void AutoSetBaud(WHMeter_COMM_MNG *pWHMeterCommMng)
{
	UINT8 u8CurrBaud[3], u8SetBaud, i, uctemp, u8SetSeccess;
	UINT16 uIdx;

	if(pWHMeterCommMng->sAutoCfgMng.u8EnterAutoSetBaudFlag == FALSE)
	{
		return;
	}
	/*if((gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == START_CHARGE_VAL)
		|| (gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == START_CHARGE_VAL))	
	{
		return;
	}*/

	pWHMeterCommMng->sAutoCfgMng.u8EnterAutoSetBaudFlag = FALSE;

	if(gEvChargeInfo.u8MeterBaud == 0)  //9600
	{
		u8CurrBaud[0] = m9600;
		u8CurrBaud[1] = m2400;
		u8CurrBaud[2] = m19200;
		u8SetBaud = 0x53; //0x20; //m9600;
	}
	else if(gEvChargeInfo.u8MeterBaud == 1)  //19200
	{
		u8CurrBaud[0] = m19200;
		u8CurrBaud[1] = m2400;
		u8CurrBaud[2] = m9600;
		u8SetBaud = 0x73; //0x40; //m19200;
	}
	else //2400
	{
		u8CurrBaud[0] = m2400;
		u8CurrBaud[1] = m9600;
		u8CurrBaud[2] = m19200;
		u8SetBaud = 0x3B; //0x08; // m2400;
	}

	pWHMeterCommMng->pUartXTXBuff[0] = HW_SOI1;
	pWHMeterCommMng->pUartXTXBuff[1] = pWHMeterCommMng->pWHMeterInfo->Addr[0];
	pWHMeterCommMng->pUartXTXBuff[2] = pWHMeterCommMng->pWHMeterInfo->Addr[1];
	pWHMeterCommMng->pUartXTXBuff[3] = pWHMeterCommMng->pWHMeterInfo->Addr[2];
	pWHMeterCommMng->pUartXTXBuff[4] = pWHMeterCommMng->pWHMeterInfo->Addr[3];
	pWHMeterCommMng->pUartXTXBuff[5] = pWHMeterCommMng->pWHMeterInfo->Addr[4];
	pWHMeterCommMng->pUartXTXBuff[6] = pWHMeterCommMng->pWHMeterInfo->Addr[5];
	pWHMeterCommMng->pUartXTXBuff[7] = HW_SOI2;
	pWHMeterCommMng->pUartXTXBuff[8] = 0x17;
	pWHMeterCommMng->pUartXTXBuff[9] = 0x01;							//数据长度L
	pWHMeterCommMng->pUartXTXBuff[10] = u8SetBaud;
	pWHMeterCommMng->pUartXTXBuff[11] = WHGL_U8CheckSum(pWHMeterCommMng->pUartXTXBuff, 11);
	pWHMeterCommMng->pUartXTXBuff[12] = HW_EOI;

	u8SetSeccess = 0;
	for(i = 0; i < 3; i++)
	{
		pWHMeterCommMng->pUartDev->fInitUart(u8CurrBaud[i], UARTX_PARITY_EVEN);				//InitUart5
		pWHMeterCommMng->pUartDev->fEnableRx();							//使能收中断
		OSTimeDly(5);
		WHGL_WakeUp_1(pWHMeterCommMng);

		pWHMeterCommMng->pUartDev->fSendDataTXY(pWHMeterCommMng->pUartXTXBuff, 13);
		gSelfTest.fSetTaskID(pWHMeterCommMng->FeedDogTaskID);
		OSTimeDly(100);

		if (dv_Uart4.pData > 8)
		{
			uctemp = 0;
			for (uIdx = 0; uIdx < pWHMeterCommMng->pUartDev->pData && uIdx < UARTX_RX_BUF_MAX_WHMeter - 10; uIdx++)
			{
				if (HW_SOI1 == pWHMeterCommMng->pUartXRecvBuff[uIdx])
				{
					uctemp = 1;
					break;
				}
			}

			if((uctemp == 1) && (pWHMeterCommMng->pUartDev->pData >= (uIdx+13)))
			{
				if ((0x97 == pWHMeterCommMng->pUartXRecvBuff[uIdx + 8]) && (u8SetBaud == pWHMeterCommMng->pUartXRecvBuff[uIdx + 10]))	
				{				
					u8SetSeccess = 1;
					break;
				}	
			}
		}

	}  //end: for(i = 0; i < 2; i++)

	if(u8SetSeccess == 1)
	{
		pWHMeterCommMng->sAutoCfgMng.u8AutoSetBaudResult = 1;	

		pWHMeterCommMng->sAutoCfgMng.u8CfgTimePriceModeTryCnt = CFG_METER_TRY_CNT_MAX;
		pWHMeterCommMng->sAutoCfgMng.u8CfgTimePriceModeFlag = TRUE;

		pWHMeterCommMng->sAutoCfgMng.u8CfgTimeTryCnt = CFG_METER_TRY_CNT_MAX;
		pWHMeterCommMng->sAutoCfgMng.u8CfgTimeFlag = TRUE;
	}
	else
	{
		pWHMeterCommMng->sAutoCfgMng.u8AutoSetBaudResult = 2;
	}
}

//void AutoGetAddr(UCHAR u8MeterNo)
//{
//	UINT8 u8SetBaud, i;
//	UINT8 u8gWHAddr_Buf[6];
//
//	if(u8MeterNo > 1)
//	{
//		return;
//	}
//
//	if(gs_MeterAutoCfgMng.u8EnterAutoGetAddrFlag[u8MeterNo] == FALSE)
//	{
//		return;
//	}
//	if((gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == START_CHARGE_VAL)
//		|| (gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == START_CHARGE_VAL))	
//	{
//		return;
//	}
//
//	gs_MeterAutoCfgMng.u8EnterAutoGetAddrFlag[u8MeterNo] = FALSE;
//
//	if(gEvChargeInfo.u8MeterBaud == 0)  //9600
//	{		
//		u8SetBaud = m9600;
//	}
//	else if(gEvChargeInfo.u8MeterBaud == 1)  //19200
//	{		
//		u8SetBaud = m19200;
//	}
//	else //2400
//	{		
//		u8SetBaud = m2400;
//	}
//
////	u8SetBaud = m9600;
//
//	dv_Uart4.fInitUart(u8SetBaud, UARTX_PARITY_EVEN);				//InitUart5
//	dv_Uart4.fEnableRx();							//使能收中断
//	OSTimeDly(5);
//	WHGL_WakeUp_1();
//	//OSTimeDly(50);
//	UINT16 uIdx = 0;
//	BYTE byFrameCtrlCode = 0x13;
//	gUart4TXBuff[0] = HW_SOI1;
//	gUart4TXBuff[1] = 0xAA;						//99广播地址;
//	gUart4TXBuff[2] = 0xAA;
//	gUart4TXBuff[3] = 0xAA;
//	gUart4TXBuff[4] = 0xAA;
//	gUart4TXBuff[5] = 0xAA;
//	gUart4TXBuff[6] = 0xAA;
//	gUart4TXBuff[7] = HW_SOI2;
//	gUart4TXBuff[8] = byFrameCtrlCode;
//	gUart4TXBuff[9] = 0x00;							//数据长度L
//	gUart4TXBuff[10] = WHGL_U8CheckSum(gUart4TXBuff, 10);
//	gUart4TXBuff[11] = HW_EOI;
//	dv_Uart4.fSendDataTXY(gUart4TXBuff, 12);
//	gSelfTest.fSetTaskID(TASK_ID_UART4);
//	OSTimeDly(100);
//
//	if (dv_Uart4.pData > 8)
//	{
//		for (uIdx = 0; uIdx < dv_Uart4.pData && uIdx < RS485_UART4_RX_MAX_NUM - 10; uIdx++)
//		{
//			if (HW_SOI1 == gUart4RecvBuff[uIdx])
//			{
//				break;
//			}
//		}
//
//		if (0x93 == gUart4RecvBuff[uIdx + 8] && 0x06 == gUart4RecvBuff[uIdx + 9])	//68H A0 … A5 68H 8AH 00H CS 16H
//		{
//			u8gWHAddr_Buf[0] = (UINT8)gUart4RecvBuff[uIdx + 1];
//			u8gWHAddr_Buf[1] = (UINT8)gUart4RecvBuff[uIdx + 2];
//			u8gWHAddr_Buf[2] = (UINT8)gUart4RecvBuff[uIdx + 3];
//			u8gWHAddr_Buf[3] = (UINT8)gUart4RecvBuff[uIdx + 4];
//			u8gWHAddr_Buf[4] = (UINT8)gUart4RecvBuff[uIdx + 5];
//			u8gWHAddr_Buf[5] = (UINT8)gUart4RecvBuff[uIdx + 6];	
//			gs_MeterAutoCfgMng.u8AutoGetAddrResult[u8MeterNo] = 1;
//
//			for(i = 0; i < 6;i++)
//			gWH_Info[u8MeterNo].Addr[i] = u8gWHAddr_Buf[i];
//		}	
//		else
//		{
//			gs_MeterAutoCfgMng.u8AutoGetAddrResult[u8MeterNo] = 2;
//		}
//	}
//	else
//	{
//		gs_MeterAutoCfgMng.u8AutoGetAddrResult[u8MeterNo] = 2;
//	}
//
//}
//void AutoCfgFeeTime(void)
//{
//	UCHAR u8MeterNo;
//
//	if((gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == START_CHARGE_VAL)
//		|| (gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == START_CHARGE_VAL))	
//	{
//		return;
//	}		
//
//		for(u8MeterNo = 0; u8MeterNo < 2; u8MeterNo++)
//		{
//			//设置时段费率参数
//			if(gs_MeterAutoCfgMng.u8CfgTimePriceModeFlag[u8MeterNo] == TRUE)
//			{
//				if(gs_MeterAutoCfgMng.u8CfgTimePriceModeTryCnt[u8MeterNo] > 0)
//				{
//					if(CfgTimePriceMode(u8MeterNo) == TRUE)
//					{
//						gs_MeterAutoCfgMng.u8CfgTimePriceModeResult[u8MeterNo] = 0;
//						gs_MeterAutoCfgMng.u8CfgTimePriceModeFlag[u8MeterNo] = FALSE;
//						gs_MeterAutoCfgMng.u8CfgTimePriceModeTryCnt[u8MeterNo] = 0;
//					}
//					else
//					{
//						gs_MeterAutoCfgMng.u8CfgTimePriceModeResult[u8MeterNo] = 1;
//						gs_MeterAutoCfgMng.u8CfgTimePriceModeTryCnt[u8MeterNo]--;
//					}
//				}			
//			}
//
//			//设置系统时间
//			if(gs_MeterAutoCfgMng.u8CfgTimeFlag[u8MeterNo] == TRUE)
//			{
//				if(gs_MeterAutoCfgMng.u8CfgTimeTryCnt[u8MeterNo] > 0)
//				{
//					if(CfgSigleMeterTime(u8MeterNo) == TRUE)
//					{
//						gs_MeterAutoCfgMng.u8CfgTimeFlag[u8MeterNo] = FALSE;
//						gs_MeterAutoCfgMng.u8CfgTimeTryCnt[u8MeterNo] = 0;
//						gs_MeterAutoCfgMng.u8CfgTimeResult[u8MeterNo] = 0;
//					}
//					else
//					{
//						gs_MeterAutoCfgMng.u8CfgTimeTryCnt[u8MeterNo]--;
//						gs_MeterAutoCfgMng.u8CfgTimeResult[u8MeterNo] = 1;
//					}
//				}			
//			}
//		} // for(u8MeterNo = 0; u8MeterNo < 2; u8MeterNo++)
//}




UCHAR Meter_Sample_Deal(WHMeter_COMM_MNG *pWHMeterCommMng)
{
	UCHAR ucMeterRtn;

	if(WH_IS_DC_3 == pWHMeterCommMng->pWHMeterInfo->u8WH_TYPE)
	{
		stDTSD3366ElectricSig1[0].byLabelDI2 = 0xE0+HW_TRANSFERS_ADD;
		stDTSD3366ElectricSig1[0].bySigForm = 3;
	}
	else
	{
		stDTSD3366ElectricSig1[0].byLabelDI2 = 0x00+HW_TRANSFERS_ADD;
		stDTSD3366ElectricSig1[0].bySigForm  = 2;
	}

	ucMeterRtn = Routine_Read_Meter_Data_Deal(pWHMeterCommMng);

	return ucMeterRtn;

}

void InitWHMeter_CommMng(UINT8 u8MeterNo, UINT8 u8UartNo)
{
	if(u8MeterNo >= WH_Meter_NUM)
	{
		return;
	}
	if(u8UartNo == UART_NO_UART4)
	{
		gs_WHMeterCommMng[u8MeterNo].u8UartNo = UART_NO_UART4;
		gs_WHMeterCommMng[u8MeterNo].FeedDogTaskID = TASK_ID_UART4;

		gs_WHMeterCommMng[u8MeterNo].pUartXTXBuff = gUart4TXBuff;
		gs_WHMeterCommMng[u8MeterNo].pUartXRecvBuff = gUart4RecvBuff;

		gs_WHMeterCommMng[u8MeterNo].pUartDev = &dv_Uart4;
	}
	else if(u8UartNo == UART_NO_UART5)
	{
		gs_WHMeterCommMng[u8MeterNo].u8UartNo = UART_NO_UART5;
		gs_WHMeterCommMng[u8MeterNo].FeedDogTaskID = TASK_ID_UART5;

		gs_WHMeterCommMng[u8MeterNo].pUartXTXBuff = gUart5TXBuff;
		gs_WHMeterCommMng[u8MeterNo].pUartXRecvBuff = gUart5RecvBuff;

		gs_WHMeterCommMng[u8MeterNo].pUartDev = &dv_Uart5;
	}
	else
	{
		return;
	}

	gs_WHMeterCommMng[u8MeterNo].u8CommAlm = 0;
	gs_WHMeterCommMng[u8MeterNo].u8CommFailCount = 0;
	gs_WHMeterCommMng[u8MeterNo].u8SampleCycleCount = 0;

	gs_WHMeterCommMng[u8MeterNo].pWHMeterInfo = &gWH_Info[u8MeterNo];
}

//20211027 lkf 预留电表内容
void Task_Sample_WHMeter(void)
{
	static UINT8 su8FirstRun = 1;

	
	if(su8FirstRun)
	{
		su8FirstRun = 0;

		gEvChargeInfo.u8MeterBaud = 0;

		GetIntervalSampleNo();	

		InitWHMeter_CommMng(WH_Meter_No_0, UART_NO_UART4);

		InitWHMeter_CommMng(WH_Meter_No_1, UART_NO_UART5);

		gs_WHMeterCommMng[WH_Meter_No_0].sAutoCfgMng.u8CfgTimePriceModeTryCnt = CFG_METER_TRY_CNT_MAX;
		gs_WHMeterCommMng[WH_Meter_No_0].sAutoCfgMng.u8CfgTimePriceModeFlag = 0;
		gs_WHMeterCommMng[WH_Meter_No_0].sAutoCfgMng.u8CfgTimeTryCnt = CFG_METER_TRY_CNT_MAX;
		gs_WHMeterCommMng[WH_Meter_No_0].sAutoCfgMng.u8CfgTimeFlag = 0;

		gs_WHMeterCommMng[WH_Meter_No_1].sAutoCfgMng.u8CfgTimePriceModeTryCnt = CFG_METER_TRY_CNT_MAX;
		gs_WHMeterCommMng[WH_Meter_No_1].sAutoCfgMng.u8CfgTimePriceModeFlag = 0;
		gs_WHMeterCommMng[WH_Meter_No_1].sAutoCfgMng.u8CfgTimeTryCnt = CFG_METER_TRY_CNT_MAX;
		gs_WHMeterCommMng[WH_Meter_No_1].sAutoCfgMng.u8CfgTimeFlag = 0;
	}

	//电表1
	Meter_Sample_Deal(&gs_WHMeterCommMng[WH_Meter_No_0]);

	//电表2
	Meter_Sample_Deal(&gs_WHMeterCommMng[WH_Meter_No_1]);
}