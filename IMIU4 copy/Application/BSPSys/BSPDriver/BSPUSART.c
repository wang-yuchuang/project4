/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPUSART.h
** @Founder      : WYC834
** @Date         : 2025.09.04
** @Function     : USART驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "main.h"
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPQueue.h"
#include "BSPUserHook.h"
#include "BSPIRQ.h"
#include "BSPUSART.h"
#include "usart.h"
#include "BSPDIDO.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/

extern DMA_HandleTypeDef hdma_usart6_rx;
static u8 sUSARTSendBuff[eUSARTNUM][QUE_SIZEOF(USART_QUEUE_SIZE, USART_QUEUE_NUM)];//定义USART发送缓冲区空间
static t_UsartCyRcvBuf sUSARTRcvCyBuf[eUSARTNUM];                               //USART接收环形缓冲区
static u8 sUSARTCyUnDealNum[eUSARTNUM] = {0};                                   //USART环形缓冲区未处理信息数目

u16 UsartRcvDataLenth = 0;				    /*串口接收数据长度*/
u8 UsartRcvDataBuff[eUSARTNUM][USART_DATA_LEN] = {0};	/*串口接收数据buff*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static UART_HandleTypeDef GetUsartHandle(e_SCINUM sciNum)
** @Input     : sciNum:SCI通道号
** @Output    : 串口对应句柄
** @Function  : 获取串口对应句柄
** @The notes :
**===========================================================================================*/
static UART_HandleTypeDef* GetUsartHandle(e_USARTNUM usartNum);

/*=============================================================================================
** @Name      : static void USARTRcvCyBufInit(e_USARTNUM usartNum)
** @Input     : usartNum:CAN通道号
** @Output    : void
** @Function  : 环形接收缓冲区初始化
** @The notes :
**===========================================================================================*/
static void USARTRcvCyBufInit(e_USARTNUM usartNum);

/*=============================================================================================
** @Name      : static u8 USARTReadRcvCyBuf(e_USARTNUM usartNum, t_USARTMSG *pRcvMsg)
** @Input     : usartNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART接收处理任务读USART接收环形缓冲区信息出来处理
** @The notes :
**===========================================================================================*/
static u8 USARTReadRcvCyBuf(e_USARTNUM usartNum, t_USARTMSG *pRcvMsg);

/*=============================================================================================
** @Name      : static u8 USARTWriteRcvCyBuf(e_USARTNUM usartNum, u8 * data,u8 size)
** @Input     : usartNum:USART通道号, *data:USART信息接收数组指针, size：读取到的数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART接收中断函数将USART报文写入接收环形缓冲区
** @The notes :
**===========================================================================================*/
static u8 USARTWriteRcvCyBuf(e_USARTNUM usartNum, u8 * wdata, u8 size);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPUSARTInit(e_USARTNUM usartNum)
** @Input     : usartNum:USART通道号
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : USART驱动模块初始化
** @The notes : BSP调用,用户调用
**===========================================================================================*/
void BSPUSARTInit(e_USARTNUM usartNum)
{
    //模块未使能不进行初始化
    if((eUSART6 == usartNum) && (1 != gBSPFunCfg.Bits.USART6En))
    {
        return;
    }
    if(usartNum >= eUSARTNUM)
    {
        return;
    }

    switch(usartNum)
    {
        case eUSART6:
            MX_USART6_UART_Init();
            __HAL_UART_CLEAR_IDLEFLAG(&huart6);//清除空闲标志
            __HAL_UART_ENABLE_IT(&huart6,UART_IT_IDLE);//使能空闲中断
            HAL_UARTEx_ReceiveToIdle_DMA(&huart6,UsartRcvDataBuff[eUSART6],USART_DATA_LEN);//这个函数最要是设置接收缓存区
        break;
        default:
            return;
        break;
    }
}

/*=============================================================================================
** @Name      : u8 BSPUSARTSendDataOnISR(e_USARTNUM usartNum, u8 *sendbuf, u8 size)
** @Input     : usartNum:USART通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART中断方式发送串口数据
** @The notes : 直接发送,暂不实现
**===========================================================================================*/
u8 BSPUSARTSendDataOnISR(e_USARTNUM usartNum, u8 *sendbuf, u8 size)
{       
    //模块未使能
    if((eUSART6 == usartNum) && (1 != gBSPFunCfg.Bits.USART6En))
    {
        return(FALSE);
    }
    UART_HandleTypeDef *huart;
    huart = GetUsartHandle(usartNum);//获取对应串口句柄
    //BSP_DISABLE_IRQ();
    switch(usartNum)
    {   
        case eUSART6:
            RS485_SendEnable();
        break;
        default:
        break;

    }
    if(HAL_UART_Transmit_IT(huart, sendbuf, size) != HAL_OK)//中断方式发送
    {
        return(FALSE);
    }
    switch(usartNum)
    {   
        case eUSART6:
            RS485_ReceiveEnable();
        break;
        default:
        break;
    }
    //BSP_ENABLE_IRQ();
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : usartNum:USART通道号*sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART询问方式发送串口数据
** @The notes :  直接发送,用户调用
**===========================================================================================*/
u8 BSPSCISendDataOnAsk(e_USARTNUM usartNum, u8 *sendbuf, u8 size)
{
    //模块未使能
    if((eUSART6 == usartNum) && (1 != gBSPFunCfg.Bits.USART6En))
    {
        return(FALSE);
    }
    UART_HandleTypeDef *huart;
    u8 sendstate = FALSE;
    huart = GetUsartHandle(usartNum);//获取对应串口句柄
    //BSP_DISABLE_IRQ();

    switch(usartNum)
    {   
        case eUSART6:
            RS485_SendEnable();
        break;
        default:
        break;

    }
    if(HAL_UART_Transmit(huart, sendbuf, size, 1000) != HAL_OK)//轮询方式发送
    {
        sendstate = FALSE;
    }else
    {
        sendstate = TRUE;
    }
    
    switch(usartNum)
    {   
        case eUSART6:
            RS485_ReceiveEnable();
        break;
        default:
        break;
    }
    //BSP_ENABLE_IRQ();
    return(sendstate);
}

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnDMA(e_USARTNUM usartNum, u8 *sendbuf, u8 size)
** @Input     : usartNum:USART通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART DMA方式发送串口数据
** @The notes :  直接发送,用户调用
**===========================================================================================*/
u8 BSPSCISendDataOnDMA(e_USARTNUM usartNum, u8 *sendbuf, u8 size)
{
    //dma没配置
    return(FALSE);
}

/*=============================================================================================
** @Name      : void BSPUSARTBufInit(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化USART发送/接收BUF
** @The notes : Main()函数调用，上电初始化
**===========================================================================================*/
void BSPUSARTBufInit(void)
{
    u8 usartNum = 0; 
    //初始化发送缓冲区队列和接收缓冲区队列
    for(usartNum = 0; usartNum < eUSARTNUM; usartNum++)
    {
        //环形接收缓冲区初始化
        USARTRcvCyBufInit((e_USARTNUM)usartNum);
        //发送队列缓冲区初始化
        (void)QueInit((void *)&sUSARTSendBuff[usartNum], USART_QUEUE_SIZE, USART_QUEUE_NUM);
    }
}
         
/*=============================================================================================
** @Name      : u8 BSPUSARTSendQin(e_USARTNUM usartNum, u8 *tdata, u8 size)
** @Input     : canNum:通道号 *data:发送信息数组 xize 发送数组大小（不超过100）
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(数据压入队列)
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPUSARTSendQin(e_USARTNUM usartNum, u8 *tdata, u8 size)
{

    if((eUSART6 == usartNum) && (1 != gBSPFunCfg.Bits.USART6En))
    {
        return(FALSE);
    }
    if((usartNum >= eUSARTNUM) || (size > USART_DATA_LEN))
    {
        return(FALSE);
    }
    u8 result = 0;
    t_USARTMSG msg;
    memcpy(msg.data,tdata, size);
    msg.len = size;
    result = QueIn( (void*)sUSARTSendBuff[usartNum], (void*)(&msg));//发送数据入队列
    return(result);
}     

/*=============================================================================================
** @Name      : void BSPUSARTSendTask(void)
** @Input     : void
** @Output    : void
** @Function  : USART信息发送任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPUSARTSendTask()
{
	u8 index = 0;
	static t_USARTMSG txMsg;
	static u8 sHisErr[eUSARTNUM] = {TRUE};

	//BSP_DISABLE_IRQ();
	for(index = 0; index < eUSARTNUM; index++)
	{
		if(QueGetUnitNum((void *)&sUSARTSendBuff[index]))                     //判断USART发送缓冲区中是否有值
		{
			if(FALSE == sHisErr[index])                                     //上次发送失败
			{
				(void)BSPUSARTSendMsg((e_USARTNUM)index, &txMsg);               //直接重发一次
                                osDelay(5);//防止串口报文粘连
				sHisErr[index] = TRUE;
			}
			else if(QueOut((void *)&sUSARTSendBuff[index], (void *)(&txMsg))) //出对列成功
			{
				sHisErr[index] = BSPUSARTSendMsg((e_USARTNUM)index, &txMsg);    //发送数据	
                                osDelay(5);//防止串口报文粘连
			}
		}
	}
	//BSP_ENABLE_IRQ();
}

/*=============================================================================================
** @Name      : u8 BSPUSARTSendMsg(e_USARTNUM usartNum, t_USARTMSG *msg)
** @Input     : usartNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : USART发送信息(直接发送)
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPUSARTSendMsg(e_USARTNUM usartNum, t_USARTMSG *msg)
{	
    if(0 == msg->len)
    {
        return(FALSE);
    }
    //模块未使能
    if((eUSART6 == usartNum) && (1 != gBSPFunCfg.Bits.USART6En))
    {
        return(FALSE);
    }
    if(TRUE != BSPSCISendDataOnAsk(usartNum, msg->data, msg->len))//串口轮询发送
    {
        return(FALSE);
    }
    return(TRUE);
}

/*=============================================================================================
** @Name      : void BSPUSARTRcvTask(void)
** @Input     : void
** @Output    : void
** @Function  : USART接收信息处理任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPUSARTRcvTask()
{
    u8 index = 0;
    t_USARTMSG rcvMsg;
    
    
    //BSP_DISABLE_IRQ();
    for(index = 0; index < eUSARTNUM; index++)
    {
        //从缓冲区读取接收到的报文提交到用户处理
        if(USARTReadRcvCyBuf((e_USARTNUM)index, &rcvMsg) == TRUE)
        {
            //计算缓冲区未处理信息
            if(sUSARTCyUnDealNum[index] >= 1)
            {
                sUSARTCyUnDealNum[index]--;
            }

            //USART接收报文处理函数
            (void)USARTRcvMsgHandle_Hook((e_USARTNUM)index, &rcvMsg);
			
        }
    }
    //BSP_ENABLE_IRQ();
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static UART_HandleTypeDef GetUsartHandle(e_USARTNUM usartNum)
** @Input     : sciNum:SCI通道号
** @Output    : 串口对应句柄
** @Function  : 获取串口对应句柄
** @The notes :
**===========================================================================================*/
static UART_HandleTypeDef* GetUsartHandle(e_USARTNUM usartNum)
{
     switch(usartNum)
    {
        case eUSART6:
            return &huart6;
        break;
        default:
            return &huart6;
        break;
    }
}

/*=============================================================================================
** @Name      : static void USARTRcvCyBufInit(e_USARTNUM usartNum)
** @Input     : usartNum:CAN通道号
** @Output    : void
** @Function  : 环形接收缓冲区初始化
** @The notes :
**===========================================================================================*/
static void USARTRcvCyBufInit(e_USARTNUM usartNum)
{
    if(usartNum >= eUSARTNUM)
    {
            return;
    }	
    memset((u8 *)(&(sUSARTRcvCyBuf[usartNum])), 0, sizeof(sUSARTRcvCyBuf[usartNum]));
    sUSARTRcvCyBuf[usartNum].uFullFlag   = 0;   
    sUSARTRcvCyBuf[usartNum].uReadPoint  = 0;
    sUSARTRcvCyBuf[usartNum].uWritePoint = 0;
}

/*=============================================================================================
** @Name      : static u8 USARTReadRcvCyBuf(e_USARTNUM usartNum, t_USARTMSG *pRcvMsg)
** @Input     : usartNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART接收处理任务读USART接收环形缓冲区信息出来处理
** @The notes :
**===========================================================================================*/
static u8 USARTReadRcvCyBuf(e_USARTNUM usartNum, t_USARTMSG *pRcvMsg)
{  
    if((usartNum >= eUSARTNUM) || (pRcvMsg == (void *)0))
    {
        return(FALSE);
    }
    u8 result = 0;
    //有数据则把数据从缓冲区中读出来
    if((sUSARTRcvCyBuf[usartNum].uReadPoint != sUSARTRcvCyBuf[usartNum].uWritePoint)
            || (sUSARTRcvCyBuf[usartNum].uFullFlag != 0))
    {
        *pRcvMsg = sUSARTRcvCyBuf[usartNum].uRcvBuf[sUSARTRcvCyBuf[usartNum].uReadPoint];
        
        if(++sUSARTRcvCyBuf[usartNum].uReadPoint >= USART_RCV_CYBUF)
        {
                sUSARTRcvCyBuf[usartNum].uReadPoint = 0;
        }           
        sUSARTRcvCyBuf[usartNum].uFullFlag = 0;
        result = TRUE;
    }
    //缓冲区无数据
    else 
    {
        result = FALSE;
    }

    return(result);
}

/*=============================================================================================
** @Name      : static u8 USARTWriteRcvCyBuf(e_USARTNUM usartNum, u8 * wdata,u8 size)
** @Input     : usartNum:USART通道号, *data:USART信息接收数组指针, size：读取到的数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART接收中断函数将USART报文写入接收环形缓冲区
** @The notes :
**===========================================================================================*/
static u8 USARTWriteRcvCyBuf(e_USARTNUM usartNum, u8 * wdata, u8 size)
{
    if((usartNum >= eUSARTNUM) || (wdata == (void *)0))
    {
        return(FALSE);
    }
    u8 i = 0;
    u8 result = 0;
    u8 wrtPoint = 0;
    //计算缓冲区未处理信息
    if(sUSARTCyUnDealNum[usartNum] < 0xff)
    {
        sUSARTCyUnDealNum[usartNum]++;
    }
    //获得缓冲区写入点
    wrtPoint = sUSARTRcvCyBuf[usartNum].uWritePoint;
    //缓冲区未满
    if((0 == sUSARTRcvCyBuf[usartNum].uFullFlag))
    {

        //写长度
        sUSARTRcvCyBuf[usartNum].uRcvBuf[wrtPoint].len = size;		

        //写数据
        for (i = 0; i < sUSARTRcvCyBuf[usartNum].uRcvBuf[wrtPoint].len; i++)
        {
                sUSARTRcvCyBuf[usartNum].uRcvBuf[wrtPoint].data[i] = wdata[i];
        }
        //判断写缓冲指针是否超过最大值
        if(++sUSARTRcvCyBuf[usartNum].uWritePoint >= USART_RCV_CYBUF)
        {
                sUSARTRcvCyBuf[usartNum].uWritePoint= 0;
        }
        //判断读指针是否等于写指针
        if(sUSARTRcvCyBuf[usartNum].uWritePoint == sUSARTRcvCyBuf[usartNum].uReadPoint)
        {
                sUSARTRcvCyBuf[usartNum].uFullFlag = 1;
        }
        result = TRUE;
    }
    //缓冲区写满
    else
    {
        result = FALSE;
    }

    return(result);
}
/*=============================================================================================
** @Name      : void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *UartHandle, uint16_t Size)
** @Input     : uUartHandle:usart句柄 size：
** @Output    : void
** @Function  : USART接收回调函数重定义
** @The notes :
**===========================================================================================*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *UartHandle, uint16_t Size) {
  
    if(UartHandle == &huart6){
        HAL_UART_DMAStop(&huart6);  //暂停DMA接收
        UsartRcvDataLenth = USART_DATA_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart6_rx);//这个是本次接收到的长度
        USARTWriteRcvCyBuf(eUSART6,UsartRcvDataBuff[eUSART6],UsartRcvDataLenth);
        //重启接收
        memset(UsartRcvDataBuff[eUSART6],0,USART_DATA_LEN);  //清除接收到的数据
    }
}
