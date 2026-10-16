/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SystemData.c
** @Founder      : WYC834
** @Date         : 2025.10.21
** @Function     : 系统数据模块
** @Instructions : 系统自身所有信息
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "SystemData.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
SystemData_TypeDef gSystemData;//系统信息
s16 SystemData_51[SystemData_Len];
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void InPutSystemDataArray (u8 index ,s16 data);
** @Input     : index:系统数据下标 data:数据
** @Output    : void
** @Function  : 输入系统数据
** @The notes :
**===========================================================================================*/
void InPutSystemDataArray (u8 index ,s16 data)
{
    if (index >= SystemData_Len) return;  /* 越界保护: 防止写穿数组破坏堆栈 */
    SystemData_51[index] = data;
}

/*=============================================================================================
** @Name      : s16 GetSystemDataArray (u8 index);
** @Input     : index:系统数据下标
** @Output    : 系统数据
** @Function  : 获取系统数据
** @The notes :
**===========================================================================================*/
s16 GetSystemDataArray (u8 index)
{
    if (index >= SystemData_Len) return 0;  /* 越界保护 */
    return SystemData_51[index];
}

/*=============================================================================================
** @Name      : void InPutDiState(u8 state)
** @Input     : state:DI状态输入
** @Output    : void
** @Function  : DI状态输入
** @The notes : 
**===========================================================================================*/
void InPutDiState(u8 state)
{
    gSystemData.u8DiState = state;
}

/*=============================================================================================
** @Name      : u8 GetSwAddr(void)
** @Input     : void
** @Output    : 获取拨码地址
** @Function  : 
** @The notes : 
**===========================================================================================*/
u8 GetSwAddr(void)
{
    return gSystemData.u8SwAddr;
}
/*=============================================================================================
** @Name      : void InPutDoState(u8 channel, u8 data)
** @Input     : channel:do通道 data:do状态
** @Output    : void
** @Function  : do状态输入
** @The notes : 
**===========================================================================================*/
void InPutDoState(u8 channel, u8 data)
{
    if(channel > 1)
    {
        return;
    }
    gSystemData.u8DoState[channel] = data;
}   

/*=============================================================================================
** @Name      : void InPutSwAddr(u8 data)
** @Input     : data:拨码地址
** @Output    : void
** @Function  : 拨码地址输入
** @The notes : 
**===========================================================================================*/
void InPutSwAddr(u8 data)
{
    gSystemData.u8SwAddr = data;
}

/*=============================================================================================
** @Name      : void InPutNtcTemp(u8 channel, f32 data)
** @Input     : channel:ntc通道 data:温度数据
** @Output    : void
** @Function  : ntc温度数据输入
** @The notes : 
**===========================================================================================*/
void InPutNtcTemp(u8 channel, f32 data)
{
    if(channel > 1)
    {
        return;
    }
    gSystemData.fNtcTemp[channel] = data;
}

/*=============================================================================================
** @Name      : void InPutBatShuntTemp(f32 data)
** @Input     : data:温度数据
** @Output    : void
** @Function  : 电池端分流器温度数据输入
** @The notes : 
**===========================================================================================*/
void InPutBatShuntTemp(f32 data)
{
    gSystemData.fBatShuntTemp = data;
}

/*=============================================================================================
** @Name      : void InPutBatShuntVolt(f32 data)
** @Input     : data:电池端分流器电压数据
** @Output    : void
** @Function  : 电池端分流器电压数据
** @The notes : 
**===========================================================================================*/
void InPutBatShuntVolt(f32 data)
{
    gSystemData.fBatShuntVolt = data;
}
/*=============================================================================================
** @Name      : f32 GetBatShuntVolt(void)
** @Input     : void
** @Output    : 输出电池端分流器电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetBatShuntVolt(void)
{
    return gSystemData.fBatShuntVolt;
}

/*=============================================================================================
** @Name      : void InPutfAdjustBatShuntVolt(f32 data)
** @Input     : data:电池端分流器校准后电压数据
** @Output    : void
** @Function  : 电池端分流器校准后电压数据
** @The notes : 校准后
**===========================================================================================*/
void InPutfAdjustBatShuntVolt(f32 data)
{
    gSystemData.fAdjustBatShuntVolt = data;
}
/*=============================================================================================
** @Name      : f32 GetfAdjustBatShuntVolt(void)
** @Input     : void
** @Output    : 
** @Function  : 电池端分流器校准后电压数据
** @The notes : 校准后
**===========================================================================================*/
f32 GetfAdjustBatShuntVolt(void)
{
    return gSystemData.fAdjustBatShuntVolt;
}



/*=============================================================================================
** @Name      : void InPutBatShuntCurrent(f32 data)
** @Input     : data:电池端分流器电流数据
** @Output    : void
** @Function  : 电池端分流器电流数据
** @The notes : 
**===========================================================================================*/
void InPutBatShuntCurrent(f32 data)
{
    gSystemData.fBatShuntCurrent = data;
}

/*=============================================================================================
** @Name      : f32 GetBatShuntCurrent(void)
** @Input     : void
** @Output    : data:电池端分流器电流数据
** @Function  : 返回电池端分流器电流数据
** @The notes : 
**===========================================================================================*/
f32 GetBatShuntCurrent(void)
{
    return gSystemData.fBatShuntCurrent;
}
/*=============================================================================================
** @Name      : void InPutfAdjustBatShuntCurrent(f32 data)
** @Input     : 电池端分流器校准后电流数据
** @Output    : void
** @Function  : 
** @The notes : 校准后
**===========================================================================================*/
void InPutfAdjustBatShuntCurrent(f32 data)
{
    gSystemData.fAdjustBatShuntCurrent = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustBatShuntCurrent(void)
** @Input     : void
** @Output    : 返回电池端分流器校准后电流数据
** @Function  : 
** @The notes : 校准后
**===========================================================================================*/
f32 GetfAdjustBatShuntCurrent(void)
{
    return gSystemData.fAdjustBatShuntCurrent;
}

/*=============================================================================================
** @Name      : void InPutfBatShuntVoltBackup(f32 data)
** @Input     : data:电池端分流器电压数据(备份)
** @Output    : void
** @Function  : 电池端分流器电压数据（备份）
** @The notes : 
**===========================================================================================*/
void InPutfBatShuntVoltBackup(f32 data)
{
    gSystemData.fBatShuntVoltBackup = data;
}
/*=============================================================================================
** @Name      : f32 GetfBatShuntVoltBackup(void)
** @Input     : void
** @Output    : 返回电池端分流器校准后电流数据(备份)
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetfBatShuntVoltBackup(void)
{
    return gSystemData.fBatShuntVoltBackup;
}

/*=============================================================================================
** @Name      : void InPutfAdjustBatShuntVoltBackup(f32 data)
** @Input     : data:电池端分流器校准后电压数据(备份)
** @Output    : void
** @Function  : 电池端分流器校准后电压数据（备份）
** @The notes : 校准后
**===========================================================================================*/
void InPutfAdjustBatShuntVoltBackup(f32 data)
{
    gSystemData.fAdjustBatShuntVoltBackup = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustBatShuntVoltBackup(void)
** @Input     : void
** @Output    : 返回电池端分流器校准后电压数据（备份）
** @Function  : 
** @The notes : 校准后
**===========================================================================================*/
f32 GetfAdjustBatShuntVoltBackup(void)
{
    return gSystemData.fAdjustBatShuntVoltBackup;
}


/*=============================================================================================
** @Name      : void InPutfBatShuntCurrentBackup(f32 data)
** @Input     : data:电池端分流器电流数据(备份)
** @Output    : void
** @Function  : 电池端分流器电流数据（备份）
** @The notes : 
**===========================================================================================*/
void InPutfBatShuntCurrentBackup(f32 data)
{
    gSystemData.fBatShuntCurrentBackup = data;
}

/*=============================================================================================
** @Name      : f32 GetfBatShuntCurrentBackup(void)
** @Input     : void
** @Output    : 返回电池端分流器电流数据（备份）
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetfBatShuntCurrentBackup(void)
{
    return gSystemData.fBatShuntCurrentBackup;
}

/*=============================================================================================
** @Name      : void InPutfAdjustBatShuntCurrentBackup(f32 data)
** @Input     : 电池端分流器校准后电流数据（备份）
** @Output    : void
** @Function  : 
** @The notes : 校准后
**===========================================================================================*/
void InPutfAdjustBatShuntCurrentBackup(f32 data)
{
    gSystemData.fAdjustBatShuntCurrentBackup = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustBatShuntCurrentBackup(void)
** @Input     : void
** @Output    : 返回电池端分流器校准后电流数据（备份）
** @Function  : 
** @The notes : 校准后
**===========================================================================================*/
f32 GetfAdjustBatShuntCurrentBackup(void)
{
    return gSystemData.fAdjustBatShuntCurrentBackup;
}

/*=============================================================================================
** @Name      : void InPutHallVolt(f32 data)
** @Input     : data:霍尔电压数据
** @Output    : void
** @Function  : 霍尔电压数据输入
** @The notes : 
**===========================================================================================*/
void InPutHallVolt(f32 data)
{
    gSystemData.fHallVolt = data;
}

/*=============================================================================================
** @Name      : f32 GetHallVolt(void)
** @Input     : void
** @Output    : 输出霍尔电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetHallVolt(void)
{
    return gSystemData.fHallVolt;
}

/*=============================================================================================
** @Name      : void InPutfAdjustZeroHallVolt(f32 data)
** @Input     : 霍尔校零电压数据
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
void InPutfAdjustZeroHallVolt(f32 data)
{
    gSystemData.fAdjustZeroHallVolt = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustZeroHallVolt(void)
** @Input     : void
** @Output    : 输出霍尔校零电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetfAdjustZeroHallVolt(void)
{
    return gSystemData.fAdjustZeroHallVolt;
}

/*=============================================================================================
** @Name      : void InPutfAdjustHallVolt(f32 data)
** @Input     : 霍尔校准后电压
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
void InPutfAdjustHallVolt(f32 data)
{
    gSystemData.fAdjustHallVolt = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustHallVolt(void)
** @Input     : void
** @Output    : 霍尔校准后电压
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetfAdjustHallVolt(void)
{
    return gSystemData.fAdjustHallVolt;
}

/*=============================================================================================
** @Name      : void InPutHallCurrent(f32 data)
** @Input     : data:霍尔电流数据
** @Output    : void
** @Function  : 霍尔电流数据输入
** @The notes : 
**===========================================================================================*/
void InPutHallCurrent(f32 data)
{
    gSystemData.fHallCurrent = data;
}
/*=============================================================================================
** @Name      : f32 GetHallCurrent(void)
** @Input     : void
** @Output    : 输出霍尔电流数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetHallCurrent(void)
{
    return gSystemData.fHallCurrent;
}

/*=============================================================================================
** @Name      : void InPutfAdjustHallCurrent(f32 data)
** @Input     : 霍尔校准后电流数据
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
void InPutfAdjustHallCurrent(f32 data)
{
    gSystemData.fAdjustHallCurrent = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustHallCurrent(void)
** @Input     : void
** @Output    : 输出霍尔校准后电流数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetfAdjustHallCurrent(void)
{
    return gSystemData.fAdjustHallCurrent;
}

/*=============================================================================================
** @Name      : void InPutHall5Volt(f32 data)
** @Input     : data:霍尔5v供电电压数据
** @Output    : void
** @Function  : 霍尔5v供电电压数据输入
** @The notes : 
**===========================================================================================*/
void InPutHall5Volt(f32 data)
{
    gSystemData.fHall5Volt = data;
}

/*=============================================================================================
** @Name      : void InPutVcc5Volt(f32 data)
** @Input     : data:vcc供电电压数据
** @Output    : void
** @Function  : vcc供电电压数据输入
** @The notes : 
**===========================================================================================*/
void InPutVcc5Volt(f32 data)
{
    gSystemData.fVcc5Volt = data;
}
/*=============================================================================================
** @Name      : void InPutHvP5Volt(f32 data)
** @Input     : data:高压侧分流器正5v电压数据
** @Output    : void
** @Function  : 高压侧分流器正5v电压数据输入
** @The notes : 
**===========================================================================================*/
void InPutHvP5Volt(f32 data)
{
    gSystemData.fHvP5Volt = data;
}

/*=============================================================================================
** @Name      : void InPutHvN5Volt(f32 data)
** @Input     : data:高压侧分流器负5v电压数据
** @Output    : void
** @Function  : 高压侧分流器负5v电压数据输入
** @The notes : 
**===========================================================================================*/
void InPutHvN5Volt(f32 data)
{
    gSystemData.fHvN5Volt = data;
}

/*=============================================================================================
** @Name      : void InPutBatVolt(f32 data)
** @Input     : data:电压数据
** @Output    : void
** @Function  : 电池电压数据
** @The notes : 
**===========================================================================================*/
void InPutBatVolt(f32 data)
{
    gSystemData.fBatVolt = data;
}
/*=============================================================================================
** @Name      : f32 GetBatVolt(void)
** @Input     : void
** @Output    : 获取电池电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetBatVolt(void)
{
    return gSystemData.fBatVolt;
}
/*=============================================================================================
** @Name      : void InPutfAdjustBatVolt(f32 data)
** @Input     : 获取校准后电池电压数据
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
void InPutfAdjustBatVolt(f32 data)
{
    gSystemData.fAdjustBatVolt = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustBatVolt(void)
** @Input     : void
** @Output    : 校准后电池电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetfAdjustBatVolt(void)
{
    return gSystemData.fAdjustBatVolt;
}

/*=============================================================================================
** @Name      : void InPutLinkVolt(f32 data)
** @Input     : data:接触器外端电压数据
** @Output    : void
** @Function  : 接触器外端电压数据
** @The notes : 
**===========================================================================================*/
void InPutLinkVolt(f32 data)
{
    gSystemData.fLinkVolt = data;
}
/*=============================================================================================
** @Name      : f32 GetLinkVolt(void)
** @Input     : void
** @Output    : 获取Link电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetLinkVolt(void)
{
    return gSystemData.fLinkVolt;
}

/*=============================================================================================
** @Name      : void InPutfAdjustLinkVolt(f32 data)
** @Input     :
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
void InPutfAdjustLinkVolt(f32 data)
{
    gSystemData.fAdjustLinkVolt = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustLinkVolt(void)
** @Input     : void
** @Output    : 
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetfAdjustLinkVolt(void)
{
    return gSystemData.fAdjustLinkVolt;
}

/*=============================================================================================
** @Name      : void InPutLinkShuntVolt(f32 data)
** @Input     : data:负载端分流器电压数据
** @Output    : void
** @Function  : 负载端分流器电压数据
** @The notes : 
**===========================================================================================*/
void InPutLinkShuntVolt(f32 data)
{
    gSystemData.fLinkShuntVolt = data;
}
/*=============================================================================================
** @Name      : f32 GetLinkShuntVolt(void)
** @Input     : void
** @Output    : 输出负载端分流器电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetLinkShuntVolt(void)
{
    return gSystemData.fLinkShuntVolt;
}

/*=============================================================================================
** @Name      :void InPutfAdjustLinkShuntVolt(f32 data)
** @Input     : 校准后接触器外端分流器电流数据
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
void InPutfAdjustLinkShuntVolt(f32 data)
{
    gSystemData.fAdjustLinkShuntVolt = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustLinkShuntVolt(void)
** @Input     : void
** @Output    : 
** @Function  : 返回校准后接触器外端分流器电流数据
** @The notes : 
**===========================================================================================*/
f32 GetfAdjustLinkShuntVolt(void)
{
    return gSystemData.fAdjustLinkShuntVolt;
}

/*=============================================================================================
** @Name      : void InPutBShuntCurrent(f32 data)
** @Input     : data:接触器外端分流器电流数据
** @Output    : void
** @Function  : 接触器外端分流器电流数据
** @The notes : 
**===========================================================================================*/
void InPutLinkShuntCurrent(f32 data)
{
    gSystemData.fLinkShuntCurrent = data;
}

/*=============================================================================================
** @Name      : f32 GetLinkShuntCurrent(void)
** @Input     : void 
** @Output    : data:接触器外端分流器电流数据
** @Function  : 返回接触器外端分流器电流数据
** @The notes : 
**===========================================================================================*/
f32 GetLinkShuntCurrent(void)
{
    return gSystemData.fLinkShuntCurrent;
}

/*=============================================================================================
** @Name      : void InPutfAdjustLinkShuntCurrent(f32 data)
** @Input     : 校准后接触器外端分流器电流数据
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
void InPutfAdjustLinkShuntCurrent(f32 data)
{
    gSystemData.fAdjustLinkShuntCurrent = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustLinkShuntCurrent(void)
** @Input     : void
** @Output    : 
** @Function  : 返回校准后接触器外端分流器电流数据
** @The notes : 
**===========================================================================================*/
f32 GetfAdjustLinkShuntCurrent(void)
{
    return gSystemData.fAdjustLinkShuntCurrent;
}


/*=============================================================================================
** @Name      : void InPutNpeVolt(f32 data)
** @Input     : data:npe采样电压数据
** @Output    : void
** @Function  : npe采样电压数据输入
** @The notes : 
**===========================================================================================*/
void InPutNpeVolt(f32 data)
{
    gSystemData.fNpeVolt = data;
}
/*=============================================================================================
** @Name      : void InPutPeGndVolt(f32 data)
** @Input     : data:pe对负极电压数据
** @Output    : void
** @Function  : pe对负极电压数据
** @The notes : 
**===========================================================================================*/
void InPutPeGndVolt(f32 data)
{
    gSystemData.fPeGndVolt = data;
}
/*=============================================================================================
** @Name      : f32 GetPeGndVolt(void)
** @Input     : void
** @Output    : 输出pe对负极电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetPeGndVolt(void)
{
    return gSystemData.fPeGndVolt;
}

/*=============================================================================================
** @Name      : void InPutfAdjustPeGndVolt(f32 data)
** @Input     : 输出校准后pe对负极电压数据
** @Output    : void
** @Function  : 
** @The notes : 
**===========================================================================================*/
void InPutfAdjustPeGndVolt(f32 data)
{
    gSystemData.fAdjustPeGndVolt = data;
}

/*=============================================================================================
** @Name      : f32 GetfAdjustPeGndVolt(void)
** @Input     : void
** @Output    : 校准后pe对负极电压数据
** @Function  : 
** @The notes : 
**===========================================================================================*/
f32 GetfAdjustPeGndVolt(void)
{
    return gSystemData.fAdjustPeGndVolt;
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
