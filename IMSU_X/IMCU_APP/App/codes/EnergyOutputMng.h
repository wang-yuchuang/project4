#ifndef ENERGYOUTPUTMNG_H
#define ENERGYOUTPUTMNG_H


#include "main.h"
#include "can_ring.h"
#include "Uart6_new.h"

/* 枪充电阶段定义 */
//gs_EnergyOutputMng.GunChgPhase
enum  tagGunChgPhaseDef
{ 
	GunChgPhase_Init = 0,   //初始
	GunChgPhase_ReqApprove,//申请启动自检(需要模块)
	GunChgPhase_GotApprove,//申请到模块资源
	GunChgPhase_SelfChk,   //自检
	GunChgPhase_Config,   //配置
	GunChgPhase_Charging,   //充电（接触器已吸合）
	GunChgPhase_Stop,				//结束
	GunChgPhase_EnterCEM,			//进入超时处理
	GunChgPhase_EnterCEM_Phase2,	//进入超时处理 -- 发生报文超时，需要等模块停止输出后才进入CRM重连阶段
	GunChgPhase_EnterEVTimeout,   //进入超时处理
	GunChgPhase_Max
};

//输出设备类型定义 
enum ConverterDevTypeDef
{
	ConverterDevType_Rect = 0,  //AC/DC整流模块 - 给车充电用
	ConverterDevType_Rect_Batt, //AC/DC整流模块 - 给储能电池充电用
	ConverterDevType_DCDC,	  //DC/DC模块 - 由电池供电,给车充电用
	ConverterDevType_Max
};
//gs_EnergyOutputMng.ConverterOutCtrlReq  输出设备当前控制命令
enum ConverterOutCtrlReqCmdDef
{
	ConverterOutCtrlReq_Close = 0,  //关
	ConverterOutCtrlReq_Forward,    //正向打开
	ConverterOutCtrlReq_Reverse,	  //反向打开
	ConverterOutCtrlReq_Max	  
};

//gs_EnergyOutputMng.ConverterIsValidFor 输出设备可以用于什么用途
enum ConverterIsValidForDef
{
	ConverterIsValidFor_None = 0,
	ConverterIsValidFor_EVChg,			//汽车充电
	ConverterIsValidFor_EnergyBatt,		//储能电池充电
	ConverterIsValidFor_PV,		//PV
};
//注释：汽车充电期间，保持 储能电池补电模块 打开状态，以保证DCDC模块一直处于通电状态，避免断电状态突然上电引起地址重排
//      当需要给汽车充电时，先把 储能电池补电模块 定义成汽车充电 用途

//gs_EnergyOutputMng.ParallelRelayDisconnectDelay
#define	ParallelRelayDisconnect_Delay				3  //单位:秒
//gs_EnergyOutputMng.ParallelRelayConnectDelay
#define	ParallelRelayConnect_Delay					3  //单位:秒

//gs_EnergyOutputMng.ConverterOutCtrlReq_Delay
#define	mConverterOutCtrlReq_Delay					5  //单位:秒

//gs_EnergyOutputMng.ConverteSerialParellModeCtrlReq_Delay
#define	mConverteSerialParellModeCtrlReq_Delay		3  //次数	

typedef struct tagEnergyOutputMng
{  		
//	UINT8	GunChgPhase[All_GUN_NUM];				//充电阶段

	//并机控制
	UINT8	GunOwner[All_GUN_NUM];					//当前枪资源归属
	UINT8	ParallelRelayDisconnectDelay;			//当需要撤除另组资源时，先关模块输出，间隔一定延时后断开接触器
	UINT8	ParallelRelayConnectDelay;				//当需要并入另组资源时，先关模块输出，间隔一定延时后并上接触器
	UINT8	ParallelRelayStat;						//并机接触器状态
	UINT8	ParallelRelayStat_2;					//环网接触器状态
	UINT8	GunOutRelayStat[All_GUN_NUM];

	//枪角度的控制
	UINT8	GunPwrOnOffReq[All_GUN_NUM];			//当前枪的模块输出开关控制
//	UINT8	GunRelayOnOffReq[All_GUN_NUM];			//当前枪的输出接触器开关控制
	float   GunVoltReq[All_GUN_NUM];				//当前枪的模块输出电压控制
	float   GunCurrReq[All_GUN_NUM];				//当前枪的模块输出电流控制
	float	fGunMaxOutCurrCap[All_GUN_NUM];			//最大电流能力
	float	fGunRealOutPutCurr[All_GUN_NUM];		//输出电流
	float	fGunRealOutPutVolt[All_GUN_NUM];		//输出电压

	float	fEsBattPwrCapDistribution[All_GUN_NUM];		//储能电池的功率分配
	float	fACPwrCapDistribution[All_GUN_NUM];		//交流输入的功率分配

	//各输出设备可以用于做什么用途
	UINT8	ConverterIsValidFor[All_GUN_NUM][ConverterDevType_Max];	

	//各类模块角度的控制
	UINT8	ConverterOutCtrlReq[All_GUN_NUM][ConverterDevType_Max];		//各输出设备的控制命令
	UINT8	ConverteSerialParellModeCtrlReq[All_GUN_NUM][ConverterDevType_Max];		//各输出设备的高低压模式切换命令
	UINT8	ConverteSerialParellModeCtrlReq_Back[All_GUN_NUM][ConverterDevType_Max];		//各输出设备的高低压模式切换命令
	UINT8	ConverteSerialParellModeCtrlReq_Delay[All_GUN_NUM][ConverterDevType_Max];		//各输出设备的高低压模式切换命令

	float   ConverterVoltReq[All_GUN_NUM][ConverterDevType_Max];		//当前枪的模块输出电压控制
	float   ConverterCurrReq[All_GUN_NUM][ConverterDevType_Max];		//当前枪的模块输出电流控制	
	float	fConverterMaxOutCurrCap[All_GUN_NUM][ConverterDevType_Max];	//最大电流能力
	float	fConverterRealOutPutCurr[All_GUN_NUM][ConverterDevType_Max];//当前输出电流
	float	fConverterRealOutPutVolt[All_GUN_NUM][ConverterDevType_Max];//当前输出电压

	//下面这两项用于切换模块方向控制
	UINT8	ConverterOutCtrlReq_Backup[All_GUN_NUM][ConverterDevType_Max];		//各输出设备的控制命令备份
	UINT8	ConverterOutCtrlReq_Delay[All_GUN_NUM][ConverterDevType_Max];		//各输出设备的控制命令生效，需要一定延时

	UINT16  u16StartTime[All_GUN_NUM];
	UINT16  u16StartTimeCnt;

	//DCDC模块保护计数器
	UINT8  u8ConverterNoProectDelay[All_GUN_NUM];
	UINT8  u8ESBattIsNotStableFlag[All_GUN_NUM];


	float	fGunReqPwrFromImmu_Batt[All_GUN_NUM];	//充电期间向上位机申请的电池功率
	float	fGunReqPwrFromImmu_AC[All_GUN_NUM];	//充电期间向上位机申请的交流功率

	UINT8  u8IsFirstEntChging[All_GUN_NUM];    //刚进入充电阶段
	float   fConvtCtrlCurrReq_Bak[All_GUN_NUM][ConverterDevType_Max];	//当前枪的各模块输出电流控制备份

}EnergyOutputMng_TypeDef;

extern EnergyOutputMng_TypeDef gs_EnergyOutputMng;

extern void init_EnergyOutputMng(void);
extern void AutoEnergyOutputMng(void);

//extern unsigned int g_u32MpptAdjustPeriod;

#endif
