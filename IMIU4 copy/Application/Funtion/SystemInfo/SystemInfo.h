/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SystemInfo.h
** @Founder      : WYC834
** @Date         : 2025.11.4
** @Function     : 自身基本信息
** @Instructions : 系统基本信息
**===========================================================================================*/
#ifndef _SYSTEMINFO_H
#define _SYSTEMINFO_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define DIVICE_MARK 22012   //设备标识
#define DIVICE_ADDR 20   //设备地址
#define HARDWARE_VERSION   (00)   //硬件版本
#define SOFTWARE_VERSIONH  01   //软件版本H
#define SOFTWARE_VERSIONL  01   //软件版本L
#define BSP_VERSION 0           //BSP版本
#define CAN_LIB_VERSION 0       //CAN通信包版本
#define UART_LIB_VERSION 0      //USAT通信包版本


/*=============================================================================================
** @Data type definition
**===========================================================================================*/
enum
{
    SystemTime_Y = 0,                     //校时信息-年
	SystemTime_MD = 1,                    //校时信息-月日
	SystemTime_HM = 2,                    //校时信息-时分
	SystemTime_MS = 3,                    //校时信息-毫秒
    SystemTime_len = 4,
};
enum
{
    DeviceMark = 0,
    DeviceAddr = 1,
    HardwareVersion = 2,
    SoftwareVersionL = 3,
    SoftwareVersionH = 4,
    BspVersion = 5,
    CanComLibVersion = 6,
    UsartComLibVersion = 7,
    RunTimesL = 8,
    RunTimesH = 9,
    DeviceInfo1_len = 10,//设备信息
};

enum
{
    WorkMode = 0,
    WorkMode_Len = 1,
};


/*系统工作模式值*/
typedef enum
{
    eMODE_Normal  = 0,                    /*普通模式*/
    eMODE_Tooling  = 1,                   /*工装模式*/
    eMODE_Debug = 5,                      /*调试模式*/

}e_WorkMode;


extern u16 SystemTime_0[SystemTime_len];
extern u16 SystemInfo_1[DeviceInfo1_len];
extern u16 SystemWorkState_50[WorkMode_Len];
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
void InitSystemInfo(void);

/*=============================================================================================
** @Name      : u16 GetSystemInfoArray (u8 index);
** @Input     : index:系统数据下标
** @Output    : 系统数据
** @Function  : 获取系统数据
** @The notes :
**===========================================================================================*/
u16 GetSystemInfoArray (u8 index);

/*=============================================================================================
** @Name      : void InPutSystemInfoArray (u8 index ,u16 data);
** @Input     : index:系统数据下标 data:数据
** @Output    : void
** @Function  : 输入系统数据
** @The notes :
**===========================================================================================*/
void InPutSystemInfoArray (u8 index ,u16 data);

/*=============================================================================================
** @Name      : u16 GetSystemWorkModeArray (u8 index);
** @Input     : index:系统工作模式信息下标
** @Output    : 系统权限信息
** @Function  : 获取系统工作模式信息
** @The notes :
**===========================================================================================*/
u16 GetSystemWorkModeArray (u8 index);

/*=============================================================================================
** @Name      : void InPutSystemWorkModeArray (u8 index ,u16 data);
** @Input     : index:系统工作模式信息下标 data:数据
** @Output    : void
** @Function  : 输入系统工作模式信息
** @The notes :
**===========================================================================================*/
void InPutSystemWorkModeArray (u8 index ,u16 data);



#endif