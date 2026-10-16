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
#ifndef _BSPCAN_H
#define _BSPCAN_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "fdcan.h"
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
//CAN接收发送缓存队列大小
#define CAN_QUEUE_SIZE          sizeof(t_CANMSG)    //CAN发送报文字节空间大小
#define CAN_QUEUE_NUM           15                //CAN发送报文队列大小
#define CAN_RCV_CYBUF           64                  //CAN接收环形缓冲区大小

#define FDCAN_BYTES_1           1                  //CAN数据长度
#define FDCAN_BYTES_2           2                  //CAN数据长度
#define FDCAN_BYTES_3           3                  //CAN数据长度
#define FDCAN_BYTES_4           4                  //CAN数据长度
#define FDCAN_BYTES_5           5                  //CAN数据长度
#define FDCAN_BYTES_6           6                  //CAN数据长度
#define FDCAN_BYTES_7           7                  //CAN数据长度
#define FDCAN_BYTES_8           8                  //CAN数据长度
#define FDCAN_BYTES_12          12                  //CAN数据长度
#define FDCAN_BYTES_16          16                  //CAN数据长度
#define FDCAN_BYTES_20          20                  //CAN数据长度
#define FDCAN_BYTES_24          24                  //CAN数据长度
#define FDCAN_BYTES_32          32                  //CAN数据长度
#define FDCAN_BYTES_48          48                  //CAN数据长度
#define FDCAN_BYTES_64          64                  //CAN数据长度
/*=============================================================================================
** @Data type definition
**===========================================================================================*/
//CAN通道号定义
typedef enum
{
    eCAN1 = 0,
    
    eCANNUM
}e_CANNUM;

//CAN数据结构体
typedef struct
{
    u32 ID;         //帧标识符
    u8 Len;         //数据长度：只能为
    u8 Data[64];     //数据
}t_CANMSG;

//环形接收缓冲区
typedef struct
{
    u8 WritePoint;                  //CAN的写指针
    u8 ReadPoint;                   //CAN的读指针
    u8 FullFlag;                    //CAN缓冲区满标志
    t_CANMSG RcvBuf[CAN_RCV_CYBUF]; //CAN接收缓冲区信息
}s_CyRcvBuf;


/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPCanBufInit(void)
** @Input     : void
** @Output    : void
** @Function  : 初始化CAN发送/接收BUF
** @The notes : Main()函数调用，上电初始化
**===========================================================================================*/
void BSPCanBufInit(void);

/*=============================================================================================
** @Name      : u8 BSPCANInit(e_CANNUM canNum)
** @Input     : canNum:CAN通道号 osc:晶振频率 rate:CAN波特率
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : CAN驱动模块初始化
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANInit(e_CANNUM canNum);

/*=============================================================================================
** @Name      : u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(直接发送)
** @The notes : 用户调用(尽量少用,会丢帧)
**===========================================================================================*/
u8 BSPCANSendMsg(e_CANNUM canNum, t_CANMSG *msg);

/*=============================================================================================
** @Name      : u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg)
** @Input     : canNum:通道号 *msg:发送信息结构体
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : CAN发送信息(数据压入队列)
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPCANSendQin(e_CANNUM canNum, t_CANMSG *msg);

/*=============================================================================================
** @Name      : void BSPCANSendTask(void)
** @Input     : void
** @Output    : void
** @Function  : CAN信息发送任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANSendTask(void);

/*=============================================================================================
** @Name      : void BSPCANRcvTask(void)
** @Input     : void
** @Output    : void
** @Function  : CAN信息接收任务
** @The notes : BSP调用
**===========================================================================================*/
void BSPCANRcvTask(void);

/*=============================================================================================
** @Name      : u8 BSPCANLoop_test(e_CANNUM canNum)
** @Input     : canNum:通道号 
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : 
** @The notes : CAN回环测试
**===========================================================================================*/
u8 BSPCANLoop_test(e_CANNUM canNum);

/*=============================================================================================
** @Name      : void FDCAN_RxFifo0CallbackTypeDef(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
** @Input     : *hfdcan:can句柄  		RxFifo0ITs:
** @Output    : void
** @Function  : can接收回调函数，接收can报文硬件中断调用
** @The notes : can初始化时注册
**===========================================================================================*/
void FDCAN_RxFifo0CallbackTypeDef(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
/*=============================================================================================
** @Name      : u8 BSPCANLoop_test(e_CANNUM canNum)
** @Input     : canNum:通道号  		dirct:1-发送，0-接收
** @Output    : 发送结果:TRUE-成功 FALSE-失败
** @Function  : 
** @The notes : CAN测试报文与参数设置
**===========================================================================================*/

void fdcan1_config(void);
//u8 BSPCAN_debug(e_CANNUM canNum, t_CANMSG *msg, u8 dirct);
#endif

