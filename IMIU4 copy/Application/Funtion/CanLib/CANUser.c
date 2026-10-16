/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANUser.c
** @Founder      : WYC834
** @Date         : 2025.11.23
** @Function     : CAN通信包用户接口
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "CANInterface.h"
#include "CANUser.h"
#include "SystemInfo.h"
#include "SystemData.h"
#include "SystemPara.h"
#include "SystemDebug.h"
#include "SystemDiag.h"
#include "Insulation.h"
#include "BSPCAN.h"
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/

/*=======================================基本配置信息=============================================*/
u8 gCANBaseDef_3[CANINDEX3_LEN] =
{
    CAN_DEVTAB_NUM,                 /*通信设备表数目1*/
    BMU_INDEX_NUM,                  /*主控主索引数目16*/
    CAN_TRTAB_NUM,                  /*收发通信表数目10*/
    CAN_TX_VER_NUM,                 /*主动发送校时帧信息配置组数0*/
    CAN_TX_BRE_NUM,                 /*主动上传心跳帧信息配置组数3*/
    CAN_TX_MUL_NUM,                 /*主动更新发送长帧配置组数8*/
    CAN_TX_SIN_NUM,                 /*主动更新发送单帧配置组数6*/
    CAN_TX_CEL_NUM,                 /*主动上传单体信息配置组数6*/
    CAN_TX_PAR_NUM                  /*主动上传参数信息配置组数*/
};



/*=======================================校时帧发送配置===========================================*/
u8 gCANTxVerDef_10[CANINDEX10_LEN] =
{
    1                               /*校时帧发送方式(0:不转发不发送 1:仅转发 2:转发和周期发)*/

    /*CAN1口1s周期*/
    /*eCAN1,*/                      /*第1组校时帧发送通道*/
    /*232,*/                        /*第1组校时帧发送周期L(1ms)*/
    /*3*/                           /*第1组校时帧发送周期H(1ms)*/
};

/*=======================================心跳帧发送配置===========================================*/
u8 gCANTxBreDef_11[CANINDEX11_LEN] =
{
    1,                              /*心跳帧发送使能(0:禁止 1:使能)*/

    /*CAN0口1s周期 */
    eCAN1,                          /*第1组心跳帧发送通道*/
    232,                            /*第1组心跳帧发送周期L(1ms)*/
    3,                              /*第1组心跳帧发送周期H(1ms)*/

};

/*=======================================主动更新(长帧)发送配置====================================*/
u8 gCANTxMulDef_12[CANINDEX12_LEN] =
{
    0,                              /*主动更新长帧发送使能(0:禁止 1:使能)*/

    // /*第4组:主索引 82(调试结果),CAN0口1s周期 */
    // 82,                             /*主动更新长帧主索引号*/
    // 0,                              /*主动更新长帧起始子索引号*/
    // GINDEX82_LEN,                   /*主动更新长帧子索引长度*/
    // 0xe8,                           /*主动更新长帧发送周期L(1ms)*/
    // 0x03,                           /*主动更新长帧发送周期H(1ms)*/
    // CAN_UPDATE_PORT,                /*主动更新长帧发送通道*/
    // 0xff,                           /*主动更新长帧发送目标地址*/

    // /*第5组:主索引 83(校准结果),CAN0口1s周期 */
    // 83,                             /*主动更新长帧主索引号*/
    // 0,                              /*主动更新长帧起始子索引号*/
    // GINDEX83_LEN,                   /*主动更新长帧子索引长度*/
    // 0xe8,                           /*主动更新长帧发送周期L(1ms)*/
    // 0x03,                           /*主动更新长帧发送周期H(1ms)*/
    // CAN_UPDATE_PORT,                /*主动更新长帧发送通道*/
    // 0xff,                           /*主动更新长帧发送目标地址*/


};

/*=======================================主动发送(单帧)发送配置====================================*/
u8 gCANTxSinDef_13[CANINDEX13_LEN] =
{
    1,                              /*主动更新单帧发送使能(0:禁止 1:使能)*/

    /*第1组:主索引 51(系统信息),CAN0口10ms周期 */
    51,                             /*主动发送单帧主索引号*/
    0,                              /*主动发送单帧起始子索引号*/
    SystemData_Len,                   /*主动发送单帧子索引长度*/
    (0x000A & 0xff),            /*主动发送单帧发送周期L(1ms)*/
    ((0x000A >> 8) & 0xff),     /*主动发送单帧发送周期H(1ms)*/
    CAN_UPDATE_PORT,                /*主动发送单帧发送通道*/
    0xff,                           /*主动发送单帧发送目标地址*/

    /*第2组:主索引 54(权限信息),CAN0口1s周期 */
    54,                             /*主动发送单帧主索引号*/
    0,                              /*主动发送单帧起始子索引号*/
    PermissionInfo_Len,                   /*主动发送单帧子索引长度*/
    (0x03e8 & 0xff),            /*主动发送单帧发送周期L(1ms)*/
    ((0x03e8 >> 8) & 0xff),     /*主动发送单帧发送周期H(1ms)*/
    CAN_UPDATE_PORT,                /*主动发送单帧发送通道*/
    0xff,  

    /*第3组:主索引 56(控制反馈),CAN0口1s周期 */
    56,                             /*主动发送单帧主索引号*/
    0,                              /*主动发送单帧起始子索引号*/
    CtrlFeedback_len,                   /*主动发送单帧子索引长度*/
    (0x03e8 & 0xff),            /*主动发送单帧发送周期L(1ms)*/
    ((0x03e8 >> 8) & 0xff),     /*主动发送单帧发送周期H(1ms)*/
    CAN_UPDATE_PORT,                /*主动发送单帧发送通道*/
    0xff, 

    /*第4组:主索引 57(诊断信息),CAN0口1s周期 */
    57,                             /*主动发送单帧主索引号*/
    0,                              /*主动发送单帧起始子索引号*/
    SystemDiag_Len,                   /*主动发送单帧子索引长度*/
    (0x03e8 & 0xff),            /*主动发送单帧发送周期L(1ms)*/
    ((0x03e8 >> 8) & 0xff),     /*主动发送单帧发送周期H(1ms)*/
    CAN_UPDATE_PORT,                /*主动发送单帧发送通道*/
    0xff, 

    /*第5组:主索引 58(诊断信息),CAN0口1s周期 */
    58,                             /*主动发送单帧主索引号*/
    0,                              /*主动发送单帧起始子索引号*/
    Calibration_Len,                   /*主动发送单帧子索引长度*/
    (0x03e8 & 0xff),            /*主动发送单帧发送周期L(1ms)*/
    ((0x03e8 >> 8) & 0xff),     /*主动发送单帧发送周期H(1ms)*/
    CAN_UPDATE_PORT,                /*主动发送单帧发送通道*/
    0xff, 

    /*第6组:主索引 61(调试反馈),CAN0口1s周期 */
    61,                             /*主动发送单帧主索引号*/
    0,                              /*主动发送单帧起始子索引号*/
    DebugFeedback_Len,                   /*主动发送单帧子索引长度*/
    (0x03e8 & 0xff),            /*主动发送单帧发送周期L(1ms)*/
    ((0x03e8 >> 8) & 0xff),     /*主动发送单帧发送周期H(1ms)*/
    CAN_UPDATE_PORT,                /*主动发送单帧发送通道*/
    0xff,
};


/*=======================================主动发送(单体)发送配置===================================*/
u8 gCANTxCelDef_14[CANINDEX14_LEN] =
{
    1,                              /*主动更新单体发送使能(0:禁止 1:使能)*/

    // /*第1组:主索引 92(单体电压1),CAN0口1s周期 */
    // 92,                             /*主动发送单体主索引号*/
    // 0,                              /*主动发送单体起始子索引号*/
    // GINDEX92_LEN,                   /*主动发送单体子索引长度*/
    // 50,                             /*主动发送单体映射起始主索引号*/
    // 0xe8,                           /*主动发送单体发送周期L(1ms)*/
    // 0x03,                           /*主动发送单体发送周期H(1ms)*/
    // CAN_UPDATE_PORT,                /*主动发送单体发送通道*/
    // 0xff,                           /*主动发送单体发送目标地址*/

};
/*=======================================主动发送(参数)发送配置===================================*/
u8 gCANTxParDef_15[CANINDEX15_LEN] =
{
    0,                              /*主动更新参数发送使能(0:禁止 1:使能)*/

    // /*第1组:主索引 102(系统通用参数),CAN1口10s周期 */
    // 102,                            /*主动发送参数主索引号*/
    // 0,                              /*主动发送参数起始子索引号*/
    // Parameter_Len,                  /*主动发送参数子索引长度*/
    // 0,                              /*主动发送参数映射起始主索引号*/
    // 0x10,                           /*主动发送单帧发送周期L(1ms)*/
    // 0x27,                           /*主动发送单帧发送周期H(1ms)*/
    // CAN_UPDATE_PORT,                /*主动发送参数发送通道*/
    // 0xff,                           /*主动发送参数发送目标地址*/


};



/*=======================================用户自定义信息(通信协议)=================================*/


/*=======================================通信信息主索引表(BMU)====================================*/
const t_CANIndexTab Filed_BMUTab[BMU_INDEX_NUM] =
{
    /*内部信息 23个主索引*/
    DeclareIndexTableEntry(CAN_WRITE,    0,    SystemTime_0,          CAN_DT_U16, SystemTime_len),//校时信息
    DeclareIndexTableEntry(CAN_READONLY, 1,    SystemInfo_1,          CAN_DT_U16, DeviceInfo1_len),//设备信息
    DeclareIndexTableEntry(CAN_WRITE,    2,    SystemSerialNum_2,     CAN_DT_U8,  SerialNum_len),//序列号
    DeclareIndexTableEntry(CAN_WRITE,    50,   SystemWorkState_50,    CAN_DT_U16, WorkMode_Len),//工作模式
    DeclareIndexTableEntry(CAN_READONLY, 51,   SystemData_51,         CAN_DT_S16, SystemData_Len),//采样信息
    DeclareIndexTableEntry(CAN_WRITE,    52,   CalibrationCtrl_52,    CAN_DT_U16, SysCorrCmd_Len),//校准命令长度
    DeclareIndexTableEntry(CAN_WRITE,    53,   AccessPassword_53,     CAN_DT_U16, Password_Len),//权限密码
    DeclareIndexTableEntry(CAN_READONLY, 54,   SystemPermission_54,   CAN_DT_U16, PermissionInfo_Len),//权限级别
    DeclareIndexTableEntry(CAN_WRITE,    55,   UserCtrlCmd_55,        CAN_DT_U16, UserCtrlCmd_len),//用户控制命令
    DeclareIndexTableEntry(CAN_READONLY, 56,   CtrlCmdFeedback_56,    CAN_DT_U16, CtrlFeedback_len),//控制命令反馈
    DeclareIndexTableEntry(CAN_READONLY, 57,   SystemDiag_57,         CAN_DT_U16, SystemDiag_Len),//故障告警信息
    DeclareIndexTableEntry(CAN_READONLY, 58,   CaliFeedback_58,       CAN_DT_U16, Calibration_Len),//采样校准反馈信息
    DeclareIndexTableEntry(CAN_WRITE,    60,   SystemDebugCmd_60,     CAN_DT_U16, Debug_Len),//系统调试命令
    DeclareIndexTableEntry(CAN_READONLY, 61,   DebugFeedback_61,      CAN_DT_U16, DebugFeedback_Len),//系统调试反馈信息
    DeclareIndexTableEntry(CAN_READONLY, 102,  ParameterNow_102,      CAN_DT_U16, Parameter_Len),//常规信息当前值
    DeclareIndexTableEntry(CAN_WRITE,    107,  ParameterSet_107,      CAN_DT_U16, Parameter_Len), // 常规信息设置值
};

/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void CANDeviceTableInit(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信设备表初始化
** @The notes : CAN通信包总初始化调用
**===============================================================================================*/
void CANDeviceTableInit(void)
{
    /*将通信主索引表创建到设备表*/
    CANDevTabEntryInit_API((u8)SystemInfo_1[DeviceAddr], BMU_INDEX_NUM, Filed_BMUTab, 0);
}

/*=================================================================================================
** @Name      : void CANCommuPortUserInit(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信口用户初始化
** @The notes : CAN通信包总初始化调用
**===============================================================================================*/
void CANCommuPortUserInit(void)
{
    /*用户配置波特率重新初始化*/
    //DEVCANResetInit(eCAN0);     /*初始化CAN0*/
    //DEVCANResetInit(eCAN1);     /*初始化CAN1*/
    //DEVCANResetInit(eCAN2);     /*初始化CAN2*/
}

/*=================================================================================================
** @Name      : void CANSetMsgCycleTxEnAPI(u8 state)
** @Input     : state:0禁止发送 1使能发送
** @Output    : void
** @Function  : 设置CAN通信包周期上传所有信息使能状态
** @The notes :
**===============================================================================================*/
void CANSetMsgCycleTxEnAPI(u8 state)
{
    /*禁止*/
    if(0 == state)
    {
        gCANTxVerDef_10[0] = 0;
        gCANTxBreDef_11[0] = 0;
        gCANTxMulDef_12[0] = 0;
        gCANTxSinDef_13[0] = 0;
        gCANTxCelDef_14[0] = 0;
        gCANTxParDef_15[0] = 0;
    }
    /*使能*/
    else
    {
        gCANTxVerDef_10[0] = 1;
        gCANTxBreDef_11[0] = 1;
        gCANTxMulDef_12[0] = 1;
        gCANTxSinDef_13[0] = 1;
        gCANTxCelDef_14[0] = 1;
        gCANTxParDef_15[0] = 1;
    }
}


/*=================================================================================================
** @Name      : void CANSetParaCycleTxEnAPI(u8 state)
** @Input     : state:0禁止发送 1使能发送
** @Output    : void
** @Function  : 设置CAN通信包周期上传参数信息使能状态
** @The notes :
**===============================================================================================*/
void CANSetParaCycleTxEnAPI(u8 state)
{
    /*禁止*/
    if(0 == state)
    {
        gCANTxParDef_15[0] = 0;
    }
    /*使能*/
    else
    {
        gCANTxParDef_15[0] = 1;
    }
}

/*=================================================================================================
** @Static function definition
**===============================================================================================*/


