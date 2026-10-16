
#include "Upgrade.h"

NOINIT UpGradeCtrl gs_UpGradeCtrl;


void InitUpgrade(void)
{
	UCHAR i;

	gs_UpGradeCtrl.u8UpgradeStat = UpgradeStatDef_Init;
	gs_UpGradeCtrl.u8UpgradeEndStat = 0;
	gs_UpGradeCtrl.u8UpgradeEnable = 0;
	gs_UpGradeCtrl.u8UpgradeNeedReset = 0;
	gs_UpGradeCtrl.u8UpgradeWriteFlag = 0;
	gs_UpGradeCtrl.RcvByteNum = 0;

	gs_UpGradeCtrl.u32UpgradeWaitIndex = 0;
	gs_UpGradeCtrl.fProgress = 0;
	gs_UpGradeCtrl.u32TotalFrameNumber = 0;
	gs_UpGradeCtrl.u32FileSize = 0;
	gs_UpGradeCtrl.u16CalRcvCRC = 0;
	gs_UpGradeCtrl.u16BinFileCRC = 0;
	gs_UpGradeCtrl.u32TotalRcvByteNum = 0;
	gs_UpGradeCtrl.ByteNumPerFrame = 0;

	gs_UpGradeCtrl.RcvFrameNo = 0;
	
}

void FLASH_EXTableWriteBytes(UINT8 *pData,UINT32 u32Addr, UINT32 u32Len)
{
	UINT32 u32WriteAddr = 0;
	//MutexLock(gEvChargeInfo.hMetex_FLASH);
	if(gEvChargeInfo.u8BootVersion >= BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k)
	{
		u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO_OVER512K * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	}
	else
	{
		u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	}
	u32WriteAddr = u32Addr + u32WriteAddr;
	SPIFlashWrite(pData, u32WriteAddr, u32Len);
	//MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void FLASH_UpgradeWriteBytes(UINT8 *pData,UINT32 u32Addr, UINT32 u32Len)
{
	UINT32 u32WriteAddr = 0;
	//MutexLock(gEvChargeInfo.hMetex_FLASH);
	if(gEvChargeInfo.u8BootVersion >= BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k)
	{
		u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO_OVER512K * EX_PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + EX_FLASH_INFO;
	}
	else
	{
		u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO * EX_PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + EX_FLASH_INFO;
	}

	
	u32WriteAddr = u32Addr + u32WriteAddr;
	SPIFlashWrite(pData, u32WriteAddr, u32Len);
	//MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void FLASH_UpgradeReadBytes(UINT8 *pData,UINT32 u32Addr, UINT32 u32Len)
{
	UINT32 u32WriteAddr = 0;
	//MutexLock(gEvChargeInfo.hMetex_FLASH);
	if(gEvChargeInfo.u8BootVersion >= BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k)
	{
		u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO_OVER512K * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + EX_FLASH_INFO;
	}
	else
	{
		u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + EX_FLASH_INFO;
	}

	
	u32WriteAddr = u32Addr + u32WriteAddr;
	SPIFlashRead(pData, u32WriteAddr, u32Len);
	//MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void FLASH_EXTableReadBytes(UINT8 *pData,UINT32 u32Addr, UINT32 u32Len)
{
	UINT32 u32WriteAddr = 0;
	//MutexLock(gEvChargeInfo.hMetex_FLASH);
	if(gEvChargeInfo.u8BootVersion >= BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k)
	{
		u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO_OVER512K * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	}
	else
	{
		u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	}
	//u32WriteAddr = (EX_UPGRADE_START_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	u32WriteAddr = u32Addr + u32WriteAddr;
	SPIFlashRead(pData, u32WriteAddr, u32Len);
	//MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}


void UpGradeEraseExFlash(void)
{
	//½«FLASH (8Blocks) Ë¢Îª FF
	UINT8 u8BlockNo = 0;
	UINT32 Addres = 0;
	UINT8 u8MaxBlockNum,u8BaseBlockNo;

	if(gEvChargeInfo.u8BootVersion >= BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k)
	{
		u8MaxBlockNum = EX_UPGRADE_BLOCK_MAX_NUM_OVER512K;
		u8BaseBlockNo = EX_UPGRADE_START_BLOCK_NO_OVER512K;
	}
	else
	{
		u8MaxBlockNum = EX_UPGRADE_BLOCK_MAX_NUM;
		u8BaseBlockNo = EX_UPGRADE_START_BLOCK_NO;
	}

	for (u8BlockNo = 0; u8BlockNo < u8MaxBlockNum; u8BlockNo++)
	{
		Addres = ((u8BlockNo + u8BaseBlockNo) * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE) + 1;
		
		SPIFlashEraseBlock(Addres);
		OSTimeDly(10);
	}
}
void UpgradeWriteFlash(void)
{
	UINT32 Addres;

	Addres = (gs_UpGradeCtrl.u32UpgradeWaitIndex-1) * UPGRADE_ONE_FRAME_LENGTH;

	FLASH_UpgradeWriteBytes(gs_UpGradeCtrl.UpGradeData, Addres, gs_UpGradeCtrl.RcvByteNum);

}
