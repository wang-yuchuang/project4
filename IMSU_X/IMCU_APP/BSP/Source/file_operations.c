/**
  ******************************************************************************
  * @file    file_operations.c 
  * @author  Zuo Zhenglai (derek)
  * @version V1.0.0
  * @date    15-Nov-2014
  * @brief   Write/read file on the disk.
  ******************************************************************************
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_usb.h"
#include "main.h"
#include "secondary_upgrade.h"
#include "imeu2_upgrade.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
FIL MyFile11;
FRESULT res;
uint32_t bytesWritten;
uint8_t wtext[] = "Record: RequireVolt RequireCurr SOC  Curr time CellMaxVolt CellMinVolt\r\n";
uint8_t MSC_File_Operations(UCHAR* pBuff,UINT16 length);
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
NOINIT UINT8 u8_usb_Buf[258];
unsigned char g_u8ChgingLogFileOpenFlag = 0;
uint8_t MSC_File_Operations(UCHAR* pBuff,UINT16 length)
{
  uint16_t bytesread = 0;
  int i = 0;
  int j = 0;
  UNUSED(i);  UNUSED(j);
  UNUSED(bytesread);

  if((g_u8ChgingLogFileOpenFlag == 0)
	 && (f_open(&MyFile11, "0:Log.txt",FA_READ|FA_OPEN_ALWAYS|FA_WRITE) != FR_OK))
  {
	  g_u8ChgingLogFileOpenFlag = 0;
	  return 3;
  }
  else
  {
	 g_u8ChgingLogFileOpenFlag = 1;

	f_lseek(&MyFile11,MyFile11.fptr+MyFile11.fsize);
    res= f_write (&MyFile11, pBuff, length, (void *)&bytesWritten); 
    //f_close(&MyFile11);
    //MyFile11.fsize = 0;
    if((bytesWritten == 0) || (res != FR_OK)) 
    {
      return 2;
    }
     else
    {
       return 1;
    }
  }  
}

void MSC_File_U1_Operations(void)
{
	unsigned char u8FindFlag, u8PowerModuleTypeNo;
	int length;
	uint16_t bytesread = 0;
	int iTempValue= 0;
	int iRemainder = 0;
	int iBytesNum = 0;
	int iReadTimes = 0;
	int i = 0;
	int j = 0;UNUSED(j);
	__XXX_UNION_VALUE unTempVal;
	//static UINT8 s_u8_POWER_ON_Flag = 1;
    if(gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[0] == 0)
    {
		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] == 2)
		{
			  if(f_open(&MyFile11, "0:U1Result.txt",FA_READ|FA_OPEN_ALWAYS|FA_WRITE) != FR_OK) 
			  {
#ifdef	Debug_Print_En
			UINT8 u8Idx = 0;
			memset(u8gTempDebugBuf_System, 0, DEBUG_BUF_BYTES_MAX_NUM);
			u8Idx += sprintf((char*)u8gTempDebugBuf_System,"Open U1Result.txt Fail");
			WriteDebugInfo(u8gTempDebugBuf_System, u8Idx);
#endif
			  }
			  else
			  {
#ifdef	Debug_Print_En
			UINT8 u8Idx = 0;
			memset(u8gTempDebugBuf_System, 0, DEBUG_BUF_BYTES_MAX_NUM);
			u8Idx += sprintf((char*)u8gTempDebugBuf_System,"Open U1Result.txt Success");
			WriteDebugInfo(u8gTempDebugBuf_System, u8Idx);
#endif
				  memset(u8_usb_Buf, 0, BYTES_NUM_PER_PAGE);
				  length = sprintf((char *)u8_usb_Buf, "%s Success number = %d\r\n",  gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade, gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0]);
				  f_lseek(&MyFile11,MyFile11.fptr+MyFile11.fsize);
				  res= f_write (&MyFile11, u8_usb_Buf, length, (void *)&bytesWritten); 
				  f_close(&MyFile11);
				  MyFile11.fsize = 0;
			  }
				OSTimeDly(10);
			  gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] = 0;
		}
        return;
    }
	UNUSED(bytesread);

	u8FindFlag = 0;
	for(u8PowerModuleTypeNo = 0; u8PowerModuleTypeNo < gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum; u8PowerModuleTypeNo++)
	{
		if(f_open(&MyFile11, (const char *)gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[u8PowerModuleTypeNo],FA_READ|FA_OPEN_EXISTING) == FR_OK) 
		{
			u8FindFlag = 1;
			break;
		}
	}
	if(u8FindFlag == 0)
	{
		if(f_open(&MyFile11, "0:FORCEU1.bin", FA_READ|FA_OPEN_EXISTING) == FR_OK) 
		{
			u8FindFlag = 2;
		}		
	}

	if(u8FindFlag == 0)
	{
		return;
	}

	/*if(f_open(&MyFile11, "0:U1.bin",FA_READ|FA_OPEN_EXISTING) != FR_OK) 
	{
		return;
	}
	else*/	
	{
		if(gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[0] == 1)
		{
			FlashClean_U1_Data();
			unTempVal.u32Value = MyFile11.fsize;
			iRemainder = (MyFile11.fsize%BYTES_NUM_PER_PAGE);
			iTempValue = (MyFile11.fsize - (iRemainder));
			iReadTimes = iTempValue/BYTES_NUM_PER_PAGE;
			if(iRemainder > 0)
			{
				iReadTimes = iReadTimes + 1;
			}

			for(i = 0; (i < iReadTimes); i++)
			{
				f_lseek(&MyFile11, MyFile11.fptr);
				memset(u8_usb_Buf, 0, BYTES_NUM_PER_PAGE);
				res = f_read (&MyFile11, u8_usb_Buf, BYTES_NUM_PER_PAGE, (void *)&bytesWritten); 
				FlashWrite_U1_Data(u8_usb_Buf, iBytesNum, bytesWritten);
				iBytesNum += bytesWritten;
				if(iBytesNum >= MyFile11.fsize)
				{
					break;
				}
			}
		
			if(MyFile11.fsize > 1000)
			{				
				if(u8FindFlag == 2)
				{
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_IS_FORCE_UPDATE_FLAG;

					gEvChargeInfo.u16_U1_Bytes = MyFile11.fsize;
				}
				else
				{
					dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, RECT_IS_UPDATE_FLAG);
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_IS_UPDATE_FLAG;
					ExSetSigInfo(E_TYPE_SYS, ID_U1_BYTES_NUM, unTempVal,NULL, BY_E2PROM);
					memcpy(&gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade[0], &gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[u8PowerModuleTypeNo][2], 7);	
					gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade[7] = 0;
					gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[0] = 1;
				}
				gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] = 1;
			}  
			gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[0] = 0;
		}
		

		f_close(&MyFile11);
		OSTimeDly(5);
		MyFile11.fsize = 0;
		if((bytesWritten == 0) || (res != FR_OK)) 
		{
			return;
		}
		else
		{
			 return;
		}
	}  
}

void MSC_File_U2_Operations(void)
{
	unsigned char u8FindFlag,u8PowerModuleTypeNo;
        UINT16 length;
	uint16_t bytesread = 0;
	int iTempValue= 0;
	int iRemainder = 0;
	int iBytesNum = 0;
	int iReadTimes = 0;
	__XXX_UNION_VALUE unTempVal;
	int i = 0;
	int j = 0;UNUSED(j);
	//static UINT8 s_u8_POWER_ON_Flag = 1;
    if(gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[1] == 0)
    {
		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] == 2)
		{
			  if(f_open(&MyFile11, "0:U2Result.txt",FA_READ|FA_OPEN_ALWAYS|FA_WRITE) != FR_OK) 
			  {
#ifdef	Debug_Print_En
			UINT8 u8Idx = 0;
			memset(u8gTempDebugBuf_System, 0, DEBUG_BUF_BYTES_MAX_NUM);
			u8Idx += sprintf((char*)u8gTempDebugBuf_System,"Open U2Result.txt Fail");
			WriteDebugInfo(u8gTempDebugBuf_System, u8Idx);
#endif
			  }
			  else
			  {
#ifdef	Debug_Print_En
			UINT8 u8Idx = 0;
			memset(u8gTempDebugBuf_System, 0, DEBUG_BUF_BYTES_MAX_NUM);
			u8Idx += sprintf((char*)u8gTempDebugBuf_System,"Open U2Result.txt Success");
			WriteDebugInfo(u8gTempDebugBuf_System, u8Idx);
#endif
				  memset(u8_usb_Buf, 0, BYTES_NUM_PER_PAGE);
				  length = sprintf((char *)u8_usb_Buf, "%s Success number = %d\r\n", gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade, gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1]);
				  f_lseek(&MyFile11,MyFile11.fptr+MyFile11.fsize);
				  res= f_write (&MyFile11, u8_usb_Buf, length, (void *)&bytesWritten); 
				  f_close(&MyFile11);
				  MyFile11.fsize = 0;
			  }
			OSTimeDly(10);
			  gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] = 0;
		}
        return;
    }
	UNUSED(bytesread);
	
	u8FindFlag = 0;
	for(u8PowerModuleTypeNo = 0; u8PowerModuleTypeNo < gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum; u8PowerModuleTypeNo++)
	{
		if(f_open(&MyFile11, (const char *)gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[u8PowerModuleTypeNo],FA_READ|FA_OPEN_EXISTING) == FR_OK) 
		{
			u8FindFlag = 1;
			break;
		}
	}

	if(u8FindFlag == 0)
	{
		if(f_open(&MyFile11, "0:FORCEU2.bin", FA_READ|FA_OPEN_EXISTING) == FR_OK) 
		{
			u8FindFlag = 2;
		}		
	}

	if(u8FindFlag == 0)
	{
		return;
	}

	/*if(f_open(&MyFile11, "0:U2.bin",FA_READ|FA_OPEN_EXISTING) != FR_OK) 
	{
		return;
	}
	else*/	
	{
		if(gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[1] == 1)
		{
			FlashClean_U2_Data();
			unTempVal.u32Value = MyFile11.fsize;
			iRemainder = (MyFile11.fsize%BYTES_NUM_PER_PAGE);
			iTempValue = (MyFile11.fsize - (iRemainder));
			iReadTimes = iTempValue/BYTES_NUM_PER_PAGE;
			if(iRemainder > 0)
			{
				iReadTimes = iReadTimes + 1;
			}
			for(i = 0; (i < iReadTimes); i++)
			{
				f_lseek(&MyFile11, MyFile11.fptr);
				memset(u8_usb_Buf, 0, BYTES_NUM_PER_PAGE);
				res = f_read (&MyFile11, u8_usb_Buf, BYTES_NUM_PER_PAGE, (void *)&bytesWritten); 
				FlashWrite_U2_Data(u8_usb_Buf, iBytesNum, bytesWritten);
				iBytesNum += bytesWritten;
				if(iBytesNum >= MyFile11.fsize)
				{
					break;
				}
			}
			if(MyFile11.fsize > 1000)
			{				
				if(u8FindFlag == 2)
				{
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_IS_FORCE_UPDATE_FLAG;

					//ExSetSigInfo(E_TYPE_SYS, ID_U2_BYTES_NUM, unTempVal,NULL, BY_E2PROM);
					gEvChargeInfo.u16_U2_Bytes = MyFile11.fsize;
				}
				else
				{
					dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, RECT_IS_UPDATE_FLAG);

					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_IS_UPDATE_FLAG;

					ExSetSigInfo(E_TYPE_SYS, ID_U2_BYTES_NUM, unTempVal,NULL, BY_E2PROM);

					memcpy(&gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade[0], &gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[u8PowerModuleTypeNo][2], 7);

					gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade[7] = 0;
					gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[1] = 1;
				}
				
				gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] = 1;
			}
			gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[1] = 0;
		}
		f_close(&MyFile11);
		OSTimeDly(5);
		MyFile11.fsize = 0;
		if((bytesWritten == 0) || (res != FR_OK)) 
		{
			return;
		}
		else
		{
			return;
		}
	}  
}

void MSC_File_IMEU2_Operations(UINT8 u8Addr)
{
	unsigned char u8FindFlag;
	int length;
	uint16_t bytesread = 0;
	int iTempValue= 0;
	int iRemainder = 0;
	int iBytesNum = 0;
	int iReadTimes = 0;
	int i = 0;
	int j = 0;UNUSED(j);
	__XXX_UNION_VALUE unTempVal;

	if((u8Addr != 0) && (u8Addr != 1))
	{
		return;
	}

	if(g_stImeu2UpgradeInfo[0].u8_POWER_ON_Flag == 0)
	{
		if(g_stImeu2UpgradeInfo[u8Addr].u8RectUpdateSuccessWriteUDisk_Flag == 2)
		{
				if(f_open(&MyFile11, "0:IMEU2Result.txt",FA_READ|FA_OPEN_ALWAYS|FA_WRITE) != FR_OK) 
				{
#ifdef	Debug_Print_En
					UINT8 u8Idx = 0;
					memset(u8gTempDebugBuf_System, 0, DEBUG_BUF_BYTES_MAX_NUM);
					u8Idx += sprintf((char*)u8gTempDebugBuf_System,"Open IMEU2Result.txt Fail");
					WriteDebugInfo(u8gTempDebugBuf_System, u8Idx);
#endif
				}
				else
				{
#ifdef	Debug_Print_En
					UINT8 u8Idx = 0;
					memset(u8gTempDebugBuf_System, 0, DEBUG_BUF_BYTES_MAX_NUM);
					u8Idx += sprintf((char*)u8gTempDebugBuf_System,"Open IMEU2Result.txt Success");
					WriteDebugInfo(u8gTempDebugBuf_System, u8Idx);
#endif
					memset(u8_usb_Buf, 0, BYTES_NUM_PER_PAGE);
					length = sprintf((char*)u8_usb_Buf, "IMEU2 Addr = %d finish upgrade\r\n",  u8Addr);
					f_lseek(&MyFile11,MyFile11.fptr+MyFile11.fsize);
					res= f_write (&MyFile11, u8_usb_Buf, length, (void *)&bytesWritten); 
					f_close(&MyFile11);
					MyFile11.fsize = 0;
				}
				OSTimeDly(10);
				g_stImeu2UpgradeInfo[u8Addr].u8RectUpdateSuccessWriteUDisk_Flag = 0;
		}
		return;
	}
	UNUSED(bytesread);

	u8FindFlag = 0;
	
	if(f_open(&MyFile11, "0:IMEU2.bin", FA_READ|FA_OPEN_EXISTING) == FR_OK) 
	{
		u8FindFlag = 1;
	}

	if(u8FindFlag == 0)
	{
		if(f_open(&MyFile11, "0:FORCEU1.bin", FA_READ|FA_OPEN_EXISTING) == FR_OK) 
		{
			u8FindFlag = 2;
		}		
	}

	if(u8FindFlag == 0)
	{
		return;
	}

	if(g_stImeu2UpgradeInfo[0].u8_POWER_ON_Flag == 1)
	{
		FlashClean_IMEU2_Data();
		unTempVal.u32Value = MyFile11.fsize;
		iRemainder = (MyFile11.fsize%BYTES_NUM_PER_PAGE);
		iTempValue = (MyFile11.fsize - (iRemainder));
		iReadTimes = iTempValue/BYTES_NUM_PER_PAGE;
		if(iRemainder > 0)
		{
			iReadTimes = iReadTimes + 1;
		}

		for(i = 0; (i < iReadTimes); i++)
		{
			f_lseek(&MyFile11, MyFile11.fptr);
			memset(u8_usb_Buf, 0, BYTES_NUM_PER_PAGE);
			res = f_read (&MyFile11, u8_usb_Buf, BYTES_NUM_PER_PAGE, (void *)&bytesWritten); 
			FlashWrite_IMEU2_Data(u8_usb_Buf, iBytesNum, bytesWritten);
			iBytesNum += bytesWritten;
			if(iBytesNum >= MyFile11.fsize)
			{
				break;
			}
		}
		unTempVal.u32Value = iBytesNum;
		memset(u8_usb_Buf, 0, BYTES_NUM_PER_PAGE);
		u8_usb_Buf[0] = unTempVal.chtemp[0];
		u8_usb_Buf[1] = unTempVal.chtemp[1];
		u8_usb_Buf[2] = unTempVal.chtemp[2];
		u8_usb_Buf[3] = unTempVal.chtemp[3];
		FlashWrite_IMEU2_DataLen(u8_usb_Buf, 0, 4);
	
		if(MyFile11.fsize > 1000)
		{				
			if(u8FindFlag == 2)
			{
				if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 1) 
				{
					g_stImeu2UpgradeInfo[0].u8UpgradeFlag = TRUE;
				}

				if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist == 1) 
				{
					g_stImeu2UpgradeInfo[1].u8UpgradeFlag = TRUE;
				}
			}
			else
			{
				if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 1) 
				{
					g_stImeu2UpgradeInfo[0].u8UpgradeFlag = TRUE;
				}

				if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist == 1) 
				{
					g_stImeu2UpgradeInfo[1].u8UpgradeFlag = TRUE;
				}
			}
			g_stImeu2UpgradeInfo[0].u8RectUpdateSuccessWriteUDisk_Flag = 1;
			g_stImeu2UpgradeInfo[1].u8RectUpdateSuccessWriteUDisk_Flag = 1;
		}  
		g_stImeu2UpgradeInfo[0].u8_POWER_ON_Flag = 0;
	}
	

	f_close(&MyFile11);
	OSTimeDly(5);
	MyFile11.fsize = 0;
	if((bytesWritten == 0) || (res != FR_OK)) 
	{
		return;
	}
	else
	{
		return;
	}
	
}

/************************ (C) COPYRIGHT INFY POWER  *****END OF FILE****/
