#ifndef UPGRADE_H
#define UPGRADE_H

#include "main.h"
#include "EtherNet.h"
/* 说明
   共16Mbits(2M Bytes) 分32 Blocks 
   每个Block分16个扇区，每个扇区16Pages,每个Page 256bytes
   每Block: 有256页,有64Kbyes
   每sector: 有4Kbytes
    
   Block0: 0x00 0000 - 0x00 FFFF
   Block1: 0x01 0000 - 0x01 FFFF
   .
   .
   .
   Block31: 0x1F 0000 - 0x1F FFFF

   擦除最小单位为sector, 费时200ms. 擦除block要2秒，擦除chip要20秒。
	写数据时，使用02命令，page program
	支持一次写最少1个字节，最多256个字节。
*/
//升级标志
#define EX_FLASH_UPDATA_1    0xA6
#define EX_FLASH_UPDATA_2    0x6A

#define EX_FLASH_UPDATA_1_SIZEOVER512K    0xA9	//新版本BOOT，支持超过512K的APP升级，所以flash存储位置变了
#define EX_FLASH_UPDATA_2_SIZEOVER512K    0x9A

//最大支持512K bytes
#define EX_FLASH_INFO						(256*2)    //存放文件信息?
#define EX_UPGRADE_START_BLOCK_NO			23		
#define EX_UPGRADE_BLOCK_MAX_NUM			8
#define EX_PAGE_NUM_PER_BLOCK				256
#define EX_BYTES_NUM_PER_PAGE				256
#define EX_RECV_DATA_MAX_NUM				((EX_UPGRADE_BLOCK_MAX_NUM*(EX_PAGE_NUM_PER_BLOCK*EX_BYTES_NUM_PER_PAGE))-EX_FLASH_INFO)
#define EX_RECV_CACHE_MAX_NUM				(EX_PAGE_NUM_PER_BLOCK*EX_BYTES_NUM_PER_PAGE)
#define EX_RECV_ONE_CACHE_NUM				512

//BOOT V2 支持超过512K bytes
#define EX_UPGRADE_START_BLOCK_NO_OVER512K			19		
#define EX_UPGRADE_BLOCK_MAX_NUM_OVER512K			13
#define EX_RECV_DATA_MAX_NUM_OVER512K				((EX_UPGRADE_BLOCK_MAX_NUM_OVER512K*(EX_PAGE_NUM_PER_BLOCK*EX_BYTES_NUM_PER_PAGE))-EX_FLASH_INFO)

#define  UPGRADE_ONE_FRAME_LENGTH	1024		//单帧字节数



typedef struct _UpGradeCtrl
{
	UINT8 u8UpgradeEnable;  //0-禁止 1-使能
	UINT8 u8UpgradeStat;
	UINT8 u8UpgradeEndStat;  //充电结果
	UINT8 u8UpgradeNeedReset;	////升级文件下载完成,需要重启   0-不需要  1-需要
	UINT8 u8UpgradeWriteFlag;

	//UINT8 u8SoftVer[2];
	//UINT8 u8ProtocolVer[2];

	UINT8 u8UpdateFileType;


	UINT8 UpGradeData[UPGRADE_ONE_FRAME_LENGTH+1];

	char cRcvFileName[33];

	UINT16 RcvByteNum;
	UINT16 RcvFrameNo;

	UINT16 ByteNumPerFrame;

	UINT16 u16BinFileCRC;
	UINT16 u16CalRcvCRC;

	//UINT32 u32UpgradeReqIndex;
	UINT32 u32UpgradeWaitIndex;
	UINT32 u32TotalFrameNumber;

	UINT32 u32TotalRcvByteNum;		//总已接收字节个数
	UINT32 u32FileSize;
	float fProgress;
}UpGradeCtrl;
extern NOINIT UpGradeCtrl gs_UpGradeCtrl;

//gs_UpGradeCtrl.u8UpgradeStat
enum UpgradeStatDef
{
	UpgradeStatDef_Init = 0,  //
	UpgradeStatDef_Start,   //已启动
	UpgradeStatDef_CrcFail,   //已结束
	UpgradeStatDef_End,   //已结束	
};


//gs_UpGradeCtrl.u8UpdateFileType
enum UpdateFileTypeDef
{
	UpdateFileTypeDef_0 = 0,    //终端文件  IMSU-D
	UpdateFileTypeDef_1,		//RECT-PFC U1
	UpdateFileTypeDef_2,		//RECT-DCDC U2
	UpdateFileABUTypeDef_1,		//ABU-PFC U1
	UpdateFileABUTypeDef_2,		//ABU-DCDC U2
	UpdateFileEU2_1TypeDef,		//EU2 1
	UpdateFileEU2_2TypeDef,		//EU2 2
//by lxr01202 
	UpdateFileMPPT_1TypeDef = 9,  //MPPT 1
	UpdateFileMPPT_2TypeDef,  //MPPT 2
};

//传输命令结果
enum UpdateStartResultDef
{
	UpdateStartResult_DownLoaded = 0,  //
	UpdateStartResult_DownLoadedFailed, //
	UpdateStartResult_DownLoadeding, //
	UpdateStartResult_Idle, //
	UpdateStartResult_InstallFailed, //
	UpdateStartResult_Installing, //
	UpdateStartResult_Installed
};

//gs_UpGradeCtrl.u8UpgradeNeedReset
#define	UPGRADE_RESET_DELAY		31			//秒

void InitUpgrade(void);
void FLASH_UpgradeReadBytes(UINT8 *pData,UINT32 u32Addr, UINT32 u32Len);
void FLASH_EXTableReadBytes(UINT8 *pData,UINT32 u32Addr, UINT32 u32Len);
void FLASH_EXTableWriteBytes(UINT8 *pData,UINT32 u32Addr, UINT32 u32Len);
void FLASH_UpgradeWriteBytes(UINT8 *pData,UINT32 u32Addr, UINT32 u32Len);
void UpGradeEraseExFlash(void);
void UpgradeWriteFlash(void);

#endif
