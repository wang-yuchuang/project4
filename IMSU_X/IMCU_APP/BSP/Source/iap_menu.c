/**
  ******************************************************************************
  * @file    iap_menu.c 
  * @author  Zuo Zhenglai (derek)
  * @version V1.0.0
  * @date    15-Nov-2014
  * @brief   COMMAND IAP Execute Application
  ******************************************************************************

  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_usb.h"
#include "main.h"
#include "HistoryEvent.h"
#include "secondary_upgrade.h"
#include <ucos_ii.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEMO_INIT       ((uint8_t)0x00)
#define DEMO_IAP        ((uint8_t)0x01)
FATFS USBH_fatfs;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t UploadCondition = 0x00;
DIR dir;
FILINFO fno;
static uint8_t Demo_State = DEMO_INIT;

/* Private function prototypes -----------------------------------------------*/
extern uint8_t MSC_File_Operations(UCHAR* pBuff,UINT16 length);
/* Private functions ---------------------------------------------------------*/
 extern FIL MyFile11;

 extern void Clear_FSLock_FilBuf(void);
 extern void Clear_FatfsBuf(FATFS* fs);
void USB_Process(void)
{
  UCHAR temp;
  switch(Demo_State)
  {
  case DEMO_INIT:
    if(f_mount(&USBH_fatfs, "", 0 ) == FR_OK )
    {
		Demo_State = DEMO_IAP;
    }
    break;
     
    
  default:
    break;
  }
  if(Appli_state == APPLICATION_DISCONNECT)
  {
	   gs_HistAlmWriteToUdisc.u8HistAlmFileOpenFlag = 0;
	  gs_HistAlmWriteToUdisc.u8OpenHistAlmFileCmd = 0;
	  gs_HistAlmWriteToUdisc.u16CurrLogIdx = 0;
	  gs_HistAlmWriteToUdisc.u8ReadFromFlash_Flag = 0;
	  gs_HistAlmWriteToUdisc.u16CurrLogIdx_FromFlash = 0;

		g_u8ChgingLogFileOpenFlag = 0;
		f_close(&MyFile11);
		MyFile11.fsize = 0;
		Clear_FatfsBuf(&USBH_fatfs);
		#if _FS_LOCK
		Clear_FSLock_FilBuf();
		#endif

	OSTimeDly(100);
	if(gEvChargeInfo.u8RectUpgradeFlag == FALSE)
	{
		gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[0] = 1;
		gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[1] = 1;
		gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] = FALSE;
		gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] = FALSE;
		gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] = 0;
		gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] = 0;

		if(g_u8ChgingLogFileOpenFlag == 1)
		{
			g_u8ChgingLogFileOpenFlag = 0;
			/*f_close(&MyFile11);
			MyFile11.fsize = 0;

			#if _FS_LOCK
			Clear_FSLock_FilBuf();
			#endif*/
		}
	}

	//IMEU2 
	if((g_stImeu2UpgradeInfo[0].u8UpgradeFlag == FALSE) && (g_stImeu2UpgradeInfo[1].u8UpgradeFlag == FALSE))
	{
		g_stImeu2UpgradeInfo[0].u8_POWER_ON_Flag = 1;
	}

  }
}

void USB_Log_Process(UCHAR* pBuff)
{
  UINT16 length;
  UCHAR temp, u8Result, u8NeedWrite;
  FRESULT res;

  uint32_t bytesWritten;
  int i = 0;
  int j = 0;
  UNUSED(i);  UNUSED(j);
  UNUSED(bytesWritten);
	
  u8Result = 0;

  switch(Demo_State)
  {
 /* case DEMO_INIT:
    if(f_mount(&USBH_fatfs, "", 0 ) == FR_OK )
    {
		Demo_State = DEMO_IAP;
    }
    break;*/
    
  case DEMO_IAP:	 

    if(USBH_MSC_IsReady(&hUSBHost))
    {  
      if(Appli_state == APPLICATION_READY)
      {
		  if(g_u8ChgingLogFileOpenFlag == 1)
		  {
		  }
		  else if(gs_HistAlmWriteToUdisc.u8HistAlmFileOpenFlag == 1)
		  {
		  }		  
		  else 
		  {  
			   gs_HistAlmWriteToUdisc.u8OpenHistAlmFileCmd = 0;
			  gs_HistAlmWriteToUdisc.u16CurrLogIdx = 0;
			  //gs_HistAlmWriteToUdisc.u8ReadFromFlash_Flag = 0;
			  gs_HistAlmWriteToUdisc.u16CurrLogIdx_FromFlash = 0;

			   g_u8ChgingLogFileOpenFlag = 0;
			 
			   return;
			 // else
			  {
				  res = f_open(&MyFile11, "0:HistEvt.txt",FA_READ|FA_CREATE_ALWAYS|FA_WRITE);
				  if(res == FR_OK)
				  {
					  gs_HistAlmWriteToUdisc.u8HistAlmFileOpenFlag = 1;
					  f_lseek(&MyFile11,MyFile11.fptr+MyFile11.fsize);
				  }
				  else
				  {
					   u8Result = 2;		
					  dv_USB.u16USBFlag +=2;
					   //u8Result = 1;		
					  return;
				  }
				  
			  }
			}
	  
		  for(i = 0; i < 10; i++)
		  {
			   u8NeedWrite = 0;

			   //u8NeedWrite = u8CheckNeedRecord(&length);

			   if(u8NeedWrite == FALSE)
			   {
				   //u8NeedWrite = u8HistAlmWriteToUdisc(&length);	
			   }

			   if(gs_HistAlmWriteToUdisc.u8HistAlmFileOpenFlag == 1)
			   {
					u8NeedWrite = u8HistAlmWriteToUdisc(&length);	
					if((gs_HistAlmWriteToUdisc.u8ReadFromFlash_Flag == 1)
						&& (u8NeedWrite == 0))
					{
						u8NeedWrite = u8HistAlmWriteToUdisc_FromFlash(&length);	
					}
			   }
			   

		    if(u8NeedWrite)
			{
				gEvChargeInfo.u8USB_AppDogTimer = 0;
				length = MIN(DEBUG_BUF_BYTES_MAX_NUM, length);

				  if(length == 0)
				  {
					 dv_USB.u16USBFlag = 1;
				  }
				  else
				  {
					   
					   res= f_write (&MyFile11, pBuff, length, (void *)&bytesWritten);

					  /* f_close(&MyFile11);
					   OSTimeDly(10);*/
					 					   
						//MyFile11.fsize = 0;
						if((bytesWritten == 0) || (res != FR_OK)) 
						{
							u8Result = 2;
							 dv_USB.u16USBFlag +=2;
						 	 break;
						}
						else
						{
							res = f_sync(&MyFile11);

							u8Result = 1;
							 dv_USB.u16USBFlag = 1;
						   	 break;
						}
				  }
		   } //  if(u8CheckNeedRecord(&length))
		   else
		   {
			   break;
		   }
		  }// for(i = 0; i < 10; i++)
		  OSTimeDly(10);
	  } // if(Appli_state == APPLICATION_READY)
	}  // if(USBH_MSC_IsReady(&hUSBHost))
    else
    {
      dv_USB.u16USBFlag +=2;
    }
    break;
    
  default:
    break;
  }
 /* if(Appli_state == APPLICATION_DISCONNECT)
  {
	OSTimeDly(100);
  }*/
}

void USB_Rect_U1_Copy_Process(void)
{
	switch(Demo_State)
	{
	case DEMO_IAP:
		if(USBH_MSC_IsReady(&hUSBHost))
		{  
			extern void MSC_File_U1_Operations(void);
			if(Appli_state == APPLICATION_READY)
			{
				MSC_File_U1_Operations();
			}
		}
		break;
	default:break;
	}
}

void USB_Rect_U2_Copy_Process(void)
{
	switch(Demo_State)
	{
	case DEMO_IAP:
		if(USBH_MSC_IsReady(&hUSBHost))
		{  
			extern void MSC_File_U2_Operations(void);
			if(Appli_state == APPLICATION_READY)
			{
				MSC_File_U2_Operations();
			}
		}
		break;
	default:
          break;
	}
}

void USB_IMEU2_Copy_Process(void)
{
	switch(Demo_State)
	{
		case DEMO_IAP:
			if(USBH_MSC_IsReady(&hUSBHost))
			{
				if(Appli_state == APPLICATION_READY)
				{
					extern void MSC_File_IMEU2_Operations(UINT8 u8Addr);
					if(Appli_state == APPLICATION_READY)
					{
						MSC_File_IMEU2_Operations(0);
						MSC_File_IMEU2_Operations(1);
					}
				}
			}
			break;
		default:
			break;
	}
}

/************************ (C) COPYRIGHT INFY POWER  *****END OF FILE****/
