/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Insulation.h
** @Founder      : WYC834
** @Date         : 2025.10.21
** @Function     : 绝缘检测策略
** @Instructions : 
**===========================================================================================*/
#ifndef _INSULATION_H
#define _INSULATION_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define mInsulationVoltStableLimit	     2    //绝缘检测电压稳定阀值(PE对地电压经420倍放大,ADC抖动需留余量)
#define mInsulationStartMINVoltage       10   //绝缘检测可以启动检测到最小电压值
#define mInsulationVoltStableTimeLimit	 1000    //绝缘检测电压稳定时间阀值，超过则报错
#define NORMAL_VALUE	5000
#define FIRST_STEP_R_P  3000U
#define FIRST_STEP_R_N  2606.2
#define SECOND_STEP_R_P 0
#define SECOND_STEP_R_N 952.048

#define SHORT_VOLT_LIMIT	0
#define ALARM_LIMIT_RATE	500  //500欧/V
#define FAULT_LIMIT_RATE	100  //100欧/V
#define INS_TERM_STATE_HOLD_TIME    5  //终态(End/OverTime/Fault)锁存时间，单位同RunTimers(≈1s/tick)


/*=============================================================================================
** @Data type definition
**===========================================================================================*/
enum
{
	INSULATION_STEP_1 = 0,
	INSULATION_STEP_2 = 1,
	INSULATION_STEP_3 = 2,
	ALL_INSULATION_STEP,
};
enum
{
	eInsulation_Start = 0,
	eInsulation_Process,
	eInsulation_Stop,
	eInsulation_End,
	eInsulation_OverTime,
	eInsulation_Fault,
};//0:自动检测 1：正在检测 2：停止检测 3：检测完成 4：检测超时 5：检测失败 

enum
{
	AutoDetect = 0,
	StartDetect = 1,
	StopDetect = 2,
};

enum
{
	First_Time = 0,
	Second_Time = 1,
	ALL_GET_VOLT_TIME,
};

enum
{
    UserCtrl = 0,		// 0:自动检测      1：开始检测   2：停止检测
	DoCtrlChannl = 1,	// 继电器通道选择   0：所有继电器 1：DO1 2:DO2
	DoCtrlState = 2,	// 继电器状态       0：断开 1：闭合
	DoCtrlConfirm = 3,		// 继电器控制确认   1：确认
	UserCtrlCmd_len = 4,
};

enum
{
    CtrlFeedback = 0,	// 绝缘检测控制结果
	FeedbackRes = 1,	//保留
	CtrlFeedback_len = 2,
};

struct _InsulationDetectMng
{
	UCHAR ucInsulationMode;        //0:Control 1: AutoDetect 2：StopDetect
	UCHAR ucInsulationCtrlStep;
	UCHAR ucGetVoltDelay;
	UCHAR ucVoltStable;  
	UCHAR SwitchRelays;

	UCHAR ucInsulationState;
	UCHAR ucInsulationAlarmState_Disp;  // 告警显示，只在重新采集后才更新
    UCHAR ucIsDuringChargingPhase;		//是否是充电期间的绝缘检测   0-否 1-是
	
	float fBatVoltage;
	float fN2GVoltage;
	float fP2GVoltage;
	float fBatVoltageBack;   //用于检测母排电压是否稳定
	float fN2GVoltageBack;
	float fP2GVoltageBack;
	float fN2GVoltageCalc[ALL_GET_VOLT_TIME];
	float fP2GVoltageCalc[ALL_GET_VOLT_TIME];
	float fN2GResistance;//负对地电阻
	float fP2GResistance;//正对地电阻
	float fN2GResistanceLast;//负对地电阻，主要是为了保存最近一次的计算结果，方便查找问题
	float fP2GResistanceLast;//正对地电阻，主要是为了保存最近一次的计算结果，方便查找问题
};
typedef struct _InsulationDetectMng InsulationDetectMng;

extern InsulationDetectMng gs_InsulationDetectMng;
extern u16 UserCtrlCmd_55[UserCtrlCmd_len];
extern u16 CtrlCmdFeedback_56[CtrlFeedback_len];
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/

/*=============================================================================================
** @Name      : void InsulationProcess(void);
** @Input     : void
** @Output    : void
** @Function  : 绝缘检测流程
** @The notes :
**===========================================================================================*/
void InsulationProcess(void);
/*=============================================================================================
** @Name      : void ControlRLYSwitchTime(void);
** @Input     : void
** @Output    : void
** @Function  : 板上继电器切换至少间隔时间（ms）
** @The notes : 
**===========================================================================================*/
void ControlRLYSwitchTime(void);

void Insulation_InitInsulationData(void);


#endif /* _INSULATION_H */