/**
 ******************************************************************************
 * Copyright(c) Infy Power 2023-2024
 * @file    imeu2_upgrade.c
 * @author  ZZC955
 * @version V1.0
 * @date    2024-1-6
 * @brief   EU2升级模块
 * @history 2024-1-6 ZZC955 创建
 ******************************************************************************
 */

/* 头文件 */
#include "imeu2_upgrade.h"
#include "interface.h"
#include "bsp_usb.h"

#define UPGRADE_USB_RAM_BUF_SIZE 256U

/* 全局变量定义 */
UINT8 u8Usb2RamBuf[UPGRADE_USB_RAM_BUF_SIZE] = {0};

/* 内部函数声明 */


/* 内部函数定义 */


/* 外部函数定义 */
void EraseImeu2ExFlash(void)
{
    UINT8 u8BlockNo = 0;
    UINT32 u32Addres = 0;
    UINT8 u8MaxBlockNum, u8BaseBlockNo;

    u8MaxBlockNum = IMEU2_UPGRADE_BLOCK_MAX_NUM;
    u8BaseBlockNo = IMEU2_UPGRADE_START_BLOCK_NO;
    for (u8BlockNo = 0; u8BlockNo < u8MaxBlockNum; u8BlockNo++) {
        u32Addres = ((u8BlockNo + u8BaseBlockNo) * EX_PAGE_NUM_PER_BLOCK * EX_BYTES_NUM_PER_PAGE) + 1U;
        SPIFlashEraseBlock(u32Addres);
    }
}

void WriteImeu2ExFlash(void)
{
    UINT16 u16BytesRead;
    FRESULT res;
    UINT32 u32Addr;
    UINT32 u32TotalRead = 0;
    co_dword dword;
    UINT8 u8WriteBuf[4U];

    while (1) {
        res = f_read(&MyFile11, u8Usb2RamBuf, UPGRADE_USB_RAM_BUF_SIZE, (void *)&u16BytesRead);
        if ((u16BytesRead > 0) && (u16BytesRead <= UPGRADE_USB_RAM_BUF_SIZE) && (res == FR_OK)) {
            u32Addr = IMEU2_UPDATE_DATA_ADDR + u32TotalRead;
            SPIFlashWrite(u8Usb2RamBuf, u32Addr, u16BytesRead);
            u32TotalRead += u16BytesRead;
        } else {
            break;
        }
    }
    dword.v = u32TotalRead;
    u8WriteBuf[0] = dword.b[0];
    u8WriteBuf[1] = dword.b[1];
    u8WriteBuf[2] = dword.b[2];
    u8WriteBuf[3] = dword.b[3];
    SPIFlashWrite(u8WriteBuf, IMEU2_UPDATE_BASE_ADDR, 4U);
}

static UINT8 g_u8SendBuf[80] = {0};
void Imeu2Querystatus(UINT8 Addr)
{
    UINT16 u16Crc;

    g_u8SendBuf[0] = Addr;
    g_u8SendBuf[1] = 0x03U;
    g_u8SendBuf[2] = 0x06U;
    g_u8SendBuf[3] = 0xF0U;
    u16Crc = CalcCodeCRCFor485(g_u8SendBuf, 4U);
    g_u8SendBuf[4] = (UINT8)(u16Crc);
    g_u8SendBuf[5] = (UINT8)(u16Crc>>8);
    dv_Uart5.fSendDataTXY(g_u8SendBuf, 6U);
}

void Imeu2JumptoBoot(UINT8 Addr)
{
	UINT16 u16Crc;

    g_u8SendBuf[0] = Addr;
    g_u8SendBuf[1] = 0x06U;
    g_u8SendBuf[2] = 0x09U;
    g_u8SendBuf[3] = 0xF1U;
    g_u8SendBuf[4] = 0x01U;
    g_u8SendBuf[5] = 0x03U;
    g_u8SendBuf[6] = 0x03U;
    u16Crc = CalcCodeCRCFor485(g_u8SendBuf, 7U);
    g_u8SendBuf[7] = (UINT8)(u16Crc);
    g_u8SendBuf[8] = (UINT8)(u16Crc>>8);
    dv_Uart5.fSendDataTXY(g_u8SendBuf, 9U);
}

void Imeu2Handshake(Secondary_Upgrade_info_t *me)
{
    UINT16 u16Crc;

    g_u8SendBuf[0] = me->CurrDecviceAddr;
    g_u8SendBuf[1] = 0x06U;
    g_u8SendBuf[2] = 0x0dU;
    g_u8SendBuf[3] = 0xF1U;
    g_u8SendBuf[4] = 0x01U;
    g_u8SendBuf[5] = 0x05U;
    g_u8SendBuf[6] = 0x01U;
    g_u8SendBuf[7] = 0xFFU;
    g_u8SendBuf[8] = 0xFFU;
    g_u8SendBuf[9] = (UINT8)(me->TotalSectionNum);
    g_u8SendBuf[10] = (UINT8)(me->TotalSectionNum>>8);
    u16Crc = CalcCodeCRCFor485(g_u8SendBuf, 11U);
    g_u8SendBuf[11] = (UINT8)(u16Crc);
    g_u8SendBuf[12] = (UINT8)(u16Crc>>8);
    dv_Uart5.fSendDataTXY(g_u8SendBuf, 13U);
}

void Imeu2StartSection(Secondary_Upgrade_info_t *me)
{
    UINT16 u16Crc;
    UINT16 u16Data;
    UINT32 u32Addr;

    u32Addr = CFG_IMEU2_APP_ADDR + me->CurrSendSection * 1024U;
    u16Data = me->CurrSectionBytes;
    g_u8SendBuf[0] = me->CurrDecviceAddr;
    g_u8SendBuf[1] = 0x06U;
    g_u8SendBuf[2] = 0x0EU;
    g_u8SendBuf[3] = 0xF2U;
    g_u8SendBuf[4] = (UINT8)u32Addr;
    g_u8SendBuf[5] = (UINT8)(u32Addr >> 8U);
    g_u8SendBuf[6] = (UINT8)(u32Addr >> 16U);
    g_u8SendBuf[7] = (UINT8)(u32Addr >> 24U);
    g_u8SendBuf[8] = (UINT8)(u16Data);
    g_u8SendBuf[9] = (UINT8)(u16Data>>8);
    u16Data = me->CurrCRC;
    g_u8SendBuf[10] = (UINT8)(u16Data);
    g_u8SendBuf[11] = (UINT8)(u16Data>>8);
    u16Crc = CalcCodeCRCFor485(g_u8SendBuf, 12U);
    g_u8SendBuf[12] = (UINT8)(u16Crc);
    g_u8SendBuf[13] = (UINT8)(u16Crc>>8);
    dv_Uart5.fSendDataTXY(g_u8SendBuf, 14U);
}

void Imeu2TransmitMessage(Secondary_Upgrade_info_t *me, UINT8 *pu8Buf)
{
    UINT16 u16Crc;
    UINT8 RemainByte;
    UINT16 u16DataSeq;
    UINT8 i;

    g_u8SendBuf[0] = me->CurrDecviceAddr;
    g_u8SendBuf[1] = 0x06U;
    if (me->CurrSendStep < (me->TotalFramePerSection - 1U))
	{
		RemainByte = 64u;
	}
	else
	{
		RemainByte = me->CurrSectionBytes - me->CurrSectionSendBytes;
	}
    g_u8SendBuf[2] = 0x09U + RemainByte;
    g_u8SendBuf[3] = 0xF3U;
    u16DataSeq = me->CurrSendStep * 64U;
    g_u8SendBuf[4] = (UINT8)(u16DataSeq);
    g_u8SendBuf[5] = (UINT8)(u16DataSeq>>8);
    g_u8SendBuf[6] = RemainByte;
    for (i = 0; i < RemainByte; i++) {
        g_u8SendBuf[7 + i] = pu8Buf[i + (me->CurrSendStep * 64u)];
    }
    u16Crc = CalcCodeCRCFor485(g_u8SendBuf, (RemainByte + 7U));
    g_u8SendBuf[RemainByte + 7U] = (UINT8)(u16Crc);
    g_u8SendBuf[RemainByte + 8U] = (UINT8)(u16Crc>>8);
    dv_Uart5.fSendDataTXY(g_u8SendBuf, (9U + RemainByte));
    me->CurrSectionSendBytes += RemainByte;
    me->CurrSendStep++;
}

void Imeu2EndOneSection(Secondary_Upgrade_info_t *me)
{
    UINT16 u16Crc;

    g_u8SendBuf[0] = me->CurrDecviceAddr;
    g_u8SendBuf[1] = 0x06U;
    g_u8SendBuf[2] = 0x07U;
    g_u8SendBuf[3] = 0xF4U;
    if (me->CurrSendSection >= (me->TotalSectionNum - 1U))
	{
		g_u8SendBuf[4] = 2u;
	}
	else
	{
		g_u8SendBuf[4] = 1u;
	}
    u16Crc = CalcCodeCRCFor485(g_u8SendBuf, 5U);
    g_u8SendBuf[5] = (UINT8)(u16Crc);
    g_u8SendBuf[6] = (UINT8)(u16Crc>>8);
    dv_Uart5.fSendDataTXY(g_u8SendBuf, 7U);
}

void Imeu2Reboot(INT8U Addr)
{
    UINT16 u16Crc;

    g_u8SendBuf[0] = Addr;
    g_u8SendBuf[1] = 0x06U;
    g_u8SendBuf[2] = 0x08U;
    g_u8SendBuf[3] = 0xF1U;
    g_u8SendBuf[4] = 0x01U;
    g_u8SendBuf[5] = 0x02U;
    u16Crc = CalcCodeCRCFor485(g_u8SendBuf, 6U);
    g_u8SendBuf[6] = (UINT8)(u16Crc);
    g_u8SendBuf[7] = (UINT8)(u16Crc>>8);
    dv_Uart5.fSendDataTXY(g_u8SendBuf, 8U);
}
