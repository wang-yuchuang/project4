#include "main.h"
#include "secondary_upgrade.h"

#define	RS485_TX_MAX_NUM_IMEU	MIN(RS485_UART4_TX_MAX_NUM, RS485_UART5_TX_MAX_NUM)
#define	RS485_RX_MAX_NUM_IMEU	MIN(RS485_UART4_RX_MAX_NUM, RS485_UART5_RX_MAX_NUM)

extern NOINIT UINT8 gUart4RecvBuff[RS485_UART4_RX_MAX_NUM];
extern NOINIT UINT8 gUart4TXBuff[RS485_UART4_TX_MAX_NUM];

extern NOINIT UINT8 gUart5RecvBuff[RS485_UART5_RX_MAX_NUM];
extern NOINIT UINT8 gUart5TXBuff[RS485_UART5_TX_MAX_NUM];

//UINT8 u8OilPumpAlarm(IMEU_INFO *pImeuInfo)
//{
//	UINT8 u8GunNo;
//	if(pImeuInfo->u8UartNo == UART_NO_UART4)
//	{
//		u8GunNo = 0;
//	}
//	else
//	{
//		u8GunNo = 1;
//	}
//
//	 if(gEvChargeInfo.u8Gun1OilPumpMask[u8GunNo] == 1)
//	 {
//		return FALSE;
//	 }
//
//	 if(pImeuInfo->u8InterSide_1_CycleAlm == TRUE && (gEvChargeInfo.u8Gun1OilPumpMask[u8GunNo]  == 0)) //内循环告警 条件： 压力/DI/液位
//	 {
//		return TRUE;
//	 }
//
//	 if(pImeuInfo->DIStat_1.BIT_FIELD.DI6 && (gEvChargeInfo.u8Gun1OilPumpMask[u8GunNo]  == 0))//油位低！！
//	 {
//		return TRUE;
//	 }
//	 if((pImeuInfo->u8_iol_Bump_State != 0) && (gEvChargeInfo.u8Gun1OilPumpMask[u8GunNo]  == 0))
//	 {
//		return TRUE;
//	 }
//	 return FALSE;
//}

void UartXSendData_Deal_IMEU(IMEU_INFO *pImeuInfo, UINT16 data2)
{
	if(pImeuInfo->u8UartNo == UART_NO_UART4)
	{
		dv_Uart4.fSendDataTXY(pImeuInfo->pUartXTXBuff, data2);
		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
		//RS485_UART4_SendDlay(5);
	}
	else
	{
		dv_Uart5.fSendDataTXY(pImeuInfo->pUartXTXBuff, data2);
		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
		//RS485_UART5_SendDlay(5);
	}	
}
UINT8 u8UartXRcvIsOK_IMEU2(IMEU_INFO *pImeuInfo, UINT16* pVal)
{
	UINT16 iStartPst = 0;
	UINT8 u8RevSOI = FALSE;
	UINT8 u8RevEOI = FALSE;
	if(pImeuInfo->u8UartNo == UART_NO_UART4)
	{
		pImeuInfo->u16RcvLenth = dv_Uart4.pData;
	}
	else
	{
		pImeuInfo->u16RcvLenth = dv_Uart5.pData;
	}

	if (pImeuInfo->u16RcvLenth < 13)
	{
		return FALSE;
	}

	for (iStartPst = 0; iStartPst < pImeuInfo->u16RcvLenth && iStartPst < RS485_RX_MAX_NUM_IMEU; iStartPst++)
	{
		if (YDN23_START == pImeuInfo->pUartXRecvBuff[iStartPst])
		{
			*pVal = iStartPst;
			u8RevSOI = TRUE;
			break;
		}
	}
	if (FALSE == u8RevSOI)
	{
		return FALSE;
	}

	for (; iStartPst < pImeuInfo->u16RcvLenth && iStartPst < RS485_RX_MAX_NUM_IMEU; iStartPst++)
	{
		if (YDN23_END == pImeuInfo->pUartXRecvBuff[iStartPst])
		{
			u8RevEOI = TRUE;
			break;
		}
	}

	if ((TRUE == u8RevEOI) && (TRUE == u8RevSOI))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT8 u8CheckYDN23IsValid_IMEU(IMEU_INFO *pImeuInfo, UINT16 SOIPst)
{
	//UINT8* p5Buf = 0;
	//UINT8* p5Buf = 0;
	UINT8 CID1 = 0;
	UINT8 CID2 = 0;
	UINT8 ADDR = 0;
	UINT16 IICRC = 0;
	UINT16 i;
	UINT16 RevCRC = 0;
	UINT16 pCheckSumPst = 0;
	UINT8*  p5Buf = pImeuInfo->pUartXRecvBuff;

	SOIPst = MIN((RS485_RX_MAX_NUM_IMEU - (YDN23_CID2_PST + 1)), SOIPst);	//防止溢出！
	p5Buf = pImeuInfo->pUartXRecvBuff + SOIPst;
	ADDR = TwoAscii2Hex(p5Buf[YDN23_ADDR_PST], p5Buf[YDN23_ADDR_PST+1]);
	CID1 = (UINT8)TwoAscii2Hex(p5Buf[YDN23_CID1_PST], p5Buf[YDN23_CID1_PST + 1]);
	CID2 = (UINT8)TwoAscii2Hex(p5Buf[YDN23_CID2_PST], p5Buf[YDN23_CID2_PST + 1]);
	if ((pImeuInfo->u8DevAddr) != ADDR)
	{
		return FALSE;
	}

	if (CID1 != YDN23_IMEU_CID1)
	{
		return FALSE;
	}

	if (CID2 != 0)				//RTN
	{
		return FALSE;
	}

	//IICRC = CalcRcvCheckSum(UART5_SEQUECE_NO);
	//IICRC = Uart5_CalcRcvCheckSum();
	//UINT16 i = 0;
	//UINT16 IICRC = 0;
	for (i = 0; i < (pImeuInfo->u16RcvLenth - 6) && (i < RS485_RX_MAX_NUM_IMEU); i++)
	{
		IICRC += p5Buf[YDN23_VER_PST + i];
	}
	IICRC = IICRC%65536;
	IICRC = ((~IICRC)+1);

	pCheckSumPst = pImeuInfo->u16RcvLenth - 5;
	pCheckSumPst = MIN((RS485_RX_MAX_NUM_IMEU - 5), pCheckSumPst);//防止溢出！
	RevCRC = (UINT16)(TwoAscii2Hex(p5Buf[pCheckSumPst], p5Buf[pCheckSumPst + 1]) << 8)
			+ TwoAscii2Hex(p5Buf[pCheckSumPst + 2], p5Buf[pCheckSumPst + 2 + 1]);
	if (IICRC != RevCRC)
	{
		return FALSE;
	}
	return TRUE;
}



UINT8 Pack_IMEU_Cmd(IMEU_INFO *pImeuInfo, UINT8 CID1, UINT8 CID2,UINT8 u8DataLen, UINT8* pData)
{
	UINT8 lenChk = 0;
	UINT16 iLenghId = 0;
	UINT8 CheckSumH = 0;
	UINT8 CheckSumL = 0;
	UINT16 iCHECKSUM = 0;
	UINT8 u8SendLengh = 0;
	UINT8 kk;
	memset(pImeuInfo->pUartXTXBuff, 0, RS485_TX_MAX_NUM_IMEU - 1);
	pImeuInfo->pUartXTXBuff[0]= YDN23_SOI_VAL;
	pImeuInfo->pUartXTXBuff[1] = Hex2Ascii(YDN23_VER_VAL>>4);
	pImeuInfo->pUartXTXBuff[2] = Hex2Ascii(YDN23_VER_VAL&0x0f);
	pImeuInfo->pUartXTXBuff[3] = Hex2Ascii((pImeuInfo->u8DevAddr)>>4);
	pImeuInfo->pUartXTXBuff[4] = Hex2Ascii((pImeuInfo->u8DevAddr)&0x0f);
	pImeuInfo->pUartXTXBuff[5] = Hex2Ascii(CID1>>4);
	pImeuInfo->pUartXTXBuff[6] = Hex2Ascii(CID1&0x0f);
	pImeuInfo->pUartXTXBuff[7] = Hex2Ascii(CID2>>4);
	pImeuInfo->pUartXTXBuff[8] = Hex2Ascii(CID2&0x0f);

	iLenghId = 13;
	if(u8DataLen > 0)
	{
		for(kk = 0; kk < u8DataLen; kk++)
		{
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[kk]>>4);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[kk]&0x0f);
		}
	}

	lenChk = LenCheckSum((iLenghId -13));//Because init value 13
	pImeuInfo->pUartXTXBuff[9] = Hex2Ascii((lenChk & 0x0f));
	pImeuInfo->pUartXTXBuff[10] = Hex2Ascii(((iLenghId -13)>>8) & 0x0f);
	pImeuInfo->pUartXTXBuff[11] = Hex2Ascii(((iLenghId -13) & 0xf0)>>4);
	pImeuInfo->pUartXTXBuff[12] = Hex2Ascii((iLenghId -13) & 0x0f);
	iCHECKSUM = CalcSendCheckSum(pImeuInfo->pUartXTXBuff, iLenghId);
	CheckSumH = 0xff&(iCHECKSUM>>8);
	CheckSumL = 0xff&(iCHECKSUM);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumH>>4);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumH&0x0f);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumL>>4);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumL&0x0f);
	pImeuInfo->pUartXTXBuff[iLenghId++] = YDN23_EOI_VAL;
	u8SendLengh = iLenghId;
	return u8SendLengh;
}



void UnPackCMD_IMEU_41Data(IMEU_INFO *pImeuInfo, UINT16 SOIPst)
{
	UINT8 u8TempBuf[20];
	float fVal = 0;
    UNUSED(fVal);
	UINT8* p5Buf = pImeuInfo->pUartXRecvBuff + SOIPst;
	UINT16 u16Value= 0;
	UNUSED(u16Value);

	if (RS485_RX_MAX_NUM_IMEU - SOIPst <= 110)					//协议发生变化 需要更改
	{
		return;													//后边数组没空间进行解析了！
	}
	UINT8 u8CID2Check = 0;
	p5Buf = p5Buf + 13;
	u8CID2Check = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	p5Buf+=2;
	if(0x41!= u8CID2Check)
	{
		return;
	}
	//2	DI[9-16] 
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	pImeuInfo->DIStat_2.BYTE_FIELD = u8TempBuf[0];
	p5Buf += 2;
	//3	DI[1-8]
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	pImeuInfo->DIStat_1.BYTE_FIELD = u8TempBuf[0];
	p5Buf += 2;

	////4	WS_AB
	//pImeuInfo->WS_AB_Alm = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	//p5Buf += 2;

	//6	D0[9-16]
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	pImeuInfo->DOStat_2.BYTE_FIELD = u8TempBuf[0];
	p5Buf += 2;

	//5	D0[1-8]
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	pImeuInfo->DOStat_1.BYTE_FIELD = u8TempBuf[0];
	p5Buf += 2;

	

	//7	D0[17-24]
	/*u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	pImeuInfo->DOStat_3.BYTE_FIELD = u8TempBuf[0];
	p5Buf += 2;*/

	//8	水泵状态 
	//0正常、
	//1空转、
	//2 PWM输入悬空、
	//3堵转/过流
	//其他 待定
	//pImeuInfo->u8_Water_Bump_State  = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	//p5Buf += 2;

	////9 油泵状态
	//pImeuInfo->u8_iol_Bump_State  = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	//p5Buf += 2;

	////10	第1路转速控制
	//pImeuInfo->u8_ch_1_Speed  = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	//p5Buf += 2;

	////11	第2路转速控制
	//pImeuInfo->u8_ch_2_Speed  = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	//p5Buf += 2;

	////12	第3路转速控制
	//pImeuInfo->u8_ch_3_Speed  = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	//p5Buf += 2;

	////13	第4路转速控制
	//pImeuInfo->u8_ch_4_Speed  = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	//p5Buf += 2;

	////14	转速1回采
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u16_ch_1_Back_Speed = (((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1];

	////15	转速2回采
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u16_ch_2_Back_Speed = (((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1];

	////16	转速3回采
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u16_ch_3_Back_Speed = (((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1];

	////17	转速4回采
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u16_ch_4_Back_Speed = (((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1];

	//18	TP1
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_1_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//19	TP2
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_2_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//20	TP3
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_3_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//21	TP4
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_4_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//22	TP5
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_5_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//23	TP6
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_6_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//24	TP7
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_7_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//25	TP8
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_8_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//26	TP9
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_9_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	
	//27	TP10
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	pImeuInfo->fTP1000_10_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa

	////4	WS_AB
	pImeuInfo->WS_AB_Alm = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	p5Buf += 2;

	//PWM1实际风机转速控制（百分比）
	pImeuInfo->u8_ch_1_Speed = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	p5Buf += 2;

	//PWM1实际风机转速控制（百分比）
	pImeuInfo->u8_ch_2_Speed = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	p5Buf += 2;
	
	////28	TP11
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->fTP1000_11_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	//
	////29	TP12
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->fTP1000_12_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa
	//
	////30	TP13
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->fTP1000_13_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa

	////31	TP14
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->fTP1000_14_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa

	////32	TP15
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->fTP1000_15_Val = (((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1)-50;//Mpa

	////33	PS1
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->f_PS_1_Val = ((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.001;//Mpa

	////34	PS2
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->f_PS_2_Val = ((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.001;//Mpa

	////35	Analog(Current)
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->f_Analog_Val = ((((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1])*0.1;//L/min
	
	//36	外循环1告警
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u8OutSide_1_CycleAlm = u8TempBuf[0];

	////37	外循环2告警
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u8OutSide_2_CycleAlm = u8TempBuf[0];
	////38	内循环1告警
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u8InterSide_1_CycleAlm = u8TempBuf[0];
	////39	内循环2告警
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u8InterSide_2_CycleAlm = u8TempBuf[0];
	////40	充电枪1过温
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//if((gEvChargeInfo.u8Is70SystemFlag == SYSTEM_IS_140_KW))
	//{
	//	pImeuInfo->u8OverTempOfGun_1 = u8TempBuf[0];			//2021年2月5日  IMEU 140千瓦会上报  70千瓦就不会报告警
	//}
	////41	充电枪2过温
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	////pImeuInfo->u8OverTempOfGun_2 = //u8TempBuf[0];			//2021年2月5日 IMEU 永远不会报告警
	////42	充电枪3过温
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//if((gEvChargeInfo.u8Is70SystemFlag == SYSTEM_IS_140_KW))
	//{
	//	pImeuInfo->u8OverTempOfGun_3 = u8TempBuf[0];			//2021年2月5日  IMEU 140千瓦会上报  70千瓦就不会报告警
	//}

	//43	充电枪4过温
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	////pImeuInfo->u8OverTempOfGun_4 = //u8TempBuf[0];			//2021年2月5日 IMEU 永远不会报告警
	////枪PT1000连接情况
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//pImeuInfo->u8PT1_PT2Disconnect = u8TempBuf[0] &0x01;
	//pImeuInfo->u8PT3_PT4Disconnect = (u8TempBuf[0]>>1) &0x01;
	//pImeuInfo->u8PT5_PT6Disconnect = (u8TempBuf[0]>>2) &0x01;
	//pImeuInfo->u8PT7_PT8Disconnect = (u8TempBuf[0]>>3) &0x01;

	////44	告警1-9
	//p5Buf += 18;

	////45	用户自定义信号数P  2字节
	//u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//u16Value = ((((UINT16)u8TempBuf[0]) << 8) + u8TempBuf[1]);
	//if(u16Value>=3)
	//{
	//	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//	pImeuInfo->u8_HW_Version[0] = 'A';
	//	pImeuInfo->u8_HW_Version[1] = '0';
	//	pImeuInfo->u8_HW_Version[2] = 0x30+u8TempBuf[0];
	//	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	//	u16Value = ((((UINT16)u8TempBuf[0]) << 8) + u8TempBuf[1]);
	//	
	//	
	//	pImeuInfo->u8_SW_Version[0] = 0x30 + (u16Value/100);
	//	pImeuInfo->u8_SW_Version[1] = 0x30 + ((u16Value%100)/10);
	//	pImeuInfo->u8_SW_Version[2] = 0x30 + (u16Value%10);
	//	
	//}
}





void UnPackCMD_IMEU_51Data(IMEU_INFO *pImeuInfo, UINT16 SOIPst)
{
	UINT8 u8TempBuf[20];
	float fVal = 0;
	UNUSED(fVal);
	UINT8* p5Buf = pImeuInfo->pUartXRecvBuff + SOIPst;
	UINT16 u16Value= 0;

	if (RS485_RX_MAX_NUM_IMEU - SOIPst <= 110)					//协议发生变化 需要更改
	{
		return;													//后边数组没空间进行解析了！
	}
	p5Buf = p5Buf + 15;

	p5Buf += 10*2; //跳过
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u16Value = (((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1];

	pImeuInfo->fHwVersion = (float)u16Value;


	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u8TempBuf[1] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));p5Buf += 2;
	u16Value = (((UINT16)u8TempBuf[0])<<8) + u8TempBuf[1];

	pImeuInfo->fSwVersion = (float)u16Value/100;
	
}




static void IMEU_SampleCMD41_1(IMEU_INFO *pImeuInfo)
{
	UINT8 Rst = FALSE;
	UINT16 SOIPst = 0;
	UINT8 uUAR5_Idx = 0;
	UINT8 u8ChargingState[3];
	UINT8 u8Len = 0;
	//__XXX_UNION_VALUE unTempVal;
	memset(u8ChargingState, 0, 3);

	/*
	if(pImeuInfo->u8UartNo == UART_NO_UART4)
	{
		if(gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == START_CHARGE_VAL || gEvChargeInfo.u8_A_TestMode == TRUE)
		{
			//if(gEvChargeInfo.u8_A_BUS_Gun_No == 2)
			{
			//	u8ChargingState[0] |= (0x01 << 1);
			}
			//else
			{
				u8ChargingState[0] |= (0x01 << 0);
			}
		}
	}

	if(pImeuInfo->u8UartNo == UART_NO_UART5)
	{
		if(gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == START_CHARGE_VAL || gEvChargeInfo.u8_B_TestMode == TRUE)
		{
			//if(gEvChargeInfo.u8_B_BUS_Gun_No == 3)
			{
			//	u8ChargingState[0] |= (0x01 << 2);
			}
			//else
			{
				u8ChargingState[0] |= (0x01 << 0);
			}
		}
	}

	if((pImeuInfo->u8UartNo == UART_NO_UART4)
		&& (gEvChargeInfo.u8Gun1OilPumpMask[0]==0))  //有 油泵		0：使能		1：禁止
	{
		u8ChargingState[1] = 0;
	}
	else if((pImeuInfo->u8UartNo == UART_NO_UART5)
		&& (gEvChargeInfo.u8Gun1OilPumpMask[1]==0))  //有 油泵		0：使能		1：禁止
	{
		u8ChargingState[1] = 0;
	}
	else
	{
		u8ChargingState[1] = 1;
		//if(gEvChargeInfo.u8Is70SystemFlag == SYSTEM_IS_70_KW)
		//{
		//	u8ChargingState[1] |= 0x02;//目的是让IMEU的软件不跑140KW逻辑里的关油泵电源，它用的DO11
		//}
	}*/

	u8Len = Pack_IMEU_Cmd(pImeuInfo, YDN23_IMEU_CID1, 0x41, 0, u8ChargingState);
	memset(pImeuInfo->pUartXRecvBuff , 0, (RS485_RX_MAX_NUM_IMEU - 1));
	//dv_Uart5.fSendDataTXY(gUart5TXBuff, u8Len);
	pImeuInfo->UartXSendData(pImeuInfo,u8Len);
	OSTimeDly(50);
	//RS485_UART5_SendDlay(200);
	for (uUAR5_Idx = 0; uUAR5_Idx < 10; uUAR5_Idx++)
	{
		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
		if (u8UartXRcvIsOK_IMEU2(pImeuInfo, &SOIPst))
		{
			gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
			if (u8CheckYDN23IsValid_IMEU(pImeuInfo, SOIPst))
			{
				gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
				UnPackCMD_IMEU_41Data(pImeuInfo, SOIPst);
				Rst = TRUE;
				pImeuInfo->u8CommCount = 0;
				pImeuInfo->u8CommError = FALSE;
			}
			break;
		}
		OSTimeDly(35);
	}

	if (FALSE == Rst)
	{
		pImeuInfo->u8CommCount++;
		if(pImeuInfo->u8CommCount > 5)
		{
			pImeuInfo->u8CommError = TRUE;
			if(pImeuInfo->u8UartNo == UART_NO_UART4)
			{
				dv_Uart4.fInitUart(m9600, UARTX_PARITY_NONE);				//InitUart4
				dv_Uart4.fEnableRx();				
			}
			else
			{
				dv_Uart5.fInitUart(m9600, UARTX_PARITY_NONE);				//InitUart5
				dv_Uart5.fEnableRx();				
			}
			OSTimeDly(10);
		}
	}
}

static void IMEU_SampleCMD51_1(IMEU_INFO *pImeuInfo)
{
	UINT16 SOIPst = 0;
	UINT8 uUAR5_Idx = 0;
	UINT8 u8Len = 0;

	u8Len = Pack_IMEU_Cmd(pImeuInfo, YDN23_IMEU_CID1, 0x51, 0, 0);
	memset(pImeuInfo->pUartXRecvBuff , 0, (RS485_RX_MAX_NUM_IMEU - 1));
	//dv_Uart5.fSendDataTXY(gUart5TXBuff, u8Len);
	pImeuInfo->UartXSendData(pImeuInfo,u8Len);
	OSTimeDly(50);
	//RS485_UART5_SendDlay(200);
	for (uUAR5_Idx = 0; uUAR5_Idx < 10; uUAR5_Idx++)
	{
		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
		if (u8UartXRcvIsOK_IMEU2(pImeuInfo, &SOIPst))
		{
			gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
			if (u8CheckYDN23IsValid_IMEU(pImeuInfo, SOIPst))
			{
				gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
				UnPackCMD_IMEU_51Data(pImeuInfo, SOIPst);
				pImeuInfo->u8CommCount = 0;
				pImeuInfo->u8CommError = FALSE;

				pImeuInfo->u8GotVersionFlag = 1;
			}
			break;
		}
		OSTimeDly(35);
	}

	
}
//UINT8 Pack_IMEU_XXXCmd(UINT8 CID1,UINT8 CID2,UINT8 CMDType, UINT8 uAddr, UINT8 u8ArgNum, float *pfloatVal)
//{
//	//float fVal1 = 0;
//	//float fVal2 = 0;
//	//UINT8 u8Val = 0;
//	UINT8 lenChk = 0;
//	UINT16 iLenghId = 0;
//	UINT8 CheckSumH = 0;
//	UINT8 CheckSumL = 0;
//	UINT16 iCHECKSUM = 0;
//	UINT8 u8SendLengh = 0;
//	UINT8 pFloatArarry[12];
//	//__XXX_UNION_VALUE unTempVAL;
//	memset(gUart5TXBuff, 0, RS485_UART5_TX_MAX_NUM - 1);
//	gUart5TXBuff[0]= YDN23_SOI_VAL;
//	gUart5TXBuff[1] = Hex2Ascii(YDN23_VER_VAL>>4);
//	gUart5TXBuff[2] = Hex2Ascii(YDN23_VER_VAL&0x0f);
//	gUart5TXBuff[3] = Hex2Ascii((YDN23_IMIU_ADDR+uAddr)>>4);
//	gUart5TXBuff[4] = Hex2Ascii((YDN23_IMIU_ADDR+uAddr)&0x0f);
//	gUart5TXBuff[5] = Hex2Ascii(CID1>>4);
//	gUart5TXBuff[6] = Hex2Ascii(CID1&0x0f);
//	gUart5TXBuff[7] = Hex2Ascii(CID2>>4);
//	gUart5TXBuff[8] = Hex2Ascii(CID2&0x0f);
//
//	if (0 == u8ArgNum)
//	{
//		iLenghId = 0;
//		lenChk = LenCheckSum(iLenghId);						//Because init value 13
//		gUart5TXBuff[9] = Hex2Ascii((lenChk & 0x0f));
//		gUart5TXBuff[10] = Hex2Ascii((iLenghId >> 8) & 0x0f);
//		gUart5TXBuff[11] = Hex2Ascii(((iLenghId) & 0xf0)>>4);
//		gUart5TXBuff[12] = Hex2Ascii((iLenghId) & 0x0f);
//
//		iCHECKSUM = CalcSendCheckSum(gUart5TXBuff, 12);
//		CheckSumH = 0xff&(iCHECKSUM>>8);
//		CheckSumL = 0xff&(iCHECKSUM);
//		gUart5TXBuff[13] = Hex2Ascii(CheckSumH>>4);
//		gUart5TXBuff[14] = Hex2Ascii(CheckSumH&0x0f);
//		gUart5TXBuff[15] = Hex2Ascii(CheckSumL>>4);
//		gUart5TXBuff[16] = Hex2Ascii(CheckSumL&0x0f);
//		gUart5TXBuff[17] = YDN23_EOI_VAL;
//		u8SendLengh = 18;
//		return u8SendLengh;
//	}
//	else
//	{
//		if (0x48 == CID2)
//		{
//			//switch (CMDType)
//			//{
//			iLenghId = 13;
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(CMDType>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(CMDType&0x0f);
//
//			floattoascii(pfloatVal[0], pFloatArarry);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[0]>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[0]&0x0f);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[1]>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[1]&0x0f);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[2]>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[2]&0x0f);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[3]>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[3]&0x0f);
//			
//			floattoascii(pfloatVal[1], pFloatArarry);		
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[0]>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[0]&0x0f);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[1]>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[1]&0x0f);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[2]>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[2]&0x0f);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[3]>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(pFloatArarry[3]&0x0f);
//
//			lenChk = LenCheckSum((iLenghId -13));//Because init value 13
//			gUart5TXBuff[9] = Hex2Ascii((lenChk & 0x0f));
//			gUart5TXBuff[10] = Hex2Ascii(((iLenghId -13)>>8) & 0x0f);
//			gUart5TXBuff[11] = Hex2Ascii(((iLenghId -13) & 0xf0)>>4);
//			gUart5TXBuff[12] = Hex2Ascii((iLenghId -13) & 0x0f);
//			iCHECKSUM = CalcSendCheckSum(gUart5TXBuff, iLenghId);
//			CheckSumH = 0xff&(iCHECKSUM>>8);
//			CheckSumL = 0xff&(iCHECKSUM);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(CheckSumH>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(CheckSumH&0x0f);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(CheckSumL>>4);
//			gUart5TXBuff[iLenghId++] = Hex2Ascii(CheckSumL&0x0f);
//			gUart5TXBuff[iLenghId++] = YDN23_EOI_VAL;
//			u8SendLengh = iLenghId;
//			return u8SendLengh;
//			//}
//		}
//		return 0;
//	}
//}

//0 状态没有变化不需要发送！
//1 状态发生变化，启动充电！
//2 状态发生变化，停止充电！
UINT8 CheckChargingState(IMEU_INFO *pImeuInfo)
{
	static UINT8 su8LastState_1 = 5;
	static UINT8 su8LastState_2 = 5;
	UINT8 u8NowState = 0;

	if(pImeuInfo->u8UartNo == UART_NO_UART4)
	{
		if(gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == START_CHARGE_VAL)
		{
			u8NowState = 1;
		}
		else
		{
			u8NowState = 2;
		}

		if(su8LastState_1 != u8NowState)
		{
			su8LastState_1 = u8NowState;
			return u8NowState;
		}
		else
		{
			return 0;
		}
	}
	else if(pImeuInfo->u8UartNo == UART_NO_UART5)
	{
		if(gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == START_CHARGE_VAL)
		{
			u8NowState = 1;
		}
		else
		{
			u8NowState = 2;
		}

		if(su8LastState_2 != u8NowState)
		{
			su8LastState_2 = u8NowState;
			return u8NowState;
		}
		else
		{
			return 0;
		}
	}
	
	/*if(gEvChargeInfo.u8_B_Charge_CMD == START_CHARGE_VAL || gEvChargeInfo.u8_A_Charge_CMD == START_CHARGE_VAL)
	{
		u8NowState = 1;
	}
	else
	{
		u8NowState = 2;
	}*/

	/*if(su8LastState != u8NowState)
	{
		su8LastState = u8NowState;
		return u8NowState;
	}
	else
	{
		return 0;
	}*/

	return 0;
}

UINT8 IMEU_0X44_CMD(IMEU_INFO *pImeuInfo, UINT8 u8CmdType, UINT8* pData)
{
	UINT8 u8CID2Check = 0;
	UINT8* p5Buf = NULL;
	UINT8 lenChk = 0;
	UINT16 iLenghId = 0;
	UINT8 CheckSumH = 0;
	UINT8 CheckSumL = 0;
	UINT16 iCHECKSUM = 0;
	UINT8 u8SendLengh = 0;
	memset(pImeuInfo->pUartXTXBuff, 0, RS485_TX_MAX_NUM_IMEU - 1);
	pImeuInfo->pUartXTXBuff[0]= YDN23_SOI_VAL;
	pImeuInfo->pUartXTXBuff[1] = Hex2Ascii(YDN23_VER_VAL>>4);
	pImeuInfo->pUartXTXBuff[2] = Hex2Ascii(YDN23_VER_VAL&0x0f);
	pImeuInfo->pUartXTXBuff[3] = Hex2Ascii((pImeuInfo->u8DevAddr)>>4);
	pImeuInfo->pUartXTXBuff[4] = Hex2Ascii((pImeuInfo->u8DevAddr)&0x0f);
	pImeuInfo->pUartXTXBuff[5] = Hex2Ascii(YDN23_IMEU_CID1>>4);
	pImeuInfo->pUartXTXBuff[6] = Hex2Ascii(YDN23_IMEU_CID1&0x0f);
	pImeuInfo->pUartXTXBuff[7] = Hex2Ascii(0x44>>4);
	pImeuInfo->pUartXTXBuff[8] = Hex2Ascii(0x44&0x0f);
	iLenghId = 13;
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(u8CmdType>>4);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(u8CmdType&0x0f);
	switch (u8CmdType)
	{
	case YDN23_IMEU_44CMD_MAX_BUMP_SPEED:
		pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]>>4);	
		pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]&0x0f);
		pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
		pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
		pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
		pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
		pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
		pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
		break;
	default:
		return FALSE;
	}

	lenChk = LenCheckSum((iLenghId -13));//Because init value 13
	pImeuInfo->pUartXTXBuff[9] = Hex2Ascii((lenChk & 0x0f));
	pImeuInfo->pUartXTXBuff[10] = Hex2Ascii(((iLenghId -13)>>8) & 0x0f);
	pImeuInfo->pUartXTXBuff[11] = Hex2Ascii(((iLenghId -13) & 0xf0)>>4);
	pImeuInfo->pUartXTXBuff[12] = Hex2Ascii((iLenghId -13) & 0x0f);
	iCHECKSUM = CalcSendCheckSum(pImeuInfo->pUartXTXBuff, iLenghId);
	CheckSumH = 0xff&(iCHECKSUM>>8);
	CheckSumL = 0xff&(iCHECKSUM);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumH>>4);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumH&0x0f);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumL>>4);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumL&0x0f);
	pImeuInfo->pUartXTXBuff[iLenghId++] = YDN23_EOI_VAL;
	u8SendLengh = iLenghId;
	memset(pImeuInfo->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMEU - 1));
	pImeuInfo->UartXSendData(pImeuInfo,u8SendLengh);
	//gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//RS485_UART5_SendDlay(5);
	UINT8 uUAR5_Idx = 0;
	UINT16 SOIPst = 0;
	//1S超时！
	for (uUAR5_Idx = 0; uUAR5_Idx < 10; uUAR5_Idx++)
	{
		if (u8UartXRcvIsOK_IMEU2(pImeuInfo, &SOIPst))
		{
			gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
			if (u8CheckYDN23IsValid_IMEU(pImeuInfo, SOIPst))
			{
				p5Buf = pImeuInfo->pUartXRecvBuff + SOIPst;
				p5Buf = p5Buf + 13;
				u8CID2Check = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
				p5Buf+=2;
				if(0x45 == u8CID2Check)
				{
					gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
					return TRUE;
				}
			}
			break;
		}
		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
		OSTimeDly(50);
	}
	OSTimeDly(50);
	return FALSE;
}
/*
void IMEU_OIL_MAX_Speed(IMEU_INFO *pImeuInfo)
{
	static UINT8 suFlag1 = TRUE;
        static UINT8 suFlag2 = TRUE;

	UINT8 u8SendData[7];
	memset(u8SendData, 0, 5);
	static UINT8 suLastValue1 = 0;
        static UINT8 suLastValue2 = 0;
	__XXX_UNION_VALUE unVal;
	//ExGetSigInfo(E_TYPE_SYS, ID_u8OilBumpMaxSpeed, &unVal, NULL, BY_E2PROM);
	if(pImeuInfo->u8UartNo == UART_NO_UART4)
	{
            if(suFlag1 == TRUE)
            {
               // suLastValue1 = gEvChargeInfo.u8OilPumpMaxSpeed;// unVal.chtemp[0];
		suFlag1 = FALSE;
		return;
            }
            
            if(gEvChargeInfo.u8OilPumpMaxSpeed != suLastValue1)
            {
                   // suLastValue1 = gEvChargeInfo.u8OilPumpMaxSpeed;//unVal.chtemp[0];
                    u8SendData[0] = suLastValue1; 
                    IMEU_0X44_CMD(pImeuInfo, YDN23_IMEU_44CMD_MAX_BUMP_SPEED, u8SendData);
            }
	}
        else if(pImeuInfo->u8UartNo == UART_NO_UART5)
	{
            if(suFlag2 == TRUE)
            {
                //suLastValue2 = gEvChargeInfo.u8OilPumpMaxSpeed;// unVal.chtemp[0];
		suFlag2 = FALSE;
		return;
            }
            
            if(gEvChargeInfo.u8OilPumpMaxSpeed != suLastValue2)
            {
                    //suLastValue2 = gEvChargeInfo.u8OilPumpMaxSpeed;//unVal.chtemp[0];
                    u8SendData[0] = suLastValue2; 
                    IMEU_0X44_CMD(pImeuInfo, YDN23_IMEU_44CMD_MAX_BUMP_SPEED, u8SendData);
            }
	} 
	
}
*/
UINT8 IMEU_0X45_CMD(IMEU_INFO *pImeuInfo, UINT8 u8CmdType, UINT8* pData)
{
	UINT8 lenChk = 0;
	UINT16 iLenghId = 0;
	UINT8 CheckSumH = 0;
	UINT8 CheckSumL = 0;
	UINT16 iCHECKSUM = 0;
	UINT8 u8SendLengh = 0;
	memset(pImeuInfo->pUartXTXBuff, 0, RS485_TX_MAX_NUM_IMEU - 1);
	pImeuInfo->pUartXTXBuff[0]= YDN23_SOI_VAL;
	pImeuInfo->pUartXTXBuff[1] = Hex2Ascii(YDN23_VER_VAL>>4);
	pImeuInfo->pUartXTXBuff[2] = Hex2Ascii(YDN23_VER_VAL&0x0f);
	pImeuInfo->pUartXTXBuff[3] = Hex2Ascii((pImeuInfo->u8DevAddr)>>4);
	pImeuInfo->pUartXTXBuff[4] = Hex2Ascii((pImeuInfo->u8DevAddr)&0x0f);
	pImeuInfo->pUartXTXBuff[5] = Hex2Ascii(YDN23_IMEU_CID1>>4);
	pImeuInfo->pUartXTXBuff[6] = Hex2Ascii(YDN23_IMEU_CID1&0x0f);
	pImeuInfo->pUartXTXBuff[7] = Hex2Ascii(0x45>>4);
	pImeuInfo->pUartXTXBuff[8] = Hex2Ascii(0x45&0x0f);
	iLenghId = 13;
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(u8CmdType>>4);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(u8CmdType&0x0f);
	switch (u8CmdType)
	{
		case YDN23_IMEU_CMDTYPE_CTRL_45_START_STOP:
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]>>4);			//0x01-开始充电 	0x00-停止充电
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]&0x0f);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
			break;
		case YDN23_IMEU_CMDTYPE_45_CTRL_DO:
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]>>4);			//Byte0: DO0-----DO7
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]&0x0f);		
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[1]>>4);			//Byte1: DO8-----DO15
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[1]&0x0f);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[2]>>4);		
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[2]&0x0f);	
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[3]>>4);		
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[3]&0x0f);	
			break;
		case YDN23_IMEU_CMDTYPE_CTRL_45_ADD_OIL://73
			/*pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0);			
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);*/
			//break;
		case YDN23_IMEU_CMDTYPE_CTRL_45_OIL_CONTINUE://74
			//pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]>>4);			//Byte0: 0 关油泵   1开油泵
			//pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]&0x0f);		
			//pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[1]>>4);		
			//pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[1]&0x0f);
			//pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
			//pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
			//pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0>>4);			
			//pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(0&0x0f);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]>>4);			//Byte0: DO0-----DO7
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[0]&0x0f);		
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[1]>>4);			//Byte1: DO8-----DO15
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[1]&0x0f);
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[2]>>4);		
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[2]&0x0f);	
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[3]>>4);		
			pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(pData[3]&0x0f);	
			break;
		default:
			return FALSE;
	}

	lenChk = LenCheckSum((iLenghId -13));//Because init value 13
	pImeuInfo->pUartXTXBuff[9] = Hex2Ascii((lenChk & 0x0f));
	pImeuInfo->pUartXTXBuff[10] = Hex2Ascii(((iLenghId -13)>>8) & 0x0f);
	pImeuInfo->pUartXTXBuff[11] = Hex2Ascii(((iLenghId -13) & 0xf0)>>4);
	pImeuInfo->pUartXTXBuff[12] = Hex2Ascii((iLenghId -13) & 0x0f);
	iCHECKSUM = CalcSendCheckSum(pImeuInfo->pUartXTXBuff, iLenghId);
	CheckSumH = 0xff&(iCHECKSUM>>8);
	CheckSumL = 0xff&(iCHECKSUM);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumH>>4);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumH&0x0f);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumL>>4);
	pImeuInfo->pUartXTXBuff[iLenghId++] = Hex2Ascii(CheckSumL&0x0f);
	pImeuInfo->pUartXTXBuff[iLenghId++] = YDN23_EOI_VAL;
	u8SendLengh = iLenghId;
	memset(pImeuInfo->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMEU - 1));
	pImeuInfo->UartXSendData(pImeuInfo,u8SendLengh);
	//gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//RS485_UART5_SendDlay(5);
	//UINT8 uUAR5_Idx = 0;
	//UINT16 SOIPst = 0;
	//1S超时！
	/*for (uUAR5_Idx = 0; uUAR5_Idx < 10; uUAR5_Idx++)
	{
		if (u8UartXRcvIsOK_IMEU2(pImeuInfo, &SOIPst))
		{
			gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
			if (u8CheckYDN23IsValid_IMEU(pImeuInfo, SOIPst))
			{
				p5Buf = pImeuInfo->pUartXRecvBuff + SOIPst;
				p5Buf = p5Buf + 13;
				u8CID2Check = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
				p5Buf+=2;
				if(0x45 == u8CID2Check)
				{
					gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
					return TRUE;
				}
			}
			break;
		}
		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
		OSTimeDly(50);
	}*/
	OSTimeDly(50);
	return FALSE;
}

void IMEU_Ctrl(IMEU_INFO *pImeuInfo)
{
	UINT8 u8CtrlDO = FALSE;
	UINT8 u8SendData[7];
	UINT32 u32Value = 0;
	memset(u8SendData, 0, 6);
	//UINT8 u8ChargingRst = 0;
	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO1 != pImeuInfo->DOStat_1.BIT_FIELD.DO1)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO2 != pImeuInfo->DOStat_1.BIT_FIELD.DO2)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO3 != pImeuInfo->DOStat_1.BIT_FIELD.DO3)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO4 != pImeuInfo->DOStat_1.BIT_FIELD.DO4)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO5 != pImeuInfo->DOStat_1.BIT_FIELD.DO5)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO6 != pImeuInfo->DOStat_1.BIT_FIELD.DO6)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO7 != pImeuInfo->DOStat_1.BIT_FIELD.DO7)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO8 != pImeuInfo->DOStat_1.BIT_FIELD.DO8)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO9 != pImeuInfo->DOStat_2.BIT_FIELD.DO9)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO10 != pImeuInfo->DOStat_2.BIT_FIELD.DO10)
	{
		u8CtrlDO = TRUE;
	}

	if(pImeuInfo->DO_CtrlValue.BIT_FIELD.DO11 != pImeuInfo->DOStat_2.BIT_FIELD.DO11)
	{
		u8CtrlDO = TRUE;
	}
	if(u8CtrlDO == TRUE)
	{
		u32Value = pImeuInfo->DO_CtrlValue.UIN32_FIELD;
		u8SendData[0] = (u32Value>>24)&0xff;
		u8SendData[1] = (u32Value>>16)&0xff;
                u8SendData[2] = (u32Value>>8)&0xff;
		u8SendData[3] = (u32Value)&0xff;
		IMEU_0X45_CMD(pImeuInfo,YDN23_IMEU_CMDTYPE_45_CTRL_DO, u8SendData);
		OSTimeDly(50);
	}

	if(pImeuInfo->u8PWMCtrlSpeed[0] != pImeuInfo->u8_ch_1_Speed)
	{
		if(pImeuInfo->u8PWMCtrlSpeed[0] > pImeuInfo->u8_ch_1_Speed)
		{
			u32Value = pImeuInfo->u8PWMCtrlSpeed[0] - pImeuInfo->u8_ch_1_Speed;
		}
		else
		{
			u32Value = pImeuInfo->u8_ch_1_Speed - pImeuInfo->u8PWMCtrlSpeed[0];
		}

		if(u32Value > 1)
		{
			u32Value = pImeuInfo->u8PWMCtrlSpeed[0];
			u8SendData[0] = (u32Value>>24)&0xff;
			u8SendData[1] = (u32Value>>16)&0xff;
					u8SendData[2] = (u32Value>>8)&0xff;
			u8SendData[3] = (u32Value)&0xff;
			IMEU_0X45_CMD(pImeuInfo,YDN23_IMEU_CMDTYPE_CTRL_45_ADD_OIL, u8SendData);
			OSTimeDly(50);
		}
	}

	if(pImeuInfo->u8PWMCtrlSpeed[1] != pImeuInfo->u8_ch_2_Speed)
	{
		if(pImeuInfo->u8PWMCtrlSpeed[1] > pImeuInfo->u8_ch_2_Speed)
		{
			u32Value = pImeuInfo->u8PWMCtrlSpeed[1] - pImeuInfo->u8_ch_2_Speed;
		}
		else
		{
			u32Value = pImeuInfo->u8_ch_2_Speed - pImeuInfo->u8PWMCtrlSpeed[1];
		}

		if(u32Value > 1)
		{
			u32Value = pImeuInfo->u8PWMCtrlSpeed[1];
			u8SendData[0] = (u32Value>>24)&0xff;
			u8SendData[1] = (u32Value>>16)&0xff;
					u8SendData[2] = (u32Value>>8)&0xff;
			u8SendData[3] = (u32Value)&0xff;
			IMEU_0X45_CMD(pImeuInfo,YDN23_IMEU_CMDTYPE_CTRL_45_OIL_CONTINUE, u8SendData);
			OSTimeDly(50);
		}
	}

	//if(pImeuInfo->u8AddOilWater_0X74_SendIMEUFlag == TRUE)
	//{
	//	pImeuInfo->u8AddOilWater_0X74_SendIMEUFlag = FALSE;
	//	if(pImeuInfo->u8AddOilWater_0X74_Flag == FALSE)
	//	{
	//			u8SendData[0] = 0;//0 油泵停止
	//	}
	//	else
	//	{
	//			u8SendData[0] = 1;//1 油泵启动
	//	}
	//	IMEU_0X45_CMD(pImeuInfo, YDN23_IMEU_CMDTYPE_CTRL_45_OIL_CONTINUE, u8SendData);
	//	gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//	OSTimeDly(300);
	//	IMEU_0X45_CMD(pImeuInfo, YDN23_IMEU_CMDTYPE_CTRL_45_OIL_CONTINUE, u8SendData);
	//}

	//if(pImeuInfo->u8AddOil_0X73_Flag == TRUE)
	//{
	//	pImeuInfo->u8AddOil_0X73_Flag = FALSE;
	//	OSTimeDly(50);
	//	IMEU_0X45_CMD(pImeuInfo, YDN23_IMEU_CMDTYPE_CTRL_45_ADD_OIL, u8SendData);
	//	gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//}


	//u8ChargingRst = CheckChargingState(pImeuInfo);//0不发送		 1发送充电		2发送停止
	//if(u8ChargingRst)//0不发送		 1发送充电		2发送停止
	//{
	//	if(u8ChargingRst == 1)
	//	{
	//		u8SendData[0] = 1;//0x01-开始充电	0x00-停止充电

	//	}
	//	else if(u8ChargingRst == 2)
	//	{
	//		u8SendData[0] = 0;//0x01-开始充电	0x00-停止充电
	//	}
	//	gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//	u8Rst =IMEU_0X45_CMD(pImeuInfo, YDN23_IMEU_CMDTYPE_CTRL_45_START_STOP, u8SendData);
	//	gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//	if(u8Rst == FALSE)
	//	{
	//		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//		IMEU_0X45_CMD(pImeuInfo, YDN23_IMEU_CMDTYPE_CTRL_45_START_STOP, u8SendData);
	//		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//		IMEU_0X45_CMD(pImeuInfo, YDN23_IMEU_CMDTYPE_CTRL_45_START_STOP, u8SendData);
	//		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	//	}
	//}
}


void IMEU_Sampler(IMEU_INFO *pImeuInfo)
{
	gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
	OSTimeDly(5);	
	IMEU_SampleCMD41_1(pImeuInfo);
	OSTimeDly(50);
	//IMEU_Unify_Args();
	IMEU_Ctrl(pImeuInfo);

	if(pImeuInfo->u8GotVersionFlag == 0)
	{
		OSTimeDly(50);
		IMEU_SampleCMD51_1(pImeuInfo);
	}

	OSTimeDly(50);
}


IMEU_INFO gIMEU_Info[2];

void InitIMEU_Info_uart4(IMEU_INFO *pImeu, UINT8 u8Addr)
{
	pImeu->u8DevAddr = YDN23_IMEU_BASE_ADDR;
	pImeu->DIStat_1.BYTE_FIELD = 0;
	pImeu->DIStat_2.BYTE_FIELD = 0;
	pImeu->DOStat_1.BYTE_FIELD = 0;
	pImeu->DOStat_2.BYTE_FIELD = 0;
	pImeu->DO_CtrlValue.UIN32_FIELD = 0;
	pImeu->u8CommCount = 0;
	pImeu->u8CommError = TRUE;
	pImeu->u8_SW_Version[0] = 0;
	pImeu->u8_SW_Version[1] = 0;
	pImeu->fSwVersion = 0;
	pImeu->fHwVersion = 0;
	pImeu->u8GotVersionFlag = 0;

	pImeu->u8UartNo = UART_NO_UART4;
	pImeu->pUartXTXBuff = gUart4TXBuff;
	pImeu->pUartXRecvBuff = gUart4RecvBuff;

	pImeu->UartXSendData = UartXSendData_Deal_IMEU;
	pImeu->FeedDogTaskID = TASK_ID_UART4;
	//pImeu->UartXRcvIsOK = u8Uart4RcvIsOK_1;
	//pImeu->u8CheckYDN23IsValid = u8CheckYDN23IsValid_1;


	pImeu->u8PWMCtrlSpeed[0] = 0;
	pImeu->u8PWMCtrlSpeed[1] = 0;
}



extern NOINIT UINT8 gUart5TXBuff[RS485_UART5_TX_MAX_NUM];
void InitIMEU_Info_uart5(IMEU_INFO *pImeu, UINT8 u8Addr)
{
	pImeu->u8DevAddr = YDN23_IMEU_BASE_ADDR+u8Addr;
	pImeu->DIStat_1.BYTE_FIELD = 0;
	pImeu->DIStat_2.BYTE_FIELD = 0;
	pImeu->DOStat_1.BYTE_FIELD = 0;
	pImeu->DOStat_2.BYTE_FIELD = 0;
	pImeu->DO_CtrlValue.UIN32_FIELD = 0;
	pImeu->u8CommCount = 0;
	pImeu->u8CommError = FALSE;
	pImeu->u8_SW_Version[0] = 0;
	pImeu->u8_SW_Version[1] = 0;
	pImeu->fSwVersion = 0;
	pImeu->fHwVersion = 0;
	pImeu->u8GotVersionFlag = 0;

	pImeu->u8UartNo = UART_NO_UART5;
	pImeu->pUartXTXBuff = gUart5TXBuff;
	pImeu->pUartXRecvBuff = gUart5RecvBuff;

	pImeu->UartXSendData = UartXSendData_Deal_IMEU;
	pImeu->FeedDogTaskID = TASK_ID_UART5;

	//pImeu->UartXRcvIsOK = u8Uart5RcvIsOK_2;
	//pImeu->u8CheckYDN23IsValid = u8CheckYDN23IsValid_2;

	pImeu->u8PWMCtrlSpeed[0] = 0;
	pImeu->u8PWMCtrlSpeed[1] = 0;
}

UINT8 u8UartXRcvIsOK_IMEU2Upgrade(Secondary_Upgrade_info_t *me, IMEU_INFO *pImeuInfo, UINT16* pVal)
{
	UINT16 iStartPst = 0;
	UINT8 u8RevSOI = FALSE;
	if(pImeuInfo->u8UartNo == UART_NO_UART4)
	{
		pImeuInfo->u16RcvLenth = dv_Uart4.pData;
	}
	else
	{
		pImeuInfo->u16RcvLenth = dv_Uart5.pData;
	}

	if (pImeuInfo->u16RcvLenth < 13)
	{
		return FALSE;
	}

	for (iStartPst = 0; iStartPst < pImeuInfo->u16RcvLenth && iStartPst < RS485_RX_MAX_NUM_IMEU; iStartPst++)
	{
		if (me->CurrDecviceAddr == pImeuInfo->pUartXRecvBuff[iStartPst])
		{
			*pVal = iStartPst;
			u8RevSOI = TRUE;
			break;
		}
	}
	if (FALSE == u8RevSOI)
	{
		return FALSE;
	}

	if (TRUE == u8RevSOI)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT8 u8CheckUpgradeIsValid_IMEU(Secondary_Upgrade_info_t *me,IMEU_INFO *pImeuInfo, UINT16 SOIPst)
{
	UINT8 Cmd = 0;
	UINT8 ADDR = 0;
	UINT8 u8Len = 0;
	UINT16 IICRC = 0;
	UINT16 RevCRC = 0;
	UINT16 pCheckSumPst = 0;
	UINT8*  p5Buf = pImeuInfo->pUartXRecvBuff;

	SOIPst = MIN((RS485_RX_MAX_NUM_IMEU - (YDN23_CID2_PST + 1)), SOIPst);	//防止溢出！
	p5Buf = pImeuInfo->pUartXRecvBuff + SOIPst;
	ADDR = p5Buf[0];
	Cmd = p5Buf[1];
	u8Len = p5Buf[2];

	if ((me->CurrDecviceAddr) != ADDR)
	{
		return FALSE;
	}

	if (Cmd != 0x03)
	{
		return FALSE;
	}

	if (u8Len != (pImeuInfo->u16RcvLenth - SOIPst - 5))
	{
		return FALSE;
	}

	IICRC = CalcCodeCRCFor485(p5Buf, pImeuInfo->u16RcvLenth - SOIPst - 2);
	pCheckSumPst = pImeuInfo->u16RcvLenth - SOIPst - 2;
	pCheckSumPst = MIN((RS485_RX_MAX_NUM_IMEU - 2), pCheckSumPst);//防止溢出！
	RevCRC = (UINT16)((p5Buf[pCheckSumPst] << 8) + p5Buf[pCheckSumPst + 1]);
	if (IICRC != RevCRC)
	{
		return FALSE;
	}
	return TRUE;
}

void IMEU2RcvUpgradeDataDeal(Secondary_Upgrade_info_t *me,IMEU_INFO *pImeuInfo);
void IMEU2RcvUpgradeDataDeal(Secondary_Upgrade_info_t *me,IMEU_INFO *pImeuInfo)
{
	UINT8 uUAR5_Idx = 0;
	UINT16 SOIPst = 0;
	
	for (uUAR5_Idx = 0; uUAR5_Idx < 10; uUAR5_Idx++)
	{
		gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
		if (u8UartXRcvIsOK_IMEU2Upgrade(me, pImeuInfo, &SOIPst))
		{
			gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
			if (u8CheckUpgradeIsValid_IMEU(me, pImeuInfo, SOIPst))
			{
				gSelfTest.fSetTaskID(pImeuInfo->FeedDogTaskID);
				SecondaryUnpackData(me, pImeuInfo->pUartXRecvBuff + SOIPst, pImeuInfo->u16RcvLenth - SOIPst);
			}
			break;
		}
		OSTimeDly(5);
	}
}