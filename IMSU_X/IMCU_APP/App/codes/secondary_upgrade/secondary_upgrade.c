/**
 ******************************************************************************
 * Copyright(c) Infy Power 2023-2024
 * @file    IMEU_Upgrage.c
 * @author  ZZC955
 * @version V1.0
 * @date    2024-1-2
 * @brief   沿用LX项目的升级EU2升级模块，经过改动为IMBTU1，灯板，EU2板升级
 * @history 2024-1-2 ZZC955从LX项目移到本项目
 ******************************************************************************
 */

#include "secondary_upgrade.h"
#include "imeu2_upgrade.h"

CAN_HandleTypeDef CAN_1_Handle;
CanRxMsgTypeDef Rxdata;
coTime TxTimer;
Secondary_Upgrade_info_t g_stImeu2UpgradeInfo[2] = {0};
static UINT8 g_u8TransmitBuf[1024] = {0};
NOINIT MyTimer_info_t IMEU_upgrade_timer;
NOINIT MyTimer_info_t IMEU_upgrade_timeout;
NOINIT MyTimer_info_t IMEU_upgrade_idle_timer;
NOINIT MyTimer_info_t QueryTimer;
NOINIT MyTimer_info_t SendTimer;
NOINIT MyTimer_info_t handshakeTimer;
NOINIT MyTimer_info_t reBootTimer;
extern INT8U IMEU_GetVersion[2];

extern void IMEU2RcvUpgradeDataDeal(Secondary_Upgrade_info_t *me,IMEU_INFO *pImeuInfo);

#define INT_STATE_MACHINE(_P, _MachineId, _fnProc) ((_P)->MachineIdx = (_MachineId), (_P)->fproc = (_fnProc))

// static函数声明
IMEU_UPGRADE_MACHINE_INFO IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_state_max];

static int UpgradeMachineIdle(Secondary_Upgrade_info_t *me);
static int UpgradeMachineJump(Secondary_Upgrade_info_t *me);
static int UpgradeMachineWaitingHandshake(Secondary_Upgrade_info_t *me);
static int UpgradeMachineStart(Secondary_Upgrade_info_t *me);
static int UpgradeMachineTransmit(Secondary_Upgrade_info_t *me);
static int UpgradeMachineFinish(Secondary_Upgrade_info_t *me);
static int UpgradeMachineEnd(Secondary_Upgrade_info_t *me);
static int UpgradeMachineReboot(Secondary_Upgrade_info_t *me);

void SecondaryUpgradeInit(UINT8 u8UpgradeType, Secondary_Upgrade_info_t *me, UINT8 u8Addr)
{
    me->Machine.Step = eState_Idle;
    me->MachineState = eIMEU_Upgrade_idle;
	me->fProgress = 0;
	me->u8CurrUpgradeState = 0;
	me->u8TotalUpgradeState = 0;
	me->u8UpgradeErrCode = 0;
	me->u8_POWER_ON_Flag = 1;
	me->u8RectUpdateSuccessWriteUDisk_Flag = 0;
	me->u16UpdateTimeOutCount = 0;
    INT_STATE_MACHINE(&IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_idle], eIMEU_Upgrade_idle, UpgradeMachineIdle);
    INT_STATE_MACHINE(&IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_jump], eIMEU_Upgrade_jump, UpgradeMachineJump);
    INT_STATE_MACHINE(&IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_waiting_handshake], eIMEU_Upgrade_waiting_handshake, UpgradeMachineWaitingHandshake);
    INT_STATE_MACHINE(&IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_start], eIMEU_Upgrade_start, UpgradeMachineStart);
    INT_STATE_MACHINE(&IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_transmit], eIMEU_Upgrade_transmit, UpgradeMachineTransmit);
    INT_STATE_MACHINE(&IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_finish], eIMEU_Upgrade_finish, UpgradeMachineFinish);
    INT_STATE_MACHINE(&IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_end], eIMEU_Upgrade_end, UpgradeMachineEnd);
    INT_STATE_MACHINE(&IMEU_Upgrade_MachineInfo[eIMEU_Upgrade_reboot], eIMEU_Upgrade_reboot, UpgradeMachineReboot);
   
   if (u8UpgradeType == UPGRADE_TYPE_IMEU2) {
        me->CurrDecviceAddr = CFG_IMEU2_ADDR + u8Addr;
        me->QuerystatusFunc = Imeu2Querystatus;
        me->JumptoBootFunc = Imeu2JumptoBoot;
        me->u32FlashBaseAddr = IMEU2_UPDATE_BASE_ADDR;
        me->u32FlashDataAddr = IMEU2_UPDATE_DATA_ADDR;
        me->HandshakeFunc = Imeu2Handshake;
        me->StartSectionFunc = Imeu2StartSection;
        me->TransmitMessageFunc = Imeu2TransmitMessage;
        me->EndOneSectionFunc = Imeu2EndOneSection;
        me->RebootFunc = Imeu2Reboot;
        me->u16QueryInterval = 1000U;
        me->u16TransmitInterval = 70U;
        me->u8CommType = COMM_TYPE_485;
        me->u8BoardType = UPGRADE_TYPE_IMEU2;
    }

}

void SecondaryUpdateTimeOut(Secondary_Upgrade_info_t *me)
{
	if(me->u8UpgradeFlag == TRUE)
	{
	  	if (me->u16UpdateTimeOutCount >= (10*60))  
		{
			me->u8UpgradeFlag = FALSE;
			if(me->u8CurrUpgradeState == 3)
			{
				me->u8TotalUpgradeState = 3;
			}
		}
	}
	else
	{
		me->u16UpdateTimeOutCount = 0;
	}
}

UINT8 SecondaryUpgradeProc(void)
{
	if (g_stImeu2UpgradeInfo[0].u8UpgradeFlag == TRUE) 
	{
		SecondaryUpdateTimeOut(&g_stImeu2UpgradeInfo[0]);
		IMEU2RcvUpgradeDataDeal(&g_stImeu2UpgradeInfo[0],&gIMEU_Info[0]);
		SecondaryUpgradeMachine(&g_stImeu2UpgradeInfo[0]);
		if (g_stImeu2UpgradeInfo[0].TotalSectionNum != 0) 
		{
			g_stImeu2UpgradeInfo[0].fProgress = ((float)g_stImeu2UpgradeInfo[0].CurrSendSection  *100) / (float)g_stImeu2UpgradeInfo[0].TotalSectionNum;
		}
		return TRUE;
	}

	if (g_stImeu2UpgradeInfo[1].u8UpgradeFlag == TRUE)
	{
		SecondaryUpdateTimeOut(&g_stImeu2UpgradeInfo[1]);
		IMEU2RcvUpgradeDataDeal(&g_stImeu2UpgradeInfo[1],&gIMEU_Info[1]);
		SecondaryUpgradeMachine(&g_stImeu2UpgradeInfo[1]);
		if (g_stImeu2UpgradeInfo[1].TotalSectionNum != 0)
		{
			g_stImeu2UpgradeInfo[1].fProgress = ((float)g_stImeu2UpgradeInfo[1].CurrSendSection  *100) / (float)g_stImeu2UpgradeInfo[1].TotalSectionNum;
		}
		return TRUE;
	}

	return FALSE;
}

/************************* IMEU2 upgrade deamon idle *************************/
static void UpgradeIdleInit(Secondary_Upgrade_info_t *me)
{
	me->CurrSendSection = 0u;
	me->SendByteNum = 0u;
	me->u8CurrDeviceStatus = 0xFF;
	me->IsRetransmit = FALSE;
	MyTimeout_start(&IMEU_upgrade_timer);
	MyTimeout_start(&QueryTimer);
	MyTimeout_start(&IMEU_upgrade_idle_timer);
}

static INT8U UpgradeIdle(Secondary_Upgrade_info_t *me)
{
	if (MyTimeout(&QueryTimer, me->u16QueryInterval))
	{
		me->QuerystatusFunc(me->CurrDecviceAddr);
	}

	//IMEU status -- 0 Communication route
	// if ((me->u8CurrDeviceStatus == 0u) || (me->u8CurrDeviceStatus == 18u))
	if (me->u8CurrDeviceStatus != 0xFF)
	{
		me->u8CurrUpgradeState = 1;
		me->u8TotalUpgradeState = 1;
		MyTimeout_start(&SendTimer);
		return eIMEU_Upgrade_jump;
	}
	else if (me->u8CurrDeviceStatus == 1u)//IMEU status -- 1 waiting handshake
	{
		me->u8CurrUpgradeState = 1;
		me->u8TotalUpgradeState = 1;
		MyTimeout_start(&SendTimer);
		return eIMEU_Upgrade_waiting_handshake;
	}

	if (MyTimeout(&IMEU_upgrade_idle_timer, mSec_X(10u)))
	{
		MyTimeout_stop(&IMEU_upgrade_timeout);
	}

	return eIMEU_Upgrade_idle;
}

static void UpgradeIdleExit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_stop(&IMEU_upgrade_timer);
	MyTimeout_stop(&QueryTimer);
	// if (me->CurrDecviceAddr == CFG_IMBTU1_ADDR_1)
	{
		MyTimeout_start(&IMEU_upgrade_timeout);
	}
}
/************************* IMEU2 upgrade deamon idle *************************/

/************************* IMEU2 upgrade deamon jump *************************/
static void UpgradeJumpInit(Secondary_Upgrade_info_t *me)
{
	//add for 2.06
	// gEvChargeInfo.u8IMEU2_NeedSilence = !FALSE;
    me->u8CurrDeviceStatus = 0xFF;
    MyTimeout_start(&SendTimer);
    MyTimeout_start(&IMEU_upgrade_timer);
	MyTimeout_start(&QueryTimer);
	// co_trace1(3u, "<IMEU_Update> daemon jump init, Device Addr is 0x%x", me->CurrDecviceAddr);
}

static INT8U UpgradeJump(Secondary_Upgrade_info_t *me)
{
	if (MyTimeout(&SendTimer, mMs_X(100u)))
	{
		me->JumptoBootFunc(me->CurrDecviceAddr);
        if (me->u8CommType == COMM_TYPE_485) {
            MyTimeout_stop(&SendTimer);
            MyTimeout_start(&QueryTimer);
        }
    }
	
	//Query IMEU status every 20ms
	if (MyTimeout(&QueryTimer, me->u16QueryInterval))
	{
		me->QuerystatusFunc(me->CurrDecviceAddr);
        if (me->u8CommType == COMM_TYPE_485) {
            MyTimeout_stop(&QueryTimer);
            MyTimeout_start(&SendTimer);
        }
	}

	//IMEU status -- 1 waiting handshake
	if ((me->u8CurrDeviceStatus == 1u) || (me->u8CurrDeviceStatus == 13u))
	{
		return eIMEU_Upgrade_waiting_handshake;
	}
	
	return eIMEU_Upgrade_jump;
}

static void UpgradeJumpExit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_stop(&IMEU_upgrade_timer);
	MyTimeout_stop(&QueryTimer);
}
/************************* IMEU2 upgrade deamon jump *************************/

/************************* IMEU2 upgrade deamon waiting handshake *************************/
static void UpgradeWaitingHandshakeInit(Secondary_Upgrade_info_t *me)
{
	UINT32 Addres = 0;
	UCHAR Buf[4] = {0};
	INT16 remainder;
	co_dword dword;

	MutexLock(gEvChargeInfo.hMetex_FLASH);

	Addres = me->u32FlashBaseAddr;
	SPIFlashRead(Buf, Addres, 4u);
	MutexUnlock(gEvChargeInfo.hMetex_FLASH);

	dword.b[0] = Buf[0];
	dword.b[1] = Buf[1];
	dword.b[2] = Buf[2];
	dword.b[3] = Buf[3];
	me->TotalByteNum = dword.v;
	me->TotalSectionNum = me->TotalByteNum / 1024U;
	remainder = me->TotalByteNum % 1024U;

	if (remainder > 0)
	{
		me->TotalSectionNum += 1u;
	}
	MyTimeout_start(&IMEU_upgrade_timer);
	MyTimeout_start(&QueryTimer);
	MyTimeout_start(&handshakeTimer);
    me->u8CurrDeviceStatus = 0xFF;
	// co_trace1(3u, "<IMEU_Update> daemon waiting handshake init, Device Addr is 0x%x", me->CurrDecviceAddr);
}

static INT8 UpgradeWaitingHandshake(Secondary_Upgrade_info_t *me)
{
	//Query IMEU status every 100ms
	if (MyTimeout(&handshakeTimer, mMs_X(100u)))
	{
		me->HandshakeFunc(me);
        if (me->u8CommType == COMM_TYPE_485) {
            MyTimeout_stop(&handshakeTimer);
            MyTimeout_start(&QueryTimer);
        }
	}

	if (MyTimeout(&QueryTimer, me->u16QueryInterval))
	{
		me->QuerystatusFunc(me->CurrDecviceAddr);
        if (me->u8CommType == COMM_TYPE_485) {
            MyTimeout_stop(&QueryTimer);
            MyTimeout_start(&handshakeTimer);
        }
	}

	//IMEU status -- 4 Erase succeeded   13 Format succeeded
	if ((me->u8CurrDeviceStatus == 4u) || (me->u8CurrDeviceStatus == 13u))
	{
		return eIMEU_Upgrade_start;
	}
	
	return eIMEU_Upgrade_waiting_handshake;
}

static void IMEU_upgrade_daemon_waiting_handshake_exit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_stop(&IMEU_upgrade_timer);
	MyTimeout_stop(&QueryTimer);
	MyTimeout_stop(&handshakeTimer);
}

static BOOL ReadUpgradeFile(UINT8 *pBuf, Secondary_Upgrade_info_t *me)
{
    UINT16 RemainBytes = 0u;
    UINT32 Addr = me->u32FlashDataAddr;
    MutexLock(gEvChargeInfo.hMetex_FLASH);

    if (me->CurrSendSection > 88U) // max APP size 88k
    {
        MutexUnlock(gEvChargeInfo.hMetex_FLASH);
        return FALSE;
    }

    if (me->CurrSendSection < (me->TotalSectionNum - 1)) // Full section 1024 bytes
    {
        Addr = me->u32FlashDataAddr + me->CurrSendSection * 1024U;
        SPIFlashRead(pBuf, Addr, 1024U);
        me->CurrSectionBytes = 1024u;
    }
    else // less than 1024 bytes
    {
        RemainBytes = me->TotalByteNum - me->CurrSendSection * 1024U;
        Addr = me->u32FlashDataAddr + me->CurrSendSection * 1024U;
        SPIFlashRead(pBuf, Addr, RemainBytes);
        me->CurrSectionBytes = RemainBytes;
    }

    MutexUnlock(gEvChargeInfo.hMetex_FLASH);
    return TRUE;
}

/************************* IMEU2 upgrade deamon start *************************/
static void UpgradeStartInit(Secondary_Upgrade_info_t *me)
{
    INT8 FrameNum = 0u;
    UINT8 u8BytesPerFrame;
    // INT32U StartAddr = 0u;

    if (me->u8CommType == COMM_TYPE_485) {
        u8BytesPerFrame = 64U;
    } else {
        u8BytesPerFrame = 6U;
    }
    ReadUpgradeFile(g_u8TransmitBuf, me);
    FrameNum = me->CurrSectionBytes / u8BytesPerFrame; // How many frames are there in each segment
    if (me->CurrSectionBytes % u8BytesPerFrame)        // 6 byte frames
    {
        me->TotalFramePerSection = FrameNum + 1;
    }
    else
    {
        me->TotalFramePerSection = FrameNum;
    }

    me->CurrCRC = CalcCodeCRC(g_u8TransmitBuf, me->CurrSectionBytes); // calculate crc
    me->CurrSectionSendBytes = 0u;
    MyTimeout_start(&IMEU_upgrade_timer);
    MyTimeout_start(&QueryTimer);
    MyTimeout_start(&handshakeTimer);
    me->u8CurrDeviceStatus = 0xFF;
    // StartAddr = CFG_IMBTU1_APP_ADDR + me->CurrSendSection * 1024;
    // co_trace1(3u, "<IMEU_Update> daemon start init, Device Addr is 0x%x, CurrSection is: %d, Bytes is: %d, Flash Addr is: 0x%x", me->CurrDecviceAddr, me->CurrSendSection, me->CurrSectionBytes, StartAddr);
}

static INT8U IMEU_upgrade_daemon_start(Secondary_Upgrade_info_t *me)
{
	if (MyTimeout(&handshakeTimer, mMs_X(100u)))
	{
		if (me->IsRetransmit)
		{
			return eIMEU_Upgrade_transmit;
		}
		me->StartSectionFunc(me);
        if (me->u8CommType == COMM_TYPE_485) {
            MyTimeout_stop(&handshakeTimer);
            MyTimeout_start(&QueryTimer);
        }
	}
	
	if (MyTimeout(&QueryTimer, me->u16QueryInterval))
	{
		me->QuerystatusFunc(me->CurrDecviceAddr);
        if (me->u8CommType == COMM_TYPE_485) {
            MyTimeout_stop(&QueryTimer);
            MyTimeout_start(&handshakeTimer);
        }
	}

	//IMEU status -- 6 start transmit
	if (me->u8CurrDeviceStatus == 6u)
	{
		return eIMEU_Upgrade_transmit;
	}

	return eIMEU_Upgrade_start;
}

static void UpgradeStartExit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_stop(&IMEU_upgrade_timer);
	MyTimeout_stop(&QueryTimer);
	MyTimeout_stop(&handshakeTimer);
}
/************************* IMEU2 upgrade deamon start *************************/

/************************* IMEU2 upgrade deamon transmit *************************/
static void IMEU_upgrade_daemon_transmit_init(Secondary_Upgrade_info_t *me)
{
	MyTimeout_start(&QueryTimer);
	me->CurrSendStep = 0u;
    me->u8CurrDeviceStatus = 0xFF;
	// co_trace1(3u, "<IMEU_Update> daemon transmit init, Device Addr is 0x%x", me->CurrDecviceAddr);
}

static INT8 IMEU_upgrade_daemon_transmit(Secondary_Upgrade_info_t *me)
{
	//if (me->SendByteNum >= me->TotalByteNum)
	if (me->CurrSendStep >= me->TotalFramePerSection)
	{
		return eIMEU_Upgrade_finish;
	}
	if (MyTimeout(&QueryTimer, me->u16TransmitInterval))
	{
        if (me->TransmitMessageFunc != NULL) {
            me->TransmitMessageFunc(me, g_u8TransmitBuf);
        }
	}

	return eIMEU_Upgrade_transmit;
}

static void UpgradeTransmitExit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_stop(&QueryTimer);
	me->CurrSendStep = 0u;
}
/************************* IMEU2 upgrade deamon transmit *************************/


/************************* IMEU2 upgrade deamon finish *************************/
static void IMEU_upgrade_daemon_finish_init(Secondary_Upgrade_info_t *me)
{
	MyTimeout_start(&QueryTimer);
	MyTimeout_start(&SendTimer);
    me->u8CurrDeviceStatus = 0xFF;
	// co_trace1(3u, "<IMEU_Update> daemon finish init, Device Addr is 0x%x", me->CurrDecviceAddr);
}

static INT8 UpgradeFinish(Secondary_Upgrade_info_t *me)
{
	//send one section finsish
	if (MyTimeout(&SendTimer, mMs_X(100u)))
	{
		me->EndOneSectionFunc(me);
        if (me->u8CommType == COMM_TYPE_485) {
            MyTimeout_stop(&SendTimer);
            MyTimeout_start(&QueryTimer);
        }
	}
	//query imeu upgrade status
	if (MyTimeout(&QueryTimer, me->u16QueryInterval))
	{
		me->QuerystatusFunc(me->CurrDecviceAddr);
        if (me->u8CommType == COMM_TYPE_485) {
            MyTimeout_stop(&QueryTimer);
            MyTimeout_start(&SendTimer);
        }
	}

	//IMEU status -- 10 write succeed / 18 wait backup
	if ((me->u8CurrDeviceStatus == 10u) || (me->u8CurrDeviceStatus == 18u))
	{
		if(me->u8RectUpdateSuccessWriteUDisk_Flag == 1)
		{
			me->u8RectUpdateSuccessWriteUDisk_Flag = 2;
		}
		me->IsRetransmit = FALSE;
		if (me->CurrSendSection >= me->TotalSectionNum-1)
		{
			// co_trace1(3u, "<IMEU_Update> daemon transmit, All Section(%d) Send succeed", me->TotalSectionNum);
			return eIMEU_Upgrade_end;
		}
		else
		{
			me->CurrSendSection++;
			// co_trace1(3u, "<IMEU_Update> daemon transmit, No.%d Section Send Finish, TotalNum is: %d", me->CurrSendSection, me->TotalSectionNum);
			return eIMEU_Upgrade_start;
		}
	}
	else if ((me->u8CurrDeviceStatus == 9u) || (me->u8CurrDeviceStatus == 11u))
	{
		// me->IsRetransmit = !FALSE;
		me->u8CurrUpgradeState = 3;
		me->u8UpgradeErrCode = me->u8CurrDeviceStatus;
		// co_trace1(3u, "<IMEU_Update> daemon transmit, Current section(No.%d) send failed, Retransmit", me->CurrSendSection+1);
		return eIMEU_Upgrade_start;
	}

	return eIMEU_Upgrade_finish;
}

static void UpgradeFinishExit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_stop(&SendTimer);
	MyTimeout_stop(&QueryTimer);
}
/************************* IMEU2 upgrade deamon finish *************************/

/************************* IMEU2 upgrade deamon end *************************/
static void UpgradeEndInit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_start(&QueryTimer);
    me->u8CurrDeviceStatus = 0xFF;
	// co_trace1(3u, "<IMEU_Update> daemon end init, Device Addr is 0x%x", me->CurrDecviceAddr);
}

static INT8U IMEU_upgrade_daemon_end(Secondary_Upgrade_info_t *me)
{
    static UINT8 s_u8Wait2Reboot = 0;
    if (MyTimeout(&QueryTimer, me->u16QueryInterval))
    {
		me->QuerystatusFunc(me->CurrDecviceAddr);
	}

	//waiting backup
	if (me->u8CurrDeviceStatus == 18u)
	{
        if (s_u8Wait2Reboot == 0) {
            MyTimeout_start(&reBootTimer);
            s_u8Wait2Reboot = 1;
        } else {
            if (MyTimeout(&reBootTimer, mSec_X(5U))) {
                s_u8Wait2Reboot = 0;
                MyTimeout_stop(&reBootTimer);
                return eIMEU_Upgrade_reboot;
            }
        }
    }

	return eIMEU_Upgrade_end;
}

static void UpgradeEndExit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_stop(&QueryTimer);
}
/************************* IMEU2 upgrade deamon end *************************/


/************************* IMEU2 upgrade deamon reboot *************************/
static void UpgradeRebootInit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_start(&QueryTimer);
	MyTimeout_start(&SendTimer);
    me->u8CurrDeviceStatus = 0xFF;
	// co_trace1(3u, "<IMEU_Update> daemon reboot init, Device Addr is 0x%x", me->CurrDecviceAddr);
}

static INT8 UpgradeReboot(Secondary_Upgrade_info_t *me)
{
    static UINT8 s_u8RebootCount = 0;
    if (me->u8CurrDeviceStatus == 0u)
    {
		// co_trace1(3u, "<IMEU_Update> Addr is: 0x%x IMEU2 was upgrade succeed!!!", me->CurrDecviceAddr);
#if 0
		if (me->CurrDecviceAddr == CFG_IMBTU1_ADDR_1)
		{
			me->CurrDecviceAddr = CFG_IMBTU1_ADDR_2;
			IMEU_GetVersion[0] = FALSE;
		}
		else if (me->CurrDecviceAddr == CFG_IMBTU1_ADDR_2)
		{
			me->CurrDecviceAddr = CFG_IMBTU1_ADDR_1;
			IMEU_GetVersion[1] = FALSE;
			TCU_info.OTAInfo.StartUpgrade &= ~CFG_NEED_UPGRADE_IMEU2;//upgrade IMEU2 succeed
			TCU_info.OTAInfo.ClearUpgrade |= CFG_NEED_UPGRADE_IMEU2;
			TCU_info.OTAInfo.OTASucceedFlag |= CFG_NEED_UPGRADE_IMEU2;
			MyTimeout_stop(&IMEU_upgrade_timeout);
		}
		return eIMEU_Upgrade_idle;
#endif
	}

	if (MyTimeout(&SendTimer, mSec_X(1u)))
	{
		if(s_u8RebootCount < 3)
		{
			me->RebootFunc(me->CurrDecviceAddr);
		}
        //me->RebootFunc(me->CurrDecviceAddr);
        MyTimeout_start(&QueryTimer);
        s_u8RebootCount++;
        if (s_u8RebootCount >= 10U) {
            s_u8RebootCount = 0;
            MyTimeout_stop(&SendTimer);
            MyTimeout_stop(&QueryTimer);
            me->u8UpgradeFlag = 0;
            return eIMEU_Upgrade_idle;
        }
    }
	
	if (MyTimeout(&QueryTimer, mMs_X(150u)))
	{
		// me->QuerystatusFunc(me->CurrDecviceAddr);
	}

	return eIMEU_Upgrade_reboot;
}

static void UpgradeRebootExit(Secondary_Upgrade_info_t *me)
{
	MyTimeout_stop(&QueryTimer);
	MyTimeout_stop(&SendTimer);
}
/************************* IMEU2 upgrade deamon reboot *************************/







/************************* IMEU2 upgrade machine *************************/
static int UpgradeMachineIdle(Secondary_Upgrade_info_t *me)
{
	static INT8 NewState = 0u;

	if (me->Machine.Step == eState_Idle)
	{
		UpgradeIdleInit(me);
		me->Machine.Step++;
	}

	if (me->Machine.Step == eState_Normal)
	{
		NewState = UpgradeIdle(me);
		if (NewState != me->Machine.TaskID)
		{
			me->Machine.Step++;
		}
	}

	if (me->Machine.Step == eState_Exit)
	{
		UpgradeIdleExit(me);
		me->Machine.TaskID	= NewState;
		NewState						= 0u;
		me->Machine.Step		= eState_Idle;
	}

	return me->Machine.TaskID;
}

static int UpgradeMachineJump(Secondary_Upgrade_info_t *me)
{
	static INT8 NewState = 0u;

	if (me->Machine.Step == eState_Idle)
	{
		UpgradeJumpInit(me);
		me->Machine.Step++;
	}

	if (me->Machine.Step == eState_Normal)
	{
		NewState = UpgradeJump(me);
		if (NewState != me->Machine.TaskID)
		{
			me->Machine.Step++;
		}
	}

	if (me->Machine.Step == eState_Exit)
	{
		UpgradeJumpExit(me);
		me->Machine.TaskID	= NewState;
		NewState						= 0u;
		me->Machine.Step		= eState_Idle;
	}

	return me->Machine.TaskID;
}

static int UpgradeMachineWaitingHandshake(Secondary_Upgrade_info_t *me)
{
	static INT8 NewState = 0u;

	if (me->Machine.Step == eState_Idle)
	{
		UpgradeWaitingHandshakeInit(me);
		me->Machine.Step++;
	}

	if (me->Machine.Step == eState_Normal)
	{
		NewState = UpgradeWaitingHandshake(me);
		if (NewState != me->Machine.TaskID)
		{
			me->Machine.Step++;
		}
	}

	if (me->Machine.Step == eState_Exit)
	{
		IMEU_upgrade_daemon_waiting_handshake_exit(me);
		me->Machine.TaskID	= NewState;
		NewState						= 0u;
		me->Machine.Step		= eState_Idle;
	}

	return me->Machine.TaskID;
}

static int UpgradeMachineStart(Secondary_Upgrade_info_t *me)
{
	static INT8 NewState = 0u;

	if (me->Machine.Step == eState_Idle)
	{
		UpgradeStartInit(me);
		me->Machine.Step++;
	}

	if (me->Machine.Step == eState_Normal)
	{
		NewState = IMEU_upgrade_daemon_start(me);
		if (NewState != me->Machine.TaskID)
		{
			me->Machine.Step++;
		}
	}

	if (me->Machine.Step == eState_Exit)
	{
		UpgradeStartExit(me);
		me->Machine.TaskID	= NewState;
		NewState						= 0u;
		me->Machine.Step		= eState_Idle;
	}

	return me->Machine.TaskID;
}

static int UpgradeMachineTransmit(Secondary_Upgrade_info_t *me)
{
	static INT8 NewState = 0u;

	if (me->Machine.Step == eState_Idle)
	{
		IMEU_upgrade_daemon_transmit_init(me);
		me->Machine.Step++;
	}

	if (me->Machine.Step == eState_Normal)
	{
		NewState = IMEU_upgrade_daemon_transmit(me);
		if (NewState != me->Machine.TaskID)
		{
			me->Machine.Step++;
		}
	}

	if (me->Machine.Step == eState_Exit)
	{
		UpgradeTransmitExit(me);
		me->Machine.TaskID	= NewState;
		NewState						= 0u;
		me->Machine.Step		= eState_Idle;
	}

	return me->Machine.TaskID;
}

static int UpgradeMachineFinish(Secondary_Upgrade_info_t *me)
{
	static INT8 NewState = 0u;

	if (me->Machine.Step == eState_Idle)
	{
		IMEU_upgrade_daemon_finish_init(me);
		me->Machine.Step++;
	}

	if (me->Machine.Step == eState_Normal)
	{
		NewState = UpgradeFinish(me);
		if (NewState != me->Machine.TaskID)
		{
			me->Machine.Step++;
		}
	}

	if (me->Machine.Step == eState_Exit)
	{
		UpgradeFinishExit(me);
		me->Machine.TaskID	= NewState;
		NewState						= 0u;
		me->Machine.Step		= eState_Idle;
	}

	return me->Machine.TaskID;
}

static int UpgradeMachineEnd(Secondary_Upgrade_info_t *me)
{
	static INT8 NewState = 0u;

	if (me->Machine.Step == eState_Idle)
	{
		UpgradeEndInit(me);
		me->Machine.Step++;
	}

	if (me->Machine.Step == eState_Normal)
	{
		NewState = IMEU_upgrade_daemon_end(me);
		if (NewState != me->Machine.TaskID)
		{
			me->Machine.Step++;
		}
	}

	if (me->Machine.Step == eState_Exit)
	{
		UpgradeEndExit(me);
		me->Machine.TaskID	= NewState;
		NewState						= 0u;
		me->Machine.Step		= eState_Idle;
	}

	return me->Machine.TaskID;
}

static int UpgradeMachineReboot(Secondary_Upgrade_info_t *me)
{
	static INT8 NewState = 0u;

	if (me->Machine.Step == eState_Idle)
	{
		UpgradeRebootInit(me);
		me->Machine.Step++;
	}

	if (me->Machine.Step == eState_Normal)
	{
		NewState = UpgradeReboot(me);
		if (NewState != me->Machine.TaskID)
		{
			me->Machine.Step++;
		}
	}

	if (me->Machine.Step == eState_Exit)
	{
		UpgradeRebootExit(me);
		me->Machine.TaskID	= NewState;
		NewState						= 0u;
		me->Machine.Step		= eState_Idle;
		me->u8CurrUpgradeState = 2;
		me->u8TotalUpgradeState = 2;
	}

	return me->Machine.TaskID;
}
/************************* IMEU2 upgrade machine *************************/

void SecondaryUpgradeMachine(Secondary_Upgrade_info_t *me)
{
    INT8 iTempPhase;
    iTempPhase = IMEU_Upgrade_MachineInfo[me->MachineState].fproc(me);
    me->MachineState = iTempPhase;
}

void SecondaryUnpackData(Secondary_Upgrade_info_t *me, UINT8 *pu8RxBuf, UINT16 u16Len)
{
	if (u16Len != 13U) 
	{
		return;
	} 
	else 
	{
		// 暂不校验CRC
		me->u8CurrDeviceStatus = pu8RxBuf[6];
	}
}

