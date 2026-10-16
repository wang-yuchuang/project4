
/*
  *****************************************************************************
  *
  * Name:bsp_uart_hal.h
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

#ifndef __BSP_UART_HAL_H
#define __BSP_UART_HAL_H

// ================* Include Files ************************
#include "includes.h"
#include <STM32F4xx_HAL_RCC_EX.H>

// ================* Definitions and Marcos ***************

#define UART_BUF_RX_SIZE   256
#define UART_BUF_TX_SIZE   256

typedef enum 
{
  USART_RS232,
  UART_485A,
  UART_485B,
  USART_DBG,
  NUM_USARTS
}usart_peripherals_e;

typedef  struct 
{
  USART_TypeDef*        usart_peri;
  IRQn_Type             irq;
  GPIO_TypeDef*         tx_port;
  GPIO_TypeDef*         rx_port;
  GPIO_TypeDef*         rts_port;
  GPIO_TypeDef*         cts_port;
  uint16_t              alt_func;
  uint16_t              tx_pin;
  uint16_t              rx_pin;
  uint16_t              cts_pin;
  uint16_t              rts_pin;
  UART_InitTypeDef      uart_setting;
//  void                  (*fp)(uint8_t);
}usart_conf_s;

typedef enum
{
  UART_1200_BAUD,
  UART_9600_BAUD,
  UART_19200_BAUD,
  UART_57600_BAUD,
  UART_115200_BAUD,
  UART_NUM_BUAD
}uart_baud_e;

typedef enum  {
  UART_BUF_STATUS_FREE,
  UART_BUF_STATUS_INUSE,
  UART_BUF_STATUS_DISABLED,
  UART_BUF_NUM_STATUS
} uart_buf_status_e;

typedef enum  {
  UART_STATUS_BUSY,
  UART_STATUS_FREE,
  UART_STATUS_ERROR,
  UART_NUM_STATUS
} uart_status_e;


typedef struct
{
  uint8_t au8RxBuf[UART_BUF_RX_SIZE];
  uint8_t au8TxBuf[UART_BUF_TX_SIZE];
  uint16_t u16RxBufCnt;
  uint16_t u16TxBufCnt;
  uint16_t u16ISRTxCnt;
  uint8_t  u8TxComplete;
  uint8_t  u8Error;
  uint8_t  u8RxStatus;
  uint8_t  u8TxStatus;
}uart_data_s;

// RS232 Port
#define M530S_RS232_PERIPHERAL                  USART6
#define M530S_RS232_CLK_ENABLE()                __USART6_CLK_ENABLE()
#define M530S_RS232_CLK_DISABLE()               __USART6_CLK_DISABLE()
#define M530S_RS232_AF                          GPIO_AF8_USART6
#define M530S_RS232_IRQn                        USART6_IRQn

#define M530S_RS232_TX_PIN                      GPIO_PIN_6
#define M530S_RS232_TX_GPIO_PORT                GPIOC
#define M530S_RS232_TX_GPIO_CLK_ENABLE()        __GPIOC_CLK_ENABLE()
#define M530S_RS232_TX_GPIO_CLK_DISABLE()       __GPIOC_CLK_DISABLE()

#define M530S_RS232_RX_PIN                      GPIO_PIN_7
#define M530S_RS232_RX_GPIO_PORT                GPIOC
#define M530S_RS232_RX_GPIO_CLK_ENABLE()        __GPIOC_CLK_ENABLE()
#define M530S_RS232_RX_GPIO_CLK_DISABLE()       __GPIOC_CLK_DISABLE()

#define M530S_RS232_CTS_PIN                     GPIO_PIN_15
#define M530S_RS232_CTS_GPIO_PORT               GPIOG
#define M530S_RS232_CTS_GPIO_CLK_ENABLE()       __GPIOG_CLK_ENABLE()
#define M530S_RS232_CTS_GPIO_CLK_DISABLE()      __GPIOG_CLK_DISABLE()

#define M530S_RS232_RTS_PIN                     GPIO_PIN_8
#define M530S_RS232_RTS_GPIO_PORT               GPIOG
#define M530S_RS232_RTS_GPIO_CLK_ENABLE()       __GPIOG_CLK_ENABLE()
#define M530S_RS232_RTS_GPIO_CLK_DISABLE()      __GPIOG_CLK_DISABLE()

// RS485 A Port
#define M530S_RS485A_PERIPHERAL                 UART4 
#define M530S_RS485A_CLK_ENABLE()               __UART4_CLK_ENABLE()
#define M530S_RS485A_CLK_DISABLE()              __UART4_CLK_DISABLE()
#define M530S_RS485A_AF                         GPIO_AF8_UART4
#define M530S_RS485A_IRQn                       UART4_IRQn

#define M530S_RS485A_TX_PIN                     GPIO_PIN_10
#define M530S_RS485A_TX_GPIO_PORT               GPIOC
#define M530S_RS485A_TX_GPIO_CLK_ENABLE()       __GPIOC_CLK_ENABLE()
#define M530S_RS485A_TX_GPIO_CLK_DISABLE()      __GPIOC_CLK_DISABLE()

#define M530S_RS485A_RX_PIN                     GPIO_PIN_11
#define M530S_RS485A_RX_GPIO_PORT               GPIOC
#define M530S_RS485A_RX_GPIO_CLK_ENABLE()       __GPIOC_CLK_ENABLE()
#define M530S_RS485A_RX_GPIO_CLK_DISABLE()      __GPIOC_CLK_DISABLE()

#define M530S_RS485A_SWITCH_PIN                 GPIO_PIN_3
#define M530S_RS485A_SWITCH_GPIO_PORT           GPIOD
#define M530S_RS485A_SWITCH_GPIO_CLK_ENABLE()   __GPIOD_CLK_ENABLE()
#define M530S_RS485A_SWITCH_GPIO_CLK_DISABLE()  __GPIOD_CLK_DISABLE()

#define RS485A_SWITCH_TO_RX()                   HAL_GPIO_WritePin(M530S_RS485A_SWITCH_GPIO_PORT, M530S_RS485A_SWITCH_PIN, GPIO_PIN_RESET)
#define RS485A_SWITCH_TO_TX()                   HAL_GPIO_WritePin(M530S_RS485A_SWITCH_GPIO_PORT, M530S_RS485A_SWITCH_PIN, GPIO_PIN_SET)

// RS485 B Port
#define M530S_RS485B_PERIPHERAL                 UART5
#define M530S_RS485B_CLK_ENABLE()               __UART5_CLK_ENABLE()
#define M530S_RS485B_CLK_DISABLE()              __UART5_CLK_DISABLE()
#define M530S_RS485B_AF                         GPIO_AF8_UART5
#define M530S_RS485B_IRQn                       UART5_IRQn

#define M530S_RS485B_TX_PIN                     GPIO_PIN_12
#define M530S_RS485B_TX_GPIO_PORT               GPIOC
#define M530S_RS485B_TX_GPIO_CLK_ENABLE()       __GPIOC_CLK_ENABLE()
#define M530S_RS485B_TX_GPIO_CLK_DISABLE()      __GPIOC_CLK_DISABLE()

#define M530S_RS485B_RX_PIN                     GPIO_PIN_2
#define M530S_RS485B_RX_GPIO_PORT               GPIOD
#define M530S_RS485B_RX_GPIO_CLK_ENABLE()       __GPIOD_CLK_ENABLE()
#define M530S_RS485B_RX_GPIO_CLK_DISABLE()      __GPIOD_CLK_DISABLE()

#define M530S_RS485B_SWITCH_PIN                 GPIO_PIN_6
#define M530S_RS485B_SWITCH_GPIO_PORT           GPIOD
#define M530S_RS485B_SWITCH_GPIO_CLK_ENABLE()   __GPIOD_CLK_ENABLE()
#define M530S_RS485B_SWITCH_GPIO_CLK_DISABLE()  __GPIOD_CLK_DISABLE()

#define RS485B_SWITCH_TO_RX()                   HAL_GPIO_WritePin(M530S_RS485B_SWITCH_GPIO_PORT, M530S_RS485B_SWITCH_PIN, GPIO_PIN_RESET)
#define RS485B_SWITCH_TO_TX()                   HAL_GPIO_WritePin(M530S_RS485B_SWITCH_GPIO_PORT, M530S_RS485B_SWITCH_PIN, GPIO_PIN_SET)

// Debug Port
#define M530S_RSDBG_PERIPHERAL                  USART1
#define M530S_RSDBG_CLK_ENABLE()                __USART1_CLK_ENABLE()
#define M530S_RSDBG_CLK_DISABLE()               __USART1_CLK_DISABLE()
#define M530S_RSDBG_AF                          GPIO_AF7_USART1
#define M530S_RSDBG_IRQn                        USART1_IRQn

#define M530S_RSDBG_TX_PIN                      GPIO_PIN_9
#define M530S_RSDBG_TX_GPIO_PORT                GPIOA
#define M530S_RSDBG_TX_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()
#define M530S_RSDBG_TX_GPIO_CLK_DISABLE()       __GPIOA_CLK_DISABLE()

#define M530S_RSDBG_RX_PIN                      GPIO_PIN_10
#define M530S_RSDBG_RX_GPIO_PORT                GPIOA
#define M530S_RSDBG_RX_GPIO_CLK_ENABLE()        __GPIOA_CLK_ENABLE()
#define M530S_RSDBG_RX_GPIO_CLK_DISABLE()       __GPIOA_CLK_DISABLE()

#define M530S_RS232_DFLT_BAUD                   UART_57600_BAUD
#define M530S_RS485A_DFLT_BAUD                  UART_115200_BAUD
#define M530S_RS485B_DFLT_BAUD                  UART_19200_BAUD
#define M530S_RSDBG_DFLT_BAUD                   UART_9600_BAUD

#define M530S_RS232_DFLT_WLEN                   UART_WORDLENGTH_8B
#define M530S_RS485A_DFLT_WLEN                  UART_WORDLENGTH_8B
#define M530S_RS485B_DFLT_WLEN                  UART_WORDLENGTH_8B
#define M530S_RSDBG_DFLT_WLEN                   UART_WORDLENGTH_8B

#define M530S_RS232_DFLT_STPBITS                UART_STOPBITS_1
#define M530S_RS485A_DFLT_STPBITS               UART_STOPBITS_1
#define M530S_RS485B_DFLT_STPBITS               UART_STOPBITS_1
#define M530S_RSDBG_DFLT_STPBITS                UART_STOPBITS_1

#define M530S_RS232_DFLT_PARITY                 UART_PARITY_NONE
#define M530S_RS485A_DFLT_PARITY                UART_PARITY_NONE
#define M530S_RS485B_DFLT_PARITY                UART_PARITY_NONE
#define M530S_RSDBG_DFLT_PARITY                 UART_PARITY_NONE

//#define M530S_RS232_DFLT_FLOWCNTRL              UART_HWCONTROL_RTS_CTS
#define M530S_RS232_DFLT_FLOWCNTRL              UART_HWCONTROL_NONE
#define M530S_RS485A_DFLT_FLOWCNTRL             UART_HWCONTROL_NONE
#define M530S_RS485B_DFLT_FLOWCNTRL             UART_HWCONTROL_NONE
#define M530S_RSDBG_DFLT_FLOWCNTRL              UART_HWCONTROL_NONE

// ================* Variables ****************************
extern const usart_conf_s usart_conf[NUM_USARTS];
extern uart_data_s uart_data_485A, uart_data_485B, uart_data_232, uart_data_dbg;

extern UART_HandleTypeDef UartRS232Handle;
extern UART_HandleTypeDef Uart485AHandle;
extern UART_HandleTypeDef Uart485BHandle;
extern UART_HandleTypeDef UartDBGHandle;

// ================* Function Prototypes ******************
// Function Prototypes
void bsp_InitUart(void);
void bsp_uart_Enable(uint8_t uart_no);
void bsp_uart_Disable(uint8_t uart_no);
void bsp_uart_485A_ISR(void);
void bsp_uart_485B_ISR(void);
void bsp_uart_232_ISR(void);
void bsp_uart_dbg_ISR(void);
void uart_data_init(uint8_t uart_no);

// Interface
void bsp_uart_DataRecv(void);
void bsp_uart_DataSend_Blocking(uint8_t uart, uint8_t *pu8Buffer, uint16_t ulCount);
void bsp_uart_DataSend_NonBlocking(uint8_t uart, uint8_t *pu8Buffer, uint16_t ulCount);
uint8_t bsp_uart_IsDataSendComplete(uint8_t uart);
void bsp_CfgUartISR(void);
#endif // __BSP_UART_232_H
// End of file
