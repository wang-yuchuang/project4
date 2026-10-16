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
#ifndef _BSPUSART_H
#define _BSPUSART_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPGPIO.h"
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
//USART接收发送缓存队列大小
#define USART_QUEUE_SIZE          sizeof(t_USARTMSG)    //USART发送报文字节空间大小
#define USART_QUEUE_NUM           10                  //USART发送报文队列大小
#define USART_RCV_CYBUF           10                  //USART接收环形缓冲区大小
#define	USART_DATA_LEN            200               //USART接收发送数据最大长度

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
//USART通道定义
typedef enum 
{
    eUSART6 = 0,
    eUSARTNUM
}e_USARTNUM;

//SCI发送信息结构
typedef struct
{
    u8 data[USART_DATA_LEN];  //需发送数据值
    u8 len;                 //需发送数据实际长度

}t_USARTMSG;


//SCI通道定义
typedef enum
{
    eBAUD_2400 = 2400,
    eBAUD_4800 = 4800,
    eBAUD_9600 = 9600,
    eBAUD_19200 = 19200,
    eBAUD_38400 = 38400,
    eBAUD_56000 = 56000,
    eBAUD_57600 = 57600
    
}e_USARTBAUD;

//环形接收缓冲区
typedef struct
{
    u8 uWritePoint;                  //USART的写指针
    u8 uReadPoint;                   //USART的读指针
    u8 uFullFlag;                    //USART缓冲区满标志
    t_USARTMSG uRcvBuf[USART_RCV_CYBUF];        //USART接收缓冲区信息
}t_UsartCyRcvBuf;
extern u8 UsartRcvDataBuff[eUSARTNUM][USART_DATA_LEN];
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPUSARTInit(e_USARTNUM usartNum)
** @Input     : usartNum:USART通道号
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : USART驱动模块初始化
** @The notes : BSP调用,用户调用
**===========================================================================================*/
void BSPUSARTInit(e_USARTNUM usartNum);

/*=============================================================================================
** @Name      : u8 BSPUSARTSendDataOnISR(e_USARTNUM sciNum, u8 *sendbuf, u8 size)
** @Input     : usartNum:USART通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART中断方式发送串口数据
** @The notes : 直接发送,暂不实现
**===========================================================================================*/
u8 BSPUSARTSendDataOnISR(e_USARTNUM usartNum, u8 *sendbuf, u8 size);

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnAsk(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : usartNum:USART通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART询问方式发送串口数据
** @The notes :  直接发送,用户调用
**===========================================================================================*/
u8 BSPSCISendDataOnAsk(e_USARTNUM usartNum, u8 *sendbuf, u8 size);

/*=============================================================================================
** @Name      : u8 BSPSCISendDataOnDMA(e_SCINUM sciNum, u8 *sendbuf, u8 size)
** @Input     : usartNum:USART通道号 *sendbuf:发送的数据 size:数据长度
** @Output    : TRUE:成功 FALSE:失败
** @Function  : USART DMA方式发送串口数据
** @The notes :  直接发送,用户调用
**===========================================================================================*/
u8 BSPSCISendDataOnDMA(e_USARTNUM usartNum, u8 *sendbuf, u8 size);

/*=============================================================================================
** @Name      : void BSPUSARTBufInit(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化USART发送/接收BUF
** @The notes : Main()函数调用，上电初始化
**===========================================================================================*/
void BSPUSARTBufInit(void);
  
/*=============================================================================================
** @Name      : u8 BSPUSARTSendQin(e_USARTNUM usartNum, u8 *tdata, u8 size)
** @Input     : canNum:通道号 *data:发送信息数组 xize 发送数组大小
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(数据压入队列)
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPUSARTSendQin(e_USARTNUM usartNum, u8 *tdata, u8 size);
  
/*=============================================================================================
** @Name      : void BSPUSARTSendTask(void)
** @Input     : void
** @Output    : void
** @Function  : USART信息发送任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPUSARTSendTask(void);

/*=============================================================================================
** @Name      : u8 BSPUSARTSendMsg(e_USARTNUM usartNum, t_USARTMSG *msg)
** @Input     : usartNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : USART发送信息(直接发送)
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPUSARTSendMsg(e_USARTNUM usartNum, t_USARTMSG *msg);

/*=============================================================================================
** @Name      : void BSPUSARTRcvTask(void *p)
** @Input     : *p:任务函数格式需要空指针
** @Output    : void
** @Function  : USART接收信息处理任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPUSARTRcvTask(void);


#endif

