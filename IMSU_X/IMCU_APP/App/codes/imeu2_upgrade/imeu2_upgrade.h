/**
  ******************************************************************************
  * @file    imeu2_upgrade.h
  * @author  ZZC955
  * @version V1.0
  * @date    2024-1-6
  * @brief   灯板升级模块
  * @history 2024-1-6 ZZC955 创建
  ******************************************************************************
*/

#ifndef IMEU2_UPGRADE_H
#define IMEU2_UPGRADE_H

#include "Upgrade.h"
#include "bsp_usb.h"
#include "secondary_upgrade.h"

#define CFG_IMEU2_APP_ADDR   0xA000UL

#define IMEU2_UPGRADE_BLOCK_MAX_NUM 2U
#define IMEU2_UPGRADE_START_BLOCK_NO 6U
#define IMEU2_UPDATE_BASE_ADDR (IMEU2_UPGRADE_START_BLOCK_NO * EX_PAGE_NUM_PER_BLOCK * EX_BYTES_NUM_PER_PAGE)
#define IMEU2_UPDATE_DATA_ADDR (IMEU2_UPDATE_BASE_ADDR + 256U)

extern FIL MyFile11;

void EraseImeu2ExFlash(void);
void WriteImeu2ExFlash(void);
void Imeu2Querystatus(UINT8 Addr);
void Imeu2JumptoBoot(UINT8 Addr);
void Imeu2Handshake(Secondary_Upgrade_info_t *me);
void Imeu2StartSection(Secondary_Upgrade_info_t *me);
void Imeu2TransmitMessage(Secondary_Upgrade_info_t *me, UINT8 *pu8Buf);
void Imeu2EndOneSection(Secondary_Upgrade_info_t *me);
void Imeu2Reboot(INT8U Addr);

#endif
