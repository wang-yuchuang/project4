
#ifndef	HISTORYEVENT_H
#define	HISTORYEVENT_H


/*历史事件存放的Block地址*/
#define HIST_EVENT_BLOCK_NUM			3	 //16Mbit共有 32个block
#define HIST_EVENT_BLOCK_BASE_NO		15   //起始BLOCK号
#define BYTES_PER_HIST_EVENT			64	 //每条历史事件字节数
#define EVENT_NUMBER_PER_BLOCK			(PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE / BYTES_PER_HIST_EVENT)   //每块包含的历史事件条数 1024

#define HIST_EVENT_NO_ADDR_PST			60		//历史事件序号位置
#define HIST_EVENT_WriteOk_FLAG_PST		63		//历史事件写成功标志 位置
#define HIST_EVENT_WriteOk_FLAG			0xA5	//历史事件写成功标志
#define HIST_EVENT_WriteFail_FLAG		0x5A	//历史事件写失败标志  -- 例如写了一半掉电了

#define HIST_EVENT_BASE_ADDR			(HIST_EVENT_BLOCK_BASE_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE)  //起始地址

#define HIST_EVENT_MAX_NUMBER			(EVENT_NUMBER_PER_BLOCK * HIST_EVENT_BLOCK_NUM)		      //flash中存放最大历史事件条数	

/************************************************/
/*历史事件定义                                  */
/************************************************/
#define HISTORY_EVENT_RESET                   10
#define HISTORY_EVENT_CTRL_FAN_ON         11
#define HISTORY_EVENT_SETDATA_CHG         12
#define HISTORY_EVENT_ENTER_TEST         13

#define HISTORY_EVENT_CTRL_FAN_ON_IMEU2         14








#define HISTORY_EVENT_DEBUG_MODE		         14
#define HISTORY_EVENT_IMMU2_STOP_BATT_CHG         15

#define HISTORY_EVENT_EnterManualMode         16
#define HISTORY_EVENT_SystemFault		         17
#define HISTORY_EVENT_BattError		         18

#define HISTORY_EVENT_MENU_RESET              254
#define HISTORY_EVENT_SETDATA_DEFAULT         255



//内存中缓存条数
#define	HISTORYEVENT_RAM_NUM	200


typedef struct tagHistoryEvent
{
    unsigned char EventType;			/*事件类型  0:正常*/
    unsigned char EventTable[2];                /*事件表下标 (16bits 高字节在前)*/    
    unsigned char EventTime[6];			/*事件发生时间:月,日,时,分,秒*/
    unsigned char cValue;			/*字节型事件数据:开关量告警,控制量,设置量*/
    unsigned char fValue[4];                    /*浮点型事件数据:模拟量告警,控制量,设置量*/ 
}HistoryEvent_TypeDef;			      /*每个记录13个字节*/


typedef struct tagHistoryEventMng
{
    HistoryEvent_TypeDef  HistEventBuff[HISTORYEVENT_RAM_NUM];
    unsigned short   HistEventBuffWrPos;      //写入指针,指向HistEventBuff[]        
    unsigned short   HistEventBuffRdPos;      //读取指针,指向HistEventBuff[]      
    unsigned char   ucClearEventFlag;        //清历史事件标志 1- 是 
    unsigned short  u16HistoryEventNum;       //历史事件个数 //内存中的个数
    unsigned char   HistoryEventUpdate;       //刷新标志 > 0 需要刷新
	unsigned char  HistEventFull;	//内存中
}tagHistoryEventMng_TypeDef;	

typedef struct tagHistEventTable
{
	UINT16 u16TotalNum;
	UINT16 u16CurrSqcNo;
	UINT16 u16CurrPageIdx;			//永远是可以需要写的那个PAGE
	UINT16 u16LastPageIdx;			//最后一页,即序号最大那一页
}tagHistEventTable_def;


typedef struct tagHistAlmWriteToUdisc
{
	unsigned char u8HistAlmFileOpenFlag;	//文件打开标志
	unsigned char u8OpenHistAlmFileCmd;	//来自外界的存储命令标志

	UINT16 u16CurrLogIdx;					//当前读取条数
	UINT16 u16CurrLogIdxPos;				//当前读取位置

	UINT16 u16CurrLogIdx_FromFlash;			//从FLASH读取	
	UINT8  u8ReadFromFlash_Flag;			//0-不需要读取FLASH   1-需要读取
}tagHistAlmWriteToUdisc_def;
extern tagHistAlmWriteToUdisc_def gs_HistAlmWriteToUdisc;

//------------------------------------------------------------------------------
// 			  全局变量申明
//------------------------------------------------------------------------------
extern tagHistoryEventMng_TypeDef gs_HistoryEventMng;

extern tagHistEventTable_def gs_HistEventTable;

//------------------------------------------------------------------------------
// 			  全局函数申明
//------------------------------------------------------------------------------
void RecordEvent(UCHAR ucType, unsigned short u16Table, UCHAR ucData, signed int  fData);
void Init_HistoryEvent(void);
void FLASH_GetHistEventTable(tagHistEventTable_def* pTable);
void FlashCleanHistEvent(void);
UINT8 HistEventSaveToFlash(void);

UINT8 u8HistAlmWriteToUdisc(UINT16* uPLen);
UINT8 u8HistAlmWriteToUdisc_FromFlash(UINT16* uPLen);

#endif
/*end*/





