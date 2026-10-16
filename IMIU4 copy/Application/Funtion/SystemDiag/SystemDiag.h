/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SystemDiag.c
** @Founder      : WYC834
** @Date         : 2026.1.8
** @Function     : 自身诊断模块
** @Instructions : 系统诊断自身故障告警
**===========================================================================================*/
#ifndef _SYSTEMDIAG_H
#define _SYSTEMDIAG_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define DIAG_FILTER_TICK            50                      /*故障诊断滤波周期[必须与实际周期匹配](1ms)*/


#define FAULT_DATATYPE              16                      /*告警数据类型*/
#define FAULT_INDEX_LEN             3                       /*故障索引个数*/
/*=============================================================================================
** @Data type definition
**===========================================================================================*/
enum
{
	SystemAlarmInfo1 = 0,
	SystemAlarmInfo2 = 1,
	SystemAlarmInfo3 = 2,
    SystemFaultInfo1 = 3,
    SystemFaultInfo2 = 4,
    SystemFaultInfo3 = 5,
    SystemDiag_Len = 6,//故障告警信息长度

};


enum
{
    Ka84917InitFail = 0,
    Ka84917ComFail = 1,
    Ka84917P5VAbnormal = 2,
    Ka84917N5VAbnormal = 3,
    LinkVoltAbnormal = 4,
    BatVoltAbnormal = 5,
    HallOverCurrent = 6,
    Hall5vAbnormal = 7,
    HallAdAbnormal = 8,
    ShuntSampAbnormal = 9,
    ShuntHallOverDiff = 10,
    InsuVoltInstable = 11,
    InsuVoltUnder = 12,
    Ka84922ComFail = 13,
    Rn8209ComFail = 14,
    Board5vAbnormal = 15,
};

/*故障诊断滤波状态*/
typedef enum
{
    eErr_A    = 0,                      /*正常状态*/
    eErr_B    = 1,                      /*异常状态*/
    eErr_AtoB = 2,                      /*内部使用*/
    eErr_BtoA = 3,                      /*内部使用*/
    eErr_State
}e_ErrState;

/*故障诊断类型/级别*/
typedef enum
{
    eErr_Norm   = 0,                    /*正常(不检测)*/
    eErr_Warm   = 1,                    /*仅提示类*/
    eErr_SysErr = 2,                    /*系统故障类*/
}e_ErrSort;

/*告警ID号*/
typedef enum
{
	
    eERR_ID_917_INT = 0,                /*高压采集芯片917初始化异常*/
    eERR_ID_917_COM = 1,                /*高压采集芯片917通信异常*/
    eERR_ID_917_P5V = 2,                /*高压采样芯片+5V回检异常*/
    eERR_ID_917_N5V = 3,                /*高压采样芯片-5V回检异常*/
    eERR_ID_LIN_VOL = 4,                /*LINK总压采样值异常*/
    eERR_ID_BAT_VOL = 5,                /*电池总压采样值异常*/
    eERR_ID_HAL_OVE = 6,                /*霍尔过载故障*/
    eERR_ID_HAL_SUP = 7,                /*霍尔供电异常*/
    eERR_ID_HAL_ADC = 8,                /*霍尔AD值异常*/
    eERR_ID_SHU_BSA = 9,                /*电池端分流器采样值异常*/
    eERR_ID_SHU_DIF = 10,               /*电池端分流器与霍尔采样偏差过大*/
    eERR_ID_SHU_LSA = 11,               /*负载端分流器采样值异常*/
    eERR_ID_INS_STA = 12,               /*绝缘检测电压不稳*/
    eERR_ID_INS_LOW = 13,               /*绝缘检测电压过小或负值*/
    eERR_ID_922_COM = 14,               /*菊花链芯片通信异常*/
    eERR_ID_820_COM = 15,               /*计量芯片通信异常*/
    eERR_ID_BOA_S5V = 16,               /*板子5V供电电压回检异常*/
    eERR_ID_CUR_SAM = 17,               /*电池端分流器采样自检异常*/

    eERR_ID_FINISH  = 18,           /*最大18个故障诊断(故障和告警)*/
    
}e_ERR_ID;

/*故障诊断滤波参数*/
typedef struct                          /*定义故障滤波参数结构*/
{
    e_ErrSort errSort;                  /*故障类型[0:正常(不检测),1:仅提示类，2:充电限制类,3:放电限制类,4:充放电限制类,5:充电保护类,6:放电保护类,7:充放电保护类,8:系统故障类]*/
    u16 AtoBTime;                       /*A to B的时间*/
    u16 BtoATime;                       /*B to A的时间*/
    u8 maxTimes;                        /*允许自恢复最大次数(10min内)[0:不允许自恢复,255:无限次,1-254:允许自恢复次数]*/
}t_FilPara;

/*故障诊断滤波状态*/
typedef struct                          /*定义故障滤波状态结构*/
{
    u8 enable;                          /*使能*/
    u16 timer;                          /*滤波计数器*/
    e_ErrState nowState;                /*滤波瞬时实时状态*/
    e_ErrState filState;                /*滤波稳定状态*/
    u8 resTimes;                        /*已自恢复次数(10min内)*/
}t_FilState;

extern u16 SystemDiag_57[SystemDiag_Len];
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
void InitSystemDiag(void);

/*=================================================================================================
** @Name      : void DiagErrFilterInit(void)
** @Input     : swhSort:接触器名称[0,11] state:控制接触器状态 0:断开 1:闭合
** @Output    : void
** @Function  : 故障诊断滤波初始化
** @The notes : 滤波处理数组清零
**===============================================================================================*/
void DiagErrFilterInit(void);

/*=================================================================================================
** @Name      : void DiagErrDisplayTask(void)
** @Input     : void
** @Output    : void
** @Function  : 故障显示任务
** @The notes : 100ms周期,将滤波故障转入故障显示位
**===============================================================================================*/
void DiagErrDisplayTask(void);

/*=================================================================================================
** @Name      : void DiagErrFilterTask(u8 from, u8 to)
** @Input     : from-to:检测故障滤波起始和结束ID(包括from不包to)
** @Output    : void
** @Function  : 故障诊断滤波任务
** @The notes : 50ms周期调用,与DIAG_FILTER_TICK一致
**===============================================================================================*/
void DiagErrFilterTask(e_ERR_ID from, e_ERR_ID to);

/*=================================================================================================
** @Name      : void DiagSetErrNowStateAPI(e_ERR_ID id, e_ErrState state)
** @Input     : id:故障ID state:目标故障状态
** @Output    : void
** @Function  : 设置故障诊断滤波当前瞬时状态
** @The notes :
**===============================================================================================*/
void DiagSetErrNowStateAPI(e_ERR_ID id, e_ErrState state);

/*=================================================================================================
** @Name      : void DiagSetErrFilStateAPI(e_ERR_ID id, e_ErrState state)
** @Input     : id:故障ID state:目标故障状态
** @Output    : void
** @Function  : 设置故障诊断滤波当前稳定状态
** @The notes : 只能eErr_A或eErr_B
**===============================================================================================*/
void DiagSetErrFilStateAPI(e_ERR_ID id, e_ErrState state);

/*=================================================================================================
** @Name      : e_ErrState DiagGetErrLastStateAPI(e_ERR_ID id)
** @Input     : id:故障ID
** @Output    : 当前稳定故障状态
** @Function  : 获取故障当前稳定状态
** @The notes : 不包括过渡状态
**===============================================================================================*/
e_ErrState DiagGetErrLastStateAPI(e_ERR_ID id);

/*=================================================================================================
** @Name      : e_ErrSort DiagGetGroupNowErrLevelAPI(void)
** @Input     : void
** @Output    : 当前故障级别[0,8]
** @Function  : 获取当前故障级别
** @The notes : 当前故障的最高级别
**===============================================================================================*/
e_ErrSort DiagGetGroupNowErrLevelAPI(void);

/*=================================================================================================
** @Name      : e_ErrSort DiagGetFilErrSortAPI(e_ERR_ID id)
** @Input     : id:故障ID
** @Output    : 故障类型[0,8]
** @Function  : 获取故障类型
** @The notes :
**===============================================================================================*/
e_ErrSort DiagGetFilErrSortAPI(e_ERR_ID id);


/*=================================================================================================
** @Name      : u8 GetCurrentSamplingMode(void)
** @Input     : void
** @Output    : 电流采样通道设置:0:HALL 1:分流器 2：HALL和分流器
** @Function  : 获取电流采样通道设置
** @The notes :
**===============================================================================================*/
u8 GetCurrentSamplingMode(void);


#endif