
#include "main.h"
#include "EtherNet.h"
#include "can_ring.h"
#include "HistoryEvent.h"
#include "secondary_upgrade.h"
#include "imeu2_upgrade.h"
 /***********************************************************************************************
	#define RECORD_BLOCK_MAX_NUM		3			3个扇区
	#define PAGE_NUM_PER_BLOCK			256			每个扇区 256页
	#define BYTES_NUM_PER_PAGE			256			每页 256字节
						______________
						|			  |
						|_____________|
	i16CurrPageIdx----->|			  |
						|_____________|
						|			  |
						|_____________|

	gFlashOptInfo.u32TotalRecordNum				总记录数
	gFlashOptInfo.u32CurrPageIdx				当前记录页的索引

	SaveRecord2Flash							这个信号由	API_CreateRecordLog 生产！
	gEvChargeInfo.u8SendCharge RecordFlag		只有收到2A 下发的交易号或是09远程停止充电指令才置位，
	所以离线卡充电无需处理。
	A/B 枪 存储再一个片区域内,然后记录的时候用一个字节标记A枪还是B枪的
************************************************************************************************/

#define	TABLE_STEP_BY_PAGE			(sizeof(gChargeLogTable) / BYTES_NUM_PER_PAGE)			/**/
#define	TABLE_BYTES_BY_END_PAGE		(sizeof(gChargeLogTable) % BYTES_NUM_PER_PAGE)		/**/
__FLASH_CTRL_INFO gFlashOptInfo = {0};
NOINIT __CHARGE_RECORD SaveRecord2Flash, stViewChargeRecordFlash;
NOINIT ChargeLogTable	gChargeLogTable;
NOINIT __CHARGE_RECORD SendRecord_1;
NOINIT __CHARGE_RECORD SendRecord_2;

NOINIT UINT8 DEBUG_Flash[300];
NOINIT UINT16 u16DEBUG_Flash_ADDR;
NOINIT UINT16 u16DEBUG_Flash_LEN;

UINT8  FlashWaitClean_U1_Data(void);
UINT8  FlashWaitClean_U2_Data(void);
void FlashWrite_U1_Data(UINT8* pData, UINT32 u32Addr, UINT32 len);
void FlashWrite_U2_Data(UINT8* pData, UINT32 u32Addr, UINT32 len);
void FlashWrite_IMEU2_Data(UINT8* pData, UINT32 u32Addr, UINT32 len);
void FlashWrite_IMEU2_DataLen(UINT8* pData, UINT32 u32Addr, UINT32 len);
void FlashClean_U1_Data(void);
void FlashClean_U2_Data(void);
void FlashClean_IMEU2_Data(void);
void FlashCleanRecords(void);

void FLASH_ClrUnRptFlag(UINT16 u16PageIdx)
{
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	UINT32 Addres = 0;
	UINT8 u8Num[6];
	if (u16PageIdx > RECORD_BLOCK_MAX_NUM * PAGE_NUM_PER_BLOCK)
	{
		MutexUnlock(gEvChargeInfo.hMetex_FLASH);
		return;
	}
	Addres = ((u16PageIdx) * BYTES_NUM_PER_PAGE) + ADDR_UN_RPT_FLAG_PST;
	u8Num[0] = VAL_RPT_FLAG;
	
	SPIFlashWrite(u8Num, Addres, 1);			
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

UINT16 FLASH_GetUnRptNum(UINT16* pLastUnRptIDX)
{
	UINT16 u16UnRptNum = 0;
	UINT8 u8BlockNo = 0;
	UINT16 u16PageIdx = 0;
	UINT32 Addres = 0;
	UINT8 u8Num[6];
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	for (u8BlockNo = 0; u8BlockNo < RECORD_BLOCK_MAX_NUM; u8BlockNo++)
	{
		for (u16PageIdx = 0; u16PageIdx < PAGE_NUM_PER_BLOCK; u16PageIdx++)
		{
			Addres = ((u8BlockNo * PAGE_NUM_PER_BLOCK + u16PageIdx) * BYTES_NUM_PER_PAGE) + ADDR_TOTAL_NUM_PST;
			SPIFlashRead(u8Num, Addres, 6);
			if (u8Num[0] == 0xff
				&& u8Num[1] == 0xff
				&& u8Num[2] == 0xff
				&& u8Num[3] == 0xff
				&& u8Num[4] == 0xff
				&& u8Num[5] == 0xff)//全FF 说明这一页没有使用
			{
				//break;
			}
			else  if(u8Num[5] == PAGE_WriteOk_FLAG)
			{
				if (u8Num[4] == VAL_UN_RPT_FLAG)  //0x5A 已经上传。	FF未上传
				{
					*pLastUnRptIDX = (u8BlockNo * PAGE_NUM_PER_BLOCK + u16PageIdx);
					u16UnRptNum++;													
				}
				gEvChargeInfo.u32FLASHUnRptNum = u16UnRptNum;
			}
		}
	}
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
	return u16UnRptNum;
}


void FLASH_GetRecordsTable(ChargeLogTable* pTable)
{
	UINT8 u8BlockNo = 0;
	UINT16 u16PageIdx = 0;
	UINT32 Addres = 0;
	UINT8 u8Num[6];
	pTable->u16UnRptNum = 0;
	pTable->u32CurrPageIdx = 0;
	pTable->u32TotalNum = 0;
	pTable->u16LastPageIdx = 0;
	gEvChargeInfo.u32FLASHUnRptNum = 0;
	UINT8 u8FindPageFlag = FALSE;
	UINT32 uMaxSqcNo = 0;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	for (u8BlockNo = 0; u8BlockNo < RECORD_BLOCK_MAX_NUM; u8BlockNo++)
	{
		for (u16PageIdx = 0; u16PageIdx < PAGE_NUM_PER_BLOCK; u16PageIdx++)
		{
			Addres = ((u8BlockNo * PAGE_NUM_PER_BLOCK + u16PageIdx) * BYTES_NUM_PER_PAGE) + ADDR_TOTAL_NUM_PST;
			SPIFlashRead(u8Num, Addres, 6);
			if (u8Num[0] == 0xff && u8Num[1] == 0xff && u8Num[2] == 0xff && u8Num[3] == 0xff && u8Num[4] == 0xff && u8Num[5] == 0xff)
			{
				if (u8FindPageFlag==FALSE)
				{
					pTable->u32CurrPageIdx = u8BlockNo * PAGE_NUM_PER_BLOCK + u16PageIdx;
					u8FindPageFlag = TRUE;
				}
			}
			else  if(u8Num[5] == PAGE_WriteOk_FLAG)  //数据有效
			{
				//pTable->u32CurrPageIdx = u8BlockNo * PAGE_NUM_PER_BLOCK + u16PageIdx;
				uMaxSqcNo = ((UINT32)u8Num[0]<<24) + ((UINT32)u8Num[1]<<16) + ((UINT32)u8Num[2]<<8) + (UINT32)u8Num[3];
				if (uMaxSqcNo >= pTable->u32TotalNum)
				{
					pTable->u32TotalNum = uMaxSqcNo;
					pTable->u16LastPageIdx = u8BlockNo * PAGE_NUM_PER_BLOCK + u16PageIdx;
				}

				if (u8Num[4] == VAL_UN_RPT_FLAG)
				{
					pTable->u16UnRptNum++;						//0x5A 已经上传 	0XFF未上传。
				}
				gEvChargeInfo.u32FLASHUnRptNum = pTable->u16UnRptNum;
			}
		}
	}
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void FlashCleanRecords(void)
{
	UINT8 u8BlockNo = 0;
	UINT32 Addres = 0;
	gChargeLogTable.u16UnRptNum = 0;
	gChargeLogTable.u32CurrPageIdx = 0;
	gChargeLogTable.u32TotalNum = 0;
	gChargeLogTable.u16LastPageIdx = 0;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	//将FLASH 刷为 FF
	for (u8BlockNo = 0; u8BlockNo < RECORD_BLOCK_MAX_NUM; u8BlockNo++)
	{
		Addres = (u8BlockNo * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
		SPIFlashEraseBlock(Addres);
		OSTimeDly(10);
	}
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

UINT8  FlashWaitClean_U1_Data(void)
{
	int i = 0;
	for(i = 0; i < 1000; i++)
	{
		OSTimeDly(10);
		if(gEvChargeInfo.u8_Clean_U1_Flash == FALSE)
		{
			return TRUE;
		}
	}
	return FALSE;
}

UINT8  FlashWaitClean_U2_Data(void)
{
	int i = 0;
	for(i = 0; i < 1000; i++)
	{
		OSTimeDly(10);
		if(gEvChargeInfo.u8_Clean_U2_Flash == FALSE)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void FlashClean_U1_Data(void)
{
	UINT8 u8BlockNo = 0;
	UINT32 Addres = 0;
	//if(TRUE == gEvChargeInfo.u8_Clean_U1_Flash)
	{
		MutexLock(gEvChargeInfo.hMetex_FLASH);
		//将FLASH 刷为 FF
		u8BlockNo = RECT_U1_SAVE_BLOCK_NO;
		Addres = (u8BlockNo * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
		SPIFlashEraseBlock(Addres);
		OSTimeDly(2);
		Addres = ((u8BlockNo+1) * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
		SPIFlashEraseBlock(Addres);
//by  lxr01202
		OSTimeDly(2);
		Addres = ((u8BlockNo+2) * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
		SPIFlashEraseBlock(Addres);
		MutexUnlock(gEvChargeInfo.hMetex_FLASH);
		gEvChargeInfo.u8_Clean_U1_Flash = FALSE;
	}
}

void FlashClean_U2_Data(void)
{
	UINT8 u8BlockNo = 0;
	UINT32 Addres = 0;
	//if(TRUE == gEvChargeInfo.u8_Clean_U2_Flash)
	{
		MutexLock(gEvChargeInfo.hMetex_FLASH);
		u8BlockNo = RECT_U2_SAVE_BLOCK_NO;
		Addres = (u8BlockNo * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
		SPIFlashEraseBlock(Addres);
		OSTimeDly(2);
		Addres = ((u8BlockNo+1) * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
		SPIFlashEraseBlock(Addres);
		MutexUnlock(gEvChargeInfo.hMetex_FLASH);
		gEvChargeInfo.u8_Clean_U2_Flash = FALSE;
	}
}

void FlashClean_IMEU2_Data(void)
{
	UINT8 u8BlockNo = 0;
	UINT32 Addres = 0;

	MutexLock(gEvChargeInfo.hMetex_FLASH);
	//将FLASH 刷为 FF
	u8BlockNo = IMEU2_UPGRADE_START_BLOCK_NO;
	Addres = (u8BlockNo * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
	SPIFlashEraseBlock(Addres);
	OSTimeDly(2);
	Addres = ((u8BlockNo+1) * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
	SPIFlashEraseBlock(Addres);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

//NOINIT UINT8 u8Buffer[BYTES_NUM_PER_PAGE + 2];
//void FlashDebug_U1_Data(void);
//void FlashDebug_U1_Data(void)
//{
//	__XXX_UNION_VALUE unVal;
//	static int siFlag = TRUE;
//	int i = 0;
//	//int iReadBytes = 0;
//	UINT32 SAddres = 0;
//	ExGetSigInfo(E_TYPE_SYS, ID_U1_BYTES_NUM, &unVal,NULL, BY_E2PROM);
//
//	int iRemainder = (gEvChargeInfo.u16_U1_Bytes%BYTES_NUM_PER_PAGE);
//	int iTempValue = (gEvChargeInfo.u16_U1_Bytes - (iRemainder));
//	iTempValue = iTempValue/BYTES_NUM_PER_PAGE;
//
//	if(FALSE == siFlag)
//	{
//		MutexLock(gEvChargeInfo.hMetex_FLASH);
//		SAddres = (RECT_U1_SAVE_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
//		for(i = 0; i < iTempValue; i++)
//		{
//			memset(u8Buffer, 0, BYTES_NUM_PER_PAGE);
//			SPIFlashRead(u8Buffer ,SAddres + (i*BYTES_NUM_PER_PAGE), BYTES_NUM_PER_PAGE);
//			OSTimeDly(2);
//		}
//
//		if(iRemainder > 0)
//		{
//                        memset(u8Buffer, 0, BYTES_NUM_PER_PAGE);
//			SPIFlashRead(u8Buffer ,SAddres + (iTempValue*BYTES_NUM_PER_PAGE), iRemainder);
//			OSTimeDly(2);
//		}
//		MutexUnlock(gEvChargeInfo.hMetex_FLASH);
//	}
//}

void FlashWrite_U1_Data(UINT8* pData, UINT32 u32Addr, UINT32 len)
{
	UINT32 iActualAddr= 0;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	//将FLASH 刷为 FF
	iActualAddr = (RECT_U1_SAVE_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + u32Addr;
	SPIFlashWrite(pData, iActualAddr, len);
	OSTimeDly(2);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void FlashWrite_U2_Data(UINT8* pData, UINT32 u32Addr, UINT32 len)
{
	UINT32 iActualAddr= 0;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	//将FLASH 刷为 FF
	iActualAddr = (RECT_U2_SAVE_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + u32Addr;
	SPIFlashWrite(pData, iActualAddr, len);
	OSTimeDly(2);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void FlashWrite_IMEU2_Data(UINT8* pData, UINT32 u32Addr, UINT32 len)
{
	UINT32 iActualAddr= 0;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	//将FLASH 刷为 FF
	iActualAddr = IMEU2_UPDATE_DATA_ADDR + u32Addr;
	SPIFlashWrite(pData, iActualAddr, len);
	OSTimeDly(2);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void FlashWrite_IMEU2_DataLen(UINT8* pData, UINT32 u32Addr, UINT32 len)
{
	UINT32 iActualAddr= 0;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	iActualAddr = IMEU2_UPDATE_BASE_ADDR + u32Addr;
	SPIFlashWrite(pData, iActualAddr, len);
	OSTimeDly(2);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

UINT16 FLASH_GetPageIdx(UINT32 uSqsNo)
{
	UINT16	PageIdx = 0;
	UINT32	Addres = 0;
	UINT8 u8Val[8];
	UINT32 u32TotalRecordNuM = 0;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	for (PageIdx = 0; PageIdx < (RECORD_BLOCK_MAX_NUM * PAGE_NUM_PER_BLOCK); PageIdx++)
	{
		Addres = (PageIdx * BYTES_NUM_PER_PAGE) + ADDR_TOTAL_NUM_PST;
		SPIFlashRead(u8Val, Addres, 6);
		u32TotalRecordNuM = ((u8Val[0]<<24) + (u8Val[1]<<16) + (u8Val[2]<<8) + u8Val[3]);
		if ((u8Val[5] == PAGE_WriteOk_FLAG) && (u32TotalRecordNuM == uSqsNo))
		{
			MutexUnlock(gEvChargeInfo.hMetex_FLASH);
			return PageIdx;
		}
	}
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
	return PageIdx;
}

UINT8 FLASH_GetUnRptPageIdx(UINT16 *pPageIdx, UINT16 u16LastOpPageIdx)
{
	UINT16	PageIdx = 0;
	UINT32	Addres = 0;
	UINT8 u8Val[8];
	//UINT32 u32TotalNum, u32ViewRecordNo;
	UINT16 u16LastPageIdx;
	
	u16LastPageIdx = u16LastOpPageIdx;

	MutexLock(gEvChargeInfo.hMetex_FLASH);

	for (PageIdx = 0; PageIdx < (RECORD_BLOCK_MAX_NUM * PAGE_NUM_PER_BLOCK); PageIdx++)
	{
		Addres = (u16LastPageIdx * BYTES_NUM_PER_PAGE) + ADDR_TOTAL_NUM_PST;
		SPIFlashRead(u8Val, Addres, 6);
		//u32TotalRecordNuM = ((u8Val[0]<<24) + (u8Val[1]<<16) + (u8Val[2]<<8) + u8Val[3]);
		if (u8Val[5] == PAGE_WriteOk_FLAG)
		{
			/*u16ValidRecordNum++;

			if(u16ValidRecordNum >= u32TotalNum)
			{
				MutexUnlock(gEvChargeInfo.hMetex_FLASH);
				return FALSE;
			}*/

			if (u8Val[4] == VAL_UN_RPT_FLAG)
			{
				*pPageIdx = u16LastPageIdx;
				MutexUnlock(gEvChargeInfo.hMetex_FLASH);
				return TRUE;
			}			
		}		
		
		if(u16LastPageIdx > 0)
		{
			u16LastPageIdx--;
		}
		else
		{
			u16LastPageIdx = (PAGE_NUM_PER_BLOCK * RECORD_BLOCK_MAX_NUM - 1);
		}
	}
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
	return FALSE;
}

UINT8 FLASH_ReadRecords(UINT32 u32Addr, UINT8* pBuf, UINT16 u16Len)
{
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	if (u32Addr >= (RECORD_BLOCK_MAX_NUM * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE))
	{
		memset(pBuf, 0, u16Len);
		MutexUnlock(gEvChargeInfo.hMetex_FLASH);
		return FALSE;
	}
	SPIFlashRead(pBuf, u32Addr, u16Len);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
	return TRUE;
}

//返回值，当前操作页
UINT16 FLASH_WriteRecords(ChargeLogTable *pTable, __CHARGE_RECORD* pStRecord, unsigned char u8NeedRpt)
{
	UINT16 u16OptPageIdx = 0;
	UINT32 Addres = 0;
	UINT32 u32Val = 0;
	UINT8 u8Num[6];
	UINT8 u8DebugNum[6];
	UINT8 u8CheckFLASH[6];
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	//擦写 BLOCK
	if (0 == pTable->u32CurrPageIdx
		|| PAGE_NUM_PER_BLOCK == pTable->u32CurrPageIdx
		|| (PAGE_NUM_PER_BLOCK *(RECORD_BLOCK_MAX_NUM - 1)) == pTable->u32CurrPageIdx)																																//擦除该扇区！！
	{
		Addres = (pTable->u32CurrPageIdx * BYTES_NUM_PER_PAGE) + 1;																							//这个地址只要在这个扇区即可
		SPIFlashEraseBlock(Addres);
		Delay(500);
	}
	else
	{
		Addres = pTable->u32CurrPageIdx * BYTES_NUM_PER_PAGE;
		//防止 写一半断电情况,导致下一条记录 也无法存储，	只要有一个字节不是 0XFF  说明被动过
		SPIFlashRead(u8CheckFLASH, Addres, 6);
		if (u8CheckFLASH[0] != 0xff || u8CheckFLASH[1] != 0xff || u8CheckFLASH[2] != 0xff || u8CheckFLASH[3] != 0xff  || u8CheckFLASH[4] != 0xff || u8CheckFLASH[5] != 0xff)
		{
			//当前页面的成功标志位置要写上失败标志，以防下次继续判断错误
			u8Num[0] = PAGE_WriteFail_FLAG;
			Addres = ((pTable->u32CurrPageIdx) * BYTES_NUM_PER_PAGE) + PAGE_WriteOk_FLAG_PST;
			SPIFlashWrite(u8Num, Addres, 1);

			pTable->u32CurrPageIdx++;

			if (pTable->u32CurrPageIdx >= RECORD_BLOCK_MAX_NUM * PAGE_NUM_PER_BLOCK)
			{
				pTable->u32CurrPageIdx = 0;
			}

			if (0 == pTable->u32CurrPageIdx
				|| PAGE_NUM_PER_BLOCK == pTable->u32CurrPageIdx
				|| (PAGE_NUM_PER_BLOCK *(RECORD_BLOCK_MAX_NUM - 1)) == pTable->u32CurrPageIdx)														//擦除该扇区！！
			{
				Addres = (pTable->u32CurrPageIdx * BYTES_NUM_PER_PAGE) + 1;																			//这个地址只要在这个扇区即可
				SPIFlashEraseBlock(Addres);
			}
		}
	}

	UINT16 u16LEN11 = 0;
	u16OptPageIdx = pTable->u32CurrPageIdx;
	Addres = pTable->u32CurrPageIdx * BYTES_NUM_PER_PAGE;
	UINT8* pRecord = (UINT8*)&(pStRecord->u8SpearType);
	u16LEN11 = (sizeof(__CHARGE_RECORD)/sizeof(UINT8));

	SPIFlashWrite(pRecord, Addres, u16LEN11);		//只要不超过250字节即可！
	
	//写 总记录数
	pTable->u32TotalNum++;
	u32Val = pTable->u32TotalNum;
	Addres = ((pTable->u32CurrPageIdx) * BYTES_NUM_PER_PAGE) + ADDR_TOTAL_NUM_PST;
	u8Num[0] = (((u32Val) >> 24)& 0xff);
	u8Num[1] = ((u32Val >> 16) & 0xff);
	u8Num[2] = ((u32Val >> 8) & 0xff);
	u8Num[3] = (u32Val & 0xff);
	SPIFlashWrite(u8Num, Addres, (4));

	//DEBUG
	SPIFlashRead(u8DebugNum, Addres, (4));

	//直接将 未上报记录标记为 已经上报
	if (u8NeedRpt == FALSE)
	{
		Addres = ((pTable->u32CurrPageIdx) * BYTES_NUM_PER_PAGE) + ADDR_UN_RPT_FLAG_PST;
		u8Num[0] = VAL_RPT_FLAG;
		SPIFlashWrite(u8Num, Addres, 1);
	}
	else
	{
		pTable->u16UnRptNum++;
	}
	
	//页成功标志
	u8Num[0] = PAGE_WriteOk_FLAG;
	Addres = ((pTable->u32CurrPageIdx) * BYTES_NUM_PER_PAGE) + PAGE_WriteOk_FLAG_PST;
	SPIFlashWrite(u8Num, Addres, 1);

	pTable->u16LastPageIdx = pTable->u32CurrPageIdx;

		
	pTable->u32CurrPageIdx++;
	if (pTable->u32CurrPageIdx >= RECORD_BLOCK_MAX_NUM * PAGE_NUM_PER_BLOCK)
	{
		pTable->u32CurrPageIdx = 0;		
	}

	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
	
	return u16OptPageIdx;
}

void DebugFlash(void);
void DebugFlash(void)
{
	OSTimeDly(100);
	if (u16DEBUG_Flash_LEN <= 0)
	{
		return;
	}
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	SPIFlashRead(DEBUG_Flash, u16DEBUG_Flash_ADDR, u16DEBUG_Flash_LEN);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}


static void RefreshFlashTime(void)
{
	struct tm  tmNow;
	static UCHAR  Sec = 0;
	gRTCData.fGetLocalTime(&tmNow);
	if((UCHAR)tmNow.tm_sec != Sec)/*1秒钟*/
	{
		Sec = (UCHAR)tmNow.tm_sec;
		/*if(gEvChargeInfo.i8ProcRecordDelay>0)
		{
			gEvChargeInfo.i8ProcRecordDelay--;
		}*/
		if(gs_EthAppCommMng.i32EV_FLASH_RecordDelay>0)
		{
			gs_EthAppCommMng.i32EV_FLASH_RecordDelay--;
		}
	}
}
void RealTimeChgDataSave(void)
{
	UCHAR i, j;
	UINT32 Addres;
	UINT8 u8BlockBase;
	UCHAR *pTest;
	UCHAR ucdata[5];
	

	//掉电记录 起始信息
	for(j = 0; j < 2; j++)
	{
		if(gs_EepromRecordMng.u8WriteStartDataFlag[j])
		{
			gs_EepromRecordMng.u8WriteStartDataFlag[j] = 0;

			gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j] = 0;

			if(j == 0)
			{
				u8BlockBase = GUN_A_CHG_DATA_BLOCK_BASE;
			}
			else
			{
				u8BlockBase = GUN_B_CHG_DATA_BLOCK_BASE;
			}

			gSelfTest.fSetTaskID(Task_ID_Flash);

			//写启动数据
			pTest = (UCHAR *)&gs_EepromRecordMng.StartData[j];	
			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_StartData) * BYTES_NUM_PER_PAGE);
			SPIFlashEraseBlock(Addres+1);
			gSelfTest.fSetTaskID(Task_ID_Flash);
			OSTimeDly(20);

			SPIFlashWrite(pTest, Addres, mRecordEepromStartDataSize);
			gSelfTest.fSetTaskID(Task_ID_Flash);

			//写启动时间phase
			pTest = (UCHAR *)&gs_EepromRecordMng.u8StartTimePhase[j];
			Addres += mRecordEepromStartDataSize;
			SPIFlashWrite(pTest, Addres, 3);
			gSelfTest.fSetTaskID(Task_ID_Flash);

			//写结束时间phase			
			pTest = (UCHAR *)&gs_EepromRecordMng.u8EndTimePhase[j];	
			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_EndTimePhase) * BYTES_NUM_PER_PAGE);
			Addres += (gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j]*3);
			SPIFlashWrite(pTest, Addres, 3);
			gSelfTest.fSetTaskID(Task_ID_Flash);	

			//OSTimeDly(10);

			//写kwh		
			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_Kwh) * BYTES_NUM_PER_PAGE);
			//Addres += gs_EepromRecordMng.u8LastTimePhase[j]*4;
			Addres += gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j]*5;			
			floattoascii(gs_EepromRecordMng.fStepKwh[j], ucdata); 
			ucdata[4] = gs_EepromRecordMng.u8LastTimePhase[j];
			SPIFlashWrite(ucdata, Addres, 5);
			gSelfTest.fSetTaskID(Task_ID_Flash);


			//写结束时间phase地址		
			pTest = (UCHAR *)&gs_EepromRecordMng.u8EndTimePhase[j];	
			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_No) * BYTES_NUM_PER_PAGE);
			Addres += (gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j]);
			SPIFlashWrite(&gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j], Addres, 1);
			gSelfTest.fSetTaskID(Task_ID_Flash);	
			

			OSTimeDly(20);

		}
	}

	//掉电记录 半点信息
	for(j = 0; j < 2; j++)
	{
		if(gs_EepromRecordMng.u8WriteTimeDataFlag[j])
		{
			gs_EepromRecordMng.u8WriteTimeDataFlag[j] = 0;
			
			if(gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j] >= 47)
			{				
				continue;
			}
			gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j]++;


			if(j == 0)
			{
				u8BlockBase = GUN_A_CHG_DATA_BLOCK_BASE;
			}
			else
			{
				u8BlockBase = GUN_B_CHG_DATA_BLOCK_BASE;
			}

			//写半点数据
			pTest = (UCHAR *)&gs_EepromRecordMng.TimeData[j];
			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_RealData) * BYTES_NUM_PER_PAGE);
			Addres += ((UINT32)gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j] * mRecordEepromTimeDataSize);
			SPIFlashWrite(pTest, Addres, mRecordEepromTimeDataSize);
			gSelfTest.fSetTaskID(Task_ID_Flash);


			//写kwh		
			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_Kwh) * BYTES_NUM_PER_PAGE);
			//Addres += (gs_EepromRecordMng.u8LastTimePhase[j]*4);
			Addres += gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j]*5;			
			floattoascii(gs_EepromRecordMng.fStepKwh[j], ucdata); 
			ucdata[4] = gs_EepromRecordMng.u8LastTimePhase[j];
			SPIFlashWrite(ucdata, Addres, 5);
			gSelfTest.fSetTaskID(Task_ID_Flash);	


			//写结束时间phase			
			pTest = (UCHAR *)&gs_EepromRecordMng.u8EndTimePhase[j];	
			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_EndTimePhase) * BYTES_NUM_PER_PAGE);
			Addres += (gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j] * 3);
			SPIFlashWrite(pTest, Addres, 3);
			gSelfTest.fSetTaskID(Task_ID_Flash);

			//写结束时间phase地址		
			pTest = (UCHAR *)&gs_EepromRecordMng.u8EndTimePhase[j];	
			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_No) * BYTES_NUM_PER_PAGE);
			Addres += (gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j]);
			SPIFlashWrite(&gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j], Addres, 1);
			gSelfTest.fSetTaskID(Task_ID_Flash);	

			OSTimeDly(20);
		}
	}

	//掉电记录 存完FLASH， 清除EEPROM记录
	for(j = 0; j < 2; j++)
	{
		if(gs_EepromRecordMng.u8ClearDataFlag[j])
		{
			gs_EepromRecordMng.u8ClearDataFlag[j] = 0;
			gs_EepromRecordMng.u8WiteEndTimePhaseFlag[j] = 0;

			if(j == 0)
			{
				u8BlockBase = GUN_A_CHG_DATA_BLOCK_BASE;
			}
			else
			{
				u8BlockBase = GUN_B_CHG_DATA_BLOCK_BASE;
			}

			Addres = ((u8BlockBase * PAGE_NUM_PER_BLOCK + GUN_CHG_DATA_PAG_StartData) * BYTES_NUM_PER_PAGE);
			SPIFlashEraseBlock(Addres+1);
			
			OSTimeDly(20);
		}
	}
}
/*********************************************************************************************
	FLAHS 立即记录逻辑：
	SetCharge STOP  Cmd	  停止命令后	
	API_CreateRecordLog   并置写FLASH 的标志。
	如果离线 记录未上报索引
***********************************************************************************************/
void TASK_FlashProc(void* pdata)
{
	OSTimeDly(50);
	OSTimeDly(50);
	SPIFlashInit();
	OSTimeDly(500);
	OSTimeDly(500);

	ChargeLogTable LogTable;
	FLASH_GetRecordsTable(&LogTable);
	gChargeLogTable = LogTable;
	u16DEBUG_Flash_ADDR = 0; 
	u16DEBUG_Flash_LEN = sizeof(SaveRecord2Flash);

	FLASH_GetHistEventTable(&gs_HistEventTable);
	RecordEvent(HISTORY_EVENT_RESET, mVERHIGH, mVERLOW, 0);
   // gs_HistoryEventMng.ucClearEventFlag  = TRUE;
	for(;;)
	{
		
		gSelfTest.fSetTaskID(Task_ID_Flash);
		RefreshFlashTime();

		if(TRUE == gFlashOptInfo.u8CleanRecordsFlag&&(gEvChargeInfo.u8RectUpgradeFlag == FALSE))
		{
			FlashCleanRecords();
			gFlashOptInfo.u8CleanRecordsFlag = FALSE;
			OSTimeDly(100);
			FLASH_GetRecordsTable(&LogTable);
			gChargeLogTable = LogTable;
			gSelfTest.fSetTaskID(Task_ID_Flash);
		}

		gSelfTest.fSetTaskID(Task_ID_Flash);
		if (TRUE == gFlashOptInfo.u8CleanBulkFlag&&(gEvChargeInfo.u8RectUpgradeFlag == FALSE))
		{
			SPIFlashEraseChip();
			FlashCleanRecords();
			gFlashOptInfo.u8CleanBulkFlag = FALSE;
			OSTimeDly(100);
			FLASH_GetRecordsTable(&LogTable);
			gChargeLogTable = LogTable;
			gSelfTest.fSetTaskID(Task_ID_Flash);
		}

		if(gEvChargeInfo.u8RectUpgradeFlag == FALSE)
		{
			//ChargeRecordDeal();
		}
		gSelfTest.fSetTaskID(Task_ID_Flash);
		OSTimeDly(3);

		gSelfTest.fSetTaskID(Task_ID_Flash);
		//掉电充电记录恢复处理
		if(gs_EepromRecordMng.u8InitCompleteFlag[GUN_NO_A] == EEPROM_RECORD_InitFlag_WriteFlash && (gEvChargeInfo.u8RectUpgradeFlag == FALSE))
		{
			gSelfTest.fSetTaskID(Task_ID_Flash);
			//SaveRecordLog_FromEEPROM(GUN_NO_A);
			gs_EepromRecordMng.u8InitCompleteFlag[GUN_NO_A] = EEPROM_RECORD_InitFlag_WriteOk;
			gs_EepromRecordMng.u8ClearDataFlag[GUN_NO_A] = TRUE;
			gSelfTest.fSetTaskID(Task_ID_Flash);
			OSTimeDly(100);
		}
		if(gs_EepromRecordMng.u8InitCompleteFlag[GUN_NO_B] == EEPROM_RECORD_InitFlag_WriteFlash && (gEvChargeInfo.u8RectUpgradeFlag == FALSE))
		{
			gSelfTest.fSetTaskID(Task_ID_Flash);
			//SaveRecordLog_FromEEPROM(GUN_NO_B);
			gs_EepromRecordMng.u8InitCompleteFlag[GUN_NO_B] = EEPROM_RECORD_InitFlag_WriteOk;
			gs_EepromRecordMng.u8ClearDataFlag[GUN_NO_B] = TRUE;
			gSelfTest.fSetTaskID(Task_ID_Flash);
			OSTimeDly(100);
		}

		//FlashClean_U1_Data();
		//FlashClean_U2_Data();

		//FlashDebug_U1_Data();
		//掉电记录需要保存的数据
		//RealTimeChgDataSave();

		OSTimeDly(20);

		//历史事件
		if((TRUE == gs_HistoryEventMng.ucClearEventFlag) && (gEvChargeInfo.u8RectUpgradeFlag == FALSE))
		{
			FlashCleanHistEvent();
			gs_HistoryEventMng.ucClearEventFlag = FALSE;

			OSTimeDly(50);

		}


		if(gEvChargeInfo.u8RectUpgradeFlag == FALSE)
		{
			if(HistEventSaveToFlash() == TRUE)
			{
				OSTimeDly(20);
			}		
		}		
		
		//DebugFlash();
		gSelfTest.fSetTaskID(Task_ID_Flash);
	}
}
