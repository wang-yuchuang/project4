

/*
  *****************************************************************************
  *
  * Name:bsp_timer_hal.h
  * Function: 	         
  * Version: 					                     
  * Update:					                           
  * Date: 			                       
  * Designer:                             			
  * Modified Date:                            	
  * Modified Engineer:				                  
  *
  *****************************************************************************
*/

#ifndef __BSP_TIMER_HAL_H
#define __BSP_TIMER_HAL_H

// ================* Include Files ************************
#include "includes.h"


// ================* Definitions and Marcos ***************

#define M530S_GEN_TIMER                 (TIM6)
#define M530S_GEN_TIMER_CLK_Enable      __TIM6_CLK_ENABLE
#define M530S_GEN_TIMER_CLK_Disable     __TIM6_CLK_DISABLE
#define M530S_GEN_TIMER_PERIOD_uS       (500)
#define M530S_SYS_FREQ_MHZ              (168)

// We have to calculate Period and Prescaler to generate periodic interrupt
// Both should have values less than 65536

#define M530S_TIM_COUNT                 (100)
#define M530S_PRESCALER                 ((M530S_SYS_FREQ_MHZ * M530S_GEN_TIMER_PERIOD_uS)/M530S_TIM_COUNT)

#define M530S_GEN_TIMER_IRQ             (TIM6_DAC_IRQn)
#define M530S_GEN_TIMER_ENABLE()        TIM_Cmd(M530S_GEN_TIMER, ENABLE)

//  General Software Timers
typedef enum 
{
  FIRST_GEN_TIMER,
  MB_PKT_SEAL_GEN_TIMERA = FIRST_GEN_TIMER,
  MB_INTER_PKT_GEN_TIMERA,
  MB_TX_COMPLETE_TIMERA,
  MB_PKT_SEAL_GEN_TIMERB,
  MB_INTER_PKT_GEN_TIMERB,
  MB_TX_COMPLETE_TIMERB,
  LAST_GEN_TIMER,
  M530S_NUM_GEN_TIMERS= LAST_GEN_TIMER
}M530S_Gen_Timer_e;

// ================* Variables ****************************
extern uint16_t m530s_GenTimerCount[M530S_NUM_GEN_TIMERS];

#define bsp_GenTimer_Load(Timer, TimeCount) (m530s_GenTimerCount[(Timer)] = (TimeCount))
//#define bsp_GenTimer_IsExpired((Timer))                 (0 == m530s_GenTimerCount[(Timer)])
// ================* Function Prototypes ******************
void bsp_InitGenTimer(void);
void bsp_GenTimer_Enable(void);
void bsp_GenTimer_Disable(void);
void bsp_Gen_Timer_ISR(void);
void bsp_CfgGenTimerISR(void);

#endif // __BSP_TIMER_H
// End of file
