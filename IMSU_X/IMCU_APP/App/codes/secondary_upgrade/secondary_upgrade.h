/**
 ******************************************************************************
 * Copyright(c) Infy Power 2023-2024
 * @file    secondary_upgrade.h
 * @author  ZZC955
 * @version V1.0
 * @date    2024-1-2
 * @brief   沿用LX项目的升级EU2升级模块，经过改动为IMBTU1，灯板，EU2板升级
 * @history 2024-1-2 ZZC955从LX项目移到本项目
 ******************************************************************************
 */


#ifndef SECONDARY_UPGRADE_H
#define SECONDARY_UPGRADE_H

#include "base_def.h"

#define CFG_IMBTU1_ADDR_1 0xA0U
#define CFG_IMBTU1_ADDR_2 0xA1U

#define CFG_IMLU2_ADDR    0x20U

#define CFG_IMEU2_ADDR    0xB8U

#define UPGRADE_TYPE_IMBTU1 1U
#define UPGRADE_TYPE_IMLU2  2U
#define UPGRADE_TYPE_IMEU2  3U

#define COMM_TYPE_CAN       0U
#define COMM_TYPE_485       1U

enum
{
	eState_Idle = 0,
	eState_Normal,
	eState_Exit,
};

enum
{
	eIMEU_Upgrade_idle = 0u,
	eIMEU_Upgrade_jump,
	eIMEU_Upgrade_waiting_handshake,
	eIMEU_Upgrade_start,
	eIMEU_Upgrade_transmit,
	eIMEU_Upgrade_finish,
	eIMEU_Upgrade_end,
	eIMEU_Upgrade_reboot,
	eIMEU_Upgrade_state_max,
};

typedef struct T_Machine
{
	INT8 Step;
	INT8 Times;
	INT8 TaskID;
} Machine_t;

typedef struct Secondary_Upgrade_info
{
	INT8 MachineState;
	INT8 TotalSectionNum;
	INT8 WorkMode;
	INT8 TotalFramePerSection;
	INT8 CurrSendSection;
	INT8 IsRetransmit;
	INT16 CurrSendStep;
	INT16 CurrSectionBytes;
	INT16 CurrSectionSendBytes;
	INT16 CurrCRC;
	UINT16 u16QueryInterval;
	UINT16 u16TransmitInterval;
	UINT16 u16UpdateTimeOutCount;
	INT32 TotalByteNum;
	INT32 SendByteNum;
	UINT32 u32FlashBaseAddr;
	UINT32 u32FlashDataAddr;
	Machine_t Machine;
	UINT8 CurrDecviceAddr;
	UINT8 u8CurrDeviceStatus;
	UINT8 u8CurrUpgradeState;
	UINT8 u8TotalUpgradeState;
	UINT8 u8UpgradeErrCode;
	UINT8 u8UpgradeFlag;
	UINT8 u8UpgradeCmd;
	UINT8 u8CommType;
	UINT8 u8BoardType;
	UINT8 u8_POWER_ON_Flag;
	UINT8 u8RectUpdateSuccessWriteUDisk_Flag;
	float fProgress;
	void (*QuerystatusFunc)(UINT8 u8Addr);
	void (*JumptoBootFunc)(UINT8 u8Addr);
	void (*HandshakeFunc)(struct Secondary_Upgrade_info *me);
	void (*StartSectionFunc)(struct Secondary_Upgrade_info *me);
	void (*TransmitMessageFunc)(struct Secondary_Upgrade_info *me, UINT8 *pu8Buf);
	void (*EndOneSectionFunc)(struct Secondary_Upgrade_info *me);
	void (*RebootFunc)(UINT8 u8Addr);
} Secondary_Upgrade_info_t;

struct _IMEUUpgradeMachineInfo
{
	int MachineIdx;
	int (*fproc) (Secondary_Upgrade_info_t *me);
};
typedef struct _IMEUUpgradeMachineInfo IMEU_UPGRADE_MACHINE_INFO;

void SecondaryUpgradeMachine(Secondary_Upgrade_info_t *me);
void SecondaryUpgradeInit(UINT8 u8UpgradeType, Secondary_Upgrade_info_t *me, UINT8 u8Addr);
void SecondaryUnpackData(Secondary_Upgrade_info_t *me, UINT8 *pu8RxBuf, UINT16 u16Len);
UINT8 SecondaryUpgradeProc(void);

UINT16 CalculateCRC(unsigned char* puchMsg, unsigned char usDataLen);

extern Secondary_Upgrade_info_t g_stImeu2UpgradeInfo[2];

#endif
