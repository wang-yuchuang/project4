#include "bsp_uart.h"
#include "bsp_can.h"
#include "bsp_adc.h"
#include "bsp_pwm.h"
#include "bsp_capture.h"
#include "bsp_eeprom.h"    
#include "bsp_fsmc.h"   
#include "base_def.h"   
#include "interface.h"
#include "main.h"

GPIO_InitTypeDef  GPIO_InitStruct;

UART_HandleTypeDef Uart1Handle;
UART_HandleTypeDef Uart2Handle;
UART_HandleTypeDef Uart3Handle;
UART_HandleTypeDef Uart4Handle;
UART_HandleTypeDef Uart5Handle;
UART_HandleTypeDef Uart6Handle;

extern INT8U USART1_Rx_Buffer[UART1_RECV_MAX_NUM];
extern INT8U USART2_Rx_Buffer[UART2_RECV_MAX_NUM];
extern INT8U USART3_Rx_Buffer[UART3_RECV_MAX_NUM];
extern INT8U UART4_Rx_Buffer[RS485_UART4_RX_MAX_NUM];
extern INT8U UART5_Rx_Buffer[RS485_UART5_RX_MAX_NUM];
extern INT8U USART6_Rx_Buffer[MDBUS_BYTES_MAX_NUM];

DMA_HandleTypeDef  hdma_usart1_tx;
DMA_HandleTypeDef  hdma_usart2_tx;
DMA_HandleTypeDef  hdma_usart3_tx;
DMA_HandleTypeDef  hdma_uart4_tx;
DMA_HandleTypeDef  hdma_uart5_tx;
DMA_HandleTypeDef  hdma_usart6_tx;

DMA_HandleTypeDef  hdma_usart1_rx;
DMA_HandleTypeDef  hdma_usart2_rx;
DMA_HandleTypeDef  hdma_usart3_rx;
DMA_HandleTypeDef  hdma_uart4_rx;
DMA_HandleTypeDef  hdma_uart5_rx;
DMA_HandleTypeDef  hdma_usart6_rx;




#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */

void HAL_TIM_Base_GPIO_MspInit(TIM_HandleTypeDef *htim)
{	
	if (htim->Instance == TIM2)
	{
		//GPIO_InitStruct.Pin = GPIO_Pin_8 | GPIO_Pin_5;  //PA5  ETR
		//GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		//HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  
		//A04			PA5			TIME2_CH1   
		GPIO_InitStruct.Pin = GPIO_Pin_5;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}


	if (htim->Instance == TIM8)
	{
		GPIO_InitStruct.Pin = GPIO_Pin_0;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	//A04			PE5			TIME9_CH1
	GPIO_InitStruct.Pin = GPIO_Pin_5;	
	GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 3, 1);
	HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);  

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
	GPIO_InitTypeDef  GPIO_InitStruct;  
	if (CAN1 == hcan->Instance)
	{
		GPIO_InitStruct.Pin = GPIO_Pin_9;//0x0200;		//GPIO_Pin_9
		GPIO_InitStruct.Mode = 0x02;
		GPIO_InitStruct.Speed = 0x02;
		GPIO_InitStruct.Pull = 0x01;	
		GPIO_InitStruct.Alternate = 0x09;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Pin_8;//0x0100;		//GPIO_Pin_8
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(CANx_RX_IRQn, 4, 0);
		HAL_NVIC_EnableIRQ(CANx_RX_IRQn);
	}
	else
	{
		GPIO_InitStruct.Pin = GPIO_Pin_6;	//GPIO_Pin_13
		GPIO_InitStruct.Mode = 0x02;
		GPIO_InitStruct.Speed = 0x02;
		GPIO_InitStruct.Pull = 0x01;
		GPIO_InitStruct.Alternate = 0x09;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Pin_12;  //GPIO_Pin_12;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(CAN2_RX_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(CAN2_RX_IRQn);
	}
}
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef  GPIO_InitStruct;  

	if(hi2c->Instance == I2C1)
	{
		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;      //此行原有

		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   //GPIO配置为输出

		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;         //强上拉

		HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);

		HAL_GPIO_WritePin(GPIOB, 6, GPIO_PIN_SET);       //拉高SCL
		HAL_GPIO_WritePin(GPIOB, 7, GPIO_PIN_SET);       //拉高SDA

		hi2c->Instance->CR1 = I2C_CR1_SWRST; //复位 I2C 控制器
		hi2c->Instance->CR1 = 0 ; //解除复位（不会自动清除）


		RCC->AHB1ENR |= 0x00000002;
		GPIO_InitStruct.Pin = GPIO_Pin_6 | GPIO_Pin_7;//0xc0;
		GPIO_InitStruct.Mode = 0x12;
		GPIO_InitStruct.Pull = 0x00;
		GPIO_InitStruct.Speed = 0x02;
		GPIO_InitStruct.Alternate = 0x04;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		RCC->APB1ENR |= 0x00200000;
		RCC->APB1RSTR |= (0x200000);
		RCC->APB1RSTR &= ~(0x200000);
	}
	else if(hi2c->Instance == I2C3)
	{
		INT8U i = 0u;
		__I2C3_CLK_ENABLE();
		/**************** Configure to pull up first *******************/
		GPIO_InitStruct.Pin = GPIO_PIN_8;      //此行原有
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   //GPIO配置为输出
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;         //强上拉
		HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_PIN_9;      //此行原有
		HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);

		for (i = 0u; i < 9; i++)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
      Delay(500);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
      Delay(500);
		}
		HAL_GPIO_WritePin(GPIOA, 8, GPIO_PIN_SET);       //拉高SCL
		HAL_GPIO_WritePin(GPIOC, 9, GPIO_PIN_SET);       //拉高SDA
		Delay(500);
		hi2c->Instance->CR1 |= I2C_CR1_SWRST; //复位 I2C 控制器
		Delay(500);
		hi2c->Instance->CR1 &= ~I2C_CR1_SWRST ; //解除复位（不会自动清除）
		/**************** Configure to pull up first *******************/
		
		GPIO_InitStruct.Pin = GPIO_Pin_8;//0x0100;//
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Pin_9;//0x0200;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		//RCC->APB1RSTR |= 0x800000;
		//RCC->APB1RSTR &= ~(0x800000);
	}

}
void I2C3_MspInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;  
	GPIO_InitStruct.Pin = GPIO_Pin_8;//0x0100;//
	GPIO_InitStruct.Mode = 0x12;
	GPIO_InitStruct.Pull = 0;
	GPIO_InitStruct.Speed = 2;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_Pin_9;//0x0200;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	RCC->APB1RSTR |= 0x800000;
	RCC->APB1RSTR &= ~(0x800000);
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	if(hi2c->Instance == I2C1)
	{
		GPIO_InitStruct.Pin = GPIO_Pin_6 | GPIO_Pin_7;//0xc0;
		GPIO_InitStruct.Mode = 1;
		GPIO_InitStruct.Pull = 1;
		GPIO_InitStruct.Speed = 2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOB, 0xc0, GPIO_PIN_SET); 		
	}
	else if(hi2c->Instance == I2C3)
	{
		GPIO_InitStruct.Pin = GPIO_Pin_8;//0x0100;
		GPIO_InitStruct.Mode = 1;
		GPIO_InitStruct.Pull = 1;
		GPIO_InitStruct.Speed = 2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Pin_9;//0x0200;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOA, 0x0100, GPIO_PIN_SET); 
		HAL_GPIO_WritePin(GPIOC, 0x0200, GPIO_PIN_SET); 
	}
}

void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
	GPIO_InitStruct.Mode      = 0x02;
	GPIO_InitStruct.Pull      = 0x01;
	GPIO_InitStruct.Speed     = 0x03;
	GPIO_InitStruct.Alternate = 0xC;
	GPIO_InitStruct.Pin   = 0xCFB3;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); 
	GPIO_InitStruct.Pin   = 0xFF83;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); 
	GPIO_InitStruct.Pin   = 0xF03F;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct); 
	GPIO_InitStruct.Pin   = 0x023F;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct); 
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{	
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	if (huart->Instance == UART4)
	{
		//UART4:	PC10--TX	PC11--RX
		GPIO_InitStruct.Pin       = GPIO_Pin_10;//0x0400;
		GPIO_InitStruct.Mode      = 0x02;
		GPIO_InitStruct.Pull      = 0x01;
		GPIO_InitStruct.Speed     = 0x02;
		GPIO_InitStruct.Alternate = 0x08;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		GPIO_InitStruct.Pin       = GPIO_Pin_11;//0x0800;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(UART4_IRQn, 6, 1);
		HAL_NVIC_EnableIRQ(UART4_IRQn);  
	}

	if (huart->Instance == UART5)
	{
		//UART5:	PC12-TX	PD2-RX
		GPIO_InitStruct.Pin       = GPIO_Pin_12;//0x1000;
		GPIO_InitStruct.Mode      = 0x02;
		GPIO_InitStruct.Pull      = 0x01;
		GPIO_InitStruct.Speed     = 0x02;
		GPIO_InitStruct.Alternate = 0x08;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Pin_2;//0x04;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(UART5_IRQn, 6, 2);
		HAL_NVIC_EnableIRQ(UART5_IRQn);
	}

	if (huart->Instance == USART6)
	{
		//UART6:	PC6--TX  PC7--RX
		GPIO_InitStruct.Pin       = GPIO_Pin_6;//0x40;
		GPIO_InitStruct.Mode      = 0x02;
		GPIO_InitStruct.Pull      = 0x01;
		GPIO_InitStruct.Speed     = 0x02;
		GPIO_InitStruct.Alternate = 0x08;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Pin_7;//0x80;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(USART6_IRQn, 3, 0);//6, 0
		HAL_NVIC_EnableIRQ(USART6_IRQn);
	}

	if (huart->Instance == USART1)
	{
		//UART1	PA10--RX PA9--TX
		GPIO_InitStruct.Pin = GPIO_Pin_10;
		GPIO_InitStruct.Mode      = 0x02;
		GPIO_InitStruct.Pull      = 0x01;
		GPIO_InitStruct.Speed     = 0x02;
		GPIO_InitStruct.Alternate = 0x07;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Pin_9;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(USART1_IRQn, 6, 3);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}

	if (huart->Instance == USART3)
	{
		//UART3 PB11--RX PB10-TX
		GPIO_InitStruct.Pin       = GPIO_Pin_10;//0x1000;
		GPIO_InitStruct.Mode      = 0x02;
		GPIO_InitStruct.Pull      = 0x01;
		GPIO_InitStruct.Speed     = 0x02;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Pin_11;//0x04;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(USART3_IRQn, 6, 4);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}

	if (huart->Instance == USART1)//DMA2_Channel4_Stream7 - Tx DMA2_Channel4_Stream5 - Rx
	{
		__DMA2_CLK_ENABLE();

		__HAL_LINKDMA(&Uart1Handle, hdmatx, hdma_usart1_tx); // 与串口引脚相连接
		hdma_usart1_tx.Instance = DMA2_Stream7;
		hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;  // 存储器到外设
		hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_usart1_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_usart1_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_usart1_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_tx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_usart1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		HAL_DMA_Init(&hdma_usart1_tx);
		HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 10);
		HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);//使能dma中断

		__HAL_LINKDMA(&Uart1Handle, hdmarx, hdma_usart1_rx); // 与串口引脚相连接
		hdma_usart1_rx.Instance = DMA2_Stream5;
		hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;  //外设到存储器
		hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_usart1_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_usart1_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_usart1_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_rx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		HAL_DMA_Init(&hdma_usart1_rx);
		HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 5, 7);
		HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);//使能dma中断

		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//使能uart1 IDLE中断
		HAL_UART_Receive_DMA(huart, USART1_Rx_Buffer, UART1_RECV_MAX_NUM);
	}
	else if (huart->Instance == USART2)//DMA2_Channel4_Stream7 - Tx DMA2_Channel4_Stream5 - Rx
	{
		__DMA1_CLK_ENABLE();

		__HAL_LINKDMA(&Uart2Handle, hdmatx, hdma_usart2_tx); // 与串口引脚相连接
		hdma_usart2_tx.Instance = DMA1_Stream6;
		hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;  // 存储器到外设
		hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_usart2_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_usart2_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_usart2_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart2_tx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_usart2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		HAL_DMA_Init(&hdma_usart2_tx);
		HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 8);
		HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);//使能dma中断

		__HAL_LINKDMA(&Uart2Handle, hdmarx, hdma_usart2_rx); // 与串口引脚相连接
		hdma_usart2_rx.Instance = DMA1_Stream5;
		hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;  //外设到存储器
		hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_usart2_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_usart2_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_usart2_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart2_rx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_usart2_rx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		HAL_DMA_Init(&hdma_usart2_rx);
		HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 9);
		HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);//使能dma中断

		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//使能uart2 IDLE中断
		HAL_UART_Receive_DMA(huart, USART2_Rx_Buffer, UART2_RECV_MAX_NUM);
	}
	else if (huart->Instance == USART3)//DMA1_Channel4_Stream3 - Tx DMA1_Channel4_Stream1 - Rx
	{
		__DMA1_CLK_ENABLE();

		__HAL_LINKDMA(&Uart3Handle, hdmatx, hdma_usart3_tx); // 与串口引脚相连接
		hdma_usart3_tx.Instance = DMA1_Stream3;
		hdma_usart3_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;  // 存储器到外设
		hdma_usart3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_usart3_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_usart3_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_usart3_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_usart3_tx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_usart3_tx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_usart3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart3_tx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_usart3_tx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_usart3_tx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		HAL_DMA_Init(&hdma_usart3_tx);
		HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 6);
		HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);//使能dma中断
		
#if (CFG_Usart3_use_for_trace == 0u)
		__HAL_LINKDMA(&Uart3Handle, hdmarx, hdma_usart3_rx); // 与串口引脚相连接
		hdma_usart3_rx.Instance = DMA1_Stream1;
		hdma_usart3_rx.Init.Channel = DMA_CHANNEL_4;
		hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;  //外设到存储器
		hdma_usart3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_usart3_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_usart3_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_usart3_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart3_rx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_usart3_rx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_usart3_rx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		HAL_DMA_Init(&hdma_usart3_rx);
		HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 11);
		HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);//使能dma中断

		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//使能uart3 IDLE中断
		HAL_UART_Receive_DMA(huart, USART3_Rx_Buffer, UART3_RECV_MAX_NUM);
#endif
	}
	else if (huart->Instance == UART4)//DMA1_Channel4_Stream4 - Tx DMA1_Channel4_Stream2 - Rx
	{
#if (CFG_Uart4_send_by_DMA)
		__DMA1_CLK_ENABLE();

		__HAL_LINKDMA(&Uart4Handle, hdmatx, hdma_uart4_tx); // 与串口引脚相连接
		hdma_uart4_tx.Instance = DMA1_Stream4;
		hdma_uart4_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_uart4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;  // 存储器到外设
		hdma_uart4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_uart4_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_uart4_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_uart4_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_uart4_tx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_uart4_tx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_uart4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_uart4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_uart4_tx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_uart4_tx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_uart4_tx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		//HAL_DMA_DeInit(&hdma_uart4_tx);
		HAL_DMA_Init(&hdma_uart4_tx);
		HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 1);
		HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);//使能dma中断

		__HAL_LINKDMA(&Uart4Handle, hdmarx, hdma_uart4_rx); // 与串口引脚相连接
		hdma_uart4_rx.Instance = DMA1_Stream2;
		hdma_uart4_rx.Init.Channel = DMA_CHANNEL_4;
		hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;  //外设到存储器
		hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_uart4_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_uart4_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_uart4_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_uart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_uart4_rx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_uart4_rx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_uart4_rx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		//HAL_DMA_DeInit(&hdma_uart4_rx);
		HAL_DMA_Init(&hdma_uart4_rx);
		HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 3);
		HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);//使能dma中断

		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//使能uart5 IDLE中断
		HAL_UART_Receive_DMA(huart, UART4_Rx_Buffer, RS485_UART4_RX_MAX_NUM);
#endif
	}
	else if (huart->Instance == UART5)//DMA1_Channel4_Stream7 - Tx DMA1_Channel4_Stream0 - Rx
	{
#if (CFG_Uart5_send_by_DMA)
		__DMA1_CLK_ENABLE();

		__HAL_LINKDMA(&Uart5Handle, hdmatx, hdma_uart5_tx); // 与串口引脚相连接
		hdma_uart5_tx.Instance = DMA1_Stream7;
		hdma_uart5_tx.Init.Channel = DMA_CHANNEL_4;
		hdma_uart5_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;  // 存储器到外设
		hdma_uart5_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_uart5_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_uart5_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_uart5_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_uart5_tx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_uart5_tx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_uart5_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_uart5_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_uart5_tx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_uart5_tx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_uart5_tx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		//HAL_DMA_DeInit(&hdma_uart5_tx);
		HAL_DMA_Init(&hdma_uart5_tx);
		HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 4);
		HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);//使能dma中断
		
		__HAL_LINKDMA(&Uart5Handle, hdmarx, hdma_uart5_rx); // 与串口引脚相连接
		hdma_uart5_rx.Instance = DMA1_Stream0;
		hdma_uart5_rx.Init.Channel = DMA_CHANNEL_4;
		hdma_uart5_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;  //外设到存储器
		hdma_uart5_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_uart5_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_uart5_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_uart5_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_uart5_rx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_uart5_rx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_uart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_uart5_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_uart5_rx.Init.Mode = DMA_NORMAL;                 // 正常模式
		hdma_uart5_rx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		//hdma_uart5_rx.Init.Priority = DMA_PRIORITY_MEDIUM;      // 优先级中
		//HAL_DMA_DeInit(&hdma_uart5_rx);
		HAL_DMA_Init(&hdma_uart5_rx);
		HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 2);
		HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);//使能dma中断

		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);//使能uart5 IDLE中断
		HAL_UART_Receive_DMA(huart, UART5_Rx_Buffer, RS485_UART5_RX_MAX_NUM);
#endif
	}
#if (CFG_Uart6_send_by_DMA == 1u)
	else if (huart->Instance == USART6)//DMA2_Channel5_Stream6 - Tx
	{
		__DMA2_CLK_ENABLE();

		__HAL_LINKDMA(&Uart6Handle, hdmatx, hdma_usart6_tx); // 与串口引脚相连接
		hdma_usart6_tx.Instance = DMA2_Stream6;
		hdma_usart6_tx.Init.Channel = DMA_CHANNEL_5;
		hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;  // 存储器到外设
		hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		hdma_usart6_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		hdma_usart6_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_usart6_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;      // 外设非增量
		hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;          // 存储器增强
		hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart6_tx.Init.Mode = DMA_NORMAL;                 // 正常模式
		//hdma_usart6_tx.Init.Priority = DMA_PRIORITY_LOW;       // 优先级低
		hdma_usart6_tx.Init.Priority = DMA_PRIORITY_HIGH;      // 优先级中
		//HAL_DMA_DeInit(&hdma_usart6_tx);
		HAL_DMA_Init(&hdma_usart6_tx);
		HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);//使能dma中断
	}
#endif
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	static DMA_HandleTypeDef  hdma_adc;
	static DMA_HandleTypeDef  hdma_adc_3;

	if (ADC1 == hadc->Instance)
	{
        //extern unsigned char  u8Is1000VHardwareA00(void);
		GPIO_InitStruct.Pin =  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6;												
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP | GPIO_MODE_AF_PP;								
		GPIO_InitStruct.Pull = 0x00;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


		GPIO_InitStruct.Pin = GPIO_Pin_0;															//PB0		ad_curr
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        //extern unsigned char  u8Is1000VHardwareA00(void);
		GPIO_InitStruct.Pin = GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_2;		
	
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);  

		hdma_adc.Instance = DMA2_Stream0;
		hdma_adc.Init.Channel  = DMA_CHANNEL_0;
		hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_adc.Init.Mode = DMA_CIRCULAR;					//0x0100;
		hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;			//0x020000;
		hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;         
		hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
		hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
		hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE; 
		HAL_DMA_Init(&hdma_adc);
		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);
		HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 12);   
		HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	}
	else if (ADC3 == hadc->Instance)
	{
		//A04
		GPIO_InitStruct.Pin = GPIO_Pin_9 | GPIO_Pin_7| GPIO_Pin_10;									//A04	PA5  PA6 用于输入捕捉功能！
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP | GPIO_MODE_AF_PP;								//0x03;
		GPIO_InitStruct.Pull = 0x00;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

		hdma_adc_3.Instance = DMA2_Stream1;
		hdma_adc_3.Init.Channel  = DMA_CHANNEL_2;
		hdma_adc_3.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc_3.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc_3.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc_3.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_adc_3.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_adc_3.Init.Mode = DMA_CIRCULAR;								//0x0100;
		hdma_adc_3.Init.Priority = DMA_PRIORITY_HIGH;						//0x020000;
		hdma_adc_3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;					//0;         
		hdma_adc_3.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;		//1;
		hdma_adc_3.Init.MemBurst = DMA_MBURST_SINGLE;						//0;
		hdma_adc_3.Init.PeriphBurst = DMA_PBURST_SINGLE;					//0; 
		HAL_DMA_Init(&hdma_adc_3);
		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc_3);
		HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 13);   
		HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
	}
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

  GPIO_InitStruct.Mode = 0x02;
  GPIO_InitStruct.Pull = 0x01;
  GPIO_InitStruct.Speed = 0x03;
  GPIO_InitStruct.Alternate = 0x02;
  GPIO_InitStruct.Pin = GPIO_Pin_8; //0x0100;	PC8 PWMG2		PF8 PWMG2  
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  GPIO_InitStruct.Alternate = 0x09;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_Pin_6; 
  GPIO_InitStruct.Alternate = 0x03;					//TIME10
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);			//PF6 AC PWM
}

