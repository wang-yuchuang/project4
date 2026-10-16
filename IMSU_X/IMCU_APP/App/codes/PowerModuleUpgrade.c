#include "main.h"
#include "can_ring.h"
#include "Uart6_new.h"
#include "HistoryEvent.h"
#include "Upgrade.h"
#include "ABU.h"

UpgradePowerModule *gpRectOTA = &gs_UpgradePowerModuleCtrl;

#define R_PRPTO_DEVNO_0A	0x0A
#define R_PRPTO_DEVNO_0B	0x0B

#define CAN_RECT_SENDFILE_DEVNO				0x1F
#define	CAN_CONTROLLER_SELF_ADDR 			0xF0
#define CAN_RECT_UPDT_SEND_DEVNO			0x02
#define CAN_RECT_UPDT_SEND_CMDNO			0x7F
#define CAN_RECT_NEW_FORCRE_TRIG_DCDC		0x04
#define CAN_RECT_NEW_FORCRE_TRIG_PFC		0x44
#define CAN_RECT_FILETRANS_SEND_DCDC_06		0x08
#define CAN_RECT_FILETRANS_RECV_DCDC_NEXT	0x09
#define CAN_RECT_FILETRANS_SEND_DCDC_32		0x10
#define CAN_RECT_FILETRANS_SEND_PFC_06		0x48
#define CAN_RECT_FILETRANS_RECV_PFC_NEXT	0x49
#define CAN_RECT_FILETRANS_SEND_PFC_32		0x50

#define CAN_REQUIRE_ADDR_01_FLAG			0x01
#define CAN_REQUIRE_DATA_02_FLAG			0x02
#define CAN_REQUIRE_INVALID_FLAG			0x5E
#define CAN_UPGRADE_MAX_BLOCK_NUM			256

#define R_PROTO_BROADCAST		0x3F
#define R_PROTO_SELF_ADDR		gs_CANComm.bySelfAddress

#define IS_UPGRADE_DC	TRUE
#define IS_UPGRADE_PFC	FALSE

//模块回复的错误码定义
typedef enum tagRECT_RET_ERR_Code
{
	RECT_RET_ERR_Code_NO_ERR = 0x00,
	RECT_RET_ERR_Code_WRONG_ADDR = 0x01,
	RECT_RET_ERR_Code_WRONG_CMD = 0x02,
	RECT_RET_ERR_Code_WRONG_DATA = 0x03,
	RECT_RET_ERR_Code_IN_STARTING = 0x07, //地址重排未完成/**/
}RECT_RET_ERR_Code;

extern UINT8 s_u8FirstGetFlag_MPPT;
extern CanRxMsgTypeDef RxMsg[CAN_3_MAX_BUFF];
extern void ClenRBuff(void);
extern ERR_CODE_CAN_R SetGrp_OnOff_No10(int SGNo, void* pRawData,void* pSetData);
extern ERR_CODE_CAN_R GetSingle_VoltCurr_Dev_0A(int SGNo, void* pRawData,void* pSetData);
extern ERR_CODE_CAN_R GetSingle_VerInfo_SingleModule(int SGNo, void* pRawData,void* pSetData);
extern void SendAddressingCmd(void);

//by lxr01202
//NOINIT UINT8 u8UpgradeBuffer_NextData[BYTES_NUM_PER_PAGE + BYTES_NUM_PER_PAGE + BYTES_NUM_PER_PAGE];
NOINIT UINT8 u8UpgradeBuffer_NextData[BYTES_NUM_PER_PAGE + BYTES_NUM_PER_PAGE];

NOINIT UINT8 u8UpgradeBuffer[BYTES_NUM_PER_PAGE + BYTES_NUM_PER_PAGE];
//NOINIT UINT8 u8UpgradeBuffer[BYTES_NUM_PER_PAGE + BYTES_NUM_PER_PAGE + BYTES_NUM_PER_PAGE];

UINT32 u32StartReadUAddr = 0;
void ReadFileContex(UINT32 u32Addr, UINT8 bDCDC, UINT16 u16Len);
void ReadFile_NextData(UINT32 u32Addr, UINT8 bDCDC, UINT16 u16Len);
void SendTrigRCT_Old(BYTE byRectAddress, BOOL bDCDC, BOOL bForced);						//6980783
void SendTrigRCT_New(BYTE byRectAddress, BOOL bDCDC, BOOL bForced);
UINT8 Wait_RectRequst_4Next(BYTE byRectAddress, UINT8 bDCDC, int nTimeOut_mSec, UINT32* pNeedStartAddr, UINT16* pNeedRLen, UINT8 u8NeedCheckAddr);
UINT8 Triggle_Rect_4Updt(BYTE byRectAddress, BOOL bDCDC, BOOL bForced, BOOL bIsDisChkRectType);
void Send_Bin_Contex_2_Rect(BYTE byRectAddress, UINT8 bDCDC, UINT16 u16WantLengh);
static UINT8 Handle_RectUpdt_4One(BYTE byAddress, BOOL bDCDC, BOOL isForceUpgrade);
UINT16 u16Calc1CRC(UINT8* u8pData, UINT16 u16Len);
void AfterOneUpgrade(void);
void Dear_FailsRECT_Upgrade( BOOL bDCDC);
void SendTrigZEROAddr(UINT8 u8SAddr);
UINT8 u8RcvMsg(UINT16 u8TimeOutMS);
//editor cjy
void CAN3_ReInit();
void SendRectSillenceModeBroadcast(BYTE byRectAddress, UINT8 u8Data);
static void UpgradeSingleRect(UINT8 u8RecAddr, UINT8 bDCDC);
void RefreshRectOnOffCtrl();

void UpDataBeep(UINT8 u8IsLong, UINT8 u8Times, UINT32 u8BeepInterval, UINT32 u8BeepOffInterval);
#define BEEP_ON_LONG	TRUE
#define BEEP_ON_OFF		FALSE
void UpDataBeep(UINT8 u8IsLong, UINT8 u8Times, UINT32 u8BeepInterval, UINT32 u8BeepOffInterval)
{
	UINT8 u8Idx = 0;
	for(u8Idx = 0; u8Idx< u8Times; u8Idx++)
	{
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		dv_Output.fOpenBeep();
		OSTimeDly(u8BeepInterval);
		if(!u8IsLong)
		{
			dv_Output.fCloseBeep();
            OSTimeDly(u8BeepOffInterval);
		}
		else
		{
			OSTimeDly(500);
			gSelfTest.fSetTaskID(TASK_ID_CAN3);
		}
		
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
	}


	dv_Output.fCloseBeep();
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
}

static void UpgradePowerModuleBeep(INT8U times, INT32U timeout)
{
	INT8U i = 0u;

	for (i = 0u; i < times; i++)
	{
		dv_Output.fOpenBeep();
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		OSTimeDly(timeout);

		dv_Output.fCloseBeep();
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		if (i < (times-1))
		{
			OSTimeDly(timeout);
		}
	}
}

UINT32 Math_round_100(float Num)
{
	UINT32 uTemp = 0;
	UINT32 a = 0;

	uTemp = (UINT32)(Num * 1000);
	a = uTemp % 10;

	if (a > 4)
	{
		uTemp += 10;
	}

	return uTemp / 10;
}

UINT32 Math_round_1000(float Num)
{
	UINT32 uTemp = 0;
	UINT32 a = 0;

	uTemp = (UINT32)(Num * 10000);
	a = uTemp % 10;

	if (a > 4)
	{
		uTemp += 10;
	}

	return uTemp / 10;
}

UINT8 u8RcvMsg(UINT16 u8TimeOutMS)
{
	UINT8 u8Rst = 0;
	UINT16 u16Idx = 0;
	u8TimeOutMS = MAX(6,u8TimeOutMS);
	for (u16Idx = 0; u16Idx < u8TimeOutMS/6; u16Idx++)
	{
		OSTimeDly(3);
		u8Rst = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
		if (u8Rst > 0)
		{
			return u8Rst;
		}
	}
	return u8Rst;
}
UINT8 UnPackBoarstData(UINT8 u8RcvMsgNum, UINT8 u8DevNo,UINT8 u8Cmd, CANCommData_TypeDef* PData)
{
	UINT8 u8RcvSrcAddr,u8RcvCmd, u8RcvDevNo,u8RcvErrCode, u8RcvDstAddr, u8Rtn, u16temp;
	UINT8 u8Idx = 0;UNUSED(u8Idx);
	UINT8 u8Addr = 0;UNUSED(u8Addr);
	UINT8 u8Buf[8];
        u8RcvSrcAddr = 0; UNUSED(u8RcvSrcAddr);

	if(u8RcvMsgNum > CAN_3_MAX_BUFF)
	{
		return ERR_CAN_R_COMM_FAIL;
	}

	u8Rtn = ERR_CAN_R_COMM_FAIL;

	for (u8Idx = 0; u8Idx < u8RcvMsgNum; u8Idx++)
	{
		u8RcvErrCode = ((RxMsg[u8Idx].ExtId>>26)&0x07);	
		u8RcvCmd = ((RxMsg[u8Idx].ExtId>>16)&0x3f);	
		u8RcvDevNo = ((RxMsg[u8Idx].ExtId>>22)&0x0f);
		u8RcvDstAddr = ((RxMsg[u8Idx].ExtId>>8)&0xff);	

		//地址重排
		if(u8RcvErrCode == RECT_RET_ERR_Code_IN_STARTING)
		{
			gs_CANComm.bNeedReConfig = TRUE;
			return ERR_CAN_R_IN_STARTING;
		}

		if (u8RcvErrCode != RECT_RET_ERR_Code_NO_ERR) 
		{
			continue;
		}

		if ((u8Cmd == u8RcvCmd) && (u8DevNo == u8RcvDevNo) && (u8RcvDstAddr == R_PROTO_SELF_ADDR))
		{
			u8Buf[0] = RxMsg[u8Idx].Data[0]; u8Buf[1] = RxMsg[u8Idx].Data[1];
			u8Buf[2] = RxMsg[u8Idx].Data[2]; u8Buf[3] = RxMsg[u8Idx].Data[3];
			u8Buf[4] = RxMsg[u8Idx].Data[4]; u8Buf[5] = RxMsg[u8Idx].Data[5];
			u8Buf[6] = RxMsg[u8Idx].Data[6]; u8Buf[7] = RxMsg[u8Idx].Data[7];
			if (R_PRPTO_DEVNO_0A == u8DevNo)
			{
				u8Rtn = ERR_CAN_R_NO_ERR;
				switch(u8Cmd)
				{
				case 0x02:
					gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = u8Buf[2];
					gs_UpgradePowerModuleCtrl.u8RealReplyCommRCT_Num = u8Buf[2];
					break;
				case 0x23:
					if((u8Buf[0] == 0x10) && (u8Buf[1] == 0x10))
					{
						u16temp = ((UINT16)u8Buf[6]<<8) + u8Buf[7];
						if(u16temp > mRECTNUM)
						{
							u16temp = mRECTNUM;
						}
						gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = u16temp;
						gs_UpgradePowerModuleCtrl.u8RealReplyCommRCT_Num = u16temp;
					}
					break;
				default:
					break;
				}
			}
		}
	}

	return u8Rtn;
}

UINT8 UnPackABUVerData(UINT8 u8RcvMsgNum, UINT8 u8DevNo,UINT8 u8Cmd)
{
	UINT8 u8RcvCmd, u8RcvDevNo,u8RcvErrCode, u8RcvDstAddr, u8RcvSrcAddr, u8Rtn;
	UINT8 u8Idx = 0;UNUSED(u8Idx);
	UINT8 u8Addr = 0;UNUSED(u8Addr);

	if(u8RcvMsgNum > CAN_3_MAX_BUFF)
	{
		return ERR_CAN_R_COMM_FAIL;
	}

	u8Rtn = ERR_CAN_R_COMM_FAIL;

	for (u8Idx = 0; u8Idx < u8RcvMsgNum; u8Idx++)
	{
		u8RcvErrCode = ((RxMsg[u8Idx].ExtId>>26)&0x07);	
		u8RcvCmd = ((RxMsg[u8Idx].ExtId>>16)&0x3f);	
		u8RcvDevNo = ((RxMsg[u8Idx].ExtId>>22)&0x0f);
		u8RcvDstAddr = ((RxMsg[u8Idx].ExtId>>8)&0xff);	
		u8RcvSrcAddr = (RxMsg[u8Idx].ExtId & 0xff);

		//地址重排
		if(u8RcvErrCode == RECT_RET_ERR_Code_IN_STARTING)
		{
			gs_CANComm.bNeedReConfig = TRUE;
			return ERR_CAN_R_IN_STARTING;
		}

		if (u8RcvErrCode != RECT_RET_ERR_Code_NO_ERR) 
		{
			continue;
		}

		if ((u8Cmd == u8RcvCmd) && (u8DevNo == u8RcvDevNo) && (u8RcvDstAddr == R_PROTO_SELF_ADDR) && (u8RcvSrcAddr == ABU_Addr))
		{
			if (R_PRPTO_DEVNO_0A == u8DevNo)
			{
				u8Rtn = ERR_CAN_R_NO_ERR;
				switch(u8Cmd)
				{
					case 0x07:
						ABU->DC_Version  = (float)(RxMsg[u8Idx].Data[0]) * 100 + (float)(RxMsg[u8Idx].Data[1] >> 4) * 10 + (float)(RxMsg[u8Idx].Data[1] & 0x0F);
						ABU->PFC_Version  = (float)(RxMsg[u8Idx].Data[2]) * 100 + (float)(RxMsg[u8Idx].Data[3] >> 4) * 10 + (float)(RxMsg[u8Idx].Data[3] & 0x0F);
						break;
					default:
						break;
				}
			}
		}
	}

	return u8Rtn;
}

ERR_CODE_CAN_R CAN_Get_RCT_Num(void* pRawData,void* pSetData)
{
	UINT8 u8RcvMsgNum;
	UINT8 u8Rtn;
	//SGNo 组号 1 2
	UINT8 u8CMD;
	CANCommData_TypeDef* PData = (CANCommData_TypeDef*)pRawData;
	canmsg SendMsg;


	/*if(gEvChargeInfo.u8ModuleType == ACDC_IS_PCS_TYPE)
	{
		u8CMD = 0x23;
		SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (u8CMD << 16) + ((R_PROTO_BROADCAST) << 8) + R_PROTO_SELF_ADDR;
		SendMsg.Data[0] = 0x10100000;
		SendMsg.Data[1] = 0;
	}
	else*/
	{
		u8CMD = 0x02;
		SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (u8CMD << 16) + ((R_PROTO_BROADCAST) << 8) + R_PROTO_SELF_ADDR;
		SendMsg.Data[0] = 0;
		SendMsg.Data[1] = 0;		
	}
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
		ClenRBuff();
		gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(300);
	u8RcvMsgNum = u8RcvMsg(36);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackBoarstData(u8RcvMsgNum, R_PRPTO_DEVNO_0A, u8CMD, PData);		
	}
	else
	{
		u8Rtn = ERR_CAN_R_WRONG_ADDR;
	}

	return (ERR_CODE_CAN_R)u8Rtn;
}

ERR_CODE_CAN_R GetABU_VerInfo(void)
{
	UINT8 u8RcvMsgNum;
	UINT8 u8Rtn;
	UINT8 u8CMD;	 
	canmsg SendMsg;

	u8CMD = 0x07;
	SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (u8CMD << 16) + ((ABU_Addr) << 8) + R_PROTO_SELF_ADDR;
	SendMsg.Data[0] = 0;
	SendMsg.Data[1] = 0;		
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(15);
	u8RcvMsgNum = u8RcvMsg(36);
	if (u8RcvMsgNum > 0)
	{
		u8Rtn = UnPackABUVerData(u8RcvMsgNum, R_PRPTO_DEVNO_0A, u8CMD);
	}
	else
	{
		u8Rtn = ERR_CAN_R_COMM_FAIL;
	}
	return (ERR_CODE_CAN_R)u8Rtn;
}

int GetOnline_RectNum(void)
{
	int j = 0;
	gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = 0;
	gs_UpgradePowerModuleCtrl.u8RealReplyCommRCT_Num = 0;
	
	UINT8 u8Rtn = ERR_CAN_R_WRONG_ADDR;

	for(j = 0; j < 3; j++)
	{
		u8Rtn = CAN_Get_RCT_Num(&gs_CANComm, &gs_CANComm);
		if(ERR_CAN_R_NO_ERR == u8Rtn)
		{
			break;
		}
	}
		
	// if(ERR_CAN_R_NO_ERR == u8Rtn)
	// {
	// 	return gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num;
	// }
	gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = MAX(1, gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num);
	//gs_UpgradePowerModuleCtrl.u8UpDatedTotalRCTNum = 0; 
    return gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num;
}


void SendTrigRCT_Old(BYTE byRectAddress, BOOL bDCDC, BOOL bForced)
{
	canmsg SendMsg;
	UINT32 uiDH, uiDL;
	uiDH = 0;
	uiDL = 0;
	if(!bForced)
	{
		SendMsg.Id = 0x06980783 + (((UINT32)byRectAddress)<<11);//old
		uiDH = 0x03f000fe;
		uiDL = ((bDCDC ? 1 : 2)) << 16;				//数据域区分 DC  PFC
	}
	else
	{
		//强制  ID 区分 DC还是PFC
		//1F0400f0		1F4400F0
		SendMsg.Id = (CAN_RECT_SENDFILE_DEVNO<<24) + ((bDCDC ? CAN_RECT_NEW_FORCRE_TRIG_DCDC : CAN_RECT_NEW_FORCRE_TRIG_PFC) <<16) + (byRectAddress<<8) + CAN_CONTROLLER_SELF_ADDR;
		uiDH = 0;
		uiDL = 0;
	}
	SendMsg.Data[0] = uiDH;
	SendMsg.Data[1] = uiDL;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(3);
        return;
}

void SendTrigRCT_New(BYTE byRectAddress, BOOL bDCDC, BOOL bForced)
{
	canmsg SendMsg_2;
	UINT32 uiDH, uiDL;
	//027F00F0  01  DC触发     02 PFC触发		03PFC 强制触发		04 DC强制下载	
	SendMsg_2.Id  = (CAN_RECT_UPDT_SEND_DEVNO<<24) + (CAN_RECT_UPDT_SEND_CMDNO<<16) + (byRectAddress<<8) + CAN_CONTROLLER_SELF_ADDR;
	uiDH = 0;
	uiDL = 0;
	//*B----1：DC下载触发；2：BOOST (PFC)下载触发，3：PFC强制下载，4：DC强制下载 5：DCBOOT 6：PFCBOOT
	if(bDCDC)
	{
		uiDL = 0x01;//(UINT32)(bForced ? 0x04 : 0x01);                  //byData[7]
	}
	else
	{
		uiDL = 0x02;//(UINT32)(bForced ? 0x03 : 0x02);                 //byData[7]
	}

//	if(gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == RECT_IS_PFCBOOT_UPDATE_FLAG)		//BOOT单独处理
//	{
//		uiDL = 0x06;                  								//byData[7]
//	}
//	 if(gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == RECT_IS_DCDCBOOT_UPDATE_FLAG)
//	{
//		uiDL = 0x05;                 								//byData[7]
//	}
	SendMsg_2.Data[0] = uiDH;
	SendMsg_2.Data[1] = uiDL;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	//ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg_2, 8);
	OSTimeDly(3);
}

void SendTrigZEROAddr(UINT8 u8SAddr)
{
	canmsg SendMsg_2;
	UINT32 uiDH, uiDL;	
	SendMsg_2.Id  = 0x024200f0;
	SendMsg_2.Id = SendMsg_2.Id + ((UINT16)u8SAddr<<8);
	uiDH = 0;
	uiDL = 0;
	SendMsg_2.Data[0] = uiDH;
	SendMsg_2.Data[1] = uiDL;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg_2, 8);
	OSTimeDly(2);
}

UINT8 Check_RectModelName(BYTE byRectAddress, UINT8 bDCDC)
{
	char cModelName[8];
	UINT8 u8RcvMsgNum;
	UINT8 u8RcvSrcAddr, u8RcvCmd, u8RcvDevNo, u8RcvDstAddr;
	UINT8 u8Idx = 0;
	UINT16 u16temp;
	canmsg SendMsg;

	SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (0x0B << 16) + (byRectAddress << 8) + CAN_CONTROLLER_SELF_ADDR;	

	SendMsg.Data[0] = 0;
	SendMsg.Data[1] = 0;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(5);

	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		if(u8RcvMsgNum > CAN_3_MAX_BUFF)
		{
			return ERR_CAN_R_COMM_FAIL;
		}

		for (u8Idx = 0; u8Idx < u8RcvMsgNum; u8Idx++)
		{
			u8RcvCmd = ((RxMsg[u8Idx].ExtId>>16)&0x3f);	
			u8RcvDevNo = ((RxMsg[u8Idx].ExtId>>22)&0x0f);
			u8RcvDstAddr = ((RxMsg[u8Idx].ExtId>>8)&0xff);	
			u8RcvSrcAddr = (RxMsg[u8Idx].ExtId & 0xff);

			if ((u8RcvCmd == 0xB) 
				&& (u8RcvDstAddr == CAN_CONTROLLER_SELF_ADDR)
				&& (byRectAddress == u8RcvSrcAddr)
				&& (u8RcvDevNo == R_PRPTO_DEVNO_0A))
			{
					u16temp = ((UINT16)RxMsg[u8Idx].Data[6]<<8) + ((UINT16)RxMsg[u8Idx].Data[7]);
					cModelName[0] = RxMsg[u8Idx].Data[0];
					if(bDCDC)
					{
						sprintf( (char*)(cModelName+1), "%04dU2", u16temp);
						cModelName[7] = 0;
						if(strcmp(gs_UpgradePowerModuleCtrl.cU2BinFileName_Upgrade, cModelName) == 0)
						{
							return ERR_CAN_R_NO_ERR;
						}
					}
					else
					{						
						sprintf( (char*)(cModelName+1), "%04dU1", u16temp);
						cModelName[7] = 0;
						if(strcmp(gs_UpgradePowerModuleCtrl.cU1BinFileName_Upgrade, cModelName) == 0)
						{
							return ERR_CAN_R_NO_ERR;
						}
					}
			}
		}//END for
	} // end: if (u8RcvMsgNum > 0)



	return ERR_CAN_R_COMM_FAIL;
}
void CtrlRectOff_BroadCast(void)
{
	UINT32 u32DataH, u32DataL;
	//float ftemp;
	canmsg SendMsg;
	

	UINT8 u8CMD = 0x1A;
//by lxr01202  使得模块升级时不发送关机命令
//	u32DataH = ((UINT32)RECT_OFF<<24);	
	u32DataH = ((UINT32)RECT_ON<<24);	
	u32DataL = 0;	

	SendMsg.Id = ((UINT32)R_PRPTO_DEVNO_0A << 22) + ((UINT32)u8CMD << 16) + ((UINT32)0x3F << 8) + R_PROTO_SELF_ADDR;			
	
	SendMsg.Data[0] = u32DataH;
	SendMsg.Data[1] = u32DataL;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);

	OSTimeDly(3);

}

UINT8 Wait_RectRequst_4Next(BYTE byRectAddress, UINT8 bDCDC, int nTimeOut_mSec, UINT32* pNeedStartAddr, UINT16* pNeedRLen, UINT8 u8NeedCheckAddr)
{
	UINT8 u8Buf[8];
	UINT8 u8RcvMsgNum;
	UINT8 u8RcvSrcAddr, u8RcvCmd, u8RcvDevNo, u8RcvDstAddr;
	UINT8 u8FindFlag = FALSE;
	UINT16 ReadLen = 0;
	UINT32 ReadAddr = 0;
	UINT8 u8Rtn = ERR_CAN_R_COMM_FAIL;
	UINT8 u8Idx = 0;
	UINT8 u8II = 0;
	UINT8 u8NeedAddedFlag = FALSE;
	int i = 0;
	for(i = 0; i < nTimeOut_mSec; i++)
	{
		if(nTimeOut_mSec!=1)OSTimeDly(5);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
		if (u8RcvMsgNum > 0)
		{
			if(u8RcvMsgNum > CAN_3_MAX_BUFF)
			{
				gs_UpgradePowerModuleCtrl.u8UpgradeErrCode = 1;
				return ERR_CAN_R_COMM_FAIL;
			}

			for (u8Idx = 0; u8Idx < u8RcvMsgNum; u8Idx++)
			{
				//u8RcvErrCode = ((RxMsg[u8Idx].ExtId>>26)&0x07);	
				u8RcvCmd = ((RxMsg[u8Idx].ExtId>>16)&0xff);	
				u8RcvDevNo = ((RxMsg[u8Idx].ExtId>>24)&0xff);
				u8RcvDstAddr = ((RxMsg[u8Idx].ExtId>>8)&0xff);	
				u8RcvSrcAddr = (RxMsg[u8Idx].ExtId & 0xff);
				//地址重排
				//if(u8RcvErrCode == RECT_RET_ERR_Code_IN_STARTING)
				//{
				//	gs_CANComm.bNeedReConfig = TRUE;
				//	return ERR_CAN_R_IN_STARTING;
				//}

				//if (u8RcvErrCode != RECT_RET_ERR_Code_NO_ERR) 
				//{
				//	continue;
				//}

				//if (((u8RcvCmd == CAN_RECT_FILETRANS_RECV_DCDC_NEXT && (bDCDC == IS_UPGRADE_DC)) || (u8RcvCmd == CAN_RECT_FILETRANS_RECV_PFC_NEXT && (bDCDC == IS_UPGRADE_PFC))) 
				//	&& (u8RcvDstAddr == CAN_CONTROLLER_SELF_ADDR)
				//	&& (((byRectAddress == u8RcvSrcAddr) && (u8NeedCheckAddr == TRUE)) || (u8NeedCheckAddr == FALSE))
				//	&& (u8RcvDevNo == CAN_RECT_SENDFILE_DEVNO))
				if (((u8RcvCmd == CAN_RECT_FILETRANS_RECV_DCDC_NEXT) || (u8RcvCmd == CAN_RECT_FILETRANS_RECV_PFC_NEXT)) 
					&& (u8RcvDstAddr == CAN_CONTROLLER_SELF_ADDR)
					&& (((byRectAddress == u8RcvSrcAddr) && (u8NeedCheckAddr == TRUE)) || (u8NeedCheckAddr == FALSE))
					&& (u8RcvDevNo == CAN_RECT_SENDFILE_DEVNO))
				{
					u8Buf[0] = RxMsg[u8Idx].Data[0]; u8Buf[1] = RxMsg[u8Idx].Data[1];
					u8Buf[2] = RxMsg[u8Idx].Data[2]; u8Buf[3] = RxMsg[u8Idx].Data[3];
					u8Buf[4] = RxMsg[u8Idx].Data[4]; u8Buf[5] = RxMsg[u8Idx].Data[5];
					u8Buf[6] = RxMsg[u8Idx].Data[6]; u8Buf[7] = RxMsg[u8Idx].Data[7];

					ReadAddr = (u8Buf[0] << 24) + (u8Buf[1] << 16) + (u8Buf[2] << 8) + u8Buf[3];
					ReadLen = (u8Buf[4] << 8) + u8Buf[5];
					*pNeedStartAddr = ReadAddr;
					*pNeedRLen = ReadLen;
					gs_UpgradePowerModuleCtrl.u16_PM_ReadLen = ReadLen;
					gs_UpgradePowerModuleCtrl.u32_PM_StartAddress = ReadAddr;
					u8NeedAddedFlag = TRUE;
					for(u8II = 0; u8II < mRECTNUM; u8II++)
					{
						if(gs_UpgradePowerModuleCtrl.u8FailsAddr[u8II] == u8RcvSrcAddr)
						{
							u8NeedAddedFlag = FALSE;			//避免重复加入，所有遍历，简单粗暴			
						}
					}
					if((u8NeedAddedFlag == TRUE) && (u8RcvSrcAddr >= 0x40))
					{
						gs_UpgradePowerModuleCtrl.u8FailsAddr[gs_UpgradePowerModuleCtrl.u8TotalFailsNum++] = u8RcvSrcAddr;
					}

					if(u8NeedCheckAddr == FALSE)
					{
						SendTrigZEROAddr(u8RcvSrcAddr);
					}
			
					if(0x01 == u8Buf[7] && 0x00 == u8Buf[6] && 0x06 == u8Buf[5] && 0x00 == u8Buf[4])//00 06 00 01   模块请求地址
					{
						gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag = CAN_REQUIRE_ADDR_01_FLAG;
					}
					else
					{
						gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag = CAN_REQUIRE_DATA_02_FLAG;
					}

					u8FindFlag = TRUE;
					break;
				}
			}//END for  RcvMsgNum	
		}
		else
		{
			u8Rtn = ERR_CAN_R_WRONG_ADDR;
		}

		if(u8FindFlag == TRUE)
		{
			u8Rtn = ERR_CAN_R_NO_ERR;
			break;
		}
	}//end for

	return u8Rtn;
}

UINT8 Wait_2_RectUpgradingCMD(BYTE byRectAddress, UINT8 bDCDC, int nTimeOut_mSec, UINT32* pNeedStartAddr, UINT16* pNeedRLen, UINT8 u8NeedCheckAddr)
{
	UINT8 u8Buf[8];
	UINT8 u8RcvMsgNum;
	UINT8 u8RcvSrcAddr, u8RcvCmd, u8RcvDevNo, u8RcvDstAddr;
	UINT8 u8FindFlag = FALSE;
	UINT16 ReadLen = 0;
	UINT32 ReadAddr = 0;
	UINT8 u8Rtn = ERR_CAN_R_COMM_FAIL;
	UINT8 u8Idx = 0;
	UINT8 u8II = 0;UNUSED(u8II);
	UINT8 u8NeedAddedFlag = FALSE;UNUSED(u8NeedAddedFlag);
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		if(u8RcvMsgNum > CAN_3_MAX_BUFF)
		{
			return ERR_CAN_R_COMM_FAIL;
		}

		for (u8Idx = 0; u8Idx < u8RcvMsgNum; u8Idx++)
		{
			u8RcvCmd = ((RxMsg[u8Idx].ExtId>>16)&0xff);	
			u8RcvDevNo = ((RxMsg[u8Idx].ExtId>>24)&0xff);
			u8RcvDstAddr = ((RxMsg[u8Idx].ExtId>>8)&0xff);	
			u8RcvSrcAddr = (RxMsg[u8Idx].ExtId & 0xff);

			if (((u8RcvCmd == CAN_RECT_FILETRANS_RECV_DCDC_NEXT) || (u8RcvCmd == CAN_RECT_FILETRANS_RECV_PFC_NEXT)) 
					&& (u8RcvDstAddr == CAN_CONTROLLER_SELF_ADDR)
					&& (((byRectAddress == u8RcvSrcAddr) && (u8NeedCheckAddr == TRUE)) || (u8NeedCheckAddr == FALSE))
					&& (u8RcvDevNo == CAN_RECT_SENDFILE_DEVNO))
			{
				u8Buf[0] = RxMsg[u8Idx].Data[0]; u8Buf[1] = RxMsg[u8Idx].Data[1];
				u8Buf[2] = RxMsg[u8Idx].Data[2]; u8Buf[3] = RxMsg[u8Idx].Data[3];
				u8Buf[4] = RxMsg[u8Idx].Data[4]; u8Buf[5] = RxMsg[u8Idx].Data[5];
				u8Buf[6] = RxMsg[u8Idx].Data[6]; u8Buf[7] = RxMsg[u8Idx].Data[7];

				ReadAddr = (u8Buf[0] << 24) + (u8Buf[1] << 16) + (u8Buf[2] << 8) + u8Buf[3];
				ReadLen = (u8Buf[4] << 8) + u8Buf[5];
				*pNeedStartAddr = ReadAddr;
				*pNeedRLen = ReadLen;
				u8FindFlag = TRUE;
				break;
			}
		}//END for  RcvMsgNum	
	}
	else
	{
		u8Rtn = ERR_CAN_R_WRONG_ADDR;
	}

	if(u8FindFlag == TRUE)
	{
		u8Rtn = ERR_CAN_R_NO_ERR;
	}

	return u8Rtn;
}


UINT8 Triggle_Rect_4Updt(BYTE byRectAddress, BOOL bDCDC, BOOL bForced, BOOL bIsDisChkRectType)
{
	UINT32 iDataAddr = 0;
	UINT16 iRLen = 0;
	
	UINT16 u16Idx = 0;
	UINT8 u8RstFlag = FALSE;

	int iIntervalTimes = 99;

	if(bIsDisChkRectType == FALSE)  //强制升级的不需要进行模块类型检查
	{
#if 0
		//先检查型号类型名称是否与升级文件一致
		if(Check_RectModelName(byRectAddress, bDCDC) != ERR_CAN_R_NO_ERR)
		{
			OSTimeDly(5);

			if(Check_RectModelName(byRectAddress, bDCDC) != ERR_CAN_R_NO_ERR)
			{
				return FALSE;
			}
		}
#endif
	}
	
	for(u16Idx = 0; u16Idx < 500; u16Idx++)	//editor cjy	src=2000
	{
		iIntervalTimes++;
		if(iIntervalTimes >= 98)
		{
			//SendTrigRCT_Old(byRectAddress, bDCDC, bForced);//xry/2022-11-22 delete
			SendTrigRCT_New(byRectAddress, bDCDC, bForced);
			iIntervalTimes = 0;
		}
		OSTimeDly(20);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		gs_UpgradePowerModuleCtrl.u32Last_PM_StartAddress = 0;
		gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag = CAN_REQUIRE_INVALID_FLAG;
		if(ERR_CAN_R_NO_ERR == Wait_RectRequst_4Next(byRectAddress, bDCDC, 1, &iDataAddr, &iRLen, TRUE))
		{
			if(gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag == CAN_REQUIRE_ADDR_01_FLAG
				&& gs_UpgradePowerModuleCtrl.u32_PM_StartAddress == 0x00000000)
			{
				ReadFileContex(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress, bDCDC, gs_UpgradePowerModuleCtrl.u16_PM_ReadLen);
				gs_UpgradePowerModuleCtrl.u32Last_PM_StartAddress = 0;
				if(bDCDC)
				{
					gs_UpgradePowerModuleCtrl.u16RemainBytes = gEvChargeInfo.u16_U2_Bytes;
				}
				else
				{
					gs_UpgradePowerModuleCtrl.u16RemainBytes = gEvChargeInfo.u16_U1_Bytes;
				}
				gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
				gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
				gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
				gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
				ReadFile_NextData(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 6, bDCDC, CAN_UPGRADE_MAX_BLOCK_NUM);
				gs_UpgradePowerModuleCtrl.u16NextBytesNum = CAN_UPGRADE_MAX_BLOCK_NUM;
				gs_UpgradePowerModuleCtrl.u16CRC = u16Calc1CRC(u8UpgradeBuffer_NextData, gs_UpgradePowerModuleCtrl.u16NextBytesNum);
				u8RstFlag = TRUE;
			}
			break;
		}
	}
	return u8RstFlag;
}

void ReadFileContex(UINT32 u32Addr, UINT8 bDCDC, UINT16 u16Len)
{
	UINT32 u32StartAddr = 0;
	if(bDCDC)
	{
		u32StartAddr = (RECT_U2_SAVE_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	}
	else
	{
		u32StartAddr = (RECT_U1_SAVE_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	}
	u32StartAddr = u32Addr + u32StartAddr;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	memset(u8UpgradeBuffer, 0, BYTES_NUM_PER_PAGE+20);    
	SPIFlashRead(u8UpgradeBuffer ,u32StartAddr, u16Len);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}

void ReadFile_NextData(UINT32 u32Addr, UINT8 bDCDC, UINT16 u16Len)
{
	UINT32 u32StartAddr = 0;
	if(bDCDC)
	{
		u32StartAddr = (RECT_U2_SAVE_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	}
	else
	{
		u32StartAddr = (RECT_U1_SAVE_BLOCK_NO * PAGE_NUM_PER_BLOCK * BYTES_NUM_PER_PAGE);
	}
	u32StartAddr = u32Addr + u32StartAddr;
	MutexLock(gEvChargeInfo.hMetex_FLASH);
	memset(u8UpgradeBuffer_NextData, 0, BYTES_NUM_PER_PAGE+20);
	SPIFlashRead(u8UpgradeBuffer_NextData ,u32StartAddr, u16Len);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);
}
UINT8 TxCAN3Buff_Upgrade(canmsg * pRcvMsg, UINT8 u8Len)
{
	UINT8 uRst = FALSE;

	MutexLock(gs_CANComm.hMutexWriteCAN3Port);
	uRst = CAN3_Send(pRcvMsg, u8Len);
	g_u8Can3TxProtectDelay = 2;
	while(g_u8Can3TxProtectDelay > 0);
	MutexUnlock(gs_CANComm.hMutexWriteCAN3Port);

	//WriteTxCAN3Buff(pRcvMsg, u8Len);

	return uRst;
}
void Send_Bin_Addr_2_Rect(BYTE byRectAddress, UINT8 bDCDC)
{
	canmsg SendMsg;
	//UINT32 uiDH, uiDL;
	//uiDH = 0;uiDL= 0;
	u8UpgradeBuffer[7] = gs_UpgradePowerModuleCtrl.u16CRC&0xff;
	u8UpgradeBuffer[6] = (gs_UpgradePowerModuleCtrl.u16CRC>>8)&0xff;
	SendMsg.Id = (CAN_RECT_SENDFILE_DEVNO << 24) + ((bDCDC ? CAN_RECT_FILETRANS_SEND_DCDC_06 : CAN_RECT_FILETRANS_SEND_PFC_06) << 16) + ((byRectAddress) << 8) + CAN_CONTROLLER_SELF_ADDR;
	SendMsg.Data[0] = (u8UpgradeBuffer[0]<<24) + (u8UpgradeBuffer[1]<<16) + (u8UpgradeBuffer[2]<<8)+u8UpgradeBuffer[3];
	SendMsg.Data[1] = (u8UpgradeBuffer[4]<<24) + (u8UpgradeBuffer[5]<<16) + (u8UpgradeBuffer[6]<<8)+u8UpgradeBuffer[7];
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	ClenRBuff();
	if(gs_UpgradePowerModuleCtrl.u16NextBytesNum == 0)
	{
		gs_CANComm.fTxCAN3Buff(&SendMsg, 6);
	}
	else
	{
		gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	}
	OSTimeDly(2);
}


void Send_Bin_Contex_2_Rect(BYTE byRectAddress, UINT8 bDCDC, UINT16 u16WantLengh)
{
	UINT32 iAddr;
	UINT16 iRLen;
	canmsg SendMsg;
	UINT8* pData;
	UINT8 u8Remainder = 0;
	UINT8 u8CyclTimes = 0;
	//UINT8 uVal = 0;
	UINT8 u8Idx = 0;
	BYTE byCmdNo_Start = bDCDC ? CAN_RECT_FILETRANS_SEND_DCDC_32 : CAN_RECT_FILETRANS_SEND_PFC_32;
	pData = u8UpgradeBuffer;

	if(u16WantLengh < 8)
	{
		SendMsg.Id = (CAN_RECT_SENDFILE_DEVNO << 24) + ((byCmdNo_Start) << 16) + ((byRectAddress) << 8) + CAN_CONTROLLER_SELF_ADDR;
		SendMsg.Data[0] = (pData[0]<<24) + (pData[1]<<16) + (pData[2]<<8) + pData[3];
		SendMsg.Data[1] = (pData[4]<<24) + (pData[5]<<16) + (pData[6]<<8) + pData[7];
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		ClenRBuff();
		gs_CANComm.fTxCAN3Buff(&SendMsg, u16WantLengh);
		OSTimeDly(2);
	}
	else
	{
		u8Remainder = u16WantLengh%8;
		u8CyclTimes = ((u16WantLengh - u8Remainder)/8);
		for(u8Idx = 0; u8Idx < u8CyclTimes && (u8Idx < 33); u8Idx++)						//限制32 否则数组越界了！
		{
			SendMsg.Id = (CAN_RECT_SENDFILE_DEVNO << 24) + ((byCmdNo_Start + u8Idx) << 16) + ((byRectAddress) << 8) + CAN_CONTROLLER_SELF_ADDR;
			SendMsg.Data[0] = (pData[0]<<24) + (pData[1]<<16) + (pData[2]<<8) + pData[3];
			SendMsg.Data[1] = (pData[4]<<24) + (pData[5]<<16) + (pData[6]<<8) + pData[7];
			pData = pData + 8;
			gSelfTest.fSetTaskID(TASK_ID_CAN3);
			ClenRBuff();
			if(ERR_CAN_R_NO_ERR == (ERR_CODE_CAN_R)Wait_2_RectUpgradingCMD(byRectAddress, bDCDC, 1, &iAddr, &iRLen, TRUE))
			{
				if((iAddr == 0x00000000) && (iRLen==6))
				{
					break;
				}
			}
			gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
			OSTimeDly(RECT_UPGRADE_SEND_INTERVAL);//xry/20221126 modify		//2023-9-19 edit, src = 10.
		}

		if(u8Remainder > 0)
		{
			SendMsg.Id = (CAN_RECT_SENDFILE_DEVNO << 24) + ((byCmdNo_Start + u8Idx) << 16) + ((byRectAddress) << 8) + CAN_CONTROLLER_SELF_ADDR;
			SendMsg.Data[0] = (pData[0]<<24) + (pData[1]<<16) + (pData[2]<<8) + pData[3];
			SendMsg.Data[1] = (pData[4]<<24) + (pData[5]<<16) + (pData[6]<<8) + pData[7];
			gSelfTest.fSetTaskID(TASK_ID_CAN3);
			//ClenRBuff();
			gs_CANComm.fTxCAN3Buff(&SendMsg, u8Remainder);
			OSTimeDly(2);
		}
	}
}

NOINIT UINT16 u16crcTable[260];
UINT16 u16Calc1CRC(UINT8* u8pData, UINT16 u16Len)
{
	// bulid the crcTable
	UINT8 z;
	UINT16 i = 0;
	memset(u16crcTable, 0, 256);
	for (i = 0; i <= 255; i++)
	{
		u16crcTable[i] = 0;
		z = (UINT8)((i ^ (i >> 1)) & 0x00FF);
		z = (UINT8)((z ^ (z >> 2)) & 0x00FF);
		z = (UINT8)((z ^ (z >> 4)) & 0x00FF);
		//u16crcTable[i] = (UInt16)((Convert.ToBoolean(z & 1)) ? 0xC001 : 0);
		u16crcTable[i] = (UINT16)(((z & 1)) ? 0xC001 : 0);
		u16crcTable[i] ^= (UINT16)((i << 6) ^ (i << 7));
	}

	// cal the crc
	UINT16 uiCRC = 0;
        UINT16 index = 0;
	for (i = 0; i < u16Len; i++)
	{
		index = (UINT16)((UINT16)(uiCRC ^ u8pData[i]) & 0x00FF);
		uiCRC = (UINT16)((uiCRC >> 8) ^ u16crcTable[index]);
	}

	return uiCRC;
}

void DearNexData(UINT8 bDCDC);
void DearNexData(UINT8 bDCDC)
{
	UINT16 u16WantLen = 0;
	if((gs_UpgradePowerModuleCtrl.u16RemainBytes >= (CAN_UPGRADE_MAX_BLOCK_NUM + 20)) && (gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag == CAN_REQUIRE_ADDR_01_FLAG))			//读的是地址！！！ 所以是大于！
	{
		ReadFile_NextData(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 6, bDCDC, CAN_UPGRADE_MAX_BLOCK_NUM);
		gs_UpgradePowerModuleCtrl.u16NextBytesNum = CAN_UPGRADE_MAX_BLOCK_NUM;
		gs_UpgradePowerModuleCtrl.u16CRC = u16Calc1CRC(u8UpgradeBuffer_NextData, gs_UpgradePowerModuleCtrl.u16NextBytesNum);
	}
	else if((gs_UpgradePowerModuleCtrl.u16RemainBytes > 20) && (gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag == CAN_REQUIRE_ADDR_01_FLAG))									//说明还有不足256字节的数据 不可能有等于20的情况
	{
		u16WantLen = (gs_UpgradePowerModuleCtrl.u16RemainBytes - 6 - 6 - 2 - 6);
		ReadFile_NextData(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 6, bDCDC, u16WantLen);
		gs_UpgradePowerModuleCtrl.u16NextBytesNum = u16WantLen;
		gs_UpgradePowerModuleCtrl.u16CRC = u16Calc1CRC(u8UpgradeBuffer_NextData, gs_UpgradePowerModuleCtrl.u16NextBytesNum);
	}
	else if((gs_UpgradePowerModuleCtrl.u16RemainBytes >= 14) && (gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag == CAN_REQUIRE_ADDR_01_FLAG))
	{
		ReadFile_NextData(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 6, bDCDC, 2);
		//ReadFile_NextData(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 12, bDCDC, 2);																					//将去发送6字节地址
		gs_UpgradePowerModuleCtrl.u16NextBytesNum = 2;
		gs_UpgradePowerModuleCtrl.u16CRC = u16Calc1CRC(u8UpgradeBuffer_NextData, gs_UpgradePowerModuleCtrl.u16NextBytesNum);
	}
	else if((gs_UpgradePowerModuleCtrl.u16RemainBytes >= 6) && (gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag == CAN_REQUIRE_ADDR_01_FLAG))
	{
		gs_UpgradePowerModuleCtrl.u16NextBytesNum = 0;
	}
//by lxr01202   修正短包数据长度处理  正确计算剩余数据长度：
	// else if (gs_UpgradePowerModuleCtrl.u16RemainBytes > 0)
	// {
    //       u16WantLen = MIN(gs_UpgradePowerModuleCtrl.u16RemainBytes, CAN_UPGRADE_MAX_BLOCK_NUM);
    //       ReadFile_NextData(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 6,bDCDC,u16WantLen);
    //       gs_UpgradePowerModuleCtrl.u16NextBytesNum = u16WantLen;
	// }
}

UINT8 Send_BinFile_2Rect(BYTE byRectAddress, UINT8 bDCDC)
{
	UINT32 iAddr;
	UINT16 iRLen;
	int iLen = 0;
	UINT16 u16WantLen = 0;UNUSED(u16WantLen);
	int iErrorTimes = 0;
	UINT8 u8FinishedFlag = FALSE;
	gs_UpgradePowerModuleCtrl.u16TimerCount = 0;
	for(;;)
	{
		OSTimeDly(5);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		
		if(gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag == CAN_REQUIRE_ADDR_01_FLAG)
		{
			//co_trace(3u, "<CAN3> Send 01 require resopnse");
			Send_Bin_Addr_2_Rect(byRectAddress, bDCDC);
			gs_UpgradePowerModuleCtrl.u16JustSendBytes = 6;
			gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag = CAN_REQUIRE_INVALID_FLAG;
			iLen = gs_UpgradePowerModuleCtrl.u16RemainBytes - gs_UpgradePowerModuleCtrl.u16JustSendBytes;
			if(iLen <= 0)
			{
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 4, "d", byRectAddress);
				// co_trace(3u, "<PM OTA> No.0x%02X Rect update end, for reason 1.", byRectAddress);
				gs_UpgradePowerModuleCtrl.u16RemainBytes = iLen;
				u8FinishedFlag = TRUE;
				break;
			}
			if( ((gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 6) >= gEvChargeInfo.u16_U2_Bytes) &&  (bDCDC == IS_UPGRADE_DC))
			{
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 5, "d", byRectAddress);
				// co_trace(3u, "<PM OTA> No.0x%02X Rect update end, for reason 2.", byRectAddress);
				gs_UpgradePowerModuleCtrl.u16RemainBytes = iLen;
				u8FinishedFlag = TRUE;
				break;
			}

			if( ((gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 6) >= gEvChargeInfo.u16_U1_Bytes) &&  (bDCDC == IS_UPGRADE_PFC))
			{
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 6, "d", byRectAddress);
				// co_trace(3u, "<PM OTA> No.0x%02X Rect update end, for reason 3.", byRectAddress);
				gs_UpgradePowerModuleCtrl.u16RemainBytes = iLen;
				u8FinishedFlag = TRUE;
				break;
			}
		}

		if(gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag == CAN_REQUIRE_DATA_02_FLAG)
		{
			//co_trace(3u, "<CAN3> Send 02 require resopnse");
			Send_Bin_Contex_2_Rect(byRectAddress, bDCDC, gs_UpgradePowerModuleCtrl.u16_PM_ReadLen);
			gs_UpgradePowerModuleCtrl.u16JustSendBytes = gs_UpgradePowerModuleCtrl.u16_PM_ReadLen;
			gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag = CAN_REQUIRE_INVALID_FLAG;
			iLen = gs_UpgradePowerModuleCtrl.u16RemainBytes - gs_UpgradePowerModuleCtrl.u16JustSendBytes;
		}

		if(ERR_CAN_R_NO_ERR == (ERR_CODE_CAN_R)Wait_RectRequst_4Next(byRectAddress, bDCDC, 60, &iAddr, &iRLen, TRUE))
		{
			if(gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag == CAN_REQUIRE_ADDR_01_FLAG
				&& gs_UpgradePowerModuleCtrl.u32_PM_StartAddress == 0x00000000)
			{
				gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
				gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
				ReadFileContex(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress, bDCDC, gs_UpgradePowerModuleCtrl.u16_PM_ReadLen);
				gs_UpgradePowerModuleCtrl.u32Last_PM_StartAddress = 0;
				gs_UpgradePowerModuleCtrl.u16JustSendBytes = 0;
				if(bDCDC)
				{
					gs_UpgradePowerModuleCtrl.u16RemainBytes = gEvChargeInfo.u16_U2_Bytes;
				}
				else
				{
					gs_UpgradePowerModuleCtrl.u16RemainBytes = gEvChargeInfo.u16_U1_Bytes;
				}
				ReadFile_NextData(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress + 6, bDCDC, CAN_UPGRADE_MAX_BLOCK_NUM);
				gs_UpgradePowerModuleCtrl.u16NextBytesNum = CAN_UPGRADE_MAX_BLOCK_NUM;
				gs_UpgradePowerModuleCtrl.u16CRC = u16Calc1CRC(u8UpgradeBuffer_NextData, gs_UpgradePowerModuleCtrl.u16NextBytesNum);
			}
			else if((gs_UpgradePowerModuleCtrl.u32_PM_StartAddress != gs_UpgradePowerModuleCtrl.u32Last_PM_StartAddress) && (gs_UpgradePowerModuleCtrl.u32_PM_StartAddress > gs_UpgradePowerModuleCtrl.u32Last_PM_StartAddress))
			{
				gs_UpgradePowerModuleCtrl.u32Last_PM_StartAddress = gs_UpgradePowerModuleCtrl.u32_PM_StartAddress;

				ReadFileContex(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress, bDCDC, gs_UpgradePowerModuleCtrl.u16_PM_ReadLen);

				iErrorTimes = 0;

				iLen = gs_UpgradePowerModuleCtrl.u16RemainBytes - gs_UpgradePowerModuleCtrl.u16JustSendBytes;

				if(iLen > 0)
				{
					gs_UpgradePowerModuleCtrl.u16RemainBytes = iLen;
					DearNexData(bDCDC);
				}
				else
				{
					// EVENT_RECORD(eHIS_EVENT_TYPE_6, 7, "d", byRectAddress);
					// co_trace(3u, "<PM OTA> No.0x%02X Rect update end, for reason 4.", byRectAddress);
					gs_UpgradePowerModuleCtrl.u16RemainBytes = iLen;
					u8FinishedFlag = TRUE;
					break;
				}
			}//end if
			else
			{
				if((gs_UpgradePowerModuleCtrl.u32_PM_StartAddress == gs_UpgradePowerModuleCtrl.u32Last_PM_StartAddress))
				{
					ReadFileContex(gs_UpgradePowerModuleCtrl.u32_PM_StartAddress, bDCDC, gs_UpgradePowerModuleCtrl.u16_PM_ReadLen);
					iLen = gs_UpgradePowerModuleCtrl.u16RemainBytes;
					if(iLen > 0)
					{
						DearNexData(bDCDC);
					}

					iErrorTimes++;
					if(iErrorTimes > 5)
					{
						// EVENT_RECORD(eHIS_EVENT_TYPE_6, 8, "d", byRectAddress);
						// co_trace(3u, "<PM OTA> No.0x%02X Rect update end, for reason 5.", byRectAddress);
						u8FinishedFlag = FALSE;
						gs_UpgradePowerModuleCtrl.u8UpgradeErrCode = 4;
						break;
					}
				}
			}//end else
		}//end if
//by lxr01202   大文件传输事件得增加
		if(gs_UpgradePowerModuleCtrl.u16TimerCount > 300)	//edit src-100 U2 update fail sometimes.
		{
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 9, "d", byRectAddress);
			// co_trace(3u, "<PM OTA> No.0x%02X Rect update end, for reason 6.", byRectAddress);
			u8FinishedFlag = FALSE;
			gs_UpgradePowerModuleCtrl.u8UpgradeErrCode = 3;
			break;
		}

		//current module ugrade progress
		if(bDCDC)
		{
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = (float)(gEvChargeInfo.u16_U2_Bytes-gs_UpgradePowerModuleCtrl.u16RemainBytes) / (float)gEvChargeInfo.u16_U2_Bytes * 100;
		}
		else
		{
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = (float)(gEvChargeInfo.u16_U1_Bytes-gs_UpgradePowerModuleCtrl.u16RemainBytes) / (float)gEvChargeInfo.u16_U1_Bytes * 100;
		}
	}//end for

		//current module ugrade progress  //20250103 edit
		if(bDCDC)
		{
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = (float)(gEvChargeInfo.u16_U2_Bytes-gs_UpgradePowerModuleCtrl.u16RemainBytes) / (float)gEvChargeInfo.u16_U2_Bytes * 100;
		}
		else
		{
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = (float)(gEvChargeInfo.u16_U1_Bytes-gs_UpgradePowerModuleCtrl.u16RemainBytes) / (float)gEvChargeInfo.u16_U1_Bytes * 100;
		}

	if(u8FinishedFlag)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


static UINT8 Handle_RectUpdt_4One(BYTE byRectAddress, BOOL bDCDC, BOOL isForceUpgrade)
{
	UINT8 i, u8Rtn = TRUE;
	gs_UpgradePowerModuleCtrl.u16_PM_ReadLen = 0;
	gs_UpgradePowerModuleCtrl.u32_PM_StartAddress = 0;
	gs_UpgradePowerModuleCtrl.u32_PM_StartAddress = 0;
	gs_UpgradePowerModuleCtrl.u8_PM_Is_RequreDataFlag = 0;
	gs_UpgradePowerModuleCtrl.u32Last_PM_StartAddress = 0;
	gs_UpgradePowerModuleCtrl.u16JustSendBytes = 0;
	if(bDCDC)
	{
		gs_UpgradePowerModuleCtrl.u16RemainBytes = gEvChargeInfo.u16_U2_Bytes;
	}
	else
	{
		gs_UpgradePowerModuleCtrl.u16RemainBytes = gEvChargeInfo.u16_U1_Bytes;
	}

	//初始化CAN3	editor cjy
	CAN3_ReInit();

	#if ENABLE_OTA_RECT_SILLENCE
	//edit for shield
	for(i = 0; i < 3; ++i)
		SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA1);           //PCS 升级 ABU得静默
	#endif

	//Upgrade state 
	gs_UpgradePowerModuleCtrl.u8CurrModuleAddr = byRectAddress;
	gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 1;
	gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
	gs_UpgradePowerModuleCtrl.u8UpgradeErrCode = 0;
	
	//first try normal, if failed, try forced!
	if(!Triggle_Rect_4Updt(byRectAddress, bDCDC, FALSE, isForceUpgrade))
	{
		if(!Triggle_Rect_4Updt(byRectAddress, bDCDC, TRUE, isForceUpgrade))
		{
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 2, "d", byRectAddress);
			// co_trace(3u, "<CAN3> Triggle module: 0x%02X, Failed", byRectAddress);
		#if ENABLE_OTA_RECT_SILLENCE
		//for(UINT8 i = 0; i < 3; ++i)
			//SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
		#endif
			gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 3;
			gs_UpgradePowerModuleCtrl.u8UpgradeErrCode = 2;
			return FALSE;
		}
	}

	// EVENT_RECORD(eHIS_EVENT_TYPE_6, 3, "d", byRectAddress);
	// co_trace(3u, "<CAN3> Triggle module: 0x%02X, Succeed", byRectAddress);

	if(!Send_BinFile_2Rect(byRectAddress, bDCDC))
	{
		if (bDCDC)
		{
			// co_trace(3u, "<CAN3> PowerModule: 0x%02X, upgrade U2 failed", byRectAddress);
		}
		else
		{
			// co_trace(3u, "<CAN3> PowerModule: 0x%02X, upgrade U1 failed", byRectAddress);
		}

		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState  = 3;
		u8Rtn = FALSE;
	}
	else
	{
		if (bDCDC)
		{
			// co_trace(3u, "<CAN3> upgrade powermodule: 0x%02X U2, Succeed", byRectAddress);
		}
		else
		{
			// co_trace(3u, "<CAN3> upgrade powermodule: 0x%02X U1, Succeed", byRectAddress);
		}

		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 2;
	}

#if ENABLE_OTA_RECT_SILLENCE
		//edit for shield
		//for(UINT8 i = 0; i < 3; ++i)
			//SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif

	return u8Rtn;
}

void TrigAutoAllowcation(void)
{
	canmsg SendMsg;
	UINT8 u8CMD = 0x1F;
	SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + ((UINT32)u8CMD << 16) + ((UINT32)R_PROTO_BROADCAST << 8) + CAN_CONTROLLER_SELF_ADDR;

	SendMsg.Data[0] = 0;
	SendMsg.Data[1] = 0;
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(2);
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(2);
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(5);
}
void AfterOneUpgrade(void)
{
	UINT8 u8Idx = 0;
	//TrigAutoAllowcation();
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	for(u8Idx = 0; u8Idx < 3; u8Idx++)
	{
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		OSTimeDly(250);
		 
		CtrlRectOff_BroadCast(); 
		OSTimeDly(250);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		
		CtrlRectOff_BroadCast(); 
	}

	for(u8Idx = 0; u8Idx < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num; u8Idx++)
	{
		GetSingle_VoltCurr_Dev_0A(u8Idx, &gs_CANComm, &gs_CANComm);
		OSTimeDly(20);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
	}
}

void Dear_FailsRECT_Upgrade( BOOL bDCDC)
{
	gs_UpgradePowerModuleCtrl.u8TotalFailsNum = 0;
	UINT8 u8InvalidAddr = 0;
	UINT32 iAddr = 0;
	UINT16 iRLen = 0;
	memset(gs_UpgradePowerModuleCtrl.u8FailsAddr, 0, mRECTNUM);
	UINT16 u16Idx = 0;

	UINT8 u8RectU1U2No = (UINT8)bDCDC;  //dcdc-u2   pdc-u1
	for(u16Idx = 0; u16Idx < 50; u16Idx++)				//220*100 21秒 //100
	{
		//OSTimeDly(105);
		OSTimeDly(15);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		if(ERR_CAN_R_NO_ERR == (ERR_CODE_CAN_R)Wait_RectRequst_4Next(u8InvalidAddr, bDCDC, 3, &iAddr, &iRLen, FALSE));
		{
			//
		}
	}

	UINT8 u8IsOK = FALSE;
	UNUSED(u8IsOK);
	if(gs_UpgradePowerModuleCtrl.u8TotalFailsNum > 0)
	{
		u8RcvMsg(1);u8RcvMsg(1);u8RcvMsg(1);u8RcvMsg(1);u8RcvMsg(1);u8RcvMsg(1);
		for(u16Idx = 0; u16Idx < gs_UpgradePowerModuleCtrl.u8TotalFailsNum; u16Idx++)
		{
			if(Handle_RectUpdt_4One(gs_UpgradePowerModuleCtrl.u8FailsAddr[u16Idx],bDCDC, TRUE))
			{
				AfterOneUpgrade();
				if(gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count > 0)
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count--;

				gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8RectU1U2No]++;
				UpDataBeep(BEEP_ON_OFF, 1, 200, 100);
			}
			else
			{
				AfterOneUpgrade();
				u8IsOK = TRUE;
			}
		}
	}
}

void DleayUpgrade(void);
void DleayUpgrade(void)
{
	UINT8 u8III = 0;
	for(u8III = 0; u8III < 12; u8III++)
	{
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
	//	OSTimeDly(505);
		//POWER_MODULE_PWRON_FOR_UPGRADE();
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		CtrlRectOff_BroadCast(); 
		OSTimeDly(5);
		CtrlRectOff_BroadCast(); 
		OSTimeDly(5);
	}
}

//editor cjy
void CAN3_ReInit()
{
	gs_CANComm.u16AllCommFailResetDelay = 0;
	gs_CANComm.u16RectCommPeriodResetDelay = 0;

	BSP_InitSPI3();
	OSTimeDly(500);
	InitCan3Port();
	OSTimeDly(500);
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
}

void SendRectSillenceModeBroadcast(BYTE byRectAddress, UINT8 u8Data)
{
	canmsg SendMsg;
	//027F00F0  01  DC触发     02 PFC触发		03 PFC 强制触发		04 DC强制下载	
	//0x1F0FxxF0
//	SendMsg.Id = (R_PRPTO_DEVNO_0A << 22) + (CAN_RECT_OTA_SILLENCE_MODE_CMD<<16) + ((UINT32)R_PROTO_BROADCAST << 8) + R_PROTO_SELF_ADDR;
	SendMsg.Id = 0x02A4FAF0;
	//data 0~7: 0x11 0x25 _ _ _ _ _ 0xA0
	SendMsg.Data[0] = (0x11 << 24) + (0x1E << 16);	//
	SendMsg.Data[1] = u8Data;		//before OTA:0xA0	//after OTA:0xA1
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	//ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);
	OSTimeDly(3);
}

//文件名字符串中获取版本号 file3_104 -> return 104
UINT16 RectVer_String2Num(char *str)
{
	UINT8 i;
	UINT16 u16Temp = 104;
	for(i = 0; (str[i] != 0) && ((str[i+2] != 0)); ++i)
	{
		if( ((str[i] >= '0') && (str[i] <= '9'))
		 && ((str[i+1] >= '0') && (str[i+1] <= '9'))
		 && ((str[i+2] >= '0') && (str[i+2] <= '9')) )
		{
			//Ver: 10.01 -> 1001
			if((str[i+3] >= '0') && (str[i+3] <= '9'))
			{
				u16Temp = (UINT16)((str[i] - '0') * 1000 + (str[i+1] - '0') * 100 + (str[i+2] - '0') * 10 + (str[i+3] - '0'));
			}
			else	//Ver: 1.03 -> 103
			{
				u16Temp = (UINT16)((str[i] - '0') * 100 + (str[i+1] - '0') * 10 + (str[i+2] - '0'));
			}
			break;
		}
	}

	return u16Temp;
}
//去掉重复的模块地址，防止反复升同一模块
static void CheckRepeatAddr(UINT8 *ArrayName, UINT8 *pNum)
{
	UINT8 i, j, u8Idx, u8Rtn;
	UINT8 au8Out[MAX_UPGRADE_RECT_NUM];

	if(*pNum < 2)
	{
		return;
	}
	
	memset(au8Out, 0x40, sizeof(au8Out));
	for(i = 0, u8Idx = 0; i < *pNum; ++i)
	{
		u8Rtn = TRUE;
		for(j = 0; j < u8Idx; ++j)
		{
			if(au8Out[j] == ArrayName[i])
			{
				u8Rtn = FALSE;
			}
		}
		if(u8Rtn)
		{
			au8Out[u8Idx++] = ArrayName[i];
		}
	}

	*pNum = u8Idx;
	memcpy(ArrayName, au8Out, u8Idx);
}

//补救单个模块
static void UpgradeSingleRect(UINT8 u8RecAddr, UINT8 bDCDC)
{
	if(Handle_RectUpdt_4One(u8RecAddr,	(BOOL)bDCDC, FALSE))
	{
		AfterOneUpgrade();
		gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[(eZERO_OR_ONE)bDCDC]++;
		gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count--;
		UpDataBeep(BEEP_ON_OFF, 1, 200, 100); 

	}
	else
	{
		//AfterOneUpgrade();
		SendAddressingCmd();
		SendAddressingCmd();

		DleayUpgrade();
		//gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
		//u8IsOK = TRUE;
	}

	DleayUpgrade();
	u8RcvMsg(30);
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
}
//救回升级异常的模块
static void UpgradeAppointedRects(UINT8* ArrayName, UINT8 u8Num, UINT8 bDCDC)
{
	UINT8 u8Idx, u8RecAddr;

	CheckRepeatAddr(ArrayName ,&u8Num);
	for(u8Idx = 0; u8Idx < u8Num; ++u8Idx)
	{
		u8RecAddr = ArrayName[u8Idx];
		//co_trace(3u, "<CAN3> Upgrade Rect No.%d.", u8RecAddr);
		UpgradeSingleRect(u8RecAddr, bDCDC);
	}
}
//等待重排完成
static void WaitAddressingFinish()
{		
	// co_trace(3u, "<CAN3> Waiting Addressing.");		
#if ENABLE_OTA_RECT_SILLENCE
	for(UINT8 i = 0; i < 3; ++i)
	SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA1);
#endif
	SendAddressingCmd();
	SendAddressingCmd();

	DleayUpgrade();
}
//查版本，记录升级异常的模块地址
static void CheckOnlineRectVer(UINT8 bDCDC)
{
	UINT8 u8Idx, u8Num;
	UINT16 u16Temp;
	ERR_CODE_CAN_R err;

	gpRectOTA->u8RectNum_NotUpg = 0;
	gpRectOTA->u8RectNum_Ver0 = 0;
	gpRectOTA->u8RectNum_NotAckVer = 0;
	memset(gpRectOTA->u8RectNotUpg, 0, sizeof(gpRectOTA->u8RectNotUpg));
	memset(gpRectOTA->u8RectVer0, 0, sizeof(gpRectOTA->u8RectVer0));
	
	//防止漏掉模块
	u8Num = MAX(mRECTNUM, gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num);
	for(u8Idx = 0; u8Idx < u8Num; u8Idx++)
	{
		//查版本，有回复的才记录
		err = GetSingle_VerInfo_SingleModule(u8Idx, &gs_CANComm, &gs_CANComm);
		//co_trace(3u, "<CAN3> Rect No.%d, GetVer ErrorCode is %d.", u8Idx, err);
		if(ERR_CAN_R_NO_ERR == err)
		{
			u16Temp = (bDCDC)? Math_round_100(gs_CANComm.RectData[u8Idx].fDC_Ver) : 
								Math_round_100(gs_CANComm.RectData[u8Idx].fAC_Ver);
			
			//版本变0，但仍参与重排的
			if(u16Temp < VOID_RECT_VER)
			{
				gpRectOTA->u8RectVer0[gpRectOTA->u8RectNum_Ver0++] = u8Idx;
				if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[(eZERO_OR_ONE)bDCDC] <= gpRectOTA->u8RectNum_Ver0)
				{
					break;
				}
			}
			//漏升的
			else if(u16Temp != gpRectOTA->u16RectUpgVer[(eZERO_OR_ONE)bDCDC])
			{
				gpRectOTA->u8RectNotUpg[gpRectOTA->u8RectNum_NotUpg++] = u8Idx;		
				// gpRectOTA->u8RectNum_NotUpg = 0;		
			}
			// co_trace(3u, "<CAN3> Rect No.%d Ver is %d.", u8Idx, u16Temp);
		}
		else
		{
			//由于有模块变异常地址，重排后总数会少
			gpRectOTA->u8RectNum_NotAckVer++;
			// co_trace(3u, "<CAN3> Rect No.%d don't Ack Ver.", u8Idx);
		}

		OSTimeDly(20);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
	}
}

//查版本，记录升级异常的模块地址
static void CheckOnlineABUVer(UINT8 bDCDC)
{
	UINT8 u8Idx, u8Num;
	UINT16 u16Temp;
	ERR_CODE_CAN_R err;

	gpRectOTA->u8RectNum_NotUpg = 0;
	gpRectOTA->u8RectNum_Ver0 = 0;
	gpRectOTA->u8RectNum_NotAckVer = 0;
	memset(gpRectOTA->u8RectNotUpg, 0, sizeof(gpRectOTA->u8RectNotUpg));
	memset(gpRectOTA->u8RectVer0, 0, sizeof(gpRectOTA->u8RectVer0));
	
	//防止漏掉模块
	u8Num = 1;
	for(u8Idx = 0; u8Idx < u8Num; u8Idx++)
	{
		//查版本，有回复的才记录
		err = GetABU_VerInfo();
		if(ERR_CAN_R_NO_ERR == err)
		{
			u16Temp = (bDCDC)? (ABU->DC_Version) : 
								(ABU->PFC_Version);
			
			//版本变0，但仍参与重排的
			if(u16Temp < VOID_RECT_VER)
			{
				gpRectOTA->u8RectVer0[gpRectOTA->u8RectNum_Ver0++] = ABU_Addr;
				if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[(eZERO_OR_ONE)bDCDC] <= gpRectOTA->u8RectNum_Ver0)
				{
					break;
				}
			}
			//漏升的
			else if(u16Temp != gpRectOTA->u16RectUpgVer[(eZERO_OR_ONE)bDCDC])
			{
				gpRectOTA->u8RectNotUpg[gpRectOTA->u8RectNum_NotUpg++] = ABU_Addr;
			}
		}
		else
		{
			//由于有模块变异常地址，重排后总数会少
			gpRectOTA->u8RectNum_NotAckVer++;
		}

		OSTimeDly(20);
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
	}
}

//by lxr01202  新增MPPT_U1升级函数
UINT8 u8_MPPT_U1_UpDataProc(void);
UINT8 u8_MPPT_U1_UpDataProc(void)
{
	INT8U ProcessedModuleNum = 0u;
	UINT8 u8IIII = 0;UNUSED(u8IIII);
	UINT8 u8RecAddr = 0, i, u8Idx;
	UNUSED(u8RecAddr);
	UINT8 u8FlagDCDC;	// 0-PFC	1-DC
	static UINT16 u16UpData_U1Delay = 0;	//PCS_U1升级清零延时计数
	static UINT8 u8FlagClear_U1 =0;	//PCS_U1升级清零标志位
	UINT8 u8IsOK = FALSE;UNUSED(u8IsOK);
	//editor cjy
	u8FlagDCDC = (UINT8)IS_UPGRADE_PFC;
	gpRectOTA->u16RectUpgVer[0] = RectVer_String2Num(gs_UpGradeCtrl.cRcvFileName);//(gEvChargeInfo.OTAFileName[2]);

	if(gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] == TRUE)//处理过了，不需要再处理！
	{
		return FALSE;
	}
#if 0
	if(gEvChargeInfo.u8Power_on_Delay < 35)				//延时60S进行处理！ 保证能FLASH 已经OK
	{
		OSTimeDly(2);									//250*4ms
		return FALSE;
	}
#endif

	gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count = 0;
	gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] = 0;

	UINT8 u8DebugFlag = 0;UNUSED(u8DebugFlag);

	

//by lxr01202  添加MPPT升级信息
	if (((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == MPPT_IS_UPDATE_FLAG)
		|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == MPPT_IS_FORCE_UPDATE_FLAG)))
	{
		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 1;
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
		gEvChargeInfo.u8RectUpgradeFlag = TRUE;

		WaitAddressingFinish();
		DleayUpgrade();
		DleayUpgrade();

		//UpDataBeep(BEEP_ON_OFF, 2, 75, 75);
		//UpDataBeep(BEEP_ON_LONG,2, 5, 250);

		if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == MPPT_IS_UPDATE_FLAG) || (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == MPPT_IS_FORCE_UPDATE_FLAG))&& (GetOnline_RectNum()))
		{
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 1, "d", gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num);
			// co_trace(3u, "<CAN3> scan powermoudle num is: %d", gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num);
			//gs_UpgradePowerModuleCtrl.u8UpDatedTotalRCTNum = 0;
#if 0
			for(u8RecAddr = 0; (u8RecAddr < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num) && (u8RecAddr < mRECTNUM); u8RecAddr++)
			{
				if(Handle_RectUpdt_4One(u8RecAddr,	IS_UPGRADE_PFC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);  
				}
				else
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
				}
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
			}
#else
//楼下备电柜地址从80开始
			u8RecAddr = 0x80;

#if 1
			//by lxr01202 升级模块U1 改为4个，但目前只适用于A2419，支持模块依次升级 
			if(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE)
			{
				gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = 4;
			}



			for(u8Idx = 0; u8Idx < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num; u8Idx++)
			{				
				GetSingle_VerInfo_SingleModule(u8Idx, &gs_CANComm, &gs_CANComm);
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 22, "dd", u8Idx, Math_round_100(gs_CANComm.RectData[u8Idx].fAC_Ver));
				// co_trace(3u, "<CAN3> Rect No.%d U1 Ver is %d. Clear to 100!", u8Idx, Math_round_100(gs_CANComm.RectData[u8Idx].fDC_Ver));
				gs_CANComm.RectData[u8Idx].fAC_Ver = VOID_RECT_VER * 0.0100;
			}
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 21, "d", gpRectOTA->u16RectUpgVer[u8FlagDCDC]);
			// co_trace(3u, "<CAN3> Upgrade Pack Ver is %d.", gpRectOTA->u16RectUpgVer[u8FlagDCDC]);
#endif

			while (1)
			{
				if (ProcessedModuleNum >= gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num)
				{
					break;
				}

				if(Handle_RectUpdt_4One(u8RecAddr, (BOOL)u8FlagDCDC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100); 
					u8RecAddr++;
				}
				else
				{
					AfterOneUpgrade();
					//SendAddressingCmd();
					//SendAddressingCmd();
		
					//DleayUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
					u8RecAddr++;	//editor cjy
				}

				ProcessedModuleNum++;
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
				
				// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)	//如果升级成功，直接上报
				// 	{
				// 		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
				// 		gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
				// 	}				
			}
			gSelfTest.fSetTaskID(TASK_ID_CAN3);

			//editor cjy
			/***** 查版本不对的，升第二、三遍 ****/
			//等待重排结束
			WaitAddressingFinish();
			//查版本
			// co_trace(3u, "<CAN3> Check Ver after first upgrade!");
			CheckOnlineRectVer(u8FlagDCDC);
	
			//升第二遍，将第一遍中由于地址重排漏掉的升级一遍
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 11, "d", gpRectOTA->u8RectNum_NotUpg);
			// co_trace(3u, "<CAN3> Second Upgrade start! Sum is %d", gpRectOTA->u8RectNum_NotUpg);
			if(gpRectOTA->u8RectNum_NotUpg)
			UpgradeAppointedRects(gpRectOTA->u8RectNotUpg, gpRectOTA->u8RectNum_NotUpg, u8FlagDCDC);

			//升第三遍
			if(gpRectOTA->u8RectNum_Ver0)
			{
			gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 12, "d", gpRectOTA->u8RectNum_Ver0);
			// co_trace(3u, "<CAN3> Third Upgrade! Rect Ver_0 Sum is %d", gpRectOTA->u8RectNum_Ver0);
			UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);				
			}


			//重新将变异常地址的升回来	U1升挂的概率更高，多等几次
			if((gpRectOTA->u8RectNum_NotAckVer) || (gpRectOTA->u8RectNum_Ver0))		//前面没有异常时，直接过
			{
				for(i = 0; i < 3; ++i)
				{
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
					if(gs_UpgradePowerModuleCtrl.u8TotalFailsNum)
						WaitAddressingFinish();
				}
			}


#if 0		//旧模块没有静默模式，会有0版本模块触发一次不能救回来，故检查多几次，直到查不到0版本模块
			//重复三次，排除版本0的模块
			for(i = 0; i < 3; ++i)		
			{
				//等待重排的时间逐渐延长
				for(UINT8 ii = 0; ii <= i; ++ii)
					WaitAddressingFinish();

				CheckOnlineRectVer(u8FlagDCDC);

				//全部有版本回复，且无版本0模块，跳出
				if((!gpRectOTA->u8RectNum_Ver0) && (!gpRectOTA->u8RectNum_NotAckVer))
					break;

				//升级，使其升成功，或变异常地址
				gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 15, "dd", gpRectOTA->u8RectNum_Ver0, (i+1));
				// co_trace(3u, "<CAN3> Rect Ver 0 Sum is %d. Retry count: %d", gpRectOTA->u8RectNum_Ver0, (i+1));
				UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);

				if(gpRectOTA->u8RectNum_NotAckVer)
				{
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_NotAckVer;
					WaitAddressingFinish();
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
				}
				
			}//end for


#endif


#endif

			
#if ENABLE_OTA_RECT_SILLENCE
			for(i = 0; i < 3; ++i)
				SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif
			//end of upgrade


			gSelfTest.fSetTaskID(TASK_ID_CAN3);
		} // if(GetOnline_RectNum())

		

		//while
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
		u8FlagClear_U1 = 1;
		/*if((gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count <= 0))
		{
			UpDataBeep(BEEP_ON_OFF, 10, 200, 100);      
		}*/
		if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)//在线所有模块升级成功，则上报升级成功
		{
			// co_trace(3u, "<CAN3> U1 upgrade succeed Num is: %d", gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0]);
			//UpDataBeep(BEEP_ON_LONG,3, 5, 250);
			UpgradePowerModuleBeep(5u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
		}
		else
		{
			// co_trace(3u, "<CAN3> total U1 upgrade failed");
			//UpDataBeep(BEEP_ON_LONG,20, 250, 250);
			UpgradePowerModuleBeep(2u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 3;
			// LX_OTA_fail();
		}

		gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] = TRUE;
		gEvChargeInfo.u8Power_on_Delay = 0;

		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] == 1)
		{
			gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] = 2;
		}
		return TRUE;
	}
	else
	{
		//判断出 无模块需要升级，则以后也不处理了！
		//su8Deal_Flag = TRUE;										//处理过了！
		gEvChargeInfo.u8RectUpgradeFlag = FALSE;
		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;

		if(((gs_UpgradePowerModuleCtrl.u8TotalUpgradeState == 2)		//升级完成，且升级成功,延时清除
			|| (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState != 0))&&		                                                  	  //升级失败，延时清除													
			(u8FlagClear_U1 == 1))								//PCS_U1升级时才清除
			{
				u16UpData_U1Delay++;
				if(u16UpData_U1Delay >= 7000)
				{
					u8FlagClear_U1 = 0;
					u16UpData_U1Delay = 0;
					gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
					gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
				}
			}

		return FALSE;
	}
}


//by lxr01202  新增MPPTU2升级函数
UINT8 u8_MPPT_U2_UpDataProc(void);
UINT8 u8_MPPT_U2_UpDataProc(void)
{

	INT8U ProcessedModuleNum = 0u;
	UINT8 u8ii = 0, i, u8Idx;
	UNUSED(u8ii);
	UINT8 u8U1IsFail = FALSE;UNUSED(u8U1IsFail);
	UINT8 u8RecAddr = 0;
	UINT8 u8FlagDCDC;	// 0-PFC	1-DC
	static UINT16 u16UpData_U2Delay = 0;		//MPPT_U2升级清零延时计数
	static	UINT8 u8FlagClear_U2 =0;	
	//edit cjy
	u8FlagDCDC = (UINT8)IS_UPGRADE_DC;
	gpRectOTA->u16RectUpgVer[1] = RectVer_String2Num(gs_UpGradeCtrl.cRcvFileName);//(gEvChargeInfo.OTAFileName[3]);

	if(gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] == TRUE)//处理过了，不需要再处理！
	{
		return FALSE;
	}
#if 0
	if(gEvChargeInfo.u8Power_on_Delay < 35)				//延时60S进行处理！ 保证能FLASH 已经OK
	{
		OSTimeDly(2);									//250*4ms
		return FALSE;
	}
#endif
	gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count = 0;
	gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] = 0;



	UINT8 u8IIII = 0;UNUSED(u8IIII);
	UINT8 u8DebugFlag = 0;UNUSED(u8DebugFlag);
	UINT8 u8IsOK = FALSE;UNUSED(u8IsOK);

	 if (((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == MPPT_IS_UPDATE_FLAG) || 
			(gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == MPPT_IS_FORCE_UPDATE_FLAG)))
	{
		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 1;
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
		gEvChargeInfo.u8RectUpgradeFlag = TRUE;

		WaitAddressingFinish();
		DleayUpgrade();
		DleayUpgrade();
		
		if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == MPPT_IS_UPDATE_FLAG)|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == MPPT_IS_FORCE_UPDATE_FLAG))&& (GetOnline_RectNum()))
		{
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 1, "d", gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num);
			//UpDataBeep(BEEP_ON_OFF, 2, 75, 75);
			//UpDataBeep(BEEP_ON_LONG,2, 5, 250);
			//gs_UpgradePowerModuleCtrl.u8UpDatedTotalRCTNum = 0;
#if 0
			for(u8RecAddr = 0; (u8RecAddr < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num) && (u8RecAddr < mRECTNUM); u8RecAddr++)
			{
				if(Handle_RectUpdt_4One(u8RecAddr, IS_UPGRADE_DC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);
				}
				else
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
				}
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
			}
#else

			u8RecAddr = 0x80;

#if 1
			//by lxr01202 升级模块U1 改为4个，但目前只适用于A2419，支持模块依次升级 
			if(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE)
			{
				gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = 4;
			}

			for(u8Idx = 0; u8Idx < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num; u8Idx++)
			{
				GetSingle_VerInfo_SingleModule(u8Idx, &gs_CANComm, &gs_CANComm);
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 22, "dd", u8Idx, Math_round_100(gs_CANComm.RectData[u8Idx].fDC_Version));
				// co_trace(3u, "<CAN3> Rect No.%d U2 Ver is %d. Clear to 100!", u8Idx, Math_round_100(gs_CANComm.RectData[u8Idx].fDC_Ver));
				gs_CANComm.RectData[u8Idx].fDC_Ver = VOID_RECT_VER * 0.0100;
			}
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 21, "d", gpRectOTA->u16RectUpgVer[u8FlagDCDC]);
			// co_trace(3u, "<CAN3> Upgrade Pack Ver is %d.", gpRectOTA->u16RectUpgVer[u8FlagDCDC]);
#endif

			while (1)
			{
				if (ProcessedModuleNum >= gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num)
				{
					break;
				}
				
				if(Handle_RectUpdt_4One(u8RecAddr, (BOOL)u8FlagDCDC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);
					u8RecAddr++;
				}
				else
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
					u8RecAddr++;	//editor cjy
				}
				ProcessedModuleNum++;
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
				
				// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] > 0)	//如果升级成功，直接上报
				// 	{
				// 		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
				// 		gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
				// 	}			
			}
#endif

			gSelfTest.fSetTaskID(TASK_ID_CAN3);

			//editor cjy
			/***** 查版本不对的，升第二遍 ****/
			//等待重排结束
			WaitAddressingFinish();
			//查版本
			// co_trace(3u, "<CAN3> Check Ver after first upgrade!");
			CheckOnlineRectVer(u8FlagDCDC);

			//升第二遍，将第一遍中由于地址重排漏掉的正常模块升级
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 11, "d", gpRectOTA->u8RectNum_NotUpg);
			// co_trace(3u, "<CAN3> Second Upgrade start! Sum is %d", gpRectOTA->u8RectNum_NotUpg);
			if(gpRectOTA->u8RectNum_NotUpg)
			UpgradeAppointedRects(gpRectOTA->u8RectNotUpg, gpRectOTA->u8RectNum_NotUpg, u8FlagDCDC);

			//触发升级，使变地址
			if(gpRectOTA->u8RectNum_Ver0)
			{
			gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 12, "d", gpRectOTA->u8RectNum_Ver0);
			// co_trace(3u, "<CAN3> Third Upgrade! Rect Ver 0 Sum is %d", gpRectOTA->u8RectNum_Ver0);
			UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);
			}

			//收集所有异常地址，升回来
			//for(i = 0; i < 3; ++i)	//editor cjy	U2不用等太长，检查一次就能把能救的救回来，就不回来了的都是因为U1挂了
			if((gpRectOTA->u8RectNum_NotAckVer) || (gpRectOTA->u8RectNum_Ver0))		//前面没有异常时，直接过
			{
				for(i = 0; i < 3; ++i)
				{
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
					if(gs_UpgradePowerModuleCtrl.u8TotalFailsNum)
						WaitAddressingFinish();
				}
			}

			// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] > 0)	//如果升级成功，直接上报
			// {
			// 	gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			// 	gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
			// }	

#if 0		//旧模块没有静默模式，会有0版本模块触发一次不能救回来，故检查多几次，直到查不到0版本模块
			for(i = 0; i < 3; ++i)		
			{
				//等待重排的时间逐渐延长
				for(UINT8 ii = 0; ii <= i; ++ii)
					WaitAddressingFinish();

				CheckOnlineRectVer(u8FlagDCDC);

				//全部有版本回复，且无版本0模块，跳出
				if((!gpRectOTA->u8RectNum_Ver0) && (!gpRectOTA->u8RectNum_NotAckVer))
					break;

				//升级，使其升成功，或变异常地址
				gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 15, "dd", gpRectOTA->u8RectNum_Ver0, (i+1));
				// co_trace(3u, "<CAN3> Rect Ver 0 Sum is %d. Retry count: %d", gpRectOTA->u8RectNum_Ver0, (i+1));
				UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);
			
				if(gpRectOTA->u8RectNum_NotAckVer)
				{
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_NotAckVer;
					WaitAddressingFinish();
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
				}
			}
#endif



#if ENABLE_OTA_RECT_SILLENCE
			for(i = 0; i < 3; ++i)
				SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif
			//end of upgrade
                              
			gSelfTest.fSetTaskID(TASK_ID_CAN3);
		}

		
		//while
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
		u8FlagClear_U2 = 1;
		/*if(gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count <= 0)
		{
			UpDataBeep(BEEP_ON_OFF, 10, 200, 100);
		}*/
		if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] > 0)//在线所有模块升级成功，则升级完成
		{
			// co_trace(3u, "<CAN3> U2 upgrade succeed, Num is: %d", gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1]);
			//UpDataBeep(BEEP_ON_OFF, gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count, 200, 100);
			//UpDataBeep(BEEP_ON_LONG,3, 5, 250);
			UpgradePowerModuleBeep(5u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
		}
		else
		{
			// co_trace(3u, "<CAN3> U2 upgrade failed");
			//UpDataBeep(BEEP_ON_LONG,20, 250,250);
			UpgradePowerModuleBeep(2u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 3;
			// LX_OTA_fail();
		}

		memset(gs_UpgradePowerModuleCtrl.u8FailsAddr, 0, mRECTNUM);

		gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] = TRUE;
		gEvChargeInfo.u8Power_on_Delay = 0;
		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] == 1)
		{
			gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] = 2;
		}
		return TRUE;
	}


	else
	{
		//判断出 无模块需要升级，则以后也不处理了！
		//su8Deal_Flag = TRUE;										//处理过了！
		gEvChargeInfo.u8RectUpgradeFlag = FALSE;
		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;

		if( (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState == 2)										//升级完成，延时清除升级进度与升级总状态标志位
			|| (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState != 0)										//升级失败，延时清除升级进度与升级总状态标志位
			&&(u8FlagClear_U2 == 1))						//PCS_U2升级才清除
		{
			u16UpData_U2Delay++;
			if(u16UpData_U2Delay >= 7000)
			{
				u8FlagClear_U2 = 0;
				u16UpData_U2Delay = 0;
				gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
				gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
			}
		}

		return FALSE;
	}
}






UINT8 u8_U1_UpDataProc(void);
UINT8 u8_U1_UpDataProc(void)
{
	INT8U ProcessedModuleNum = 0u;
	UINT8 u8IIII = 0;UNUSED(u8IIII);
	UINT8 u8RecAddr = 0, i, u8Idx;
	UNUSED(u8RecAddr);
	UINT8 u8FlagDCDC;	// 0-PFC	1-DC
	static UINT16 u16UpData_U1Delay = 0;	//PCS_U1升级清零延时计数
	static UINT8 u8FlagClear_U1 =0;	//PCS_U1升级清零标志位
	UINT8 u8IsOK = FALSE;UNUSED(u8IsOK);
	//editor cjy
	u8FlagDCDC = (UINT8)IS_UPGRADE_PFC;
	gpRectOTA->u16RectUpgVer[0] = RectVer_String2Num(gs_UpGradeCtrl.cRcvFileName);//(gEvChargeInfo.OTAFileName[2]);

	if(gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] == TRUE)//处理过了，不需要再处理！
	{
		return FALSE;
	}
#if 0
	if(gEvChargeInfo.u8Power_on_Delay < 35)				//延时60S进行处理！ 保证能FLASH 已经OK
	{
		OSTimeDly(2);									//250*4ms
		return FALSE;
	}
#endif

	gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count = 0;
	gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] = 0;

	UINT8 u8DebugFlag = 0;UNUSED(u8DebugFlag);

	if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == RECT_IS_UPDATE_FLAG)
		|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == RECT_IS_FORCE_UPDATE_FLAG)))
	{
		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 1;
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
		gEvChargeInfo.u8RectUpgradeFlag = TRUE;

		WaitAddressingFinish();
		DleayUpgrade();
		DleayUpgrade();

		//UpDataBeep(BEEP_ON_OFF, 2, 75, 75);
		//UpDataBeep(BEEP_ON_LONG,2, 5, 250);

		if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == RECT_IS_UPDATE_FLAG)|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == RECT_IS_FORCE_UPDATE_FLAG))&& (GetOnline_RectNum()))
		{
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 1, "d", gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num);
			// co_trace(3u, "<CAN3> scan powermoudle num is: %d", gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num);
			//gs_UpgradePowerModuleCtrl.u8UpDatedTotalRCTNum = 0;
#if 0
			for(u8RecAddr = 0; (u8RecAddr < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num) && (u8RecAddr < mRECTNUM); u8RecAddr++)
			{
				if(Handle_RectUpdt_4One(u8RecAddr,	IS_UPGRADE_PFC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);  
				}
				else
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
				}
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
			}
#else
			u8RecAddr = 0u;

#if 1
			//by lxr01202 升级模块U1 改为10个，但目前只适用于A2419_IMSU_X，支持模块依次升级 
			if(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE)
			{
				gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = 0;
				//gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = 10;         //屏蔽之前给定的死值，按照读取的组模块数量进行累加。
				for(u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
				{
					gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num += gs_CANComm.GrpData[u8Idx].nCommRectNum;
				}
				
			}

			for(u8Idx = 0; u8Idx < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num; u8Idx++)
			{				
				GetSingle_VerInfo_SingleModule(u8Idx, &gs_CANComm, &gs_CANComm);
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 22, "dd", u8Idx, Math_round_100(gs_CANComm.RectData[u8Idx].fAC_Ver));
				// co_trace(3u, "<CAN3> Rect No.%d U1 Ver is %d. Clear to 100!", u8Idx, Math_round_100(gs_CANComm.RectData[u8Idx].fDC_Ver));
				gs_CANComm.RectData[u8Idx].fAC_Ver = VOID_RECT_VER * 0.0100;
			}
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 21, "d", gpRectOTA->u16RectUpgVer[u8FlagDCDC]);
			// co_trace(3u, "<CAN3> Upgrade Pack Ver is %d.", gpRectOTA->u16RectUpgVer[u8FlagDCDC]);
#endif

			while (1)
			{
				if (ProcessedModuleNum >= gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num)
				{
					break;
				}

				if(Handle_RectUpdt_4One(u8RecAddr, (BOOL)u8FlagDCDC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100); 
					u8RecAddr++;
				}
				else
				{
					AfterOneUpgrade();
					//SendAddressingCmd();
					//SendAddressingCmd();
		
					//DleayUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
					u8RecAddr++;	//editor cjy
				}

				ProcessedModuleNum++;
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
				
				// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)	//如果升级成功，直接上报
				// 	{
				// 		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
				// 		gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
				// 	}				
			}
			gSelfTest.fSetTaskID(TASK_ID_CAN3);

			//editor cjy
			/***** 查版本不对的，升第二、三遍 ****/
			//等待重排结束
			WaitAddressingFinish();
			//查版本
			// co_trace(3u, "<CAN3> Check Ver after first upgrade!");
			CheckOnlineRectVer(u8FlagDCDC);
	
			//升第二遍，将第一遍中由于地址重排漏掉的升级一遍
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 11, "d", gpRectOTA->u8RectNum_NotUpg);
			// co_trace(3u, "<CAN3> Second Upgrade start! Sum is %d", gpRectOTA->u8RectNum_NotUpg);
			if(gpRectOTA->u8RectNum_NotUpg)
			UpgradeAppointedRects(gpRectOTA->u8RectNotUpg, gpRectOTA->u8RectNum_NotUpg, u8FlagDCDC);

			//升第三遍
			if(gpRectOTA->u8RectNum_Ver0)
			{
			gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 12, "d", gpRectOTA->u8RectNum_Ver0);
			// co_trace(3u, "<CAN3> Third Upgrade! Rect Ver_0 Sum is %d", gpRectOTA->u8RectNum_Ver0);
			UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);				
			}


			//重新将变异常地址的升回来	U1升挂的概率更高，多等几次
			if((gpRectOTA->u8RectNum_NotAckVer) || (gpRectOTA->u8RectNum_Ver0))		//前面没有异常时，直接过
			{
				for(i = 0; i < 3; ++i)
				{
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
					if(gs_UpgradePowerModuleCtrl.u8TotalFailsNum)
						WaitAddressingFinish();
				}
			}

			// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)	//如果升级成功，直接上报
			// 	{
			// 		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			// 		gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
			// 	}	

#if 0		//旧模块没有静默模式，会有0版本模块触发一次不能救回来，故检查多几次，直到查不到0版本模块
			//重复三次，排除版本0的模块
			for(i = 0; i < 3; ++i)		
			{
				//等待重排的时间逐渐延长
				for(UINT8 ii = 0; ii <= i; ++ii)
					WaitAddressingFinish();

				CheckOnlineRectVer(u8FlagDCDC);

				//全部有版本回复，且无版本0模块，跳出
				if((!gpRectOTA->u8RectNum_Ver0) && (!gpRectOTA->u8RectNum_NotAckVer))
					break;

				//升级，使其升成功，或变异常地址
				gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 15, "dd", gpRectOTA->u8RectNum_Ver0, (i+1));
				// co_trace(3u, "<CAN3> Rect Ver 0 Sum is %d. Retry count: %d", gpRectOTA->u8RectNum_Ver0, (i+1));
				UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);

				if(gpRectOTA->u8RectNum_NotAckVer)
				{
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_NotAckVer;
					WaitAddressingFinish();
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
				}
				
			}//end for

			

#endif


#endif


			
#if ENABLE_OTA_RECT_SILLENCE
			for(i = 0; i < 3; ++i)
				SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif
			//end of upgrade


			gSelfTest.fSetTaskID(TASK_ID_CAN3);
		} // if(GetOnline_RectNum())

		

		//while
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
		u8FlagClear_U1 = 1;
		/*if((gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count <= 0))
		{
			UpDataBeep(BEEP_ON_OFF, 10, 200, 100);      
		}*/
		if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)//在线所有模块升级成功，则上报升级成功
		{
			// co_trace(3u, "<CAN3> U1 upgrade succeed Num is: %d", gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0]);
			//UpDataBeep(BEEP_ON_LONG,3, 5, 250);
			UpgradePowerModuleBeep(5u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
		}
		else
		{
			// co_trace(3u, "<CAN3> total U1 upgrade failed");
			//UpDataBeep(BEEP_ON_LONG,20, 250, 250);
			UpgradePowerModuleBeep(2u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 3;
			// LX_OTA_fail();
		}

		gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] = TRUE;
		gEvChargeInfo.u8Power_on_Delay = 0;

		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] == 1)
		{
			gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] = 2;
		}
		return TRUE;
	}

	else
	{
		//判断出 无模块需要升级，则以后也不处理了！
		//su8Deal_Flag = TRUE;										//处理过了！
		gEvChargeInfo.u8RectUpgradeFlag = FALSE;
		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;

		if(((gs_UpgradePowerModuleCtrl.u8TotalUpgradeState == 2)		//升级完成，且升级成功,延时清除
			|| (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState != 0))&&		                                                  	  //升级失败，延时清除													
			(u8FlagClear_U1 == 1))								//PCS_U1升级时才清除
			{
				u16UpData_U1Delay++;
				if(u16UpData_U1Delay >= 7000)
				{
					u8FlagClear_U1 = 0;
					u16UpData_U1Delay = 0;
					gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
					gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
				}
			}

		return FALSE;
	}
}



UINT8 u8_U2_UpDataProc(void);
UINT8 u8_U2_UpDataProc(void)
{

	INT8U ProcessedModuleNum = 0u;
	UINT8 u8ii = 0, i, u8Idx;
	UNUSED(u8ii);
	UINT8 u8U1IsFail = FALSE;UNUSED(u8U1IsFail);
	UINT8 u8RecAddr = 0;
	UINT8 u8FlagDCDC;	// 0-PFC	1-DC
	static UINT16 u16UpData_U2Delay = 0;		//PCS_U2升级清零延时计数
	static	UINT8 u8FlagClear_U2 =0;	
	//edit cjy
	u8FlagDCDC = (UINT8)IS_UPGRADE_DC;
	gpRectOTA->u16RectUpgVer[1] = RectVer_String2Num(gs_UpGradeCtrl.cRcvFileName);//(gEvChargeInfo.OTAFileName[3]);

	if(gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] == TRUE)//处理过了，不需要再处理！
	{
		return FALSE;
	}
#if 0
	if(gEvChargeInfo.u8Power_on_Delay < 35)				//延时60S进行处理！ 保证能FLASH 已经OK
	{
		OSTimeDly(2);									//250*4ms
		return FALSE;
	}
#endif
	gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count = 0;
	gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] = 0;



	UINT8 u8IIII = 0;UNUSED(u8IIII);
	UINT8 u8DebugFlag = 0;UNUSED(u8DebugFlag);
	UINT8 u8IsOK = FALSE;UNUSED(u8IsOK);

	if (((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == RECT_IS_UPDATE_FLAG) || 
			(gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == RECT_IS_FORCE_UPDATE_FLAG)))
	{
		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 1;
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
		gEvChargeInfo.u8RectUpgradeFlag = TRUE;

		WaitAddressingFinish();
		DleayUpgrade();
		DleayUpgrade();
		
		if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == RECT_IS_UPDATE_FLAG)|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == RECT_IS_FORCE_UPDATE_FLAG))&& (GetOnline_RectNum()))
		{
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 1, "d", gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num);
			//UpDataBeep(BEEP_ON_OFF, 2, 75, 75);
			//UpDataBeep(BEEP_ON_LONG,2, 5, 250);
			//gs_UpgradePowerModuleCtrl.u8UpDatedTotalRCTNum = 0;
#if 0
			for(u8RecAddr = 0; (u8RecAddr < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num) && (u8RecAddr < mRECTNUM); u8RecAddr++)
			{
				if(Handle_RectUpdt_4One(u8RecAddr, IS_UPGRADE_DC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);
				}
				else
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
				}
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
			}
#else

			u8RecAddr = 0u;

#if 1
			//by lxr01202 升级模块U1 改为10个，但目前只适用于A2419_IMSU_X，支持模块依次升级 
			if(gEvChargeInfo.u8ModuleType == ACDC_IS_35KWPCS_TYPE)
			{
				gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = 0;
				//gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num = 10;
				for(u8Idx = 0; u8Idx < _MAX_GRP_NUM; u8Idx++)
				{
					gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num += gs_CANComm.GrpData[u8Idx].nCommRectNum;
				}
				
			}

			for(u8Idx = 0; u8Idx < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num; u8Idx++)
			{
				GetSingle_VerInfo_SingleModule(u8Idx, &gs_CANComm, &gs_CANComm);
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 22, "dd", u8Idx, Math_round_100(gs_CANComm.RectData[u8Idx].fDC_Version));
				// co_trace(3u, "<CAN3> Rect No.%d U2 Ver is %d. Clear to 100!", u8Idx, Math_round_100(gs_CANComm.RectData[u8Idx].fDC_Ver));
				gs_CANComm.RectData[u8Idx].fDC_Ver = VOID_RECT_VER * 0.0100;
			}
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 21, "d", gpRectOTA->u16RectUpgVer[u8FlagDCDC]);
			// co_trace(3u, "<CAN3> Upgrade Pack Ver is %d.", gpRectOTA->u16RectUpgVer[u8FlagDCDC]);
#endif

			while (1)
			{
				if (ProcessedModuleNum >= gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num)
				{
					break;
				}
				
				if(Handle_RectUpdt_4One(u8RecAddr, (BOOL)u8FlagDCDC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);
					u8RecAddr++;
				}
				else
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
					u8RecAddr++;	//editor cjy
				}
				ProcessedModuleNum++;
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
				
				// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] > 0)	//如果升级成功，直接上报
				// 	{
				// 		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
				// 		gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
				// 	}			
			}
#endif

			gSelfTest.fSetTaskID(TASK_ID_CAN3);

			//editor cjy
			/***** 查版本不对的，升第二遍 ****/
			//等待重排结束
			WaitAddressingFinish();
			//查版本
			// co_trace(3u, "<CAN3> Check Ver after first upgrade!");
			CheckOnlineRectVer(u8FlagDCDC);

			//升第二遍，将第一遍中由于地址重排漏掉的正常模块升级
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 11, "d", gpRectOTA->u8RectNum_NotUpg);
			// co_trace(3u, "<CAN3> Second Upgrade start! Sum is %d", gpRectOTA->u8RectNum_NotUpg);
			if(gpRectOTA->u8RectNum_NotUpg)
			UpgradeAppointedRects(gpRectOTA->u8RectNotUpg, gpRectOTA->u8RectNum_NotUpg, u8FlagDCDC);

			//触发升级，使变地址
			if(gpRectOTA->u8RectNum_Ver0)
			{
			gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
			// EVENT_RECORD(eHIS_EVENT_TYPE_6, 12, "d", gpRectOTA->u8RectNum_Ver0);
			// co_trace(3u, "<CAN3> Third Upgrade! Rect Ver 0 Sum is %d", gpRectOTA->u8RectNum_Ver0);
			UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);
			}

			//收集所有异常地址，升回来
			//for(i = 0; i < 3; ++i)	//editor cjy	U2不用等太长，检查一次就能把能救的救回来，就不回来了的都是因为U1挂了
			if((gpRectOTA->u8RectNum_NotAckVer) || (gpRectOTA->u8RectNum_Ver0))		//前面没有异常时，直接过
			{
				for(i = 0; i < 3; ++i)
				{
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
					if(gs_UpgradePowerModuleCtrl.u8TotalFailsNum)
						WaitAddressingFinish();
				}
			}

			// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] > 0)	//如果升级成功，直接上报
			// {
			// 	gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			// 	gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
			// }	

#if 0		//旧模块没有静默模式，会有0版本模块触发一次不能救回来，故检查多几次，直到查不到0版本模块
			for(i = 0; i < 3; ++i)		
			{
				//等待重排的时间逐渐延长
				for(UINT8 ii = 0; ii <= i; ++ii)
					WaitAddressingFinish();

				CheckOnlineRectVer(u8FlagDCDC);

				//全部有版本回复，且无版本0模块，跳出
				if((!gpRectOTA->u8RectNum_Ver0) && (!gpRectOTA->u8RectNum_NotAckVer))
					break;

				//升级，使其升成功，或变异常地址
				gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
				// EVENT_RECORD(eHIS_EVENT_TYPE_6, 15, "dd", gpRectOTA->u8RectNum_Ver0, (i+1));
				// co_trace(3u, "<CAN3> Rect Ver 0 Sum is %d. Retry count: %d", gpRectOTA->u8RectNum_Ver0, (i+1));
				UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);
			
				if(gpRectOTA->u8RectNum_NotAckVer)
				{
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_NotAckVer;
					WaitAddressingFinish();
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
				}
			}



#endif


#if ENABLE_OTA_RECT_SILLENCE
			for(i = 0; i < 3; ++i)
				SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif
			//end of upgrade
                              
			gSelfTest.fSetTaskID(TASK_ID_CAN3);
		}

		
		//while
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
		u8FlagClear_U2 = 1;
		/*if(gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count <= 0)
		{
			UpDataBeep(BEEP_ON_OFF, 10, 200, 100);
		}*/
		if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] > 0)//在线所有模块升级成功，则升级完成
		{
			// co_trace(3u, "<CAN3> U2 upgrade succeed, Num is: %d", gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1]);
			//UpDataBeep(BEEP_ON_OFF, gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count, 200, 100);
			//UpDataBeep(BEEP_ON_LONG,3, 5, 250);
			UpgradePowerModuleBeep(5u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
		}
		else
		{
			// co_trace(3u, "<CAN3> U2 upgrade failed");
			//UpDataBeep(BEEP_ON_LONG,20, 250,250);
			UpgradePowerModuleBeep(2u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 3;
			// LX_OTA_fail();
		}

		memset(gs_UpgradePowerModuleCtrl.u8FailsAddr, 0, mRECTNUM);

		gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] = TRUE;
		gEvChargeInfo.u8Power_on_Delay = 0;
		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] == 1)
		{
			gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] = 2;
		}
		return TRUE;
	}

	else
	{
		//判断出 无模块需要升级，则以后也不处理了！
		//su8Deal_Flag = TRUE;										//处理过了！
		gEvChargeInfo.u8RectUpgradeFlag = FALSE;
		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;

		if( (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState == 2)										//升级完成，延时清除升级进度与升级总状态标志位
			|| (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState != 0)										//升级失败，延时清除升级进度与升级总状态标志位
			&&(u8FlagClear_U2 == 1))						//PCS_U2升级才清除
		{
			u16UpData_U2Delay++;
			if(u16UpData_U2Delay >= 7000)
			{
				u8FlagClear_U2 = 0;
				u16UpData_U2Delay = 0;
				gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
				gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
			}
		}

		return FALSE;
	}
}

UINT8 u8_ABU_U1_UpDataProc(void);
UINT8 u8_ABU_U1_UpDataProc(void)
{
	INT8U ProcessedModuleNum = 0u;
	UINT8 u8IIII = 0;UNUSED(u8IIII);
	UINT8 u8RecAddr = 0, i, u8Idx;
	UNUSED(u8RecAddr);
	UINT8 u8FlagDCDC;	// 0-PFC	1-DC
	static UINT16 u16UpDataABU_U1Delay = 0;		//ABU_U1升级清零延时计数
	static UINT8 u8FlagClearABU_U1 =0;	//PCS_U1升级清零标志位
	UINT8 u8IsOK = FALSE;UNUSED(u8IsOK);


	u8FlagDCDC = (UINT8)IS_UPGRADE_PFC;
	gpRectOTA->u16RectUpgVer[0] = RectVer_String2Num(gs_UpGradeCtrl.cRcvFileName);//(gEvChargeInfo.OTAFileName[2]);

	if(gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] == TRUE)//处理过了，不需要再处理！
	{
		return FALSE;
	}

	gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count = 0;
	gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] = 0;

	UINT8 u8DebugFlag = 0;
	UNUSED(u8DebugFlag);

	if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == ABU_IS_UPDATE_FLAG)
		|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == ABU_IS_FORCE_UPDATE_FLAG)))
	{
		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 1;
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
		gEvChargeInfo.u8RectUpgradeFlag = TRUE;

		WaitAddressingFinish();
		DleayUpgrade();
		DleayUpgrade();


		if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == ABU_IS_UPDATE_FLAG)|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == ABU_IS_FORCE_UPDATE_FLAG)) && (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.ABU1Exist == TRUE))
		{
#if 0
			for(u8RecAddr = 0; (u8RecAddr < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num) && (u8RecAddr < mRECTNUM); u8RecAddr++)
			{
				if(Handle_RectUpdt_4One(u8RecAddr,	IS_UPGRADE_PFC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);  
				}
				else
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
				}
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
			}
#else
			u8RecAddr = ABU_Addr;	//FA
			ABU->PFC_Version = VOID_RECT_VER * 0.0100;

			while (1)
			{
				if (ProcessedModuleNum >= 1)
				{
					break;
				}

				if(Handle_RectUpdt_4One(u8RecAddr, (BOOL)u8FlagDCDC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100); 
				}
				else
				{
					AfterOneUpgrade();
					//SendAddressingCmd();
					//SendAddressingCmd();
		
					//DleayUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
				}

				ProcessedModuleNum++;
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);		
			}
			gSelfTest.fSetTaskID(TASK_ID_CAN3);

	//editor cjy
			/***** 查版本不对的，升第二、三遍 ****/
#if ENABLE_OTA_RECT_SILLENCE
			for(UINT8 i = 0; i < 3; ++i)
			SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif
			//查版本
			CheckOnlineABUVer(u8FlagDCDC);

			//升第二遍，将第一遍中由于地址重排漏掉的升级一遍
			if(gpRectOTA->u8RectNum_NotUpg )
			UpgradeAppointedRects(gpRectOTA->u8RectNotUpg, gpRectOTA->u8RectNum_NotUpg, u8FlagDCDC);

			//升第三遍
			if(gpRectOTA->u8RectNum_Ver0 )
			{
			gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
			UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);				
			}

			//重新将变异常地址的升回来	U1升挂的概率更高，多等几次
			if((gpRectOTA->u8RectNum_NotAckVer) || (gpRectOTA->u8RectNum_Ver0))		//前面没有异常时，直接过
			{
				for(i = 0; i < 3; ++i)
				{
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
				}
			}

			// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)	//如果升级成功，直接上报
			// {
			// 	gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			// 	gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
			// }	

#if 0		//旧模块没有静默模式，会有0版本模块触发一次不能救回来，故检查多几次，直到查不到0版本模块
			//重复三次，排除版本0的模块
			for(i = 0; i <3; ++i) 		
			{
				CheckOnlineABUVer(u8FlagDCDC);

				//全部有版本回复，且无版本0模块，跳出
				if((!gpRectOTA->u8RectNum_Ver0) && (!gpRectOTA->u8RectNum_NotAckVer))
					break;

				//升级，使其升成功，或变异常地址
				gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
				UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);

				if(gpRectOTA->u8RectNum_NotAckVer)
				{
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_NotAckVer;
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
				}
				
			}//end for
#endif

			

#endif



			
#if ENABLE_OTA_RECT_SILLENCE
			for(i = 0; i < 3; ++i)
				SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif
			//end of upgrade


			gSelfTest.fSetTaskID(TASK_ID_CAN3);
		} // if(GetOnline_RectNum())

		

		//while
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
		u8FlagClearABU_U1 = 1;

		if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)//在线所有模块升级成功，则上报升级成功
		{
			UpgradePowerModuleBeep(5u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
		}
		else
		{
			UpgradePowerModuleBeep(2u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 3;
		}

		gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] = TRUE;
		gEvChargeInfo.u8Power_on_Delay = 0;

		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] == 1)
		{
			gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] = 2;
		}
		return TRUE;
	}
	else
	{
		//判断出 无模块需要升级，则以后也不处理了！
		gEvChargeInfo.u8RectUpgradeFlag = FALSE;
		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;

		if(((gs_UpgradePowerModuleCtrl.u8TotalUpgradeState == 2)		//升级完成，且升级成功,延时清除
			|| (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState != 0))&&		                                                  	  //升级失败，延时清除													
			(u8FlagClearABU_U1 == 1)  &&										//升级失败，延时清除升级进度与升级总状态标志位
			((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == ABU_IS_UPDATE_FLAG)
			|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == ABU_IS_FORCE_UPDATE_FLAG)))				
			{
				u16UpDataABU_U1Delay++;
				if(u16UpDataABU_U1Delay >= 7000)
				{
					u8FlagClearABU_U1 = 0;
					u16UpDataABU_U1Delay = 0;
					gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
					gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
				}
			}

		return FALSE;
	}
}



UINT8 u8_ABU_U2_UpDataProc(void);
UINT8 u8_ABU_U2_UpDataProc(void)
{
	INT8U ProcessedModuleNum = 0u;
	UINT8 u8IIII = 0;UNUSED(u8IIII);
	UINT8 u8RecAddr = 0, i, u8Idx;
	UNUSED(u8RecAddr);
	UINT8 u8FlagDCDC;	// 0-PFC	1-DC
	static UINT16 u16UpDataABU_U2Delay = 0;		//ABU_U2升级清零延时计数
	static UINT8 u8FlagClearABU_U2 =0;	//ABU_U2升级清零标志位
	UINT8 u8IsOK = FALSE;UNUSED(u8IsOK);


	u8FlagDCDC = (UINT8)IS_UPGRADE_DC;
	gpRectOTA->u16RectUpgVer[1] = RectVer_String2Num(gs_UpGradeCtrl.cRcvFileName);//(gEvChargeInfo.OTAFileName[2]);

	if(gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] == TRUE)//处理过了，不需要再处理！
	{
		return FALSE;
	}

	gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count = 0;
	gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] = 0;

	UINT8 u8DebugFlag = 0;
	UNUSED(u8DebugFlag);

	if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == ABU_IS_UPDATE_FLAG)
		|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == ABU_IS_FORCE_UPDATE_FLAG)))
	{
		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 1;
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
		gEvChargeInfo.u8RectUpgradeFlag = TRUE;

		WaitAddressingFinish();
		DleayUpgrade();
		DleayUpgrade();


		if(((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == ABU_IS_UPDATE_FLAG)|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == ABU_IS_FORCE_UPDATE_FLAG)) && (gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.ABU1Exist == TRUE))
		{
#if 0
			for(u8RecAddr = 0; (u8RecAddr < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num) && (u8RecAddr < mRECTNUM); u8RecAddr++)
			{
				if(Handle_RectUpdt_4One(u8RecAddr,	IS_UPGRADE_PFC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);  
				}
				else
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
				}
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);
			}
#else
			u8RecAddr = ABU_Addr;	//FA
			ABU->PFC_Version = VOID_RECT_VER * 0.0100;

			while (1)
			{
				if (ProcessedModuleNum >= 1)
				{
					break;
				}

				if(Handle_RectUpdt_4One(u8RecAddr, (BOOL)u8FlagDCDC, FALSE))
				{
					AfterOneUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC]++;
					UpDataBeep(BEEP_ON_OFF, 1, 200, 100); 
				}
				else
				{
					AfterOneUpgrade();
					//SendAddressingCmd();
					//SendAddressingCmd();
		
					//DleayUpgrade();
					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
					u8IsOK = TRUE;
				}

				ProcessedModuleNum++;
				DleayUpgrade();
				u8RcvMsg(30);
				gSelfTest.fSetTaskID(TASK_ID_CAN3);		
			}
			gSelfTest.fSetTaskID(TASK_ID_CAN3);

	//editor cjy
			/***** 查版本不对的，升第二、三遍 ****/
#if ENABLE_OTA_RECT_SILLENCE
			for(UINT8 i = 0; i < 3; ++i)
			SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);      //升级ABU不需要静默
#endif
			//查版本
			CheckOnlineABUVer(u8FlagDCDC);

			//升第二遍，将第一遍中由于地址重排漏掉的升级一遍
			if(gpRectOTA->u8RectNum_NotUpg )
			UpgradeAppointedRects(gpRectOTA->u8RectNotUpg, gpRectOTA->u8RectNum_NotUpg, u8FlagDCDC);

			//升第三遍
			if(gpRectOTA->u8RectNum_Ver0 )
			{
			gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
			UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);				
			}

			//重新将变异常地址的升回来	U1升挂的概率更高，多等几次
			if((gpRectOTA->u8RectNum_NotAckVer) || (gpRectOTA->u8RectNum_Ver0))		//前面没有异常时，直接过
			{
				for(i = 0; i < 3; ++i)
				{
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
				}
			}

			// if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)	//如果升级成功，直接上报
			// {
			// 	gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			// 	gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
			// }	

#if 0		//旧模块没有静默模式，会有0版本模块触发一次不能救回来，故检查多几次，直到查不到0版本模块
			//重复三次，排除版本0的模块
			for(i = 0; i <3; ++i) 		
			{
				CheckOnlineABUVer(u8FlagDCDC);

				//全部有版本回复，且无版本0模块，跳出
				if((!gpRectOTA->u8RectNum_Ver0) && (!gpRectOTA->u8RectNum_NotAckVer))
					break;

				//升级，使其升成功，或变异常地址
				gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_Ver0;
				UpgradeAppointedRects(gpRectOTA->u8RectVer0, gpRectOTA->u8RectNum_Ver0, u8FlagDCDC);

				if(gpRectOTA->u8RectNum_NotAckVer)
				{
					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC] -= gpRectOTA->u8RectNum_NotAckVer;
					Dear_FailsRECT_Upgrade((BOOL)u8FlagDCDC);
				}
				
			}//end for
#endif

			

#endif



			
#if ENABLE_OTA_RECT_SILLENCE
			for(i = 0; i < 3; ++i)
				SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
#endif
			//end of upgrade


			gSelfTest.fSetTaskID(TASK_ID_CAN3);
		} // if(GetOnline_RectNum())

		

		//while
		gSelfTest.fSetTaskID(TASK_ID_CAN3);
		dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
		u8FlagClearABU_U2 = 1;

		if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] > 0)//在线所有模块升级成功，则上报升级成功
		{
			UpgradePowerModuleBeep(5u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
		}
		else
		{
			UpgradePowerModuleBeep(2u, 250u);
			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 3;
		}

		gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] = TRUE;
		gEvChargeInfo.u8Power_on_Delay = 0;

		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] == 1)
		{
			gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] = 2;
		}
		return TRUE;
	}
	else
	{
		//判断出 无模块需要升级，则以后也不处理了！
		gEvChargeInfo.u8RectUpgradeFlag = FALSE;
		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;

		if(((gs_UpgradePowerModuleCtrl.u8TotalUpgradeState == 2)		//升级完成，且升级成功,延时清除
			|| (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState != 0))&&		                                                  	  //升级失败，延时清除													
			(u8FlagClearABU_U2 == 1)  &&										//升级失败，延时清除升级进度与升级总状态标志位
			((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == ABU_IS_UPDATE_FLAG)
			|| (gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == ABU_IS_FORCE_UPDATE_FLAG)))				
			{
				u16UpDataABU_U2Delay++;
				if(u16UpDataABU_U2Delay >= 7000)
				{
					u8FlagClearABU_U2 = 0;
					u16UpDataABU_U2Delay = 0;
					gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
					gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
				}
			}

		return FALSE;
	}
}

// UINT8 u8_U1_boot_UpDataProc(void);
// UINT8 u8_U1_boot_UpDataProc(void)
// {
// 	INT8U ProcessedModuleNum = 0u;
// 	UINT8 u8IIII = 0;UNUSED(u8IIII);
// 	UINT8 u8RecAddr = 0, i, u8Idx;
// 	UNUSED(u8RecAddr);
// 	UINT8 u8FlagDCDC;	// 0-PFC	1-DC 
// 	static UINT16 u16UpData_U1_BOOTDelay = 0;	//PCS_U1_BOOT升级清零延时计数

// 	//editor cjy
// 	u8FlagDCDC = (UINT8)IS_UPGRADE_PFC;
// 	gpRectOTA->u16RectUpgVer[0] = RectVer_String2Num(gs_UpGradeCtrl.cRcvFileName);//(gEvChargeInfo.OTAFileName[2]);

// 	if(gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] == TRUE)//处理过了，不需要再处理！
// 	{
// 		return FALSE;
// 	}

// 	gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count = 0;
// 	gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] = 0;

// 	UINT8 u8DebugFlag = 0;UNUSED(u8DebugFlag);

// 	if((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == RECT_IS_PFCBOOT_UPDATE_FLAG))
// 	{
// 		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 1;
// 		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
// 		gEvChargeInfo.u8RectUpgradeFlag = TRUE;

// 		WaitAddressingFinish();
// 		DleayUpgrade();
// 		DleayUpgrade();

// 		//UpDataBeep(BEEP_ON_OFF, 2, 75, 75);
// 		//UpDataBeep(BEEP_ON_LONG,2, 5, 250);

// 		if((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == RECT_IS_PFCBOOT_UPDATE_FLAG))
// 		{

// 			u8RecAddr = 0u;
// #if 1
// 			for(u8Idx = 0; u8Idx < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num; u8Idx++)
// 			{				
// 				GetSingle_VerInfo_SingleModule(u8Idx, &gs_CANComm, &gs_CANComm);
// 				gs_CANComm.RectData[u8Idx].fAC_Ver = VOID_RECT_VER * 0.0100;
// 			}
// #endif
// 			while (1)
// 			{
// 				if (ProcessedModuleNum >= gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num)
// 				{
// 					break;
// 				}

// 				if(Handle_RectUpdt_4One(u8RecAddr, (BOOL)u8FlagDCDC, FALSE))
// 				{
// 					AfterOneUpgrade();
// 					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC]++;
// 					UpDataBeep(BEEP_ON_OFF, 1, 200, 100); 
// 					u8RecAddr++;
// 				}
// 				else
// 				{
// 					//AfterOneUpgrade();
// 					SendAddressingCmd();
// 					SendAddressingCmd();
		
// 					DleayUpgrade();
// 					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
// 					u8RecAddr++;	//editor cjy
// 				}

// 				ProcessedModuleNum++;
// 				DleayUpgrade();
// 				u8RcvMsg(30);
// 				gSelfTest.fSetTaskID(TASK_ID_CAN3);
				
// 			}
		
// 			#if ENABLE_OTA_RECT_SILLENCE
// 			for(i = 0; i < 3; ++i)
// 				SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
// 			#endif
// 			//end of upgrade

// 			gSelfTest.fSetTaskID(TASK_ID_CAN3);
// 		} // if(GetOnline_RectNum())

// 		//while
// 		gSelfTest.fSetTaskID(TASK_ID_CAN3);
// 		dv_E2PROM.fWriteE2PROM(E2PROM_U1_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
// 		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;

// 		if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[0] > 0)
// 		{

// 			UpgradePowerModuleBeep(5u, 250u);
// 			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
// 			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
// 		}
// 		else
// 		{
// 			UpgradePowerModuleBeep(2u, 250u);
// 			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 3;

// 		}

// 		gs_UpgradePowerModuleCtrl.u8Deal_Flag[0] = TRUE;
// 		gEvChargeInfo.u8Power_on_Delay = 0;

// 		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] == 1)
// 		{
// 			gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[0] = 2;
// 		}
// 		return TRUE;
// 	}
// 	else
// 	{
// 		//判断出 无模块需要升级，则以后也不处理了！
// 		//su8Deal_Flag = TRUE;										//处理过了！
// 		gEvChargeInfo.u8RectUpgradeFlag = FALSE;
// 		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;

// 		if(((gs_UpgradePowerModuleCtrl.u8TotalUpgradeState == 2)									//升级完成，延时清除升级进度与升级总状态标志位
// 			|| (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState != 0))&&								//升级失败，延时清除升级进度与升级总状态标志位
// 			(gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] == RECT_IS_PFCBOOT_UPDATE_FLAG))		//PCS_U1_BOOT升级才清除
// 			{
// 				u16UpData_U1_BOOTDelay++;
// 				if(u16UpData_U1_BOOTDelay >= 7000)
// 				{
// 					u16UpData_U1_BOOTDelay = 0;
// 					gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
// 					gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
// 				}
// 			}

// 		return FALSE;
// 	}
	
// }


// UINT8 u8_U2_boot_UpDataProc(void);
// UINT8 u8_U2_boot_UpDataProc(void)
// {
// 	INT8U ProcessedModuleNum = 0u;
// 	UINT8 u8ii = 0, i, u8Idx;
// 	UNUSED(u8ii);
// 	UINT8 u8U1IsFail = FALSE;UNUSED(u8U1IsFail);
// 	UINT8 u8RecAddr = 0;
// 	UINT8 u8FlagDCDC;	// 0-PFC	1-DC
// 	static UINT16 u16UpData_U2_BOOTDelay = 0;		//PCS_U2升级清零延时计数
// 	//edit cjy
// 	u8FlagDCDC = (UINT8)IS_UPGRADE_DC;
// 	gpRectOTA->u16RectUpgVer[1] = RectVer_String2Num(gs_UpGradeCtrl.cRcvFileName);//(gEvChargeInfo.OTAFileName[3]);

// 	if(gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] == TRUE)//处理过了，不需要再处理！
// 	{
// 		return FALSE;
// 	}

// 	gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count = 0;
// 	gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] = 0;

// 	UINT8 u8IIII = 0;UNUSED(u8IIII);
// 	UINT8 u8DebugFlag = 0;UNUSED(u8DebugFlag);
// 	UINT8 u8IsOK = FALSE;UNUSED(u8IsOK);

// 	if ((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == RECT_IS_DCDCBOOT_UPDATE_FLAG))
// 	{
// 		gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 1;
// 		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[0] = RECT_ISNT_UPDATE_FLAG;
// 		gEvChargeInfo.u8RectUpgradeFlag = TRUE;

// 		WaitAddressingFinish();
// 		DleayUpgrade();
// 		DleayUpgrade();
		
// 		if((gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == RECT_IS_DCDCBOOT_UPDATE_FLAG))
// 		{

// 			u8RecAddr = 0u;

// #if 1
// 			for(u8Idx = 0; u8Idx < gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num; u8Idx++)
// 			{
// 				GetSingle_VerInfo_SingleModule(u8Idx, &gs_CANComm, &gs_CANComm);
// 				gs_CANComm.RectData[u8Idx].fDC_Ver = VOID_RECT_VER * 0.0100;
// 			}

// #endif

// 			while (1)
// 			{
// 				if (ProcessedModuleNum >= gs_UpgradePowerModuleCtrl.u8TotalCommRCT_Num)
// 				{
// 					break;
// 				}
				
// 				if(Handle_RectUpdt_4One(u8RecAddr, (BOOL)u8FlagDCDC, FALSE))
// 				{
// 					AfterOneUpgrade();
// 					gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[u8FlagDCDC]++;
// 					UpDataBeep(BEEP_ON_OFF, 1, 200, 100);
// 					u8RecAddr++;
// 				}
// 				else
// 				{
// 					AfterOneUpgrade();
// 					gs_UpgradePowerModuleCtrl.u8UpDated_Fails_Count++;
// 					u8IsOK = TRUE;
// 					u8RecAddr++;	//editor cjy
// 				}
// 				ProcessedModuleNum++;
// 				DleayUpgrade();
// 				u8RcvMsg(30);
// 				gSelfTest.fSetTaskID(TASK_ID_CAN3);

		
// 			}

// 		#if ENABLE_OTA_RECT_SILLENCE
// 			for(i = 0; i < 3; ++i)
// 				SendRectSillenceModeBroadcast(R_PROTO_BROADCAST, 0xA0);
// 		#endif
// 			//end of upgrade
                              
// 			gSelfTest.fSetTaskID(TASK_ID_CAN3);
// 		}
		
// 		// while
// 		gSelfTest.fSetTaskID(TASK_ID_CAN3);
// 		dv_E2PROM.fWriteE2PROM(E2PROM_U2_UPDATA_FLAG, RECT_ISNT_UPDATE_FLAG);
// 		gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] = RECT_ISNT_UPDATE_FLAG;
		
// 		if(gs_UpgradePowerModuleCtrl.u8UpDated_Success_Count[1] > 0)
// 		{

// 			UpgradePowerModuleBeep(5u, 250u);
// 			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 2;
// 			gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 100;
// 		}
// 		else
// 		{
// 			UpgradePowerModuleBeep(2u, 250u);
// 			gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 3;
// 		}


// 		gs_UpgradePowerModuleCtrl.u8Deal_Flag[1] = TRUE;
// 		gEvChargeInfo.u8Power_on_Delay = 0;
// 		if(gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] == 1)
// 		{
// 			gs_UpgradePowerModuleCtrl.u8RectUpdateSuccessWriteUDisk_Flag[1] = 2;
// 		}
// 		return TRUE;
// 	}
// 	else
// 	{
// 		//判断出 无模块需要升级，则以后也不处理了！
// 		//su8Deal_Flag = TRUE;										//处理过了！
// 		gEvChargeInfo.u8RectUpgradeFlag = FALSE;
// 		gs_UpgradePowerModuleCtrl.u8CurrUpgradeState = 0;

// 		if(((gs_UpgradePowerModuleCtrl.u8TotalUpgradeState == 2)									//升级完成，延时清除升级进度与升级总状态标志位
// 			|| (gs_UpgradePowerModuleCtrl.u8TotalUpgradeState != 0))&&									//升级失败，延时清除升级进度与升级总状态标志位
// 			(gs_UpgradePowerModuleCtrl.u8WhetherNeedUpdatFlag[1] == RECT_IS_DCDCBOOT_UPDATE_FLAG))		//PCS_U2_BOOT升级才清除
// 		{
// 			u16UpData_U2_BOOTDelay++;
// 			if(u16UpData_U2_BOOTDelay >= 7000)
// 			{
// 				u16UpData_U2_BOOTDelay = 0;
// 				gs_UpgradePowerModuleCtrl.fCurrModuleProgress = 0;
// 				gs_UpgradePowerModuleCtrl.u8TotalUpgradeState = 0;
// 			}
// 		}

// 		return FALSE;
// 	}
	
// }

