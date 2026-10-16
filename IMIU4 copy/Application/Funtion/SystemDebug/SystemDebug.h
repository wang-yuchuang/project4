/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SystemDebug.c
** @Founder      : WYC834
** @Date         : 2025.12.10
** @Function     : 系统调试校准模块
** @Instructions : 系统自身调试校准
**===========================================================================================*/
#ifndef _SYSTEMDEBUG_H
#define _SYSTEMDEBUG_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define PASSWORDH           0X2020                  //权限密码高16位
#define PASSWORDL           0X0101                  //权限密码低16位


#define HV_CORR_BLOWUP          1                //放大倍数
#define HV_CORR_FIL_T           1                   //电压滤波次数

#define OC_CORR_BLOWUP          1               //放大倍数
#define OC_CORR_FIL_T           1                   //电流滤波次数

#define MAX_HALL_CORR_10MV  10                      //不能大于10mv，对应2A电流
#define HALL_ZERO_CORR_0A  2500                      //HALL在0安时，对应2500mv
/*=============================================================================================
** @Data type definition
**===========================================================================================*/
// static u8 sCSampExpFlag = 0;            /*电流采样异常标志 bit0:霍尔5V异常 bit1:霍尔回检信号异常 bit2:模拟前端采样异常 bit3:霍尔ADC通道异常 bit4:霍尔AD值异常 bit5:分流器AD值异常 bit6:电流校零异常[0:正常 1:异常]*/
// static s32 sRealCurr10mA = 0;           /*实时瞬时电流采样值 10mA*/
// static u32 sRealVolt10uV = 0;           /*实时瞬时电压采样值 0.01mV*/


// static s16 sCurrZeroCorr = 0;           /*电流零点修正值(10mA)*/
// static u8 sCurrZeroFin = 0;             /*电流零点修正完成标志 1完成 2失败*/

// /*同时使能霍尔和分流器时以分流器为主,霍尔作为参考单独记录*/
// static s32 sHallRealCurr10mA = 0;       /*霍尔实时瞬时电流采样值 10mA*/
// static s32 sHallRealVolt10uV = 0;       /*实时瞬时霍尔电压采样值 0.01mV*/
// static s16 sHallZeroCorr = 0;           /*霍尔电流零点修正值(10mA)*/


/*系统采样校准命令值*/
typedef enum
{
	eCORR_INVALID = 0,                  /*0:无校准*/
	eCORR_HANDZRO = 1,                  /*1:电流零飘手动校准*/
	eCORR_HALOUTS = 2,                  /*2:霍尔电流单点校准*/
	eCORR_HALOUTD = 3,                  /*3:霍尔电流双点校准*/
	eCORR_BSHUCHS = 4,                  /*4:BAT分流器充电电流单点校准*/
	eCORR_BSHUCHD = 5,                  /*5:BAT分流器充电电流双点校准*/
	eCORR_BSHUDHS = 6,                  /*6:BAT分流器放电电流单点校准*/
	eCORR_BSHUDHD = 7,                  /*7:BAT分流器放电电流双点校准*/

	eCORR_LSHUCHS = 8,                  /*8:LINK分流器充电电流单点校准*/
	eCORR_LSHUCHD = 9,                  /*9:LINK分流器充电电流双点校准*/
	eCORR_LSHUDHS = 10,                 /*10:LINK分流器放电电流单点校准*/
	eCORR_LSHUDHD = 11,                 /*11:LINK分流器放电电流双点校准*/

	eCORR_SUMVOLS = 12,                 /*12:电池总压单点校准*/
	eCORR_SUMVOLD = 13,                 /*13:电池总压双点校准*/
	eCORR_LINVOLS = 14,                 /*14:Link电压单点校准*/
	eCORR_LINVOLD = 15,                 /*15:Link电压双点校准*/
	eCORR_PEGVOLS = 16,                 /*16:绝缘检测PE单点电压校准*/
	eCORR_PEGVOLD = 17,                 /*17:绝缘检测PE双点电压校准*/
}e_SysSampCorrCmd;

/*系统采样校准命令值*/
typedef enum
{
	SysCorrCmdType = 0,                  /*采样校准类型*/
	SysCorrValue = 1,                  /*校准点基准值*/
    SysCorrCmd_Len = 14,             /*校准命令长度*/
}e_CalibrationCtrl_52;


/*权限密码*/
typedef enum
{
	PasswordH = 0,                  /*权限密码H*/
	PasswordL = 1,                  /*权限密码L*/
	Password_Len = 2,
}e_AccessPassword;

typedef enum
{
    PermissionLevel  = 0,	//当前权限级别
    PermissionChannel = 1,	//当前权限通道
    PermissionInfo_Len = 2,
}e_Permission;

typedef enum
{
    eAuthL_User = 0,                                        /*普通用户*/
    eAuthL_Admin = 1,                                       /*超级用户(管理人员)*/
}e_AuthOpenLever;                                           /*权限开放级别*/

typedef enum
{
    eAuthL_Can = 0,                                        /*Can*/
    eAuthL_Reserved = 1,                                   /*保留*/
    eAuthL_Usart = 2,                                      /*Usart*/
}e_AuthChannel;   

typedef enum
{

    CalibrationType = 0,//校准类型
    CalibrationState  = 1,//校准状态
    CalibrationResult = 2,//校准结果
	Calibration_Len = 3,
}e_CaliFeedback_58;

typedef enum
{
	UnCalib = 0,			//未校准
	DelCalibParam = 1,		//清除校准参数
	WaitFirstBaseV = 2,		//等待第一个基准高压
	SampleFirstV = 3,		//采集第1个测量电压
	WaitSecondBaseV = 4,	//等待第2个基准电压
	SampleSecondV = 5,		//采集第2个测量电压
	CalcCalibParam = 6,		//校准计算处理

	CalibSuccess = 100,		 //校准成功
	CalibFail = 101,		 //校准失败
}e_CalibrationStep;



typedef enum
{
	eRESULT_HANDZRO = 0,                  /*0:电流零飘手动校准*/
	eRESULT_HALOUTS = 1,                  /*1:霍尔电流单点校准*/
	eRESULT_HALOUTD = 2,                  /*2:霍尔电流双点校准*/
	eRESULT_SHUCHGS = 3,                  /*3:分流器充电电流单点校准*/
	eRESULT_SHUCHGD = 4,                  /*4:分流器充电电流双点校准*/
	eRESULT_SHUDHGS = 5,                  /*5:分流器放电电流单点校准*/
	eRESULT_SHUDHGD = 6,                  /*6:分流器放电电流双点校准*/
	eRESULT_SUMVOLS = 7,                  /*7:电池总压单点校准*/
	eRESULT_SUMVOLD = 8,                  /*8:电池总压双点校准*/
	eRESULT_LINVOLS = 9,                 /*9:Link电压单点校准*/
	eRESULT_LINVOLD = 10,                 /*10:Link电压双点校准*/
	eRESULT_PEGVOLS = 11,                 /*11:绝缘检测PE单点电压校准*/
	eRESULT_PEGVOLD = 12,                 /*12:绝缘检测PE双点电压校准*/
}e_SysSampCorrResult;

//系统调试类型命令值
typedef enum
{
    DebugType = 0,                   //调试类型
    DebugOption = 1,                 //调试选项
    DebugState = 2,                  //调试状态
    DebugEepWriteH = 3,              //EEP写数据高
    DebugEepWriteL = 4,              //EEP写数据低
    DebugComfirm= 5,                //调试确认
    Debug_Len                     	
}ENUM_Debug;

typedef enum
{
    DebugFbType = 0,                   //调试类型
    DebugFbOption = 1,                 //调试选项
    DebugFbState = 2,                  //调试状态
    DebugFbEepWriteH = 3,              //EEP写数据高
    DebugFbEepWriteL = 4,              //EEP写数据低
    DebugFbEepReadH = 5,              //EEP读数据高
    DebugFbEepReadL = 6,              //EEP读数据低
    DebugFbResult = 7,                //调试结果
    DebugFeedback_Len                 //调试反馈信息长度          
}ENUM_Debug_Type;


typedef enum                                /*高压校准类型*/
{
    eHVCorr_SumVS = 0,                      /*电池总压单点校准*/
    eHVCorr_SumVD = 1,                      /*电池总压双点校准*/
    eHVCorr_LinVS = 2,                      /*Link+电压单点校准*/
    eHVCorr_LinVD = 3,                      /*Link+电压双点校准*/
    eHVCorr_PegVS = 4,                      /*PE电压单点校准*/
    eHVCorr_PegVD = 5,                      /*PE电压双点校准*/
}e_HVCorrType;

typedef enum                                /*高压校准值序号*/
{
	eHVCorr_SumVK = 0,                      /*电池总压采样通道校准K值*/
	eHVCorr_SumVB,                          /*电池总压采样通道校准B值*/
	eHVCorr_LinVK,                          /*Link电压采样通道校准K值*/
	eHVCorr_LinVB,                          /*Link电压采样通道校准B值*/
	eHVCorr_PegVK,                          /*PE电压采样通道校准K值*/
	eHVCorr_PegVB,                          /*PE电压采样通道校准B值*/
}e_HVCorrValue;


typedef enum                                /*电流校准类型*/
{
    eOCCorr_HalOutCS = 0,                   /*霍尔输出电流单点校准*/
    eOCCorr_HalOutCD = 1,                   /*霍尔输出电流双点校准*/
    eOCCorr_BShuChgS = 2,                   /*电池端分流器充电电流单点校准*/
    eOCCorr_BShuChgD = 3,                   /*电池端分流器充电电流双点校准*/
    eOCCorr_BShuDhgS = 4,                   /*电池端分流器放电电流单点校准*/
    eOCCorr_BShuDhgD = 5,                   /*电池端分流器放电电流双点校准*/
    eOCCorr_LShuChgS = 6,                   /*负载端分流器充电电流单点校准*/
    eOCCorr_LShuChgD = 7,                   /*负载端分流器充电电流双点校准*/
    eOCCorr_LShuDhgS = 8,                   /*负载端分流器放电电流单点校准*/
    eOCCorr_LShuDhgD = 9,                   /*负载端分流器放电电流双点校准*/

}e_OCCorrType;

typedef enum                                /*电流校准值序号*/
{
	eOCCorr_HalOutK = 0,                    /*霍尔电流采样通道校准K值*/
	eOCCorr_HalOutB,                        /*霍尔电流采样通道校准B值*/
	eOCCorr_BShuChK,                        /*分流器充电电流采样通道校准K值*/
	eOCCorr_BShuChB,                        /*分流器充电电流采样通道校准B值*/
	eOCCorr_BShuDhK,                        /*分流器放电电流采样通道校准K值*/
	eOCCorr_BShuDhB,                        /*分流器放电电流采样通道校准B值*/
	eOCCorr_LShuChK,                        /*分流器充电电流采样通道校准K值*/
	eOCCorr_LhuChB,                         /*分流器充电电流采样通道校准B值*/
	eOCCorr_LShuDhK,                        /*分流器放电电流采样通道校准K值*/
	eOCCorr_LShuDhB,                        /*分流器放电电流采样通道校准B值*/

}e_OCCorrValue;

//系统调试类型命令值
typedef enum
{
    eDBGCMD_CTRLDRY = 0,                 //干接点调试
    eDBGCMD_CTRLRLY = 1,                 //继电器器调试
    eDBGCMD_RWEEPPRM = 2,                //EEP读写调试
    eDBGCMD_FIN                          //命令结束
}e_DebugTypeCmd;

/*系统调试结果值*/
typedef enum
{
    eDBGRES_UNEXE = 0,                  //未执行调试
	eDBGRES_SUCCE,                      //调试成功
	eDBGRES_NOMOD,                      //非调试模式
	eDBGRES_ERRPA,                      //错误参数
	eDBGRES_CANOF,                      //通信中断
	eDBGRES_FIN                         //调试结果结束
}e_DebugResult;

extern u16 CalibrationCtrl_52[SysCorrCmd_Len];//系统校准命令信息
extern u16 AccessPassword_53[Password_Len];//权限密码
extern u16 SystemPermission_54[PermissionInfo_Len];
extern u16 CaliFeedback_58[Calibration_Len];//校准反馈信息
extern u16 SystemDebugCmd_60[Debug_Len];//系统调试命令
extern u16 DebugFeedback_61[DebugFeedback_Len];//系统调试反馈
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void InPutSystemPermissionArray (u8 index ,u16 data);
** @Input     : index:系统权限信息下标 data:数据
** @Output    : void
** @Function  : 输入系统权限信息
** @The notes :
**===========================================================================================*/
void InPutSystemPermissionArray (u8 index ,u16 data);

/*=============================================================================================
** @Name      : u16 GetSystemPermissionArray (u8 index);
** @Input     : index:系统权限信息下标
** @Output    : 系统权限信息
** @Function  : 获取系统权限信息
** @The notes :
**===========================================================================================*/
u16 GetSystemPermissionArray (u8 index);

/*=================================================================================================
** @Name      : void AuthorityInit(void)
** @Input     : void
** @Output    : void
** @Function  : 管理权限初始化
** @The notes : 默认权限处理
**===============================================================================================*/
void AuthorityInit(void);

/*=================================================================================================
** @Name      : void RcvSysAuthorityCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 系统接收切换管理权限命令处理函数
** @The notes : 通信口命令,按钮和内部口默认最高权限不需切换
**===============================================================================================*/
void RcvSysAuthorityCmdAPI(u8 port);

/*=================================================================================================
** @Name      : void UpdateHVoltCorrResult(void)
** @Input     : void
** @Output    : void
** @Function  : 更新高压校准结果
** @The notes : 根据校准标志位
**===============================================================================================*/
void UpdateHVoltCorrResult(void);

/*=================================================================================================
** @Name      : void RcvSysCalibrationCmdAPI(u16 cmd, u8 port)
** @Input     : cmd:校准指令 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 系统接收校准命令处理函数
** @The notes : 通信口命令,按钮和内部口默认最高权限不需切换
**===============================================================================================*/
void RcvSysCalibrationCmdAPI(u16 cmd, u8 port);

/*=================================================================================================
** @Name      : void ClrEEPHVoltCurrKBInfo(u8 chan)
** @Input     : chan:高压通道 0:电池总压 1:预充电压 2:负载电压 3:link+电压 4:link-电压
** @Output    : void
** @Function  : 清除EEP高压采样校准KB值
** @The notes :
**===============================================================================================*/
void ClrEEPHVoltCurrKBInfo(u8 chan);

/*=============================================================================================
** @Name      : void ClrCurrZeroFinFlagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电流零点校零标志
** @The notes : 重新校零
**===========================================================================================*/
void ClrCurrZeroFinFlagAPI(void);

/*=============================================================================================
** @Name      : u8 SampGetCurrZeroFinFlagAPI(void)
** @Input     : void
** @Output    : 电流采样零飘校准完成标志[0未校准 1校准成功 2校准失败]
** @Function  : 获取电流采样零飘校准完成标志
** @The notes :
**===========================================================================================*/
u8 SampGetCurrZeroFinFlagAPI(void);

/*=================================================================================================
** @Name      : void UpdateOutCurrCorrResult(void)
** @Input     : void
** @Output    : void
** @Function  : 更新电流校准结果
** @The notes : 根据校准标志位
**===============================================================================================*/
void UpdateOutCurrCorrResult(void);

/*=================================================================================================
** @Name      : void RcvGroupDebugOperCmdAPI(u8 index)
** @Input     : index:调试命令索引
** @Output    : void
** @Function  : 接收切换调试操作命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvGroupDebugOperCmdAPI(u8 index);

/*=================================================================================================
** @Name      : void RcvGroupDebugAffirmCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 10:按钮 11:内部
** @Output    : void
** @Function  : 接收调试确认命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvGroupDebugAffirmCmdAPI(u8 port);

/*=================================================================================================
** @Name      : void TacSetGroupDebugAffirmCmdAPI(u8 cmd)
** @Input     : cmd:调试确认命令 1:功能调试确认 2:EEPROM读调试确认 3:EEPROM写调试确认
** @Output    : void
** @Function  : 调试确认命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void TacSetGroupDebugAffirmCmdAPI(u8 cmd);

/*=================================================================================================
** @Name      : void TacDebugOutDryDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制输出干接点状态
** @The notes : 接收输出干接点调试确认命令时调用
**===============================================================================================*/
u8 TacDebugOutDryDeal(void);

/*=================================================================================================
** @Name      : void TacDebugSwitchDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制继电器状态
** @The notes : 接收继电器调试确认命令时调用
**===============================================================================================*/
u8 TacDebugSwitchDeal(void);

/*=================================================================================================
** @Name      : void DebugReadEepData(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式读取EEPROM参数值
** @The notes : 接收EEPROM参数调试确认命令时调用
**===============================================================================================*/
u8 TacDebugReadEepDeal(void);

/*=================================================================================================
** @Name      : void TacDebugWriteEepDeal(void)
** @Input     : void
** @Output    : TRUE:写入成功 FALSE:写入失败
** @Function  : 调试模式写EEP参数值
** @The notes : 接收写EEP参数调试确认命令时调用
**===============================================================================================*/
u8 TacDebugWriteEepDeal(void);

/*=================================================================================================
** @Name      : void RcdEEPHVoltCurrKBInfo(u8 chan)
** @Input     : chan:高压通道 0:电池总压 1:预充电压  2:负载电压 3:link+电压 4:link-电压
** @Output    : void
** @Function  : 记录高压校准KB值到EEP
** @The notes :
**===============================================================================================*/
void RcdEEPHVoltCurrKBInfo(u8 chan);

/*=============================================================================================
** @Name      : void CalcCurrZeroCorr(s32 curr)
** @Input     : curr:空闲时采样霍尔电流零飘值 单位:1mv
** @Output    : void
** @Function  : 计算霍尔电流零飘值
** @The notes : 必须在高压接触器全断开(无电流)时调用,由上层决定，将空闲霍尔偏移成2.5V
**===========================================================================================*/
void CalcCurrZeroCorr(s32 curr);
/*=============================================================================================
** @Name      : void HigVoltSampCorrProcess(void)
** @Input     : void
** @Output    : void
** @Function  : 高压采样校准处理过程
** @The notes : 1s周期调用
**===========================================================================================*/
void HigVoltSampCorrProcess(void);

/*=============================================================================================
** @Name      : void OutCurrSampCorrProcess(void)
** @Input     : void
** @Output    : void
** @Function  : 电流采样校准处理过程
** @The notes : 1s周期调用
**===========================================================================================*/
void OutCurrSampCorrProcess(void);

#endif