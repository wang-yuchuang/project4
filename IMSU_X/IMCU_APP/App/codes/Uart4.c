#include "main.h"
#include "Upgrade.h"



//stIMIU_Info.u8InsltCheckByVolt
//enum INSLT_CHK_MODE_Def
//{
//	INSLT_CHK_Mode_IMIU_Rpt_Alm = 0,   //由IMIU产生绝缘告警
//	INSLT_CHK_Mode_IMIU_Rpt_Volt,      //绝缘下降按电压来
//	INSLT_CHK_Mode_LeakageCurr,		//单枪霍尔漏电流模式
//};

//coTime  coSystemTicksClock; // system Ticks




NOINIT UINT8 gUart4RecvBuff[RS485_UART4_RX_MAX_NUM];
NOINIT UINT8 gUart4TXBuff[RS485_UART4_TX_MAX_NUM];
//static NOINIT UINT16 byReadValidBuff1[RS485_UART4_RX_MAX_NUM];


NOINIT stIMIU_Info gIMIU_Info_UART4[IMIU_MAX_NUM_UART4];
IMIU_COMM_MNG gImiuCommMng_uart4[IMIU_MAX_NUM_UART4] = {0};


extern void IMIUSample_1(IMIU_COMM_MNG *pImiu);



void InitIMIU_Uart4_E2prom(void)
{
	__XXX_UNION_VALUE unVal;
	ExGetSigInfo(E_TYPE_IMIU1, ID_fSetJYMAlm_Resist, &unVal, NULL, BY_E2PROM);
	//ExGetSigInfo(E_TYPE_IMIU1, ID_u8JYMAlmStatMask, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU1, ID_u8SetJYMCtrlMode, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU1, ID_u8InsltCheckByVolt, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_IMIU2, ID_fSetJYMAlm_Resist, &unVal, NULL, BY_E2PROM);
	//ExGetSigInfo(E_TYPE_IMIU2, ID_u8JYMAlmStatMask, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU2, ID_u8SetJYMCtrlMode, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU2, ID_u8InsltCheckByVolt, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_IMIU3, ID_fSetJYMAlm_Resist, &unVal, NULL, BY_E2PROM);
	//ExGetSigInfo(E_TYPE_IMIU3, ID_u8JYMAlmStatMask, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU3, ID_u8SetJYMCtrlMode, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU3, ID_u8InsltCheckByVolt, &unVal, NULL, BY_E2PROM);

	ExGetSigInfo(E_TYPE_IMIU4, ID_fSetJYMAlm_Resist, &unVal, NULL, BY_E2PROM);
	//ExGetSigInfo(E_TYPE_IMIU4, ID_u8JYMAlmStatMask, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU4, ID_u8SetJYMCtrlMode, &unVal, NULL, BY_E2PROM);
	ExGetSigInfo(E_TYPE_IMIU4, ID_u8InsltCheckByVolt, &unVal, NULL, BY_E2PROM);
}









void InitIMIUInfo_Uart4(void)
{
	unsigned char i;

	for(i = 0; i < IMIU_MAX_NUM_UART4; i++)
	{
		gIMIU_Info_UART4[i].u8JYMAlmStat[0] = 0;				//无告警
		gIMIU_Info_UART4[i].u8JYMAction[0] = 0;
		gIMIU_Info_UART4[i].u8RunEarth_DetectAlm[0] = 0;
		gIMIU_Info_UART4[i].fSetJYMAlm_Resist[0] = 74;
		gIMIU_Info_UART4[i].u8SetJYMCtrlStat[0] = 2;
		gIMIU_Info_UART4[i].u8SetGroundCtrlStat[0] = 1;        //0-禁止 1-使能
		gIMIU_Info_UART4[i].fJYMCurr[0] = 0;
		gIMIU_Info_UART4[i].fJYMVolt[0] = 650;
		gIMIU_Info_UART4[i].fJYMP2gVolt[0] = 0;
		gIMIU_Info_UART4[i].fJYMN2gVolt[0] = 0;
		gIMIU_Info_UART4[i].fJYMP2gResist[0] = 0;
		gIMIU_Info_UART4[i].fJYMN2gResist[0] = 0;
		gIMIU_Info_UART4[i].u8InsltCheckByVolt = FALSE;		//0绝缘下降电阻不按电压来   1按电压来
		gIMIU_Info_UART4[i].u8JYMAlmStatMask = FALSE;

		gIMIU_Info_UART4[i].u8CTRLInsulationCMD[0] = 0u;
		gIMIU_Info_UART4[i].u8EarthFalt[0] = 0;	
		gIMIU_Info_UART4[i].u8SelfChkResult[0] = 0;
		gIMIU_Info_UART4[i].u8CTRLInterDOCMD_NO = 0;
		gIMIU_Info_UART4[i].u8CTRLInterDOCMD[0] = 0;
		gIMIU_Info_UART4[i].u8CTRLInterDOCMD[1] = 0;

		gIMIU_Info_UART4[i].fRunJYMAlm_ResistPerV[0] = 100;
		gIMIU_Info_UART4[i].fSetJYMAlm_Resist_PerV[0] = 100;


		gImiuCommMng_uart4[i].u8UartNo = UART_NO_UART4;
		gImiuCommMng_uart4[i].FeedDogTaskID = TASK_ID_UART4;
		gImiuCommMng_uart4[i].pUartXTXBuff = gUart4TXBuff;
		gImiuCommMng_uart4[i].pUartXRecvBuff = gUart4RecvBuff;
		gImiuCommMng_uart4[i].u8DevAddr = YDN23_IMIU_ADDR+i;

		gImiuCommMng_uart4[i].u8InsltCommAlm = 0;
		gImiuCommMng_uart4[i].u8CommCount = 0;
		gImiuCommMng_uart4[i].u8CommStat = 0;

		gImiuCommMng_uart4[i].pImiuInfo = &gIMIU_Info_UART4[i];
	}
}

//static UCHAR s_u8CTRLInsulationCMD_Bak[3] = {0};
DCLeankageSensor_COMM_MNG gs_DCLeankageSensorMng;
void TASK_Uart4Proc(void* pdata)
{
	__XXX_UNION_VALUE unTempVal;

	static INT8U ch = 0;

	dv_Uart4.fInitUart(m9600, UARTX_PARITY_EVEN);				//InitUart4
	dv_Uart4.fEnableRx();										//使能收中断

	OSTimeDly(50);

	
	DCLeakageCurrentSensor_Init(UART_NO_UART4, &gs_DCLeankageSensorMng);

	//InitIMEU_Info_uart4();

	for(;;)
	{
		
		OSTimeDly(10);
		gSelfTest.fSetTaskID(TASK_ID_UART4);

		if(gImiuCommMng_uart4[ch].pImiuInfo->u8JYMAlmStatMask == TRUE)
		{
			gImiuCommMng_uart4[ch].pImiuInfo->u8JYMAlmStat[0] = FALSE;
			gImiuCommMng_uart4[ch].pImiuInfo->u8InsltCommAlm[0] = FALSE;		
		}
		else
		{
			//20201201 改为自动检测方式
			gImiuCommMng_uart4[ch].pImiuInfo->u8SetJYMCtrlStat[0] = 0x02;
			gImiuCommMng_uart4[ch].pImiuInfo->u8SetGroundCtrlStat[0] = 1;
			gImiuCommMng_uart4[ch].pImiuInfo->fSetJYMAlm_Resist_PerV[0] = 100;
			IMIUSample_1(&gImiuCommMng_uart4[ch]);
		}

		ch++;
		if(ch >= IMIU_MAX_NUM_UART4)
		{
			ch = 0;
		}


		//IMEU2采集
		/*ExGetSigInfo(E_TYPE_SYS, ID_u8IMEU1_En, &unTempVal, NULL, BY_E2PROM);

		if(unTempVal.chtemp[0] == 0)
		{
		gIMEU_Info[0].u8CommError = FALSE;
		}
		else
		{
		OSTimeDly(10);
		IMEU_Sampler(&gIMEU_Info[0]);
		}*/

		DCLeakageSensorTask(&gs_DCLeankageSensorMng);

		if(gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.LeakageSensorEn == 1)
		{
			DCLeakageSensorTask(&gs_DCLeankageSensorMng);
//by lxr01202
			gs_DCLeankageSensorMng.u16DCLeakageCurrentSensorOkStat = TRUE;
		}
		else
		{
			gs_DCLeankageSensorMng.u16DCLeakageCurrentSensorOkStat = FALSE;
		}
	}
}

