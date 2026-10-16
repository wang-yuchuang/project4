#include "main.h"
#include "can_ring.h"
#include "Uart6_new.h"
#include "HistoryEvent.h"
#include "ABU.h"






//20211102 lkf 1KV22KW模块，涉及模块内部串并联模式切换，因电池电压最低不于520V，所以工作电压不能低于520V
#define BEG_1K_SW_VOLT	520.000001

static UINT8 s_u8MpptGrp0NumBuf;
static float fChargePower,fDisChargePower,TemCurr;
NOINIT UpgradePowerModule gs_UpgradePowerModuleCtrl;
CANCommData_TypeDef gs_CANComm;
canmsg TxCAN_3Buffer;
UINT8 TxCAN3Buff(canmsg * pRcvMsg, UINT8 u8Len);
UINT8 RxCAN3Buff(UCHAR u8Num, CanRxMsgTypeDef* pRcvMsg);



UINT32 u32Mask_1 = 0x0380FF00;
UINT32 u32Mask_2 = 0x0380E000;
UINT32 u32RcvID_1 = 0x0280F000;
UINT32 u32RcvID_2 = 0x03001010;
UCHAR	DSTADDR = 0;
float fgAVolt[mRECTNUM];
float fgBVolt[mRECTNUM];


UINT8 s_u8FirstGetFlag_MPPT = 3;


#define FRQ_1_QUICK			1
#define FRQ_10_TIMES		10
#define FRQ_20_TIMES		20
#define FRQ_50_TIMES		50

#define R_PRPTO_DEVNO_0A	0x0A
#define R_PRPTO_DEVNO_0B	0x0B





#define R_PROTO_BROADCAST		0x3F
//#define R_PROTO_SELF_ADDR		0xF0 //监控自己的地址/**/
#define R_PROTO_SELF_ADDR		gs_CANComm.bySelfAddress

//模块回复的错误码定义
typedef enum tagRECT_RET_ERR_Code
{
	RECT_RET_ERR_Code_NO_ERR = 0x00,
	RECT_RET_ERR_Code_WRONG_ADDR = 0x01,
	RECT_RET_ERR_Code_WRONG_CMD = 0x02,
	RECT_RET_ERR_Code_WRONG_DATA = 0x03,
	RECT_RET_ERR_Code_IN_STARTING = 0x07, //地址重排未完成/**/
}RECT_RET_ERR_Code;


//NOINIT UINT8 u8Can3_S_Buff[CAN_FRAME_SIZE];
CanRxMsgTypeDef RxMsg[CAN_3_MAX_BUFF];
static float s_fGrpMinVoltBuf;
static float s_fGrpMaxVoltBuf;
static float s_fGrpRateCurrBuf;
static float s_fGrpRatePowerBuf;

static float s_fGrpExtVolt;
static float s_fGrpAvailableCurr;
static float s_fGrpAvailablePwr;

static float s_fConvertOutCurrBuf;
static float s_fConvertOutVoltBuf;

void CAN3_GetRectCtlVoltCurr(UCHAR  i, float* fVal1, float* fVal2);
UINT8 CAN3_GetRectCtlStatus(UCHAR i);
void CAN3_SetRectCtlVoltCurr(UCHAR i, float fVal1, float fVal2);
UCHAR CAN3_MutexInit(void);
void CAN3_SetRectCtlStatus(UCHAR i,UINT8 u8OnOff);
void CAN3_InitRectData(UCHAR isFirstFlag);

void NoSampDeal(void);
float String2Float(UINT8* pData);
int CAN3_Send(canmsg *pMsg, UINT8 DataL);
void Init_CmdInfos(void);
void ClenRBuff(void);
UINT8 u8PackSendMsg(UINT8 u8DevNo,UINT8 u8Cmd, UINT8 u8GSNo,UINT32 Dh,UINT32 Dl);
UINT8 u8PackSendMsg_MPPT(UINT8 u8DevNo,UINT8 u8Cmd, UINT8 u8GSNo,UINT32 Dh,UINT32 Dl);
/*UINT8 u8RcvMsg(UINT16 u8TimeOutMS);*/
UINT8 UnPackData(UINT8 u8RcvMsgNum, UINT8 u8DevNo,UINT8 u8Cmd, UINT8 u8GSNo, CANCommData_TypeDef* PData);

ERR_CODE_CAN_R GetSingle_VoltCurr(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetSingle_GrpAndStatus(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetSingle_PhaseVolts(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetSingle_VerInfo(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetSingle_VerInfo_SingleModule(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R SetSingle_OnOff(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R SetSingle_LedFlash(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R SetSingle_Reset(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetRECT_TEM_0X23(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetRECT_MAXTEM_0X23(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetMPPT_DCTEM_0X0E(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetMPPT_PFCTEM_0X0E(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetMPPT_DC_TRANS_TEM_0X0E(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetMPPT_PFC_TRANS_TEM_0X0E(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetRECT_bStatFaltCode1110_PCS_0X23(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetRECT_bStatFaltCode1111_PCS_0X23(int SGNo, void* pRawData,void* pSetData);


ERR_CODE_CAN_R GetGrp_VoltCurr(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetGrp_NumAndStatus(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R SetGrp_OnOff_No10(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R SetGrp_WalkIn(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R SetGrp_AdjVoltCurr_No13(int SGNo, void* pRawData,void* pSetData);
ERR_CODE_CAN_R GetGrp_MaxMinVoltCurr(int SGNo, void* pRawData,void* pSetData);
void TrigAutoAllowcation(void);
void DealRawCanMsgToIMMU2(canmsg * pRcvMsg, UINT8 u8GSNo, UINT8 IsSend);

extern void TaskConverterOutAction(void);
extern void SendRectSillenceModeBroadcast(BYTE byRectAddress, UINT8 u8Data);
extern UINT8 u8_U1_UpDataProc(void);
extern UINT8 u8_U2_UpDataProc(void);
extern UINT8 u8_ABU_U1_UpDataProc(void);
extern UINT8 u8_ABU_U2_UpDataProc(void);
extern UINT8 u8_MPPT_U1_UpDataProc(void);
extern UINT8 u8_MPPT_U2_UpDataProc(void);




/*************************************************************************************************************
 * @brief 初始化CAN3模块的数据。
 * 
 * 该函数用于初始化CAN3模块的各种参数，包括电压、电流、工作模式等。
 * 在首次调用时，会重置所有相关的状态和参数，确保模块处于初始状态。
 * 
 * @param isFirstFlag 指示是否为首次初始化的标志，TRUE表示首次初始化，FALSE表示重配置。
 *************************************************************************************************************/

void CAN3_InitRectData(UCHAR isFirstFlag)                 //初始化模块数据
{
	UINT8 k, u8Idx = 0;
	int i = 0;
	//gEvChargeInfo.fMaxOutCurr = 240;
	//gEvChargeInfo.fMaxOutVolt = 750;
	s_fGrpMinVoltBuf = 50;
	s_fGrpMaxVoltBuf = 50;
	s_fGrpRateCurrBuf = 0;
	s_fGrpRatePowerBuf = 0;

	//LKF 只初始化时执行一次，以后重配置时不能再执行
	if(isFirstFlag)
	{
		gs_CANComm.u8IsACOver = FALSE;       
		gs_CANComm.u8IsACUnder = FALSE;
		gs_CANComm.u8IsAllCommFail = FALSE;
		gs_CANComm.u16AllCommFailResetDelay = 0;
		gs_CANComm.u16RectCommPeriodResetDelay = 0;
		for(i = 0;i < _MAX_GRP_NUM; i++)                    //遍历最大组数 _MAX_GRP_NUM，将模块的开关状态初始化为 RECT_OFF（关闭状态）。
		{
			gs_CANComm.u8GRP_RectOnOff[i] = RECT_OFF;
			gs_CANComm.fRect_GRP_NeedOutVolt[i] = 100;
			gs_CANComm.fRect_GRP_NeedOutCurr[i] = 1;
			gs_CANComm.u8GRP_WorkModeSet[i] = 0;
			gs_CANComm.u8GRP_SerialParallelModeSet[i] = 0;
			gs_CANComm.u8GRP_SerialParallelModeSet_Back[i] = 0;
			gs_CANComm.u8GRP_SerialParallelModeSet_Flag[i] = 0;
			gs_CANComm.u8GRP_IsSupportSerialParallelModeSet[i] = 0;
		}
		//gs_CANComm.u8G1_RectOnOff = RECT_OFF;
		//gs_CANComm.u8G2_RectOnOff = RECT_OFF;
		//gs_CANComm.fRect_1_NeedOutVolt = 100;
		//gs_CANComm.fRect_1_NeedOutCurr = 1;
		//gs_CANComm.fRect_2_NeedOutVolt = 100;
		//gs_CANComm.fRect_2_NeedOutCurr = 1;
	}

	for (u8Idx= 0; u8Idx < mRECTNUM; u8Idx++)                       //初始化各个模块的电压、电流、工作模式、串并联模式等参数。
	{		
		gs_CANComm.AddrInfo[u8Idx] = u8Idx;                             //模块地址初始化
		gs_CANComm.iCount[u8Idx] = 0;                                 
		gs_CANComm.RectData[u8Idx].bIsExisted = FALSE;                      
		gs_CANComm.RectData[u8Idx].nBelongedGrp = 0xff;                      
		gs_CANComm.RectData[u8Idx].bCommFail = TRUE;
		gs_CANComm.RectData[u8Idx].nCommFailTimes = 0;
		gs_CANComm.RectData[u8Idx].fRealTime_Volt = 0;
		gs_CANComm.RectData[u8Idx].fRealTime_Curr = 0;
		gs_CANComm.RectData[u8Idx].fExtVolt = 0;
		gs_CANComm.RectData[u8Idx].fAvailableCurr = 0;
		gs_CANComm.RectData[u8Idx].fRealTime_Temp = 0;
		gs_CANComm.RectData[u8Idx].fDC_Ver = 0;
		gs_CANComm.RectData[u8Idx].fAC_Ver = 0;
		gs_CANComm.RectData[u8Idx].fHW_Ver = 0;
		gs_CANComm.RectData[u8Idx].fMax_OutVolt = 0;
		gs_CANComm.RectData[u8Idx].fMin_OutVolt = 0;
//		gs_CANComm.RectData[u8Idx].fMax_OutCurr= 100; 
		gs_CANComm.RectData[u8Idx].fMax_OutCurr= 133;            //by lxr01202   初始化最大输出电流为MPPT最大的输出电流
		gs_CANComm.RectData[u8Idx].fRatedPower= 0;
		gs_CANComm.RectData[u8Idx].bAC_Off = 0;
		gs_CANComm.RectData[u8Idx].bAC_OverVolt = 0;
		gs_CANComm.RectData[u8Idx].bDC_LowVolt = 0;
		gs_CANComm.RectData[u8Idx].bAC_NoBalance = 0;
		gs_CANComm.RectData[u8Idx].bAC_PhaseLack = 0;
		gs_CANComm.RectData[u8Idx].bCurr_NoBalace = 0;
		gs_CANComm.RectData[u8Idx].bID_Conflict = 0;
		gs_CANComm.RectData[u8Idx].bPowerLimited = 0;
		//gs_CANComm.RectData[u8Idx].bRectCommFail = 0;
		gs_CANComm.RectData[u8Idx].bWalkIn_Enabled = 0;
		gs_CANComm.RectData[u8Idx].bDC_OverVolt = 0;
		gs_CANComm.RectData[u8Idx].bOverTemp = 0;
		gs_CANComm.RectData[u8Idx].bFanFault = 0;
		gs_CANComm.RectData[u8Idx].bProtected = 0;
		gs_CANComm.RectData[u8Idx].bRectFail = 0;
		gs_CANComm.RectData[u8Idx].bDC_Off = 0;
		gs_CANComm.RectData[u8Idx].bDisCh_Err = 0;
		gs_CANComm.RectData[u8Idx].bOn_Sleep = 0;
		gs_CANComm.RectData[u8Idx].bPFC_Err = 0;
		gs_CANComm.RectData[u8Idx].bInternalCommErr = 0;
		gs_CANComm.RectData[u8Idx].bInternalDC_LowVolt = 0;
		gs_CANComm.RectData[u8Idx].bDC_Shorted = 0;
		gs_CANComm.RectData[u8Idx].nCommFailTimes = 0;
		gs_CANComm.RectData[u8Idx].fAB_Volt = 411;
		gs_CANComm.RectData[u8Idx].fBC_Volt = 412;
		gs_CANComm.RectData[u8Idx].fCA_Volt = 413;
		
		gs_CANComm.RectData[u8Idx].bStatCode[0] = 0;
		gs_CANComm.RectData[u8Idx].bStatCode[1] = 0;
		gs_CANComm.RectData[u8Idx].bStatCode[2] = 0;
		gs_CANComm.RectData[u8Idx].bStatCode[3] = 0;
		gs_CANComm.RectData[u8Idx].fOutSiteMaxCurr = 0;
		gs_CANComm.RectData[u8Idx].fOutSiteVolt = 0;
		gs_CANComm.RectData[u8Idx].fDC_Ver = 0;
		gs_CANComm.RectData[u8Idx].fAC_Ver = 0;
		gs_CANComm.RectData[u8Idx].u32Rect_Tem = 0;
		gs_CANComm.RectData[u8Idx].u32Rect_Tem_MAX = 0;
		gs_CANComm.RectData[u8Idx].bConvertStat = 0;
		gs_CANComm.RectData[u8Idx].u32APhase_Volt = 0;
		gs_CANComm.RectData[u8Idx].u32BPhase_Volt = 0;
		gs_CANComm.RectData[u8Idx].u32CPhase_Volt = 0;
		gs_CANComm.RectData[u8Idx].i32APhase_Curr = 0;
		gs_CANComm.RectData[u8Idx].i32BPhase_Curr = 0;
		gs_CANComm.RectData[u8Idx].i32CPhase_Curr = 0;
		gs_CANComm.RectData[u8Idx].fActivePower = 0;
		gs_CANComm.RectData[u8Idx].fHz = 0;

		gs_CANComm.RectData[u8Idx].fRealTime_Volt_BattSide = 0;
		gs_CANComm.RectData[u8Idx].fRealTime_Curr_BattSide = 0;

		gs_CANComm.RectData[u8Idx].bPCSWorkStatus = 0;
		gs_CANComm.RectData[u8Idx].bPCSWorkMode = 0;

		for(k = 0; k < 4; k++)
		{
			gs_CANComm.RectData[u8Idx].bBarcode_24[k] = 0;
			gs_CANComm.RectData[u8Idx].bBarcode_25[k] = 0;
			gs_CANComm.RectData[u8Idx].bBarcode_26[k] = 0;
			gs_CANComm.RectData[u8Idx].bBarcode_27[k] = 0;
			gs_CANComm.RectData[u8Idx].bBarcode_28[k] = 0;
			gs_CANComm.RectData[u8Idx].bBarcode_29[k] = 0;
		}

		for(k = 0; k < 4; k++)
		{
			gs_CANComm.RectData[u8Idx].bStatCode_Invert[k] = 0;	
			gs_CANComm.RectData[u8Idx].bDCFaltCode_PCS[k] = 0;	
			gs_CANComm.RectData[u8Idx].bStatFaltCode_PCS[k] = 0;
			gs_CANComm.RectData[u8Idx].bStatFaltCode_PCS[4+k] = 0;
			gs_CANComm.RectData[u8Idx].bStatFaltCode_PCS_35KW[k] = 0;
			gs_CANComm.RectData[u8Idx].bStatFaltCode_PCS_35KW[4+k] = 0;
		}
	}

	if(isFirstFlag)
	{
		for (u8Idx= 0; u8Idx < mMPPTNUM; u8Idx++)                  //初始化各个MPPT的电压、电流、工作模式、串并联模式等参数。
		{		
			gs_CANComm.AddrInfo_Mppt[u8Idx] = u8Idx;
			gs_CANComm.iCount_Mppt[u8Idx] = 0;
			gs_CANComm.MpptData[u8Idx].bIsExisted = FALSE;
			gs_CANComm.MpptData[u8Idx].nBelongedGrp = 0xff;
			gs_CANComm.MpptData[u8Idx].bCommFail = TRUE;
			gs_CANComm.MpptData[u8Idx].nCommFailTimes = 0;
			gs_CANComm.MpptData[u8Idx].fRealTime_Volt = 0;
			gs_CANComm.MpptData[u8Idx].fRealTime_Curr = 0;
			gs_CANComm.MpptData[u8Idx].fExtVolt = 0;

			gs_CANComm.MpptData[u8Idx].fRealTime_Volt_BattSide = 0;
			gs_CANComm.MpptData[u8Idx].fRealTime_Curr_BattSide = 0;

			gs_CANComm.MpptData[u8Idx].fAvailableCurr = 0;
			gs_CANComm.MpptData[u8Idx].fRealTime_Temp = 0;
			gs_CANComm.MpptData[u8Idx].fDC_Ver = 0;
			gs_CANComm.MpptData[u8Idx].fAC_Ver = 0;
			gs_CANComm.MpptData[u8Idx].fHW_Ver = 0;
			gs_CANComm.MpptData[u8Idx].fMax_OutVolt = 0;
			gs_CANComm.MpptData[u8Idx].fMin_OutVolt = 0;
			gs_CANComm.MpptData[u8Idx].fMax_OutCurr= 0;
			gs_CANComm.MpptData[u8Idx].fRatedPower= 0;
			gs_CANComm.MpptData[u8Idx].bAC_Off = 0;
			gs_CANComm.MpptData[u8Idx].bAC_OverVolt = 0;
			gs_CANComm.MpptData[u8Idx].bDC_LowVolt = 0;
			gs_CANComm.MpptData[u8Idx].bAC_NoBalance = 0;
			gs_CANComm.MpptData[u8Idx].bAC_PhaseLack = 0;
			gs_CANComm.MpptData[u8Idx].bCurr_NoBalace = 0;
			gs_CANComm.MpptData[u8Idx].bID_Conflict = 0;
			gs_CANComm.MpptData[u8Idx].bPowerLimited = 0;
			//gs_CANComm.MpptData[u8Idx].bRectCommFail = 0;
			gs_CANComm.MpptData[u8Idx].bWalkIn_Enabled = 1;
			gs_CANComm.MpptData[u8Idx].bDC_OverVolt = 0;
			gs_CANComm.MpptData[u8Idx].bOverTemp = 0;
			gs_CANComm.MpptData[u8Idx].bFanFault = 0;
			gs_CANComm.MpptData[u8Idx].bProtected = 0;
			gs_CANComm.MpptData[u8Idx].bRectFail = 0;
			gs_CANComm.MpptData[u8Idx].bDC_Off = 0;
			gs_CANComm.MpptData[u8Idx].bDisCh_Err = 0;
			gs_CANComm.MpptData[u8Idx].bOn_Sleep = 0;
			gs_CANComm.MpptData[u8Idx].bPFC_Err = 0;
			gs_CANComm.MpptData[u8Idx].bInternalCommErr = 0;
			gs_CANComm.MpptData[u8Idx].bInternalDC_LowVolt = 0;
			gs_CANComm.MpptData[u8Idx].bDC_Shorted = 0;
			gs_CANComm.MpptData[u8Idx].nCommFailTimes = 0;
			gs_CANComm.MpptData[u8Idx].fAB_Volt = 411;
			gs_CANComm.MpptData[u8Idx].fBC_Volt = 412;
			gs_CANComm.MpptData[u8Idx].fCA_Volt = 413;
			
			gs_CANComm.MpptData[u8Idx].bStatCode[0] = 0;
			gs_CANComm.MpptData[u8Idx].bStatCode[1] = 0;
			gs_CANComm.MpptData[u8Idx].bStatCode[2] = 0;
			gs_CANComm.MpptData[u8Idx].bStatCode[3] = 0;


			gs_CANComm.MpptData[u8Idx].bConvertStat = 0;
			/*gs_CANComm.MpptData[u8Idx].fA_Volt = 0;
			gs_CANComm.MpptData[u8Idx].fB_Volt = 0;
			gs_CANComm.MpptData[u8Idx].fC_Volt = 0;
			gs_CANComm.MpptData[u8Idx].fA_Curr = 0;
			gs_CANComm.MpptData[u8Idx].fB_Curr = 0;
			gs_CANComm.MpptData[u8Idx].fC_Curr = 0;*/
			gs_CANComm.MpptData[u8Idx].fActivePower = 0;
			gs_CANComm.MpptData[u8Idx].fHz = 0;

			gs_CANComm.RectData[u8Idx].bPCSWorkStatus = 0;
			gs_CANComm.RectData[u8Idx].bPCSWorkMode = 0;

			for(k = 0; k < 4; k++)
			{
				gs_CANComm.MpptData[u8Idx].bBarcode_24[k] = 0;
				gs_CANComm.MpptData[u8Idx].bBarcode_25[k] = 0;
				gs_CANComm.MpptData[u8Idx].bBarcode_26[k] = 0;
				gs_CANComm.MpptData[u8Idx].bBarcode_27[k] = 0;
				gs_CANComm.MpptData[u8Idx].bBarcode_28[k] = 0;
				gs_CANComm.MpptData[u8Idx].bBarcode_29[k] = 0;
			}
			for(k = 0; k < 4; k++)
			{
				gs_CANComm.MpptData[u8Idx].bStatCode_Invert[k] = 0;	
				gs_CANComm.MpptData[u8Idx].bDCFaltCode_PCS[k] = 0;	
				gs_CANComm.MpptData[u8Idx].bStatFaltCode_PCS[k] = 0;
				gs_CANComm.MpptData[u8Idx].bStatFaltCode_PCS[4+k] = 0;
				gs_CANComm.MpptData[u8Idx].bStatFaltCode_PCS_35KW[k] = 0;
				gs_CANComm.MpptData[u8Idx].bStatFaltCode_PCS_35KW[4+k] = 0;
			}
		}
	}
	for (u8Idx= 0; u8Idx < _MAX_GRP_NUM; u8Idx++)                         
	{
		if((isFirstFlag)
			|| (u8Idx != MPPT_GRP_NO))            //MPPT组不初始化
		{
			gs_CANComm.GrpData[u8Idx].nCommRectNum = 0;
			gs_CANComm.GrpData[u8Idx].u8GrpCMDResponeseFlag = TRUE;
			gs_CANComm.GrpData[u8Idx].u8GrpCMDResponeseDelayTimes = 0;
			gs_CANComm.GrpData[u8Idx].bAllRect_Off = TRUE;
			gs_CANComm.GrpData[u8Idx].fOutVolt = 0;
			gs_CANComm.GrpData[u8Idx].fExtVolt = 0;
			gs_CANComm.GrpData[u8Idx].fAvailableCurr = 0;
			gs_CANComm.GrpData[u8Idx].fAvailablePwr = 0;
			gs_CANComm.GrpData[u8Idx].fOutCurr = 0;

			gs_CANComm.GrpData[u8Idx].fRealTime_Volt_BattSide = 0;
			gs_CANComm.GrpData[u8Idx].fRealTime_Curr_BattSide = 0;

			gs_CANComm.GrpData[u8Idx].bAllRectProtected = 0;
			gs_CANComm.GrpData[u8Idx].bAllRectFailed = 0;
			gs_CANComm.GrpData[u8Idx].bAllRect_Off = 0;		//0 系统关机
			gs_CANComm.GrpData[u8Idx].fMax_OutVolt = 770;
			gs_CANComm.GrpData[u8Idx].fMin_OutVolt = 100;
			gs_CANComm.GrpData[u8Idx].fMax_OutCurr = 100;
			gs_CANComm.GrpData[u8Idx].fRatedMaxCurr = 0;
			//gs_CANComm.GrpData[u8Idx].fMax_OutCurr_ring = 240;
			gs_CANComm.GrpData[u8Idx].f_Power_ofThisVoltMax = 0;
			gs_CANComm.GrpData[u8Idx].u8PowerofThisVoltMaxIsValidFlag = 0;
			gs_CANComm.GrpData[u8Idx].fOutSiteMaxVolt = 0;
			gs_CANComm.GrpData[u8Idx].fOutSiteMaxCurr = 0;
			gs_CANComm.GrpData[u8Idx].fRatedPower = 0;

			gs_CANComm.GrpData[u8Idx].bIsAnyRectAlarm = 0;

			gs_CANComm.GrpData[u8Idx].u8ConvertStat_ForwardNum = 0;
			gs_CANComm.GrpData[u8Idx].u8ConvertStat_ReverseNum = 0;
			gs_CANComm.GrpData[u8Idx].u8ConvertStat_ProtectNum = 0; 
			gs_CANComm.GrpData[u8Idx].bIsAllDCOutClose = FALSE;

			gs_CANComm.GrpData[u8Idx].fDCInputVolt = 0;
			gs_CANComm.GrpData[u8Idx].fDCInputCurr = 0;

			gs_CANComm.GrpData[u8Idx].nValidRectNum = 0; 
			gs_CANComm.GrpData[u8Idx].nExistRectNum = 0; 

			gs_CANComm.GrpData[u8Idx].TotalActivePower = 0;
			gs_CANComm.GrpData[u8Idx].TotalReactivePower = 0;
		}

		//LKF 只初始化时执行一次，以后重配置时不能再执行
		if(isFirstFlag)
		{
			gs_CANComm.GrpData[u8Idx].u8IsNoNormalRect = FALSE;
			gs_CANComm.GrpData[u8Idx].u8AllCommFails = FALSE;
			gs_CANComm.GrpData[u8Idx].u8HaveDCShortAlm = FALSE;
		}
	}

	/*环网*/
	gs_CANComm.nScanedRectNum = 0; //The rect num after Reconfigure done
	gs_CANComm.nCommFailedNum = 0;
	//LKF 只初始化时执行一次，以后重配置时不能再执行
	if(isFirstFlag)
	{
		gs_CANComm.bySelfAddress = 0xF0; //本桩地址/**/
		//gs_CANComm.nTotalRingStub = 0;//所有环网桩数/**/
		gs_CANComm.bIsRingEnabled = 0; //根据配置是否开启环网/**/
		gs_CANComm.bNeedReConfig = TRUE;
	}
	
	gs_CANComm.nOnlineTotalGrp = 0; //實?在?的各?模塊的???必须为0,1,2/**/
	for (u8Idx= 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
	{		
		if((isFirstFlag)
			||(!((u8Idx >= MPPT_GRP_NO_MIN) && (u8Idx <= MPPT_GRP_NO_MAX))))
		{
			gs_CANComm.bIsGrpExist[u8Idx] = 0;//该组存在状态/**/
			gs_CANComm.nNotAccessbleCount[u8Idx] = 0;
			gs_CANComm.bIsGrp_NotAvailable[u8Idx] = 0;
		}

		//LKF 只初始化时执行一次，以后重配置时不能再执行
		if(isFirstFlag)
		{
			gs_CANComm.byGrpIdx[u8Idx] = u8Idx+1;//真实的组号，根据监控地址固定写死1~8/**/
			gs_CANComm.bIsInitClosed[u8Idx] = 0; //是否初始化关闭了该组模块/**/  
		}		
	}	
	/*环网 end*/

	gs_CANComm.bRectTemperatureIsValid = 0;         //模块温度是否有效
	gs_CANComm.fMaxRectTemperature = 25;            //最大模块温度25°
	
}

/*************************************************************************************************************
 * @brief 获取指定模块组的控制电压和电流值。
 *
 * @param i 模块组索引，范围应在 0 到 _MAX_GRP_NUM 之间。
 * @param fVal1 指向存储输出电压值的浮点数指针。
 * @param fVal2 指向存储输出电流值的浮点数指针。
 *
 * 此函数通过互斥锁确保在读取电压和电流值时的线程安全。
 *************************************************************************************************************/
void CAN3_GetRectCtlVoltCurr(UCHAR i, float *fVal1, float *fVal2) // 获取某一组模块的控制的电压电流值
{
	MutexLock(gs_CANComm.hMutexReadWriteCANCommData);

	if (i  < _MAX_GRP_NUM)                        
	{
		*fVal1 = gs_CANComm.fRect_GRP_NeedOutVolt[i];           //模块组i的输出电压值
		*fVal2 = gs_CANComm.fRect_GRP_NeedOutCurr[i];           //模块组i的输出电流值
	}	
	MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);
	return ;
}

/*************************************************************************************************************
 * @brief 获取模块i的开关状态
 * @param i 模块索引号
 * @return UINT8 返回模块组i的开关状态，若索引无效则返回最小组号状态
 * @note 该函数线程安全，内部使用互斥锁保护共享数据
 *************************************************************************************************************/
UINT8 CAN3_GetRectCtlStatus(UCHAR i)                           //得到模块i的状态
{
	MutexLock(gs_CANComm.hMutexReadWriteCANCommData);
	if (i  < _MAX_GRP_NUM)
	{
		MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);
		return gs_CANComm.u8GRP_RectOnOff[i];                     //返回模块组i的开关
	}	
	MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);
	return gs_CANComm.u8GRP_RectOnOff[GRP_NO_MIN];
}




/*************************************************************************************************************
 * @brief 设置整流模块的控制电压和电流值
 * 
 * @param i 模块索引号（0 ~ _MAX_GRP_NUM-1）
 * @param fVal1 需要设置的输出电压值
 * @param fVal2 需要设置的输出电流值
 * @note 该函数会操作共享数据，内部已加互斥锁保护
 *************************************************************************************************************/
void CAN3_SetRectCtlVoltCurr(UCHAR i, float fVal1, float fVal2)      //设置模块i的控制电压电流值
{
	MutexLock(gs_CANComm.hMutexReadWriteCANCommData);
	if (i  < _MAX_GRP_NUM)
	{
		gs_CANComm.fRect_GRP_NeedOutVolt[i] = fVal1;
		gs_CANComm.fRect_GRP_NeedOutCurr[i] = fVal2;
	}	
	MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);
}




UCHAR CAN3_MutexInit(void)                             //初始化互斥锁
{
	gs_CANComm.hMutexReadWriteCANCommData = MutexCreate(UPPER_PRIO_READ_WRITE_CANCOMM_DATA);
	if(gs_CANComm.hMutexReadWriteCANCommData == NULL)
	{
		return FALSE;
	}
	return  TRUE;
}




void CAN3_SetRectCtlStatus(UCHAR i,UINT8 u8OnOff)         //设置模块i的控制开关状态
{
	MutexLock(gs_CANComm.hMutexReadWriteCANCommData);
	if (i  < _MAX_GRP_NUM)
	{
		gs_CANComm.u8GRP_RectOnOff[i] = u8OnOff;          //设置模块组i的开关
	}	
	MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);
}




static UCHAR u8GetRectDigital(UCHAR mode, UCHAR i)         //得到模块i的数字信号
{
	UCHAR  ctemp = 0;

	if(i > mRECTNUM - 1)
	{
		return  0;
	}
	MutexLock(gs_CANComm.hMutexReadWriteCANCommData);


	MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);

	return  ctemp;
}




static UCHAR CAN3_get_rect_barcode(UINT16 nID, void *pDataBuf)              //得到模块的条形码
{
	//UCHAR i;

	if(nID > mRECTNUM - 1)
	{
		return  FALSE;
	}
	else
	{
		return  TRUE;
	}
}




static UCHAR GetRectLocation(UCHAR uclocation)
{
      return 0;
}





static UCHAR CAN3_GetRectID(UCHAR ucShelfNo)                                 //得到模块的ID号
{
	UCHAR ctemp = mDISABLE;
	//GetCharSetData(SETDATA_RectACPhaseEnable_NUM, &ctemp);
	if(ctemp == mDISABLE)
	{										
		return ucShelfNo;
	}
	else
	{
		ctemp = GetRectLocation(ucShelfNo);
		if(ctemp == mRECTNUM)
			return ucShelfNo;
		else
			return ctemp;
	}
}




static float CAN3_GetRectAnalog(UCHAR uGrpNo, UCHAR RectId, UCHAR SigID)         //得到模块组i的模拟量
{
	MutexLock(gs_CANComm.hMutexReadWriteCANCommData);

	if (uGrpNo  < _MAX_GRP_NUM)
	{
		CANCommData_TypeDef* PData = (CANCommData_TypeDef*)(&gs_CANComm);
		switch (SigID)
		{
		case ID_RECT_OUT_VOLT:
			MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);
			return PData->GrpData[uGrpNo].fOutVolt;                    //返回模块组i的输出电压值
			break;
		case ID_RECT_OUT_CURR:
			MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);
			return PData->GrpData[uGrpNo].fOutCurr;                    //返回模块组i的输出电流值
			break;
		default:
			break;
		}
	}
	else//RECT
	{

	}

	MutexUnlock(gs_CANComm.hMutexReadWriteCANCommData);
	return 0;
}





UINT8 RxCAN3Buff(UCHAR u8Num, CanRxMsgTypeDef* pRcvMsg)           //CAN3接收数据
{
	UINT16 uFrameNum = 0;
	u8Num = MIN(u8Num,CAN_3_MAX_BUFF);
	uFrameNum = MCP2515_Read(pRcvMsg, u8Num);
	return uFrameNum;
}
UINT8 g_u8Can3TxProtectDelay = 0;



UINT8 TxCAN3Buff(canmsg * pRcvMsg, UINT8 u8Len)                  //CAN3发送数据
{
	UINT8 uRst = FALSE;

	/*MutexLock(gs_CANComm.hMutexWriteCAN3Port);
	uRst = CAN3_Send(pRcvMsg, u8Len);
	g_u8Can3TxProtectDelay = 2;
	while(g_u8Can3TxProtectDelay > 0);
	MutexUnlock(gs_CANComm.hMutexWriteCAN3Port);*/

	//WriteTxCAN3Buff(pRcvMsg, u8Len);
MutexLock(gs_CANComm.hMutexWriteCAN3Port);
	uRst = CAN3_Send(pRcvMsg, u8Len);
MutexUnlock(gs_CANComm.hMutexWriteCAN3Port);
	return uRst;
}




//如果想删本线程代码，围绕这个结构体删除，别的线程调用的API可以做空函数或自行处理！
void InitgCANComm(void)
{
	Init_CmdInfos();

	gs_CANComm.fTxCAN3Buff = TxCAN3Buff;
	gs_CANComm.fRxCAN3Buff = RxCAN3Buff;
	gs_CANComm.fInitRectData = CAN3_InitRectData;                                    //初始化模块数据
	gs_CANComm.fMutexInit = CAN3_MutexInit;						//互斥锁

	gs_CANComm.fGetRectCtlVoltCurr = CAN3_GetRectCtlVoltCurr;	//得到模块控制电压电流
	gs_CANComm.fSetRectCtlVoltCurr = CAN3_SetRectCtlVoltCurr;	//设置模块控制电压电流
	gs_CANComm.fGetRectCtlStatus = CAN3_GetRectCtlStatus;		//得到模块控制状态
	gs_CANComm.fSetRectCtlStatus = CAN3_SetRectCtlStatus;		//设置模块控制状态
	gs_CANComm.u8GetRectDigital = u8GetRectDigital;             //得到模块数字量
	gs_CANComm.fGetRectAnalog = CAN3_GetRectAnalog;             //得到模块模拟量
	gs_CANComm.fGetRectBarcode = CAN3_get_rect_barcode;         //得到模块条形码
	gs_CANComm.fGetRectIDForShelf = CAN3_GetRectID;             //得到模块ID号
	gs_CANComm.iTotalRectNum = 0;                               //模块总数 = 0

	gs_CANComm.hMutexWriteCAN3Port = MutexCreate(UPPER_PRIO_WRITE_CAN3);	
	if (gs_CANComm.hMutexWriteCAN3Port == NULL)
	{
		gs_CANComm.hMutexWriteCAN3Port = MutexCreate(UPPER_PRIO_WRITE_CAN3);
	}

	ABU->ControlInfo.ControlBitmap.Data = 0u;
	ABU->CommFailed = !FALSE;
	ABU->CommFailedLast = !FALSE;
	ABU->IsGetABUVersion = FALSE;
}


int CAN3_Send(canmsg *pMsg, UINT8 DataL)
{
	return MCP2515_Send_2(pMsg, DataL);
}



void ClenRBuff(void)
{
	for (UINT8 u8Idx = 0; u8Idx < CAN_3_MAX_BUFF; u8Idx++)
	{
		RxMsg[u8Idx].ExtId = 0;
	}
}



UINT8 u8PackSendMsg(UINT8 u8DevNo,UINT8 u8Cmd, UINT8 u8GSNo,UINT32 Dh,UINT32 Dl)     //打包发送数据
{
	UINT8 u8Rst = FALSE;
	UNUSED(u8Rst);
	canmsg SendMsg;
	if (u8GSNo >= mRECTNUM && R_PRPTO_DEVNO_0A == u8DevNo)
	{
		return FALSE;
	}
	if (u8GSNo >= _MAX_GRP_NUM && R_PRPTO_DEVNO_0B == u8DevNo)
	{
		return FALSE;
	}
	if (!(R_PRPTO_DEVNO_0A == u8DevNo || R_PRPTO_DEVNO_0B == u8DevNo))
	{
		return FALSE;
	}
		
	if (u8DevNo == R_PRPTO_DEVNO_0A)
	{
		SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (u8Cmd << 16) + ((gs_CANComm.AddrInfo[u8GSNo]) << 8) + R_PROTO_SELF_ADDR;			//模块地址 0 1 2 3
	}
	else
	{
		SendMsg.Id = (R_PRPTO_DEVNO_0B << 22) + (u8Cmd << 16) + (gs_CANComm.byGrpIdx[u8GSNo] << 8) + R_PROTO_SELF_ADDR;			//组 1 2 
	}
	SendMsg.Data[0] = Dh;
	SendMsg.Data[1] = Dl;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);            
	ClenRBuff();
	u8Rst = gs_CANComm.fTxCAN3Buff(&SendMsg, 8);

	//Deal raw CAN to IMMU2
	DealRawCanMsgToIMMU2(&SendMsg, u8GSNo, TRUE);
	return TRUE;
}


//by lxr01202     MPPT打包发送数据（地址0x80 81 82 83）
UINT8 u8PackSendMsg_MPPT(UINT8 u8DevNo,UINT8 u8Cmd, UINT8 u8GSNo,UINT32 Dh,UINT32 Dl)     //MPPT打包发送数据
{
	UINT8 MPPT_ADRRS[4] = {0x80,0x81,0x82,0x83};          //MPPT地址   只在这个函数调用    后续根据模块地址直接改这个数组
	UINT8 MPPT_dest = MPPT_ADRRS[u8GSNo];
	UINT8 u8Rst = FALSE;
	UNUSED(u8Rst);
	canmsg SendMsg;
	if (u8GSNo >= mRECTNUM && R_PRPTO_DEVNO_0A == u8DevNo)
	{
		return FALSE;
	}
	if (u8GSNo >= _MAX_GRP_NUM && R_PRPTO_DEVNO_0B == u8DevNo)
	{
		return FALSE;
	}
	if (!(R_PRPTO_DEVNO_0A == u8DevNo || R_PRPTO_DEVNO_0B == u8DevNo))
	{
		return FALSE;
	}
		
	if (u8DevNo == R_PRPTO_DEVNO_0A)
	{
		SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (u8Cmd << 16) + (MPPT_dest << 8) + R_PROTO_SELF_ADDR;			//模块地址 0 1 2 3
	}
	else
	{
		SendMsg.Id = (R_PRPTO_DEVNO_0B << 22) + (u8Cmd << 16) + (gs_CANComm.byGrpIdx[u8GSNo] << 8) + R_PROTO_SELF_ADDR;			//组 1 2 
	}
	SendMsg.Data[0] = Dh;
	SendMsg.Data[1] = Dl;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);            
	ClenRBuff();
	u8Rst = gs_CANComm.fTxCAN3Buff(&SendMsg, 8);

	//Deal raw CAN to IMMU2
	DealRawCanMsgToIMMU2(&SendMsg, u8GSNo, TRUE);
	return TRUE;
}

void SendAddressingCmd(void)
{
	canmsg SendMsg;

	SendMsg.Id = 0x757f803;

	SendMsg.Data[0] = 0x04000001;
	SendMsg.Data[1] = 0x00;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);              //接收CAN3数据
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);                       //发送CAN3数据
	OSTimeDly(3);
}




//20190618 等待固定时钟节拍
UINT8 u8RcvMsg_WaitTicks(UINT16 u8TimeOutMS)
{
	UINT8 u8Rst = 0;
	//UINT16 u16Idx = 0;
	u8TimeOutMS = MAX(3,u8TimeOutMS);
	//for (u16Idx = 0; u16Idx < u8TimeOutMS/6; u16Idx++)
	//{
	OSTimeDly(u8TimeOutMS);
	u8Rst = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	//if (u8Rst > 0)
	//{
	//	return u8Rst;
	//}
	//}
	return u8Rst;
}



UINT8 UnPackData(UINT8 u8RcvMsgNum, UINT8 u8DevNo,UINT8 u8Cmd, UINT8 u8GSNo, CANCommData_TypeDef* PData)                //解包接收数据
{
	UINT8 u8RcvSrcAddr,u8RcvCmd, u8RcvDevNo,u8RcvErrCode, u8RcvDstAddr, u8Rtn;
	UINT8 u8Idx = 0;
	UINT8 k, u8Found;
	UINT16 u16temp;
	float  fMax_OutCurr, fRatedPower, ftemp;
	union 
	{
		UINT32 u32buf;
		INT32  i32buf;
	}union_temp;
	union
	{
		unsigned char u8temp;
		signed char i8temp;
	}union_u8temp;
	union
	{
		unsigned char uctemp[4];
		UINT32 u32temp;
	}uniontemp;
	RECT_RAW_DATA *pSingleRectData;
	co_dword dword;

	UINT8 u8Buf[8];
	/*if (u8GSNo >= mRECTNUM && R_PRPTO_DEVNO_0A == u8DevNo)
	{
		return FALSE;
	}
	if (u8GSNo >= _MAX_GRP_NUM && R_PRPTO_DEVNO_0B == u8DevNo)
	{
		return FALSE;
	}
	if (!(R_PRPTO_DEVNO_0A == u8DevNo || R_PRPTO_DEVNO_0B == u8DevNo))
	{
		return FALSE;
	}*/
	if(u8RcvMsgNum > CAN_3_MAX_BUFF)
	{
		return ERR_CAN_R_COMM_FAIL;
	}

	u8Rtn = ERR_CAN_R_COMM_FAIL;

	for (u8Idx = 0; u8Idx < u8RcvMsgNum; u8Idx++)
	{
		u8RcvErrCode = ((RxMsg[u8Idx].ExtId>>26)&0x07);	//错误码
		u8RcvCmd = ((RxMsg[u8Idx].ExtId>>16)&0x3f);		//命令号
		u8RcvDevNo = ((RxMsg[u8Idx].ExtId>>22)&0x0f);	//设备号
		u8RcvDstAddr = ((RxMsg[u8Idx].ExtId>>8)&0xff);	//目标地址
		u8RcvSrcAddr = (RxMsg[u8Idx].ExtId & 0xff);		//源地址

		//Deal raw CAN to IMMU2
		canmsg SendMsg;
		SendMsg.Id = RxMsg[u8Idx].ExtId;
		uniontemp.uctemp[0] = RxMsg[u8Idx].Data[0];
		uniontemp.uctemp[1] = RxMsg[u8Idx].Data[1];
		uniontemp.uctemp[2] = RxMsg[u8Idx].Data[2];
		uniontemp.uctemp[3] = RxMsg[u8Idx].Data[3];
		SendMsg.Data[0] = uniontemp.u32temp;
		uniontemp.uctemp[0] = RxMsg[u8Idx].Data[4];
		uniontemp.uctemp[1] = RxMsg[u8Idx].Data[5];
		uniontemp.uctemp[2] = RxMsg[u8Idx].Data[6];
		uniontemp.uctemp[3] = RxMsg[u8Idx].Data[7];
		SendMsg.Data[1] = uniontemp.u32temp;
		DealRawCanMsgToIMMU2(&SendMsg, u8GSNo, FALSE);

		pSingleRectData = NULL;

#ifdef	MPPT_Enable
		if((R_PRPTO_DEVNO_0B == u8DevNo) && (u8GSNo >= MPPT_GRP_NO_MIN && u8GSNo <= MPPT_GRP_NO_MAX))  //MPPT模组
		{
			if(u8RcvSrcAddr >= MPPT_MODULE_BASE_ADDR)  //单模块命令
			{
				u8RcvSrcAddr -= MPPT_MODULE_BASE_ADDR;

				if(u8RcvSrcAddr < mMPPTNUM)
				{
					pSingleRectData = &gs_CANComm.MpptData[u8RcvSrcAddr];
				}
				else
				{
					continue;
				}
			}
			else if(u8RcvSrcAddr == (u8GSNo+1))  //模组命令
			{
				u8RcvSrcAddr = u8GSNo+1;
			}	
			else
			{
				//否则，认为不正常，不处理
				continue;
			}
		}
		//MPPT模块暂时不支持0A命令
		/*else if((R_PRPTO_DEVNO_0A == u8DevNo) && (u8RcvSrcAddr >= MPPT_MODULE_BASE_ADDR))
		{
			continue;
		}*/
		//其它时间收到MPPT模块信息，不理会
		else if(u8RcvSrcAddr >= MPPT_MODULE_BASE_ADDR)
		{
			continue;
		}
		else //其它模组
		{
			if(u8RcvSrcAddr < mRECTNUM)
			{
				pSingleRectData = &gs_CANComm.RectData[u8RcvSrcAddr];
			}

			//地址重排
			if(u8RcvErrCode == RECT_RET_ERR_Code_IN_STARTING)
			{
				gs_CANComm.bNeedReConfig = TRUE;

				gs_CANComm.u8AddressScanDelay = RECT_ADDRESS_SCAN_DELAY;
				gs_CANComm.u8SingleRectScanCmdCnt = SINGLE_RECT_SCAN_LOOP_NUM;

				return ERR_CAN_R_IN_STARTING;
			}
		}
#else
		if(u8RcvSrcAddr < mRECTNUM)
		{
			pSingleRectData = &gs_CANComm.RectData[u8RcvSrcAddr];
		}

		//地址重排
		if(u8RcvErrCode == RECT_RET_ERR_Code_IN_STARTING)
		{
			gs_CANComm.bNeedReConfig = TRUE;

			gs_CANComm.u8AddressScanDelay = RECT_ADDRESS_SCAN_DELAY;
			gs_CANComm.u8SingleRectScanCmdCnt = SINGLE_RECT_SCAN_LOOP_NUM;

			return ERR_CAN_R_IN_STARTING;
		}

#endif
		
		/*有故障码不处理数据*/
		if (u8RcvErrCode != RECT_RET_ERR_Code_NO_ERR) 
		{
			continue;
		}
		/*发送和接收的设备号一致、命令号一致、目标地址和 监控一致，处理信息*/
		if ((u8Cmd == u8RcvCmd) && (u8DevNo == u8RcvDevNo) && (u8RcvDstAddr == R_PROTO_SELF_ADDR))
		{
			u8Buf[0] = RxMsg[u8Idx].Data[0]; 
            u8Buf[1] = RxMsg[u8Idx].Data[1];
			u8Buf[2] = RxMsg[u8Idx].Data[2]; 
            u8Buf[3] = RxMsg[u8Idx].Data[3];
			u8Buf[4] = RxMsg[u8Idx].Data[4]; 
            u8Buf[5] = RxMsg[u8Idx].Data[5];
			u8Buf[6] = RxMsg[u8Idx].Data[6]; 
            u8Buf[7] = RxMsg[u8Idx].Data[7];

			if (R_PRPTO_DEVNO_0B == u8DevNo)  //组命令
			{
				//环网：增加判断源地址 是否 与本监控组号一致
				//if(u8RcvSrcAddr == gs_CANComm.byGrpIdx[u8GSNo])
				{
					u8Rtn = ERR_CAN_R_NO_ERR;		//只要收到命令号、设备号、目的地址、源地址都正确的回复，就认为收到回复

					switch(u8Cmd)
					{
					case 0x01:
						u8RcvSrcAddr--;
						if(u8RcvSrcAddr < _MAX_GRP_NUM)
						{
							if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
							{
								if((u8RcvSrcAddr >= ACDC_GRP_NO_MIN)&&(u8RcvSrcAddr <= ACDC_GRP_NO_MAX))
								{
									if(gs_CANComm.u8GRP_WorkModeSet[u8RcvSrcAddr] == GRP_WORKMODE_SET_REVERSE)
									{
										PData->GrpData[u8RcvSrcAddr].fDCInputVolt = String2Float(u8Buf);					//1或2组 系统电压
										//PData->GrpData[u8RcvSrcAddr].fDCInputCurr = -(ABS(String2Float(u8Buf + 4)));		//1或2组 系统电压
									}
									else
									{
										PData->GrpData[u8RcvSrcAddr].fOutVolt = String2Float(u8Buf);					//1或2组 系统电压
										//PData->GrpData[u8RcvSrcAddr].fOutCurr = (ABS(String2Float(u8Buf + 4)));		//1或2组 系统电压

										gs_CANComm.GrpData[u8RcvSrcAddr].fDCInputVolt = 0;
										gs_CANComm.GrpData[u8RcvSrcAddr].fDCInputCurr = 0;
									}
								}
								else if(gs_CANComm.u8GRP_WorkModeSet[u8RcvSrcAddr] == GRP_WORKMODE_SET_REVERSE)
								{
									PData->GrpData[u8RcvSrcAddr].fDCInputVolt = String2Float(u8Buf);					//1或2组 系统电压
									PData->GrpData[u8RcvSrcAddr].fDCInputCurr = -(ABS(String2Float(u8Buf + 4)));		//1或2组 系统电压
								}
								else
								{
									PData->GrpData[u8RcvSrcAddr].fOutVolt = String2Float(u8Buf);					//1或2组 系统电压
									PData->GrpData[u8RcvSrcAddr].fOutCurr = String2Float(u8Buf + 4);				//1或2组 系统电压

									gs_CANComm.GrpData[u8RcvSrcAddr].fDCInputVolt = 0;
									gs_CANComm.GrpData[u8RcvSrcAddr].fDCInputCurr = 0;
								}

								gs_CANComm.GrpData[u8RcvSrcAddr].fExtVolt = String2Float(u8Buf);

							}  //if(gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)
							//BEG
							else
							{
								if(gs_CANComm.u8GRP_WorkModeSet[u8RcvSrcAddr] == GRP_WORKMODE_SET_REVERSE)
								{
									PData->GrpData[u8RcvSrcAddr].fDCInputVolt = String2Float(u8Buf);					//1或2组 系统电压
									PData->GrpData[u8RcvSrcAddr].fDCInputCurr = -(ABS(String2Float(u8Buf + 4)));		//1或2组 系统电压
								}
								else
								{
									PData->GrpData[u8RcvSrcAddr].fOutVolt = String2Float(u8Buf);					//1或2组 系统电压
									PData->GrpData[u8RcvSrcAddr].fOutCurr = String2Float(u8Buf + 4);				//1或2组 系统电压

									gs_CANComm.GrpData[u8RcvSrcAddr].fDCInputVolt = 0;
									gs_CANComm.GrpData[u8RcvSrcAddr].fDCInputCurr = 0;
								}
							}
						}						
						break;
					case 0x02:
						
						u8RcvSrcAddr--;
						if(u8RcvSrcAddr < _MAX_GRP_NUM)
						{
							PData->GrpData[u8RcvSrcAddr].u8GrpCMDResponeseDelayTimes = 0;
							PData->GrpData[u8RcvSrcAddr].u8GrpCMDResponeseFlag = TRUE;

							PData->GrpData[u8RcvSrcAddr].nCommRectNum = (int)RxMsg[u8Idx].Data[2];
							PData->GrpData[u8RcvSrcAddr].bAllRectProtected = (RxMsg[u8Idx].Data[1] >> 2)&0x01;
							PData->GrpData[u8RcvSrcAddr].bAllRectFailed = (RxMsg[u8Idx].Data[1] >> 1)&0x01;
							PData->GrpData[u8RcvSrcAddr].bAllRect_Off = RxMsg[u8Idx].Data[1] & 0x01;

							PData->bIsGrpExist[u8RcvSrcAddr] = TRUE;
						}
						break;
					case 0x1A://广播
						break;
					case 0x13:
						break;
					case 0x1B://调节电流
						break;
					case 0x0A:			
						//PData->GrpData[u8GSNo].fMax_OutVolt = (ftemp >= 50.0) ? ftemp : 750.0;
						ftemp = (float)(RxMsg[u8Idx].Data[0] << 8) + RxMsg[u8Idx].Data[1];
						if(ftemp < 50)
						{
							ftemp = 750;
						}
						if(ftemp > s_fGrpMaxVoltBuf)
						{
							s_fGrpMaxVoltBuf = ftemp;
						}

						//PData->GrpData[u8GSNo].fMin_OutVolt = MAX(50.0, ftemp);
						ftemp = (float)(RxMsg[u8Idx].Data[2] << 8) + RxMsg[u8Idx].Data[3];
						ftemp = MAX(50.0, ftemp);
						if(ftemp < s_fGrpMinVoltBuf)
						{
							s_fGrpMinVoltBuf = ftemp;
						}
						
						//PData->GrpData[u8GSNo].fMax_OutCurr = (float)(RxMsg[u8Idx].Data[4] << 8) + RxMsg[u8Idx].Data[5];
						//PData->GrpData[u8GSNo].fMax_OutCurr *= 0.1;
						fMax_OutCurr = (float)(RxMsg[u8Idx].Data[4] << 8) + RxMsg[u8Idx].Data[5];
//					if((gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType != ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE))
					if((gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE))
					{					
						fMax_OutCurr *= 0.1;
					}						

						//增加额定功率/**/
						fRatedPower = (float)(RxMsg[u8Idx].Data[6] << 8) + RxMsg[u8Idx].Data[7];
						fRatedPower *= 0.01;

						if((fMax_OutCurr < 10.0) || (fMax_OutCurr > 300.0))
						{
							fRatedPower = 15.0;
							fMax_OutCurr = 30.0;
						}
						else if(fRatedPower < 10.0)
						{
							fRatedPower = (fMax_OutCurr > 31.0) ? 20.0 : 15.0;
						}

						//20190618 单模块的功率，最大电流
						//if(u8RcvSrcAddr < mRECTNUM)
						if(pSingleRectData != NULL)
						{
							if(pSingleRectData->bFanFault || pSingleRectData->bProtected
								|| pSingleRectData->bRectFail)
							{
								fMax_OutCurr = 0;
								fRatedPower = 0;
							}

							pSingleRectData->fMax_OutCurr = fMax_OutCurr;

//						if((gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE) && (gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType != ACDC_IS_35KWPCS_TYPE))
						if((gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE) && (gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE))
						{
							pSingleRectData->fRatedPower = fRatedPower;
						}

							pSingleRectData->bIsExisted = TRUE;
							pSingleRectData->bCommFail = FALSE;
							pSingleRectData->nCommFailTimes = 0;
						}

						s_fGrpRateCurrBuf += fMax_OutCurr;
						s_fGrpRatePowerBuf += fRatedPower;


						//PData->GrpData[u8GSNo].fMax_OutVolt = (float)(RxMsg[u8Idx].Data[0] << 8) + RxMsg[u8Idx].Data[1];
						//PData->GrpData[u8GSNo].fMax_OutVolt = (PData->GrpData[u8GSNo].fMax_OutVolt >= 50.0) ? PData->GrpData[u8GSNo].fMax_OutVolt : 1000;
						//PData->GrpData[u8GSNo].fMin_OutVolt = (float)(RxMsg[u8Idx].Data[2] << 8) + RxMsg[u8Idx].Data[3];
						//PData->GrpData[u8GSNo].fMin_OutVolt = MAX(50.0, PData->GrpData[u8GSNo].fMin_OutVolt);

						//ftemp = (float)(RxMsg[u8Idx].Data[2] << 8) + RxMsg[u8Idx].Data[3];
						//PData->GrpData[u8GSNo].fMin_OutVolt = MAX(50.0, ftemp);
				
						//fMax_OutCurr = (float)(RxMsg[u8Idx].Data[4] << 8) + RxMsg[u8Idx].Data[5];
						//fMax_OutCurr *= 0.1;

						////增加额定功率/**/
						//fRatedPower = (float)(RxMsg[u8Idx].Data[6] << 8) + RxMsg[u8Idx].Data[7];
						//fRatedPower *= 0.01;

						//if((fMax_OutCurr < 10.0) || (fMax_OutCurr > 300.0))
						//{
						//	fRatedPower = 15.0;
						//	fMax_OutCurr = 30.0;
						//}
						//else if(fRatedPower < 10.0)
						//{
						//	fRatedPower = (fMax_OutCurr > 31.0) ? 20.0 : 15.0;
						//}
						//PData->GrpData[u8GSNo].fRatedPower = fRatedPower * PData->GrpData[u8GSNo].nCommRectNum;//模块是单个回复的/**/
						//PData->GrpData[u8GSNo].fMax_OutCurr = fMax_OutCurr * PData->GrpData[u8GSNo].nCommRectNum;//模块是单个回复的/**/
						//PData->GrpData[u8GSNo].fRatedMaxCurr = PData->GrpData[u8GSNo].fMax_OutCurr;
						break;

					case 0x03:
						//if(u8RcvSrcAddr < mRECTNUM)
						if(pSingleRectData != NULL)
						{
							pSingleRectData->fRealTime_Volt = String2Float(u8Buf);
							pSingleRectData->fRealTime_Curr = String2Float(u8Buf + 4);

							//非环网模式下直接置存在标志
							//if(gEvChargeInfo.u8IsRingEnabled == FALSE)
							{
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;

								if(pSingleRectData->fRealTime_Volt > s_fConvertOutVoltBuf)
								{
									s_fConvertOutVoltBuf = pSingleRectData->fRealTime_Volt;
								}

								s_fConvertOutCurrBuf += pSingleRectData->fRealTime_Curr;
							}	
						}
						break;
					case 0x04:                           //光伏告警状态上传  0x04

						//if(u8RcvSrcAddr < mRECTNUM)
						if(pSingleRectData != NULL)
						{
							pSingleRectData->bConvertStat =  RxMsg[u8Idx].Data[0];

							pSingleRectData->nBelongedGrp = (int)RxMsg[u8Idx].Data[2];
							//pSingleRectData->fRealTime_Temp = (float)RxMsg[u8Idx].Data[4];

							pSingleRectData->bStatCode[3] = RxMsg[u8Idx].Data[3];

							union_u8temp.u8temp = RxMsg[u8Idx].Data[4];       
							pSingleRectData->fRealTime_Temp = (float)(union_u8temp.i8temp);                            //模块环境温度
							//以下解析状态字/**/
							pSingleRectData->bStatCode[2] = RxMsg[u8Idx].Data[5];
							if((gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE) && (gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE))
							{
								pSingleRectData->bAC_Off = (RxMsg[u8Idx].Data[5] >> 7) & 0x01;            //AC 断开
								pSingleRectData->bAC_OverVolt = (RxMsg[u8Idx].Data[5] >> 6) & 0x01;          //AC 过压
								pSingleRectData->bDC_LowVolt = (RxMsg[u8Idx].Data[5] >> 5) & 0x01;          //DC 低压
								pSingleRectData->bAC_NoBalance = (RxMsg[u8Idx].Data[5] >> 4) & 0x01;         //AC 不平衡
								pSingleRectData->bAC_PhaseLack = (RxMsg[u8Idx].Data[5] >> 3) & 0x01;
								pSingleRectData->bCurr_NoBalace = (RxMsg[u8Idx].Data[5] >> 2) & 0x01;
								pSingleRectData->bID_Conflict = (RxMsg[u8Idx].Data[5] >> 1) & 0x01;
								pSingleRectData->bPowerLimited = (RxMsg[u8Idx].Data[5]) & 0x01;
							}

							pSingleRectData->bStatCode[1] = RxMsg[u8Idx].Data[6];
							//pRect_Rough->pRawRects[nIdx].bRectCommFail = (pRect_Rough->frmReply.byData[6] >> 7) & 0x01;


							if((gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE) && (gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE))
							{
								pSingleRectData->bWalkIn_Enabled = (RxMsg[u8Idx].Data[6] >> 6) & 0x01;
								pSingleRectData->bDC_OverVolt = (RxMsg[u8Idx].Data[6] >> 5) & 0x01;
								pSingleRectData->bOverTemp = (RxMsg[u8Idx].Data[6] >> 4) & 0x01;
								pSingleRectData->bFanFault = (RxMsg[u8Idx].Data[6] >> 3) & 0x01;
								pSingleRectData->bProtected = (RxMsg[u8Idx].Data[6] >> 2) & 0x01;
								pSingleRectData->bRectFail = (RxMsg[u8Idx].Data[6] >> 1) & 0x01;
								pSingleRectData->bDC_Off = (RxMsg[u8Idx].Data[6]) & 0x01;
							}

							pSingleRectData->bStatCode[0] = RxMsg[u8Idx].Data[7];

							if((gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE) && (gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE))
							{
								pSingleRectData->bDisCh_Err = (RxMsg[u8Idx].Data[7] >> 5) & 0x01;
								pSingleRectData->bOn_Sleep = (RxMsg[u8Idx].Data[7] >> 4) & 0x01;
								pSingleRectData->bPFC_Err = (RxMsg[u8Idx].Data[7] >> 3) & 0x01;
								pSingleRectData->bInternalCommErr = (RxMsg[u8Idx].Data[7] >> 2) & 0x01;
								pSingleRectData->bInternalDC_LowVolt = (RxMsg[u8Idx].Data[7] >> 1) & 0x01;
								pSingleRectData->bDC_Shorted = (RxMsg[u8Idx].Data[7]) & 0x01;
							}

							//if(gEvChargeInfo.u8IsRingEnabled == FALSE)
							{
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}	
						}
						break;
					case 0x06:
						//if(u8RcvSrcAddr < mRECTNUM)
						if(pSingleRectData != NULL)
						{
							pSingleRectData->fAB_Volt = 0.1*((float)(RxMsg[u8Idx].Data[0] << 8) + RxMsg[u8Idx].Data[1]);
							pSingleRectData->fBC_Volt = 0.1*((float)(RxMsg[u8Idx].Data[2] << 8) + RxMsg[u8Idx].Data[3]);
							pSingleRectData->fCA_Volt = 0.1*((float)(RxMsg[u8Idx].Data[4] << 8) + RxMsg[u8Idx].Data[5]);

							//if(gEvChargeInfo.u8IsRingEnabled == FALSE)
							{
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}	
						}
						break;
					case 0x07:
						//if(u8RcvSrcAddr < mRECTNUM)
						if(pSingleRectData != NULL)
						{
							if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
							{
								if((u8GSNo >= ACDC_GRP_NO_MIN)&&(u8GSNo <= ACDC_GRP_NO_MAX))
								{
									pSingleRectData->fDC_Ver  = (float)((RxMsg[u8Idx].Data[0]<<8) + RxMsg[u8Idx].Data[1]) * 0.01;
									pSingleRectData->fAC_Ver  = (float)((RxMsg[u8Idx].Data[2]<<8) + RxMsg[u8Idx].Data[3]) * 0.01;
								}
								else
								{
									pSingleRectData->fDC_Ver  = (float)(RxMsg[u8Idx].Data[0]) + (float)(RxMsg[u8Idx].Data[1] >> 4) * 0.1 + (float)(RxMsg[u8Idx].Data[1] & 0x0F) * 0.01;
									pSingleRectData->fAC_Ver  = (float)(RxMsg[u8Idx].Data[2]) + (float)(RxMsg[u8Idx].Data[3] >> 4) * 0.1 + (float)(RxMsg[u8Idx].Data[3] & 0x0F) * 0.01;
								}
							}
							else
							{
								pSingleRectData->fDC_Ver  = (float)(RxMsg[u8Idx].Data[0]) + (float)(RxMsg[u8Idx].Data[1] >> 4) * 0.1 + (float)(RxMsg[u8Idx].Data[1] & 0x0F) * 0.01;
								pSingleRectData->fAC_Ver  = (float)(RxMsg[u8Idx].Data[2]) + (float)(RxMsg[u8Idx].Data[3] >> 4) * 0.1 + (float)(RxMsg[u8Idx].Data[3] & 0x0F) * 0.01;
							}

							pSingleRectData->fHW_Ver = (float)(RxMsg[u8Idx].Data[4]) + (float)(RxMsg[u8Idx].Data[5] >> 4) * 0.1 + (float)(RxMsg[u8Idx].Data[5] & 0x0F) * 0.01;

							//if(gEvChargeInfo.u8IsRingEnabled == FALSE)
							{
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}	
						}
						break;
					case 0x0B:
						//if(u8RcvSrcAddr < mRECTNUM)
						if(pSingleRectData != NULL)
						{
							pSingleRectData->bSerialNo[0] = u8Buf[0];		
							pSingleRectData->bSerialNo[1] = u8Buf[1];	
							pSingleRectData->bSerialNo[2] = u8Buf[2];		
							pSingleRectData->bSerialNo[3] = u8Buf[3];	
							pSingleRectData->bSerialNo[4] = u8Buf[4];		
							pSingleRectData->bSerialNo[5] = u8Buf[5];	
							pSingleRectData->bSerialNo[6] = u8Buf[6];		
							pSingleRectData->bSerialNo[7] = u8Buf[7];	

							pSingleRectData->bIsExisted = TRUE;
							pSingleRectData->bCommFail = FALSE;
							pSingleRectData->nCommFailTimes = 0;
							
							//是否已有同类型名称的模块
							u8Found = 0;							
							for(k = 0; k < gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum; k++)
							{
								if((gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][0] == u8Buf[0])
									&& (gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][1] == u8Buf[6])
									&& (gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][2] == u8Buf[7]))
								{
									u8Found = 1;
									break;
								}
							}
							if((u8Found == 0)
								&& (gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum < _MAX_GRP_NUM))
							{
								k = gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum;

								gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][0] = u8Buf[0];
								gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][1] = u8Buf[6];
								gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][2] = u8Buf[7];	
								gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][3] = 0;
								gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][4] = 0;	

								//gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][0] = u8Buf[0];
								u16temp = ((UINT16)u8Buf[6]<<8) + (UINT16)u8Buf[7];
								sprintf( (char*)(&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][0]), "0:%c%04dU1.bin", u8Buf[0],u16temp);
								//gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][5] = 0;
								//gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[k][0] = u8Buf[0];
								sprintf( (char*)(&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[k][0]), "0:%c%04dU2.bin", u8Buf[0],u16temp);
								//gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[k][5] = 0;

								gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][14] = 0;
								gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[k][14] = 0;

								gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum++;
							}
						}
						break;
					case 0x0C:	
						//if(u8RcvSrcAddr < mRECTNUM)
						if(pSingleRectData != NULL)
						{
							ftemp = (float)((RxMsg[u8Idx].Data[0] << 8) + RxMsg[u8Idx].Data[1]) * 0.1; 
							//pSingleRectData->fExtVolt = ftemp;//现在01和0C命令都读，0C命令回复0，暂时用01命令
							if(ftemp > s_fGrpExtVolt)
							{
								s_fGrpExtVolt = ftemp;
							}		
							
							//20221027 lkf PCS模块还需要看它是否运行中
							if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (ACDC_IS_35KWPCS_TYPE))
								&& ((u8GSNo >= ACDC_GRP_NO_MIN)&&(u8GSNo <= ACDC_GRP_NO_MAX))
								&& (((pSingleRectData->bStatCode_Invert[0] & 0x07) == 0x03) //故障
										|| ((pSingleRectData->bStatCode_Invert[0] & 0x07) == 0x04)) //告警
								)
							{							
								fMax_OutCurr = 0;
								fRatedPower = 0;	
							}
							else if(pSingleRectData->bFanFault || pSingleRectData->bProtected || pSingleRectData->bRectFail)								
							{
								fMax_OutCurr = 0;
								fRatedPower = 0;
							}
							else
							{
								ftemp = (float)((RxMsg[u8Idx].Data[2] << 8) + RxMsg[u8Idx].Data[3]) * 0.1;

								//if(pSingleRectData->bDC_Off == FALSE) && (ftemp < 1)  //模块关机时可能 回传0
								if(ftemp < 1)
								{
									if((pSingleRectData->fRatedPower > 10) && (pSingleRectData->fExtVolt > 50))
									{
										fMax_OutCurr = pSingleRectData->fRatedPower * 1000 / pSingleRectData->fExtVolt;

										if(fMax_OutCurr > pSingleRectData->fMax_OutCurr)
										{
											fMax_OutCurr = pSingleRectData->fMax_OutCurr;
										}

										fRatedPower = fMax_OutCurr * pSingleRectData->fExtVolt/1000;
									}
									else
									{
										fMax_OutCurr = pSingleRectData->fMax_OutCurr;   // ??? 使用额定电流来替代
										fRatedPower = pSingleRectData->fRatedPower;   
									}
								}
								else
								{
									fMax_OutCurr = ftemp;
									if(pSingleRectData->fExtVolt > 50)
									{
										fRatedPower = fMax_OutCurr * pSingleRectData->fExtVolt/1000;
									}
									else
									{
										fRatedPower = pSingleRectData->fRatedPower;  
									}
								}
							}

							if((fRatedPower > pSingleRectData->fRatedPower) && (pSingleRectData->fRatedPower > 10))
							{
								fRatedPower = pSingleRectData->fRatedPower;
							}

							
							pSingleRectData->fAvailableCurr = fMax_OutCurr;
							s_fGrpAvailableCurr += 	fMax_OutCurr;

							s_fGrpAvailablePwr += (fRatedPower);

							pSingleRectData->fOutSiteVolt = pSingleRectData->fExtVolt;
							pSingleRectData->fOutSiteMaxCurr = pSingleRectData->fAvailableCurr;

							pSingleRectData->bIsExisted = TRUE;
							pSingleRectData->bCommFail = FALSE;
							pSingleRectData->nCommFailTimes = 0;
						}
						break;
//by xlr01202  增加MPPT模块温度命令
					case 0x0E:

						if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x07))                  //DC板温
						{
							if(pSingleRectData != NULL)
							{				
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32MPPT_DC_Tem = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x08))             //PFC板温
						{						
							if(pSingleRectData != NULL)
							{				
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32MPPT_PFC_Tem = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x12)&&(RxMsg[u8Idx].Data[1]== 0x02))          //DC变压器温度
						{						
							if(pSingleRectData != NULL)
							{				
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32MPPT_DC_trans_Tem = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x12)&&(RxMsg[u8Idx].Data[1]== 0x03))            //PFC变压器温度
						{						
							if(pSingleRectData != NULL)
							{				
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32MPPT_PFC_trans_Tem = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						break;
					case 0x23:	
						union_temp.u32buf = (RxMsg[u8Idx].Data[4] << 24) + (RxMsg[u8Idx].Data[5] << 16)  + (RxMsg[u8Idx].Data[6] << 8)  +  RxMsg[u8Idx].Data[7];
						ftemp = (float) union_temp.u32buf * 0.001;

						if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x12))
						{
							//if(u8RcvSrcAddr < mRECTNUM)
							if(pSingleRectData != NULL)
							{								
								//pSingleRectData->bConvertStat =  RxMsg[u8Idx].Data[7];
								pSingleRectData->bStatCode_Invert[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->bStatCode_Invert[1] = RxMsg[u8Idx].Data[6];
								pSingleRectData->bStatCode_Invert[2] = RxMsg[u8Idx].Data[5];
								pSingleRectData->bStatCode_Invert[3] = RxMsg[u8Idx].Data[4];

								if(gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE  || gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE || gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)
								{
									pSingleRectData->bOverTemp = (pSingleRectData->bStatCode_Invert[2] >> 4) & 0x01;
									pSingleRectData->bFanFault = (pSingleRectData->bStatCode_Invert[2] >> 3) & 0x01;
									pSingleRectData->bCurr_NoBalace = (pSingleRectData->bStatCode_Invert[2] >> 2) & 0x01;
									pSingleRectData->bID_Conflict = (pSingleRectData->bStatCode_Invert[2] >> 1) & 0x01;
									pSingleRectData->bEPO = (pSingleRectData->bStatCode_Invert[2]) & 0x01;

									pSingleRectData->bRectFail = (pSingleRectData->bStatCode_Invert[3] >> 7) & 0x01;	
									pSingleRectData->bDC_Shorted = (pSingleRectData->bStatCode_Invert[3] >> 2) & 0x01;

									pSingleRectData->bPCSWorkStatus = (pSingleRectData->bStatCode_Invert[0] >> 6) & 0x03;//开关机状态
									pSingleRectData->bPCSWorkMode = (pSingleRectData->bStatCode_Invert[0] >> 4) & 0x03;//充放电状态
								}


								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}	
//by lxr01202  增加1110相关故障码   适用BGG模块（22KW和35KW）
						/*1110相关故障码*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x10))
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bStatFaltCode_PCS_35KW[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->bStatFaltCode_PCS_35KW[1] = RxMsg[u8Idx].Data[6];
								pSingleRectData->bStatFaltCode_PCS_35KW[2] = RxMsg[u8Idx].Data[5];
								pSingleRectData->bStatFaltCode_PCS_35KW[3] = RxMsg[u8Idx].Data[4];
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
//by lxr01202  增加1111相关故障码             适用BGG模块（22KW和35KW）
						/*1111相关故障码*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x11))
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bStatFaltCode_PCS_35KW[4] = RxMsg[u8Idx].Data[7];
								pSingleRectData->bStatFaltCode_PCS_35KW[5] = RxMsg[u8Idx].Data[6];
								pSingleRectData->bStatFaltCode_PCS_35KW[6] = RxMsg[u8Idx].Data[5];
								pSingleRectData->bStatFaltCode_PCS_35KW[7] = RxMsg[u8Idx].Data[4];
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}	
						/*1113相关故障码*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x13))
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bStatFaltCode_PCS[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->bStatFaltCode_PCS[1] = RxMsg[u8Idx].Data[6];
								pSingleRectData->bStatFaltCode_PCS[2] = RxMsg[u8Idx].Data[5];
								pSingleRectData->bStatFaltCode_PCS[3] = RxMsg[u8Idx].Data[4];
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*1114相关故障码*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x14))
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bStatFaltCode_PCS[4] = RxMsg[u8Idx].Data[7];
								pSingleRectData->bStatFaltCode_PCS[5] = RxMsg[u8Idx].Data[6];
								pSingleRectData->bStatFaltCode_PCS[6] = RxMsg[u8Idx].Data[5];
								pSingleRectData->bStatFaltCode_PCS[7] = RxMsg[u8Idx].Data[4];
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*1115相关故障码*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)) 
							&& (RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x15))
						{
							if(pSingleRectData != NULL)
							{				
								pSingleRectData->bDCFaltCode_PCS[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->bDCFaltCode_PCS[1] = RxMsg[u8Idx].Data[6];
								pSingleRectData->bDCFaltCode_PCS[2] = RxMsg[u8Idx].Data[5];
								pSingleRectData->bDCFaltCode_PCS[3] = RxMsg[u8Idx].Data[4];
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}

//by lxr01202  新增模块内部板温上传
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)) 
							&& (RxMsg[u8Idx].Data[0] == 0x12)&&(RxMsg[u8Idx].Data[1]== 0x02))
						{
							if(pSingleRectData != NULL)
							{				
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32Rect_Tem = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
//by lxr01202  新增模块功率器件最高温度
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)) 
							&& (RxMsg[u8Idx].Data[0] == 0x12)&&(RxMsg[u8Idx].Data[1]== 0x03))
						{
							if(pSingleRectData != NULL)
							{				
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32Rect_Tem_MAX = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*交流A相相电压*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x01))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32APhase_Volt = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*交流B相相电压*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x02))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32BPhase_Volt = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*交流C相相电压*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x03))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->u32CPhase_Volt = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*交流A相电流*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x04))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->i32APhase_Curr = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*交流B相电流*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x05))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->i32BPhase_Curr = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*交流C相电流*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x06))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->i32CPhase_Curr = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*总有功功率*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x08))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->TotalActivePower = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*A相有功功率*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x09))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->ActivePowerAB = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*B相有功功率*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x0A))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->ActivePowerBC = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*C相有功功率*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x0B))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->ActivePowerCA = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*总无功功率*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x0C))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->TotalReactivePower = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*A相无功功率*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x0D))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->ReactivePowerAB = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*B相无功功率*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x0E))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->ReactivePowerBC = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						/*C相无功功率*/
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x0F))
						{
							if(pSingleRectData != NULL)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								pSingleRectData->ReactivePowerCA = dword.l;
								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x40)&&(RxMsg[u8Idx].Data[1]== 0x01))  //组输出电压
						{							
							PData->GrpData[u8GSNo].fOutVolt = ftemp;
							PData->GrpData[u8GSNo].fExtVolt = ftemp;
						}
						else if((RxMsg[u8Idx].Data[0] == 0x40)&&(RxMsg[u8Idx].Data[1]== 0x02))  //组输出电流
						{
							ftemp = (float)union_temp.i32buf * 0.001;
							PData->GrpData[u8GSNo].fOutCurr = ftemp;						
						}
						else if((RxMsg[u8Idx].Data[0] == 0x41)&&(RxMsg[u8Idx].Data[1]== 0x01))  //单模块输出电压
						{
							//if(u8RcvSrcAddr < mRECTNUM)
							if(pSingleRectData != NULL)
							{
								pSingleRectData->fRealTime_Volt =  ftemp;
								pSingleRectData->fRealTime_Volt_BattSide =  ftemp;
								//非环网模式下直接置存在标志
								//if(gEvChargeInfo.u8IsRingEnabled == FALSE)
								{
									pSingleRectData->bIsExisted = TRUE;
									pSingleRectData->bCommFail = FALSE;
									pSingleRectData->nCommFailTimes = 0;
								}	

								if(ftemp > s_fConvertOutVoltBuf)
								{
									s_fConvertOutVoltBuf = ftemp;
								}
							}	
							
						}
						else if((RxMsg[u8Idx].Data[0] == 0x41)&&(RxMsg[u8Idx].Data[1]== 0x02))  //单模块输出电流
						{
							//if(u8RcvSrcAddr < mRECTNUM)
							if(pSingleRectData != NULL)
							{
								ftemp = (float)union_temp.i32buf * 0.001;

								/*if((u8GSNo == GRP_E_DCDC)||(u8GSNo == GRP_F_DCDC))
								{
									ftemp = -ftemp;
								}			*/					

								pSingleRectData->fRealTime_Curr =  ftemp;
								pSingleRectData->fRealTime_Curr_BattSide = ftemp;
								//非环网模式下直接置存在标志
								//if(gEvChargeInfo.u8IsRingEnabled == FALSE)
								{
									pSingleRectData->bIsExisted = TRUE;
									pSingleRectData->bCommFail = FALSE;
									pSingleRectData->nCommFailTimes = 0;
								}	

								s_fConvertOutCurrBuf += ftemp;
							}								
						}

						//barcode
						else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x24))  
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bBarcode_24[0] = u8Buf[4];		
								pSingleRectData->bBarcode_24[1] = u8Buf[5];	
								pSingleRectData->bBarcode_24[2] = u8Buf[6];		
								pSingleRectData->bBarcode_24[3] = u8Buf[7];	

								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x25))  
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bBarcode_25[0] = u8Buf[4];		
								pSingleRectData->bBarcode_25[1] = u8Buf[5];	
								pSingleRectData->bBarcode_25[2] = u8Buf[6];		
								pSingleRectData->bBarcode_25[3] = u8Buf[7];	

								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x26))  
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bBarcode_26[0] = u8Buf[4];		
								pSingleRectData->bBarcode_26[1] = u8Buf[5];	
								pSingleRectData->bBarcode_26[2] = u8Buf[6];		
								pSingleRectData->bBarcode_26[3] = u8Buf[7];	

								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x27))  
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bBarcode_27[0] = u8Buf[4];		
								pSingleRectData->bBarcode_27[1] = u8Buf[5];	
								pSingleRectData->bBarcode_27[2] = u8Buf[6];		
								pSingleRectData->bBarcode_27[3] = u8Buf[7];

								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x28))  
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bBarcode_28[0] = u8Buf[4];		
								pSingleRectData->bBarcode_28[1] = u8Buf[5];	
								pSingleRectData->bBarcode_28[2] = u8Buf[6];		
								pSingleRectData->bBarcode_28[3] = u8Buf[7];	

								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;

								//识别内部型号名称
								if(pSingleRectData->bBarcode_27[0] != 0)  //不等于0，认为读到过
								{
									//是否已有同类型名称的模块
									u8Found = 0;							
									for(k = 0; k < gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum; k++)
									{
										if(	(gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][0] == pSingleRectData->bBarcode_27[0])
											&& (gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][1] == pSingleRectData->bBarcode_27[1])
											&& (gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][2] == pSingleRectData->bBarcode_27[2])
											&& (gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][3] == pSingleRectData->bBarcode_27[3])
											&& (gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][4] == pSingleRectData->bBarcode_28[0])
											)
										{
											u8Found = 1;
											break;
										}
									}
									if((u8Found == 0)
										&& (gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum < _MAX_GRP_NUM))
									{
										k = gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum;

										gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][0] = pSingleRectData->bBarcode_27[0];
										gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][1] = pSingleRectData->bBarcode_27[1];
										gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][2] = pSingleRectData->bBarcode_27[2];	
										gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][3] = pSingleRectData->bBarcode_27[3];
										gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleType[k][4] = pSingleRectData->bBarcode_28[0];	

										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][0] = '0';
										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][1] = ':';
										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][2] = pSingleRectData->bBarcode_27[0];
										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][3] = pSingleRectData->bBarcode_27[1];
										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][4] = pSingleRectData->bBarcode_27[2];
										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][5] = pSingleRectData->bBarcode_27[3];
										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][6] = pSingleRectData->bBarcode_28[0];
										sprintf( (char*)&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][7], "%s", "U1.bin");

										memcpy((char*)&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[k][0], (char*)&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][0], 7);

										sprintf( (char*)&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[k][7], "%s", "U2.bin");

										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[k][14] = 0;
										gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[k][14] = 0;

										gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum++;
									}
								}
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x29))  
						{
							if(pSingleRectData != NULL)
							{
								pSingleRectData->bBarcode_29[0] = u8Buf[4];		
								pSingleRectData->bBarcode_29[1] = u8Buf[5];	
								pSingleRectData->bBarcode_29[2] = u8Buf[6];		
								pSingleRectData->bBarcode_29[3] = u8Buf[7];

								pSingleRectData->bIsExisted = TRUE;
								pSingleRectData->bCommFail = FALSE;
								pSingleRectData->nCommFailTimes = 0;
							}
						}
						else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x33))  //组额定功率
						{							
							if(pSingleRectData != NULL)
							{								
								pSingleRectData->fRatedPower = ftemp* 0.001;
								s_fGrpRatePowerBuf += pSingleRectData->fRatedPower;
							}
						}
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x01)&&(RxMsg[u8Idx].Data[1]== 0x02))
						{
							u8RcvSrcAddr--;
							if(u8RcvSrcAddr < _MAX_GRP_NUM)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								PData->GrpData[u8RcvSrcAddr].TotalActivePower = dword.l;
							}
						}
						else if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
							&& (RxMsg[u8Idx].Data[0] == 0x01)&&(RxMsg[u8Idx].Data[1]== 0x03))
						{
							u8RcvSrcAddr--;
							if(u8RcvSrcAddr < _MAX_GRP_NUM)
							{
								dword.b[3] = RxMsg[u8Idx].Data[4];
								dword.b[2] = RxMsg[u8Idx].Data[5];
								dword.b[1] = RxMsg[u8Idx].Data[6];
								dword.b[0] = RxMsg[u8Idx].Data[7];
								PData->GrpData[u8RcvSrcAddr].TotalReactivePower = dword.l;
							}
						}

						//else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x03))  //A相电压
						//{
						//	if(u8RcvSrcAddr < mRECTNUM)
						//	{
						//		gs_CANComm.RectData[u8RcvSrcAddr].fA_Volt =  ftemp;
						//	}									
						//}
						//else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x04))  //B相电压
						//{
						//	if(u8RcvSrcAddr < mRECTNUM)
						//	{
						//		gs_CANComm.RectData[u8RcvSrcAddr].fB_Volt =  ftemp;
						//	}		
						//}
						//else if((RxMsg[u8Idx].Data[0] == 0x11)&&(RxMsg[u8Idx].Data[1]== 0x05))  //C相电压
						//{
						//	if(u8RcvSrcAddr < mRECTNUM)
						//	{
						//		gs_CANComm.RectData[u8RcvSrcAddr].fC_Volt =  ftemp;
						//	}		
						//}
						//else if((RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x04))  //A相电流
						//{
						//	if(u8RcvSrcAddr < mRECTNUM)
						//	{
						//		gs_CANComm.RectData[u8RcvSrcAddr].fA_Curr =  ftemp;
						//	}		
						//}
						//else if((RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x05))  //B相电流
						//{
						//	if(u8RcvSrcAddr < mRECTNUM)
						//	{
						//		gs_CANComm.RectData[u8RcvSrcAddr].fB_Curr =  ftemp;
						//	}		
						//}
						//else if((RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x06))  //C相电流
						//{
						//	if(u8RcvSrcAddr < mRECTNUM)
						//	{
						//		gs_CANComm.RectData[u8RcvSrcAddr].fC_Curr =  ftemp;
						//	}		
						//}
						//else if((RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x07))  //频率
						//{
						//	if(u8RcvSrcAddr < mRECTNUM)
						//	{
						//		gs_CANComm.RectData[u8RcvSrcAddr].fHz =  ftemp;
						//	}		
						//}
						//else if((RxMsg[u8Idx].Data[0] == 0x21)&&(RxMsg[u8Idx].Data[1]== 0x08))  //总有功功率
						//{
						//	if(u8RcvSrcAddr < mRECTNUM)
						//	{
						//		gs_CANComm.RectData[u8RcvSrcAddr].fActivePower =  ftemp;
						//	}		
						//}
						break;
					default:
						break;
					}
				}
			}
			else  //单个模块命令 -- MPPT模块不支持
			{		

				//增加判断源地址 一致
				//u8RcvSrcAddr = (RxMsg[u8Idx].ExtId & 0xff);
				if(u8RcvSrcAddr == gs_CANComm.AddrInfo[u8GSNo])
				{
					u8Rtn = ERR_CAN_R_NO_ERR;		//只要收到命令号、设备号、目的地址都正确的回复，就认为收到回复

					PData->RectData[u8GSNo].bCommFail = FALSE;
					PData->RectData[u8GSNo].nCommFailTimes = 0;

					switch(u8Cmd)
					{
					case 0x03:
						
						PData->RectData[u8GSNo].fRealTime_Volt = String2Float(u8Buf);
						PData->RectData[u8GSNo].fRealTime_Curr = String2Float(u8Buf + 4);

						//非环网模式下直接置存在标志
						//if(gEvChargeInfo.u8IsRingEnabled == FALSE)
						{
							PData->RectData[u8GSNo].bIsExisted = TRUE;
							PData->RectData[u8GSNo].bCommFail = FALSE;
							PData->RectData[u8GSNo].nCommFailTimes = 0;
						}						
						break;

					case 0x04:
						PData->RectData[u8GSNo].nBelongedGrp = (int)RxMsg[u8Idx].Data[2];
						PData->RectData[u8GSNo].fRealTime_Temp = (float)RxMsg[u8Idx].Data[4];
						//以下解析状态字/**/
						PData->RectData[u8GSNo].bStatCode[2] = RxMsg[u8Idx].Data[5];
						PData->RectData[u8GSNo].bAC_Off = (RxMsg[u8Idx].Data[5] >> 7) & 0x01;
						PData->RectData[u8GSNo].bAC_OverVolt = (RxMsg[u8Idx].Data[5] >> 6) & 0x01;
						PData->RectData[u8GSNo].bDC_LowVolt = (RxMsg[u8Idx].Data[5] >> 5) & 0x01;
						PData->RectData[u8GSNo].bAC_NoBalance = (RxMsg[u8Idx].Data[5] >> 4) & 0x01;
						PData->RectData[u8GSNo].bAC_PhaseLack = (RxMsg[u8Idx].Data[5] >> 3) & 0x01;
						PData->RectData[u8GSNo].bCurr_NoBalace = (RxMsg[u8Idx].Data[5] >> 2) & 0x01;
						PData->RectData[u8GSNo].bID_Conflict = (RxMsg[u8Idx].Data[5] >> 1) & 0x01;
						PData->RectData[u8GSNo].bPowerLimited = (RxMsg[u8Idx].Data[5]) & 0x01;

						PData->RectData[u8GSNo].bStatCode[1] = RxMsg[u8Idx].Data[6];
						//pRect_Rough->pRawRects[nIdx].bRectCommFail = (pRect_Rough->frmReply.byData[6] >> 7) & 0x01;
						PData->RectData[u8GSNo].bWalkIn_Enabled = (RxMsg[u8Idx].Data[6] >> 6) & 0x01;
						PData->RectData[u8GSNo].bDC_OverVolt = (RxMsg[u8Idx].Data[6] >> 5) & 0x01;
						PData->RectData[u8GSNo].bOverTemp = (RxMsg[u8Idx].Data[6] >> 4) & 0x01;
						PData->RectData[u8GSNo].bFanFault = (RxMsg[u8Idx].Data[6] >> 3) & 0x01;
						PData->RectData[u8GSNo].bProtected = (RxMsg[u8Idx].Data[6] >> 2) & 0x01;
						PData->RectData[u8GSNo].bRectFail = (RxMsg[u8Idx].Data[6] >> 1) & 0x01;
						PData->RectData[u8GSNo].bDC_Off = (RxMsg[u8Idx].Data[6]) & 0x01;

						PData->RectData[u8GSNo].bStatCode[0] = RxMsg[u8Idx].Data[7];
						PData->RectData[u8GSNo].bDisCh_Err = (RxMsg[u8Idx].Data[7] >> 5) & 0x01;
						PData->RectData[u8GSNo].bOn_Sleep = (RxMsg[u8Idx].Data[7] >> 4) & 0x01;
						PData->RectData[u8GSNo].bPFC_Err = (RxMsg[u8Idx].Data[7] >> 3) & 0x01;
						PData->RectData[u8GSNo].bInternalCommErr = (RxMsg[u8Idx].Data[7] >> 2) & 0x01;
						PData->RectData[u8GSNo].bInternalDC_LowVolt = (RxMsg[u8Idx].Data[7] >> 1) & 0x01;
						PData->RectData[u8GSNo].bDC_Shorted = (RxMsg[u8Idx].Data[7]) & 0x01;
						break;
					case 0x06:
						PData->RectData[u8GSNo].fAB_Volt = 0.1*((float)(RxMsg[u8Idx].Data[0] << 8) + RxMsg[u8Idx].Data[1]);
						PData->RectData[u8GSNo].fBC_Volt = 0.1*((float)(RxMsg[u8Idx].Data[2] << 8) + RxMsg[u8Idx].Data[3]);
						PData->RectData[u8GSNo].fCA_Volt = 0.1*((float)(RxMsg[u8Idx].Data[4] << 8) + RxMsg[u8Idx].Data[5]);
						break;
					case 0x07:
						if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
						{
							pSingleRectData->fDC_Ver  = (float)((RxMsg[u8Idx].Data[0]<<8) + RxMsg[u8Idx].Data[1]) * 0.01;
							pSingleRectData->fAC_Ver  = (float)((RxMsg[u8Idx].Data[2]<<8) + RxMsg[u8Idx].Data[3]) * 0.01;
							pSingleRectData->fHW_Ver  = (float)((RxMsg[u8Idx].Data[4]<<8) + RxMsg[u8Idx].Data[5]) * 0.01;
						}
						else
						{
							PData->RectData[u8GSNo].fDC_Ver  = (float)(RxMsg[u8Idx].Data[0]) + (float)(RxMsg[u8Idx].Data[1] >> 4) * 0.1 + (float)(RxMsg[u8Idx].Data[1] & 0x0F) * 0.01;
							PData->RectData[u8GSNo].fAC_Ver  = (float)(RxMsg[u8Idx].Data[2]) + (float)(RxMsg[u8Idx].Data[3] >> 4) * 0.1 + (float)(RxMsg[u8Idx].Data[3] & 0x0F) * 0.01;
							PData->RectData[u8GSNo].fHW_Ver = (float)(RxMsg[u8Idx].Data[4]) + (float)(RxMsg[u8Idx].Data[5] >> 4) * 0.1 + (float)(RxMsg[u8Idx].Data[5] & 0x0F) * 0.01;
						}
						break;
					case 0x1A:
						break;
					case 0x0c:
						PData->RectData[u8GSNo].fOutSiteVolt = (float)(((RxMsg[u8Idx].Data[0] << 8) + RxMsg[u8Idx].Data[1])*0.1);
						PData->RectData[u8GSNo].fOutSiteMaxCurr = 0.1*((float)(RxMsg[u8Idx].Data[2] << 8) + RxMsg[u8Idx].Data[3]);
						break;

					default:
						break;
					}
				}
			}
		}
	}

	return u8Rtn;
}

ERR_CODE_CAN_R GetSingle_VoltCurr(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
    UINT8 u8Rtn;
	//SGNo 模块地址0-1-2-3...
	UINT8 u8CMD = 0x03;	 
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(20);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		s_fConvertOutVoltBuf = 0;
		s_fConvertOutCurrBuf = 0;
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);
                
        if(ERR_CAN_R_NO_ERR == u8Rtn)
		{
		
			if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
			{
				if((SGNo >= ACDC_GRP_NO_MIN)&&(SGNo <= ACDC_GRP_NO_MAX))
				{		
					if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
					{
						//gs_CANComm.GrpData[SGNo].fOutVolt = s_fConvertOutVoltBuf;
						//gs_CANComm.GrpData[SGNo].fOutCurr = s_fConvertOutCurrBuf;
						gs_CANComm.GrpData[SGNo].fDCInputCurr = s_fConvertOutCurrBuf;
					}
					else
					{
						//gs_CANComm.GrpData[SGNo].fOutVolt = s_fConvertOutVoltBuf;					//1或2组 系统电压
						gs_CANComm.GrpData[SGNo].fOutCurr = s_fConvertOutCurrBuf;

						gs_CANComm.GrpData[SGNo].fDCInputVolt = 0;
						gs_CANComm.GrpData[SGNo].fDCInputCurr = 0;
					}
				}
			}
			else
			{
				gs_CANComm.GrpData[SGNo].fOutVolt = s_fConvertOutVoltBuf;
				//gs_CANComm.GrpData[SGNo].fExtVolt = s_fConvertOutVoltBuf;
				gs_CANComm.GrpData[SGNo].fOutCurr = s_fConvertOutCurrBuf;
			}

		}
		
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}
ERR_CODE_CAN_R GetSingle_VoltCurr_Dev_0A(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
    UINT8 u8Rtn;
	//SGNo 模块地址0-1-2-3...
	UINT8 u8CMD = 0x03;	 
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0A,u8CMD, SGNo, 0, 0);
	OSTimeDly(3);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0A, u8CMD, SGNo, PData);	
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}
//ERR_CODE_CAN_R GetSingle_Volt_0x23(int SGNo, void* pRawData,void* pSetData)
//{
//	UINT32 u32DataH = 0x41010000;
//	UINT8 u8Rtn,u8RcvMsgNum;
//	UINT8 u8CMD = 0x23;
//	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
//	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
//	OSTimeDly(10);
//	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
//	if (u8RcvMsgNum > 0)
//	{
//		s_fConvertOutVoltBuf = 0;
//		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
//		gs_CANComm.GrpData[SGNo].fOutVolt = s_fConvertOutVoltBuf;
//		gs_CANComm.GrpData[SGNo].fExtVolt = s_fConvertOutVoltBuf;		
//	}
//	else
//	{
//		u8Rtn = ERR_CAN_R_COMM_FAIL;
//	}
//
//	return (ERR_CODE_CAN_R)u8Rtn;
//}

ERR_CODE_CAN_R GetSingle_Curr_0x23(int SGNo, void* pRawData,void* pSetData)
{
	UINT32 u32DataH = 0x41020000;
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(10);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		s_fConvertOutCurrBuf = 0;
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
		gs_CANComm.GrpData[SGNo].fOutCurr = s_fConvertOutCurrBuf;
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetSingle_GrpAndStatus(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
        UINT8 u8Rtn;
	UINT8 u8CMD = 0x04;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(10);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);		
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetSingle_PhaseVolts(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
        UINT8 u8Rtn;
	UINT8 u8CMD = 0x06;	 
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(8);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);		
	}
	else
	{	
		u8Rtn = ERR_CAN_R_NO_ERR;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetSingle_SerialNo(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
        UINT8 u8Rtn;
	UINT8 u8CMD = 0x0B;	 
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(8);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);		
	}
	else
	{	
		u8Rtn = ERR_CAN_R_NO_ERR;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetSingle_VerInfo(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
        UINT8 u8Rtn;
	//SGNo 模块地址0-1-2-3...
	UINT8 u8CMD = 0x07;	 
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(10);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetSingle_VerInfo_SingleModule(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
	UINT8 u8Rtn;
	//SGNo 模块地址0-1-2-3...
	UINT8 u8CMD = 0x07;	 

	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0A, u8CMD, SGNo, 0, 0);	
	OSTimeDly(15);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0A, u8CMD, SGNo, PData);
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R SetSingle_OnOff(int SGNo, void* pRawData,void* pSetData)
{
	return ERR_CAN_R_NO_ERR;
}

ERR_CODE_CAN_R SetSingle_LedFlash(int SGNo, void* pRawData,void* pSetData)
{
	return ERR_CAN_R_NO_ERR;
}

ERR_CODE_CAN_R SetSingle_Reset(int SGNo, void* pRawData,void* pSetData)
{
	return ERR_CAN_R_NO_ERR;
}

ERR_CODE_CAN_R GetGrp_MaxCapacity(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
    UINT8 u8Rtn;
	//SGNo 组号	1 2
	UINT8 u8CMD = 0x0A;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	if(SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_WRONG_ADDR;
	}

	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(20);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		s_fGrpMinVoltBuf = 50;
		s_fGrpMaxVoltBuf = 50;
		s_fGrpRateCurrBuf = 0;
		s_fGrpRatePowerBuf = 0;

		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
		if(ERR_CAN_R_NO_ERR == u8Rtn)
		{
			if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
			{
				if(!(SGNo >= ACDC_GRP_NO_MIN && SGNo <= ACDC_GRP_NO_MAX))
				{
					PData->GrpData[SGNo].fRatedPower = s_fGrpRatePowerBuf;
				}
			}
			else
			{
				PData->GrpData[SGNo].fRatedPower = s_fGrpRatePowerBuf;
			}
	
			PData->GrpData[SGNo].fMax_OutCurr = s_fGrpRateCurrBuf;
			PData->GrpData[SGNo].fRatedMaxCurr = s_fGrpRateCurrBuf;	

			PData->GrpData[SGNo].fMax_OutVolt = s_fGrpMaxVoltBuf;
			PData->GrpData[SGNo].fMin_OutVolt = s_fGrpMinVoltBuf;
		}
	}
	else
	{
		u8Rtn = ERR_CAN_R_WRONG_ADDR;		
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetGrp_ExtVolt(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
        UINT8 u8Rtn;
	//SGNo 组号	1 2
	UINT8 u8CMD = 0x0C;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	if(SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_WRONG_ADDR;
	}

	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(20);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		s_fGrpExtVolt = 0;
		s_fGrpAvailableCurr = 0;
		s_fGrpAvailablePwr = 0;
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
		if(ERR_CAN_R_NO_ERR == u8Rtn)
		{
			if(gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE)  //PSC模块改成在01命令中查询
			{
				PData->GrpData[SGNo].fExtVolt = s_fGrpExtVolt;  //20221206 LKF 当前PCS上报0C命令都是0
			}
			PData->GrpData[SGNo].fAvailableCurr = s_fGrpAvailableCurr;
			PData->GrpData[SGNo].fAvailablePwr = s_fGrpAvailablePwr;

			//PData->GrpData[SGNo].fExtVolt = s_fGrpExtVolt;
			PData->GrpData[SGNo].fOutSiteMaxCurr = s_fGrpAvailableCurr;
			PData->GrpData[SGNo].f_Power_ofThisVoltMax = s_fGrpAvailablePwr;

			if(gs_CANComm.GrpData[SGNo].u8PowerofThisVoltMaxIsValidFlag == 1)
			{
				gs_CANComm.GrpData[SGNo].u8PowerofThisVoltMaxIsValidFlag = 2;
			}
		}
	}
	else
	{
		u8Rtn = ERR_CAN_R_WRONG_ADDR;		
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetGrp_NumAndStatus(int SGNo, void* pRawData,void* pSetData)               //模块组的数量及状态
{
	UINT8 u8RcvMsgNum;
        UINT8 u8Rtn;
	//SGNo 组号 1 2
	UINT8 u8CMD = 0x02;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	if(SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_WRONG_ADDR;
	}

	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(5);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);		
	}
	else
	{
		u8Rtn = ERR_CAN_R_WRONG_ADDR;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetSingle_ConvertStat(int SGNo, void* pRawData,void* pSetData)
{
	UINT32 u32DataH; 

if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
	 u32DataH = 0x11120000;
else
	 u32DataH = 0x11110000;

	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(10);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);		
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetSingle_ConvertStat_ex_PCS(int SGNo, UINT32 u32CmdDataH)
{
	UINT32 u32DataH; 

	u32DataH = u32CmdDataH;	

	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = &gs_CANComm;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(5);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);		
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

/**
 * @brief 获取指定组的电压电流数据
 * @param SGNo 组号(1-2)
 * @param pRawData 原始数据缓冲区指针,类型为CANCommData_TypeDef*
 * @param pSetData 设置数据缓冲区指针(未使用)
 * @return ERR_CODE_CAN_R 错误码
 *         - ERR_CAN_R_WRONG_ADDR: 组号无效或通信失败
 *         - 其他值: 参见UnPackData函数返回值
 * @note 通过CAN总线读取指定组的电压电流信息,使用命令0x01
 */
ERR_CODE_CAN_R GetGrp_VoltCurr(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
	UINT8 u8Rtn;
	//SGNo 组号	1 2
	UINT8 u8CMD = 0x01;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	if(SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_WRONG_ADDR;
	}

	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(5);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);

	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);			
	}
	else
	{
		u8Rtn = ERR_CAN_R_WRONG_ADDR;		
	}
	return (ERR_CODE_CAN_R)u8Rtn;


	//UINT8 u8RcvMsgNum;
	//UINT8 u8Rtn;
	////SGNo 组号	1 2
	//UINT8 u8CMD = 0x03;				//读模块信息
	//CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	//u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	//OSTimeDly(8);
	//u8RcvMsgNum = u8RcvMsg(40);
	//if (u8RcvMsgNum > 0)
	//{
	//	u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);		
	//}
	//else
	//{
	//	u8Rtn = ERR_CAN_R_WRONG_ADDR;
	//}
	//return (ERR_CODE_CAN_R)u8Rtn;
}


// 20221206 LKF 这个目前是DCDC模块调用
/**
 * @brief 获取指定组号的电压值(命令0x23)
 * @param SGNo 组号
 * @param pRawData 原始数据指针,类型为CANCommData_TypeDef*
 * @param pSetData 设置数据指针(未使用)
 * @return ERR_CODE_CAN_R 返回CAN通信错误码
 *         - ERR_CAN_R_COMM_FAIL: 通信失败
 *         - 其他值: UnPackData函数返回值
 * @note 该函数通过CAN总线读取电源柜组电压,并更新全局结构体gs_CANComm中的相关电压数据
 */
ERR_CODE_CAN_R GetGrp_Volt_0X23(int SGNo, void *pRawData, void *pSetData)
{
	UINT32 u32DataH = 0x41010000;
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(10);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		s_fConvertOutVoltBuf = 0;
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);		
		gs_CANComm.GrpData[SGNo].fOutVolt = s_fConvertOutVoltBuf;
		gs_CANComm.GrpData[SGNo].fExtVolt = s_fConvertOutVoltBuf;

		gs_CANComm.GrpData[SGNo].fRealTime_Volt_BattSide = s_fConvertOutVoltBuf;

	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetGrp_Curr_0X23(int SGNo, void* pRawData,void* pSetData)
{
	UINT32 u32DataH = 0x41020000;
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		s_fConvertOutCurrBuf = 0;
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
		gs_CANComm.GrpData[SGNo].fOutCurr = s_fConvertOutCurrBuf;

		gs_CANComm.GrpData[SGNo].fRealTime_Curr_BattSide = s_fConvertOutVoltBuf;
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetRECT_bStatFaltCode1110_PCS_0X23(int SGNo, void* pRawData,void* pSetData)      //by lxr01202 新增模块1110故障码
{
	UINT32 u32DataH = 0x11100000;    //0x11   0x10
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	

	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetRECT_bStatFaltCode1111_PCS_0X23(int SGNo, void* pRawData,void* pSetData)            //by lxr01202 新增模块1111故障码
{
	UINT32 u32DataH = 0x11110000;    //0x11   0x11
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	

	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetRECT_TEM_0X23(int SGNo, void* pRawData,void* pSetData)       //by lxr01202 新增模块内部温度
{
	UINT32 u32DataH = 0x12020000;    //0x12   0x02
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	

	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}



ERR_CODE_CAN_R GetRECT_MAXTEM_0X23(int SGNo, void* pRawData,void* pSetData)      //by lxr01202 新增模块内部功率器件最高温度
{
	UINT32 u32DataH = 0x12030000;    //0x12   0x03
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	

	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}


ERR_CODE_CAN_R GetMPPT_DCTEM_0X0E(int SGNo, void* pRawData,void* pSetData)      //by lxr01202 新增MPPT模块内部DC板温温度
{
	UINT32 u32DataH = 0x11070000;    //0x11   0x07
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x0E;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}


ERR_CODE_CAN_R GetMPPT_PFCTEM_0X0E(int SGNo, void* pRawData,void* pSetData)      //by lxr01202 新增MPPT模块内部PFC板温温度
{
	UINT32 u32DataH = 0x11080000;    //0x11   0x08
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x0E;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	

	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetMPPT_DC_TRANS_TEM_0X0E(int SGNo, void* pRawData,void* pSetData)      //by lxr01202 新增MPPT模块内部DC变压器温度
{
	UINT32 u32DataH = 0x12020000;    //0x12   0x02
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x0E;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	

	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetMPPT_PFC_TRANS_TEM_0X0E(int SGNo, void* pRawData,void* pSetData)      //by lxr01202 新增MPPT模块内部PFC变压器温度
{
	UINT32 u32DataH = 0x12030000;    //0x12   0x03
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x0E;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	

	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}


ERR_CODE_CAN_R GetGrp_ActivePower_0X23(int SGNo, void* pRawData,void* pSetData)
{
	UINT32 u32DataH = 0x01020000;
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(5);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}
ERR_CODE_CAN_R GetGrp_ReactivePower_0X23(int SGNo, void* pRawData,void* pSetData)
{
	UINT32 u32DataH = 0x01030000;
	UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x23;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(5);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}
void Float2u32Data(UINT32 *u32Data, float fVal);
/**
 * @brief 将浮点数转换为无符号32位整数数据。
 *
 * 此函数接收一个浮点数，并将其转换为无符号32位整数格式。
 * 转换过程中使用了联合体来处理浮点数的字节表示。
 *
 * @param[out] u32Data 指向存储转换结果的无符号32位整数指针。
 * @param[in] fVal 要转换的浮点数值。
 */
void Float2u32Data(UINT32 *u32Data, float fVal)
{
	__XXX_UNION_VALUE unVal;

	unVal.ftemp = fVal;
	*u32Data = (unVal.chtemp[3]<<24) + (unVal.chtemp[2]<<16) + (unVal.chtemp[1]<<8) + (unVal.chtemp[0]);
	return;
	//for(UINT8 i = 0; i < 4; i++)
	//{
	//	unVal.chtemp[4 - i - 1] = pData[i];
	//}
	//return unVal.ftemp;
}

/**
 * @brief 设置 IIN 限制
 *
 * 此函数用于设置 IIN 限制，仅适用于 REG1K0100G 设备。
 * 根据设备编号（0x0A 或 0x0B），确定目标地址并发送相应的 CAN 消息。
 * 如果总矩形数量大于等于 1，则根据设置的 IIN 限制值进行计算；否则，默认限制为 500。
 *
 * @param SGNo 设备编号
 * @param pRawData 原始数据指针（未使用）
 * @param pSetData 设置数据指针（未使用）
 * @return ERR_CODE_CAN_R 返回错误代码，成功时返回 ERR_CAN_R_NO_ERR
 */
ERR_CODE_CAN_R SetBroad_IinLimit(int SGNo, void *pRawData, void *pSetData)
{
	// Set IIN limit
	//	(only for REG1K0100G)	0x12	0x05	0	0	IIN limit (float point)
	//	The destination address is 0x3F  if the device No.is 0x0A.
	//	The destination address is group No. if the device No.is 0x0B
	UNUSED(pRawData);
	UNUSED(pSetData);
	UINT8 u8CMD = 0x0F;	
	UINT32 uiDH, uiDL;
	UINT8 u8Rst = FALSE;
	UNUSED(u8Rst);
	canmsg SendMsg;
	if(gs_CANComm.iTotalRectNum >=1)
	{
		//if(gEvChargeInfo.fSET_Rect_IinLimit)
		gEvChargeInfo.fCtrl_Rect_IinLimit = (gEvChargeInfo.fSET_Rect_IinLimit/gs_CANComm.iTotalRectNum);
	}
	else
	{
		gEvChargeInfo.fCtrl_Rect_IinLimit = 500;//不限制！!
	}

	gEvChargeInfo.fCtrl_Rect_IinLimit = MAX(2, gEvChargeInfo.fCtrl_Rect_IinLimit);
	//__XXX_UNION_VALUE unVal;
	Float2u32Data(&uiDL, gEvChargeInfo.fCtrl_Rect_IinLimit);
	uiDH = 0x12050000;
	SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (u8CMD << 16) + ((R_PROTO_BROADCAST) << 8) + R_PROTO_SELF_ADDR;
	SendMsg.Data[0] = uiDH;
	SendMsg.Data[1] = uiDL;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	ClenRBuff();
	u8Rst = gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(3);
	return ERR_CAN_R_NO_ERR;
}

/**
 * @brief 设置指定组的开关状态
 *
 * 该函数用于控制指定组的模块开关状态。根据传入的组号和设备类型，发送相应的广播命令以开启或关闭模块。
 *
 * @param SGNo 组号，指定要控制的模块组
 * @param pRawData 指向原始数据的指针，包含模块的当前状态信息
 * @param pSetData 指向设置数据的指针，未使用
 *
 * @return ERR_CODE_CAN_R 返回错误代码，成功时返回 ERR_CAN_R_NO_ERR
 */
ERR_CODE_CAN_R SetGrp_OnOff_No10(int SGNo, void *pRawData, void *pSetData)
{
	// UINT8 u8Rst = FALSE;
	// UINT8 u8Idx = 0;
	// 1为关机，0为开机
	// 注：该命令为广播命令，无回复。设备号为0x0A时，命令中目标地址的模块地址为3F；设备号为0x0B时，目标地址为相应的组号。
	// 监控发送： 02 9A 3F F0  01 00 00 00 00 00 00 00——设置所有模块关机
	// 模块回复：广播无回复
	// 监控发送： 02 9A 00 F0  00 00 00 00 00 00 00 00——设置0#模块开机
	// 模块回复： 02 9A F0 00  00 00 00 00 00 00 00 00——0#模块回复当前为开机
	// 如果地址0#模块的组号是2#，用组设备号设置（该组的所有模块以模块地址号回复）
	// 监控发送： 02 DA 02 F0  01 00 00 00 00 00 00 00——设置组2模块关机
	// 模块回复： 02 DA F0 00  01 00 00 00 00 00 00 00——组2的模块（地址0）当前设定
	UINT32 u32Val = 0;
	UINT8 u8CMD = 0x1A;			//1A 控制模块开关！！广播指令无回复！	设备号为A时，3F 广播		设备号为B时,	目标地址为组号
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
		u32Val = (PData->u8GRP_RectOnOff[SGNo]<<24);
	else
	{
		//20211120 lkf 增加1KV模块的高低压模式切换处理
		if(PData->u8GRP_SerialParallelModeSet_Flag[SGNo] > 0)
		{
			u32Val = ((UINT32)RECT_OFF<<24); 
		}
		else
		{
			u32Val = (PData->u8GRP_RectOnOff[SGNo]<<24);
		}
	}

	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32Val, 0);
	OSTimeDly(3);

	if(PData->u8GRP_RectOnOff[SGNo] == RECT_OFF)
	{
		gs_CANComm.GrpData[SGNo].u8PowerofThisVoltMaxIsValidFlag = 0;
	}
	else if(gs_CANComm.GrpData[SGNo].u8PowerofThisVoltMaxIsValidFlag == 0)
	{
		gs_CANComm.GrpData[SGNo].u8PowerofThisVoltMaxIsValidFlag = 1;
	}

	return ERR_CAN_R_NO_ERR;
}

/**
 * @brief 设置串行/并行模式
 *
 * 此函数根据给定的组号和原始数据设置串行或并行模式。
 * 如果模式设置为串行，则发送特定的命令和数据；
 * 如果模式未定义，则发送默认数据。
 *
 * @param SGNo 组号，用于指定要设置的模式
 * @param pRawData 指向原始数据的指针，包含当前模式信息
 * @param pSetData 指向设置数据的指针，未使用
 *
 * @return ERR_CODE_CAN_R 返回错误代码，表示操作结果
 */
ERR_CODE_CAN_R SetGrp_SerialParellMode(int SGNo, void *pRawData, void *pSetData)
{
	UINT32 u32DataH, u32DataL;
	//UINT8 u8Rtn,u8RcvMsgNum;
	//	float ftemp;
	UINT8 u8CMD = 0x0F;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	u32DataH = 0x11140000;

	//20211120 lkf 增加1KV模块的高低压模式切换处理
	if(PData->u8GRP_SerialParallelModeSet[SGNo] == GRP_SerialParallelMode_SET_Serial)
	{
		u32DataL = 0xA1; 
	}
	/*else if(PData->u8GRP_SerialParallelModeSet[SGNo] == GRP_SerialParallelMode_SET_Parall)
	{
		u32DataL = 0xA0; 
	}*/
	else 
	{
		u32DataL = 0xA2; 
	}

	//u32DataL = 0xA1; 
	
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, u32DataL);

	OSTimeDly(5);

	return ERR_CAN_R_NO_ERR;
}

ERR_CODE_CAN_R SetGrp_OnOff_4RingSlaveGrp(UCHAR SGNo, UCHAR u8SetData)
{
	canmsg SendMsg;
	//UINT8 u8Val = (UINT8)*pSetData;
	UINT8 u8CMD = 0x1A;			//1A 控制模块开关！！广播指令无回复！	设备号为A时，3F 广播		设备号为B时,	目标地址为组号


	if ((SGNo < 1) || (SGNo > 16))
	{
		return ERR_CAN_R_WRONG_ADDR;
	}	

	SendMsg.Id = (R_PRPTO_DEVNO_0B << 22) + ((UINT32)u8CMD << 16) + ((UINT32)SGNo << 8) + R_PROTO_SELF_ADDR;			//组 1 2 

	SendMsg.Data[0] = ((UINT32)u8SetData << 24);
	SendMsg.Data[1] = 0;
	//ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);

	OSTimeDly(5);

	return ERR_CAN_R_NO_ERR;
}

ERR_CODE_CAN_R SetGrp_WalkIn(int SGNo, void* pRawData,void* pSetData)
{

	UINT8 u8CMD = 0x13;									//设置walkin
	__XXX_UNION_VALUE untemp;
	__16BITS_UNION_VALUE untemp_16bits;
	UINT16 u16RectLoadStartTime;
	UINT32 uiDH,uiDL;

	if (SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_COMM_FAIL;
	}

	u16RectLoadStartTime = *(UINT16*)pSetData;

	untemp.u32Value = 0;
	if(u16RectLoadStartTime == 0)
	{
		untemp.chtemp[3] = 0;						//禁止
	}
	else
	{
		untemp.chtemp[3] = 1;						//使能
	}
	
	uiDH = untemp.u32Value;

	untemp_16bits.u16Temp = u16RectLoadStartTime;

	untemp.u32Value = 0;
	untemp.chtemp[1] = untemp_16bits.chtemp[1];
	untemp.chtemp[0] = untemp_16bits.chtemp[0];
	uiDL = untemp.u32Value;


	u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
	OSTimeDly(3);

	return ERR_CAN_R_NO_ERR;
}
ERR_CODE_CAN_R SetDCDCGrp_WorkMode_copy(int SGNo, void* pRawData,void* pSetData, UCHAR u8FistFlag)
{

	UINT8 u8CMD = 0x24;									//设置walkin
	__XXX_UNION_VALUE untemp;
	__16BITS_UNION_VALUE untemp_16bits;
	UINT32 uiDH,uiDL;

	

	if (SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_COMM_FAIL;
	}

	if (((SGNo >= DCDC_GRP_NO_MIN) && (SGNo <= DCDC_GRP_NO_MAX))
		|| ((SGNo >= MPPT_GRP_NO_MIN) && (SGNo <=MPPT_GRP_NO_MAX)))
	{
		if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_MPPT)
		{
			u8CMD = 0x0F;

			untemp.u32Value = 0;
			untemp.chtemp[3] = 0x11;   
			untemp.chtemp[2] = 0x11;   
			uiDH = untemp.u32Value;

			untemp.u32Value = 0;
			untemp.chtemp[0] = 0xA1; 
			uiDL = untemp.u32Value;

			u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
			OSTimeDly(3);
		}
		else
		{
			//主要给DCDC模块恢复成DCDC模式
			if(u8FistFlag > 0)
			{
				u8CMD = 0x0F;
				untemp.u32Value = 0;
				untemp.chtemp[3] = 0x11;   
				untemp.chtemp[2] = 0x11;   
				uiDH = untemp.u32Value;

				untemp.u32Value = 0;
				untemp.chtemp[0] = 0xA0; 
				uiDL = untemp.u32Value;
				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(3);
			}
		}
	}

	if ((SGNo >= ACDC_GRP_NO_MIN) && (SGNo <= ACDC_GRP_NO_MAX))
	{
		if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
		{
			if(u8FistFlag > 0)
			{
				//设置并网模式
				untemp.u32Value = 0;
				untemp.chtemp[3] = 0x21;   
				untemp.chtemp[2] = 0x10;   
				uiDH = untemp.u32Value;

				untemp_16bits.u16Temp = (UINT16)0xA1;  //A1-并网  A2-离网
				untemp.u32Value = 0;
				untemp.chtemp[1] = untemp_16bits.chtemp[1];
				untemp.chtemp[0] = untemp_16bits.chtemp[0];
				uiDL = untemp.u32Value;

				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(3);	
			}

			if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
			{
				//直流起机模式
				untemp.u32Value = 0;
				untemp.chtemp[3] = 0x11;   
				untemp.chtemp[2] = 0x27;   
				uiDH = untemp.u32Value;

				//20220414 LKF 通过HXM沟通，JN储能应用必须直流起机模式
				untemp_16bits.u16Temp = (UINT16)0xA0;  //直流起机模式

				//if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
				//{
				//	untemp_16bits.u16Temp = (UINT16)0xA0;  //直流起机模式
				//}
				//else
				//{
				//	untemp_16bits.u16Temp = (UINT16)0xA1;  //交流起机模式
				//}

				untemp.u32Value = 0;
				untemp.chtemp[1] = untemp_16bits.chtemp[1];
				untemp.chtemp[0] = untemp_16bits.chtemp[0];
				uiDL = untemp.u32Value;

				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(3);
			}


			//20220414 LKF 通过HXM沟通，JN储能应用必须直流调度模式
			untemp.u32Value = 0;
			untemp.chtemp[3] = 0x31;   
			untemp.chtemp[2] = 0x11;   
			uiDH = untemp.u32Value;
			untemp_16bits.u16Temp = (UINT16)0xA0;  //指流调度模式
			untemp.u32Value = 0;
			untemp.chtemp[1] = untemp_16bits.chtemp[1];
			untemp.chtemp[0] = untemp_16bits.chtemp[0];
			uiDL = untemp.u32Value;

			u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
			OSTimeDly(3);
//by lxr01202  由于A2419带外部N继电器，需要快速并离网切换，所以需要模块N继电器常闭，所以本系统需要设置A1。0x211B，A0不吸合（默认状态），A1吸合
			untemp.u32Value = 0;
			untemp.chtemp[3] = 0x21;   
			untemp.chtemp[2] = 0x1B;   
			uiDH = untemp.u32Value;
			untemp_16bits.u16Temp = (UINT16)0xA1;  //外部N继电器常闭
			untemp.u32Value = 0;
			untemp.chtemp[1] = untemp_16bits.chtemp[1];
			untemp.chtemp[0] = untemp_16bits.chtemp[0];
			uiDL = untemp.u32Value;

			u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
			OSTimeDly(3);
//by lxr01202   新增41 02命令    DCDC模块直流高压侧电流（mA）  （A2419）
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x41;   
					untemp.chtemp[2] = 0x02;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0x00;  
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

//by lxr01202     
			        untemp.u32Value = 0;
			        untemp.chtemp[3] = 0x21;   
			        untemp.chtemp[2] = 0x10;   
			        uiDH = untemp.u32Value;

					if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
						{
							untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
						}
					else
						{
							untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
						}
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(5);
			//20220414 LKF 通过HXM沟通，JN储能应用必须恒流模式
			untemp.u32Value = 0;
			untemp.chtemp[3] = 0x31;   
			untemp.chtemp[2] = 0x12;   
			uiDH = untemp.u32Value;
			untemp_16bits.u16Temp = (UINT16)0xA0;  //横流模式
			untemp.u32Value = 0;
			untemp.chtemp[1] = untemp_16bits.chtemp[1];
			untemp.chtemp[0] = untemp_16bits.chtemp[0];
			uiDL = untemp.u32Value;

			u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
			OSTimeDly(3);

			//20220421 LKF 通过HXM沟通，并离网系统设置，纯并网系统，对直流侧电压最低值可以低至600V
			untemp.u32Value = 0;
			untemp.chtemp[3] = 0x21;   
			untemp.chtemp[2] = 0x41;   
			uiDH = untemp.u32Value;
			untemp_16bits.u16Temp = (UINT16)0xA1;  //纯并网模式
			untemp.u32Value = 0;
			untemp.chtemp[1] = untemp_16bits.chtemp[1];
			untemp.chtemp[0] = untemp_16bits.chtemp[0];
			uiDL = untemp.u32Value;

			u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
			OSTimeDly(3);
		}
		else                                                    
		{
		  // BEG 模块
			u8CMD = 0x24;	
			untemp.u32Value = 0;
			untemp.chtemp[3] = 0x21;   
			untemp.chtemp[2] = 0x10;   
			uiDH = untemp.u32Value;

			if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
			{
				untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
			}
			else
			{
				untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
			}
			untemp.u32Value = 0;
			untemp.chtemp[1] = untemp_16bits.chtemp[1];
			untemp.chtemp[0] = untemp_16bits.chtemp[0];
			uiDL = untemp.u32Value;

			u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
			OSTimeDly(5);
		}


	}


	//DCDC 模块
	if((SGNo >= DCDC_GRP_NO_MIN) && (SGNo <=DCDC_GRP_NO_MAX))
	{
		u8CMD = 0x24;	
		untemp.u32Value = 0;
		untemp.chtemp[3] = 0x21;   
		untemp.chtemp[2] = 0x10;   
		uiDH = untemp.u32Value;

		if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
		{
			untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
		}
		else
		{
			untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
		}
		untemp.u32Value = 0;
		untemp.chtemp[1] = untemp_16bits.chtemp[1];
		untemp.chtemp[0] = untemp_16bits.chtemp[0];
		uiDL = untemp.u32Value;

		u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
		OSTimeDly(5);
	}

	return ERR_CAN_R_NO_ERR;
}

#if 1
ERR_CODE_CAN_R SetDCDCGrp_WorkMode(int SGNo, void* pRawData,void* pSetData, UCHAR u8FistFlag)
{

	UINT8 u8CMD = 0x24;									//设置walkin
	__XXX_UNION_VALUE untemp;
	__16BITS_UNION_VALUE untemp_16bits;
	UINT32 uiDH,uiDL,uiflag;
	float fChargePower,fDisChargePower;
	union
	{
		UINT32 u32temp;
		INT32 i32temp;
	}union_temp;
	

	if (SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_COMM_FAIL;
	}

	if (((SGNo >= DCDC_GRP_NO_MIN) && (SGNo <= DCDC_GRP_NO_MAX))
		|| ((SGNo >= MPPT_GRP_NO_MIN) && (SGNo <=MPPT_GRP_NO_MAX)))
	{
		if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_MPPT)                      //设置为MPPT模式
		{
			u8CMD = 0x0F;

			untemp.u32Value = 0;
			untemp.chtemp[3] = 0x11;   
			untemp.chtemp[2] = 0x11;   
			uiDH = untemp.u32Value;

			untemp.u32Value = 0;
			untemp.chtemp[0] = 0xA1; 
			uiDL = untemp.u32Value;

			u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
			OSTimeDly(3);
		}
		else
		{
			//主要给DCDC模块恢复成DCDC模式                                                        //设置成DCDC模式
			if(u8FistFlag > 0)
			{
				u8CMD = 0x0F;
				untemp.u32Value = 0;
				untemp.chtemp[3] = 0x11;   
				untemp.chtemp[2] = 0x11;   
				uiDH = untemp.u32Value;

				untemp.u32Value = 0;
				untemp.chtemp[0] = 0xA0; 
				uiDL = untemp.u32Value;
				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(3);
			}
		}
	}

	if ((SGNo >= ACDC_GRP_NO_MIN) && (SGNo <= ACDC_GRP_NO_MAX))
	{
            if(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE)                                       //35KW
		{
			if(u8FistFlag > 0)
			{
				//设置并网模式
				untemp.u32Value = 0;
				untemp.chtemp[3] = 0x21;   
				untemp.chtemp[2] = 0x10;   
				uiDH = untemp.u32Value;

				untemp_16bits.u16Temp = (UINT16)0xA1;  //A1-并网  A2-离网
				untemp.u32Value = 0;
				untemp.chtemp[1] = untemp_16bits.chtemp[1];
				untemp.chtemp[0] = untemp_16bits.chtemp[0];
				uiDL = untemp.u32Value;

				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(3);	
			}

			if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)                          //自动模式
			{
				if(gEvChargeInfo.u8TaskIsAcDispatchMode == TRUE)                   //AC调度模式
				{

//by lxr01202 新增交流调度下  设置储能模式 21 18 A1使能
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x21;   
					untemp.chtemp[2] = 0x18;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA1;  //储能模式
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);

//设置交流调度模式
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0x11;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA1;  //交流调度模式
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);


//by lxr01202  新增0x24 31 24 AC有功降载速率设置命令（A2419）
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0x24;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = 3000;  
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);


//by lxr01202  新增0x24 31 25 AC有功升载速率设置命令（A2419）
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0x25;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = 3000;  
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);

					//by lxr01202 新增交流调度下  设置储能模式的国别
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0xC1;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0x10;      //EN50549欧标
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);
//by lxr01202 新增交流调度下  设置有功功率	31 23 单位MW
//设置P
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0x23;   
					uiDH = untemp.u32Value;
					if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
					{
						fDisChargePower = (float)(gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].ChargeVoltReq * gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].EVDischargeCurrReq);
						if(fDisChargePower > (gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].BattInfoFromCan.BMS_Limit_DischargePower*1000))
						{
							fDisChargePower = gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].BattInfoFromCan.BMS_Limit_DischargePower*1000;
						}
						union_temp.i32temp = (INT32)((fDisChargePower * 1000) / (gs_CANComm.GrpData[SGNo].nCommRectNum));
					}
					else
					{
						fChargePower = (float)(gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].ChargeVoltReq * gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].ChargeCurrReq);
						if(fChargePower > (gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].BattInfoFromCan.BMS_Limit_ChargePower*1000))
						{
							fChargePower = gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].BattInfoFromCan.BMS_Limit_ChargePower*1000;
						}
						//union_temp.i32temp = (-(INT32)(fChargePower * 1000));
						union_temp.i32temp = (INT32)((fChargePower * 1000) / (gs_CANComm.GrpData[SGNo].nCommRectNum));
					}
					untemp.u32Value = union_temp.u32temp;
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);          //对组下发命令
					OSTimeDly(3);

					
//by lxr01202   新增0x24  21  14   孤岛检测命令
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x21;   
					untemp.chtemp[2] = 0x14;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA0;  //孤岛检测开启
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);

//by lxr01202     
			        untemp.u32Value = 0;
			        untemp.chtemp[3] = 0x21;   
			        untemp.chtemp[2] = 0x10;   
			        uiDH = untemp.u32Value;

					if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
						{
							untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
						}
					else
						{
							untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
						}
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(5);


					//开关机,这里开关机一定要放在设置有功功率后面，否则无法开机
					SetGrp_OnOff_No10(SGNo, &gs_CANComm, &gs_CANComm);

					//是否要设置Q
					if(gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].CtrlReactivePower != 0)
					{
						//无功模式
						untemp.u32Value = 0;
						untemp.chtemp[3] = 0x21;   
						untemp.chtemp[2] = 0x17;   
						uiDH = untemp.u32Value;
						untemp_16bits.u16Temp = (UINT16)0xA2;	//恒无功
						untemp.u32Value = 0;
						untemp.chtemp[1] = untemp_16bits.chtemp[1];
						untemp.chtemp[0] = untemp_16bits.chtemp[0];
						uiDL = untemp.u32Value;

						u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
						OSTimeDly(3);

						//设置Q
						float ftemp, ftemp1;
						untemp.u32Value = 0;
						untemp.chtemp[3] = 0x21;           //35KW模块才具备
						untemp.chtemp[2] = 0x08;   
						uiDH = untemp.u32Value;
						ftemp = gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].CtrlReactivePower;
						ftemp1 = gs_CANComm.GrpData[SGNo].fRatedPower;
						//union_temp.i32temp = (INT32)((ftemp*1000)/ftemp1);
                                                union_temp.i32temp = (INT32)((ftemp*1000));
						uiDL = union_temp.u32temp;

						u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
						OSTimeDly(3);
					}
					else
					{
						//设置无功使能
						untemp.u32Value = 0;
						untemp.chtemp[3] = 0x21;   
						untemp.chtemp[2] = 0x17;   
						uiDH = untemp.u32Value;
						untemp_16bits.u16Temp = (UINT16)0xA0;	//无功禁止
						untemp.u32Value = 0;
						untemp.chtemp[1] = untemp_16bits.chtemp[1];
						untemp.chtemp[0] = untemp_16bits.chtemp[0];
						uiDL = untemp.u32Value;

						u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
						OSTimeDly(3);
					}
				}
				else                                                                //自动模式下的直流调度
				{
					//20220414 LKF 通过HXM沟通，JN储能应用必须直流调度模式
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0x11;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA0;  //直流调度模式
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);

//by lxr01202 新增直流调度下  取消储能模式 21 18 A0使能
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x21;   
					untemp.chtemp[2] = 0x18;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA0;  //取消储能模式
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);


//by lxr01202  由于A2419带外部N继电器，需要快速并离网切换，所以需要模块N继电器常闭，所以本系统需要设置A1。0x211B，A0不吸合（默认状态），A1吸合
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x21;   
					untemp.chtemp[2] = 0x1B;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA1;  //外部N继电器常闭
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);

//by lxr01202   新增41 02命令    DCDC模块直流高压侧电流（mA）  （A2419）
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x41;   
					untemp.chtemp[2] = 0x02;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0x00;  
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;
					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);


//by lxr01202   新增0x24  21  14   孤岛检测命令
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x21;   
					untemp.chtemp[2] = 0x14;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA0;  //孤岛检测开启
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);

// //by lxr01202  新增0x24 31 23 AC有功功率设置命令（A2419）
// 					untemp.u32Value = 0;
// 					untemp.chtemp[3] = 0x31;   
// 					untemp.chtemp[2] = 0x23;   
// 					uiDH = untemp.u32Value;
// 					untemp_16bits.u16Temp = 0x00;  
// 					untemp.u32Value = 0;
// 					untemp.chtemp[1] = untemp_16bits.chtemp[1];
// 					untemp.chtemp[0] = untemp_16bits.chtemp[0];
// 					uiDL = untemp.u32Value;

// 					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
// 					OSTimeDly(3);




// //by lxr01202  新增0x24 11 01    模块直流电压 MV
// 					untemp.u32Value = 0;
// 					untemp.chtemp[3] = 0x11;   
// 					untemp.chtemp[2] = 0x01;   
// 					uiDH = untemp.u32Value;
// 					untemp_16bits.u16Temp = 0x00;  
// 					untemp.u32Value = 0;
// 					untemp.chtemp[1] = untemp_16bits.chtemp[1];
// 					untemp.chtemp[0] = untemp_16bits.chtemp[0];
// 					uiDL = untemp.u32Value;

// 					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
// 					OSTimeDly(3);

// //by lxr01202  新增0x24 11 02    模块直流电压 MV
// 					untemp.u32Value = 0;
// 					untemp.chtemp[3] = 0x11;   
// 					untemp.chtemp[2] = 0x02;   
// 					uiDH = untemp.u32Value;
// 					untemp_16bits.u16Temp = 0x00;  
// 					untemp.u32Value = 0;
// 					untemp.chtemp[1] = untemp_16bits.chtemp[1];
// 					untemp.chtemp[0] = untemp_16bits.chtemp[0];
// 					uiDL = untemp.u32Value;

// 					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
// 					OSTimeDly(3);

// //by lxr01202  新增0x24 11 03    模块组号
// 					untemp.u32Value = 0;
// 					untemp.chtemp[3] = 0x11;   
// 					untemp.chtemp[2] = 0x03;   
// 					uiDH = untemp.u32Value;
// 					untemp_16bits.u16Temp = 0x00;  
// 					untemp.u32Value = 0;
// 					untemp.chtemp[1] = untemp_16bits.chtemp[1];
// 					untemp.chtemp[0] = untemp_16bits.chtemp[0];
// 					uiDL = untemp.u32Value;

// 					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
// 					OSTimeDly(3);

//by lxr01202     
			        untemp.u32Value = 0;
			        untemp.chtemp[3] = 0x21;   
			        untemp.chtemp[2] = 0x10;   
			        uiDH = untemp.u32Value;

					if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
						{
							untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
						}
					else
						{
							untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
						}
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(5);

				}
			}
			else                                                                          //手动模式
			{
				//GB34120认证同时给4路控制有功功率和无功功率
				if(gs_IMMU2_LCCU_Mng.u8ManualIsAcDispatchMode == TRUE)                    //手动模式下的交流调度
				{

//by lxr01202 新增交流调度下  设置储能模式 21 18 A1使能
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x21;   
					untemp.chtemp[2] = 0x18;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA1;  //储能模式
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);

//设置交流调度模式
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0x11;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA1;  //交流调度模式

					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;
					
					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);

//by lxr01202 新增交流调度下  设置有功功率	31 23 单位MW
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0x23;   
					uiDH = untemp.u32Value;
					union_temp.i32temp = (INT32)(gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[SGNo] * 100000);
					untemp.u32Value = union_temp.u32temp;
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);
					
//by lxr01202 新增交流调度下  设置有系统直流总电流	10 02 单位MA
         
					// untemp.u32Value = 0;
					// untemp.chtemp[3] = 0x10;   
					// untemp.chtemp[2] = 0x02;   
					// uiDH = untemp.u32Value;
					// union_temp.i32temp = (INT32)(gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpP[SGNo] * 100000 / gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[SGNo] * 100000);                   //需要计算电流
					// untemp.u32Value = union_temp.u32temp;
					// uiDL = untemp.u32Value;
					// uiflag = 0;

					// u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					// OSTimeDly(3);
				

//逆变 整流
		        untemp.u32Value = 0;
			    untemp.chtemp[3] = 0x21;   
			    untemp.chtemp[2] = 0x10;   
			     uiDH = untemp.u32Value;

				if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
					{
						untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
					}
				else
					{
						untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
					}
				untemp.u32Value = 0;
				untemp.chtemp[1] = untemp_16bits.chtemp[1];
				untemp.chtemp[0] = untemp_16bits.chtemp[0];
				uiDL = untemp.u32Value;

				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(5);

					//开关机,这里开关机一定要放在设置有功功率后面，否则无法开机
					SetGrp_OnOff_No10(SGNo, &gs_CANComm, &gs_CANComm);

					//是否要设置Q
					if(gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[SGNo] != 0)
					{
						//无功模式
						untemp.u32Value = 0;
						untemp.chtemp[3] = 0x21;   
						untemp.chtemp[2] = 0x17;   
						uiDH = untemp.u32Value;
						untemp_16bits.u16Temp = (UINT16)0xA2;	//恒无功
						untemp.u32Value = 0;
						untemp.chtemp[1] = untemp_16bits.chtemp[1];
						untemp.chtemp[0] = untemp_16bits.chtemp[0];
						uiDL = untemp.u32Value;

						u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
						OSTimeDly(3);

						//设置Q
						untemp.u32Value = 0;
						untemp.chtemp[3] = 0x21;   
						untemp.chtemp[2] = 0x08;   
						uiDH = untemp.u32Value;
						union_temp.i32temp = (INT32)gs_IMMU2_LCCU_Mng.i16ManualCtrlGrpQ[SGNo];
						uiDL = union_temp.u32temp;

						u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
						OSTimeDly(3);
					}
					else
					{
						//设置无功使能
						untemp.u32Value = 0;
						untemp.chtemp[3] = 0x21;   
						untemp.chtemp[2] = 0x17;   
						uiDH = untemp.u32Value;
						untemp_16bits.u16Temp = (UINT16)0xA0;	//无功禁止
						untemp.u32Value = 0;
						untemp.chtemp[1] = untemp_16bits.chtemp[1];
						untemp.chtemp[0] = untemp_16bits.chtemp[0];
						uiDL = untemp.u32Value;

						u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
						OSTimeDly(3);
					}
				}
				else                                  //手动模式下直流调度
				{
					//20220414 LKF 通过HXM沟通，JN储能应用必须直流调度模式
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x31;   
					untemp.chtemp[2] = 0x11;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA0;  //直流调度模式
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);


//by lxr01202 新增直流调度下  取消储能模式 21 18 A0使能
					untemp.u32Value = 0;
					untemp.chtemp[3] = 0x21;   
					untemp.chtemp[2] = 0x18;   
					uiDH = untemp.u32Value;
					untemp_16bits.u16Temp = (UINT16)0xA0;  //取消储能模式
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(3);


// //by lxr01202  新增0x24 11 01    模块直流电压 MV
// 					untemp.u32Value = 0;
// 					untemp.chtemp[3] = 0x11;   
// 					untemp.chtemp[2] = 0x01;   
// 					uiDH = untemp.u32Value;
// 					untemp_16bits.u16Temp = 0x00;  
// 					untemp.u32Value = 0;
// 					untemp.chtemp[1] = untemp_16bits.chtemp[1];
// 					untemp.chtemp[0] = untemp_16bits.chtemp[0];
// 					uiDL = untemp.u32Value;

// 					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
// 					OSTimeDly(3);

// //by lxr01202  新增0x24 11 02    模块直流电压 MV
// 					untemp.u32Value = 0;
// 					untemp.chtemp[3] = 0x11;   
// 					untemp.chtemp[2] = 0x02;   
// 					uiDH = untemp.u32Value;
// 					untemp_16bits.u16Temp = 0x00;  
// 					untemp.u32Value = 0;
// 					untemp.chtemp[1] = untemp_16bits.chtemp[1];
// 					untemp.chtemp[0] = untemp_16bits.chtemp[0];
// 					uiDL = untemp.u32Value;

// 					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
// 					OSTimeDly(3);


// //by lxr01202  新增0x24 11 03    模块组号
// 					untemp.u32Value = 0;
// 					untemp.chtemp[3] = 0x11;   
// 					untemp.chtemp[2] = 0x03;   
// 					uiDH = untemp.u32Value;
// 					untemp_16bits.u16Temp = 0x00;  
// 					untemp.u32Value = 0;
// 					untemp.chtemp[1] = untemp_16bits.chtemp[1];
// 					untemp.chtemp[0] = untemp_16bits.chtemp[0];
// 					uiDL = untemp.u32Value;

// 					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
// 					OSTimeDly(3);


			        untemp.u32Value = 0;
			        untemp.chtemp[3] = 0x21;   
			        untemp.chtemp[2] = 0x10;   
			        uiDH = untemp.u32Value;

					if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
						{
							untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
						}
					else
						{
							untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
						}
					untemp.u32Value = 0;
					untemp.chtemp[1] = untemp_16bits.chtemp[1];
					untemp.chtemp[0] = untemp_16bits.chtemp[0];
					uiDL = untemp.u32Value;

					u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
					OSTimeDly(5);


				}
			}
		}



		else if(gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE)
		{
// BEG 模块
//直流起机模式
				u8CMD = 0x24;	

				untemp.u32Value = 0;
				untemp.chtemp[3] = 0x11;   
				untemp.chtemp[2] = 0x27;   
				uiDH = untemp.u32Value;

				//20220414 LKF 通过HXM沟通，JN储能应用必须直流起机模式
				untemp_16bits.u16Temp = (UINT16)0xA0;  //直流起机模式
				untemp.u32Value = 0;
				untemp.chtemp[1] = untemp_16bits.chtemp[1];
				untemp.chtemp[0] = untemp_16bits.chtemp[0];
				uiDL = untemp.u32Value;
				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(3);

//20220414 LKF 通过HXM沟通，JN储能应用必须直流调度模式
				untemp.u32Value = 0;
				untemp.chtemp[3] = 0x31;   
				untemp.chtemp[2] = 0x11;   
				uiDH = untemp.u32Value;
				untemp_16bits.u16Temp = (UINT16)0xA0;  //指流调度模式
				untemp.u32Value = 0;
				untemp.chtemp[1] = untemp_16bits.chtemp[1];
				untemp.chtemp[0] = untemp_16bits.chtemp[0];
				uiDL = untemp.u32Value;
				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(3);

//逆变 整流
		        untemp.u32Value = 0;
			    untemp.chtemp[3] = 0x21;   
			    untemp.chtemp[2] = 0x10;   
			     uiDH = untemp.u32Value;

				if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
					{
						untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
					}
				else
					{
						untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
					}
				untemp.u32Value = 0;
				untemp.chtemp[1] = untemp_16bits.chtemp[1];
				untemp.chtemp[0] = untemp_16bits.chtemp[0];
				uiDL = untemp.u32Value;

				u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
				OSTimeDly(5);
		}
	}


	//DCDC 模块
	if((SGNo >= DCDC_GRP_NO_MIN) && (SGNo <=DCDC_GRP_NO_MAX))
	{
		u8CMD = 0x24;	
		untemp.u32Value = 0;
		untemp.chtemp[3] = 0x21;   
		untemp.chtemp[2] = 0x10;   
		uiDH = untemp.u32Value;

		if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
		{
			untemp_16bits.u16Temp = (UINT16)0xA1;  //逆变模式
		}
		else
		{
			untemp_16bits.u16Temp = (UINT16)0xA0;  //整流模式
		}
		untemp.u32Value = 0;
		untemp.chtemp[1] = untemp_16bits.chtemp[1];
		untemp.chtemp[0] = untemp_16bits.chtemp[0];
		uiDL = untemp.u32Value;

		u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiDH, uiDL);
		OSTimeDly(5);
	}

	return ERR_CAN_R_NO_ERR;
}
#endif





/**
 * by lxr01202
 * @brief 设置指定组MPPT的电压和电流值
 * @param SGNo 组号(1-2)
 * @param pRawData 原始数据指针,指向CANCommData_TypeDef结构
 * @param pSetData 设置数据指针(未使用)
 * @return ERR_CODE_CAN_R 错误码
 *         - ERR_CAN_R_NO_ERR: 设置成功
 *         - ERR_CAN_R_COMM_FAIL: 组号超出范围
 * @note 通过CAN总线向设备发送电压电流设置命令(0x1B)
 *       电压单位为mV,电流单位为mA
 */
ERR_CODE_CAN_R SetGrp_AdjVoltCurr_No13(int SGNo, void* pRawData,void* pSetData)
{
	//SGNo 组号	  1 2
	//UINT8 u8Rst = FALSE;
	//UINT8 u8Idx = 0;
	UINT8 u8CMD = 0x1B;									//设置电压	电流
	//UINT8 uVal= 0;
	//canmsg SendMsg;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	if (SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_COMM_FAIL;
	}


	UINT32 uiSetVolt,uiSetCurr;
	//float ftemp;

	//20211102 lkf 1KV22KW模块，涉及模块内部串并联模式切换，因电池电压最低不于520V，所以工作电压不能低于520V
	//ftemp = PData->fRect_GRP_NeedOutVolt[SGNo];
	/*if((SGNo >= ACDC_GRP_NO_MIN) && (SGNo <= ACDC_GRP_NO_MAX))
	{
		if(ftemp < REG_1K_SW_VOLT)
		{
			ftemp = REG_1K_SW_VOLT;
		}
	}
	uiSetVolt = (UINT32)((ftemp) * 1000);*/

	uiSetVolt = (UINT32)(PData->fRect_GRP_NeedOutVolt[SGNo] * 1000);          //控制MPPT输出电压
	uiSetCurr = (UINT32)(PData->fRect_GRP_NeedOutCurr[SGNo] * 1000);          //控制MPPT输出电流
 //   uiSetCurr = (UINT32)(260000000);          //控制MPPT输出电流     by lxr01202  单个130A   一组260A    不需要限制电流  （有问题，并离网会继续给电池充）                                    
	u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, SGNo, uiSetVolt, uiSetCurr);
	OSTimeDly(3);
	
	//注：设备号为0x0A时，命令中目标地址的模块地址为3F; 
	//	设备号为0x0B时，命令中目标地址为组号。电压单位为mV；总电流单3位为mA，为设定的所有模块的输出电流之和。
	//	如果分组设置有改变，请在分组设置4s以后再下发该命令!
	//SendMsg.Id = (R_PRPTO_DEVNO_0B << 22) + (u8CMD << 16) + ((SGNo+1) << 8) + R_PROTO_SELF_ADDR;			//1组 或 2组
	//SendMsg.Data[0] = uiSetVolt;
	//SendMsg.Data[1] = uiSetCurr;
	//gSelfTest.fSetTaskID(TASK_ID_CAN3);
	//gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	return ERR_CAN_R_NO_ERR;
}





ERR_CODE_CAN_R SetGrp_AdjVolt_0x24(int SGNo, void* pRawData,void* pSetData)
{
	UINT32 u32DataH, u32DataL;
	//UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x24;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	u32DataH = 0x41010000;
	u32DataL = (UINT32)(PData->fRect_GRP_NeedOutVolt[SGNo] * 1000);
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, u32DataL);
	
	OSTimeDly(5);

	return ERR_CAN_R_NO_ERR;
}

ERR_CODE_CAN_R SetGrp_AdjCurr_0x24(int SGNo, void* pRawData,void* pSetData)
{

	UINT32 u32DataH, u32DataL;
	//UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 uctemp;
//	float ftemp;
	UINT8 u8CMD = 0x24;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	u32DataH = 0x41020000;

	uctemp = gs_CANComm.GrpData[SGNo].nCommRectNum;
	if(uctemp > gs_CANComm.GrpData[SGNo].u8ConvertStat_ReverseNum)
	{
		uctemp = gs_CANComm.GrpData[SGNo].u8ConvertStat_ReverseNum;
	}
	if(uctemp < 1)
	{
		u32DataL = 0;
	}
	else
	{
		u32DataL = (UINT32)(PData->fRect_GRP_NeedOutCurr[SGNo] / uctemp * 1000);
	}
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, u32DataL);

	OSTimeDly(5);

	return ERR_CAN_R_NO_ERR;
}
ERR_CODE_CAN_R SetGrp_AdjVolt_0x24_BEG(int SGNo, void* pRawData,void* pSetData)                  //1001 系统直流总压
{
	UINT32 u32DataH, u32DataL;
	float ftemp;
	//UINT8 u8Rtn,u8RcvMsgNum;
	UINT8 u8CMD = 0x24;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	u32DataH = 0x10010000;

	//20211102 lkf 1KV22KW模块，涉及模块内部串并联模式切换，因电池电压最低不于520V，所以工作电压不能低于520V
	ftemp = PData->fRect_GRP_NeedOutVolt[SGNo];
	if((SGNo >= ACDC_GRP_NO_MIN) && (SGNo <= ACDC_GRP_NO_MAX))
	{
		if(ftemp < BEG_1K_SW_VOLT)
		{
			//ftemp = BEG_1K_SW_VOLT;  //20230306 lkf 屏蔽？
		}
	}
	if((gEvChargeInfo.u8TaskIsAcDispatchMode == TRUE))   //交流调度
		{
			if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)   //逆变模式，电压直接放开
			{
				u32DataL = (UINT32)(ftemp * 1000);
			}
			else                                           //整流模式
			{
				// if((fChargePower == 0))         //如果下发功率为0,则让模块电压比电池电压低，待机模式，无电流
				// {
				// 	u32DataL = ((gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].BattInfoFromCan.BMS_SumVolt - 50)) * 1000;
				// }
				// else           //否则，放开直流侧电压
				{
					u32DataL = gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].BattInfoFromCan.BMS_Limit_ChargeVoltage * 1000;
				}
			}
		}
		else
		{
			u32DataL = (UINT32)(ftemp * 1000);
		}
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, u32DataL);

	OSTimeDly(3);

	return ERR_CAN_R_NO_ERR;
}

ERR_CODE_CAN_R SetGrp_AdjCurr_0x24_BEG(int SGNo, void* pRawData,void* pSetData)                 //1002 系统直流总电流
{
	union
	{
		UINT32 u32temp;
		INT32 i32temp;
	}union_temp;
	UINT32 u32DataH, u32DataL;
	UINT8 u8CMD = 0x24;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	u32DataH = 0x10020000;

	/*uctemp = gs_CANComm.GrpData[SGNo].nCommRectNum;
	if(uctemp > gs_CANComm.GrpData[SGNo].u8ConvertStat_ReverseNum)
	{
	uctemp = gs_CANComm.GrpData[SGNo].u8ConvertStat_ReverseNum;
	}
	if(uctemp < 1)
	{
	u32DataL = 0;
	}
	else
	{
	u32DataL = (UINT32)(PData->fRect_GRP_NeedOutCurr[SGNo] / uctemp * 1000);
	}*/

	if (gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)
	//if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
	{
		if((SGNo >= ACDC_GRP_NO_MIN) && (SGNo <= ACDC_GRP_NO_MAX))
		{
			if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
			{
				u32DataL = (UINT32)(PData->fRect_GRP_NeedOutCurr[SGNo] * 1000);
			}
			else
			{
				union_temp.i32temp = (-((INT32)(PData->fRect_GRP_NeedOutCurr[SGNo] * 1000)));
				u32DataL = union_temp.u32temp;	
			}
		}
		else 
		{
			u32DataL = (UINT32)(PData->fRect_GRP_NeedOutCurr[SGNo] * 1000);
		}	
	}

	else
	{
		if((gEvChargeInfo.u8TaskIsAcDispatchMode == TRUE))   //交流调度
		{
			if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)   //逆变模式
			{
				// if(fDisChargePower == 0)         //如果下发功率为0,则让直流侧电流为0
				// {
				// 	u32DataL = 0;
				// }
				// else
				{
					u32DataL = (UINT32)(PData->fRect_GRP_NeedOutCurr[SGNo] * 1000);
				}
			}
			else
			{
				// if(fChargePower == 0)         //如果下发功率为0,则让直流侧电流为0
				// {
				// 	u32DataL = 0;
				// }            
				// else
				{
					u32DataL = gs_IMMU2_LCCU_Mng.ESBattMng[SGNo].BattInfoFromCan.BMS_Limit_ChargeCurrent * 1000;
				}
			}
		}
		else
		{
			if(gs_CANComm.u8GRP_WorkModeSet[SGNo] == GRP_WORKMODE_SET_REVERSE)
			{
				u32DataL = (UINT32)(PData->fRect_GRP_NeedOutCurr[SGNo] * 1000);
			}
			else
			{
				u32DataL = (UINT32)(PData->fRect_GRP_NeedOutCurr[SGNo] * 1000);
			}
		}
	}

	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, u32DataL);

	OSTimeDly(3);

	return ERR_CAN_R_NO_ERR;
}
ERR_CODE_CAN_R GetGrp_MaxMinVoltCurr(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
        UINT8 u8Rtn;
	//SGNo 组号	 1 2
	//UINT8 u8Rst = FALSE;
	//UINT8 u8Idx = 0;
	UINT8 u8CMD = 0x0A;
	//UINT8 uVal= 0;
	//canmsg SendMsg;
	CANCommData_TypeDef*PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, 0, 0);
	OSTimeDly(5);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = u8RcvMsg_WaitTicks(15);
	if (u8RcvMsgNum > 0)
	{
		//u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);
		s_fGrpRateCurrBuf = 0;
		s_fGrpRatePowerBuf = 0;

		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);
		if(ERR_CAN_R_NO_ERR == u8Rtn)
		{
			PData->GrpData[SGNo].fRatedPower = s_fGrpRatePowerBuf;
			PData->GrpData[SGNo].fMax_OutCurr = s_fGrpRateCurrBuf;
			PData->GrpData[SGNo].fRatedMaxCurr = s_fGrpRateCurrBuf;	
		}
	}
	else
	{
		u8Rtn = ERR_CAN_R_WRONG_ADDR;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}


float String2Float(UINT8* pData)
{
	__XXX_UNION_VALUE unVal;

	for(UINT8 i = 0; i < 4; i++)
	{
		unVal.chtemp[4 - i - 1] = pData[i];
	}
	return unVal.ftemp;
}

void Init_CmdInfos(void)
{
	CAN_RECT_FRAME_INFO* p = gs_CANComm.cmdInfos;
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_0_R_CMD_ID_S_03,	GetSingle_VoltCurr);//0
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_1_R_CMD_ID_S_04,	GetSingle_GrpAndStatus);//1
	INIT_RECT_CMD(p, FRQ_20_TIMES, _CAN_2_R_CMD_ID_S_06,	GetSingle_PhaseVolts);//2
	INIT_RECT_CMD(p, FRQ_50_TIMES, _CAN_3_R_CMD_ID_S_07,	GetSingle_VerInfo);//3
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_4_R_CMD_ID_S_1A,    SetSingle_OnOff);//4
	INIT_RECT_CMD(p, FRQ_50_TIMES, _CAN_5_R_CMD_ID_S_14,	SetSingle_LedFlash);//5
	INIT_RECT_CMD(p, FRQ_50_TIMES, _CAN_6_R_CMD_ID_S_1E,	SetSingle_Reset);//6
	//INIT_RECT_CMD(p, FRQ_50_TIMES, _CAN_7_R_CMD_ID_S_0C,	GetSingle_Power);//7

	INIT_RECT_CMD(p, FRQ_1_QUICK, _CAN_8_R_CMD_ID_G_01,		GetGrp_VoltCurr);//8
	INIT_RECT_CMD(p, FRQ_1_QUICK, _CAN_9_R_CMD_ID_G_02,		GetGrp_NumAndStatus);//9
	INIT_RECT_CMD(p, FRQ_1_QUICK, _CAN_10_R_CMD_ID_G_1A,	SetGrp_OnOff_No10);//10
	INIT_RECT_CMD(p, FRQ_1_QUICK, _CAN_11_R_CMD_ID_G_0F,	SetBroad_IinLimit);//11
	INIT_RECT_CMD(p, FRQ_50_TIMES, _CAN_12_R_CMD_ID_G_13,	SetGrp_WalkIn);//12
	INIT_RECT_CMD(p, FRQ_50_TIMES, _CAN_13_R_CMD_ID_G_1B,	SetGrp_AdjVoltCurr_No13);//13
	INIT_RECT_CMD(p, FRQ_50_TIMES, _CAN_14_R_CMD_ID_G_0A,	GetGrp_MaxMinVoltCurr);//14
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_15_R_CMD_ID_G_0B,	GetRECT_TEM_0X23);//15
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_16_R_CMD_ID_G_0C,	GetRECT_MAXTEM_0X23);//16
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_17_R_CMD_ID_G_0D,	GetRECT_bStatFaltCode1110_PCS_0X23);//17
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_18_R_CMD_ID_G_0E,	GetRECT_bStatFaltCode1111_PCS_0X23);//18
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_19_R_CMD_ID_G_10,	GetMPPT_DCTEM_0X0E);//19
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_20_R_CMD_ID_G_11,	GetMPPT_PFCTEM_0X0E);//20
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_21_R_CMD_ID_G_12,	GetMPPT_DC_TRANS_TEM_0X0E);//21
	INIT_RECT_CMD(p, FRQ_10_TIMES, _CAN_22_R_CMD_ID_G_13,	GetMPPT_PFC_TRANS_TEM_0X0E);//22
}


void InitCan3Port(void)
{
	UINT32 u32Mask, u32RcvID;

	MCP2515_init(CAN_125KBPS);

	//Rec Buff[0] / [1] 屏蔽码设置
	u32Mask = 0x0380FF00;
	MCP2515_Set_Recmask(0, u32Mask);
	MCP2515_Set_Recmask(1, u32Mask);

	//Rec Buff[0] 接收 0A/0B设备号 消息, 目的地址需要满足本桩地址
	u32RcvID = 0x02800000 + ((UINT32)gs_CANComm.bySelfAddress<<8);
	MCP2515_Set_RecID(0, u32RcvID);
	MCP2515_Set_RecID(1, u32RcvID);

	//Rec Buff[1] 接收 0C设备号 消息, 目的地址必须等于本桩的组号或广播地址
	u32RcvID = 0x03000000 + ((UINT32)gs_CANComm.bySelfAddress<<8);
	//u32RcvID = 0x03000000  + ((UINT32)gs_CANComm.byGrpIdx[GRP_A]<<8);
	MCP2515_Set_RecID(2, u32RcvID);
	//u32RcvID = 0x03000000  + ((UINT32)gs_CANComm.byGrpIdx[GRP_B]<<8);
	MCP2515_Set_RecID(3, u32RcvID);
	//u32RcvID = 0x03000000 + ((UINT32)0x1f<<8);		//
	MCP2515_Set_RecID(4, u32RcvID);
	MCP2515_Set_RecID(5, u32RcvID);
	
	
	OSTimeDly(4);

	
}

static void PCS_ReadActivePower_0x23(INT8U ch, INT8U cmd)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;
	CANCommData_TypeDef* PData = &gs_CANComm;

	dword.v = 0u;
	if (cmd == eType_Total)
	{
		dword.b[2] = 0x08;
	}
	else if (cmd == eType_AB)
	{
		dword.b[2] = 0x09;
	}
	else if (cmd == eType_BC)
	{
		dword.b[2] = 0x0A;
	}
	else if (cmd == eType_CA)
	{
		dword.b[2] = 0x0B;
	}
	else
	{
		return;
	}

	dword.b[3] = 0x21;
	u8PackSendMsg(R_PRPTO_DEVNO_0B, 0x23, ch, dword.v, 0);
	OSTimeDly(3u);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, 0x23, ch, PData);	
	}
}

static void PCS_ReadReactivePower_0x23(INT8U ch, INT8U cmd)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;
	CANCommData_TypeDef* PData = &gs_CANComm;

	dword.v = 0u;
	if (cmd == eType_Total)
	{
		dword.b[2] = 0x0C;
	}
	else if (cmd == eType_AB)
	{
		dword.b[2] = 0x0D;
	}
	else if (cmd == eType_BC)
	{
		dword.b[2] = 0x0E;
	}
	else if (cmd == eType_CA)
	{
		dword.b[2] = 0x0F;
	}
	else
	{
		return;
	}

	dword.b[3] = 0x21;
	u8PackSendMsg(R_PRPTO_DEVNO_0B, 0x23, ch, dword.v, 0);
	OSTimeDly(3u);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, 0x23, ch, PData);	
	}
}

static void PCS_ReadPhaseVolt_0x23(INT8U ch, INT8U cmd)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;
	CANCommData_TypeDef* PData = &gs_CANComm;

	dword.v = 0u;
	if (cmd == eType_AB)
	{
		dword.b[2] = 0x01;
	}
	else if (cmd == eType_BC)
	{
		dword.b[2] = 0x02;
	}
	else if (cmd == eType_CA)
	{
		dword.b[2] = 0x03;
	}
	else
	{
		return;
	}

	dword.b[3] = 0x21;
	u8PackSendMsg(R_PRPTO_DEVNO_0B, 0x23, ch, dword.v, 0);
	OSTimeDly(3u);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, 0x23, ch, PData);	
	}
}

static void PCS_ReadPhaseCurr_0x23(INT8U ch, INT8U cmd)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;
	CANCommData_TypeDef* PData = &gs_CANComm;

	dword.v = 0u;
	if (cmd == eType_AB)
	{
		dword.b[2] = 0x04;
	}
	else if (cmd == eType_BC)
	{
		dword.b[2] = 0x05;
	}
	else if (cmd == eType_CA)
	{
		dword.b[2] = 0x06;
	}
	else
	{
		return;
	}

	dword.b[3] = 0x21;
	u8PackSendMsg(R_PRPTO_DEVNO_0B, 0x23, ch, dword.v, 0);
	OSTimeDly(3u);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, 0x23, ch, PData);	
	}
}

void SetAndGetGroupCommand(void)
{
	UINT8 u8Rtn, iGIdx;	
	static UINT8 s_u8FirstFlag = 3;
	static UINT8 s_u8FirstFlag_ForSetWorkMode = 3;
//	UINT8 u8ReconfigFlag = 0;
	UINT16 u16RectLoadStartTime;    
	static UINT8 u8LastGRP_RectOnOff[_MAX_GRP_NUM] = {0};

	if(gs_CANComm.u8AddressScanDelay > 0)
	{
		s_u8FirstFlag = 3;
		s_u8FirstFlag_ForSetWorkMode = 3;
		gs_CANComm.fInitRectData(0);
	}


	
if((gIMIU_Info_UART4[0].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART4[1].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART4[2].u8JYMAlmStat[0] == TRUE)
		|| (gIMIU_Info_UART4[3].u8JYMAlmStat[0] == TRUE)
		|| (gEvChargeInfo.u8EPOAlm == TRUE)
		|| (TRUE == gEvChargeInfo.u8SPDAlm)
		|| (gEvChargeInfo.u8DoorAlm == TRUE)
		|| (gs_IMMU2_LCCU_Mng.IMMU2Stat.FaultedStatus == 1)
		|| (gs_CANComm.u8AddressScanDelay > 0)
		|| (gs_EsSampleInfo.Batt_CanComm_Break_Alm[0] == TRUE)
		|| (gs_EsSampleInfo.Batt_CanComm_Break_Alm[1] == TRUE)
		|| (gs_EsSampleInfo.Batt_CanComm_Break_Alm[2] == TRUE)
		|| (gs_EsSampleInfo.Batt_CanComm_Break_Alm[3] == TRUE)               //by lxr01202  新增BMS通信中断诊断,BMS通信故障禁止充放电
		|| (gEvChargeInfo.u8TelaidianHostCommAlm == TRUE)                    //by lxr01202   新增后台通信中断诊断,BMS通信故障禁止充放电
	  )
		{
			//通信中断，先关停处理               
			//CCU 通信中断  屏蔽掉模块关机，防止离网时候还在工作
			{
				// for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
				// {
				// 	gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+iGIdx] = RECT_OFF;

				// 	//开关机,这里开关机一定要放在设置有功功率后面，否则无法开机
				// 	SetGrp_OnOff_No10(iGIdx, &gs_CANComm, &gs_CANComm);		
				// }
		
			}

			return;
		}




	//4.广播各组缓启动命令	
	if(s_u8FirstFlag > 0)
	{
		for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
		{
			if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
			{
				continue;
			}

			u16RectLoadStartTime = (UINT16)gEvChargeInfo.fRectLoadStartTime*100;
			u16RectLoadStartTime = 0;
			SetGrp_WalkIn(iGIdx, &gs_CANComm, &u16RectLoadStartTime);

			gs_CANComm.u8GRP_SerialParallelModeSet[iGIdx] = 0;
			gs_CANComm.u8GRP_SerialParallelModeSet_Back[iGIdx] = 0;
			gs_CANComm.u8GRP_SerialParallelModeSet_Flag[iGIdx] = mGRP_SerialParallelModeSet_Flag;
			gs_CANComm.u8GRP_IsSupportSerialParallelModeSet[iGIdx] = 0;
		}		

		s_u8FirstFlag--;
	}

	//5.设定DCDC模块工作模式
	if(gEvChargeInfo.u8SetWorkModeDelay == 0)
	{	
		gEvChargeInfo.u8SetWorkModeDelay = gEvChargeInfo.u8SetWorkModeInterval;

		for(iGIdx = GRP_NO_MIN; iGIdx <= GRP_NO_MAX; iGIdx++)
		{
			if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
			{
				continue;
			}

			SetDCDCGrp_WorkMode(iGIdx, &gs_CANComm, &gs_CANComm, s_u8FirstFlag_ForSetWorkMode);
		}
		if(s_u8FirstFlag_ForSetWorkMode > 0)
		{
			s_u8FirstFlag_ForSetWorkMode--;
		}
	}
	
	//1.广播各组模块的输出电压、电流（1s1次）
	if(gEvChargeInfo.u8SetAdjVoltCurrDelay == 0)
	{
		gEvChargeInfo.u8SetAdjVoltCurrDelay = gEvChargeInfo.u8SetAdjVoltCurrInterval;

		for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
		{
			if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
			{
				continue;
			}
//by lxr01202
			if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				&& ((iGIdx >= ACDC_GRP_NO_MIN) && (iGIdx <= ACDC_GRP_NO_MAX)))
			{			
				SetGrp_AdjVolt_0x24_BEG(iGIdx, &gs_CANComm, &gs_CANComm);
				SetGrp_AdjCurr_0x24_BEG(iGIdx, &gs_CANComm, &gs_CANComm);
			}
			else if(((iGIdx >= ACDC_GRP_NO_MIN) && (iGIdx <= ACDC_GRP_NO_MAX))
				&& (gs_CANComm.u8GRP_WorkModeSet[iGIdx] == GRP_WORKMODE_SET_REVERSE))
			{
				SetGrp_AdjVolt_0x24_BEG(iGIdx, &gs_CANComm, &gs_CANComm);
				SetGrp_AdjCurr_0x24_BEG(iGIdx, &gs_CANComm, &gs_CANComm);
			}
			else if(((iGIdx >= DCDC_GRP_NO_MIN) && (iGIdx <= DCDC_GRP_NO_MAX))
				&& (gs_CANComm.u8GRP_WorkModeSet[iGIdx] == GRP_WORKMODE_SET_REVERSE))
			{
				SetGrp_AdjVolt_0x24(iGIdx, &gs_CANComm, &gs_CANComm);
				SetGrp_AdjCurr_0x24(iGIdx, &gs_CANComm, &gs_CANComm);
			}
			else                                                                      //MPPT 输出电压电流
			{			
				SetGrp_AdjVoltCurr_No13(iGIdx, &gs_CANComm, &gs_CANComm);
			}		
		}
	}
	
	//2.广播各组模块的开关命令
	for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
	{
		if(u8LastGRP_RectOnOff[GRP_1_ACDC+iGIdx] != gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+iGIdx])      
		{
			u8LastGRP_RectOnOff[GRP_1_ACDC+iGIdx] = gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+iGIdx];
			gEvChargeInfo.u8SetOnOffDelay = 0;
		}
	}
	if(gEvChargeInfo.u8SetOnOffDelay == 0)
	{
		gEvChargeInfo.u8SetOnOffDelay = gEvChargeInfo.u8SetOnOffInterval;

		for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
		{
			if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
			{
				gs_CANComm.GrpData[iGIdx].u8PowerofThisVoltMaxIsValidFlag = 0;
				continue;
			}

			SetGrp_OnOff_No10(iGIdx, &gs_CANComm, &gs_CANComm);

			if(gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE)
			{
				//20211120 lkf 增加1KV模块的高低压模式切换处理
				if(gs_CANComm.u8GRP_SerialParallelModeSet_Flag[iGIdx] > 0)
				{
					SetGrp_SerialParellMode(iGIdx, &gs_CANComm, &gs_CANComm);
					gs_CANComm.u8GRP_SerialParallelModeSet_Flag[iGIdx]--;
				}
			}
		}
	}
	

	//3.查询模块组的输出电压电流命令(3S1次)
	if(gEvChargeInfo.u8GetVoltCurrDelay == 0)
	{
		gEvChargeInfo.u8GetVoltCurrDelay = gEvChargeInfo.u8GetVoltCurrInterval;

		
		for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
		{
			if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
			{
				continue;
			}

			if(((iGIdx >= DCDC_GRP_NO_MIN) && (iGIdx <= DCDC_GRP_NO_MAX))
				&& (gs_CANComm.u8GRP_WorkModeSet[iGIdx] == GRP_WORKMODE_SET_REVERSE))
			{
				u8Rtn = GetGrp_Volt_0X23(iGIdx, &gs_CANComm, &gs_CANComm);
				if(u8Rtn != ERR_CAN_R_NO_ERR)
				{
					gs_CANComm.nNotAccessbleCount[iGIdx]++;
					if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
					{
						gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
					}
				}
				else
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
					gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
				}
				u8Rtn = GetGrp_Curr_0X23(iGIdx, &gs_CANComm, &gs_CANComm);
				if(u8Rtn != ERR_CAN_R_NO_ERR)
				{
					gs_CANComm.nNotAccessbleCount[iGIdx]++;
					if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
					{
						gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
					}
				}
				else
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
					gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
				}
			}

			if((iGIdx >= MPPT_GRP_NO_MIN) && (iGIdx <= MPPT_GRP_NO_MAX))
			{
//by lxr01202 新增MPPT DC板温  

                            u8Rtn = GetMPPT_DCTEM_0X0E(iGIdx, &gs_CANComm, &gs_CANComm);	

                            if(u8Rtn != ERR_CAN_R_NO_ERR)
                              {
                                    gs_CANComm.nNotAccessbleCount[iGIdx]++;
                                    if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
                                    {
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
                                    }
                              }
                            else
                              {
                                  gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
                                  gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
                              }
//by lxr01202 新增MPPT PFC板温  
			u8Rtn = GetMPPT_PFCTEM_0X0E(iGIdx, &gs_CANComm, &gs_CANComm);	

			if(u8Rtn != ERR_CAN_R_NO_ERR)
			{
				gs_CANComm.nNotAccessbleCount[iGIdx]++;
				if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
				}
			}
			else
			{
				gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
				gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
			}
//by lxr01202 新增MPPT DC变压器温度  
			u8Rtn = GetMPPT_DC_TRANS_TEM_0X0E(iGIdx, &gs_CANComm, &gs_CANComm);	

			if(u8Rtn != ERR_CAN_R_NO_ERR)
			{
				gs_CANComm.nNotAccessbleCount[iGIdx]++;
				if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
				}
			}
			else
			{
				gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
				gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
			}
//by lxr01202 新增MPPT PFC变压器温度  
			u8Rtn = GetMPPT_PFC_TRANS_TEM_0X0E(iGIdx, &gs_CANComm, &gs_CANComm);	

			if(u8Rtn != ERR_CAN_R_NO_ERR)
			{
				gs_CANComm.nNotAccessbleCount[iGIdx]++;
				if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
				}
			}
			else
			{
				gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
				gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
			}
                        
			}

			
			u8Rtn = GetGrp_VoltCurr(iGIdx, &gs_CANComm, &gs_CANComm);	
			//GetSingle_VoltCurr(iGIdx, &gs_CANComm, &gs_CANComm);

			if(u8Rtn != ERR_CAN_R_NO_ERR)
			{
				gs_CANComm.nNotAccessbleCount[iGIdx]++;
				if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
				}
			}
			else
			{
				gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
				gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
			}
//by lxr01202  模块1110故障码
			u8Rtn = GetRECT_bStatFaltCode1110_PCS_0X23(iGIdx, &gs_CANComm, &gs_CANComm);	

			if(u8Rtn != ERR_CAN_R_NO_ERR)
			{
				gs_CANComm.nNotAccessbleCount[iGIdx]++;
				if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
				}
			}
			else
			{
				gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
				gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
			}
//by lxr01202  模块1111故障码
			u8Rtn = GetRECT_bStatFaltCode1111_PCS_0X23(iGIdx, &gs_CANComm, &gs_CANComm);	

			if(u8Rtn != ERR_CAN_R_NO_ERR)
			{
				gs_CANComm.nNotAccessbleCount[iGIdx]++;
				if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
				}
			}
			else
			{
				gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
				gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
			}
//by lxr01202  模块内部温度
			u8Rtn = GetRECT_TEM_0X23(iGIdx, &gs_CANComm, &gs_CANComm);	

			if(u8Rtn != ERR_CAN_R_NO_ERR)
			{
				gs_CANComm.nNotAccessbleCount[iGIdx]++;
				if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
				}
			}
			else
			{
				gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
				gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
			}
//by lxr01202 新增模块内部最高功率器件温度
			u8Rtn = GetRECT_MAXTEM_0X23(iGIdx, &gs_CANComm, &gs_CANComm);	

			if(u8Rtn != ERR_CAN_R_NO_ERR)
			{
				gs_CANComm.nNotAccessbleCount[iGIdx]++;
				if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
				}
			}
			else
			{
				gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
				gs_CANComm.nNotAccessbleCount[iGIdx] = 0;
			}
		
		}
	}

	//4.查询模块组的有功无功命令（3s1次）
	if(gEvChargeInfo.u8GetPowerInfoDelay == 0)
	{	
		gEvChargeInfo.u8GetPowerInfoDelay = gEvChargeInfo.u8GetPowerInfoInterval;

		for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
		{
			if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
			{
				continue;
			}

			if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				&& ((iGIdx >= ACDC_GRP_NO_MIN) && (iGIdx <= ACDC_GRP_NO_MAX)))
			{			
				GetGrp_ActivePower_0X23(iGIdx, &gs_CANComm, &gs_CANComm);
				GetGrp_ReactivePower_0X23(iGIdx, &gs_CANComm, &gs_CANComm);
			}
		}
	}
	
	//20220416 每圈都采集的单模块信息（1s1次）
	if(gEvChargeInfo.u8GetSingleEveryTimeDelay == 0)
	{
		gEvChargeInfo.u8GetSingleEveryTimeDelay = gEvChargeInfo.u8GetSingleEveryTimeInterval;

		if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
		{
			static UINT32 s_u32SampleConvrtStat_PCS_step = 0x11100000;
			for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
			{
				if(!((iGIdx >= ACDC_GRP_NO_MIN) && (iGIdx <= ACDC_GRP_NO_MAX)))			
				{
					continue;
				}
				if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] > 0)
				{
					GetSingle_ConvertStat_ex_PCS(iGIdx, s_u32SampleConvrtStat_PCS_step);

					//GetSingle_ConvertStat(iGIdx, &gs_CANComm, &gs_CANComm);	

					GetSingle_VoltCurr(iGIdx, &gs_CANComm, &gs_CANComm);

					//20221027 lkf 增加0B命令采集
					GetGrp_ExtVolt(iGIdx, &gs_CANComm, &gs_CANComm);

//by lxr01202 新增PCS模块及MPPT模块温度采集命令
					GetRECT_TEM_0X23(iGIdx, &gs_CANComm, &gs_CANComm);
					GetRECT_MAXTEM_0X23(iGIdx, &gs_CANComm, &gs_CANComm);
					GetRECT_bStatFaltCode1110_PCS_0X23(iGIdx, &gs_CANComm, &gs_CANComm);
					GetRECT_bStatFaltCode1111_PCS_0X23(iGIdx, &gs_CANComm, &gs_CANComm);
					GetMPPT_DCTEM_0X0E(iGIdx, &gs_CANComm, &gs_CANComm);
					GetMPPT_PFCTEM_0X0E(iGIdx, &gs_CANComm, &gs_CANComm);					
					GetMPPT_DC_TRANS_TEM_0X0E(iGIdx, &gs_CANComm, &gs_CANComm);
					GetMPPT_PFC_TRANS_TEM_0X0E(iGIdx, &gs_CANComm, &gs_CANComm);					
				}	
				else
				{
					gs_CANComm.GrpData[iGIdx].u8PowerofThisVoltMaxIsValidFlag = 0;
				}
			}	
			if(s_u32SampleConvrtStat_PCS_step == 0x11100000)
			{
				s_u32SampleConvrtStat_PCS_step = 0x11110000;
			}
			
			else if(s_u32SampleConvrtStat_PCS_step == 0x11110000)
			{
				s_u32SampleConvrtStat_PCS_step = 0x11120000;
			}

			else if(s_u32SampleConvrtStat_PCS_step == 0x11120000)
			{
				s_u32SampleConvrtStat_PCS_step = 0x11130000;
			}

			else if(s_u32SampleConvrtStat_PCS_step == 0x11130000)
			{
				s_u32SampleConvrtStat_PCS_step = 0x11140000;
			}
			else if(s_u32SampleConvrtStat_PCS_step == 0x11140000)
			{
				s_u32SampleConvrtStat_PCS_step = 0x11150000;
			}
			else //if(s_u32SampleConvrtStat_PCS_step == 0x11150000)
			{
				s_u32SampleConvrtStat_PCS_step = 0x11100000;
			}
		}
	}
};



ERR_CODE_CAN_R GetSingle_Barcode(int SGNo, void* pRawData,UINT32 u32DataH)
{
	UINT8 u8RcvMsgNum;
	UINT8 u8Rtn;
	//UINT32 u32DataH = 0x11240000;

	//SGNo 模块地址0-1-2-3...
	UINT8 u8CMD = 0x23;	 
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(10);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);
		/*if(u8Rtn == ERR_CAN_R_COMM_FAIL)
		{
		if (SGNo >= 0 && SGNo < mRECTNUM)
		{
		PData->RectData[SGNo].nCommFailTimes++;
		if (PData->RectData[SGNo].nCommFailTimes > 10)
		{
		PData->RectData[SGNo].bCommFail = TRUE;
		}
		}
		}		*/
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;

		/*if (SGNo >= 0 && SGNo < mRECTNUM)
		{
		PData->RectData[SGNo].nCommFailTimes++;
		if (PData->RectData[SGNo].nCommFailTimes > 10)
		{
		PData->RectData[SGNo].bCommFail = TRUE;
		}
		}*/

	}
	return (ERR_CODE_CAN_R)u8Rtn;
}
ERR_CODE_CAN_R GetGrp_MaxCapacity_0x23(int SGNo, void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
	UINT8 u8Rtn;
	//SGNo 组号	1 2
	UINT8 u8CMD = 0x23;
	UINT32 u32DataH = 0x11330000;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;

	if(SGNo >= _MAX_GRP_NUM)
	{
		return ERR_CAN_R_WRONG_ADDR;
	}

	u8PackSendMsg(R_PRPTO_DEVNO_0B,u8CMD, SGNo, u32DataH, 0);
	OSTimeDly(20);
	//u8RcvMsgNum = u8RcvMsg(40);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		s_fGrpRatePowerBuf = 0;
		u8Rtn = UnPackData(u8RcvMsgNum, R_PRPTO_DEVNO_0B, u8CMD, SGNo, PData);	
		if(ERR_CAN_R_NO_ERR == u8Rtn)
		{
			if((SGNo >= ACDC_GRP_NO_MIN && SGNo <= ACDC_GRP_NO_MAX))
			{
				PData->GrpData[SGNo].fRatedPower = s_fGrpRatePowerBuf;
			}			
		}
	}
	else
	{
		u8Rtn = ERR_CAN_R_WRONG_ADDR;		
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}
enum SINGLE_RECT_CMD_NO_Def
{
	SINGLE_RECT_CMD_NO_0_GetMaxCap = 0,
	SINGLE_RECT_CMD_NO_1_GetConvertStat,
	SINGLE_RECT_CMD_NO_2_GetRectNumber,
	SINGLE_RECT_CMD_NO_3_GetRectExtVolt,
	SINGLE_RECT_CMD_NO_4_GetConvertStat_2,
	SINGLE_RECT_CMD_NO_GetGrp1Info,
	SINGLE_RECT_CMD_NO_GetGrp2Info,
	SINGLE_RECT_CMD_NO_GetGrp3Info,
	SINGLE_RECT_CMD_NO_GetGrp4Info,
	SINGLE_RECT_CMD_NO_GetGrp5Info,
	SINGLE_RECT_CMD_NO_GetGrp6Info,
	SINGLE_RECT_CMD_NO_GetGrp7Info,
	SINGLE_RECT_CMD_NO_GetGrp8Info,
	SINGLE_RECT_CMD_NO_GetGrp9Info,
	SINGLE_RECT_CMD_NO_GetGrp10Info,
	SINGLE_RECT_CMD_NO_GetGrp11Info,
	SINGLE_RECT_CMD_NO_GetGrp12Info,
	SINGLE_RECT_CMD_NO_MAX,
};



void SetAndGetSingleCommand(void)
{
	UINT8 u8Rtn, iGIdx, nTotalRectNum, u8GrpNo;	
	static UINT8 s_u8Step = 0;

	/*static UINT8 s_u8MpptNum = 0;
	static UINT8 s_u8FirstGetFlag = 3;
	
	static UINT8 s_u8MpptRepowerFlag = 0;*/


	static UINT8 s_u8FirstGetFlag = 3;	
//	static UINT8 s_u8FirstGetFlag_MPPT = 3;

	static UINT8 s_u8MpptNum = 0;
	static UINT8 s_u8MpptRepowerFlag = 0;

	static UINT8 s_u8ReadSerealNoStep = 0;

	static UINT8 s_u8ReadPowerStep = 0;

	static UINT8 s_u8ReadPhaseInfoStep = 0;

	if(gEvChargeInfo.u8GetSingleInfoDelay != 0)
	{
		return;
	}
	else
	{
		gEvChargeInfo.u8GetSingleInfoDelay = gEvChargeInfo.u8GetSingleInfoInterval;
	}

	if(gs_CANComm.u8AddressScanDelay > 0)
	{
		//if(s_u8Step > 1)
		{
			s_u8Step = 0;
		}
		
		 s_u8FirstGetFlag = 3;
		 s_u8FirstGetFlag_MPPT = 3;
	}

	//20220722 lkf 跳过
	if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
	{
		if(s_u8Step == SINGLE_RECT_CMD_NO_3_GetRectExtVolt)
		{
			s_u8Step++;
		}
	}

	switch(s_u8Step)
	{
		case SINGLE_RECT_CMD_NO_0_GetMaxCap:
			nTotalRectNum = 0;
			for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
			{
				if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
				{
					continue;
				}

				if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)||(gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE))
				{
					if (iGIdx >= ACDC_GRP_NO_MIN && iGIdx <= ACDC_GRP_NO_MAX)
					{
						u8Rtn = GetGrp_MaxCapacity_0x23(iGIdx, &gs_CANComm, &gs_CANComm);
					}
				}

				u8Rtn = GetGrp_MaxCapacity(iGIdx, &gs_CANComm, &gs_CANComm);
				if(u8Rtn != ERR_CAN_R_NO_ERR)
				{
					gs_CANComm.nNotAccessbleCount[iGIdx]++;
					if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
					{
						gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
					}
				}
				else
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
					gs_CANComm.nNotAccessbleCount[iGIdx] = 0;					
				}

				//nTotalRectNum += gs_CANComm.GrpData[iGIdx].nCommRectNum;
			}
			//gs_CANComm.nScanedRectNum = nTotalRectNum;

			for(iGIdx = 0; iGIdx < mRECTNUM; iGIdx++)
			{
				gs_CANComm.RectData[iGIdx].nCommFailTimes++;
			}
			for(iGIdx = 0; iGIdx < mMPPTNUM; iGIdx++)
			{
				gs_CANComm.MpptData[iGIdx].nCommFailTimes++;
			}
			s_u8Step++;
			break;

		case SINGLE_RECT_CMD_NO_1_GetConvertStat:
			nTotalRectNum = 0;
			for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
			{				
				if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
				{
					continue;
				}

				//u8Rtn = GetSingle_ConvertStat(iGIdx, &gs_CANComm, &gs_CANComm);	
				u8Rtn = GetSingle_GrpAndStatus(iGIdx, &gs_CANComm, &gs_CANComm);
				if(u8Rtn != ERR_CAN_R_NO_ERR)
				{
					gs_CANComm.nNotAccessbleCount[iGIdx]++;
					if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
					{
						gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
					}
				}
				else
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
					gs_CANComm.nNotAccessbleCount[iGIdx] = 0;					
				}
			}		

			for(iGIdx = 0; iGIdx < mRECTNUM; iGIdx++)
			{
				gs_CANComm.RectData[iGIdx].nCommFailTimes++;
			}
			for(iGIdx = 0; iGIdx < mMPPTNUM; iGIdx++)
			{
				gs_CANComm.MpptData[iGIdx].nCommFailTimes++;
			}

			s_u8Step++;
			
			break;

		//20220416 PCS 改成01命令采集		
		case SINGLE_RECT_CMD_NO_3_GetRectExtVolt:
			if(gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE) 
			{
				nTotalRectNum = 0;
				for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
				{
					//只采集ACDC模块?
					if(!((iGIdx >= ACDC_GRP_NO_MIN && iGIdx <= ACDC_GRP_NO_MAX)))			
					{
						continue;
					}
					if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
					{
						continue;
					}		
					u8Rtn = GetGrp_ExtVolt(iGIdx, &gs_CANComm, &gs_CANComm);
				}	
			}
			s_u8Step++;
			break;


		case SINGLE_RECT_CMD_NO_2_GetRectNumber:
			nTotalRectNum = 0;
			for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
			{				
				if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
				{
					continue;
				}

				u8Rtn = GetGrp_NumAndStatus(iGIdx, &gs_CANComm, &gs_CANComm);
				if(u8Rtn != ERR_CAN_R_NO_ERR)
				{
					gs_CANComm.nNotAccessbleCount[iGIdx]++;
					if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
					{
						gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
					}
				}
				else
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
					gs_CANComm.nNotAccessbleCount[iGIdx] = 0;					
				}

				nTotalRectNum += gs_CANComm.GrpData[iGIdx].nCommRectNum;
			}
			gs_CANComm.nScanedRectNum = nTotalRectNum;

			/*for(iGIdx = 0; iGIdx < mRECTNUM; iGIdx++)
			{
				gs_CANComm.RectData[iGIdx].nCommFailTimes++;
			}
			for(iGIdx = 0; iGIdx < mMPPTNUM; iGIdx++)
			{
				gs_CANComm.MpptData[iGIdx].nCommFailTimes++;
			}*/
			s_u8Step++;
			break;

		case SINGLE_RECT_CMD_NO_4_GetConvertStat_2:

			nTotalRectNum = 0;
			for(iGIdx = 0; iGIdx < _MAX_GRP_NUM; iGIdx++)
			{
				if(gEvChargeInfo.u8PowerModuleNumCfg[iGIdx] == 0)
				{
					continue;
				}

				//20220416 PCS 改成每圈都采集
				if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				  &&
				  ((iGIdx >= ACDC_GRP_NO_MIN) && (iGIdx <= ACDC_GRP_NO_MAX)))			
				{
					continue;
				}

				if((iGIdx >= MPPT_GRP_NO_MIN) && (iGIdx <= MPPT_GRP_NO_MAX))			
				{
					continue;
				}

				u8Rtn = GetSingle_ConvertStat(iGIdx, &gs_CANComm, &gs_CANComm);	
				if(u8Rtn != ERR_CAN_R_NO_ERR)
				{
					gs_CANComm.nNotAccessbleCount[iGIdx]++;
					if(gs_CANComm.nNotAccessbleCount[iGIdx] >= 10) 
					{
						gs_CANComm.bIsGrp_NotAvailable[iGIdx] = TRUE;
					}
				}
				else
				{
					gs_CANComm.bIsGrp_NotAvailable[iGIdx] = FALSE;
					gs_CANComm.nNotAccessbleCount[iGIdx] = 0;					
				}
			}	
			s_u8Step++;
			break;
	
		case SINGLE_RECT_CMD_NO_GetGrp1Info:
		case SINGLE_RECT_CMD_NO_GetGrp2Info:
		case SINGLE_RECT_CMD_NO_GetGrp3Info:
		case SINGLE_RECT_CMD_NO_GetGrp4Info:
		case SINGLE_RECT_CMD_NO_GetGrp5Info:
		case SINGLE_RECT_CMD_NO_GetGrp6Info:
		case SINGLE_RECT_CMD_NO_GetGrp7Info:
		case SINGLE_RECT_CMD_NO_GetGrp8Info:
		case SINGLE_RECT_CMD_NO_GetGrp9Info:
		case SINGLE_RECT_CMD_NO_GetGrp10Info:
		case SINGLE_RECT_CMD_NO_GetGrp11Info:
		case SINGLE_RECT_CMD_NO_GetGrp12Info:
			u8GrpNo = s_u8Step - SINGLE_RECT_CMD_NO_GetGrp1Info + ACDC_GRP_NO_MIN;	    
			while((gEvChargeInfo.u8PowerModuleNumCfg[u8GrpNo] == 0) && (s_u8Step < SINGLE_RECT_CMD_NO_GetGrp12Info))
			{
				//20220416 lkf 为了防止模块地址发生变化，导致某些在位，但组数量又配置为0而不采集，导致某些模块通讯状态异常得不到更新
				//				所以增加单模块采集命令
				GetSingle_GrpAndStatus(u8GrpNo, &gs_CANComm, &gs_CANComm);

				s_u8Step++;
				u8GrpNo = s_u8Step - SINGLE_RECT_CMD_NO_GetGrp1Info;
			}

			if(gEvChargeInfo.u8PowerModuleNumCfg[u8GrpNo] > 0)
			{
				if(gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE)
				{
					GetSingle_VoltCurr(u8GrpNo, &gs_CANComm, &gs_CANComm);	
//by lxr1202  模块内部温度
					GetRECT_TEM_0X23(u8GrpNo, &gs_CANComm, &gs_CANComm);
					GetRECT_MAXTEM_0X23(u8GrpNo, &gs_CANComm, &gs_CANComm);	
//by lxr1202  模块1110 1111故障码
					GetRECT_bStatFaltCode1110_PCS_0X23(u8GrpNo, &gs_CANComm, &gs_CANComm);
					GetRECT_bStatFaltCode1111_PCS_0X23(u8GrpNo, &gs_CANComm, &gs_CANComm);
//by lxr01202  MPPT温度
					GetMPPT_DCTEM_0X0E(u8GrpNo, &gs_CANComm, &gs_CANComm);
					GetMPPT_PFCTEM_0X0E(u8GrpNo, &gs_CANComm, &gs_CANComm);
					GetMPPT_DC_TRANS_TEM_0X0E(u8GrpNo, &gs_CANComm, &gs_CANComm);
					GetMPPT_PFC_TRANS_TEM_0X0E(u8GrpNo, &gs_CANComm, &gs_CANComm);


				}
				else
				{
					 //20220416 PCS 改成每圈都采集
					if(!((iGIdx >= ACDC_GRP_NO_MIN) && (iGIdx <= ACDC_GRP_NO_MAX)))	
					{
						GetSingle_VoltCurr(u8GrpNo, &gs_CANComm, &gs_CANComm);	
					}
				}

				// if((u8GrpNo >= ACDC_GRP_NO_MIN) && (u8GrpNo <= ACDC_GRP_NO_MAX))
				// 	GetSingle_PhaseVolts(u8GrpNo, &gs_CANComm, &gs_CANComm);
				//这里为了降低CAN负载率，单步读取
				if((u8GrpNo >= ACDC_GRP_NO_MIN) && (u8GrpNo <= ACDC_GRP_NO_MAX))
				{
					if(s_u8ReadPhaseInfoStep == 0)
					{
						PCS_ReadPhaseVolt_0x23(u8GrpNo, eType_AB);
					}
					else if(s_u8ReadPhaseInfoStep == 1)
					{
						PCS_ReadPhaseVolt_0x23(u8GrpNo, eType_BC);
					}
					else if(s_u8ReadPhaseInfoStep == 2)
					{
						PCS_ReadPhaseVolt_0x23(u8GrpNo, eType_CA);
					}
					else if(s_u8ReadPhaseInfoStep == 3)
					{
						PCS_ReadPhaseCurr_0x23(u8GrpNo, eType_AB);
						PCS_ReadPhaseCurr_0x23(u8GrpNo, eType_BC);
						PCS_ReadPhaseCurr_0x23(u8GrpNo, eType_CA);
					}
					// else if(s_u8ReadPhaseInfoStep == 4)
					// {
					// 	PCS_ReadPhaseCurr_0x23(u8GrpNo, eType_BC);
					// }
					// else if(s_u8ReadPhaseInfoStep == 5)
					// {
					// 	PCS_ReadPhaseCurr_0x23(u8GrpNo, eType_CA);
					// }
				}
				//GetSingle_PhaseVolts(u8GrpNo, &gs_CANComm, &gs_CANComm);

				//这里为了降低CAN负载率，单步读取
				if((u8GrpNo >= ACDC_GRP_NO_MIN) && (u8GrpNo <= ACDC_GRP_NO_MAX))
				{
					if(s_u8ReadPowerStep == 0)
					{
						PCS_ReadReactivePower_0x23(u8GrpNo, eType_Total);
					}
					else if(s_u8ReadPowerStep == 1)
					{
						PCS_ReadReactivePower_0x23(u8GrpNo, eType_AB);
					}
					else if(s_u8ReadPowerStep == 2)
					{
						PCS_ReadReactivePower_0x23(u8GrpNo, eType_BC);
					}
					else if(s_u8ReadPowerStep == 3)
					{
						PCS_ReadReactivePower_0x23(u8GrpNo, eType_CA);
					}
					else if(s_u8ReadPowerStep == 4)
					{
						PCS_ReadActivePower_0x23(u8GrpNo, eType_Total);
					}
					else if(s_u8ReadPowerStep == 5)
					{
						PCS_ReadActivePower_0x23(u8GrpNo, eType_AB);
					}
					else if(s_u8ReadPowerStep == 6)
					{
						PCS_ReadActivePower_0x23(u8GrpNo, eType_BC);
					}
					else if(s_u8ReadPowerStep == 7)
					{
						PCS_ReadActivePower_0x23(u8GrpNo, eType_CA);
					}
				}

				if((s_u8FirstGetFlag > 0)
					|| ((u8GrpNo >= MPPT_GRP_NO_MIN && u8GrpNo <= MPPT_GRP_NO_MAX) && (s_u8FirstGetFlag_MPPT > 0)))
				{

					if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)|| (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
						&& (u8GrpNo >= ACDC_GRP_NO_MIN && u8GrpNo <= ACDC_GRP_NO_MAX))
					{
						GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11240000);
						GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11250000);
						GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11260000);
						GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11270000);
						GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11280000);
						GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11290000);

						GetSingle_VerInfo(u8GrpNo, &gs_CANComm, &gs_CANComm);
					}
					else
					{
						GetSingle_SerialNo(u8GrpNo, &gs_CANComm, &gs_CANComm);
						GetSingle_VerInfo(u8GrpNo, &gs_CANComm, &gs_CANComm);
					}

					s_u8ReadSerealNoStep = 0;
				}
				//20220701 lkf 防止漏了序列号
				else
				{
					if(((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)|| (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
						&& (u8GrpNo >= ACDC_GRP_NO_MIN && u8GrpNo <= ACDC_GRP_NO_MAX))
					{
						if(s_u8ReadSerealNoStep == 0)
						{
							GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11240000);
						}
						else if(s_u8ReadSerealNoStep == 1) 
						{
							GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11250000);
						}
						else if(s_u8ReadSerealNoStep == 2) 
						{
							GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11260000);							
						}
						else if(s_u8ReadSerealNoStep == 3) 
						{
							GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11270000);
						}
						else if(s_u8ReadSerealNoStep == 4) 
						{
							GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11280000);
						}
						else if(s_u8ReadSerealNoStep == 5) 
						{
							GetSingle_Barcode(u8GrpNo, &gs_CANComm, 0x11290000);
						}						
						else if(s_u8ReadSerealNoStep == 6) 
						{
							GetSingle_VerInfo(u8GrpNo, &gs_CANComm, &gs_CANComm);
						}					
					}
					else
					{
						if(s_u8ReadSerealNoStep == 0)
						{
							GetSingle_SerialNo(u8GrpNo, &gs_CANComm, &gs_CANComm);
						}
						else if(s_u8ReadSerealNoStep == 1)
						{
							GetSingle_VerInfo(u8GrpNo, &gs_CANComm, &gs_CANComm);
						}					
					}					
				}
			}  // if(gEvChargeInfo.u8PowerModuleNumCfg[u8GrpNo] > 0)

		
			s_u8Step++;
			if(s_u8Step > SINGLE_RECT_CMD_NO_GetGrp12Info)
			{
				s_u8Step = 0;  // SINGLE_RECT_CMD_NO_GetGrp12Info 必须是最后一个

				s_u8ReadSerealNoStep++;
				if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)|| (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				 {
					if(s_u8ReadSerealNoStep >= 7)
					{
						s_u8ReadSerealNoStep = 0;
					}
				}
				else if(s_u8ReadSerealNoStep >= 2)
				{
					s_u8ReadSerealNoStep = 0;
				}

				s_u8ReadPowerStep++;
				if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)|| (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				 {
					if(s_u8ReadPowerStep >= 8)
					{
						s_u8ReadPowerStep = 0;
					}
				}

				s_u8ReadPhaseInfoStep++;
				if((gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)|| (gEvChargeInfo.u8ModuleType == ACDC_IS_BEG_TYPE) || (gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE))
				 {
					if(s_u8ReadPhaseInfoStep >= 4)
					{
						s_u8ReadPhaseInfoStep = 0;
					}
				}


				for(iGIdx = 0; iGIdx < mRECTNUM; iGIdx++)
				{
					if(gs_CANComm.RectData[iGIdx].nCommFailTimes > 10)
					{
						gs_CANComm.RectData[iGIdx].bCommFail = TRUE;
					}
				}

				for(iGIdx = 0; iGIdx < mMPPTNUM; iGIdx++)
				{
					if(gs_CANComm.MpptData[iGIdx].nCommFailTimes > 10)
					{
						gs_CANComm.MpptData[iGIdx].bCommFail = TRUE;
					}
				}


				if(gs_CANComm.u8SingleRectScanCmdCnt > 0)
				{
					gs_CANComm.u8SingleRectScanCmdCnt--;
				}

				if(s_u8FirstGetFlag > 0)
				{
					s_u8FirstGetFlag--;
				}

				if(s_u8FirstGetFlag_MPPT > 0)
				{
					s_u8FirstGetFlag_MPPT--;
				}

				nTotalRectNum = 0; 
				for(iGIdx = 0; iGIdx < mMPPTNUM; iGIdx++)
				{
					if(gs_CANComm.MpptData[iGIdx].nCommFailTimes > 10)
					{
						gs_CANComm.MpptData[iGIdx].bCommFail = TRUE;
					}
					else if(gs_CANComm.MpptData[iGIdx].bIsExisted == TRUE)
					{
						nTotalRectNum++;						
					}
				}
				if(nTotalRectNum > 0)
				{
					if(s_u8MpptRepowerFlag == 0)
					{
						s_u8FirstGetFlag_MPPT = 2;
					}
					s_u8MpptRepowerFlag = 1;

					if(s_u8MpptNum < nTotalRectNum)
					{
						s_u8MpptNum = nTotalRectNum;
						s_u8FirstGetFlag_MPPT = 2;
					}
				}
				else
				{
					s_u8MpptRepowerFlag = 0;
					s_u8MpptNum = 0;
				}
			}

			break;
		

		default:
			s_u8Step = 0;
			break;
	}

}

void NoSampDeal(void)
{
	UINT8 i, u8Idx, uctemp;
	UINT8 u8IsFail[_MAX_GRP_NUM];
	UINT8 u8AcIsUnderFail = TRUE;
	UINT8 u8AcIsOverFail = FALSE;
	UINT8 u8IsAnyRectAlm[_MAX_GRP_NUM];
	UINT8 u8IsGrpAllRectDcOff[_MAX_GRP_NUM];

	UINT8 u8ForwardNum[_MAX_GRP_NUM];
	UINT8 u8ReverseNum[_MAX_GRP_NUM];

	UINT8 u8ProtectNum[_MAX_GRP_NUM];

	UINT8 u8RectNum;
	static UINT8 su8LastRectNum = 0;
	UINT8 u8RectTemparatureIsValid;
	float fRectTemparature;

	if(gs_CANComm.u8AddressScanDelay > 0)
	{
		return;
	}

	//模块在升级
	if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
	{
		return;
	}

	if((gEvChargeInfo.u8ACInputContactorCtrlStat == DO_ON)&&(gEvChargeInfo.u8ACInputContactorCloseAlmDelayFlag == TRUE))
	{
		return;
	}

	if(gs_CANComm.u8SingleRectScanCmdCnt > 0)
	{
		return;
	}


	u8RectNum = 0;
	for(i = 0; i < _MAX_GRP_NUM; i++)
	{
		u8RectNum += gs_CANComm.GrpData[i].nCommRectNum;

	}

	if (u8RectNum >= su8LastRectNum)
	{
		su8LastRectNum = u8RectNum;
	}

	for(i = 0; i < _MAX_GRP_NUM; i++)
	{
		u8IsFail[i] = TRUE;
		u8IsAnyRectAlm[i] = FALSE;
		u8IsGrpAllRectDcOff[i] = TRUE;

		u8ForwardNum[i] = 0;
		u8ReverseNum[i] = 0;
		u8ProtectNum[i] = 0;
	}	

	u8RectTemparatureIsValid = 0;
	fRectTemparature = -40;

	//for (u8Idx= 0; u8Idx < su8LastRectNum; u8Idx++)
	for (u8Idx= 0; u8Idx < mRECTNUM; u8Idx++)	
	{
		if (TRUE == gs_CANComm.RectData[u8Idx].bIsExisted)
		{
			//20200604 获取温度
			if (FALSE == gs_CANComm.RectData[u8Idx].bCommFail)
			{
				u8RectTemparatureIsValid = 1;
				if(fRectTemparature < gs_CANComm.RectData[u8Idx].fRealTime_Temp)
				{
					fRectTemparature = gs_CANComm.RectData[u8Idx].fRealTime_Temp;
				}


				for(i = 0; i < _MAX_GRP_NUM; i++)
				{
					if ((GRP_NO_MIN+1+i) == gs_CANComm.RectData[u8Idx].nBelongedGrp)
					{
						//u8IsFail[GRP_NO_MIN+i] = FALSE;
						break;
					}
				}	

				if(i < _MAX_GRP_NUM)
				{

					if((gEvChargeInfo.u8ModuleType != ACDC_IS_PCS_TYPE) || (gEvChargeInfo.u8ModuleType != ACDC_IS_BEG_TYPE))
					{
						if (FALSE == gs_CANComm.RectData[u8Idx].bCommFail
							&& FALSE == (gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x10)
							&& FALSE == (gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x08)
							//	&& FALSE == (gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x04)
							&& FALSE == (gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x02))
						{
							u8IsFail[GRP_NO_MIN+i] = FALSE;
						}

						if((gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x01) == 0)
						{
							u8IsGrpAllRectDcOff[GRP_NO_MIN+i] = FALSE;
						}	

						if ((TRUE == gs_CANComm.RectData[u8Idx].bCommFail)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode[2] & RECT_STAT_CODE_2_ALM_BITs) != 0)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode[1] & RECT_STAT_CODE_1_ALM_BITs) != 0)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode[0] & RECT_STAT_CODE_0_ALM_BITs) != 0))
						{
							u8IsAnyRectAlm[GRP_NO_MIN+i] = TRUE;
						}

						if((gs_CANComm.RectData[u8Idx].bConvertStat & 0x01) != 0)
						{
							u8ReverseNum[i]++;
						}		
						else
						{
							u8ForwardNum[i]++;
						}
					}
	
					else if((i >= ACDC_GRP_NO_MIN && i <= ACDC_GRP_NO_MAX) )
					{
						if ((FALSE == gs_CANComm.RectData[u8Idx].bCommFail)
							&& ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[2] & RECT_STAT_CODE_2_FALT_BITs_PCS) == 0)
							&& ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[1] & RECT_STAT_CODE_1_FALT_BITs_PCS) == 0)
							&& ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & RECT_STAT_CODE_0_FALT_BITs_PCS) == 0)
							//&& ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[3] & RECT_STAT_CODE_3_FALT_BITs_PCS) == 0)
							&& ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[3] & 0xFE) == 0)
							&& ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & 0x07) != 3)  //故障
							)
						{
							u8IsFail[GRP_NO_MIN+i] = FALSE;
						}

						if((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & 0x07) == 0x01)
						{
							u8IsGrpAllRectDcOff[GRP_NO_MIN+i] = FALSE;
						}	

						if ((TRUE == gs_CANComm.RectData[u8Idx].bCommFail)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[2] & RECT_STAT_CODE_2_ALM_BITs_PCS) != 0)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[1] & RECT_STAT_CODE_1_ALM_BITs_PCS) != 0)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & RECT_STAT_CODE_0_ALM_BITs_PCS) != 0)
							//|| ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[3] & RECT_STAT_CODE_3_ALM_BITs_PCS) != 0)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[3] & 0xFE) != 0)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & 0x07) == 3)  //故障
							|| ((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & 0x07) == 4)  //告警
							)
						{
							u8IsAnyRectAlm[GRP_NO_MIN+i] = TRUE;
						}

						if((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & 0x07) == 0x01)
						{
							if((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & 0x10) != 0)
							{
								u8ReverseNum[i]++;
							}
							else if((gs_CANComm.RectData[u8Idx].bStatCode_Invert[0] & 0x20) != 0)
							{
								u8ForwardNum[i]++;
							}
						}						
					}
					else 
					{
						if (FALSE == gs_CANComm.RectData[u8Idx].bCommFail
							&& FALSE == (gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x10)
							&& FALSE == (gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x08)
							//	&& FALSE == (gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x04)
							&& FALSE == (gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x02))
						{
							u8IsFail[GRP_NO_MIN+i] = FALSE;
						}

						if((gs_CANComm.RectData[u8Idx].bStatCode[1] & 0x01) == 0)
						{
							u8IsGrpAllRectDcOff[GRP_NO_MIN+i] = FALSE;
						}	

						if ((TRUE == gs_CANComm.RectData[u8Idx].bCommFail)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode[2] & RECT_STAT_CODE_2_ALM_BITs) != 0)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode[1] & RECT_STAT_CODE_1_ALM_BITs) != 0)
							|| ((gs_CANComm.RectData[u8Idx].bStatCode[0] & RECT_STAT_CODE_0_ALM_BITs) != 0))
						{
							u8IsAnyRectAlm[GRP_NO_MIN+i] = TRUE;
						}

						if((gs_CANComm.RectData[u8Idx].bConvertStat & 0x01) != 0)
						{
							u8ReverseNum[i]++;
						}		
						else
						{
							u8ForwardNum[i]++;
						}
					}
				
				} //if(i < _MAX_GRP_NUM)
			}  //if (FALSE == gs_CANComm.RectData[u8Idx].bCommFail)
		}// end:  if (TRUE == gs_CANComm.RectData[u8Idx].bIsExisted)
	} //end: for (u8Idx= 0; u8Idx < mRECTNUM; u8Idx++)	

	gs_CANComm.bRectTemperatureIsValid = u8RectTemparatureIsValid;
	gs_CANComm.fMaxRectTemperature = fRectTemparature;


	for(i = 0; i < _MAX_GRP_NUM; i++)
	{
#ifdef MPPT_Enable
		if((i >= MPPT_GRP_NO_MIN) && (i <= MPPT_GRP_NO_MAX))
		{
			//MPPT模块不处理
			continue;
		}
#endif


		gs_CANComm.GrpData[i].u8ConvertStat_ForwardNum = u8ForwardNum[i];
		gs_CANComm.GrpData[i].u8ConvertStat_ReverseNum = u8ReverseNum[i];
		gs_CANComm.GrpData[i].u8ConvertStat_ProtectNum = u8ProtectNum[i];		
		

		if(gEvChargeInfo.u8PowerModuleNumCfg[i] == 0)
		{
			gs_CANComm.GrpData[i].bIsAnyRectAlarm = FALSE;
			gs_CANComm.GrpData[i].u8IsNoNormalRect = FALSE;
			gs_CANComm.GrpData[i].bIsAllDCOutClose = FALSE;
			continue;
		}

		//判断A组是否无正常模块，不能充电
		if(u8IsFail[i] == TRUE)
		{
			gs_CANComm.GrpData[i].u8IsNoNormalRect = TRUE;
		}
		else
		{
			gs_CANComm.GrpData[i].u8IsNoNormalRect = FALSE;
		}
		//判断A组是否存在模块告警
		if(u8IsAnyRectAlm[i] == TRUE)
		{
			gs_CANComm.GrpData[i].bIsAnyRectAlarm = TRUE;
		}
		else
		{
			gs_CANComm.GrpData[i].bIsAnyRectAlarm = FALSE;
		}

		//判断整组是否已关闭DC输出
		if(u8IsGrpAllRectDcOff[i] == TRUE)
		{
			gs_CANComm.GrpData[i].bIsAllDCOutClose = TRUE;
		}
		else
		{
			gs_CANComm.GrpData[i].bIsAllDCOutClose = FALSE;
		}

	} // for(i = 0; i < _MAX_GRP_NUM; i++)				

	
	if(gEvChargeInfo.u8ACInputContactorCloseAlmDelayFlag == FALSE)		
	{

		//for (u8Idx= 0; u8Idx < su8LastRectNum; u8Idx++)
		for (u8Idx= 0; u8Idx < mRECTNUM; u8Idx++)	
		{
			if ((gs_CANComm.RectData[u8Idx].bIsExisted == TRUE)
				&& ((gs_CANComm.RectData[u8Idx].nBelongedGrp >= (ACDC_GRP_NO_MIN+1))&&(gs_CANComm.RectData[u8Idx].nBelongedGrp <= (ACDC_GRP_NO_MAX+1))))
			{
				if (gs_CANComm.RectData[u8Idx].fAB_Volt > 330
					|| gs_CANComm.RectData[u8Idx].fBC_Volt > 330
					|| gs_CANComm.RectData[u8Idx].fCA_Volt > 330)                //判断是否欠压
				{
					u8AcIsUnderFail = FALSE;
				}
				if (gs_CANComm.RectData[u8Idx].fAB_Volt > 510
					|| gs_CANComm.RectData[u8Idx].fBC_Volt > 510
					|| gs_CANComm.RectData[u8Idx].fCA_Volt > 510)               //判断是否过压
				{
					u8AcIsOverFail = TRUE;
				}
			}
		}

		if (TRUE == u8AcIsUnderFail)
		{
			gs_CANComm.u8IsACUnder = TRUE;
		}
		else
		{
			gs_CANComm.u8IsACUnder = FALSE;
		}

		if (u8AcIsOverFail == TRUE)
		{
			gs_CANComm.u8IsACOver = TRUE;
		}
		else
		{
			gs_CANComm.u8IsACOver = FALSE;
		}
	}




	//模块不正常，清输出电压、电流
	for (u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
	{			
		if((!gs_CANComm.bIsGrpExist[u8Idx])
			|| (gs_CANComm.bIsGrp_NotAvailable[u8Idx] == TRUE)
			|| (gEvChargeInfo.u8PowerModuleNumCfg[u8Idx] == 0))
		{
			//模块不正常，清输出电压、电流
			gs_CANComm.GrpData[u8Idx].fOutVolt = 0;					
			gs_CANComm.GrpData[u8Idx].fOutCurr = 0; 
			gs_CANComm.GrpData[u8Idx].fExtVolt = 0;
			gs_CANComm.GrpData[u8Idx].fAvailableCurr = 0;


			gs_CANComm.GrpData[u8Idx].fRatedPower = 0;
			gs_CANComm.GrpData[u8Idx].fMax_OutCurr = 0;
			gs_CANComm.GrpData[u8Idx].fRatedMaxCurr = 0;
			gs_CANComm.GrpData[u8Idx].nCommRectNum = 0;	

			gs_CANComm.GrpData[u8Idx].TotalActivePower = 0;
			gs_CANComm.GrpData[u8Idx].TotalReactivePower = 0;		
		}
	}


	//判断所有模块通信中断	
	if(gEvChargeInfo.u8ACInputContactorCloseAlmDelayFlag == FALSE)
	{
		uctemp = TRUE;
		//for (u8Idx = 0; u8Idx < su8LastRectNum; u8Idx++)
		for (u8Idx= 0; u8Idx < mRECTNUM; u8Idx++)	
		{
			if (FALSE == gs_CANComm.RectData[u8Idx].bCommFail)
			{
				uctemp = FALSE;
				break;
			}
		}
		if (uctemp == TRUE)
		{
			gs_CANComm.u8IsAllCommFail = TRUE;
		}
		else 
		{
			gs_CANComm.u8IsAllCommFail = FALSE;
		}
	}
}


/************************************************/
//函数名称:										
//函数功能:		
//当用户修桩数量时和桩序号时，需要重新配置CAN屏蔽码，以及环网节点配置
//更改日期:      								
/************************************************/
void Reflash_CAN_SelftAddr(void)
{
	static unsigned char su8FirstFlag = 1;
	static UCHAR	s_u8IsRingEnabled_Bak;
	UCHAR i;

	//切换环网使能需要初始化
	if(s_u8IsRingEnabled_Bak != gEvChargeInfo.u8IsRingEnabled)
	{
		s_u8IsRingEnabled_Bak = gEvChargeInfo.u8IsRingEnabled;
		su8FirstFlag = 1;
		gs_CANComm.fInitRectData(0);
	}

	//非环网模式
	//if(gEvChargeInfo.u8IsRingEnabled == FALSE) 
	{
		if(gEvChargeInfo.u8IsRingEnabled == FALSE)
		{
			gs_CANComm.bIsRingEnabled = FALSE;
		}
		else
		{
			gs_CANComm.bIsRingEnabled = TRUE;
		}
		if(su8FirstFlag == 1)
		{
			su8FirstFlag = 0;
			gs_CANComm.bySelfAddress = 0xF0;
			for(i = 0; i < _MAX_GRP_NUM; i++)
			{
				gs_CANComm.byGrpIdx[i] = i+1;
			}
			
			InitCan3Port();
		}		
	}
	
}



#define AC_3_PHASE_LOW_VOLT		320
#define AC_3_PHASE_OVER_VOLT	440


//检查是否需要重新初始化CAN3口
void ReInitRectCanCommPort(void)
{
	UCHAR i, u8AllModuleIsOff;

	u8AllModuleIsOff = 1;
	for(i = ACDC_GRP_NO_MIN; i <= ACDC_GRP_NO_MAX; i++)
	{
		if(gs_CANComm.u8GRP_RectOnOff[i] == RECT_ON)
		{
			u8AllModuleIsOff = 0;
			break;
		}
	}
	for(i = DCDC_GRP_NO_MIN; i <= DCDC_GRP_NO_MAX; i++)
	{
		if(gs_CANComm.u8GRP_RectOnOff[i] == RECT_ON)
		{
			u8AllModuleIsOff = 0;
			break;
		}
	}

	if (gs_CANComm.u8IsAllCommFail == FALSE)
	{
		gs_CANComm.u16AllCommFailResetDelay = 0;
	}	

	if((gs_CANComm.u16AllCommFailResetDelay > 60)
		|| ((gs_CANComm.u16RectCommPeriodResetDelay > 1440)  //24小时
			&& (u8AllModuleIsOff == 1))
		)
	{	
		gs_CANComm.u16AllCommFailResetDelay = 0;
		gs_CANComm.u16RectCommPeriodResetDelay = 0;
		gs_Can3TxMng.ucClearBufFlag = 1;
		BSP_InitSPI3();
		OSTimeDly(500);
		InitCan3Port();
		OSTimeDly(500);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
	}
}



////20190924 Debu处理
void Debug_Grp_Deal(void)
{
	UCHAR i, u8GRP_WorkModeSet, uRectOnOff;
	float ftemp;

	if(gEvChargeInfo.u8TEST_MODE_FLAG == FALSE)
	{
		return;
	}
	
	if(gs_CANComm.u8AddressScanDelay > 0) 
	{
		return;
	}

	//模块在升级
	if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
	{
		return;
	}
	for(i = 0; i < _MAX_GRP_NUM; i++)
	{		
		uRectOnOff = RECT_OFF;

		/*if(i ==  GRP_E_DCDC || i ==  GRP_F_DCDC)  
		{
		u8GRP_WorkModeSet = GRP_WORKMODE_SET_MPPT;
		}
		else */
		if(gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[i] == IMMU2ManualCtrlGrpCmd_Chging)
		{
			if((i >=  GRP_5_MPPT) && (i <=  GRP_8_MPPT))  
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_MPPT;
			}
			else if((i >=  GRP_1_ACDC) && (i <=  GRP_4_ACDC)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_FORWORD;
			}
			else  if((i >=  GRP_9_DCDC) && (i <=  GRP_12_DCDC)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_FORWORD;
			}
			uRectOnOff = RECT_ON;
		}
		else if(gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[i] == IMMU2ManualCtrlGrpCmd_Dischging)
		{
			if((i >=  GRP_5_MPPT) && (i <=  GRP_8_MPPT)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_MPPT;
			}
			else if((i >=  GRP_1_ACDC) && (i <=  GRP_4_ACDC)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_REVERSE;
			}
			else  if((i >=  GRP_9_DCDC) && (i <=  GRP_12_DCDC)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_FORWORD;
			}

			uRectOnOff = RECT_ON;
		}
		else if(gs_IMMU2_LCCU_Mng.u8ManualCtrlGrpCmd[i] == IMMU2ManualCtrlGrpCmd_Stop)
		{
			if((i >=  GRP_5_MPPT) && (i <=  GRP_8_MPPT))  
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_MPPT;
			}
			else if((i >=  GRP_1_ACDC) && (i <=  GRP_4_ACDC)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_FORWORD;
			}
			else  if((i >=  GRP_9_DCDC) && (i <=  GRP_12_DCDC)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_FORWORD;
			}
			uRectOnOff = RECT_OFF;
		}
		else
		{
			if((i >=  GRP_5_MPPT) && (i <=  GRP_8_MPPT))  
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_MPPT;
			}
			else if((i >=  GRP_1_ACDC) && (i <=  GRP_4_ACDC)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_FORWORD;
			}
			else  if((i >=  GRP_9_DCDC) && (i <=  GRP_12_DCDC)) 
			{
				u8GRP_WorkModeSet = GRP_WORKMODE_SET_FORWORD;
			}
			uRectOnOff = RECT_OFF;
		}

		ftemp = gs_IMMU2_LCCU_Mng.fManualCtrlGrpCurr[i];
		if(ftemp < 0.5)
		{
			ftemp = 0.1;
		}

		gs_CANComm.fSetRectCtlVoltCurr(i, gs_IMMU2_LCCU_Mng.fManualCtrlGrpVolt[i], ftemp);

		gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC+i] = uRectOnOff;

		gs_CANComm.u8GRP_WorkModeSet[i] = u8GRP_WorkModeSet;
	}
	
}
void InitUpgradePowerModule(void)
{
	//UINT8 i;
	memset((UCHAR *)&gs_UpgradePowerModuleCtrl, 0, sizeof(UpgradePowerModule));

	gs_UpgradePowerModuleCtrl.u8OnlinePowerModuleTypeNum = 0;
	//sprintf( (char*)(&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[0][0]), "0:V1704U1.bin");
	//sprintf( (char*)(&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[0][0]), "0:V1704U2.bin");
	//sprintf( (char*)(&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U1[1][0]), "0:V1705U1.bin");
	//sprintf( (char*)(&gs_UpgradePowerModuleCtrl.cOnlinePowerModuleTypeName_U2[1][0]), "0:V1705U2.bin");

	gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[0] = 1;
	gs_UpgradePowerModuleCtrl.u8_POWER_ON_Flag[1] = 1;
	gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] = FALSE;
	gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] = FALSE;
	gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] = 0;
	gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] = 0;

	//Upgrde state init
	gs_UpgradePowerModuleCtrl.u8CurrModuleAddr = 0;
	gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;
	gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
	gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
	gs_UpgradePowerModuleCtrl.u8UpgradeErrCode = 0;
}


void UnPackData_MpptGrp0(UINT8 u8RcvMsgNum, UINT8 u8DevNo,  UINT8 u8Cmd, UINT8 u8Addr)
{
	UINT8 u8Idx;
	UINT8 u8RcvSrcAddr,u8RcvCmd, u8RcvDevNo,u8RcvErrCode, u8RcvDstAddr;

	if(u8RcvMsgNum > CAN_3_MAX_BUFF)
	{
		return ;
	}	

	for (u8Idx = 0; u8Idx < u8RcvMsgNum; u8Idx++)
	{
		u8RcvErrCode = ((RxMsg[u8Idx].ExtId>>26)&0x07);	
		u8RcvCmd = ((RxMsg[u8Idx].ExtId>>16)&0x3f);	
		u8RcvDevNo = ((RxMsg[u8Idx].ExtId>>22)&0x0f);
		u8RcvDstAddr = ((RxMsg[u8Idx].ExtId>>8)&0xff);
		u8RcvSrcAddr = (RxMsg[u8Idx].ExtId & 0xff);

		if (u8RcvErrCode != RECT_RET_ERR_Code_NO_ERR) 
		{
			continue;
		}

		if ((u8Cmd == u8RcvCmd) && (u8DevNo == u8RcvDevNo) && (u8RcvDstAddr == R_PROTO_SELF_ADDR))
		{
			//u8Buf[0] = RxMsg[u8Idx].Data[0]; u8Buf[1] = RxMsg[u8Idx].Data[1];
			//u8Buf[2] = RxMsg[u8Idx].Data[2]; u8Buf[3] = RxMsg[u8Idx].Data[3];
			//u8Buf[4] = RxMsg[u8Idx].Data[4]; u8Buf[5] = RxMsg[u8Idx].Data[5];
			//u8Buf[6] = RxMsg[u8Idx].Data[6]; u8Buf[7] = RxMsg[u8Idx].Data[7];

			if((u8DevNo == R_PRPTO_DEVNO_0B) && (u8Addr == MPPT_GRP_REAL_GRP_NO))
			{
				if((u8RcvSrcAddr == MPPT_GRP_REAL_GRP_NO) && (u8Cmd == 0x02))
				{
					s_u8MpptGrp0NumBuf = (UINT8)RxMsg[u8Idx].Data[2];
				}
			}

			//u8Rtn = ERR_CAN_R_NO_ERR;	
		}		
	} /// for (u8Idx = 0; u8Idx < u8RcvMsgNum; u8Idx++)

}


void QueryGrp_0_MpptNum(void)
{	
	UINT8 iIdx;
	canmsg SendMsg;
	UINT8 u8RcvMsgNum;

	if(gEvChargeInfo.u8QueryMpptNumDelay != 0)
	{
		return;
	}
	else
	{
		gEvChargeInfo.u8QueryMpptNumDelay = gEvChargeInfo.u8QueryMpptNumInterval;
	}

	//读模块数
	SendMsg.Id = (R_PRPTO_DEVNO_0B << 22) + (0x02 << 16) + (MPPT_GRP_REAL_GRP_NO << 8) + R_PROTO_SELF_ADDR;			//组 1 2 

	SendMsg.Data[0] = 0;
	SendMsg.Data[1] = 0;

	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);

	OSTimeDly(8);

	s_u8MpptGrp0NumBuf = 0;

	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		UnPackData_MpptGrp0(u8RcvMsgNum, R_PRPTO_DEVNO_0B, 0x02, MPPT_GRP_REAL_GRP_NO);		
	}

	if(s_u8MpptGrp0NumBuf > 0)
	{
		SendMsg.Data[0] =  ((MPPT_GRP_NO+1) <<24);
		SendMsg.Data[1] = 0;
		for(iIdx = 0; iIdx < mMPPTNUM_A; iIdx++)
		{
			//设置模块组号
			SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (0x16 << 16) + ((MPPT_MODULE_BASE_ADDR+iIdx) << 8) + R_PROTO_SELF_ADDR;			
			ClenRBuff();
			gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
			OSTimeDly(3);
		}
#ifdef	MPPT_GRP_NO_2
		SendMsg.Data[0] =  ((MPPT_GRP_NO_2+1) <<24);
		SendMsg.Data[1] = 0;
		for(iIdx = 0; iIdx < mMPPTNUM_B; iIdx++)
		{
			//设置模块组号
			SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (0x16 << 16) + ((MPPT_MODULE_BASE_ADDR+mMPPTNUM_A+iIdx) << 8) + R_PROTO_SELF_ADDR;			
			ClenRBuff();
			gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
			OSTimeDly(3);
		}
#endif
#ifdef	MPPT_GRP_NO_3
		SendMsg.Data[0] =  ((MPPT_GRP_NO_3+1) <<24);
		SendMsg.Data[1] = 0;
		for(iIdx = 0; iIdx < mMPPTNUM_C; iIdx++)
		{
			//设置模块组号
			SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (0x16 << 16) + ((MPPT_MODULE_BASE_ADDR+mMPPTNUM_A+mMPPTNUM_B+iIdx) << 8) + R_PROTO_SELF_ADDR;			
			ClenRBuff();
			gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
			OSTimeDly(3);
		}
#endif
#ifdef	MPPT_GRP_NO_4
		SendMsg.Data[0] =  ((MPPT_GRP_NO_4+1) <<24);
		SendMsg.Data[1] = 0;
		for(iIdx = 0; iIdx < mMPPTNUM_D; iIdx++)
		{
			//设置模块组号
			SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (0x16 << 16) + ((MPPT_MODULE_BASE_ADDR+mMPPTNUM_A+mMPPTNUM_B+mMPPTNUM_C+iIdx) << 8) + R_PROTO_SELF_ADDR;			
			ClenRBuff();
			gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
			OSTimeDly(3);
		}
#endif

	}
}

void ManualClearPCSFaltCmd(UINT8 u8GrpNo)
{
	UINT8 u8CMD = 0x24;									//设置walkin
	__XXX_UNION_VALUE untemp;
	__16BITS_UNION_VALUE untemp_16bits;
	UINT32 uiDH,uiDL;



	if (u8GrpNo >= _MAX_GRP_NUM)
	{
		return;
	}


	untemp.u32Value = 0;
	untemp.chtemp[3] = 0x71;   
	untemp.chtemp[2] = 0x4C;   
	uiDH = untemp.u32Value;

	untemp_16bits.u16Temp = (UINT16)0xA1;  

	untemp.u32Value = 0;
	untemp.chtemp[1] = untemp_16bits.chtemp[1];
	untemp.chtemp[0] = untemp_16bits.chtemp[0];
	uiDL = untemp.u32Value;

	u8PackSendMsg(R_PRPTO_DEVNO_0B, u8CMD, u8GrpNo, uiDH, uiDL);
	OSTimeDly(3);
}


//20220701 lkf 增加转发IMMU2的命令
void DealCmdFromIMMU2(void)        //转发MU2下发的模块参数命令
{	
	UINT8 u8RcvMsgNum, i, j;
	canmsg SendMsg;
	union
	{
		unsigned char uctemp[4];
		UINT32 u32temp;
	}uniontemp;

	if(gs_IMMU2_LCCU_Mng.u8NeedAckPcsCmdResult != 1)
	{
		return;
	}
	uniontemp.uctemp[3] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[0];   
	uniontemp.uctemp[2] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[1];
	uniontemp.uctemp[1] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[2];
	uniontemp.uctemp[0] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[3];

	SendMsg.Id = uniontemp.u32temp;

	uniontemp.uctemp[3] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[4];   
	uniontemp.uctemp[2] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[5];
	uniontemp.uctemp[1] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[6];
	uniontemp.uctemp[0] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[7];

	SendMsg.Data[0] = uniontemp.u32temp;
	
	uniontemp.uctemp[3] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[8];   
	uniontemp.uctemp[2] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[9];
	uniontemp.uctemp[1] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[10];
	uniontemp.uctemp[0] = gs_IMMU2_LCCU_Mng.u8PcsCmdBuf[11];

	SendMsg.Data[1] = uniontemp.u32temp;

	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);

	OSTimeDly(15);

	
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		if(u8RcvMsgNum > 10)
			u8RcvMsgNum = 10;
		gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn_Num = u8RcvMsgNum;

		for(j = 0; j < u8RcvMsgNum; j++)
		{
			uniontemp.u32temp = RxMsg[j].ExtId;
			gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn[j][0] = uniontemp.uctemp[3];		
			gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn[j][1] = uniontemp.uctemp[2];
			gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn[j][2] = uniontemp.uctemp[1];   
			gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn[j][3] = uniontemp.uctemp[0]; 

			for(i = 0; i < 8; i++)
			{
				gs_IMMU2_LCCU_Mng.u8PcsCmdBuf_Rtn[j][4+i] = RxMsg[j].Data[i];  
			}
		}

		gs_IMMU2_LCCU_Mng.u8NeedAckPcsCmdResult = 2;	
	}
	//没收到回复，直接清0
	else
	{
		gs_IMMU2_LCCU_Mng.u8AckPcsRtn = 0x02;		//invalid command
		gs_IMMU2_LCCU_Mng.u8NeedAckPcsCmdResult = 2;
	}

}

void DealRawCanMsgToIMMU2(canmsg * pRcvMsg, UINT8 u8GSNo, UINT8 IsSend)       //处理原始CAN消息并将其分配到IMMU2的消息队列中
{
	//前5个MSG放发送，后10个MSG放接受
	UINT8 i = 0;

	if(gs_IMMU2_LCCU_Mng.u8IsTrigger != TRUE)
	{
		return;
	}

	if(gs_IMMU2_LCCU_Mng.u8CanMsgType != 0xFF)
	{
		if(u8GSNo != gs_IMMU2_LCCU_Mng.u8CanMsgType)
		{
			return;
		}
	}

	if(IsSend == TRUE)
	{
		for(i=0; i<5; i++)
		{
			if(gs_IMMU2_LCCU_Mng.CANRawSendMsg.IsExist[i] == 0)
			{
				gs_IMMU2_LCCU_Mng.CANRawSendMsg.IsExist[i] = TRUE;
				gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Id = pRcvMsg->Id;
				gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Data[0] = pRcvMsg->Data[0];
				gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Data[1] = pRcvMsg->Data[1];
				break;
			}
		}
	}
	else
	{
		for(i=5; i<CAN_MSG_MAX_NUM; i++)
		{
			if(gs_IMMU2_LCCU_Mng.CANRawSendMsg.IsExist[i] == 0)
			{
				gs_IMMU2_LCCU_Mng.CANRawSendMsg.IsExist[i] = TRUE;
				gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Id = pRcvMsg->Id;
				gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Data[0] = pRcvMsg->Data[0];
				gs_IMMU2_LCCU_Mng.CANRawSendMsg.SendMsg[i].Data[1] = pRcvMsg->Data[1];
				break;
			}
		}
	}
}

/************************************************/
//函数名称:										
//函数功能:										
//更改日期:      								
/************************************************/
void TASK_CAN3_Proc(void* pdata)
{
	UINT8 iIdx = 0;
	
	//__AC_MIN_MCCB_ON(); __AC_MIN_MCCB_ON(); __AC_MIN_MCCB_ON(); __AC_MIN_MCCB_ON();
	for (iIdx = 0; iIdx < 10; iIdx++)
	{
		OSTimeDly(10);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
	}
	BSP_InitSPI3();
	OSTimeDly(50);	
	InitgCANComm();

	InitUpgradePowerModule();

	gs_CANComm.fInitRectData(1);
	Reflash_CAN_SelftAddr();	

	//TEST
	UINT8 tmp = mcp2515_ReadRegister(MCP_CANSTAT);					//读控制寄存器
	tmp = mcp2515_ReadRegister(MCP_CANCTRL);
	tmp = mcp2515_ReadRegister(MCP_CANSTAT);
	tmp = mcp2515_ReadRegister(MCP_CNF1);
	tmp = mcp2515_ReadRegister(MCP_CNF2);
	tmp = mcp2515_ReadRegister(MCP_CNF3);

	OSTimeDly(4);
	gEvChargeInfo.u8RectUpgradeFlag = FALSE;
	gEvChargeInfo.u8Power_on_Delay = 0;
	//gEvChargeInfo.u8_TotalMCCBDelayTimes = 0;
	

	//OSTimeDly(4);
	SendAddressingCmd();
	gs_CANComm.u8AddressScanDelay = RECT_ADDRESS_SCAN_DELAY;
	gs_CANComm.u8SingleRectScanCmdCnt = SINGLE_RECT_SCAN_LOOP_NUM;
	OSTimeDly(4);

	//__PV_INPUT_ON();
	
	//开机后需下发一次关闭静默，防止断电、异常重启等情况下，下级设备还处在静默
#if ENABLE_OTA_RECT_SILLENCE
	for(UINT8 i = 0; i < 3; ++i)
		SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif

	for(;;)
	{
		
		for (iIdx = MPPT_GRP_NO_MIN; iIdx <= MPPT_GRP_NO_MAX; iIdx++)
		{
			gs_CANComm.u8GRP_WorkModeSet[iIdx] = GRP_WORKMODE_SET_MPPT;
		}
		gEvChargeInfo.fMaxOutVolt = 1000;
		gEvChargeInfo.fSysSetOverVolt = 1020;

	
		//发数据 并延时
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		OSTimeDly(3);

		//交流输入接触器控制
		if(gs_CANComm.u8AddressScanDelay == 0)  //监控刚上电时先不作控制
		{
			
		}
		else
		{
			gs_CANComm.u16AllCommFailResetDelay = 0;
		}
		
		//20190924 Debu处理
		Debug_Grp_Deal();


		//if (gEvChargeInfo.u8TEST_MODE_FLAG == TRUE)//线程不运行
		//{
		//	continue;
		//}

		if(u8_ABU_U1_UpDataProc())
		{
			continue;
		}

		if(u8_ABU_U2_UpDataProc())
		{
			continue;
		}

		if(u8_U1_UpDataProc())                   //PCS U1升级
		{
			continue;
		}


		if(u8_U2_UpDataProc())                 //PCS U2升级
		{
			continue;
		}


		if(u8_MPPT_U1_UpDataProc())                  //by lxr01202
		{
			continue;
		}


		if(u8_MPPT_U2_UpDataProc())                //by lxr01202
		{
			continue;
		}


		gEvChargeInfo.u8RectUpgradeFlag = FALSE;

		//CAN通信地址刷新
		Reflash_CAN_SelftAddr();		
		
		//检查是否需要重新初始化CAN3口
		ReInitRectCanCommPort();

		//查询光伏模块
		QueryGrp_0_MpptNum();

		if(gEvChargeInfo.u8ManualClearPCSFaltCmd > 0)
		{
			ManualClearPCSFaltCmd(gEvChargeInfo.u8ManualClearPCSFaltCmd);
			gEvChargeInfo.u8ManualClearPCSFaltCmd = 0;
		}

		//各模组输出管理
		TaskConverterOutAction();

		SetAndGetGroupCommand();
		gSelfTest.fSetTaskID(TASK_ID_CAN3);

		SetAndGetSingleCommand();
		gSelfTest.fSetTaskID(TASK_ID_CAN3);

		DealCmdFromIMMU2();

		if(gs_CANComm.u8AddressScanDelay > 0)
		{
			gs_CANComm.u8AddressScanDelay--;
		}

		NoSampDeal();
		ABU_Comm();
	}
}
