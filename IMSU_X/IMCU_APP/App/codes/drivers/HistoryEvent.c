#include "main.h"
#include "HistoryEvent.h"


NOINIT tagHistoryEventMng_TypeDef gs_HistoryEventMng;
tagHistEventTable_def gs_HistEventTable;

tagHistAlmWriteToUdisc_def gs_HistAlmWriteToUdisc = {0};
/*==========================================================================*
* 函数名称 : Init_HistoryEvent
* 函数功能 : 初始历史事件
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
void	Init_HistoryEvent(void)
{
    unsigned short i;
    
    for(i = 0; i < HISTORYEVENT_RAM_NUM; i++)
    {
        gs_HistoryEventMng.HistEventBuff[i].EventType = 0xff;
    }
          
    gs_HistoryEventMng.HistEventBuffWrPos = 0;     
    gs_HistoryEventMng.HistEventBuffRdPos = 0;   
    gs_HistoryEventMng.ucClearEventFlag = 0;
    gs_HistoryEventMng.u16HistoryEventNum = 0;   
    gs_HistoryEventMng.HistoryEventUpdate = 0; 
	gs_HistoryEventMng.HistEventFull = 0;
}

/*==========================================================================*
* 函数名称 : RecordEvent
* 函数功能 : 记录历史事件
* 入口参数 : 
            ucType - 事件类型 (历史事件定义 见 historyevent.h)
            u16Table - 事件下标
            ucData - 字节事件数据
            fData - 浮点型数据数据
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
void RecordEvent(UCHAR ucType, unsigned short u16Table, UCHAR ucData, signed int fData)
{
    unsigned char i;
    
    union
    {
      unsigned short  u16temp;
      unsigned char   chtemp[2];					
    }untempShort;  //临时变量
    
    union
    {
      float  ftemp;
      unsigned char   chtemp[4];	
	  unsigned int   u32temp;
	  signed int i32temp;
    }untempFloat;  //临时变量
  
    untempShort.u16temp = u16Table;
    untempFloat.i32temp = fData;
    
        
    gs_HistoryEventMng.HistEventBuff[gs_HistoryEventMng.HistEventBuffWrPos].EventType = ucType;
    gs_HistoryEventMng.HistEventBuff[gs_HistoryEventMng.HistEventBuffWrPos].EventTable[0] = untempShort.chtemp[0];
    gs_HistoryEventMng.HistEventBuff[gs_HistoryEventMng.HistEventBuffWrPos].EventTable[1] = untempShort.chtemp[1];
    for(i = 0; i < 4; i++)
    {
        gs_HistoryEventMng.HistEventBuff[gs_HistoryEventMng.HistEventBuffWrPos].fValue[i] = untempFloat.chtemp[i];
    }
    
    gs_HistoryEventMng.HistEventBuff[gs_HistoryEventMng.HistEventBuffWrPos].cValue = ucData;
    
    /*月-日-时-分-秒*/
    RecordTime(gs_HistoryEventMng.HistEventBuff[gs_HistoryEventMng.HistEventBuffWrPos].EventTime);
       
    gs_HistoryEventMng.HistEventBuffWrPos++;
    if(gs_HistoryEventMng.HistEventBuffWrPos >= HISTORYEVENT_RAM_NUM)
    {
        gs_HistoryEventMng.HistEventBuffWrPos = 0;
		gs_HistoryEventMng.HistEventFull = 1;
    }    
    
    if(gs_HistoryEventMng.HistoryEventUpdate < 255)
        gs_HistoryEventMng.HistoryEventUpdate++;

	if(gs_HistoryEventMng.u16HistoryEventNum < HISTORYEVENT_RAM_NUM)
	{
		gs_HistoryEventMng.u16HistoryEventNum++;
	}
    
}





UINT16 FLASH_WriteHistEvent(tagHistEventTable_def *pTable, HistoryEvent_TypeDef* pStRecord)
{
	UINT16 u16OptPageIdx = 0;
	UINT32 Addres = 0;
	UINT16 u16Val = 0;
	UINT8 u8Num[4];
	UINT8 u8CheckFLASH[4];

	MutexLock(gEvChargeInfo.hMetex_FLASH);

	if(pTable->u16CurrPageIdx >= HIST_EVENT_MAX_NUMBER)
	{
		pTable->u16CurrPageIdx  = 0;
	}

	//擦写 BLOCK
	if((pTable->u16CurrPageIdx % EVENT_NUMBER_PER_BLOCK) == 0)
	{
		Addres = pTable->u16CurrPageIdx * BYTES_PER_HIST_EVENT + HIST_EVENT_BASE_ADDR;																							//这个地址只要在这个扇区即可
		SPIFlashEraseBlock(Addres+1);
		OSTimeDly(1000);
	}	
	else
	{
		Addres = pTable->u16CurrPageIdx * BYTES_PER_HIST_EVENT+ HIST_EVENT_BASE_ADDR;

		//防止 写一半断电情况,导致下一条记录 也无法存储，	只要有一个字节不是 0XFF  说明被动过
		SPIFlashRead(u8CheckFLASH, Addres+HIST_EVENT_NO_ADDR_PST, 4);

		if (u8CheckFLASH[0] != 0xff || u8CheckFLASH[1] != 0xff || u8CheckFLASH[2] != 0xff || u8CheckFLASH[3] != 0xff)
		{
			//当前页面的成功标志位置要写上失败标志，以防下次继续判断错误
			u8Num[0] = HIST_EVENT_WriteFail_FLAG;
			SPIFlashWrite(u8Num, Addres + HIST_EVENT_WriteOk_FLAG_PST, 1);

			pTable->u16CurrPageIdx++;		

			if (pTable->u16CurrPageIdx >= HIST_EVENT_MAX_NUMBER)
			{
				pTable->u16CurrPageIdx = 0;
			}

			Addres = pTable->u16CurrPageIdx * BYTES_PER_HIST_EVENT + HIST_EVENT_BASE_ADDR;	

			//擦写 BLOCK
			if((pTable->u16CurrPageIdx % EVENT_NUMBER_PER_BLOCK) == 0)
			{																				
				SPIFlashEraseBlock(Addres+1);
				OSTimeDly(1000);
			}	
		}
	}

	u16OptPageIdx = pTable->u16CurrPageIdx;

	UINT16 u16LEN11 = 0;
	u16LEN11 = (sizeof(HistoryEvent_TypeDef)/sizeof(UINT8));
	u16LEN11 = MIN(u16LEN11, 60);

	SPIFlashWrite((UINT8*)pStRecord, Addres, u16LEN11);		
	
	//写序号
	pTable->u16CurrSqcNo++;	
	u16Val = pTable->u16CurrSqcNo;
	u8Num[0] = ((u16Val >> 8) & 0xff);
	u8Num[1] = (u16Val & 0xff);
	u8Num[2] = 0;
	u8Num[3] = HIST_EVENT_WriteOk_FLAG;
	SPIFlashWrite(u8Num, Addres+HIST_EVENT_NO_ADDR_PST, (4));

	pTable->u16LastPageIdx = pTable->u16CurrPageIdx;
		
	pTable->u16CurrPageIdx++;
	if(pTable->u16CurrPageIdx >= HIST_EVENT_MAX_NUMBER)
	{
		pTable->u16CurrPageIdx  = 0;
	}

	if(pTable->u16TotalNum < HIST_EVENT_MAX_NUMBER)
	{
		pTable->u16TotalNum++;
	}

	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
	
	return u16OptPageIdx;
}


void FLASH_GetHistEventTable(tagHistEventTable_def* pTable)
{
	UINT8 u8Num[BYTES_PER_HIST_EVENT];

	UINT8 u8BlockNo;
	UINT16 u16PageIdx;
	UINT32 Addres ;	
	UINT16 u16ReadSqcNo;
	UINT16 u16TotalNum;
	UINT32 u32temp;

	UINT8 u8FindPageFlag = FALSE;
	UCHAR u8FoundLargeNoFlag = 0;
	UCHAR u8FoundSmallNoFlag = 0;

	UINT16 u16MaxSqcNo = 0;
	UINT16 u16MaxSqcNo_Position = 0;
	UINT32 u32MaxSqcNo_2 = 0;	//注意这个要long型
	UINT16 u16MaxSqcNo_Position_2 = 0; 
	
	UINT16 u16MinSqcNo = 0xFFFF;
	UINT16 u16MinSqcNo_Position = 0;
	UINT16 u16MinSqcNo_2 = 0xFFFF;
	UINT16 u16MinSqcNo_Position_2 = 0;

	
	pTable->u16CurrPageIdx = 0;
	pTable->u16TotalNum = 0;
	pTable->u16CurrSqcNo = 0;
	pTable->u16LastPageIdx = 0;
	u16TotalNum = 0;

	MutexLock(gEvChargeInfo.hMetex_FLASH);	
	for (u8BlockNo = 0; u8BlockNo < HIST_EVENT_BLOCK_NUM; u8BlockNo++)
	{
		for (u16PageIdx = 0; u16PageIdx < EVENT_NUMBER_PER_BLOCK; u16PageIdx++)
		{
			Addres = u16PageIdx * BYTES_PER_HIST_EVENT + (HIST_EVENT_BLOCK_BASE_NO + u8BlockNo)*PAGE_NUM_PER_BLOCK*BYTES_NUM_PER_PAGE + HIST_EVENT_NO_ADDR_PST;

			SPIFlashRead(u8Num, Addres, 4);

			if (u8Num[0] == 0xff && u8Num[1] == 0xff && u8Num[2] == 0xff && u8Num[3] == 0xff)
			{
				if (u8FindPageFlag==FALSE)
				{
					pTable->u16CurrPageIdx = (UINT16)u8BlockNo * EVENT_NUMBER_PER_BLOCK + u16PageIdx;
					u8FindPageFlag = TRUE;
				}
				break;  //当前块不再查找
			}
			else if(u8Num[3] == HIST_EVENT_WriteOk_FLAG)  //数据有效
			{
				u16TotalNum++;

				u16ReadSqcNo = ((UINT16)u8Num[0]<<8) + (UINT16)u8Num[1];
				if(u16ReadSqcNo >= u16MaxSqcNo)
				{
					u16MaxSqcNo = u16ReadSqcNo;
					u16MaxSqcNo_Position = u16PageIdx + (UINT16)u8BlockNo * EVENT_NUMBER_PER_BLOCK;  //位置
				}

				if(u16ReadSqcNo <= u16MinSqcNo)
				{
					u16MinSqcNo = u16ReadSqcNo;
					u16MinSqcNo_Position = u16PageIdx + (UINT16)u8BlockNo * EVENT_NUMBER_PER_BLOCK;  //位置
				}

				//出现接近上限的序号，另外存一份
				if(u16ReadSqcNo >= (0xffff - HIST_EVENT_MAX_NUMBER))
				{
					u8FoundLargeNoFlag = 1;
					if(u16ReadSqcNo <= u16MinSqcNo_2)
					{
						u16MinSqcNo_2 = u16ReadSqcNo;
						u16MinSqcNo_Position_2 = u16PageIdx + (UINT16)u8BlockNo * EVENT_NUMBER_PER_BLOCK;  //位置
					}
				}				

				//出现从0开始的序号，另外存一份
				if(u16ReadSqcNo < HIST_EVENT_MAX_NUMBER)
				{
					u8FoundSmallNoFlag = 1;
					u32temp = (UINT32)u16ReadSqcNo+0x10000;
					if(u32temp >= u32MaxSqcNo_2)
					{
						u32MaxSqcNo_2 = u32temp;
						u16MaxSqcNo_Position_2 = u16PageIdx + (UINT16)u8BlockNo * EVENT_NUMBER_PER_BLOCK;  //位置
					}
				}
			} // else if(u8Num[3] == HIST_ALM_LOG_WriteOk_FLAG)  //数据有效
		} // for (u16PageIdx = 0; u16PageIdx < LOG_NUMBER_OF_HIST_ALM_PER_BLOCK; u16PageIdx++)
	} //for (u8BlockNo = 0; u8BlockNo < HIST_ALM_LOG_BLOCK_NUM; u8BlockNo++)

	if(u16TotalNum > 0)
	{
		pTable->u16TotalNum = u16TotalNum;
		if((u8FoundSmallNoFlag == 1) && (u8FoundLargeNoFlag == 1))
		{
			pTable->u16LastPageIdx = u16MaxSqcNo_Position_2;	
			pTable->u16CurrSqcNo = (UINT16)(u32MaxSqcNo_2 - 0x10000);
		}
		else
		{
			pTable->u16LastPageIdx = u16MaxSqcNo_Position;  
			pTable->u16CurrSqcNo = u16MaxSqcNo;
		}

		//20210709 start: 把flash中的历史告警拷贝到ram中
		UINT16 u16HistAlmIndex_InRam;
		if(u16TotalNum >= HISTORYEVENT_RAM_NUM)
		{
			gs_HistoryEventMng.HistEventBuffWrPos = 0;
			gs_HistoryEventMng.HistEventBuffRdPos = 0;
			gs_HistoryEventMng.HistEventFull = 1;			
			gs_HistoryEventMng.u16HistoryEventNum = HISTORYEVENT_RAM_NUM;
			u16HistAlmIndex_InRam = 0;
		}
		else
		{
			gs_HistoryEventMng.HistEventBuffWrPos = u16TotalNum;	
			gs_HistoryEventMng.HistEventBuffRdPos = u16TotalNum;
			gs_HistoryEventMng.u16HistoryEventNum = u16TotalNum;
			gs_HistoryEventMng.HistEventFull = 0;
			u16HistAlmIndex_InRam = 0;
		}
		
		//从最后一条开始恢复
		Addres = pTable->u16LastPageIdx * BYTES_PER_HIST_EVENT + HIST_EVENT_BASE_ADDR;
		for(u16PageIdx = 0; u16PageIdx < pTable->u16TotalNum; u16PageIdx++)
		{			
			SPIFlashRead(u8Num, Addres, BYTES_PER_HIST_EVENT);
			if(u8Num[63] == HIST_EVENT_WriteOk_FLAG)
			{
				memcpy(&gs_HistoryEventMng.HistEventBuff[u16HistAlmIndex_InRam], u8Num, (sizeof(HistoryEvent_TypeDef)/sizeof(UINT8)));
				u16HistAlmIndex_InRam++;
			}
			
			if(u16HistAlmIndex_InRam >= gs_HistoryEventMng.u16HistoryEventNum)
			{
				break;
			}
		

			if(Addres >= ( BYTES_PER_HIST_EVENT + HIST_EVENT_BASE_ADDR))
			{
				Addres -= BYTES_PER_HIST_EVENT;
			}
			else
			{
				Addres = (HIST_EVENT_MAX_NUMBER - 1)*BYTES_PER_HIST_EVENT+ HIST_EVENT_BASE_ADDR;
			}
			
		}  //for(u16PageIdx = 0; u16PageIdx < pTable->u16TotalNum; u16PageIdx++)
		////20210709 end: 把flash中的历史告警拷贝到ram中

	} // if(u16TotalNum > 0)
	

	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void FlashCleanHistEvent(void)
{
	UINT8 u8BlockNo = 0;
	UINT32 Addres = 0;
	unsigned short ii;

	gs_HistEventTable.u16CurrPageIdx = 0;
	gs_HistEventTable.u16TotalNum = 0;
	gs_HistEventTable.u16CurrSqcNo = 0;
	gs_HistEventTable.u16LastPageIdx = 0;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	//将FLASH 刷为 FF
	for (u8BlockNo = 0; u8BlockNo < HIST_EVENT_BLOCK_NUM; u8BlockNo++)
	{
		Addres = ((HIST_EVENT_BLOCK_BASE_NO + u8BlockNo) * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1 ;
		SPIFlashEraseBlock(Addres);
		OSTimeDly(1000);
	}

	 
    
    for(ii = 0; ii < HISTORYEVENT_RAM_NUM; ii++)
    {
        gs_HistoryEventMng.HistEventBuff[ii].EventType = 0xff;
    }
          
    gs_HistoryEventMng.HistEventBuffWrPos = 0;     
    gs_HistoryEventMng.HistEventBuffRdPos = 0;   
    gs_HistoryEventMng.ucClearEventFlag = 0;
    gs_HistoryEventMng.u16HistoryEventNum = 0;   
    gs_HistoryEventMng.HistoryEventUpdate = 0; 
	gs_HistoryEventMng.HistEventFull = 0;

	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}    

UINT8 HistEventSaveToFlash(void)
{
	UINT16 i;
	//tagHistAlmLogTable_def LogTableTemp;
	UINT8 u8WriteFlag = 0;

	for(i = 0; i < HISTORYEVENT_RAM_NUM; i++)
    {                        
        if(gs_HistoryEventMng.HistEventBuffRdPos == gs_HistoryEventMng.HistEventBuffWrPos)
        {
            break;
        }

		FLASH_WriteHistEvent(&gs_HistEventTable, &gs_HistoryEventMng.HistEventBuff[gs_HistoryEventMng.HistEventBuffRdPos]);

		gs_HistoryEventMng.HistEventBuffRdPos++;
        if(gs_HistoryEventMng.HistEventBuffRdPos >= HISTORYEVENT_RAM_NUM)
        {
            gs_HistoryEventMng.HistEventBuffRdPos = 0; 
		}
	
		u8WriteFlag = TRUE;
		break;

	}

	return u8WriteFlag;
}

//NOINIT HistoryAlarm_TypeDef  UdiscHistAlmBuf;
extern NOINIT UCHAR  URecrodBuff[DEBUG_BUF_BYTES_MAX_NUM];
UINT8 u8HistAlmWriteToUdisc(UINT16* uPLen)
{
	UINT16 u16temp, ii;
	UCHAR u8Num;
	 union
    {
      unsigned short  u16temp;
      unsigned char   chtemp[2];					
    }untempShort;  //临时变量
	  union
    {
      float  ftemp;
      unsigned char   chtemp[4];	
	  signed int i32temp;
    }untempFloat;  //临时变量

	//u8Num  = FLASH_ReadHistAlmLog_ForDisp(&gs_HistAlmLogTable, gs_HistAlmWriteToUdisc.u16CurrLogIdx,  (UCHAR *)&UdiscHistAlmBuf, 1);
	u8Num = gs_HistoryEventMng.u16HistoryEventNum;

	if(gs_HistoryEventMng.u16HistoryEventNum == 0)
	{
		return 0;
	}
	
	if(gs_HistAlmWriteToUdisc.u16CurrLogIdx >= gs_HistoryEventMng.u16HistoryEventNum)
	{
		return 0;
	}
	ii = gs_HistAlmWriteToUdisc.u16CurrLogIdx;
	gs_HistAlmWriteToUdisc.u16CurrLogIdx++;

	u16temp = 0;
	u16temp += sprintf( (char*)URecrodBuff + u16temp, "%d.  ",ii+1);

	//for(ii = 0; ii < g_u16AlarmTableNameNum; ii++)
	//{
	//	if(UdiscHistAlmBuf.AlarmTable == AlarmName[ii].AlarmTableNo)
	//	{
	//		u16temp += snprintf( (char*)URecrodBuff+u16temp, 16, "%s", (const char*)(AlarmName[ii].AlarmName));
	//		//u16temp += strncpy((char*)URecrodBuff+u16temp, (char const*)(AlarmName[ii].AlarmName), strnlen((const char*)(AlarmName[ii].AlarmName), 16));
	//		break;
	//	}
	//}

	u16temp += sprintf( (char*)URecrodBuff + u16temp, "  ");


	untempShort.chtemp[0] = gs_HistoryEventMng.HistEventBuff[ii].EventTable[0];
    untempShort.chtemp[1] = gs_HistoryEventMng.HistEventBuff[ii].EventTable[1];

	untempFloat.chtemp[0] = gs_HistoryEventMng.HistEventBuff[ii].fValue[0];
	untempFloat.chtemp[1] = gs_HistoryEventMng.HistEventBuff[ii].fValue[1];
	untempFloat.chtemp[2] = gs_HistoryEventMng.HistEventBuff[ii].fValue[2];
	untempFloat.chtemp[3] = gs_HistoryEventMng.HistEventBuff[ii].fValue[3];

	u16temp += sprintf( (char*)URecrodBuff+u16temp, "Event%d  %d  %d  %d ", 
		gs_HistoryEventMng.HistEventBuff[ii].EventType, untempShort.u16temp,
		gs_HistoryEventMng.HistEventBuff[ii].cValue, (int)(untempFloat.i32temp));	

	u16temp += sprintf( (char*)URecrodBuff + u16temp, "Time %02d-%02d-%02d %02d:%02d:%02d  ", 
		gs_HistoryEventMng.HistEventBuff[ii].EventTime[0], gs_HistoryEventMng.HistEventBuff[ii].EventTime[1],gs_HistoryEventMng.HistEventBuff[ii].EventTime[2],gs_HistoryEventMng.HistEventBuff[ii].EventTime[3],
		gs_HistoryEventMng.HistEventBuff[ii].EventTime[4], gs_HistoryEventMng.HistEventBuff[ii].EventTime[5]);
	
	

	URecrodBuff[u16temp] = '\r';
	URecrodBuff[u16temp+1] = '\n';

	u16temp += 2;

	*uPLen = u16temp;

	

	return 1;
}


//20220627 从flash读取
UINT8 u8HistAlmWriteToUdisc_FromFlash(UINT16* uPLen)
{
	UINT16 u16temp, ii;
	UINT32 u32temp;
	UINT32 Addres;
	UINT8 u8Num[BYTES_PER_HIST_EVENT];
	HistoryEvent_TypeDef sHistEventBuf_Temp;
	union
	{
		unsigned short  u16temp;
		unsigned char   chtemp[2];					
	}untempShort;  //临时变量
	union
	{
		float  ftemp;
		unsigned char   chtemp[4];	
		signed int i32temp;
	}untempFloat;  //临时变量

	/*if(gs_HistEventTable.u16TotalNum <= gs_HistoryEventMng.u16HistoryEventNum)
	{
		return;
	}*/

	
	ii = gs_HistoryEventMng.u16HistoryEventNum + gs_HistAlmWriteToUdisc.u16CurrLogIdx_FromFlash;

	if(ii >= gs_HistEventTable.u16TotalNum)
	{
		return 0;
	}

	if(gs_HistEventTable.u16LastPageIdx >= ii)
	{
		u32temp = gs_HistEventTable.u16LastPageIdx - ii;
	}
	else
	{
		u32temp = gs_HistEventTable.u16LastPageIdx + HIST_EVENT_MAX_NUMBER - ii;
	}

	//从最后一条开始恢复
	Addres = u32temp * BYTES_PER_HIST_EVENT + HIST_EVENT_BASE_ADDR;

	SPIFlashRead(u8Num, Addres, BYTES_PER_HIST_EVENT);

	//20220627 lkf 先简单处理，写标志不ok的直接跳过，
	if(u8Num[63] == HIST_EVENT_WriteOk_FLAG)
	{
		memcpy(&sHistEventBuf_Temp, u8Num, (sizeof(HistoryEvent_TypeDef)/sizeof(UINT8)));

		u16temp = 0;
		u16temp += sprintf( (char*)URecrodBuff + u16temp, "%d.  ", ii+1);


		u16temp += sprintf( (char*)URecrodBuff + u16temp, "  ");


		untempShort.chtemp[0] = sHistEventBuf_Temp.EventTable[0];
		untempShort.chtemp[1] = sHistEventBuf_Temp.EventTable[1];

		untempFloat.chtemp[0] = sHistEventBuf_Temp.fValue[0];
		untempFloat.chtemp[1] = sHistEventBuf_Temp.fValue[1];
		untempFloat.chtemp[2] = sHistEventBuf_Temp.fValue[2];
		untempFloat.chtemp[3] = sHistEventBuf_Temp.fValue[3];

		u16temp += sprintf( (char*)URecrodBuff+u16temp, "Event%d  %d  %d  %d ", 
			sHistEventBuf_Temp.EventType, untempShort.u16temp,
			sHistEventBuf_Temp.cValue, (int)(untempFloat.i32temp));	

		u16temp += sprintf( (char*)URecrodBuff + u16temp, "Time %02d-%02d-%02d %02d:%02d:%02d  ", 
			sHistEventBuf_Temp.EventTime[0], sHistEventBuf_Temp.EventTime[1],sHistEventBuf_Temp.EventTime[2],sHistEventBuf_Temp.EventTime[3],
			sHistEventBuf_Temp.EventTime[4], sHistEventBuf_Temp.EventTime[5]);



		URecrodBuff[u16temp] = '\r';
		URecrodBuff[u16temp+1] = '\n';

		u16temp += 2;

		*uPLen = u16temp;

		gs_HistAlmWriteToUdisc.u16CurrLogIdx_FromFlash++;
		
	}
	//20220627 lkf 先简单处理，写标志不ok的直接跳过，
	else
	{
		gs_HistAlmWriteToUdisc.u16CurrLogIdx_FromFlash++;
		return 0;
	}
	

	return 1;

}  