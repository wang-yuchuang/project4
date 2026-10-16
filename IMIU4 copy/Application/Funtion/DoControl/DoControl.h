/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2026-03-05 14:19:10
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2026-03-05 16:03:12
 * @FilePath: \IMIU4-toolingcheck_update4\Application\Funtion\DoControl\DoControl.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : DoControl.c
** @Founder      : WYC834
** @Date         : 
** @Function     : 外部控制函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _DOCONTROL_H
#define _DOCONTROL_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPGPIO.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
enum tag_DO_def
{
    DO_1,
    DO_2,
    DO_Max,
};

typedef struct tagControl
{
    u8 DOStateSet[DO_Max];
}Control_TypeDef;
extern Control_TypeDef gControl;//外部控制设置
/*=============================================================================================
** @Interface function definition
**===========================================================================================*/

/*=============================================================================================
** @Name      : void TASK_ControlProc (void)
** @Input     : void
** @Output    : void
** @Function  : freeRtos控制任务
** @The notes :
**===========================================================================================*/
void TASK_ControlProc (void);

/*=============================================================================================
** @Name      : void TASK_LedControl(void)
** @Input     : void
** @Output    : void
** @Function  : led定时控制任务
** @The notes :
**===========================================================================================*/
void TASK_LedControl(void);
/*=============================================================================================
** @Name      : void TASK_ALM_LED_Control(void)
** @Input     : void
** @Output    : void
** @Function  : ALM led定时控制任务
** @The notes : 1s执行一次
**===========================================================================================*/
void TASK_ALM_LED_Control(void);
/*=============================================================================================
** @Name      : void TASK_LedOn(void) 
** @Input     : void
** @Output    : void
** @Function  : led长亮
** @The notes : 
**===========================================================================================*/
void TASK_LedOn(void);
/*=============================================================================================
** @Name      : void DoSelectCtrl(u8 num ,u8 state)
** @Input     : num：控制的DO序号1-24   state：承载控制DO输出状态 0：关  1：开
** @Output    : 
** @Function  : 控制DO1-DO24
** @The notes : 用户调用
**===========================================================================================*/
void DoSelectCtrl(u8 num ,u8 state);

/*=============================================================================================
** @Name      : void CloseDOCmd(void) 
** @Input     : void
** @Output    : void
** @Function  : 关闭DO
** @The notes : 
**===========================================================================================*/
void CloseDOCmd(void);
#endif 