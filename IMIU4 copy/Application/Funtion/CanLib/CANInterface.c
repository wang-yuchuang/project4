/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : CANInterface.c
** @Founder      : SBK326
** @Date         : 2019.11.1
** @Function     : CAN通信包用户接口处理
** @Instructions :
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "BSPTypeDef.h"
#include "CANTabOper.h"
#include "CANTxCycle.h"
#include "CANUser.h"
#include "CANInterface.h"
#include "SystemInfo.h"
#include "SystemPara.h"
#include "SystemDebug.h"
#include "BSPCAN.h"
#include "Insulation.h"
#include "network_layer/can_uds_network_layer.h"        /*DEFAULT_SESSION枚举定义*/
#include "services_layer/device_role/sid_internal_header.h"  /*cur_session_type外部变量声明*/
#include "DoControl.h"
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
t_CANDevTab gCanDevTable[CAN_DEVTAB_NUM] = {0};                     /*CAN通信设备表*/
t_CANTxRcvTab gCanTxRcvTable[CAN_TRTAB_NUM] = {0};                  /*CAN通信收发处理表*/
t_CANTxCycTab gCanTxCycTable[CAN_TX_ALL_NUM] = {0};                 /*CAN通信周期发送处理表*/

t_CANLibIndexMap gCanIndexMap = {0};                                /*CAN通信包主索引映射信息*/

/*升级静默功能: 其他设备升级静默计时器(10ms单位递减)*/
#define OTHER_UPG_SILENCE_TIMEOUT   500    /*超时500次=5秒(10ms周期)无其他设备UDS报文则退出静默*/
u16 g_OtherUpgSilenceTimer = 0;           /*其他设备升级静默剩余计时*/

extern u8 gCANBaseDef_3[];                                          /*通信包基本配置信息*/
extern u8 gCANTxCelDef_14[];                                        /*主动更新单体配置表*/
extern u8 gCANTxCelDef_14[];                                        /*主动更新单体配置表*/
extern u16 gGBmuGenPara_102[];                                      /*主控通用参数值*/
/*=================================================================================================
** @Static function declaration
**===============================================================================================*/

/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void BSPUserCfgInit(void)
** @Input     : void
** @Output    : void
** @Function  : CAN通信包所有初始化
** @The notes : 用户必须初始化调用
**===============================================================================================*/
void CANLibAllInit_API(void)
{
    u8 i = 0;

    /*通信口用户初始化*/
    CANCommuPortUserInit();
    /*系统信息初始化*/
    InitSystemInfo();

    /*通信包用户定义参数初始化*/
    
    gCanIndexMap.index90Type = CAN_DT_U16;
    gCanIndexMap.index91Type = CAN_DT_U16;
    gCanIndexMap.index92Type = CAN_DT_U16;
    gCanIndexMap.index93Type = CAN_DT_U16;
    gCanIndexMap.index94Type = CAN_DT_S8;
    gCanIndexMap.index95Type = CAN_DT_S8;

    gCanIndexMap.index100Type = CAN_DT_U16;
    gCanIndexMap.index101Type = CAN_DT_U16;
    gCanIndexMap.index102Type = CAN_DT_U16;
    gCanIndexMap.index103Type = CAN_DT_U16;
    gCanIndexMap.index104Type = CAN_DT_U8;

    /*通信包配置参数初始化*/
    //SystemInfo_1[DeviceAddr] = gGBmuGenPara_102[eBmuGenPara102_BmuDev];
    //SystemInfo_1[HardwareVersion] = 00;

    /*通信包设备表初始化*/
    CANDeviceTableInit();

    /*通信包收发通信表初始化*/
    if(gCanTxRcvTable != NULL)
    {
    	for(i = 0; i < gCANBaseDef_3[eCANRBas3_TRTabNum]; i++)
    	{
    		/*清空所有收发通信表*/
    		CANClrTxRcvTabInfo(i);
    	}
    }

    /*通信包周期发送表初始化*/
    CANTxCycleTabInit();
}

/*=================================================================================================
** @Name      : u8 CANRcvShieldDeal_Hook(u32 canID, u8 canNum)
** @Input     : canID:报文ID  canNum:CAN通道号
** @Output    : TURE:需屏蔽 FALSE:不需屏蔽
** @Function  : 接收到需屏蔽的报文用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
u8 CANRcvShieldDeal_Hook(u32 canID, u8 canNum)
{
    // /*广播报文可能不需要接收处理*/
	// if(0xff == ((canID >> 8) & 0xff))
	// {
	// 	/*同级BMU和从控信息不处理*/
	// 	if(/*((canID & 0xff) >= BMU_BAS_DEVADDR) && */((canID & 0xff) < 70))
	// 	{
	// 		return(TRUE);
	// 	}
	// }

	return(FALSE);
}

/*=================================================================================================
** @Name      : void CANRcvVerUserDeal_Hook(u8 saddr, u8 canNum)
** @Input     : saddr:源地址  canNum:CAN通道号
** @Output    : void
** @Function  : 接收到校时报文后用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
void CANRcvVerUserDeal_Hook(u8 saddr, u8 canNum)
{
    /*将时间信息更新到系统时钟*/
	//SysClockExtVerifyAPI();
}

/*=================================================================================================
** @Name      : void CANRcvBreUserDeal_Hook(u8 saddr, u8 canNum)
** @Input     : saddr:源地址  canNum:CAN通道号
** @Output    : void
** @Function  : 接收到心跳报文后用户处理
** @The notes : 用户需要时补充完善
**===============================================================================================*/
void CANRcvBreUserDeal_Hook(u8 saddr, u8 canNum)
{
    /*通信诊断计数*/
	//DiagRcvDevMsgAddAPI(saddr);
	//DiagRcvEcuRepCheckAPI(saddr);
}

/*=================================================================================================
** @Name      : u8 CANRcvRupUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : saddr:源地址  mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收到请求修改报文后用户处理
** @The notes : 用户需要时补充完善,对相应索引值进行处理
**===============================================================================================*/
u8 CANRcvRupUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
{
    /*通信诊断计数*/
	//DiagRcvDevMsgAddAPI(saddr);
	//DiagRcvEcuRepCheckAPI(saddr);

    /*非上层设备控制不执行*/
	if(saddr <= SystemInfo_1[DeviceAddr])
	{
		/*权限非法*/
		return(eCANERRREAS_AUTHERR);
	}

    /*执行请求修改命令*/
    if(2 == mIndex)                     /*写设备序列号*/
    {
    	/*执行写设备序列号处理*/
    	return(RcvCanDeviceSerialCfgDeal(sIndex, sIndexNum, canNum));
    }
    /*执行请求修改命令*/
    else if(22 == mIndex)               /*写参数批量配置值*/
    {
    	/*执行写参数批量配置写数据处理*/
    	//return(RcvCanDnloadParaWRDeal(sIndex, sIndexNum, canNum));
    }
    else if(50 == mIndex)               /*系统控制*/
    {
    	/*执行系统控制处理*/
    	return(RcvCanSysCtrlCmd(sIndex, sIndexNum, canNum));
    }
    /*执行请求修改命令*/
    else if(52 == mIndex)              
    {
        return(RcvCanGroupCalibrationDeal(sIndex, sIndexNum, canNum));/*执行校准命令处理*/
    	
    }
    /*执行请求修改命令*/
    else if(53 == mIndex)               /*权限密码*/
    {
    	/*执行权限密码处理*/
    	return(RcvCanGroupPasswordDeal(sIndex, sIndexNum, canNum));
    }
    else if(55 == mIndex)               /*系统控制*/
    {
    	/*执行系统控制处理*/
    	return(RcvCanGroupCtrlCmdDeal(saddr, sIndex, sIndexNum, canNum));
    }
    // else if(87 == mIndex)               /*用户控制*/
    // {
    	/*执行用户控制处理*/
    	//return(RcvCanUserCtrlCmdDeal(sIndex, sIndexNum, canNum));
    // }
    else if(60 == mIndex)               /*系统调试*/
    {
    	/*执行系统调试处理*/
    	return(RcvCanGroupDebugCmdDeal(sIndex, sIndexNum, canNum));
    }
    else if(107 == mIndex)              /*系统通用参数设置*/
    {
    	/*执行系统通用参数设置处理*/
    	return(RcvCanSystemGenParaCfgDeal(sIndex, sIndexNum, canNum));
    }


    return(eCANERRREAS_INDEXERR);
}

/*=================================================================================================
** @Name      : void CANRcvReqUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
** @Input     : saddr:源地址  mIndex:主索引号 sIndex:起始子索引号 sIndexNum:子索引数目 canNum:CAN通道号
** @Output    : void
** @Function  : 接收到请求查询报文后(查询回复前)用户处理
** @The notes : 用户需要时补充完善,更新相应索引值(不自动跟新的值)
**===============================================================================================*/
void CANRcvReqUserDeal_Hook(u8 saddr, u8 mIndex, u8 sIndex, u8 sIndexNum, u8 canNum)
{
    /*查询系统时钟当前时间*/
	if(0 == mIndex)
	{
	    /*更新系统时钟显示*/
		//SysClockDisplayAPI();
	}
    /*查询主控设备当前条码序列号*/
	else if(2 == mIndex)
	{
		if(0 == sIndex)
		{
			/*读设备序列号到显示区域*/
			//SysDevSerialDispayAPI();
		}
	}
}

/*=================================================================================================
** @Name      : void CANRcvCycUpUserDeal_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域 DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : 接收到周期更新报文后用户处理
** @The notes : 用户需要时补充完善,处理接收周期发送的长帧、单帧、单体和参数信息
**===============================================================================================*/
void CANRcvCycUpUserDeal_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
//	u8 saddr = 0;

//	saddr = ID & 0xff;

    /*通信诊断计数*/
	//DiagRcvDevMsgAddAPI(saddr);
	//DiagRcvEcuRepCheckAPI(saddr);
}

/*=================================================================================================
** @Name      : void CANSendMsgUser_Hook (u32 ID, u8 *pdata, u8 DLC, u8 canNum)
** @Input     : ID:CANID *pdata:CAN数据域 DLC:数据域长度 canNum:CAN通道号
** @Output    : void
** @Function  : CAN通信包发送报文处理
** @The notes : 用户必须补充完善
**===============================================================================================*/
void CANSendMsgUser_Hook(u32 ID, u8 *pdata, u8 DLC, u8 canNum)
{
	/*升级静默: 本机或其他设备升级时,阻断CanLib所有报文发送
	 *UDS响应(can_uds_arch.c)直接调BSPCANSendQin,不受影响*/
	if(TRUE == UpgSilence_IsActive())
	{
		return;
	}
	u8 i = 0;
	t_CANMSG msg;

	if(DLC > 64)
	{
		return;
	}

	msg.ID = ID;
	msg.Len = DLC;

	for(i = 0; i < DLC; i++)
	{
		msg.Data[i] = pdata[i];
	}

	//不满64byte则补0满64byte
	// msg.Len = 64;
	// for(i = DLC; i < 8; i++)
	// {
	// 	msg.Data[i] = 0;
	// }

    /*发送CAN报文*/
	BSPCANSendQin((e_CANNUM)canNum, &msg);
}

/*=================================================================================================
** @Name      : u8 CANTxCycUpUserEnState_Hook(e_CANFunCode funCode, u8 mIndex, u8 canNum)
** @Input     : funCode:功能码  mIndex:主索引号 canNum:CAN通道号
** @Output    : 用户允许发送状态 TRUE:允许 FALSE:禁止
** @Function  : 判断用户允许发送指定主索引报文状态
** @The notes : 用户需要时补充完善,对指定主索引在指定状态下才允许发送(比如调试信息)
**===============================================================================================*/
u8 CANTxCycUpUserEnState_Hook(e_CANFunCode funCode, u8 mIndex, u8 canNum)
{
	static u8 sDebugInfoTime = 10;

	/*调试信息(长帧)*/
	if((eCANFUN_UPM == funCode) && ((82 == mIndex) || (83 == mIndex)))
	{
		/*调试模式允许发送*/
		if(eMODE_Debug == SystemWorkState_50[WorkMode])
		{
			sDebugInfoTime = 10;
			return(TRUE);
		}
		/*非调试模式*/
		else
		{
			/*非调试模式允许发送完10次*/
			if(sDebugInfoTime > 0)
			{
				sDebugInfoTime--;
				return(TRUE);
			}
			/*非调试模式且已发送完10次发送*/
			else
			{
				return(FALSE);
			}
		}
	}

	return(TRUE);
}

/*=================================================================================================
** @Name      : t_CANMapTrans CANIndexMapTransUser_Hook(e_CANFunCode funCode, u8 mIndex, u8 DLC)
** @Input     : funCode:功能码  mIndex:映射主索引号 DLC:报文长度
** @Output    : void
** @Function  : CAN通信包将映射报文信息转换成真实信息
** @The notes : 用户必须补充完善,单体信息和参数信息报文的映射
**===============================================================================================*/
t_CANMapTrans CANIndexMapTransUser_Hook(e_CANFunCode funCode, u8 mIndex, u8 DLC)
{
    /*根据报文映射信息[功能码/主索引/报文长度]转换成真实索引信息*/
	t_CANMapTrans canMapInfo = {0};


    //周期主动更新参数报文
    if(eCANFUN_UPP == funCode)
    {
    	/*系统通用参数*/
    	if(mIndex < gCanIndexMap.map101Index)
    	{
    		// canMapInfo.realMIndex = 100;
    		// canMapInfo.realSIndex = mIndex * 4;
    		// canMapInfo.realSIndexNum = DLC / gCanIndexMap.index100Type;
    	}
    }
    //非周期主动更新帧报文
    else
    {
		canMapInfo.realMIndex = 0xff;
		canMapInfo.realSIndex = 0xff;
		canMapInfo.realSIndexNum = 0xff;
    }

    return(canMapInfo);
}

/*=================================================================================================
** @Name      : u8 RcvCanDeviceSerialCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收写设备序列号命令CAN报文处理
** @The notes : CAN通信包接收请求修改(2)处理调用
**===============================================================================================*/
u8 RcvCanDeviceSerialCfgDeal(u8 index, u8 len, u8 canNum)
{
    /*无CAN口控制权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin != GetSystemPermissionArray(PermissionLevel)) || (canNum != GetSystemPermissionArray(PermissionChannel)))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > SerialNum_len) || (index == SerialNum_len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    if(TRUE != RcvDeviceSerialParaCfgAPI())
	{
		return(eCANERRREAS_STATEERR);
	}
    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupCalibrationDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收执行校准命令CAN报文处理
** @The notes : CAN通信包接收请求修改(52)处理调用
**===============================================================================================*/
u8  RcvCanGroupCalibrationDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;
    /*控制命令索引错误*/
    if(((index + len) > SysCorrCmd_Len) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //采样校准类型
            case 0:
                    // CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
				break;

            //校准点基准值
            case 1:
            	/*执行基准电压或电流输入*/
            	//InputCalibrationInfoMsg(CalibrationCtrl_52[SysCorrValue]);
                break;

            default:
                break;
        }
    }
    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupPasswordDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收权限密码命令CAN报文处理
** @The notes : CAN通信包接收请求修改(53)处理调用
**===============================================================================================*/
u8 RcvCanGroupPasswordDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*控制命令索引错误*/
    if(((index + len) > Password_Len) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //权限密码高位
            case 0:
				break;

            //权限密码低位
            case 1:
            	/*执行密码检测*/
            	RcvSysAuthorityCmdAPI(eAuthL_Can);
            	//SendCanGroupAuthorityInfoMsg();
                break;

            default:
                break;
        }
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupCtrlCmdDeal(u8 saddr, u8 index, u8 len, u8 canNum)
** @Input     : saddr:命令源地址 index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收控制命令CAN报文处理
** @The notes : CAN通信包接收请求修改(55)处理调用
**===============================================================================================*/
u8 RcvCanGroupCtrlCmdDeal(u8 saddr, u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*仅在普通模式使用*/
    if((canNum >= eCANNUM)
        || (eAuthL_User  != GetSystemPermissionArray(PermissionLevel)))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*控制命令索引错误*/
    if(((index + len) > UserCtrlCmd_len) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        switch(i)
        {
            //绝缘检测控制
            case UserCtrl:
				gs_InsulationDetectMng.ucInsulationMode = UserCtrlCmd_55[UserCtrl];
                gs_InsulationDetectMng.ucInsulationState = eInsulation_Start;
                break;
            //继电器控制 //继电器通道选择 //继电器控制确认
            case DoCtrlState:
                if((1 == UserCtrlCmd_55[DoCtrlChannl])&& (1 == UserCtrlCmd_55[DoCtrlConfirm] ))
				{
                    gControl.DOStateSet[DO_1] = UserCtrlCmd_55[DoCtrlState];
                }else if((2 == UserCtrlCmd_55[DoCtrlChannl]) && (1 == UserCtrlCmd_55[DoCtrlConfirm] ))
                {
                    gControl.DOStateSet[DO_2] = UserCtrlCmd_55[DoCtrlState];
                }else if((0 == UserCtrlCmd_55[DoCtrlChannl]) && (1 == UserCtrlCmd_55[DoCtrlConfirm] ))
                {
                    gControl.DOStateSet[DO_1] = UserCtrlCmd_55[DoCtrlState];
                    gControl.DOStateSet[DO_2] = UserCtrlCmd_55[DoCtrlState];
                }
                break;

            default:
                break;
        }
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanGroupDebugCmdDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收电池组调试命令CAN报文处理
** @The notes : CAN通信包接收请求修改(88)处理调用
**===============================================================================================*/
u8 RcvCanGroupDebugCmdDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin  != GetSystemPermissionArray(PermissionLevel)))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*调试命令索引错误*/
    if(((index + len) > Debug_Len) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
    	//调试命令设置
        if(i < DebugComfirm)
        {
            RcvGroupDebugOperCmdAPI(i);
        }
        //调试确认
        else if(i == DebugComfirm)
        {
            RcvGroupDebugAffirmCmdAPI(canNum);
        }
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : u8 RcvCanSystemGenParaCfgDeal(u8 index, u8 len, u8 canNum)
** @Input     : index:子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收系统通用参数设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(105)处理调用
**===============================================================================================*/
u8 RcvCanSystemGenParaCfgDeal(u8 index, u8 len, u8 canNum)
{
    u8 i = 0;
    u8 reault = TRUE;

    /*无CAN口调试权限*/
    if((canNum >= eCANNUM)
        || (eAuthL_Admin  != GetSystemPermissionArray(PermissionLevel)))
    {
        /*权限未开启*/
    	return(eCANERRREAS_AUTHERR);
    }

    /*设置命令索引错误*/
    if(((index + len) > Parameter_Len) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    for(i = index; i < (index + len); i++)
    {
        /*写入参数处理*/
    	reault &= RcvSystemGenParaCfgAPI(i, canNum);
    }

    /*有写成功*/
    if(TRUE == reault)
    {
        /*请求备份FLASH*/
        RcvParaSetFlashSaveAPI();
    }
    /*有写失败*/
    if(FALSE == reault)
    {
        /*参数非法*/
    	return(eCANERRREAS_RANGEERR);
    }

    return(eCANERRREAS_NORMAL);
}


/*=================================================================================================
** @Name      : u8 RcvCanSysCtrlCmd(u8 index, u8 len, u8 canNum)
** @Input     : index:起始子索引号 len:控制索引长度 canNum:CAN口
** @Output    : 执行结果:0:正常 1:索引非法 2:CRC非法 3:权限非法 4:状态非法 5:参数非法
** @Function  : 接收系统工作模式设置命令CAN报文处理
** @The notes : CAN通信包接收请求修改(50)处理调用
**===============================================================================================*/
u8 RcvCanSysCtrlCmd(u8 index, u8 len, u8 canNum)
{
    u16 modeVal = 0;
    
    /*控制命令索引错误(使用WorkMode_Len而非DeviceInfo1_len)*/
    if(((index + len) > WorkMode_Len) || (0 == len))
    {
        return(eCANERRREAS_INDEXERR);
    }

    /*读取CAN写入后的工作模式值*/
    modeVal = SystemWorkState_50[WorkMode];

    /*验证工作模式值合法性*/
    if((eMODE_Normal != modeVal) && (eMODE_Tooling != modeVal) && (eMODE_Debug != modeVal))
    {
        /*参数非法,恢复为正常模式*/
        SystemWorkState_50[WorkMode] = eMODE_Normal;
        return(eCANERRREAS_RANGEERR);
    }

    /*模式合法,根据模式执行相应操作*/
    switch(modeVal)
    {
        case eMODE_Normal:
            /*正常模式:恢复权限和调试状态*/
            SystemPermission_54[PermissionLevel] = eAuthL_User;
            SystemPermission_54[PermissionChannel] = eAuthL_Reserved;
            break;

        case eMODE_Tooling:
            /*工装模式:保持当前权限*/
            break;

        case eMODE_Debug:
            /*调试模式:保持当前权限*/
            break;

        default:
            break;
    }

    return(eCANERRREAS_NORMAL);
}

/*=================================================================================================
** @Name      : void UpgSilence_RefreshOtherTimer(void)
** @Input     : void
** @Output    : void
** @Function  : 刷新其他设备升级静默计时器(收到其他设备UDS报文时调用)
** @The notes : 
**===============================================================================================*/
void UpgSilence_RefreshOtherTimer(void)
{
    g_OtherUpgSilenceTimer = OTHER_UPG_SILENCE_TIMEOUT;
}

/*=================================================================================================
** @Name      : void UpgSilence_TimerTick(void)
** @Input     : void
** @Output    : void
** @Function  : 升级静默计时器周期递减(10ms周期调用)
** @The notes : 放在CANLibHandleTask_API的10ms任务中调用
**===============================================================================================*/
void UpgSilence_TimerTick(void)
{
    if(g_OtherUpgSilenceTimer > 0)
    {
        g_OtherUpgSilenceTimer--;
    }
}

/*=================================================================================================
** @Name      : u8 UpgSilence_IsActive(void)
** @Input     : void
** @Output    : TRUE:升级静默激活 FALSE:正常发送
** @Function  : 查询升级静默状态是否激活
** @The notes : 本机升级(非默认会话)或其他设备升级(计时器未超时)时返回TRUE
**===============================================================================================*/
u8 UpgSilence_IsActive(void)
{
    /*本机升级: 非默认会话视为升级中*/
    if(cur_session_type != DEFAULT_SESSION)
    {
        return(TRUE);
    }
    /*其他设备升级: 计时器未超时*/
    if(g_OtherUpgSilenceTimer > 0)
    {
        return(TRUE);
    }
    return(FALSE);
}


/*=================================================================================================
** @Static function definition
**===============================================================================================*/
