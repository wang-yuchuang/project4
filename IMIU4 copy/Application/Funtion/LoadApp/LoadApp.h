/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : LoadApp.c
** @Founder      : WYP40505
** @Date         : 2026.05.06
** @Function     : 在线升级通信包模块
** @Instructions : APP应用程序使用
**===============================================================================================*/
#ifndef _UDSLOADAPP_H
#define _UDSLOADAPP_H
/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "BSPCAN.h"
#include "BSPFLASH.h"
#include "BSPGD25Q32E.h"

/*=================================================================================================
** @Macro definition
**===============================================================================================*/

// #define FLASH_DISPART_NUM               (5U)                        //FLASH分区

// #define FLASH_PART0_BOOT_ADDR           (0x08000000U)               //boot 起始地址   
// #define FLASH_PART0_BOOT_SIZE           (0xA000U)                    //boot 限制大小 40K
// #define FLASH_PART1_APP_ADDR            (0x0800A000U)               //app 起始地址   
// #define FLASH_PART1_APP_SIZE            (0x34800U)                   //app 限制大小 210K
// //** 没有备份区**//
// #define FLASH_PART3_APP_PARAM_ADDR      (FLASH_PART1_APP_ADDR + FLASH_PART1_APP_SIZE)             //app参数 起始地址   
// #define FLASH_PART3_APP_PARAM_SIZE      (2048U)                     //app参数 限制大小 2K

// #define FLASH_PART4_BOOT_PARAM_ADDR     (FLASH_PART3_APP_PARAM_ADDR + FLASH_PART3_APP_PARAM_SIZE)             //boot参数 起始地址   
// #define FLASH_PART4_BOOT_PARAM_SIZE     (2048U)                     //boot参数 限制大小 2K

// #define FLASH_PART5_SERIALNUM_ADDR      (FLASH_PART4_BOOT_PARAM_ADDR + FLASH_PART4_BOOT_PARAM_SIZE)              //序列号 起始地址   
// #define FLASH_PART5_SERIALNUM_SIZE      (2048U)                     //序列号 限制大小 2K



#define UPDATE_FLAG_BYTE0    0xAA
#define UPDATE_FLAG_BYTE1    0x55
#define UPDATE_FLAG_EXFLASH  0xA0
#define UPDATE_FLAG_INFLASH  0x50

#define FLASH_USER_UPDATE_FLAG_ADDR         ((uint32_t)(FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE*2))//将FLASH尾部的倒数第3块2K作为升级标志写入


/*=================================================================================================
** @Data type definition
**===============================================================================================*/

typedef enum
{
    RET_UPDATE_INFO_OK = 0,
    RET_UPDATE_INFO_ERR_HEAD = -1,
    RET_UPDATE_INFO_ERR_CHKSUM = -2,
}RET_UPDATE_FLAG;

typedef enum
{
    UPDATE_STA_START = 0,
    UPDATE_STA_ERASE_DATA,
    UPDATE_STA_PROGRAM_DATA,
    UPDATE_STA_PROGRAM_FLAG,
    UPDATE_STA_END,
    UPDATE_STA_FAIL,
}UPDATE_STATE;


typedef struct 
{
    u32 size;
}update_info_t;//升级相关信息

typedef struct
{
    u32 app_start_addr;
    u32 app_end_addr;
    u32 ext_flash_addr;
}update_info_ext_t;


/*按照小端模式(低位在前)存储数据*/
typedef enum                          /*BOOT参数写FLASH(扇区0)存放序号*/
{
    // eBOOT_PARA_FLAG_L_ID = 0,         /*BOOT参数配置标志低8位序号*/
    // eBOOT_PARA_FLAG_H_ID = 1,         /*BOOT参数配置标志高8位序号*/
    // eBOOT_EDV_ECU_L_ID = 2,           /*BOOT配置设备地址低8位序号*/
    // eBOOT_EDV_ECU_H_ID = 3,           /*BOOT配置设备地址高8位序号*/

    // eBOOT_JUM_FLAG_MODE_ID = 4,       /*程序跳转标志工作模式序号*/
    // eBOOT_JUM_FLAG_STAT_ID = 5,       /*程序跳转标志工作状态序号*/
    // eBOOT_APP_RUN_FLAG_L_ID = 6,      /*应用程序运行标志低8位序号*/
    // eBOOT_APP_RUN_FLAG_H_ID = 7,      /*应用程序运行标志高8位序号*/
    // eBOOT_APP_TRY_FLAG_L_ID = 8,      /*应用程序尝试跳转标志低8位序号*/
    // eBOOT_APP_TRY_FLAG_H_ID = 9,      /*应用程序尝试跳转标志高8位序号*/
    // eBOOT_APP_COP_FLAG_L_ID = 10,     /*应用程序备份有效标志低8位序号*/
    // eBOOT_APP_COP_FLAG_H_ID = 11,     /*应用程序备份有效标志高8位序号*/

    // eBOOT_HW_CFGBIT_L_ID = 12,        /*硬件参数配置记录位低8位序号*/
    // eBOOT_HW_CFGBIT_H_ID = 13,        /*硬件参数配置记录位高8位序号*/
    // eBOOT_CAN0_RATE_L_ID = 14,        /*硬件参数CAN0波特率低8位序号*/
    // eBOOT_CAN0_RATE_H_ID = 15,        /*硬件参数CAN0波特率高8位序号*/
    // eBOOT_CAN1_RATE_L_ID = 16,        /*硬件参数CAN1波特率低8位序号*/
    // eBOOT_CAN1_RATE_H_ID = 17,        /*硬件参数CAN1波特率高8位序号*/
    // eBOOT_CAN2_RATE_L_ID = 18,        /*硬件参数CAN2波特率低8位序号*/
    // eBOOT_CAN2_RATE_H_ID = 19,        /*硬件参数CAN2波特率高8位序号*/
    // eBOOT_SCI0_ADDR_L_ID = 20,        /*硬件参数RS485-0站地址低8位序号*/
    // eBOOT_SCI0_ADDR_H_ID = 21,        /*硬件参数RS485-0站地址高8位序号*/
    // eBOOT_SCI0_RATE_L_ID = 22,        /*硬件参数RS485-0波特率低8位序号*/
    // eBOOT_SCI0_RATE_H_ID = 23,        /*硬件参数RS485-0波特率高8位序号*/
    // eBOOT_SCI1_ADDR_L_ID = 24,        /*硬件参数RS485-1站地址低8位序号*/
    // eBOOT_SCI1_ADDR_H_ID = 25,        /*硬件参数RS485-1站地址高8位序号*/
    // eBOOT_SCI1_RATE_L_ID = 26,        /*硬件参数RS485-1波特率低8位序号*/
    // eBOOT_SCI1_RATE_H_ID = 27,        /*硬件参数RS485-1波特率高8位序号*/
    // eBOOT_SCI2_ADDR_L_ID = 28,        /*硬件参数RS485-2站地址低8位序号*/
    // eBOOT_SCI2_ADDR_H_ID = 29,        /*硬件参数RS485-2站地址高8位序号*/
    // eBOOT_SCI2_RATE_L_ID = 30,        /*硬件参数RS485-2波特率低8位序号*/
    // eBOOT_SCI2_RATE_H_ID = 31,        /*硬件参数RS485-2波特率高8位序号*/
    
    /*写入升级标志
 * 升级标志段包含几个内容：
 * [0~3]字节，AA55, 用于标记 存在升级数据，需要升级
 * [4~7]字节，待升级数据的大小，小端排序
 * [8~11]字节，如果使用外部nor-flash作为升级数据暂存，则该段为外部nor-flash的升级数据存放起始地址，如果使用的是内部eflash作为升级数据暂存，boot将忽略该4字节
 */
    eBOOT_UDS_IAP_FLAG_ID0 = 0,     //升级标志  升级：0XAA  正常模式：0x5A
    eBOOT_UDS_IAP_FLAG_ID1 = 1,     //跳转标志  升级：0X55  正常模式：0x5A
    eBOOT_UDS_IAP_TYPE_ID0 = 2,     //存储类型  外部flash：0xA0  内部flash：0x50
    eBOOT_UDS_IAP_TYPE_EXT = 3,     //保留

    eBOOT_UDS_IAP_DATALEN_ID0 = 4,      //升级数据(文件)的大小
    eBOOT_UDS_IAP_DATALEN_ID1 = 5,      //升级数据(文件)的大小
    eBOOT_UDS_IAP_DATALEN_ID2 = 6,      //升级数据(文件)的大小
    eBOOT_UDS_IAP_DATALEN_ID3 = 7,      //升级数据(文件)的大小

    eBOOT_UDS_IAP_FLASH_ADDR_ID0 = 8,   //外部FLASH地址
    eBOOT_UDS_IAP_FLASH_ADDR_ID1 = 9,   //外部FLASH地址
    eBOOT_UDS_IAP_FLASH_ADDR_ID2 = 10,  //外部FLASH地址
    eBOOT_UDS_IAP_FLASH_ADDR_ID3 = 11,  //外部FLASH地址

    // eBOOT_UDS_IAP_FLAG_ID0 = 32, /*UDS 升级*/
    // eBOOT_UDS_IAP_FLAG_ID1 = 33, /*UDS 升级*/
    // eBOOT_UDS_IAP_FLAG_ID2 = 34, /*UDS 升级*/
    // eBOOT_UDS_IAP_FLAG_ID3 = 35, /*UDS 升级*/

    eBOOT_PARA_FLASH_ID_LEN = 40      /*Boot参数写FLASH(扇区0)长度(写FLASH长度必须是8的倍数)*/
}e_BootParaFlashID;

typedef enum                                /*功能码*/
{
    LOAD_REQ_FUN = 13,                      /*升级查询*/
    LOAD_AEQ_FUN = 14,                      /*升级回复*/
    LOAD_RUP_FUN = 15,                      /*升级修改*/
}e_MsgFunCode;

#if 0
/*=================================================================================================
** @Data type definition
**===============================================================================================*/
typedef struct
{
    u8 canNum;                              /*CAN通道号*/
    u8 canType;                             /*CAN通道类型,0:上行通道 1:下行通道*/
}s_LoadRouteCfg;

typedef enum                                /*CAN通道路由类型*/
{
    eCANROUTE_UP = 0,                       /*上行通道*/
    eCANROUTE_DN = 1,                       /*下行通道*/
}e_CanRouteType;

typedef enum                               /*芯片类型*/
{
    eNXP_S32K146 = 0,                      /*S32K146*/
    eNXP_KEA128 = 1,                       /*KEA128*/
    eGD32F_303 = 2,                        /*GD32F303*/
    eAPM32E_103 = 3,                       /*APM32E103*/
    eGD32F_407 = 4,                        /*GD32F407*/
    eSTM32G0B1 = 5,                         /*stm32g0b1rct6*/
}e_McuType;



typedef enum                                /*信息类型主索引*/
{
    eSEARCHDEV_MSG = 0xf0,                  /*广播查询设备报文*/
    eHANDLE_MSG    = 0xf1,                  /*升级握手报文*/
    eLOADSTART_MSG = 0xf2,                  /*启动升级传输报文*/
    eLOADDATA_MSG  = 0xf3,                  /*发送升级数据报文*/
    eLOADFIN_MSG   = 0xf4,                  /*结束升级传输报文*/
}e_MsgCmdType;

typedef enum                                /*握手命令*/
{
    eLoCmd_ParaCfg = 1,                     /*设备参数设置命令*/
    eLoCmd_ProgRes = 2,                     /*设备程序复位命令*/
    eLoCmd_ChanMod = 3,                     /*切换工作模式命令*/
    eLoCmd_ProRdWr = 4,                     /*程序读写命令(安装或备份)*/
    eLoCmd_LoaCtrl = 5,                     /*程序升级命令*/
}e_HandleCmdType;

typedef enum                                /*操作命令*/
{
    eCtrlCmd_NorApp  = 0,                   /*切换正常应用模式命令*/
    eCtrlCmd_RemTra  = 1,                   /*切换远程传输模式命令*/
    eCtrlCmd_LocBoot = 3,                   /*切换本地升级模式命令*/
    eCtrlCmd_RemBoot = 4,                   /*切换本地升级模式命令*/

    eCtrlCmd_ReqInst = 1,                   /*请求安装命令(备份FLASH到内部FLASH)*/
    eCtrlCmd_EndInst = 2,                   /*终止安装命令*/
    eCtrlCmd_ReqCopy = 3,                   /*请求备份命令(内部FLASH到备份FLASH)*/
    eCtrlCmd_EndCopy = 4,                   /*终止备份命令*/
    eCtrlCmd_ReqBack = 5,                   /*请求回滚命令(备份FLASH到内部FLASH)*/
    eCtrlCmd_EndBack = 6,                   /*终止回滚命令*/

    eCtrlCmd_ReqLoad = 1,                   /*请求升级命令(上位机到设备)*/
    eCtrlCmd_EndLoad = 2                    /*终止升级命令*/
}e_CtrlCmdType;

typedef enum                                /*工作模式*/
{
    eLoMod_NorApp = 0,                      /*0:应用正常模式(APP)*/
    eLoMod_RemTra,                          /*1:远程传输模式(APP)*/
    eLoMod_NotApp,                          /*2:应用缺失模式(BOOT)*/
    eLoMod_LocBoot,                         /*3:本地升级模式(BOOT)*/
    eLoMod_RemBoot,                         /*4:远程升级模式(BOOT)*/

    eLoMod_Fin,                             /*工作模式结束*/
}e_LoadWorkMode;

typedef enum                                /*工作状态*/
{
    eLoSta_Route = 0,                       /*0:通信路由状态*/
    eLoSta_HandWait,                        /*1:等待握手状态(切换到可升级模式)*/
    eLoSta_HandSucc,                        /*2:握手成功状态(匹配固件识别码成功)*/
    eLoSta_HandFail,                        /*3:握手失败状态(匹配固件识别码失败)*/
    eLoSta_ErasSucc,                        /*4:擦除成功状态(擦除备份Flash成功)*/
    eLoSta_ErasFail,                        /*5:擦除失败状态(擦除备份Flash失败)*/
    eLoSta_LoaStart,                        /*6:开始升级状态(接收到段数据开始升级数据)*/
    eLoSta_LoaTrans,                        /*7:正在传输状态(下载固件数据流)*/
    eLoSta_TranSucc,                        /*8:传输成功*/
    eLoSta_TranFail,                        /*9:传输丢帧*/
    eLoSta_WritSucc,                        /*10:写入成功(写备份/内部Flash成功)*/
    eLoSta_WritFail,                        /*11:写入失败(写备份/内部Flash失败)*/
    eLoSta_WaitInst,                        /*12:等待安装(远程升级传输完成)*/
    eLoSta_FormSucc,                        /*13:格式化成功(擦除内部Flash成功)*/
    eLoSta_FormFail,                        /*14:格式化失败(擦除内部Flash失败)*/
    eLoSta_DoInstal,                        /*15:正在安装(从备份Flash拷贝到内部)*/
    eLoSta_InstSucc,                        /*16:安装成功*/
    eLoSta_InstFail,                        /*17:安装失败*/
    eLoSta_WaitCopy,                        /*18:等待备份(本地升级传输完成)*/
    eLoSta_Copying,                         /*19:正在备份(从内部Flash拷贝到备份)*/
    eLoSta_CopySucc,                        /*20:备份成功*/
    eLoSta_CopyFail,                        /*21:备份失败*/
    eLoSta_NotProg,                         /*22:无源程序*/

}e_LoadWorkState;


/*按照小端模式(低位在前)存储数据*/
typedef enum                                /*BOOT参数写FLASH(扇区0)存放序号*/
{
    // eBOOT_PARA_FLAG_L_ID = 0,               /*BOOT参数配置标志低8位序号*/
    // eBOOT_PARA_FLAG_H_ID = 1,               /*BOOT参数配置标志高8位序号*/
    // eBOOT_EDV_ECU_L_ID = 2,                 /*BOOT配置设备地址低8位序号*/
    // eBOOT_EDV_ECU_H_ID = 3,                 /*BOOT配置设备地址高8位序号*/

    // eBOOT_JUM_FLAG_MODE_ID = 4,             /*程序跳转标志工作模式序号*/
    // eBOOT_JUM_FLAG_STAT_ID = 5,             /*程序跳转标志工作状态序号*/
    // eBOOT_APP_RUN_FLAG_L_ID = 6,            /*应用程序运行标志低8位序号*/
    // eBOOT_APP_RUN_FLAG_H_ID = 7,            /*应用程序运行标志高8位序号*/
    // eBOOT_APP_TRY_FLAG_L_ID = 8,            /*应用程序尝试跳转标志低8位序号*/
    // eBOOT_APP_TRY_FLAG_H_ID = 9,            /*应用程序尝试跳转标志高8位序号*/
    // eBOOT_APP_COP_FLAG_L_ID = 10,           /*应用程序备份有效标志低8位序号*/
    // eBOOT_APP_COP_FLAG_H_ID = 11,           /*应用程序备份有效标志高8位序号*/

    // eBOOT_HW_CFGBIT_L_ID = 12,              /*硬件参数配置记录位低8位序号*/
    // eBOOT_HW_CFGBIT_H_ID = 13,              /*硬件参数配置记录位高8位序号*/
    // eBOOT_CAN0_RATE_L_ID = 14,              /*硬件参数CAN0波特率低8位序号*/
    // eBOOT_CAN0_RATE_H_ID = 15,              /*硬件参数CAN0波特率高8位序号*/
    // eBOOT_CAN1_RATE_L_ID = 16,              /*硬件参数CAN1波特率低8位序号*/
    // eBOOT_CAN1_RATE_H_ID = 17,              /*硬件参数CAN1波特率高8位序号*/
    // eBOOT_CAN2_RATE_L_ID = 18,              /*硬件参数CAN2波特率低8位序号*/
    // eBOOT_CAN2_RATE_H_ID = 19,              /*硬件参数CAN2波特率高8位序号*/
    // eBOOT_SCI0_ADDR_L_ID = 20,              /*硬件参数RS485-0站地址低8位序号*/
    // eBOOT_SCI0_ADDR_H_ID = 21,              /*硬件参数RS485-0站地址高8位序号*/
    // eBOOT_SCI0_RATE_L_ID = 22,              /*硬件参数RS485-0波特率低8位序号*/
    // eBOOT_SCI0_RATE_H_ID = 23,              /*硬件参数RS485-0波特率高8位序号*/
    // eBOOT_SCI1_ADDR_L_ID = 24,              /*硬件参数RS485-1站地址低8位序号*/
    // eBOOT_SCI1_ADDR_H_ID = 25,              /*硬件参数RS485-1站地址高8位序号*/
    // eBOOT_SCI1_RATE_L_ID = 26,              /*硬件参数RS485-1波特率低8位序号*/
    // eBOOT_SCI1_RATE_H_ID = 27,              /*硬件参数RS485-1波特率高8位序号*/
    // eBOOT_SCI2_ADDR_L_ID = 28,              /*硬件参数RS485-2站地址低8位序号*/
    // eBOOT_SCI2_ADDR_H_ID = 29,              /*硬件参数RS485-2站地址高8位序号*/
    // eBOOT_SCI2_RATE_L_ID = 30,              /*硬件参数RS485-2波特率低8位序号*/
    // eBOOT_SCI2_RATE_H_ID = 31,              /*硬件参数RS485-2波特率高8位序号*/
    // eBOOT_UDS_IAP_FLAG_ID0 = 32, /*UDS 升级*/
    // eBOOT_UDS_IAP_FLAG_ID1 = 33, /*UDS 升级*/
    // eBOOT_UDS_IAP_FLAG_ID2 = 34, /*UDS 升级*/
    // eBOOT_UDS_IAP_FLAG_ID3 = 35, /*UDS 升级*/

    eBOOT_UDS_IAP_FLAG_ID0 = 0,     //升级标志  升级：0XAA  正常模式：0x5A
    eBOOT_UDS_IAP_FLAG_ID1 = 1,     //升级标志  升级：0X55  正常模式：0x5A
    eBOOT_UDS_IAP_TYPE_ID0 = 2,     //存储类型  外部flash：0xA0  内部flash：0x50
    eBOOT_UDS_IAP_TYPE_EXT = 3,     //保留

    eBOOT_UDS_IAP_DATALEN_ID0 = 4,      //升级数据(文件)的大小
    eBOOT_UDS_IAP_DATALEN_ID1 = 5,      //升级数据(文件)的大小
    eBOOT_UDS_IAP_DATALEN_ID2 = 6,      //升级数据(文件)的大小
    eBOOT_UDS_IAP_DATALEN_ID3 = 7,      //升级数据(文件)的大小

    eBOOT_UDS_IAP_FLASH_ADDR_ID0 = 8,   //外部FLASH地址
    eBOOT_UDS_IAP_FLASH_ADDR_ID1 = 9,   //外部FLASH地址
    eBOOT_UDS_IAP_FLASH_ADDR_ID2 = 10,  //外部FLASH地址
    eBOOT_UDS_IAP_FLASH_ADDR_ID3 = 11,  //外部FLASH地址

    eBOOT_PARA_FLASH_ID_LEN = 40 /*Boot参数写FLASH(扇区0)长度(写FLASH长度必须是8的倍数)*/
}e_BootParaFlashID;

typedef enum                                /*应用程序参数写FLASH扇区2存放序号*/
{
    eAPP_PARA_FLAG_L_ID = 0,                /*应用程序参数配置标志低8位序号*/
    eAPP_PARA_FLAG_H_ID = 1,                /*应用程序参数配置标志高8位序号*/
    eAPP_DEV_ECU_L_ID = 2,                  /*应用程序设备地址低8位序号*/
    eAPP_DEV_ECU_H_ID = 3,                  /*应用程序设备地址高8位序号*/
    eAPP_PARA_SEC2_ID_LEN                   /*应用程序参数写FLASH扇区2长度*/
}e_AppParaFlashID;

typedef enum                                /*应用程序硬件参数写FLASH扇区0存放序号*/
{
    eAPP_HWPARA_FLAG_L_ID = 0,              /*应用程序硬件参数配置标志低8位序号*/
    eAPP_HWPARA_FLAG_H_ID = 1,              /*应用程序硬件参数配置标志高8位序号*/
    eAPP_CAN0_RATE_L_ID = 4,                /*应用程序CAN0波特率低8位序号*/
    eAPP_CAN0_RATE_H_ID = 5,                /*应用程序CAN0波特率高8位序号*/
    eAPP_CAN1_RATE_L_ID = 6,                /*应用程序CAN1波特率低8位序号*/
    eAPP_CAN1_RATE_H_ID = 7,                /*应用程序CAN1波特率高8位序号*/
    eAPP_CAN2_RATE_L_ID = 8,                /*应用程序CAN2波特率低8位序号*/
    eAPP_CAN2_RATE_H_ID = 9,                /*应用程序CAN2波特率高8位序号*/
    eAPP_SCI0_ADDR_L_ID = 10,               /*应用程序RS485-0站地址低8位序号*/
    eAPP_SCI0_ADDR_H_ID = 11,               /*应用程序RS485-0站地址高8位序号*/
    eAPP_SCI0_RATE_L_ID = 12,               /*应用程序RS485-0波特率低8位序号*/
    eAPP_SCI0_RATE_H_ID = 13,               /*应用程序RS485-0波特率高8位序号*/
    eAPP_SCI1_ADDR_L_ID = 14,               /*应用程序RS485-1站地址低8位序号*/
    eAPP_SCI1_ADDR_H_ID = 15,               /*应用程序RS485-1站地址高8位序号*/
    eAPP_SCI1_RATE_L_ID = 16,               /*应用程序RS485-1波特率低8位序号*/
    eAPP_SCI1_RATE_H_ID = 17,               /*应用程序RS485-1波特率高8位序号*/
    eAPP_SCI2_ADDR_L_ID = 18,               /*应用程序RS485-2站地址低8位序号*/
    eAPP_SCI2_ADDR_H_ID = 19,               /*应用程序RS485-2站地址高8位序号*/
    eAPP_SCI2_RATE_L_ID = 20,               /*应用程序RS485-2波特率低8位序号*/
    eAPP_SCI2_RATE_H_ID = 21,               /*应用程序RS485-2波特率高8位序号*/

    eAPP_HWPARA_SEC0_ID_LEN                 /*应用程序硬件参数写FLASH扇区0长度*/
}e_AppHWParaID;
#endif
/*=================================================================================================
** @Interface function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : void BootLoaderInit(void)
** @Input     : void
** @Output    : void
** @Function  : 在线升级模块初始化
** @The notes : BOOT和APP有区别
**===============================================================================================*/
void APPLoaderInit(void);

/*=================================================================================================
** @Name      : void CanRcvLoadMsgHandleAPI(t_CANMSG *msg, u8 canNum)
** @Input     : msg:接收到的报文 canNum:接收CAN通道号
** @Output    : void
** @Function  : 在线升级模块CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvLoadMsgHandleAPI(t_CANMSG *msg, u8 canNum);



#endif
