#include "main.h"
//#include "TelaidianProto.h"

NOINIT ALM_DEAL gAlmDeal;

UINT8 SysAlmReferesh(void);



UINT8 SysAlmReferesh(void)
{
	void *PEInfo;
	UINT8 u8IsAlm = FALSE;
	UINT8 u8TelaidianFaltCode = 0;
	PEInfo = GetgEquipInfo(E_RECT_INFO);
	if ((((CANCommData_TypeDef*)PEInfo)->u8IsAllCommFail))				//ËùÓÐÄ£¿éÍ¨ÐÅÊ§°Ü
	{
		u8TelaidianFaltCode = 0x4C;  //³äµçÄ£¿éÍ¨ÐÅ¹ÊÕÏ
		u8IsAlm = TRUE;
	}

	if((gIMIU_Info_UART4[0].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART4[1].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART4[2].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART4[3].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[0].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8InsltCommAlm[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8InsltCommAlm[0] == TRUE)
		)
	{
		u8IsAlm = TRUE;
	}



	if((gIMIU_Info_UART4[0].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART4[1].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART4[2].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART4[3].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[0].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[1].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[2].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART5[3].u8JYMAlmStat[0] == TRUE)
		)
	{
		u8IsAlm = TRUE;
	}

	//EPO
	PEInfo = GetgEquipInfo(E_TYPE_SYS);
	if ((((EV_CHARGE_INFO*)PEInfo)->u8EPOAlm))
	{
		u8TelaidianFaltCode = 0x55;  //
		u8IsAlm = TRUE;
	}

	//SPD
	PEInfo = GetgEquipInfo(E_TYPE_SYS);
	if ((((EV_CHARGE_INFO*)PEInfo)->u8SPDAlm))
	{
		u8TelaidianFaltCode = 0x35;  //·ÀÀ×¹ÊÕÏ
		u8IsAlm = TRUE;
	}

//by lxr01202      ÆÁ±ÎÃÅ½û¸æ¾¯
/*
	PEInfo = GetgEquipInfo(E_TYPE_SYS);
	if ((((EV_CHARGE_INFO*)PEInfo)->u8DoorAlm))
	{
		u8IsAlm = TRUE;
	}
*/
	//ÍøÂç¸æ¾¯
	if(gEvChargeInfo.u8NET_Mask == FALSE)
	{
		//if(gEvChargeInfo.u8NetCommAlm == TRUE)
		if(gEvChargeInfo.u8TelaidianHostCommAlm == TRUE)
		{
			u8IsAlm = TRUE;
		}
	}	


	if (u8IsAlm == TRUE)
	{				
		return TRUE;
	}

	UNUSED(u8TelaidianFaltCode);

	return FALSE;



}

void TASK_AlmProc(void* pdata)
{
	UINT8 u8Idx = 0;

	for (u8Idx = 0; u8Idx < 20; u8Idx++)
	{
		OSTimeDly(50);
		gSelfTest.fSetTaskID(TASK_ID_ALARM);
		OSTimeDly(80);
		gSelfTest.fSetTaskID(TASK_ID_ALARM);
	}
	gEvChargeInfo.u8ACGun_LeakageAlm = FALSE;
	gEvChargeInfo.u8TotalAC_LeakageAlm = FALSE;

	for (;;)
	{
		
		OSTimeDly(50);
		gSelfTest.fSetTaskID(TASK_ID_ALARM);

		if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
		{
			OSTimeDly(50);
			continue;
		}

		if (gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)
		{
			continue;
		}

		if ((gEvChargeInfo.u8_A_TestMode == TRUE)
			|| (gEvChargeInfo.u8_B_TestMode == TRUE))
		{
			continue;
		}

		
		if (SysAlmReferesh())
		{
			gEvChargeInfo.u8SysAlm = TRUE;
			gSelfTest.fSetTaskID(TASK_ID_ALARM);
			//__A_RED_LED_ON();//xry 20230904 delete, powercube led control by immu2
			//continue;
		}
		else
		{
			gEvChargeInfo.u8SysAlm = FALSE;
		}

	
		
		gSelfTest.fSetTaskID(TASK_ID_ALARM);
		OSTimeDly(50);
		
	}
}


