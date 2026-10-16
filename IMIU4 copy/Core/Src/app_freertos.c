/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OSUserTasks.h"
#include "BSPTypeDef.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STACK_MONITOR_ENABLE  1  /* 设为1启用堆栈水面监控日志 */
static unsigned long TaskMark1[6];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId myTask01Handle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osThreadId myTask04Handle;
osThreadId myTask05Handle;
osThreadId myTask06Handle;
osTimerId myTimer1msHandle;
osTimerId myTimer5msHandle;
osTimerId myTimer10ms01Handle;
osTimerId myTimer10ms02Handle;
osTimerId myTimer10ms03Handle;
osTimerId myTimer50ms01Handle;
osTimerId myTimer50ms02Handle;
osTimerId myTimer50ms03Handle;
osTimerId myTimer100ms01Handle;
osTimerId myTimer100ms02Handle;
osTimerId myTimer100ms03Handle;
osTimerId myTimer500ms01Handle;
osTimerId myTimer500ms02Handle;
osTimerId myTimer500ms03Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTask01(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void StartTask05(void const * argument);
void StartTask06(void const * argument);
void TimerTask1ms(void const * argument);
void TimerTask5ms(void const * argument);
void TimerTask10ms01(void const * argument);
void TimerTask10ms02(void const * argument);
void TimerTask10ms03(void const * argument);
void TimerTask50ms01(void const * argument);
void TimerTask50ms02(void const * argument);
void TimerTask50ms03(void const * argument);
void TimerTask100ms01(void const * argument);
void TimerTask100ms02(void const * argument);
void TimerTask100ms03(void const * argument);
void TimerTask500ms01(void const * argument);
void TimerTask500ms02(void const * argument);
void TimerTask500ms03(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void  MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer3ms */
  osTimerDef(myTimer1ms, TimerTask1ms);
  myTimer1msHandle = osTimerCreate(osTimer(myTimer1ms), osTimerPeriodic, NULL);

  /* definition and creation of myTimer5ms */
  osTimerDef(myTimer5ms, TimerTask5ms);
  myTimer5msHandle = osTimerCreate(osTimer(myTimer5ms), osTimerPeriodic, NULL);

  /* definition and creation of myTimer10ms01 */
  osTimerDef(myTimer10ms01, TimerTask10ms01);
  myTimer10ms01Handle = osTimerCreate(osTimer(myTimer10ms01), osTimerPeriodic, NULL);

  /* definition and creation of myTimer10ms02 */
  osTimerDef(myTimer10ms02, TimerTask10ms02);
  myTimer10ms02Handle = osTimerCreate(osTimer(myTimer10ms02), osTimerPeriodic, NULL);

  /* definition and creation of myTimer10ms03 */
  osTimerDef(myTimer10ms03, TimerTask10ms03);
  myTimer10ms03Handle = osTimerCreate(osTimer(myTimer10ms03), osTimerPeriodic, NULL);

  /* definition and creation of myTimer50ms01 */
  osTimerDef(myTimer50ms01, TimerTask50ms01);
  myTimer50ms01Handle = osTimerCreate(osTimer(myTimer50ms01), osTimerPeriodic, NULL);

  /* definition and creation of myTimer50ms02 */
  osTimerDef(myTimer50ms02, TimerTask50ms02);
  myTimer50ms02Handle = osTimerCreate(osTimer(myTimer50ms02), osTimerPeriodic, NULL);

  /* definition and creation of myTimer50ms03 */
  osTimerDef(myTimer50ms03, TimerTask50ms03);
  myTimer50ms03Handle = osTimerCreate(osTimer(myTimer50ms03), osTimerPeriodic, NULL);

  /* definition and creation of myTimer100ms01 */
  osTimerDef(myTimer100ms01, TimerTask100ms01);
  myTimer100ms01Handle = osTimerCreate(osTimer(myTimer100ms01), osTimerPeriodic, NULL);

  /* definition and creation of myTimer100ms02 */
  osTimerDef(myTimer100ms02, TimerTask100ms02);
  myTimer100ms02Handle = osTimerCreate(osTimer(myTimer100ms02), osTimerPeriodic, NULL);

  /* definition and creation of myTimer100ms03 */
  osTimerDef(myTimer100ms03, TimerTask100ms03);
  myTimer100ms03Handle = osTimerCreate(osTimer(myTimer100ms03), osTimerPeriodic, NULL);

  /* definition and creation of myTimer500ms01 */
  osTimerDef(myTimer500ms01, TimerTask500ms01);
  myTimer500ms01Handle = osTimerCreate(osTimer(myTimer500ms01), osTimerPeriodic, NULL);

  /* definition and creation of myTimer500ms02 */
  osTimerDef(myTimer500ms02, TimerTask500ms02);
  myTimer500ms02Handle = osTimerCreate(osTimer(myTimer500ms02), osTimerPeriodic, NULL);

  /* definition and creation of myTimer500ms03 */
  osTimerDef(myTimer500ms03, TimerTask500ms03);
  myTimer500ms03Handle = osTimerCreate(osTimer(myTimer500ms03), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  osTimerStart(myTimer1msHandle,1);
  osTimerStart(myTimer5msHandle,5);
  osTimerStart(myTimer10ms01Handle,10);
  osTimerStart(myTimer10ms02Handle,10);
  osTimerStart(myTimer10ms03Handle,10);
  osTimerStart(myTimer50ms01Handle,50);
  osTimerStart(myTimer50ms02Handle,50);
  osTimerStart(myTimer50ms03Handle,50);
  osTimerStart(myTimer100ms01Handle,100);
  osTimerStart(myTimer100ms02Handle,100);
  osTimerStart(myTimer100ms03Handle,100);
  osTimerStart(myTimer500ms01Handle,500);
  osTimerStart(myTimer500ms02Handle,500);
  osTimerStart(myTimer500ms03Handle,500);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of myTask01 */
  osThreadDef(myTask01, StartTask01, osPriorityRealtime, 0, 128*8);
  myTask01Handle = osThreadCreate(osThread(myTask01), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityHigh, 0, 128*16);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityHigh, 0, 128*8);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* definition and creation of myTask04 */
  osThreadDef(myTask04, StartTask04, osPriorityNormal, 0, 128*24);
  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

  /* definition and creation of myTask05 */
  osThreadDef(myTask05, StartTask05, osPriorityBelowNormal, 0, 128*8);
  myTask05Handle = osThreadCreate(osThread(myTask05), NULL);

  /* definition and creation of myTask06 */
  osThreadDef(myTask06, StartTask06, osPriorityLow, 0, 128*24);
  myTask06Handle = osThreadCreate(osThread(myTask06), NULL);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartTask01 */
/**
  * @brief  Function implementing the myTask01 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask01 */
void StartTask01(void const * argument)
{
  // TickType_t start_tick, end_tick;



  /* USER CODE BEGIN StartTask01 */
  /* Infinite loop */
  for(;;)
  {
    // start_tick = xTaskGetTickCount();
    
    PriorityRealtimeTask();

    // end_tick = xTaskGetTickCount();

    // SEGGER_RTT_printf(0,"StartTask01 start_tick (%08x)\r\n",start_tick);
    // SEGGER_RTT_printf(0,"StartTask01 end_tick (%08x)\r\n",end_tick);


    // TaskMark1[0] = uxTaskGetStackHighWaterMark(NULL);
    //  SEGGER_RTT_printf(0,"StartTask01 TaskMark1[0] (%d)\r\n",TaskMark1[0]);
    //  if (TaskMark1[0]<200)
    //  {
    //   SEGGER_RTT_printf(0," StartTask01 StackHighWater (%d)\r\n",TaskMark1[0]);
    //  }
#if STACK_MONITOR_ENABLE
    TaskMark1[0] = uxTaskGetStackHighWaterMark(NULL);
    if (TaskMark1[0] < 512)
    {
      SEGGER_RTT_printf(0, "WARN: StartTask01 StackLow (%lu)\r\n", TaskMark1[0]);
    }
#endif
     
    osDelay(1);
  }
  /* USER CODE END StartTask01 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
#if 0  /* === 原始StartTask02（保留备份）=== */
void StartTask02(void const * argument)
{


  /* USER CODE BEGIN StartTask02 */
  SEGGER_RTT_printf(0, "T02: watermark=%d\n", (int)uxTaskGetStackHighWaterMark(NULL));
  TickType_t start_tick, end_tick;

  /* Infinite loop */
  for(;;)
  {
    PriorityHighTask();

    TaskMark1[1] = uxTaskGetStackHighWaterMark(NULL);
#if STACK_MONITOR_ENABLE
    if (TaskMark1[1] < 512)
    {
      SEGGER_RTT_printf(0, "WARN: StartTask02 StackLow (%lu)\r\n", TaskMark1[1]);
    }
#endif
     osDelay(3);
  }
  /* USER CODE END StartTask02 */
}
#endif  /* === 原始StartTask02结束 === */

/* NEW: 使用vTaskDelayUntil实现精准10ms定周期采样 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  SEGGER_RTT_printf(0, "T02: watermark=%d\n", (int)uxTaskGetStackHighWaterMark(NULL));
  static int t02_loop_cnt = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  /* Infinite loop */
  for(;;)
  {
    PriorityHighTask();
    if (++t02_loop_cnt <= 3) SEGGER_RTT_printf(0, "T02 loop #%d\n", t02_loop_cnt);

    TaskMark1[1] = uxTaskGetStackHighWaterMark(NULL);
#if STACK_MONITOR_ENABLE
    if (TaskMark1[1] < 512)
    {
      SEGGER_RTT_printf(0, "WARN: StartTask02 StackLow (%lu)\r\n", TaskMark1[1]);
    }
#endif
    /* 精确10ms周期: 快路径(6ms SPI) + Calc3Fast(<1ms) + 剩余CPU时间归其他任务 */
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20));
    //osDelay(3);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  // TickType_t start_tick, end_tick;


  /* Infinite loop */
  for(;;)
  {
    // start_tick = xTaskGetTickCount();

    PriorityAboveNormalTask();

    // end_tick = xTaskGetTickCount();

    // SEGGER_RTT_printf(0,"StartTask03 start_tick (%08x)\r\n",start_tick);
    //   SEGGER_RTT_printf(0,"StartTask03 end_tick (%08x)\r\n",end_tick);

    TaskMark1[2] = uxTaskGetStackHighWaterMark(NULL);
    
    // SEGGER_RTT_printf(0,"StartTask03 TaskMark1[2]  (%d)\r\n",TaskMark1[2]);
    // if (TaskMark1[2]<200)
    // {
    //  SEGGER_RTT_printf(0," StartTask03 StackHighWater (%d)\r\n",TaskMark1[2]);
    // }
    osDelay(1);
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
  SEGGER_RTT_printf(0, "T04: watermark=%d\n", (int)uxTaskGetStackHighWaterMark(NULL));
  static int t04_loop_cnt = 0;
  // TickType_t start_tick, end_tick;

  /* Infinite loop */
  for(;;)
  {
    // start_tick = xTaskGetTickCount();
    PriorityNormalTask1(); 
    if (++t04_loop_cnt <= 3) SEGGER_RTT_printf(0, "T04 loop #%d\n", t04_loop_cnt);

    // end_tick = xTaskGetTickCount();

    // SEGGER_RTT_printf(0,"StartTask04 start_tick (%08x)\r\n",start_tick);
    // SEGGER_RTT_printf(0,"StartTask04 end_tick (%08x)\r\n",end_tick);


    TaskMark1[3] = uxTaskGetStackHighWaterMark(NULL);
    // SEGGER_RTT_printf(0,"StartTask04 TaskMark1[3]  (%d)\r\n",TaskMark1[3]);
    // if (TaskMark1[3]<200)
    // {
    //  SEGGER_RTT_printf(0," StartTask04 StackHighWater (%d)\r\n",TaskMark1[3]);
    // }
#if STACK_MONITOR_ENABLE
    if (TaskMark1[3] < 512)
    {
      SEGGER_RTT_printf(0, "WARN: StartTask04 StackLow (%lu)\r\n", TaskMark1[3]);
    }
#endif
      osDelay(5);
  }
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the myTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void const * argument)
{
  /* USER CODE BEGIN StartTask05 */
  // TickType_t start_tick, end_tick;



  /* Infinite loop */

  for(;;)
  {
      // start_tick = xTaskGetTickCount();
      PriorityNormalTask2();
      // end_tick = xTaskGetTickCount();

      // SEGGER_RTT_printf(0,"StartTask05 start_tick (%08x)\r\n",start_tick);
      // SEGGER_RTT_printf(0,"StartTask05 end_tick (%08x)\r\n",end_tick);

      TaskMark1[4] = uxTaskGetStackHighWaterMark(NULL);
      // SEGGER_RTT_printf(0,"StartTask05 TaskMark1[4]  (%d)\r\n",TaskMark1[4]);
      // if (TaskMark1[4]<200)
      // {
      //  SEGGER_RTT_printf(0," StartTask05 StackHighWater (%d)\r\n",TaskMark1[4]);
      // }
      osDelay(5);
  }
  /* USER CODE END StartTask05 */
}

/* USER CODE BEGIN Header_StartTask06 */
/**
* @brief Function implementing the myTask06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask06 */
void StartTask06(void const * argument)
{
  /* USER CODE BEGIN StartTask06 */
  // TickType_t start_tick, end_tick;



  /* Infinite loop */
  for(;;)
  {
    // start_tick = xTaskGetTickCount();

    PriorityBelowNormalTask();

    // end_tick = xTaskGetTickCount();

    //  SEGGER_RTT_printf(0,"StartTask06 start_tick (%08x)\r\n",start_tick);
    //   SEGGER_RTT_printf(0,"StartTask06 end_tick (%08x)\r\n",end_tick);
  
    TaskMark1[5] = uxTaskGetStackHighWaterMark(NULL);
    // SEGGER_RTT_printf(0,"StartTask06 TaskMark1[5]  (%d)\r\n",TaskMark1[5]);
    // if (TaskMark1[5]<200)
    //   {
    //    SEGGER_RTT_printf(0," StartTask06 StackHighWater (%d)\r\n",TaskMark1[5]);
    //   }
    osDelay(20);
  }
  /* USER CODE END StartTask06 */
}


/* TimerTask1ms function */
void TimerTask1ms(void const * argument)
{
  /* USER CODE BEGIN TimerTask1ms */
    Task1ms();
  /* USER CODE END TimerTask1ms */
}

/* TimerTask5ms function */
void TimerTask5ms(void const * argument)
{
  /* USER CODE BEGIN TimerTask5ms */
  Task5ms();
  /* USER CODE END TimerTask5ms */
}

/* TimerTask10ms01 function */
void TimerTask10ms01(void const * argument)
{
  /* USER CODE BEGIN TimerTask10ms01 */
    static u8 state = 0;
    Task10ms();                         
    
    switch (state)
    {
        default:
            state = 0;
                       
            
        case 0:
            Task50msBy10ms_1();   
            Task100msBy10ms_1();
            break;
            
        case 1:
            Task50msBy10ms_2();
            Task100msBy10ms_2();
            break;
            
        case 2:
            Task50msBy10ms_3();
            Task100msBy10ms_3();
            break;
            
        case 3:
            Task50msBy10ms_4();   
            Task100msBy10ms_4();
            break;
            
        case 4:
            Task50msBy10ms_5();
            Task100msBy10ms_5(); 
            break;
            
        case 5:
            Task50msBy10ms_1();
            Task100msBy10ms_6();
            break;
            
        case 6:
            Task50msBy10ms_2();
            Task100msBy10ms_7();
            break;
            
        case 7:
            Task50msBy10ms_3();
            Task100msBy10ms_8(); 
            
        case 8:
            Task50msBy10ms_4();
            Task100msBy10ms_9(); 
            break;
            
        case 9:
            Task50msBy10ms_5();
            break;
    }
    
    state ++;
    if(state >= 10)
    {
        state = 0;
    }
  /* USER CODE END TimerTask10ms01 */
}

/* TimerTask10ms02 function */
void TimerTask10ms02(void const * argument)
{
  /* USER CODE BEGIN TimerTask10ms02 */

  /* USER CODE END TimerTask10ms02 */
}

/* TimerTask10ms03 function */
void TimerTask10ms03(void const * argument)
{
  /* USER CODE BEGIN TimerTask10ms03 */

  /* USER CODE END TimerTask10ms03 */
}

/* TimerTask50ms01 function */
void TimerTask50ms01(void const * argument)
{
  /* USER CODE BEGIN TimerTask50ms01 */

  /* USER CODE END TimerTask50ms01 */
}

/* TimerTask50ms02 function */
void TimerTask50ms02(void const * argument)
{
  /* USER CODE BEGIN TimerTask50ms02 */

  /* USER CODE END TimerTask50ms02 */
}

/* TimerTask50ms03 function */
void TimerTask50ms03(void const * argument)
{
  /* USER CODE BEGIN TimerTask50ms03 */

  /* USER CODE END TimerTask50ms03 */
}

/* TimerTask100ms01 function */
void TimerTask100ms01(void const * argument)
{
  /* USER CODE BEGIN TimerTask100ms01 */
    static u8 state  = 0;
    static u8 state4 = 0;               
    Task100ms();                                       
    switch(state4)                      
    {
        default:
            state4 = 0;
                        
            
        case 0:
            Task400msBy100ms_1();  
            break;
            
        case 1:
            Task400msBy100ms_2();
            break;
            
        case 2:
            Task400msBy100ms_3();
            break;
            
        case 3:
            Task400msBy100ms_4();  
            break;
    }
    
    state4++;
    if(state4 >= 4)
    {
        state4 = 0;
    }
    
    switch(state)                       
    {
        default:
            state = 0;
                       
            
        case 0:
            Task1sBy100ms_1();
            break;
            
        case 1:
            Task1sBy100ms_2();
            break;
            
        case 2:
            Task1sBy100ms_3();
            break;
            
        case 3:
            Task1sBy100ms_4();
            break;
            
        case 4:
            Task1sBy100ms_5();
            break;
            
        case 5:
            Task1sBy100ms_6();
            break;
            
        case 6:
            Task1sBy100ms_7();
            break;
            
        case 7:
            Task1sBy100ms_8();
            break;
            
        case 8:
            Task1sBy100ms_9();
            break;
            
        case 9:
            Task1sBy100ms_10();
            break;
    }
    
    state++;
    if(state >= 10)
    {
        state = 0;
    }
  /* USER CODE END TimerTask100ms01 */
}

/* TimerTask100ms02 function */
void TimerTask100ms02(void const * argument)
{
  /* USER CODE BEGIN TimerTask100ms02 */

  /* USER CODE END TimerTask100ms02 */
}

/* TimerTask100ms03 function */
void TimerTask100ms03(void const * argument)
{
  /* USER CODE BEGIN TimerTask100ms03 */

  /* USER CODE END TimerTask100ms03 */
}

/* TimerTask500ms01 function */
void TimerTask500ms01(void const * argument)
{
  /* USER CODE BEGIN TimerTask500ms01 */
    static u8 state = 0;
    
    Task500ms();                        
    
    switch(state)                       
    {
        default:
            state = 0;
                    
            
        case 0:
            Task1sBy500ms_1();
            Task2sBy500ms_1();
            break;
            
        case 1:
            Task1sBy500ms_2();
            Task2sBy500ms_2();
            break;
            
        case 2:
            Task1sBy500ms_1();
            Task2sBy500ms_3();
            break;
            
        case 3:
            Task1sBy500ms_2();
            Task2sBy500ms_4();
            break;
    }
    
    state ++;
    if(state >= 4)
    {
        state = 0;
    }
  /* USER CODE END TimerTask500ms01 */
}

/* TimerTask500ms02 function */
void TimerTask500ms02(void const * argument)
{
  /* USER CODE BEGIN TimerTask500ms02 */

  /* USER CODE END TimerTask500ms02 */
}

/* TimerTask500ms03 function */
void TimerTask500ms03(void const * argument)
{
  /* USER CODE BEGIN TimerTask500ms03 */

  /* USER CODE END TimerTask500ms03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  (void)xTask;
  SEGGER_RTT_printf(0, "STACK OVERFLOW: %s\r\n", pcTaskName);
  taskDISABLE_INTERRUPTS();
  for(;;);
}

void vApplicationMallocFailedHook(void)
{
  SEGGER_RTT_printf(0, "MALLOC FAILED\r\n");
  taskDISABLE_INTERRUPTS();
  for(;;);
}
/* USER CODE END Application */

