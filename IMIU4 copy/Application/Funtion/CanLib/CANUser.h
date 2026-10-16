/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANUser.h
** @Founder      : WYC834
** @Date         : 2025.11.23
** @Function     : CAN通信包用户接口
** @Instructions :
**===============================================================================================*/
#ifndef _CANUSER_H
#define _CANUSER_H

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/
/*=======================================通信口读写方式定义=======================================*/
#define CAN_UPDATE_PORT             0                       /*主动更新上传(监控)CAN口*/
#define CAN_READONLY                0                       /*CAN口只读*/
#define CAN_WRITE                   0x01                    /*CAN口可写(CAN1)*/


/*=======================================通信包配置===============================================*/
/*电池串数达240与否及有无内阻信息确定单体信息增加索引个数*/
#if((1 == BMU_BSU_COM_TYPE) && (0 == BMU_BSU_CAN_TYPE))    /*使能自研从控BSU CAN通信信息才测内阻*/
#if((GINDEX93_LEN > 0) && (GINDEX95_LEN > 0))              /*单体电压和温度均超过240*/
#define CELL_INDEX_NUM              8                      /*BMU单体主索引增加个数(CAN通信上传增加主索引数目)*/
#define TX_MUL_R_INDEX              2                      /*主动更新发送内阻信息长帧主索引数*/
#elif(GINDEX93_LEN > 0)                                    /*单体电压/内阻超过240*/
#define CELL_INDEX_NUM              7                      /*BMU单体主索引增加个数(CAN通信上传增加主索引数目)*/
#define TX_MUL_R_INDEX              2                      /*主动更新发送内阻信息长帧主索引数*/
#elif(GINDEX95_LEN > 0)                                    /*单体温度超过240*/
#define CELL_INDEX_NUM              4                      /*BMU单体主索引增加个数(CAN通信上传增加主索引数目)*/
#define TX_MUL_R_INDEX              1                      /*主动更新发送内阻信息长帧主索引数*/
#else                                                      /*单体电压/内阻和温度均不超过240*/
#define CELL_INDEX_NUM              3                      /*BMU单体主索引增加个数(CAN通信上传增加主索引数目)*/
#define TX_MUL_R_INDEX              1                      /*主动更新发送内阻信息长帧主索引数*/
#endif
#else                                                      /*没有内阻信息只考虑单体电压和温度*/
#if((GINDEX93_LEN > 0) && (GINDEX95_LEN > 0))              /*单体电压和温度均超过240*/
#define CELL_INDEX_NUM              2                      /*BMU单体主索引增加个数(CAN通信上传增加主索引数目)*/
#elif((GINDEX93_LEN > 0) || (GINDEX95_LEN > 0))            /*单体电压或温度其一超过240*/
#define CELL_INDEX_NUM              1                      /*BMU单体主索引增加个数(CAN通信上传增加主索引数目)*/
#else                                                      /*单体电压和温度均不超过240*/
#define CELL_INDEX_NUM              0                      /*BMU单体主索引增加个数(CAN通信上传增加主索引数目)*/
#endif
#define TX_MUL_R_INDEX              0                      /*主动更新发送内阻信息长帧主索引数*/
#endif

/*通信包基本配置*/
#define CAN_DEVTAB_NUM              1                       /*创建设备表数目*/
#define CAN_TRTAB_NUM               10                      /*通信收发处理表数目*/
#define BMU_INDEX_NUM               16     					/*CAN通信主索引总数(创建设备表的主索引数目)*/

/*路由功能配置*/
#define CAN_ROUTE_NUM               2                       /*路由CAN通道数目*/
#define CAN_ROUTE0_NUM              1                       /*路由口1转发通道数目*/
#define CAN_ROUTE1_NUM              1                       /*路由口2转发通道数目*/
#define CAN_ROUTE2_NUM              0                       /*路由口3转发通道数目*/

/*周期发送信息长度配置*/
#define CAN_TX_VER_NUM              0                       /*主动发送校时帧信息配置组数*/
#define CAN_TX_BRE_NUM              3                       /*主动上传心跳帧信息配置组数*/
#if(eCAN0 != CAN_UPDATE_PORT)                               /*监控不在CAN0口(多8组保护信息在CAN0口发送)*/
#define CAN_TX_MUL_NUM              (13+TX_MUL_R_INDEX)     /*主动更新发送长帧配置组数*/
#define CAN_TX_SIN_NUM              31                      /*主动更新发送单帧配置组数(不能大于36)*/
#else
#define CAN_TX_MUL_NUM              (8+TX_MUL_R_INDEX)    /*主动更新发送长帧配置组数  (8+TX_MUL_R_INDEX)*/
#define CAN_TX_SIN_NUM              6                      /*主动更新发送单帧配置组数(不能大于36)*/
#endif

#define CAN_TX_CEL_NUM              0                       /*主动上传单体信息配置组数*/
#define CAN_TX_PAR_NUM              0                       /*主动上传参数信息配置组数*/

/*周期发送信息配置总组数*/
#define CAN_TX_ALL_NUM              (CAN_TX_MUL_NUM+CAN_TX_SIN_NUM+CAN_TX_CEL_NUM+CAN_TX_PAR_NUM)

/*参数事件读写长度配置*/
#define CAN_RW_PAR_NUM              45                      /*一次(长帧)最大读写参数字节数*/
#define CAN_RO_RCD_NUM              51                      /*一次(长帧)最大读取数据记录信息字数*/
#define CAN_RO_WAV_NUM              50                      /*一次(长帧)最大读取故障录波信息字数*/

/*=======================================子索引长度配置===========================================*/
#define CANINDEX0_LEN               4                       /*CAN通信主索引0数组长度(校时信息)*/
#define CANINDEX1_LEN               8                       /*CAN通信主索引1数组长度(设备信息)*/
#define CANINDEX2_LEN               23                      /*CAN通信主索引2数组长度(设备序列号信息)*/
#define CANINDEX3_LEN               9                       /*CAN通信主索引3数组长度(CAN通信包基本信息)*/

#define CANINDEX10_LEN              (1+3*CAN_TX_VER_NUM)    /*CAN通信主索引10数组长度(CAN发送校时报文信息)*/
#define CANINDEX11_LEN              (1+3*CAN_TX_BRE_NUM)    /*CAN通信主索引11数组长度(CAN发送心跳报文信息)*/
#define CANINDEX12_LEN              (1+7*CAN_TX_MUL_NUM)    /*CAN通信主索引12数组长度(CAN发送主动更新多帧报文信息)*/
#define CANINDEX13_LEN              (1+7*CAN_TX_SIN_NUM)    /*CAN通信主索引13数组长度(CAN发送上传单帧报文信息)*/
#define CANINDEX14_LEN              (1+8*CAN_TX_CEL_NUM)    /*CAN通信主索引14数组长度(CAN发送上传单体信息单帧报文信息)*/
#define CANINDEX15_LEN              (1+8*CAN_TX_PAR_NUM)    /*CAN通信主索引15数组长度(CAN发送上传参数信息单帧报文信息)*/


/*=======================================通信数据类型定义=========================================*/
#define CAN_DT_U0                   0                       /*空类型*/
#define CAN_DT_U8                   1                       /*U8类型*/
#define CAN_DT_U16                  2                       /*U16类型*/
#define CAN_DT_U32                  4                       /*U32类型*/
#define CAN_DT_S8                   1                       /*S8类型*/
#define CAN_DT_S16                  2                       /*S16类型*/
#define CAN_DT_S32                  4                       /*S32类型*/

/*=======================================通信数据周期定义=========================================*/
/*监控口非CAN0口或不发送CATL协议(正常周期发送监控信息)*/
#define CAN_UP50_T                  100                     /*子索引50[状态]信息周期(1ms)*/
#define CAN_UP51_T                  100                     /*子索引51[限流]信息周期(1ms)*/
#define CAN_UP52_T                  500                     /*子索引52[限压]信息周期(1ms)*/
#define CAN_UP53_T                  500                     /*子索引53[告警]信息周期(1ms)*/


/*=================================================================================================
** @Data type definition
**===============================================================================================*/


/*CAN通信包基本配置信息索引定义*/
typedef enum
{
    eCANRBas3_DevNum = 0,               /*通信包设备表数目*/
	eCANRBas3_IndexNum = 1,             /*主控主索引数目*/
	eCANRBas3_TRTabNum = 2,             /*收发通信表数目*/
	eCANRBas3_TxVerNum = 3,             /*主动发送校时帧信息配置组数*/
	eCANRBas3_TxBreNum = 4,             /*主动上传心跳帧信息配置组数*/
	eCANRBas3_TxMulNum = 5,             /*主动更新发送长帧配置组数*/
	eCANRBas3_TxSinNum = 6,             /*主动更新发送单帧配置组数*/
	eCANRBas3_TxCelNum = 7,             /*主动上传单体信息配置组数*/
	eCANRBas3_TxParNum = 8              /*主动上传参数信息配置组数*/
}e_CANBaseInfo3;

/*CAN通信路由信息索引定义*/
typedef enum
{
    eCANRout4_En = 0,                   /*路由使能*/
    eCANRout4_Num = 1,                  /*路由组数*/
}e_CANRoutInfo4;

/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CANDeviceTableInit(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信设备表初始化
** @The notes : CAN通信包总初始化调用
**===============================================================================================*/
void CANDeviceTableInit(void);

/*=================================================================================================
** @Name      : void CANCommuPortUserInit(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信口用户初始化
** @The notes : CAN通信包总初始化调用
**===============================================================================================*/
void CANCommuPortUserInit(void);

/*=================================================================================================
** @Name      : void CANSetMsgCycleTxEnAPI(u8 state)
** @Input     : state:0禁止发送 1使能发送
** @Output    : void
** @Function  : 设置CAN通信包周期上传所有信息使能状态
** @The notes :
**===============================================================================================*/
void CANSetMsgCycleTxEnAPI(u8 state);

/*=================================================================================================
** @Name      : void CANSetParaCycleTxEnAPI(u8 state)
** @Input     : state:0禁止发送 1使能发送
** @Output    : void
** @Function  : 设置CAN通信包周期上传参数信息使能状态
** @The notes :
**===============================================================================================*/
void CANSetParaCycleTxEnAPI(u8 state);

#endif
