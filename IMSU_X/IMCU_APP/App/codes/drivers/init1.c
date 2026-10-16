#include "main.h"
#include "EtherNet.h"
#include "HistoryEvent.h"


void InitHIHardware(void)
{
	//I2C
	dv_E2PROM.fReadE2PROM(0);
	Delay(5000);
	dv_E2PROM.fReadE2PROM(1);
	Delay(5000);
	dv_E2PROM.fReadE2PROM(2);
	Delay(5000);

	//rtc
	dv_RTC.fDvInitRTC();

	WTDOG_FEED();

	//if(dv_LCD.fLCDOnLine()!=mTRUE)
	//{
	//	dv_LCD.fSetnoLCD(TRUE);
	//}
	//dv_LCD.fInitLCD();
	//dv_LCD.fOpenGreenLED();
	//dv_LCD.fCloseRedLED();
	//dv_LCD.fCloseYellowLED();

	WTDOG_FEED();
	WTDOG_FEED();
	dv_Output.fCloseBeep();
	//dv_LCD.fControlLCD(IOC_SET_BL, LCD_ON | LCD_HALF_CONTRAST);/*锟斤拷锟斤拷锟斤拷  0x60*/
	dv_CAN1.fCAN1BusInit();
	WTDOG_FEED();
}


void  InitOBJ(void)
{
	init_gs_Measure();
	init_gs_E2PROM();
	InitgCANComm();
	init_gs_SetData();
	init_gFANMngInfo();
	//init_gs_Screen();

	Init_IMMU2_LCCU();

	Init_HistoryEvent();
}

static void UpdateSetData(void)
{
	UINT16  i;
	UCHAR * pTest, E2PROMPageRead[20], E2PROMPageWrite[20];
	
	pTest = &gSetDataInfo.SetData.NameData.SysType;	/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
	for(i = 0; i < mSETDATADIV16; i++)
	{
		dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, E2PROMCharData + i * 16, 16);
		if(CompareSetData(pTest, E2PROMPageRead, 16) == FALSE)
		{
			WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData + i * 16, 16);
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData1 + i * 16, 16);
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData2 + i * 16, 16);
		}
		pTest += 16;
		WTDOG_FEED();
	}
}



void	WriteE2PROMBuffer(UCHAR * pTest, UCHAR * pData, UCHAR len)
{
	UCHAR  i;

	for(i = 0;i < len;i++)
		*pData++ = *pTest++;		/*写锟斤拷锟斤拷锟斤拷*/
}


static void	ReadE2PROMBuffer(UCHAR * pTest, UCHAR * pData, UCHAR len)
{
	UCHAR  i;
	for(i = 0;i < len;i++)
		*pTest++ = *pData++;		/*锟斤拷锟斤拷锟斤拷锟斤拷*/
}

static void  initRAM(void)
{
}


unsigned char	CompareSetData(unsigned char *pRAM1, unsigned char *pRAM2, UINT16 Length)
{
	UINT16 ii;
	for(ii = 0;ii < Length;ii++)
	{
		if(*pRAM1++ != *pRAM2++)
			return	FALSE;							/*锟斤拷锟斤拷锟斤拷锟捷诧拷锟斤拷同*/
	}
	return	TRUE;									/*锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷同*/
}


void	CopySetData(unsigned char *pRAM1, unsigned char *pRAM2, UINT16 Length)
{
	UINT16 ii;
  for(ii = 0;ii < Length;ii++)					/*循锟斤拷锟斤拷锟斤拷*/
		*pRAM1++ = *pRAM2++;
}


static void ReadData(void)
{
	UINT16 ii;
	UCHAR  test1, test2, test3;
	UCHAR * pTest;
	UCHAR * t1;
	UCHAR * t2;

	//UCHAR EEPROM_RxBuffer1[8192+20];
	//UCHAR EEPROM_RxBuffer2[8192+20];

	t1 = &gNET_TCP_Info.u8ReadBuf[0].u8DataFlag;		//9k
	t2 = &gNET_TCP_Info.u8ReadBuf[6].u8DataFlag;		//9k
	
	UCHAR	E2PROMPageRead[E2PROM_BUFFER_SIZE];				/*E2PROM锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷*/
	UCHAR	E2PROMPageWrite[E2PROM_BUFFER_SIZE];			/*E2PROM锟斤拷锟斤拷写锟斤拷锟斤拷锟斤拷*/

	//read data from E2PROM
	pTest = &gSetDataInfo.SetData.NameData.SysType;			/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
	for(ii = 0;ii < mSETDATADIV16;ii++)
	{
		dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, E2PROMCharData + ii * 16, 16);/*锟斤拷E2PROM16锟街斤拷*/
		ReadE2PROMBuffer(pTest, E2PROMPageRead, 16);
		pTest += 16;
		WTDOG_FEED();								/*喂锟斤拷*/
	}

	/*锟斤拷锟斤拷1*/
	pTest = t1;			/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
	for(ii = 0;ii < mSETDATADIV16;ii++)
	{
		dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, E2PROMCharData1 + ii * 16, 16);/*锟斤拷E2PROM16锟街斤拷*/
		ReadE2PROMBuffer(pTest, E2PROMPageRead, 16);
		pTest += 16;
		WTDOG_FEED();								/*喂锟斤拷*/
	}

	/*锟斤拷锟斤拷2*/
	pTest = t2;			/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
	for(ii = 0;ii < mSETDATADIV16;ii++)
	{	
		dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, E2PROMCharData2 + ii * 16, 16);/*锟斤拷E2PROM16锟街斤拷*/
		ReadE2PROMBuffer(pTest, E2PROMPageRead, 16);
		pTest += 16;
		WTDOG_FEED();								/*喂锟斤拷*/
	}

	/*锟饺斤拷锟斤拷锟斤拷锟斤拷锟斤拷*/
	/*锟酵憋拷锟斤拷1锟饺斤拷*/
	test1 = CompareSetData(&gSetDataInfo.SetData.NameData.SysType, t1, sizeof(gSetDataInfo.SetData));
	WTDOG_FEED();									/*喂锟斤拷*/

	/*锟酵憋拷锟斤拷2锟饺斤拷*/
	test2 = CompareSetData(&gSetDataInfo.SetData.NameData.SysType, t2, sizeof(gSetDataInfo.SetData));
	WTDOG_FEED();								/*喂锟斤拷*/

	/*锟斤拷锟斤拷1锟酵憋拷	锟斤拷2锟饺斤拷*/
	test3 = CompareSetData(t1,	t2, sizeof(gSetDataInfo.SetData));
	WTDOG_FEED();									/*喂锟斤拷*/

	/*锟酵憋拷锟斤拷1锟饺斤拷*/
	if((test1 == 0) && (test2 == 0) && (test3 == 0))/*1!=2!=3*/
	{
		//Trap2();								/*锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟捷撅拷锟斤拷锟斤拷同,锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷*/
		/*锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟捷撅拷锟斤拷锟斤拷同,取锟斤拷一锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷*/
		/*save data to E2PROM:锟斤拷锟斤拷锟斤拷锟捷憋拷锟斤拷2,3*/
		pTest = &gSetDataInfo.SetData.NameData.SysType;	/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
		for(ii = 0;ii < mSETDATADIV16;ii++)
		{
			WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
			pTest += 16;
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData1 + ii * 16, 16);	/*写E2PROM16锟街斤拷*/
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData2 + ii * 16, 16);	/*写E2PROM16锟街斤拷*/
			WTDOG_FEED();							/*喂锟斤拷*/
		}
	}
	else if((test1 == 1) && (test2 == 0) && (test3 == 0))	/*1==2!=3*/
	{
		/*save data to E2PROM:锟斤拷锟斤拷锟斤拷锟斤拷3*/
		pTest = &gSetDataInfo.SetData.NameData.SysType;		/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
		for(ii = 0;ii < mSETDATADIV16;ii++)
		{
			WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
			pTest += 16;
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData2 + ii * 16, 16);	/*写E2PROM16锟街斤拷*/
			WTDOG_FEED();							/*喂锟斤拷*/
		}
	}
	else if((test1 == 0) && (test2 == 1) && (test3 == 0))	/*1==3!=2*/
	{
		/*save data to E2PROM:锟斤拷锟斤拷锟斤拷锟斤拷2*/
		pTest = &gSetDataInfo.SetData.NameData.SysType;		/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
		for(ii = 0;ii < mSETDATADIV16;ii++)
		{
			WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
			pTest += 16;
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData1 + ii * 16, 16);	/*写E2PROM16锟街斤拷*/
			WTDOG_FEED();							/*喂锟斤拷*/
		}
	}
	else if((test1 == 0) && (test2 == 0) && (test3 == 1))	/*2==3!=1*/
	{
		/*锟斤拷锟斤拷锟节达拷锟斤拷锟斤拷锟斤拷锟斤拷*/
		CopySetData(&gSetDataInfo.SetData.NameData.SysType, t1, sizeof(gSetDataInfo.SetData));
		WTDOG_FEED();								/*喂锟斤拷*/
		/*save data to E2PROM:锟斤拷锟斤拷锟斤拷锟斤拷1*/
		pTest = &gSetDataInfo.SetData.NameData.SysType;		/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
		for(ii = 0;ii < mSETDATADIV16;ii++)
		{
			WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
			pTest += 16;
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData + ii * 16, 16);/*写E2PROM16锟街斤拷*/
			WTDOG_FEED();							/*喂锟斤拷*/
		}
	}

	WTDOG_FEED();									/*喂锟斤拷*/
}

void start(void)
{
    /*锟斤拷锟斤拷LCD*/
    //if(dv_LCD.fGetnoLCD())
    //{
    //    return;
    //}
    //dv_Output.fCloseBeep();
    //gScreenType.fClearScreen();
    WTDOG_FEED();
    //dv_LCD.fControlLCD(IOC_SET_LCD_CURSOR, 0x44);
    //dv_LCD.fWriteLCD("Start...", DIRECT_NORMAL, 0);
    //dv_LCD.fControlLCD(IOC_SET_LCD_CURSOR, 0x64);
    //dv_LCD.fWriteLCD("??...", DIRECT_NORMAL, 0);
    WTDOG_FEED();
    return;
}

static void ChargeArgsInit(UINT8 u8BackCoef);
static void ChargeArgsInit(UINT8 u8BackCoef)
{
	__XXX_UNION_VALUE unTempVal;
	unsigned int i;

	
	if(u8BackCoef)
	{
		unTempVal.ftemp = 1;
		ExSetSigInfo(E_TYPE_SYS, ID_TP1000_1_CoefA, unTempVal,NULL, BY_E2PROM);
		WTDOG_FEED();

		unTempVal.ftemp = 0;	
		ExSetSigInfo(E_TYPE_SYS, ID_TP1000_1_CoefB, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();

		unTempVal.ftemp = 1;
		ExSetSigInfo(E_TYPE_SYS, ID_TP1000_2_CoefA, unTempVal,NULL, BY_E2PROM);
		WTDOG_FEED();

		unTempVal.ftemp = 0;	
		ExSetSigInfo(E_TYPE_SYS, ID_TP1000_2_CoefB, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();

		unTempVal.ftemp = 1;
		ExSetSigInfo(E_TYPE_SYS, ID_fCC1_1Coef_A, unTempVal, NULL, BY_E2PROM);
		unTempVal.ftemp = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_fCC1_1Coef_B,unTempVal, NULL, BY_E2PROM);

		unTempVal.ftemp = 1;
		ExSetSigInfo(E_TYPE_SYS, ID_fCC1_2Coef_A, unTempVal, NULL, BY_E2PROM);
		unTempVal.ftemp = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_fCC1_2Coef_B, unTempVal, NULL, BY_E2PROM);

		unTempVal.ftemp = 1;
		ExSetSigInfo(E_TYPE_SYS, ID_fBattVolt_1CoefA, unTempVal,NULL, BY_E2PROM);

		unTempVal.ftemp = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_fBattVolt_1CoefB, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();

		unTempVal.ftemp = 1;
		ExSetSigInfo(E_TYPE_SYS, ID_fBattVolt_2CoefA, unTempVal, NULL, BY_E2PROM);

		unTempVal.ftemp = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_fBattVolt_2CoefB, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();
	}


	for(i = 0; i < 48; i++)
	{
		ExSetSigInfo(E_TYPE_SYS, ID_fTimeServicePrice1+i, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();
	}


	unTempVal.ftemp = 50;
	ExSetSigInfo(E_TYPE_SYS, ID_fMinOutVolt, unTempVal, NULL, BY_E2PROM);


	unTempVal.chtemp[0] = 100;
	ExSetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed, unTempVal, NULL, BY_E2PROM);

	WTDOG_FEED();
	unTempVal.chtemp[0] = '1';
	ExSetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1+0, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1+0, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1+0, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = '5';
	ExSetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1+1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1+1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1+1, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = '1';
	ExSetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1+2, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1+2, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1+2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = '7';
	ExSetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1+3, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1+3, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1+3, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = '1';
	ExSetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1+4, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1+4, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1+4, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = '9';
	ExSetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1+5, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1+5, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1+5, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	//锟斤拷缘锟斤拷
	//unTempVal.chtemp[0] = 0;	//锟斤拷锟斤拷锟斤拷锟斤拷
	//ExSetSigInfo(E_TYPE_IMIU1, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	//ExSetSigInfo(E_TYPE_IMIU2, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);

	//unTempVal.chtemp[0] = 0; 
	//ExSetSigInfo(E_TYPE_IMIU1, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	////unTempVal.chtemp[0] = 0; 
	//ExSetSigInfo(E_TYPE_IMIU2, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	//WTDOG_FEED();

	/*unTempVal.ftemp = 75;
	ExSetSigInfo(E_TYPE_IMIU1, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU2, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 0x02;
	ExSetSigInfo(E_TYPE_IMIU1, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU2, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);*/
	WTDOG_FEED();

	//锟斤拷锟斤拷锟斤拷锟�
	unTempVal.ftemp = 1.00;
	ExSetSigInfo(E_TYPE_WH1, ID_fACWhCoef, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_WH2, ID_fACWhCoef, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 1;
	ExSetSigInfo(E_TYPE_WH1, ID_u8WH_TYPE, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_WH2, ID_u8WH_TYPE, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 1;
	ExSetSigInfo(E_TYPE_WH1, ID_u8WH_ALM_Mask, unTempVal, NULL, BY_E2PROM);
	//unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_WH2, ID_u8WH_ALM_Mask, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	//
	unTempVal.ftemp = 480;
	ExSetSigInfo(E_TYPE_SYS, ID_LocalTimeZone, unTempVal, NULL, BY_E2PROM);

	WTDOG_FEED();
	OSTimeDly(30);

	unTempVal.ftemp = 760;
	ExSetSigInfo(E_TYPE_SYS, ID_fSysSetOverVolt, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	OSTimeDly(20);

	unTempVal.ftemp = 1;
	ExSetSigInfo(E_TYPE_SYS, ID_fRectLoadStartTime, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	OSTimeDly(10);

	unTempVal.ftemp = 500;
	ExSetSigInfo(E_TYPE_SYS, ID_fMaxOutCurr,unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = 750;
	ExSetSigInfo(E_TYPE_SYS, ID_fMaxOutVolt,unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = 200;
	ExSetSigInfo(E_TYPE_SYS, ID_fSetLimitMaxOutCurr_1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_fSetLimitMaxOutCurr_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.u32Value = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_U1_BYTES_NUM, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_U2_BYTES_NUM, unTempVal, NULL, BY_E2PROM);
	



	unTempVal.chtemp[0] = TRUE;
	ExSetSigInfo(E_TYPE_SYS, ID_u8PolarityAlmMask_1,unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8PolarityAlmMask_2,unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 0;										//0 锟斤拷锟斤拷锟斤拷锟�	1锟斤拷锟斤拷约锟斤拷锟斤拷锟�
	ExSetSigInfo(E_TYPE_SYS, ID_u8SetCalcMoneyType, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = TRUE;										//1使锟杰硷拷锟� CC1	
	ExSetSigInfo(E_TYPE_SYS, ID_u8CC1CheckEnable_1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8CC1CheckEnable_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = TRUE;										//锟皆硷拷使锟斤拷 CC1	
	ExSetSigInfo(E_TYPE_SYS, ID_u8SelfTestEnable_1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8SelfTestEnable_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = TRUE;										//压锟斤拷小锟斤拷10V使锟斤拷
	ExSetSigInfo(E_TYPE_SYS, ID_u8MCCBOnCheckEnable_1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8MCCBOnCheckEnable_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = TRUE;										//锟斤拷锟斤拷锟窖故癸拷锟�
	ExSetSigInfo(E_TYPE_SYS, ID_u8OutOverVoltEnable_1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8OutOverVoltEnable_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = FALSE;										//锟脚斤拷锟芥警锟斤拷锟斤拷
	ExSetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8DoorAlmMask_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = TRUE;										//0		PWM锟斤拷锟皆匡拷锟斤拷取锟斤拷
	ExSetSigInfo(E_TYPE_SYS, ID_u8PWMPolarityCover, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();



	//EPO 锟芥警锟斤拷平
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8EPOAlmVltLEVEL, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	OSTimeDly(30);

	unTempVal.chtemp[0] = 1;										//刷锟斤拷锟斤拷锟斤拷锟斤拷		1MT
	ExSetSigInfo(E_TYPE_SYS, ID_u8BrushFactorType, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	
	unTempVal.chtemp[0] = 55;			
	ExSetSigInfo(E_TYPE_SYS, ID_u8System_Ultra_Temp, unTempVal,NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 45;			
	ExSetSigInfo(E_TYPE_SYS, ID_u8System_Start_Temp, unTempVal,NULL, BY_E2PROM);

	
	unTempVal.ftemp = 5;
	ExSetSigInfo(E_TYPE_SYS, ID_fGunOverTemIntervalTime, unTempVal,NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 1;									//锟斤拷锟竭筹拷锟绞癸拷锟�
	ExSetSigInfo(E_TYPE_SYS, ID_u8NET_OffLine_Charge_Enable, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = 10;										//10锟斤拷锟斤拷
	ExSetSigInfo(E_TYPE_SYS, ID_fMaxOffLineTime, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.u32Value = 1;
	ExSetSigInfo(E_TYPE_SYS, ID_u32PowerOnTimes, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.u32Value = 0;				//SPD 
	ExSetSigInfo(E_TYPE_SYS, ID_u8SPDAlmMask, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();


	unTempVal.chtemp[0] = 192;
	ExSetSigInfo(E_TYPE_SYS, ID_u8IP_1, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 168;
	ExSetSigInfo(E_TYPE_SYS, ID_u8IP_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 1;
	ExSetSigInfo(E_TYPE_SYS, ID_u8IP_3, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 7;
	ExSetSigInfo(E_TYPE_SYS, ID_u8IP_4, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8IP_5, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8IP_6, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 192;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Gateway_1, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 168;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Gateway_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 1;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Gateway_3, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 1;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Gateway_4, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Gateway_5, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Gateway_6, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 255;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_1, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 255;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 255;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_3, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_4, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_5, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_6, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 120;
	ExSetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_1, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 24;
	ExSetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 255;
	ExSetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_3, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 86;
	ExSetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_4, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_5, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_6, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	
	//mac 锟斤拷锟杰革拷!!
	//....................

	unTempVal.u32Value = 4460;
	ExSetSigInfo(E_TYPE_SYS, ID_u32NET_PORT, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 0;									
	ExSetSigInfo(E_TYPE_SYS, ID_NET_DHCP, unTempVal, NULL, BY_E2PROM);
	
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_RemoteServerIsDomainName_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_DNSServerIP_NUM_1, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_DNSServerIP_NUM_2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_DNSServerIP_NUM_3, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_DNSServerIP_NUM_4, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_DNSServerIP_NUM_5, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_DNSServerIP_NUM_6, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = TRUE;	
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_MASK_NET_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = TRUE;	
	ExSetSigInfo(E_TYPE_SYS, ID_u8BRUSH_ALM_Mask, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();


	unTempVal.chtemp[0] = 0;	
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_FeeMode_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 0;	
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_ServiceFeeMode_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_fServicePrice_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = 1.0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_fFixPrice_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 0;	
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_AdjustMode_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = 200;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_Adjust_Volt1, unTempVal, NULL, BY_E2PROM);

	WTDOG_FEED();

	unTempVal.ftemp = 540;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_Adjust_Volt2, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = (float)mSETDATAVERHIGH + (float)mSETDATAVERLOW / 100;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_SetDataVersion_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = (float)mVERHIGH + (float)mVERLOW / 100 + 0.0001;;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_SoftwareVersion_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	for(i = 0; i < 48; i++)
	{
		unTempVal.ftemp = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_fTimePrice1+i, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();
		
	}
	

	for(i = 0; i < 60; i++)
	{
		unTempVal.chtemp[0] = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_u8StationCode_No1+i, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();
	}

	
	

	for(i = 0; i < 32; i++)
	{
		unTempVal.chtemp[0] = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_RemoteServerDomainName_NUM_1+i, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();
	}

	for(i = 0; i < 32; i++)
	{
		unTempVal.chtemp[0] = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_TelaidianStarionID_NUM_1+i, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();
	}
	for(i = 0; i < 32; i++)
	{
		unTempVal.chtemp[0] = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_TelaidianUserName_NUM_1+i, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();
	}
	for(i = 0; i < 32; i++)
	{
		unTempVal.chtemp[0] = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_TelaidianPassword_NUM_1+i, unTempVal, NULL, BY_E2PROM);
		WTDOG_FEED();
	}

	gs_EthAppCommMng.u16NetFrameHead = 0xF7AA;
	unTempVal.u32Value = 0xF7AA;
	ExSetSigInfo(E_TYPE_SYS, ID_NetFrameHead_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_EVChargeCodeOnUI_NUM, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8IsRingEnabled, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_bySelfAddress, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_nTotalRingStub, unTempVal, NULL, BY_E2PROM);


	unTempVal.ftemp = 5;
	ExSetSigInfo(E_TYPE_SYS, ID_fRingStartMergeCurr, unTempVal,NULL, BY_E2PROM);

	unTempVal.ftemp = 500;
	ExSetSigInfo(E_TYPE_SYS, ID_AllRingsPwrLmt, unTempVal,NULL, BY_E2PROM);
	unTempVal.ftemp = 1000;
	ExSetSigInfo(E_TYPE_SYS, ID_RingChgCurrIncSpeed, unTempVal,NULL, BY_E2PROM);
	

	unTempVal.chtemp[0] = 0;	//Disable
	ExSetSigInfo(E_TYPE_SYS, ID_u8GROUP_Detect_Enbl, unTempVal, NULL,  BY_E2PROM);
	
	
	unTempVal.chtemp[0] = (0x00<<4) + 0x00;					//L 00 2400锟斤拷锟斤拷锟斤拷		H00 645协锟斤拷
	ExSetSigInfo(E_TYPE_SYS, ID_WH_TYPE_BAURD, unTempVal,NULL, BY_E2PROM);

	

	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_bIsGunInsertMode, unTempVal, NULL, BY_E2PROM);

	unTempVal.u32Value = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_nGunInsertMode_Start_Hour, unTempVal,NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_nGunInsertMode_Start_Min, unTempVal,NULL, BY_E2PROM);

	unTempVal.u32Value = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_nGunInsertMode_Stop_Hour, unTempVal,NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_nGunInsertMode_Stop_Min, unTempVal,NULL, BY_E2PROM);

	unTempVal.ftemp = 30;
	ExSetSigInfo(E_TYPE_SYS, ID_fContactorShortThreshold, unTempVal,NULL, BY_E2PROM);


	unTempVal.ftemp = 860;
	ExSetSigInfo(E_TYPE_SYS, ID_fDCDCInputAlowMaxVolt, unTempVal,NULL, BY_E2PROM);

	unTempVal.ftemp = 0.5;
	ExSetSigInfo(E_TYPE_SYS, ID_fDischargeC10FromEV, unTempVal,NULL, BY_E2PROM);
	unTempVal.ftemp = 300;
	ExSetSigInfo(E_TYPE_SYS, ID_fDischargeLmtVoltFromEV, unTempVal,NULL, BY_E2PROM);
	unTempVal.ftemp = 0.9;
	ExSetSigInfo(E_TYPE_SYS, ID_fDischargeVoltCoeffStopVsRatedFromEV, unTempVal,NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 10;
	ExSetSigInfo(E_TYPE_SYS, ID_u8DischargeLmtSocFromEV, unTempVal,NULL, BY_E2PROM);


	for(i = 0; i < 12; i++)
	{
		unTempVal.chtemp[0] = 0;
		ExSetSigInfo(E_TYPE_SYS, ID_u8GRP1_MODULE_NUM+i, unTempVal,NULL, BY_E2PROM);
	}

	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8ACInputContactorOpenDelay, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8LeakageSensorEn, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8ModuleType, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 0;
	ExSetSigInfo(E_TYPE_SYS, ID_u8TaskIsAcDispatchMode, unTempVal, NULL, BY_E2PROM);

	//锟斤拷缘锟斤拷
	unTempVal.chtemp[0] = 0;	//锟斤拷锟斤拷锟斤拷锟斤拷
	ExSetSigInfo(E_TYPE_IMIU1, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU2, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU3, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU4, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU5, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU6, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU7, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU8, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU9, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU10, ID_u8InsltCheckByVolt, unTempVal, NULL, BY_E2PROM);


	unTempVal.chtemp[0] = 0; 
	ExSetSigInfo(E_TYPE_IMIU1, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU2, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU3, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU4, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	unTempVal.chtemp[0] = 1; 
	ExSetSigInfo(E_TYPE_IMIU5, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU6, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU7, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU8, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU9, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU10, ID_u8JYMAlmStatMask, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();

	unTempVal.ftemp = 75;
	ExSetSigInfo(E_TYPE_IMIU1, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU2, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU3, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU4, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU5, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU6, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU7, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU8, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU9, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU10, ID_fSetJYMAlm_Resist, unTempVal, NULL, BY_E2PROM);

	//锟皆讹拷模式
	unTempVal.chtemp[0] = 0x01;
	ExSetSigInfo(E_TYPE_IMIU1, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	ExSetSigInfo(E_TYPE_IMIU2, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();
	ExSetSigInfo(E_TYPE_IMIU3, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU4, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU5, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU6, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU7, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU8, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU9, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_IMIU10, ID_u8SetJYMCtrlMode, unTempVal, NULL, BY_E2PROM);
	WTDOG_FEED();


	//锟斤拷缘锟斤拷
	unTempVal.chtemp[0] = 1;	//锟斤拷锟斤拷锟斤拷锟斤拷
	ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp1_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp2_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp3_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp4_En, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 0;	//锟斤拷锟斤拷锟斤拷锟斤拷
	ExSetSigInfo(E_TYPE_SYS, ID_u8ABU1_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8ABU2_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8ABU3_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8ABU4_En, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 1;	
	ExSetSigInfo(E_TYPE_SYS, ID_u8IMEU1_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8IMEU2_En, unTempVal, NULL, BY_E2PROM);


	unTempVal.chtemp[0] = 0;	
	ExSetSigInfo(E_TYPE_SYS, ID_u8FanMode1, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8FanMode2, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8FanMode3, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 100;	
	ExSetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed_2, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8FanMaxSpeed_3, unTempVal, NULL, BY_E2PROM);


	unTempVal.ftemp = 45;										
	ExSetSigInfo(E_TYPE_SYS, ID_fFanStartTemp2, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_fFanStartTemp3, unTempVal, NULL, BY_E2PROM);

	unTempVal.ftemp = 55;										
	ExSetSigInfo(E_TYPE_SYS, ID_fFanFullTemp2, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_fFanFullTemp3, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 0;	//
	ExSetSigInfo(E_TYPE_SYS, ID_u8ACGridInputBreak_En, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 0;	//
	ExSetSigInfo(E_TYPE_SYS, ID_u8ACLoadOutputBreak_En, unTempVal, NULL, BY_E2PROM);

	unTempVal.chtemp[0] = 1;	//
	ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp1AlowChgOrDischgDI_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp2AlowChgOrDischgDI_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp3AlowChgOrDischgDI_En, unTempVal, NULL, BY_E2PROM);
	ExSetSigInfo(E_TYPE_SYS, ID_u8BattGrp4AlowChgOrDischgDI_En, unTempVal, NULL, BY_E2PROM);

	gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[1] = 0;
	gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[0] = 0;
	//by lxr01202
	//gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.Door_Open = 1;
	gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.Door_Open = 0;
	//gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.SPD = 1;
	gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.SPD = 0;
	gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.LeakageSensorEn = 0;            //漏锟斤拷锟斤拷  锟斤拷i
	gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.SmokeDIEn = 0;
	gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.FloodingDIEn = 0;
	gEvChargeInfo.u16AttributesBitsMap_1.BIT_FIELD.ToppleDIEn = 0;
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[1];
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 0, unTempVal, NULL, BY_E2PROM);	
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_1.BYTE_FIELD[0];	
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 1, unTempVal, NULL, BY_E2PROM);


	gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[1] = 0;
	gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[0] = 0;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt2Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt3Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU1Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.IMEU2Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1CanCommEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt2CanCommEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt3CanCommEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4CanCommEn = 1;
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[1];
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 2, unTempVal, NULL, BY_E2PROM);	
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_2.BYTE_FIELD[0];	
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 3, unTempVal, NULL, BY_E2PROM);


	gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[1] = 0;
	gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[0] = 0;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt1ChgForbitDiEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt2ChgForbitDiEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt3ChgForbitDiEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt4ChgForbitDiEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt1InputContactorEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt2InputContactorEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt3InputContactorEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt4InputContactorEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt1IsolationSwEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt2IsolationSwEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt3IsolationSwEn = 1;
	gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt4IsolationSwEn = 1;
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[1];
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 4, unTempVal, NULL, BY_E2PROM);	
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_3.BYTE_FIELD[0];	
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 5, unTempVal, NULL, BY_E2PROM);

	gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[1] = 0;
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[1];
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 6, unTempVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[0] = 0;
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_4.BYTE_FIELD[0];	
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 7, unTempVal, NULL, BY_E2PROM);

	gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[1] = 0;
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[1];
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 8, unTempVal, NULL, BY_E2PROM);
	gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[0] = 0;
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_5.BYTE_FIELD[0];	
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 9, unTempVal, NULL, BY_E2PROM);

	gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[1] = 0;
	gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[0] = 0;
	gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU1Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU2Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU3Exist = 1;
	gEvChargeInfo.u16AttributesBitsMap_6.BIT_FIELD.IMIU4Exist = 1;
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[1];
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 10, unTempVal, NULL, BY_E2PROM);
	unTempVal.chtemp[0] = gEvChargeInfo.u16AttributesBitsMap_6.BYTE_FIELD[0];	
	ExSetSigInfo(E_TYPE_SYS, ID_u8AttributeBitsFromIMM2_1 + 11, unTempVal, NULL, BY_E2PROM);
	
	WTDOG_FEED();

}






void RectifierUpgradeFlag_Init(void)
{
	dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
	dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
	gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
	gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
	gEvChargeInfo.u8_Clean_U1_Flash = TRUE;
	gEvChargeInfo.u8_Clean_U2_Flash = TRUE;
}



/*锟斤拷锟斤拷锟斤拷始锟斤拷*/
void  InitRunArgs(void)
{
	UINT8 u8Mac[8] = {0,0,0,0,0,0,0,0};
	//UINT8 u8Mac[8] = {0xAC,0x65,0xDD,0x30,0xAC,0x3C,0,0};

	__XXX_UNION_VALUE unTempVal;
	UINT8 i;
	//UCHAR	ctemp;
	float	ftemp, fversion;
	UCHAR	SetFlag1, SetFlag2;
	UCHAR*	pTest;
	UINT16	ii, u16temp;
	UCHAR	E2PROMPageRead[E2PROM_BUFFER_SIZE];			/*E2PROM锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷*/
	UCHAR	E2PROMPageWrite[E2PROM_BUFFER_SIZE];			/*E2PROM锟斤拷锟斤拷写锟斤拷锟斤拷锟斤拷*/
	UCHAR   ucdata[3];
	ucdata[0] = 0;
	ucdata[0] = ucdata[0];
	
	//硬锟斤拷锟芥本
	dv_E2PROM.fReadE2PROMPage(gEvChargeInfo.u8HardwareVer, E2PROM_HW_VER_HIGH, 2);
	
	SetFlag1 = dv_E2PROM.fReadE2PROM(E2PROMSetFlag1);							/*锟斤拷E2PROM锟斤拷锟矫憋拷志*/
	SetFlag2 = dv_E2PROM.fReadE2PROM(E2PROMSetFlag2);
	if((SetFlag1 != 0x5A) || (SetFlag2 != 0xa3))	
	{
		dv_E2PROM.fReadE2PROMPage(u8Mac, E2PROMMACADD, 6);
		for(i = 0; i < 6; i++)
		{
			unTempVal.chtemp[0] = u8Mac[i];
			ExSetSigInfo(E_TYPE_SYS, ID_u8Mac_1 + i, unTempVal,NULL, BY_E2PROM);
		}

		
		ChargeArgsInit(TRUE);
		WTDOG_FEED();

		//模锟斤拷锟斤拷锟斤拷锟斤拷志锟斤拷始锟斤拷
		RectifierUpgradeFlag_Init();

		//锟铰硷拷锟斤拷始锟斤拷
		gs_HistoryEventMng.ucClearEventFlag = TRUE;

		//save data to E2PROM
		pTest = &gSetDataInfo.SetData.NameData.SysType;					/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
		for(ii = 0;ii < mSETDATADIV16;ii++)								/*8192锟街斤拷:锟斤拷锟斤拷锟斤拷锟斤拷*/
		{
			WriteE2PROMBuffer(pTest, E2PROMPageWrite, 16);
			pTest += 16;
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData + ii * 16, 16);		/*写E2PROM16锟街斤拷*/
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData1 + ii * 16, 16);		/*写E2PROM16锟街斤拷*/
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMCharData2 + ii * 16, 16);		/*写E2PROM16锟街斤拷*/
			dv_E2PROM.fWriteE2PROMPage(E2PROMPageWrite, E2PROMDEFAULTCharData + ii * 16, 16);	/*写E2PROM16锟街斤拷*/
			WTDOG_FEED();
		}

		dv_E2PROM.fWriteE2PROM(E2PROMNumber, 0);
		//dv_E2PROM.fWriteE2PROM(E2PROMTimeFlag1, 0x55);
		//dv_E2PROM.fWriteE2PROM(E2PROMTimeFlag2, 0xaa);



		WTDOG_FEED();

		
		dv_E2PROM.fWriteE2PROM(E2PROMRResetReasonPos, 0);
		dv_E2PROM.fWriteE2PROM(E2PROMRResetReason1, 0);
		WTDOG_FEED();
		dv_E2PROM.fWriteE2PROM(E2PROMRResetReason2, 0);
		dv_E2PROM.fWriteE2PROM(E2PROMRResetReason3, 0);		
		WTDOG_FEED();

		dv_E2PROM.fWriteE2PROM(E2PROMDEFAULTFlag1, 0x55);
		dv_E2PROM.fWriteE2PROM(E2PROMDEFAULTFlag2, 0x66);

		dv_E2PROM.fWriteE2PROM(E2PROMSetFlag1, 0x5A);
		dv_E2PROM.fWriteE2PROM(E2PROMSetFlag2, 0xa3);
	}
	else										/*锟斤拷锟斤拷锟斤拷*/
	{
		SetFlag1 = dv_E2PROM.fReadE2PROM(E2PROMDEFAULTFlag1);
		SetFlag2 = dv_E2PROM.fReadE2PROM(E2PROMDEFAULTFlag2);
		if((SetFlag1 != 0x55) || (SetFlag2 != 0x66))				//锟街革拷默锟斤拷值
		{
			//read data from E2PROM
			pTest = &gSetDataInfo.SetData.NameData.SysType;			/*锟斤拷锟斤拷锟斤拷锟捷匡拷始锟斤拷址*/
			for(ii = 0;ii < mSETDATADIV16;ii++)
			{
				dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, E2PROMDEFAULTCharData + ii * 16, 16);/*锟斤拷E2PROM16锟街斤拷*/  	
				ReadE2PROMBuffer(pTest, E2PROMPageRead, 16);
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageRead, E2PROMCharData + ii * 16, 16);
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageRead, E2PROMCharData1 + ii * 16, 16);
				dv_E2PROM.fWriteE2PROMPage(E2PROMPageRead, E2PROMCharData2 + ii * 16, 16);
				pTest += 16;
				WTDOG_FEED();								
			}
			dv_E2PROM.fWriteE2PROM(E2PROMDEFAULTFlag1, 0x55);
			dv_E2PROM.fWriteE2PROM(E2PROMDEFAULTFlag2, 0x66);
		}

	
		//read data from E2PROM
		ReadData();							/*锟斤拷E2PROM锟叫讹拷取锟斤拷锟斤拷*/
		WTDOG_FEED();


		GetFloatSetData(SETDATA_SetDataVersion_NUM, &ftemp);
		
		//锟斤拷锟矫版本锟斤拷锟斤拷
		//SP_V101_Update();
		if(ftemp < 1.00 - 0.002)
		{
			/*SP_SetData_V105();
			UpdateSetData();*/
		}

		

		fversion = (float)mSETDATAVERHIGH + (float)mSETDATAVERLOW / 100;
		SetFloatSetData(SETDATA_SetDataVersion_NUM, &fversion);
		

		GetFloatSetData(SETDATA_SoftwareVersion_NUM, &ftemp);
		if(fabs1(ftemp, (float)mVERHIGH + (float)mVERLOW / 100 + 0.0001))
		{
			ftemp = (float)mVERHIGH + (float)mVERLOW / 100 + 0.0001;
			SetFloatSetData(SETDATA_SoftwareVersion_NUM, &ftemp);
		}
		GetFloatSetData(SETDATA_SoftwareVersion_NUM, &ftemp);
	}

	WTDOG_FEED();

	gs_E2PROM.ResetReasonRecordPos = dv_E2PROM.fReadE2PROM(E2PROMRResetReasonPos);
	if(gs_E2PROM.ResetReasonRecordPos > 2)
	{
		gs_E2PROM.ResetReasonRecordPos = 0;
		dv_E2PROM.fWriteE2PROM(E2PROMRResetReasonPos, 0);
		dv_E2PROM.fWriteE2PROM(E2PROMRResetReason1, 0);
		WTDOG_FEED();
		dv_E2PROM.fWriteE2PROM(E2PROMRResetReason2, 0);
		dv_E2PROM.fWriteE2PROM(E2PROMRResetReason3, 0);
	}
	else
	{
		if(gs_E2PROM.ResetReasonRecordPos == 0)
		{
			ucdata[0] = 2;
			ucdata[1] = 1;
			ucdata[2] = 0;			
		}
		else if(gs_E2PROM.ResetReasonRecordPos == 1)
		{
			ucdata[0] = 0;
			ucdata[1] = 2;
			ucdata[2] = 1;			
		}
		else //if(gs_E2PROM.ResetReasonRecordPos == 2)
		{
			ucdata[0] = 1;
			ucdata[1] = 0;
			ucdata[2] = 2;			
		}
		for(i = 0; i < 3; i++)
		{
			u16temp = E2PROMRResetReason1 + ucdata[i]*5;
			gs_E2PROM.ResetReasonRecord[i] = dv_E2PROM.fReadE2PROM(u16temp);
			pTest = (UCHAR*)&gs_E2PROM.u32ResetReasonDogBit[i];
			dv_E2PROM.fReadE2PROMPage(E2PROMPageRead, u16temp+1, 4);
			ReadE2PROMBuffer(pTest, E2PROMPageRead, 4);
			WTDOG_FEED();
		}
	}

	WTDOG_FEED();
	
	//gs_CANComm.LastRectNum = dv_E2PROM.fReadE2PROM(E2PROMNumber);
	
	initRAM();

	WTDOG_FEED();

	/*锟街革拷锟斤拷锟矫碉拷LCD锟皆比讹拷*/
	//GetCharSetData(SETDATA_BackLight_NUM, &ctemp);
	//ctemp &= 0x3f;
	//ctemp |= LCD_ON;
	//dv_LCD.fControlLCD(IOC_SET_BL, LCD_ON | ctemp);

	//gRTCData.fSetLocalTimeZone();

	//gRTCData.fGetLocalTimeZone();
	gRTCData.fDataInitRTC();

	WTDOG_FEED();

	gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = dv_E2PROM.fReadE2PROM(E2PROM_U1_UPDATA_FLAG);

	gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = dv_E2PROM.fReadE2PROM(E2PROM_U2_UPDATA_FLAG);

	WTDOG_FEED();

	if(RECT_IS_UPDATE_FLAG == gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0])
	{
		dv_E2PROM.fReadE2PROMPage((UINT8 *)gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade, E2PROM_Rect_Upgade_BinFileName_U1, 8);
	}
	else if(RECT_IS_FORCE_UPDATE_FLAG == gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0])  //强锟斤拷锟斤拷锟斤拷锟斤拷支锟街碉拷锟斤拷
	{
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
	}

	WTDOG_FEED();
	if(RECT_IS_UPDATE_FLAG == gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1])
	{
		dv_E2PROM.fReadE2PROMPage((UINT8 *)gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade, E2PROM_Rect_Upgade_BinFileName_U2, 8);
	}
	else if(RECT_IS_FORCE_UPDATE_FLAG == gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1])  //强锟斤拷锟斤拷锟斤拷锟斤拷支锟街碉拷锟斤拷
	{
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
	}

	

	WTDOG_FEED();


	//20210322 锟斤拷取BOOT锟芥本锟斤拷锟津单癸拷锟斤拷锟叫讹拷(值锟斤拷锟斤拷小锟斤拷0x80)
	ucdata[0] = dv_E2PROM.fReadE2PROM(E2PROM_BOOT_VER_VALID_FLAG);
	if(ucdata[0] == BOOT_VER_VALID_FLAG_Value)
	{
		gEvChargeInfo.u8BootVersion = dv_E2PROM.fReadE2PROM(E2PROM_BOOT_VERSION);

		if((gEvChargeInfo.u8BootVersion == 0) || (gEvChargeInfo.u8BootVersion == 0xFF))
		{
			gEvChargeInfo.u8BootVersion = 0;
		}
	}
	else
	{
		gEvChargeInfo.u8BootVersion = 0;
	}

	WTDOG_FEED();
}
