
/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPTIM.c
** @Founder      : WYC834
** @Date         : 2025.10.11
** @Function     : Timer定时器驱动函数模块
** @Instructions : 需要用到us级延时，rtos只提供ms级，故这里用定时器阻塞实现us级延时
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPConfig.h"
#include "BSPTypeDef.h"
#include "BSPTIM.h"
#include "tim.h"
#include "BSPIRQ.h"
// #include "main.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern TIM_HandleTypeDef htim6;
extern volatile u_BSPFunCfg gBSPFunCfg;
extern unsigned long int RunTimers; 
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Name      : u8 BSPTIMInit(e_TIMNum chanel)
** @Input     : chanel:TIMx定时器号 
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : tim驱动模块初始化
** @The notes : BSP调用
**===========================================================================================*/
u8 BSPTIMInit(e_TIMNum chanel)
{
    switch(chanel)
    {
        /*tim6初始化，实现us级计数, 移到KA84917_maintask*/
        case eTIM6:
            break;	
        /*tim7初始化，实现秒级级计数*/
        case eTIM7:
            MX_TIM7_Init();
            HAL_TIM_Base_Start_IT(&htim7);
            HAL_TIM_Base_Start(&htim7);
            break;	
        default:
            break;
    }
    return(TRUE);
}


// void delay_us_tim(u32 nus)
// {
//     u32 ticks = nus - 1;
//     u32 told,tnow,tcnt = 0;	    	
//     told = htim6.Instance->CNT;                 //刚进入时的计数器值
//     while(1)
//     {
//         tnow = htim6.Instance->CNT;
//         if(tnow != told)
//         {
//             if(tnow > told) 
//                 tcnt += tnow - told;            //递增计数器
//             else
//                 tcnt += 0xFFFF - told + tnow;	    
//             told = tnow;
//             if(tcnt >= ticks)
//                 break;                          //时间超过/等于要延迟的时间,则退出.
//         }  
//     }
  
// }



u32 GetRunTimers(void)
{
   u32 val;
   BSP_DISABLE_IRQ();
   val = RunTimers;
   BSP_ENABLE_IRQ();
   return val;
}



/*=============================================================================================
** @Static function definition
**===========================================================================================*/

