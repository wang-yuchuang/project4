#include "bsp_uart.h"
#include "main.h"

/* UART handler declaration */
//UART_HandleTypeDef Uart1Handle;
//UART_HandleTypeDef Uart2Handle;
//UART_HandleTypeDef Uart4Handle;
//UART_HandleTypeDef Uart5Handle;
//UART_HandleTypeDef Uart6Handle;

extern UART_HandleTypeDef   Uart1Handle;
extern UART_HandleTypeDef   Uart2Handle;
extern UART_HandleTypeDef   Uart3Handle;
extern UART_HandleTypeDef   Uart4Handle;
extern UART_HandleTypeDef   Uart5Handle;
extern UART_HandleTypeDef   Uart6Handle;


/* Buffer used for transmission */

/* Buffer used for reception */

const uint32_t  u32BaudRate[7]={9600,4800,2400,1200,19200,38400,115200};

//static GPIO_InitTypeDef  GPIO_InitStruct;



/************************************************************************************************************
 * @brief  初始化UART1串口
 * @param  u8BaudRateOffset 波特率偏移量(不能超过m115200)
 * @retval HAL_OK 初始化成功
 * @retval HAL_ERROR 初始化失败
 ************************************************************************************************************/
int8_t BSP_Uart1Init(uint8_t u8BaudRateOffset)
{	
	if(u8BaudRateOffset > m115200)
	{
		u8BaudRateOffset= m115200;
	}
	Uart1Handle.Instance        = USART1;
	Uart1Handle.Init.BaudRate   = u32BaudRate[u8BaudRateOffset];
	Uart1Handle.Init.WordLength = 0x00;
	Uart1Handle.Init.StopBits   = 0x00;
	Uart1Handle.Init.Parity     = 0x00;
	Uart1Handle.Init.HwFlowCtl  = 0x00;
	Uart1Handle.Init.Mode       = 0x0C;

	if(HAL_UART_Init(&Uart1Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

/************************************************************************************************************
 * @brief  初始化UART2串口
 * @param  u8BaudRateOffset 波特率偏移量(不能超过m115200)
 * @retval HAL_OK 初始化成功, HAL_ERROR 初始化失败
 ************************************************************************************************************/
int8_t BSP_Uart2Init(uint8_t u8BaudRateOffset)
{	
	if(u8BaudRateOffset > m115200)
	{
		u8BaudRateOffset = m115200;
	}
	Uart2Handle.Instance        = USART2;
	Uart2Handle.Init.BaudRate   = u32BaudRate[u8BaudRateOffset];
	Uart2Handle.Init.WordLength = 0x00;
	Uart2Handle.Init.StopBits   = 0x00;
	Uart2Handle.Init.Parity     = 0x00;
	Uart2Handle.Init.HwFlowCtl  = 0x00;
	Uart2Handle.Init.Mode       = 0x0004;//#define UART_MODE_TX_RX	((uint32_t)(USART_CR1_TE |USART_CR1_RE))

	if(HAL_UART_Init(&Uart2Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

/************************************************************************************************************
 * @brief 初始化UART3接口
 * @param u8BaudRateOffset 波特率偏移量(不能超过m115200)
 * @return 执行状态
 *   @retval HAL_OK 初始化成功
 *   @retval HAL_ERROR 初始化失败
 ************************************************************************************************************/
int8_t BSP_Uart3Init(uint8_t u8BaudRateOffset)
{	
	Uart3Handle.State = HAL_UART_STATE_RESET;

	if(u8BaudRateOffset > m115200)
	{
		u8BaudRateOffset= m115200;
	}
	Uart3Handle.Instance        = USART3;
	Uart3Handle.Init.BaudRate   = u32BaudRate[u8BaudRateOffset];

	Uart3Handle.Init.StopBits   = UART_STOPBITS_1;//1个停止位
	Uart3Handle.Init.WordLength = 0;							//USART_WordLength_9b;//0x00;
	Uart3Handle.Init.Parity     = UART_PARITY_NONE;

	Uart3Handle.Init.HwFlowCtl  = 0x00;
	Uart3Handle.Init.Mode       = UART_MODE_TX_RX;

	if(HAL_UART_Init(&Uart3Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	
	return HAL_OK;
}

/************************************************************************************************************
 * @brief  初始化UART4串口
 * @param  u8BaudRateOffset 波特率偏移量(不能超过m115200)
 * @param  ParityCase 校验方式(UARTX_PARITY_EVEN表示偶校验，其他值为无校验)
 * @retval HAL_OK 初始化成功
 * @retval HAL_ERROR 初始化失败
 * @note   1. 默认配置: 1个停止位, 无硬件流控, 全双工模式
 *         2. 初始化后会设置RS485为接收模式
 ************************************************************************************************************/
int8_t BSP_Uart4Init(uint8_t u8BaudRateOffset, uint8_t ParityCase)
{
	Uart4Handle.State = HAL_UART_STATE_RESET;

	if(u8BaudRateOffset > m115200)
	{
		u8BaudRateOffset= m115200;
	}
	Uart4Handle.Instance        = UART4;
	Uart4Handle.Init.BaudRate   = u32BaudRate[u8BaudRateOffset];

	Uart4Handle.Init.StopBits   = 0x00;						//1个停止位
	if (UARTX_PARITY_EVEN == ParityCase)
	{
		Uart4Handle.Init.WordLength = 0x1000;					//USART_WordLength_9b;//0x00;
		Uart4Handle.Init.Parity     = UART_PARITY_EVEN;				//SERIAL_PARITY_EVEN;//0x00;
	}
	else
	{
		Uart4Handle.Init.WordLength = 0;					//USART_WordLength_9b;//0x00;
		Uart4Handle.Init.Parity     = UART_PARITY_NONE;
	}

	Uart4Handle.Init.HwFlowCtl  = 0x00;
	Uart4Handle.Init.Mode       = 0x0C;

	if(HAL_UART_Init(&Uart4Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	BSP_Uart4_RS485_1_Ctrl(RS485_CTRL_RX);

	//if(u8BaudRateOffset > m115200)
	//{
	//	u8BaudRateOffset= m115200;
	//}
	//Uart4Handle.Instance        = UART4;
	//Uart4Handle.Init.BaudRate   = u32BaudRate[u8BaudRateOffset];
	//Uart4Handle.Init.WordLength = 0x00;
	//Uart4Handle.Init.StopBits   = 0x00;
	//Uart4Handle.Init.Parity     = 0x00;
	//Uart4Handle.Init.HwFlowCtl  = 0x00;
	//Uart4Handle.Init.Mode       = 0x0C;
	//
	//if(HAL_UART_Init(&Uart4Handle) != HAL_OK)
	//{
	//	return HAL_ERROR;
	//}
	//BSP_Uart4_RS485_1_Ctrl(RS485_CTRL_RX);
	return HAL_OK;
}

/************************************************************************************************************
 * @brief  初始化UART5串口
 * @param  u8BaudRateOffset 波特率偏移量(不能超过m115200)
 * @param  ParityCase 校验模式(UARTX_PARITY_EVEN表示偶校验)
 * @retval HAL_OK 初始化成功, HAL_ERROR 初始化失败
 * @note   初始化后会默认将RS485设置为接收模式
 ************************************************************************************************************/
int8_t BSP_Uart5Init(uint8_t u8BaudRateOffset, uint8_t ParityCase)
{
	Uart5Handle.State = HAL_UART_STATE_RESET;

	if(u8BaudRateOffset > m115200)
	{
		u8BaudRateOffset= m115200;
	}
	Uart5Handle.Instance        = UART5;
	Uart5Handle.Init.BaudRate   = u32BaudRate[u8BaudRateOffset];
	
	Uart5Handle.Init.StopBits   = 0x00;						//1个停止位
	if (UARTX_PARITY_EVEN == ParityCase)
	{
		Uart5Handle.Init.WordLength = 0x1000;					//USART_WordLength_9b;//0x00;
		Uart5Handle.Init.Parity     = UART_PARITY_EVEN;				//SERIAL_PARITY_EVEN;//0x00;
	}
	else
	{
		Uart5Handle.Init.WordLength = 0;					//USART_WordLength_9b;//0x00;
		Uart5Handle.Init.Parity     = UART_PARITY_NONE;
	}
	
	Uart5Handle.Init.HwFlowCtl  = 0x00;
	Uart5Handle.Init.Mode       = 0x0C;
	
	if(HAL_UART_Init(&Uart5Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	BSP_Uart5_RS485_2_Ctrl(RS485_CTRL_RX);
	return HAL_OK;
}
/************************************************************************************************************
 * @brief  初始化UART6接口
 * @param  u8BaudRateOffset 波特率偏移量(不能超过m115200)
 * @retval HAL_OK 初始化成功
 * @retval HAL_ERROR 初始化失败
 * @note   默认配置: 无校验位, 1位停止位, 无硬件流控
 ************************************************************************************************************/
int8_t BSP_Uart6Init(uint8_t u8BaudRateOffset)
{
	if (u8BaudRateOffset > m115200)
	{
		u8BaudRateOffset= m115200;
	}
	Uart6Handle.Instance        = USART6;
	Uart6Handle.Init.BaudRate   = u32BaudRate[u8BaudRateOffset];

	Uart6Handle.Init.StopBits   = 0x00;

	//Uart6Handle.Init.WordLength = 0x1000;						//USART_WordLength_9b;//0x00;
	//Uart6Handle.Init.Parity     = UART_PARITY_EVEN;				//SERIAL_PARITY_EVEN;//0x00;
	Uart6Handle.Init.WordLength = 0x00;
	Uart6Handle.Init.Parity     = UART_PARITY_NONE;

	Uart6Handle.Init.HwFlowCtl  = 0x00;
	Uart6Handle.Init.Mode       = 0x0C;
	
	if(HAL_UART_Init(&Uart6Handle) != HAL_OK)
	{
		return HAL_ERROR;
	}
	//BSP_Uart6_RTS_Ctrl(GPIO_PIN_SET);
	//BSP_Uart6_RS485_3_Ctrl(RS485_CTRL_RX);
	return HAL_OK;
}

/************************************************************************************************************
 * @brief 使能UART5接收功能
 *
 * 该函数通过调用底层驱动接口TXY_UART_Receive_Enable，使能UART5的接收功能。
 ************************************************************************************************************/
void BSP_Uart5RxEnable(void)
{
	TXY_UART_Receive_Enable(&Uart5Handle);
}

/************************************************************************************************************
 * @brief  使能UART4接收功能
 * @note   调用TXY_UART_Receive_Enable函数来启用UART4的接收功能
 ************************************************************************************************************/
void BSP_Uart4RxEnable(void)
{
	TXY_UART_Receive_Enable(&Uart4Handle);
}

/************************************************************************************************************
 * @brief 使能UART1接收功能
 *
 * 该函数通过调用底层驱动接口TXY_UART_Receive_Enable()来开启UART1的接收功能。
 ************************************************************************************************************/
void BSP_Uart1RxEnable(void)
{
	TXY_UART_Receive_Enable(&Uart1Handle);
}

/************************************************************************************************************
 * @brief  使能UART2接收功能
 * @note   调用TXY_UART_Receive_Enable函数来启用UART2的接收功能
 ************************************************************************************************************/
void BSP_Uart2RxEnable(void)
{
	TXY_UART_Receive_Enable(&Uart2Handle);
}

/************************************************************************************************************
 * @brief 使能UART6接收功能
 *
 * 该函数通过调用底层接口TXY_UART_Receive_Enable()来启用UART6的接收功能。
 ************************************************************************************************************/
void BSP_Uart6RxEnable(void)
{
	TXY_UART_Receive_Enable(&Uart6Handle);
}

/************************************************************************************************************
 * @brief  通过UART4以中断方式发送数据
 * @param  pbuffer: 指向要发送的数据缓冲区的指针
 * @param  Size: 要发送的数据大小(字节数)
 * @retval HAL_OK: 发送成功
 *         HAL_ERROR: 发送失败
 ************************************************************************************************************/
int8_t BSP_Uart4Tx(uint8_t *pbuffer, uint16_t Size)
{
	if (HAL_UART_Transmit_IT(&Uart4Handle, pbuffer, Size) != HAL_OK)
	{
        return HAL_ERROR;
    }
    return HAL_OK;
}

/************************************************************************************************************
 * @brief 通过UART5发送数据（非阻塞式）
 * @param pbuffer 指向要发送的数据缓冲区的指针
 * @param Size 要发送的数据大小（字节数）
 * @retval HAL_OK 发送成功
 * @retval HAL_ERROR 发送失败
 ************************************************************************************************************/
int8_t BSP_Uart5Tx(uint8_t *pbuffer, uint16_t Size)
{
	if (HAL_UART_Transmit_IT(&Uart5Handle, pbuffer, Size) != HAL_OK)
	// if(HAL_UART_Transmit_IT(&Uart5Handle, (uint8_t *)aTxStartMessage, Size)!= HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}
/************************************************************************************************************
 * @brief 通过UART6发送数据（中断模式）
 * @param pbuffer 指向要发送的数据缓冲区的指针
 * @param Size 要发送的数据大小（字节数）
 * @retval HAL_OK 发送成功
 * @retval HAL_ERROR 发送失败
 ************************************************************************************************************/
int8_t BSP_Uart6Tx(uint8_t *pbuffer, uint16_t Size)
{
	if (HAL_UART_Transmit_IT(&Uart6Handle, pbuffer, Size) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}


/************************************************************************************************************
 * @brief  通过UART4接收数据（中断模式）
 * @param  pbuffer: 接收数据缓冲区指针
 * @param  Size: 要接收的数据大小
 * @retval 执行状态: HAL_OK(成功) / HAL_ERROR(失败)
 ************************************************************************************************************/
int8_t BSP_Uart4Rx(uint8_t *pbuffer, uint16_t Size)
{
	if (HAL_UART_Receive_IT(&Uart4Handle, pbuffer, Size) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}


/************************************************************************************************************
 * @brief  通过UART5接收数据（中断模式）
 * @param  pbuffer: 接收数据缓冲区指针
 * @param  Size: 要接收的数据大小
 * @retval HAL_OK: 接收启动成功
 *         HAL_ERROR: 接收启动失败
 ************************************************************************************************************/
int8_t BSP_Uart5Rx(uint8_t *pbuffer, uint16_t Size)
{
	// if(HAL_UART_Receive_IT(&Uart5Handle, (uint8_t *)gs_Uart5.RxBuffer, UART_RX_BUFFER_SIZE) != HAL_OK)
	if (HAL_UART_Receive_IT(&Uart5Handle, pbuffer, Size) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

/************************************************************************************************************
 * @brief 通过UART6接收数据（中断模式）
 * @param pbuffer 接收数据缓冲区指针
 * @param Size 要接收的数据大小（字节数）
 * @retval HAL_OK 接收启动成功
 * @retval HAL_ERROR 接收启动失败
 ************************************************************************************************************/
int8_t BSP_Uart6Rx(uint8_t *pbuffer, uint16_t Size)
{
	if (HAL_UART_Receive_IT(&Uart6Handle, pbuffer, Size) != HAL_OK)
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

/************************************************************************************************************
 * @brief 控制UART6的RTS引脚状态
 * @param u8CtrlStatus 要设置的GPIO引脚状态(GPIO_PIN_SET/GPIO_PIN_RESET)
 * @note 当前函数为空实现，预留用于未来功能扩展
 ************************************************************************************************************/
void BSP_Uart6_RTS_Ctrl(GPIO_PinState u8CtrlStatus)
{
	// HAL_GPIO_WritePin(GPIOG,GPIO_PIN_8,u8CtrlStatus);//0x0100
	return;
}

/************************************************************************************************************
 * @brief 获取UART6的CTS信号状态
 * @note  当前实现固定返回0，实际应用中应读取GPIO状态
 * @retval uint8_t CTS信号状态（0或1）
 ************************************************************************************************************/
uint8_t BSP_GetUart6_CTS(void)
{
	// return HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_15);//0x8000
	return 0;
}



/************************************************************************************************************
 * @brief 控制UART4的RS485_1方向引脚状态
 * @param u8CtrlStatus 引脚状态(GPIO_PIN_SET/GPIO_PIN_RESET)
 * @note 该函数通过控制GPIOD的PIN3来切换RS485收发方向
 ************************************************************************************************************/
void BSP_Uart4_RS485_1_Ctrl(GPIO_PinState u8CtrlStatus)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, u8CtrlStatus); // 0x8 维持不变
}





/************************************************************************************************************
 * @brief  控制UART5 RS485通道2的发送/接收状态
 * @param  u8CtrlStatus 控制状态(GPIO_PIN_SET/GPIO_PIN_RESET)
 * @note   通过控制PG8引脚电平来切换RS485收发器方向
 ************************************************************************************************************/
void BSP_Uart5_RS485_2_Ctrl(GPIO_PinState u8CtrlStatus)
{
	// HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,u8CtrlStatus);//0x40 PD6用于 UART-RX了
	// HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,u8CtrlStatus);//PF10在新板中用于UART5了
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, u8CtrlStatus);
}

/************************************************************************************************************
 * @brief 控制UART6 RS485通道3的使能状态
 * @param u8CtrlStatus 控制状态，GPIO_PIN_SET或GPIO_PIN_RESET
 * @note 原PF10引脚在新板中已改为UART5使用
 ************************************************************************************************************/
void BSP_Uart6_RS485_3_Ctrl(GPIO_PinState u8CtrlStatus)
{
	// HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,u8CtrlStatus);PF10在新板中用于UART5了
}



/************************************************************************************************************
 * @brief  获取UART4接口的当前状态
 * @retval HAL_UART_StateTypeDef 返回UART4的状态值
 * @note   就绪状态(HAL_UART_STATE_READY)表示发送接收已完成
 ************************************************************************************************************/
int BSP_GetUart4State(void)
{
	// HAL_UART_STATE_READY is TxRx end
	return HAL_UART_GetState(&Uart4Handle);
}

/************************************************************************************************************
 * @brief  获取UART5接口的当前状态
 * @retval HAL_UART_StateTypeDef 返回UART5的状态值
 * @note   当返回值为HAL_UART_STATE_READY时表示发送/接收操作已完成
 ************************************************************************************************************/
int BSP_GetUart5State(void)
{
	// HAL_UART_STATE_READY is TxRx end
	return HAL_UART_GetState(&Uart5Handle);
}

/************************************************************************************************************
 * @brief  获取UART6接口的当前状态
 * @retval HAL_UART_StateTypeDef 返回UART6的状态值
 * @note   状态HAL_UART_STATE_READY表示收发已完成
 ************************************************************************************************************/
int BSP_GetUart6State(void)
{
	// HAL_UART_STATE_READY is TxRx end
	return HAL_UART_GetState(&Uart6Handle);
}

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *Uart6Handle)
//{
//
//}
//
//
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *Uart6Handle)
//{
//
//}
//
//void HAL_UART_ErrorCallback(UART_HandleTypeDef *Uart6Handle)
//{
//
//}
