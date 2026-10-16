#include "main.h"
#include "EtherNet.h"
#include "can_ring.h"
//#include "Sampler_Modbus_AcMeter.h"
#include "secondary_upgrade.h"



NOINIT INT8U  UART5RxLength;
NOINIT UINT8 gUart5RecvBuff[RS485_UART5_RX_MAX_NUM];
NOINIT UINT8 gUart5TXBuff[RS485_UART5_TX_MAX_NUM];


//UINT32 g_u32Rs485SeqNo;
//UINT16 g_u16Rs485HeartBeatNo;

NOINIT stIMIU_Info gIMIU_Info_UART5[IMIU_MAX_NUM_UART5];
IMIU_COMM_MNG gImiuCommMng_uart5[IMIU_MAX_NUM_UART5] = {0};

extern void IMIUSample_1(IMIU_COMM_MNG *pImiu);

void Uart5_Hook(void)
{
	//if(gs_Measure.isTest)
	//{   
	//空转
	//}
	//else
	//{      
	if(dv_Uart5.rev_msg_end_time_cnt > 0)
	{
		dv_Uart5.rev_msg_end_time_cnt++;

		if(dv_Uart5.rev_msg_end_time_cnt >= dv_Uart5.rev_msg_end_time)  //空闲间隔时间
		{
			if(dv_Uart5.pData >= 8)
			{
				dv_Uart5.ReceiveFlag = 1;    
				dv_Uart5.rev_msg_end_time_cnt = 0;
			}
			else
			{
				dv_Uart5.pData = 0;
			}
		}            
	}
	//}
}

void RefreshUart5Time(void)
{
	struct tm  tmNow;
	static UCHAR  Sec = 0;

	gRTCData.fGetLocalTime(&tmNow);
	if((UCHAR)tmNow.tm_sec != Sec)/*1秒钟*/
	{
		Sec = (UCHAR)tmNow.tm_sec;


	}
}

void InitIMIU_Uart5_E2prom(void)
{
	__XXX_UNION_VALUE unVal;
	ExGetSigInfo(E_TYPE_IMIU5, ID_fSetJYMAlm_Resist, &unVal, NULL, BY_E2PROM);
	//ExGetSigInfo(E_TYPE_IMIU5, ID_u8JYMAlmStatMask, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU5, ID_u8SetJYMCtrlMode, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU5, ID_u8InsltCheckByVolt, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_IMIU6, ID_fSetJYMAlm_Resist, &unVal, NULL, BY_E2PROM);
	//ExGetSigInfo(E_TYPE_IMIU6, ID_u8JYMAlmStatMask, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU6, ID_u8SetJYMCtrlMode, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU6, ID_u8InsltCheckByVolt, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_IMIU7, ID_fSetJYMAlm_Resist, &unVal, NULL, BY_E2PROM);
	//ExGetSigInfo(E_TYPE_IMIU7, ID_u8JYMAlmStatMask, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU7, ID_u8SetJYMCtrlMode, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU7, ID_u8InsltCheckByVolt, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_IMIU8, ID_fSetJYMAlm_Resist, &unVal, NULL, BY_E2PROM);
	//ExGetSigInfo(E_TYPE_IMIU8, ID_u8JYMAlmStatMask, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU8, ID_u8SetJYMCtrlMode, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU8, ID_u8InsltCheckByVolt, &unVal, NULL, BY_E2PROM);
}
void InitIMIUInfo_Uart5(void)
{
	unsigned char i;

	for(i = 0; i < IMIU_MAX_NUM_UART5; i++)
	{
		gIMIU_Info_UART5[i].u8JYMAlmStat[0] = 0;				//无告警
		gIMIU_Info_UART5[i].u8JYMAction[0] = 0;
		gIMIU_Info_UART5[i].u8RunEarth_DetectAlm[0] = 0;
		gIMIU_Info_UART5[i].fSetJYMAlm_Resist[0] = 74;
		gIMIU_Info_UART5[i].u8SetJYMCtrlStat[0] = 2;      //3手动  2后台  1自动
		gIMIU_Info_UART5[i].u8SetGroundCtrlStat[0] = 1;        //0-禁止 1-使能
		gIMIU_Info_UART5[i].fJYMCurr[0] = 0;
		gIMIU_Info_UART5[i].fJYMVolt[0] = 650;
		gIMIU_Info_UART5[i].fJYMP2gVolt[0] = 0;
		gIMIU_Info_UART5[i].fJYMN2gVolt[0] = 0;
		gIMIU_Info_UART5[i].fJYMP2gResist[0] = 0;
		gIMIU_Info_UART5[i].fJYMN2gResist[0] = 0;
		gIMIU_Info_UART5[i].u8InsltCheckByVolt = FALSE;		//0绝缘下降电阻不按电压来   1按电压来
		gIMIU_Info_UART5[i].u8JYMAlmStatMask = FALSE;

		gIMIU_Info_UART5[i].u8CTRLInsulationCMD[0] = 0u;
		gIMIU_Info_UART5[i].u8EarthFalt[0] = 0;	
		gIMIU_Info_UART5[i].u8SelfChkResult[0] = 0;
		gIMIU_Info_UART5[i].u8CTRLInterDOCMD_NO = 0;
		gIMIU_Info_UART5[i].u8CTRLInterDOCMD[0] = 0;
		gIMIU_Info_UART5[i].u8CTRLInterDOCMD[1] = 0;

		gIMIU_Info_UART5[i].fRunJYMAlm_ResistPerV[0] = 100;
		gIMIU_Info_UART5[i].fSetJYMAlm_Resist_PerV[0] = 100;

		gImiuCommMng_uart5[i].u8UartNo = UART_NO_UART5;
		gImiuCommMng_uart5[i].FeedDogTaskID = TASK_ID_UART5;
		gImiuCommMng_uart5[i].pUartXTXBuff = gUart5TXBuff;
		gImiuCommMng_uart5[i].pUartXRecvBuff = gUart5RecvBuff;
		gImiuCommMng_uart5[i].u8DevAddr = YDN23_IMIU_ADDR+i;

		gImiuCommMng_uart5[i].u8InsltCommAlm = 0;
		gImiuCommMng_uart5[i].u8CommCount = 0;
		gImiuCommMng_uart5[i].u8CommStat = 0;

		gImiuCommMng_uart5[i].pImiuInfo = &gIMIU_Info_UART5[i];
	}
}

void IMEU2_PV_Deal(void)
{
	if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist == 0) 
	{
		gs_EsSampleInfo.PV_Contactor_Detection[0] = FALSE;
		gs_EsSampleInfo.PV_SPD_Detection[0] = FALSE;
		gs_EsSampleInfo.PV_Isolation_Switch[0] = 0;

		gs_EsSampleInfo.PV_Contactor_Detection[1] = FALSE;
		gs_EsSampleInfo.PV_SPD_Detection[1] = FALSE;
		gs_EsSampleInfo.PV_Isolation_Switch[1] = 0;

		gs_EsSampleInfo.PV_Contactor_Detection[2] = FALSE;
		gs_EsSampleInfo.PV_SPD_Detection[2] = FALSE;
		gs_EsSampleInfo.PV_Isolation_Switch[2] = 0;
	}
	else
	{
		//PV1控制
		if((gIMEU_Info[1].u8CommError == FALSE)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1Exist == 1))
		{
			//控制反馈
			//if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[0] == 0)
			{
				if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV1ContactorEn)
				{
					//if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] == DO_ON) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI7 == 1))   //断开
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[0] = TRUE;
					//}
					//else if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] == DO_OFF) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI7 == 0))   //闭合
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[0] = TRUE;
					//}
					//else
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[0] = FALSE;
					//}		

					gs_EsSampleInfo.PV_Contactor_Detection[0] = gIMEU_Info[1].DIStat_1.BIT_FIELD.DI3;
				}
				else
				{
					gs_EsSampleInfo.PV_Contactor_Detection[0] = FALSE;
				}
			}

			//SPD检测
			if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV1SPDEn)
			{
				//if(gIMEU_Info[0].DIStat_2.BIT_FIELD.DI10 == 1)  //断开
				//{
				//	gs_EsSampleInfo.PV_SPD_Detection[0] = TRUE;
				//}
				//else
				//{
				//	gs_EsSampleInfo.PV_SPD_Detection[0] = FALSE;
				//}
				gs_EsSampleInfo.PV_SPD_Detection[0] = gIMEU_Info[1].DIStat_1.BIT_FIELD.DI2;
			}
			else
			{
				gs_EsSampleInfo.PV_SPD_Detection[0] = FALSE;
			}

			//隔离开关
			if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV1IsolationSwEn)
			{
				//if(gIMEU_Info[0].DIStat_2.BIT_FIELD.DI13 == 1)  //断开
				//{
				//	gs_EsSampleInfo.PV_Isolation_Switch[0] = 0;
				//}
				//else
				//{
				//	gs_EsSampleInfo.PV_Isolation_Switch[0] = 1;
				//}
				gs_EsSampleInfo.PV_Isolation_Switch[0] = gIMEU_Info[1].DIStat_1.BIT_FIELD.DI1;
			}
			else
			{
				gs_EsSampleInfo.PV_Isolation_Switch[0] = 0;
			}

			//PV控制
			if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] == DO_ON)
				&& (gs_EsSampleInfo.PV_SPD_Detection[0] == FALSE))
			{
				gIMEU_Info[1].DO_CtrlValue.BIT_FIELD.DO1 = 1;
			}
			else
			{
				gIMEU_Info[1].DO_CtrlValue.BIT_FIELD.DO1 = 0;
			}
		}

		//PV2控制
		if((gIMEU_Info[1].u8CommError == FALSE)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt2Exist == 1))
		{
			//控制反馈
			//if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[1] == 0)
			{
				if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV2ContactorEn)
				{
					//if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[1] == DO_ON) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI8 == 1))   //断开
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[1] = TRUE;
					//}
					//else if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[1] == DO_OFF) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI8 == 0))   //闭合
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[1] = TRUE;
					//}
					//else
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[1] = FALSE;
					//}	
					gs_EsSampleInfo.PV_Contactor_Detection[1] = gIMEU_Info[1].DIStat_1.BIT_FIELD.DI6;
				}
				else
				{
					gs_EsSampleInfo.PV_Contactor_Detection[1] = FALSE;
				}
			}

			//SPD检测
			if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV2SPDEn)
			{
				//if(gIMEU_Info[0].DIStat_2.BIT_FIELD.DI11 == 1)  //断开
				//{
				//	gs_EsSampleInfo.PV_SPD_Detection[1] = TRUE;
				//}
				//else
				//{
				//	gs_EsSampleInfo.PV_SPD_Detection[1] = FALSE;
				//}
				gs_EsSampleInfo.PV_SPD_Detection[1] = gIMEU_Info[1].DIStat_1.BIT_FIELD.DI5;
			}
			else
			{
				gs_EsSampleInfo.PV_SPD_Detection[1] = FALSE;
			}

			//隔离开关
			if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV2IsolationSwEn)
			{
				//if(gIMEU_Info[0].DIStat_2.BIT_FIELD.DI14 == 1)  //断开
				//{
				//	gs_EsSampleInfo.PV_Isolation_Switch[1] = 0;
				//}
				//else
				//{
				//	gs_EsSampleInfo.PV_Isolation_Switch[1] = 1;
				//}
				gs_EsSampleInfo.PV_Isolation_Switch[1] = gIMEU_Info[1].DIStat_1.BIT_FIELD.DI4;
			}
			else
			{
				gs_EsSampleInfo.PV_Isolation_Switch[1] = 0;
			}

			//PV控制
			if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[1] == DO_ON)
				&& (gs_EsSampleInfo.PV_SPD_Detection[1] == FALSE))
			{
				gIMEU_Info[1].DO_CtrlValue.BIT_FIELD.DO2 = 1;
			}
			else
			{
				gIMEU_Info[1].DO_CtrlValue.BIT_FIELD.DO2 = 0;
			}
		}

		//PV3控制
		if((gIMEU_Info[1].u8CommError == FALSE)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt3Exist == 1))
		{
			//控制反馈
			//if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[2] == 0)
			{
				if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV3ContactorEn)
				{
					//if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[2] == DO_ON) && (gIMEU_Info[0].DIStat_2.BIT_FIELD.DI9 == 1))   //断开
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[2] = TRUE;
					//}
					//else if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[2] == DO_OFF) && (gIMEU_Info[0].DIStat_2.BIT_FIELD.DI9 == 0))   //闭合
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[2] = TRUE;
					//}
					//else
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[2] = FALSE;
					//}	
					gs_EsSampleInfo.PV_Contactor_Detection[2] = gIMEU_Info[1].DIStat_2.BIT_FIELD.DI13;
				}
				else
				{
					gs_EsSampleInfo.PV_Contactor_Detection[2] = FALSE;
				}
			}

			//SPD检测
			if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV3SPDEn)
			{
				//if(gIMEU_Info[0].DIStat_2.BIT_FIELD.DI12 == 1)  //断开
				//{
				//	gs_EsSampleInfo.PV_SPD_Detection[2] = TRUE;
				//}
				//else
				//{
				//	gs_EsSampleInfo.PV_SPD_Detection[2] = FALSE;
				//}
				gs_EsSampleInfo.PV_SPD_Detection[2] = gIMEU_Info[1].DIStat_2.BIT_FIELD.DI12;
			}
			else
			{
				gs_EsSampleInfo.PV_SPD_Detection[2] = FALSE;
			}

			//隔离开关
			if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV3IsolationSwEn)
			{
				//if(gIMEU_Info[0].DIStat_2.BIT_FIELD.DI15 == 1)  //断开
				//{
				//	gs_EsSampleInfo.PV_Isolation_Switch[2] = 0;
				//}
				//else
				//{
				//	gs_EsSampleInfo.PV_Isolation_Switch[2] = 1;
				//}
				gs_EsSampleInfo.PV_Isolation_Switch[2] = gIMEU_Info[1].DIStat_2.BIT_FIELD.DI11;
			}
			else
			{
				gs_EsSampleInfo.PV_Isolation_Switch[2] = 0;
			}

			//PV控制
			if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[2] == DO_ON)
				&& (gs_EsSampleInfo.PV_SPD_Detection[2] == FALSE))
			{
				gIMEU_Info[1].DO_CtrlValue.BIT_FIELD.DO3 = 1;
			}
			else
			{
				gIMEU_Info[1].DO_CtrlValue.BIT_FIELD.DO3 = 0;
			}
		}
	} //  if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 0)   else

	if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist == 0) 
	{
		gs_EsSampleInfo.PV_Contactor_Detection[3] = FALSE;
		gs_EsSampleInfo.PV_SPD_Detection[3] = FALSE;
		gs_EsSampleInfo.PV_Isolation_Switch[3] = 0;
	}
	else
	{
		//PV4控制
		if((gIMEU_Info[1].u8CommError == FALSE)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4Exist == 1))
		{
			//控制反馈
			//if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[3] == 0)
			{
				if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV4ContactorEn)
				{
					//if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[3] == DO_ON) && (gIMEU_Info[1].DIStat_1.BIT_FIELD.DI3 == 1))   //断开
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[3] = TRUE;
					//}
					//else if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[3] == DO_OFF) && (gIMEU_Info[1].DIStat_1.BIT_FIELD.DI3 == 0))   //闭合
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[3] = TRUE;
					//}
					//else
					//{
					//	gs_EsSampleInfo.PV_Contactor_Detection[3] = FALSE;
					//}	

					gs_EsSampleInfo.PV_Contactor_Detection[3] = gIMEU_Info[1].DIStat_2.BIT_FIELD.DI16;
				}
				else
				{
					gs_EsSampleInfo.PV_Contactor_Detection[0] = FALSE;
				}
			}

			//SPD检测
			if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV4SPDEn)
			{
				//if(gIMEU_Info[1].DIStat_1.BIT_FIELD.DI4 == 1)  //断开
				//{
				//	gs_EsSampleInfo.PV_SPD_Detection[3] = TRUE;
				//}
				//else
				//{
				//	gs_EsSampleInfo.PV_SPD_Detection[3] = FALSE;
				//}
				gs_EsSampleInfo.PV_SPD_Detection[3] = gIMEU_Info[1].DIStat_2.BIT_FIELD.DI15;
			}
			else
			{
				gs_EsSampleInfo.PV_SPD_Detection[3] = FALSE;
			}

			//隔离开关
			if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV4IsolationSwEn)
			{
				//if(gIMEU_Info[1].DIStat_1.BIT_FIELD.DI5 == 1)  //断开
				//{
				//	gs_EsSampleInfo.PV_Isolation_Switch[3] = 0;
				//}
				//else
				//{
				//	gs_EsSampleInfo.PV_Isolation_Switch[3] = 1;
				//}
				gs_EsSampleInfo.PV_Isolation_Switch[3] = gIMEU_Info[1].DIStat_2.BIT_FIELD.DI14;
			}
			else
			{
				gs_EsSampleInfo.PV_Isolation_Switch[3] = 0;
			}

			//PV控制
			if((gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[3] == DO_ON)
				&& (gs_EsSampleInfo.PV_SPD_Detection[3] == FALSE))
			{
				gIMEU_Info[1].DO_CtrlValue.BIT_FIELD.DO4 = 1;
			}
			else
			{
				gIMEU_Info[1].DO_CtrlValue.BIT_FIELD.DO4 = 0;
			}
		}
	} //  if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist == 0)   else
}
void IMEU2_Batt_Deal(void)
{
	//IMEU2采集
	//ExGetSigInfo(E_TYPE_SYS, ID_u8IMEU1_En, &unTempVal, NULL, BY_E2PROM);
	if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 0) 
	{
		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[0] = FALSE;
		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[1] = FALSE;
		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = FALSE;
		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[3] = FALSE;		
	}
	else
	{
		//电池1
		if((gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt1InputContactorEn) && (gIMEU_Info[0].u8CommError == FALSE)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1Exist == 1))
		{
			//if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[0] == 0)
			//{
			//	if((gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[0] == DO_ON) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI1 == 1))   //断开
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[0] = TRUE;
			//	}
			//	else if((gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[0] == DO_OFF) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI1 == 0))   //闭合
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[0] = TRUE;
			//	}
			//	else
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[0] = FALSE;
			//	}
			//}
			gs_EsSampleInfo.Battry_Grp1_Input_Contactor[0] = gIMEU_Info[0].DIStat_1.BIT_FIELD.DI1;
		}
		else
		{
			gs_EsSampleInfo.Battry_Grp1_Input_Contactor[0] = FALSE;
		}
		//电池2
		if((gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt2InputContactorEn) && (gIMEU_Info[0].u8CommError == FALSE)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt2Exist == 1))
		{
			//if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[1] == 0)
			//{
			//	if((gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[1] == DO_ON) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI2 == 1))   //断开
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[1] = TRUE;
			//	}
			//	else if((gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[1] == DO_OFF) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI2 == 0))   //闭合
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[1] = TRUE;
			//	}
			//	else
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[1] = FALSE;
			//	}
			//}
			gs_EsSampleInfo.Battry_Grp1_Input_Contactor[1] = gIMEU_Info[0].DIStat_1.BIT_FIELD.DI2;
		}
		else
		{
			gs_EsSampleInfo.Battry_Grp1_Input_Contactor[1] = FALSE;
		}
		//电池3
		if((gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt3InputContactorEn) && (gIMEU_Info[0].u8CommError == FALSE)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt3Exist == 1))
		{
			//if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[2] == 0)
			//{
			//	if((gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[2] == DO_ON) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI3 == 1))   //断开
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = TRUE;
			//	}
			//	else if((gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[2] == DO_OFF) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI3 == 0))   //闭合
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = TRUE;
			//	}
			//	else
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = FALSE;
			//	}
			//}
			gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = gIMEU_Info[0].DIStat_1.BIT_FIELD.DI3;
		}
		else
		{
			gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = FALSE;
		}

		//电池4
		if((gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt4InputContactorEn) && (gIMEU_Info[0].u8CommError == FALSE)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4Exist == 1))
		{
			//if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[2] == 0)
			//{
			//	if((gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[2] == DO_ON) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI3 == 1))   //断开
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = TRUE;
			//	}
			//	else if((gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[2] == DO_OFF) && (gIMEU_Info[0].DIStat_1.BIT_FIELD.DI3 == 0))   //闭合
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = TRUE;
			//	}
			//	else
			//	{
			//		gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] = FALSE;
			//	}
			//}
			gs_EsSampleInfo.Battry_Grp1_Input_Contactor[3] = gIMEU_Info[0].DIStat_1.BIT_FIELD.DI4;
		}
		else
		{
			gs_EsSampleInfo.Battry_Grp1_Input_Contactor[3] = FALSE;
		}

	} // if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 0)  else


}
void IMEU2_SysSensor_Deal(void)
{	
	//IMEU2采集
	//ExGetSigInfo(E_TYPE_SYS, ID_u8IMEU2_En, &unTempVal, NULL, BY_E2PROM);
	if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 0)
	{
		gs_EsSampleInfo.SmokeDI = FALSE;
		gs_EsSampleInfo.SmokeDI_AlmDelay = 0;

		gs_EsSampleInfo.FloodingDI = FALSE;
		gs_EsSampleInfo.ToppleDI = FALSE;

		gs_EsSampleInfo.AC_Input_Contactor = FALSE;

		gs_EsSampleInfo.ACLoad_Output_Breaker = FALSE;

		gs_EsSampleInfo.Low_Volt_Breaker = FALSE;
	}
	else
	{	
		if((gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.AC_Input_Contactor) && (gIMEU_Info[0].u8CommError == FALSE))
		{			
			gs_EsSampleInfo.AC_Input_Contactor = gIMEU_Info[0].DIStat_2.BIT_FIELD.DI16; 			
		}
		else
		{
			gs_EsSampleInfo.AC_Input_Contactor= FALSE;
		}

		if((gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.ACLoad_Output_Breaker) && (gIMEU_Info[0].u8CommError == FALSE))
		{			
			gs_EsSampleInfo.ACLoad_Output_Breaker = gIMEU_Info[0].DIStat_2.BIT_FIELD.DI15; 			
		}
		else
		{
			gs_EsSampleInfo.ACLoad_Output_Breaker= FALSE;
		}

		if((gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.FloodingDIEn) && (gIMEU_Info[0].u8CommError == FALSE))
		{			
			gs_EsSampleInfo.FloodingDI = gIMEU_Info[0].WS_AB_Alm;
		}
		else
		{
			gs_EsSampleInfo.FloodingDI = FALSE;
		}

		if((gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.ToppleDIEn) && (gIMEU_Info[0].u8CommError == FALSE))
		{			
			if(gIMEU_Info[0].DIStat_2.BIT_FIELD.DI13 == 0)
				gs_EsSampleInfo.ToppleDI = 1;
			else 
				gs_EsSampleInfo.ToppleDI = 0;
		}
		else
		{
			gs_EsSampleInfo.ToppleDI= FALSE;
		}

		if((gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.SmokeDIEn) && (gIMEU_Info[0].u8CommError == FALSE))
		{			
			if(gIMEU_Info[0].DIStat_2.BIT_FIELD.DI15 == 0)  //闭合
			{
				if(gs_EsSampleInfo.SmokeDI_AlmDelay == 0)
				{
					gs_EsSampleInfo.SmokeDI_AlmDelay = 1;
					gs_EsSampleInfo.SmokeDI= FALSE;
				}
				else if(gs_EsSampleInfo.SmokeDI_AlmDelay > 20)
				{
					gs_EsSampleInfo.SmokeDI= TRUE;
				}
			}
			else if((gs_EsSampleInfo.SmokeDI_AlmDelay >= 4)&&(gs_EsSampleInfo.SmokeDI_AlmDelay < 16))
			{
				//中间先关8秒钟的DO予以确认
			}
			else
			{
				gs_EsSampleInfo.SmokeDI= FALSE;
				gs_EsSampleInfo.SmokeDI_AlmDelay = 0;
			}			
		}
		else
		{
			gs_EsSampleInfo.SmokeDI= FALSE;
			gs_EsSampleInfo.SmokeDI_AlmDelay = 0;
		}

		//烟感电源
		if(gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.SmokeDIEn)
		{
			//在确认告警前，断开DO，如果之后还处于告警状态，才确认烟感告警
			if((gs_EsSampleInfo.SmokeDI_AlmDelay >= 6) && (gs_EsSampleInfo.SmokeDI_AlmDelay < 10))
			{
				gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO1 = 0;
			}
			else
			{
				gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO1 = 1;
			}
		}
        else
        {
                gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO1 = 0;
        }

		//低压开关状态
		if((gEvChargeInfo.u8LowVoltAlmVltLEVEL == gIMEU_Info[0].DIStat_2.BIT_FIELD.DI12) && (gIMEU_Info[0].u8CommError == FALSE))
		{			
			gs_EsSampleInfo.Low_Volt_Breaker = TRUE; 			
		}
		else
		{
			gs_EsSampleInfo.Low_Volt_Breaker= FALSE;
		}
        
	}   //if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist == 0)  else
}
void IMEU2_Sample(void)
{
	if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 0) 
	{
		gIMEU_Info[0].u8CommError = FALSE;
	}
	else
	{
		OSTimeDly(10);
		IMEU_Sampler(&gIMEU_Info[0]);
	}

	if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist == 0) 
	{
		gIMEU_Info[1].u8CommError = FALSE;
	}
	else
	{
		OSTimeDly(10);
		IMEU_Sampler(&gIMEU_Info[1]);
	}

	IMEU2_PV_Deal();
	IMEU2_Batt_Deal();
	IMEU2_SysSensor_Deal();
}
void TASK_Uart5Proc(void* pdata)
{
	static INT8U ch = 0;

	UNUSED(pdata);
	dv_Uart5.fInitUart(m9600, UARTX_PARITY_NONE);				//InitUart5
	dv_Uart5.fEnableRx();										//使能收中断

	//UINT16 uSendLengh;
	//g_u32Rs485SeqNo = 0;
	//g_u16Rs485HeartBeatNo = 0;

	//MODBUS_ACMETER_Init(&gs_DeviceClass[DEVICE_CLASS_MODBUS_ACMETER], UART_NO_UART5);

	InitIMEU_Info_uart5(&gIMEU_Info[0], 0);
	InitIMEU_Info_uart5(&gIMEU_Info[1], 1);

	SecondaryUpgradeInit(UPGRADE_TYPE_IMEU2, &g_stImeu2UpgradeInfo[0], 0);
	SecondaryUpgradeInit(UPGRADE_TYPE_IMEU2, &g_stImeu2UpgradeInfo[1], 1);

	for(;;)
	{
		/*
		OSTimeDly(10);
		gSelfTest.fSetTaskID(TASK_ID_UART5);
		if (gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)
		{
		continue;
		}

		RefreshUart5Time();

		MODBUS_ACMETER_Sample();

		MODBUS_ACMETER_ParamUnify();*/

		//MODBUS_ACMETER_SendCmd();

		

		OSTimeDly(10);
		gSelfTest.fSetTaskID(TASK_ID_UART5);

		//upgrade imeu2
		if(SecondaryUpgradeProc())
		{
			continue;
		}

		if(gImiuCommMng_uart5[ch].pImiuInfo->u8JYMAlmStatMask == TRUE)
		{
			gImiuCommMng_uart5[ch].pImiuInfo->u8JYMAlmStat[0] = FALSE;
			gImiuCommMng_uart5[ch].pImiuInfo->u8InsltCommAlm[0] = FALSE;		
		}
		else
		{
			//20201201 改为自动检测方式
			gImiuCommMng_uart5[ch].pImiuInfo->u8SetJYMCtrlStat[0] = 2;
			gImiuCommMng_uart5[ch].pImiuInfo->u8SetGroundCtrlStat[0] = 1;
			gImiuCommMng_uart5[ch].pImiuInfo->fSetJYMAlm_Resist_PerV[0] = 100;
			IMIUSample_1(&gImiuCommMng_uart5[ch]);
		}

		ch++;
		if(ch >= IMIU_MAX_NUM_UART5)
		{
			ch = 0;
		}

		IMEU2_Sample();

	}   // for()
}
