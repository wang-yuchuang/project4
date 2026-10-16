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
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "DoControl.h"
#include "BSPDIDO.h"
#include "SystemData.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "SystemInfo.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
Control_TypeDef gControl;//外部控制设置
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

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
void TASK_ControlProc (void)
{   
    if(SystemWorkState_50[WorkMode]!= eMODE_Tooling)
    {       
        DoSelectCtrl(DO_1,gControl.DOStateSet[DO_1]);
        DoSelectCtrl(DO_2,gControl.DOStateSet[DO_2]);
    }
}
/*=============================================================================================
** @Name      : void DoSelectCtrl(u8 num ,u8 state)
** @Input     : num：控制的DO序号1-2   state：承载控制DO输出状态 0：断开继电器  1：闭合继电器
** @Output    : 
** @Function  : 控制DO1-DO24
** @The notes : 用户调用
**===========================================================================================*/
void DoSelectCtrl(u8 num ,u8 state)
{
    if(num > 2 || state > 1)
    {
        return;
    }
    if(num == DO_1)
    {
        if(state)
        {
            CRELAY1_Contr_Close();
        }else
        {
            CRELAY1_Contr_Open();
        } 
    }else if(num == DO_2)
    {
        if(state)
        {
            CRELAY2_Contr_Close();
        }else
        {
            CRELAY2_Contr_Open();
        }   
    }
}
/*=============================================================================================
** @Name      : void TASK_LedControl(void)
** @Input     : void
** @Output    : void
** @Function  : led定时控制任务
** @The notes : 1s执行一次
**===========================================================================================*/
void TASK_LedControl(void)
{
    static u8 sStep = 0;

    if(0 == sStep)     
    {
        sStep = 1;
        LED_RUN_ON();
        BitSet16(SystemData_51[DidoGpioState],RunLed);//记录状态
        
    }
    else
    {
        sStep = 0;
        LED_RUN_OFF();
        BitClr16(SystemData_51[DidoGpioState],RunLed);//记录状态
    }
}

/*=============================================================================================
** @Name      : void TASK_ALM_LED_Control(void)
** @Input     : void
** @Output    : void
** @Function  : ALM led定时控制任务
** @The notes : 1s执行一次
**===========================================================================================*/
void TASK_ALM_LED_Control(void)
{
    static u8 sStep = 0;

    if(0 == sStep)     
    {
        sStep = 1;
        IMD_ALM_ON();
        BitSet16(SystemData_51[DidoGpioState],Almled);//记录状态
        
    }
    else
    {
        sStep = 0;
        IMD_ALM_OFF();
        BitClr16(SystemData_51[DidoGpioState],Almled);//记录状态
    }
}

/*=============================================================================================
** @Name      : void TASK_LedOn(void) 
** @Input     : void
** @Output    : void
** @Function  : led长亮
** @The notes : 
**===========================================================================================*/
void TASK_LedOn(void)  
{
    LED_RUN_ON();
}

/*=============================================================================================
** @Name      : void CloseDOCmd(void) 
** @Input     : void
** @Output    : void
** @Function  : 关闭DO
** @The notes : 
**===========================================================================================*/
void CloseDOCmd(void)
{
    //控制关闭继电器
        DoSelectCtrl(0,1);
        DoSelectCtrl(1,1);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/