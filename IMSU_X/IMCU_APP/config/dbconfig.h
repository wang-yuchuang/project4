#ifndef DBCONFIG_H
#define DBCONFIG_H

#include "main.h"
#include "EtherNet_lwip.h"
#include "EtherNet.h"
#include "Uart6_new.h"
#include "resolv.h"
#include "can_ring.h"
#include "PowerSwitch.h"
#include "secondary_upgrade.h"
#include "eeprom_map.h"
#include "flash_map.h"
#include "Measure.h"
#include "ESPCU.h"
#include "CCU.h"
#include "lwip_comm.h"
#include "ethernetif.h"
#include "api.h"
#include "CCMRAM.h"
#include "cJSON.h"
#include "ring_buffer.h"
#include "ESPCU_dbstd.def"
#include "espcu_coapp.h"

#define ALLGRP 5			  // GRPMAX是实际上的组数，ALLGRP是规约容纳的最多组数
#define MAX_BAT_GRP GRP_BAT	  // 数组. 电池簇BMS数据信息.
#define MAX_PCS_GRP GRP_PCS	  // 数组. 功率单元实时控制调节.
#define MAX_MPPT_GRP GRP_MPPT // 数组. 功率单元实时控制调节.
#define MAX_IMEU_GRP IMEU
#define MAX_IMIU_GRP IMIU
#define MAX_PMDEV_GRP GRPMAX
#define ESPCU_DBSTD_LASTFIELD {NULL, NULL, 0, 0, NULL},
extern BYTE gSoftDI[64];
#define ON 0xA1
#define OFF 0xA0
#define ESPCU_GsetInfo_Size 512
typedef union
{
	int intval;
	float floatval;
} espcu_data;

typedef enum
{
	CTRLWORD = 1,
	DEV_SUX = 2,
	DEV_MEU1,
	DEV_PM,
	DEV_ABU,
	DEV_MIU,
	DEV_MLU,
	DEV_TYPEMAX,
} DeviceType;


typedef enum
{
	AutoRun = 0,
	Manual_Debug,
	Manual_CtrlOffGrid,
	CertificationTest,
	CS
} WorkMode;
typedef enum
{
	PCS = 0x01,
	MPPT,
	sdB2B_Chg,
	sdB2B_Dischg,
	biB2B,
	biBus2ACOut,
	biBus2DCOut,
	ABU

} PwRMODType;
typedef enum
{
	DataType_float = 0x01,
	DataType_int,
	DataType_string,
	DataType_float_array,
	DataType_int_array,
	DataType_string_array,
	DataType_Ending
} DataType;

typedef struct
{
	void *multiref;
	float *pdata;
	union
	{
		int data_integer;
		float data_float;
	} data;
	DataType ucdatatype;
	const char *pstring;
} _TAB_2ESCC_DATABASE_;

typedef struct
{
	int subcmd;
	int seq;
	int dev;
	const char *pstring;
	int arg1;
	int arg2;
} _TAB_2ESCC_CTRLCORRES_;

typedef struct
{
	float Run_Statu;
	float OnOff_Statu;
	float Alm1_Sys;
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD_BMS_DATA
#define ESPCU_DBSTD(num, desc, type, dev) float desc[MAX_BAT_GRP];
#include "ESPCU_dbstd.def"
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD_PCS_ADJ
#define ESPCU_DBSTD(num, desc, type, dev) float desc[MAX_PCS_GRP];
#include "ESPCU_dbstd.def"
} _STRUCT_gESCC_HeartBeat_Info_;

typedef struct
{
	float DevType;
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD_AUX_DEV_UPLOAD
#define ESPCU_DBSTD(num, desc, type, dev) float desc[MAX_IMEU_GRP];
#include "ESPCU_dbstd.def"
} _STRUCT_gESPCU_auxDevUpload_Info_;

typedef struct
{
	float PM_Tp;
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD_PM_GRP_DATA
#define ESPCU_DBSTD(num, desc, type, dev) float desc[MAX_PMDEV_GRP];
#include "ESPCU_dbstd.def"
} _STRUCT_gESPCU_PWRModGrp_Info_;

typedef struct
{
	float PM_Tp;
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD_SIG_GRP_DATA
#define ESPCU_DBSTD(num, desc, type, dev) float desc[MAX_PMDEV_GRP];
#include "ESPCU_dbstd.def"
} _STRUCT_gESPCU_PWRModSig_Info_;

typedef struct
{
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD_HEARTBEATESPCU
#define ESPCU_DBSTD(num, desc, type, dev) float desc;
#include "ESPCU_dbstd.def"
} _STRUCT_gESPCU_HeartBeat_Info_;

typedef struct
{
#undef ESPCU_DBSTD(num, desc, type, dev)
#define ESPCU_DBSTD_SETREQ
#define ESPCU_DBSTD(num, desc, type, dev) float desc;
#include "ESPCU_dbstd.def"
} _STRUCT_gESPCU_Gset_Info_;
#pragma pack(1)  // 默认对齐方式 用于e2rom存储gset值
typedef struct _tagESPCU_GsetInfo
{
	UINT32 occupied;//已初始化
	_STRUCT_gESPCU_Gset_Info_ gset_info;
	UINT16 crc;

} ESPCU_GsetInfo, *pESPCU_GsetInfo;
#pragma pack()

typedef union _ESPCU_GsetInfo_UNION
{
	ESPCU_GsetInfo Gset_Config_Params;  // 4+Gset+2字节
	UINT8 parallel[ESPCU_GsetInfo_Size];						//用于通过联合体字节对齐，1k 方便后续数据字节操作并且e2rom空间占位
} ESPCU_GsetInfo_UNION, *pESPCU_GsetInfo_UNION;
extern char *gESPCU_siglPMBarcode[MAX_PMDEV_GRP];
extern char *gESPCU_DevStr[5];
extern _TAB_2ESCC_DATABASE_ gtabESCCDatabase[];
#if (IMEU > 0)
extern _TAB_2ESCC_DATABASE_ gtabESCC_auxDevUpload_Database[];
#endif
extern _TAB_2ESCC_DATABASE_ gtabESPCU_HeartBeat_Database[];
extern _TAB_2ESCC_CTRLCORRES_ gtabESPCU_CtrlReq_Database[];
extern _TAB_2ESCC_DATABASE_ gtabESCC_PwrModGrp_Database[];
extern _TAB_2ESCC_DATABASE_ gtabESPCU_GSetReq_Database[];

extern _STRUCT_gESPCU_HeartBeat_Info_ gESPCU_HeartBeat_Info;
extern _STRUCT_gESCC_HeartBeat_Info_ gESCC_HeartBeat_Info;
extern _STRUCT_gESPCU_Gset_Info_ gESPCU_Gset_Info;

extern _STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_PCS_ModGrp_Info;
extern _STRUCT_gESPCU_PWRModSig_Info_ gESPCU_PCS_ModSig_Info;
extern _STRUCT_gESPCU_auxDevUpload_Info_ gESPCU_auxDevUpload_Info;
#if (GRP_MPPT > 0)
extern _STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_MPPT_ModGrp_Info;
#endif
#if (GRP_B2B_CHG > 0)
extern _STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_B2B_CHGModGrp_Info;
#endif
#if (GRP_B2B_DIS > 0)
extern _STRUCT_gESPCU_PWRModGrp_Info_ gESPCU_B2B_DISModGrp_Info;
#endif

void dbprint_table(_TAB_2ESCC_DATABASE_ *pDatabase);
#endif