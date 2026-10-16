#include "main.h"
#include "Uart6_new.h"
#include "can_ring.h"
#include "ABU.h"

//NOINIT stBMS_INFO	gEVBms_1_Info;
//NOINIT stBMSCommCtrlInfo gBms_1_CommInfo;
//NOINIT ChargerContext CAN1_ChargerCtx;



void CAN_1_SendCallBack(void)//2MS进一次中断
{
	return;
}

void UnPackESData(UINT16 iRStartPst)
{
	UINT8 u8SrcAddress, ucBattNo;
	UINT16 u16temp;
	union 
	{
		unsigned short u16temp;
		signed short i16temp;
	}union_temp;

	u8SrcAddress = (CanRxMsgBuff_A[iRStartPst].ExtId & 0xFF);
	switch(0x3ff&(CanRxMsgBuff_A[iRStartPst].ExtId >> 16))
	{
		case 0x32:
			if((u8SrcAddress >= 0x32)&&(u8SrcAddress<=0x35))
			{
				ucBattNo = u8SrcAddress - 0x32;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8ExistStat = 1;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8CommFailAlm = 0;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8CommAlmDelay = 0;

				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8Valid = 1;

				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[1] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[0]);
				if(u16temp < 6)
				{
					gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_ProState = u16temp;
				}
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[3] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[2]);
				if(u16temp < 7)
				{
					gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_WorkState = u16temp;
				} 
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[5] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[4]);
				if(u16temp < 3)
				{
					gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_ChgState = u16temp;
				} 

				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[7] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[6]);
				if(u16temp < 6)
				{
					gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_WorkMode = u16temp;
				} 
			}
			break;
                        
                        
		case 0x33:
			if((u8SrcAddress >= 0x32)&&(u8SrcAddress<=0x35))
			{
                //电池组的限制充电电流、限制放电电流、限制充电功率、限制放电功率
				ucBattNo = u8SrcAddress - 0x32;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8ExistStat = 1;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8CommFailAlm = 0;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8CommAlmDelay = 0;

				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[1] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[0]);
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_Limit_ChargeCurrent = (float)u16temp*0.1;
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[3] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[2]);
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_Limit_DischargeCurrent = (float)u16temp*0.1;
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[5] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[4]);
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_Limit_ChargePower = (float)u16temp*0.1;
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[7] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[6]);
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_Limit_DischargePower = (float)u16temp*0.1;
			}
			break;
		case 0x34:
			if((u8SrcAddress >= 0x32)&&(u8SrcAddress<=0x35))
			{
                               //电池组的限制充电电压和限制放电电压
				ucBattNo = u8SrcAddress - 0x32;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8ExistStat = 1;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8CommFailAlm = 0;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8CommAlmDelay = 0;

				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[1] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[0]);
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_Limit_ChargeVoltage = (float)u16temp*0.1;
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[3] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[2]);
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_Limit_DischargeVoltage = (float)u16temp*0.1;		
			}
			break;
		case 0x3C:
			if((u8SrcAddress >= 0x32)&&(u8SrcAddress<=0x35))
			{
                //电池组的总电压、总输出电流、输出功率、输出电压
				ucBattNo = u8SrcAddress - 0x32;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8ExistStat = 1;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8CommFailAlm = 0;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.u8CommAlmDelay = 0;

				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[1] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[0]);
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_SumVolt = (float)u16temp*0.1;
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[3] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[2]);
				union_temp.u16temp = u16temp;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_OutCurrent = ((float)union_temp.i16temp)*0.1;
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[5] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[4]);
				union_temp.u16temp = u16temp;
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_OutPower = ((float)union_temp.i16temp)*0.1;
				u16temp = (UINT16)((gCan1RxMsgBuf[iRStartPst].RxMsg.Data[7] << 8) + gCan1RxMsgBuf[iRStartPst].RxMsg.Data[6]);
				gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].BattInfoFromCan.BMS_OutVolt = (float)u16temp*0.1;
			}
			break;
		default:
			break;
	}			
}



void Read_A_AllBuf_ES(void)
{
	UINT16 iIdx = 0;
	for (iIdx = 0;iIdx < CAN_A_B_BUF_MAX_NUM; iIdx++)
	{
		if (CanRxMsgBuff_A[iIdx].ExtId != 0)
		{
			gCan1RxMsgBuf[iIdx].RxMsg = CanRxMsgBuff_A[iIdx];
			UnPackESData(iIdx);
			gCan1RxMsgBuf[iIdx].iFlag = FALSE;
			CanRxMsgBuff_A[iIdx].ExtId = 0;
		}
	}
	return;
}

//20230627
#define BMS_CAN_COMM_TIMEOUE	10		//seconds
void TASK_CAN1Proc(void* pdata)
{
	OSTimeDly(1000);
	gSelfTest.fSetTaskID(Tasko_ID_CAN1);
	OSTimeDly(1000);
	gSelfTest.fSetTaskID(Tasko_ID_CAN1);
	OSTimeDly(1000);
	gSelfTest.fSetTaskID(Tasko_ID_CAN1);
	if(IsTopway())
	{
		OSTimeDly(1000);
		gSelfTest.fSetTaskID(Tasko_ID_CAN1);
		OSTimeDly(1000);
		gSelfTest.fSetTaskID(Tasko_ID_CAN1);
		OSTimeDly(1000);
		gSelfTest.fSetTaskID(Tasko_ID_CAN1);
	}

	BSP_Can_1_Init();
	BSP_Can1_RxEnable();

	BSP_CanCheck(0);


	for (;;)
	{
		
		gSelfTest.fSetTaskID(Tasko_ID_CAN1);
		OSTimeDly(10);

		if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
		{
			OSTimeDly(50);
			continue;
		}

		Read_A_AllBuf_ES();

		//电池1CAN通信状态判断
		if(gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.u8CommAlmDelay > BMS_CAN_COMM_TIMEOUE)
		{
			gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.u8CommFailAlm = 1;
		}
		if((gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1Exist == 1)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1CanCommEn == 1))			
		{
			if((gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.u8ExistStat == 0)
				|| (gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.u8CommFailAlm == 1))
			{
				gs_EsSampleInfo.Batt_CanComm_Break_Alm[0] = 1;

				gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.u8Valid = 0;
			}
			else
			{
				gs_EsSampleInfo.Batt_CanComm_Break_Alm[0] = 0;

				gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.u8Valid = 1;
			}
		}
		else
		{
			gs_EsSampleInfo.Batt_CanComm_Break_Alm[0] = 0;

			gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.u8Valid = 0;
		}

		

		//电池2CAN通信状态判断
		if(gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.u8CommAlmDelay > BMS_CAN_COMM_TIMEOUE)
		{
			gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.u8CommFailAlm = 1;
		}
		if((gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt2Exist == 1)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt2CanCommEn == 1))	
		{
			if((gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.u8ExistStat == 0)
				|| (gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.u8CommFailAlm == 1))
			{
				gs_EsSampleInfo.Batt_CanComm_Break_Alm[1] = 1;

				gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.u8Valid = 0;
			}
			else
			{
				gs_EsSampleInfo.Batt_CanComm_Break_Alm[1] = 0;
				gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.u8Valid = 1;
			}
		}
		else
		{
			gs_EsSampleInfo.Batt_CanComm_Break_Alm[1] = 0;
			gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.u8Valid = 0;
		}

		

		//电池3CAN通信状态判断
		if(gs_IMMU2_LCCU_Mng.ESBattMng[2].BattInfoFromCan.u8CommAlmDelay > BMS_CAN_COMM_TIMEOUE)
		{
			gs_IMMU2_LCCU_Mng.ESBattMng[2].BattInfoFromCan.u8CommFailAlm = 1;
		}
		if((gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt3Exist == 1)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt3CanCommEn == 1))	
		{
			if((gs_IMMU2_LCCU_Mng.ESBattMng[2].BattInfoFromCan.u8ExistStat == 0)
				|| (gs_IMMU2_LCCU_Mng.ESBattMng[2].BattInfoFromCan.u8CommFailAlm == 1))
			{
				gs_EsSampleInfo.Batt_CanComm_Break_Alm[2] = 1;

				gs_IMMU2_LCCU_Mng.ESBattMng[2].BattInfoFromCan.u8Valid = 0;
			}
			else
			{
				gs_EsSampleInfo.Batt_CanComm_Break_Alm[2] = 0;

				gs_IMMU2_LCCU_Mng.ESBattMng[2].BattInfoFromCan.u8Valid = 1;
			}
		}
		else
		{
			gs_EsSampleInfo.Batt_CanComm_Break_Alm[2] = 0;

			gs_IMMU2_LCCU_Mng.ESBattMng[2].BattInfoFromCan.u8Valid = 0;
		}

		//电池4CAN通信状态判断
		if(gs_IMMU2_LCCU_Mng.ESBattMng[3].BattInfoFromCan.u8CommAlmDelay > BMS_CAN_COMM_TIMEOUE)
		{
			gs_IMMU2_LCCU_Mng.ESBattMng[3].BattInfoFromCan.u8CommFailAlm = 1;
		}
		if((gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4Exist == 1)
			&& (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4CanCommEn == 1))	
		{
			if((gs_IMMU2_LCCU_Mng.ESBattMng[3].BattInfoFromCan.u8ExistStat == 0)
				|| (gs_IMMU2_LCCU_Mng.ESBattMng[3].BattInfoFromCan.u8CommFailAlm == 1))
			{
				gs_EsSampleInfo.Batt_CanComm_Break_Alm[3] = 1;

				gs_IMMU2_LCCU_Mng.ESBattMng[3].BattInfoFromCan.u8Valid = 0;
			}
			else
			{
				gs_EsSampleInfo.Batt_CanComm_Break_Alm[3] = 0;

				gs_IMMU2_LCCU_Mng.ESBattMng[3].BattInfoFromCan.u8Valid = 1;
			}
		}
		else
		{
			gs_EsSampleInfo.Batt_CanComm_Break_Alm[3] = 0;

			gs_IMMU2_LCCU_Mng.ESBattMng[3].BattInfoFromCan.u8Valid = 0;
		}

//by lxr01202   新增ABU通信处理
//       ABU_Comm();
	}
}






