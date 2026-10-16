#include "main.h"
//#include "TelaidianProto.h"
#include "Uart6_new.h"
#include "EtherNet.h"
#include "can_ring.h"
#include "EnergyOutputMng.h"
NOINIT UINT8 u8gTempDebugBuf_A[DEBUG_BUF_BYTES_MAX_NUM];
NOINIT UINT8 u8gTempDebugBuf_B[DEBUG_BUF_BYTES_MAX_NUM];
NOINIT UINT8 u8gTempDebugBuf_System[DEBUG_BUF_BYTES_MAX_NUM];

coTime coSystemTicksClock = 0u;

//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鈧棃鏁撻弬銈嗗闁跨喖銈虹拋瑙勫闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔哄嵆閹惧懏瀚归柨鐕傛嫹
//NOINIT unsigned char Stop_SECC_STATUS_CODE_backup[2];
//NOINIT unsigned char Stop_EV_ERROR_CODE_backup[2];
//NOINIT unsigned char Stop_SECC_ERROR_CODE_backup[2];
//NOINIT unsigned char Stop_CP_OSCILLATOR_backup[2];
//NOINIT unsigned char Stop_evccAttn_backup[2];
//NOINIT unsigned short Stop_CP_VOLTAGE_backup[2];
//NOINIT float fStop_evseMaximumCurrentLimit_backup[2];
//NOINIT float fStop_evseMaximumVoltLimit_backup[2];
//NOINIT float fStop_evseMaximumPowerLimit_backup[2];
//NOINIT float fStop_evseMaximumRequireCurrent_backup[2];
//NOINIT float fStop_evseMaximumRequireVolt_backup[2];
//NOINIT float fStop_evseMaximumCtrlCurrent_backup[2];
//NOINIT float fStop_evseMaximumCtrlVolt_backup[2];
//NOINIT float fStop_evseMaximumOutCurrent_backup[2];
//NOINIT float fStop_evseMaximumOutVolt_backup[2];
//NOINIT float fStop_EV_MAXIMUM_CURRENT_LIMIT_backup[2];
//NOINIT float fStop_EV_MAXIMUM_VOLTAGE_LIMIT_backup[2];



//V2 闁跨喕濮ら張锟� 闁跨喕鍓兼导娆愬闁跨喐鏋婚幏鐑芥晸閺傘倖瀚笹ET SET 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崗銊︾梾闁跨喐鏋婚幏鐤洣闁跨喐鏋婚幏宄板箵閸欐牠鏁撻弬銈嗗闁跨喐鏋婚幏铚傜闁跨喐宓庨幉瀣闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风ěET SET闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔诲Ν鐠囇勫闁跨喐鏋婚幏宄扮杽闁跨喕顢滈敐蹇斿
const __SIG_E2P_ADDR stArgsSigAddr[]=
{
	{E_TYPE_SYS,		ID_fCC1_1Coef_A,					ADDR_CC1_1Coef_A,						IS_FLOAT},
	{E_TYPE_SYS,		ID_fCC1_1Coef_B,					ADDR_CC1_1Coef_B,						IS_FLOAT},
	{E_TYPE_SYS,		ID_fCC1_2Coef_A,					ADDR_CC1_2Coef_A,						IS_FLOAT},
	{E_TYPE_SYS,		ID_fCC1_2Coef_B,					ADDR_CC1_2Coef_B,						IS_FLOAT},
	{E_TYPE_SYS,		ID_fSysSetOverVolt,					ADDR_fSysSetOverVolt,					IS_FLOAT},
	{E_TYPE_SYS,		ID_fRectLoadStartTime,				ADDR_fRectLoadStartTime,				IS_FLOAT},
	{E_TYPE_SYS,		ID_fMaxOutCurr,						ADDR_MaxOutCurr,						IS_FLOAT},
	{E_TYPE_SYS,		ID_fMaxOutVolt,						ADDR_MaxOutVolt,						IS_FLOAT},
	{E_TYPE_SYS,		ID_fSetLimitMaxOutCurr_1,			ADDR_fSetLimitMaxOutCurr_1,				IS_FLOAT},
	{E_TYPE_SYS,		ID_fSetLimitMaxOutCurr_2,			ADDR_fSetLimitMaxOutCurr_2,				IS_FLOAT},


	{E_TYPE_SYS,		ID_fBattVolt_1CoefA,				ADDR_fBattVolt_1CoefA,					IS_FLOAT},
	{E_TYPE_SYS,		ID_fBattVolt_1CoefB,				ADDR_fBattVolt_1CoefB,					IS_FLOAT},
	{E_TYPE_SYS,		ID_fBattVolt_2CoefA,				ADDR_fBattVolt_2CoefA,					IS_FLOAT},
	{E_TYPE_SYS,		ID_fBattVolt_2CoefB,				ADDR_fBattVolt_2CoefB,					IS_FLOAT},

	{E_TYPE_SYS,		ID_TP1000_1_CoefA,				ADDR_fTP100_1CoefA,					IS_FLOAT},
	{E_TYPE_SYS,		ID_TP1000_1_CoefB,				ADDR_fTP100_1CoefB,					IS_FLOAT},
	{E_TYPE_SYS,		ID_TP1000_2_CoefA,				ADDR_fTP100_2CoefA,					IS_FLOAT},
	{E_TYPE_SYS,		ID_TP1000_2_CoefB,				ADDR_fTP100_2CoefB,					IS_FLOAT},

	//{E_TYPE_SYS,		ID_u32_B_CTS_DB_T1,					ADDR_u32_B_CTS_DB_T1,					IS_ULONG},
	//{E_TYPE_SYS,		ID_u32_B_DB_DB_T2,					ADDR_u32_B_DB_DB_T2,					IS_ULONG},
	//{E_TYPE_SYS,		ID_u32_A_CTS_DB_T1,					ADDR_u32_A_CTS_DB_T1,					IS_ULONG},
	//{E_TYPE_SYS,		ID_u32_A_DB_DB_T2,					ADDR_u32_A_DB_DB_T2,					IS_ULONG},
	//{E_TYPE_SYS,		ID_u16SetBCLTimeOutVal_1,			ADDR_u16SetBCLTimeOutVal_1,				IS_ULONG},
	//{E_TYPE_SYS,		ID_u16SetBCLTimeOutVal_2,			ADDR_u16SetBCLTimeOutVal_2,				IS_ULONG},
	//{E_TYPE_SYS,		ID_u16SetBRMTimeOutVal_1,			ADDR_u16SetBRMTimeOutVal_1,				IS_ULONG},
	//{E_TYPE_SYS,		ID_u16SetBRMTimeOutVal_2,			ADDR_u16SetBRMTimeOutVal_2,				IS_ULONG},
	//{E_TYPE_SYS,		ID_u16SetBCSTimeOutVal_1,			ADDR_u16SetBCSTimeOutVal_1,				IS_ULONG},
	//{E_TYPE_SYS,		ID_u16SetBCSTimeOutVal_2,			ADDR_u16SetBCSTimeOutVal_2,				IS_ULONG},

	{E_TYPE_SYS,		ID_u8PolarityAlmMask_1,				ADDR_u8PolarityAlmMask_1,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8PolarityAlmMask_2,				ADDR_u8PolarityAlmMask_2,				IS_CHAR},

	{E_TYPE_SYS,		ID_u8SetCalcMoneyType,				ADDR_u8SetCalcMoneyType,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8CC1CheckEnable_1,				ADDR_u8CC1CheckEnable_1,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8CC1CheckEnable_2,				ADDR_u8CC1CheckEnable_2,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8SelfTestEnable_1,				ADDR_u8SelfTestEnable_1,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8SelfTestEnable_2,				ADDR_u8SelfTestEnable_2,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8MCCBOnCheckEnable_1,			ADDR_u8MCCBOnCheckEnable_1,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8MCCBOnCheckEnable_2,			ADDR_u8MCCBOnCheckEnable_2,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8OutOverVoltEnable_1,			ADDR_u8OutOverVoltEnable_1,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8OutOverVoltEnable_2,			ADDR_u8OutOverVoltEnable_2,				IS_CHAR},
	{E_TYPE_SYS,		ID_u8DoorAlmMask_1,					ADDR_u8DoorAlmMask_1,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8DoorAlmMask_2,					ADDR_u8DoorAlmMask_2,					IS_CHAR},
	//{E_TYPE_SYS,		ID_u8HasElec_ALock,					ADDR_u8HasElec_Lock_1,					IS_CHAR},
	//{E_TYPE_SYS,		ID_u8HasElec_BLock,					ADDR_u8HasElec_Lock_2,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8PWMPolarityCover,				ADDR_u8PWMPolarityCover,				IS_CHAR},
	/*{E_TYPE_SYS,		ID_u8_A_LOCKType,					ADDR_u8_A_LOCKType,						IS_CHAR},
	{E_TYPE_SYS,		ID_u8_B_LOCKType,					ADDR_u8_B_LOCKType,						IS_CHAR},
	{E_TYPE_SYS,		ID_u8_C_LOCKType,					ADDR_u8_C_LOCKType,						IS_CHAR},
	{E_TYPE_SYS,		ID_u8LOCK_A_DIType,					ADDR_u8LOCK_A_DIType,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8LOCK_B_DIType,					ADDR_u8LOCK_B_DIType,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8LOCK_C_DIType,					ADDR_u8LOCK_C_DIType,					IS_CHAR},*/
	{E_TYPE_SYS,		ID_u8EPOAlmVltLEVEL,				ADDR_u8EPOAlmVltLEVEL,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8BrushFactorType,				ADDR_u8BrushFactorType,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8System_Start_Temp,					ADDR_u8EV_StartTemp,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8System_Ultra_Temp,					ADDR_u8EV_StopTemp,						IS_CHAR},
	{E_TYPE_SYS,		ID_u8NET_OffLine_Charge_Enable,		ADDR_u8NET_OffLine_Charge_Enable,		IS_CHAR},
	{E_TYPE_SYS,		ID_fMaxOffLineTime,					ADDR_fMaxOffLineTime,					IS_ULONG},
	{E_TYPE_SYS,		ID_u32PowerOnTimes,					ADDR_u32PowerOnTimes,					IS_ULONG},
	//{E_TYPE_SYS,		ID_u8_A_ForceVer2011,				ADDR_u8_A_ForceVer2011,					IS_CHAR},
	//{E_TYPE_SYS,		ID_u8_B_ForceVer2011,				ADDR_u8_B_ForceVer2011,					IS_CHAR},

	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撴潪鍨啞鐠€锟�
	{E_TYPE_SYS,		ID_NET_DHCP,						ADDR_DHCP_VALUE,						IS_CHAR},
	{E_TYPE_SYS,		ID_u32NET_PORT,						ADDR_u32_NET_PORT,						IS_ULONG},
	{E_TYPE_SYS,		ID_u8IP_1,							ADDR_u8IP_1,							IS_CHAR},
	{E_TYPE_SYS,		ID_u8IP_2,							ADDR_u8IP_2,							IS_CHAR},		
	{E_TYPE_SYS,		ID_u8IP_3,							ADDR_u8IP_3,							IS_CHAR},
	{E_TYPE_SYS,		ID_u8IP_4,							ADDR_u8IP_4,							IS_CHAR},	
	{E_TYPE_SYS,		ID_u8IP_5,							ADDR_u8IP_5,							IS_CHAR},
	{E_TYPE_SYS,		ID_u8IP_6,							ADDR_u8IP_6,							IS_CHAR},	
	{E_TYPE_SYS,		ID_u8Mac_1,							ADDR_u8Mac_1,							IS_CHAR},
	{E_TYPE_SYS,		ID_u8Mac_2,							ADDR_u8Mac_2,							IS_CHAR},		
	{E_TYPE_SYS,		ID_u8Mac_3,							ADDR_u8Mac_3,							IS_CHAR},
	{E_TYPE_SYS,		ID_u8Mac_4,							ADDR_u8Mac_4,							IS_CHAR},	
	{E_TYPE_SYS,		ID_u8Mac_5,							ADDR_u8Mac_5,							IS_CHAR},
	{E_TYPE_SYS,		ID_u8Mac_6,							ADDR_u8Mac_6,							IS_CHAR},	
	{E_TYPE_SYS,		ID_u8Gateway_1,						ADDR_u8Gateway_1,						IS_CHAR},
	{E_TYPE_SYS,		ID_u8Gateway_2,						ADDR_u8Gateway_2,						IS_CHAR},		
	{E_TYPE_SYS,		ID_u8Gateway_3,						ADDR_u8Gateway_3,						IS_CHAR},
	{E_TYPE_SYS,		ID_u8Gateway_4,						ADDR_u8Gateway_4,						IS_CHAR},	
	{E_TYPE_SYS,		ID_u8Gateway_5,						ADDR_u8Gateway_5,						IS_CHAR},
	{E_TYPE_SYS,		ID_u8Gateway_6,						ADDR_u8Gateway_6,						IS_CHAR},	
	{E_TYPE_SYS,		ID_u8Subnetmask_1,					ADDR_u8Subnetmask_1,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8Subnetmask_2,					ADDR_u8Subnetmask_2,					IS_CHAR},		
	{E_TYPE_SYS,		ID_u8Subnetmask_3,					ADDR_u8Subnetmask_3,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8Subnetmask_4,					ADDR_u8Subnetmask_4,					IS_CHAR},	
	{E_TYPE_SYS,		ID_u8Subnetmask_5,					ADDR_u8Subnetmask_5,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8Subnetmask_6,					ADDR_u8Subnetmask_6,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8PowerStarIP_1,					ADDR_u8PowerStarIP_1,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8PowerStarIP_2,					ADDR_u8PowerStarIP_2,					IS_CHAR},		
	{E_TYPE_SYS,		ID_u8PowerStarIP_3,					ADDR_u8PowerStarIP_3,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8PowerStarIP_4,					ADDR_u8PowerStarIP_4,					IS_CHAR},	
	{E_TYPE_SYS,		ID_u8PowerStarIP_5,					ADDR_u8PowerStarIP_5,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8PowerStarIP_6,					ADDR_u8PowerStarIP_6,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8SPDAlmMask,					ADDR_u8SPDAlmMask,						IS_CHAR},


	{E_TYPE_SYS,		ID_SETDATA_RemoteServerIsDomainName_NUM,			ADDR_SETDATA_RemoteServerIsDomainName,						IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_DNSServerIP_NUM_1,					ADDR_DNSServerIP_1,					IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_DNSServerIP_NUM_2,					ADDR_DNSServerIP_2,					IS_CHAR},		
	{E_TYPE_SYS,		ID_SETDATA_DNSServerIP_NUM_3,					ADDR_DNSServerIP_3,					IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_DNSServerIP_NUM_4,					ADDR_DNSServerIP_4,					IS_CHAR},	
	{E_TYPE_SYS,		ID_SETDATA_DNSServerIP_NUM_5,					ADDR_DNSServerIP_5,					IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_DNSServerIP_NUM_6,					ADDR_DNSServerIP_6,					IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_RemoteServerDomainName_NUM_1,		ADDR_RemoteServerDomainName_1,						IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+1),	(ADDR_RemoteServerDomainName_1+1),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+2),	(ADDR_RemoteServerDomainName_1+2),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+3),	(ADDR_RemoteServerDomainName_1+3),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+4),	(ADDR_RemoteServerDomainName_1+4),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+5),	(ADDR_RemoteServerDomainName_1+5),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+6),	(ADDR_RemoteServerDomainName_1+6),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+7),	(ADDR_RemoteServerDomainName_1+7),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+8),	(ADDR_RemoteServerDomainName_1+8),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+9),	(ADDR_RemoteServerDomainName_1+9),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+10),	(ADDR_RemoteServerDomainName_1+10),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+11),	(ADDR_RemoteServerDomainName_1+11),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+12),	(ADDR_RemoteServerDomainName_1+12),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+13),	(ADDR_RemoteServerDomainName_1+13),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+14),	(ADDR_RemoteServerDomainName_1+14),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+15),	(ADDR_RemoteServerDomainName_1+15),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+16),	(ADDR_RemoteServerDomainName_1+16),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+17),	(ADDR_RemoteServerDomainName_1+17),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+18),	(ADDR_RemoteServerDomainName_1+18),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+19),	(ADDR_RemoteServerDomainName_1+19),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+20),	(ADDR_RemoteServerDomainName_1+20),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+21),	(ADDR_RemoteServerDomainName_1+21),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+22),	(ADDR_RemoteServerDomainName_1+22),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+23),	(ADDR_RemoteServerDomainName_1+23),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+24),	(ADDR_RemoteServerDomainName_1+24),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+25),	(ADDR_RemoteServerDomainName_1+25),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+26),	(ADDR_RemoteServerDomainName_1+26),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+27),	(ADDR_RemoteServerDomainName_1+27),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+28),	(ADDR_RemoteServerDomainName_1+28),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+29),	(ADDR_RemoteServerDomainName_1+29),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+30),	(ADDR_RemoteServerDomainName_1+30),					IS_CHAR},
	{E_TYPE_SYS,		(ID_SETDATA_RemoteServerDomainName_NUM_1+31),	(ADDR_RemoteServerDomainName_1+31),					IS_CHAR},

	{E_TYPE_SYS,		ID_u8LowVoltAlmVltLEVEL,				ADDR_u8LowVoltAlmVltLEVEL,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8NLINEAlmVltLEVEL,					ADDR_u8NLineAlmVltLEVEL,					IS_CHAR},
	
	//{E_TYPE_SYS,		ID_A_ChargeCMD,							INVALID_ITEM,						IS_CHAR},
	//{E_TYPE_SYS,		ID_B_ChargeCMD,							INVALID_ITEM,						IS_CHAR},

	{E_TYPE_WH1,		ID_fACWhCoef,						ADDR1_fACWhCoef,			IS_FLOAT},
	{E_TYPE_WH2,		ID_fACWhCoef,						ADDR2_fACWhCoef,			IS_FLOAT},
	{E_TYPE_WH1,		ID_u8WH_TYPE,						ADDR1_u8WH_TYPE,			IS_CHAR},
	{E_TYPE_WH2,		ID_u8WH_TYPE,						ADDR2_u8WH_TYPE,			IS_CHAR},
	{E_TYPE_WH1,		ID_u8WH_ALM_Mask,					ADDR1_u8WH_ALM_Mask,		IS_CHAR},
	{E_TYPE_WH2,		ID_u8WH_ALM_Mask,					ADDR2_u8WH_ALM_Mask,		IS_CHAR},

	{E_TYPE_IMIU1,		ID_u8JYMAlmStatMask,				ADDR1_u8JYMAlmStatMask,		IS_CHAR},
	{E_TYPE_IMIU2,		ID_u8JYMAlmStatMask,				ADDR2_u8JYMAlmStatMask,		IS_CHAR},
	{E_TYPE_IMIU1,		ID_u8SetJYMCtrlMode,				ADDR1_u8SetJYMCtrlMode,		IS_CHAR},
	{E_TYPE_IMIU2,		ID_u8SetJYMCtrlMode,				ADDR2_u8SetJYMCtrlMode,		IS_CHAR},
	{E_TYPE_IMIU1,		ID_u8InsltCheckByVolt,				ADDR1_u8InsltCheckByVolt,	IS_CHAR},
	{E_TYPE_IMIU2,		ID_u8InsltCheckByVolt,				ADDR2_u8InsltCheckByVolt,	IS_CHAR},
	{E_TYPE_IMIU1,		ID_fSetJYMAlm_Resist,				ADDR1_fSetJYMAlm_Resist,	IS_FLOAT},
	{E_TYPE_IMIU2,		ID_fSetJYMAlm_Resist,				ADDR2_fSetJYMAlm_Resist,	IS_FLOAT},
	{E_TYPE_IMIU3,		ID_u8JYMAlmStatMask,				ADDR3_u8JYMAlmStatMask,		IS_CHAR},
	{E_TYPE_IMIU3,		ID_u8SetJYMCtrlMode,				ADDR3_u8SetJYMCtrlMode,		IS_CHAR},
	{E_TYPE_IMIU3,		ID_u8InsltCheckByVolt,				ADDR3_u8InsltCheckByVolt,	IS_CHAR},
	{E_TYPE_IMIU3,		ID_fSetJYMAlm_Resist,				ADDR3_fSetJYMAlm_Resist,	IS_FLOAT},

	{E_TYPE_IMIU4,		ID_u8JYMAlmStatMask,				ADDR3_u8JYMAlmStatMask+1,	IS_CHAR},
	{E_TYPE_IMIU4,		ID_u8SetJYMCtrlMode,				ADDR3_u8SetJYMCtrlMode+1,	IS_CHAR},
	{E_TYPE_IMIU4,		ID_u8InsltCheckByVolt,				ADDR3_u8InsltCheckByVolt+1,	IS_CHAR},
	{E_TYPE_IMIU4,		ID_fSetJYMAlm_Resist,				ADDR3_fSetJYMAlm_Resist+1,	IS_FLOAT},

	{E_TYPE_IMIU5,		ID_u8JYMAlmStatMask,				ADDR3_u8JYMAlmStatMask+2,	IS_CHAR},
	{E_TYPE_IMIU5,		ID_u8SetJYMCtrlMode,				ADDR3_u8SetJYMCtrlMode+2,	IS_CHAR},
	{E_TYPE_IMIU5,		ID_u8InsltCheckByVolt,				ADDR3_u8InsltCheckByVolt+2,	IS_CHAR},
	{E_TYPE_IMIU5,		ID_fSetJYMAlm_Resist,				ADDR3_fSetJYMAlm_Resist+2,	IS_FLOAT},

	{E_TYPE_IMIU6,		ID_u8JYMAlmStatMask,				ADDR3_u8JYMAlmStatMask+3,	IS_CHAR},
	{E_TYPE_IMIU6,		ID_u8SetJYMCtrlMode,				ADDR3_u8SetJYMCtrlMode+3,	IS_CHAR},
	{E_TYPE_IMIU6,		ID_u8InsltCheckByVolt,				ADDR3_u8InsltCheckByVolt+3,	IS_CHAR},
	{E_TYPE_IMIU6,		ID_fSetJYMAlm_Resist,				ADDR3_fSetJYMAlm_Resist+3,	IS_FLOAT},

	{E_TYPE_IMIU7,		ID_u8JYMAlmStatMask,				ADDR3_u8JYMAlmStatMask+4,	IS_CHAR},
	{E_TYPE_IMIU7,		ID_u8SetJYMCtrlMode,				ADDR3_u8SetJYMCtrlMode+4,	IS_CHAR},
	{E_TYPE_IMIU7,		ID_u8InsltCheckByVolt,				ADDR3_u8InsltCheckByVolt+4,	IS_CHAR},
	{E_TYPE_IMIU7,		ID_fSetJYMAlm_Resist,				ADDR3_fSetJYMAlm_Resist+4,	IS_FLOAT},

	{E_TYPE_IMIU8,		ID_u8JYMAlmStatMask,				ADDR3_u8JYMAlmStatMask+5,	IS_CHAR},
	{E_TYPE_IMIU8,		ID_u8SetJYMCtrlMode,				ADDR3_u8SetJYMCtrlMode+5,	IS_CHAR},
	{E_TYPE_IMIU8,		ID_u8InsltCheckByVolt,				ADDR3_u8InsltCheckByVolt+5,	IS_CHAR},
	{E_TYPE_IMIU8,		ID_fSetJYMAlm_Resist,				ADDR3_fSetJYMAlm_Resist+5,	IS_FLOAT},

	{E_TYPE_IMIU9,		ID_u8JYMAlmStatMask,				ADDR3_u8JYMAlmStatMask+6,	IS_CHAR},
	{E_TYPE_IMIU9,		ID_u8SetJYMCtrlMode,				ADDR3_u8SetJYMCtrlMode+6,	IS_CHAR},
	{E_TYPE_IMIU9,		ID_u8InsltCheckByVolt,				ADDR3_u8InsltCheckByVolt+6,	IS_CHAR},
	{E_TYPE_IMIU9,		ID_fSetJYMAlm_Resist,				ADDR3_fSetJYMAlm_Resist+6,	IS_FLOAT},

	{E_TYPE_IMIU10,		ID_u8JYMAlmStatMask,				ADDR3_u8JYMAlmStatMask+7,	IS_CHAR},
	{E_TYPE_IMIU10,		ID_u8SetJYMCtrlMode,				ADDR3_u8SetJYMCtrlMode+7,	IS_CHAR},
	{E_TYPE_IMIU10,		ID_u8InsltCheckByVolt,				ADDR3_u8InsltCheckByVolt+7,	IS_CHAR},
	{E_TYPE_IMIU10,		ID_fSetJYMAlm_Resist,				ADDR3_fSetJYMAlm_Resist+7,	IS_FLOAT},

	{E_TYPE_SYS,		ID_u8StationCode_No1,				ADDR_u8EVStationCode1,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No2,				ADDR_u8EVStationCode2,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No3,				ADDR_u8EVStationCode3,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No4,				ADDR_u8EVStationCode4,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No5,				ADDR_u8EVStationCode5,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No6,				ADDR_u8EVStationCode6,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No7,				ADDR_u8EVStationCode7,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No8,				ADDR_u8EVStationCode8,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No9,				ADDR_u8EVStationCode9,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No10,				ADDR_u8EVStationCode10,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No11,				ADDR_u8EVStationCode11,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No12,				ADDR_u8EVStationCode12,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No13,				ADDR_u8EVStationCode13,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No14,				ADDR_u8EVStationCode14,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No15,				ADDR_u8EVStationCode15,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No16,				ADDR_u8EVStationCode16,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No17,				ADDR_u8EVStationCode17,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No18,				ADDR_u8EVStationCode18,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No19,				ADDR_u8EVStationCode19,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No20,				ADDR_u8EVStationCode20,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No21,				ADDR_u8EVStationCode21,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No22,				ADDR_u8EVStationCode22,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No23,				ADDR_u8EVStationCode23,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No24,				ADDR_u8EVStationCode24,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No25,				ADDR_u8EVStationCode25,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No26,				ADDR_u8EVStationCode26,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No27,				ADDR_u8EVStationCode27,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No28,				ADDR_u8EVStationCode28,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No29,				ADDR_u8EVStationCode29,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No30,				ADDR_u8EVStationCode30,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No31,				ADDR_u8EVStationCode31,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No32,				ADDR_u8EVStationCode32,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No33,				ADDR_u8EVStationCode33,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No34,				ADDR_u8EVStationCode34,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No35,				ADDR_u8EVStationCode35,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No36,				ADDR_u8EVStationCode36,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No37,				ADDR_u8EVStationCode37,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No38,				ADDR_u8EVStationCode38,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No39,				ADDR_u8EVStationCode39,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No40,				ADDR_u8EVStationCode40,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No41,				ADDR_u8EVStationCode41,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No42,				ADDR_u8EVStationCode42,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No43,				ADDR_u8EVStationCode43,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No44,				ADDR_u8EVStationCode44,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No45,				ADDR_u8EVStationCode45,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No46,				ADDR_u8EVStationCode46,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No47,				ADDR_u8EVStationCode47,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No48,				ADDR_u8EVStationCode48,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No49,				ADDR_u8EVStationCode49,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No50,				ADDR_u8EVStationCode50,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No51,				ADDR_u8EVStationCode51,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No52,				ADDR_u8EVStationCode52,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No53,				ADDR_u8EVStationCode53,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No54,				ADDR_u8EVStationCode54,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No55,				ADDR_u8EVStationCode55,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No56,				ADDR_u8EVStationCode56,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No57,				ADDR_u8EVStationCode57,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No58,				ADDR_u8EVStationCode58,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No59,				ADDR_u8EVStationCode59,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8StationCode_No60,				ADDR_u8EVStationCode60,		IS_CHAR},

	{E_TYPE_SYS,		ID_u8OffLinePwsd1,				ADDR_u8OffLinePwsd1,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8OffLinePwsd2,				ADDR_u8OffLinePwsd2,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8OffLinePwsd3,				ADDR_u8OffLinePwsd3,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8OffLinePwsd4,				ADDR_u8OffLinePwsd4,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8OffLinePwsd5,				ADDR_u8OffLinePwsd5,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8OffLinePwsd6,				ADDR_u8OffLinePwsd6,		IS_CHAR},

	{E_TYPE_SYS,		ID_SETDATA_TelaidianStarionID_NUM_1,		ADDR_TelaidianStarionID_1,			IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_TelaidianStarionID_NUM_32,		ADDR_TelaidianStarionID_32,			IS_CHAR},

	{E_TYPE_SYS,		ID_SETDATA_TelaidianUserName_NUM_1,		ADDR_TelaidianUserName_1,			IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_TelaidianUserName_NUM_32,		ADDR_TelaidianUserName_32,			IS_CHAR},

	{E_TYPE_SYS,		ID_SETDATA_TelaidianPassword_NUM_1,		ADDR_TelaidianPassword_1,			IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_TelaidianPassword_NUM_32,		ADDR_TelaidianPassword_32,			IS_CHAR},

	{E_TYPE_SYS,		ID_LocalTimeZone,						ADDR_LocalTimeZone,						IS_FLOAT},
	{E_TYPE_SYS,		ID_SETDATA_MASK_NET_NUM,				ADDR_MaskNet,						IS_CHAR},
	{E_TYPE_SYS,		ID_u8BRUSH_ALM_Mask,				ADDR_u8BRUSH_ALM_Mask,						IS_CHAR},

	
	/*{E_TYPE_SYS,		ID_fTimePrice1,		ADDR_fTimePrice1,						IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+1),	(ADDR_fTimePrice1+1),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+2),	(ADDR_fTimePrice1+2),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+3),	(ADDR_fTimePrice1+3),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+4),	(ADDR_fTimePrice1+4),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+5),	(ADDR_fTimePrice1+5),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+6),	(ADDR_fTimePrice1+6),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+7),	(ADDR_fTimePrice1+7),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+8),	(ADDR_fTimePrice1+8),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+9),	(ADDR_fTimePrice1+9),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+10),	(ADDR_fTimePrice1+10),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+11),	(ADDR_fTimePrice1+11),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+12),	(ADDR_fTimePrice1+12),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+13),	(ADDR_fTimePrice1+13),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+14),	(ADDR_fTimePrice1+14),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+15),	(ADDR_fTimePrice1+15),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+16),	(ADDR_fTimePrice1+16),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+17),	(ADDR_fTimePrice1+17),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+18),	(ADDR_fTimePrice1+18),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+19),	(ADDR_fTimePrice1+19),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+20),	(ADDR_fTimePrice1+20),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+21),	(ADDR_fTimePrice1+21),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+22),	(ADDR_fTimePrice1+22),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+23),	(ADDR_fTimePrice1+23),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+24),	(ADDR_fTimePrice1+24),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+25),	(ADDR_fTimePrice1+25),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+26),	(ADDR_fTimePrice1+26),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+27),	(ADDR_fTimePrice1+27),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+28),	(ADDR_fTimePrice1+28),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+29),	(ADDR_fTimePrice1+29),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+30),	(ADDR_fTimePrice1+30),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+31),	(ADDR_fTimePrice1+31),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+32),	(ADDR_fTimePrice1+32),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+33),	(ADDR_fTimePrice1+33),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+34),	(ADDR_fTimePrice1+34),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+35),	(ADDR_fTimePrice1+35),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+36),	(ADDR_fTimePrice1+36),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+37),	(ADDR_fTimePrice1+37),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+38),	(ADDR_fTimePrice1+38),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+39),	(ADDR_fTimePrice1+39),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+40),	(ADDR_fTimePrice1+40),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+41),	(ADDR_fTimePrice1+41),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+42),	(ADDR_fTimePrice1+42),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+43),	(ADDR_fTimePrice1+43),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+44),	(ADDR_fTimePrice1+44),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+45),	(ADDR_fTimePrice1+45),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+46),	(ADDR_fTimePrice1+46),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimePrice1+47),	(ADDR_fTimePrice1+47),					IS_FLOAT},

	{E_TYPE_SYS,		ID_fTimeServicePrice1,		ADDR_fTimeServicePrice1,			IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+1),	(ADDR_fTimeServicePrice1+1),		IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+2),	(ADDR_fTimeServicePrice1+2),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+3),	(ADDR_fTimeServicePrice1+3),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+4),	(ADDR_fTimeServicePrice1+4),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+5),	(ADDR_fTimeServicePrice1+5),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+6),	(ADDR_fTimeServicePrice1+6),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+7),	(ADDR_fTimeServicePrice1+7),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+8),	(ADDR_fTimeServicePrice1+8),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+9),	(ADDR_fTimeServicePrice1+9),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+10),	(ADDR_fTimeServicePrice1+10),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+11),	(ADDR_fTimeServicePrice1+11),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+12),	(ADDR_fTimeServicePrice1+12),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+13),	(ADDR_fTimeServicePrice1+13),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+14),	(ADDR_fTimeServicePrice1+14),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+15),	(ADDR_fTimeServicePrice1+15),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+16),	(ADDR_fTimeServicePrice1+16),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+17),	(ADDR_fTimeServicePrice1+17),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+18),	(ADDR_fTimeServicePrice1+18),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+19),	(ADDR_fTimeServicePrice1+19),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+20),	(ADDR_fTimeServicePrice1+20),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+21),	(ADDR_fTimeServicePrice1+21),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+22),	(ADDR_fTimeServicePrice1+22),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+23),	(ADDR_fTimeServicePrice1+23),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+24),	(ADDR_fTimeServicePrice1+24),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+25),	(ADDR_fTimeServicePrice1+25),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+26),	(ADDR_fTimeServicePrice1+26),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+27),	(ADDR_fTimeServicePrice1+27),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+28),	(ADDR_fTimeServicePrice1+28),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+29),	(ADDR_fTimeServicePrice1+29),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+30),	(ADDR_fTimeServicePrice1+30),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+31),	(ADDR_fTimeServicePrice1+31),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+32),	(ADDR_fTimeServicePrice1+32),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+33),	(ADDR_fTimeServicePrice1+33),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+34),	(ADDR_fTimeServicePrice1+34),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+35),	(ADDR_fTimeServicePrice1+35),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+36),	(ADDR_fTimeServicePrice1+36),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+37),	(ADDR_fTimeServicePrice1+37),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+38),	(ADDR_fTimeServicePrice1+38),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+39),	(ADDR_fTimeServicePrice1+39),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+40),	(ADDR_fTimeServicePrice1+40),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+41),	(ADDR_fTimeServicePrice1+41),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+42),	(ADDR_fTimeServicePrice1+42),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+43),	(ADDR_fTimeServicePrice1+43),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+44),	(ADDR_fTimeServicePrice1+44),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+45),	(ADDR_fTimeServicePrice1+45),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+46),	(ADDR_fTimeServicePrice1+46),					IS_FLOAT},
	{E_TYPE_SYS,		(ID_fTimeServicePrice1+47),	(ADDR_fTimeServicePrice1+47),					IS_FLOAT},*/

	
	{E_TYPE_SYS,		ID_SETDATA_FeeMode_NUM,	ADDR_SETDATA_FeeMode_NUM,				IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_ServiceFeeMode_NUM,ADDR_SETDATA_ServiceFeeMode_NUM,	IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_fServicePrice_NUM,	ADDR_SETDATA_fServicePrice_NUM,	IS_FLOAT},
	{E_TYPE_SYS,		ID_SETDATA_fFixPrice_NUM,	ADDR_SETDATA_fFixPrice_NUM,			IS_FLOAT},


	
	{E_TYPE_SYS,		ID_SETDATA_AdjustMode_NUM,	ADDR_SETDATA_AdjustMode_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_SETDATA_Adjust_Volt1,	ADDR_SETDATA_Adjust_Volt1,			IS_FLOAT},
	{E_TYPE_SYS,		ID_SETDATA_Adjust_Volt2,	ADDR_SETDATA_Adjust_Volt2,			IS_FLOAT},	

	{E_TYPE_SYS,		ID_SETDATA_SetDataVersion_NUM,	SETDATA_SetDataVersion_NUM,		IS_FLOAT},
	{E_TYPE_SYS,		ID_SETDATA_SoftwareVersion_NUM,	SETDATA_SoftwareVersion_NUM,	IS_FLOAT},	
	{E_TYPE_SYS,		ID_SETDATA_EVChargeCodeOnUI_NUM,ADDR_SETDATA_EVChargeCodeOnUI_NUM,	IS_CHAR},	

	{E_TYPE_SYS,		ID_NetFrameHead_NUM,			ADDR_NetFrameHead_NUM,			IS_ULONG},

	{E_TYPE_SYS,		ID_u8IsRingEnabled,				ADDR_u8IsRingEnabled,			IS_CHAR},
	{E_TYPE_SYS,		ID_fRingStartMergeCurr,			ADDR_fRingStartMergeCurr,		IS_FLOAT},
	{E_TYPE_SYS,		ID_bIsGunInsertMode,			ADDR_bIsGunInsertMode,			IS_CHAR},

	{E_TYPE_SYS,		ID_nGunInsertMode_Start_Hour,	ADDR_nGunInsertMode_Start_Hour,	IS_CHAR},
	{E_TYPE_SYS,		ID_nGunInsertMode_Start_Min,	ADDR_nGunInsertMode_Start_Min,	IS_CHAR},
	{E_TYPE_SYS,		ID_nGunInsertMode_Stop_Hour,	ADDR_nGunInsertMode_Stop_Hour,	IS_CHAR},
	{E_TYPE_SYS,		ID_nGunInsertMode_Stop_Min,		ADDR_nGunInsertMode_Stop_Min,	IS_CHAR},

	{E_TYPE_SYS,		ID_bySelfAddress,				ADDR_bySelfAddress,				IS_CHAR},
	{E_TYPE_SYS,		ID_nTotalRingStub,				ADDR_nTotalRingStub,			IS_CHAR},

	{E_TYPE_SYS,		ID_AllRingsPwrLmt,				ADDR_AllRingsPwrLmt,			IS_FLOAT},
	{E_TYPE_SYS,		ID_RingChgCurrIncSpeed,			ADDR_RingChgCurrIncSpeed,		IS_FLOAT},

	/*{E_TYPE_SYS,		ID_IsSingleGunMode,				ADDR_IsSingleGunMode,			IS_CHAR},
	{E_TYPE_SYS,		ID_IsSingleGunUseNo,			ADDR_IsSingleGunUseNo,			IS_CHAR},*/

	{E_TYPE_SYS,		ID_fContactorShortThreshold,	ADDR_fContactorShortThreshold,	IS_FLOAT},

	/*{E_TYPE_SYS,		ID_u8GBT_OverFrom,						ADDR_u8GBT_OverFrom,			IS_CHAR},
	{E_TYPE_SYS,		ID_u8GunOverTemCtrlType,				ADDR_GunOverTemCtrlType,			IS_CHAR},
	{E_TYPE_SYS,		ID_u8GunOverTempLevel_1,				ADDR_EnGunOverTemLevel1,			IS_CHAR},
	{E_TYPE_SYS,		ID_GunOverTemDiAlmStat,					ADDR_GunOverTemDiAlmStat,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GunUltra_Temp_Level_2,				ADDR_EnGunOverTemLevel2,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8Gun_OverTemp_CLEAR,				ADDR_Gun_Temp_CLEAR,			IS_CHAR},*/
	
	{E_TYPE_SYS,		ID_fGunOverTemLmtCurrRatio,		ADDR_fGunOverTemLmtCurrRatio,	IS_FLOAT},
	{E_TYPE_SYS,		ID_fGunOverTemIntervalTime,		ADDR_fGunOverTemIntervalTime,	IS_FLOAT},

	//{E_TYPE_SYS,		ID_u8ShuimuAutoEndSoc,			ADDR_u8ShuimuAutoEndSoc,		IS_CHAR},
	{E_TYPE_SYS,		ID_UserPassword_L2_1,			ADDR_UserPassword_L2_1,		IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L2_1+1),		(ADDR_UserPassword_L2_1+1),	IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L2_1+2),		(ADDR_UserPassword_L2_1+2),	IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L2_1+3),		(ADDR_UserPassword_L2_1+3),	IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L2_1+4),		(ADDR_UserPassword_L2_1+4),	IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L2_1+5),		(ADDR_UserPassword_L2_1+5),	IS_CHAR},

	{E_TYPE_SYS,		ID_UserPassword_L3_1,			ADDR_UserPassword_L3_1,		IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L3_1+1),		(ADDR_UserPassword_L3_1+1),	IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L3_1+2),		(ADDR_UserPassword_L3_1+2),	IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L3_1+3),		(ADDR_UserPassword_L3_1+3),	IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L3_1+4),		(ADDR_UserPassword_L3_1+4),	IS_CHAR},
	{E_TYPE_SYS,		(ID_UserPassword_L3_1+5),		(ADDR_UserPassword_L3_1+5),	IS_CHAR},

	//{E_TYPE_SYS,		ID_StChgByOverVoltOrCurr_Source, ADDR_StChgByOverVoltOrCurr_Source,	IS_CHAR},	
	{E_TYPE_SYS,		ID_fMinOutVolt,					ADDR_MinOutVolt,		IS_FLOAT},

	{E_TYPE_SYS,		ID_u8FanMaxSpeed,			ADDR_u8FanMaxSpeed,		IS_CHAR},
	//{E_TYPE_SYS,		ID_u8ButtonChgEn,			ADDR_u8ButtonChgEn,		IS_CHAR},

	{E_TYPE_SYS,		ID_WH_TYPE_BAURD,				ADDR_WH_TYPE_BAURD,	IS_CHAR},
	/*{E_TYPE_SYS,		ID_HAS_AC_GUN,					ADDR_HAS_AC_GUN,	IS_CHAR},
	{E_TYPE_SYS,		ID_1_GUN_2_GUN_MODE,			ADDR_1_GUN_2_GUN_MODE,	IS_CHAR},
	{E_TYPE_SYS,		ID_ISLT_SELF_OR_ISO425,			ADDR_ISLT_SELF_OR_ISO425,	IS_CHAR},*/
	//{E_TYPE_SYS,		ID_ONE_BUTTON,					ADDR_ONE_BUTTON,	IS_CHAR},

	/*{E_TYPE_SYS,		ID_u8WIFI_MODE,					ADDR_u8WIFI_MODE,	IS_CHAR},

	{E_TYPE_SYS,		ID_u8WIFI_SSID_1,					ADDR_u8WIFI_SSID_1,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_2,					ADDR_u8WIFI_SSID_2,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_3,					ADDR_u8WIFI_SSID_3,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_4,					ADDR_u8WIFI_SSID_4,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_5,					ADDR_u8WIFI_SSID_5,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_6,					ADDR_u8WIFI_SSID_6,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_7,					ADDR_u8WIFI_SSID_7,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_8,					ADDR_u8WIFI_SSID_8,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_9,					ADDR_u8WIFI_SSID_9,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_10,					ADDR_u8WIFI_SSID_10,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_11,					ADDR_u8WIFI_SSID_11,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_12,					ADDR_u8WIFI_SSID_12,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_13,					ADDR_u8WIFI_SSID_13,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_14,					ADDR_u8WIFI_SSID_14,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_15,					ADDR_u8WIFI_SSID_15,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_16,					ADDR_u8WIFI_SSID_16,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_17,					ADDR_u8WIFI_SSID_17,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_18,					ADDR_u8WIFI_SSID_18,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_19,					ADDR_u8WIFI_SSID_19,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_20,					ADDR_u8WIFI_SSID_20,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_21,					ADDR_u8WIFI_SSID_21,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_22,					ADDR_u8WIFI_SSID_22,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_23,					ADDR_u8WIFI_SSID_23,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_24,					ADDR_u8WIFI_SSID_24,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_25,					ADDR_u8WIFI_SSID_25,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_26,					ADDR_u8WIFI_SSID_26,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_27,					ADDR_u8WIFI_SSID_27,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_28,					ADDR_u8WIFI_SSID_28,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_29,					ADDR_u8WIFI_SSID_29,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_30,					ADDR_u8WIFI_SSID_30,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_31,					ADDR_u8WIFI_SSID_31,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_32,					ADDR_u8WIFI_SSID_32,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_33,					ADDR_u8WIFI_SSID_33,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_34,					ADDR_u8WIFI_SSID_34,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_35,					ADDR_u8WIFI_SSID_35,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_36,					ADDR_u8WIFI_SSID_36,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_37,					ADDR_u8WIFI_SSID_37,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_38,					ADDR_u8WIFI_SSID_38,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_39,					ADDR_u8WIFI_SSID_39,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_40,					ADDR_u8WIFI_SSID_40,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_41,					ADDR_u8WIFI_SSID_41,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_42,					ADDR_u8WIFI_SSID_42,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_43,					ADDR_u8WIFI_SSID_43,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_44,					ADDR_u8WIFI_SSID_44,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_45,					ADDR_u8WIFI_SSID_45,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_46,					ADDR_u8WIFI_SSID_46,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_47,					ADDR_u8WIFI_SSID_47,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_48,					ADDR_u8WIFI_SSID_48,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_49,					ADDR_u8WIFI_SSID_49,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_50,					ADDR_u8WIFI_SSID_50,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_51,					ADDR_u8WIFI_SSID_51,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_52,					ADDR_u8WIFI_SSID_52,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_53,					ADDR_u8WIFI_SSID_53,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_54,					ADDR_u8WIFI_SSID_54,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_55,					ADDR_u8WIFI_SSID_55,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_56,					ADDR_u8WIFI_SSID_56,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_57,					ADDR_u8WIFI_SSID_57,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_58,					ADDR_u8WIFI_SSID_58,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_59,					ADDR_u8WIFI_SSID_59,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_SSID_60,					ADDR_u8WIFI_SSID_60,	IS_CHAR},


	{E_TYPE_SYS,		ID_u8WIFI_PSWD_1,					ADDR_u8WIFI_PSWD_1,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_2,					ADDR_u8WIFI_PSWD_2,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_3,					ADDR_u8WIFI_PSWD_3,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_4,					ADDR_u8WIFI_PSWD_4,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_5,					ADDR_u8WIFI_PSWD_5,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_6,					ADDR_u8WIFI_PSWD_6,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_7,					ADDR_u8WIFI_PSWD_7,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_8,					ADDR_u8WIFI_PSWD_8,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_9,					ADDR_u8WIFI_PSWD_9,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_10,					ADDR_u8WIFI_PSWD_10,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_11,					ADDR_u8WIFI_PSWD_11,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_12,					ADDR_u8WIFI_PSWD_12,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_13,					ADDR_u8WIFI_PSWD_13,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_14,					ADDR_u8WIFI_PSWD_14,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_15,					ADDR_u8WIFI_PSWD_15,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8WIFI_PSWD_16,					ADDR_u8WIFI_PSWD_16,	IS_CHAR},

	{E_TYPE_SYS,		ID_KEY_BOARD_CHARGE_ENBL,			ADDR_KEY_BOARD_CHARGE_ENBL,	IS_CHAR},
	
	
	{E_TYPE_SYS,		ID_DC_GUN_AC_TOTAL_POWER_SET,		ADDR_DC_GUN_AC_TOTAL_POWER_SET,	IS_FLOAT},

	{E_TYPE_SYS,		ID_SETDATA_AC_MCCB_DELAY,			ADDR_SETDATA_AC_MCCB_DELAY,	IS_CHAR},
	
	{E_TYPE_SYS,		ID_TOP_FAN_RUNNING_TIMER,			ADDR_TOP_FAN_RUNNING_TIMER,	IS_CHAR},
	{E_TYPE_SYS,		ID_TOP_FAN_STOP_TIMER,				ADDR_TOP_FAN_STOP_TIMER,	IS_CHAR},
	
	{E_TYPE_SYS,		ID_AC_POWER_SET,				ADDR_AC_POWER_SET,			IS_FLOAT},
	{E_TYPE_SYS,		ID_SUPPORT_WIFI,				ADDR_SUPPORT_WIFI,			IS_CHAR},
	{E_TYPE_SYS,		ID_IMIU_WH_SINGLE,				ADDR_IMIU_WH_SINGLE,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8PLC_TYPE,					ADDR_PLC_TYPE,				IS_CHAR},*/
	{E_TYPE_SYS,		ID_u8GROUP_Detect_Enbl,			ADDR_GROUP_Detect_Enbl,		IS_CHAR},

	/*{E_TYPE_SYS,		ID_IN_I_LIMIT,					ADDR_IN_I_LIMIT,		IS_FLOAT},
	{E_TYPE_SYS,		ID_PLC_WORK_MODE,				ADDR_PLC_WORK_MODE,		IS_CHAR},
	{E_TYPE_SYS,		ID_PLUG_IN_CHAR_EN,				ADDR_PLUG_IN_CHAR_EN,	IS_CHAR},*/

	{E_TYPE_SYS,		ID_U1_BYTES_NUM,				ADDR_U1_BYTES_NUM,	IS_ULONG},
	{E_TYPE_SYS,		ID_U2_BYTES_NUM,				ADDR_U2_BYTES_NUM,	IS_ULONG},

	{E_TYPE_SYS,		ID_u8ACInputContactorOpenDelay,			ADDR_u8ACInputContactorOpenDelay,		IS_CHAR},
	{E_TYPE_SYS,		ID_fDCDCInputAlowMaxVolt,		ADDR_fDCDCInputAlowMaxVolt,					IS_FLOAT},
	{E_TYPE_SYS,		ID_u8GRP1_MODULE_NUM,			ADDR_u8GRP1_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP2_MODULE_NUM,			ADDR_u8GRP2_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP3_MODULE_NUM,			ADDR_u8GRP3_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP4_MODULE_NUM,			ADDR_u8GRP4_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP5_MODULE_NUM,			ADDR_u8GRP5_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP6_MODULE_NUM,			ADDR_u8GRP6_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP7_MODULE_NUM,			ADDR_u8GRP7_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP8_MODULE_NUM,			ADDR_u8GRP8_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP9_MODULE_NUM,			ADDR_u8GRP9_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP10_MODULE_NUM,			ADDR_u8GRP10_MODULE_NUM,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP11_MODULE_NUM,			ADDR_u8GRP11_MODULE_NUM,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8GRP12_MODULE_NUM,			ADDR_u8GRP12_MODULE_NUM,	IS_CHAR},
	{E_TYPE_SYS,		ID_u8DischargeLmtSocFromEV,		ADDR_u8DischargeLmtSocFromEV,	IS_CHAR},
	{E_TYPE_SYS,		ID_fDischargeC10FromEV,			ADDR_fDischargeC10FromEV,		IS_FLOAT},
	{E_TYPE_SYS,		ID_fDischargeLmtVoltFromEV,		ADDR_fDischargeLmtVoltFromEV,	IS_FLOAT},
	{E_TYPE_SYS,		ID_fDischargeVoltCoeffStopVsRatedFromEV,ADDR_fDischargeVoltCoeffStopVsRatedFromEV,IS_FLOAT},

	/*{E_TYPE_SYS,		ID_u8HasElec_CLock,					ADDR_u8HasElec_Lock_3,					IS_CHAR},

	{E_TYPE_SYS,		ID_u8Gun1IsLiquidCool,					ADDR_u8Gun1IsLiquidCool,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8Gun2IsLiquidCool,					ADDR_u8Gun2IsLiquidCool,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8Gun1TempSensorEn,					ADDR_u8Gun1TempSensorEn,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8Gun2TempSensorEn,					ADDR_u8Gun2TempSensorEn,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8IMEUWorkMode,					ADDR_u8IMEUWorkMode,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8OilPumpMaxSpeed,					ADDR_u8OilPumpMaxSpeed,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8LiquidCoolFanMaxSpeed,					ADDR_u8LiquidCoolFanMaxSpeed,					IS_CHAR},

	{E_TYPE_SYS,		ID_u8Gun1OilPumpMask,					ADDR_u8Gun1OilPumpMask,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8Gun2OilPumpMask,					ADDR_u8Gun2OilPumpMask,					IS_CHAR},

	{E_TYPE_SYS,		ID_fChgGun1CableResistorValue,			ADDR_fChgGun1CableResistorValue,		IS_FLOAT},
	{E_TYPE_SYS,		ID_fChgGun2CableResistorValue,			ADDR_fChgGun2CableResistorValue,		IS_FLOAT},

	{E_TYPE_SYS,		ID_u8OilPumpStopDelay,					ADDR_u8OilPumpStopDelay,					IS_CHAR},*/

	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1,					Addr_u8AttributeBitsFromIMM2_1,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+1,					Addr_u8AttributeBitsFromIMM2_1+1,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+2,					Addr_u8AttributeBitsFromIMM2_1+2,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+3,					Addr_u8AttributeBitsFromIMM2_1+3,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+4,					Addr_u8AttributeBitsFromIMM2_1+4,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+5,					Addr_u8AttributeBitsFromIMM2_1+5,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+6,					Addr_u8AttributeBitsFromIMM2_1+6,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+7,					Addr_u8AttributeBitsFromIMM2_1+7,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+8,					Addr_u8AttributeBitsFromIMM2_1+8,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+9,					Addr_u8AttributeBitsFromIMM2_1+9,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+10,				Addr_u8AttributeBitsFromIMM2_1+10,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+11,				Addr_u8AttributeBitsFromIMM2_1+11,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+12,				Addr_u8AttributeBitsFromIMM2_1+12,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+13,				Addr_u8AttributeBitsFromIMM2_1+13,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+14,				Addr_u8AttributeBitsFromIMM2_1+14,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+15,				Addr_u8AttributeBitsFromIMM2_1+15,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+16,				Addr_u8AttributeBitsFromIMM2_1+16,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+17,				Addr_u8AttributeBitsFromIMM2_1+17,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+18,				Addr_u8AttributeBitsFromIMM2_1+18,					IS_CHAR},
	{E_TYPE_SYS,		ID_u8AttributeBitsFromIMM2_1+19,				Addr_u8AttributeBitsFromIMM2_1+19,					IS_CHAR},

	{E_TYPE_SYS,		ID_u8LeakageSensorEn,			Addr_u8LeakageSensorEn,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8ModuleType,				Addr_u8ModuleType,		IS_CHAR},
	{E_TYPE_SYS,		ID_u8TaskIsAcDispatchMode,		ADDR_u8TaskIsAcDispatchMode,		IS_CHAR},
		
	{E_TYPE_INVALID,	INVALID_ITEM,					INVALID_ITEM,				IS_CHAR},
};


UINT8 CheckBrushSem(void)
{
	return gEvChargeInfo.u8BrushSem;
}

void Put_BrushSem(void)
{
	gEvChargeInfo.u8BrushSem = TRUE;
}

UINT8 Pop_BrushSem(void)
{
	if (gEvChargeInfo.u8BrushSem)
	{
		gEvChargeInfo.u8BrushSem = FALSE;
		return TRUE;
	}
	return FALSE;
}

void Put_A_StopSem(void)
{
	gEvChargeInfo.u8A_NoCardStopSem = TRUE;
}

void Put_B_StopSem(void)
{
	gEvChargeInfo.u8B_NoCardStopSem = TRUE;
}

UINT8 Pop_A_StopSem(void)
{
	if (gEvChargeInfo.u8A_NoCardStopSem)
	{
		gEvChargeInfo.u8A_NoCardStopSem = FALSE;
		return TRUE;
	}
	return FALSE;
}

UINT8 Pop_B_StopSem(void)
{
	if (gEvChargeInfo.u8B_NoCardStopSem)
	{
		gEvChargeInfo.u8B_NoCardStopSem = FALSE;
		return TRUE;
	}
	return FALSE;
}
void Put_Ether_3CSem(UINT8 u8SemVal)
{
	UINT8 uIdx = 0;
	for (uIdx = 0; uIdx < ETHER_SEM_NUM; uIdx++)
	{
		if (u8SemVal == gEvChargeInfo.u8EherSem[uIdx])
		{
			return;										//闁跨喐鍩呴棃鈺傚闁跨喐鏋婚幏铚傜瘍闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崢瀣晸閸撳尅缍囬幏鐑芥晸閺傘倖瀚�
		}
	}

	for (uIdx = 0; uIdx < ETHER_SEM_NUM; uIdx++)
	{
		if (ETHER_SEM_INVALID == gEvChargeInfo.u8EherSem[uIdx])
		{
			gEvChargeInfo.u8EherSem[uIdx] = u8SemVal;
			return;
		}
	}
}

UINT8 Pop_Ether_3CSem(void)
{
	UINT8 uIdx = 0;
	UINT8 uRst = 0;
	for (uIdx = 0; uIdx < ETHER_SEM_NUM; uIdx++)
	{
		if (ETHER_SEM_INVALID != gEvChargeInfo.u8EherSem[uIdx])
		{
			uRst = gEvChargeInfo.u8EherSem[uIdx];
			gEvChargeInfo.u8EherSem[uIdx] = ETHER_SEM_INVALID;
			return uRst;
		}
	}
	return ETHER_SEM_INVALID;
}

void Init_EtherSendRecordSem(void)
{
	gEvChargeInfo.u8SendRecordSem[0] = ETHER_SEM_INVALID;
	gEvChargeInfo.u8SendRecordSem[1] = ETHER_SEM_INVALID;
}

void Put_EtherSendRecordSem(UINT16 u16Idx)
{
	if (gEvChargeInfo.u8SendRecordSem[0] == ETHER_SEM_INVALID)
	{
		gEvChargeInfo.u8SendRecordSem[0] = TRUE;
		gEvChargeInfo.u16SendRecordIdx[0] = (UINT16)(MAX(0.1,(u16Idx-1)));
	}

	if (gEvChargeInfo.u8SendRecordSem[1] == ETHER_SEM_INVALID)
	{
		gEvChargeInfo.u8SendRecordSem[1] = TRUE;
		gEvChargeInfo.u16SendRecordIdx[1] = (UINT16)(MAX(0.1,(u16Idx-1)));
	}
}

UINT8 u8Pop_EtherSendRecordSem(UINT16* u16Idx)
{
	if (gEvChargeInfo.u8SendRecordSem[0] == TRUE)
	{
		gEvChargeInfo.u8SendRecordSem[0] = ETHER_SEM_INVALID;
		*u16Idx = gEvChargeInfo.u16SendRecordIdx[0];
		return TRUE;
	}

	if (gEvChargeInfo.u8SendRecordSem[1] == TRUE)
	{
		gEvChargeInfo.u8SendRecordSem[1] = ETHER_SEM_INVALID;
		*u16Idx = gEvChargeInfo.u16SendRecordIdx[1];
		return TRUE;
	}
	return FALSE;
}

void RecordTime(unsigned char *pTime)
{
	struct tm  tmNow;

	gRTCData.fGetLocalTime(&tmNow);
	*pTime++ = tmNow.tm_year - 2000;				/*闁跨喐鏋婚幏锟�*/
	*pTime++ = tmNow.tm_mon;						/*闁跨喐鏋婚幏锟�*/
	*pTime++ = tmNow.tm_mday;						/*闁跨喐鏋婚幏锟�*/
	*pTime++ = tmNow.tm_hour;						/*閺冿拷*/
	*pTime++ = tmNow.tm_min;						/*闁跨喐鏋婚幏锟�*/
	*pTime   = tmNow.tm_sec;						/*闁跨喐鏋婚幏锟�*/
}

void Trap2(void)
{
	Trap1();
}

void Trap1(void)
{
	Trap();
}

void Trap(void)
{
	gSelfTest.fSetStopFeedDog();
}

void Delay(UINT32 delay_count)
{
	UINT32 i,j;
	j = (UINT32)(delay_count*3.5);
	for (i=0; i<j; i++) {}
}

unsigned char fabs1(float fNum1, float fNum2)
{
	fNum1 = fNum1 - fNum2;
	if (fNum1 < 0)
		fNum1 = -fNum1;
	if (fNum1 > 0.0001)
		return TRUE;
	return FALSE;	
}

const int _ytab[2][12] =
{
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

long _timezone = 0;
long _dstbias = 0;

void SetE2prmValue(UINT8 Etype, UINT16 Idx, __XXX_UNION_VALUE unVal)
{
	float fVal = 0;
	UINT8 u8Val = 0;
	UINT16 u16Idx = 0;
	UINT32 u32Addr = INVALID_ITEM;
	UINT16 IdxOffset;

	IdxOffset = 0;
	if(Etype == E_TYPE_SYS)
	{
		if((Idx >= ID_SETDATA_TelaidianStarionID_NUM_1) && (Idx <= ID_SETDATA_TelaidianStarionID_NUM_32))
		{
			IdxOffset = Idx - ID_SETDATA_TelaidianStarionID_NUM_1;
			Idx = ID_SETDATA_TelaidianStarionID_NUM_1;
		}
		else if((Idx >= ID_SETDATA_TelaidianUserName_NUM_1) && (Idx <= ID_SETDATA_TelaidianUserName_NUM_32))
		{
			IdxOffset = Idx - ID_SETDATA_TelaidianUserName_NUM_1;
			Idx = ID_SETDATA_TelaidianUserName_NUM_1;
		}
		else if((Idx >= ID_SETDATA_TelaidianPassword_NUM_1) && (Idx <= ID_SETDATA_TelaidianPassword_NUM_32))
		{
			IdxOffset = Idx - ID_SETDATA_TelaidianPassword_NUM_1;
			Idx = ID_SETDATA_TelaidianPassword_NUM_1;
		}
	}

	UINT16 uTNum = (sizeof(stArgsSigAddr))/sizeof(stArgsSigAddr[0]);
	for (u16Idx = 0; u16Idx < uTNum; u16Idx++)
	{
		if (stArgsSigAddr[u16Idx].u16SigIdx == Idx
			&& stArgsSigAddr[u16Idx].u8EType == Etype)
		{
			u32Addr = stArgsSigAddr[u16Idx].u32E2pAddr;
			break;
		}
	}

	if(stArgsSigAddr[u16Idx].u8SigType == IS_CHAR)
	{
		u32Addr += IdxOffset;
	}

	if (INVALID_ITEM != u32Addr)
	{
		if (stArgsSigAddr[u16Idx].u8SigType == IS_FLOAT)
		{
			if (u32Addr < FLOAT_U32_ARGS_MAX_ADDR)
			{
				fVal = unVal.ftemp;
				SetFloatSetData(u32Addr, &fVal);
			}
		}
		
		if (stArgsSigAddr[u16Idx].u8SigType == IS_CHAR)
		{
			if (u32Addr < CHAR_ARGS_MAX_ADDR)
			{
				u8Val = unVal.chtemp[0];
				SetCharSetData(u32Addr, &u8Val);
			}
		}
		
		if (stArgsSigAddr[u16Idx].u8SigType == IS_ULONG)
		{
			if (u32Addr < FLOAT_U32_ARGS_MAX_ADDR)
			{
				fVal = unVal.ftemp;
				SetFloatSetData(u32Addr, &fVal);
			}
		}
	}
}

// 闁跨喓鐛ら棃鈺傚闁跨喐鏋婚幏鐑芥晸閸婄喎顦柨鐔告灮閹凤拷 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹疯渹绡冮柨鐔告灮閹凤拷 闁跨喕鍓奸悮瀛樺ID闁跨喍鑼庨崙銈嗗瀵繘鏁撻弬銈嗗閻楃敻鏁撴笟銉ュ皡閹风兘鏁撻弬銈嗗闁綁鏁撻弬銈嗗闁跨喖銈洪顒佸閻╂挳鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲瀚欓柨鐔告灮閹烽攱顢栭柨鐕傛嫹
// 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻惃鍡氼啇閹风兘鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔稿焻閿濆繑瀚归柨鐔告灮閹风〇alloc闁跨喐鏋婚幏宄扮础闁跨喐鏋婚幏鐑芥晸閸欘偄鍤栭幏鐑芥晸閹存帪绱濇担鍧楁晸閻偆娅㈤幏鐑芥晸閺傘倖瀚归柨鐔绘閿涘苯褰囬柨鐔诲壖閻氬瓨瀚归崣顏堟晸閺傘倖瀚归柨鐔封偓鐔奉槵ID闁跨喐鏋婚幏鐑芥晸閼存氨灏ㄩ幏绋〥闁跨喐鏋婚幏锟�
// 閸樺鏁撻弬銈嗗闁跨喐鏋婚幏宄板箵闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹波閺嬪嫰鏁撻弬銈嗗閸忓啴鏁撻弬銈嗗!,	闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭憰渚€鏁撻弬銈嗗闁跨喐鏋婚幏铚傜闁跨喐鏋婚幏鐑芥晸閼存矮绱幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷810G闁跨喕绶濋崠鈩冨閹枫垽鏁撻弬銈嗗
void ExSetSigInfo(UINT8 Etype, UINT16 Idx, __XXX_UNION_VALUE unVal,void* p,UINT8 We2p)
{
	//float fVal = 0;
	//UINT8 u8Val = 0;
	void* pEqpInfo = NULL;
	switch (Etype)
	{
	case E_TYPE_SYS:
		pEqpInfo = (EV_CHARGE_INFO*)(&gEvChargeInfo);
		break;	
	case E_TYPE_WH1:
		pEqpInfo = (stWHInfo*)(&gWH_Info[WH_Meter_No_0]);
		break;
	case E_TYPE_WH2:
		pEqpInfo = (stWHInfo*)(&gWH_Info[WH_Meter_No_1]);
		break;
	case E_TYPE_BRUSH:
		pEqpInfo = (BRUSH_INFO*)(&gBrushInfo);
		break;
//by lxr01202    闁跨喓娼鹃棃鈺傚IMIU闁跨喐鏋婚幏鐑芥晸閼存氨灏ㄩ幏绋〥闁跨喐鏋婚幏绋禔RT4闁跨喐鏋婚幏鐑芥晸閺傘倖瀚笽MIU5闁跨喐鏋婚幏鐑芥晸閺傘倖瀚笽MIU8闁跨喐鏋婚幏绋禔RT5闁跨喐鏋婚幏鐑芥晸閺傘倖瀚笽MIU1闁跨喐鏋婚幏鐑芥晸閺傘倖瀚笽MIU4
	case E_TYPE_IMIU1:
//		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[0]);
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[0]);
		break;
	case E_TYPE_IMIU2:
//		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[1]);
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[1]);
		break;
	case E_TYPE_IMIU3:
//		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[2]);
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[2]);
		break;
	case E_TYPE_IMIU4:
//		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[3]);
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[3]);
		break;
	case E_TYPE_IMIU5:
//		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[0]);
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[0]);
		break;
	case E_TYPE_IMIU6:
//		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[1]);
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[1]);
		break;
	case E_TYPE_IMIU7:
//		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[2]);
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[2]);
		break;
	case E_TYPE_IMIU8:
//		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[3]);
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[3]);
		break;
	default:
		return;
		break;
	}

	if (pEqpInfo == NULL)
	{
		return;
	}

	if (BY_E2PROM == We2p)
	{
		SetE2prmValue(Etype, Idx, unVal);
	}

	if (E_TYPE_IMIU1 == Etype || E_TYPE_IMIU2 == Etype || E_TYPE_IMIU3 == Etype)
	{
		switch (Idx)
		{	

		case ID_u8QuickSmpIMIUFlag:
			((stIMIU_Info*)pEqpInfo)->u8QuickSmpIMIUFlag = unVal.chtemp[0];
			break;
		case ID_u8InsltCheckByVolt:
			((stIMIU_Info*)pEqpInfo)->u8InsltCheckByVolt = unVal.chtemp[0];
			break;
		case ID_u8JYMAlmStatMask:
			((stIMIU_Info*)pEqpInfo)->u8JYMAlmStatMask = unVal.chtemp[0];
			break;
		case ID_fSetJYMAlm_Resist:						//	闁跨喐鏋婚幏椋庣喘闁跨喕濮ょ拃锕傛晸閺傘倖瀚归崐锟�
			((stIMIU_Info*)pEqpInfo)->fSetJYMAlm_Resist[0] = unVal.ftemp;
			break;
		case ID_u8SetJYMCtrlMode:						//	闁跨喐鏋婚幏椋庣喘闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽濮搁幀锟� 
			((stIMIU_Info*)pEqpInfo)->u8SetJYMCtrlStat[0] = unVal.chtemp[0];
			break;
		case ID_u8CTRLInsulationCMDFlag:
			((stIMIU_Info*)pEqpInfo)->u8CTRLInsulationCMDFlag[0] = unVal.chtemp[0];
			break;
		case ID_u8CTRLInsulationCMD:							//0x02闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷0x01闁跨喐鏋婚幏椋庡閿燂拷0x00闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�
			((stIMIU_Info*)pEqpInfo)->u8CTRLInsulationCMD[0] = unVal.chtemp[0];
			break;
		case ID_u8InsltCommAlm:									//闁跨喐鏋婚幏椋庣喘闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柅姘舵晸閺傘倖瀚归悩鑸碘偓锟�  TRUE 闁跨喕濮ょ拃锟�
			((stIMIU_Info*)pEqpInfo)->u8InsltCommAlm[0] = unVal.chtemp[0];
			break;
		default:
			break;
		}
	}
	else if (E_TYPE_WH1 == Etype || E_TYPE_WH2 == Etype)
	{
		switch (Idx)
		{
		case ID_WH_Reserved:
			break;
		case ID_u8WH_TYPE:
			((stWHInfo*)pEqpInfo)->u8WH_TYPE = unVal.chtemp[0];
			break;
		case ID_u8WH_ALM_Mask:
			((stWHInfo*)pEqpInfo)->u8WH_ALM_Mask = unVal.chtemp[0];
			break;
		case ID_u8WHCommAlm:
			((stWHInfo*)pEqpInfo)->u8WHCommAlm = unVal.chtemp[0];
			break;
		case ID_fWHRealValue:
			((stWHInfo*)pEqpInfo)->fWHRealValue = unVal.ftemp;
			break;
		case ID_fWH_A_Volt:
			((stWHInfo*)pEqpInfo)->fWH_A_Volt = unVal.ftemp;
			break;
		case ID_fWH_B_Volt:
			((stWHInfo*)pEqpInfo)->fWH_B_Volt = unVal.ftemp;
			break;
		case ID_fWH_C_Volt:
			((stWHInfo*)pEqpInfo)->fWH_C_Volt = unVal.ftemp;
			break;
		case ID_fWH_A_Curr:
			((stWHInfo*)pEqpInfo)->fWH_A_Curr = unVal.ftemp;
			break;
		case ID_fWH_B_Curr:
			((stWHInfo*)pEqpInfo)->fWH_B_Curr = unVal.ftemp;
			break;
		case ID_fWH_C_Curr:
			((stWHInfo*)pEqpInfo)->fWH_C_Curr = unVal.ftemp;
			break;
		case ID_fWHStartValue:
			((stWHInfo*)pEqpInfo)->fWHStartValue = unVal.ftemp;
			break;
		case ID_fWHStartTempValue:
			((stWHInfo*)pEqpInfo)->fWHStartTempValue = unVal.ftemp;
			break;
		case ID_fACWhCoef:
			((stWHInfo*)pEqpInfo)->fACWhCoef = unVal.ftemp;
			break;
		default:
			break;
		}
	}
	
	else if (E_TYPE_BRUSH == Etype)
	{
		
	}
	else if (E_TYPE_SYS == Etype)
	{
		switch (Idx)
		{
		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
		case ID_fCC1_1Coef_A:
			((EV_CHARGE_INFO*)pEqpInfo)->fCC1_1Coef_A = unVal.ftemp;
			break;
		case ID_fCC1_1Coef_B:
			((EV_CHARGE_INFO*)pEqpInfo)->fCC1_1Coef_B = unVal.ftemp;
			break;
		case ID_fCC1_2Coef_A:
			((EV_CHARGE_INFO*)pEqpInfo)->fCC1_2Coef_A = unVal.ftemp;
			break;
		case ID_fCC1_2Coef_B:
			((EV_CHARGE_INFO*)pEqpInfo)->fCC1_2Coef_B = unVal.ftemp;
			break;
		case ID_fSysSetOverVolt:
			((EV_CHARGE_INFO*)pEqpInfo)->fSysSetOverVolt = unVal.ftemp;
			break;
		case ID_fRectLoadStartTime:
			((EV_CHARGE_INFO*)pEqpInfo)->fRectLoadStartTime = unVal.ftemp;
			break;
		case ID_fMaxOutCurr:
			((EV_CHARGE_INFO*)pEqpInfo)->fMaxOutCurr = unVal.ftemp;
			break;
		case ID_fMaxOutVolt:
			((EV_CHARGE_INFO*)pEqpInfo)->fMaxOutVolt = unVal.ftemp;
			break;
		case ID_fMinOutVolt:
			((EV_CHARGE_INFO*)pEqpInfo)->fMinOutVolt = unVal.ftemp;
			break;
		case ID_fBattVolt_1CoefA:
			((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_1CoefA = unVal.ftemp;
			break;
		case ID_fBattVolt_1CoefB:
			((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_1CoefB = unVal.ftemp;
			break;
		case ID_fBattVolt_2CoefA:
			((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_2CoefA = unVal.ftemp;
			break;
		case ID_fBattVolt_2CoefB:
			((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_2CoefB = unVal.ftemp;
			break;

		case ID_TP1000_1_CoefA:
			((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_1_CoefK = unVal.ftemp;
			break;
		case ID_TP1000_1_CoefB:
			((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_1_CoefB = unVal.ftemp;
			break;

		case ID_TP1000_2_CoefA:
			((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_2_CoefK = unVal.ftemp;
			break;
		case ID_TP1000_2_CoefB:
			((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_2_CoefB = unVal.ftemp;
			break;

		case ID_fSetLimitMaxOutCurr_1:
			((EV_CHARGE_INFO*)pEqpInfo)->fSetLimitMaxOutCurr_x[GUN_NO_A] = unVal.ftemp;
			break;		
		case ID_fSetLimitMaxOutCurr_2:
			((EV_CHARGE_INFO*)pEqpInfo)->fSetLimitMaxOutCurr_x[GUN_NO_B] = unVal.ftemp;
			break;

		//4闁跨喕顢滈弬銈嗗闁跨喐鏋婚幏锟�
		case ID_u32PowerOnTimes:
			((EV_CHARGE_INFO*)pEqpInfo)->u32PowerOnTimes = unVal.u32Value;
			break;
		

		case ID_U1_BYTES_NUM:
			((EV_CHARGE_INFO*)pEqpInfo)->u16_U1_Bytes = unVal.u32Value;
			break;

		case ID_U2_BYTES_NUM:
			((EV_CHARGE_INFO*)pEqpInfo)->u16_U2_Bytes = unVal.u32Value;
			break;

		case ID_u32NET_PORT:
			((EV_CHARGE_INFO*)pEqpInfo)->u32NET_PORT = unVal.u32Value;
			break;
		case ID_NetFrameHead_NUM:
			gs_EthAppCommMng.u16NetFrameHead = unVal.u32Value;
			break;
		case ID_NET_DHCP:		// NULL!!
			gEvChargeInfo.u8DHCPClientEn = unVal.chtemp[0];
			break;


		//闁跨喕顢滈弬銈嗗闁跨喐鏋婚幏锟�
		case ID_u8PolarityAlmMask_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PolarityAlmMask_1 = unVal.chtemp[0];
			break;			
		case ID_u8PolarityAlmMask_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PolarityAlmMask_2 = unVal.chtemp[0];
			break;

		case ID_fMaxOffLineTime:
			((EV_CHARGE_INFO*)pEqpInfo)->fMaxOffLineTime = unVal.ftemp;
			break;

		case ID_u8SetCalcMoneyType:
			((EV_CHARGE_INFO*)pEqpInfo)->u8SetCalcMoneyType = unVal.chtemp[0];
			break;

		case ID_u8CC1CheckEnable_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8CC1CheckEnable_1 = unVal.chtemp[0];
			break;

		case ID_u8CC1CheckEnable_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8CC1CheckEnable_2 = unVal.chtemp[0];
			break;

		case ID_u8SelfTestEnable_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8SelfTestEnable_1 = unVal.chtemp[0];
			break;

		case ID_u8SelfTestEnable_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8SelfTestEnable_2 = unVal.chtemp[0];
			break;

		case ID_u8MCCBOnCheckEnable_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8MCCBOnCheckEnable_1 = unVal.chtemp[0];
			break;		
		case ID_u8MCCBOnCheckEnable_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8MCCBOnCheckEnable_2 = unVal.chtemp[0];
			break;		
		case ID_u8OutOverVoltEnable_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8OutOverVoltEnable_1 = unVal.chtemp[0];
			break;		

		case ID_u8OutOverVoltEnable_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8OutOverVoltEnable_2 = unVal.chtemp[0];
			break;		
		case ID_u8DoorAlmMask_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8DoorAlmMask_1 = unVal.chtemp[0];
			break;				
		case ID_u8DoorAlmMask_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8DoorAlmMask_2 = unVal.chtemp[0];
			break;				
		
		case ID_u8PWMPolarityCover:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PWMPolarityCover = unVal.chtemp[0];
			break;
		
		case ID_u8EPOAlmVltLEVEL:
			((EV_CHARGE_INFO*)pEqpInfo)->u8EPOAlmVltLEVEL = unVal.chtemp[0];
			break;	
		case ID_u8LowVoltAlmVltLEVEL:
			((EV_CHARGE_INFO*)pEqpInfo)->u8LowVoltAlmVltLEVEL = unVal.chtemp[0];
			break;		
		case ID_u8NLINEAlmVltLEVEL:
			((EV_CHARGE_INFO*)pEqpInfo)->u8NLineAlmVltLEVEL = unVal.chtemp[0];
			break;		
		case ID_u8BrushFactorType:
			((EV_CHARGE_INFO*)pEqpInfo)->u8BrushFactorType = unVal.chtemp[0];
			break;		
		case ID_u8System_Start_Temp:
			((EV_CHARGE_INFO*)pEqpInfo)->u8EV_StartTemp = unVal.chtemp[0];
			break;			
		case ID_u8System_Ultra_Temp:
			((EV_CHARGE_INFO*)pEqpInfo)->u8EV_Ultra_Temp = unVal.chtemp[0];
			break;				
		case ID_u8NET_OffLine_Charge_Enable:
			((EV_CHARGE_INFO*)pEqpInfo)->u8NET_OffLine_Charge_Enable = unVal.chtemp[0];
			break;
		case ID_A_ChargeCMD:
			((EV_CHARGE_INFO*)pEqpInfo)->u8_Charge_CMD[GUN_NO_A] = unVal.chtemp[0];
			break;
		case ID_B_ChargeCMD:
			((EV_CHARGE_INFO*)pEqpInfo)->u8_Charge_CMD[GUN_NO_B] = unVal.chtemp[0];
			break;
	
		case ID_u8IP_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[0] = unVal.chtemp[0];
			break;
		case ID_u8IP_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[1] = unVal.chtemp[0];
			break;
		case ID_u8IP_3:
			((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[2] = unVal.chtemp[0];
			break;
		case ID_u8IP_4:
			((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[3] = unVal.chtemp[0];
			break;
		case ID_u8IP_5:
			((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[4] = unVal.chtemp[0];
			break;
		case ID_u8IP_6:
			((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[5] = unVal.chtemp[0];
			break;
		case ID_u8Mac_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[0] = unVal.chtemp[0];
			break;
		case ID_u8Mac_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[1] = unVal.chtemp[0];
			break;			
		case ID_u8Mac_3:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[2] = unVal.chtemp[0];
			break;			
		case ID_u8Mac_4:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[3] = unVal.chtemp[0];
			break;			
		case ID_u8Mac_5:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[4] = unVal.chtemp[0];
			break;			
		case ID_u8Mac_6:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[5] = unVal.chtemp[0];
			break;			
		case ID_u8Gateway_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[0] = unVal.chtemp[0];
			break;		
		case ID_u8Gateway_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[1] = unVal.chtemp[0];
			break;		
		case ID_u8Gateway_3:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[2] = unVal.chtemp[0];
			break;		
		case ID_u8Gateway_4:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[3] = unVal.chtemp[0];
			break;		
		case ID_u8Gateway_5:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[4] = unVal.chtemp[0];
			break;		
		case ID_u8Gateway_6:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[5] = unVal.chtemp[0];
			break;		
		case ID_u8Subnetmask_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[0] = unVal.chtemp[0];
			break;	
		case ID_u8Subnetmask_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[1] = unVal.chtemp[0];
			break;	
		case ID_u8Subnetmask_3:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[2] = unVal.chtemp[0];
			break;	
		case ID_u8Subnetmask_4:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[3] = unVal.chtemp[0];
			break;	
		case ID_u8Subnetmask_5:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[4] = unVal.chtemp[0];
			break;	
		case ID_u8Subnetmask_6:
			((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[5] = unVal.chtemp[0];
			break;	
		case ID_u8PowerStarIP_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[0] = unVal.chtemp[0];
			break;	
		case ID_u8PowerStarIP_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[1] = unVal.chtemp[0];
			break;	
		case ID_u8PowerStarIP_3:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[2] = unVal.chtemp[0];
			break;	
		case ID_u8PowerStarIP_4:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[3] = unVal.chtemp[0];
			break;	
		case ID_u8PowerStarIP_5:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[4] = unVal.chtemp[0];
			break;	
		case ID_u8PowerStarIP_6:
			((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[5] = unVal.chtemp[0];
			break;
		case ID_u8SPDAlmMask:
			((EV_CHARGE_INFO*)pEqpInfo)->u8SPDAlmMask = unVal.chtemp[0];
			break;
		case ID_SETDATA_MASK_NET_NUM:
			((EV_CHARGE_INFO*)pEqpInfo)->u8NET_Mask = unVal.chtemp[0];
			break;

		

		

		case ID_u8BRUSH_ALM_Mask:
			((EV_CHARGE_INFO*)pEqpInfo)->u8BRUSH_ALM_Mask = unVal.chtemp[0];
			break;


		case ID_u8StationCode_No1:case ID_u8StationCode_No2:case ID_u8StationCode_No3:case ID_u8StationCode_No4:case ID_u8StationCode_No5:case ID_u8StationCode_No6:case ID_u8StationCode_No7:
		case ID_u8StationCode_No8:case ID_u8StationCode_No9:case ID_u8StationCode_No10:case ID_u8StationCode_No11:case ID_u8StationCode_No12:case ID_u8StationCode_No13:case ID_u8StationCode_No14:
		case ID_u8StationCode_No15:case ID_u8StationCode_No16:case ID_u8StationCode_No17:case ID_u8StationCode_No18:case ID_u8StationCode_No19:case ID_u8StationCode_No20:case ID_u8StationCode_No21:
		case ID_u8StationCode_No22:case ID_u8StationCode_No23:case ID_u8StationCode_No24:case ID_u8StationCode_No25:case ID_u8StationCode_No26:case ID_u8StationCode_No27:case ID_u8StationCode_No28:
		case ID_u8StationCode_No29:case ID_u8StationCode_No30:case ID_u8StationCode_No31:case ID_u8StationCode_No32:case ID_u8StationCode_No33:case ID_u8StationCode_No34:case ID_u8StationCode_No35:
		case ID_u8StationCode_No36:case ID_u8StationCode_No37:case ID_u8StationCode_No38:case ID_u8StationCode_No39:case ID_u8StationCode_No40:case ID_u8StationCode_No41:case ID_u8StationCode_No42:
		case ID_u8StationCode_No43:case ID_u8StationCode_No44:case ID_u8StationCode_No45:case ID_u8StationCode_No46:case ID_u8StationCode_No47:case ID_u8StationCode_No48:case ID_u8StationCode_No49:
		case ID_u8StationCode_No50:case ID_u8StationCode_No51:case ID_u8StationCode_No52:case ID_u8StationCode_No53:case ID_u8StationCode_No54:case ID_u8StationCode_No55:case ID_u8StationCode_No56:
		case ID_u8StationCode_No57:case ID_u8StationCode_No58:case ID_u8StationCode_No59:case ID_u8StationCode_No60:

			((EV_CHARGE_INFO*)pEqpInfo)->u16EVCode[Idx-ID_u8StationCode_No1] = unVal.chtemp[0];
			break;

		
		case ID_u8OffLinePwsd1:case ID_u8OffLinePwsd2:case ID_u8OffLinePwsd3:case ID_u8OffLinePwsd4:case ID_u8OffLinePwsd5:case ID_u8OffLinePwsd6:
			((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[0][Idx-ID_u8OffLinePwsd1] = unVal.chtemp[0];
			break;



		case ID_UserPassword_L2_1:case (ID_UserPassword_L2_1+1):case (ID_UserPassword_L2_1+2):case (ID_UserPassword_L2_1+3):case (ID_UserPassword_L2_1+4):case (ID_UserPassword_L2_1+5):
			((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[1][Idx-ID_UserPassword_L2_1] = unVal.chtemp[0];
			break;
		case ID_UserPassword_L3_1:case (ID_UserPassword_L3_1+1):case (ID_UserPassword_L3_1+2):case (ID_UserPassword_L3_1+3):case (ID_UserPassword_L3_1+4):case (ID_UserPassword_L3_1+5):
			((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[2][Idx-ID_UserPassword_L3_1] = unVal.chtemp[0];
			break;			

		

		case ID_SETDATA_DNSServerIP_NUM_1:
			((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[0] = unVal.chtemp[0];
			break;	
		case ID_SETDATA_DNSServerIP_NUM_2:
			((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[1] = unVal.chtemp[0];
			break;	
		case ID_SETDATA_DNSServerIP_NUM_3:
			((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[2] = unVal.chtemp[0];
			break;	
		case ID_SETDATA_DNSServerIP_NUM_4:
			((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[3] = unVal.chtemp[0];
			break;	

		case ID_SETDATA_RemoteServerIsDomainName_NUM:
			((EV_CHARGE_INFO*)pEqpInfo)->u8RemoteServerIsDomainName = unVal.chtemp[0];
			break;		
		case ID_SETDATA_RemoteServerDomainName_NUM_1:case (ID_SETDATA_RemoteServerDomainName_NUM_1+1):case (ID_SETDATA_RemoteServerDomainName_NUM_1+2):case (ID_SETDATA_RemoteServerDomainName_NUM_1+3):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+4):case (ID_SETDATA_RemoteServerDomainName_NUM_1+5):case (ID_SETDATA_RemoteServerDomainName_NUM_1+6):case (ID_SETDATA_RemoteServerDomainName_NUM_1+7):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+8):case (ID_SETDATA_RemoteServerDomainName_NUM_1+9):case (ID_SETDATA_RemoteServerDomainName_NUM_1+10):case (ID_SETDATA_RemoteServerDomainName_NUM_1+11):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+12):case (ID_SETDATA_RemoteServerDomainName_NUM_1+13):case (ID_SETDATA_RemoteServerDomainName_NUM_1+14):case (ID_SETDATA_RemoteServerDomainName_NUM_1+15):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+16):case (ID_SETDATA_RemoteServerDomainName_NUM_1+17):case (ID_SETDATA_RemoteServerDomainName_NUM_1+18):case (ID_SETDATA_RemoteServerDomainName_NUM_1+19):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+20):case (ID_SETDATA_RemoteServerDomainName_NUM_1+21):case (ID_SETDATA_RemoteServerDomainName_NUM_1+22):case (ID_SETDATA_RemoteServerDomainName_NUM_1+23):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+24):case (ID_SETDATA_RemoteServerDomainName_NUM_1+25):case (ID_SETDATA_RemoteServerDomainName_NUM_1+26):case (ID_SETDATA_RemoteServerDomainName_NUM_1+27):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+28):case (ID_SETDATA_RemoteServerDomainName_NUM_1+29):case (ID_SETDATA_RemoteServerDomainName_NUM_1+30):case (ID_SETDATA_RemoteServerDomainName_NUM_1+31):
			((EV_CHARGE_INFO*)pEqpInfo)->u16ServerName[Idx-ID_SETDATA_RemoteServerDomainName_NUM_1] = unVal.chtemp[0];
			break;

		case ID_u8IsRingEnabled:
			gEvChargeInfo.u8IsRingEnabled = unVal.chtemp[0];
			break;	
		case ID_bySelfAddress:
			gEvChargeInfo.bySelfAddress = unVal.chtemp[0];
			break;
		case ID_nTotalRingStub:
			gEvChargeInfo.nTotalRingStub = unVal.chtemp[0];
			break;
		

		case ID_fRingStartMergeCurr:
			g_RingAllInfo.fRingStartMergeCurr = unVal.ftemp;
			break;		
		case ID_bIsGunInsertMode:
			g_RingAllInfo.emRingMergeMode = (RING_MERGE_MODE)unVal.chtemp[0];
			break;	

		case ID_nGunInsertMode_Start_Hour:
			g_RingAllInfo.nGunInsertMode_Start[0] = unVal.chtemp[0];
			break;		
		case ID_nGunInsertMode_Start_Min:
			g_RingAllInfo.nGunInsertMode_Start[1] = unVal.chtemp[0];
			break;		
		case ID_nGunInsertMode_Stop_Hour:
			g_RingAllInfo.nGunInsertMode_Stop[0] = unVal.chtemp[0];
			break;		
		case ID_nGunInsertMode_Stop_Min:
			g_RingAllInfo.nGunInsertMode_Stop[1] = unVal.chtemp[0];
			break;	
		case ID_AllRingsPwrLmt:
			g_RingAllInfo.fAllRingsPwrLmt = unVal.ftemp;
			break;		
		case ID_RingChgCurrIncSpeed:
			g_RingAllInfo.fRingCurrIncSpeed = unVal.ftemp;
			break;		
		case ID_fContactorShortThreshold:
			gEvChargeInfo.fContactorShortThreshold  = unVal.ftemp;
			break;		
		

		case ID_u8FanMaxSpeed:
			gEvChargeInfo.u8FanMaxSpeed = unVal.chtemp[0];			
			break;
		
		
		case ID_WH_TYPE_BAURD:
			gEvChargeInfo.u8TYPE_BAURD = unVal.chtemp[0];	
			break;
		

		case ID_u8GROUP_Detect_Enbl:
			gEvChargeInfo.u8Earth_DetectEnable = unVal.chtemp[0];
			break;

		case ID_u8ACInputContactorOpenDelay:
			gEvChargeInfo.u8ACInputContactorOpenDelayCfg = unVal.chtemp[0];
			break;	
		case ID_u8LeakageSensorEn:
			gEvChargeInfo.u8LeakageSensorEn = unVal.chtemp[0];
			break;	
		case ID_u8ModuleType:
			gEvChargeInfo.u8ModuleType = unVal.chtemp[0];
			break;	
		case ID_fDCDCInputAlowMaxVolt:
			gEvChargeInfo.fDCDCInputAlowMaxVolt = unVal.ftemp;
			break;		

		case ID_fDischargeC10FromEV:
			gEvChargeInfo.fDischargeC10FromEV = unVal.ftemp;
			break;		
		case ID_fDischargeLmtVoltFromEV:
			gEvChargeInfo.fDischargeLmtVoltFromEV = unVal.ftemp;
			break;		
		case ID_fDischargeVoltCoeffStopVsRatedFromEV:
			gEvChargeInfo.fDischargeVoltCoeffStopVsRatedFromEV = unVal.ftemp;
			break;		
		case ID_u8DischargeLmtSocFromEV:
			gEvChargeInfo.u8DischargeLmtSocFromEV = unVal.chtemp[0];
			break;	
		case ID_u8GRP1_MODULE_NUM:                                                //濡繝鏁撻弬銈嗗闁跨喐鏋婚幏锟�1闁跨喐鏋婚幏閿嬆侀柨鐔告灮閹风兘鏁撻弬銈嗗闁跨噦鎷�
			gEvChargeInfo.u8PowerModuleNumCfg[0] = unVal.chtemp[0];
			break;	
		case ID_u8GRP2_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[1] = unVal.chtemp[0];
			break;	
		case ID_u8GRP3_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[2] = unVal.chtemp[0];
			break;	
		case ID_u8GRP4_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[3] = unVal.chtemp[0];
			break;	
		case ID_u8GRP5_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[4] = unVal.chtemp[0];
			break;	
		case ID_u8GRP6_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[5] = unVal.chtemp[0];
			break;	
		case ID_u8GRP7_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[6] = unVal.chtemp[0];
			break;	
		case ID_u8GRP8_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[7] = unVal.chtemp[0];
			break;	
		case ID_u8GRP9_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[8] = unVal.chtemp[0];
			break;	
		case ID_u8GRP10_MODULE_NUM:
			gEvChargeInfo.u8PowerModuleNumCfg[9] = unVal.chtemp[0];
			break;

		case ID_u8TaskIsAcDispatchMode:
			gEvChargeInfo.u8TaskIsAcDispatchMode = unVal.chtemp[0];
			break;	
	
		
		default:
			break;
		}
	}
	else
	{
		
	}
	return;
}


__XXX_UNION_VALUE unionGetE2pVal(UINT8 Etype, UINT16 Idx, unsigned char* p)
{
	__XXX_UNION_VALUE unVal;
        unVal.chtemp[0] = 0xff;
	float fdata = 0;
	UINT8 u8data = 0;
	UINT16 u16Idx = 0;
	UINT32 u32Addr = INVALID_ITEM;
	UINT16 IdxOffset;

	IdxOffset = 0;
	if(Etype == E_TYPE_SYS)
	{
		if((Idx >= ID_SETDATA_TelaidianStarionID_NUM_1) && (Idx <= ID_SETDATA_TelaidianStarionID_NUM_32))
		{
			IdxOffset = Idx - ID_SETDATA_TelaidianStarionID_NUM_1;
			Idx = ID_SETDATA_TelaidianStarionID_NUM_1;			
		}
		else if((Idx >= ID_SETDATA_TelaidianUserName_NUM_1) && (Idx <= ID_SETDATA_TelaidianUserName_NUM_32))
		{
			IdxOffset = Idx - ID_SETDATA_TelaidianUserName_NUM_1;
			Idx = ID_SETDATA_TelaidianUserName_NUM_1;
		}
		else if((Idx >= ID_SETDATA_TelaidianPassword_NUM_1) && (Idx <= ID_SETDATA_TelaidianPassword_NUM_32))
		{
			IdxOffset = Idx - ID_SETDATA_TelaidianPassword_NUM_1;
			Idx = ID_SETDATA_TelaidianPassword_NUM_1;
		}
	}

	for (u16Idx = 0; u16Idx < ((sizeof(stArgsSigAddr))/sizeof(stArgsSigAddr[0])); u16Idx++)
	{
		if (stArgsSigAddr[u16Idx].u16SigIdx == Idx
			&& stArgsSigAddr[u16Idx].u8EType == Etype)
		{
			u32Addr = stArgsSigAddr[u16Idx].u32E2pAddr;
			break;
		}
	}

	//闁跨喐鏋婚幏宄板絿闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔诲壖閻氬瓨瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 
        if(p != NULL)
	{
          *p = stArgsSigAddr[u16Idx].u8SigType;
        }
	
	if(stArgsSigAddr[u16Idx].u8SigType == IS_CHAR)
	{
		u32Addr += IdxOffset;
	}

	if (stArgsSigAddr[u16Idx].u8SigType == IS_FLOAT)
	{
		if (INVALID_ITEM != u32Addr && u32Addr < FLOAT_U32_ARGS_MAX_ADDR)
		{
			GetFloatSetData(u32Addr, &fdata);
			unVal.ftemp = fdata;
		}
	}
	else if (stArgsSigAddr[u16Idx].u8SigType == IS_CHAR)
	{
		if (INVALID_ITEM != u32Addr && u32Addr < CHAR_ARGS_MAX_ADDR)
		{
			GetCharSetData(u32Addr, &u8data);
			unVal.chtemp[0] = u8data;
		}
	}
	else if (stArgsSigAddr[u16Idx].u8SigType == IS_ULONG)
	{
		if (INVALID_ITEM != u32Addr && u32Addr < FLOAT_U32_ARGS_MAX_ADDR)
		{
			GetFloatSetData(u32Addr, &fdata);
			unVal.ftemp = fdata;				//闁跨喐鏋婚幏铚傝礋闁跨喐鏋婚幏鐑芥晸缂佺偞鍞婚幏鐑芥晸閺傘倖瀚归崜宥夋晸閿燂拷4闁跨喐鏋婚幏鐑芥晸鐞涙鏋婚幏绋OAT 闁跨喐鏋婚幏宄扮础闁跨喐鏋婚幏閿嬆侀柨鐔告灮閹风兘鏁撻弬銈嗗閻氬潡鏁撴鎭掑劃閾旀棏浜烽幏鐑芥晸閺傘倖瀚归柨鐔告灮閹疯渹绱撮柨鐕傛嫹
		}
	}

	return unVal;
}

// 闁跨喓鐛ら棃鈺傚闁跨喐鏋婚幏鐑芥晸閸婄喎顦柨鐔告灮閹凤拷 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹疯渹绡冮柨鐔告灮閹凤拷 闁跨喕鍓奸悮瀛樺ID闁跨喍鑼庨崙銈嗗瀵繘鏁撻弬銈嗗閻楃敻鏁撴笟銉ュ皡閹风兘鏁撻弬銈嗗闁綁鏁撻弬銈嗗闁跨喖銈洪顒佸閻╂挳鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲瀚欓柨鐔告灮閹烽攱顢栭柨鐕傛嫹
// 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻惃鍡氼啇閹风兘鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔稿焻閿濆繑瀚归柨鐔告灮閹风〇alloc闁跨喐鏋婚幏宄扮础闁跨喐鏋婚幏鐑芥晸閸欘偄鍤栭幏鐑芥晸閹存帪绱濇担鍧楁晸閻偆娅㈤幏鐑芥晸閺傘倖瀚归柨鐔绘閿涘苯褰囬柨鐔诲壖閻氬瓨瀚归崣顏堟晸閺傘倖瀚归柨鐔封偓鐔奉槵ID闁跨喐鏋婚幏鐑芥晸閼存氨灏ㄩ幏绋〥闁跨喐鏋婚幏锟�
// 閸樺鏁撻弬銈嗗闁跨喐鏋婚幏宄板箵闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹波閺嬪嫰鏁撻弬銈嗗閸忓啴鏁撻弬銈嗗!,	闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭憰渚€鏁撻弬銈嗗闁跨喐鏋婚幏铚傜闁跨喐鏋婚幏鐑芥晸閼存矮绱幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷810G闁跨喕绶濋崠鈩冨閹枫垽鏁撻弬銈嗗
void ExGetSigInfo(UINT8 Etype, UINT16 Idx, __XXX_UNION_VALUE* unVal,void* p,UINT8 Re2p)
{
	__XXX_UNION_VALUE unTemp;
	void *pEqpInfo = NULL;
	__XXX_UNION_VALUE* PunVal = NULL;
	//p = NULL;
	unsigned char *pSigType = NULL;
	//UNUSED(p);
	PunVal = unVal;
	pSigType = p;

	switch (Etype)
	{
	case E_TYPE_SYS:
		pEqpInfo = (EV_CHARGE_INFO*)(&gEvChargeInfo);
		break;	
	case E_TYPE_WH1:
		pEqpInfo = (stWHInfo*)(&gWH_Info[WH_Meter_No_0]);
		break;
	case E_TYPE_WH2:
		pEqpInfo = (stWHInfo*)(&gWH_Info[WH_Meter_No_1]);
		break;
	case E_TYPE_BRUSH:
		pEqpInfo = (BRUSH_INFO*)(&gBrushInfo);
		break;
	case E_TYPE_IMIU1:
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[0]);
		break;
	case E_TYPE_IMIU2:
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[1]);
		break;
	case E_TYPE_IMIU3:
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[2]);
		break;
	case E_TYPE_IMIU4:
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART4[3]);
		break;
	case E_TYPE_IMIU5:
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[0]);
		break;
	case E_TYPE_IMIU6:
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[1]);
		break;
	case E_TYPE_IMIU7:
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[2]);
		break;
	case E_TYPE_IMIU8:
		pEqpInfo = (stIMIU_Info*)(&gIMIU_Info_UART5[3]);
		break;
	default:
		return;
		break;
	}

	if (pEqpInfo == NULL)
	{
		return;
	}

	unTemp = unionGetE2pVal(Etype, Idx, pSigType);
	*PunVal = unTemp;

	if (E_TYPE_IMIU1 == Etype || E_TYPE_IMIU2 == Etype || E_TYPE_IMIU3 == Etype)
	{
		switch (Idx)
		{
		case ID_u8SetJYMCtrlMode://02 闁岸鏁撻弬銈嗗闁跨喐鏋婚幏宄板酱闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�
			if (BY_E2PROM == Re2p)
			{
				((stIMIU_Info*)pEqpInfo)->u8SetJYMCtrlStat[0] = unTemp.chtemp[0];//u8GetE2pVal(Etype, Idx);
			}
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8SetJYMCtrlStat[0];
			break;
		case ID_u8InsltCheckByVolt:
			if (BY_E2PROM == Re2p)
			{
				((stIMIU_Info*)pEqpInfo)->u8InsltCheckByVolt = unTemp.chtemp[0];	//u8GetE2pVal(Etype, Idx);
			}
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8InsltCheckByVolt;
			break;
		case ID_u8JYMAlmStatMask:
			if (BY_E2PROM == Re2p)
			{
				((stIMIU_Info*)pEqpInfo)->u8JYMAlmStatMask = unTemp.chtemp[0];		//u8GetE2pVal(Etype, Idx);
			}
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8JYMAlmStatMask;
			break;
		case ID_fSetJYMAlm_Resist:
			if (BY_E2PROM == Re2p)
			{
				((stIMIU_Info*)pEqpInfo)->fSetJYMAlm_Resist[0] = unTemp.ftemp;//fGetE2pVal(Etype, Idx);
			}
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fSetJYMAlm_Resist[0];
			break;

		
		case ID_u8QuickSmpIMIUFlag:
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8QuickSmpIMIUFlag;
			break;
		case ID_fJYMCurr:
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fJYMCurr[0];
			break;
		case ID_fJYMVolt:
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fJYMVolt[0];
			break;
		case ID_fJYMP2gVolt:
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fJYMP2gVolt[0];
			break;
		case ID_fJYMN2gVolt:
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fJYMN2gVolt[0];
			break;
		case ID_fJYMP2gResist:
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fJYMP2gResist[0];
			break;
		case ID_fJYMN2gResist:
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fJYMN2gResist[0];
			break;
		case ID_u8JYMAlmStat:							
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8JYMAlmStat[0];
			break;
		case ID_fJYMAction:							
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8JYMAction[0];
			break;
		case ID_fRunJYMShunt_Coef:					
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fRunJYMShunt_Coef[0];
			break;
		case ID_fRunJYMCurr_CoefA:			
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fRunJYMCurr_CoefA[0];
			break;
		case ID_fRunJYMCurr_CoefB:					
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fRunJYMCurr_CoefB[0];
			break;
		case ID_fRunJYMVolt_CoefA:					
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fRunJYMVolt_CoefA[0];
			break;
		case ID_fRunJYMVolt_CoefB:
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fRunJYMVolt_CoefB[0];
			break;
		case ID_fRunJYMAlm_Resist:
			PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fRunJYMAlm_Resist[0];
			break;
		case ID_u8RunJYMCtrlStat:
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8RunJYMCtrlStat[0];
			break;
		case ID_fSetJYMShunt_Coef:
			//PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fSetJYMShunt_Coef[0];
			break;
		case ID_fSetJYMCurr_CoefA:
			//PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fSetJYMCurr_CoefA[0];
			break;
		case ID_fSetJYMCurr_CoefB:
			//PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fSetJYMCurr_CoefB[0];
			break;
		case ID_fSetJYMVolt_CoefA:
			//PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fSetJYMVolt_CoefA[0];
			break;
		case ID_fSetJYMVolt_CoefB:
			//PunVal->ftemp = ((stIMIU_Info*)pEqpInfo)->fSetJYMVolt_CoefB[0];
			break;
		case ID_u8CTRLInsulationCMDFlag:
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8CTRLInsulationCMDFlag[0];
			break;
		case ID_u8CTRLInsulationCMD:
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8CTRLInsulationCMD[0];
			break;
		case ID_u8InsltCommAlm:
			PunVal->chtemp[0] = ((stIMIU_Info*)pEqpInfo)->u8InsltCommAlm[0];
			break;
		default:
			break;
		}
	}
	else if (E_TYPE_WH1 == Etype || E_TYPE_WH2 == Etype)
	{
		//pEqpInfo = (stWHInfo*)(&gWH_Info[GUN_NO_B]);
		switch (Idx)
		{
		case ID_WH_Reserved:
			break;
		case ID_u8WH_TYPE:
			if (BY_E2PROM == Re2p)
			{
				((stWHInfo*)pEqpInfo)->u8WH_TYPE = unTemp.chtemp[0];	//u8GetE2pVal(Etype,Idx);
			}
			PunVal->chtemp[0] = ((stWHInfo*)pEqpInfo)->u8WH_TYPE;
			break;
		case ID_fACWhCoef:
			if (BY_E2PROM == Re2p)
			{
				((stWHInfo*)pEqpInfo)->u8WH_TYPE = unTemp.chtemp[0];	//u8GetE2pVal(Etype,Idx);
			}
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fACWhCoef;
			break;
		case ID_u8WH_ALM_Mask:
			if (BY_E2PROM == Re2p)
			{
				((stWHInfo*)pEqpInfo)->u8WH_ALM_Mask = unTemp.chtemp[0];	//u8GetE2pVal(Etype,Idx);
			}
			PunVal->chtemp[0] = ((stWHInfo*)pEqpInfo)->u8WH_ALM_Mask;
			break;
		case ID_u8WHCommAlm:
			PunVal->chtemp[0] = ((stWHInfo*)pEqpInfo)->u8WHCommAlm;
			break;
		case ID_fWHRealValue:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWHRealValue;
			break;
		case ID_fWH_A_Volt:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWH_A_Volt;
			break;
		case ID_fWH_B_Volt:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWH_B_Volt;
			break;
		case ID_fWH_C_Volt:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWH_C_Volt;
			break;
		case ID_fWH_A_Curr:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWH_A_Curr;
			break;
		case ID_fWH_B_Curr:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWH_B_Curr;
			break;
		case ID_fWH_C_Curr:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWH_C_Curr;
			break;
		case ID_fWHStartValue:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWHStartValue;
			break;
		case ID_fWHStartTempValue:
			PunVal->ftemp = ((stWHInfo*)pEqpInfo)->fWHStartTempValue;
			break;
		default:
			break;
		}
	}
	else if (E_TYPE_BRUSH == Etype)
	{
		
	}
	else if (E_TYPE_SYS == Etype)
	{
		switch (Idx)
		{
/**************************************	UIN32 闁跨喐鏋婚幏鐑芥晸闁颁絻顕滈幏鐑芥晸閺傘倖瀚�	*****************************************************************/
		case ID_u32PowerOnTimes:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u32PowerOnTimes = unTemp.u32Value;
			}
			PunVal->u32Value = ((EV_CHARGE_INFO*)pEqpInfo)->u32PowerOnTimes;
			break;
		
		case ID_U1_BYTES_NUM:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u16_U1_Bytes = unTemp.u32Value;
			}
			PunVal->u32Value = ((EV_CHARGE_INFO*)pEqpInfo)->u16_U1_Bytes;
			break;

		case ID_U2_BYTES_NUM:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u16_U2_Bytes = unTemp.u32Value;
			}
			PunVal->u32Value = ((EV_CHARGE_INFO*)pEqpInfo)->u16_U2_Bytes;
			break;


/**************************************FLOAT 闁跨喐鏋婚幏鐑芥晸闁颁絻顕滈幏鐑芥晸閺傘倖瀚� ******************************************************************/
		case ID_fCC1_1Coef_A:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fCC1_1Coef_A = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fCC1_1Coef_A;
			break;
		case ID_fCC1_1Coef_B:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fCC1_1Coef_B = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fCC1_1Coef_B;
			break;			
		case ID_fCC1_2Coef_A:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fCC1_2Coef_A = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fCC1_2Coef_A;
			break;	
		case ID_fCC1_2Coef_B:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fCC1_2Coef_B = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fCC1_2Coef_B;
			break;	
		case ID_fSysSetOverVolt:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fSysSetOverVolt = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fSysSetOverVolt;
			break;				
		case ID_fRectLoadStartTime:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fRectLoadStartTime = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fRectLoadStartTime;
			break;		
		case ID_fMaxOutCurr:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fMaxOutCurr = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fMaxOutCurr;
			break;		
		case ID_fMaxOutVolt:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fMaxOutVolt = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fMaxOutVolt;
			break;
		case ID_fMinOutVolt:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fMinOutVolt = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fMinOutVolt;
			break;
		case ID_fBattVolt_1CoefA:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_1CoefA = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_1CoefA;
			break;
		case ID_fBattVolt_1CoefB:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_1CoefB = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_1CoefB;
			break;			
		case ID_fBattVolt_2CoefA:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_2CoefA = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_2CoefA;
			break;
		case ID_fBattVolt_2CoefB:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_2CoefB = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fBattVolt_2CoefB;
			break;

		case ID_TP1000_1_CoefA:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_1_CoefK = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_1_CoefK;
			break;
		case ID_TP1000_1_CoefB:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_1_CoefB = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_1_CoefB;
			break;

		case ID_TP1000_2_CoefA:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_2_CoefK = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_2_CoefK;
			break;
		case ID_TP1000_2_CoefB:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_2_CoefB = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fTP1000_2_CoefB;
			break;

		case ID_fSetLimitMaxOutCurr_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fSetLimitMaxOutCurr_x[GUN_NO_A] = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fSetLimitMaxOutCurr_x[GUN_NO_A];
			break;
		case ID_fSetLimitMaxOutCurr_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fSetLimitMaxOutCurr_x[GUN_NO_B] = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fSetLimitMaxOutCurr_x[GUN_NO_B];
			break;
		case ID_fMaxOffLineTime:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->fMaxOffLineTime = unTemp.ftemp;
			}
			PunVal->ftemp = ((EV_CHARGE_INFO*)pEqpInfo)->fMaxOffLineTime;
			break;

/******************************闁跨喕顢滈弬銈嗗 闁跨喐鏋婚幏鐑芥晸闁颁絻顕滈幏鐑芥晸閺傘倖瀚�***************************************************/
		

		

		case ID_u8SPDAlmMask:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8SPDAlmMask = unTemp.chtemp[0];
			}
			PunVal->u32Value = ((EV_CHARGE_INFO*)pEqpInfo)->u8SPDAlmMask;
			break;
		case ID_u8PolarityAlmMask_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PolarityAlmMask_1 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PolarityAlmMask_1;
			break;	
		case ID_u8PolarityAlmMask_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PolarityAlmMask_2 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PolarityAlmMask_2;
			break;	
		case ID_u8SetCalcMoneyType:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8SetCalcMoneyType = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8SetCalcMoneyType;
			break;
		case ID_u8CC1CheckEnable_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8CC1CheckEnable_1 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8CC1CheckEnable_1;
			break;
		case ID_u8CC1CheckEnable_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8CC1CheckEnable_2 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8CC1CheckEnable_2;
			break;
		case ID_u8SelfTestEnable_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8SelfTestEnable_1 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8SelfTestEnable_1;
			break;
		case ID_u8SelfTestEnable_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8SelfTestEnable_2 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8SelfTestEnable_2;
			break;
		case ID_u8MCCBOnCheckEnable_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8MCCBOnCheckEnable_1 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8MCCBOnCheckEnable_1;
			break;
		case ID_u8MCCBOnCheckEnable_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8MCCBOnCheckEnable_2 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8MCCBOnCheckEnable_2;
			break;
		case ID_u8OutOverVoltEnable_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8OutOverVoltEnable_1 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8OutOverVoltEnable_1;
			break;
		case ID_u8OutOverVoltEnable_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8OutOverVoltEnable_2 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8OutOverVoltEnable_2;
			break;
		case ID_u8DoorAlmMask_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8DoorAlmMask_1 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8DoorAlmMask_1;
			break;
		case ID_u8DoorAlmMask_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8DoorAlmMask_2 = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8DoorAlmMask_2;
			break;
		
		case ID_u8PWMPolarityCover:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PWMPolarityCover = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PWMPolarityCover;
			break;
		
		
		
		case ID_u8EPOAlmVltLEVEL:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8EPOAlmVltLEVEL = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8EPOAlmVltLEVEL;
			break;
		case ID_u8LowVoltAlmVltLEVEL:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8LowVoltAlmVltLEVEL = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8LowVoltAlmVltLEVEL;
			break;
		case ID_u8NLINEAlmVltLEVEL:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8NLineAlmVltLEVEL = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8NLineAlmVltLEVEL;
			break;
		case ID_u8BrushFactorType:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8BrushFactorType = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8BrushFactorType;
			break;
		case ID_u8System_Start_Temp:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8EV_StartTemp = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8EV_StartTemp;
			break;
		case ID_u8System_Ultra_Temp:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8EV_Ultra_Temp = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8EV_Ultra_Temp;
			break;
		case ID_u8NET_OffLine_Charge_Enable:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8NET_OffLine_Charge_Enable = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8NET_OffLine_Charge_Enable;
			break;
		case ID_A_ChargeCMD:
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8_Charge_CMD[GUN_NO_A];
			break;
		case ID_B_ChargeCMD:
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8_Charge_CMD[GUN_NO_B];
			break;
		

		case ID_u32NET_PORT:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u32NET_PORT = unTemp.u32Value;
			}
			PunVal->u32Value = ((EV_CHARGE_INFO*)pEqpInfo)->u32NET_PORT;
			break;
		case ID_NetFrameHead_NUM:
			if (BY_E2PROM == Re2p)
			{
				gs_EthAppCommMng.u16NetFrameHead = unTemp.u32Value;
			}
			PunVal->u32Value = gs_EthAppCommMng.u16NetFrameHead;
			break;

		case ID_NET_DHCP:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8DHCPClientEn = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8DHCPClientEn;
			break;

		case ID_u8IP_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[0] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[0];
			break;
		case ID_u8IP_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[1];
			break;
		case ID_u8IP_3:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[2] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[2];
			break;
		case ID_u8IP_4:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[3] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[3];
			break;
		case ID_u8IP_5:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[4] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[4];
			break;
		case ID_u8IP_6:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[5] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8IP_1[5];
			break;
		case ID_u8Mac_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[0] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[0];
			break;
		case ID_u8Mac_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[1];
			break;
		case ID_u8Mac_3:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[2] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[2];
			break;
		case ID_u8Mac_4:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[3] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[3];
			break;
		case ID_u8Mac_5:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[4] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[4];
			break;
		case ID_u8Mac_6:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[5] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Mac[5];
			break;
		case ID_u8Gateway_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[0] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[0];
			break;
		case ID_u8Gateway_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[1];
			break;
		case ID_u8Gateway_3:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[2] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[2];
			break;
		case ID_u8Gateway_4:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[3] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[3];
			break;
		case ID_u8Gateway_5:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[4] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[4];
			break;
		case ID_u8Gateway_6:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[5] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Gateway[5];
			break;
		case ID_u8Subnetmask_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[0] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[0];
			break;
		case ID_u8Subnetmask_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[1];
			break;
		case ID_u8Subnetmask_3:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[2] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[2];
			break;
		case ID_u8Subnetmask_4:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[3] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[3];
			break;
		case ID_u8Subnetmask_5:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[4] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[4];
			break;
		case ID_u8Subnetmask_6:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[5] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8Subnetmask[5];
			break;
		case ID_u8PowerStarIP_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[0] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[0];
			break;
		case ID_u8PowerStarIP_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[1];
			break;
		case ID_u8PowerStarIP_3:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[2] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[2];
			break;
		case ID_u8PowerStarIP_4:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[3] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[3];
			break;
		case ID_u8PowerStarIP_5:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[4] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[4];
			break;
		case ID_u8PowerStarIP_6:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[5] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8PowerStarIP[5];
			break;

		case ID_u8StationCode_No1:case ID_u8StationCode_No2:case ID_u8StationCode_No3:case ID_u8StationCode_No4:case ID_u8StationCode_No5:case ID_u8StationCode_No6:case ID_u8StationCode_No7:
		case ID_u8StationCode_No8:case ID_u8StationCode_No9:case ID_u8StationCode_No10:case ID_u8StationCode_No11:case ID_u8StationCode_No12:case ID_u8StationCode_No13:case ID_u8StationCode_No14:
		case ID_u8StationCode_No15:case ID_u8StationCode_No16:case ID_u8StationCode_No17:case ID_u8StationCode_No18:case ID_u8StationCode_No19:case ID_u8StationCode_No20:case ID_u8StationCode_No21:
		case ID_u8StationCode_No22:case ID_u8StationCode_No23:case ID_u8StationCode_No24:case ID_u8StationCode_No25:case ID_u8StationCode_No26:case ID_u8StationCode_No27:case ID_u8StationCode_No28:
		case ID_u8StationCode_No29:case ID_u8StationCode_No30:case ID_u8StationCode_No31:case ID_u8StationCode_No32:case ID_u8StationCode_No33:case ID_u8StationCode_No34:case ID_u8StationCode_No35:
		case ID_u8StationCode_No36:case ID_u8StationCode_No37:case ID_u8StationCode_No38:case ID_u8StationCode_No39:case ID_u8StationCode_No40:case ID_u8StationCode_No41:case ID_u8StationCode_No42:
		case ID_u8StationCode_No43:case ID_u8StationCode_No44:case ID_u8StationCode_No45:case ID_u8StationCode_No46:case ID_u8StationCode_No47:case ID_u8StationCode_No48:case ID_u8StationCode_No49:
		case ID_u8StationCode_No50:case ID_u8StationCode_No51:case ID_u8StationCode_No52:case ID_u8StationCode_No53:case ID_u8StationCode_No54:case ID_u8StationCode_No55:case ID_u8StationCode_No56:
		case ID_u8StationCode_No57:case ID_u8StationCode_No58:case ID_u8StationCode_No59:case ID_u8StationCode_No60:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u16EVCode[Idx-ID_u8StationCode_No1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u16EVCode[Idx-ID_u8StationCode_No1];
			break;

		

		case ID_u8OffLinePwsd1:case ID_u8OffLinePwsd2:case ID_u8OffLinePwsd3:case ID_u8OffLinePwsd4:case ID_u8OffLinePwsd5:case ID_u8OffLinePwsd6:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[0][Idx-ID_u8OffLinePwsd1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[0][Idx-ID_u8OffLinePwsd1];
			break;



		case ID_UserPassword_L2_1:case (ID_UserPassword_L2_1+1):case (ID_UserPassword_L2_1+2):case (ID_UserPassword_L2_1+3):case (ID_UserPassword_L2_1+4):case (ID_UserPassword_L2_1+5):
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[1][Idx-ID_UserPassword_L2_1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[1][Idx-ID_UserPassword_L2_1];
			break;
		case ID_UserPassword_L3_1:case (ID_UserPassword_L3_1+1):case (ID_UserPassword_L3_1+2):case (ID_UserPassword_L3_1+3):case (ID_UserPassword_L3_1+4):case (ID_UserPassword_L3_1+5):
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[2][Idx-ID_UserPassword_L3_1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u16UserPassword[2][Idx-ID_UserPassword_L3_1];
			break;			
		

		case ID_SETDATA_DNSServerIP_NUM_1:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[0] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[0];
			break;	
		case ID_SETDATA_DNSServerIP_NUM_2:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[1];
			break;	
		case ID_SETDATA_DNSServerIP_NUM_3:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[2] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[2];
			break;	
		case ID_SETDATA_DNSServerIP_NUM_4:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[3] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8DNSIp[3];
			break;	

		case ID_SETDATA_RemoteServerIsDomainName_NUM:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8RemoteServerIsDomainName = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8RemoteServerIsDomainName;
			break;		
		case ID_SETDATA_RemoteServerDomainName_NUM_1:case (ID_SETDATA_RemoteServerDomainName_NUM_1+1):case (ID_SETDATA_RemoteServerDomainName_NUM_1+2):case (ID_SETDATA_RemoteServerDomainName_NUM_1+3):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+4):case (ID_SETDATA_RemoteServerDomainName_NUM_1+5):case (ID_SETDATA_RemoteServerDomainName_NUM_1+6):case (ID_SETDATA_RemoteServerDomainName_NUM_1+7):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+8):case (ID_SETDATA_RemoteServerDomainName_NUM_1+9):case (ID_SETDATA_RemoteServerDomainName_NUM_1+10):case (ID_SETDATA_RemoteServerDomainName_NUM_1+11):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+12):case (ID_SETDATA_RemoteServerDomainName_NUM_1+13):case (ID_SETDATA_RemoteServerDomainName_NUM_1+14):case (ID_SETDATA_RemoteServerDomainName_NUM_1+15):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+16):case (ID_SETDATA_RemoteServerDomainName_NUM_1+17):case (ID_SETDATA_RemoteServerDomainName_NUM_1+18):case (ID_SETDATA_RemoteServerDomainName_NUM_1+19):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+20):case (ID_SETDATA_RemoteServerDomainName_NUM_1+21):case (ID_SETDATA_RemoteServerDomainName_NUM_1+22):case (ID_SETDATA_RemoteServerDomainName_NUM_1+23):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+24):case (ID_SETDATA_RemoteServerDomainName_NUM_1+25):case (ID_SETDATA_RemoteServerDomainName_NUM_1+26):case (ID_SETDATA_RemoteServerDomainName_NUM_1+27):
		case (ID_SETDATA_RemoteServerDomainName_NUM_1+28):case (ID_SETDATA_RemoteServerDomainName_NUM_1+29):case (ID_SETDATA_RemoteServerDomainName_NUM_1+30):case (ID_SETDATA_RemoteServerDomainName_NUM_1+31):
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u16ServerName[Idx-ID_SETDATA_RemoteServerDomainName_NUM_1] = unTemp.chtemp[0];
			}			
			break;
		case ID_SETDATA_MASK_NET_NUM:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8NET_Mask = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8NET_Mask;
			break;
		case ID_u8BRUSH_ALM_Mask:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8BRUSH_ALM_Mask = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8BRUSH_ALM_Mask;
			break;

		case ID_u8IsRingEnabled:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8IsRingEnabled = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8IsRingEnabled;
			break;
		case ID_bySelfAddress:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.bySelfAddress = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.bySelfAddress;
			break;
		case ID_nTotalRingStub:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.nTotalRingStub = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.nTotalRingStub;
			break;
		

		case ID_fRingStartMergeCurr:
			if (BY_E2PROM == Re2p)
			{
				g_RingAllInfo.fRingStartMergeCurr = unTemp.ftemp;;
			}
			PunVal->ftemp = g_RingAllInfo.fRingStartMergeCurr;
			break;
		case ID_AllRingsPwrLmt:
			if (BY_E2PROM == Re2p)
			{
				g_RingAllInfo.fAllRingsPwrLmt = unTemp.ftemp;;
			}
			PunVal->ftemp = g_RingAllInfo.fAllRingsPwrLmt;
			break;
		case ID_RingChgCurrIncSpeed:
			if (BY_E2PROM == Re2p)
			{
				g_RingAllInfo.fRingCurrIncSpeed = unTemp.ftemp;;
			}
			PunVal->ftemp = g_RingAllInfo.fRingCurrIncSpeed;
			break;		
		
		case ID_WH_TYPE_BAURD:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8TYPE_BAURD = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8TYPE_BAURD;
			break;
		
		case ID_u8GROUP_Detect_Enbl:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8Earth_DetectEnable = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8Earth_DetectEnable;
			break;

		case ID_bIsGunInsertMode:
			if (BY_E2PROM == Re2p)
			{
				g_RingAllInfo.emRingMergeMode = (RING_MERGE_MODE)unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = g_RingAllInfo.emRingMergeMode;
			break;

		case ID_nGunInsertMode_Start_Hour:
			if (BY_E2PROM == Re2p)
			{
				g_RingAllInfo.nGunInsertMode_Start[0] = unTemp.chtemp[0];
			}
			PunVal->u32Value = g_RingAllInfo.nGunInsertMode_Start[0];
			break;		
		case ID_nGunInsertMode_Start_Min:
			if (BY_E2PROM == Re2p)
			{
				g_RingAllInfo.nGunInsertMode_Start[1] = unTemp.chtemp[0];
			}
			PunVal->u32Value = g_RingAllInfo.nGunInsertMode_Start[1];
			break;		
		case ID_nGunInsertMode_Stop_Hour:
			if (BY_E2PROM == Re2p)
			{
				g_RingAllInfo.nGunInsertMode_Stop[0] = unTemp.chtemp[0];
			}
			PunVal->u32Value = g_RingAllInfo.nGunInsertMode_Stop[0];
			break;		
		case ID_nGunInsertMode_Stop_Min:
			if (BY_E2PROM == Re2p)
			{
				g_RingAllInfo.nGunInsertMode_Stop[1] = unTemp.chtemp[0];
			}
			PunVal->u32Value = g_RingAllInfo.nGunInsertMode_Stop[1];
			break;		

		case ID_fContactorShortThreshold:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.fContactorShortThreshold = unTemp.ftemp;;
			}
			PunVal->ftemp = gEvChargeInfo.fContactorShortThreshold;
			break;		
		
		case ID_u8FanMaxSpeed:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8FanMaxSpeed = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8FanMaxSpeed;
			break;
		
		

		
		case ID_u8ACInputContactorOpenDelay:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8ACInputContactorOpenDelayCfg = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8ACInputContactorOpenDelayCfg;
			break;	
		case ID_u8LeakageSensorEn:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8LeakageSensorEn = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8LeakageSensorEn;
			break;	
		case ID_u8ModuleType:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8ModuleType = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8ModuleType;
			break;		
		case ID_fDCDCInputAlowMaxVolt:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.fDCDCInputAlowMaxVolt  = unTemp.ftemp;
			}
			PunVal->ftemp = gEvChargeInfo.fDCDCInputAlowMaxVolt ;
			break;		

		case ID_fDischargeC10FromEV:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.fDischargeC10FromEV  = unTemp.ftemp;
			}
			PunVal->ftemp = gEvChargeInfo.fDischargeC10FromEV ;
			break;		
		case ID_fDischargeLmtVoltFromEV:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.fDischargeLmtVoltFromEV  = unTemp.ftemp;
			}
			PunVal->ftemp = gEvChargeInfo.fDischargeLmtVoltFromEV ;
			break;		
		case ID_fDischargeVoltCoeffStopVsRatedFromEV:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.fDischargeVoltCoeffStopVsRatedFromEV  = unTemp.ftemp;
			}
			PunVal->ftemp = gEvChargeInfo.fDischargeVoltCoeffStopVsRatedFromEV ;
			break;		
		case ID_u8DischargeLmtSocFromEV:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8DischargeLmtSocFromEV = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8DischargeLmtSocFromEV;
			break;		

		case ID_u8GRP1_MODULE_NUM:                                          //濡繝鏁撻弬銈嗗闁跨喐鏋婚幏锟�1闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[0] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[0];			
			break;	
		case ID_u8GRP2_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[1] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[1];
			break;	
		case ID_u8GRP3_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[2] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[2];
			break;	
		case ID_u8GRP4_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[3] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[3];
			break;	
		case ID_u8GRP5_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[4] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[4];
			break;	
		case ID_u8GRP6_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[5] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[5];
			break;	
		case ID_u8GRP7_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[6] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[6];
			break;	
		case ID_u8GRP8_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[7] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[7];
			break;	
		case ID_u8GRP9_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[8] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[8];
			break;	
		case ID_u8GRP10_MODULE_NUM:
			if (BY_E2PROM == Re2p)
			{
				gEvChargeInfo.u8PowerModuleNumCfg[9] = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = gEvChargeInfo.u8PowerModuleNumCfg[9];
			break;	
		case ID_u8TaskIsAcDispatchMode:
			if (BY_E2PROM == Re2p)
			{
				((EV_CHARGE_INFO*)pEqpInfo)->u8TaskIsAcDispatchMode = unTemp.chtemp[0];
			}
			PunVal->chtemp[0] = ((EV_CHARGE_INFO*)pEqpInfo)->u8TaskIsAcDispatchMode;
			break;
		
		default:
			break;
		}
	}
	else
	{

	}
	return;
}


struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf)
{
	time_t time = *timer;
	unsigned long dayclock, dayno;
	int year = EPOCH_YR;

	dayclock = (unsigned long) time % SECS_DAY;
	dayno = (unsigned long) time / SECS_DAY;

	tmbuf->tm_sec = dayclock % 60;
	tmbuf->tm_min = (dayclock % 3600) / 60;
	tmbuf->tm_hour = dayclock / 3600;
	tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
	while (dayno >= (unsigned long) YEARSIZE(year))
	{
		dayno -= YEARSIZE(year);
		year++;
	}
	tmbuf->tm_year = year - YEAR0;
	tmbuf->tm_yday = dayno;
	tmbuf->tm_mon = 0;
	while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon])
	{
		dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
		tmbuf->tm_mon++;
	}
	tmbuf->tm_mday = dayno + 1;
	tmbuf->tm_isdst = 0;

	return tmbuf;
}

time_t Getsec(INT16 Index)
{
	UCHAR Mon, Day, Hour, i;
	UINT16 NumDay = 0;
	time_t sec;
	GetCharSetData(Index, &Mon);
	GetCharSetData(Index+1, &Day);
	GetCharSetData(Index+2, &Hour);

	for(i = 0; i < (Mon-1); i++)
	{
		NumDay += _ytab[0][i];
	}
	sec = (NumDay + Day - 1) * DAY_SECONDS + Hour * HOUR_SECONDS + GMT_TIME_MIN+31622400;
	return sec;
}

time_t mktime_r(struct tm *tmbuf)
{
	long day, year;
	int tm_year;
	int yday, month;
	long seconds;
	int overflow;
	long dst;

	tmbuf->tm_min += tmbuf->tm_sec / 60;
	tmbuf->tm_sec %= 60;
	if (tmbuf->tm_sec < 0)
	{
		tmbuf->tm_sec += 60;
		tmbuf->tm_min--;
	}
	tmbuf->tm_hour += tmbuf->tm_min / 60;
	tmbuf->tm_min = tmbuf->tm_min % 60;
	if (tmbuf->tm_min < 0)
	{
		tmbuf->tm_min += 60;
		tmbuf->tm_hour--;
	}
	day = tmbuf->tm_hour / 24;
	tmbuf->tm_hour= tmbuf->tm_hour % 24;
	if (tmbuf->tm_hour < 0)
	{
		tmbuf->tm_hour += 24;
		day--;
	}
	tmbuf->tm_year += tmbuf->tm_mon / 12;
	tmbuf->tm_mon %= 12;
	if (tmbuf->tm_mon < 0)
	{
		tmbuf->tm_mon += 12;
		tmbuf->tm_year--;
	}
	day += (tmbuf->tm_mday - 1);
	while (day < 0)
	{
		if(--tmbuf->tm_mon < 0)
		{
			tmbuf->tm_year--;
			tmbuf->tm_mon = 11;
		}
		day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
	}
	while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon])
	{
		day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
		if (++(tmbuf->tm_mon) == 12)
		{
			tmbuf->tm_mon = 0;
			tmbuf->tm_year++;
		}
	}
	tmbuf->tm_mday = day + 1;
	year = EPOCH_YR;
	if (tmbuf->tm_year < year - YEAR0) return (time_t) -1;
		seconds = 0;
	day = 0;
	overflow = 0;

	tm_year = tmbuf->tm_year + YEAR0;

	if (TIME_MAX / 365 < tm_year - year) overflow++;
	day = (tm_year - year) * 365;
	if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;
	day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
	day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
	day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

	yday = month = 0;
	while (month < tmbuf->tm_mon)
	{
		yday += _ytab[LEAPYEAR(tm_year)][month];
		month++;
	}
	yday += (tmbuf->tm_mday - 1);
	if (day + yday < 0) overflow++;
	day += yday;

	tmbuf->tm_yday = yday;
	tmbuf->tm_wday = (day + 4) % 7;               // Day 0 was thursday (4)
	seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;
	if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;
	seconds += day * SECS_DAY;

	// Now adjust according to timezone and daylight saving time
	if (((_timezone > 0) && (TIME_MAX - _timezone < seconds))
		|| ((_timezone < 0) && (seconds < -_timezone)))
		overflow++;
	seconds += _timezone;

	if (tmbuf->tm_isdst)
		dst = _dstbias;
	else
		dst = 0;

	if (dst > seconds) overflow++;        // dst is always non-negative
		seconds -= dst;

	if (overflow) return (time_t) -1;
	if ((time_t) seconds != seconds) return (time_t) -1;
	return (time_t) seconds;
}


void  floattoascii(float fin, unsigned char * pDataBuf)
{
  union
  {
	  float	ftemp;						          /*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷*/
	  unsigned long	Ltemp;
	  unsigned char	chtemp[4];					    /*闁跨喕顢滈弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�*/
  }unTempVal;

  unTempVal.ftemp = fin;/*閸欐牞顩︽潪顒勬晸閺傘倖瀚归柨鐔惰寧闂堚晜瀚归柨鐔告灮閹风兘鏁撻弬銈嗗*/
#ifdef _LITTLE_ENDIAN_TYPE
  *pDataBuf       = unTempVal.chtemp[0];/*鏉烆剟鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲鐔柨鐔告灮閹凤拷3,2,1,闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�*/
  *(pDataBuf + 1) = unTempVal.chtemp[1];
  *(pDataBuf + 2) = unTempVal.chtemp[2];
  *(pDataBuf + 3) = unTempVal.chtemp[3];
#else
  *pDataBuf       = unTempVal.chtemp[3];/*鏉烆剟鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗闁跨喐鏋婚幏锟�,鐏忛箖鏁撻弬銈嗗1,2,3*/
  *(pDataBuf + 1) = unTempVal.chtemp[2];
  *(pDataBuf + 2) = unTempVal.chtemp[1];
  *(pDataBuf + 3) = unTempVal.chtemp[0];
#endif
}

void  FourByte2float(unsigned char * pDataBuf, float * pfout)
{
  union
  {
	  float			ftemp;						          /*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷*/
	  unsigned long	Ltemp;
	  unsigned char	chtemp[4];					    /*闁跨喕顢滈弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�*/
  }unTempVal;

#ifdef _LITTLE_ENDIAN_TYPE
  unTempVal.chtemp[0] = *pDataBuf;/*鏉烆剟鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲鐔柨鐔告灮閹凤拷3,2,1,闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�*/
  unTempVal.chtemp[1] = *(pDataBuf + 1);
  unTempVal.chtemp[2] = *(pDataBuf + 2);
  unTempVal.chtemp[3] = *(pDataBuf + 3);
#else
  unTempVal.chtemp[3] = *pDataBuf;/*鏉烆剟鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗闁跨喐鏋婚幏锟�,鐏忛箖鏁撻弬銈嗗1,2,3*/
  unTempVal.chtemp[2] = *(pDataBuf + 1);
  unTempVal.chtemp[1] = *(pDataBuf + 2);
  unTempVal.chtemp[0] = *(pDataBuf + 3);
#endif
  *pfout = unTempVal.ftemp;/*閸欐牞顩︽潪顒勬晸閺傘倖瀚归柨鐔惰寧闂堚晜瀚归柨鐔告灮閹风兘鏁撻弬銈嗗*/
}

unsigned char	BCDToHex(unsigned char	Input)
{
	return	(Input & 0x0f) + (Input >> 4) * 10;
}


unsigned char	RTCWeekToHex (unsigned char Input)
{
  UCHAR out=0,i;
  for(i=0;i<6;i++)
  {
    out += ((Input>>i)&0x01)*(i+1);
  }
	return	out;
}


unsigned char	HexToRTCWeek(unsigned char	Input)
{
    UCHAR out=0,i;
    for(i=0;i<6;i++)
    {
        if(Input>(i+1))
        {
            out |=0x01<<i;
            Input = Input-i-1;
        }
        else
        {
            break;
        } 
    }
	return	out;
}

unsigned char	HexToBCD(unsigned char	Input)
{
	return	(Input % 10) + (Input / 10) * 16;
}


char *strncpyz(char *pDst, const char *pSrc, int nDstLen)
{
  char *p = pDst;

  if (p == NULL)
	{
    return NULL;
	}

	if (pSrc != NULL)
	{
		for( nDstLen -= 1; *pSrc && (nDstLen > 0); nDstLen -- )
		{
			*p ++ = *pSrc ++;
		}
	}

  *p = '\0';   /* append a string end flag '\0'    */

  return pDst;
}

float CharToFloat(volatile UINT32 data)
{
	/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗闁跨喕顢滈弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�*/
	union
	{
		FLOAT32 f32Temp;/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷*/
		UINT32 u32Temp;
	}unTempVal;
	unTempVal.u32Temp = data;
	return unTempVal.f32Temp;
}

unsigned char Hex2Ascii(unsigned char Input)
{
	if(Input > 9)
	{
		return Input + 0x37; /*A - F*/
	}
	else
	{
		return Input + 0x30; /*0 - 9*/
	}
}
unsigned char Ascii2Hex(unsigned char Input)
{
	if(Input > 0x39)
	{
		return Input - 0x37;					/*'A' - 'F'*/
	}
	else
	{
		return Input - 0x30;					/*'0' - '9'*/
	}
}
unsigned char TwoAscii2Hex(unsigned char InputH, unsigned char InputL)
{
	return	((Ascii2Hex(InputH)) << 4) + Ascii2Hex(InputL);
}

UINT16 CalcSendCheckSum(UINT8* pSendBuf, UINT16 dataL)
{
	UINT16 i = 0;
	UINT16 IICRC;
	IICRC = 0;
	for (i = 0; i < dataL; i++)
	{
		IICRC += pSendBuf[YDN23_VER_PST + i];
	}
	IICRC = IICRC%65536;

	return ((~IICRC)+1);
}

//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚瑰▽锟犳晸閺傘倖瀚归柨鐔告灮閹凤拷 闁跨喕顫楃拠褎瀚归柨鐔活潡绾板瀚�1闁跨喐鏋婚幏鐑芥晸鐞涙濡幘鍛闁跨喐鏋婚幏锟�0x7e,闁跨喐鏋婚幏宄版啽闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崨鏇㈡晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻敓锟�
UINT16 CalcRcvCheckSum(UINT8 UartNo)
{
	UINT16 i = 0;
	UINT16 IICRC = 0;
	if (UART5_SEQUECE_NO == UartNo)
	{
		for (i = 0; i < (dv_Uart5.pData - 6) && (i < RS485_UART5_RX_MAX_NUM); i++)
		{
			IICRC += gUart5RecvBuff[YDN23_VER_PST + i];
		}
	}
	else if (UART6_SEQUECE_NO == UartNo)
	{
		for (i = 0; i < (dv_Uart6.Head - 6) && (i < UART6_YDN23_RECV_MAX_NUM); i++)
		{
			IICRC += Uart6_YDN23_RecvBuff[YDN23_VER_PST + i];
		}
	}
	else if (UART4_SEQUECE_NO == UartNo)
	{
		for (i = 0; i < (dv_Uart4.pData - 6) && (i < RS485_UART4_RX_MAX_NUM); i++)
		{
			IICRC += gUart4RecvBuff[YDN23_VER_PST + i];
		}
	}
	/*else  if (UART1_SEQUECE_NO == UartNo)
	{
		for (i = 0; i < (gWifiData.u16WifiYdnLen - 6) && (i < UART1_RECV_MAX_NUM); i++)
		{
			IICRC += gWifiData.u8UART1WIFIYdnData[YDN23_VER_PST + i];
		}
	}*/
	else
	{
		return 0;
	}

	IICRC = IICRC%65536;
	return ((~IICRC)+1);
}

UINT16 CalcCodeCRC(UINT8 *data, UINT16 len)
{
	UINT16 i = 0;
	UINT16 j = 0;
	UINT16 sCRC = 0xffff;

	for(i = 0; i < len; i++)
	{
		sCRC = sCRC ^ data[i];
		for(j = 0; j < 8; j++)
		{
			if(sCRC & 0x01)
			{
			  sCRC = sCRC ^ 0xA001;
			}
			sCRC = sCRC >> 1;
		}
	}

	return(sCRC);
}

UINT16 CalcCodeCRCFor485(UINT8 *data, UINT16 len)
{
	UINT16 i = 0;
	UINT16 j = 0;
	UINT16 sCRC = 0xffff;

	for(i = 0; i < len; i++)
	{
		sCRC = sCRC ^ data[i];
		for(j = 0; j < 8; j++)
		{
			if(sCRC & 0x01)
			{
                sCRC = sCRC >> 1U;
			    sCRC = sCRC ^ 0xA001;
			} else {
                sCRC = sCRC >> 1U;
            }
		}
	}
	return(sCRC);
}

UINT8 LenCheckSum(UINT16 wLen)
{
	UINT8 byLenCheckSum;

	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹疯渹璐�0闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风柉顩﹂柨鐔告灮閹风兘鏁撻弬銈嗗
	if (wLen == 0)
	{
		return 0;
	}

	byLenCheckSum = 0;
	byLenCheckSum += wLen & 0x000F;         //閸欐牠鏁撻弬銈嗗闁跨噦鎷�4闁跨喐鏋婚幏绋T
	byLenCheckSum += (wLen >> 4) & 0x000F;  //閸欐牠鏁撻弬銈嗗4~7闁跨喐鏋婚幏绋T
	byLenCheckSum += (wLen >> 8) & 0x000F;  //閸欐牠鏁撻弬銈嗗8~11闁跨喐鏋婚幏绋T
	byLenCheckSum %= 16;                    //濡拷16
	byLenCheckSum = (~byLenCheckSum) + 1;   //閸欐牠鏁撻弬銈嗗闁跨喐鏋婚幏锟�1
	byLenCheckSum &= 0x0F;                  //閸欘亜褰�4闁跨喐鏋婚幏绋T

	return byLenCheckSum;
}







//闁跨喎澹欓悮瀛樺闁跨喐鏋婚幏锟� 娑撳搫浠犲銏ゆ晸閺傘倖瀚圭紒缁㈠灲閹风兘鏁撻敓锟�
//void SetChargeSTOP_Cmd(UINT8 u8StopReason)
//{
//	return;
//}



void Ctrl_B_RectOut(float fCurr, float fVolt)
{

}



float fGet_A_RectOutVolt(void)
{
	void *PEInfo;
	PEInfo = GetgEquipInfo(E_TYPE_WH1);
	return (((stWHInfo*)PEInfo)->fWH_A_Volt);
}

float fGet_B_RectOutVolt(void)
{
	void *PEInfo;
	PEInfo = GetgEquipInfo(E_TYPE_WH2);
	return (((stWHInfo*)PEInfo)->fWH_A_Volt);
}

float fGet_A_RectOutCurr(void)
{
	/*void *PEInfo;
	PEInfo = GetgEquipInfo(E_TYPE_WH1);
	return (((stWHInfo*)PEInfo)->fWH_A_Curr);*/

	return gs_EnergyOutputMng.fGunRealOutPutVolt[GUN_NO_A];

}

float fGet_B_RectOutCurr(void)
{
	/*void *PEInfo;
	PEInfo = GetgEquipInfo(E_TYPE_WH2);
	return (((stWHInfo*)PEInfo)->fWH_A_Curr);*/

	return gs_EnergyOutputMng.fGunRealOutPutVolt[GUN_NO_B];

}
void * GetgEquipInfo(UINT8 u8Etype)
{
	switch (u8Etype)
	{
	case E_TYPE_SYS:
		return &gEvChargeInfo;
		break;
	case E_TYPE_WH1:
		return &gWH_Info[WH_Meter_No_0];
		break;
	case E_TYPE_WH2:
		return &gWH_Info[WH_Meter_No_1];
		break;
	case E_TYPE_BRUSH:
		return &gBrushInfo;
		break;
	case E_RECT_INFO:
		return &gs_CANComm;
		break;	
	case  E_TYPE_IMIU1:
//by lxr01202
//		return &gIMIU_Info_UART4[0];
		return &gIMIU_Info_UART5[0];
		break;
	case E_TYPE_IMIU2:
//		return &gIMIU_Info_UART4[1];
		return &gIMIU_Info_UART5[1];
		break;
	case E_TYPE_IMIU3:
//		return &gIMIU_Info_UART4[2];
		return &gIMIU_Info_UART5[2];
		break;
	case E_TYPE_IMIU4:
//		return &gIMIU_Info_UART4[3];
		return &gIMIU_Info_UART5[3];
		break;
	case E_TYPE_IMIU5:
//		return &gIMIU_Info_UART5[0];
		return &gIMIU_Info_UART4[0];
		break;
	case E_TYPE_IMIU6:
//		return &gIMIU_Info_UART5[1];
		return &gIMIU_Info_UART4[1];
		break;
	case E_TYPE_IMIU7:
//		return &gIMIU_Info_UART5[2];
		return &gIMIU_Info_UART4[2];
		break;
	case E_TYPE_IMIU8:
//		return &gIMIU_Info_UART5[3];
		return &gIMIU_Info_UART4[3];
		break;

	//case E_TYPE_OTHER:
	//	break;
	default:
		if(gSelfTest.StopFeedDogReason == 0)
		{
			gSelfTest.u32DogBit_Bak = u8Etype;
			gSelfTest.StopFeedDogReason = 1;
			gs_E2PROM.fSetHistoryEventUpdate();
		}
		dv_Output.fOpenBeep();
		dv_Output.fOpenBeep();
		OSTimeDly(700);
		OSTimeDly(700);
		OSTimeDly(700);
		OSTimeDly(700);
		OSTimeDly(700);
		Trap1();
                break;
	}
		
	dv_Output.fOpenBeep();
	OSTimeDly(700);
	dv_Output.fCloseBeep();
	OSTimeDly(700);
	dv_Output.fOpenBeep();
	OSTimeDly(700);
	dv_Output.fCloseBeep();
	OSTimeDly(700);
	dv_Output.fOpenBeep();
	OSTimeDly(700);
	dv_Output.fCloseBeep();
	OSTimeDly(700);
	dv_Output.fOpenBeep();
	OSTimeDly(700);
	dv_Output.fCloseBeep();
	OSTimeDly(700);
	Trap1();
	return NULL;
}






/*==========================================================================*
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 : RecordChargeRecord
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 : 閸愭瑩鏁撻弬銈嗗闁跨喐鏋婚幏宄扮秿
* 闁跨喐鏋婚幏鐤槵闁跨喐鏋婚幏鐑芥晸閿燂拷 :  enum RECORD_CHG_EVENT_DEF
* 闁跨喐鏋婚幏鐑芥晸閼哄倽顕滈幏鐑芥晸閺傘倖瀚� : 
* 鐠囨挳鏁撻弬銈嗗     :                  
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�     :          
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�     : 
* 闁跨喓娼鹃弨鍦€嬮幏宄扮秿 :
*==========================================================================*/

ChargeRecordMng_TypeDef gs_ChargeRecordMng[2] = {0};
SECCStatusRecordMng_TypeDef gs_SECCStatusRecordMng[2] = {0};

void RecordChargeRecord(unsigned char GunNo, unsigned char j)
{
	if(GunNo >1)
	{
		return;
	}

	gs_ChargeRecordMng[GunNo].ChargeRecord[gs_ChargeRecordMng[GunNo].CurrChargeRecord].u16EndReason = j;
	gs_ChargeRecordMng[GunNo].ChargeRecord[gs_ChargeRecordMng[GunNo].CurrChargeRecord].u16ChgDuration = 0;
	gs_ChargeRecordMng[GunNo].ChargeRecord[gs_ChargeRecordMng[GunNo].CurrChargeRecord].u16ChgEnegy = 0;

	gs_ChargeRecordMng[GunNo].CurrChargeRecord++;
	if(gs_ChargeRecordMng[GunNo].CurrChargeRecord >= CHARGE_RECORD_NUM)
	{
		gs_ChargeRecordMng[GunNo].CurrChargeRecord = 0;     
		gs_ChargeRecordMng[GunNo].ChargeRecordFull = 1;      
	}     

	if(gs_ChargeRecordMng[GunNo].ChargeRecordFull == 1)
		gs_ChargeRecordMng[GunNo].ucChargeRecordNum = CHARGE_RECORD_NUM;
	else
		gs_ChargeRecordMng[GunNo].ucChargeRecordNum = gs_ChargeRecordMng[GunNo].CurrChargeRecord;

	if(gs_ChargeRecordMng[GunNo].WriteChargeRecordFlag < 255)
		gs_ChargeRecordMng[GunNo].WriteChargeRecordFlag++;
}


/*==========================================================================*
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 : init_ChargeRecordMng
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 : 闁跨喐鏋婚幏宄邦潗闁跨喐鏋婚幏锟�
* 闁跨喐鏋婚幏鐤槵闁跨喐鏋婚幏鐑芥晸閿燂拷 : 
* 闁跨喐鏋婚幏鐑芥晸閼哄倽顕滈幏鐑芥晸閺傘倖瀚� : 
* 鐠囨挳鏁撻弬銈嗗     :                  
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�     :          
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�     : 
* 闁跨喓娼鹃弨鍦€嬮幏宄扮秿 :
*==========================================================================*/
void init_ChargeRecordMng(unsigned char GunNo)
{
	if(GunNo >1)
	{
		return;
	}

	memset((unsigned char *)&gs_ChargeRecordMng[GunNo], 0, sizeof(ChargeRecordMng_TypeDef));

	gs_ChargeRecordMng[GunNo].CurrentMachineStatBakup = 0xff;

	memset((unsigned char *)&gs_SECCStatusRecordMng[GunNo], 0, sizeof(SECCStatusRecordMng_TypeDef));

	gs_SECCStatusRecordMng[GunNo].CurrentMachineStatBakup = 0xff;
}

/*==========================================================================*
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 : GetChargeRecordIndex
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗 : 闁跨喐鏋婚幏宄板絿闁跨喐鏋婚幏鐑芥晸閺傘倖瀚硅ぐ鏇㈡晸閺傘倖瀚归柨鐔告灮閹凤拷
* 闁跨喐鏋婚幏鐤槵闁跨喐鏋婚幏鐑芥晸閿燂拷 : 
* 闁跨喐鏋婚幏鐑芥晸閼哄倽顕滈幏鐑芥晸閺傘倖瀚� : 
* 鐠囨挳鏁撻弬銈嗗     :                  
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�     :          
* 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚�     : 
* 闁跨喓娼鹃弨鍦€嬮幏宄扮秿 :
*==========================================================================*/
unsigned char GetChargeRecordIndex(unsigned char GunNo, unsigned char nID)
{
	unsigned char  idx;

	if(GunNo >1)
	{
		return 0;
	}

	if (nID > CHARGE_RECORD_NUM - 1)
	{    
		return  0;
	}
	else if (gs_ChargeRecordMng[GunNo].ucChargeRecordNum == 0)
	{    
		return  0;
	}
	else
	{
		if(gs_ChargeRecordMng[GunNo].CurrChargeRecord == 0)
		{
			idx = CHARGE_RECORD_NUM - 1;
		}
		else
		{
			idx = gs_ChargeRecordMng[GunNo].CurrChargeRecord - 1;
		}
		while(nID > 0)
		{
			if (idx > 0)
			{
				idx--;
			}
			else
			{
				idx = CHARGE_RECORD_NUM - 1;
			}
			nID--;
		}
		return  idx;
	}
}

unsigned char u8Is1000VHardwareA00(void)
{
	if(HARDWARE_IS_1000V == gEvChargeInfo.u8Is_1000V_Hardware)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void MyTimeout_start(MyTimer_info_t *timer)
{
    coTime now;

    MutexLock(gEvChargeInfo.hMetex_Timer);
    now = HalTimeGet_ms();
    timer->Enable = !FALSE;
    timer->Timeout = now;
    MutexUnlock(gEvChargeInfo.hMetex_Timer);
}

coRegs MyTimeout(MyTimer_info_t *timer, coLong ticks)
{
    coRegs ret = FALSE;
    coTime now;

    MutexLock(gEvChargeInfo.hMetex_Timer);
    now = HalTimeGet_ms();

    if (timer->Enable == FALSE)
    {
        MutexUnlock(gEvChargeInfo.hMetex_Timer);
        return ret;
    }

    if (now < timer->Timeout)
    {
        timer->Timeout = now;
        MutexUnlock(gEvChargeInfo.hMetex_Timer);
        return ret;
    }

    if ((now - timer->Timeout) >= (2 * ticks))
    {
        //*timep += ticks;
        timer->Timeout = now;
        ret = !FALSE;
    }
    else if ((now - timer->Timeout) >= ticks)
    {
        timer->Timeout += ticks;
        ret = !FALSE;
    }

    MutexUnlock(gEvChargeInfo.hMetex_Timer);
    return ret;
}

void MyTimeout_stop(MyTimer_info_t *timer)
{
	timer->Enable = FALSE;
	timer->Timeout = 0;
}



coRegs coTimeout(coTime *timep, coLong ticks)
{
	coRegs  ret = FALSE;
	coTime  now;

	MutexLock(gEvChargeInfo.hMetex_Timer);
	now = HalTimeGet_ms();

	if (ticks == 0u)
	{
		*timep = now;
	}
	else if (now < *timep)
	{
		*timep = now;
		MutexUnlock(gEvChargeInfo.hMetex_Timer);
		return ret;
	}
	else if ((now - *timep) >= (2 *ticks))
	{
		//*timep += ticks;
		*timep = now;
		ret = !FALSE;
	}
	else if ((now - *timep) >= ticks)
	{
		*timep += ticks;
		ret = !FALSE;
	}

	MutexUnlock(gEvChargeInfo.hMetex_Timer);
	return ret;
}