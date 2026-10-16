#include "main.h"


static UINT8 PackYDN23XXXCmd_IMIU(UINT8 CID2,UINT8 CMDType,IMIU_COMM_MNG *pImiu);
static void UnPackCMD_46Data_1(IMIU_COMM_MNG *pImiu, UINT16 SOIPst);
static void UnPackCMD_41Data_1(IMIU_COMM_MNG *pImiu, UINT16 SOIPst);
static UINT8 u8CheckYDN23IsValid_1(UINT16 SOIPst,UINT8 uAddr);
//static UINT8 u8Uart4RcvIsOK_1(UINT16* pVal);
static void IMIUSampleCMD41_1(IMIU_COMM_MNG *pImiu);
static void IMIUSampleCMD46_1(IMIU_COMM_MNG *pImiu);
static void IMIUUnifyArgs_1(IMIU_COMM_MNG *pImiu);

#define	RS485_TX_MAX_NUM_IMIU	MIN(RS485_UART4_TX_MAX_NUM, RS485_UART5_TX_MAX_NUM)
#define	RS485_RX_MAX_NUM_IMIU	MIN(RS485_UART4_RX_MAX_NUM, RS485_UART5_RX_MAX_NUM)

UINT8 u8UartXRcvIsOK_1(UINT8 u8UartNo, UINT16* pVal)
{
	UINT16 iStartPst = 0;
	UINT8 u8RevSOI = FALSE;
	UINT8 u8RevEOI = FALSE;
	UINT16 u16RcvLenth;
	UINT8 * pUartXRecvBuff;

	if(u8UartNo == UART_NO_UART4)
	{
		u16RcvLenth = dv_Uart4.pData;
		pUartXRecvBuff = gUart4RecvBuff;
	}
	else
	{
		u16RcvLenth = dv_Uart5.pData;
		pUartXRecvBuff = gUart5RecvBuff;
	}

	if (u16RcvLenth < 13)
	{
		return FALSE;
	}

	for (iStartPst = 0; iStartPst < u16RcvLenth && iStartPst < RS485_RX_MAX_NUM_IMIU; iStartPst++)
	{
		if (YDN23_START == pUartXRecvBuff[iStartPst])
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

	for (; iStartPst < u16RcvLenth && iStartPst < RS485_RX_MAX_NUM_IMIU; iStartPst++)
	{
		if (YDN23_END == pUartXRecvBuff[iStartPst])
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

UINT8 u8CheckYDN23IsValid_X(UINT8 u8UartNo, UINT8 u8DevAddr,UINT8 u8CID1, UINT16 SOIPst)
{
	//UINT8* p5Buf = 0;
	//UINT16* p5Buf = 0;
	UINT8 CID1 = 0;
	UINT8 CID2 = 0;
	UINT8 ADDR = 0;
	UINT16 IICRC = 0;
	UINT16 i;
	UINT16 RevCRC = 0;
	UINT16 pCheckSumPst = 0;
	UINT16 u16RcvLenth;
	UINT8 * p5Buf;

	SOIPst = MIN((RS485_RX_MAX_NUM_IMIU - (YDN23_CID2_PST + 1)), SOIPst);	//防止溢出！

	if(u8UartNo == UART_NO_UART4)
	{
		u16RcvLenth = dv_Uart4.pData;
		p5Buf = gUart4RecvBuff+ SOIPst;
	}
	else
	{
		u16RcvLenth = dv_Uart5.pData;
		p5Buf = gUart5RecvBuff+ SOIPst;
	}

	
	ADDR = TwoAscii2Hex(p5Buf[YDN23_ADDR_PST], p5Buf[YDN23_ADDR_PST+1]);
	CID1 = (UINT8)TwoAscii2Hex(p5Buf[YDN23_CID1_PST], p5Buf[YDN23_CID1_PST + 1]);
	CID2 = (UINT8)TwoAscii2Hex(p5Buf[YDN23_CID2_PST], p5Buf[YDN23_CID2_PST + 1]);
	if ((u8DevAddr) != ADDR)
	{
		return FALSE;
	}

	if (CID1 != u8CID1)
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
	for (i = 0; i < (u16RcvLenth - 6) && (i < RS485_RX_MAX_NUM_IMIU); i++)
	{
		IICRC += p5Buf[YDN23_VER_PST + i];
	}
	IICRC = IICRC%65536;
	IICRC = ((~IICRC)+1);

	pCheckSumPst = u16RcvLenth - 5;
	pCheckSumPst = MIN((RS485_RX_MAX_NUM_IMIU - 5), pCheckSumPst);//防止溢出！
	RevCRC = (UINT16)(TwoAscii2Hex(p5Buf[pCheckSumPst], p5Buf[pCheckSumPst + 1]) << 8)
			+ TwoAscii2Hex(p5Buf[pCheckSumPst + 2], p5Buf[pCheckSumPst + 2 + 1]);
	if (IICRC != RevCRC)
	{
		return FALSE;
	}
	return TRUE;
}
void UartXSendData_Deal(UINT8 u8UartNo, UCHAR *pUartXTXBuff, UINT16 data2)
{
	if(u8UartNo == UART_NO_UART4)
	{
		dv_Uart4.fSendDataTXY(pUartXTXBuff, data2);
		//gSelfTest.fSetTaskID(FeedDogTaskID);
		//RS485_UART4_SendDlay(5);
	}
	else
	{
		dv_Uart5.fSendDataTXY(pUartXTXBuff, data2);
		//gSelfTest.fSetTaskID(FeedDogTaskID);
		//RS485_UART5_SendDlay(5);
	}	
}


/**
 * @brief 解析IMIU通信命令41数据包的第一部分
 * @details 该函数处理从UART4或UART5接收到的IMIU数据,主要解析:
 *          - 绝缘板分流器电流
 *          - 绝缘板母排电压
 *          - 绝缘板正/负对PE电压
 *          - 绝缘板正/负对PE电阻
 *          - 绝缘板告警状态
 *          - 绝缘动作状态
 * 
 * @param pImiu IMIU通信管理结构体指针
 * @param SOIPst 数据包起始位置偏移量
 * 
 * @note 函数会跳过前13字节的数据头和前4组8字节的数据(板载温度、外扩温度、霍尔电压1/2)
 * @note 告警状态会有20个采集周期的延迟清除机制
 */
static void UnPackCMD_41Data_1(IMIU_COMM_MNG *pImiu,UINT16 SOIPst)
{
	UINT8* p5Buf;
	stIMIU_Info *Ptr;
        UINT16 u16RcvLenth;

	if(pImiu->u8UartNo == UART_NO_UART4)
	{
		u16RcvLenth = dv_Uart4.pData;
		p5Buf = gUart4RecvBuff+ SOIPst;
	}
	else
	{
		u16RcvLenth = dv_Uart5.pData;
		p5Buf = gUart5RecvBuff+ SOIPst;
	}
	Ptr = pImiu->pImiuInfo;

	static UINT8 su8Delay = 30;
	float fVal = 0;
	UINT8 u8TempBuf[6];


	if (RS485_RX_MAX_NUM_IMIU - SOIPst <= 100)					//协议发生变化 需要更改
	{
		return;													//后边数组没空间进行解析了！
	}
	p5Buf = p5Buf + 13;
	/*********************************************************************************
		Jump
		1	板载温度
		2	外扩温度
		3	霍尔电压1
		4	霍尔电压2
	***********************************************************************************/
	p5Buf+=8;
	p5Buf+=8;
	p5Buf+=8;
	p5Buf+=8;
	//5	绝缘板分流器电流
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf+=8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fJYMCurr[0] = ABS(fVal);

	//6 绝缘板母排电压
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);//1.047*fVal -3.04;	1.044*fVal -4.41;
	Ptr->fJYMVolt[0] = fVal;//1.044*fVal -4.41;

	//7 绝缘板正对PE电压
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fJYMP2gVolt[0] = fVal;

	//8	绝缘板负对PE电压
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fJYMN2gVolt[0] = fVal;

	//9	绝缘板正对PE电阻
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fJYMP2gResist[0] = fVal;

	//10	绝缘板负对PE电阻
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fJYMN2gResist[0] = fVal;

	//11	绝缘板告警
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	p5Buf += 2;
	if (0 == Ptr->u8JYMAlmStatMask)
	{
		//要把绝缘监测告警消失 延时一下，好让客户看到
		//逻辑为如果 告警则立即 如果上传不告警可以延迟一会，让客户能看到。
		if ((INSLT_CHK_Mode_IMIU_Rpt_Volt == Ptr->u8InsltCheckByVolt))
		{
			Ptr->u8JYMAlmStat[0] = 0;		//不用这个告警，电阻自己判断。
		}
		else
		{
			if (TRUE == u8TempBuf[0])
			{
				Ptr->u8JYMAlmStat[0] = u8TempBuf[0];
				su8Delay = 0;
			}
			else
			{
				if (su8Delay < 20)   //让告警 持续20个采集周期！
				{
					su8Delay++;
				}
				else
				{
					Ptr->u8JYMAlmStat[0] = u8TempBuf[0];
				}
			}
		}
	}
	else
	{
		Ptr->u8JYMAlmStat[0] = 0;
	}

	//12	绝缘动作状态	0x02待机中，0x01检测中，0x00脱离
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	p5Buf += 2;
	Ptr->u8JYMAction[0] = u8TempBuf[0];

	p5Buf += 4;
    //17 接地告警状态
    u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
    p5Buf += 2;
    Ptr->u8RunEarth_DetectAlm[0] = (u8TempBuf[0] >> 4) & 0x01;
}

static void UnPackCMD_46Data_1(IMIU_COMM_MNG *pImiu, UINT16 SOIPst)
{
	UINT8* p5Buf;
	stIMIU_Info *Ptr;
        UINT16 u16RcvLenth;

	if(pImiu->u8UartNo == UART_NO_UART4)
	{
		u16RcvLenth = dv_Uart4.pData;
		p5Buf = gUart4RecvBuff+ SOIPst;
	}
	else
	{
		u16RcvLenth = dv_Uart5.pData;
		p5Buf = gUart5RecvBuff+ SOIPst;
	}
	Ptr = pImiu->pImiuInfo;

	float fVal = 0;
	UINT8 u8TempBuf[6];


	if (RS485_RX_MAX_NUM_IMIU - SOIPst <= 110)	//协议发生变化 需要更改
	{
		return;//后边数组没空间进行解析了！
	}

	p5Buf = p5Buf + 13;
	//1		霍尔1校准系数a
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);

	Ptr->fRunJYMAlm_ResistPerV[0] = fVal;

	//2		霍尔1校准系数b
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	//gEvChargeInfo.fBattVolt_1CoefB = fVal;

	//3		霍尔2校准系数a
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	//gEvChargeInfo.fBattVolt_2CoefA = fVal;

	//4		霍尔2校准系数b
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	//gEvChargeInfo.fBattVolt_2CoefB = fVal;

	//5	绝缘板分流器系数
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fRunJYMShunt_Coef[0] = fVal;

	//6	绝缘板分流器电流校准系数a
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fRunJYMCurr_CoefA[0] = fVal;

	//7	绝缘板分流器电流校准系数b
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fRunJYMCurr_CoefB[0] = fVal;

	//8	绝缘板母排电压采样校准系数a
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fRunJYMVolt_CoefA[0] = fVal;

	//9	绝缘板母排电压采样校准系数b
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fRunJYMVolt_CoefB[0] = fVal;

	//10	绝缘告警绝缘电阻值
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	u8TempBuf[1] = TwoAscii2Hex(*(p5Buf+2), *(p5Buf+3));
	u8TempBuf[2] = TwoAscii2Hex(*(p5Buf+4), *(p5Buf+5));
	u8TempBuf[3] = TwoAscii2Hex(*(p5Buf+6), *(p5Buf+7));
	p5Buf += 8;
	FourByte2float(u8TempBuf, &fVal);
	Ptr->fRunJYMAlm_Resist[0] = fVal;

	//11	绝缘检测控制状态
	u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
	p5Buf += 2;
	Ptr->u8RunJYMCtrlStat[0] = u8TempBuf[0];


	//12        接地检测使能
    u8TempBuf[0] = TwoAscii2Hex(*p5Buf, *(p5Buf+1));
    p5Buf += 2;
    Ptr->u8RunEarth_DetectEnable[0] = u8TempBuf[0];
    //gEvChargeInfo.u8UNUSED_2 = u8TempBuf[0];

}

static UINT8 PackYDN23XXXCmd_IMIU(UINT8 CID2,UINT8 CMDType, IMIU_COMM_MNG *pImiu)
{
	stIMIU_Info *Ptr = pImiu->pImiuInfo;
	UINT8 uAddr = pImiu->u8DevAddr;
	UINT8 *pTxBuf = pImiu->pUartXTXBuff;	

	float fVal1 = 0;
	float fVal2 = 0;
	UINT8 u8Val = 0;
	UINT8 lenChk = 0;
	UINT16 iLenghId = 0;
	UINT8 CheckSumH = 0;
	UINT8 CheckSumL = 0;
	UINT16 iCHECKSUM = 0;
	UINT8 u8SendLengh = 0;
	UINT8 pFloatArarry[12];
	//__XXX_UNION_VALUE unTempVAL;
	memset(pTxBuf, 0, RS485_TX_MAX_NUM_IMIU - 1);
	pTxBuf[0]= YDN23_SOI_VAL;
	pTxBuf[1] = Hex2Ascii(YDN23_VER_VAL>>4);
	pTxBuf[2] = Hex2Ascii(YDN23_VER_VAL&0x0f);
	pTxBuf[3] = Hex2Ascii((uAddr)>>4);
	pTxBuf[4] = Hex2Ascii((uAddr)&0x0f);
	pTxBuf[5] = Hex2Ascii(YDN23_IMIU_CID1>>4);
	pTxBuf[6] = Hex2Ascii(YDN23_IMIU_CID1&0x0f);
	pTxBuf[7] = Hex2Ascii(CID2>>4);
	pTxBuf[8] = Hex2Ascii(CID2&0x0f);

	if ((0x41 == CID2)
		||(0x46 == CID2))
	{
		iLenghId = 0;
		lenChk = LenCheckSum(iLenghId);						//Because init value 13
		pTxBuf[9] = Hex2Ascii((lenChk & 0x0f));
		pTxBuf[10] = Hex2Ascii((iLenghId >> 8) & 0x0f);
		pTxBuf[11] = Hex2Ascii(((iLenghId) & 0xf0)>>4);
		pTxBuf[12] = Hex2Ascii((iLenghId) & 0x0f);

		iCHECKSUM = CalcSendCheckSum(pTxBuf, 12);
		CheckSumH = 0xff&(iCHECKSUM>>8);
		CheckSumL = 0xff&(iCHECKSUM);
		pTxBuf[13] = Hex2Ascii(CheckSumH>>4);
		pTxBuf[14] = Hex2Ascii(CheckSumH&0x0f);
		pTxBuf[15] = Hex2Ascii(CheckSumL>>4);
		pTxBuf[16] = Hex2Ascii(CheckSumL&0x0f);
		pTxBuf[17] = YDN23_EOI_VAL;
		u8SendLengh = 18;
		return u8SendLengh;
	}
	else
	{
		if (0x48 == CID2)
		{
			switch (CMDType)
			{
			case 0x80://1	80H		霍尔1校准系数a		霍尔1校准系数b
				//fVal1 = Ptr->fBattVolt_1CoefA;
				//fVal2 = Ptr->fBattVolt_1CoefB;
				break;
			case 0x81://2	81H		霍尔2校准系数a		霍尔2校准系数b
				//fVal1 = Ptr->fBattVolt_2CoefA;
				//fVal2 = Ptr->fBattVolt_2CoefB;
				break;
			case 0x82://3	82H	绝缘板分流器系数	0
				//fVal1 = gIMIU_Info_UART4[0].fSetJYMShunt_Coef[0];
				fVal2 = 0;
				break;
			case 0x83://4	83H	绝缘板电流采样校准系数a	绝缘板电流采样校准系数b
				//fVal1 = gIMIU_Info_UART4[0].fSetJYMCurr_CoefA[0];
				//fVal2 = gIMIU_Info_UART4[0].fSetJYMCurr_CoefB[0];
				break;
			case 0x84://5	84H	绝缘板母排电压采样校准系数a	绝缘板母排电压采样校准系数b
				//fVal1 = gIMIU_Info_UART4[0].fSetJYMVolt_CoefA[0];
				//fVal2 = gIMIU_Info_UART4[0].fSetJYMVolt_CoefB[0];
				break;
			case 0x85://6	85H	绝缘告警绝缘电阻值	0
				//7E 32 30 42 34 36 30 34 38 44 30 31 32 38 35 30 30 30 30 42 38 34 31 30 30 30 30 30 30 30 30 46 39 46 33 0D
				fVal1 = Ptr->fSetJYMAlm_Resist[0];
				fVal2 = 0;
				break;
			case 0x8C://6	85H	绝缘告警绝缘电阻值	0
				//7E 32 30 42 34 36 30 34 38 44 30 31 32 38 35 30 30 30 30 42 38 34 31 30 30 30 30 30 30 30 30 46 39 46 33 0D
				fVal1 = Ptr->fSetJYMAlm_Resist_PerV[0];
				fVal2 = 500;
				break;
			case 0x86://7	86H	绝缘检测控制 (3：手动 2：后台 1：自动，0：禁止)	0
				fVal1 = Ptr->u8SetJYMCtrlStat[0];
				fVal2 = Ptr->u8SetGroundCtrlStat[0];
				break;
			case 0x87://8	87H	电池反接是否告警( 1：使能，0禁止)
				fVal2 = 0;
				break;
			case 0x94:
				fVal1 = 0;
				fVal2 = 0;
				break;
			default:
				break;
			}

			iLenghId = 13;
			pTxBuf[iLenghId++] = Hex2Ascii(CMDType>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(CMDType&0x0f);
			floattoascii(fVal1, pFloatArarry);
			if (CMDType == 0x86)
			{
				pTxBuf[iLenghId++] = Hex2Ascii(Ptr->u8SetJYMCtrlStat[0]>>4);
				pTxBuf[iLenghId++] = Hex2Ascii(Ptr->u8SetJYMCtrlStat[0]&0x0f);
			}
			else if (CMDType == 0x94)
			{
				pTxBuf[iLenghId++] = Hex2Ascii(1>>4);
				pTxBuf[iLenghId++] = Hex2Ascii(1&0x0f);
			}
			else
			{
				pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[0]>>4);
				pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[0]&0x0f);
			}
			
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[1]>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[1]&0x0f);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[2]>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[2]&0x0f);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[3]>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[3]&0x0f);
			floattoascii(fVal2, pFloatArarry);
			if (CMDType == 0x94)
			{
				if (Ptr->u8CTRLInterDOCMD_NO == 0)
				{
					pTxBuf[iLenghId++] = Hex2Ascii(Ptr->u8CTRLInterDOCMD[0]>>4);
					pTxBuf[iLenghId++] = Hex2Ascii(Ptr->u8CTRLInterDOCMD[0]&0x0f);		
				}
				else
				{
					pTxBuf[iLenghId++] = Hex2Ascii(Ptr->u8CTRLInterDOCMD[1]>>4);
					pTxBuf[iLenghId++] = Hex2Ascii(Ptr->u8CTRLInterDOCMD[1]&0x0f);		
				}						
			}
			else if (CMDType == 0x86)     //接地告警
                {
                    pTxBuf[iLenghId++] = Hex2Ascii(Ptr->u8SetGroundCtrlStat[0]>>4);
                    pTxBuf[iLenghId++] = Hex2Ascii(Ptr->u8SetGroundCtrlStat[0]&0x0f);
                    // pTxBuf[iLenghId++] = Hex2Ascii(0x01>>4);
                    // pTxBuf[iLenghId++] = Hex2Ascii(0x01&0x0f);//Ptr->u8SetJYMCtrlStat[0]&0x0f);
                }
			else
			{
				pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[0]>>4);
				pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[0]&0x0f);
			}			
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[1]>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[1]&0x0f);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[2]>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[2]&0x0f);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[3]>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(pFloatArarry[3]&0x0f);
			lenChk = LenCheckSum((iLenghId -13));//Because init value 13
			pTxBuf[9] = Hex2Ascii((lenChk & 0x0f));
			pTxBuf[10] = Hex2Ascii(((iLenghId -13)>>8) & 0x0f);
			pTxBuf[11] = Hex2Ascii(((iLenghId -13) & 0xf0)>>4);
			pTxBuf[12] = Hex2Ascii((iLenghId -13) & 0x0f);
			iCHECKSUM = CalcSendCheckSum(pTxBuf, iLenghId);
			CheckSumH = 0xff&(iCHECKSUM>>8);
			CheckSumL = 0xff&(iCHECKSUM);
			pTxBuf[iLenghId++] = Hex2Ascii(CheckSumH>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(CheckSumH&0x0f);
			pTxBuf[iLenghId++] = Hex2Ascii(CheckSumL>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(CheckSumL&0x0f);
			pTxBuf[iLenghId++] = YDN23_EOI_VAL;
			u8SendLengh = iLenghId;
			return u8SendLengh;
		}

		if (0x45 == CID2)
		{
			switch (CMDType)
			{
			case 0x80://1	80H	后台控制绝缘检测命令
				u8Val = Ptr->u8CTRLInsulationCMD[0];
				break;
			case 0x81:/************************************************************************
					2	81H	板内DO状态控制动作
					（低4位表示DO号，高4位表示动作）	DO号：0x00~0x0f
					动作：0x10：动作  0x00：正常
				************************************************************************/				
				if (Ptr->u8CTRLInterDOCMD_NO == 0)
				{
					u8Val = Ptr->u8CTRLInterDOCMD[0];
				}
				else
				{
					u8Val = Ptr->u8CTRLInterDOCMD[1];	
				}			
				break;
			case 0x82:/***********************************************************************
					3	82H	外接DO状态控制
					（低4位表示DO号，高4位表示动作）	DO号：0x00~0x0f
					动作：0x10：动作  0x00：正常
				  ************************************************************************/
				//u8Val = gEvChargeInfo.u8CTRLEx1DOCMD;
				break;
			case 0x83:/*************************************************************************
					4	83H	外扩DO状态控制
					（低4位表示DO号，高4位表示动作）	DO号：0x00~0x0f
					动作：0x10：动作  0x00：正常
				*************************************************************************/
				//u8Val = gEvChargeInfo.u8CTRLEx2DOCMD;
				break;
			default:
				break;
			}
			iLenghId = 13;
			pTxBuf[iLenghId++] = Hex2Ascii(CMDType>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(CMDType&0x0f);
			pTxBuf[iLenghId++] = Hex2Ascii(u8Val>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(u8Val&0x0f);
			lenChk = LenCheckSum((iLenghId -13));//Because init value 13
			pTxBuf[9] = Hex2Ascii((lenChk & 0x0f));
			pTxBuf[10] = Hex2Ascii(((iLenghId -13)>>8) & 0x0f);
			pTxBuf[11] = Hex2Ascii(((iLenghId -13) & 0xf0)>>4);
			pTxBuf[12] = Hex2Ascii((iLenghId -13) & 0x0f);
			iCHECKSUM = CalcSendCheckSum(pTxBuf, iLenghId);
			CheckSumH = 0xff&(iCHECKSUM>>8);
			CheckSumL = 0xff&(iCHECKSUM);
			pTxBuf[iLenghId++] = Hex2Ascii(CheckSumH>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(CheckSumH&0x0f);
			pTxBuf[iLenghId++] = Hex2Ascii(CheckSumL>>4);
			pTxBuf[iLenghId++] = Hex2Ascii(CheckSumL&0x0f);
			pTxBuf[iLenghId++] = YDN23_EOI_VAL;
			u8SendLengh = iLenghId;
			return u8SendLengh;
		}
	}
	return 0;
}

static void IMIUSampleCMD41_1(IMIU_COMM_MNG *pImiu)
{	
	UINT8 Rst = FALSE;
	UINT16 SOIPst = 0;
	UINT8 uIdx = 0;
	//static UINT8 suCommFailCount[3] = {0};

	PackYDN23XXXCmd_IMIU(0x41, 0xff, pImiu);
	memset(pImiu->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMIU - 1));
	//dv_Uart4.fSendDataTXY(gUart4TXBuff,18);
	UartXSendData_Deal(pImiu->u8UartNo, pImiu->pUartXTXBuff, 18 );

	OSTimeDly(200/2);
	for (uIdx = 0; uIdx < 10; uIdx++)
	{
		if (u8UartXRcvIsOK_1(pImiu->u8UartNo, &SOIPst))
		{
			if (u8CheckYDN23IsValid_X(pImiu->u8UartNo, pImiu->u8DevAddr, YDN23_IMIU_CID1, SOIPst))
			{
				UnPackCMD_41Data_1(pImiu, SOIPst);
				Rst = TRUE;
				pImiu->u8CommCount= 0;
				pImiu->pImiuInfo->u8InsltCommAlm[0] = FALSE;
			}
			break;
		}
		OSTimeDly(10);
	}

	if (FALSE == Rst)
	{
		if (pImiu->u8CommCount < 5)
		{
			pImiu->u8CommCount++;
		}
		else
		{
			//BSP_InitGpioF();					//重新初始化	UART5的收发控制引脚
			//OSTimeDly(30);
			if (1 == pImiu->pImiuInfo->u8JYMAlmStatMask)	//屏蔽告警
			{
				pImiu->pImiuInfo->u8InsltCommAlm[0] = FALSE;
			}
			else
			{
				pImiu->pImiuInfo->u8InsltCommAlm[0] = TRUE;
			}
		}
	}
}

static void IMIUSampleCMD46_1(IMIU_COMM_MNG *pImiu)
{	
	UINT16 SOIPst = 0;
	UINT8 uIdx = 0;
	PackYDN23XXXCmd_IMIU(0x46, 0xff, pImiu);
	memset(pImiu->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMIU - 1));
	//dv_Uart4.fSendDataTXY(gUart4TXBuff,18);
	UartXSendData_Deal(pImiu->u8UartNo, pImiu->pUartXTXBuff, 18 );
	OSTimeDly(200/2);
	for (uIdx = 0; uIdx < 10; uIdx++)
	{
		if (u8UartXRcvIsOK_1(pImiu->u8UartNo, &SOIPst))
		{
			if (u8CheckYDN23IsValid_X(pImiu->u8UartNo, pImiu->u8DevAddr, YDN23_IMIU_CID1, SOIPst))
			{
				UnPackCMD_46Data_1(pImiu, SOIPst);
			}
			break;
		}
		OSTimeDly(10);
	}
}

static void IMIUUnifyArgs_1(IMIU_COMM_MNG *pImiu)
{
	stIMIU_Info *Ptr;
	UINT8 u8CTRLInterDOCMDFlag = 0;

	UINT8 uSendLengh = 0;

	Ptr = pImiu->pImiuInfo;


	//if ((ABS(Ptr->fSetJYMAlm_Resist[0] - Ptr->fRunJYMAlm_Resist[0]) > 0.02))
	if ((ABS(Ptr->fRunJYMAlm_ResistPerV[0] - Ptr->fSetJYMAlm_Resist_PerV[0]) > 0.02))	
	{
		//uSendLengh = PackYDN23XXXCmd_IMIU(0x48, 0x85, pImiu);							//6		85H	绝缘告警绝缘电阻值	0
		uSendLengh = PackYDN23XXXCmd_IMIU(0x48, 0x8C, pImiu);	
		memset(pImiu->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMIU - 1));
		UartXSendData_Deal(pImiu->u8UartNo, pImiu->pUartXTXBuff, uSendLengh );
		OSTimeDly(600/2);
	}
	
	
	// if ((ABS(Ptr->u8SetJYMCtrlStat[0] - Ptr->u8RunJYMCtrlStat[0]) > 0.02))
	// {
		uSendLengh = PackYDN23XXXCmd_IMIU(0x48, 0x86 ,pImiu);							//7	86H	绝缘检测控制 (3：手动 2：后台 1：自动，0：禁止) 0
		memset(pImiu->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMIU - 1));
		UartXSendData_Deal(pImiu->u8UartNo, pImiu->pUartXTXBuff, uSendLengh );
		OSTimeDly(600/2);
	// }

	if (TRUE == Ptr->u8CTRLInsulationCMDFlag[0])
	{
		MutexLock(gEvChargeInfo.hMutexWriteEVInfo);
		Ptr->u8CTRLInsulationCMDFlag[0] = FALSE;
		MutexUnlock(gEvChargeInfo.hMutexWriteEVInfo);

		uSendLengh = PackYDN23XXXCmd_IMIU(0x45, 0x80,pImiu);								//1 80H 后台控制绝缘检测命令	0x02待机，	0x01检测，	0x00脱离
		memset(pImiu->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMIU - 1));
		UartXSendData_Deal(pImiu->u8UartNo, pImiu->pUartXTXBuff, uSendLengh );
		OSTimeDly(600/2);
		uSendLengh = PackYDN23XXXCmd_IMIU(0x45, 0x80,pImiu);								//1 80H 后台控制绝缘检测命令	0x02待机，	0x01检测，	0x00脱离
		memset(pImiu->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMIU - 1));
		UartXSendData_Deal(pImiu->u8UartNo, pImiu->pUartXTXBuff, uSendLengh );
		OSTimeDly(600/2);
		uSendLengh = PackYDN23XXXCmd_IMIU(0x45, 0x80,pImiu);								//1 80H 后台控制绝缘检测命令	0x02待机，	0x01检测，	0x00脱离
		memset(pImiu->pUartXRecvBuff, 0, (RS485_RX_MAX_NUM_IMIU - 1));
		UartXSendData_Deal(pImiu->u8UartNo, pImiu->pUartXTXBuff, uSendLengh );
		OSTimeDly(600/2);
	}
}

void IMIUSample_1(IMIU_COMM_MNG *pImiu)
{
	if(pImiu->u8UartNo == UART_NO_UART4)
	{
		dv_Uart4.fInitUart(m9600, UARTX_PARITY_NONE);				//InitUart5
		dv_Uart4.fEnableRx();   //使能收中断	
	}
	else if(pImiu->u8UartNo == UART_NO_UART5)
	{
		dv_Uart5.fInitUart(m9600, UARTX_PARITY_NONE);				//InitUart5
		dv_Uart5.fEnableRx();									//使能收中断
	}
	else
	{
		return;
	}
	OSTimeDly(80/2);
	IMIUSampleCMD41_1(pImiu);
	IMIUSampleCMD46_1(pImiu);
	gSelfTest.fSetTaskID(pImiu->FeedDogTaskID);
	IMIUUnifyArgs_1(pImiu);                          //IMIU统一参数
}

//static void IMIU_CheckIfCommFail(UINT8 ch)
//{
//	dv_Uart4.fInitUart(m9600, UARTX_PARITY_NONE);				//InitUart5
//	dv_Uart4.fEnableRx();							//使能收中断
//	OSTimeDly(80/2);
//	IMIUSampleCMD41_1(ch);
//}