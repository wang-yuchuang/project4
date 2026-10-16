
#ifndef __CAN_RING_MAIN_H
#define __CAN_RING_MAIN_H

#include "main.h"


typedef unsigned long time_t_ring;


typedef enum tagRingMergeStat
{
	STAT_NO_MERGE = 0,
	STAT_PENDING,
	STAT_MERGING,
}RINGNODE_MERGE_STAT;


typedef enum tagRingNodeStat
{
	_RingNodeStat_Idle = 0,
	_RingNodeStat_Slave, //1~32枪Idx来区分作为哪个枪的从机/**/
	_RingNodeStat_SubGun,		//辅助枪	
	_RingNodeStat_MasterGun,
	_RingNodeStat_NotAvailable,
	tagRingNodeStat_Max
}tagRingNodeStat_Def;

#define MULTI_GUN_MAX_NUM	4


typedef enum tagSlaveRingCmd
{
	_SlaveRingCmd_None = 0,
	_SlaveRingCmd_ReqBeAuxGun = 0x01,
	_SlaveRingCmd_QuitRing = 0x02,
	_SlaveRingCmd_ReqBeSlave = 0x03,
}tagSlaveRingCmd_Def;

typedef struct _RING_NODE RING_NODE;
struct _RING_NODE
{
	BYTE						byNodeAddress;	//该枪地址/**/
	
	RINGNODE_MERGE_STAT			emMergeStatus;	//0--No merge, 1--Pending, 2--Merging
	tagRingNodeStat_Def			emRingNodeState;
	BYTE						nLockedMasterID;//当作为从机时，此id表示被那把枪锁定
	BYTE						nMasterGunNo;	//0-不是从枪，>0指主枪号
	BOOL						bIsNeedExtCurr;	//当作为从机或主机时，根据限流点判断，该枪还是否需要旁边的枪来并机，以此优化和决定并机和取消并机的时机	
	BOOL						bCommFail;		//10s时间没收到CMD1就算CommFail
	BOOL						bIsExist;		//是否存在，收到过数据即认为存在 0-不存在 1-存在	
	UCHAR						u8AppendInfoByte[2];//节点发送的附加信息码，用于主从机之间传递消息

	BYTE						u8ConvtorCtrlOutStat;		//ACDC\DCDC当前输出状态
	BYTE						u8Reserve;		//20211120 lkf 用于主枪通知从机，高低压切换模式

	BYTE						u8RingRelayFailDelay;		//环网继电器粘连判断计数器 秒
	BYTE						u8RingRelayBreakDelay;		//环网继电器断路判断计数器 秒
	
	float						fOutVolt;
	float						fOutCurr;
	float						fMaxMergeCurr;
	float						fOutCurr_ACDC;
	float						fOutCurr_DCDC;
	float						fInputCurr_ACDC;
	float						fInputCurr_DCDC;
	float						fOutVolt_ACDC;
	float						fOutVolt_DCDC;
	float						fInputVolt_ACDC;
	float						fInputVolt_DCDC;
	float						fMaxMergeCurr_ACDC;
	float						fMaxMergeCurr_DCDC;	
	float						fkWhReading;		//for multiple-gun mode
	float						fGunLmtCurr;		//枪限制电流	
	float						fConvtExtVolt;		//模块外侧电压

	float						fMaxAlowPwr_ACDC;	//最大允许功率
	float						fMaxAlowPwr_DCDC;	

	time_t_ring					tmLastRefresh;		//上次收到CMD1刷新时间
	time_t						tmStartAsMaster;	//以开设锁定主机的时间作为开始时间

	//for convince of searching
	RING_NODE*				left;
	RING_NODE*				right;
};

//u8ConvtorCtrlOutStat
//#define	NODE_ConverterDev_CtrlStat_BitMask				0x03
#define	NODE_ConverterDev_DCDC_CtrlStat_StartBit		2
#define	NODE_ConverterDev_ACDC_DCDC_CtrlStat_BitMask	(0x03|0x0C)
#define	NODE_ConverterDev_ACDC_DCDC_CtrlStat_Close		(0x00|0x00)

typedef enum tagRingMergeMode
{
	RING_MODE_DUE_CURRRENT = 0,
	RING_MODE_INSERT,
	RING_MODE_ONECAR_MULTIGUN
}RING_MERGE_MODE;

#define MAX_SUPPORTED_RING_NODE	16
typedef struct tagRingList
{	
	RING_MERGE_MODE		emRingMergeMode;	
	UCHAR				nGunInsertMode_Start[2];	//插枪切换开始和结束时间, 如果是插枪切换模式的话  H时-M分
	UCHAR				nGunInsertMode_Stop[2];		//H时-M分
	UCHAR				u8RingAddSlaveMode;			//环网增加模式 0-由IMMU2指定  1-自己查找
	unsigned long		u32RingTimeSec;				//环网时间，用于延时处理 单位：秒
	float				fRingStartMergeCurr;		//开启环网电流
	float				fAllRingsPwrLmt;			//环网总功率限制
	float				fRingCurrIncSpeed;			//环网充电电流上升速率
	
	UCHAR				nNodeTotal;					//总接入节点数, 根据配置确定，一旦改变必须重新建链表，故需要重启
	UCHAR				nOnlineTotal;				//实际在线的节点数目
	UCHAR				nExistTotal;				//存在过的节点数目
	UCHAR				bIsRingCfgErr;
	UCHAR				bAnyGunIsRunning;			//是否有枪在充电中
	RING_NODE			allNodes[MAX_SUPPORTED_RING_NODE];
}RING_ALL_INFO;

//bAbnormalDelay
#define	SLAVE_ABNORMAL_DELAY	10

typedef struct _RING_SALVE RING_SALVE;
struct _RING_SALVE
{
	RING_NODE*			pNode;

	BYTE				bIsOnRight;			//1-表示本枪在主枪右侧  0-左侧

	BYTE				bAbnormalDelay;		//异常判断延时确认

	//此两项信息用于通知从机并入
	UCHAR				u8MasterGunNodeAddr;
	UCHAR				u8MasterNodeAddr;

	BYTE				CtrlQuitRing; //0--off,1--on

	BYTE				emSetPwrOnOff_ACDC; //0--off,1--正向 2--反向
	BYTE				emSetPwrOnOff_DCDC; //0--off,1--正向 2--反向
	BYTE				emSetOutRelayOnOff; //0--off,1--on
	BYTE				emSetRingRelayOnOff; //0--off,1--on

	BYTE				CurrentMergeDelayFlag;		//刚并入置1，此时不能输出电流，需要经过一定延时才能输出

	float				fVolt2Sent;			//需要发给它多大电压，由主机计算得到/**/
	float				fCurr2Sent_ACDC;	//需要发给它多大电流，由主机计算得到/**/
	float				fCurr2Sent_DCDC;	//需要发给它多大电流，由主机计算得到/**/
		
	time_t_ring			tm_CtrlQuitRing;
	time_t_ring			tm_LastStartMerging;
	time_t_ring			tm_LastMergeTime;			//正式并入的时间
	time_t_ring			tmlastCtrlRingRelayDiscon;

	time_t_ring			tmlastCtrlRingRelayCon;

	//for continence of searching
	RING_SALVE*		left;
	RING_SALVE*		right;
};


#define MAX_RING_SLAVE_NUM			10

enum RING_GUN_No_Def
{
	RING_GUN_No_MasterGun = 0,
	RING_GUN_No_Aux1,
	RING_GUN_No_Aux2,
	MAX_RING_GUN_NUM,	//最大环网枪数量，第一个为主枪
};
typedef struct tagRingGun
{
	RING_NODE*		pNode;					//从枪节点指针	
	RING_SALVE*		pLeftEdge;
	RING_SALVE*		pRightEdge;

	BYTE			nSlaveTotal;
	BYTE			CtrlQuitRing;		//0--off,1--on
	BYTE			u8WaitSlaveGunKwhReading; //1-有效	

	BYTE			bAbnormalDelay;		//异常判断延时确认

	//枪所在节点
	BYTE			emSetPwrOnOff_ACDC; //0--off,1--正向 2--反向
	BYTE			emSetPwrOnOff_DCDC; //0--off,1--正向 2--反向
	BYTE			emSetOutRelayOnOff; //0--off,1--on
	BYTE			emSetRingRelayOnOff; //0--off,1--on
	float			fVolt2Sent;			//需要发给它多大电压，由主机计算得到
	float			fCurr2Sent_ACDC;	//需要发给它多大电流，由主机计算得到
	float			fCurr2Sent_DCDC;	//需要发给它多大电流，由主机计算得到		

	float			fMaxVolt_ACDC;
	float			fMaxVolt_DCDC;
	float			fMaxVolt_Input_ACDC;
	float			fMaxVolt_Input_DCDC;
	float			fTotalOutCurr_ACDC;
	float			fTotalOutCurr_DCDC;
	float			fTotalOutCurr;
	float			fTotalInputCurr_ACDC;
	float			fTotalInputCurr_DCDC;
	float			fTotalInputCurr;
	float			fMaxMergeCurr_ACDC;
	float			fMaxMergeCurr_DCDC;	
	float			fTotalMaxMergeCurr;

	float			fSlaveGunStart_kWhReading;
	float			fSlaveGunEnd_kWhReading;

	float			fOutCurrOverGunLmtValue;			//输出电流超出枪限制值，需要修正偏差
	float			fEachGunCtrlOutCurrSum;				//每把枪统计控制输出电流和

	float			fMaxAlowPwr_ACDC;
	float			fMaxAlowPwr_DCDC;

	float			fMaxMergeCurr_ACDC_Ex;
	float			fMaxMergeCurr_DCDC_Ex;	
	float			fMaxAlowPwr_ACDC_Ex;
	float			fMaxAlowPwr_DCDC_Ex;

	time_t_ring		tm_CtrlQuitRing;
	time_t_ring		tmlastCtrlRingRelayDiscon;
	time_t_ring		tmlastCtrlRingRelayCon;

}tagRingGun_Def;

//tmlastCtrlRingRelayDiscon
#define	RING_RELAY_DISCON_DELAY	5		//s
//RingGun_Stat
enum Ring_Gun_Stat_def
{
	Ring_Gun_Stat_NoExist = 0,
	Ring_Gun_Stat_Normal,
	Ring_Gun_Stat_CommFail,
};
//bIsReqCurrNeedIncDecSlave_DCDC
enum IncDecConvtOptDef
{
	IncDecConvtOpt_None = 0,
	IncDecConvtOpt_Dec,
	IncDecConvtOpt_Inc,
};
typedef struct tagRingMaster
{
	RING_NODE*		pNode;	

	//从机	
	UCHAR           u8RingSlavesBufNo[MAX_RING_SLAVE_NUM];		//指向s_RingSlavesBuf[x]
	UCHAR			u8RingSlavesBuf_IsUsed[MAX_RING_SLAVE_NUM];	//s_RingSlavesBuf 占用情况
	UCHAR			u8RingSlavesBuf_InHandling;					//指示s_RingSlavesBuf[x]	
	UCHAR			nSlaveTotal;								//包括主枪和辅助枪的从机
	RING_SALVE*		slaves[MAX_RING_SLAVE_NUM];					//最多支持10个slave同时， 指向s_RingSlavesBuf[x]
	RING_SALVE*		pSlave_InHandling;							//正在处理并机或者并机取消请求的Node/**/

	//枪	
	UCHAR			nRingGunTotal;
	UCHAR			nRingAuxGunTotal;
	UCHAR			RingGun_Stat[MAX_RING_GUN_NUM];				
	tagRingGun_Def	sRingGuns[MAX_RING_GUN_NUM];
		
	//主机信息
	BYTE				bIsReceivedLock;						//收到BMS锁定主机请求
	BYTE				bIsReceivedAdj;							//是否收到BMS调节电压电流命令
	BYTE				bIsRecievedStop;						//是否收到停机命令
	BYTE				bIsReceivedStoppOK;						//是否收到BMS通知停机OK的命令, 要确保BMS那边先停机OK才行
	BYTE				bOneCarMultiGunCommFail;				//一车多枪从枪通信中断

	BYTE				bIsCommOverTimeReconfig;				//否收到超时重连命令 - 删除所有从机
	BYTE				emSetOutRelayOnOff;						//0--off,1--on
	float				fSetVolt;
	float				fSetCurr;
	float				fSetCurr_DCDC;
	float				fSetCurr_ACDC;
	float				fTotalOutVolt;
	float				fTotalOutVolt_ACDC; 
	float				fTotalOutVolt_DCDC; 
	float				fTotalOutCurr_ACDC; 
	float				fTotalOutCurr_DCDC; 
	float				fTotalOutCurr;
	float				fTotalInputVolt_ACDC; 
	float				fTotalInputVolt_DCDC; 
	float				fTotalInputCurr_ACDC; 
	float				fTotalInputCurr_DCDC; 
	float				fTotalInputVolt;
	float				fTotalInputCurr;
	float				fTotalMaxMergeCurr_ACDC;
	float				fTotalMaxMergeCurr_DCDC;
	float				fTotalMaxMergeCurr;
	float				fOutCurrCorrectValue;			//输出电流修正偏差	
	float				fSlaveTotalOutKwhReading;

	float				fTotalAlowPwr_ACDC;
	float				fTotalAlowPwr_DCDC;

//20211117 LKF 从机在并机处理预充阶段，有些时候它的能力也需要计算进来
	float				fTotalAlowPwr_ACDC_Ex;
	float				fTotalAlowPwr_DCDC_Ex;
	float				fTotalMaxMergeCurr_ACDC_Ex;
	float				fTotalMaxMergeCurr_DCDC_Ex;
	float				fTotalMaxMergeCurr_Ex;

	//根据IMMU2分配的电源功率来计算所得,进入充电前，该值等于模块额定值，进入充电阶段后，实时计算
	float				fTotalMaxMergeCurr_ACDC_FromSource;
	float				fTotalMaxMergeCurr_DCDC_FromSource;
	float				fTotalMaxMergePwr_ACDC_FromSource;
	float				fTotalMaxMergePwr_DCDC_FromSource;

	
	BYTE				bIsReqCurrNeedIncDecSlave_DCDC; //需求电流比较大，是否需要增加从机
	BYTE				bIsReqCurrNeedIncDecSlave_ACDC;
	float				fReqCurrNeedDecValue_DCDC;		//需求电流比较小，目前超出的电流值大小
	float				fReqCurrNeedDecValue_ACDC;	

	RING_SALVE*			pSlave_ToBeRemovedByCurr;		//因电流需求降低，减少从机时选定的从机
	BYTE				ToRemoveSlaveByCurrDelay;		//因电流需求降低，减少从机时选定的从机	

	time_t_ring			tm_LastCancelMerging;
	time_t_ring			tmlastCtrlChgCurr;				//上一次控制调整充电电流
	time_t_ring			tm_FirstEnterCharging;			//刚进入环网Charing状态的时间
	time_t_ring			tm_CancelMergeDelay;

}RING_AS_MASTER;

typedef struct tagRingAsSlave
{
	RING_NODE		*pMaster;
	RING_NODE		*pMasterGun;

	//作为从机时，相关信息
	BOOL				bReceivedStop;				//收到BMS那边通知的主动申请退出环网命令
	BOOL				bReceivedAdj_FromMasterGun;	//收到主机的调压命令，说明开始进入配置阶段

	//从枪申请并入环网命令
	UCHAR				u8ReqBeSlaveGun_ToMasterGun;//通过CAN收到主枪识别码，需要申请加入主枪
	UCHAR				u8ReqBeWhoseSlaveGun;		//通过CAN收到主枪识别码，需要申请加入的主枪号
	UCHAR				u8BeSlaveGunAck_FromMasterGun;//收到主枪的确认
	UCHAR				u8ReqBeSlave_ToMasterGun;		//通过CAN收到主枪识别码，需要申请加入主枪
	UCHAR				u8ReqBeWhoseSlave;				//通过CAN收到主枪识别码，需要申请加入的主枪号
	UCHAR				u8BeSlaveAck_FromMasterGun;		//收到主枪的确认
	UCHAR				u8RcvJoinReq_FromMaster;		//收到主枪的请求并入
	UCHAR				u8RcvJoinReq_MasterNo;			//收到主枪的请求并入主机号
	UCHAR				u8RcvJoinReq_MasterGunNo;		//收到主枪的请求并入主枪号	
	UCHAR				u8ReqQuitRingCmd;				//申请退出环网命令		
	BYTE				u8RcvMasterGunCommOvertimeCnt;	//接收主枪通信超时计数	
	UCHAR				u8InfoQuitRing_FromMasterGun;	//主机通知退出
	
	BYTE				emSetPwrOnOff_ACDC;
	BYTE				emSetPwrOnOff_DCDC;
	BYTE				emSetRingRelayOnOff;		//0--off,1--on
	BYTE				emSetOutRelayOnOff;			//0--off,1--on
	float				fBusRealVolt;				//用于判断母排电压跟当前输出是否一
	float				fReqVolt;
	float				fReqCurr_ACDC;
	float				fReqCurr_DCDC;	
	
	time_t_ring			tm_eqQuitRingCmd;
	time_t_ring			tm_LasReceive_Cmd_FromMaster;	//上次收到命令时间，当10s内没收到主机命令时自动申请断网
	time_t_ring			tmlastCtrlRingRelayDiscon;
	//time_t_ring			tmlastCtrlRingRelayCon;
	time_t_ring			tmLastCancel;					//记录上次从机取消的时间

}RING_AS_SLAVE;

typedef struct tagRingAsIdle
{
	BOOL				bReceivedLock;//收到BMS那边通知的	
	BYTE				emSetPwrOnOff_ACDC;
	BYTE				emSetPwrOnOff_DCDC;
	BYTE				emSetRingRelayOnOff; //0--off,1--on
	BYTE				emSetOutRelayOnOff;  //0--off,1--on
	float				fReqVolt;
	float				fReqCurr_ACDC;
	float				fReqCurr_DCDC;	
	time_t_ring			tm_LasReceive_Cmd_FromMaster;//上次收到命令时间，当10s内没收到主机命令时自动申请断网
}RING_AS_IDLE;


typedef enum tagRingState
{
	RING_STATE_IDLE = 0,
	//Master
	RING_STATE_MasterGun_FindSlaveGun,
	RING_STATE_MasterGun_Charging,
	RING_STATE_MasterGun_Wait_Merging,
	RING_STATE_MasterGun_Wait_Cancelling, 
	RING_STATE_MasterGun_Stopping, 

	//Slave Gun
	RING_STATE_SlaveGun_WaitMasterGun,
	RING_STATE_SlaveGun_Charging,
	RING_STATE_SlaveGun_Merge_Cancelling,

	//Slave
	RING_STATE_Slave_BeforeMerge,
	RING_STATE_Slave_Merging,
	RING_STATE_Slave_ReqCancell,
	RING_STATE_Slave_Merge_Cancelling,

	RING_MAX_STM_STATE
}RING_STATE_MACHINE_STATE;


typedef RING_STATE_MACHINE_STATE (*ON_RING_STATE_PROC)(void* pArg);

typedef struct tagStateMachineItem
{
//	char						szStateName[16];
	UCHAR	emState;
	ON_RING_STATE_PROC		pfnAction;
	time_t_ring					tm_Enter;// the first time that enter this state
}RING_STM_ITEM;


typedef struct stCanRingData
{
	//环网信息
	RING_NODE			*pNodeMe;	//以me为节点建立双向链表，便于管理
	RING_AS_MASTER		master;		//作为主机时用
	RING_AS_SLAVE		slave;		//作为从机时用
	BOOL				bIsGunA;	//本组数据是否来自A枪
	BOOL				bSelfIDConflict; //自己的ID与其它装ID冲突
	BOOL				bIsGunMergable;
	BOOL				bIsRingRelayShortFail;
	BOOL				bIsRingRelayBreakFail;
	RING_STATE_MACHINE_STATE emCurrState;
	RING_STM_ITEM		stateMachineLoop[RING_MAX_STM_STATE];
	RING_STM_ITEM		*pLastState;// Last状态
	RING_STM_ITEM		*pCurrState;// 当前状态
	//tagGunCtrlReq_Def	*pGunCtrlReq;
	RING_SALVE*			pRingSlaveBuf;
}CAN_RING_DATA;

#define INIT_RING_STATE_MACHINE(_P, _MachineId, _fnProc) ((_P)->stateMachineLoop[_MachineId].emState = (_MachineId), (_P)->stateMachineLoop[_MachineId].pfnAction = (_fnProc))

time_t_ring time_ring(unsigned char ucflag);

BOOL Ring_Initialize(unsigned char ucflag);


void Refresh_SelfNode(CAN_RING_DATA* pRing_Rough);
void Judge_NodeCommFail(CAN_RING_DATA* pRing_Rough);


enum _RING_CTRL_CMD_def
{
	RING_CTRL_CMD_NONE = 0,	
	RING_CTRL_CMD_LOCK_AS_MASTER,		//Lock gun as master
	RING_CTRL_CMD_RQ_ST_SLAVE,			//Request for Stopping Slave
	RING_CTRL_CMD_ST_MASTER,			//Stop Master Charging
	//RING_CTRL_CMD_VOLT_ADJ,				//Ring DC voltage control
	//RING_CTRL_CMD_CURR_ADJ,				//Ring Adjust Rect Total Curr
	RING_CTRL_CMD_ADJ_VOLT_CURR,		//Ring Adjust Rect Total Curr and Voltage 
	RING_CTRL_CMD_EndOfStop,			//Notice End of Stop(Internal)
};
int Ring_Ctrl(UINT8 u8GunNo, UINT8 u8CmdNo, VAR_VALUE *pVarVal1, VAR_VALUE *pVarVal2); //u8CmdNo: enum _RING_CTRL_CMD_def


extern RING_ALL_INFO		g_RingAllInfo;
extern CAN_RING_DATA*		g_pRing_A;
extern CAN_RING_DATA*		g_pRing_B;

extern CanRxMsgTypeDef RxMsg_Ring[CAN_3_MAX_BUFF];

extern RING_NODE *FindNodeLink(UCHAR u8NodeAddress);
//#pragma pack()
extern void Refresh_SelfNode(CAN_RING_DATA* pRing_Rough);

#define	RING_EXCHANGE_UDP_PERIOD	200		//*2ms
#define RingExchangeInfo_BufSize	500
typedef struct RingExchangeInfo_
{
	HANDLE				hMutex;
	unsigned char Sendbuf[RingExchangeInfo_BufSize];
	UINT16 u16SendDataLenth;
	UINT16 u16SendDelayCnt;
}RingExchangeInfo_Def;
extern NOINIT RingExchangeInfo_Def gs_RingExchangeInfo;
extern void PrepareRingExchangeInfo(void);

#define LCCU_NUMBER_RING   (MAX_SUPPORTED_RING_NODE/2)

#define LCCU_EXCHANGE_COMM_ALM_DELAY	20			//s
typedef struct _Lccu_Exchange_Info_Def
{
	UINT8 u8LccuAddr;
	UINT8 u8CommAlmStat;
	UINT8 u8CommAlmDelay;
	UINT8 u8RunStatForACContactor;				//各从机通过它智慧主机需要启动交流接触器
	UINT8 u8DoorOpenAlm;
	signed short i16Temparature;
}Lccu_Exchange_Info_Def;
extern Lccu_Exchange_Info_Def gs_LccuExchangeInfo[LCCU_NUMBER_RING];

#define		DEBUG_AUTO_ADD_DEC_SLAVE_En		0		//使能自动判断加减从机, 有IMMU2监控配置情况下不需要

#define		fContactorShortThreshold_RingRelayShort		60	//判断环网接触器短路，门限电压 V

#define SLAVE_MERGE_OUTPUT_DELAY		12			//从机刚并入时需要按实际电压来控制输出 


//荷兰储能项目，环网只并本桩内的枪
//#define	HeLan_ES_Ring_OnlyMergeAnotherGunWithinSeftCabinet		1

#endif
