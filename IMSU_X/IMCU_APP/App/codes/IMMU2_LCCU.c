#include "main.h"
#include "EtherNet.h"
#include "Upgrade.h"
#include "HistoryEvent.h"
#include "ABU.h"
#include "secondary_upgrade.h"
#include "bsp_pwm.h"

extern UINT8 u8A_SpearLOCKDeal(UINT8 u8IsLock);
extern UINT8 u8B_SpearLOCKDeal(UINT8 u8IsLock);
static NOINIT UCHAR s_u8RectTypeNo[mRECTNUM];




Enji_Master_Slave_Comm_Mng_def gs_IMMU2_LCCU_Mng = {0};


NOINIT UINT8 IMMU2_LCCU_RecvBuff[ENJI_M_S_COMM_RCV_BUF_NUM][UART6_ENJI_RECV_MAX_NUM];







void Init_IMMU2_LCCU(void)
{
	UCHAR i;

	for(i = 0; i < 2; i++)
	{
		gs_IMMU2_LCCU_Mng.u8GunTaskType[i] = GUN_TASK_TYPE_Idle;
		gs_IMMU2_LCCU_Mng.u8EsBattTaskType[i] = ES_BATT_TASK_TYPE_Idle;
		gs_IMMU2_LCCU_Mng.fTaskMpptOutKw_ACDC[i] = 0;
		gs_IMMU2_LCCU_Mng.fTaskMpptOutKw_DCDC[i] = 0;
		gs_IMMU2_LCCU_Mng.u8InsultChkOnOffCtrl[i] = 0;
	}
	gs_IMMU2_LCCU_Mng.u8InsultChkOnOffCtrl[2] = 0;

	for(i = 0; i < ImmuCtrlGrpNo_MAX; i++)
	{
		gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[i] = 0;
		gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd_Backup[i] = 0;
		gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd_Delay[i] = 0;
	}

	for(i = 0; i < BATT_GRP_MAX_NUM; i++)
	{
		gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[i] = 0;
		gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[i] = 0;
		gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[i] = 0;
		


		gs_IMMU2_LCCU_Mng.ESBattMng[i].RemainCap = 100;

		gs_IMMU2_LCCU_Mng.ESBattMng[i].fMaxAlowPwr_Disch_ACDC = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].fMaxAlowPwr_Disch_DCDC = 0;

		gs_IMMU2_LCCU_Mng.ESBattMng[i].fMaxAlowPwr_Chg_ACDC = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].fMaxAlowPwr_Chg_MPPT = 0;



		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.u8ExistStat = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.u8Valid = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.u8CommFailAlm = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.u8CommAlmDelay = 0;

		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_ProState = 0;			//0x32
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_WorkState = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_ChgState = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_WorkMode = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_Limit_ChargeCurrent = 0;		//0x33
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_Limit_DischargeCurrent = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_Limit_ChargePower = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_Limit_DischargePower = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_Limit_ChargeVoltage = 0;	//0x34
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_Limit_DischargeVoltage = 0;
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_SumVolt = 0;		//0x3C
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_OutCurrent = 0;	
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_OutPower = 0;	
		gs_IMMU2_LCCU_Mng.ESBattMng[i].BattInfoFromCan.BMS_OutVolt = 0;

		gs_IMMU2_LCCU_Mng.ESBattMng[i].u8IsAllowChgOrDischgFromDI = 0;

		gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[i] = DO_OFF;
	}
	

	gs_IMMU2_LCCU_Mng.u8ACInputMccbCtrl = DO_OFF;
	gs_IMMU2_LCCU_Mng.u8ACOutputMccbCtrl = DO_OFF;
	

	gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_Delay = 0;
	gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No = 0;

	gs_IMMU2_LCCU_Mng.u8NeedRptSubDevVersion_Delay = 0;
	gs_IMMU2_LCCU_Mng.u8NeedRptSubDevVersion_No = 0;

	for(i = 0; i < ENJI_M_S_COMM_RCV_BUF_NUM; i++)
	{
		gs_IMMU2_LCCU_Mng.RcvBuf[i].RecvBuff = IMMU2_LCCU_RecvBuff[i];
	}	

	gs_IMMU2_LCCU_Mng.u8NeedAckPcsCmdResult = 0;
	gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn_Num = 0;

	gs_IMMU2_LCCU_Mng.u8ManualIsAcDispatchMode = FALSE;
	gs_IMMU2_LCCU_Mng.u8TaskIsAcDispatchMode = FALSE;

	gs_IMMU2_LCCU_Mng.u8IsTrigger = FALSE;
	gs_IMMU2_LCCU_Mng.u8CanMsgType = 0;
	gs_IMMU2_LCCU_Mng.u8CANRawSendCount = 0;
	gs_IMMU2_LCCU_Mng.u8CANRawSendInterval = 3;
	gs_IMMU2_LCCU_Mng.u16CANRawSendTimeOUTCount = 0;
	gs_IMMU2_LCCU_Mng.u16CANRawSendTimeOUT = 180;
	for(i = 0; i < CAN_MSG_MAX_NUM; i++)
	{
		gs_IMMU2_LCCU_Mng.CANRawSendMsg.IsExist[i] = FALSE;
		gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Id = 0;
	}
}

UCHAR Immu2_Lccu_Comm_Send(unsigned char* pSendData, unsigned short iSize)
{
	UINT8 u8Result;

	//if((gEvChargeInfo.u8RcvYdn23Flag == RcvYdn23Flag_NET)
	//	|| (gEvChargeInfo.u8RcvYdn23Flag_UART1 == RcvYdn23Flag_NET))
	{
		u8Result = NET_Send(pSendData, iSize, 100);

		if (u8Result == TRUE)
		{
			//for (u16Idx = 0; u16Idx < 1000; u16Idx++)
			//{
			//	//gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
			//	gSelfTest.fSetTaskID(Task_ID_Uart1);
			//	OSTimeDly(3);				
			//}
			gs_EthAppCommMng.i16CommFailTimes++;
		}
		else
		{
			gs_EthAppCommMng.i16CommFailTimes++;
		}

		return u8Result;
	}

//#ifdef DEBUG_LCCU_IMMU2_COMM
//	gs_IMMU2_LCCU_Mng.u8TxIsBusy = TRUE;
//	dv_Uart1.fSendDataTXY(pSendData, iSize);
//#else 	
//	gs_DisplayCtrl.u8TxIsBusy = TRUE;
//	dv_Uart6.fSendDataTXY(pSendData, iSize);
//#endif
}

/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
UINT8 UnpackCfgLccuParas(UINT8 BufNo)                       //设置单个IMSU-D配置参数（CFGS）
{
	__XXX_UNION_VALUE unTempVal;
	UINT8  ucRtn, ucSubCmd, uctemp, i;
	UINT16 u16DataLen, u16temp;
	UINT32 u32temp;
	time_t tmset;
	struct tm  tmNow;

	INT32 Rcv_Counter,Sys_Counter;

	struct tm  tmNow_2;


	
	ucRtn = 0x00;  //no error

	u16DataLen =((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_5];
	
	ucSubCmd = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+0];	

	gs_IMMU2_LCCU_Mng.u8AckCfgCmd = ucSubCmd;

	//更新从机本地时间
	if((gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].u16RcvContentLen <  2)
		|| (u16DataLen < 2))
	{
		ucRtn = 0x03;  //format error
		return ucRtn;
	}	

		switch(ucSubCmd)
		{
			case 0x02:
				if(u16DataLen != 5)
				{
					return 0x03;   //format error
				}
				u32temp = ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1]<<24) 
						+ ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2]<<16)
						+ ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3]<<8)
						+ ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4]);
				
				//tmset= ULONG_SUB_SLONG( (long)u32temp, -(long)(gRTCData.localtimezone * MIN_SECONDS));
				tmset= ULONG_SUB_SLONG( (long)u32temp, -(long)(0 * MIN_SECONDS));
				Rcv_Counter = tmset;

				gRTCData.fGetLocalTime(&tmNow_2);
				tmNow_2.tm_year -= 1900;
				tmNow_2.tm_mon -= 1;
				Sys_Counter = mktime_r(&tmNow_2);
				if (ABS(Rcv_Counter - Sys_Counter) > 50)
				{
					gmtime_r(&tmset, &tmNow);
					tmNow.tm_year += 1900;
					tmNow.tm_mon += 1;

					gRTCData.fSetLocalTime(&tmNow);
				}

				
				break;

			case 0x03:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[1] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[1] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1, unTempVal, NULL, BY_E2PROM);
				}
				if(gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2])
				{
					gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[0] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[0];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+1, unTempVal, NULL, BY_E2PROM);
				}

//bit.0  EPO Voltage Level (0- High, 1-Low)
				uctemp = gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.EPO_Level;
				ExGetSigInfo(E_TYPE_SYS, ID_u8EPOAlmVltLEVEL, &unTempVal, NULL, BY_E2PROM);
				if (uctemp == 0x00) //0 高电平
				{
					if (unTempVal.chtemp[0] != 1)
					{
						unTempVal.chtemp[0] = 1;//设置为1
						ExSetSigInfo(E_TYPE_SYS, ID_u8EPOAlmVltLEVEL, unTempVal, NULL, BY_E2PROM);
					}
				}
				else
				{
					if (unTempVal.chtemp[0] != 0)
					{
						unTempVal.chtemp[0] = 0;
						ExSetSigInfo(E_TYPE_SYS, ID_u8EPOAlmVltLEVEL, unTempVal, NULL, BY_E2PROM);
					}
				}

//bit.1  Door Open(0-Disabled, 1-Enabled)
				uctemp = gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.Door_Open;
				ExGetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_1, &unTempVal, NULL, BY_E2PROM);
				if (uctemp == 0x01)	 //门禁使能
				{
					if (unTempVal.chtemp[0] != FALSE)
					{
						unTempVal.chtemp[0] = FALSE;
						ExSetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_1, unTempVal, NULL, BY_E2PROM);
						ExSetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_2, unTempVal, NULL, BY_E2PROM);
					}					
				}
				else
				{
					if (unTempVal.chtemp[0] != TRUE)
					{
						unTempVal.chtemp[0] = TRUE;
						ExSetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_1, unTempVal, NULL, BY_E2PROM);
						ExSetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_2, unTempVal, NULL, BY_E2PROM);
					}					
				}


//bit.2  SPD (0-Disabled, 1-Enabled)(交流防雷)
				uctemp = gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.SPD;
				ExGetSigInfo(E_TYPE_SYS, ID_u8SPDAlmMask, &unTempVal, NULL, BY_E2PROM);
				if (uctemp == 0x01)	 //SPD使能
				{
					if (unTempVal.chtemp[0] != 0)
					{
						unTempVal.chtemp[0] = 0;
						ExSetSigInfo(E_TYPE_SYS, ID_u8SPDAlmMask, unTempVal, NULL, BY_E2PROM);
					}
				}
				else
				{
					if (unTempVal.chtemp[0] != 1)
					{
						unTempVal.chtemp[0] = 1;				//禁用 
						ExSetSigInfo(E_TYPE_SYS, ID_u8SPDAlmMask, unTempVal, NULL, BY_E2PROM);
					}
				}


//bit.3  AC Grid Input MCB Breaker(0-Disabled, 1-Enabled)(交流输入断路器状态)
				//uctemp = (gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] >> 3) & 0x01;
				uctemp = gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.AC_Input_Breaker;
				if(gEvChargeInfo.u8ACGridInputMCB_En != uctemp)
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8ACGridInputBreak_En, unTempVal, NULL, BY_E2PROM);
				}


//bit.4  ACLoad Output MCB(0-Disabled, 1-Enabled)(负载断路器)		
				//uctemp = (gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] >> 4) & 0x01;
				uctemp = gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.ACLoad_Output_Breaker;
				if(gEvChargeInfo.u8ACLoadOutputMCB_En != uctemp)
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8ACLoadOutputBreak_En, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.LeakageSensorEn;
				if(gEvChargeInfo.u8LeakageSensorEn != uctemp)
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8LeakageSensorEn, unTempVal, NULL, BY_E2PROM);
				}


				// LowVoltMCBBreaker Level
				uctemp = gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.LowVoltMCBBreaker_Level;
				ExGetSigInfo(E_TYPE_SYS, ID_u8LowVoltAlmVltLEVEL, &unTempVal, NULL, BY_E2PROM);
				if (uctemp == 0x00) //0 高电平
				{
					if (unTempVal.chtemp[0] != 1)
					{
						unTempVal.chtemp[0] = 1;//设置为1
						ExSetSigInfo(E_TYPE_SYS, ID_u8LowVoltAlmVltLEVEL, unTempVal, NULL, BY_E2PROM);
					}
				}
				else
				{
					if (unTempVal.chtemp[0] != 0)
					{
						unTempVal.chtemp[0] = 0;
						ExSetSigInfo(E_TYPE_SYS, ID_u8LowVoltAlmVltLEVEL, unTempVal, NULL, BY_E2PROM);
					}
				}

				// N_LineContactor Level    N线接触器控制
				uctemp = gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.N_LineContactor_Level;
				ExGetSigInfo(E_TYPE_SYS, ID_u8NLINEAlmVltLEVEL, &unTempVal, NULL, BY_E2PROM);
				if (uctemp == 0x00) //0 高电平
				{
					if (unTempVal.chtemp[0] != 1)
					{
						unTempVal.chtemp[0] = 1;//设置为1
						ExSetSigInfo(E_TYPE_SYS, ID_u8NLINEAlmVltLEVEL, unTempVal, NULL, BY_E2PROM);
					}
				}
				else
				{
					if (unTempVal.chtemp[0] != 0)
					{
						unTempVal.chtemp[0] = 0;
						ExSetSigInfo(E_TYPE_SYS, ID_u8NLINEAlmVltLEVEL, unTempVal, NULL, BY_E2PROM);
					}
				}
				break;
			case 0x04:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[1] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[1] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+2, unTempVal, NULL, BY_E2PROM);
				}
				if(gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2])
				{
					gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[0] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2];
					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[0];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+3, unTempVal, NULL, BY_E2PROM);
				}

				//uctemp = (gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] >> 6) & 0x01;
				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8BattGrp1_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp1_En, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt2Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8BattGrp2_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp2_En, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt3Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8BattGrp3_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp3_En, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8BattGrp4_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp4_En, unTempVal, NULL, BY_E2PROM);
				}


				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.ABU1Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8ABU1_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8ABU1_En, unTempVal, NULL, BY_E2PROM);
				}
				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.ABU2Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8ABU2_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8ABU2_En, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.ABU3Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8ABU3_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8ABU3_En, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.ABU4Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8ABU4_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8ABU4_En, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8IMEU1_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8IMEU1_En, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist;	
				ExGetSigInfo(E_TYPE_SYS, ID_u8IMEU2_En, &unTempVal, NULL, BY_E2PROM);
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8IMEU2_En, unTempVal, NULL, BY_E2PROM);
				}
				break;

			case 0x05:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[1] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[1] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+4, unTempVal, NULL, BY_E2PROM);
				}
				if(gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2])
				{
					gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[0] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[0];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+5, unTempVal, NULL, BY_E2PROM);
				}

				uctemp = gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt1ChgForbitDiEn;
				if(gEvChargeInfo.u8BattGrpXAlowChgOrDischgDI_En[0] != uctemp)
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp1AlowChgOrDischgDI_En, unTempVal, NULL, BY_E2PROM);
				}

				//uctemp = (gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] >> 6) & 0x01;
				uctemp = gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt2ChgForbitDiEn;
				if(gEvChargeInfo.u8BattGrpXAlowChgOrDischgDI_En[1] != uctemp)
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp2AlowChgOrDischgDI_En, unTempVal, NULL, BY_E2PROM);
				}

				//uctemp = (gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] >> 7) & 0x01;
				uctemp = gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt3ChgForbitDiEn;
				if(gEvChargeInfo.u8BattGrpXAlowChgOrDischgDI_En[2] != uctemp)
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp3AlowChgOrDischgDI_En, unTempVal, NULL, BY_E2PROM);
				}

				//uctemp = (gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] >> 8) & 0x01;
				uctemp = gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt4ChgForbitDiEn;
				if(gEvChargeInfo.u8BattGrpXAlowChgOrDischgDI_En[3] != uctemp)
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp4AlowChgOrDischgDI_En, unTempVal, NULL, BY_E2PROM);
				}
				break;

			case 0x06:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[1] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[1] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+6, unTempVal, NULL, BY_E2PROM);
				}
				if(gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2])
				{
					gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[0] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[0];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+7, unTempVal, NULL, BY_E2PROM);
				}

				break;

			case 0x07:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[1] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[1] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+8, unTempVal, NULL, BY_E2PROM);
				}
				if(gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2])
				{
					gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[0] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[0];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+9, unTempVal, NULL, BY_E2PROM);
				}

				break;
			case 0x08:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[1] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[1] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+10, unTempVal, NULL, BY_E2PROM);
				}
				if(gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2])
				{
					gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[0] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2];

					unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[0];
					ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1+11, unTempVal, NULL, BY_E2PROM);
				}

				for(i = 0; i < 8; i++)
				{
					u16temp = (0x01<<i);	
					if(gEvChargeInfo.u16AttributesBitsMap_6.INT_FIELD & u16temp)
					{
						uctemp = 0;
					}
					else
					{
						uctemp = 1;
					}				
					ExGetSigInfo(E_TYPE_IMIU1+i, ID_u8JYMAlmStatMask, &unTempVal, NULL, BY_E2PROM);
					if(unTempVal.chtemp[0] != uctemp)
					{
						unTempVal.chtemp[0] = uctemp;
						ExSetSigInfo(E_TYPE_IMIU1+i, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
					}
				}
				//只支持8路
				for(; i < 10; i++)
				{
					/*u16temp = (0x01<<i);	
					if(gEvChargeInfo.u16AttributesBitsMap_6.INT_FIELD & u16temp)
					{
						uctemp = 0;
					}
					else
					{
						uctemp = 1;
					}				
					ExGetSigInfo(E_TYPE_IMIU1+i, ID_u8JYMAlmStatMask, &unTempVal, NULL, BY_E2PROM);
					if(unTempVal.chtemp[0] != uctemp)
					{
						unTempVal.chtemp[0] = uctemp;
						ExSetSigInfo(E_TYPE_IMIU1+i, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
					}*/
				}
				

				/*if(gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU1Exist == 1)
				{
					uctemp = 0;
				}
				else
				{
					uctemp = 1;
				}				
				ExGetSigInfo(E_TYPE_IMIU1, ID_u8JYMAlmStatMask, &unTempVal, NULL, BY_E2PROM);
				if(unTempVal.chtemp[0] != uctemp)
				{
					unTempVal.chtemp[0] = uctemp;
					ExSetSigInfo(E_TYPE_IMIU1, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
				}*/

				break;
			
			case 0x0B:

				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed, &unTempVal, NULL, BY_E2PROM);
				if (unTempVal.chtemp[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.chtemp[0]= gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x0C:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}
				/*u16temp =((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2];
				ftemp = (float)u16temp / 10;
				ExGetSigInfo(E_TYPE_SYS, ID_fMaxOutVolt, &unTempVal, NULL, BY_E2PROM);
				if (ABS(unTempVal.ftemp - ftemp) > 0.05)
				{
					unTempVal.ftemp = ftemp;
					ExSetSigInfo(E_TYPE_SYS, ID_fMaxOutVolt, unTempVal, NULL, BY_E2PROM);
				}
				ftemp += 10;
				ExGetSigInfo(E_TYPE_SYS, ID_fSysSetOverVolt, &unTempVal, NULL, BY_E2PROM);
				if (ABS(unTempVal.ftemp - ftemp) > 0.05)
				{
					unTempVal.ftemp = ftemp;
					ExSetSigInfo(E_TYPE_SYS, ID_fSysSetOverVolt, unTempVal, NULL, BY_E2PROM);
				}	*/
				break;
			case 0x09:

				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed_2, &unTempVal, NULL, BY_E2PROM);
				if (unTempVal.chtemp[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.chtemp[0]= gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed_2, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x0A:

				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed_3, &unTempVal, NULL, BY_E2PROM);
				if (unTempVal.chtemp[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.chtemp[0]= gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed_3, unTempVal, NULL, BY_E2PROM);
				}				
				break;


			case 0x11:
			case 0x12:
			case 0x13:
			case 0x14:
			case 0x15:
			case 0x16:
			case 0x17:
			case 0x18:
			case 0x19:
			case 0x1A:
			case 0x1B:
			case 0x1C:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				i = ucSubCmd - 0x11;
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
				
				ExGetSigInfo(E_TYPE_SYS, ID_u8GRP1_MODULE_NUM+i, &unTempVal, NULL, BY_E2PROM);
	
				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0]= uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8GRP1_MODULE_NUM+i, unTempVal, NULL, BY_E2PROM);
				}	
				
				break;

			case 0x26:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_u8FanMode1, &unTempVal, NULL, BY_E2PROM);
				if (unTempVal.chtemp[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.chtemp[0]= gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8FanMode1, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x27:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_u8System_Start_Temp, &unTempVal, NULL, BY_E2PROM);
				if (unTempVal.chtemp[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.chtemp[0]= gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8System_Start_Temp, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x28:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_u8System_Ultra_Temp, &unTempVal, NULL, BY_E2PROM);
				if (unTempVal.chtemp[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.chtemp[0]= gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8System_Ultra_Temp, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x20:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_u8FanMode2, &unTempVal, NULL, BY_E2PROM);
				if (unTempVal.chtemp[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.chtemp[0]= gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8FanMode2, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x21:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_fFanStartTemp2, &unTempVal, NULL, BY_E2PROM);
				uctemp = (UINT8)(unTempVal.ftemp + 0.0001);
				if (uctemp != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.ftemp= (float)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_fFanStartTemp2, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x22:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_fFanFullTemp2, &unTempVal, NULL, BY_E2PROM);
				uctemp = (UINT8)(unTempVal.ftemp + 0.0001);
				if (uctemp != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.ftemp= (float)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_fFanFullTemp2, unTempVal, NULL, BY_E2PROM);
				}					
				break;
			case 0x23:
				if(u16DataLen != 3)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_u8FanMode3, &unTempVal, NULL, BY_E2PROM);
				if (unTempVal.chtemp[0] != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.chtemp[0]= gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_u8FanMode3, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x24:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_fFanStartTemp3, &unTempVal, NULL, BY_E2PROM);
				uctemp = (UINT8)(unTempVal.ftemp + 0.0001);
				if (uctemp != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.ftemp= (float)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_fFanStartTemp3, unTempVal, NULL, BY_E2PROM);
				}				
				break;
			case 0x25:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				ExGetSigInfo(E_TYPE_SYS, ID_fFanFullTemp3, &unTempVal, NULL, BY_E2PROM);
				uctemp = (UINT8)(unTempVal.ftemp + 0.0001);
				if (uctemp != gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1])
				{
					unTempVal.ftemp= (float)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					ExSetSigInfo(E_TYPE_SYS, ID_fFanFullTemp3, unTempVal, NULL, BY_E2PROM);
				}					
				break;

			case 0x29:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
				ExGetSigInfo(E_TYPE_SYS, ID_u8ModuleType, &unTempVal, NULL, BY_E2PROM);

				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0]= uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8ModuleType, unTempVal, NULL, BY_E2PROM);
				}	

				break;

			case 0x2A:
				if(u16DataLen != 2)
				{
					return 0x03;	//format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
				ExGetSigInfo(E_TYPE_SYS, ID_u8TaskIsAcDispatchMode, &unTempVal, NULL, BY_E2PROM);

				if (uctemp != unTempVal.chtemp[0])
				{
					unTempVal.chtemp[0]= uctemp;
					ExSetSigInfo(E_TYPE_SYS, ID_u8TaskIsAcDispatchMode, unTempVal, NULL, BY_E2PROM);
				}	
				// if(1 == uctemp)
				// {
				// 	gs_IMMU2_LCCU_Mng.u8TaskIsAcDispatchMode = TRUE;
				// }
				// else
				// {
				// 	gs_IMMU2_LCCU_Mng.u8TaskIsAcDispatchMode = FALSE;
				// }
				break;

			default:
				ucRtn = 0x02; //0x02	CMD invalid
				break;

		} // end: switch(ucSubCmd)



	return ucRtn;
}


/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
void UnpackIMMU2Stat(UINT8 BufNo)                             //USYS
{
	//__XXX_UNION_VALUE unTempVal;
	UINT8 i;
	UINT16 u16DataLen, u16temp, u16Pos;
	INT16 i16Value;
	UINT32 u32temp;
	//time_t tmset;
	//struct tm  tmNow;
	//float ftemp;

	u16DataLen =((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_5];

	if((gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].u16RcvContentLen <  12)
		|| (u16DataLen < 12))
	{		
		return;
	}	
	
	u16Pos = ENJI_PROT_DATA+0;
	gs_IMMU2_LCCU_Mng.IMMU2Stat.FaultedStatus = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos++];

	//System Alarm1 Bit map
	u16temp  = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
	u16Pos += 2;
	gs_IMMU2_LCCU_Mng.IMMU2Stat.PVSPdStatus = (UINT8)(u16temp & 0X01);

	if(gs_IMMU2_LCCU_Mng.IMMU2Stat.PVSPdStatus)  //PV-SPD alarm
	{
		if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] == DO_ON)
		{
		//	__A_PV_INPUT_OFF();
		}
	}
	else
	{
		if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] == DO_OFF)
		{
		//	__A_PV_INPUT_ON();
		}
	}

	gs_IMMU2_LCCU_Mng.IMMU2Stat.OPENDOOR  = (UINT8)((u16temp & 0x02) >> 1);        //by lxr01202   新增 门禁打开，需要关闭光伏动作

	if((gs_IMMU2_LCCU_Mng.IMMU2Stat.OPENDOOR == 1) && (gEvChargeInfo.u8TEST_MODE_FLAG == FALSE))  //DOOR alarm
	{
		__MPPT_1_INPUT_OFF();
		__MPPT_2_INPUT_OFF();
	}


	//RESERVE 32
	u16Pos += 32;

	//Backup Battery Status
	u32temp  = ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<24) 
		+ ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1]<<16)
		+ ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+2]<<8)
		+ (UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+3];
	u16Pos += 4;
	
	//BATT_GRP_MAX_NUM
	gs_IMMU2_LCCU_Mng.IMMU2Stat.BattStatus[0] = (u32temp&0x0f);
	gs_IMMU2_LCCU_Mng.IMMU2Stat.BattStatus[1] = ((u32temp>>4)&0x0f);
	gs_IMMU2_LCCU_Mng.IMMU2Stat.BattStatus[2] = ((u32temp>>8)&0x0f);
	gs_IMMU2_LCCU_Mng.IMMU2Stat.BattStatus[3] = ((u32temp>>12)&0x0f);

	for(i = 0; i < BATT_GRP_MAX_NUM; i++)
	{
		//A
		u16temp  = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
		gs_IMMU2_LCCU_Mng.ESBattMng[i].fOutVolt = (float)u16temp/10;                  //电池输出电压
		u16Pos += 2;

		i16Value  = ((INT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
		gs_IMMU2_LCCU_Mng.ESBattMng[i].fOutCurr = (float)i16Value/10;                  //电池输出电流
		u16Pos += 2;

		gs_IMMU2_LCCU_Mng.ESBattMng[i].RemainCap = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos];
		u16Pos += 1;

		u16temp  = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
		gs_IMMU2_LCCU_Mng.ESBattMng[i].MaxAllowChargeVoltage = (float)u16temp/10;                //最大允许充电电压
		u16Pos += 2;

		i16Value  = ((INT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
		gs_IMMU2_LCCU_Mng.ESBattMng[i].MaxAllowChargeCurrent = (float)i16Value/10;               //最大允许充电电流
		u16Pos += 2;

		//RESERVE
		u16Pos += 4;
	}

	for(i = 0; i < BATT_GRP_MAX_NUM; i++)
	{
		u16temp  = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
		gs_IMMU2_LCCU_Mng.ESBattMng[i].fMaxAlowPwr_Disch_ACDC = (float)u16temp/10;
		u16Pos += 2;

		u16temp  = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
		gs_IMMU2_LCCU_Mng.ESBattMng[i].fMaxAlowPwr_Disch_DCDC = (float)u16temp/10;
		u16Pos += 2;

		u16temp  = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
		gs_IMMU2_LCCU_Mng.ESBattMng[0].fMaxAlowPwr_Chg_ACDC = (float)u16temp/10;
		u16Pos += 2;

		i16Value = ((INT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
		gs_IMMU2_LCCU_Mng.ESBattMng[i].CtrlReactivePower = i16Value/10;

		if(ABS(gs_IMMU2_LCCU_Mng.ESBattMng[i].LastCtrlReactivePower-gs_IMMU2_LCCU_Mng.ESBattMng[i].CtrlReactivePower) > 1)
		{
			gs_IMMU2_LCCU_Mng.ESBattMng[i].LastCtrlReactivePower = gs_IMMU2_LCCU_Mng.ESBattMng[i].CtrlReactivePower;
			gEvChargeInfo.u8SetWorkModeDelay = 0;
		}
		u16Pos += 2;
		
		//RESERVE
		u16Pos += 2;
	}
	
}


/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
UINT16 GetAttributesBit(void)
{
	UINT16 ug16Val;

	//bit.0~ bit.1 EPO Voltage Level (0x00- High, 0x01-Low)
	//bit.2~ bit.3 Door Open(0x00-Disabled, 0x01-Enabled)
	//bit.4~ bit.5 SPD (0x00-Disabled, 0x01-Enabled)
	
	ug16Val = 0;
	

	
	return ug16Val;
}
UINT16 GetAttributesBit_RectGrpExistStat(void)
{
	UINT8 i;
	UINT16 ug16Val;
	__XXX_UNION_VALUE	ungVal;

	for(i = 0; i < 6; i++)
	{
		ug16Val = 0;
		ExGetSigInfo(E_TYPE_SYS, ID_u8GRP1_MODULE_NUM + i, &ungVal, NULL, BY_E2PROM);
		if (ungVal.chtemp[0] > 0)
		{
			ug16Val |= (0x01<<(i));
		}
	}

	return ug16Val;
}
/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
UCHAR LccuRptParas(void)                  //CFGG
{
	__XXX_UNION_VALUE unTempVal;

	UINT8  uctemp, u8Idx, u8Rtn;
	UINT16 u16DataCount, u16temp, ii;
	UINT32 u32temp;
	//UINT8 pFloatArarry[8];
	//struct tm  tmNow;
	//time_t now;




	gs_IMMU2_LCCU_Mng.SendBuff[0] = ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = 0x01;

	u16DataCount = ENJI_PROT_DATA;

	//9	Controller Address	1	BYTE	Range: 1~32
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = g_u8IMMU_LCCU_COMM_Addr;
	
	//1 Controller Barcode	24	STRING	
	for (u8Idx = 0; u8Idx < 24; u8Idx++)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
	}

	//2	Mac Address	8	BYTE[]
	for (u8Idx = 0; u8Idx < 6; u8Idx++)
	{
		ExGetSigInfo(E_TYPE_SYS, ID_u8Mac_1 + u8Idx, &unTempVal, NULL, BY_SRAM);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = unTempVal.chtemp[0];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;

	//8	SW version	8	STRING	
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = '0'+mVERHIGH;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = '.';

	uctemp = (mVERLOW/10);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = '0'+uctemp;

	uctemp = (mVERLOW%10);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = '0'+uctemp;

	for (u8Idx = 0; u8Idx < 4; u8Idx++)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
	}

	//	RTC Time
	u32temp = (UINT32)gRTCData.fGetSecondsGMT2LOCAL();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>24)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>16)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp)&0xff);

	//6	Attributes bits map 	
	/*u16temp = GetAttributesBit();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp)&0xff);*/
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[0];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[0];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[0];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[0];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[0];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[0];

	


	
	ExGetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed_2, &unTempVal, NULL, BY_E2PROM);         //System Fan-2 Speed Level PWM2      
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed_3, &unTempVal, NULL, BY_E2PROM);         //System Fan-3 Speed Level PWM3      
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed, &unTempVal, NULL, BY_E2PROM);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];                //System Fan-1 Speed Level PWM1

	
	u16temp = (UINT16)(gEvChargeInfo.fMaxOutVolt *10);                                //System Max Volt Level
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//预留5个16bit参数
	for(u8Idx = 0; u8Idx < 4; u8Idx++)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  0;
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  0;
	}
	


	/*u16temp = GetAttributesBit_RectGrpExistStat();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp)&0xff);*/
	for(u8Idx = 0; u8Idx < 12; u8Idx++)            
	{
		ExGetSigInfo(E_TYPE_SYS, ID_u8GRP1_MODULE_NUM + u8Idx, &unTempVal, NULL, BY_E2PROM);        //PM Group1 Number --PM Group12 Number
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];
	}
	
	// ExGetSigInfo(E_TYPE_SYS, ID_u8FanMode2, &unTempVal, NULL, BY_E2PROM);        
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  0;
	ExGetSigInfo(E_TYPE_SYS, ID_fFanStartTemp2, &unTempVal, NULL, BY_E2PROM);                
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  (UINT8)unTempVal.ftemp;
	ExGetSigInfo(E_TYPE_SYS, ID_fFanFullTemp2, &unTempVal, NULL, BY_E2PROM);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  (UINT8)unTempVal.ftemp;

	ExGetSigInfo(E_TYPE_SYS, ID_u8FanMode3, &unTempVal, NULL, BY_E2PROM);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  0;
	ExGetSigInfo(E_TYPE_SYS, ID_fFanStartTemp2, &unTempVal, NULL, BY_E2PROM);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  (UINT8)unTempVal.ftemp;
	ExGetSigInfo(E_TYPE_SYS, ID_fFanFullTemp3, &unTempVal, NULL, BY_E2PROM);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  (UINT8)unTempVal.ftemp;

	ExGetSigInfo(E_TYPE_SYS, ID_u8FanMode1, &unTempVal, NULL, BY_E2PROM);                //System Dehumidification Fan-1 Work Mode(PWM1)
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  0;
	ExGetSigInfo(E_TYPE_SYS, ID_u8System_Start_Temp, &unTempVal, NULL, BY_E2PROM);         //System Over Temp Point(for system cooling fan-1 start)
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8System_Ultra_Temp, &unTempVal, NULL, BY_E2PROM);        //System Ultra Temp Point(for system cooling fan-1 full-speed)
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];

	ExGetSigInfo(E_TYPE_SYS, ID_u8ModuleType, &unTempVal, NULL, BY_E2PROM);               //PCS Type Setting
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];
	ExGetSigInfo(E_TYPE_SYS, ID_u8TaskIsAcDispatchMode, &unTempVal, NULL, BY_E2PROM);     //PCS  DC/AC Adjust P+Q
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  unTempVal.chtemp[0];

	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送	
	u8Rtn = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//OSTimeDly(35);

	return u8Rtn;
}


/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
UINT32 u16GetAlm()
{
	void * PEInfo;
	UINT32 ug16Val = 0;
	PEInfo = GetgEquipInfo(E_TYPE_SYS);
	if ((((EV_CHARGE_INFO*)PEInfo)->u8DoorAlm))
	{
		ug16Val |= 0x01;  //bit.0: Door Open Alarm( 0-Normal, 1-Alarm)
	}

	PEInfo = GetgEquipInfo(E_TYPE_SYS);
	if ((((EV_CHARGE_INFO*)PEInfo)->u8EPOAlm))                                             //epo
	{
		ug16Val |= (0x01<<1);//bit.1: EPO Alarm( 0-Normal, 1-Alarm)
	}
	PEInfo = GetgEquipInfo(E_TYPE_SYS);
	if ((((EV_CHARGE_INFO*)PEInfo)->u8SPDAlm))                                             //spd
	{
		ug16Val |= (0x01 << 2);//bit.2: SPD Alarm( 0-Normal, 1-Alarm)
	}


    if((gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.LeakageSensorEn == 1)                 //bit.8: Leakage Sensor Alarm( 0-Normal, 1-Alarm)
	  && (gs_DCLeankageSensorMng.u16DCLeakageCurrentSensorOkStat == FALSE))
	{
		ug16Val |= (0x01 << 8);
	}
	
	//PE
	if (gs_EsSampleInfo.PE_Connect)                                  //bit.9: PE Communication Fail Alarm( 0-Normal, 1-Alarm)
	{
		ug16Val |= (0x01 << 9);
	}


	//PEInfo = GetgEquipInfo(E_RECT_INFO);
	//if (gs_CANComm.u8IsAllCommFail == TRUE)
	//{
	//	ug16Val |= (0x01 << 3);						//bit.8: All Rectifier Communication Fail Alarm (0-Normal, 1-Alarm)
	//}
	//else  if (//(gs_CANComm.u8IsACOver == TRUE) || (gs_CANComm.u8IsACUnder == TRUE) ||
	//	 (((CANCommData_TypeDef*)PEInfo)->GrpData[GRP_B].u8IsNoNormalRect) 
	//	&& (((CANCommData_TypeDef*)PEInfo)->GrpData[GRP_A].u8IsNoNormalRect)
	//	&& (((CANCommData_TypeDef*)PEInfo)->GrpData[GRP_E_DCDC].u8IsNoNormalRect)
	//	&& (((CANCommData_TypeDef*)PEInfo)->GrpData[GRP_F_DCDC].u8IsNoNormalRect))
	//{
	//	ug16Val |= (0x01 << 3);						//bit.3: All Rectifier Fail Alarm( 0-Normal, 1-Alarm)
	//}
	//else  if (gs_CANComm.u8IsACOver == TRUE)
	//{
	//	ug16Val |= (0x01 << 10);
	//}
	//else  if (gs_CANComm.u8IsACUnder == TRUE)
	//{
	//	ug16Val |= (0x01 << 11);
	//}
	//
	//if(g_RingAllInfo.bIsRingCfgErr == TRUE)
	//{
	//	ug16Val |= (0x01 << 12);
	//}

	/*
	bit.4: System over temp Alarm( 0-Normal, 1-Alarm)
	bit.5: System Fan Failed Alarm( 0-Normal, 1-Alarm)
	bit.6: Other Alarm( 0-Normal, 1-Alarm)
	bit 7: Mains Fail Alarm (0-Normal, 1-Alarm)
	bit.8: All Rectifier Communication Fail Alarm (0-Normal, 1-Alarm)
	bit.9: Ground Fault Alarm (0-Normal, 1-Alarm)
	bit10:AC over Volt
	bit11:AC Lower Volt
	bit.12: Isolation Communication Fail Alarm (0-Normal, 1-Alarm)
	*/

	

	if (gs_CANComm.u8IsACOver == TRUE)         
	{
		ug16Val |= (0x01 << 10);
	}
	if (gs_CANComm.u8IsACUnder == TRUE)
	{
		ug16Val |= (0x01 << 11);
	}

	//bit12:AC Grid Input MCB Disconnect (0-Normal, 1-Alarm)(交流输入断路器状态)
	if (gs_EsSampleInfo.AC_Input_Breaker)                             
	{
		ug16Val |= (0x01 << 12);
	}

	//bit13:ACload Output MCB Disconnect (0-Normal, 1-Alarm)(交流负载输出断路器状态)
	if (gs_EsSampleInfo.ACLoad_Output_Breaker)              
	{
		ug16Val |= (0x01 << 13);
	}
	//bit14.AC Contactor Alarm(0-Normal, 1-Alarm) (交流输入接触器状态)
	if (gs_EsSampleInfo.AC_Input_Contactor)
	{
		ug16Val |= (0x01 << 14);
	}

	if (gIMEU_Info[0].u8CommError == TRUE)
	{
		ug16Val |= (0x01 << 15);
	}
	if (gIMEU_Info[1].u8CommError == TRUE)
	{
		ug16Val |= (0x01 << 16);
	}

	/*
	bit17.ABU_1 CommFail Alarm(0-Normal, 1-Alarm)
	bit18.ABU_1 ConfiglFail Alarm(0-Normal, 1-Alarm)
	bit19.ABU_3 CommFail Alarm(0-Normal, 1-Alarm)
	bit20.ABU_4 CommFail Alarm(0-Normal, 1-Alarm)
	*/
	if (ABU->CommFailed == TRUE)
	{
		ug16Val |= (0x01 << 17);
	}

	if (ABU->ConfigFailed == TRUE)
	{
		ug16Val |= (0x01 << 18);
	}

	if(gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.SmokeDIEn == 1)
	{
		if (gs_EsSampleInfo.SmokeDI)
		{
			ug16Val |= (0x01 << 21);
		}
	}
	if(gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.FloodingDIEn == 1)
	{
		if (gs_EsSampleInfo.FloodingDI)
		{
			ug16Val |= (0x01 << 22);
		}
	}
	if(gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.ToppleDIEn == 1)
	{
		if (gs_EsSampleInfo.ToppleDI)
		{
			ug16Val |= (0x01 << 23);
		}
	}

	if(gs_EsSampleInfo.Low_Volt_Breaker == TRUE)
	{
		ug16Val |= (0x01 << 24);
	}

	if(ABU->_0x1115Byte2_info.bit.NLineContactorErr == 1)
	{
		ug16Val |= (0x01 << 25);
	}

	////AC INPUT
	//uctemp = BSP_u8GetDI5Status();
	////0-闭合 1-断开
	//if(uctemp == 0)
	//{
	//	ug16Val |= (0x01 << 12);
	//}

	////AC OUTPUT
	//uctemp = BSP_u8GetDI2Status();
	////0-闭合 1-断开
	//if(uctemp == 0)
	//{
	//	ug16Val |= (0x01 << 13);
	//}

	/*if(gIMIU_Info_UART4[0].u8InsltCommAlm[0] == TRUE)
	{
	ug16Val |= (0x01 << 12);
	}
	if(gIMIU_Info_UART4[0].u8JYMAlmStat[0] == TRUE)
	{
	ug16Val |= (0x01 << 13);
	}
	if(gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE)
	{
	ug16Val |= (0x01 << 14);
	}
	if(gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
	{
	ug16Val |= (0x01 << 15);
	}
	if(gIMIU_Info3.u8InsltCommAlm[0] == TRUE)
	{
	ug16Val |= (0x01 << 14);
	}
	if(gIMIU_Info3.u8JYMAlmStat[0] == TRUE)
	{
	ug16Val |= (0x01 << 15);
	}*/


	return ug16Val;
}





UINT32 u16GetAlm_2()
{
	UINT32 ug16Val = 0;

	if(gIMIU_Info_UART4[0].u8InsltCommAlm[0] == TRUE)           //RS4851
	{
		ug16Val |= (0x01 << 0);
	}
	if(gIMIU_Info_UART4[0].u8JYMAlmStat[0] == TRUE)
	{
		ug16Val |= (0x01 << 1);
	}
	if(gIMIU_Info_UART4[1].u8InsltCommAlm[0] == TRUE)
	{
		ug16Val |= (0x01 << 2);
	}
	if(gIMIU_Info_UART4[1].u8JYMAlmStat[0] == TRUE)
	{
		ug16Val |= (0x01 << 3);
	}
	if(gIMIU_Info_UART4[2].u8InsltCommAlm[0] == TRUE)
	{
		ug16Val |= (0x01 << 4);
	}
	if(gIMIU_Info_UART4[2].u8JYMAlmStat[0] == TRUE)
	{
		ug16Val |= (0x01 << 5);
	}
	if(gIMIU_Info_UART4[3].u8InsltCommAlm[0] == TRUE)
	{
		ug16Val |= (0x01 << 6);
	}
	if(gIMIU_Info_UART4[3].u8JYMAlmStat[0] == TRUE)
	{
		ug16Val |= (0x01 << 7);
	}

	if(gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE)                     //RS4852
	{
		ug16Val |= (0x01 << 8);
	}
	if(gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
	{
		ug16Val |= (0x01 << 9);
	}
	if(gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE)
	{
		ug16Val |= (0x01 << 10);
	}
	if(gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
	{	
		ug16Val |= (0x01 << 11);
	}
	if(gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE)
	{
		ug16Val |= (0x01 << 12);
	}
	if(gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
	{
		ug16Val |= (0x01 << 13);
	}
	if(gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE)
	{
		ug16Val |= (0x01 << 14);
	}
	if(gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)
	{
		ug16Val |= (0x01 << 15);
	}
	if(gIMIU_Info_UART5[0].u8RunEarth_DetectAlm[0] == TRUE)
	{
		ug16Val |= (0x01 << 28);                        //30bit位传接地告警IMIU1
	}
	// if(gIMIU_Info_UART5[1].u8RunEarth_DetectAlm[0] == TRUE)
	// {
	// 	ug16Val |= (0x01 << 29);                        //31bit位传接地告警IMIU2
	// }
	
	return ug16Val;
}




UINT32 u16GetAlm_3()
{
	UINT32 ug16Val = 0;

	//电池柜禁充禁放干接点检测(
	if(gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[0] == TRUE)
	{
		ug16Val |= (0x01 << 0);
	}
	if(gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[1] == TRUE)
	{
		ug16Val |= (0x01 << 1);
	}
	if(gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[2] == TRUE)
	{
		ug16Val |= (0x01 << 2);
	}
	if(gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[3] == TRUE)
	{
		ug16Val |= (0x01 << 3);
	}

	//电池柜直流输入接触器开关反馈信号
	if(gs_EsSampleInfo.Battry_Grp1_Input_Contactor[0] == TRUE)
	{
		ug16Val |= (0x01 << 4);
	}

	if(gs_EsSampleInfo.Battry_Grp1_Input_Contactor[1] == TRUE)
	{
		ug16Val |= (0x01 << 5);
	}
	if(gs_EsSampleInfo.Battry_Grp1_Input_Contactor[2] == TRUE)
	{
		ug16Val |= (0x01 << 6);
	}
	if(gs_EsSampleInfo.Battry_Grp1_Input_Contactor[3] == TRUE)
	{
		ug16Val |= (0x01 << 7);
	}
	//电池柜直流接入隔离开关反馈信号
	if(gs_EsSampleInfo.Battry_Grp1_Isolation_Switch[0] == TRUE)
	{
		ug16Val |= (0x01 << 8);
	}
	if(gs_EsSampleInfo.Battry_Grp1_Isolation_Switch[1] == TRUE)
	{
		ug16Val |= (0x01 << 9);
	}
	if(gs_EsSampleInfo.Battry_Grp1_Isolation_Switch[2] == TRUE)
	{
		ug16Val |= (0x01 << 10);
	}
	if(gs_EsSampleInfo.Battry_Grp1_Isolation_Switch[3] == TRUE)
	{
		ug16Val |= (0x01 << 11);
	}

	//电池柜CAN通信中断情况
	if(gs_EsSampleInfo.Batt_CanComm_Break_Alm[0] == TRUE)
	{
		ug16Val |= (0x01 << 12);
	}
	if(gs_EsSampleInfo.Batt_CanComm_Break_Alm[1] == TRUE)
	{
		ug16Val |= (0x01 << 13);
	}
	if(gs_EsSampleInfo.Batt_CanComm_Break_Alm[2] == TRUE)
	{
		ug16Val |= (0x01 << 14);
	}
	if(gs_EsSampleInfo.Batt_CanComm_Break_Alm[3] == TRUE)
	{
		ug16Val |= (0x01 << 15);
	}


	return ug16Val;
}


UINT32 u16GetAlm_4()
{
	UINT32 ug16Val = 0;

	//光伏接触器反馈信号
	if(gs_EsSampleInfo.PV_Contactor_Detection[0] == TRUE)
	{
		ug16Val |= (0x01 << 0);
	}

	if(gs_EsSampleInfo.PV_Contactor_Detection[1] == TRUE)
	{
		ug16Val |= (0x01 << 1);
	}
	if(gs_EsSampleInfo.PV_Contactor_Detection[2] == TRUE)
	{
		ug16Val |= (0x01 << 2);
	}
	if(gs_EsSampleInfo.PV_Contactor_Detection[3] == TRUE)
	{
		ug16Val |= (0x01 << 3);
	}

	//光伏防雷器状态检测
	if(gs_EsSampleInfo.PV_SPD_Detection[0] == TRUE)
	{
		ug16Val |= (0x01 << 4);
	}
	if(gs_EsSampleInfo.PV_SPD_Detection[1] == TRUE)
	{
		ug16Val |= (0x01 << 5);
	}
	if(gs_EsSampleInfo.PV_SPD_Detection[2] == TRUE)
	{
		ug16Val |= (0x01 << 6);
	}
	if(gs_EsSampleInfo.PV_SPD_Detection[3] == TRUE)
	{
		ug16Val |= (0x01 << 7);
	}

	//光伏隔离开关反馈信号
	if(gs_EsSampleInfo.PV_Isolation_Switch[0] == TRUE)
	{
		ug16Val |= (0x01 << 8);
	}

	if(gs_EsSampleInfo.PV_Isolation_Switch[1] == TRUE)
	{
		ug16Val |= (0x01 << 9);
	}
	if(gs_EsSampleInfo.PV_Isolation_Switch[2] == TRUE)
	{
		ug16Val |= (0x01 << 10);
	}
	if(gs_EsSampleInfo.PV_Isolation_Switch[3] == TRUE)
	{
		ug16Val |= (0x01 << 11);
	}

	//直流负载防雷器状态检测
	if(gs_EsSampleInfo.DCload_Grp1_SPD_Detection[0] == TRUE)
	{
		ug16Val |= (0x01 << 12);
	}
	if(gs_EsSampleInfo.DCload_Grp1_SPD_Detection[1] == TRUE)
	{
		ug16Val |= (0x01 << 13);
	}
	if(gs_EsSampleInfo.DCload_Grp1_SPD_Detection[2] == TRUE)
	{
		ug16Val |= (0x01 << 14);
	}
	if(gs_EsSampleInfo.DCload_Grp1_SPD_Detection[3] == TRUE)
	{
		ug16Val |= (0x01 << 15);
	}

	//直流负载隔离开关反馈信号
	if(gs_EsSampleInfo.DCLoad_Grp1_Isolation_Switch[0] == TRUE)
	{
		ug16Val |= (0x01 << 16);
	}

	if(gs_EsSampleInfo.DCLoad_Grp1_Isolation_Switch[1] == TRUE)
	{
		ug16Val |= (0x01 << 17);
	}
	if(gs_EsSampleInfo.DCLoad_Grp1_Isolation_Switch[2] == TRUE)
	{
		ug16Val |= (0x01 << 18);
	}
	if(gs_EsSampleInfo.DCLoad_Grp1_Isolation_Switch[3] == TRUE)
	{
		ug16Val |= (0x01 << 19);
	}


	return ug16Val;
}



UINT32 u16GetAlm_RectGroup()
{
	UINT8 u8Idx,uctemp;
	UINT32 u32temp;
	UINT32 ug16Val = 0;
	void * PEInfo = GetgEquipInfo(E_RECT_INFO);

	if (gs_CANComm.u8IsAllCommFail == TRUE)
	{
		ug16Val |= (0x02 << 0);						//bit.0: All Rectifier Communication Fail Alarm (0-Normal, 1-Alarm)
	}
	else
	{
		uctemp = 1;
		for(u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
		{
			if ((gs_CANComm.GrpData[GRP_1_ACDC+u8Idx].u8IsNoNormalRect == FALSE)&&(gEvChargeInfo.u8PowerModuleNumCfg[GRP_1_ACDC+u8Idx] > 0))
			{
				uctemp = 0;
				break;
			}
		}
		if(uctemp)
		{
			ug16Val |= (0x01 << 0);
		}
	}

	u32temp = 0;
	for(u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
	{
		if((u8Idx >= GRP_5_MPPT) && (u8Idx <= GRP_8_MPPT))
		{
			continue;
		}

		u32temp += 2;
		if(gEvChargeInfo.u8PowerModuleNumCfg[GRP_1_ACDC+u8Idx] > 0)
		{
			if(gs_CANComm.bIsGrp_NotAvailable[GRP_1_ACDC+u8Idx] == TRUE)
			{
				ug16Val |= (0x02<<u32temp);	
			}
			else if (gs_CANComm.GrpData[GRP_1_ACDC+u8Idx].u8IsNoNormalRect)
			{
				ug16Val |= (0x01<<u32temp);	
			}
		}
	}
	
	return ug16Val;	
}

/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
UINT8 IsMpptStat(void)
{
	UINT8 u8Idx, u8IsOnWork;

	u8IsOnWork = 0;
	for (u8Idx= 0; u8Idx < mMPPTNUM; u8Idx++)
	{					
		if((gs_CANComm.MpptData[u8Idx].bIsExisted == TRUE)
			&& (gs_CANComm.MpptData[u8Idx].bCommFail == FALSE)
			&& (gs_CANComm.MpptData[u8Idx].bDC_Off == FALSE)  //0-开机 1-关机
			)
		{
			u8IsOnWork = 1;
			break;
		}
	}
	return u8IsOnWork;
}

/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/

//20230206 注意：需要与void TaskConverterOutAction(void)中的故障判断保持一致!
// u8BattNo -  0指CCU本身（不可用原因)   1~4指电池1~4（不可用原因）
//
/*
1-后台通信中断
2-绝缘故障
3-EPO
4-SPD
5-门禁
6-IMMU2故障
7-模块排地址
8-IMIU通信中断
11-IMMU2下发电池Error
12-电池BMS的DO禁止充放
13-电池BMS报告电池未就绪（充放电限制或保护）
14-对应BMS支路绝缘故障
*/
UCHAR StartFailRtnCode(UCHAR u8BattNo)
{
	UCHAR ucErrorReasonCode, u8ImiuStat;

	ucErrorReasonCode = 0;
	u8ImiuStat = 0;

	if(gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)
	{
		return 0;
	}

	if(gEvChargeInfo.u8TelaidianHostCommAlm == TRUE)
	{
		ucErrorReasonCode = 1;
	}
	else if((gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)
		/*|| (gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)*/
		/*|| (gIMIU_Info_UART4[0].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART4[1].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART4[2].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART4[3].u8InsltCommAlm[0] == TRUE)*/
                  )
	{
		ucErrorReasonCode = 2;				
	}
	else if(gEvChargeInfo.u8EPOAlm == TRUE)
	{
		ucErrorReasonCode = 3;
	}
	else if(TRUE == gEvChargeInfo.u8SPDAlm)
	{
		ucErrorReasonCode = 4;
	}
	else if(gEvChargeInfo.u8DoorAlm == TRUE)
	{
		ucErrorReasonCode = 5;
	}
	else if(gs_IMMU2_LCCU_Mng.IMMU2Stat.FaultedStatus == 1)
	{
		ucErrorReasonCode = 6;
	}
	else if(gs_CANComm.u8AddressScanDelay > 0)		//20211019 lkf 新增排地址关模块的处理
	{
		ucErrorReasonCode = 7;
	}
        else if((gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE))
        {
		ucErrorReasonCode = 8;
	}

	//CCU Falt
	if(u8BattNo == 0x00)
	{
		return ucErrorReasonCode;
	}
			
		
	if(ucErrorReasonCode == 0)
	{
		switch(u8BattNo)
		{
			case 1:
				if((gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)||(gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE))
				{
					u8ImiuStat = 1;
				}
				break;
			case 2:
				if((gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)||(gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE))
				{
					u8ImiuStat = 1;
				}
				break;
			case 3:
				if((gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)||(gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE))
				{
					u8ImiuStat = 1;
				}
				break;
			case 4:
				if((gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)||(gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE))
				{
					u8ImiuStat = 1;
				}
				break;
			default:
				return ucErrorReasonCode;
				break;
		}

		u8BattNo--;
		if(gs_IMMU2_LCCU_Mng.IMMU2Stat.BattStatus[u8BattNo] == Immu2RptBattStat_Error)
		{
			ucErrorReasonCode = 11;
		}
		else if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8IsAllowChgOrDischgFromDI == 0)//DI禁止充放电
		{
			ucErrorReasonCode = 12;
		}
		else if//0:未就绪 1:就绪 2:充电截止 3:放电截止 4:充放电限制 5:充放电保护
			((gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.u8Valid == 1)
				&&(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 0||gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 4||gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 5)
			)
		{
			ucErrorReasonCode = 13;
		}
		else if(u8ImiuStat == 14)
		{
			ucErrorReasonCode = 14;
		}
	}

	return ucErrorReasonCode;
}

UCHAR LccuRptStat(void)
{
	UINT8 i, j, u8Rtn;
	UINT8  uctemp, u8Idx, u8PCSWorkStatus = 0, u8PCSWorkMode = 0;
	UINT16 u16DataCount, u16temp, ii;
	UINT32 u32temp;
	float ftemp;
	//UINT8 pFloatArarry[8];
	//struct tm  tmNow;
	union
	{
		SHORT i16temp;
		UINT8 uctemp[2];
	}ShortUnionTemp;
	union
	{
		float f32temp;
		UINT8 uctemp[4];
	}FloatUnionTemp;

	float fConvtVolt;


	gs_IMMU2_LCCU_Mng.SendBuff[0]= ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = 0xB0;

	u16DataCount = ENJI_PROT_DATA;

	//2	System Available Status
	uctemp = 0;
	if(gs_UpGradeCtrl.u8UpgradeStat == UpgradeStatDef_Start)
	{
		uctemp = 3;
	}
	else if(gEvChargeInfo.u8SysAlm == TRUE)
	{
		uctemp = 2;
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uctemp;

	//3 运行模式
	uctemp = 0;
	if (gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)
	{
		uctemp = 1;
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uctemp;

	//time
	//u32temp = gRTCData.RTCGMTSec;
	u32temp = (UINT32)gRTCData.fGetSecondsGMT2LOCAL();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>24)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>16)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp)&0xff);


	//System Alarm1 Bit map
	u32temp = u16GetAlm();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>24)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>16)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp)&0xff);

	//System Alarm2 Bit map
	u32temp = u16GetAlm_2();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>24)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>16)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp)&0xff);

	//System Alarm3 Bit map
	u32temp = u16GetAlm_3();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>24)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>16)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp)&0xff);

	//System Alarm4 Bit map
	u32temp = u16GetAlm_4();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>24)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>16)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp)&0xff);


	//RectGroup Alarm Bit Map
	u32temp = u16GetAlm_RectGroup();
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>24)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>16)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp>>8)&0xff);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u32temp)&0xff);

	//TBD 10Bytes
	for(u8Idx = 0; u8Idx < 10; u8Idx++)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
	}

	//APP Upgrading Type
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_UpGradeCtrl.u8UpdateFileType;

	//Upgrading SubID（升级组号/地址）
	if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_0)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0xF0;
	}
//by lxr01202 添加MPPT_U1和U_2的升级地址
	else if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_2) \
	|| (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_2) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_1TypeDef) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_2TypeDef))
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_UpgradePowerModuleCtrl.u8CurrModuleAddr;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_1TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[0].CurrDecviceAddr;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_2TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[1].CurrDecviceAddr;
	}
	else 
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0x00;
	}

	//Upgrading Status（当前模块升级状态）
//by lxr01202 添加MPPT_U1和U_2的升级状态
	if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_2) \
	|| (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_2) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_1TypeDef) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_2TypeDef))
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_UpgradePowerModuleCtrl.u8CurrUpgradeState;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_1TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[0].u8CurrUpgradeState;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_2TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[1].u8CurrUpgradeState;
	}
	else 
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0x00;
	}

	//Upgrading SubID Schedule（模块升级进度百分比）
//by lxr01202 添加MPPT_U1和U_2的升级进度百分比
	if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_2) \
	|| (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_2) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_1TypeDef) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_2TypeDef))
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)gs_UpgradePowerModuleCtrl.fCurrModuleProgress;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_1TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[0].fProgress;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_2TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[1].fProgress;
	}
	else 
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0x00;
	}

	//Upgrading Tot Schedule（模块升级总体进度）
//by lxr01202 添加MPPT_U1和U_2的升级总进度百分比
	if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_2) \
	|| (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_2) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_1TypeDef) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_2TypeDef))
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_UpgradePowerModuleCtrl.u8TotalUpgradeState;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_1TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[0].u8TotalUpgradeState;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_2TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[1].u8TotalUpgradeState;
	}
	else 
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0x00;
	}

	//Upgrading Error Code
	if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_2) \
	|| (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_2) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_1TypeDef) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_2TypeDef))
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_UpgradePowerModuleCtrl.u8UpgradeErrCode;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_1TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[0].u8UpgradeErrCode;
	}
	else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_2TypeDef)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)g_stImeu2UpgradeInfo[1].u8UpgradeErrCode;
	}
	else 
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0x00;
	}

	//PCS Grp#1 Idx:2 RunSTatus
	if (gs_CANComm.nScanedRectNum)//模块数量不为0
	{
		for (i = 0u; i < gs_CANComm.nScanedRectNum; i++)
		{
			if ((GRP_1_ACDC+1) == (gs_CANComm.RectData[i].nBelongedGrp))
			{
				if((i+1) >= gs_CANComm.nScanedRectNum)
				{
					gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
					break;
				}
				for (j = i+1; j < gs_CANComm.nScanedRectNum; j++)
				{
					if ((GRP_1_ACDC+1) == (gs_CANComm.RectData[j].nBelongedGrp))
					{
						gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gs_CANComm.RectData[j].bStatCode_Invert[0];
						break;
					}
					else if (j == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
					{
						gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
					}
				}
				break;
			}
			else if (i == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
			}
		}
	}
	else//All PowerModule Communication failed
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}
	//PCS Grp#2 Idx:2 RunSTatus
	if (gs_CANComm.nScanedRectNum)
	{
		for (i = 0u; i < gs_CANComm.nScanedRectNum; i++)
		{
			if ((GRP_2_ACDC+1) == (gs_CANComm.RectData[i].nBelongedGrp))
			{
				if((i+1) >= gs_CANComm.nScanedRectNum)
				{
					gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
					break;
				}
				for (j = i+1; j < gs_CANComm.nScanedRectNum; j++)
				{
					if ((GRP_2_ACDC+1) == (gs_CANComm.RectData[j].nBelongedGrp))
					{
						gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gs_CANComm.RectData[j].bStatCode_Invert[0];
						break;
					}
					else if (j == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
					{
						gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
					}
				}
				break;
			}
			else if (i == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
			}
		}
	}
	else//All PowerModule Communication failed
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}
	//PCS Grp#3 Idx:2 RunSTatus
	if (gs_CANComm.nScanedRectNum)
	{
		for (i = 0u; i < gs_CANComm.nScanedRectNum; i++)
		{
			if ((GRP_3_ACDC+1) == (gs_CANComm.RectData[i].nBelongedGrp))
			{
				if((i+1) >= gs_CANComm.nScanedRectNum)
				{
					gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
					break;
				}
				for (j = i+1; j < gs_CANComm.nScanedRectNum; j++)
				{
					if ((GRP_3_ACDC+1) == (gs_CANComm.RectData[j].nBelongedGrp))
					{
						gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gs_CANComm.RectData[j].bStatCode_Invert[0];
						break;
					}
					else if (j == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
					{
						gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
					}
				}
				break;
			}
			else if (i == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
			}
		}
	}
	else//All PowerModule Communication failed
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}
	//PCS Grp#4 Idx:2 RunSTatus
	if (gs_CANComm.nScanedRectNum)
	{
		for (i = 0u; i < gs_CANComm.nScanedRectNum; i++)
		{
			if ((GRP_4_ACDC+1) == (gs_CANComm.RectData[i].nBelongedGrp))
			{
				if((i+1) >= gs_CANComm.nScanedRectNum)
				{
					gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
					break;
				}
				for (j = i+1; j < gs_CANComm.nScanedRectNum; j++)
				{
					if ((GRP_4_ACDC+1) == (gs_CANComm.RectData[j].nBelongedGrp))
					{
						gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gs_CANComm.RectData[j].bStatCode_Invert[0];
						break;
					}
					else if (j == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
					{
						gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
					}
				}
				break;
			}
			else if (i == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
			}
		}
	}
	else//All PowerModule Communication failed
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}

	//PCS Group RunStatus #1
	if (gs_CANComm.nScanedRectNum)
	{
		for (i = 0u; i < gs_CANComm.nScanedRectNum; i++)
		{
			if ((GRP_1_ACDC+1) == (gs_CANComm.RectData[i].nBelongedGrp))
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gs_CANComm.RectData[i].bStatCode_Invert[0];
				break;
			}
			else if (i == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
			}
		}
	}
	else//All PowerModule Communication failed
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}
	

	//PCS Group RunStatus #2
	if (gs_CANComm.nScanedRectNum)
	{
		for (i = 0u; i < gs_CANComm.nScanedRectNum; i++)
		{
			if ((GRP_2_ACDC+1) == (gs_CANComm.RectData[i].nBelongedGrp))
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gs_CANComm.RectData[i].bStatCode_Invert[0];
				break;
			}
			else if (i == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
			}
		}
	}
	else//All PowerModule Communication failed
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}
	

	//PCS Group RunStatus #3
	if (gs_CANComm.nScanedRectNum)
	{
		for (i = 0u; i < gs_CANComm.nScanedRectNum; i++)
		{
			if ((GRP_3_ACDC+1) == (gs_CANComm.RectData[i].nBelongedGrp))
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gs_CANComm.RectData[i].bStatCode_Invert[0];
				break;
			}
			else if (i == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
			}
		}
	}
	else//All PowerModule Communication failed
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}
	

	//PCS Group RunStatus #4
	if (gs_CANComm.nScanedRectNum)
	{
		for (i = 0u; i < gs_CANComm.nScanedRectNum; i++)
		{
			if ((GRP_4_ACDC+1) == (gs_CANComm.RectData[i].nBelongedGrp))
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  gs_CANComm.RectData[i].bStatCode_Invert[0];
				break;
			}
			else if (i == gs_CANComm.nScanedRectNum-1)//in case of BelongedGrp is Unavailable
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
			}
		}
	}
	else//All PowerModule Communication failed
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}

	//CCU Falt code
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = StartFailRtnCode(0);
	//PCS#1 Ctrl Fail
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = StartFailRtnCode(1);
	//PCS#2 Ctrl Fail
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = StartFailRtnCode(2);
	//PCS#3 Ctrl Fail
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = StartFailRtnCode(3);
	//PCS#4 Ctrl Fail
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = StartFailRtnCode(4);

	//ABU Run Status
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantStat0.Data;


	//leakage current
	
	if((gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.LeakageSensorEn == 1)
		&& (gs_DCLeankageSensorMng.u16DCLeakageCurrentSensorOkStat == TRUE))
		ShortUnionTemp.i16temp = (SHORT)(gs_DCLeankageSensorMng.fDCLeakageCurrent * 10);
	else 
		ShortUnionTemp.i16temp = 0;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[0];

	//1 Ambient Temperature
	ShortUnionTemp.i16temp = (SHORT)(gEvChargeInfo.fEV_InterTemp * 10);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[0];

	//2 Ambient Temperature
	ShortUnionTemp.i16temp = (SHORT)(0);
	ShortUnionTemp.i16temp = (SHORT)(gIMEU_Info[0].fTP1000_1_Val * 10);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[0];

	//3 Ambient Temperature
	ShortUnionTemp.i16temp = (SHORT)(0);
	ShortUnionTemp.i16temp = (SHORT)(gIMEU_Info[0].fTP1000_2_Val * 10);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[0];

	//4 Ambient Temperature
	ShortUnionTemp.i16temp = (SHORT)(0);
	ShortUnionTemp.i16temp = (SHORT)(gIMEU_Info[0].fTP1000_3_Val * 10);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[1];
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[0];


	for(u8Idx = 0; u8Idx < 4; u8Idx++)
	{
		//BATT-1 绝缘值 P
		u16temp = (UINT16)(gIMIU_Info_UART4[u8Idx].fJYMP2gResist[0] *10);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

		u16temp = (UINT16)(gIMIU_Info_UART4[u8Idx].fJYMN2gResist[0] *10);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	}
	for(u8Idx = 0; u8Idx < 4; u8Idx++)
	{
		//BATT-1 绝缘值 P
		u16temp = (UINT16)(gIMIU_Info_UART5[u8Idx].fJYMP2gResist[0] *10);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

		u16temp = (UINT16)(gIMIU_Info_UART5[u8Idx].fJYMN2gResist[0] *10);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	}
	//预留2路
	for(u8Idx = 0; u8Idx < 2; u8Idx++)
	{
		//BATT-1 绝缘值 P
		u16temp = (UINT16)(500*10);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

		u16temp = (UINT16)(500*10);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	}


	//模块信息（实时电压、实时电流、最大功率、开关机状态）(协议ID NO55开始)
	for(u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
	{
		if(gs_CANComm.u8GRP_RectOnOff[GRP_NO_MIN+u8Idx] == RECT_OFF)
		{
			u16temp = 0;
			fConvtVolt = 0;
		}
		else if((u8Idx >= ACDC_GRP_NO_MIN && u8Idx <= ACDC_GRP_NO_MAX) && (gs_CANComm.u8GRP_WorkModeSet[ACDC_GRP_NO_MIN+u8Idx] == GRP_WORKMODE_SET_REVERSE))
		{
			fConvtVolt = gs_CANComm.GrpData[ACDC_GRP_NO_MIN+u8Idx].fDCInputVolt;
			u16temp = (UINT16)(fConvtVolt*10);			
		}
		else
		{
			fConvtVolt = gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fOutVolt;
			u16temp = (UINT16)(fConvtVolt*10);
		}
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp>>8)&0xff); //实时电压
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp)&0xff);


		if((u8Idx >= ACDC_GRP_NO_MIN && u8Idx <= ACDC_GRP_NO_MAX)  && (gs_CANComm.u8GRP_WorkModeSet[u8Idx] == GRP_WORKMODE_SET_REVERSE))
		{
			FloatUnionTemp.f32temp =  ABS(gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fDCInputCurr);
		}
		/*else if((u8Idx >= GRP_9_DCDC && u8Idx <= GRP_12_DCDC) && (gs_CANComm.u8GRP_WorkModeSet[ACDC_GRP_NO_MAX+u8Idx] == GRP_WORKMODE_SET_REVERSE))
		{
		FloatUnionTemp.f32temp = -ABS(gs_CANComm.GrpData[GRP_A+u8Idx].fOutCurr);
		}*/
		else
		{
			FloatUnionTemp.f32temp = ABS(gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fOutCurr);
		}
		u16temp =  (UINT16)(FloatUnionTemp.f32temp * 10);
		if(gs_CANComm.u8GRP_RectOnOff[GRP_NO_MIN+u8Idx] == RECT_OFF)
		{
			u16temp = 0;
		}
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp>>8)&0xff);//实时电流
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp)&0xff);


		if((gs_CANComm.u8GRP_RectOnOff[GRP_NO_MIN+u8Idx] == RECT_ON)
			&& (!((u8Idx >= DCDC_GRP_NO_MIN) && (u8Idx <= DCDC_GRP_NO_MAX))))
		{
			ftemp = fConvtVolt * gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fMax_OutCurr / 1000;
			if(ftemp > gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fRatedPower)
			{
				ftemp = gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fRatedPower;
			}
			else if(fConvtVolt < 50)  //20220419 如果电压没采集到，也使用额定值
			{
				ftemp = gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fRatedPower;
			}

			//20221027 lkf 增加当前可用功率
			if((gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].u8PowerofThisVoltMaxIsValidFlag == 2)
				&& ((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)))
			{
				if(ftemp > gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fAvailablePwr)
				{
					ftemp = gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fAvailablePwr;
				}
			}

			u16temp = (UINT16)(ftemp * 10);
		}
		else 
		{
			u16temp = (UINT16)(gs_CANComm.GrpData[GRP_NO_MIN+u8Idx].fRatedPower*10);
		}		
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp>>8)&0xff);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp)&0xff);

		//if(gs_CANComm.u8GRP_RectOnOff[GRP_A+u8Idx] == RECT_OFF)

		//模块开关机状态和充放电状态
		for(i=0; i<mRECTNUM; i++)
		{
			/*组内且是开机状态*/
			if (((u8Idx+1) == (gs_CANComm.RectData[i].nBelongedGrp)) && (gs_CANComm.RectData[i].bPCSWorkStatus != 0))
			{
				u8PCSWorkStatus = gs_CANComm.RectData[i].bPCSWorkStatus;//开关机状态
				u8PCSWorkMode = gs_CANComm.RectData[i].bPCSWorkMode;    //充放电状态 1--放电 2--充电状态
				break;
			}
		}
		if((u8Idx >= MPPT_GRP_NO_MIN && u8Idx <= MPPT_GRP_NO_MAX))//MPPT
		{
			uctemp = IsMpptStat();
		}
		/*该组不存在或者无正常模块*/
		else if((gs_CANComm.bIsGrpExist[u8Idx] == FALSE)
			//|| (gs_CANComm.GrpData[u8Idx].bIsAllDCOutClose == TRUE)
			||(gs_CANComm.GrpData[u8Idx].u8IsNoNormalRect == TRUE))
			//&& ((u8Idx != GRP_E_DCDC) && (u8Idx != GRP_F_DCDC)))		
		{
			uctemp = 0;
		}	
		/*关机状态*/	
		else if(gs_CANComm.u8GRP_RectOnOff[GRP_NO_MIN+u8Idx] == RECT_OFF)
		{
			uctemp = 0;

		}
		/*设置为逆变状态并且最少1个模块处于逆变状态*/
		else if((gs_CANComm.u8GRP_WorkModeSet[u8Idx] == GRP_WORKMODE_SET_REVERSE)
			&& (gs_CANComm.GrpData[u8Idx].u8ConvertStat_ReverseNum > 0)
			)
		{
			uctemp = 2;

		}
		/*设置为整流状态并且最少1个模块处于整流状态*/
		else if((gs_CANComm.u8GRP_WorkModeSet[u8Idx] == GRP_WORKMODE_SET_FORWORD)
			&& (gs_CANComm.GrpData[u8Idx].u8ConvertStat_ForwardNum > 0)
			)
		{
			uctemp = 1;

		}
		/*非关机状态*/
		else if(u8PCSWorkStatus != 0)
		{
			/*放电（逆变）状态*/
			if((u8PCSWorkMode & 0x01) == 1)
			{
				uctemp = 2;

			}
			/*充电（整流）状态*/
			else if(((u8PCSWorkMode >> 1) & 0x01) == 1)
			{
				uctemp = 1;

			}
			else
			{
				uctemp = 3;
			}
		}
		else
		{
			uctemp = 0;

		}
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  uctemp;


		if((u8Idx >= ACDC_GRP_NO_MIN && u8Idx <= ACDC_GRP_NO_MAX))
		{
			if((gs_CANComm.bIsGrpExist[u8Idx] == TRUE)&&(gs_CANComm.GrpData[u8Idx].u8IsNoNormalRect == FALSE))
			{
				u16temp = (UINT16)(gs_CANComm.GrpData[GRP_1_ACDC+u8Idx].fExtVolt*10);
			}
			else
			{
				u16temp = 0xFFFF;
			}
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp>>8)&0xff);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] =  ((u16temp)&0xff);
		}
		else if((u8Idx >= MPPT_GRP_NO_MIN && u8Idx <= MPPT_GRP_NO_MAX))
		{
			//输出kwh

			FloatUnionTemp.f32temp = 0;	
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = FloatUnionTemp.uctemp[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = FloatUnionTemp.uctemp[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = FloatUnionTemp.uctemp[2];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = FloatUnionTemp.uctemp[3];
		}


	}

	//ActivePower（对应协议111位置）   lxr01202
	for(u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
	{
		if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)
		&&(u8Idx >= ACDC_GRP_NO_MIN)&&(u8Idx <= ACDC_GRP_NO_MAX))
		{

			gs_CANComm.GrpData[u8Idx].TotalActivePower = 500000;
			ShortUnionTemp.i16temp = (SHORT)(gs_CANComm.GrpData[u8Idx].TotalActivePower/100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[0];
		}
//by lxr01202  显示在屏上   协议01 02  V2G无交流测电压，只能利用单模块的相加   2108
//01 02
		else if((gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE)
		&& (u8Idx >= ACDC_GRP_NO_MIN) && (u8Idx <= ACDC_GRP_NO_MAX))
		{
        	// 计算当前组对应的五个模块索引
        	int baseIndex = (u8Idx - ACDC_GRP_NO_MIN) * 5; // 每组对应5个模块

        	// 初始化总有功功率
        	INT32 LX_TotalActivePower = 0;

        	// 处理五个模块
        	for(int i = 0; i < 5; i++) {
            	int moduleIndex = baseIndex + i;
            	// 检查模块是否存在且有效
            	if (moduleIndex <= mRECTNUM && gs_CANComm.RectData[moduleIndex].bIsExisted) {
                	LX_TotalActivePower += gs_CANComm.RectData[moduleIndex].TotalActivePower;
            	}
        }

			u16temp = (INT16S)(LX_TotalActivePower / 100000);

			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp >> 8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
	}

	//ReactivePower    by lxr01202
	for(u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
	{
    	if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)
     	&&(u8Idx >= ACDC_GRP_NO_MIN)&&(u8Idx <= ACDC_GRP_NO_MAX))
    	{
        	ShortUnionTemp.i16temp = (SHORT)(gs_CANComm.GrpData[u8Idx].TotalReactivePower/100000);
        	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[1];
        	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[0];
    	}
    	else if((gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE)
     	&&(u8Idx >= ACDC_GRP_NO_MIN)&&(u8Idx <= ACDC_GRP_NO_MAX))
    	{
        	// 计算当前组对应的五个模块索引
        	int baseIndex = (u8Idx - ACDC_GRP_NO_MIN) * 5; // 每组对应5个模块

        	// 初始化总无功功率
        	INT32 LX_TotalReactivePower = 0;

        	// 处理五个模块
        	for(int i = 0; i < 5; i++) {
            	int moduleIndex = baseIndex + i;
            	// 检查模块是否存在且有效
            	if (moduleIndex <= mRECTNUM && gs_CANComm.RectData[moduleIndex].bIsExisted) {
                	LX_TotalReactivePower += gs_CANComm.RectData[moduleIndex].TotalReactivePower;
            	}
        }

        	u16temp = (INT16S)(LX_TotalReactivePower / 100000);

        	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp >> 8);
        	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
    }
}

	for(u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
	{
		if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
		&&((u8Idx >= ACDC_GRP_NO_MIN)&&(u8Idx <= ACDC_GRP_NO_MAX)))
		{
			ShortUnionTemp.i16temp = (SHORT)(gs_IMMU2_LCCU_Mng.ESBattMng[u8Idx].CtrlReactivePower*10);
		}

		else if((gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE)
		&&(u8Idx >= ACDC_GRP_NO_MIN)&&(u8Idx <= ACDC_GRP_NO_MAX))
		{
//by lxr01202
			ShortUnionTemp.i16temp = (SHORT)((gs_IMMU2_LCCU_Mng.ESBattMng[0].CtrlReactivePower)+(gs_IMMU2_LCCU_Mng.ESBattMng[1].CtrlReactivePower)*10);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ShortUnionTemp.uctemp[0];
		}
	}

	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送
	u8Rtn = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//OSTimeDly(35);

	return u8Rtn;
}

/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
static NOINIT UCHAR s_u8RectTypeNo[mRECTNUM];
static NOINIT UCHAR s_u8RectTypeNum;

UCHAR LccuRptRectInfo(UINT8 u8TxNo)              //RTIF
{
	unsigned long long l64temp;

	UINT8  uctemp, u8Idx, u8Rtn, k, u8SingleModuleByte;
	UINT32 u16DataCount, u16temp, ii, u16SingleModuleByteNumPos;
	//UINT8 pFloatArarry[8];
	//struct tm  tmNow;
	//time_t now;
	//0x00-MPPT
	//0x01-ACDC4EV Grp1
	//0x02-ACDC4EV Grp2
	//0x03-ACDC4BatteryGrp
	//0x04-Resv
	//0x05-DCDC4EV Grp1
	//0x06-DCDC4EV Grp2
	//0x07-DCDC4DCLoad Grp
	UINT8 u8GrpNo;

	if(u8TxNo >= _MAX_GRP_NUM)
	{
		return 0;
	}
	u8GrpNo = u8TxNo;


	gs_IMMU2_LCCU_Mng.SendBuff[0]= ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = 0x04;

	u16DataCount = ENJI_PROT_DATA;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = u8TxNo;	      //组分类


	u16temp = (UINT16)(gs_CANComm.GrpData[u8GrpNo].fRatedPower * 10);//组额定功率
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	u16temp = (UINT16)(gs_CANComm.GrpData[u8GrpNo].fRatedMaxCurr * 1);//组额定电流
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	u16temp = (UINT16)(gs_CANComm.GrpData[u8GrpNo].fAvailableCurr * 1);//组最大电流
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	uctemp = 0;
	if(gs_CANComm.u8GRP_RectOnOff[u8GrpNo] == RECT_ON)
	{
		uctemp= 1;		
		if(gs_CANComm.u8GRP_WorkModeSet[u8GrpNo] == GRP_WORKMODE_SET_REVERSE)//逆变状态
		{
			uctemp= 2;
		}
		else if(gs_CANComm.u8GRP_WorkModeSet[u8GrpNo] == GRP_WORKMODE_SET_MPPT)//MPPT
		{
			uctemp= 3;
		}
	}
	/*组控制状态*/
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uctemp;

	/*控制电压*/
	u16temp = (UINT16)(gs_CANComm.fRect_GRP_NeedOutVolt[u8GrpNo] * 10);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	/*控制电流*/
	u16temp = (UINT16)(gs_CANComm.fRect_GRP_NeedOutCurr[u8GrpNo] * 1);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	/*输出电压*/
	//u16temp = (UINT16)(MAX(gs_CANComm.GrpData[u8GrpNo].fExtVolt, gs_CANComm.GrpData[u8GrpNo].fOutVolt) * 10);
	u16temp = (UINT16)(gs_CANComm.GrpData[u8GrpNo].fOutVolt * 10);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	/*输出电流*/
	if(gs_CANComm.u8GRP_WorkModeSet[u8GrpNo] == GRP_WORKMODE_SET_REVERSE)
		u16temp = (UINT16)(ABS(gs_CANComm.GrpData[u8GrpNo].fDCInputCurr) * 1);
	else
		u16temp = (UINT16)(ABS(gs_CANComm.GrpData[u8GrpNo].fOutCurr) * 1);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//单模块数据字节
	u16SingleModuleByteNumPos = u16DataCount;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
	u8SingleModuleByte = 0;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.GrpData[u8GrpNo].nCommRectNum;                



	if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)) //MPPT
	{
		s_u8RectTypeNum = 0;
		for (u8Idx= 0; u8Idx < mMPPTNUM; u8Idx++)	
		{
			if (TRUE == gs_CANComm.MpptData[u8Idx].bIsExisted)
			{
				if ((u8GrpNo+1) == gs_CANComm.MpptData[u8Idx].nBelongedGrp)
				{
					s_u8RectTypeNo[s_u8RectTypeNum] = u8Idx;
					s_u8RectTypeNum++;
				}
			}
		}
	}
	else
	{
		s_u8RectTypeNum = 0;
		for (u8Idx= 0; u8Idx < mRECTNUM; u8Idx++)	
		{
			if (TRUE == gs_CANComm.RectData[u8Idx].bIsExisted)
			{
				if ((u8GrpNo+1) == gs_CANComm.RectData[u8Idx].nBelongedGrp)
				{
					s_u8RectTypeNo[s_u8RectTypeNum] = u8Idx;
					s_u8RectTypeNum++;
				}
			}
		}
	}


	for (u8Idx= 0; u8Idx < s_u8RectTypeNum && u8Idx < 10; u8Idx++)	
	{

		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)) //MPPT
		{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bCommFail;
		}
		else
		{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bCommFail;
		}		
		u8SingleModuleByte++;

		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = u8GrpNo+1;
		u8SingleModuleByte++;
                
		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)) //MPPT
		{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0; //预留
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bStatCode[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bStatCode[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bStatCode[2];
			//gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bConvertStat;

			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[2];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[3];
		}
		else
		{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0; //预留
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode[2];
			//gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bConvertStat;

			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[2];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[3];
		}
		u8SingleModuleByte += 8;

		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)) //MPPT
		{
			u16temp = (UINT16)(gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].fRealTime_Volt * 10);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
		else
		{
			u16temp = (UINT16)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].fRealTime_Volt * 10);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
		u8SingleModuleByte += 2;

		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)) //MPPT
		{
			u16temp = (UINT16)(ABS(gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].fRealTime_Curr) * 10);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
		else
		{
			u16temp = (UINT16)(ABS(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].fRealTime_Curr) * 10);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
		u8SingleModuleByte += 2;

		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)) //MPPT
		{
			u16temp = (UINT16)((gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].fDC_Ver+0.0001) * 100);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
		else
		{
			u16temp = (UINT16)((gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].fDC_Ver+0.0001) * 100);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
		u8SingleModuleByte += 2;

		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)) //MPPT
		{
			u16temp = (UINT16)((gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].fAC_Ver+0.0001) * 100);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
		else
		{
			u16temp = (UINT16)((gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].fAC_Ver+0.0001) * 100);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		}
		u8SingleModuleByte += 2;

		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)) //MPPT
		{
			for(k = 0; k < 24; k++)
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount+k] = 0;
			}

			/*for(k = 0; k < 8; k++)
			{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[k];
			}
			for(; k < 24; k++)
			{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
			}*/

			l64temp = ((unsigned long long)gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[1]<<32)
				+ (gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[2]<<24)
				+ (gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[3]<<16)
				+ (gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[4]<<8)
				+ (gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[5]);

			sprintf((char *)&gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount], "%012lld", l64temp);
			u16DataCount += 12;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[0];
			u16temp = (UINT16)(gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[6]<<8) + gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].bSerialNo[7];	
			sprintf((char *)&gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount], "%04d", u16temp);
			u16DataCount+= 7;

			u16DataCount+= 4;
		}
		else if((u8GrpNo >= ACDC_GRP_NO_MIN)&&(u8GrpNo <= ACDC_GRP_NO_MAX))
		{
#if  0  //预留 def  	ACDC_IS_PCS_TYPE
			for(k = 0; k < 24; k++)
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount+k] = 0;
			}

			l64temp = ((unsigned long long)gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[1]<<32)
				+ (gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[2]<<24)
				+ (gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[3]<<16)
				+ (gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[4]<<8)
				+ (gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[5]);

			sprintf((char *)&gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount], "%012lld", l64temp);
			u16DataCount += 12;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[0];
			u16temp = (UINT16)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[6]<<8) + gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[7];	
			sprintf((char *)&gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount], "%04d", u16temp);
			u16DataCount+= 7;

			u16DataCount+= 4;

#else

			for(k = 0; k < 24; k++)
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount+k] = 0;
			}

			u16temp = (UINT16)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_25[2]<<8) + gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_25[3];
			sprintf((char *)&gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount], "%02d%02d%02d%05d%01d",
				(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_24[2]>>0x04),
				(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_25[0]),
				(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_25[1]>>0x04),
				u16temp,
				(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_25[1]&0x0F)
				//,
				//((gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_24[2]&0x0f)+'A'),
				//(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_24[3])
				);
			u16DataCount += 12;
			for(u16temp = 0; u16temp < 4;u16temp++)
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_27[u16temp];
			}
			for(u16temp = 0; u16temp < 4;u16temp++)
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_28[u16temp];
			}
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_24[2]&0x0f)+'A';

			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_24[3]/10 + '0');
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bBarcode_24[3]%10 + '0');

			u16DataCount+= 1;
#endif
		}
		else
		{
			for(k = 0; k < 8; k++)
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bSerialNo[k];
			}
			for(; k < 24; k++)
			{
				gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
			}
		}
		u8SingleModuleByte += 24;


//by lxr01202   62.5KW    1113  1114  1115故障上传IMMU2 （存在35KW是因为MU2那边的字节解析问题，如果在这边屏蔽35KW，函数进不去，导致MU2那边的解析会出错）
		if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
		   &&((u8GrpNo >= ACDC_GRP_NO_MIN)&&(u8GrpNo <= ACDC_GRP_NO_MAX) || (u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)))
		   
		{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[2];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatCode_Invert[3];
			u8SingleModuleByte += 4;

			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS[2];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS[3];
			u8SingleModuleByte += 4;

			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS[4];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS[5];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS[6];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS[7];
			u8SingleModuleByte += 4;

			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bDCFaltCode_PCS[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bDCFaltCode_PCS[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bDCFaltCode_PCS[2];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bDCFaltCode_PCS[3];
			u8SingleModuleByte += 4;

		}		

		if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
		&&((u8GrpNo >= ACDC_GRP_NO_MIN)&&(u8GrpNo <= ACDC_GRP_NO_MAX)||(u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX)))
		{
			//Phase-A Volt
			u16temp = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].u32APhase_Volt/100;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Phase-B Volt
			u16temp = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].u32BPhase_Volt/100;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Phase-C Volt
		if((u8GrpNo >= ACDC_GRP_NO_MIN)&&(u8GrpNo <= ACDC_GRP_NO_MAX))
		{
			u16temp = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].u32CPhase_Volt/100;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;
		}
		else if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX))
		{
			//Phase-C Volt
			u16temp = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].fCA_Volt;    //MPPT
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;
		}
			//Phase-A Current
			u16temp = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].i32APhase_Curr/100;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Phase-B Current
			u16temp = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].i32BPhase_Curr/100;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Phase-C Current
			u16temp = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].i32CPhase_Curr/100;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//AC Frequency
			u16temp = 0u;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Ambit Temperature
			u16temp = 0u;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Phase-A Output Volt
			u16temp = 0u;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Phase-B Output Volt
			u16temp = 0u;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Phase-C Output Volt
			u16temp = 0u;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Total Active Power
			u16temp = (INT16S)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].TotalActivePower / 100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Total Reactive Power
			u16temp = (INT16S)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].TotalReactivePower / 100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//Total Power Factor
			u16temp = 0u;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//PhA-ActivePower
			u16temp = (INT16S)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].ActivePowerAB / 100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//PhB-ActivePower
			u16temp = (INT16S)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].ActivePowerBC / 100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//PhC-ActivePower
			u16temp = (INT16S)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].ActivePowerCA / 100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//PhA-ReActivePower
			u16temp = (INT16S)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].ReactivePowerAB / 100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//PhB-ReActivePower
			u16temp = (INT16S)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].ReactivePowerBC / 100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;

			//PhC-ReActivePower
			u16temp = (INT16S)(gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].ReactivePowerCA / 100000);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;
		}	
//by lxr01202  A2419新增1110故障解析（适用22KW和35KW的BEG模块）
		if(((gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
		   &&((u8GrpNo >= ACDC_GRP_NO_MIN)&&(u8GrpNo <= ACDC_GRP_NO_MAX)))
		   {
			//1110
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS_35KW[0];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS_35KW[1];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS_35KW[2];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS_35KW[3];
			u8SingleModuleByte += 4;
			//1111
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS_35KW[4];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS_35KW[5];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS_35KW[6];
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].bStatFaltCode_PCS_35KW[7];
			u8SingleModuleByte += 4;
		   }
//by lxr01202 A2419新增PCS模块温度解析
		if(((gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
		   &&((u8GrpNo >= ACDC_GRP_NO_MIN)&&(u8GrpNo <= ACDC_GRP_NO_MAX)) )
		   {
			u16temp = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].u32Rect_Tem;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;
		   }
//by lxr01202 A2419 新增PCS模块最高温度解析
		if(((gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
		   &&((u8GrpNo >= ACDC_GRP_NO_MIN)&&(u8GrpNo <= ACDC_GRP_NO_MAX)))
		   {
			u16temp = gs_CANComm.RectData[s_u8RectTypeNo[u8Idx]].u32Rect_Tem_MAX;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 2;                 
		   }

//by lxr01202 A2419 新增MPPT模块DC板温解析

		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX))
		   {      		
			u16temp = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].u32MPPT_DC_Tem;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>24);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>16);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

			u8SingleModuleByte += 4;  
		   }

//by lxr01202 A2419 新增MPPT模块PFC板温解析
		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX))
		   {
			u16temp = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].u32MPPT_PFC_Tem;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>24);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>16);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 4;
		   }
//by lxr01202 A2419 新增MPPT模块DC变压器温度解析
		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX))
		   {
			u16temp = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].u32MPPT_DC_trans_Tem;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>24);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>16);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 4;
		   }
//by lxr01202 A2419 新增MPPT模块PFC变压器温度解析
		if((u8GrpNo >= MPPT_GRP_NO_MIN)&&(u8GrpNo <= MPPT_GRP_NO_MAX))
		   {
			u16temp = gs_CANComm.MpptData[s_u8RectTypeNo[u8Idx]].u32MPPT_PFC_trans_Tem;
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>24);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>16);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
			u8SingleModuleByte += 4;
		   }

		//只处理一次
		if(gs_IMMU2_LCCU_Mng.SendBuff[u16SingleModuleByteNumPos] == 0)
		{
			gs_IMMU2_LCCU_Mng.SendBuff[u16SingleModuleByteNumPos] = u8SingleModuleByte;
		}
	}  // for (u8Idx= 0; u8Idx < s_u8RectTypeNum && u8Idx < 10; u8Idx++)

	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送	
	u8Rtn = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//OSTimeDly(35);

	//extern unsigned char g_u8SentUdpFlag;
	//	if(g_u8SentUdpFlag == 0)
	//	{
	//		if(u16DataCount > RingExchangeInfo_BufSize)
	//		{
	//			u16DataCount = RingExchangeInfo_BufSize;
	//		}
	//		memcpy(gs_RingExchangeInfo.Sendbuf, gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//		gs_RingExchangeInfo.u16SendDataLenth = u16DataCount;
	//		g_u8SentUdpFlag = 1;
	//	}

	return u8Rtn;
}

UCHAR LccuRptSubDevVersion(UINT8 u8TxNo)                 //LDEV
{
	UINT8 u8SingleModuleByte, u8Rtn;
	UINT16 u16DataCount, u16temp, ii, u16SingleModuleByteNumPos;

	gs_IMMU2_LCCU_Mng.SendBuff[0] = ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = 0x0C;

	u16DataCount = ENJI_PROT_DATA;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = u8TxNo+1;	


	//单模块数据字节
	u16SingleModuleByteNumPos = u16DataCount;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
	u8SingleModuleByte = 0;

	if(u8TxNo == 0)  //IMEU-0
	{
		for(ii = 0; ii < 8; ii++)
		{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
		}
		u8SingleModuleByte += 8;

		u16temp = sprintf((char *)&gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount], "%8.2f", gIMEU_Info[0].fSwVersion);

		u16DataCount += 8;
		u8SingleModuleByte += 8;

		u16temp = sprintf((char *)&gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount], "%08d", (INT16)gIMEU_Info[0].fHwVersion);

		u16DataCount += 8;
		u8SingleModuleByte += 8;


	}

	gs_IMMU2_LCCU_Mng.SendBuff[u16SingleModuleByteNumPos] = u8SingleModuleByte;



	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送	
	u8Rtn = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//OSTimeDly(35);

	return u8Rtn;
}

UCHAR LccuRptABUInfo(void)                     //ABU信息上报   RTIF
{
	UINT8 u8Idx, u8Rtn, u8SingleModuleByte;
	UINT16 u16DataCount, u16temp, ii, u16SingleModuleByteNumPos = 0;

	gs_IMMU2_LCCU_Mng.SendBuff[0]= ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = 0x04;
	u16DataCount = ENJI_PROT_DATA;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0x0C;	         //ABU 0x0c	

	u16temp = (UINT16)(0 * 10);//RatedPower
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	u16temp = (UINT16)(0);//RatedMaxCurr
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	u16temp = (UINT16)(0);//AvailableCurr
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
#if 0
	uctemp = 0;
	if(gs_CANComm.u8GRP_RectOnOff[u8GrpNo] == RECT_ON)
	{
		uctemp= 1;		
		if(gs_CANComm.u8GRP_WorkModeSet[u8GrpNo] == GRP_WORKMODE_SET_REVERSE)
		{
			uctemp= 2;
		}
		else if(gs_CANComm.u8GRP_WorkModeSet[u8GrpNo] == GRP_WORKMODE_SET_MPPT)
		{
			uctemp= 3;
		}
	}
#endif
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;//0-关, 1-整流, 2-逆变 3-MPPT

	u16temp = (UINT16)(0);//Rect_GRP_NeedOutVolt
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	u16temp = (UINT16)(0);//Rect_GRP_NeedOutCurr
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);


	//u16temp = (UINT16)(MAX(gs_CANComm.GrpData[u8GrpNo].fExtVolt, gs_CANComm.GrpData[u8GrpNo].fOutVolt) * 10);
	u16temp = (UINT16)(0);//OutVolt
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	u16temp = (UINT16)(0);//OutCurr
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//单模块数据字节
	u16SingleModuleByteNumPos = u16DataCount;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0;
	u8SingleModuleByte = 0;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 1u;//Online number

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->CommFailed;
	u8SingleModuleByte++;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;//belong Group
	u8SingleModuleByte++;
	
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	u8SingleModuleByte += 8;

	u16temp = (UINT16)(0);//RealTime_Volt
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	u16temp = (UINT16)(0);//RealTime_Curr
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	
	u16temp = ABU->DC_Version;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;


	u16temp = ABU->PFC_Version;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	for (u8Idx = 0u; u8Idx < 17; u8Idx++)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->BarCodeString[u8Idx];
	}

	for (u8Idx = 0u; u8Idx < 7; u8Idx++)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = 0u;
	}
	u8SingleModuleByte += 24;

	//0x1112
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantStat0.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantStat1.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantStat2.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantStat3.Data;
	u8SingleModuleByte += 4;

	//0x1113
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantAlm0.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantAlm1.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantAlm2.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantAlm3.Data;
	u8SingleModuleByte += 4;

	//0x1114
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantAlm4.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantAlm5.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantAlm6.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->ContravariantAlm7.Data;
	u8SingleModuleByte += 4;

	//0x1115
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->_0x1115Byte0_info.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->_0x1115Byte1_info.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->_0x1115Byte2_info.Data;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ABU->_0x1115Byte3_info.Data;
	u8SingleModuleByte += 4;

	//AC--ABVolt
	u16temp = (UINT16)(ABU->ABU_0x21_info.AC_VoltAB / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//AC--BCVolt
	u16temp = (UINT16)(ABU->ABU_0x21_info.AC_VoltBC / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//AC--CAVolt
	u16temp = (UINT16)(ABU->ABU_0x21_info.AC_VoltCA / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//AC--ABCurr
	u16temp = (INT16S)(ABU->ABU_0x21_info.AC_CurrAB / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//AC--BCCurr
	u16temp = (INT16S)(ABU->ABU_0x21_info.AC_CurrBC / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//AC--CACurr
	u16temp = (INT16S)(ABU->ABU_0x21_info.AC_CurrCA / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//AC--freq
	u16temp = (UINT16)(ABU->ABU_0x21_info.AC_Freq / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//Ambit Temperature
	u16temp = (INT16S)(ABU->ABU_0x11_info.PowerModule_AmbientTemp / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//Phase-A Output Volt
	u16temp = (UINT16)(ABU->ABU_0x31_info.OutputVoltAB / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//Phase-B Output Volt
	u16temp = (UINT16)(ABU->ABU_0x31_info.OutputVoltBC / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//Phase-C Output Volt
	u16temp = (UINT16)(ABU->ABU_0x31_info.OutputVoltCA / 100);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//Total Active Power
	u16temp = (INT16S)(ABU->ABU_0x21_info.TotalActivePower / 100000);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//Total Reactive Power
	u16temp = (INT16S)(ABU->ABU_0x21_info.TotalReactivePower / 100000);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;

	//Total Power Factor
	u16temp = (INT16S)(ABU->ABU_0x21_info.TotalPowerFactor);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
	u8SingleModuleByte += 2;
	

	//只处理一次
	//if(gs_IMMU2_LCCU_Mng.SendBuff[u16SingleModuleByteNumPos] == 0)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16SingleModuleByteNumPos] = u8SingleModuleByte;
	}

	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送	
	u8Rtn = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//OSTimeDly(35);

	//extern unsigned char g_u8SentUdpFlag;
	//	if(g_u8SentUdpFlag == 0)
	//	{
	//		if(u16DataCount > RingExchangeInfo_BufSize)
	//		{
	//			u16DataCount = RingExchangeInfo_BufSize;
	//		}
	//		memcpy(gs_RingExchangeInfo.Sendbuf, gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//		gs_RingExchangeInfo.u16SendDataLenth = u16DataCount;
	//		g_u8SentUdpFlag = 1;
	//	}

	return u8Rtn;
}

UCHAR LccuRptCANRawInfo(void)
{
	UINT8 u8Idx, u8Rtn;
	UINT16 u16DataCount, u16temp, ii;
	union
	{
		unsigned char uctemp[4];
		UINT32 u32temp;
	}uniontemp;

	gs_IMMU2_LCCU_Mng.SendBuff[0]= ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = 0x05;
	u16DataCount = ENJI_PROT_DATA;

	u16temp = CAN_MSG_MAX_NUM * 12;
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	for (u8Idx = 0u; u8Idx < CAN_MSG_MAX_NUM; u8Idx++)
	{
		uniontemp.u32temp = gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[u8Idx].Id;
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[3];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[2];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[1];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[0];

		uniontemp.u32temp = gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[u8Idx].Data[0];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[3];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[2];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[1];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[0];

		uniontemp.u32temp = gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[u8Idx].Data[1];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[3];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[2];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[1];
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = uniontemp.uctemp[0];
	}

	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送	
	u8Rtn = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);

	return u8Rtn;
}

/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
UCHAR LccuAckCfgResult(UINT8 u8CmdId, UINT8 u8SubCmd, UINT8 Rtn)
{
	//__XXX_UNION_VALUE unTempVal;

	//UINT8  uctemp, u8Idx;
	UINT8 u8Rtn;
	UINT16 u16DataCount, u16temp, ii;
	//UINT32 u32temp;


	gs_IMMU2_LCCU_Mng.SendBuff[0]= ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = u8CmdId;

	u16DataCount = ENJI_PROT_DATA;
	
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = u8SubCmd;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = Rtn;

	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送	
	u8Rtn = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//OSTimeDly(35);

	return u8Rtn;
}
/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
UINT8 UnpackControllCmd(UINT8 BufNo)        //控制IMSU-D执行特定操作(UCTL)
{
	//__XXX_UNION_VALUE unTempVal;
	UINT8  ucRtn, ucSubCmd, uctemp, ucGunNo, ucBattNo;  //ucAdjustMode
	UINT16 u16DataLen, u16temp;
	SHORT i16temp;
//	Set_Fan_Speed(0);
	//UINT32 u32temp;
	//time_t tmset;
	//struct tm  tmNow;

	ucRtn = 0x00;  //no error

	u16DataLen =((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_5];
	
	ucSubCmd = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+0];	

	gs_IMMU2_LCCU_Mng.u8AckCtrlCmd = ucSubCmd;

	//更新从机本地时间
	if((gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].u16RcvContentLen <  2)
		|| (u16DataLen < 2))
	{
		ucRtn = 0x03;  //format error
		return ucRtn;
	}	

	ucSubCmd = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+0];	

		switch(ucSubCmd)
		{
			/*并离网自动切换模式*/
			case 0x01:
				{
					if(u16DataLen != 2)
					{
						return 0x03;   //format error
					}

					uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					if ((uctemp != 0xA0) && (uctemp != 0xA1))
					{
						return 0x04;
					}
					
					ABU->ControlInfo.OnOffGridSwitchModeAuto = uctemp;
					ABU->ControlInfo.ControlBitmap.bit.OnOffGridSwitchModeAuto = 1;
				}
				break;

			case 0x02:                   //并离网模式   ABU：A1-并网逆变（默认）；A2-离网；
				{
					if(u16DataLen != 2)
					{
						return 0x03;   //format error
					}

					uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					if ((uctemp != 0xA1) && (uctemp != 0xA2))
					{
						return 0x04;
					}
					ABU->ControlInfo.OnOffGridMode = uctemp;
					ABU->ControlInfo.ControlBitmap.bit.OnOffGridMode = 1;
				}
				break;

			case 0x03:                            //开关机    ABU：A0-关机；A1-开机
				{
					if(u16DataLen != 2)
					{
						return 0x03;   //format error
					}

					uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					if ((uctemp != 0xA0) && (uctemp != 0xA1))
					{
						return 0x04;
					}
					
					ABU->ControlInfo.OnOff = uctemp;
					ABU->ControlInfo.ControlBitmap.bit.OnOff = 1;
				}
				break;

			case 0x0B:            //电源柜声光报警   0xA0-Disconnect    0xA1-Connect

				{
					if(u16DataLen != 2)
					{
						return 0x03;   //format error
					}

					uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					if (uctemp == 0xA0)
					{
						gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO5 = 0;
					}
					else if (uctemp == 0xA1)
					{
						gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO5 = 1;
					}
					else
					{
						return 0x04;
					}
				}
				break;

			case 0x0C:                  //电源系统照明     0xA0-Disconnect  0xA1-Connect

				{
					if(u16DataLen != 2)
					{
						return 0x03;   //format error
					}

					uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
					if (uctemp == 0xA0)
					{
						gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO6 = 0;
					}
					else if (uctemp == 0xA1)
					{
						gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO6 = 1;
					}
					else
					{
						return 0x04;
					}
				}
				break;
			case 0x0D:			//交流输入断路器QF1分励脱扣        0xA0-Disconnect（手动模式下才允许控制） 0xA1-Connect

				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
				{
					return 0x04;  //Data invalid
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA0)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO7 = 0;
				}
				else if(uctemp == 0xA1)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO7 = 1;
				}
				else
				{
					return 0x04;  //Data invalid
				}

				break;
			case 0x0E:			    //负载断路器QF2分励脱扣     0xA0-Disconnect（手动模式下才允许控制） 0xA1-Connect

				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
				{
					return 0x04;  //Data invalid
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA0)
				{
					//__ENERGY_BATT_CHG_AC_OFF();
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO8 = 0;
				}
				else if(uctemp == 0xA1)
				{
					//__ENERGY_BATT_CHG_AC_ON();
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO8 = 1;
				}
				else
				{
					return 0x04;  //Data invalid
				}

				break;
//by lxr01202  风扇控制
			case 0x0F:
			case 0x10:
			case 0x11:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
				{
					return 0x04;  //Data invalid
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
				if(uctemp > 100)
				{
					uctemp = 100;
				}
                                if(uctemp == 100)
                                {
                                   BSP_SetPwm1Duty(100);
                                }
                                else if(uctemp == 0)
                                {
                                   BSP_SetPwm1Duty(0);
                                }
                                else
                                {
                                   BSP_SetPwm1Duty(uctemp);
                                }

                                  break;

			case 0x12:                                 //Reset  LCCU        
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				if(gSelfTest.StopFeedDogReason == 0)
				{
					gSelfTest.StopFeedDogReason = 5;
					gSelfTest.u32DogBit_Bak = gSelfTest.u32DogBit;
					gs_E2PROM.fSetHistoryEventUpdate();
				}
				Trap();			

				break;

			case 0x13:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				//清除Kwh统计量
				break;
			/*手动控制指令*/
			case 0x14:              //Manual Ctrl System Rect Group

				if((u16DataLen != 7) && (u16DataLen != 12))//A2105_IMMU2-LCCU-光储通信协议V1.08-2023-12-18,长度增加到12个字节
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
				{
					return 0x04;   
				}
					/*
						0x01-Start Charging
						0x02-Pending (Adjust Current)
						0x03-Start DisCharge
						0x00-Stop Charging

						0x01-GroupA_AC/DC
						0x02-GroupB_AC/DC
						0x03-GroupA_DC/DC
						0x04-GroupB_DC/DC
						0x05-GroupA_MPPT
						0x06-GroupB_MPPT

					*/

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];
				ucGunNo = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2];

				if(ucGunNo == 0)  //关所有模块
				{
					if (uctemp == 0x00)
					{
						for(ucBattNo = 0; ucBattNo < ImmuCtrlGrpNo_MAX;ucBattNo++)
						{
							gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucBattNo] = IMMU2ManualCtrlGrpCmd_Stop;
						}
						return 0x00;  //no error
					}
					else if (uctemp == 0x01)//开始充电    0x01-Start Charging
					{
						for(ucBattNo = 0; ucBattNo < ImmuCtrlGrpNo_MAX;ucBattNo++)
						{
							gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucBattNo] = IMMU2ManualCtrlGrpCmd_Chging;
							u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4];
							gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucBattNo] = (float)u16temp * 0.1;
							u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+6];
							gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucBattNo] = (float)u16temp * 0.1;

							if((ABS(gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpVolt[ucGunNo]-gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucGunNo]) > 2) \
							|| (ABS(gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpCurr[ucGunNo]-gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucGunNo]) > 1))
							{
								gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpVolt[ucGunNo] = gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucGunNo];
								gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpCurr[ucGunNo] = gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucGunNo];
								gEvChargeInfo.u8SetAdjVoltCurrDelay = 0;
							}
							
							if(u16DataLen == 12)
							{
								//交流调度
								gs_IMMU2_LCCU_Mng.u8ManualIsAcDispatchMode = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+7];
								//有功
								i16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+8] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+9];
								gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucBattNo] = i16temp;
								//无功
								i16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+10] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+11];
								gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucBattNo] = i16temp;

								if((ABS(gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpP[ucBattNo]-gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucBattNo]) > 1) \
								||(ABS(gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpQ[ucBattNo]-gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucBattNo]) > 1))
								{
									gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpP[ucBattNo] = gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucBattNo];
									gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpQ[ucBattNo] = gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucBattNo];
									gEvChargeInfo.u8SetWorkModeDelay = 0;
								}
							}
							else
							{
								//这里兼容IMMU2版本
								gs_IMMU2_LCCU_Mng.u8ManualIsAcDispatchMode = 0;
							}

						}
						return 0x00;  //no error
					}
					else if (uctemp == 0x03)          //0x03-Start DisCharge            
					{
						for(ucBattNo = 0; ucBattNo < ImmuCtrlGrpNo_MAX;ucBattNo++)
						{
							gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucBattNo] = IMMU2ManualCtrlGrpCmd_Dischging;
							u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4];
							gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucBattNo] = (float)u16temp * 0.1;
							u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+6];
							gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucBattNo] = (float)u16temp * 0.1;	
							
							if((ABS(gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpVolt[ucGunNo]-gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucGunNo]) > 2) \
							|| (ABS(gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpCurr[ucGunNo]-gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucGunNo]) > 1))
							{
								gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpVolt[ucGunNo] = gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucGunNo];
								gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpCurr[ucGunNo] = gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucGunNo];
								gEvChargeInfo.u8SetAdjVoltCurrDelay = 0;
							}						

							if(u16DataLen == 12)
							{
								//交流调度
								gs_IMMU2_LCCU_Mng.u8ManualIsAcDispatchMode = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+7];
								//有功
								i16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+8] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+9];
								gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucBattNo] = i16temp;
								//无功
								i16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+10] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+11];
								gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucBattNo] = i16temp;

								if((ABS(gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpP[ucBattNo]-gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucBattNo]) > 1) \
								||(ABS(gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpQ[ucBattNo]-gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucBattNo]) > 1))
								{
									gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpP[ucBattNo] = gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucBattNo];
									gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpQ[ucBattNo] = gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucBattNo];
									gEvChargeInfo.u8SetWorkModeDelay = 0;
								}
							}
							else
							{
								//这里兼容IMMU2版本
								gs_IMMU2_LCCU_Mng.u8ManualIsAcDispatchMode = 0;
							}
						}
						return 0x00;  //no error
					}
					else
					{
						return 0x03;
					}
				}
				else //if(ucGunNo > 0)
				{
					ucGunNo--;
				}
				

				if(ucGunNo < ImmuCtrlGrpNo_MAX)
				{
					if (uctemp == 0x01)
					{
						gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucGunNo] = IMMU2ManualCtrlGrpCmd_Chging;
						gs_CANComm.u8GRP_RectOnOff[ucGunNo] = RECT_ON;//这里立即给开关机赋值，因为手动模式，开关机命令就下发一次，避免用的上次值
					}
					else if (uctemp == 0x03)
					{
						gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucGunNo] = IMMU2ManualCtrlGrpCmd_Dischging;
						gs_CANComm.u8GRP_RectOnOff[ucGunNo] = RECT_ON;//这里立即给开关机赋值，因为手动模式，开关机命令就下发一次，避免用的上次值
					}
					else if (uctemp == 0x00)
					{
						gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucGunNo] = IMMU2ManualCtrlGrpCmd_Stop;
						gs_CANComm.u8GRP_RectOnOff[ucGunNo] = RECT_OFF;//这里立即给开关机赋值，因为手动模式，开关机命令就下发一次，避免用的上次值
					}
					else if ((uctemp == 0x02) && (gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucGunNo] == IMMU2ManualCtrlGrpCmd_Chging))
					{				
					}
					else if ((uctemp == 0x04) && (gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucGunNo] == IMMU2ManualCtrlGrpCmd_Dischging))
					{		
									
					}
					else
					{
						return 0x03;
					}
					u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4];
					gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucGunNo] = (float)u16temp * 0.1;
					u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+6];
					gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucGunNo] = (float)u16temp * 0.1;
					if((ABS(gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpVolt[ucGunNo]-gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucGunNo]) > 2) \
					|| (ABS(gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpCurr[ucGunNo]-gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucGunNo]) > 1))
					{
						gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpVolt[ucGunNo] = gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucGunNo];
						gs_IMMU2_LCCU_Mng.fLastManualCtrlGrpCurr[ucGunNo] = gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucGunNo];
						gEvChargeInfo.u8SetAdjVoltCurrDelay = 0;
					}
					if(u16DataLen == 12)
					{
						//交流调度
						gs_IMMU2_LCCU_Mng.u8ManualIsAcDispatchMode = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+7];
						//有功
						i16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+8] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+9];
						gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucGunNo] = i16temp;
						//无功
						i16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+10] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+11];
						gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucGunNo] = i16temp;

						if((ABS(gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpP[ucBattNo]-gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucBattNo]) > 1) \
						||(ABS(gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpQ[ucBattNo]-gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucBattNo]) > 1))
						{
							gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpP[ucBattNo] = gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[ucBattNo];
							gs_IMMU2_LCCU_Mng.i16LastManualCtrlGrpQ[ucBattNo] = gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[ucBattNo];
							gEvChargeInfo.u8SetWorkModeDelay = 0;
						}
					}
					else
					{
						//这里兼容IMMU2版本
						gs_IMMU2_LCCU_Mng.u8ManualIsAcDispatchMode = 0;
					}
				}
				
				break;

			case 0x15:

				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xAA)
				{
					//Control Calibrate of Bus Voltage (Gun A)  	0xAA-Excute Now		(For maintenance use only)
					//只有A/B都空闲待机态才允许校准 - 仅支持自动校准
					/*if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_Standby) 
						&& (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_Standby))*/
					{
						//ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_AdjustMode_NUM, &unTempVal, NULL, BY_E2PROM);
						//ucAdjustMode = unTempVal.chtemp[0];

						if(gEvChargeInfo.u8_AutoTestStep > 0)
						{
							//自动校准中，不处理
						}
						else// if(ucAdjustMode == 0)  //自动测试
						{
							//若在校准中，则不处理
							if ((gEvChargeInfo.u8_A_TestMode == TRUE) || (gEvChargeInfo.u8_B_TestMode == TRUE))
							{

							}
							else
							{
								gEvChargeInfo.u8_AutoTestStep = 1;
								gEvChargeInfo.u8_A_TestMode = TRUE;
							}
						}					
					}
				}// END:  if(uctemp == 0xAA)

				break;

			case 0x16:

				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xAA)
				{
					//Control Calibrate of Bus Voltage (Gun A)  	0xAA-Excute Now		(For maintenance use only)
					//只有A/B都空闲待机态才允许校准 - 仅支持自动校准
					/*if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_Standby) 
						&& (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_Standby))*/
					{
						//ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_AdjustMode_NUM, &unTempVal, NULL, BY_E2PROM);
						//ucAdjustMode = unTempVal.chtemp[0];

						if(gEvChargeInfo.u8_AutoTestStep > 0)
						{
							//自动校准中，不处理
						}
						else// if(ucAdjustMode == 0)  //自动测试
						{
							//若在校准中，则不处理
							if ((gEvChargeInfo.u8_A_TestMode == TRUE) || (gEvChargeInfo.u8_B_TestMode == TRUE))
							{

							}
							else
							{
								gEvChargeInfo.u8_AutoTestStep = 1;
								gEvChargeInfo.u8_B_TestMode = TRUE;
							}
						}					
					}
				}// END:  if(uctemp == 0xAA)

				break;

			case 0x17:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				

				if(uctemp == 0xA1)                          //手动模式
				{
					for(ucBattNo = 0; ucBattNo < ImmuCtrlGrpNo_MAX;ucBattNo++)
					{
						gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[ucBattNo] = IMMU2ManualCtrlGrpCmd_Stop;
						gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[ucBattNo] = (float)0.1;
						gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[ucBattNo] = (float)50;
					}				
//by lxr01202 手动模式下光伏自动断开
					//BSP_DO_Ctrl(NEWDO12, DO_OFF);
					//__MPPT_1_INPUT_OFF();
					//BSP_DO_Ctrl(NEWDO14, DO_OFF);
					//__MPPT_2_INPUT_OFF();
					gEvChargeInfo.u8TEST_MODE_FLAG = TRUE;
					

				}
				else                                     //自动模式
				{
					gEvChargeInfo.u8TEST_MODE_FLAG = FALSE;
					gIMEU_Info[0].u8PWMCtrlSpeed[0] = 0;
					gIMEU_Info[1].u8PWMCtrlSpeed[0] = 0;
					gIMEU_Info[0].u8PWMCtrlSpeed[1] = 0;
					gIMEU_Info[1].u8PWMCtrlSpeed[1] = 0;

// by lxr01202自动模式下，仅在离网时打开光伏（补充条件）
					// if (ABU->ContravariantStat0.bit.IsGirdOnOff) 
					// {
					// 	__MPPT_1_INPUT_ON();
					// 	__MPPT_2_INPUT_ON();
					// 	SEGGER_RTT_printf(0,"123.\n\r");
					// }
					// // 并网时也保持自动模式原有的打开逻辑
					// else 
					//{
					//	__MPPT_1_INPUT_ON();
					//	__MPPT_2_INPUT_ON();
					//	SEGGER_RTT_printf(0,"213.\n\r");
					//}
					
				}

				RecordEvent(HISTORY_EVENT_DEBUG_MODE, uctemp, 0, 0);
				break;

			case 0x18:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA1)  //储能电池充电模块输入接触器 
				{
					__ENERGY_BATT_CHG_AC_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__ENERGY_BATT_CHG_AC_OFF();
				}
				break;
			
			case 0x19:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA1)  //AC 输入接触器控制  闭合
				{
					__ACLOAD_MCCB_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__ACLOAD_MCCB_OFF();
				}
				break;

			//电池柜直流接入接触器控制信号
			case 0x20:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if((uctemp == 0xA1) && (gEvChargeInfo.u8EPOAlm == FALSE)) //接触器控制  闭合
				{
					__BATT_GRP_1_INPUT_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__BATT_GRP_1_INPUT_OFF();
				}
				break;
			case 0x21:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if((uctemp == 0xA1) && (gEvChargeInfo.u8EPOAlm == FALSE)) //接触器控制  闭合
				{
					__BATT_GRP_2_INPUT_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__BATT_GRP_2_INPUT_OFF();
				}
				break;
			case 0x22:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if((uctemp == 0xA1) && (gEvChargeInfo.u8EPOAlm == FALSE)) //接触器控制  闭合
				{
					__BATT_GRP_3_INPUT_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__BATT_GRP_3_INPUT_OFF();
				}
				break;
			case 0x23:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if((uctemp == 0xA1) && (gEvChargeInfo.u8EPOAlm == FALSE)) //接触器控制  闭合
				{
					__BATT_GRP_4_INPUT_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__BATT_GRP_4_INPUT_OFF();
				}
				break;
//by lxr01202  mppt接入接触器控制信号   DO12   DO14
			case 0x24:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA1)  //光伏接触器控制  闭合
				{
					__MPPT_1_INPUT_ON();
					__A_PV1_INPUT_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__MPPT_1_INPUT_OFF();
					__A_PV1_INPUT_OFF();
				}
				break;
			case 0x25:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA1)  //接触器控制  闭合
				{
					__MPPT_2_INPUT_ON();
					__A_PV2_INPUT_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__MPPT_2_INPUT_OFF();
					__A_PV2_INPUT_OFF();
				}
				break;
			case 0x26:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA1)  //接触器控制  闭合
				{
					__MPPT_3_INPUT_ON();
					__A_PV3_INPUT_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__MPPT_3_INPUT_OFF();
					__A_PV3_INPUT_OFF();
				}
				break;
			case 0x27:
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}
				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA1)  //接触器控制  闭合
				{
					__MPPT_4_INPUT_ON();
					__A_PV4_INPUT_ON();
				}
				else if(uctemp == 0xA0)  //断开
				{
					__MPPT_4_INPUT_OFF();
					__A_PV4_INPUT_OFF();
				}
				break;
				//DCLOAD接入接触器控制信号
			case 0x28:
				// if(u16DataLen != 2)
				// {
				// 	return 0x03;   //format error
				// }
				// uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				// if(uctemp == 0xA1)  //接触器控制  闭合
				// {
				// 	__DCLOAD_1_INPUT_ON();
				// }
				// else if(uctemp == 0xA0)  //断开
				// {
				// 	__DCLOAD_1_INPUT_OFF();
				// }
				// break;
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
				{
					return 0x04;  //Data invalid
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA0)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO2 = 0;
				}
				else if(uctemp == 0xA1)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO2 = 1;
				}
				else
				{
					return 0x04;  //Data invalid
				}
				break;
			case 0x29:
				// if(u16DataLen != 2)
				// {
				// 	return 0x03;   //format error
				// }
				// uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				// if(uctemp == 0xA1)  //接触器控制  闭合
				// {
				// 	__DCLOAD_2_INPUT_ON();
				// }
				// else if(uctemp == 0xA0)  //断开
				// {
				// 	__DCLOAD_2_INPUT_OFF();
				// }
				// break;
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
				{
					return 0x04;  //Data invalid
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA0)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO3 = 0;
				}
				else if(uctemp == 0xA1)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO3 = 1;
				}
				else
				{
					return 0x04;  //Data invalid
				}
				break;
			case 0x2A:
				// if(u16DataLen != 2)
				// {
				// 	return 0x03;   //format error
				// }
				// uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				// if(uctemp == 0xA1)  //接触器控制  闭合
				// {
				// 	__DCLOAD_3_INPUT_ON();
				// }
				// else if(uctemp == 0xA0)  //断开
				// {
				// 	__DCLOAD_3_INPUT_OFF();
				// }
				// break;
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
				{
					return 0x04;  //Data invalid
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA0)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO4 = 0;
				}
				else if(uctemp == 0xA1)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO4 = 1;
				}
				else
				{
					return 0x04;  //Data invalid
				}
				break;
			case 0x2B:
				// if(u16DataLen != 2)
				// {
				// 	return 0x03;   //format error
				// }
				// uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				// if(uctemp == 0xA1)  //接触器控制  闭合
				// {
				// 	__DCLOAD_4_INPUT_ON();
				// }
				// else if(uctemp == 0xA0)  //断开
				// {
				// 	__DCLOAD_4_INPUT_OFF();
				// }
				// break;
				if(u16DataLen != 2)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
				{
					return 0x04;  //Data invalid
				}

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(uctemp == 0xA0)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO10 = 0;
				}
				else if(uctemp == 0xA1)
				{
					gIMEU_Info[0].DO_CtrlValue.BIT_FIELD.DO10 = 1;
				}
				else
				{
					return 0x04;  //Data invalid
				}
				break;
			/*自动模式下控制PCS充放电*/
			case 0x31:
			case 0x32:
			case 0x33:
			case 0x34:

				if(u16DataLen != 6)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)
				{
					return 0x04;   
				}
				ucBattNo = ucSubCmd - 0x31;
				ucGunNo = ucBattNo;

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				
				if((ucBattNo < BATT_GRP_MAX_NUM) && (ucGunNo < ImmuCtrlGrpNo_MAX))
				{
					if (uctemp == 0x01)//充电
					{
						if(gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] != IMMU2ManualCtrlGrpCmd_Chging)
							RecordEvent(HISTORY_EVENT_IMMU2_STOP_BATT_CHG, ucGunNo, ucBattNo, 1);

						gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] = IMMU2ManualCtrlGrpCmd_Chging;

						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].u8TaskCtrlCmd = IMMU2ManualCtrlGrpCmd_Chging;

						u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3];
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].ChargeVoltReq = (float)u16temp * 0.1;

						u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5];
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].ChargeCurrReq = (float)u16temp * 0.1;

						g_u16MpptAdjustPeriod[ucBattNo] = 0;
					}					
					else if (uctemp == 0x03)//放电
					{	
						if(gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] != IMMU2ManualCtrlGrpCmd_Dischging)
							RecordEvent(HISTORY_EVENT_IMMU2_STOP_BATT_CHG, ucGunNo, ucBattNo, 3);

						gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] = IMMU2ManualCtrlGrpCmd_Dischging;

						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].u8TaskCtrlCmd = IMMU2ManualCtrlGrpCmd_Dischging;

						u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3];
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].ChargeVoltReq = (float)u16temp * 0.1;

						u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5];
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].EVDischargeCurrReq = (float)u16temp * 0.1;

						g_u16MpptAdjustPeriod[ucBattNo] = 0;
					}
					else if (uctemp == 0x00)
					{	
						if(gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] != IMMU2ManualCtrlGrpCmd_Stop)
							RecordEvent(HISTORY_EVENT_IMMU2_STOP_BATT_CHG, ucGunNo, ucBattNo, 0);


						gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] = IMMU2ManualCtrlGrpCmd_Stop;

						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].u8TaskCtrlCmd = IMMU2ManualCtrlGrpCmd_Stop;

						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].ChargeCurrReq = 0;
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].EVDischargeCurrReq = 0;

						g_u16MpptAdjustPeriod[ucBattNo] = 0;						
					}
					else if((uctemp == 0x02) && (gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] == IMMU2ManualCtrlGrpCmd_Chging))
					{
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].u8TaskCtrlCmd = IMMU2ManualCtrlGrpCmd_Chging;

						u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3];
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].ChargeVoltReq = (float)u16temp * 0.1;

						u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5];
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].ChargeCurrReq = (float)u16temp * 0.1;

						g_u16MpptAdjustPeriod[ucBattNo] = 0;
					}
					else if((uctemp == 0x04) && (gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] == IMMU2ManualCtrlGrpCmd_Dischging))
					{
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].u8TaskCtrlCmd = IMMU2ManualCtrlGrpCmd_Dischging;

						u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3];
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].ChargeVoltReq = (float)u16temp * 0.1;

						u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5];
						gs_IMMU2_LCCU_Mng.ESBattMng[ucBattNo].EVDischargeCurrReq = (float)u16temp * 0.1;

						g_u16MpptAdjustPeriod[ucBattNo] = 0;
					}
					else
					{
						return 0x03;
					}

					
					u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3];
					gs_IMMU2_LCCU_Mng.fTaskCtrlGrpVolt[ucGunNo] = (float)u16temp * 0.1;
					u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5];
					gs_IMMU2_LCCU_Mng.fTaskCtrlGrpCurr[ucGunNo] = (float)u16temp * 0.1;
					if((ABS(gs_IMMU2_LCCU_Mng.fLastTaskCtrlGrpVolt[ucGunNo]-gs_IMMU2_LCCU_Mng.fTaskCtrlGrpVolt[ucGunNo]) > 2) \
					|| (ABS(gs_IMMU2_LCCU_Mng.fLastTaskCtrlGrpCurr[ucGunNo]-gs_IMMU2_LCCU_Mng.fTaskCtrlGrpCurr[ucGunNo]) > 1))
					{
						gs_IMMU2_LCCU_Mng.fLastTaskCtrlGrpVolt[ucGunNo] = gs_IMMU2_LCCU_Mng.fTaskCtrlGrpVolt[ucGunNo];
						gs_IMMU2_LCCU_Mng.fLastTaskCtrlGrpCurr[ucGunNo] = gs_IMMU2_LCCU_Mng.fTaskCtrlGrpCurr[ucGunNo];
						gEvChargeInfo.u8SetAdjVoltCurrDelay = 0;
						if(gEvChargeInfo.u8TaskIsAcDispatchMode == 1)
						{
							gEvChargeInfo.u8SetWorkModeDelay = 0;
						}
					}
				}

				break;

			
			case 0x51:
			case 0x52:
			case 0x53:
			case 0x54:

				if(u16DataLen != 6)
				{
					return 0x03;   //format error
				}

				if(gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)
				{
					return 0x04;   
				}
				ucGunNo = ucSubCmd - 0x51+ImmuCtrlGrpNo_8;

				uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+1];

				if(ucGunNo < ImmuCtrlGrpNo_MAX)
				{
					if (uctemp == 0x01)
					{
						gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] = IMMU2ManualCtrlGrpCmd_Chging;
					}					
					else if (uctemp == 0x00)
					{	
						gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] = IMMU2ManualCtrlGrpCmd_Stop;
					}
					else if((uctemp == 0x02) && (gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[ucGunNo] == IMMU2ManualCtrlGrpCmd_Chging))
					{
					}					
					else
					{
						return 0x03;
					}
					u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+2] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+3];
					gs_IMMU2_LCCU_Mng.fTaskCtrlGrpVolt[ucGunNo] = (float)u16temp * 0.1;
					u16temp = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+5];
					gs_IMMU2_LCCU_Mng.fTaskCtrlGrpCurr[ucGunNo] = (float)u16temp * 0.1;
				}

				break;
			

			default:
				ucRtn = 0x02; //0x02	CMD invalid
				break;

		} // end: switch(ucSubCmd)



	return ucRtn;
}
/*==========================================================================*
* 函数名称 :  启动升级
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
UINT8 StartUpdate(UINT8 BufNo)
{
	//__XXX_UNION_VALUE unTempVal;
	UINT8  uctemp, i;
	UINT16 u16DataLen,  u16Pos;
	UINT32 u32temp;
	

//	u8Rtn = 0;  //0-rejiect  1-accept

	u16DataLen =((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_5];

	if((gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].u16RcvContentLen <  5)
		|| (u16DataLen < 5))
	{		
		return 0;
	}	

	u16Pos = ENJI_PROT_DATA+0;

	u32temp  = ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<24)  
		+ ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1]<<16)
		+ ((UINT32)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+2]<<8) 
		+ gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+3];

	
	if(gEvChargeInfo.u8BootVersion >= BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k)
	{
		if(u32temp > (EX_UPGRADE_BLOCK_MAX_NUM_OVER512K * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE))
		{
			return 0;
		}	
	}
	else if(u32temp > (EX_UPGRADE_BLOCK_MAX_NUM * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE))
	{
		return 0;
	}	

	u16Pos += 4;
	uctemp = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos];

	
	if((uctemp != UpdateFileTypeDef_0)
		&& (uctemp != UpdateFileTypeDef_1)
		&& (uctemp != UpdateFileTypeDef_2)
		&& (uctemp != UpdateFileABUTypeDef_1)
		&& (uctemp != UpdateFileABUTypeDef_2)
		&& (uctemp != UpdateFileEU2_1TypeDef)
		&& (uctemp != UpdateFileEU2_2TypeDef)
		&& (uctemp != UpdateFileMPPT_1TypeDef)
		&& (uctemp != UpdateFileMPPT_2TypeDef)
		)
	{
		return 0;
	}
	gs_UpGradeCtrl.u8UpdateFileType = uctemp;
	u16Pos += 1;

	if(gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].u16RcvContentLen <  (5+32))
	{
		return 0;
	}

	for(i = 0; i < 32; i++)
	{
		gs_UpGradeCtrl.cRcvFileName[i] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+i];
	}
	gs_UpGradeCtrl.cRcvFileName[32] = 0;

	if(strstr(gs_UpGradeCtrl.cRcvFileName, "IMSU_APP.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileTypeDef_0)
		{
			return 0;
		}
	}
	//by lxr01202 MPPT升级文件名判断
	else if(strstr(gs_UpGradeCtrl.cRcvFileName, "MPPT_U1.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileMPPT_1TypeDef)
		{
			return 0;
		}
	}
	else if(strstr(gs_UpGradeCtrl.cRcvFileName, "MPPT_U2.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileMPPT_2TypeDef)
		{
			return 0;
		}
	}
	else if(strstr(gs_UpGradeCtrl.cRcvFileName, "ABU_U1.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileABUTypeDef_1)
		{
			return 0;
		}
	}
	else if(strstr(gs_UpGradeCtrl.cRcvFileName, "ABU_U2.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileABUTypeDef_2)
		{
			return 0;
		}
	}

	else if(strstr(gs_UpGradeCtrl.cRcvFileName, "U1.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileTypeDef_1)
		{
			return 0;
		}
	}
	else if(strstr(gs_UpGradeCtrl.cRcvFileName, "U2.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileTypeDef_2)
		{
			return 0;
		}
	}
	else if(strstr(gs_UpGradeCtrl.cRcvFileName, "EU2_1.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileEU2_1TypeDef)
		{
			return 0;
		}
	}
	else if(strstr(gs_UpGradeCtrl.cRcvFileName, "EU2_2.bin") != NULL)
	{
		if(gs_UpGradeCtrl.u8UpdateFileType != UpdateFileEU2_2TypeDef)
		{
			return 0;
		}
	}


	else
	{
		return 0;
	}
	//gs_UpGradeCtrl.u8UpdateFileType = uctemp;

	gs_UpGradeCtrl.u32FileSize = u32temp;	

	u16Pos += 32;
	gs_UpGradeCtrl.u16BinFileCRC = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+0]<<8) 
		+ gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
	gs_UpGradeCtrl.u16CalRcvCRC = 0;

	gs_UpGradeCtrl.fProgress = 0;
	gs_UpGradeCtrl.u8UpgradeStat = UpgradeStatDef_Start;			
	gs_UpGradeCtrl.u32UpgradeWaitIndex = 1;
	gs_UpGradeCtrl.RcvFrameNo = 0;
	gs_UpGradeCtrl.u32TotalRcvByteNum = 0;

	//gs_UpGradeCtrl.u32TotalFrameNumber = gs_UpGradeCtrl.u32FileSize / gs_UpGradeCtrl.ByteNumPerFrame;
	//if((gs_UpGradeCtrl.u32FileSize % gs_UpGradeCtrl.ByteNumPerFrame) != 0)
	//{
	//	gs_UpGradeCtrl.u32TotalFrameNumber += 1;
	//}

//by lxr01202 新增MPPT升级文件（擦除flash）
	if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_1TypeDef))
	{
		FlashClean_U1_Data();
	}
	else if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_2) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_2)|| (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_2TypeDef))
	{
		FlashClean_U2_Data();
	}
	else if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_1TypeDef) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_2TypeDef))
	{
		FlashClean_IMEU2_Data();
	}
	else
	{
		UpGradeEraseExFlash();  //先擦除一遍
	}

	return 1;

}

UINT8 Update_RcvData(UINT8 BufNo)
{
	__XXX_UNION_VALUE unTempVal;
	UINT8  u8Tdata[4];
	UINT16 u16DataLen, u16RcvFrameNo, u16Pos, ii;
	UINT32 Addres;

	if(gs_UpGradeCtrl.u8UpgradeStat == UpgradeStatDef_End)
	{
		return UpdateStartResult_DownLoaded;
	}
	else if(gs_UpGradeCtrl.u8UpgradeStat == UpgradeStatDef_CrcFail)
	{
		return UpdateStartResult_DownLoadedFailed;
	}

	if(gs_UpGradeCtrl.u8UpgradeStat != UpgradeStatDef_Start)
	{
		return UpdateStartResult_Idle;
	}


//	u8Rtn = UpdateStartResult_DownLoadeding;

	u16DataLen =((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_4] << 8) + gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_5];

	if((gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].u16RcvContentLen > (UPGRADE_ONE_FRAME_LENGTH+2+9))
		|| (u16DataLen > (UPGRADE_ONE_FRAME_LENGTH+2))
		|| (u16DataLen < 2))
	{		
		return UpdateStartResult_DownLoadedFailed;
	}	

	u16Pos = ENJI_PROT_DATA+0;

	u16RcvFrameNo  = ((UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos]<<8)  
		+ (UINT16)gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos+1];
	u16Pos += 2;

	if(u16RcvFrameNo == gs_UpGradeCtrl.u32UpgradeWaitIndex)
	{
		u16DataLen -= 2;

		if((u16DataLen + gs_UpGradeCtrl.u32TotalRcvByteNum) > gs_UpGradeCtrl.u32FileSize)
		{
			return  UpdateStartResult_DownLoadedFailed;
		}	
		else
		{
			gs_UpGradeCtrl.RcvFrameNo = u16RcvFrameNo;

			gs_UpGradeCtrl.RcvByteNum = u16DataLen;

			for(ii = 0; ii < u16DataLen; ii++)
			{
				gs_UpGradeCtrl.UpGradeData[ii] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[u16Pos++];

				gs_UpGradeCtrl.u16CalRcvCRC += gs_UpGradeCtrl.UpGradeData[ii];
			}
			

			//写flash
			Addres = gs_UpGradeCtrl.u32TotalRcvByteNum;
//by lxr01202 新增MPPT升级文件（写入flash）
			if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_1) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_1TypeDef))
			{
				FlashWrite_U1_Data(gs_UpGradeCtrl.UpGradeData, Addres, gs_UpGradeCtrl.RcvByteNum);
			}
			else if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_2) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_2) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_2TypeDef))
			{
				FlashWrite_U2_Data(gs_UpGradeCtrl.UpGradeData, Addres, gs_UpGradeCtrl.RcvByteNum);
			}
			else if((gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_1TypeDef) || (gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_2TypeDef))
			{
				FlashWrite_IMEU2_Data(gs_UpGradeCtrl.UpGradeData, Addres, gs_UpGradeCtrl.RcvByteNum);
			}
			else
			{
				FLASH_UpgradeWriteBytes(gs_UpGradeCtrl.UpGradeData, Addres, gs_UpGradeCtrl.RcvByteNum);
			}

			
			gs_UpGradeCtrl.u32TotalRcvByteNum += gs_UpGradeCtrl.RcvByteNum;
			gs_UpGradeCtrl.u32UpgradeWaitIndex++;

			if(gs_UpGradeCtrl.u32TotalRcvByteNum < gs_UpGradeCtrl.u32FileSize)
			{
				gs_UpGradeCtrl.fProgress = (float)gs_UpGradeCtrl.u32TotalRcvByteNum / (float)gs_UpGradeCtrl.u32FileSize * 100;				
			}
			else  //完成
			{
				if(gs_UpGradeCtrl.u16CalRcvCRC != gs_UpGradeCtrl.u16BinFileCRC)
				{
					gs_UpGradeCtrl.u8UpgradeStat = UpgradeStatDef_CrcFail;
					return UpdateStartResult_DownLoadedFailed;
				}


				OSTimeDly(2);

				if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_2)
				{
					unTempVal.u32Value = gs_UpGradeCtrl.u32FileSize;
					dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, RECT_IS_UPDATE_FLAG);
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_IS_UPDATE_FLAG;
					ExSetSigInfo(E_TYPE_SYS, ID_U2_BYTES_NUM, unTempVal,NULL, BY_E2PROM);

					memcpy(&gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade[0], &gs_UpGradeCtrl.cRcvFileName[0], 7);	
					gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade[7] = 0;
					gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[1] = 1;
				}
				else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileTypeDef_1)
				{
					unTempVal.u32Value = gs_UpGradeCtrl.u32FileSize;
					dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, RECT_IS_UPDATE_FLAG);
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_IS_UPDATE_FLAG;
					ExSetSigInfo(E_TYPE_SYS, ID_U1_BYTES_NUM, unTempVal,NULL, BY_E2PROM);
					memcpy(&gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade[0], &gs_UpGradeCtrl.cRcvFileName[0], 7);
					gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade[7] = 0;
					gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[0] = 1;
				}
				else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_2)
				{
					unTempVal.u32Value = gs_UpGradeCtrl.u32FileSize;
					dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, ABU_IS_UPDATE_FLAG);
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = ABU_IS_UPDATE_FLAG;
					ExSetSigInfo(E_TYPE_SYS, ID_U2_BYTES_NUM, unTempVal,NULL, BY_E2PROM);

					memcpy(&gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade[0], &gs_UpGradeCtrl.cRcvFileName[0], 7);	
					gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade[7] = 0;
					gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[1] = 1;
				}
				else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileABUTypeDef_1)
				{
					unTempVal.u32Value = gs_UpGradeCtrl.u32FileSize;
					dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, ABU_IS_UPDATE_FLAG);
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = ABU_IS_UPDATE_FLAG;
					ExSetSigInfo(E_TYPE_SYS, ID_U1_BYTES_NUM, unTempVal,NULL, BY_E2PROM);
					memcpy(&gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade[0], &gs_UpGradeCtrl.cRcvFileName[0], 7);
					gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade[7] = 0;
					gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[0] = 1;
				}
//by lxr01202 新增MPPT_U1.bin和MPPT_U2.bin升级文件
				else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_1TypeDef)
				{
					unTempVal.u32Value = gs_UpGradeCtrl.u32FileSize;
					dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, MPPT_IS_UPDATE_FLAG);
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = MPPT_IS_UPDATE_FLAG;
					ExSetSigInfo(E_TYPE_SYS, ID_U1_BYTES_NUM, unTempVal,NULL, BY_E2PROM);
					memcpy(&gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade[0], &gs_UpGradeCtrl.cRcvFileName[0], 7);
					gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade[7] = 0;
					gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[0] = 1;
				}
				else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileMPPT_2TypeDef)
				{
					unTempVal.u32Value = gs_UpGradeCtrl.u32FileSize;
					dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, MPPT_IS_UPDATE_FLAG);
					gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = MPPT_IS_UPDATE_FLAG;
					ExSetSigInfo(E_TYPE_SYS, ID_U2_BYTES_NUM, unTempVal,NULL, BY_E2PROM);
					memcpy(&gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade[0], &gs_UpGradeCtrl.cRcvFileName[0], 7);
					gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade[7] = 0;
					gs_UpgradePowerModuleCtrl.uWriteBinFileNameToEepromFlag[1] = 1;
				}			


				else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_1TypeDef)
				{
					u8Tdata[3] = 0xff & (gs_UpGradeCtrl.u32FileSize>>24);
					u8Tdata[2] = 0xff & (gs_UpGradeCtrl.u32FileSize>>16);
					u8Tdata[1] = 0xff & (gs_UpGradeCtrl.u32FileSize>>8);
					u8Tdata[0] = gs_UpGradeCtrl.u32FileSize&0xff;
					FlashWrite_IMEU2_DataLen(u8Tdata, 0, 4);

					if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist == 1)
					{
						g_stImeu2UpgradeInfo[0].u8UpgradeFlag = TRUE;
					}
				}
				else if(gs_UpGradeCtrl.u8UpdateFileType == UpdateFileEU2_2TypeDef)
				{
					u8Tdata[3] = 0xff & (gs_UpGradeCtrl.u32FileSize>>24);
					u8Tdata[2] = 0xff & (gs_UpGradeCtrl.u32FileSize>>16);
					u8Tdata[1] = 0xff & (gs_UpGradeCtrl.u32FileSize>>8);
					u8Tdata[0] = gs_UpGradeCtrl.u32FileSize&0xff;
					FlashWrite_IMEU2_DataLen(u8Tdata, 0, 4);

					if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist == 1)
					{
						g_stImeu2UpgradeInfo[1].u8UpgradeFlag = TRUE;
					}
				}
				else
				{
					u8Tdata[3] = 0xff & (gs_UpGradeCtrl.u32FileSize>>24);
					u8Tdata[2] = 0xff & (gs_UpGradeCtrl.u32FileSize>>16);
					u8Tdata[1] = 0xff & (gs_UpGradeCtrl.u32FileSize>>8);
					u8Tdata[0] = gs_UpGradeCtrl.u32FileSize&0xff;
					FLASH_EXTableWriteBytes(u8Tdata,0, 4);
					OSTimeDly(2);		
					
					gs_UpGradeCtrl.u8UpgradeNeedReset = UPGRADE_RESET_DELAY;	

					gs_UpGradeCtrl.u8UpgradeWriteFlag = 1;
				}

				gs_UpGradeCtrl.u8UpgradeStat = UpgradeStatDef_End;


				

				return UpdateStartResult_DownLoaded; 
			}
		}
	}

	

	return UpdateStartResult_DownLoadeding;

}

UCHAR  LccuAckUpgradeResult(UINT8 u8CmdId, UINT8 Rtn)
{
	//__XXX_UNION_VALUE unTempVal;

	//UINT8  uctemp, u8Idx;
	UINT8 u8SendResult;
	UINT16 u16DataCount, u16temp, ii;
	//UINT32 u32temp;


	gs_IMMU2_LCCU_Mng.SendBuff[0]= ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = u8CmdId;

	u16DataCount = ENJI_PROT_DATA;

	if(u8CmdId == 0x12)
	{
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = Rtn;
	}
	else if(u8CmdId == 0x13)
	{
		u16temp = gs_UpGradeCtrl.RcvFrameNo;
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);
		gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = Rtn;
	}
	else
	{
		return TRUE;
	}
	
	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送	
	u8SendResult = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//OSTimeDly(35);

	return u8SendResult;
}

UCHAR  LccuAckPcsCmdResult(UINT8 u8CmdId, UINT8 Rtn)
{
	//__XXX_UNION_VALUE unTempVal;

	UINT8  u8Idx, j;
	UINT8 u8SendResult;
	UINT16 u16DataCount, u16temp, ii;
	//UINT32 u32temp;


	gs_IMMU2_LCCU_Mng.SendBuff[0]= ENJI_SOI_1;
	gs_IMMU2_LCCU_Mng.SendBuff[1] = ENJI_SOI_2;
	gs_IMMU2_LCCU_Mng.SendBuff[2] = ENJI_SOI_3;
	gs_IMMU2_LCCU_Mng.SendBuff[3] = u8CmdId;

	u16DataCount = ENJI_PROT_DATA;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = Rtn;

	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn_Num;

	for(j = 0; j < gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn_Num; j++)
	{
		for(u8Idx = 0; u8Idx < 12; u8Idx++)
		{
			gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn[j][u8Idx];
		}
	}

	//补上长度等信息
	u16temp = u16DataCount - ENJI_PROT_DATA;
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_4] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[ENJI_PROT_POS_5] = (UINT8)(u16temp);

	//校验码
	u16temp = 0;	
	for(ii = ENJI_PROT_POS_3; ii < u16DataCount; ii++)
	{
		u16temp += gs_IMMU2_LCCU_Mng.SendBuff[ii];
	}
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp>>8);
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = (UINT8)(u16temp);

	//EOI
	gs_IMMU2_LCCU_Mng.SendBuff[u16DataCount++] = ENJI_EOI_1;

	//发送	
	u8SendResult = Immu2_Lccu_Comm_Send(gs_IMMU2_LCCU_Mng.SendBuff, u16DataCount);
	//OSTimeDly(35);

	return u8SendResult;
}
/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
void LccuRcvDataDeal(UINT8 BufNo)
{
	UINT8 CID2, i;

	CID2 = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_POS_3];



	gs_IMMU2_LCCU_Mng.u8MasterRcvdSlaveOverTime = 0;

	if(CID2 == 0x01)  //获取参数  获取所有IMSU-D配置参数（CFGG）
	{				
		gs_IMMU2_LCCU_Mng.u8NeedRptLccuParas = 1;
	}
	else if(CID2 == 0x02)  //设置单个参数  设置单个IMSU-D配置参数（CFGS）
	{
		gs_IMMU2_LCCU_Mng.u8AckCfgRtn = UnpackCfgLccuParas(BufNo);				

		gs_IMMU2_LCCU_Mng.u8NeedAckCfgResult = 1;
	}  
	else if(CID2 == 0x03)  //控制命令   控制IMSU-D执行特定操作(UCTL)
	{
		gs_IMMU2_LCCU_Mng.u8AckCtrlRtn = UnpackControllCmd(BufNo);				

		gs_IMMU2_LCCU_Mng.u8NeedAckCtrlResult = 1;
	}  
	else if(CID2 == 0x04)  //获取模块信息 获取IMSU-D模块信息(RTIF)
	{				
		gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo = 1;
	}
	else if(CID2 == 0x05)  //获取CAN原始报文数据 获取CAN原始报文数据(GCAN)
	{
		if(0xA0 == gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+0])
		{
			gs_IMMU2_LCCU_Mng.u8IsTrigger = FALSE;
		}
		else
		{
			gs_IMMU2_LCCU_Mng.u8IsTrigger = TRUE;
			gs_IMMU2_LCCU_Mng.u8CanMsgType = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+0];
		}
		gs_IMMU2_LCCU_Mng.u8CANRawSendCount = gs_IMMU2_LCCU_Mng.u8CANRawSendInterval;
		gs_IMMU2_LCCU_Mng.u16CANRawSendTimeOUTCount = gs_IMMU2_LCCU_Mng.u16CANRawSendTimeOUT;
	}
	else if(CID2 == 0xA0)  //上层状态信息    IMMU2上层状态信息（USYS）
	{
		UnpackIMMU2Stat(BufNo);
	}  
	//else if(CID2 == 0x11)  //对 IMSU-D对上电池控制报文的回复
	//{
	//	UnpackAckBattQ(BufNo);
	//}  

	else if(CID2 == 0x12)  //对 IMSU-D启动升级   告知IMSU-D升级包大小（USIZ）
	{
		gs_IMMU2_LCCU_Mng.u8AckUpgradeRtn = StartUpdate(BufNo);
		gs_IMMU2_LCCU_Mng.u8AckUpgradeCmd = 0x12;
		gs_IMMU2_LCCU_Mng.u8NeedUpgradResult = 1;
	}  
	else if(CID2 == 0x13)  //对 IMSU-D启动升级   发送升级包数据（UDTA）
	{
		gs_IMMU2_LCCU_Mng.u8AckUpgradeRtn = Update_RcvData(BufNo);
		gs_IMMU2_LCCU_Mng.u8AckUpgradeCmd = 0x13;
		gs_IMMU2_LCCU_Mng.u8NeedUpgradResult = 1;
	}  
	else if((CID2 == 0x23)   //查询PCS组运行信息命令和设置PCS组配置命令 PCSS
		|| (CID2 == 0x24))
	{
		//上一个命令还没完
		if(gs_IMMU2_LCCU_Mng.u8NeedAckPcsCmdResult == 0)
		{
			gs_IMMU2_LCCU_Mng.u8NeedAckPcsCmdResult = 1;
			gs_IMMU2_LCCU_Mng.u8AckPcsCmd = CID2;
			gs_IMMU2_LCCU_Mng.u8PcsTimeout = 1;
			gs_IMMU2_LCCU_Mng.u8AckPcsRtn = 0;
			gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn_Num = 0;
			for(i = 0; i < 12; i++)
			{
				gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[i] = gs_IMMU2_LCCU_Mng.RcvBuf[BufNo].RecvBuff[ENJI_PROT_DATA+i];
			}		
		}			
	}

}

/*==========================================================================*
* 函数名称 : 
* 函数功能 : 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
void SendData_Net(void)
{
	UINT8 i; //, u8Tdata[2];

	//1. 汇报参数给IMMU2
//#ifdef DEBUG_LCCU_IMMU2_COMM
//	if((gs_IMMU2_LCCU_Mng.u8TxIsBusy == FALSE)
//		|| (gEvChargeInfo.u8RcvYdn23Flag_UART1 == RcvYdn23Flag_NET))
//#else 	
//	if((gs_DisplayCtrl.u8TxIsBusy == FALSE)
//		|| (gEvChargeInfo.u8RcvYdn23Flag == RcvYdn23Flag_NET))
//#endif
	{			
		if(gs_IMMU2_LCCU_Mng.u8NeedRptLccuParas)
		{				
			LccuRptParas();  //回复参数（CFGG CMD=0x01）
			gs_IMMU2_LCCU_Mng.u8NeedRptLccuParas = 0;
		}
	}

	//20210324 升级中，不支持其它个命令
	/*if(gs_UpGradeCtrl.u8UpgradeStat == UpgradeStatDef_Start)
	{
		if(gs_IMMU2_LCCU_Mng.u8NeedUpgradResult)
		{				
			u8Rtn = LccuAckUpgradeResult(gs_IMMU2_LCCU_Mng.u8AckUpgradeCmd, gs_IMMU2_LCCU_Mng.u8AckUpgradeRtn);
			gs_IMMU2_LCCU_Mng.u8NeedUpgradResult = 0;
		}

		return;
	}*/
	
	//2. 发送命令执行结果给IMMU2
//#ifdef DEBUG_LCCU_IMMU2_COMM
//	if((gs_IMMU2_LCCU_Mng.u8TxIsBusy == FALSE)
//		|| (gEvChargeInfo.u8RcvYdn23Flag_UART1 == RcvYdn23Flag_NET))
//#else 	
//	if((gs_DisplayCtrl.u8TxIsBusy == FALSE)
//		|| (gEvChargeInfo.u8RcvYdn23Flag == RcvYdn23Flag_NET))
//#endif
	{
		if(gs_IMMU2_LCCU_Mng.u8NeedAckCfgResult)
		{				
			LccuAckCfgResult(0x02, gs_IMMU2_LCCU_Mng.u8AckCfgCmd, gs_IMMU2_LCCU_Mng.u8AckCfgRtn);      //设置单个参数恢复（CFGS CMD=0x02）
			gs_IMMU2_LCCU_Mng.u8NeedAckCfgResult = 0;
		}
		else if(gs_IMMU2_LCCU_Mng.u8NeedAckCtrlResult)//获取控制命令结果（UCTL CMD=0x03）
		{				
			LccuAckCfgResult(0x03, gs_IMMU2_LCCU_Mng.u8AckCtrlCmd, gs_IMMU2_LCCU_Mng.u8AckCtrlRtn);   
			gs_IMMU2_LCCU_Mng.u8NeedAckCtrlResult = 0;
		}
		else if(gs_IMMU2_LCCU_Mng.u8NeedUpgradResult)
		{				
			LccuAckUpgradeResult(gs_IMMU2_LCCU_Mng.u8AckUpgradeCmd, gs_IMMU2_LCCU_Mng.u8AckUpgradeRtn);
			gs_IMMU2_LCCU_Mng.u8NeedUpgradResult = 0;
		}
		else if(gs_IMMU2_LCCU_Mng.u8NeedAckPcsCmdResult == 2)
		{				
			LccuAckPcsCmdResult(gs_IMMU2_LCCU_Mng.u8AckPcsCmd, gs_IMMU2_LCCU_Mng.u8AckPcsRtn);
			gs_IMMU2_LCCU_Mng.u8NeedAckPcsCmdResult = 0;
		}
	}

	

	//3. 从机发送 状态信息给主机
//#ifdef DEBUG_LCCU_IMMU2_COMM
//	if((gs_IMMU2_LCCU_Mng.u8TxIsBusy == FALSE)
//		|| (gEvChargeInfo.u8RcvYdn23Flag_UART1 == RcvYdn23Flag_NET))
//#else 	
//	if((gs_DisplayCtrl.u8TxIsBusy == FALSE)
//		|| (gEvChargeInfo.u8RcvYdn23Flag == RcvYdn23Flag_NET))
//#endif
	{
		if(gs_IMMU2_LCCU_Mng.u16LccuSendStatDelay == 0)
		{
			LccuRptStat();//回复下层状态信息（LSYS CMD = 0XB0）

			if(gs_UpGradeCtrl.u8UpgradeStat != UpgradeStatDef_Start)
			{
				gs_IMMU2_LCCU_Mng.u16LccuSendStatDelay = SLAVE_SEND_STAT_PERIOD;
			}
			else
			{
				gs_IMMU2_LCCU_Mng.u16LccuSendStatDelay = 2500;			//5s;
			}
		}
	}

	//4. 从机发送 充电信息给主机
//#ifdef DEBUG_LCCU_IMMU2_COMM
//	if((gs_IMMU2_LCCU_Mng.u8TxIsBusy == FALSE)
//		|| (gEvChargeInfo.u8RcvYdn23Flag_UART1 == RcvYdn23Flag_NET))
//#else 	
//	if((gs_DisplayCtrl.u8TxIsBusy == FALSE)
//		|| (gEvChargeInfo.u8RcvYdn23Flag == RcvYdn23Flag_NET))
//#endif
	{
		if(gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_Delay == 0)
		{				
			i = gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No;			
			for(; i < _MAX_GRP_NUM; i++)
			{
				if(gEvChargeInfo.u8PowerModuleNumCfg[i] > 0)
				{
					gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No = i;
					break;
				}
			}	
			if(i >= _MAX_GRP_NUM)
			{
				for(i = 0; i < _MAX_GRP_NUM; i++)
				{
					if(gEvChargeInfo.u8PowerModuleNumCfg[i] > 0)
					{
						gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No = i;
						break;
					}
				}	
			}			

			if(gEvChargeInfo.u8PowerModuleNumCfg[gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No] > 0)
			{
				LccuRptRectInfo(gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No);//上传模块信息（RTIF CMD = 0X04）
			}
			gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_Delay = 5;

			gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No++;
			if(gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No >= _MAX_GRP_NUM)
			{
				gs_IMMU2_LCCU_Mng.u8NeedRptLccuRectInfo_No = 0;
			}
		}
	}


	if(gs_IMMU2_LCCU_Mng.u8NeedRptSubDevVersion_Delay == 0)
	{
		gs_IMMU2_LCCU_Mng.u8NeedRptSubDevVersion_Delay = 30;

		if(gs_IMMU2_LCCU_Mng.u8NeedRptSubDevVersion_No == 0)
		{
			if(gIMEU_Info[0].u8GotVersionFlag == 1)
			{
				LccuRptSubDevVersion(gs_IMMU2_LCCU_Mng.u8NeedRptSubDevVersion_No);
			}
		}
	}

	if (gs_IMMU2_LCCU_Mng.u8NeedRptABUInfo_Delay == 0)
	{
		gs_IMMU2_LCCU_Mng.u8NeedRptABUInfo_Delay = CFG_SENDABUINFO_TIMEOUT;

		LccuRptABUInfo();
	}

	if (gs_IMMU2_LCCU_Mng.u8CANRawSendCount == 0)
	{
		gs_IMMU2_LCCU_Mng.u8CANRawSendCount = gs_IMMU2_LCCU_Mng.u8CANRawSendInterval;

		LccuRptCANRawInfo();

		//发送后清除缓存
		for(i = 0; i < CAN_MSG_MAX_NUM; i++)
		{
			gs_IMMU2_LCCU_Mng.CANRawSendMsg.IsExist[i] = 0;
			gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Id = 0;
			gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Data[0] = 0;
			gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Data[1] = 0;
		}

		if(gs_IMMU2_LCCU_Mng.u16CANRawSendTimeOUTCount == 0)
		{
			gs_IMMU2_LCCU_Mng.u8IsTrigger = FALSE;
		}
	}
}


/*==========================================================================*
* 函数名称 : 
* 函数功能 : 恩吉主从机通信处理 
* 入口参数 : 
* 出口参数 : 
* 说明     :                  
* 作者     :          
* 日期     : 
* 修改记录 :
*==========================================================================*/
void Task_Immu2LccuCommDeal(void)
{
	UINT8 i;

	//接收处理
	for(i = 0; i < ENJI_M_S_COMM_RCV_BUF_NUM; i++)
	{		
		if(gs_IMMU2_LCCU_Mng.RcvBuf[i].RcvFlag == ENJI_RCV_FLAG_OK) 
		{
			LccuRcvDataDeal(i);	

			gs_IMMU2_LCCU_Mng.RcvBuf[i].RcvOverTimeCnt = 0;
			gs_IMMU2_LCCU_Mng.RcvBuf[i].u16RcvPos = 0;
			gs_IMMU2_LCCU_Mng.RcvBuf[i].RcvFlag = ENJI_RCV_FLAG_INIT;

		}
	}


	SendData_Net();

	//无充电任务时重启
	if((gs_UpGradeCtrl.u8UpgradeNeedReset == 1)
		&& (gs_UpGradeCtrl.u8UpgradeWriteFlag == 0))
	{
		gs_UpGradeCtrl.u8UpgradeNeedReset = 0;
		Trap();
	}
}
