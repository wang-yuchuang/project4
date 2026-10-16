#include "main.h"
#include "can_ring.h"
/************************************************************************************************************************************************
*******************************************************************************************************************************************************************/
 #define ABS(a)	(((a) < 0) ? (-(a)) : (a))
float fAC_PWM_VOLTAGE_QuickVal = 0;
float fgCC1QuickVal = 0;
float fgCC1QuickVal_2 = 0;
NOINIT __XXX_UNION_VALUE ungMsrVal;
float fgCC1QuickVal_AC = 0;
static void TP1000_Ad_Deal(void);
float f_TP100_2_MidArry(float fValue);
float f_TP100_1_MidArry(float fValue);

ES_Sample_Info gs_EsSampleInfo = {0};
#define mSec_X(x) (x * 1000)
EepromRecordMng  gs_EepromRecordMng = {0};
MyTimer_info_t GetAddrTimer;
//NOINIT float fTestArry_1[100];
//NOINIT float fTestArry_2[100];
/************************************************/
/*��������:��ȡ�������� 		*/
/************************************************/
static UCHAR get_measureanalogdata(UINT16 nID, void *pDataBuf)
{
	if(nID == MACRO_UNUSED2)
	{
		return  TRUE;
	}
	else if(nID > (MACRO_UNUSED2))
	{
		return  FALSE;
	}

	MutexLock(gMeasureInfo.hMutexReadWriteMeasureData);
	*((float *)pDataBuf) = gMeasureInfo.MsrAnalogData[nID].fValue;
	MutexUnlock(gMeasureInfo.hMutexReadWriteMeasureData);

	return  TRUE;
}

/************************************************/
/*��������:��ȡ�������� 	*/
/************************************************/
static UCHAR GetUserData(UINT16 nID,void *pDataBuf,UCHAR SignalMode)
{
	//float ftemp,fcurrcoeff,flimit,fBusVolt,ftemp1,ftemp2;
	//UCHAR ctemp,i;

	//GetCharSetData(SETDATA_UserCurrCfg_NUM+nID,&ctemp);
	//if(ctemp == mNONE)
	//{
	//	*((float *)pDataBuf) = 0;
	//	return FALSE;
	//}
	//
	//if((SignalMode ==USER_CURR)||(SignalMode ==USER_POWER))
	//{
	//	get_measureanalogdata(DCEMCURR1_NUM+nID,&ftemp);
	//	GetFloatSetData(SETDATA_DCEM3ShuntCurr_NUM+nID, &fcurrcoeff);
	//	if((fcurrcoeff* CURRENTCLEARPOINT)>CURRENTCLEARVALUE)
	//	{
	//		flimit = fcurrcoeff* CURRENTCLEARPOINT;  
	//	}
	//	else
	//	{
	//		flimit = CURRENTCLEARVALUE;   
	//	}
	//	if(ftemp < flimit)
	//	{
	//		ftemp = 0;
	//	}
	//	if(SignalMode == USER_CURR)
	//	{
	//		*((float *)pDataBuf) =  ftemp;
	//	}
	//	else
	//	{
	//		get_measureanalogdata(DCV_NUM,&fBusVolt);
	//		*((float *)pDataBuf) = ftemp*fBusVolt/1000;
	//	}
	//}
	//else if(SignalMode == USER_ENERGY)
	//{
	//	*((float *)pDataBuf) = ftemp;
	//}
	//else if(SignalMode == USER_ENERGYRATE)
	//{	
	//	ftemp1 =  0;
	//	ftemp2 =  0;
	//	for(i=0;i< 4;i++)
	//	{
	//		ftemp1 +=  ftemp;
	//		if(nID = i)
	//		{
	//			ftemp2 = ftemp;
	//		}
	//	}
	//	if(ftemp1 == 0)
	//	{
	//		*((float *)pDataBuf) = 0;
	//	}
	//	else if(ftemp2 >ftemp1)
	//	{
	//		*((float *)pDataBuf) = 100;
	//	}
	//	else
	//	{
	//		*((float *)pDataBuf) = ftemp2/ftemp1;
	//	}
	//}
	return TRUE;
}
/************************************************/
/*��������:д��������*/
/************************************************/
static UCHAR set_measureanalogdata(UINT16 nID, void *pDataBuf)
{
	if(nID > MACRO_UNUSED2 - 1)
	{
	return  FALSE;
	}
	MutexLock(gMeasureInfo.hMutexReadWriteMeasureData);
	gMeasureInfo.MsrAnalogData[nID].fValue = *((float *)pDataBuf);
	MutexUnlock(gMeasureInfo.hMutexReadWriteMeasureData);
	return  TRUE;
}

/************************************************/
/*��������:�Ƿ�һ�β������?*/
/************************************************/
static UCHAR is_measureoneloop(void)
{
	return  gMeasureInfo.ucIsMeasureOneLoop;
}

/************************************************/
/*��������:���һ�β�����ɱ�־*/
/************************************************/
static void clear_measureoneloop(void)
{
  gMeasureInfo.ucIsMeasureOneLoop = FALSE;
}

/************************************************/
/*��������:���ø澯��*/
/************************************************/
static void set_alarm_limit(void)
{
	//UCHAR  i, ctemp;
	//UINT16 up, dn;
	//float f1, f2;

	//GetCharSetData(SETDATA_BattType_NUM, &ctemp);//mBATTTYPE
	//ctemp *= 2;
	//GetFloatSetData(SETDATA_DCOverVolt_NUM + ctemp, &f1);
	//GetFloatSetData(SETDATA_DCOverVolt_NUM + ctemp + 1, &f2);
	//gMeasureInfo.MsrAnalogData[DCV_NUM].AlmUpLimit[0] = f1;
	//gMeasureInfo.MsrAnalogData[DCV_NUM].AlmUpLimit[1] = f2;

	//GetFloatSetData(SETDATA_DCLowVolt_NUM + ctemp, &f1);
	//GetFloatSetData(SETDATA_DCLowVolt_NUM + ctemp + 1, &f2);
	//gMeasureInfo.MsrAnalogData[DCV_NUM].AlmDnLimit[0] = f1;
	//gMeasureInfo.MsrAnalogData[DCV_NUM].AlmDnLimit[1] = f2;

	//for(i = BATT3CURR_NUM; i <= BATT2CURR_NUM; i++)
	//{
	//	GetFloatSetData(SETDATA_BattStdCap_NUM, &f1);
	//	GetFloatSetData(SETDATA_OverCurr_NUM, &f2);
	//	gMeasureInfo.MsrAnalogData[i].AlmUpLimit[0] = f1 * f2;
	//	GetFloatSetData(SETDATA_OverCurr_NUM + 1, &f2);
	//	gMeasureInfo.MsrAnalogData[i].AlmUpLimit[1] = f1 * f2;
	//	gMeasureInfo.MsrAnalogData[i].AlmDnLimit[0] = -2;
	//	gMeasureInfo.MsrAnalogData[i].AlmDnLimit[1] = -2;
	//	gMeasureInfo.MsrAnalogData[i].MaxValue = GetCurrCoeff(i - BATT3CURR_NUM) * 3.0;
	//}
	//GetCharSetData(SETDATA_Temp3Cfg_NUM, &ctemp);
	//if(ctemp == mBATTERYTEMP)
	//{
	//	GetFloatSetData(SETDATA_BattTempOver_NUM, &gMeasureInfo.MsrAnalogData[TEMP3_NUM].AlmUpLimit[0]);
	//	GetFloatSetData(SETDATA_BattTempOver_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP3_NUM].AlmUpLimit[1]);
	//	GetFloatSetData(SETDATA_BattTempLow_NUM, &gMeasureInfo.MsrAnalogData[TEMP3_NUM].AlmDnLimit[0]);
	//	GetFloatSetData(SETDATA_BattTempLow_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP3_NUM].AlmDnLimit[1]);
	//}
	//else 
	//{
	//	GetFloatSetData(SETDATA_TempOver_NUM, &gMeasureInfo.MsrAnalogData[TEMP3_NUM].AlmUpLimit[0]);
	//	GetFloatSetData(SETDATA_TempOver_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP3_NUM].AlmUpLimit[1]);
	//	GetFloatSetData(SETDATA_TempLow_NUM, &gMeasureInfo.MsrAnalogData[TEMP3_NUM].AlmDnLimit[0]);
	//	GetFloatSetData(SETDATA_TempLow_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP3_NUM].AlmDnLimit[1]);
	//}


	//GetCharSetData(SETDATA_Temp2Cfg_NUM, &ctemp);
	//if(ctemp == mBATTERYTEMP)
	//{
	//	GetFloatSetData(SETDATA_BattTempOver_NUM, &gMeasureInfo.MsrAnalogData[TEMP2_NUM].AlmUpLimit[0]);
	//	GetFloatSetData(SETDATA_BattTempOver_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP2_NUM].AlmUpLimit[1]);
	//	GetFloatSetData(SETDATA_BattTempLow_NUM, &gMeasureInfo.MsrAnalogData[TEMP2_NUM].AlmDnLimit[0]);
	//	GetFloatSetData(SETDATA_BattTempLow_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP2_NUM].AlmDnLimit[1]);
	//}
	//else 
	//{
	//	GetFloatSetData(SETDATA_TempOver_NUM, &gMeasureInfo.MsrAnalogData[TEMP2_NUM].AlmUpLimit[0]);
	//	GetFloatSetData(SETDATA_TempOver_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP2_NUM].AlmUpLimit[1]);
	//	GetFloatSetData(SETDATA_TempLow_NUM, &gMeasureInfo.MsrAnalogData[TEMP2_NUM].AlmDnLimit[0]);
	//	GetFloatSetData(SETDATA_TempLow_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP2_NUM].AlmDnLimit[1]);
	//}

	//GetCharSetData(SETDATA_Temp1Cfg_NUM, &ctemp);
	//if(ctemp == mAMBIENTTEMP)
	//{
	//	GetFloatSetData(SETDATA_TempOver_NUM, &gMeasureInfo.MsrAnalogData[TEMP1_NUM].AlmUpLimit[0]);
	//	GetFloatSetData(SETDATA_TempOver_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP1_NUM].AlmUpLimit[1]);
	//	GetFloatSetData(SETDATA_TempLow_NUM, &gMeasureInfo.MsrAnalogData[TEMP1_NUM].AlmDnLimit[0]);
	//	GetFloatSetData(SETDATA_TempLow_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP1_NUM].AlmDnLimit[1]);
	//}
	//else 
	//{
	//	GetFloatSetData(SETDATA_BattTempOver_NUM, &gMeasureInfo.MsrAnalogData[TEMP1_NUM].AlmUpLimit[0]);
	//	GetFloatSetData(SETDATA_BattTempOver_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP1_NUM].AlmUpLimit[1]);
	//	GetFloatSetData(SETDATA_BattTempLow_NUM, &gMeasureInfo.MsrAnalogData[TEMP1_NUM].AlmDnLimit[0]);
	//	GetFloatSetData(SETDATA_BattTempLow_NUM + 1, &gMeasureInfo.MsrAnalogData[TEMP1_NUM].AlmDnLimit[1]);
	//}



	//up = SETDATA_AC3POverVolt_NUM;
	//dn = SETDATA_AC3PLowVolt_NUM;

	//for(i = AC1VA_NUM; i <= AC1VC_NUM; i++)
	//{
	//	/*������ѹ�澯����ֵ*/
	//	GetFloatSetData(up, &gMeasureInfo.MsrAnalogData[i].AlmUpLimit[0]);
	//	GetFloatSetData(up + 1, &gMeasureInfo.MsrAnalogData[i].AlmUpLimit[1]);
	//	/*������ѹ�澯����ֵ*/
	//	GetFloatSetData(dn, &gMeasureInfo.MsrAnalogData[i].AlmDnLimit[0]);
	//	GetFloatSetData(dn + 1, &gMeasureInfo.MsrAnalogData[i].AlmDnLimit[1]);
	//}
	//
	//for(i = LOAD1FUSE_NUM; i <= LOAD7FUSE_NUM; i++)				/*LOAD��˿*/
	//{
	//	/*�澯����ֵ*/
	//	GetFloatSetData(SETDATA_LoadFuseAlarm_NUM, &gMeasureInfo.MsrAnalogData[i].AlmUpLimit[0]);
	//	GetFloatSetData(SETDATA_LoadFuseAlarm_NUM, &gMeasureInfo.MsrAnalogData[i].AlmUpLimit[1]);
	//	/*�澯����ֵ*/
	//	GetFloatSetData(SETDATA_LoadFuseAlarm_NUM, &gMeasureInfo.MsrAnalogData[i].AlmDnLimit[0]);
	//	GetFloatSetData(SETDATA_LoadFuseAlarm_NUM, &gMeasureInfo.MsrAnalogData[i].AlmDnLimit[1]);
	//}

	//for(i = BATT1FUSE_NUM; i <= BATT4FUSE_NUM; i++)				/*������?*/
	//{
	//	/*�澯����ֵ*/
	//	GetFloatSetData(SETDATA_BattFuseAlarm_NUM, &gMeasureInfo.MsrAnalogData[i].AlmUpLimit[0]);
	//	GetFloatSetData(SETDATA_BattFuseAlarm_NUM, &gMeasureInfo.MsrAnalogData[i].AlmUpLimit[1]);
	//	/*�澯����ֵ*/
	//	GetFloatSetData(SETDATA_BattFuseAlarm_NUM, &gMeasureInfo.MsrAnalogData[i].AlmDnLimit[0]);
	//	GetFloatSetData(SETDATA_BattFuseAlarm_NUM, &gMeasureInfo.MsrAnalogData[i].AlmDnLimit[1]);
	//}
	//
	//for(i = DCEMCURR1_NUM; i <= DCEMCURR4_NUM; i++)
	//{
	//	GetFloatSetData(SETDATA_DCEM3ShuntCurr_NUM + i-DCEMCURR1_NUM, &f1);
	//	gMeasureInfo.MsrAnalogData[i].MaxValue = f1 * 3.0;
	//}
}

/************************************************/
/*��������:���ò���״̬ */
/************************************************/
static void set_teststatus(UCHAR data)
{
	gMeasureInfo.isTest = data;
}


//��������:�Ƿ����״�?
static UCHAR is_test(void)
{
	return  gMeasureInfo.isTest;
}

//��������:��ʼ������ģ��������
static void init_measureanalogdata(void)
{
	//UCHAR  i;

	//for(i = 0; i < MACRO_UNUSED2; i++)		
	//{
	//	gMeasureInfo.MsrAnalogData[i].fValue = 0;		/*����ֵ����*/
	//	gMeasureInfo.MsrAnalogData[i].AlarmFlag = 0;	/*���澯*/
	//	gMeasureInfo.MsrAnalogData[i].AlarmTable = 0;	/*���澯*/
	//	gMeasureInfo.MsrAnalogData[i].Buffer = 0;		/*���˻���������*/
	//}
	//
	////��ʼ������ģ������������
	//gMeasureInfo.MsrAnalogData[DCV_NUM].MaxValue = 70;			/*ֱ����ѹ����?*/
	//gMeasureInfo.MsrAnalogData[DCV_NUM].ClearValue = 10;		/*ֱ����ѹ����ֵ*/

	//for(i = BATT3CURR_NUM; i <= BATT2CURR_NUM; i++)
	//{
	//	//gMeasureInfo.MsrAnalogData[i].MaxValue = GetCurrCoeff(i - LOADCURR_NUM) * 1.5;
	//	gMeasureInfo.MsrAnalogData[i].ClearValue = 0;	/*��������ֵ*/
	//}

	//gMeasureInfo.MsrAnalogData[TEMP1_NUM].MaxValue = 102;		/*�¶�����?*/
	//gMeasureInfo.MsrAnalogData[TEMP1_NUM].ClearValue = -42;		/*�¶�����ֵ*/
	//gMeasureInfo.MsrAnalogData[TEMP2_NUM].MaxValue = 102;		/*�¶�����?*/
	//gMeasureInfo.MsrAnalogData[TEMP2_NUM].ClearValue = -42;		/*�¶�����ֵ*/
	//gMeasureInfo.MsrAnalogData[TEMP3_NUM].MaxValue = 102;		/*�¶�����?*/
	//gMeasureInfo.MsrAnalogData[TEMP3_NUM].ClearValue = -42;		/*�¶�����ֵ*/

	//for(i = AC1VA_NUM; i <= AC1VC_NUM; i++)
	//{
	//	gMeasureInfo.MsrAnalogData[i].MaxValue = 866;		/*������ѹ����?*/
	//	gMeasureInfo.MsrAnalogData[i].ClearValue = 17;	/*������ѹ����ֵ*/
	//}

	//for(i = AC1IA_NUM; i <= AC1IC_NUM; i++)
	//{
	//	gMeasureInfo.MsrAnalogData[i].MaxValue = 500;		/*������������?*/
	//	gMeasureInfo.MsrAnalogData[i].ClearValue = 0;	/*������������ֵ*/
	//}

	//for(i = LOAD1FUSE_NUM; i <= LOAD7FUSE_NUM; i++)				/*������˿*/
	//{
	//	gMeasureInfo.MsrAnalogData[i].MaxValue = 70;	/*����?*/
	//	gMeasureInfo.MsrAnalogData[i].ClearValue = 0;		/*����ֵ*/
	//}

	//for(i = BATT1FUSE_NUM; i <= BATT4FUSE_NUM; i++)				/*������?*/
	//{
	//	gMeasureInfo.MsrAnalogData[i].MaxValue = 70;	/*����?*/
	//	gMeasureInfo.MsrAnalogData[i].ClearValue = 0;		/*����ֵ*/
	//}

	//for(i = BMV1_NUM; i <= BMV9_NUM; i++)
	//{
	//	gMeasureInfo.MsrAnalogData[i].MaxValue = 70;		/*����е��ѹ����?*/
	//	gMeasureInfo.MsrAnalogData[i].ClearValue = 0;	   /*����е������?*/
	//}
	//
	//gMeasureInfo.MsrAnalogData[LOADCURR_NUM].MaxValue = 100000;//GetCurrCoeff(0) * 3.0;
	//
	//for(i = DCEMCURR1_NUM; i <= DCEMCURR4_NUM; i++)
	//{
	//	gMeasureInfo.MsrAnalogData[i].ClearValue = 0;	   /*����ֵ*/
	//}

	//gMeasureInfo.fSetAlarmLimit();
}

/************************************************/
/*��������:��ʼ��ʱ,׼����һ����������*/
/************************************************/
static void	start_delay(void)
{	
	dv_Adc.fChanelSelect(*gMeasureInfo.pMeasure++);/*׼����һ����������*/
	gMeasureInfo.MeasureCount++;
	OSTimeDly(2);
}

/************************************************/
/*��������:��ʼ����:������������ʱ��ʼ*/
/************************************************/
static void Get_AD_FilerValue(void)
{
	dv_Adc.ucStartMeasureFlag = 0;
	gMeasureInfo.MeasureCount++;
	//OSTimeDly(43);
	//dv_Adc.ucStartMeasureFlag =0;
	dv_Adc.fGetAdcValue(&gMeasureInfo.u16MeasureValue[PA3_ADC_SW_GUN_1_NO_0],PA3_ADC_SW_GUN_1_NO_0);
	dv_Adc.ucGetDataNum[PA3_ADC_SW_GUN_1_NO_0] = 0;
	dv_Adc.fGetAdcValue(&gMeasureInfo.u16MeasureValue[PA4_ADC_SW_TEMP_NO_1],PA4_ADC_SW_TEMP_NO_1);
	dv_Adc.ucGetDataNum[PA4_ADC_SW_TEMP_NO_1] = 0;
	dv_Adc.fGetAdcValue(&gMeasureInfo.u16MeasureValue[PA5_ADC_SW_oldBUS_1_None_NO_7],PA5_ADC_SW_oldBUS_1_None_NO_7);
	dv_Adc.ucGetDataNum[PA5_ADC_SW_oldBUS_1_None_NO_7] = 0;
	dv_Adc.fGetAdcValue(&gMeasureInfo.u16MeasureValue[PA6_ADC_SW_GUN_2_oldBUS_2_NO_2], PA6_ADC_SW_GUN_2_oldBUS_2_NO_2);
	dv_Adc.ucGetDataNum[PA6_ADC_SW_GUN_2_oldBUS_2_NO_2] = 0;
	dv_Adc.fGetAdcValue(&gMeasureInfo.u16MeasureValue[PB0_ADC_SW_oldHOLL_TP1000_NO_3], PB0_ADC_SW_oldHOLL_TP1000_NO_3);
	dv_Adc.ucGetDataNum[PB0_ADC_SW_oldHOLL_TP1000_NO_3] = 0;
	dv_Adc.fGetAdcValue(&gMeasureInfo.u16MeasureValue[PC0_ADC_SW_oldCP_CC_OF_AC_NO_4], PC0_ADC_SW_oldCP_CC_OF_AC_NO_4);
	dv_Adc.ucGetDataNum[PC0_ADC_SW_oldCP_CC_OF_AC_NO_4] = 0;
	dv_Adc.fGetAdcValue(&gMeasureInfo.u16MeasureValue[PC3_ADC_SW_TP1000_NO_5], PC3_ADC_SW_TP1000_NO_5);
	dv_Adc.ucGetDataNum[PC3_ADC_SW_TP1000_NO_5] = 0;
	dv_Adc.fGetAdcValue(&gMeasureInfo.u16MeasureValue[PC2_ADC_SW_oldNone_PS_NO_8], PC2_ADC_SW_oldNone_PS_NO_8);
	dv_Adc.ucGetDataNum[PC2_ADC_SW_oldNone_PS_NO_8] = 0;
	dv_Adc.ucStartMeasureFlag = 1;

}

//static float GetMaxFilerAC();
//static float GetMaxFilerAC()
//{
//	UINT8 i = 0;
//	float fVal = 0;
//	for (i = 0; i < mFIELT_NUM; i++)
//	{
//		if (AC_DMAValue[i] > fVal)
//		{
//			fVal = AC_DMAValue[i];
//		}
//	}
//	return fVal;
//}
//

#define AC_MEASURE_NUM			10		//AC_MEASURE_NUM  ����С��10��
NOINIT float fGACArry[AC_MEASURE_NUM + 2];
UINT8 u8GACIdx = 0;

float f_AC_MidArry(float fValue);
float f_AC_MidArry(float fValue)
{
	float fTemp = 0;
	if (u8GACIdx < AC_MEASURE_NUM)
	{
		fGACArry[u8GACIdx] = fValue;
		u8GACIdx++;
	}
	else if (u8GACIdx >= AC_MEASURE_NUM)
	{
		u8GACIdx = 0;
	}

	int i, j;
	for (i=0; i<AC_MEASURE_NUM-1; i++) /* ��ѭ��Ϊ����������len��������len-1�� */
	{
		for (j=0; j<AC_MEASURE_NUM-1-i; j++) /* ��ѭ��Ϊÿ�˱ȽϵĴ�������i�˱Ƚ�len-i�� */
		{ 
			if (fGACArry[j] > fGACArry[j+1]) /* ����Ԫ�رȽϣ��������򽻻�������Ϊ������ң������?�� */
			{ 
				fTemp = fGACArry[j];
				fGACArry[j] = fGACArry[j+1];
				fGACArry[j+1] = fTemp;
			}
		}
	}
	
	j = 0;
	fTemp = 0;
	for (i = (AC_MEASURE_NUM - 4); i < (AC_MEASURE_NUM); i++)
	{
		fTemp += fGACArry[i];
		j++;
	}
	fTemp = fTemp/j;
    return fTemp;
}

#define TP1000_MEASURE_NUM			30		//AC_MEASURE_NUM  ����С��6��

NOINIT float fTP1000_Arry_1[TP1000_MEASURE_NUM + 2];
NOINIT float fTP1000_Arry_2[TP1000_MEASURE_NUM + 2];
UINT8 u8TP1000_1_Idx = 0;
UINT8 u8TP1000_2_Idx = 0;
float f_TP100_1_MidArry(float fValue)
{
	float fTemp = 0;
	if (u8TP1000_1_Idx < TP1000_MEASURE_NUM)
	{
		fTP1000_Arry_1[u8TP1000_1_Idx] = fValue;
		u8TP1000_1_Idx++;
	}
	else if (u8TP1000_1_Idx >= TP1000_MEASURE_NUM)
	{
		u8TP1000_1_Idx = 0;
	}

	int i, j;
	for (i=0; i<TP1000_MEASURE_NUM-1; i++) /* ��ѭ��Ϊ����������len��������len-1�� */
	{
		for (j=0; j<TP1000_MEASURE_NUM-1-i; j++) /* ��ѭ��Ϊÿ�˱ȽϵĴ�������i�˱Ƚ�len-i�� */
		{ 
			if (fTP1000_Arry_1[j] > fTP1000_Arry_1[j+1]) /* ����Ԫ�رȽϣ��������򽻻�������Ϊ������ң������?�� */
			{ 
				fTemp = fTP1000_Arry_1[j];
				fTP1000_Arry_1[j] = fTP1000_Arry_1[j+1];
				fTP1000_Arry_1[j+1] = fTemp;
			}
		}
	}

	j = 0;
	fTemp = 0;
	//for (i = (TP1000_MEASURE_NUM - 4); i < (TP1000_MEASURE_NUM); i++)
	//{
	//	fTemp += fTP1000_Arry_1[i];
	//	j++;
	//}
	//fTemp = fTemp/j;
	for (i = ((TP1000_MEASURE_NUM/2) - 2); i < (TP1000_MEASURE_NUM) && i < ((TP1000_MEASURE_NUM/2)+2); i++)
	{
		fTemp += fTP1000_Arry_1[i];
		j++;
	}
	fTemp = fTemp/j;
	//fTemp = fTP1000_Arry_1[TP1000_MEASURE_NUM/2];
	return fTemp;
}




float f_TP100_2_MidArry(float fValue)
{
	float fTemp = 0;
	if (u8TP1000_2_Idx < TP1000_MEASURE_NUM)
	{
		fTP1000_Arry_2[u8TP1000_2_Idx] = fValue;
		u8TP1000_2_Idx++;
	}
	else if (u8TP1000_2_Idx >= TP1000_MEASURE_NUM)
	{
		u8TP1000_2_Idx = 0;
	}

	int i, j;
	for (i=0; i<TP1000_MEASURE_NUM-1; i++) /* ��ѭ��Ϊ����������len��������len-1�� */
	{
		for (j=0; j<TP1000_MEASURE_NUM-1-i; j++) /* ��ѭ��Ϊÿ�˱ȽϵĴ�������i�˱Ƚ�len-i�� */
		{ 
			if (fTP1000_Arry_2[j] > fTP1000_Arry_2[j+1]) /* ����Ԫ�رȽϣ��������򽻻�������Ϊ������ң������?�� */
			{ 
				fTemp = fTP1000_Arry_2[j];
				fTP1000_Arry_2[j] = fTP1000_Arry_2[j+1];
				fTP1000_Arry_2[j+1] = fTemp;
			}
		}
	}

	//j = 0;
	//fTemp = 0;
	//for (i = (TP1000_MEASURE_NUM - 4); i < (TP1000_MEASURE_NUM); i++)
	//{
	//	fTemp += fTP1000_Arry_2[i];
	//	j++;
	//}
	//fTemp = fTemp/j;

	j = 0;
	fTemp = 0;
	for (i = ((TP1000_MEASURE_NUM/2)- 2); i < (TP1000_MEASURE_NUM) && ((TP1000_MEASURE_NUM/2)+ 2); i++)
	{
		fTemp += fTP1000_Arry_2[i];
		j++;
	}
	fTemp = fTemp/j;
	return fTemp;
}




#define BUS2_MEASURE_NUM			10		
UINT32 fBUS_2_Arry[AC_MEASURE_NUM + 2];
UINT8 u8G_BUS_2_Idx = 0;
UINT32 u32Mid_BUS_2Arry(UINT32 fValue);
UINT32 u32Mid_BUS_2Arry(UINT32 fValue)
{
	UINT32 fTemp = 0;
	if (u8G_BUS_2_Idx < BUS2_MEASURE_NUM)
	{
		fBUS_2_Arry[u8G_BUS_2_Idx] = fValue;
		u8G_BUS_2_Idx++;
	}
	else if (u8G_BUS_2_Idx >= BUS2_MEASURE_NUM)
	{
		u8G_BUS_2_Idx = 0;
	}

	int i, j;
	for (i=0; i<AC_MEASURE_NUM-1; i++) /* ��ѭ��Ϊ����������len��������len-1�� */
	{
		for (j=0; j<AC_MEASURE_NUM-1-i; j++) /* ��ѭ��Ϊÿ�˱ȽϵĴ�������i�˱Ƚ�len-i�� */
		{ 
			if (fBUS_2_Arry[j] > fBUS_2_Arry[j+1]) /* ����Ԫ�رȽϣ��������򽻻�������Ϊ������ң������?�� */
			{ 
				fTemp = fBUS_2_Arry[j];
				fBUS_2_Arry[j] = fBUS_2_Arry[j+1];
				fBUS_2_Arry[j+1] = fTemp;
			}
		}
	}

	j = 0;
	fTemp = 0;
	for (i = (AC_MEASURE_NUM - 4); i < (AC_MEASURE_NUM); i++)
	{
		fTemp += fBUS_2_Arry[i];
		j++;
	}
	fTemp = fTemp/j;
	return fTemp;
}



//void fACPWMVoltCalc(void)
//{
//	gADTestFVal1 = GetMaxFilerAC();							//fgCC1QuickVal_AC;	
//	gADCC2Val = (((2.5*(double)gADTestFVal1)/(float)4096) * 6.00) * (2.0);
//	gADCC2Val = f_AC_MidArry((float)gADCC2Val);
//	if (gADCC2Val >= 4)
//	{
//		gEvChargeInfo.fCC1_ACPWMVoltVal = gADCC2Val;		   //PWM VOLT
//		if(gADCC2Val > 8.8)
//		{
//			UNUSED(gEvChargeInfo.fCC1_ACPWMVoltVal);
//		}
//		
//		if (gADCC2Val < 9 && gADCC2Val >= 6.3)
//		{
//			UNUSED(gEvChargeInfo.fCC1_ACPWMVoltVal);
//		}
//		else if (gADCC2Val < 6.3)
//		{
//			UNUSED(gEvChargeInfo.fCC1_ACPWMVoltVal);
//		}
//
//		if (AC_CHARGE_IS_C_STATUS)
//		{
//			gEvChargeInfo.u32PWMLoadValue = dv_Capture_PWM.Rate[AC_PWM]; 
//		}
//	}
//	else
//	{
//		gADCC2Val = gADCC2Val;
//	}
//}

#define CC1_MEASURE_NUM			4		//AC_MEASURE_NUM  ����С��10��
NOINIT float fCC1GUN_1_Arry[CC1_MEASURE_NUM + 2];
UINT8 u8GCC1_GUN_1_Idx = 0;
NOINIT float fCC1GUN_2_Arry[CC1_MEASURE_NUM + 2];
UINT8 u8GCC1_GUN_2_Idx = 0;

float f_CC1_GUN_1_MidArry(float fValue);
float f_CC1_GUN_1_MidArry(float fValue)
{
	float fTemp = 0;
	if (u8GCC1_GUN_1_Idx < CC1_MEASURE_NUM)
	{
		fCC1GUN_1_Arry[u8GCC1_GUN_1_Idx] = fValue;
		u8GCC1_GUN_1_Idx++;
	}
	else if (u8GCC1_GUN_1_Idx >= CC1_MEASURE_NUM)
	{
		u8GCC1_GUN_1_Idx = 0;
	}

	int i, j;
	for (i=0; i<CC1_MEASURE_NUM-1; i++) /* ��ѭ��Ϊ����������len��������len-1�� */
	{
		for (j=0; j<CC1_MEASURE_NUM-1-i; j++) /* ��ѭ��Ϊÿ�˱ȽϵĴ�������i�˱Ƚ�len-i�� */
		{ 
			if (fCC1GUN_1_Arry[j] > fCC1GUN_1_Arry[j+1]) /* ����Ԫ�رȽϣ��������򽻻�������Ϊ������ң������?�� */
			{ 
				fTemp = fCC1GUN_1_Arry[j];
				fCC1GUN_1_Arry[j] = fCC1GUN_1_Arry[j+1];
				fCC1GUN_1_Arry[j+1] = fTemp;
			}
		}
	}

	return fCC1GUN_1_Arry[CC1_MEASURE_NUM/2];
}

float f_CC1_GUN_2_MidArry(float fValue);
float f_CC1_GUN_2_MidArry(float fValue)
{
	float fTemp = 0;
	if (u8GCC1_GUN_2_Idx < CC1_MEASURE_NUM)
	{
		fCC1GUN_2_Arry[u8GCC1_GUN_2_Idx] = fValue;
		u8GCC1_GUN_2_Idx++;
	}
	else if (u8GCC1_GUN_2_Idx >= CC1_MEASURE_NUM)
	{
		u8GCC1_GUN_2_Idx = 0;
	}

	int i, j;
	for (i=0; i<CC1_MEASURE_NUM-1; i++) /* ��ѭ��Ϊ����������len��������len-1�� */
	{
		for (j=0; j<CC1_MEASURE_NUM-1-i; j++) /* ��ѭ��Ϊÿ�˱ȽϵĴ�������i�˱Ƚ�len-i�� */
		{ 
			if (fCC1GUN_2_Arry[j] > fCC1GUN_2_Arry[j+1]) /* ����Ԫ�رȽϣ��������򽻻�������Ϊ������ң������?�� */
			{ 
				fTemp = fCC1GUN_2_Arry[j];
				fCC1GUN_2_Arry[j] = fCC1GUN_2_Arry[j+1];
				fCC1GUN_2_Arry[j+1] = fTemp;
			}
		}
	}

	return fCC1GUN_2_Arry[CC1_MEASURE_NUM/2];
}

static void fCC1Deal(void)
{
	static float sfCC1_Volt_A;
	static float sfCC1_Volt_B;
	static float sfCC1_2_Volt_A;
	static float sfCC1_2_Volt_B;
	float fResistanceValue, ftemp;
	gADTestFVal1 = fgCC1QuickVal;												//PA3_ADC_SW_GUN_1_NO_0
	gADTestFVal1 = f_CC1_GUN_1_MidArry(fgCC1QuickVal);
	gADCC1Val = (((2.5*(double)gADTestFVal1)/(float)4096) * 6.00)*(2.0);
	gEvChargeInfo.fCC1_1VoltVal = gEvChargeInfo.fCC1_1Coef_A * gADCC1Val + gEvChargeInfo.fCC1_1Coef_B;
	if (gEvChargeInfo.u8CC1_CH_No == 0)
	{
		if (TRUE == gEvChargeInfo.u8CC1_1Set_A_Flag)
		{
			sfCC1_Volt_A = gADCC1Val;
			gEvChargeInfo.u8CC1_1Set_A_Flag = FALSE;
		}

		if (TRUE == gEvChargeInfo.u8CC1_1Set_B_Flag)
		{
			sfCC1_Volt_B = gADCC1Val;
			gEvChargeInfo.u8CC1_1Set_B_Flag = FALSE;
			gEvChargeInfo.fCC1_1Coef_A = (gEvChargeInfo.fCC1_Set_B_Volt - gEvChargeInfo.fCC1_Set_A_Volt)/(sfCC1_Volt_B  - sfCC1_Volt_A);
			gEvChargeInfo.fCC1_1Coef_B = gEvChargeInfo.fCC1_Set_B_Volt - (gEvChargeInfo.fCC1_1Coef_A * sfCC1_Volt_B);
			gEvChargeInfo.u8FloatParamUpDataCount++;
		}
	}
	
	//20211123 lkf �ɼ�PT1000
	ftemp = 12-gADCC1Val;
	if(ftemp > 0.01)
	{
		fResistanceValue = (2500 * gADCC1Val - 18000) / ftemp;

		//�¶�
		gEvChargeInfo.fTP1000_3_Temperture_Val = (float)(fResistanceValue -1000)/(float)3.822;
	}


	gADTestFVal1 = f_CC1_GUN_2_MidArry(fgCC1QuickVal_2);												//PA6_ADC_SW_GUN_2_oldBUS_2_NO_2
	gADCC2Val = (((2.5*(double)gADTestFVal1)/(float)4096) * 6.00) * (2.0);
	gEvChargeInfo.fCC1_2VoltVal = gADCC2Val * gEvChargeInfo.fCC1_2Coef_A + gEvChargeInfo.fCC1_2Coef_B;
	if (gEvChargeInfo.u8CC1_CH_No == 1)
	{
		if (TRUE == gEvChargeInfo.u8CC1_2Set_A_Flag)
		{
			sfCC1_2_Volt_A = gADCC2Val;
			gEvChargeInfo.u8CC1_2Set_A_Flag = FALSE;
		}
		if (TRUE == gEvChargeInfo.u8CC1_2Set_B_Flag)
		{
			sfCC1_2_Volt_B = gADCC2Val;
			gEvChargeInfo.u8CC1_2Set_B_Flag = FALSE;
			gEvChargeInfo.fCC1_2Coef_A = (gEvChargeInfo.fCC1_Set_B_Volt - gEvChargeInfo.fCC1_Set_A_Volt)/(sfCC1_2_Volt_B  - sfCC1_2_Volt_A);
			gEvChargeInfo.fCC1_2Coef_B = gEvChargeInfo.fCC1_Set_B_Volt - (gEvChargeInfo.fCC1_2Coef_A * sfCC1_2_Volt_B);
			gEvChargeInfo.u8FloatParamUpDataCount++;
		}
	}

	//20211123 lkf �ɼ�PT1000
	ftemp = 12-gADCC2Val;
	if(ftemp > 0.01)
	{
		fResistanceValue = (2500 * gADCC2Val - 18000) / ftemp;

		//�¶�
		gEvChargeInfo.fTP1000_4_Temperture_Val = (float)(fResistanceValue -1000)/(float)3.822;
	}


}

//#define USE_ONBOARD_PT1000		
static void TP1000_Ad_Deal(void)
{
	float fTP1000_1_Real_Volt = 0;
	float fTP1000_2_Real_Volt = 0;
	float f32adj1 = 1;
	__XXX_UNION_VALUE unTempVal;

	fTP1000_2_Real_Volt = (2.5*(double)((UINT16)(gMeasureInfo.u16MeasureValue[PB0_ADC_SW_oldHOLL_TP1000_NO_3])))/(float)4096;		
	//����
	fTP1000_2_Real_Volt =  ((1500*fTP1000_2_Real_Volt)/(5-fTP1000_2_Real_Volt));
	fTP1000_2_Real_Volt = f_TP100_2_MidArry(fTP1000_2_Real_Volt);
	gEvChargeInfo.fTP1000_2_Resist = (gEvChargeInfo.fTP1000_2_CoefK * fTP1000_2_Real_Volt) + gEvChargeInfo.fTP1000_2_CoefB;
	//�¶�
	gEvChargeInfo.fTP1000_2_Temperture_Val = (float)(gEvChargeInfo.fTP1000_2_Resist -1000)/(float)3.822;


	fTP1000_1_Real_Volt = (2.5*(double)((UINT16)(gMeasureInfo.u16MeasureValue[PC3_ADC_SW_TP1000_NO_5])))/(float)4096;	//��࣡��?
	//����
	fTP1000_1_Real_Volt = ((1500*fTP1000_1_Real_Volt)/(5-fTP1000_1_Real_Volt));
	fTP1000_1_Real_Volt = f_TP100_1_MidArry(fTP1000_1_Real_Volt);
	gEvChargeInfo.fTP1000_1_Resist = (gEvChargeInfo.fTP1000_1_CoefK * fTP1000_1_Real_Volt) + gEvChargeInfo.fTP1000_1_CoefB;
	//�¶�
	gEvChargeInfo.fTP1000_1_Temperture_Val = (float)(gEvChargeInfo.fTP1000_1_Resist -1000)/(float)3.822;



	if (gEvChargeInfo.u8CTL_TP1000_Pioint_1_Flag == TRUE)
	{
		//ȡX1
		if(ABS(gEvChargeInfo.u8TP1000_Chnl_No -1) <= 0.05)
		{
			gEvChargeInfo.fTP_1000_Resist_A_Point_Val = fTP1000_1_Real_Volt;
		}
		else
		{
			gEvChargeInfo.fTP_1000_Resist_A_Point_Val = fTP1000_2_Real_Volt;
		}
		gEvChargeInfo.u8CTL_TP1000_Pioint_1_Flag = FALSE;
	}

	if (gEvChargeInfo.u8CTL_TP1000_Pioint_2_Flag == TRUE)
	{
		gEvChargeInfo.u8CTL_TP1000_Pioint_2_Flag  = FALSE;
		//ȡX2
		if(ABS(gEvChargeInfo.u8TP1000_Chnl_No -1) <= 0.05)
		{
			gEvChargeInfo.fTP_1000_Resist_B_Point_Val = fTP1000_1_Real_Volt;
		}
		else
		{
			gEvChargeInfo.fTP_1000_Resist_B_Point_Val = fTP1000_2_Real_Volt;
		}
		
		if (gEvChargeInfo.f_CTL_TP1000_Point_2_Val > gEvChargeInfo.f_CTL_TP1000_Point_1_Val)
		{
			//����K��ֵ
			f32adj1 = (gEvChargeInfo.f_CTL_TP1000_Point_2_Val  - gEvChargeInfo.f_CTL_TP1000_Point_1_Val)/(gEvChargeInfo.fTP_1000_Resist_B_Point_Val - gEvChargeInfo.fTP_1000_Resist_A_Point_Val);
			if (f32adj1 > 0.9 && f32adj1 < 1.1)
			{
				if(ABS(gEvChargeInfo.u8TP1000_Chnl_No -1) <= 0.05)
				{
					gEvChargeInfo.fTP1000_1_CoefK  = f32adj1;
					gEvChargeInfo.fTP1000_1_CoefB =  gEvChargeInfo.f_CTL_TP1000_Point_2_Val - (gEvChargeInfo.fTP1000_1_CoefK * gEvChargeInfo.fTP_1000_Resist_B_Point_Val);
					unTempVal.ftemp = gEvChargeInfo.fTP1000_1_CoefK;
					ExSetSigInfo(E_TYPE_SYS, ID_TP1000_1_CoefA, unTempVal,NULL, BY_E2PROM);
					unTempVal.ftemp = gEvChargeInfo.fTP1000_1_CoefB;
					ExSetSigInfo(E_TYPE_SYS, ID_TP1000_1_CoefB, unTempVal, NULL, BY_E2PROM);
				}
				else
				{
					gEvChargeInfo.fTP1000_2_CoefK  = f32adj1;
					gEvChargeInfo.fTP1000_2_CoefB =  gEvChargeInfo.f_CTL_TP1000_Point_2_Val - (gEvChargeInfo.fTP1000_2_CoefK * gEvChargeInfo.fTP_1000_Resist_B_Point_Val);
					unTempVal.ftemp = gEvChargeInfo.fTP1000_2_CoefK;
					ExSetSigInfo(E_TYPE_SYS, ID_TP1000_2_CoefA, unTempVal,NULL, BY_E2PROM);
					unTempVal.ftemp = gEvChargeInfo.fTP1000_2_CoefB;
					ExSetSigInfo(E_TYPE_SYS, ID_TP1000_2_CoefB, unTempVal, NULL, BY_E2PROM);
				}
			}
		}
	}

}


static void AC_CC_Ad_Deal(void)
{
	float fTempVolt = 0;
	fTempVolt = (2.5*(double)((UINT16)(gMeasureInfo.u16MeasureValue[PC0_ADC_SW_oldCP_CC_OF_AC_NO_4])))/(float)4096;				//VO
	gEvChargeInfo.fSocketVoltVal = fTempVolt;
	gEvChargeInfo.fResist_of_cable =  ((400000 * gEvChargeInfo.fSocketVoltVal)/(1200000-(400000*gEvChargeInfo.fSocketVoltVal) - gEvChargeInfo.fSocketVoltVal))*1000;

}


#define BUS_MEASURE_NUM			10		//AC_MEASURE_NUM  ����С��6��
float f_A_BUS_Arry[BUS_MEASURE_NUM + 2];
UINT8 u8A_BUS_Idx = 0;

float f_B_BUS_Arry[BUS_MEASURE_NUM + 2];
UINT8 u8B_BUS_Idx = 0;

float fMid_A_BUS_Arry(float fValue);
float fMid_A_BUS_Arry(float fValue)
{
	float fTemp = 0;
	if (u8A_BUS_Idx < BUS_MEASURE_NUM)
	{
		f_A_BUS_Arry[u8A_BUS_Idx] = fValue;
		u8A_BUS_Idx++;
	}
	else if (u8A_BUS_Idx >= BUS_MEASURE_NUM)
	{
		u8A_BUS_Idx = 0;
	}

	int i, j;
	for (i=0; i<BUS_MEASURE_NUM-1; i++) /* ��ѭ��Ϊ����������len��������len-1�� */
	{
		for (j=0; j<BUS_MEASURE_NUM-1-i; j++) /* ��ѭ��Ϊÿ�˱ȽϵĴ�������i�˱Ƚ�len-i�� */
		{ 
			if (f_A_BUS_Arry[j] > f_A_BUS_Arry[j+1]) /* ����Ԫ�رȽϣ��������򽻻�������Ϊ������ң������?�� */
			{ 
				fTemp = f_A_BUS_Arry[j];
				f_A_BUS_Arry[j] = f_A_BUS_Arry[j+1];
				f_A_BUS_Arry[j+1] = fTemp;
			}
		}
	}

	fTemp = f_A_BUS_Arry[BUS_MEASURE_NUM/2];
	return fTemp;
}

float fMid_B_BUS_Arry(float fValue);
float fMid_B_BUS_Arry(float fValue)
{
	float fTemp = 0;
	if (u8B_BUS_Idx < BUS_MEASURE_NUM)
	{
		f_B_BUS_Arry[u8B_BUS_Idx] = fValue;
		u8B_BUS_Idx++;
	}
	else if (u8B_BUS_Idx >= BUS_MEASURE_NUM)
	{
		u8B_BUS_Idx = 0;
	}

	int i, j;
	for (i=0; i<BUS_MEASURE_NUM-1; i++) /* ��ѭ��Ϊ����������len��������len-1�� */
	{
		for (j=0; j<BUS_MEASURE_NUM-1-i; j++) /* ��ѭ��Ϊÿ�˱ȽϵĴ�������i�˱Ƚ�len-i�� */
		{ 
			if (f_B_BUS_Arry[j] > f_B_BUS_Arry[j+1]) /* ����Ԫ�رȽϣ��������򽻻�������Ϊ������ң������?�� */
			{ 
				fTemp = f_B_BUS_Arry[j];
				f_B_BUS_Arry[j] = f_B_BUS_Arry[j+1];
				f_B_BUS_Arry[j+1] = fTemp;
			}
		}
	}

	fTemp = f_B_BUS_Arry[BUS_MEASURE_NUM/2];
	return fTemp;
}
static void fBusVoltageDeal(void)
{
	__XXX_UNION_VALUE unTempVal;
	FLOAT32 f32adj1;						
	f32adj1 = 0;
	float fBUS_A_ReadVal_1;	
	float fBUS_B_ReadVal_1;			

	UINT32 Freq_1_AverValue = 0;
	UINT32 Freq_2_AverValue = 0;
   
	if(u8Is1000VHardwareA00())
	{
		//43.90115667718191377497371188223			1229.8167718191377497371188222923
		//0.00397478848446993356424961671682		1339.1479756970075520981204928738
		//0.00383255039803773419620468009154		1270.3497475991809292292193642346			67
		//0.08294821661334281312951771536912		1375.375300655221033424566641784			32
		//fBUS_A_ReadVal_1=  (((double)fFreq_1_AverValue * 0.08294821661334281312951771536912))   - 1375.375300655221033424566641784;
		Freq_1_AverValue = gFANMngInfo.u32FanPulseAver[BUS_VOLT_1_SQC_NO]*(1000/(PER_CYCL_TIME * 10));
		fBUS_A_ReadVal_1 = (((((double)Freq_1_AverValue)/1000.00) * 2.95675798052977133801222549242));
		fBUS_A_ReadVal_1 = fBUS_A_ReadVal_1 - 1694.1854199683042789223454833597;
		
		//0.04390115667718191377497371188223		1229.8167718191377497371188222923
		//fBUS_B_ReadVal_1 = (((double)fFreq_2_AverValue * 0.08294821661334281312951771536912))   - 1375.375300655221033424566641784;
		//fBUS_B_ReadVal_1 = (((((double)Freq_2_AverValue)/1000.00) * 3.8325503980377));
		//fBUS_B_ReadVal_1 = fBUS_B_ReadVal_1 - 1270.34974;
		Freq_2_AverValue = gFANMngInfo.u32FanPulseAver[BUS_VOLT_2_SQC_NO]*(1000/(PER_CYCL_TIME * 10));
		fBUS_B_ReadVal_1 = (((((double)Freq_2_AverValue)/1000.00) * 2.98377884395704820653415581448));
		fBUS_B_ReadVal_1 = fBUS_B_ReadVal_1 - 1729.5663696595841900845327850126;
		
	}
	else
	{
		gADTestFVal1 = (UINT16)(gMeasureInfo.u16MeasureValue[PA5_ADC_SW_oldBUS_1_None_NO_7]);
		fBUS_A_ReadVal_1 = (2.5*(double)gADTestFVal1)/(float)4096;				//VO
		fBUS_A_ReadVal_1 = HOLL_COEF_K*fBUS_A_ReadVal_1-HOLL_COEF_B;
		gADTestFVal1 = (UINT16)(gMeasureInfo.u16MeasureValue[PA6_ADC_SW_GUN_2_oldBUS_2_NO_2]);
		fBUS_B_ReadVal_1 = (2.5*(double)gADTestFVal1)/(float)4096;				//VO
		fBUS_B_ReadVal_1 = HOLL_COEF_K*fBUS_B_ReadVal_1-HOLL_COEF_B;
	}

	gEvChargeInfo.fEV_BattVolt_x[GUN_NO_A] = (fBUS_A_ReadVal_1 * gEvChargeInfo.fBattVolt_1CoefA) + gEvChargeInfo.fBattVolt_1CoefB;
	gEvChargeInfo.fEV_BattVolt_x[GUN_NO_B] = (fBUS_B_ReadVal_1 * gEvChargeInfo.fBattVolt_2CoefA) + gEvChargeInfo.fBattVolt_2CoefB;

	gEvChargeInfo.fEV_Batt_x_Volt_filter[GUN_NO_A] = fMid_A_BUS_Arry(gEvChargeInfo.fEV_BattVolt_x[GUN_NO_A]);
	gEvChargeInfo.fEV_Batt_x_Volt_filter[GUN_NO_B] = fMid_B_BUS_Arry(gEvChargeInfo.fEV_BattVolt_x[GUN_NO_B]);

	if (gEvChargeInfo.u8Batt_1VoltSet_A_Flag == TRUE)
	{
		//ȡX1
		gEvChargeInfo.fBattVoltRead_A_Volt = fBUS_A_ReadVal_1;
		gEvChargeInfo.u8Batt_1VoltSet_A_Flag = FALSE;
	}

	if (gEvChargeInfo.u8Batt_1VoltSet_B_Flag == TRUE)
	{
		gEvChargeInfo.u8Batt_1VoltSet_B_Flag  = FALSE;
		//ȡX2
		gEvChargeInfo.fBattVoltRead_B_Volt = fBUS_A_ReadVal_1;
		if (gEvChargeInfo.fBattSet_B_Volt > gEvChargeInfo.fBattSet_A_Volt)
		{
			//����K��ֵ
			f32adj1 = (gEvChargeInfo.fBattSet_B_Volt - gEvChargeInfo.fBattSet_A_Volt)/(gEvChargeInfo.fBattVoltRead_B_Volt  - gEvChargeInfo.fBattVoltRead_A_Volt);
			if (f32adj1 > 0.8 && f32adj1 < 1.4)
			{
				gEvChargeInfo.fBattVolt_1CoefA  = f32adj1;
				gEvChargeInfo.fBattVolt_1CoefB =  gEvChargeInfo.fBattSet_B_Volt - (gEvChargeInfo.fBattVolt_1CoefA * gEvChargeInfo.fBattVoltRead_B_Volt);
				unTempVal.ftemp = gEvChargeInfo.fBattVolt_1CoefA;
				ExSetSigInfo(E_TYPE_SYS, ID_fBattVolt_1CoefA, unTempVal,NULL, BY_E2PROM);
				unTempVal.ftemp = gEvChargeInfo.fBattVolt_1CoefB;
				ExSetSigInfo(E_TYPE_SYS, ID_fBattVolt_1CoefB, unTempVal, NULL, BY_E2PROM);
			}
		}
	}

	if (gEvChargeInfo.u8Batt_2VoltSet_A_Flag == TRUE)
	{
		//ȡX1
		gEvChargeInfo.fBattVoltRead_A_Volt = fBUS_B_ReadVal_1;
		gEvChargeInfo.u8Batt_2VoltSet_A_Flag = FALSE;
	}

	if (gEvChargeInfo.u8Batt_2VoltSet_B_Flag == TRUE)
	{
		gEvChargeInfo.u8Batt_2VoltSet_B_Flag  = FALSE;
		//ȡX2
		gEvChargeInfo.fBattVoltRead_B_Volt = fBUS_B_ReadVal_1;
		if (gEvChargeInfo.fBattSet_B_Volt > gEvChargeInfo.fBattSet_A_Volt)
		{
			//����K��ֵ
			f32adj1 = (gEvChargeInfo.fBattSet_B_Volt - gEvChargeInfo.fBattSet_A_Volt)/(gEvChargeInfo.fBattVoltRead_B_Volt - gEvChargeInfo.fBattVoltRead_A_Volt);
			if (f32adj1 > 0.8 && f32adj1 < 1.4)
			{
				gEvChargeInfo.fBattVolt_2CoefA  = f32adj1;
				gEvChargeInfo.fBattVolt_2CoefB =  gEvChargeInfo.fBattSet_B_Volt - (gEvChargeInfo.fBattVolt_2CoefA * gEvChargeInfo.fBattVoltRead_B_Volt);
				unTempVal.ftemp = gEvChargeInfo.fBattVolt_2CoefA;
				ExSetSigInfo(E_TYPE_SYS, ID_fBattVolt_2CoefA, unTempVal,NULL, BY_E2PROM);
				unTempVal.ftemp = gEvChargeInfo.fBattVolt_2CoefB;
				ExSetSigInfo(E_TYPE_SYS, ID_fBattVolt_2CoefB, unTempVal, NULL, BY_E2PROM);
			}
		}
	}
}
UINT8 u8IsDoorOpen(void)
{
//by lxr01202
/*
	UINT8 uDIVal = 0;

	uDIVal = BSP_u8GetDI7Status();
	uDIVal &= 0x01;

	if ((0x01 == uDIVal) && (FALSE == gEvChargeInfo.u8DoorAlmMask_1))
	{
		return TRUE;
	}

	return FALSE;
*/
}
UINT8 u8IsEPOActive(void)
{
	UINT8 uDIVal = 0;

	uDIVal = BSP_u8GetDI1Status();				// EPO 
	uDIVal &= 0x01;

	if ((gEvChargeInfo.u8EPOAlmVltLEVEL == uDIVal))
	{
		return TRUE;
	}

	return FALSE;
}

void Temperature_Ad_Deal(void);
void Temperature_Ad_Deal(void)
{
	//PA5 ����������ֵ���� ad_volt!!!
	gADTestFVal1 = (UINT16)(gMeasureInfo.u16MeasureValue[PA4_ADC_SW_TEMP_NO_1]);         //AD PA4	//�°���ȡPA4��
	gADTestFVal1 = (2.5*(double)gADTestFVal1)/(float)4096;
	gADTestFVal2 = gADTestFVal1 - 0.5;
	gADTestFVal2 =  (float)gADTestFVal2/0.01; 

	gEvChargeInfo.fEV_InterTemp =  gADTestFVal2;                       //-27.355
}

void PS_Deal(void);
void PS_Deal(void)
{
	//PCM 320 ѹ����ר��ѹ��������
	//����  0--1MPa		����12-36V		���?0--5V  �ͺ�20111072
	float fTempValue = 0;
	fTempValue = (UINT16)(gMeasureInfo.u16MeasureValue[PC2_ADC_SW_oldNone_PS_NO_8]);         
	fTempValue = (2.5*(double)fTempValue)/(float)4096;
	gEvChargeInfo.f_PS_VoltVal = fTempValue * 2;
	//if (u8CAN_2_Is_CHADEMO())	
	//{
	//	if(gEvChargeInfo.f_PS_VoltVal > 1.2)
	//	{
	//		gEvChargeInfo.u8EV_SpearPst[GUN_NO_B] = SPEAR_ON_LINE;
	//	}
	//	else
	//	{
	//		gEvChargeInfo.u8EV_SpearPst[GUN_NO_B] = SPEAR_NO_ON_LINE;
	//	}
	//	
	//}
	
	gEvChargeInfo.f_PS_MpaVal = gEvChargeInfo.f_PS_VoltVal* (1.00000/5.00000);
}

void GetVFAnalog(void);
void GetVFAnalog(void)
{
	Temperature_Ad_Deal();
	fCC1Deal();
	fBusVoltageDeal();
	TP1000_Ad_Deal();
	AC_CC_Ad_Deal();
	PS_Deal();

	if(gMeasureInfo.ucIsMeasureOneLoop == TRUE)
	{
		if(gMeasureInfo.u16MeasureLoopTimes >= 20)
		{
			gMeasureInfo.u16MeasureLoopTimes = 0;
			gMeasureInfo.fGet_AD_FilerValue();
		}
		gMeasureInfo.ucIsMeasureOneLoop = FALSE;
	}

}

/***********************************************
	��������:������
************************************************/
static UCHAR init_mutex(void)
{
	gMeasureInfo.hMutexReadWriteMeasureData = MutexCreate(UPPER_PRIO_READ_WRITE_MEASURE_DATA);
	if(gMeasureInfo.hMutexReadWriteMeasureData == NULL)
	{
		return FALSE;
	}
	return  TRUE;
}

MeasureData_TypeDef gMeasureInfo;

/************************************************/
/*��������:��ʼ��				*/
/************************************************/
void init_gs_Measure(void)
{
	UCHAR i;
	gMeasureInfo.hMutexReadWriteMeasureData = NULL;
	for(i=0;i<MAX_CHANEL_NUM;i++)
	{
		gMeasureInfo.u16MeasureValue[i] = 0;
	}
	gMeasureInfo.ucIsMeasureOneLoop = 0;
	gMeasureInfo.MeasureCount = 0;
	gMeasureInfo.isTest = 0;
	gMeasureInfo.MeasureDelay = 0;
	gMeasureInfo.pMeasure = 0;

	gMeasureInfo.ucAddressDelay =0;

	gEvChargeInfo.fTP1000_3_Temperture_Val = 0;
	gEvChargeInfo.fTP1000_4_Temperture_Val = 0;

	gMeasureInfo.fInitMutex = init_mutex;
	gMeasureInfo.fStartDelay = start_delay;
	gMeasureInfo.fGet_AD_FilerValue = Get_AD_FilerValue;
	gMeasureInfo.fInitMeasureAnalogData = init_measureanalogdata;
	gMeasureInfo.fSetTestStatus = set_teststatus;
	gMeasureInfo.fIsTest = is_test;
	gMeasureInfo.fSetAlarmLimit = set_alarm_limit;
	gMeasureInfo.fIsMeasureOneLoop = is_measureoneloop;
	gMeasureInfo.fClearMeasureOneLoop = clear_measureoneloop;
	gMeasureInfo.fGetMeasureAnalogData = get_measureanalogdata;
	gMeasureInfo.fSetMeasureAnalogData = set_measureanalogdata;
	gMeasureInfo.fGetVFAnalog = GetVFAnalog;
	gMeasureInfo.fGetUserData = GetUserData;
}

static void RefreshDI(UINT8 uDelayTimes)	//uDelayTimes MAX 255 MIN 0
{
	static UINT16 u16SPDAlmDelay = 0;
	UINT8 u8SPDAlm = 0;
	UINT8 u8EPOAlm = 0;

//by lxr01202
	UINT8 u8SysFanALM = 0;
	static UINT16 u16SysFanALMDelay = 0;
	static UINT16 u16EPOAlmDelay = 0;
	static UINT16 u16Forbid_ChgAndDisChrAgDelay = 0;        //����������Ŵ����˲�?
	UINT8 u8DoorAlm = 0;
	UINT8 u8temp, u8AlmStat;
	u8temp = 0; u8AlmStat = 0;
	UNUSED(u8temp);UNUSED(u8AlmStat);
	static UINT16 u16DoorAlmDelay = 0;
/************************************************************************************************************
	����˵��DI ��˵����DI  ��CPU �ܽŶԲ��ϣ�
	DI1  EPO
	DI2  ������A ����
	DI4  Aǹ��Դ����
	DI3  SPD
	DI5  ���ǹ�¶ȸ�?
	DI6  ������B ����
	DI7  �Ž��澯
	DI8  Bǹ��Դ����
***********************************************************************************************************/
	UINT8 uDIVal = 0;
	UINT8 uEx1DIVal = 0;
	uDIVal = BSP_u8GetDI1Status();				// EPO 
	uDIVal |= (BSP_u8GetDI2Status() << 1);		//������A ����
	uDIVal |= (BSP_u8GetDI3Status() << 2);		//����DI3
	uDIVal |= (BSP_u8GetDI4Status() << 3);		//DI4
	uDIVal |= (BSP_u8GetDI5Status() << 4);		//DI5 
	uDIVal |= (BSP_u8GetDI6Status() << 5);		//DI6 
	uDIVal |= (BSP_u8GetDI7Status() << 6);		//DI7 
	uDIVal |= (BSP_u8GetDI8Status() << 7);		//DI8
	uDIVal |= (BSP_u8GetDI9Status() << 8);		//DI9               //????
	uDIVal |= (BSP_u8GetDI10Status() << 9);	    //DI10              //????
	gEvChargeInfo.u8EV_InterDI_COM = gEvChargeInfo.u8EV_InterDI_COM & 0xf0;
	/*********************************************************************
		RS232-1�ӿڲ���״̬		.4
		RS232-2�ӿڲ���״̬		.5
		CAN�ӿڲ���״̬		.6
		485�ӿڲ���״̬		.7
	*********************************************************************/
	gEvChargeInfo.u8EV_InterDI_COM |= (uDIVal&0x0f); //��4λDI1-4    ����λ
	//gEvChargeInfo.u8EV_InterDI_COM &= (0xf0|uDIVal);


/*	= BSP_u8GetDI2Status();*/
	
/*	= BSP_u8GetDI4Status();*/
	
//by lxr01202   �޸�DI1Ϊ���EPO(ESD)��ͣ�������?
	//DI1
	u8EPOAlm = ((uDIVal) & 0x01);
    //u8EPO_2Alm = ((uDIVal>>4) & 0x01);
	if ((gEvChargeInfo.u8EPOAlmVltLEVEL == u8EPOAlm))
	{
		u16EPOAlmDelay = 0;
		gEvChargeInfo.u8EPOAlm = TRUE;						//�����澯��

//��ͣ���º������ؽӴ��������⼱ͣ�ָ���Ӵ����?ʱ����
//by lxr01202    ���¼�ͣ���ù���Ͽ���?
			__MPPT_1_INPUT_OFF();
			__MPPT_2_INPUT_OFF();
			//SEGGER_RTT_printf(0,"EPO_PV_OFF.\n\r");
	}
	else
	{
		u16EPOAlmDelay++;
		if (u16EPOAlmDelay > 20)
		{
			u16EPOAlmDelay = 0;
			gEvChargeInfo.u8EPOAlm = FALSE;
		}
	}



//by lxr01202
//DI2  #1���ֱ���Ӵ���������������?�����Ӵ������ϣ��պϣ��Ӵ����Ͽ����Ͽ���

if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1Exist == 1)
{
	u8temp = (uDIVal >> 1)&0x01;
	if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV1ContactorEn)
	{
		gs_EsSampleInfo.PV_Contactor_Detection[0] == TRUE;
	}
	else
	{
		gs_EsSampleInfo.PV_Contactor_Detection[0] == FALSE;
	}
}
//DI3  #2���ֱ���Ӵ���������������?�����Ӵ������ϣ��պϣ��Ӵ����Ͽ����Ͽ���
if(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1Exist == 1)
{
	u8temp = (uDIVal >> 2)&0x01;
	if(gEvChargeInfo.u16AttributesBitsMap_4.BIT_FIELD.PV2ContactorEn)
	{
		gs_EsSampleInfo.PV_Contactor_Detection[0] == TRUE;
	}
	else
	{
		gs_EsSampleInfo.PV_Contactor_Detection[0] == FALSE;
	}
}

//by lxr01202
//DI9
        if((gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt1ChgForbitDiEn == 1)&&(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt1Exist))                
        {
                u8AlmStat = 1; 
                u8temp = (uDIVal >> 8)&0x01;         	
                
                if ( (u8AlmStat == u8temp) && (gs_EsSampleInfo.Batt_CanComm_Break_Alm[0] != 1) &&
                     ((gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.BMS_Limit_ChargeCurrent <= 1) && 
                     gs_IMMU2_LCCU_Mng.ESBattMng[0].BattInfoFromCan.BMS_Limit_DischargeCurrent <= 1) )        
                {
                       u16Forbid_ChgAndDisChrAgDelay ++;                                
                 if(u16Forbid_ChgAndDisChrAgDelay > 30)                        //�����A������Ŵ����˲�?
                {
                        u16Forbid_ChgAndDisChrAgDelay = 0;                        //����˲����?
                        gs_IMMU2_LCCU_Mng.ESBattMng[0].u8IsAllowChgOrDischgFromDI = 0;  //�Ͽ�����ֹ��ŵ�?
                        gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[0] = 1;  //�澯
                }
                }
                else
                {
                        u16Forbid_ChgAndDisChrAgDelay = 0;                        //����˲����?
                        gs_IMMU2_LCCU_Mng.ESBattMng[0].u8IsAllowChgOrDischgFromDI = 1;
                        gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[0] = 0;  
                }
        }
        else
        {
                gs_IMMU2_LCCU_Mng.ESBattMng[0].u8IsAllowChgOrDischgFromDI = 1;
                gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[0] = 0; 
        }

//DI10
        if((gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt2ChgForbitDiEn == 1)&&(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt2Exist))                
        {
                u8AlmStat = 1;  //�Ͽ��澯
                //u8temp = BSP_u8GetDI2Status())                                        //��߶��? �ɼ����ߵ�ƽ
                u8temp = (uDIVal >> 9)&0x01;         	
                 //��ֹ�������DI���ţ������A��������������Ƶ������?�Ź����ж�
                if ( (u8AlmStat == u8temp) && (gs_EsSampleInfo.Batt_CanComm_Break_Alm[1] != 1) &&
                     ((gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.BMS_Limit_ChargeCurrent <= 1) && 
                     gs_IMMU2_LCCU_Mng.ESBattMng[1].BattInfoFromCan.BMS_Limit_DischargeCurrent <= 1) )        
                {
                        u16Forbid_ChgAndDisChrAgDelay ++;                                
                if(u16Forbid_ChgAndDisChrAgDelay > 30)                        //�����A������Ŵ����˲�?
                {
                        u16Forbid_ChgAndDisChrAgDelay = 0;                        //����˲����?
                        gs_IMMU2_LCCU_Mng.ESBattMng[1].u8IsAllowChgOrDischgFromDI = 0;  //�Ͽ�����ֹ��ŵ�?
                        gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[1] = 1;  //�澯
                }
                }
                else
                {
                        u16Forbid_ChgAndDisChrAgDelay = 0;                        //����˲����?
                        gs_IMMU2_LCCU_Mng.ESBattMng[1].u8IsAllowChgOrDischgFromDI = 1;
                        gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[1] = 0;  
                }
        }
        else
        {
                gs_IMMU2_LCCU_Mng.ESBattMng[1].u8IsAllowChgOrDischgFromDI = 1;
                gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[1] = 0; 
        }




//by lxr01202     DI14 ɢ�ȷ���RDB���?

	u8SysFanALM = (0x01 & (uDIVal >> 14));	 //DI7
	if (0x01 == u8SysFanALM)
	{
		gEvChargeInfo.u8SysFanALM = TRUE;
		u16SysFanALMDelay = 0;
	}
	else
	{
			gEvChargeInfo.u8SysFanALM = FALSE;
	}

//by lxr01202   ���ε����?4�����DI���?
/*
	//DI8
	//if((gEvChargeInfo.u8BattGrpXAlowChgOrDischgDI_En[3] == 1)&&(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4Exist))
	if((gEvChargeInfo.u16AttributesBitsMap_3.BIT_FIELD.Batt4ChgForbitDiEn == 1)&&(gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.Batt4Exist))	
	{
		u8AlmStat = 1;  //�Ͽ��澯
		u8temp = (uDIVal >> 7)&0x01;	//DI6			
		if (u8AlmStat == u8temp)
		{
			gs_IMMU2_LCCU_Mng.ESBattMng[3].u8IsAllowChgOrDischgFromDI = 0;  //�Ͽ�����ֹ��ŵ�?
			gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[3] = 1;  //�澯
		}
		else
		{
			gs_IMMU2_LCCU_Mng.ESBattMng[3].u8IsAllowChgOrDischgFromDI = 1;
			gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[3] = 0;  
		}
	}
	else
	{
		gs_IMMU2_LCCU_Mng.ESBattMng[3].u8IsAllowChgOrDischgFromDI = 1;
		gs_EsSampleInfo.Battry_Forbid_ChgAndDisChrg[3] = 0; 
	}
*/
	uEx1DIVal = 0;
	gEvChargeInfo.u8EV_Ex1DI = uEx1DIVal;
	uDIVal = 0x0a;	// ����λ����DI ��û���Թ���
	uDIVal |= (BSP_u8GetDI5Status() << 4);
	uDIVal |= (BSP_u8GetDI6Status() << 5);
	uDIVal |= (BSP_u8GetDI7Status() << 6);
	uDIVal |= (BSP_u8GetDI8Status() << 7);
	gEvChargeInfo.u8EV_Ex2DI = uDIVal;	
	
}


void EepromRecord_Init(void)
{
	UINT8 j;

	for(j = 0; j < 2; j++)
	{
		gs_EepromRecordMng.u8InitCompleteFlag[j] = EEPROM_RECORD_InitFlag_WriteOk;
	}
		
}


// 
// static void AC_PWM_OUT(void);
// static void AC_PWM_OUT(void)
// {
// 	float fImax = 33.42;									//22KW
// 	UINT32 u32D = 100;										//�ߵ�ƽ
// 	ExGetSigInfo(E_TYPE_SYS, ID_AC_POWER_SET, &ungMsrVal, NULL, BY_SRAM);
// 	//ungMsrVal.ftemp = 7;
// 	if (ungMsrVal.ftemp >= 41.4)
// 	{
// 		ungMsrVal.ftemp = 40;
// 	}
// 	else if (ungMsrVal.ftemp < 7)
// 	{
// 		ungMsrVal.ftemp = 7;
// 	}
// 	if (ungMsrVal.ftemp <= 7)
// 	{
// 		fImax = 7000/220;
// 	}
// 	else
// 	{
// 		fImax = ((ungMsrVal.ftemp * 1000)/1.732)/380;			//��ʱֻ��������������м���?
// 	}
// 	//���ࣺP(���ʣ���)=U����ѹ���������� I(��������)
// 	//���๦�ʣ� P = ����3 * U * I * cos�գ�����������
// 	float fICableAllowMaxCurr = 63;
// 	if(gEvChargeInfo.u8_C_LOCKType == SPEAR_TYPE_IS_SOCKET)
// 	{
// 		if (gEvChargeInfo.fResist_of_cable >= (1500 - (1500*0.04)))
// 		{
// 			fICableAllowMaxCurr = 13;
// 		}
// 		else if ((gEvChargeInfo.fResist_of_cable >= 580) && (gEvChargeInfo.fResist_of_cable <= 780))
// 		{
// 			fICableAllowMaxCurr = 20;
// 		}
// 		else if ((gEvChargeInfo.fResist_of_cable >= 170) && (gEvChargeInfo.fResist_of_cable <= 300))
// 		{
// 			fICableAllowMaxCurr = 32;
// 		}
// 		else
// 		{
// 			fICableAllowMaxCurr = 63;//��������ֻ������
// 		}	
// 	}
// 	fImax = MIN(fICableAllowMaxCurr, fImax);
// 	
// 	if (fImax <= 51)
// 	{
// 		u32D = (UINT32)(fImax/0.6);
// 	}
// 	else
// 	{
// 		u32D = (UINT32)((fImax/2.5)+ 64);
// 	}
// 	gEvChargeInfo.fAC_MAXCurrent = fImax;
// 	u32D = (uint32_t)(u32D * 14);
// 	u32D = u32D-BACK_START_COUNT;
// 	//dv_Capture_PWM.fOutputFanDriver(AC_PWM,(u32D + 50));
// 	if (gEvChargeInfo.iAC_AddCompensation > AC_PWM_COMP_DETA)//if (iAddCompensation > AC_PWM_COMP_DETA)
// 	{
// 		gEvChargeInfo.iAC_AddCompensation = AC_PWM_COMP_DETA;//iAddCompensation = AC_PWM_COMP_DETA;
// 	}
// 
// 	if (gEvChargeInfo.u32PWMLoadValue <= 0)
// 	{
// 		dv_Capture_PWM.fOutputFanDriver(AC_PWM,(u32D + gEvChargeInfo.iAC_AddCompensation));
// 	}
// 	else
// 	{
// 		dv_Capture_PWM.fOutputFanDriver(AC_PWM,(gEvChargeInfo.u32PWMLoadValue));
// 	}
// 	
// 	
// }

//#define AC_PWM_STOP() dv_Capture_PWM.fOutputFanDriver(AC_PWM, 110*14)

//if ((gEvChargeInfo.u8A_Current <= 0.4)
//	&& (gEvChargeInfo.u8B_Current <= 0.4)
//	&& (gEvChargeInfo.u8C_Current <= 0.4))
//{
//	if (gEvChargeInfo.iACFinishCheckCount == AC_FINISH_STOP_COUNT)
//	{
//		gEvChargeInfo.iACFinishCheckCount = AC_FINISH_START_COUNT;//��������
//	}
//	if (gEvChargeInfo.iACFinishCheckCount > (AC_FINISH_COUNT_TIME_OUT*5))
//	{
//		__AC_GUN_POWER_OFF();
//		gEvChargeInfo.u8AC_StartCMD = FALSE;
//		AC_PWM_STOP();
//		__AC_CHARGE_LED_OFF();
//		__AC_GUN_POWER_OFF();
//		siDelay = 0;
//		gEvChargeInfo.u8AC_StopReason = STOP_EV_NORMAL;
//	}
//}
//else
//{
//	gEvChargeInfo.iACFinishCheckCount = AC_FINISH_STOP_COUNT;
//}



UINT8 u8NeedAjustRate(void);
UINT8 u8NeedAjustRate(void)
{
	static int iCount = 0;
	OSTimeDly(10);
	iCount++;
	if(iCount >= 6)
	{
		iCount = 0;
		return TRUE;
	}
	return FALSE;
}

UINT32 u32GetCalcPulseRate();
UINT32 u32GetCalcPulseRate()
{
	if(gEvChargeInfo.fAC_MAXCurrent <= 51)
	{
		return (((UINT32)(gEvChargeInfo.fAC_MAXCurrent/0.6))*14);
	}
	else
	{
		return (((UINT32)((gEvChargeInfo.fAC_MAXCurrent/2.500)+ 64))*14);
	}
}


void AC_PWM_Pulse_Voltage_Deal(void)
{
	if (gEvChargeInfo.u8HasACGun == FALSE)
	{
		return;
	}

	if(gFANMngInfo.u32FanPulseInput[AC_PWM_FREQUENCY][0] <= AC_PWM_IS_OUTPUT)
	{
		if((gEvChargeInfo.u8AC_PWM_SampInterval >= 10) && (u8Is1000VHardwareA00()))
		{
			gEvChargeInfo.u8AC_PWM_SampInterval = 0;
			if(gEvChargeInfo.u8AC_StartCMD == FALSE)
			{
				dv_ADC_3.fADWorkPortSelect(AD_PIN_PF9);	
				dv_ADC_3.fStartMeasure(dv_ADC_3.u32OneLoopValue, mADC3_ENERGYPOINTNUM);
			}
		}
	}
	else
	{

	}

	if(u8Is1000VHardwareA00())
	{
		gADCC2Val = (((2.5*(double)fAC_PWM_VOLTAGE_QuickVal)/(float)4096) * 6.00);
	}
	else
	{
		gADCC2Val = (((2.5*(double)fAC_PWM_VOLTAGE_QuickVal)/(float)4096) * 6.00) * 2;
	}
	
	gADCC2Val = f_AC_MidArry((float)gADCC2Val);
	if (gADCC2Val >= 4)
	{
		gEvChargeInfo.fCC1_ACPWMVoltVal = gADCC2Val;		   //PWM VOLT
	}

	if (AC_CHARGE_IS_C_STATUS)
	{
		if(gEvChargeInfo.u8AC_B_EnterFlag == TRUE)
		{
			gEvChargeInfo.u32PWMLoadValue = dv_Capture_PWM.Rate[AC_PWM]; 
		}
		else
		{
			gEvChargeInfo.u32PWMLoadValue = u32GetCalcPulseRate();
		}
	}
}

//void AC_ChargeDeal(void)
//{
//	//static int siDelay = 0;
//
//	if ((TRUE == u8CAN_1_Is_GBT()) && (TRUE == u8CAN_2_Is_GBT()))
//	{
//		return;//˫ǹGBT��������AC GUN!!
//	}
//
//
//	//TEST!!
//	//gEvChargeInfo.u8AC_StartCMD = TRUE;
///*******************************TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!************************/
//	//AC_PWM_OUT();
//	//OSTimeDly(20);	//��ҪѸ�ٸı䣬̫���˲����ɼ�����
//	//return;
///*******************************TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!//TEST!!************************/
//
//
//	//gEvChargeInfo.fCC1_ACPWMVoltVal = 6;
//	if (gEvChargeInfo.u8HasACGun == FALSE)
//	{
//		AC_PWM_STOP();
//		return;
//	}
//
//	AC_PWM_Pulse_Voltage_Deal();
//
//	if (u8AC_SocketDeal())
//	{
//		return;
//	}
//
//	if (gEvChargeInfo.u8AC_StartCMD == TRUE)
//	{
//		AC_B_StateDeal();
//
//		AC_C_StateDeal();
//
//		AC_A_StateDeal();
//
//		u8ACAlarmDeal();	//ͣ����
//	}
//	else
//	{
//		gEvChargeInfo.u32PWMLoadValue = 0;
//		gEvChargeInfo.u16AC_SocketFeedBackTimes = 0;
//		dv_Adc.I32TimerCount = 0;
//		gEvChargeInfo.u8AC_B_EnterFlag = FALSE;
//		gEvChargeInfo.u8AC_C_EnterFlag = FALSE;
//		gEvChargeInfo.u16AC_SocketFeedBackDelay = 0;
//		//iAddCompensation = 0;
//		gEvChargeInfo.iAC_AddCompensation = 0;
//		//gEvChargeInfo.iACFinishCheckCount = 0;
//		gEvChargeInfo.iACStartTimeOutCount = AC_START_CLEAN_COUNT;
//		//siDelay = 0;
//		AC_PWM_STOP();
//		__AC_CHARGE_LED_OFF();
//		__AC_GUN_POWER_OFF();
//		//fACPWMVoltCalc();
//	}
//}

//void KEY_ChargeDeal(void);
//void KEY_ChargeDeal(void)
//{
//	__XXX_UNION_VALUE unTempVal;
//	ExGetSigInfo(E_TYPE_SYS, ID_KEY_BOARD_CHARGE_ENBL, &unTempVal, NULL, BY_SRAM);
//	//ֻ�ǲɼ�DI �ϱ���IMMU��Ȼ����д���?
//}

//UINT32 u32CaptureBuff[200];


void V2F_Capture_TimerCallBack(void)
{
	//int iArryIdx = 0;
	gEvChargeInfo.u32CaptureTimerCount++;
	if (gEvChargeInfo.u32CaptureTimerCount >= PER_CYCL_TIME)
	{
		dv_Capture_PWM.fGetPulseFrequency(BUS_VOLT_2_SQC_NO);
		dv_Capture_PWM.fGetPulseFrequency(BUS_VOLT_1_SQC_NO);
		gEvChargeInfo.u32CaptureTimerCount = 0;
		dv_Capture_PWM.fGetPulseFrequency(AC_PWM_FREQUENCY);
	}
	
}

UINT8 g_u8IMMU_LCCU_COMM_Addr = 0;

void RefreshSelfAddress(void)
{
	UCHAR uctemp;

	UCHAR SWAddress[4] = {0};
	static UCHAR Addressback = 0xff;

	if(gMeasureInfo.ucAddressDelay <3)
	{
		return;
	}

	SWAddress[0] = (BSP_u8GetSW1Status() == GPIO_PIN_SET) ? 0 : 0x01;
	SWAddress[1] = (BSP_u8GetSW2Status() == GPIO_PIN_SET) ? 0 : 0x02;
	SWAddress[2] = (BSP_u8GetSW3Status() == GPIO_PIN_SET) ? 0 : 0x04;
	SWAddress[3] = (BSP_u8GetSW4Status() == GPIO_PIN_SET) ? 0 : 0x08;

	uctemp = (SWAddress[0]|SWAddress[1]|SWAddress[2]|SWAddress[3]);

	//??????
	if(Addressback != uctemp)
	{
		Addressback = uctemp;
		return;
	}

	gMeasureInfo.u8SwValue = uctemp;

	uctemp += 1;

//??????????????????????????


	ZHIXIN_CLIENT_PORT = 6690;

	if(gEvChargeInfo.u8NET_Mask == TRUE)
	{
		//Addressback = 0;
		g_u8IMMU_LCCU_COMM_Addr = uctemp;
	}
	else if(g_u8IMMU_LCCU_COMM_Addr != uctemp)
	{
		g_u8IMMU_LCCU_COMM_Addr = uctemp;
		
		gEvChargeInfo.u8PowerStarIP[3] = 200 + uctemp;
		gNET_TCP_Info.iNetCreateSocketFlag = TRUE;
		g_RingAllInfo.bIsRingCfgErr = TRUE;
	}


}


static void RefreshMeasuerTime(void)
{
	struct tm  tmNow;
	static UCHAR  Sec = 0;
	gRTCData.fGetLocalTime(&tmNow);
	if((UCHAR)tmNow.tm_sec != Sec)/*1����*/
	{
		Sec = (UCHAR)tmNow.tm_sec;
		if(gMeasureInfo.ucAddressDelay <255)
		{
			gMeasureInfo.ucAddressDelay++;
		}
		
	}
}

/********��������:����ģ�����Ϳ����� ****/
void TASK_MeasureProc (void* pdata)
{
	pdata = pdata;
	gEvChargeInfo.fEV_BattVolt_x[GUN_NO_A] = 0;
	gEvChargeInfo.fEV_BattVolt_x[GUN_NO_B] = 0;
	gEvChargeInfo.fCC1_ACPWMVoltVal = 12;
	gEvChargeInfo.u32PWMLoadValue = 0;
	ACFILTER_IDX = 0;
	memset(AC_DMAValue,0,mFIELT_NUM);
	gEvChargeInfo.u8AC_StopReason = 0;
	gEvChargeInfo.u8AC_StartCMD = FALSE;
	UINT8 utempVal = 0;
	UNUSED(utempVal);
	//AC_PWM_STOP();  //dv_Capture_PWM.fOutputFanDriver(AC_PWM, 110*14)
	OSTimeDly(1000);

	for(;;)
	{
		
		gSelfTest.fSetTaskID(TASK_ID_MEASURE);
		if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
		{
			OSTimeDly(100);
			continue;
		}
		RefreshMeasuerTime();

		OSTimeDly(2);
		RefreshDI(3);
		RefreshSelfAddress();
		GetVFAnalog();
		//AC_ChargeDeal();
		//BillingData();
		//KEY_ChargeDeal();
		//GUNOverTempDelayDeal();
	}
}
