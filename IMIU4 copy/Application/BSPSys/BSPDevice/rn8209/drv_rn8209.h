/**
 ******************************************************************************
 * @file    drv_rn8209.h
 * @author  ZZC955
 * @version V1.0
 * @date    2023-9-9
 * @brief   rn8209读写操作内部头文件
 * @history 2023-9-9 ZZC955 创建
 ******************************************************************************
 */

#ifndef DRV_RN8209_H
#define DRV_RN8209_H

#define DRV_RN8209_STATIC static

#define RN8209_DUMMY_BYTE         0xAAU
#define RN8209_WRITE_BIT7         0x80U

#define SPECIAL_CMD_RN8209        0xEAU
#define DATA_RN8209_WRITE_ENABLE  0xE5U
#define DATA_RN8209_WRITE_PROTECT 0xDCU
#define DATA_RN8209_RESRT         0xFAU
#define CMD_SYSCON                0x00U
#define CMD_EMUCON                0x01U
#define CMD_SPL_IA                0x30U
#define CMD_SPL_U                 0x32U
#define CMD_IARMS                 0x22U
#define CMD_URMS                  0x24U
#define CMD_HFCONST               0x02U
#define CMD_SYS_STATUS            0x43U
#define CMD_DCIAH                 0x13U
#define CMD_DCUH                  0x15U
#define CMD_DCL                   0x16U

#define CMD_DEVICE_ID             0x7FU  // 820900h

#endif