
/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Measure.h
** @Founder      : WYP40505
** @Date         : 2026.05.06
** @Function     : 数据采集模块
** @Instructions :
**===========================================================================================*/
#ifndef _MEASURE_H
#define _MEASURE_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#if 1
#define HALL_AD_MAX                 (4090 )            //霍尔ad值告警检测最大值(mV)
#define HALL_OVERL_RANGEL           (0    )            //霍尔过载检测最小值(mV)
#define HALL_OVERL_RANGEH           (4500 )            //霍尔过载检测最大值 (mV)
#define HALL5V_RANGEL               (4.5f )           //霍尔供电5V正常最小值(V)
#define HALL5V_RANGEH               (5.5f )           //霍尔供电5V正常最大值(V)
#define HALLVDD5V_RANGEL            (4.5f )           //霍尔vdd供电5V正常最小值(V)
#define HALLVDD5V_RANGEH            (5.5f)             //霍尔vdd供电5V正常最大值(V)

// #define HALL_V_SAMP_RANGDEL         (0.27)             //霍尔单向电压范围最小值(mV)
#define HALL_C_SAMP_RANGDEL         (0.2415)             //霍尔单向电流量程最小值(A)  1LSB

#define BAT_VOLT_RANGEL             ( -5 )        //Bat端高压告警检测(V)
#define BAT_VOLT_RANGEH             ( 1700)       //Bat端高压告警检测(V)
#define BAT_VOLT_ZERO_RANGEL        (-8.0f)       //Bat端电压开路下限(V)，低于此值视为异常不做清零
#define BAT_VOLT_ZERO_RANGEH        (0.0f)        //Bat端电压开路上限(V)，开路范围[-8, 0]内不做校准直接输出0V

// #define BATSHUNT_V_SAMP_RANGDEL     (0.05)        //Bat端分流器单向电压范围最小值(mV)
#define BATSHUNT_C_SAMP_RANGDEL     (0.0407)        //Bat端分流器单向电流量程最小值(A) 1LSB

#define BATSHUNT_SELF_TEST         (5000)         //Bat端分流器采样自检异常告警检测(mA)
#define BATSHUNT_SAMP_MIN          (-75)          //Bat端分流器采样值异常告警检测最小值(mV)
#define BATSHUNT_SAMP_MAX          (75)           //Bat端分流器采样值异常告警检测最大值(mV)

#define BATSHUNT_HALL_DIFF         (10000)        //电池端分流器与霍尔采样偏差(mA)

#define BATSHUNT_5V_RANGEL         (4.5f )           //电池端分流器供电5V正常最小值(V)
#define BATSHUNT_5V_RANGEH         (5.5f )           //电池端分流器供电5V正常最大值(V)

#define LINK_VOLT_RANGEL            (-10)           //Link端高压告警检测最小值
#define LINK_VOLT_RANGEH            (1700)          //Link端高压告警检测最大值
#define LINK_VOLT_ZERO_RANGEL       (-8.0f)       //Link端电压开路下限(V)，低于此值视为异常不做清零
#define LINK_VOLT_ZERO_RANGEH       (0.0f)        //Link端电压开路上限(V)，开路范围[-8, 0]内不做校准直接输出0V


#define LINKSHUNT_SAMP_MIN          (-75)          //Link端分流器采样值异常告警检测最小值(mV)
#define LINKSHUNT_SAMP_MAX          (75)           //Link端分流器采样值异常告警检测最大值(mV)

// #define LINKSHUNT_V_SAMP_RANGDEL     (0.05)           //Link端分流器单向电压范围最小值(mV)
#define LINKSHUNT_C_SAMP_RANGDEL     (0.0407)        // 1LSB
 //Link端分流器单向电流量程最小值(A)

#define PE_VOLT_ZERO_RANGE          (1.0f)        
//PE对地电压开路检测阈值(V)，绝对值在此范围内不做校准直接输出0V
#endif

/*=============================================================================================
** @Data type definition
**===========================================================================================*/

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void KA84917_Rn8209_MeasureTask(void)
** @Input     : void
** @Output    : void
** @Function  : 84917-85922采样与计算任务
** @The notes :
**===========================================================================================*/
void KA84917_Rn8209_MeasureTask(void); 

/*==========================================================================
** @Name      : void CalcFast3Results(void)
** @Function  : 仅计算快路径3个分流器电流 (BatShuntCurrent1/2, LinkShuntCurrent)
**========================================================================*/
void CalcFast3Results(void);

/*=============================================================================================
** @Name      : void TASK_MeasureProcTask (void)
** @Input     : void
** @Output    : void
** @Function  : freeRtos采样任务
** @The notes :
**===========================================================================================*/
void TASK_MeasureProcTask (void);

/*=============================================================================================
** @Name      : void GetSystemInfoTask (void)
** @Input     : void
** @Output    : void
** @Function  : 系统信息获取任务
** @The notes :
**===========================================================================================*/
void GetSystemInfoTask(void);

/*=============================================================================================
** @Name      : void RefreshDI(void)
** @Input     : void
** @Output    : void
** @Function  : 读取DI1
** @The notes : 用户调用
**===========================================================================================*/
void RefreshDI(void);

/*=============================================================================================
** @Name      : void RefreshHall_Pfault(void)
** @Input     : void
** @Output    : void
** @Function  : 读取HALL_PFAULT
** @The notes : 用户调用
**===========================================================================================*/
void RefreshHall_Pfault(void);
/*=============================================================================================
** @Name      : void TempADSample(void)
** @Input     : void
** @Output    : void
** @Function  : 温度计算任务
** @The notes :
**===========================================================================================*/
void TempCalculate(void);

/*=============================================================================================
** @Name      : void HallCalculate(void)
** @Input     : void
** @Output    : void
** @Function  : 计算霍尔电流
** @The notes : 用户调用
**===========================================================================================*/
void HallCalculate(void);

/*=============================================================================================
** @Name      : void Ka84917VoltCurrentCalculate(void)
** @Input     : void
** @Output    : void
** @Function  : KA85917相关电压以及电流计算任务
** @The notes :
**===========================================================================================*/
void Ka84917VoltCurrentCalculate(void);

void Rn8209VoltCurrentCalculate(void);
#endif