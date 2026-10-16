#include "main.h"
#include "Uart6_new.h"
#include "can_ring.h"

//NOINIT stBMS_INFO	gEVBms_2_Info;
//NOINIT stBMSCommCtrlInfo gBms_2_CommInfo;
//NOINIT ChargerContext CAN2_ChargerCtx;

void CAN_2_SendCallBack(void)
{
	return;
}
	
void TASK_CAN2Proc(void* pdata)
{
	OSTimeDly(1000);
	gSelfTest.fSetTaskID(TASK_ID_CAN2);

	OSTimeDly(1000);
	gSelfTest.fSetTaskID(TASK_ID_CAN2);
	OSTimeDly(1000);
	gSelfTest.fSetTaskID(TASK_ID_CAN2);
	OSTimeDly(1000);

	//gEvChargeInfo.u8ManualMode = FALSE;
	gB_Result.u16Reason12 = STOP_UPPER_CTRL;

	for (;;)
	{	
		
		OSTimeDly(100);
		gSelfTest.fSetTaskID(TASK_ID_CAN2);
              

//test   MPPT ‰»ÎΩ”¥•∆˜øÿ÷∆    by lxr01202  
		//BSP_DO_Ctrl(NEWDO12, DO_ON);
		// BSP_DO_Ctrl(NEWDO12, DO_OFF);
		//BSP_DO_Ctrl(NEWDO14, DO_ON);
		// BSP_DO_Ctrl(NEWDO14, DO_OFF);
        //       BSP_SetPwm1Duty(100);
        //         BSP_SetPwm1Duty(80);
        //        BSP_SetPwm1Duty(50);
        //       BSP_SetPwm1Duty(20);
	//      BSP_SetPwm1Duty(0);
	}
}	
