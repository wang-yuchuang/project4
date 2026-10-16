/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPCAN.h
** @Founder      : WYC834
** @Date         : 2025.8.19
** @Function     : CAN驱动函数模块
** @Instructions : 用户完善
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPQueue.h"
#include "BSPIRQ.h"
#include "BSPCAN.h"
#include "fdcan.h"
#include "BSPIRQ.h"
#include "BSPUserHook.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sCANSendBuff[eCANNUM][QUE_SIZEOF(CAN_QUEUE_SIZE, CAN_QUEUE_NUM)]; //定义CAN发送缓冲区空间
static s_CyRcvBuf sCANRcvCyBuf[eCANNUM];                                    //CAN接收环形缓冲区
static u8 sCANCyUnDealNum[eCANNUM] = {0};                                   //CAN环形缓冲区未处理信息数目

FDCAN_FilterTypeDef sFilterConfig1;
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CANRcvCyBufInit(e_CANNUM canNum)
** @Input     : canNum:CAN通道号
** @Output    : void
** @Function  : 环形接收缓冲区初始化
** @The notes :
**===========================================================================================*/
static void CANRcvCyBufInit(e_CANNUM canNum);

/*=============================================================================================
** @Name      : static u8 CANWriteRcvCyBuf(e_CANNUM canNum, FDCAN_RxHeaderTypeDef *pRxMsg,u8 * data)
** @Input     : canNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN接收中断函数将CAN报文写入接收环形缓冲区
** @The notes :
**===========================================================================================*/
static u8 CANWriteRcvCyBuf(e_CANNUM canNum, FDCAN_RxHeaderTypeDef *pRxMsg,u8 * data);

/*=============================================================================================
** @Name      : static u8 CANReadRcvCyBuf(e_CANNUM canNum, t_CANMSG *pRcvMsg)
** @Input     : canNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN接收处理任务读CAN接收环形缓冲区信息出来处理
** @The notes :
**===========================================================================================*/
static u8 CANReadRcvCyBuf(e_CANNUM canNum, t_CANMSG *pRcvMsg);

/*=============================================================================================
** @Name      : static u8 GetCanFdMessageLength(u8 length)
** @Input     : length:数据长度
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
static u8 GetCanFdMessageLength(u8 length);
/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPCanBufInit(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化CAN发送/接收BUF
** @The notes : Main()函数调用，上电初始化
**===========================================================================================*/
void BSPCanBufInit(void)
{
    u8 canNum = 0;
    
    //初始化发送缓冲区队列和接收缓冲区队列
    for(canNum = 0; canNum < eCANNUM; canNum++)
    {
        //环形接收缓冲区初始化
        CANRcvCyBufInit((e_CANNUM)canNum);

        //发送队列缓冲区初始化
        (void)QueInit((void *)&sCANSendBuff[canNum], CAN_QUEUE_SIZE, CAN_QUEUE_NUM);
    }
}

/*=============================================================================================
** @Name      : void BSPCANInit(e_CANNUM canNum)
** @Input     : canNum:CAN通道号 osc:晶振频率 rate:CAN波特率
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : CAN驱动模块初始化
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANInit(e_CANNUM canNum)
{	
    if(canNum >= eCANNUM)
    {
        return;
    }
    if((canNum == eCAN1)&&(1 != gBSPFunCfg.Bits.CAN1En))
    {
        return;
    }
    switch(canNum)
    {
        case eCAN1:
            if(1 == gBSPFunCfg.Bits.CAN1En)  
            {
                MX_FDCAN1_Init();
                // fdcan1_config();
            }           
           //计算can1波特率
           // BaudRate = HAL_RCC_GetPCLK1Freq()/1000;//单位k，获取apb1时钟频率
           // BaudRate = BaudRate/(2*hfdcan1.Init.ClockDivider);
           // BaudRate = BaudRate/(hfdcan1.Init.DataTimeSeg1 +  hfdcan1.Init.DataTimeSeg2 +1);
           // InputCANStateData(eCAN1 , BaudRate);

            break;    
        default:
        
            break;        
    }	
}

/*=============================================================================================
** @Name      : u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(直接发送)
** @The notes : 用户调用(尽量少用,会丢帧)
**===========================================================================================*/
u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg)
{	
	static u8 failureCountCAN1 = 0; // CAN0 发送失败计数器  

    if(0 == msg->Len)
    {
        return(FALSE);
    }
	//模块未使能
	if(canNum == eCAN1 && 1 != gBSPFunCfg.Bits.CAN1En)
	{
		return(FALSE);
	}
	FDCAN_TxHeaderTypeDef TxHeader;
	/* Prepare Tx Header */
	TxHeader.Identifier = msg->ID;
	TxHeader.IdType = FDCAN_EXTENDED_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
        if(msg->Len<=8)
		TxHeader.DataLength =  msg->Len;
	if(msg->Len<=12&&msg->Len>8)
		TxHeader.DataLength = FDCAN_DLC_BYTES_12;
	if(msg->Len<=16&&msg->Len>12)
		TxHeader.DataLength = FDCAN_DLC_BYTES_16;
	if(msg->Len<=20&&msg->Len>16)
		TxHeader.DataLength = FDCAN_DLC_BYTES_20;
	if(msg->Len<=24&&msg->Len>20)
		TxHeader.DataLength = FDCAN_DLC_BYTES_24;
	if(msg->Len<=32&&msg->Len>24)
		TxHeader.DataLength = FDCAN_DLC_BYTES_32;
	if(msg->Len<=48&&msg->Len>32)
		TxHeader.DataLength = FDCAN_DLC_BYTES_48;
	if(msg->Len<=64&&msg->Len>48)
		TxHeader.DataLength = FDCAN_DLC_BYTES_64;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_ON;
	TxHeader.FDFormat = FDCAN_FD_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	if(canNum == eCAN1)
    {	
      BSP_DISABLE_IRQ();//关中断，防止发送被打断
		if(HAL_OK != HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader,msg->Data))
		{
			failureCountCAN1++;
		     BSP_ENABLE_IRQ();//开中断
			 if (failureCountCAN1 >= 50 )
			 {
				failureCountCAN1 = 0;//重置计数器
				BSPCANInit(eCAN1);
			 }
			return(FALSE);
		}	
		BSP_ENABLE_IRQ();//开中断
    }
	return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(数据压入队列)
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg)
{
	//模块未使能
	if(canNum != eCAN1 && 1 != gBSPFunCfg.Bits.CAN1En)
	{
		return(FALSE);
	}
	u8 result = 0;
	if(canNum >= eCANNUM)
	{
		return(FALSE);
	}
	result = QueIn((void *)sCANSendBuff[canNum], (void *)(msg));
	
	return(result);
}

/*=============================================================================================
** @Name      : void BSPCANSendTask(void)
** @Input     : void
** @Output    : void
** @Function  : CAN信息发送任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANSendTask(void)
{
	u8 index = 0;
	static t_CANMSG txMsg;
	static u8 sHisErr[eCANNUM] = {TRUE};

	//BSP_DISABLE_IRQ();
	for(index = 0; index < eCANNUM; index++)
	{
		if(QueGetUnitNum((void *)&sCANSendBuff[index]))                     //判断CAN发送缓冲区中是否有值
		{
			if(FALSE == sHisErr[index])                                     //上次发送失败
			{
				(void)BSPCANSendMsg((e_CANNUM)index, &txMsg);               //直接重发一次
				sHisErr[index] = TRUE;
			}
			else if(QueOut((void *)&sCANSendBuff[index], (void *)(&txMsg))) //出对列成功
			{
				sHisErr[index] = BSPCANSendMsg((e_CANNUM)index, &txMsg);    //发送数据
			}
		}
	}
	//BSP_ENABLE_IRQ();
}

/*=============================================================================================
** @Name      : void BSPCANRcvTask(void)
** @Input     : void
** @Output    : void
** @Function  : CAN信息接收任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANRcvTask(void)
{
    u8 index = 0;
    t_CANMSG rcvMsg;
    //BSP_DISABLE_IRQ();
    for(index = 0; index < eCANNUM; index++)
    {
        //从缓冲区读取接收到的报文提交到用户处理
        if(CANReadRcvCyBuf((e_CANNUM)index, &rcvMsg) == 1)
        {
            //计算缓冲区未处理信息
            if(sCANCyUnDealNum[index] >= 1)
            {
                sCANCyUnDealNum[index]--;
            }

            //CAN接收报文处理函数
            (void)CANRcvMsgHandle_Hook((e_CANNUM)index, &rcvMsg);
        }
    }
    //BSP_ENABLE_IRQ();
}



/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void CANRcvCyBufInit(e_CANNUM canNum)
** @Input     : canNum:CAN通道号
** @Output    : void
** @Function  : 环形接收缓冲区初始化
** @The notes :
**===========================================================================================*/
static void CANRcvCyBufInit(e_CANNUM canNum)
{
	if(canNum >= eCANNUM)
	{
		return;
	}
	//模块未使能
	if(canNum == eCAN1 && 1 != gBSPFunCfg.Bits.CAN1En)
	{
		return;
	}
	memset((u8 *)(&(sCANRcvCyBuf[canNum])), 0, sizeof(sCANRcvCyBuf[canNum]));
	sCANRcvCyBuf[canNum].FullFlag   = 0;   
	sCANRcvCyBuf[canNum].ReadPoint  = 0;
	sCANRcvCyBuf[canNum].WritePoint = 0;
}

/*=============================================================================================
** @Name      : static u8 CANWriteRcvCyBuf(e_CANNUM canNum, FDCAN_RxHeaderTypeDef *pRxMsg,u8 * data)
** @Input     : canNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针, u8 * data：读取到的数据
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN接收中断函数将CAN报文写入接收环形缓冲区
** @The notes :
**===========================================================================================*/
static u8 CANWriteRcvCyBuf(e_CANNUM canNum, FDCAN_RxHeaderTypeDef *pRxMsg,u8 * data)
{
	
	u8 i = 0;
	u8 result = 0;
	u8 wrtPoint = 0;

	if((canNum >= eCANNUM) || (pRxMsg == (void *)0))
	{
		return(FALSE);
	}
	//模块未使能
	if(canNum == eCAN1 && 1 != gBSPFunCfg.Bits.CAN1En)
	{
		return(FALSE);
	}
	//软件过滤
    // if((canNum == eCAN1) && (((pRxMsg->Identifier >> 8) & 0xff) != gSystemInfo.Addr && ((pRxMsg->Identifier >> 8)&0xff) != 0xff) //只接收地址为本设备的控制命令
    //        && ((pRxMsg->Identifier & 0xff) != gSystemInfo.Addr && (pRxMsg->Identifier & 0xff) != 0xff))//只接收地址为本设备的uds升级报文
	// {
	// 	return(FALSE);
	// }

	//计算缓冲区未处理信息
	if(sCANCyUnDealNum[canNum] < 0xff)
	{
		sCANCyUnDealNum[canNum]++;
	}

	//获得缓冲区写入点
	wrtPoint = sCANRcvCyBuf[canNum].WritePoint;

	//缓冲区未满
	if((0 == sCANRcvCyBuf[canNum].FullFlag))
	{
        sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].ID = pRxMsg->Identifier;
		//写长度
		sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].Len = GetCanFdMessageLength(pRxMsg->DataLength);		

		//写数据
		for (i = 0; i < sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].Len; i++)
		{
			sCANRcvCyBuf[canNum].RcvBuf[wrtPoint].Data[i] = data[i];
		}

		//判断写缓冲指针是否超过最大值
		if(++sCANRcvCyBuf[canNum].WritePoint >= CAN_RCV_CYBUF)
		{
			sCANRcvCyBuf[canNum].WritePoint= 0;
		}

		//判断读指针是否等于写指针
		if(sCANRcvCyBuf[canNum].WritePoint == sCANRcvCyBuf[canNum].ReadPoint)
		{
			sCANRcvCyBuf[canNum].FullFlag = 1;
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
** @Name      : static u8 CANReadRcvCyBuf(e_CANNUM canNum, t_CANMSG *pRcvMsg)
** @Input     : canNum:CAN通道号, *pRcvMsg:CAN信息结构体接收数据指针
** @Output    : TRUE:成功 FALSE:失败
** @Function  : CAN接收处理任务读CAN接收环形缓冲区信息出来处理
** @The notes :
**===========================================================================================*/
static u8 CANReadRcvCyBuf(e_CANNUM canNum, t_CANMSG *pRcvMsg)
{
	u8 result = 0;
	
	if((canNum >= eCANNUM) || (pRcvMsg == (void *)0))
	{
		return(FALSE);
	}
	//模块未使能
	if(canNum == eCAN1 && 1 != gBSPFunCfg.Bits.CAN1En)
	{
		return(FALSE);
	}
	//有数据则把数据从缓冲区中读出来
	if((sCANRcvCyBuf[canNum].ReadPoint != sCANRcvCyBuf[canNum].WritePoint)
		|| (sCANRcvCyBuf[canNum].FullFlag != 0))
	{
		*pRcvMsg = sCANRcvCyBuf[canNum].RcvBuf[sCANRcvCyBuf[canNum].ReadPoint];
		
		if(++sCANRcvCyBuf[canNum].ReadPoint >= CAN_RCV_CYBUF)
		{
			sCANRcvCyBuf[canNum].ReadPoint = 0;
		}
		
		sCANRcvCyBuf[canNum].FullFlag = 0;
		
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
** @Name      : u8 BSPCANLoop_test(e_CANNUM canNum)
** @Input     : canNum:通道号 
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : 
** @The notes : CAN回环测试
**===========================================================================================*/
u8 BSPCANLoop_test(e_CANNUM canNum) 
{
	t_CANMSG msg;
	FDCAN_RxHeaderTypeDef pRxHeader;
	u8 rx_flag = 0;
	msg.ID = 0xAAA;
	msg.Len = 8;
	if(canNum == eCAN1)
	{
		if(HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0))
		{
			if(HAL_OK != HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0,&pRxHeader, msg.Data))
				return FALSE;
			rx_flag ++;
			
		}
	}


	if(rx_flag != 0)
	{
		if(FALSE == BSPCANSendMsg(canNum,&msg))
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*=============================================================================================
** @Name      : void FDCAN_RxFifo0CallbackTypeDef(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
** @Input     : *hfdcan:can句柄  		RxFifo0ITs:
** @Output    : void
** @Function  : can接收回调函数，接收can报文硬件中断调用
** @The notes : can初始化时注册
**===========================================================================================*/
void FDCAN_RxFifo0CallbackTypeDef(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	FDCAN_RxHeaderTypeDef pRxHeader;
	u8 rd_data[64] = {0};
	if(HAL_OK != HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0,&pRxHeader, rd_data))
	{
            // Error_Handler();
			return;
			
	}
        if(hfdcan->Instance == FDCAN1)
        {
            CANWriteRcvCyBuf(eCAN1,&pRxHeader,rd_data);
        }
	
}

/*=============================================================================================
** @Name      : static u8 GetCanFdMessageLength(u8 length)
** @Input     : length:数据长度
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
static u8 GetCanFdMessageLength(u8 length)
{
    u8 datalength = 0;
    switch(length)
    {
        case FDCAN_DLC_BYTES_12:
            datalength = 12;
            break;

        case FDCAN_DLC_BYTES_16:
            datalength = 16;
            break;

        case FDCAN_DLC_BYTES_20:
            datalength = 20;
            break;

        case FDCAN_DLC_BYTES_24:
            datalength = 24;
            break;

        case FDCAN_DLC_BYTES_32:
            datalength = 32;
            break;

        case FDCAN_DLC_BYTES_48:
            datalength = 48;
            break;
            
        case FDCAN_DLC_BYTES_64:
            datalength = 64;
            break;
            
        default:
            datalength = length;
    }

    return(datalength); 
}


#if 0
void fdcan1_config(void)
{
  sFilterConfig1.IdType = FDCAN_STANDARD_ID;
  sFilterConfig1.FilterIndex = 0;
  sFilterConfig1.FilterType = FDCAN_FILTER_RANGE;
  sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig1.FilterID1 = 0x00;
  sFilterConfig1.FilterID2 = 0x7FF;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig1) != HAL_OK)
  {
    Error_Handler();
  }

  sFilterConfig1.IdType = FDCAN_EXTENDED_ID;
  sFilterConfig1.FilterIndex = 0;
  sFilterConfig1.FilterType = FDCAN_FILTER_RANGE;
  sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig1.FilterID1 = 0x00;
  sFilterConfig1.FilterID2 = 0x1FFFFFFF;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure global filter on both FDCAN instances:
  Filter all remote frames with STD and EXT ID
  Reject non matching frames with STD ID and EXT ID */
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Activate Rx FIFO 0 new message notification on both FDCAN instances */
  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_BUS_OFF, 0) != HAL_OK)
  {
    Error_Handler();
  }



  if (HAL_FDCAN_RegisterRxFifo0Callback(&hfdcan1,FDCAN_RxFifo0CallbackTypeDef) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure and enable Tx Delay Compensation, required for BRS mode.
        TdcOffset default recommended value: DataTimeSeg1 * DataPrescaler
        TdcFilter default recommended value: 0 */
    HAL_FDCAN_EnableTxDelayCompensation(&hfdcan1);
   HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan1,  hfdcan1.Init.DataTimeSeg1+1, 3);
   //HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan1, 0x0A, 0x03);
  
  
  
  // 动态计算补偿值（示例）
  //u32 TxDelayCompensation = hfdcan1.Init.DataPrescaler * 
                            //   (hfdcan1.Init.DataSyncJumpWidth + hfdcan1.Init.DataTimeSeg1);
  //HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan1, TxDelayCompensation, 15U); // 假设固定延迟1个TQ



  HAL_FDCAN_Start(&hfdcan1);
}


#endif