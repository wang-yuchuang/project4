/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Parameter.h
** @Founder      : WYC834
** @Date         : 2025.11.3
** @Function     : 自身参数模块
** @Instructions : 系统自身所有参数
**===========================================================================================*/
#ifndef _SYSTEMPARA_H
#define _SYSTEMPARA_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
enum
{
	ParameterCigFlag = 0,       //常规参数配置标志
	CanComRate = 1,             //CAN通信波特率
	RS485ComRate = 2,           //RS485通信波特率
    CanCrolVersion = 3,         //CAN控制器版本
    CurrentSampChannel = 4,     //电流采样通道选择
    ShuntVoltSampRange = 5,     //分流器单向电压范围
    ShuntCurrentSampRange = 6,   //分流器单向电流量程
    HallSignalZeroVolt = 7,      //霍尔信号零点电压
    HallVoltSampRange = 8,       //霍尔单向电压范围
    HallCurrentSampRange = 9,     //霍尔单向电流量程
    CurrentFilterMethod = 10,     //电流滤波方式
    CurrentFilterTimes = 11,     //电流滤波次数
    AfterInsuPeRlyState = 12,   //绝缘检测后PE继电器状态
    InsuSampPeriod = 13,        //绝缘采样周期
    InsuTimeLimit = 14,           //绝缘检测超时时间
    RlySwiftInterTime = 15,     //板上继电器切换至少间隔时间


    Parameter_Len = 16,//常规信息当前值 
};
enum
{
    SerialNum1 = 0,
    SerialNum2 = 1,
    SerialNum3 = 2,
    SerialNum4 = 3,
    SerialNum5 = 4,
    SerialNum6 = 5,
    SerialNum7 = 6,
    SerialNum8 = 7,
    SerialNum9 = 8,
    SerialNum10 = 9,
    SerialNum11 = 10,
    SerialNum12 = 11,
    SerialNum13 = 12,
    SerialNum14 = 13,
    SerialNum15 = 14,
    SerialNum16 = 15,
    SerialNum17 = 16,
    SerialNum18 = 17,
    SerialNum19 = 18,
    SerialNum20 = 19,
    SerialNum21 = 20,
    SerialNum22 = 21,
    SerialNum23 = 22,
    SerialNum_len = 23,
    //序列号
};

typedef struct tagParameter//若修改参数位置，eeprom.h中的存储地址也需要相应修改
{
        u8 WriteFlag;                       //eeprom中对应地址0
        u8 SerialNumber[23];                //eeprom中对应地址1  
        u16 u16DeviceMark;                  //eeprom中对应地址24
        u16 u16HardwareVersion;             //eeprom中对应地址26
        u16 u16AppVersion;                  //eeprom中对应地址28
        u16 u16BootVersion;                 //eeprom中对应地址30
        //eeprom两页
        
        u16 u16ArraySet[Parameter_Len];     //eeprom中对应地址32
        //eeprom两页
        
        u32 BatVoltKBFlag;                  //eeprom中对应地址64
        f32 fBatVoltK;                      //eeprom中对应地址68
        f32 fBatVoltB;                      //eeprom中对应地址72
        u32 PeVoltKBFlag;                   //eeprom中对应地址76
        //eeprom一页
        f32 fPeVoltK;                       //eeprom中对应地址80
        f32 fPeVoltB;                       //eeprom中对应地址84
        u32 LinkVoltKBFlag;                 //eeprom中对应地址88
        f32 fLinkVoltK;                     //eeprom中对应地址92
        //eeprom一页
        f32 fLinkVoltB;                     //eeprom中对应地址96
        u32 HallCurrKBFlag;                 //eeprom中对应地址100
        f32 fHallCurrentK;                  //eeprom中对应地址104
        f32 fHallCurrentB;                  //eeprom中对应地址108
        //eeprom一页
        u32 BatShuntChgKBFlag;              //eeprom中对应地址112
        f32 fBatShuntCurrentChgK;           //eeprom中对应地址116
        f32 fBatShuntCurrentChgB;           //eeprom中对应地址120
        u32 BatShuntDhgKBFlag;              //eeprom中对应地址124
        //eeprom一页
        f32 fBatShuntCurrentDhgK;           //eeprom中对应地址128
        f32 fBatShuntCurrentDhgB;           //eeprom中对应地址132
        u32 LinkShuntChgKBFlag;             //eeprom中对应地址136
        f32 fLinkShuntCurrentChgK;          //eeprom中对应地址140
        //eeprom一页
        f32 fLinkShuntCurrentChgB;          //eeprom中对应地址144
        u32 LinkShuntDhgKBFlag;             //eeprom中对应地址148
        f32 fLinkShuntCurrentDhgK;          //eeprom中对应地址152
        f32 fLinkShuntCurrentDhgB;          //eeprom中对应地址156
        //eeprom一页
        u32 HallZeroCorrFlag;               //eeprom中对应地址160
        f32 fHallZeroCorr;                  //eeprom中对应地址164
}Parameter_TypeDef;


extern Parameter_TypeDef stParameter;
extern u16 ParameterNow_102[Parameter_Len];
extern u16 ParameterSet_107[Parameter_Len];
extern u8 SystemSerialNum_2[SerialNum_len];//序列号
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
void InitParameterArray(void);

/*=================================================================================================
** @Name      : u8 RcvParaSetFlashSaveAPI()
** @Input     : 
** @Output    : 设置成功或失败
** @Function  : 参数设置外部flash保存处理函数
** @The notes : 
**===============================================================================================*/
u8 RcvParaSetFlashSaveAPI();

/*=================================================================================================
** @Name      : u8 RcvDeviceSerialParaCfgAPI()
** @Input     : 
** @Output    : 设置成功或失败
** @Function  : 设备序列号参数设置命令处理函数
** @The notes : 
**===============================================================================================*/
u8 RcvDeviceSerialParaCfgAPI();

/*=================================================================================================
** @Name      : u8 RcvSystemGenParaCfgAPI(u8 index, u8 port)
** @Input     : index:参数字段索引号 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 系统通用参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 RcvSystemGenParaCfgAPI(u8 index, u8 port);

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSystemGenParaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据 
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统设备通用参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSystemGenParaInfo(u8 index, u16 data);

/*=================================================================================================
** @Name      : u8 WriteIntEepSystemGenParaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据 
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入系统设备通用参数信息,单个参数写入
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSystemGenParaInfo(u8 index, u16 data);

/*=================================================================================================
** @Name      : u8 ParaCheckSGenParaEffect(u32 addr, u32 para)
** @Input     : addr:参数地址 para:参数值
** @Output    : TRUE:有效合法 FALSE:非法无效
** @Function  : 检测系统通用参数值是否有效
** @The notes : U16
**===============================================================================================*/
u8 ParaCheckSGenParaEffect(u32 addr, u32 para);
#endif