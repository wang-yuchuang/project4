#include "main.h"
#include "can_ring.h"
#include "Uart6_new.h"
#include "bsp_usb.h"
#include "HistoryEvent.h"

NOINIT UCHAR  URecrodBuff[DEBUG_BUF_BYTES_MAX_NUM];


/***********************************************
函数名称:USBProcess   			          
函数功能:              								       
入口参数: 									                 
出口参数: 									                 
资源:           					          
子函数:	         								             
更改日期:  						          
***********************************************/
//static void USBProcess(UCHAR* pBuff,UINT16 length)
//{
//	USB_Log_Process(pBuff, length);
//	OSTimeDly(2);
//	/*USB_Rect_U1_Copy_Process();
//	OSTimeDly(2);
//	USB_Rect_U2_Copy_Process();
//	OSTimeDly(2);*/
//}

static void USBProcess_RectUpgrade(void)
{	
	USB_Rect_U1_Copy_Process();
	OSTimeDly(2);
	USB_Rect_U2_Copy_Process();
	OSTimeDly(2);
}

static void USBProcess_IMEU2Upgrade(void)
{
	USB_IMEU2_Copy_Process();
	OSTimeDly(2);
}

static void USB_H_Process(void)
{
	BSP_USBH_Process();
}

static void USBInit(void)
{
	vUsbInit();
}
dv_USB_TypeDef dv_USB =
{
	1,
	USBInit,
	//USBProcess, 
	USB_H_Process,
};



#define XXXX_BUF_ARRY_MAX_NUM  200  //180
static NOINIT UINT8 u8_I_DataArry[XXXX_BUF_ARRY_MAX_NUM];

NOINIT float siLastVolt_A;
NOINIT float siLastVolt_B;
NOINIT float siLastCurr_A;
NOINIT float siLastCurr_B;
NOINIT float siLastSOC_A;
NOINIT float siLastSOC_B;
NOINIT float siLastBCPMaxCurr_A;
NOINIT float siLastBCPMaxCurr_B;
NOINIT float siLastEVSE_MaxCurr_A;
NOINIT float siLastEVSE_MaxCurr_B;

NOINIT float siLastReqVolt_A;
NOINIT float siLastReqVolt_B;
NOINIT float siLastReqCurr_A;
NOINIT float siLastReqCurr_B;
NOINIT float siLastCtrlVolt_A;
NOINIT float siLastCtrlVolt_B;
NOINIT float siLastCtrlCurr_A;
NOINIT float siLastCtrlCurr_B;

NOINIT float siLastEVSE_MaxPowerLmt_A;
NOINIT float siLastEVSE_MaxPowerLmt_B;
//const char*pV = " Volt=";
//const	char*pC= " Curr=";
//const	char*pS= " SOC=";
//const	char*pSEMaxA= " SE_MaxA=";
//const	char*pEVMaxA= " EV_MaxA=";
//const	char*pEVreqV= " EV_ReqV=";
//const	char*pEVreqA= " EV_ReqA=";
//const	char*pSECtrlV= " SE_CtrlV=";
//const	char*pSECtrlA= " SE_CtrlA=";

void AddLogSosVoltCurr_x(unsigned char u8GunNo)
{
	//UINT8 u8Idx;
	//UINT8* p = NULL;

	//CAN_RING_DATA*	pRing;
	//ChargerContext* ctx;
	//stBMS_INFO* pBmsInfo;

	//u8Idx = 0;
	//
	//if(u8GunNo == GUN_NO_B)
	//{
	//	pRing = g_pRing_B;
	//	ctx = &CAN2_ChargerCtx;
	//	pBmsInfo = &gEVBms_2_Info;

	//	p = "#B T=";
	//}
	//else
	//{
	//	pRing = g_pRing_A;
	//	ctx = &CAN1_ChargerCtx;
	//	pBmsInfo = &gEVBms_1_Info;

	//	p = "#A T=";
	//}
	//
	//memset(u8_I_DataArry, 0, XXXX_BUF_ARRY_MAX_NUM);
	//u8Idx += sprintf((char*)u8_I_DataArry,"%s",p);
	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%d",u64Time);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%d",0);
	//
	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pV);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," Volt=%d",(int)gEvChargeInfo.fEV_BattVolt_x[u8GunNo]);
	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pC);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," Curr=%d",(int)pRing->master.fTotalOutCurr);
	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pS);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," SOC=%d",(UINT8)(pBmsInfo->fBCS_SOC));				
	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pSEMaxA);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," SE_MaxA=%d",(int)gEvChargeInfo.f_PLC_MaxCurr[u8GunNo]);

	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," SE_PwrKW=%d",(int)gEvChargeInfo.fMaxPOWER_SEND[u8GunNo]);

	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pEVMaxA);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," EV_MaxA=%d",(int)pBmsInfo->fBCP_MaxAllowCurr);
	//

	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pEVreqV);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," EV_ReqV=%d",(int)ctx->SECC_TARGET_VOLTATE_CURRENT.TARGET_VOLTAGE/10);
	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pEVreqA);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," EV_ReqA=%d",(int)ctx->SECC_TARGET_VOLTATE_CURRENT.TARGET_CURRENT/10);
	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pSECtrlV);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," SE_CtrlV=%d",(int)gs_GunCtrlReq[u8GunNo].GunVoltReq);
	////u8Idx += sprintf((char*)u8_I_DataArry+u8Idx,"%s",pSECtrlA);
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," SE_CtrlA=%d",(int)gs_GunCtrlReq[u8GunNo].GunCurrReq);

	//

	//UINT8 uctemp, i;
	//uctemp = 0;
	//for(i = 0; i < pRing->master.nSlaveTotal; i++)
	//{
	//	//if((tmNow - pRing_Rough->master.slaves[i]->tm_LastMergeTime) < SLAVE_MERGE_OUTPUT_DELAY)
	//	if((pRing->master.slaves[i]->CurrentMergeDelayFlag == 1)
	//		|| (pRing->master.slaves[i]->CtrlQuitRing == TRUE))
	//	{
	//		uctemp = 1;
	//		break;
	//	}
	//}
	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," Merge=%d",uctemp);

	//u8Idx += sprintf((char*)u8_I_DataArry+u8Idx," AC/DCPwr=%d/%d",(int)gs_IMMU2_LCCU_Mng.fMaxACInputPower[u8GunNo], (int)gs_IMMU2_LCCU_Mng.fMaxDCInputPower[u8GunNo]);

	//WriteDebugInfo(u8_I_DataArry, u8Idx);
}

void AddLogSosVoltCurr(void);
void AddLogSosVoltCurr(void)
{
//extern NOINIT ChargerContext CAN1_ChargerCtx;
//extern NOINIT ChargerContext CAN2_ChargerCtx;
//
//	//UINT32 u64Time = gRTCData.fGetSecondsGMT2LOCAL();
//
//	
//	
//	
//	UINT8 u8Rst_1 = 0;
//	float ftemp = 0;
//
//#ifdef	Debug_Print_En
//	if(g_u8AfterAddDecSlavePrintCurrrentChangeDelay > 0)
//	{
//		if(gEvChargeInfo.u16USBInterValTimer >= 500)
//		{
//			gEvChargeInfo.u16USBInterValTimer = 0;
//			if(u8IsGun1_Charging())
//			{
//				AddLogSosVoltCurr_x(GUN_NO_A);
//			}
//			if(u8IsGun2_Charging())
//			{
//				AddLogSosVoltCurr_x(GUN_NO_B);
//			}
//		}		
//	}
//	else if(gEvChargeInfo.u16USBInterValTimer >= 5000)
//	{
//#else
//	if(gEvChargeInfo.u16USBInterValTimer >= 5000)
//	{
//#endif
//		gEvChargeInfo.u16USBInterValTimer = 0;
//		if(u8IsGun1_Charging())
//		{
//			//u8Rst_1 = 1;
//			ftemp = g_pRing_A->master.fTotalOutCurr;			
//
//			if(ABS(siLastCurr_A-ftemp) > 3)
//			{
//				siLastCurr_A = ftemp;
//				u8Rst_1 = 1;
//			}
//			if(ABS(gEvChargeInfo.fEV_BattVolt_x[GUN_NO_A] - siLastVolt_A) > 5)
//			{
//				siLastVolt_A = gEvChargeInfo.fEV_BattVolt_x[GUN_NO_A];
//				u8Rst_1 = 1;
//			}
//
//			/*if(ABS(gEVBms_1_Info.fBCS_SOC - siLastSOC_A) > 1.2)
//			{
//				siLastSOC_A = gEVBms_1_Info.fBCS_SOC;
//				u8Rst_1 = 1;
//			}
//
//			if(ABS(gEVBms_1_Info.fBCP_MaxAllowCurr - siLastBCPMaxCurr_A) > 2)
//			{
//				siLastBCPMaxCurr_A = gEVBms_1_Info.fBCP_MaxAllowCurr;
//				u8Rst_1 = 1;
//			}*/
//
//			if(ABS(gEvChargeInfo.f_PLC_MaxCurr[GUN_NO_A] - siLastEVSE_MaxCurr_A) > 2)
//			{
//				siLastEVSE_MaxCurr_A = gEvChargeInfo.f_PLC_MaxCurr[GUN_NO_A];
//				u8Rst_1 = 1;
//			}
//
//			if(ABS(gs_GunCtrlReq[GUN_NO_A].GunCurrReq - siLastCtrlCurr_A) > 2)
//			{
//				siLastCtrlCurr_A = gs_GunCtrlReq[GUN_NO_A].GunCurrReq;
//				u8Rst_1 = 1;
//			}   
//
//			if(ABS(gEvChargeInfo.fMaxPOWER_SEND[GUN_NO_A] - siLastEVSE_MaxPowerLmt_A) > 1)
//			{
//				siLastEVSE_MaxPowerLmt_A = gEvChargeInfo.fMaxPOWER_SEND[GUN_NO_A];
//				u8Rst_1 = 1;
//			}   
//
//			if(u8Rst_1)
//			{
//				AddLogSosVoltCurr_x(GUN_NO_A);
//			}
//		}
//
//		if(u8IsGun2_Charging())
//		{
//			//u8Rst_1 = 1;
//			ftemp = g_pRing_B->master.fTotalOutCurr;
//			
//			if(ABS(siLastCurr_B-ftemp) > 3)
//			{
//				siLastCurr_B = ftemp;
//				u8Rst_1 = 1;
//			}
//			if(ABS(gEvChargeInfo.fEV_BattVolt_x[GUN_NO_B] - siLastVolt_B) > 5)
//			{
//				siLastVolt_B = gEvChargeInfo.fEV_BattVolt_x[GUN_NO_B];
//				u8Rst_1 = 1;
//			}
//
//			/*if(ABS(gEVBms_2_Info.fBCS_SOC - siLastSOC_B) > 1.2)
//			{
//				siLastSOC_B = gEVBms_2_Info.fBCS_SOC;
//				u8Rst_1 = 1;
//			}*/
//
//			if(ABS(gEvChargeInfo.f_PLC_MaxCurr[GUN_NO_B] - siLastEVSE_MaxCurr_B) > 2)
//			{
//				siLastEVSE_MaxCurr_B = gEvChargeInfo.f_PLC_MaxCurr[GUN_NO_B];
//				u8Rst_1 = 1;
//			}
//
//			/*if(ABS(gEVBms_2_Info.fBCP_MaxAllowCurr - siLastBCPMaxCurr_B) > 2)
//			{
//				siLastBCPMaxCurr_B = gEVBms_2_Info.fBCP_MaxAllowCurr;
//				u8Rst_1 = 1;
//			}*/
//
//			if(ABS(gs_GunCtrlReq[GUN_NO_B].GunCurrReq - siLastCtrlCurr_B) > 2)
//			{
//				siLastCtrlCurr_B = gs_GunCtrlReq[GUN_NO_B].GunCurrReq;
//				u8Rst_1 = 1;
//			}   
//
//			if(ABS(gEvChargeInfo.fMaxPOWER_SEND[GUN_NO_B] - siLastEVSE_MaxPowerLmt_B) > 1)
//			{
//				siLastEVSE_MaxPowerLmt_B = gEvChargeInfo.fMaxPOWER_SEND[GUN_NO_B];
//				u8Rst_1 = 1;
//			}   
//			
//			if(u8Rst_1)
//			{
//				AddLogSosVoltCurr_x(GUN_NO_B);
//			}
//		}
//	}
}


/************************************************/
//函数名称:										
//函数功能:										
//更改日期:      								
/************************************************/
extern void MX_FATFS_Init(void);
void Task_USB_Proc(void* pdata)
{
	dv_USB.u16USBFlag = 0;
	gEvChargeInfo.u8USB_AppDogTimer = 0;
	gSelfTest.fSetTaskID(TASK_ID_RING);
	gSelfTest.fSetTaskID(TASK_ID_RING);

	gSelfTest.fSetTaskID(TASK_ID_RING);
	
	dv_USB.fUSBInit();
	MX_FATFS_Init();
	OSTimeDly(100);
//	UINT16 uLen = 0;
//	UINT8* p = 0;
//	UINT16 u8Idx= 0;
 //   UNUSED(u8Idx);
 //   UNUSED(p);
	
			/*p = "B EV normal stop";
			memset(u8gTempDebugBuf_System, 0, DEBUG_BUF_BYTES_MAX_NUM);
			u8Idx += sprintf((char*)u8gTempDebugBuf_System,"%s",p);
			WriteDebugInfo(u8gTempDebugBuf_System, u8Idx);*/
	for(;;)
	{
		
		//OSTimeDly(2);
		gSelfTest.fSetTaskID(TASK_ID_RING);
		if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
		{
			OSTimeDly(50);
			continue;
		}
		
		dv_USB.fUSB_H_Process();		
		gSelfTest.fSetTaskID(TASK_ID_RING);
		OSTimeDly(2);

		USB_Process();
		gSelfTest.fSetTaskID(TASK_ID_RING);	

		//AddLogSosVoltCurr();
		
		USB_Log_Process(URecrodBuff);
		OSTimeDly(2);

		if(dv_USB.u16USBFlag >= 3000 || gEvChargeInfo.u8USB_AppDogTimer> 250)
		{
			dv_USB.fUSBInit();
			OSTimeDly(50);
			dv_USB.u16USBFlag = 0;
			gEvChargeInfo.u8USB_AppDogTimer = 0;
		}

		gSelfTest.fSetTaskID(TASK_ID_RING);
		
		if(g_u8ChgingLogFileOpenFlag == 0  && gs_HistAlmWriteToUdisc.u8HistAlmFileOpenFlag == 0)
		{
			USBProcess_RectUpgrade();

			USBProcess_IMEU2Upgrade();	
		}
		gSelfTest.fSetTaskID(TASK_ID_RING);

		OSTimeDly(10);

    }
}