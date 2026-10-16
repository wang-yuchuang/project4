#ifndef __ABU_h__
#define __ABU_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 *  Includes
 */
#include "main.h"

/*******************************************************************************
 *  Exported Constants
 */
enum
{
	eType_Total = 0,
	eType_AB,
	eType_BC,
	eType_CA,
};

enum
{
	eType_On = 0xA0,
	eType_Off = 0xA1,
};


#define CFG_SENDABUINFO_TIMEOUT			10

/*******************************************************************************
 *  Exported Typedef
 */

/*******************************************************************************
 *  Exported Macros
 */
#define ABU_Addr			0xFA
#define CFG_ABU_RESPONSE_TIME	2

/*******************************************************************************
 *  Exported Variables
 */
typedef union TStatus0
{
	UINT8 Data;
	struct 
	{
		UINT8 Reserve 				  : 1;
		UINT8 IslandProtection  : 1;//孤岛保护
		UINT8 PhaselockingError : 1;//锁相错误
		UINT8 Reserve1 				  : 1;
		UINT8 OverInputVolt 		: 1;//输入电压高于Vbus电压告警 
		UINT8 Reserve2 				  : 2;
		UINT8 IsInversion 			: 1;//1:逆变模式 0:整流模式
	}bit;
}Status0_info_t;

typedef union TStatus1
{
	UINT8 Data;
	struct 
	{
		UINT8 CommFailedError 	: 1;//模块通信失败告警
		UINT8 Reserve						: 2;//
		UINT8 OverTemperature		: 1;//过温告警
		UINT8 FanError 				  : 1;//风扇故障告警
		UINT8 InProtection			: 1;//模块保护告警
		UINT8 IsFault						: 1;//模块故障告警
		UINT8 Reserve1					: 1;//
	}bit;
}Status1_info_t;

typedef union TStatus2
{
	UINT8 Data;
	struct 
	{
		UINT8 IsHalt						: 1;//模块关机状态
		UINT8 Reserve						: 4;//
		UINT8 UnevenFlow				: 1;//模块严重不均流
		UINT8 DuplicateID 			: 1;//模块ID重复
		UINT8 Reserve1					: 1;//
	}bit;
}Status2_info_t;

//by lxr01202   relaymode
typedef union TStatus3
{
	UINT8 Data;
	struct 
	{
		UINT8 Automaticswitching						: 2;//自动切换
	}bit;
}Status3_info_t;

typedef union TStatus4
{
	UINT8 Data;
	struct 
	{
		UINT8 inpowerlmt						: 1;//限功率状态
		UINT8 IDDUP						: 1;//ID冲突
		UINT8 OutUnBalance				: 1;//DC侧系统不均流
		UINT8 InLessPhase					: 1;//AC缺相
		UINT8 InUnBalance					: 1;//AC电压不平衡
		UINT8 UVin                            : 1;//AC欠压
		UINT8 OVin                            : 1;//AC过压
		UINT8 InOff                           : 1;//AC关机1   开机0
	}bit;
}Status4_info_t;

typedef union TStatus5
{
	UINT8 Data;
	struct 
	{
		UINT8 OutOff                           : 1;//DC关机1 DC开机0
		UINT8 Fault                            : 1;//故障状态
		UINT8 Warn                             : 1;//告警状态
		UINT8 kong                        : 1;//空
		UINT8 OT								:1;//过温
		UINT8 OVo	                           :1;//DC过压
		UINT8 Walkin                           :1;//Walkin使能
		UINT8 MCCFail                          :1;//与监控通信故障

	}bit;
}Status5_info_t;

typedef union TStatus6
{
	UINT8 Data;
	struct 
	{
		UINT8 Short                             : 1;//DC短路故障
		UINT8 UVdc                              : 1;//DC欠压
		UINT8 SciFail                           : 1;//SCI通信故障
		UINT8 BusFail                           : 1;//母线通信故障
		UINT8 Dormant                           : 1;//休眠状态
		UINT8 DisChargeErr                      : 1;//放电故障

	}bit;
}Status6_info_t;


typedef union TStatus7
{
	UINT8 Data;
	struct 
	{
		UINT8 OffGrid                          : 1;//离网状态
		UINT8 OP                               : 1;//过载
		UINT8 FRT                              : 1;//穿越故障
		UINT8 OffGridModeOnGrid                : 1;//离网模式下接电网
		UINT8 Estop                            : 1;//急停状态

	}bit;
}Status7_info_t;

typedef union TStatus8
{
	UINT8 Data;
	struct 
	{
		UINT8 OVBUS                             : 1;//母线过压
		UINT8 ACTZ                              : 1;//AC过流保护
		UINT8 Olin                              : 1;//AC输入过流
		UINT8 DCTZ                              : 1;//DC过流保护
		UINT8 PhaseErr                          : 1;//AC错相
		UINT8 InLessPhase                       : 1;//AC缺相
		UINT8 FGridFault                        : 1;//频率异常
		UINT8 VGridFault                        : 1;//电网电压故障

	}bit;
}Status8_info_t;

typedef union TStatus9
{
	UINT8 Data;
	struct 
	{
		UINT8 InvMode 				: 1;//逆变模式
		UINT8 UVdc                  : 1;//DC欠压
		UINT8 VinOff                : 1;//AC输入掉电
		UINT8 BusErr                : 1;//母线电压异常
		UINT8 AcFail                : 1;//AC过流保护锁死
		UINT8 PllFault              : 1;//锁相故障
		UINT8 IslandProtect         : 1;//孤岛保护
		UINT8 GridFault             : 1;//电网侧故障（过欠压、频率异常、错相缺相）

	}bit;
}Status9_info_t;

typedef union TContravariantStat0
{
	UINT8 Data;
	struct 
	{
		UINT8 IsOnOff							: 1;//0：OFF  1:ON
		UINT8 IsGirdOnOff					: 1;//0：并网状态   1：离网状态
		UINT8 BusGirdOnOff				: 1;//0：总线并网  1：总线离网
		UINT8 IsAlarm							: 1;//0：无故障；1故障
		UINT8 Reserve							: 2;
		UINT8 AutoMode						: 1;//0：自动并离网；1：手动并离网
		UINT8 Reserve1						: 1;
	}bit;
}ContravariantStat0_info_t;

typedef union TContravariantStat1
{
	UINT8 Data;
	struct 
	{
		UINT8 Reserve								: 8;
	}bit;
}ContravariantStat1_info_t;

typedef union TContravariantStat2
{
	UINT8 Data;
	struct 
	{
		UINT8 Reserve								: 8;
	}bit;
}ContravariantStat2_info_t;

typedef union TContravariantStat3
{
	UINT8 Data;
	struct 
	{
		UINT8 Reserve								: 8;
	}bit;
}ContravariantStat3_info_t;

typedef union TContravariantAlm0
{
	UINT8 Data;
	struct 
	{
		UINT8 ACOverVolt						: 1;//电网过压
		UINT8 ACUnderVolt						: 1;//电网欠压
		UINT8 ACOverFrequency				: 1;//电网过频
		UINT8 ACUnderFrequency			: 1;//电网欠频
		UINT8 ACFreqLoss						: 1;//频率丢失
		UINT8 ACPhaseLoss						: 1;//缺相
		UINT8 ACSequenceError				: 1;//相序反
		UINT8 ACLossN								: 1;//N线丢失
	}bit;
}ContravariantAlm0_info_t;

typedef union TContravariantAlm1
{
	UINT8 Data;
	struct 
	{
		UINT8 GridUnbalance					: 1;//电网不平衡
		UINT8 VGridLineFault				: 1;
		UINT8 PllFail								: 1;//锁相失败
		UINT8 GridVoltCrossZeroLose	: 1;//电压过零丢失
		UINT8 GridDQFastFault				: 1;//(DQ故障)
		UINT8 GridFreErr						: 1;//(频率异常)
		UINT8 GridFreChg						: 1;//(频率变化块故障)
		UINT8 OutPutOverLoad				: 1;//(输出过载)
	}bit;
}ContravariantAlm1_info_t;

typedef union TContravariantAlm2
{
	UINT8 Data;
	struct 
	{
		UINT8 GridOCP								: 1;//(电网过流)
		UINT8 IacUnbalance					: 1;//(电流不平衡)
		UINT8 Reserve								: 4;
		UINT8 RefluxFault						: 1;//(防逆流故障)
		UINT8 OutputShort						: 1;//(输出短路)
	}bit;
}ContravariantAlm2_info_t;

typedef union TContravariantAlm3
{
	UINT8 Data;
	struct 
	{
		UINT8 OutPutVoltCrossZeroLose	: 1;//(输出电压过零丢失)
		UINT8 Reserve									: 7;
	}bit;
}ContravariantAlm3_info_t;

typedef union TContravariantAlm4
{
	UINT8 Data;
	struct 
	{
		UINT8 RlyErr									: 1;//(继电器故障)
		UINT8 Reserve									: 1;
		UINT8 OnGridStartFault				: 1;//(并网启动故障)
		UINT8 Reserve1								: 5;
	}bit;
}ContravariantAlm4_info_t;

typedef union TContravariantAlm5
{
	UINT8 Data;
	struct 
	{
		UINT8 Reserve									: 8;
	}bit;
}ContravariantAlm5_info_t;

typedef union TContravariantAlm6
{
	UINT8 Data;
	struct 
	{
		UINT8 CanErr									: 1;//(Can通信故障)
		UINT8 SpiErr									: 1;//(SPI通信故障)
		UINT8 Reserve									: 1;
		UINT8 E2PROMErr								: 1;//(E2故障)
		UINT8 OTSink									: 1;//(散热器过温)
		UINT8 OTAmb										: 1;//(环境温度过高)
		UINT8 OTCabinet								: 1;//(柜内温度过高)
		UINT8 FanERR									: 1;//(风扇故障)
	}bit;
}ContravariantAlm6_info_t;

typedef union TContravariantAlm7
{
	UINT8 Data;
	struct 
	{
		UINT8 GridFault								: 1;//(电网故障)
		UINT8 SysFault								: 1;//(系统故障)
		UINT8 Reserve									: 6;
	}bit;
}ContravariantAlm7_info_t;

typedef union T_0x1115Byte0
{
	UINT8 Data;
	struct 
	{
		UINT8 U2CanErr								: 1;//(U2Can通信故障)
		UINT8 U2SpiErr								: 1;//(U2Spi通信故障)
		UINT8 Reserve									: 1;
		UINT8 U2E2PROMErr							: 1;//(U2E2故障)
		UINT8 Reserve1								: 4;
	}bit;
}_0x1115Byte0_info_t;

typedef union T0x1115Byte1
{
	UINT8 Data;
	struct 
	{
		UINT8 EPO									: 1;//急停
		UINT8 OTempTrsfm1					: 1;//(变压器过温)
		UINT8 PowerErr						: 1;
		UINT8 Reserve							: 2;
		UINT8 FanErr							: 1;//(风扇故障)
		UINT8 IoReadyErr					: 1;//(排线接触不良)
		UINT8 U2SysFault					: 1;//(U2系统故障)
	}bit;
}_0x1115Byte1_info_t;

typedef union T0x1115Byte2
{
	UINT8 Data;
	struct 
	{
		UINT8 Fan1Err							: 1;//(风扇1故障)
		UINT8 Fan2Err							: 1;//(风扇2故障)
		UINT8 Fan3Err							: 1;//(风扇3故障)
		UINT8 Fan4Err							: 1;//(风扇4故障)
		UINT8 NLineContactorErr					: 1;//(N继电器故障)
		UINT8 Reserve							: 3;
	}bit;
}_0x1115Byte2_info_t;

typedef union T0x1115Byte3
{
	UINT8 Data;
	struct 
	{
		UINT8 Reserve							: 8;
	}bit;
}_0x1115Byte3_info_t;


typedef struct TABU_0x10_info
{
	UINT16 PowerModuleNum;//模块数
	
  INT32 SysDCVolt;//系统直流电压(mV)
	INT32 SysDCCurr;//系统直流总电流(mA)
}
ABU_0x10_info_t;

typedef struct TABU_0x11_info
{
	Status0_info_t Status0;
	Status1_info_t Status1;
	Status2_info_t Status2;
	Status3_info_t Status3;
	Status4_info_t Status4;
	Status5_info_t Status5;
	Status6_info_t Status6;
	Status7_info_t Status7;
	Status8_info_t Status8;
	Status9_info_t Status9;

	
	UINT32 PowerModule_DCVolt;//模块直流侧电压(mV)
	INT32	 PowerModule_DCCurr;//模块直流侧电流(mA)
	INT32	 PowerModule_AmbientTemp;//模块环境温度(mC)
	UINT32 PowerModule_DCExtVolt;////模块直流侧外部电压(mV)
	INT32  PowerModule_DCLimitCurr;////模块直流限流值(mA)
	UINT32 PowerModule_DC_InverterDisCutOffVolt;//直流侧逆变放电截止电压(mV)	
}
ABU_0x11_info_t;

typedef struct TABU_0x21_info
{
	UINT32 AC_VoltAB;//交流A相电压(mV)
	UINT32 AC_VoltBC;//交流B相电压(mV)
	UINT32 AC_VoltCA;//交流C相电压(mV)
	INT32	 AC_CurrAB;//交流A相电流(mA)
	INT32  AC_CurrBC;//交流B相电流(mA)
	INT32  AC_CurrCA;//交流C相电流(mA)

	UINT32 AC_Freq;//交流频率(mHz)

	INT32	 TotalActivePower;//总有功功率(mW)
	INT32	 ActivePowerAB;//A相有功功率(mW)
	INT32	 ActivePowerBC;//B相有功功率(mW)
	INT32	 ActivePowerCA;//C相有功功率(mW)

	INT32	 TotalReactivePower;//总无功功率(mW)
	INT32	 ReactivePowerAB;//A相无功功率(mW)
	INT32	 ReactivePowerBC;//B相无功功率(mW)
	INT32	 ReactivePowerCA;//C相无功功率(mW)

	UINT32 TotalPowerFactor;//总功率因数
	UINT32 PowerFactorAB;//A相功率因数
	UINT32 PowerFactorBC;//B相功率因数
	UINT32 PowerFactorCA;//C相功率因数
}
ABU_0x21_info_t;

typedef struct TABU_0x31_info
{
	UINT32 OutputVoltAB;//输出A相电压(mV)
	UINT32 OutputVoltBC;//输出B相电压(mV)
	UINT32 OutputVoltCA;//输出C相电压(mV)
}
ABU_0x31_info_t;

typedef union TControlBit
{
	UINT32 Data;
	struct 
	{
		UINT32 OnOff										: 1;
		UINT32 OnOffGridMode						: 1;
		UINT32 OnOffGridSwitchModeAuto	: 1;
		UINT32 Reserve									: 29;
	}bit;
}ControlBit_info_t;


typedef struct TABU_ControlInfo
{
	UINT8 OnOffGridSwitchModeAuto;
	UINT8 OnOffGridMode;
	UINT8 OnOff;

	ControlBit_info_t ControlBitmap;
}
ABU_ControlInfo_t;

typedef struct TABU
{
	UINT8 CommFailed;
	UINT8 CommFailedLast;
	UINT8 IsGetABUVersion;
	UINT8 IsGetBarCode;
	UINT8 UnrecvTimes;
	
	Status0_info_t	Status0;
	Status1_info_t	Status1;
	Status2_info_t	Status2;
	Status3_info_t Status3;
	Status4_info_t Status4;
	Status5_info_t Status5;
	Status6_info_t Status6;
	Status7_info_t Status7;
	Status8_info_t Status8;
	Status9_info_t Status9;
	
	ContravariantStat0_info_t ContravariantStat0;
	ContravariantStat1_info_t ContravariantStat1;
	ContravariantStat2_info_t ContravariantStat2;
	ContravariantStat3_info_t ContravariantStat3;

	ContravariantAlm0_info_t ContravariantAlm0;
	ContravariantAlm1_info_t ContravariantAlm1;
	ContravariantAlm2_info_t ContravariantAlm2;
	ContravariantAlm3_info_t ContravariantAlm3;
	ContravariantAlm4_info_t ContravariantAlm4;
	ContravariantAlm5_info_t ContravariantAlm5;
	ContravariantAlm6_info_t ContravariantAlm6;
	ContravariantAlm7_info_t ContravariantAlm7;

	_0x1115Byte0_info_t	_0x1115Byte0_info;
	_0x1115Byte1_info_t	_0x1115Byte1_info;
	_0x1115Byte2_info_t _0x1115Byte2_info;
	_0x1115Byte3_info_t _0x1115Byte3_info;

	UINT32 DC_Version;
	UINT32 PFC_Version;
	
  	ABU_0x10_info_t ABU_0x10_info;
	ABU_0x11_info_t ABU_0x11_info;
	ABU_0x21_info_t ABU_0x21_info;
	ABU_0x31_info_t ABU_0x31_info;

	ABU_ControlInfo_t ControlInfo;

	unsigned long long BarCode;
	UINT8 BarCodeString[17];

	UINT8 ConfigFailed;
	UINT8 u8ConfigDelay;
}
ABU_t;


extern ABU_t *ABU;
/*******************************************************************************
 *  Exported Functions
 */
void ABU_Comm(void);

#ifdef __cplusplus
}
#endif

#endif

///:~
