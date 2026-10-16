#include "main.h"
#include "can_ring.h"
#include "Uart6_new.h"
#include "HistoryEvent.h"
#include "ABU.h"


static float s_f32MpptSetOutVolt[BATT_GRP_MAX_NUM];
static float s_f32MpptSetOutCurr_Batt[BATT_GRP_MAX_NUM];
static float s_f32ACSetOutCurr_Batt[BATT_GRP_MAX_NUM];
unsigned short g_u16MpptAdjustPeriod[BATT_GRP_MAX_NUM] = {0};
static float s_f32DCDCSetOutCurr[BATT_GRP_MAX_NUM];

static float s_fChargeCurrReq[BATT_GRP_MAX_NUM];
static float s_fDisChargeCurrReq[BATT_GRP_MAX_NUM];
static unsigned char s_u8SysFaultRecFlag = 0;
static unsigned char s_u8BattXErrorRecFlag[BATT_GRP_MAX_NUM] = {0};

//#define	RECT_ADJUST_PERIOD	1000		//*2ms
#define	RECT_ADJUST_PERIOD	1000		//*2ms
#define	MPPT_CTRL_VOLT_DELT		10		//控制压差
void TaskConverterOutAction(void)
{
	UINT8 u8GrpNo, u8BattNo, i, u8ImiuStat, uctemp;
	UINT16 u16temp;
	float ftemp, ftemp2;
	static UINT8 s_u8EnterTestModeBak = 0;
	float fBMS_Limit_ChargeCurrent, fBMS_ChargeCurrReq, fBMS_OutCurr, fBMS_OutVolt, fBMS_ChargeVoltReq, fBMS_DischargeCurrReq,fWenya_CurrReq;
	float fACDCCtrlVolt, fMPPTCtrlVolt;


	if(gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)                   //检查系统是否处于测试模式（u8TEST_MODE_FLAG 或 u8_A_TestMode、u8_B_TestMode）。
	{
		s_u8EnterTestModeBak = 1;
		return;
	}
	
//by lxr01202    增加光伏开启判断    (纯离网模式(起机就不要接电网)下光伏需要及时开启)  
	if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE && gEvChargeInfo.u8EPOAlm == FALSE)       //自动模式下，并且急停不告警
		{
			__MPPT_1_INPUT_ON();
			__MPPT_2_INPUT_ON();
			SEGGER_RTT_printf(0,"PV_ON.\n\r");
		}

	if ((gEvChargeInfo.u8_A_TestMode == TRUE) || (gEvChargeInfo.u8_B_TestMode == TRUE))
	{
		s_u8EnterTestModeBak = 1;
		return;
	}


	//先关停处理
	if(s_u8EnterTestModeBak == 1)                     //条件: s_u8EnterTestModeBak == 1，表示刚从测试模式退出。
	{
		for(u8GrpNo = 0; u8GrpNo < _MAX_GRP_NUM; u8GrpNo++)
		{
			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_OFF;			
		}

		for(u8BattNo = 0; u8BattNo < BATT_GRP_MAX_NUM; u8BattNo++)
		{
			gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8TaskCtrlCmd = IMMU2ManualCtrlGrpCmd_Init;
			gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].MaxAllowChargeCurrent = 0;     
			s_fChargeCurrReq[u8BattNo] = 0;                                    
			s_fDisChargeCurrReq[u8BattNo] = 0;

			g_u16MpptAdjustPeriod[u8BattNo] = 0;
		}

		s_u8EnterTestModeBak = 0;

		RecordEvent(HISTORY_EVENT_EnterManualMode, 0, 0, 0);

		return;
	}

	/*if(gIMIU_Info3.u8JYMAlmStat[0] == TRUE)
	{
	__A_DC_MCCB_OFF();
	}*/


	//20201104 与IMMU2通信中断，关所有模块
	if(
		gEvChargeInfo.u8TelaidianHostCommAlm == TRUE           //CCU通信中断
		|| (gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)
		/*|| (gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)*/
		|| (gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE)
		/*|| (gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE)*/

		|| (gEvChargeInfo.u8EPOAlm == TRUE)
		|| (TRUE == gEvChargeInfo.u8SPDAlm)
		|| (gEvChargeInfo.u8DoorAlm == TRUE)
		|| (gs_IMMU2_LCCU_Mng.IMMU2Stat.FaultedStatus == 1)
		|| (gs_CANComm.u8AddressScanDelay > 0)	               ////20211019 lkf 新增排地址关模块的处理
		|| (gs_EsSampleInfo.Batt_CanComm_Break_Alm[0] == TRUE)
		|| (gs_EsSampleInfo.Batt_CanComm_Break_Alm[1] == TRUE)
		|| (gs_EsSampleInfo.Batt_CanComm_Break_Alm[2] == TRUE)
		|| (gs_EsSampleInfo.Batt_CanComm_Break_Alm[3] == TRUE)	            //by lxr01202   新增BMS通讯中断处理
		)
	{
		for(u8GrpNo = 0; u8GrpNo < _MAX_GRP_NUM; u8GrpNo++)
		{
			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_OFF;			
		}

		for(i = 0; i < BATT_GRP_MAX_NUM; i++)
		{
			g_u16MpptAdjustPeriod[i] = 0;
		}
		__A_PV1_INPUT_OFF();
		__A_PV2_INPUT_OFF();
		__A_PV3_INPUT_OFF();
		__A_PV4_INPUT_OFF();

		__MPPT_1_INPUT_OFF();
		__MPPT_2_INPUT_OFF();


		//20211108 记录一下事件
		if(s_u8SysFaultRecFlag == 0)
		{
			s_u8SysFaultRecFlag = 1;
			uctemp = 0;
			if(gEvChargeInfo.u8TelaidianHostCommAlm == TRUE)
			{
				uctemp = 1;
			}
			else if((gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
				|| (gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
				|| (gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
				|| (gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)
				/*|| (gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
				|| (gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
				|| (gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
				|| (gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)*/
				|| (gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE)
				|| (gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE)
				|| (gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE)
				|| (gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE))
			{
				uctemp = 2;

				u16temp = 0;
				if(gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
					u16temp |= 0x01;
				if(gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
					u16temp |= 0x02;
				if(gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
					u16temp |= 0x04;
				if(gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)
					u16temp |= 0x08;

				if(gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE)
					u16temp |= 0x010;
				if(gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE)
					u16temp |= 0x020;
				if(gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE)
					u16temp |= 0x040;
				if(gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE)
					u16temp |= 0x080;
				
				
			}
			else if(gEvChargeInfo.u8EPOAlm == TRUE)
			{
				uctemp = 3;
			}
			else if(TRUE == gEvChargeInfo.u8SPDAlm)
			{
				uctemp = 4;
			}
			else if(gEvChargeInfo.u8DoorAlm == TRUE)
			{
				uctemp = 5;
			}
			else if(gs_IMMU2_LCCU_Mng.IMMU2Stat.FaultedStatus == 1)
			{
				uctemp = 6;
			}
			else if(gs_CANComm.u8AddressScanDelay > 0)		//20211019 lkf 新增排地址关模块的处理
			{
				uctemp = 7;
			}
			else
			{
				uctemp = 250;
			}
			RecordEvent(HISTORY_EVENT_SystemFault, uctemp, 0, u16temp);
		}
		return;
	}
	if(s_u8SysFaultRecFlag == 1)
	{
		RecordEvent(HISTORY_EVENT_SystemFault, 0, 1, 0);
	}
	s_u8SysFaultRecFlag = 0;

	__A_PV1_INPUT_ON();
	__A_PV2_INPUT_ON();
	__A_PV3_INPUT_ON();
	__A_PV4_INPUT_ON();


	
/*****************************************************************************************************/
/*      解释   by lxr01202   
/*IMMU 状态检查: 根据电池组编号检查对应 IMIU 的状态。
/*获取 BMS 参数: 从全局结构读取电池的最大允许充电电流、请求电压等。
/*设置控制电压: 根据模块类型（PCS、BEG 等）调整 ACDC 和 MPPT 的控制电压。
/*异常处理: 如果电池状态异常（如错误、禁止充放电），关闭对应模块。
/*模式控制:
/*充电模式: 调整 MPPT 和 ACDC 输出电流以满足充电需求。
/*放电模式: 调整 ACDC 和 MPPT 以支持放电。
/*停止/初始化模式: 关闭或调整模块输出。
/*****************************************************************************************************/


	for(u8BattNo = 0; u8BattNo < BATT_GRP_MAX_NUM; u8BattNo++)
	{
		//IMIU影响对应支路
		u8ImiuStat = 0;
		switch(u8BattNo)
		{
		case 0:
			if((gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)||(gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE))
			{
				u8ImiuStat = 1;
			}
			break;
		case 1:
			if((gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)||(gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE))
			{
				u8ImiuStat = 1;
			}
			break;
		case 2:
			if((gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)||(gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE))
			{
				u8ImiuStat = 1;
			}
			break;
		case 3:
			if((gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)||(gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE))
			{
				u8ImiuStat = 1;
			}
			break;
		default:
			break;
		}


		fBMS_Limit_ChargeCurrent = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].MaxAllowChargeCurrent;       //最大允许充电电流
		fBMS_ChargeCurrReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].ChargeCurrReq;                    //请求电流

		fBMS_OutCurr = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].fOutCurr;                              //输出电流
		fBMS_OutVolt = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].fOutVolt;                              //输出电压

//by lxr01202
		if (ABU->ContravariantStat0.bit.IsGirdOnOff)    //离网模式
		{
			fBMS_DischargeCurrReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].EVDischargeCurrReq + 0.5;        //+0.5A 的目的是为了直接让光伏跑起来，因为离网MU2不下发指令了（发0电流指令）
			SEGGER_RTT_printf(0,"OFF_GRID.\n\r");
		}
		else                     //并网模式 按之前的变量跑
		{
		 	fBMS_DischargeCurrReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].EVDischargeCurrReq;
			SEGGER_RTT_printf(0,"ON_GRID.\n\r");
		}



		{
			fBMS_ChargeVoltReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].MaxAllowChargeVoltage;      //最大允许充电电压
		}


		 if((gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
		{
			fBMS_ChargeVoltReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].ChargeVoltReq;                //请求电压
			fWenya_CurrReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].ChargeVoltReq;        //新增稳压模式电压  lxr01202
			/*if(fBMS_ChargeVoltReq > gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].MaxAllowChargeVoltage)
			{
				fBMS_ChargeVoltReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].MaxAllowChargeVoltage;
			}*/
		}




		if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.u8Valid == 1)                 
		{
			if(fBMS_Limit_ChargeCurrent > gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_Limit_ChargeCurrent)   //如果MU2下发的最大允许电流 > 直接从CAN获取的BMS限制的充电电流     
			{
				fBMS_Limit_ChargeCurrent = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_Limit_ChargeCurrent;
			}	
			//注意：IMMU2-充电为正，BMS-CAN-充电为负
			fBMS_OutCurr = -gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_OutCurrent;
			fBMS_OutVolt = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_SumVolt;

			fBMS_ChargeVoltReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_Limit_ChargeVoltage;

			if(fBMS_DischargeCurrReq > gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_Limit_DischargeCurrent)
			{
				fBMS_DischargeCurrReq = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_Limit_DischargeCurrent;
			}
		}
		if(fBMS_ChargeCurrReq > fBMS_Limit_ChargeCurrent)                //如果请求电流 > 限制电流
		{
			fBMS_ChargeCurrReq = fBMS_Limit_ChargeCurrent;
		}

		

		if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE))
		{
			fACDCCtrlVolt = fBMS_ChargeVoltReq;
			fMPPTCtrlVolt = fBMS_ChargeVoltReq;
		}
		else if((gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) ||(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
		{
			if(fBMS_ChargeVoltReq > gEvChargeInfo.fMaxOutVolt)
			{
				fBMS_ChargeVoltReq = gEvChargeInfo.fMaxOutVolt;
			}

                    //让ACDC输出电压低于MPPT模块
                    fACDCCtrlVolt = (fBMS_OutVolt + MPPT_CTRL_VOLT_DELT);           
                    if(fACDCCtrlVolt > fBMS_ChargeVoltReq)                      
                    {
                            fACDCCtrlVolt = fBMS_ChargeVoltReq;
                    }
					
                    fMPPTCtrlVolt = fBMS_ChargeVoltReq;

		}
       
		//20211013 增加电池状态判断
		if((gs_IMMU2_LCCU_Mng.IMMU2Stat.BattStatus[u8BattNo] == Immu2RptBattStat_Error)
			|| (gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8IsAllowChgOrDischgFromDI == 0)//禁充禁放
			|| (u8ImiuStat == 1)
			//0:未就绪 1:就绪 2:充电截止 3:放电截止 4:充放电限制 5:充放电保护
			|| ((gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.u8Valid == 1)
			&&(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 0 
				|| gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 4
				||gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 5))
			)
		{
			//if(u8BattNo == 0)
			{
				s_f32MpptSetOutCurr_Batt[u8BattNo] = 0.1;	                  //设置MPPT给电池的输出电流为0.1
				gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_OFF;           //关闭MPPT模块
				gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, 50, s_f32MpptSetOutCurr_Batt[u8BattNo]);
				
		    }
			s_f32ACSetOutCurr_Batt[u8BattNo] = 0.1;
			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_OFF;
			gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_FORWORD; 
			
			if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE))
				gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fBMS_ChargeVoltReq, s_f32ACSetOutCurr_Batt[u8BattNo]);

			else if((gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, 50, s_f32ACSetOutCurr_Batt[u8BattNo]);


			gs_CANComm.u8GRP_RectOnOff[DCDC_GRP_NO_MIN+u8BattNo] = RECT_OFF;
			s_f32DCDCSetOutCurr[u8BattNo] = 0.1;		
			gs_CANComm.u8GRP_WorkModeSet[DCDC_GRP_NO_MIN+u8BattNo] = GRP_WORKMODE_SET_FORWORD; 		
			gs_CANComm.fSetRectCtlVoltCurr(DCDC_GRP_NO_MIN+u8BattNo, 50, s_f32DCDCSetOutCurr[u8BattNo]);

			//20211108 记录一下事件
			if(s_u8BattXErrorRecFlag[u8BattNo] == 0)
			{
				s_u8BattXErrorRecFlag[u8BattNo] = 1;
				uctemp = 0;
				if(gs_IMMU2_LCCU_Mng.IMMU2Stat.BattStatus[u8BattNo] == Immu2RptBattStat_Error)
				{
					uctemp = 1;
				}
				else if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8IsAllowChgOrDischgFromDI == 0)
				{
					uctemp = 2;
				}
				else if(u8ImiuStat == 1)
				{
					uctemp = 3;
				}
				else if((gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.u8Valid == 1)
					&&(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 0||gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 4||gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 5))
				{
					uctemp = 4;
				}
				else
				{
					uctemp = 250;
				}
				RecordEvent(HISTORY_EVENT_BattError, uctemp, 0, u8BattNo);
			}
			continue;
		}
		if(s_u8BattXErrorRecFlag[u8BattNo] == 1)
		{
			RecordEvent(HISTORY_EVENT_BattError, 0, 1, u8BattNo);
		}
		s_u8BattXErrorRecFlag[u8BattNo] = 0;

	if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8TaskCtrlCmd == IMMU2ManualCtrlGrpCmd_Chging)                 //IMMU2控制充电
		{
			if(fBMS_Limit_ChargeCurrent < 3)            	     //如果BMS限制的充电电流小于3A
			{
				s_f32MpptSetOutCurr_Batt[u8BattNo] = 0.1;             //设置MPPT输出电流为0.1A

				gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_OFF;     //关闭MPPT
				gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, fMPPTCtrlVolt, s_f32MpptSetOutCurr_Batt[u8BattNo]);
				SEGGER_RTT_printf(0,"IMMU2ManualCtrlGrpCmd_Chging——RECTOFF.\n\r");

				s_f32ACSetOutCurr_Batt[u8BattNo] = 0.1;
                //by lxr01202
				gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_ON;          //by lxr01202 开启ACDC    电池充满下发整流,模块不允许关机（稳压用）
				gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_FORWORD;  
//by lxr01202  为了稳压模式，将充满电后的PCS电压调增为MU2下发的电压fWenya_CurrReq
				gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fWenya_CurrReq, s_f32ACSetOutCurr_Batt[u8BattNo]);
				//gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fBMS_ChargeVoltReq, s_f32ACSetOutCurr_Batt[u8BattNo]);
				SEGGER_RTT_printf(0,"222222888888.\n\r");

			} 

			else if((g_u16MpptAdjustPeriod[u8BattNo] == 0)
				|| ((s_fChargeCurrReq[u8BattNo] - fBMS_ChargeCurrReq) > 0.5))
			{
				s_fChargeCurrReq[u8BattNo] = fBMS_ChargeCurrReq;
				g_u16MpptAdjustPeriod[u8BattNo] = RECT_ADJUST_PERIOD;               //隔2ms调整一次MPPT

				s_f32MpptSetOutVolt[u8BattNo] = fMPPTCtrlVolt;


				ftemp = fBMS_OutCurr - fBMS_ChargeCurrReq;                  //MU2输出电流-MU2请求电流



				if(ftemp > 0)                                    //先由ACDC补足，再由MPPT补足
				{
					if(s_f32ACSetOutCurr_Batt[u8BattNo] > ftemp)
					{
						s_f32ACSetOutCurr_Batt[u8BattNo] -= ftemp;							
					}
					else
					{		
						s_f32MpptSetOutCurr_Batt[u8BattNo] -= (ftemp - s_f32ACSetOutCurr_Batt[u8BattNo]);      

						s_f32ACSetOutCurr_Batt[u8BattNo] = 0;
						if(s_f32MpptSetOutCurr_Batt[u8BattNo] < 0)
						{
							s_f32MpptSetOutCurr_Batt[u8BattNo] = 0;
						}
					}
				}


				else                                                                     //让MPPT抢占PCS的功率
				{
					ftemp = -ftemp;

					ftemp2 = MIN(fBMS_ChargeCurrReq, gs_CANComm.RectData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr) - 0.01;
					if(s_f32MpptSetOutCurr_Batt[u8BattNo] < ftemp2) 
					{
						s_f32MpptSetOutCurr_Batt[u8BattNo] += ftemp;
						if(s_f32MpptSetOutCurr_Batt[u8BattNo] > ftemp2)
						{

							s_f32MpptSetOutCurr_Batt[u8BattNo] =  MIN(fBMS_ChargeCurrReq, gs_CANComm.RectData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr);
						}
					}					
					else if(s_f32ACSetOutCurr_Batt[u8BattNo] < fBMS_ChargeCurrReq)
					{
						s_f32ACSetOutCurr_Batt[u8BattNo] += ftemp;
						if(s_f32ACSetOutCurr_Batt[u8BattNo] > fBMS_ChargeCurrReq)
						{
							s_f32ACSetOutCurr_Batt[u8BattNo] = fBMS_ChargeCurrReq;
						}						
					}
				}
//by lxr01202满功率充电  (BMS限制199A，比如PCS70A，则MPPT输出129A) 				                                                                    
				// {
				// 	float fBMS_MaxCurrent = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_Limit_ChargeCurrent;
    
				// 	// PCS电流 = MU2下发请求值（需限制不超过BMS最大电流）
				// 	s_f32ACSetOutCurr_Batt[u8BattNo] = MIN(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].ChargeCurrReq, fBMS_MaxCurrent);
					
				// 	// MPPT电流 = BMS最大电流 - PCS已分配电流（确保总电流 ≤ 199A）
				// 	s_f32MpptSetOutCurr_Batt[u8BattNo] = fBMS_MaxCurrent - s_f32ACSetOutCurr_Batt[u8BattNo];
					
				// 	// 限制MPPT电流不超过其能力
				// 	s_f32MpptSetOutCurr_Batt[u8BattNo] = MIN(s_f32MpptSetOutCurr_Batt[u8BattNo], gs_CANComm.RectData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr);
				// }

				//20221103 lkf 增加模块可用功率上限处理
				if((gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].u8PowerofThisVoltMaxIsValidFlag == 2)
					&& ((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)))
				{
					if(s_f32ACSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fAvailableCurr)
					{
						s_f32ACSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fAvailableCurr;
					}
				}
				else if ((gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				{
					if(s_f32ACSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fMax_OutCurr)
					{
						s_f32ACSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fMax_OutCurr;
                        SEGGER_RTT_printf(0, "gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fMax_OutCurr = %lf\r\n",gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fMax_OutCurr);
					}
				}

				gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_ON;
				gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, s_f32MpptSetOutVolt[u8BattNo], s_f32MpptSetOutCurr_Batt[u8BattNo]);

				gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_ON;
				gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_FORWORD;  
                                //A2419 塘头_X 没有光伏，在不满充情况下，SOC大于截止SOC，则会导致继续给电池小电流充电,而正常应该停止充电或者放电！！！
				//gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fACDCCtrlVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);
                                //by lxr01202  修改   只有无光伏时才可这样修改！！！！！！！！！！！！
                                gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fWenya_CurrReq, s_f32ACSetOutCurr_Batt[u8BattNo]);


			} // if(g_u16MpptAdjustPeriod == 0)

			else
			{
				//为了快速下调
				if(fBMS_ChargeCurrReq < 3)
				{
					/*s_f32MpptSetOutVolt[u8BattNo] = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].fOutVolt;
					if(s_f32MpptSetOutVolt[u8BattNo] < 50)
					{
					s_f32MpptSetOutVolt[u8BattNo] = 50;
					}				*/
					s_fChargeCurrReq[u8BattNo] = fBMS_ChargeCurrReq;



					s_f32MpptSetOutCurr_Batt[u8BattNo] = 0.1;

					gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_ON;
					gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, fMPPTCtrlVolt, s_f32MpptSetOutCurr_Batt[u8BattNo]);


					s_f32ACSetOutCurr_Batt[u8BattNo] = 0.1;

					gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_ON;
					gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_FORWORD;  

					gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fACDCCtrlVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);

				}
			}
		} // if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8TaskCtrlCmd == IMMU2ManualCtrlGrpCmd_Chging)

		else if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8TaskCtrlCmd == IMMU2ManualCtrlGrpCmd_Dischging)               //IMMU2控制放电
		{

//by lxr01202 新增纯离网条件下MPPT的输出电流调整
				// 估算负载需求电流，ACDC输出电流即为负载需求

					float fLoadDemandCurr = gs_CANComm.RectData[ACDC_GRP_NO_MIN+u8BattNo].fRealTime_Curr;

				if (fLoadDemandCurr < 0.1) 
				    fLoadDemandCurr = 0.1; // 确保负载电流有最小值

				// 检查是否为离网模式
				if (ABU->ContravariantStat0.bit.IsGirdOnOff) // 离网模式
				{
					SEGGER_RTT_printf(1, "OFF_GRID_DISCHARGING.\n\r");

					// 判断电池是否充满
					int isBatteryFull = 0;
					if (gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_ProState == 2 ||
						gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_SumVolt >= 
						gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_Limit_ChargeVoltage - 5.0)
					{
						isBatteryFull = 1;
					}

					if (isBatteryFull)
					{
						// 电池充满，MPPT输出调整为负载需求电流
						s_f32MpptSetOutCurr_Batt[u8BattNo] = fLoadDemandCurr;
						// 限制MPPT输出不超过其最大能力
						if (s_f32MpptSetOutCurr_Batt[u8BattNo] > gs_CANComm.RectData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr)
						{
							s_f32MpptSetOutCurr_Batt[u8BattNo] = gs_CANComm.RectData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr;
						}
						// 设置MPPT输出电压为电池当前电压+压差
						s_f32MpptSetOutVolt[u8BattNo] = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_OutVolt + MPPT_CTRL_VOLT_DELT;
						//s_f32MpptSetOutVolt[u8BattNo] = fACDCCtrlVolt;
						gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_ON;
						gs_CANComm.u8GRP_WorkModeSet[MPPT_GRP_NO_MIN+u8BattNo] = GRP_WORKMODE_SET_MPPT;
						gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, s_f32MpptSetOutVolt[u8BattNo], s_f32MpptSetOutCurr_Batt[u8BattNo]);

						// 电池已满，ACDC待机，不从电池放电
						s_f32ACSetOutCurr_Batt[u8BattNo] = 0.0; // 最小待机电流
						gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_ON;
						gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_REVERSE;
						gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fACDCCtrlVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);

						SEGGER_RTT_printf(1, "Battery Full: MPPT supplies load only, I_MPPT = %f\n\r", s_f32MpptSetOutCurr_Batt[u8BattNo]);
					}
					else // 电池未充满
					{
						// MPPT输出最大能力
						s_f32MpptSetOutCurr_Batt[u8BattNo] = gs_CANComm.RectData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr;
						s_f32MpptSetOutVolt[u8BattNo] = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_OutVolt + MPPT_CTRL_VOLT_DELT;
						if (s_f32MpptSetOutVolt[u8BattNo] > gEvChargeInfo.fMaxOutVolt)
						{
							s_f32MpptSetOutVolt[u8BattNo] = gEvChargeInfo.fMaxOutVolt;
						}
						gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_ON;
						gs_CANComm.u8GRP_WorkModeSet[MPPT_GRP_NO_MIN+u8BattNo] = GRP_WORKMODE_SET_MPPT;
						gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, s_f32MpptSetOutVolt[u8BattNo], s_f32MpptSetOutCurr_Batt[u8BattNo]);

						// 获取MPPT实际输出电流
						float fMpptOutCurr = gs_CANComm.RectData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr;          

						// 根据MPPT输出与负载需求调整ACDC
						if (fMpptOutCurr < fLoadDemandCurr) // MPPT输出不足，电池放电补足
						{
							s_f32ACSetOutCurr_Batt[u8BattNo] = fLoadDemandCurr - fMpptOutCurr;
							// 限制ACDC输出不超过其最大能力
							if (s_f32ACSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fMax_OutCurr)
							{
								s_f32ACSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fMax_OutCurr;
							}
							gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_ON;
							gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_REVERSE;
							gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_OutVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);
						}
						else // MPPT输出足够或超额，ACDC待机
						{
							s_f32ACSetOutCurr_Batt[u8BattNo] = 0.0; // 最小待机电流
							gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_ON;
							gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_REVERSE;
							gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fACDCCtrlVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);
						}
						 }  
				}
// 并网模式保持原有逻辑
			else
			{
			//对放电也要响应快
			if(fBMS_DischargeCurrReq < 0.2)                     //低放电电流请求（<0.2A）
			{
				if (((gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.u8Valid == 1) && 
					(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].BattInfoFromCan.BMS_Limit_DischargeCurrent < 0.2)) ||
					(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8IsAllowChgOrDischgFromDI == 0))
				{
					s_fDisChargeCurrReq[u8BattNo] = fBMS_DischargeCurrReq;

					//直接关ACDC模块
					s_f32ACSetOutCurr_Batt[u8BattNo] = 0.1;

					gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_OFF;                      //关闭ACDC模块
					gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_REVERSE;       //设置ACDC为逆变模式
					SEGGER_RTT_printf(0,"full_RECTOFF_2.\n\r");
					if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE))
						gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fBMS_ChargeVoltReq, s_f32ACSetOutCurr_Batt[u8BattNo]);

					else if ((gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
						gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fBMS_OutVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);

					//MPPT
					if(g_u16MpptAdjustPeriod[u8BattNo] == 0)
					{
						g_u16MpptAdjustPeriod[u8BattNo] = RECT_ADJUST_PERIOD;                 

						s_f32MpptSetOutVolt[u8BattNo] = fBMS_OutVolt + MPPT_CTRL_VOLT_DELT;                 //MPPT电压设定为电池电压+增量（确保MPPT优先）
						if(s_f32MpptSetOutVolt[u8BattNo] > gEvChargeInfo.fMaxOutVolt)
						{
							s_f32MpptSetOutVolt[u8BattNo] = gEvChargeInfo.fMaxOutVolt;                 //设置最大的输出电压为MPPT的电压
						}
						//光伏的控制
						if(fBMS_OutCurr < 0) //电池在放电         动态调整MPPT输出电流
						{
							ftemp = -fBMS_OutCurr;       
							s_f32MpptSetOutCurr_Batt[u8BattNo] += ftemp;
							if(s_f32MpptSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr)
							{
								s_f32MpptSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr;
							}
							SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-111111111111.\n\r");
						}
						else  //电池在充电
						{
							ftemp = fBMS_OutCurr - fBMS_Limit_ChargeCurrent;

							if(ftemp > 0)
							{
								if(s_f32MpptSetOutCurr_Batt[u8BattNo] > ftemp)
								{
									s_f32MpptSetOutCurr_Batt[u8BattNo] -= ftemp;
									SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-2222222222.\n\r");
								}
								else
								{
									s_f32MpptSetOutCurr_Batt[u8BattNo] = 0;
									SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-3333333333.\n\r");
								}

							}
							else
							{
								s_f32MpptSetOutCurr_Batt[u8BattNo] -= ftemp;
								if(s_f32MpptSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr)
								{
									s_f32MpptSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr;
								}
								SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-44444444444.\n\r");
							}
						}

						gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_ON;
						gs_CANComm.u8GRP_WorkModeSet[MPPT_GRP_NO_MIN+u8BattNo] = GRP_WORKMODE_SET_MPPT;  

						gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, s_f32MpptSetOutVolt[u8BattNo], s_f32MpptSetOutCurr_Batt[u8BattNo]);
					}
				}
				else
				{
					gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_ON;
					gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_REVERSE;  
					if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
						gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].ChargeVoltReq, fBMS_DischargeCurrReq);
						SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-5555555.\n\r");
				}
			}
			else if((g_u16MpptAdjustPeriod[u8BattNo] == 0)
				|| ((s_fDisChargeCurrReq[u8BattNo] - fBMS_DischargeCurrReq) > 0.5))            
			{
				g_u16MpptAdjustPeriod[u8BattNo] = RECT_ADJUST_PERIOD;
				s_fDisChargeCurrReq[u8BattNo] = fBMS_DischargeCurrReq;

				s_f32MpptSetOutVolt[u8BattNo] = fBMS_OutVolt + MPPT_CTRL_VOLT_DELT;
				if(s_f32MpptSetOutVolt[u8BattNo] > gEvChargeInfo.fMaxOutVolt)
				{
					s_f32MpptSetOutVolt[u8BattNo] = gEvChargeInfo.fMaxOutVolt;
					SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-666666666.\n\r");
				}
// 开启ACDC反向工作模式（并网放电）
				gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_ON;
				gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_REVERSE;  
// 设置ACDC输出电流为放电请求值
				s_f32ACSetOutCurr_Batt[u8BattNo] = ABS(fBMS_DischargeCurrReq);				

				//20221103 lkf 增加模块可用功率上限处理
				if((gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].u8PowerofThisVoltMaxIsValidFlag == 2)
					&& ((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)))
				{
					if(s_f32ACSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fAvailableCurr)
					{
						s_f32ACSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fAvailableCurr;
					}
				}
				else if((gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				{
					if(s_f32ACSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fMax_OutCurr)
					{
						s_f32ACSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[GRP_1_ACDC+u8BattNo].fMax_OutCurr;
					}
				}

				if(s_f32ACSetOutCurr_Batt[u8BattNo] < 0.1)
				{
					s_f32ACSetOutCurr_Batt[u8BattNo] = 0.1;
					SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-7777777777777.\n\r");
				}

				if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE))
					gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fBMS_ChargeVoltReq, s_f32ACSetOutCurr_Batt[u8BattNo]);
				else if((gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
					gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fBMS_OutVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);


				//光伏的控制
				if(fBMS_OutCurr < 0) //电池在放电       
				{
					ftemp = -fBMS_OutCurr;
					s_f32MpptSetOutCurr_Batt[u8BattNo] += ftemp;
					if(s_f32MpptSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr)
					{
						s_f32MpptSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr;
						SEGGER_RTT_printf(0,"IMMU2ManualCtrlGrpCmd_Dischging-88888888.\n\r");
					}
				}
				else             //电池在充电
				{
					ftemp = fBMS_OutCurr - fBMS_Limit_ChargeCurrent;

					if(ftemp > 0)
					{
						if(s_f32MpptSetOutCurr_Batt[u8BattNo] > ftemp)
						{
							s_f32MpptSetOutCurr_Batt[u8BattNo] -= ftemp;
							SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-99999999.\n\r");
						}
						else
						{
							s_f32MpptSetOutCurr_Batt[u8BattNo] = 0;
							SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-1001111111.\n\r");
						}
					}
					else
					{
						s_f32MpptSetOutCurr_Batt[u8BattNo] -= ftemp;
						if(s_f32MpptSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr)
						{
							s_f32MpptSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr;
							SEGGER_RTT_printf(1,"IMMU2ManualCtrlGrpCmd_Dischging-10022222222.\n\r");
						}
					}
				}

				gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_ON;
				gs_CANComm.u8GRP_WorkModeSet[MPPT_GRP_NO_MIN+u8BattNo] = GRP_WORKMODE_SET_MPPT;  

				gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, s_f32MpptSetOutVolt[u8BattNo], s_f32MpptSetOutCurr_Batt[u8BattNo]);
			} // if(g_u16MpptAdjustPeriod == 0)
			else
			{
				
			}
		} 
		}// if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8TaskCtrlCmd == IMMU2ManualCtrlGrpCmd_Dischging)

		else if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8TaskCtrlCmd == IMMU2ManualCtrlGrpCmd_Stop
			|| gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8TaskCtrlCmd == IMMU2ManualCtrlGrpCmd_Init)                      //停止
		{
			if(fBMS_Limit_ChargeCurrent < 0.5)
			{
				s_f32MpptSetOutCurr_Batt[u8BattNo] = 0.1;

				gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_OFF;
				gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, fMPPTCtrlVolt, s_f32MpptSetOutCurr_Batt[u8BattNo]);


				s_f32ACSetOutCurr_Batt[u8BattNo] = 0.1;

				gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_OFF;
				gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_FORWORD;  

				gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fACDCCtrlVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);
				SEGGER_RTT_printf(2,"IMMU2ManualCtrlGrpCmd_Stop-10033333.\n\r");
			}
			else if(g_u16MpptAdjustPeriod[u8BattNo] == 0)
			{
				g_u16MpptAdjustPeriod[u8BattNo] = RECT_ADJUST_PERIOD;

				gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_OFF;
				gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_FORWORD;  


				s_f32ACSetOutCurr_Batt[u8BattNo] = 0;
				SEGGER_RTT_printf(2,"IMMU2ManualCtrlGrpCmd_Stop-100444444.\n\r");
				if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) )
					gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fBMS_ChargeVoltReq, s_f32ACSetOutCurr_Batt[u8BattNo]);

				else if((gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
					gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fBMS_OutVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);
	

				s_f32MpptSetOutVolt[u8BattNo] = fBMS_OutVolt+MPPT_CTRL_VOLT_DELT;
				if(s_f32MpptSetOutVolt[u8BattNo] > gEvChargeInfo.fMaxOutVolt)
				{
					s_f32MpptSetOutVolt[u8BattNo] = gEvChargeInfo.fMaxOutVolt;
					SEGGER_RTT_printf(2,"IMMU2ManualCtrlGrpCmd_Stop-100555.\n\r");
				}

				ftemp = fBMS_OutCurr - fBMS_Limit_ChargeCurrent;

				if(ftemp > 0)
				{
					if(s_f32MpptSetOutCurr_Batt[u8BattNo] > ftemp)
					{
						s_f32MpptSetOutCurr_Batt[u8BattNo] -= ftemp;
						SEGGER_RTT_printf(2,"IMMU2ManualCtrlGrpCmd_Stop-100666.\n\r");
					}
					else
					{
						s_f32MpptSetOutCurr_Batt[u8BattNo] = 0;
						SEGGER_RTT_printf(2,"IMMU2ManualCtrlGrpCmd_Stop-100777.\n\r");
					}
				}
				else
				{
					s_f32MpptSetOutCurr_Batt[u8BattNo] -= ftemp;
					SEGGER_RTT_printf(2,"IMMU2ManualCtrlGrpCmd_Stop-1008888.\n\r");
					if(s_f32MpptSetOutCurr_Batt[u8BattNo] > gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr)
					{
						s_f32MpptSetOutCurr_Batt[u8BattNo] = gs_CANComm.GrpData[MPPT_GRP_NO_MIN+u8BattNo].fMax_OutCurr;
						SEGGER_RTT_printf(2,"IMMU2ManualCtrlGrpCmd_Stop-100999.\n\r");
					}
				}


				gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_ON;
				gs_CANComm.u8GRP_WorkModeSet[MPPT_GRP_NO_MIN+u8BattNo] = GRP_WORKMODE_SET_MPPT;  

				gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, s_f32MpptSetOutVolt[u8BattNo], s_f32MpptSetOutCurr_Batt[u8BattNo]);

			} // if(g_u16MpptAdjustPeriod == 0)
		} //  if(gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].u8TaskCtrlCmd == IMMU2ManualCtrlGrpCmd_Stop
		else
		{
			if(fBMS_Limit_ChargeCurrent < 0.5)
			{
				s_f32MpptSetOutCurr_Batt[u8BattNo] = 0.1;

				gs_CANComm.u8GRP_RectOnOff[MPPT_GRP_NO_MIN+u8BattNo] = RECT_OFF;
				gs_CANComm.fSetRectCtlVoltCurr(MPPT_GRP_NO_MIN+u8BattNo, fMPPTCtrlVolt, s_f32MpptSetOutCurr_Batt[u8BattNo]);



				s_f32ACSetOutCurr_Batt[u8BattNo] = 0.1;

				gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8BattNo] = RECT_OFF;
				gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8BattNo] = GRP_WORKMODE_SET_FORWORD;  

				gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8BattNo, fACDCCtrlVolt, s_f32ACSetOutCurr_Batt[u8BattNo]);
				SEGGER_RTT_printf(3,"IMMU2ManualCtrlGrpCmd_Stop-1001010100101.\n\r");
			}
		}


		//DCDC模块

		/*ftemp = gs_IMMU2_LCCU_Mng.ESBattMng[u8BattNo].fMaxAlowPwr_Disch_DCDC;

		if(ftemp > 0.4)
		{
		s_f32DCDCSetOutCurr[u8BattNo] = ftemp * 1000 / 1000; 

		gs_CANComm.u8GRP_RectOnOff[DCDC_GRP_NO_MIN+u8BattNo] = RECT_ON;			
		}
		else
		{
		gs_CANComm.u8GRP_RectOnOff[DCDC_GRP_NO_MIN+u8BattNo] = RECT_OFF;

		s_f32DCDCSetOutCurr[u8BattNo] = 0.1;
		}*/
		//gs_CANComm.u8GRP_WorkModeSet[DCDC_GRP_NO_MIN+u8BattNo] = GRP_WORKMODE_SET_FORWORD;  

		//gs_CANComm.fSetRectCtlVoltCurr(DCDC_GRP_NO_MIN+u8BattNo, 1000, s_f32DCDCSetOutCurr[u8BattNo]);

		if(gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[DCDC_GRP_NO_MIN+u8BattNo] == IMMU2ManualCtrlGrpCmd_Chging)
		{
			gs_CANComm.u8GRP_RectOnOff[DCDC_GRP_NO_MIN+u8BattNo] = RECT_ON;	
		}
		else
		{
			gs_CANComm.u8GRP_RectOnOff[DCDC_GRP_NO_MIN+u8BattNo] = RECT_OFF;	
		}
		gs_CANComm.u8GRP_WorkModeSet[DCDC_GRP_NO_MIN+u8BattNo] = GRP_WORKMODE_SET_FORWORD; 
		gs_CANComm.fSetRectCtlVoltCurr(DCDC_GRP_NO_MIN+u8BattNo, gs_IMMU2_LCCU_Mng.fTaskCtrlGrpVolt[DCDC_GRP_NO_MIN+u8BattNo], gs_IMMU2_LCCU_Mng.fTaskCtrlGrpCurr[GRP_9_DCDC+u8BattNo]);



	} // for(u8BattNo = 0; u8BattNo < 2; u8BattNo++)



	//for(u8GrpNo = 0; u8GrpNo < ImmuCtrlGrpNo_MAX; u8GrpNo++)
	//{		
	//	//
	//	if((gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[u8GrpNo] == IMMU2ManualCtrlGrpCmd_Chging)
	//		||(gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[u8GrpNo] == IMMU2ManualCtrlGrpCmd_Dischging))
	//	{
	//		if(gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd_Backup[u8GrpNo] != gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[u8GrpNo])
	//		{
	//			gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd_Backup[u8GrpNo] = gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[u8GrpNo];
	//			gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd_Delay[u8GrpNo] = mConverterOutCtrlReq_Delay;
	//			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_OFF;
	//		}
	//		else if(gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd_Delay[u8GrpNo] > 0) 
	//		{
	//			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_OFF;
	//		}
	//		else if((gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[u8GrpNo] == IMMU2ManualCtrlGrpCmd_Chging)
	//			&& ((gs_CANComm.GrpData[GRP_1_ACDC+u8GrpNo].u8ConvertStat_ForwardNum == 0)
	//				|| (gs_CANComm.GrpData[GRP_1_ACDC+u8GrpNo].u8ConvertStat_ReverseNum > 0)))
	//		{
	//			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_OFF;
	//		}
	//		else if((gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[u8GrpNo] == IMMU2ManualCtrlGrpCmd_Dischging)
	//			&& ((gs_CANComm.GrpData[GRP_1_ACDC+u8GrpNo].u8ConvertStat_ReverseNum == 0)
	//			|| (gs_CANComm.GrpData[GRP_1_ACDC+u8GrpNo].u8ConvertStat_ForwardNum > 0)))
	//		{
	//			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_OFF;
	//		}
	//		else if(gs_CANComm.GrpData[GRP_1_ACDC+u8GrpNo].u8IsNoNormalRect == TRUE)
	//		{
	//			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_OFF;
	//		}
	//		else
	//		{
	//			gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_ON;
	//		}

	//		
	//		if(gs_IMMU2_LCCU_Mng.u8TaskCtrlGrpCmd[u8GrpNo] == IMMU2ManualCtrlGrpCmd_Dischging)
	//		{
	//			gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8GrpNo] = GRP_WORKMODE_SET_REVERSE;
	//		}
	//		else
	//		{
	//			gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8GrpNo] = GRP_WORKMODE_SET_FORWORD;
	//		}
	//	}		
	//	else
	//	{
	//		gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+u8GrpNo] = RECT_OFF;

	//		gs_CANComm.u8GRP_WorkModeSet[GRP_1_ACDC+u8GrpNo] = GRP_WORKMODE_SET_FORWORD;  //默认改回正向?
	//	}       

	//	ftemp = gs_IMMU2_LCCU_Mng.fTaskCtrlGrpCurr[u8GrpNo];
	//	if(ftemp < 0.5)
	//	{
	//		ftemp = 0.1;
	//	}
	//	gs_CANComm.fSetRectCtlVoltCurr(GRP_1_ACDC+u8GrpNo, gs_IMMU2_LCCU_Mng.fTaskCtrlGrpVolt[u8GrpNo], ftemp);
	//
	//} // end: for(u8GrpNo = 0; u8GrpNo < ImmuCtrlGrpNo_MAX; u8GrpNo++)
}