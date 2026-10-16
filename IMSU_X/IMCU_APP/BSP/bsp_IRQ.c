
#define  BSP_INT_MODULE
#include <includes.h>
#include "main.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_gpio.h"

#define  BSP_INT_SRC_NBR                                 82

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR];

extern UART_HandleTypeDef   Uart1Handle;
extern UART_HandleTypeDef   Uart2Handle;
extern UART_HandleTypeDef   Uart4Handle;
extern UART_HandleTypeDef   Uart5Handle;
extern UART_HandleTypeDef   Uart6Handle;

extern CAN_HandleTypeDef    Can_1_Handle;
extern TIM_HandleTypeDef    Tim2Handle;
extern TIM_HandleTypeDef    Tim3CaptureHandle;
extern TIM_HandleTypeDef    Tim9Handle;
extern TIM_HandleTypeDef    Tim5Handle;
extern TIM_HandleTypeDef    Tim10Handle;
extern TIM_HandleTypeDef    Tim11Handle;
extern TIM_HandleTypeDef    Tim14Handle;
extern ADC_HandleTypeDef    AdcHandle;
extern ADC_HandleTypeDef    ADC_3_Handle;
extern HCD_HandleTypeDef    hhcd;

extern TIM_HandleTypeDef      Tim7Handle;

static  void  BSP_IntHandler     (CPU_DATA  int_id);
static  void  BSP_IntHandlerDummy(void);


/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcDis(int_id + 16u);
    }
}

void  BSP_IrqDisbl(void)
{
	__disable_irq();
}

void  BSP_IrqEnable(void)
{
	__enable_irq();
}

/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) 
    {
        CPU_IntSrcEn(int_id + 16u);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (CPU_DATA       int_id,
                      CPU_FNCT_VOID  isr)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}

//清空所有中断
void  BSP_IntInit (void)
{
    CPU_DATA  int_id;

    for (int_id = 0u; int_id < BSP_INT_SRC_NBR; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}

void  BSP_IntHandlerWWDG               (void)  { BSP_IntHandler(BSP_INT_ID_WWDG);                }
void  BSP_IntHandlerPVD                (void)  { BSP_IntHandler(BSP_INT_ID_PVD);                 }
void  BSP_IntHandlerTAMP_STAMP         (void)  { BSP_IntHandler(BSP_INT_ID_TAMP_STAMP);          }
void  BSP_IntHandlerRTC_WKUP           (void)  { BSP_IntHandler(BSP_INT_ID_RTC_WKUP);            }
void  BSP_IntHandlerFLASH              (void)  { BSP_IntHandler(BSP_INT_ID_FLASH);               }
void  BSP_IntHandlerRCC                (void)  { BSP_IntHandler(BSP_INT_ID_RCC);                 }
void  BSP_IntHandlerEXTI0              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI0);               }
void  BSP_IntHandlerEXTI1              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI1);               }
void  BSP_IntHandlerEXTI2              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI2);               }
void  BSP_IntHandlerEXTI3              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI3);               }
void  BSP_IntHandlerEXTI4              (void)  { BSP_IntHandler(BSP_INT_ID_EXTI4);               }
void  BSP_IntHandlerDMA1_CH0           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH0);            }
void  BSP_IntHandlerDMA1_CH1           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH1);            }
void  BSP_IntHandlerDMA1_CH2           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH2);            }
void  BSP_IntHandlerDMA1_CH3           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH3);            }
void  BSP_IntHandlerDMA1_CH4           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH4);            }
void  BSP_IntHandlerDMA1_CH5           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH5);            }
void  BSP_IntHandlerDMA1_CH6           (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_CH6);            }
void  BSP_IntHandlerADC                (void)  { BSP_IntHandler(BSP_INT_ID_ADC);                 }
void  BSP_IntHandlerCAN1_TX            (void)  { BSP_IntHandler(BSP_INT_ID_CAN1_TX);             }
void  BSP_IntHandlerCAN1_RX0           (void)  { BSP_IntHandler(BSP_INT_ID_CAN1_RX0);            }
void  BSP_IntHandlerCAN1_RX1           (void)  { BSP_IntHandler(BSP_INT_ID_CAN1_RX1);            }
void  BSP_IntHandlerCAN1_SCE           (void)  { BSP_IntHandler(BSP_INT_ID_CAN1_SCE);            }
void  BSP_IntHandlerEXTI9_5            (void)  { BSP_IntHandler(BSP_INT_ID_EXTI9_5);             }
void  BSP_IntHandlerTIM1_BRK_TIM9      (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_BRK_TIM9);       }
void  BSP_IntHandlerTIM1_UP_TIM10      (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_UP_TIM10);       }
void  BSP_IntHandlerTIM1_TRG_COM_TIM11 (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_TRG_COM_TIM11);  }
void  BSP_IntHandlerTIM1_CC            (void)  { BSP_IntHandler(BSP_INT_ID_TIM1_CC);             }
void  BSP_IntHandlerTIM2               (void)  { BSP_IntHandler(BSP_INT_ID_TIM2);                }
void  BSP_IntHandlerTIM3               (void)  { BSP_IntHandler(BSP_INT_ID_TIM3);                }
void  BSP_IntHandlerTIM4               (void)  { BSP_IntHandler(BSP_INT_ID_TIM4);                }
void  BSP_IntHandlerI2C1_EV            (void)  { BSP_IntHandler(BSP_INT_ID_I2C1_EV);             }
void  BSP_IntHandlerI2C1_ER            (void)  { BSP_IntHandler(BSP_INT_ID_I2C1_ER);             }
void  BSP_IntHandlerI2C2_EV            (void)  { BSP_IntHandler(BSP_INT_ID_I2C2_EV);             }
void  BSP_IntHandlerI2C2_ER            (void)  { BSP_IntHandler(BSP_INT_ID_I2C2_ER);             }
void  BSP_IntHandlerSPI1               (void)  { BSP_IntHandler(BSP_INT_ID_SPI1);                }
void  BSP_IntHandlerSPI2               (void)  { BSP_IntHandler(BSP_INT_ID_SPI2);                }
void  BSP_IntHandlerUSART1             (void)  { BSP_IntHandler(BSP_INT_ID_USART1);              }
void  BSP_IntHandlerUSART2             (void)  { BSP_IntHandler(BSP_INT_ID_USART2);              }
void  BSP_IntHandlerUSART3             (void)  { BSP_IntHandler(BSP_INT_ID_USART3);              }
void  BSP_IntHandlerEXTI15_10          (void)  { BSP_IntHandler(BSP_INT_ID_EXTI15_10);           }
void  BSP_IntHandlerRTCAlarm           (void)  { BSP_IntHandler(BSP_INT_ID_RTC_ALARM);           }
void  BSP_IntHandlerOTG_FS_WKUP        (void)  { BSP_IntHandler(BSP_INT_ID_OTG_FS_WKUP);         }
void  BSP_IntHandlerTIM8_BRK_TIM12     (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_BRK_TIM12);      }
void  BSP_IntHandlerTIM8_UP_TIM13      (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_UP_TIM13);       }
void  BSP_IntHandlerTIM8_TRG_COM_TIM14 (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_TRG_COM_TIM14);  }
void  BSP_IntHandlerTIM8_CC            (void)  { BSP_IntHandler(BSP_INT_ID_TIM8_CC);             }
void  BSP_IntHandlerDMA1_STREAM7       (void)  { BSP_IntHandler(BSP_INT_ID_DMA1_STREAM7);        }
void  BSP_IntHandlerFSMC               (void)  { BSP_IntHandler(BSP_INT_ID_FSMC);                }
void  BSP_IntHandlerSDIO               (void)  { BSP_IntHandler(BSP_INT_ID_SDIO);                }
void  BSP_IntHandlerTIM5               (void)  { BSP_IntHandler(BSP_INT_ID_TIM5);                }
void  BSP_IntHandlerSPI3               (void)  { BSP_IntHandler(BSP_INT_ID_SPI3);                }
void  BSP_IntHandlerUSART4             (void)  { BSP_IntHandler(BSP_INT_ID_USART4);              }
void  BSP_IntHandlerUSART5             (void)  { BSP_IntHandler(BSP_INT_ID_USART5);              }
void  BSP_IntHandlerTIM6_DAC           (void)  { BSP_IntHandler(BSP_INT_ID_TIM6_DAC);            }
void  BSP_IntHandlerTIM7               (void)  { BSP_IntHandler(BSP_INT_ID_TIM7);                }
void  BSP_IntHandlerDMA2_CH0           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH0);            }
void  BSP_IntHandlerDMA2_CH1           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH1);            }
void  BSP_IntHandlerDMA2_CH2           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH2);            }
void  BSP_IntHandlerDMA2_CH3           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH3);            }
void  BSP_IntHandlerDMA2_CH4           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH4);            }
void  BSP_IntHandlerETH                (void)  { BSP_IntHandler(BSP_INT_ID_ETH);                 }
void  BSP_IntHandlerETHWakeup          (void)  { BSP_IntHandler(BSP_INT_ID_ETH_WKUP);            }
void  BSP_IntHandlerCAN2_TX            (void)  { BSP_IntHandler(BSP_INT_ID_CAN2_TX);             }
void  BSP_IntHandlerCAN2_RX0           (void)  { BSP_IntHandler(BSP_INT_ID_CAN2_RX0);            }
void  BSP_IntHandlerCAN2_RX1           (void)  { BSP_IntHandler(BSP_INT_ID_CAN2_RX1);            }
void  BSP_IntHandlerCAN2_SCE           (void)  { BSP_IntHandler(BSP_INT_ID_CAN2_SCE);            }
void  BSP_IntHandlerOTG_FS             (void)  { BSP_IntHandler(BSP_INT_ID_OTG_FS);              }
void  BSP_IntHandlerDMA2_CH5           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH5);            }
void  BSP_IntHandlerDMA2_CH6           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH6);            }
void  BSP_IntHandlerDMA2_CH7           (void)  { BSP_IntHandler(BSP_INT_ID_DMA2_CH7);            }
void  BSP_IntHandlerUSART6             (void)  { BSP_IntHandler(BSP_INT_ID_USART6);              }
void  BSP_IntHandlerI2C3_EV            (void)  { BSP_IntHandler(BSP_INT_ID_I2C3_EV);             }
void  BSP_IntHandlerI2C3_ER            (void)  { BSP_IntHandler(BSP_INT_ID_I2C3_ER);             }
void  BSP_IntHandlerOTG_HS_EP1_OUT     (void)  { BSP_IntHandler(BSP_INT_ID_OTG_HS_EP1_OUT);      }
void  BSP_IntHandlerOTG_HS_EP1_IN      (void)  { BSP_IntHandler(BSP_INT_ID_OTG_HS_EP1_IN);       }
void  BSP_IntHandlerOTG_HS_WKUP        (void)  { BSP_IntHandler(BSP_INT_ID_OTG_HS_WKUP);         }
void  BSP_IntHandlerOTG_HS             (void)  { BSP_IntHandler(BSP_INT_ID_OTG_HS);              }
void  BSP_IntHandlerDCMI               (void)  { BSP_IntHandler(BSP_INT_ID_DCMI);                }
void  BSP_IntHandlerCRYP               (void)  { BSP_IntHandler(BSP_INT_ID_CRYP);                }
void  BSP_IntHandlerHASH_RNG           (void)  { BSP_IntHandler(BSP_INT_ID_HASH_RNG);            }
void  BSP_IntHandlerFPU                (void)  { BSP_IntHandler(BSP_INT_ID_FPU);                 }


static  void  BSP_IntHandler (CPU_DATA  int_id)
{
    CPU_FNCT_VOID  isr;
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();                                       /* Tell the OS that we are starting an ISR            */

    OSIntEnter();

    CPU_CRITICAL_EXIT();

    if (int_id < BSP_INT_SRC_NBR) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID)0u) {
            isr();
        }
    }

    OSIntExit();                                                /* Tell the OS that we are leaving the ISR            */
}

static  void  BSP_IntHandlerDummy (void)
{
}

void  CIM_Init (void)
{
    BSP_IntVectTbl[0]  = ISR_CIM_WWDG;                                         /* 16, INTISR[  0]  Window Watchdog.                      */
    BSP_IntVectTbl[1]  = ISR_CIM_PVD;                                          /* 17, INTISR[  1]  PVD through EXTI Line Detection.      */
    BSP_IntVectTbl[2]  = ISR_CIM_TAMP_STAMP;                                   /* 18, INTISR[  2]  Tamper and TimeStamp Interrupt.       */
    BSP_IntVectTbl[3]  = ISR_CIM_RTC_WKUP;                                     /* 19, INTISR[  3]  RTC Wakeup Global Interrupt.          */
    BSP_IntVectTbl[4]  = ISR_CIM_FLASH;                                        /* 20, INTISR[  4]  FLASH Global Interrupt.               */
    BSP_IntVectTbl[5]  = ISR_CIM_RCC;                                          /* 21, INTISR[  5]  RCC Global Interrupt.                 */
    BSP_IntVectTbl[6]  = ISR_CIM_EXTI0;                                        /* 22, INTISR[  6]  EXTI Line0 Interrupt.                 */
    BSP_IntVectTbl[7]  = ISR_CIM_EXTI1;                                        /* 23, INTISR[  7]  EXTI Line1 Interrupt.                 */
    BSP_IntVectTbl[8]  = ISR_CIM_EXTI2;                                        /* 24, INTISR[  8]  EXTI Line2 Interrupt.                 */
    BSP_IntVectTbl[9]  = ISR_CIM_EXTI3;                                        /* 25, INTISR[  9]  EXTI Line3 Interrupt.                 */
    BSP_IntVectTbl[10] = ISR_CIM_EXTI4;                                        /* 26, INTISR[ 10]  EXTI Line4 Interrupt.                 */
    BSP_IntVectTbl[11] = ISR_CIM_DMA1_CH0;                                     /* 27, INTISR[ 11]  DMA Channel0 Global Interrupt.        */
    BSP_IntVectTbl[12] = ISR_CIM_DMA1_CH1;                                     /* 28, INTISR[ 12]  DMA Channel1 Global Interrupt.        */
    BSP_IntVectTbl[13] = ISR_CIM_DMA1_CH2;                                     /* 29, INTISR[ 13]  DMA Channel2 Global Interrupt.        */
    BSP_IntVectTbl[14] = ISR_CIM_DMA1_CH3;                                     /* 30, INTISR[ 14]  DMA Channel3 Global Interrupt.        */
    BSP_IntVectTbl[15] = ISR_CIM_DMA1_CH4;                                     /* 31, INTISR[ 15]  DMA Channel4 Global Interrupt.        */
    BSP_IntVectTbl[16] = ISR_CIM_DMA1_CH5;                                     /* 32, INTISR[ 16]  DMA Channel5 Global Interrupt.        */
    BSP_IntVectTbl[17] = ISR_CIM_DMA1_CH6;                                     /* 33, INTISR[ 17]  DMA Channel6 Global Interrupt.        */
    BSP_IntVectTbl[18] = ISR_CIM_ADC;                                          /* 34, INTISR[ 18]  ADC1, ADC2 & ADC3 Global Interrupt.   */
    BSP_IntVectTbl[19] = ISR_CIM_CAN1_TX;                                      /* 35, INTISR[ 19]  CAN1 TX  Interrupts.                  */
    BSP_IntVectTbl[20] = ISR_CIM_CAN1_RX0;                                     /* 36, INTISR[ 20]  CAN1 RX0 Interrupts.                  */
    BSP_IntVectTbl[21] = ISR_CIM_CAN1_RX1;                                     /* 37, INTISR[ 21]  CAN1 RX1 Interrupt.                   */
    BSP_IntVectTbl[22] = ISR_CIM_CAN1_SCE;                                     /* 38, INTISR[ 22]  CAN1 SCE Interrupt.                   */
    BSP_IntVectTbl[23] = ISR_CIM_EXTI9_5;                                      /* 39, INTISR[ 23]  EXTI Line[9:5] Interrupt.             */
    BSP_IntVectTbl[24] = ISR_CIM_TIM1_BRK_TIM9;                                /* 40, INTISR[ 24]  TIM1 Break  Interrupt & TIM9 Glb int. */
    BSP_IntVectTbl[25] = ISR_CIM_TIM1_UP_TIM10;                                /* 41, INTISR[ 25]  TIM1 Update Interrupt & TIM10 Glb int.*/
    BSP_IntVectTbl[26] = ISR_CIM_TIM1_TRG_COM_TIM11;                           /* 42, INTISR[ 26]  TIM1 Trig & Commutation Interrupts... */
                                                                               /* .................and TIM11 global interrupt.           */ 
    BSP_IntVectTbl[27] = ISR_CIM_TIM1_CC;                                      /* 43, INTISR[ 27]  TIM1 Capture Compare Interrupt.       */
    BSP_IntVectTbl[28] = ISR_CIM_TIM2;                                         /* 44, INTISR[ 28]  TIM2 Global Interrupt.                */
    BSP_IntVectTbl[29] = ISR_CIM_TIM3;                                         /* 45, INTISR[ 29]  TIM3 Global Interrupt.                */
    BSP_IntVectTbl[30] = ISR_CIM_TIM4;                                         /* 46, INTISR[ 30]  TIM4 Global Interrupt.                */
    BSP_IntVectTbl[31] = ISR_CIM_I2C1_EV;                                      /* 47, INTISR[ 31]  I2C1 Event  Interrupt.                */
    BSP_IntVectTbl[32] = ISR_CIM_I2C1_ER;                                      /* 48, INTISR[ 32]  I2C1 Error  Interrupt.                */
    BSP_IntVectTbl[33] = ISR_CIM_I2C2_EV;                                      /* 49, INTISR[ 33]  I2C2 Event  Interrupt.                */
    BSP_IntVectTbl[34] = ISR_CIM_I2C2_ER;                                      /* 50, INTISR[ 34]  I2C2 Error  Interrupt.                */
    BSP_IntVectTbl[35] = ISR_CIM_SPI1;                                         /* 51, INTISR[ 35]  SPI1 Global Interrupt.                */
    BSP_IntVectTbl[36] = ISR_CIM_SPI2;                                         /* 52, INTISR[ 36]  SPI2 Global Interrupt.                */
    BSP_IntVectTbl[37] = ISR_CIM_USART1;                                       /* 53, INTISR[ 37]  USART1 Global Interrupt.              */
    BSP_IntVectTbl[38] = ISR_CIM_USART2;                                       /* 54, INTISR[ 38]  USART2 Global Interrupt.              */
    BSP_IntVectTbl[39] = ISR_CIM_USART3;                                       /* 55, INTISR[ 39]  USART3 Global Interrupt.              */
    BSP_IntVectTbl[40] = ISR_CIM_EXTI15_10;                                    /* 56, INTISR[ 40]  EXTI Line [15:10] Interrupts.         */
    BSP_IntVectTbl[41] = ISR_CIM_RTCAlarm;                                     /* 57, INTISR[ 41]  RTC Alarm EXT Line Interrupt.         */
    BSP_IntVectTbl[42] = ISR_CIM_OTG_FS_WKUP;                                  /* 58, INTISR[ 42]  USB OTG Wakeup through EXTI line int. */
    BSP_IntVectTbl[43] = ISR_CIM_TIM8_BRK_TIM12;                               /* 59, INTISR[ 43]  TIM8 Break and TIM12 global Int.      */
    BSP_IntVectTbl[44] = ISR_CIM_TIM8_UP_TIM13;                                /* 60, INTISR[ 44]  TIM8 Update and TIM13 global Int.     */
    BSP_IntVectTbl[45] = ISR_CIM_TIM8_TRG_COM_TIM14;                           /* 61, INTISR[ 45]  TIM8 Trigger/Commutation and TIM14.   */
    BSP_IntVectTbl[46] = ISR_CIM_TIM8_CC;                                      /* 62, INTISR[ 46]  TIM8 Capture Compare Int.             */
    BSP_IntVectTbl[47] = ISR_CIM_DMA1_STREAM7;                                 /* 63, INTISR[ 47]  DMA1 Stream7 Int.                     */
    BSP_IntVectTbl[48] = ISR_CIM_FSMC;                                         /* 64, INTISR[ 48]  FSMC global Int.                      */
    BSP_IntVectTbl[49] = ISR_CIM_SDIO;                                         /* 65, INTISR[ 49]  SDIO global Int.                      */
    BSP_IntVectTbl[50] = ISR_CIM_TIM5;                                         /* 66, INTISR[ 50]  TIM5 global Interrupt.                */
    BSP_IntVectTbl[51] = ISR_CIM_SPI3;                                         /* 67, INTISR[ 51]  SPI3 global Interrupt.                */
    BSP_IntVectTbl[52] = ISR_CIM_USART4;                                       /* 68, INTISR[ 52]  UART4 global Interrupt.               */
    BSP_IntVectTbl[53] = ISR_CIM_USART5;                                       /* 69, INTISR[ 53]  UART5 global Interrupt.               */
    BSP_IntVectTbl[54] = ISR_CIM_TIM6_DAC;                                     /* 70, INTISR[ 54]  TIM6 global Interrupt, DAC1 & DAC2 .. */
                                                                               /* ................ underrun error interrupts.            */
    BSP_IntVectTbl[55] = ISR_CIM_TIM7;                                         /* 71, INTISR[ 55]  TIM7 global Interrupt.                */
    BSP_IntVectTbl[56] = ISR_CIM_DMA2_CH0;                                     /* 72, INTISR[ 56]  DMA2 Channel 0 global Interrupt.      */
    BSP_IntVectTbl[57] = ISR_CIM_DMA2_CH1;                                     /* 73, INTISR[ 57]  DMA2 Channel 1 global Interrupt.      */
    BSP_IntVectTbl[58] = ISR_CIM_DMA2_CH2;                                     /* 74, INTISR[ 58]  DMA2 Channel 2 global Interrupt.      */
    BSP_IntVectTbl[59] = ISR_CIM_DMA2_CH3;                                     /* 75, INTISR[ 59]  DMA2 Channel 3 global Interrupt.      */
    BSP_IntVectTbl[60] = ISR_CIM_DMA2_CH4;                                     /* 76, INTISR[ 60]  DMA2 Channel 4 global Interrupt.      */
    BSP_IntVectTbl[61] = ISR_CIM_ETH;                                          /* 77, INTISR[ 61]  ETH global Interrupt.                 */
    BSP_IntVectTbl[61] = ISR_CIM_ETHWakeup;                                    /* 78, INTISR[ 62]  ETH WakeUp from EXTI line Interrupt.  */
    BSP_IntVectTbl[63] = ISR_CIM_CAN2_TX;                                      /* 79, INTISR[ 63]  CAN2 TX Interrupts.                   */
    BSP_IntVectTbl[64] = ISR_CIM_CAN2_RX0;                                     /* 80, INTISR[ 64]  CAN2 RX0 Interrupts.                  */
    BSP_IntVectTbl[65] = ISR_CIM_CAN2_RX1;                                     /* 81, INTISR[ 65]  CAN2 RX1 Interrupt.                   */
    BSP_IntVectTbl[66] = ISR_CIM_CAN2_SCE;                                     /* 82, INTISR[ 66]  CAN2 SCE Interrupt.                   */
    BSP_IntVectTbl[67] = ISR_CIM_OTG_FS;                                       /* 83, INTISR[ 67]  OTG global Interrupt.                 */
    BSP_IntVectTbl[68] = ISR_CIM_DMA2_CH5;                                     /* 84, INTISR[ 68]  DMA2 Channel 5 global Interrupt.      */
    BSP_IntVectTbl[69] = ISR_CIM_DMA2_CH6;                                     /* 85, INTISR[ 69]  DMA2 Channel 6 global Interrupt.      */
    BSP_IntVectTbl[70] = ISR_CIM_DMA2_CH7;                                     /* 86, INTISR[ 70]  DMA2 Channel 7 global Interrupt.      */
    BSP_IntVectTbl[71] = ISR_CIM_USART6;                                       /* 87, INTISR[ 71]  UART6 global Interrupt.               */
    BSP_IntVectTbl[72] = ISR_CIM_I2C3_EV;                                      /* 88, INTISR[ 72]  I2C3 Event  Interrupt.                */
    BSP_IntVectTbl[73] = ISR_CIM_I2C3_ER;                                      /* 89, INTISR[ 73]  I2C3 Error  Interrupt.                */
    BSP_IntVectTbl[74] = ISR_CIM_OTG_HS_EP1_OUT;                               /* 90, INTISR[ 74]  OTG HS EP1 OUT Interrupt.             */
    BSP_IntVectTbl[75] = ISR_CIM_OTG_HS_EP1_IN;                                /* 91, INTISR[ 75]  OTG HS EP1 IN Interrupt.              */
    BSP_IntVectTbl[76] = ISR_CIM_OTG_HS_WKUP;                                  /* 92, INTISR[ 76]  OTG HS Wakeup Interrupt.              */
    BSP_IntVectTbl[77] = ISR_CIM_OTG_HS;                                       /* 93, INTISR[ 77]  OTG HS global Interrupt.              */
    BSP_IntVectTbl[78] = ISR_CIM_DCMI;                                         /* 94, INTISR[ 78]  DCMI global Interrupt.                */
    BSP_IntVectTbl[79] = ISR_CIM_CRYP;                                         /* 95, INTISR[ 79]  CRYPT crypto global Interrupt.        */
    BSP_IntVectTbl[80] = ISR_CIM_HASH_RNG;                                     /* 96, INTISR[ 80]  HASH and RNG global Interrupt.        */
    BSP_IntVectTbl[81] = ISR_CIM_FPU;                                          /* 97, INTISR[ 81]  FPU global Interrupt.                 */
}

static  void  ISR_CIM_Dummy (void)
{
  int i = 1;
  while (i) {}
}

static  void  ISR_CIM_WWDG(void)
{
    // May need initation 
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_PVD(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_TAMP_STAMP(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_RTC_WKUP(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_FLASH(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_RCC(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_EXTI0(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_EXTI1(void)
{
	// May need initation
	//ISR_CIM_Dummy();
	//EXTI1_IRQHandler();
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

static  void  ISR_CIM_EXTI2(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_EXTI3(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_EXTI4(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA1_CH0(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA1_CH1(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA1_CH2(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA1_CH3(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA1_CH4(void)
{
    // May need initation
   // ISR_CIM_Dummy();
	HAL_DMA_IRQHandler(Tim3CaptureHandle.hdma[1]);
	//HAL_TIM_Base_Stop_DMA(&Tim3CaptureHandle);
	{
		HAL_TIM_IC_Stop_DMA(&Tim3CaptureHandle, TIM_CHANNEL_1);
	}
	
}

static  void  ISR_CIM_DMA1_CH5(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA1_CH6(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_ADC(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_CAN1_TX(void)
{
    // May need initation
	HAL_CAN_IRQHandler(&Can_1_Handle);	//CAN 1发送
}

static  void  ISR_CIM_CAN1_RX0(void)
{
    // May need initation
	//CAN 1 收
	HAL_CAN1_IRQHandler();
}

static  void  ISR_CIM_CAN1_RX1(void)
{
    // May need initation
	//CAN 1 收
	HAL_CAN1_IRQHandler();
}

static  void  ISR_CIM_CAN1_SCE(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_EXTI9_5(void)
{
    // May need initation
	//HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
	HAL_GPIO_EXTI_5_IRQHandler(GPIO_PIN_5);	
}

static  void  ISR_CIM_TIM1_BRK_TIM9(void)
{
    HAL_TIM_IRQHandler(&Tim9Handle);
}

static  void  ISR_CIM_TIM1_UP_TIM10(void)
{
    HAL_TIM_IRQHandler(&Tim10Handle);
}

static  void  ISR_CIM_TIM1_TRG_COM_TIM11(void)
{
   HAL_TIM_IRQHandler(&Tim11Handle);
}

static  void  ISR_CIM_TIM1_CC(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_TIM2(void)
{
   HAL_TIM_IRQHandler(&Tim2Handle);
}

static  void  ISR_CIM_TIM3(void)
{
    // May need initation
    //ISR_CIM_Dummy();
	  HAL_TIM_IRQHandler(&Tim3CaptureHandle);
}

static  void  ISR_CIM_TIM4(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_I2C1_EV(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_I2C1_ER(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_I2C2_EV(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_I2C2_ER(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_SPI1(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_SPI2(void)
{
    // May need initation
    ISR_CIM_Dummy();
}
static HAL_StatusTypeDef UART1_Transmit_IT(void);
static HAL_StatusTypeDef UART1_Transmit_IT(void)
{
	uint16_t* tmp;
	uint32_t tmp1 = 0;
        UNUSED(tmp1);
	UART_HandleTypeDef *huart = &Uart1Handle;
	tmp1 = huart->State;
	//if((tmp1 == HAL_UART_STATE_BUSY_TX) || (tmp1 == HAL_UART_STATE_BUSY_TX_RX))
	{
		if(huart->Init.WordLength == UART_WORDLENGTH_9B)
		{
			tmp = (uint16_t*) huart->pTxBuffPtr;
			huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
			if(huart->Init.Parity == UART_PARITY_NONE)
			{
				huart->pTxBuffPtr += 2;
			}
			else
			{
				huart->pTxBuffPtr += 1;
			}
		} 
		else
		{
			huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
		}

		if(--huart->TxXferCount == 0)
		{
			/* Disable the UART Transmit Complete Interrupt */
			__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
			/* Check if a receive process is ongoing or not */
			if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
			{
				huart->State = HAL_UART_STATE_BUSY_RX;
			}
			else
			{
				/* Disable the UART Parity Error Interrupt */
				__HAL_UART_DISABLE_IT(huart, UART_IT_PE);
				/* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
				__HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
				huart->State = HAL_UART_STATE_READY;
			}

			/* Wait on TC flag to be able to start a second transfer */
			//if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, UART_6_TIMEOUT_VALUE) != HAL_OK)
			//{
			//	__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
			//	__HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
			//	return HAL_TIMEOUT;
			//}
			if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET))
			{
				__NOP();
			}
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			//__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			huart->State= HAL_UART_STATE_READY;
			/* Process Unlocked */
			//__HAL_UNLOCK(huart);
			return HAL_OK;
		}
		return HAL_OK;
	}
	//else
	//{
	//	return HAL_BUSY;
	//}
}


static  void  ISR_CIM_USART1(void)
{
	uint8_t Uart1rxVal=0;
	uint32_t tmp1 = 0, tmp2 = 0;
	UART_HandleTypeDef *huart = &Uart1Handle;
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
	/* UART parity error interrupt occurred ------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_PEFLAG(huart);
		huart->ErrorCode |= HAL_UART_ERROR_PE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART frame error interrupt occurred -------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_FEFLAG(huart);
		huart->ErrorCode |= HAL_UART_ERROR_FE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART noise error interrupt occurred -------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_NEFLAG(huart);
		huart->ErrorCode |= HAL_UART_ERROR_NE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART Over-Run interrupt occurred ----------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_OREFLAG(huart);
		huart->ErrorCode |= HAL_UART_ERROR_ORE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
	/* UART in mode Transmitter ------------------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		UART1_Transmit_IT();
	}


	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART in mode Receiver ---------------------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		//if(huart->Init.Parity == UART_PARITY_NONE)
		//{
		//	Uart1rxVal = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
		//}
		//else
		//{
		//	Uart1rxVal= (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
		//}
		Uart1rxVal = huart->Instance->DR;
		if (dv_Uart1.pData < UART1_RECV_MAX_NUM - 2)
		{
			gUART1RecvData[dv_Uart1.pData++] = Uart1rxVal & 0xff;
		}

		

		gWifiReceiveFlag = WIFI_R_BYTE_START_COUNT;
        gWifiReceive_2_Flag = WIFI_R_BYTE_START_COUNT;
	}
}

static  void  ISR_CIM_USART2(void)
{
	uint8_t Uart2rxVal = 0;
	uint32_t tmp1 = 0, tmp2 = 0;
	UART_HandleTypeDef *huart = &Uart2Handle;
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
	/* UART parity error interrupt occurred ------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_PEFLAG(huart);
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART frame error interrupt occurred -------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_FEFLAG(huart);
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART noise error interrupt occurred -------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_NEFLAG(huart);
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART Over-Run interrupt occurred ----------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		__HAL_UART_CLEAR_OREFLAG(huart);
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART in mode Receiver ---------------------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		//if(huart->Init.Parity == UART_PARITY_NONE)
		//{
		//	Uart2rxVal = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
		//	
		//}
		//else
		//{
		//	Uart2rxVal= (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
		//}

		Uart2rxVal = huart->Instance->DR;
		if (dv_Uart2.pUart2Data < UART2_RECV_MAX_NUM -1)
		{
			gUart2RecvBuf[dv_Uart2.pUart2Data++] = Uart2rxVal & 0x0ff;
		}

		//if (TRUE == gEvChargeInfo.u8TEST_MODE_FLAG)
		//{
		//	if (0x5A != Uart2rxVal)						//UART1 发的是 0X5A
		//	{
		//		gEvChargeInfo.u8EV_InterDI_COM &= ~(0x01 << 5);
		//	}
		//}
	}
}

static  void  ISR_CIM_USART3(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_EXTI15_10(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_RTCAlarm(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_OTG_FS_WKUP(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_TIM8_BRK_TIM12(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_TIM8_UP_TIM13(void)
{

	ISR_CIM_Dummy();



}

static  void  ISR_CIM_TIM8_TRG_COM_TIM14(void)
{
    HAL_TIM_IRQHandler(&Tim14Handle);
}

static  void  ISR_CIM_TIM8_CC(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA1_STREAM7(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_FSMC(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_SDIO(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_TIM5(void)
{
    // May need initation
    //ISR_CIM_Dummy();
	HAL_TIM_IRQHandler(&Tim5Handle);
}

static  void  ISR_CIM_SPI3(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static HAL_StatusTypeDef UART4_Transmit_IT(void);
static HAL_StatusTypeDef UART4_Transmit_IT(void)
{
	uint16_t* tmp;
	uint32_t tmp1 = 0;
	UNUSED(tmp1);
	UART_HandleTypeDef *huart = &Uart4Handle;
	tmp1 = huart->State;
	//if((tmp1 == HAL_UART_STATE_BUSY_TX) || (tmp1 == HAL_UART_STATE_BUSY_TX_RX))
	{
		if(huart->Init.WordLength == UART_WORDLENGTH_9B)
		{
			tmp = (uint16_t*) huart->pTxBuffPtr;
			huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
			if(huart->Init.Parity == UART_PARITY_NONE)
			{
				huart->pTxBuffPtr += 2;
			}
			else
			{
				huart->pTxBuffPtr += 1;
			}
		} 
		else
		{
			huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
		}

		if(--huart->TxXferCount == 0)
		{
			/* Disable the UART Transmit Complete Interrupt */
			__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

			/* Check if a receive process is ongoing or not */
			if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
			{
				huart->State = HAL_UART_STATE_BUSY_RX;
			}
			else
			{
				/* Disable the UART Parity Error Interrupt */
				__HAL_UART_DISABLE_IT(huart, UART_IT_PE);
				/* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
				__HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
				huart->State = HAL_UART_STATE_READY;
			}

			/* Wait on TC flag to be able to start a second transfer */
			//if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, UART_6_TIMEOUT_VALUE) != HAL_OK)
			//{
			//	__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
			//	__HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
			//	return HAL_TIMEOUT;
			//}
			if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET))
			{
				__NOP();
			}
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			//__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			huart->State= HAL_UART_STATE_READY;
			/* Process Unlocked */
			//__HAL_UNLOCK(huart);
			dv_Uart4.Rs485SendCheck_LastDelay = RS485_SEND_DELAY_START_COUNT;
			return HAL_OK;
		}
		return HAL_OK;
	}
	//else
	//{
	//	return HAL_BUSY;
	//}
}

static  void  ISR_CIM_USART4(void)
{
    //HAL_UART_IRQHandler(&Uart4Handle);
	//HAL_UART4_IRQHandler();
	uint32_t tmp1 = 0, tmp2 = 0;
	uint8_t Uart4rxVal=0;
	UART_HandleTypeDef *huart = &Uart4Handle;
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
	/* UART parity error interrupt occurred ------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_PEFLAG(huart);

		huart->ErrorCode |= HAL_UART_ERROR_PE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART frame error interrupt occurred -------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_FEFLAG(huart);

		huart->ErrorCode |= HAL_UART_ERROR_FE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART noise error interrupt occurred -------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_NEFLAG(huart);

		huart->ErrorCode |= HAL_UART_ERROR_NE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART Over-Run interrupt occurred ----------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_OREFLAG(huart);

		huart->ErrorCode |= HAL_UART_ERROR_ORE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART in mode Receiver ---------------------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		//UART_Receive_IT(huart);
		Uart4rxVal = 0;
		Uart4rxVal = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
		if(dv_Uart4.pData < RS485_UART4_RX_MAX_NUM - 1)
		{
			gUart4RecvBuff[dv_Uart4.pData++] = Uart4rxVal;
		}
		else
		{
			dv_Uart4.pData = 0;
		}
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
	/* UART in mode Transmitter ------------------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		UART4_Transmit_IT();
	}

	if(huart->ErrorCode != HAL_UART_ERROR_NONE)
	{
		/* Set the UART state ready to be able to start again the process */
		huart->State = HAL_UART_STATE_READY;
		//HAL_UART_ErrorCallback(huart);
	}
}

static HAL_StatusTypeDef UART5_Transmit_IT(void);
static HAL_StatusTypeDef UART5_Transmit_IT(void)
{
	uint16_t* tmp = NULL;
	uint32_t tmp1 = 0;
        UNUSED(tmp);
        UNUSED(tmp1);
	UART_HandleTypeDef *huart = &Uart5Handle;
	tmp1 = huart->State;
	UNUSED(tmp1);
	//if((tmp1 == HAL_UART_STATE_BUSY_TX) || (tmp1 == HAL_UART_STATE_BUSY_TX_RX))
	{
		if(huart->Init.WordLength == UART_WORDLENGTH_9B)
		{
			tmp = (uint16_t*) huart->pTxBuffPtr;
			huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
			if(huart->Init.Parity == UART_PARITY_NONE)
			{
				huart->pTxBuffPtr += 2;
			}
			else
			{
				huart->pTxBuffPtr += 1;
			}
		} 
		else
		{
			huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
		}

		if(--huart->TxXferCount == 0)
		{
			/* Disable the UART Transmit Complete Interrupt */
			__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
			/* Check if a receive process is ongoing or not */
			if(huart->State == HAL_UART_STATE_BUSY_TX_RX) 
			{
				huart->State = HAL_UART_STATE_BUSY_RX;
			}
			else
			{
				/* Disable the UART Parity Error Interrupt */
				__HAL_UART_DISABLE_IT(huart, UART_IT_PE);
				/* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
				__HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
				huart->State = HAL_UART_STATE_READY;
			}

			/* Wait on TC flag to be able to start a second transfer */
			//if(UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, UART_6_TIMEOUT_VALUE) != HAL_OK)
			//{
			//	__HAL_UART_DISABLE_IT(huart, UART_IT_TXE);
			//	__HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
			//	return HAL_TIMEOUT;
			//}
			if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET))
			{
				__NOP();
			}
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			//__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			huart->State= HAL_UART_STATE_READY;
			/* Process Unlocked */
			//__HAL_UNLOCK(huart);
			dv_Uart5.Rs485SendCheck_LastDelay = RS485_SEND_DELAY_START_COUNT;
			return HAL_OK;
		}
		return HAL_OK;
	}
	//else
	//{
	//	return HAL_BUSY;
	//}
}

static  void  ISR_CIM_USART5(void)
{
   //HAL_UART_IRQHandler(&Uart5Handle);
	//HAL_UART5_IRQHandler();
	uint32_t tmp1 = 0, tmp2 = 0;
	uint8_t Uart5rxVal=0;
	UART_HandleTypeDef *huart = &Uart5Handle;
	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_PE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_PE);  
	/* UART parity error interrupt occurred ------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_PEFLAG(huart);

		huart->ErrorCode |= HAL_UART_ERROR_PE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_FE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART frame error interrupt occurred -------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_FEFLAG(huart);

		huart->ErrorCode |= HAL_UART_ERROR_FE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_NE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR);
	/* UART noise error interrupt occurred -------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_NEFLAG(huart);

		huart->ErrorCode |= HAL_UART_ERROR_NE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_ORE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART Over-Run interrupt occurred ----------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		__HAL_UART_CLEAR_OREFLAG(huart);

		huart->ErrorCode |= HAL_UART_ERROR_ORE;
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_RXNE);
	/* UART in mode Receiver ---------------------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{ 
		//UART_Receive_IT(huart);
		Uart5rxVal = 0;
		Uart5rxVal = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
		if(dv_Uart5.pData < RS485_UART5_RX_MAX_NUM - 1)
		{
			gUart5RecvBuff[dv_Uart5.pData++] = Uart5rxVal;
		}
		else
		{
			dv_Uart5.pData = 0;
		}
	}

	tmp1 = __HAL_UART_GET_FLAG(huart, UART_FLAG_TXE);
	tmp2 = __HAL_UART_GET_IT_SOURCE(huart, UART_IT_TXE);
	/* UART in mode Transmitter ------------------------------------------------*/
	if((tmp1 != RESET) && (tmp2 != RESET))
	{
		//UART_Transmit_IT(huart);
		UART5_Transmit_IT();
	}

	if(huart->ErrorCode != HAL_UART_ERROR_NONE)
	{
		/* Set the UART state ready to be able to start again the process */
		huart->State = HAL_UART_STATE_READY;
		//HAL_UART_ErrorCallback(huart);
	}
}

static  void  ISR_CIM_TIM6_DAC(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_TIM7(void)
{
    // May need initation

	TIM_HandleTypeDef *htim = &Tim7Handle;

	/* TIM Update event */
	if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET)
	{
		if(__HAL_TIM_GET_ITSTATUS(htim, TIM_IT_UPDATE) !=RESET)
		{
			__HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
			V2F_Capture_TimerCallBack();
				
		}
	}
}
static void Jump2DummyNextCh(void);
static void Jump2DummyNextCh(void)
{
	dv_Adc.fADWorkPortSelect(AD_PIN_PC0);						//PB0
	dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue, mENERGYPOINTNUM);
}
static  void  ISR_CIM_DMA2_CH0(void)
{
	UINT8 u8Idx = 0;
	UCHAR i,ucdatanum;
	INT32 jj = 0;
	UNUSED(jj);
	UINT32 u32MaxValue,u32MinValue,u32Sum;
	UINT32 DMAValue[mENERGYPOINTNUM];
	u32MaxValue = 0;
	UINT32 DMAValueCpy = 0;
	u32MinValue = 0xFFFFFFFF;  
	u32Sum = 0;
	float k,b,f32temp1,f32temp2;
	k = 1;
	b = 0;
	f32temp1 = 0;
	f32temp2 = 0;
    UNUSED(k);
	UNUSED(b);
	UNUSED(f32temp1);
	UNUSED(f32temp2);
	HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
	if(dv_Adc.ucDMASampelTime <= 2)
	{
		dv_Adc.ucDMASampelTime ++;
		return;
	}
	else
	{
		dv_Adc.ucDMASampelTime = 0;
	}

	for(i = 0;i< mENERGYPOINTNUM;i++)
	{
		DMAValue[i] = dv_Adc.u32OneLoopValue[i];
		dv_Adc.u32OneLoopValue[i]= 0;
	}

	for(i = 0;i< mENERGYPOINTNUM;i++)
	{
		u32Sum += DMAValue[i];
		if(u32MaxValue < DMAValue[i])
			u32MaxValue  = DMAValue[i];
		if(u32MinValue > DMAValue[i])
			u32MinValue  = DMAValue[i];
		if (gEvChargeInfo.u8HasACGun == TRUE)
		{
			if((PC0_ADC_SW_oldCP_CC_OF_AC_NO_4 == dv_Adc.ucOneLoopFlag) && (FALSE == u8Is1000VHardwareA00()))
			{
				if (DMAValue[i] > 546)
				{
					DMAValueCpy += DMAValue[i];
					u8Idx++;
				}
				
				if (u8Idx == 0)
				{
					gEvChargeInfo.i16AC_CP_CheckTimes++;
					if ((gEvChargeInfo.i16AC_CP_CheckTimes%(4*mENERGYPOINTNUM))== 0)			//怕它永远进去，所有加一个次数的（20次）限制！
					{
						dv_Adc.ucOneLoopFlag = 3;												//退回去重新采集！
						gEvChargeInfo.i16AC_CP_CheckTimes = 0;
					}
				}
				else
				{
					gEvChargeInfo.i16AC_CP_CheckTimes = 0;
				}

				if (gEvChargeInfo.u8AC_StartCMD == TRUE && gEvChargeInfo.u8AC_C_EnterFlag == TRUE)
				{
				}
			}
		}

		if(dv_Adc.ucStartMeasureFlag ==  1 && dv_Adc.ucOneLoopFlag < MAX_CHANEL_NUM)
		{
			ucdatanum = dv_Adc.ucGetDataNum[dv_Adc.ucOneLoopFlag];
			if(ucdatanum < mSAMPLEPOINTNUM)
			{
				dv_Adc.u32ADCValue[dv_Adc.ucOneLoopFlag][ucdatanum]= DMAValue[i];
				dv_Adc.ucGetDataNum[dv_Adc.ucOneLoopFlag]++;
			}
		}
	}
	u32Sum -=(u32MaxValue + u32MinValue);
	u32Sum = u32Sum/(mENERGYPOINTNUM - 2);

	if(PA3_ADC_SW_GUN_1_NO_0 == dv_Adc.ucOneLoopFlag)
	{
		fgCC1QuickVal = u32Sum;
	}

	if(PA6_ADC_SW_GUN_2_oldBUS_2_NO_2 == dv_Adc.ucOneLoopFlag)
	{
		if(u8Is1000VHardwareA00())
		{
			fgCC1QuickVal_2 = u32Sum;
		}
	}

	if(PA0_ADC_SW_oldGUN_2_None_NO_6 == dv_Adc.ucOneLoopFlag)
	{
		if(!u8Is1000VHardwareA00())
		{
			fgCC1QuickVal_2 = u32Sum;
		}
	}

	if((PC0_ADC_SW_oldCP_CC_OF_AC_NO_4 == dv_Adc.ucOneLoopFlag) && (FALSE == u8Is1000VHardwareA00()))
	{
		if (u8Idx > 0)
		{
			dv_Adc.I32TimerCount = 0;
			fgCC1QuickVal_AC = DMAValueCpy/u8Idx;				//简单取平均数值了
			if (ACFILTER_IDX >= (mFIELT_NUM - 1))
			{
				ACFILTER_IDX = 0;
			}
			AC_DMAValue[ACFILTER_IDX++] = fgCC1QuickVal_AC;
			fAC_PWM_VOLTAGE_QuickVal = fgCC1QuickVal_AC;
			ACFILTER_count = 0;
		}
	}
	
	if(dv_Adc.ucOneLoopFlag == (PA3_ADC_SW_GUN_1_NO_0))
	{
		//0 切 1
		dv_Adc.fADWorkPortSelect(AD_PIN_PA4);						//PA4
		dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue,mENERGYPOINTNUM);
		dv_Adc.ucOneLoopFlag++;
	}
	else if(dv_Adc.ucOneLoopFlag == (PA4_ADC_SW_TEMP_NO_1))
	{
		//TXY-AD
		dv_Adc.fADWorkPortSelect(AD_PIN_PA6);						//PA6
		dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue,mENERGYPOINTNUM);
		dv_Adc.ucOneLoopFlag++;
	}
	else if(dv_Adc.ucOneLoopFlag == (PA6_ADC_SW_GUN_2_oldBUS_2_NO_2))
	{
		//1 切 2
		dv_Adc.fADWorkPortSelect(AD_PIN_PB0);						//PB0
		dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue, mENERGYPOINTNUM);
		dv_Adc.ucOneLoopFlag++;
	}
	else if (dv_Adc.ucOneLoopFlag == (PB0_ADC_SW_oldHOLL_TP1000_NO_3))
	{
		//2 切 3
		dv_Adc.fADWorkPortSelect(AD_PIN_PC0);						//PC0
		dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue, mENERGYPOINTNUM);
		dv_Adc.ucOneLoopFlag++;
	}
	else if (dv_Adc.ucOneLoopFlag == (PC0_ADC_SW_oldCP_CC_OF_AC_NO_4))	
	{
		if(u8Is1000VHardwareA00())
		{
			dv_Adc.fADWorkPortSelect(AD_PIN_PC3);					//PC3
			dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue, mENERGYPOINTNUM);
		}
		else
		{
			Jump2DummyNextCh();
		}
		dv_Adc.ucOneLoopFlag++;
	}
	else  if (dv_Adc.ucOneLoopFlag == (PC3_ADC_SW_TP1000_NO_5))
	{
		if(u8Is1000VHardwareA00())
		{
			//dv_Adc.ucOneLoopFlag = PC2_ADC_SW_oldNone_PS_NO_8 - 1;			//PC2
			//dv_Adc.fADWorkPortSelect(AD_PIN_PC2);
			//dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue, mENERGYPOINTNUM);
			Jump2DummyNextCh();
		}
		else
		{
			dv_Adc.fADWorkPortSelect(AD_PIN_PA0);					//PA0
			dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue, mENERGYPOINTNUM);
		}
		dv_Adc.ucOneLoopFlag++;	
	}
	else  if (dv_Adc.ucOneLoopFlag == (PA0_ADC_SW_oldGUN_2_None_NO_6))
	{
		if(u8Is1000VHardwareA00())
		{
			Jump2DummyNextCh();
		}
		else
		{
			dv_Adc.fADWorkPortSelect(AD_PIN_PA5);					//PA5
			dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue, mENERGYPOINTNUM);
		}
		dv_Adc.ucOneLoopFlag++;	
	}
	else if (dv_Adc.ucOneLoopFlag == (PA5_ADC_SW_oldBUS_1_None_NO_7))
	{
		if(u8Is1000VHardwareA00())
		{
			//Jump2DummyNextCh();
			dv_Adc.ucOneLoopFlag = PC2_ADC_SW_oldNone_PS_NO_8 - 1;			//PC2
			dv_Adc.fADWorkPortSelect(AD_PIN_PC2);
			dv_Adc.fStartMeasure(dv_Adc.u32OneLoopValue, mENERGYPOINTNUM);
		}
		else
		{
			dv_Adc.fDMAStop(); 
			gMeasureInfo.u16MeasureLoopTimes++;
			dv_Adc.ucOneLoopFlag = MAX_CHANEL_NUM;
			gMeasureInfo.ucIsMeasureOneLoop = TRUE;
		}
		dv_Adc.ucOneLoopFlag++;	
	}
	else if (dv_Adc.ucOneLoopFlag == (PC2_ADC_SW_oldNone_PS_NO_8))
	{
		if(u8Is1000VHardwareA00())
		{
			Jump2DummyNextCh();
		}
		else
		{
			Jump2DummyNextCh();
		}
		dv_Adc.ucOneLoopFlag++;	
	}
	else
	{
		gMeasureInfo.u16MeasureLoopTimes++;
		dv_Adc.fDMAStop(); 
		//A04	
		dv_Adc.ucOneLoopFlag = MAX_CHANEL_NUM;
		gMeasureInfo.ucIsMeasureOneLoop = TRUE;
	}
}

static  void  ISR_CIM_DMA2_CH1(void)
{
	int i = 0;
	UCHAR ucdatanum;
	UINT32 u32MaxValue,u32MinValue,u32Sum;
	UINT32 DMAValue[mENERGYPOINTNUM+2];
	u32MaxValue = 0;
	//UINT32 DMAValueCpy = 0;
	u32MinValue = 0xFFFFFFFF;  
	u32Sum = 0;

	//A04	
	HAL_DMA_IRQHandler(ADC_3_Handle.DMA_Handle);
	if(dv_ADC_3.ucDMASampelTime <= 2)
	{
		dv_ADC_3.ucDMASampelTime ++;
		return;
	}
	else
	{
		dv_ADC_3.ucDMASampelTime = 0;
	}
	for(i = 0;i< mADC3_ENERGYPOINTNUM;i++)
	{
		DMAValue[i] = dv_ADC_3.u32OneLoopValue[i];
        UNUSED(DMAValue[i]);
		dv_ADC_3.u32OneLoopValue[i]= 0;
	}

	for(i = 0;i< mADC3_ENERGYPOINTNUM;i++)
	{
		u32Sum += DMAValue[i];
		if(u32MaxValue < DMAValue[i])
			u32MaxValue  = DMAValue[i];
		if(u32MinValue > DMAValue[i])
			u32MinValue  = DMAValue[i];

			ucdatanum = dv_ADC_3.ucGetDataNum[0];
			if(ucdatanum < mSAMPLEPOINTNUM)
			{
				dv_ADC_3.u32ADCValue[dv_ADC_3.ucOneLoopFlag][ucdatanum]= DMAValue[i];
				dv_ADC_3.ucGetDataNum[dv_ADC_3.ucOneLoopFlag]++;
			}
	}

	u32Sum -=(u32MaxValue + u32MinValue);
	u32Sum = u32Sum/(mADC3_ENERGYPOINTNUM - 2);
	fAC_PWM_VOLTAGE_QuickVal = u32Sum;

	if (dv_ADC_3.ucOneLoopFlag == 0)
	{
		dv_ADC_3.ucOneLoopFlag++;
	}
	dv_ADC_3.fDMAStop(); 
}

static  void  ISR_CIM_DMA2_CH2(void)
{
	// May need initation
	ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA2_CH3(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA2_CH4(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_ETH(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_ETHWakeup(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_CAN2_TX(void)
{
	HAL_CAN_IRQHandler(&Can_2_Handle);	//CAN2 发
}

static  void  ISR_CIM_CAN2_RX0(void)
{
	HAL_CAN2_IRQHandler();				//CAN2 收
}

static  void  ISR_CIM_CAN2_RX1(void)
{
	HAL_CAN2_IRQHandler();				//CAN2 收
}

static  void  ISR_CIM_CAN2_SCE(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_OTG_FS(void)
{
    // May need initation
     HAL_HCD_IRQHandler(&hhcd);
}

static  void  ISR_CIM_DMA2_CH5(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA2_CH6(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DMA2_CH7(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static void  ISR_CIM_USART6(void)
{
	HAL_UART6_IRQHandler();
}

static  void  ISR_CIM_I2C3_EV(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_I2C3_ER(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_OTG_HS_EP1_OUT(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_OTG_HS_EP1_IN(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_OTG_HS_WKUP(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_OTG_HS(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_DCMI(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_CRYP(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_HASH_RNG(void)
{
    // May need initation
    ISR_CIM_Dummy();
}

static  void  ISR_CIM_FPU(void)
{
    // May need initation
    ISR_CIM_Dummy();
}