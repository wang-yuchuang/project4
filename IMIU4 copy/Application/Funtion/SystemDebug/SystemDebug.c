/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : SystemDebug.c
** @Founder      : WYC834
** @Date         : 2025.12.10
** @Function     : 系统调试校准模块
** @Instructions : 系统自身调试校准
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "SystemDebug.h"
#include "SystemInfo.h"
#include "BSPCAN.h"
#include "BSPTypeDef.h"
#include "Measure.h"
#include "SystemData.h"
#include "BSPDIDO.h"
#include "DoControl.h"
#include "BSPKA84917_async.h"
#include "EEPROM.h"
#include "SystemDiag.h"
#include "SystemPara.h"
#include "EEPROM.h"
#include "Insulation.h"
#include "BSPTypeDef.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
u16 CalibrationCtrl_52[SysCorrCmd_Len];//校准控制指令
u16 AccessPassword_53[Password_Len];//权限密码
u16 SystemPermission_54[PermissionInfo_Len] = 
{
    eAuthL_User,//普通用户
    eAuthL_Reserved//默认可写端口
};
u16 CaliFeedback_58[Calibration_Len];//校准反馈信息
u16 SystemDebugCmd_60[Debug_Len];//系统调试命令
u16 DebugFeedback_61[DebugFeedback_Len];//系统调试反馈



static u16 sHVoltCorrType = 0;                              /*高压采样校准类型(按位表示各个通道校准类型[bit0:总压单点校准 bit1:总压双点校准 bit2:link单点校准 bit3:link双点校准 bit4:pe电压单点校准 bit5:pe电压双点校准][0未校准 1已校准])*/
static f32 sSumVoltMapK = 0;                                /*电池总压采样通道校准K值*/
static f32 sSumVoltMapB = 0;                                /*电池总压采样通道校准B值*/
static f32 sLinVoltMapK = 0;                                /*预充电压采样通道校准K值*/
static f32 sLinVoltMapB = 0;                                /*预充电压采样通道校准B值*/
static f32 sPegVoltMapK = 0;                                /*Link+电压采样通道校准K值*/
static f32 sPegVoltMapB = 0;                                /*Link+电压采样通道校准B值*/
//static u8 sCSampExpFlag = 0;            /*电流采样异常标志 bit0:霍尔5V异常 bit1:霍尔回检信号异常 bit2:模拟前端采样异常 bit3:霍尔ADC通道异常 bit4:霍尔AD值异常 bit5:分流器AD值异常 bit6:电流校零异常[0:正常 1:异常]*/
//static s32 sRealCurr10mA = 0;           /*实时瞬时电流采样值 10mA*/
//static u32 sRealVolt10uV = 0;           /*实时瞬时电压采样值 0.01mV*/

static s16 sCurrZeroCorr = 0;           /*电流零点修正值(10mA)*/
static u8 sCurrZeroFin = 0;             /*电流零点修正完成标志 1完成 2失败*/
// static s16 sHallZeroCorr = 0;           /*霍尔电流零点修正值(10mA)*/

/*同时使能霍尔和分流器时以分流器为主,霍尔作为参考单独记录*/
//static s32 sHallRealCurr10mA = 0;       /*霍尔实时瞬时电流采样值 10mA*/
//static s32 sHallRealVolt10uV = 0;       /*实时瞬时霍尔电压采样值 0.01


static u8 sCurrCorrType = 0;                                /*电流采样校准类型(按位表示各个通道校准类型(bit0:霍尔电流单点校准 bit1:霍尔电流双点校准  bit2:分流器充电电流单点校准 bit3:分流器充电电流双点校准  bit4:分流器放电电流单点校准 bit5:分流器放电电流双点校准[0未校准 1校准])*/
static f32 sHalOutCMapK = 0;                                /*霍尔输出电流采样通道校准K值*/
static f32 sHalOutCMapB = 0;                                /*霍尔输出电流采样通道校准B值*/
static f32 sShuChgBMapK = 0;                                /*电池端分流器充电电流采样通道校准K值*/
static f32 sShuChgBMapB = 0;                                /*电池端分流器充电电流采样通道校准B值*/
static f32 sShuDhgBMapK = 0;                                /*电池端分流器放电电流采样通道校准K值*/
static f32 sShuDhgBMapB = 0;                                /*电池端分流器放电电流采样通道校准B值*/

static f32 sShuChgLMapK = 0;                                /*负载端分流器充电电流采样通道校准K值*/
static f32 sShuChgLMapB = 0;                                /*负载端分流器充电电流采样通道校准B值*/
static f32 sShuDhgLMapK = 0;                                /*负载端分流器放电电流采样通道校准K值*/
static f32 sShuDhgLMapB = 0;                                /*负载端分流器放电电流采样通道校准B值*/
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void InPutSystemPermissionArray (u8 index ,u16 data);
** @Input     : index:系统权限信息下标 data:数据
** @Output    : void
** @Function  : 输入系统权限信息
** @The notes :
**===========================================================================================*/
void InPutSystemPermissionArray (u8 index ,u16 data)
{
    SystemPermission_54[index] = data;
}

/*=============================================================================================
** @Name      : u16 GetSystemPermissionArray (u8 index);
** @Input     : index:系统权限信息下标
** @Output    : 系统权限信息
** @Function  : 获取系统权限信息
** @The notes :
**===========================================================================================*/
u16 GetSystemPermissionArray (u8 index)
{
    return SystemPermission_54[index];
}

/*=================================================================================================
** @Name      : void AuthorityInit(void)
** @Input     : void
** @Output    : void
** @Function  : 管理权限初始化
** @The notes : 默认权限处理
**===============================================================================================*/
void AuthorityInit(void)
{
	/*普通用户权限*/
	SystemPermission_54[PermissionLevel] = eAuthL_User;
    SystemPermission_54[PermissionChannel] = eAuthL_Reserved;

    /*系统工作模式*/
    SystemWorkState_50[WorkMode] = eMODE_Normal;//普通模式
}

/*=================================================================================================
** @Name      : void RcvSysAuthorityCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 系统接收切换管理权限命令处理函数
** @The notes : CAN口通信口命令,按钮和内部口默认最高权限不需切换
**===============================================================================================*/
void RcvSysAuthorityCmdAPI(u8 port)
{
	/*密码正确*/
	if((PASSWORDH == AccessPassword_53[PasswordH]) && (PASSWORDL == AccessPassword_53[PasswordL]))
	{
		/*通信所在口为默认权限口则切换超级用户*/
        SystemPermission_54[PermissionChannel] = port;
        SystemPermission_54[PermissionLevel] = eAuthL_Admin;
	}
    else 
    {
        SystemPermission_54[PermissionChannel] = eAuthL_Reserved;
        SystemPermission_54[PermissionLevel] = eAuthL_User;
    }
}

/*=================================================================================================
** @Name      : void RcvSysCalibrationCmdAPI(u16 cmd, u8 port)
** @Input     : cmd:校准指令 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 系统接收校准命令处理函数
** @The notes : 通信口命令,按钮和内部口默认最高权限不需切换
**===============================================================================================*/
void RcvSysCalibrationCmdAPI(u16 cmd, u8 port)
{

    

    

}



/*=================================================================================================
** @Name      : void RcvSysCalibrationCmdAPI(u16 cmd, u8 port)
** @Input     : cmd:校准指令 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : void
** @Function  : 系统接收校准命令处理函数
** @The notes : 通信口命令,按钮和内部口默认最高权限不需切换
**===============================================================================================*/
void InputCalibrationInfoMsg(u16 cmd)
{

}








/*=================================================================================================
** @Name      : void HigVoltSampCorrProcess(void)
** @Input     : void
** @Output    : void
** @Function  : 高压校准处理过程
** @The notes : 1s
**===============================================================================================*/
void HigVoltSampCorrProcess(void)
{
    u8 j = 0;
    u8 step = 0;
    f32 tempV = 0;
    f32 calcK = 0;
    f32 calcB = 0;
    static u16 sErrTime = 0;
    static u16 sFilTime = 0;
    static f32 sLowStandV = 0;
    static f32 sHighStandV = 0;
    static s32 sFilSumVolt = {0};
    static f32 sLowVolt[HV_CORR_FIL_T] = {0};
    static f32 sHighVolt[HV_CORR_FIL_T] = {0};

    //非调试模式不校准
    if(eMODE_Debug != GetSystemWorkModeArray(WorkMode))
    {
        //校准数据清零
        CaliFeedback_58[CalibrationType] = eCORR_INVALID;
        CaliFeedback_58[CalibrationState] = 0;
    }
    //接收停止命令则失败
    else if(0xff == CalibrationCtrl_52[SysCorrCmdType])                       //终止命令
    {
        if((eCORR_SUMVOLS == CaliFeedback_58[CalibrationType])        /*正在总压单点校准*/
            || (eCORR_SUMVOLD == CaliFeedback_58[CalibrationType])    /*正在总压双点校准*/
            || (eCORR_PEGVOLS == CaliFeedback_58[CalibrationType])    /*正在PE电压单点校准*/
            || (eCORR_PEGVOLD == CaliFeedback_58[CalibrationType])    /*正在PE电压双点校准*/
    		|| (eCORR_LINVOLS == CaliFeedback_58[CalibrationType])    /*正在Link单点校准*/
    		|| (eCORR_LINVOLD == CaliFeedback_58[CalibrationType]))   /*正在Link双点校准*/
        {
            //处于校准中途则校准失败
            if((0 != CaliFeedback_58[CalibrationState])
                && (100 != CaliFeedback_58[CalibrationState]))
            {
                CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;

                CaliFeedback_58[CalibrationState] = 101;
            }
        }
    }
    //正在电池总压单点校准接收其他校准命令
    else if((eCORR_SUMVOLS == CaliFeedback_58[CalibrationType])       //正在总压单点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_SUMVOLS != CalibrationCtrl_52[SysCorrCmdType]))             //非总压单点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在电池总压双点校准接收其他校准命令
    else if((eCORR_SUMVOLD == CaliFeedback_58[CalibrationType])       //正在总压双点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_SUMVOLD != CalibrationCtrl_52[SysCorrCmdType]))             //非总压双点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在预充电压单点校准接收其他校准命令
    else if((eCORR_PEGVOLS == CaliFeedback_58[CalibrationType])       //正在PE电压单点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_PEGVOLS != CalibrationCtrl_52[SysCorrCmdType]))             //非预充电压单点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在预充电压双点校准接收其他校准命令
    else if((eCORR_PEGVOLD == CaliFeedback_58[CalibrationType])       //正在PE电压双点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_PEGVOLD != CalibrationCtrl_52[SysCorrCmdType]))             //非PE电压双点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在Link电压单点校准接收其他校准命令
    else if((eCORR_LINVOLS == CaliFeedback_58[CalibrationType])       //正在Link电压单点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_LINVOLS != CalibrationCtrl_52[SysCorrCmdType]))             //非Link电压单点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在Link电压双点校准接收其他校准命令
    else if((eCORR_LINVOLD == CaliFeedback_58[CalibrationType])       //正在Link电压双点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_LINVOLD != CalibrationCtrl_52[SysCorrCmdType]))             //非Link电压双点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    /*命令正确*/
    else
    {
    	/*正在高压校准*/
		if((CalibrationCtrl_52[SysCorrCmdType] >= eCORR_SUMVOLS)
            && (CalibrationCtrl_52[SysCorrCmdType] <= eCORR_LINVOLD))
		{
	    	/*当前高压校准状态为步骤*/
			step = CaliFeedback_58[CalibrationState];
		}
    	/*非高压校准*/
		else
		{
			/*保持在第一步*/
			step = 0;
		}
    }

    //根据校准状态检测信息
    switch(step)
    {
        //未校准
        case UnCalib:
            /*更新高压校准结果*/
        	UpdateHVoltCorrResult();

            //下发开始校准高压命令(总压/PE/Link+-单点或双点校准)
            if((CalibrationCtrl_52[SysCorrCmdType] >= eCORR_SUMVOLS)
            	&& (CalibrationCtrl_52[SysCorrCmdType] <= eCORR_LINVOLD))
            {
                CaliFeedback_58[CalibrationState] = 1;
                CaliFeedback_58[CalibrationType] = CalibrationCtrl_52[SysCorrCmdType];
                BitClr(CaliFeedback_58[CalibrationResult], CalibrationCtrl_52[SysCorrCmdType]);
            }
            break;

        //清除校准参数
        case DelCalibParam:
            /*电池总压校准*/
        	if((eCORR_SUMVOLS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_SUMVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//复位KB参数
                BitClr(sHVoltCorrType, eHVCorr_SumVS);
                BitClr(sHVoltCorrType, eHVCorr_SumVD);
                ClrEEPHVoltCurrKBInfo(0);

                sSumVoltMapK = HV_CORR_BLOWUP;
                sSumVoltMapB = 0;
        	}
            /*LINK电压校准*/
        	else if((eCORR_LINVOLS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_LINVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                //复位LINK电压KB参数
                BitClr(sHVoltCorrType, eHVCorr_LinVS);
                BitClr(sHVoltCorrType, eHVCorr_LinVD);
                ClrEEPHVoltCurrKBInfo(2);

                sLinVoltMapK = HV_CORR_BLOWUP;
                sLinVoltMapB = 0;
        	}
            /*PE电压校准*/
        	else if((eCORR_PEGVOLS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_PEGVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//复位KB参数
				BitClr(sHVoltCorrType, eHVCorr_PegVS);
                BitClr(sHVoltCorrType, eHVCorr_PegVD);
                ClrEEPHVoltCurrKBInfo(1);

                sPegVoltMapK = HV_CORR_BLOWUP;
                sPegVoltMapB = 0;

        	}

            for(j = 0; j < HV_CORR_FIL_T; j++)
            {
                sLowVolt[j] = 0;
                sHighVolt[j] = 0;
            }

            sErrTime = 0;
            sFilTime = 0;
            CalibrationCtrl_52[SysCorrValue] = 0;
            CaliFeedback_58[CalibrationState] = 2;
            break;

        //等待第一个基准高压
        case WaitFirstBaseV:
            //下发基准高压
            if(0 != CalibrationCtrl_52[SysCorrValue])
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sLowStandV = 0;
                    CaliFeedback_58[CalibrationState] = 3;
                }
            }
            break;

        //采集第1个基准电压
        case SampleFirstV:
            //记录基准电压(0.1V转1V) D9 CAN通讯传输电压值
            sLowStandV = (f32)CalibrationCtrl_52[SysCorrValue]/10;

            /*电池总压校准*/
        	if((eCORR_SUMVOLS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_SUMVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取电池总压采样值，1V*/
        		tempV = GetBatVolt();
        	}
            /*LINK电压校准*/
        	else if((eCORR_LINVOLS == CalibrationCtrl_52[SysCorrCmdType])
                || (eCORR_LINVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取LINK电压采样值，1V*/
        		tempV = GetLinkVolt();
        	}
            /*PE电压校准*/
        	else if((eCORR_PEGVOLS == CalibrationCtrl_52[SysCorrCmdType])
                || (eCORR_PEGVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取PE电压采样值，1V*/
        		tempV = GetPeGndVolt();
        	}

            //误差20V内(或10%V内)
            if((ABS(tempV, sLowStandV) < 20)
                || (ABS(tempV, sLowStandV) < (sLowStandV / 10)))
            {
               sLowVolt[sFilTime] = (f32)tempV;
            }
            //采样误差过大或模拟电池输出误差过大
            else
            {
                //等待3s
                sErrTime++;
                if(sErrTime >= 3)
                {
                    //等待超时则校准失败
                    sErrTime = 0;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                return;
            }

            sFilTime++;
            sErrTime = 0;

            //达到滤波采样次数则求平均值并进入下一步
            if(sFilTime >= HV_CORR_FIL_T)
            {
                //计算采样通道平均值
                sFilSumVolt = 0;

                for(j = 0; j < HV_CORR_FIL_T; j++)
                {
                    sFilSumVolt += sLowVolt[j];
                }

                sLowVolt[0] =  sFilSumVolt / HV_CORR_FIL_T;

                sFilTime = 0;

                //判断是单点校准还是双点校准
                if((eCORR_SUMVOLS == CaliFeedback_58[CalibrationType])
                    || (eCORR_PEGVOLS == CaliFeedback_58[CalibrationType])
	                || (eCORR_LINVOLS == CaliFeedback_58[CalibrationType]))
                {
                    //单点校准进入KB值计算
                    CalibrationCtrl_52[SysCorrValue] = 0;
                    CaliFeedback_58[CalibrationState] = 6;
                }
                else
                {
                    //双点校准进入第2点采集
                    CalibrationCtrl_52[SysCorrValue] = 0;
                    CaliFeedback_58[CalibrationState] = 4;
                }
            }
            break;

        //等待第2个基准电压
        case WaitSecondBaseV:
            //下发基准电压(必须比第1个基准点电压大)
            if((0 != CalibrationCtrl_52[SysCorrValue]) && (((f32)CalibrationCtrl_52[SysCorrValue]/10) > sLowStandV))
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sHighStandV = 0;
                    CaliFeedback_58[CalibrationState] = 5;
                }
            }
            break;

        //采集第2个基准电压
        case SampleSecondV:
            //记录基准电压(0.1V转1V)
            sHighStandV = (f32)CalibrationCtrl_52[SysCorrValue]/10;

            /*电池总压校准*/
        	if((eCORR_SUMVOLS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_SUMVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取电池总压采样值,1V*/
        		tempV = GetBatVolt();
        	}
            /*LINK电压校准*/
        	else if((eCORR_LINVOLS == CalibrationCtrl_52[SysCorrCmdType])
                || (eCORR_LINVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取LINK电压采样值,1V*/
        		tempV = GetLinkVolt();
        	}
            /*PE电压校准*/
        	else if((eCORR_PEGVOLS == CalibrationCtrl_52[SysCorrCmdType])
                || (eCORR_PEGVOLD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取PE电压采样值,1V*/
        		tempV = GetPeGndVolt();
        	}

            //误差20V内(或10%V内)
            if((ABS(tempV, sHighStandV) < 20)
                || (ABS(tempV, sHighStandV) < (sHighStandV / 10)))
            {
                sHighVolt[sFilTime] =(f32)tempV;
            }
            //采样误差过大或模拟电池输出误差过大
            else
            {
                //等待3s
                sErrTime++;
                if(sErrTime >= 3)
                {
                    //等待超时则校准失败
                    sErrTime = 0;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                return;
            }

            sFilTime++;
            sErrTime = 0;

            //达到滤波采样次数则求平均值并进入下一步
            if(sFilTime >= HV_CORR_FIL_T)
            {
                //计算每个采样通道平均值
                sFilSumVolt = 0;

                for(j = 0; j < HV_CORR_FIL_T; j++)
                {
                    sFilSumVolt += sHighVolt[j];
                }

                sHighVolt[0] =  sFilSumVolt / HV_CORR_FIL_T;

                sFilTime = 0;

                //进入KB值计算
                CalibrationCtrl_52[SysCorrValue] = 0;
                CaliFeedback_58[CalibrationState] = 6;
            }
            break;

        //校准计算处理
        case CalcCalibParam:
            //判断是单点校准还是双点校准
            if((eCORR_SUMVOLS == CaliFeedback_58[CalibrationType])
               	|| (eCORR_PEGVOLS == CaliFeedback_58[CalibrationType])
	            || (eCORR_LINVOLS == CaliFeedback_58[CalibrationType]))
            {
                sHighStandV = sLowStandV;
                calcK = HV_CORR_BLOWUP;

                calcB = ((f32)sLowStandV - sLowVolt[0]) * HV_CORR_BLOWUP;
            }
            else
            {
                //防止两次基准值一致除零
                if (FABS(sHighVolt[0] - sLowVolt[0]) < 0.1f)
                {
                    CaliFeedback_58[CalibrationState] = 101;
                    break;
                }
                
                calcK = (((f32)sHighStandV - (f32)sLowStandV) * HV_CORR_BLOWUP) / (((f32)sHighVolt[0] - (f32)sLowVolt[0]));
                //calcB = ((((s32)sHighVolt[0] * sLowStandV * 1000) - ((s32)sLowVolt[0] * sHighStandV * 1000))/((s32)sHighVolt[0] - (s32)sLowVolt[0]));
                calcB = (f32)sHighStandV * HV_CORR_BLOWUP - (f32)sHighVolt[0] * calcK;
            }

            /*电池总压校准*/
        	if((eCORR_SUMVOLS == CaliFeedback_58[CalibrationType])
        		|| (eCORR_SUMVOLD == CaliFeedback_58[CalibrationType]))
        	{
        		//检测电池总压采样校准是否正确
        	    tempV = (f32)(GetBatVolt() * calcK  + calcB) / HV_CORR_BLOWUP;

                //误差超过5V
                if(ABS(sHighStandV, tempV) > 5)
                {
                    //校准失败;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                //校准正确
                else
                {
                    //记录电池总压校准KB值
                    sSumVoltMapK = calcK;
                    sSumVoltMapB = calcB;
                    RcdEEPHVoltCurrKBInfo(0);

                    /*单点校准*/
                    if(eCORR_SUMVOLS == CalibrationCtrl_52[SysCorrCmdType])
                    {
                        BitSet(sHVoltCorrType, eHVCorr_SumVS);
                    }
                    /*双点校准*/
                    else
                    {
                        BitSet(sHVoltCorrType, eHVCorr_SumVD);
                    }

                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 100;
                }
        	}
            /*LINK电压校准*/
        	else if((eCORR_LINVOLS == CaliFeedback_58[CalibrationType])
                || (eCORR_LINVOLD == CaliFeedback_58[CalibrationType]))
        	{
        		//检测预充电压采样校准是否正确
        	    tempV = (f32)(GetLinkVolt()* calcK  + calcB)/ HV_CORR_BLOWUP;

                //误差超过5V
                if(ABS(sHighStandV, tempV) > 5)
                {
                    //校准失败;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                //校准正确
                else
                {
                    //记录预充电压校准KB值
                    sLinVoltMapK = calcK;
                    sLinVoltMapB = calcB;
                    RcdEEPHVoltCurrKBInfo(2);

                    /*单点校准*/
                    if(eCORR_LINVOLS == CaliFeedback_58[CalibrationType])
                    {
                        BitSet(sHVoltCorrType, eHVCorr_LinVS);
                    }
                    /*双点校准*/
                    else
                    {
                        BitSet(sHVoltCorrType, eHVCorr_LinVD);
                    }
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 100;
                }
        	}
            /*PE电压校准*/
        	else
        	{
                tempV = (f32)(GetPeGndVolt()* calcK  + calcB)/ HV_CORR_BLOWUP;
                if(ABS(sHighStandV, tempV) > 5)
                {
                    //校准失败;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                //校准正确
                else
                {
                    //记录Link-电压校准KB值
                    sPegVoltMapK = calcK;
                    sPegVoltMapB = calcB;
                    RcdEEPHVoltCurrKBInfo(1);

                    /*单点校准*/
                    if(eCORR_PEGVOLS == CalibrationCtrl_52[SysCorrCmdType])
                    {
                        BitSet(sHVoltCorrType, eHVCorr_PegVS);
                    }
                    /*双点校准*/
                    else
                    {
                        BitSet(sHVoltCorrType, eHVCorr_PegVD);
                    }

                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 100;
                }       		
        	}
            break;

        //校准成功
        case CalibSuccess:
        //校准失败
        case CalibFail:
            /*更新高压校准结果*/
        	UpdateHVoltCorrResult();

            //下发开始高压校准命令
            if((eCORR_SUMVOLS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_SUMVOLD == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_PEGVOLS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_PEGVOLD == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_LINVOLS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_LINVOLD == CalibrationCtrl_52[SysCorrCmdType]))
            {
                CaliFeedback_58[CalibrationType] = CalibrationCtrl_52[SysCorrCmdType];
                CaliFeedback_58[CalibrationState] = 1;
            }
            break;

        default:
            break;
    }
    
}

/*=================================================================================================
** @Name      : void UpdateHVoltCorrResult(void)
** @Input     : void
** @Output    : void
** @Function  : 更新高压校准结果
** @The notes : 根据校准标志位
**===============================================================================================*/
void UpdateHVoltCorrResult(void)
{
    /*电池总压已单点校准*/
	if(1 == BitGet(sHVoltCorrType, eHVCorr_SumVS))
	{
		BitSet(CaliFeedback_58[CalibrationResult], eCORR_SUMVOLS);
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], eCORR_SUMVOLS);
	}

    /*电池总压已双点校准*/
	if(1 == BitGet(sHVoltCorrType, eHVCorr_SumVD))
	{
		BitSet(CaliFeedback_58[CalibrationResult], eCORR_SUMVOLD);
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], eCORR_SUMVOLD);
	}

    /*PE电压已单点校准*/
	if((1 == BitGet(sHVoltCorrType, eHVCorr_PegVS)))
	{
		BitSet(CaliFeedback_58[CalibrationResult], eCORR_PEGVOLS);
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], eCORR_PEGVOLS);
	}

    /*PE电压已双点校准*/
	if((1 == BitGet(sHVoltCorrType, eHVCorr_PegVD)))
	{
		BitSet(CaliFeedback_58[CalibrationResult], eCORR_PEGVOLD);
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], eCORR_PEGVOLD);
	}

    /*Link电压已单点校准*/
	if((1 == BitGet(sHVoltCorrType, eHVCorr_LinVS)))
	{
		BitSet(CaliFeedback_58[CalibrationResult], eCORR_LINVOLS);
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], eCORR_LINVOLS);
	}

    /*Link电压已双点校准*/
	if((1 == BitGet(sHVoltCorrType, eHVCorr_LinVD)))
	{
		BitSet(CaliFeedback_58[CalibrationResult], eCORR_LINVOLD);
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], eCORR_LINVOLD);
	}
}

/*=================================================================================================
** @Name      : void OutCurrSampCorrProcess(void)
** @Input     : void
** @Output    : void
** @Function  : 电流校准处理过程
** @The notes : 1s
**===============================================================================================*/
void OutCurrSampCorrProcess(void)
{
    u8 j = 0;
    u8 step = 0;
    f32 tempC = 0;
    f32 calcK = 0;
    f32 calcB = 0;
    // static u8 sCorrSec = 0;
    static u16 sErrTime = 0;
    static u16 sFilTime = 0;
    static f32 sLowStandC = 0;
    static f32 sHighStandC = 0;
    static f32 sFilSumCurr = {0};
    static f32 sLowCurr[OC_CORR_FIL_T] = {0};
    static f32 sHighCurr[OC_CORR_FIL_T] = {0};

    //非调试模式不校准
    if(eMODE_Debug != GetSystemWorkModeArray(WorkMode))
    {
        //校准数据清零
        CaliFeedback_58[CalibrationType] = eCORR_INVALID;
        CaliFeedback_58[CalibrationState] = 0;
        return;
    }

    /*手动校零*/
    if(eCORR_HANDZRO ==  CalibrationCtrl_52[SysCorrCmdType])
    {
    	if((eCORR_HANDZRO != CaliFeedback_58[CalibrationType])
    		|| (1 != CaliFeedback_58[CalibrationState]))
    	{
			/*清除校零标志重新校零*/
			ClrCurrZeroFinFlagAPI();
			CaliFeedback_58[CalibrationType] = eCORR_HANDZRO;
			CaliFeedback_58[CalibrationState] = 1;
    	}

        /*校零成功*/
    	if(1 == SampGetCurrZeroFinFlagAPI())
    	{
			BitSet(CaliFeedback_58[CalibrationResult], eCORR_HANDZRO);
			CaliFeedback_58[CalibrationState] = 100;
			CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
        }
        /*校零失败*/
    	else if(2 == SampGetCurrZeroFinFlagAPI())
		{
			CaliFeedback_58[CalibrationState] = 101;
		}
    }


    //接收停止命令则失败
    else if(0xff == CalibrationCtrl_52[SysCorrCmdType])                       //终止命令
    {
        if((eCORR_HALOUTS == CaliFeedback_58[CalibrationType])        /*正在霍尔输出电流单点校准*/
            || (eCORR_HALOUTD == CaliFeedback_58[CalibrationType])    /*正在霍尔输出电流双点校准*/
            || (eCORR_BSHUCHS == CaliFeedback_58[CalibrationType])    /*正在分流器充电电流单点校准*/
            || (eCORR_BSHUCHD == CaliFeedback_58[CalibrationType])    /*正在分流器充电电流双点校准*/
            || (eCORR_BSHUDHS == CaliFeedback_58[CalibrationType])    /*正在分流器放电电流单点校准*/
            || (eCORR_BSHUDHD == CaliFeedback_58[CalibrationType])    /*正在分流器放电电流双点校准*/
            || (eCORR_LSHUCHS == CaliFeedback_58[CalibrationType])    /*正在分流器充电电流单点校准*/
            || (eCORR_LSHUCHD == CaliFeedback_58[CalibrationType])    /*正在分流器充电电流双点校准*/
            || (eCORR_LSHUDHS == CaliFeedback_58[CalibrationType])    /*正在分流器放电电流单点校准*/
            || (eCORR_LSHUDHD == CaliFeedback_58[CalibrationType])    /*正在分流器放电电流双点校准*/
            )  
        {
            //处于校准中途则校准失败
            if((0 != CaliFeedback_58[CalibrationState])
                && (100 != CaliFeedback_58[CalibrationState]))
            {
                CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;

                CaliFeedback_58[CalibrationState] = 101;
            }
        }
    }
    //正在霍尔输出电流单点校准接收其他校准命令
    else if((eCORR_HALOUTS == CaliFeedback_58[CalibrationType])       //正在霍尔输出电流单点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_HALOUTS != CalibrationCtrl_52[SysCorrCmdType]))             //非霍尔输出电流单点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在霍尔放电电流双点校准接收其他校准命令
    else if((eCORR_HALOUTD == CaliFeedback_58[CalibrationType])       //正在霍尔放电电流双点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_HALOUTD != CalibrationCtrl_52[SysCorrCmdType]))             //非霍尔放电电流双点校准命令
    {
        //校准失败
       CaliFeedback_58[CalibrationState] = 101;
    }
    //正在分流器充电电流单点校准接收其他校准命令
    else if((eCORR_BSHUCHS == CaliFeedback_58[CalibrationType])       //正在BAT分流器充电电流单点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_BSHUCHS != CalibrationCtrl_52[SysCorrCmdType]))             //非分流器充电电流单点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在分流器充电电流双点校准接收其他校准命令
    else if((eCORR_BSHUCHD == CaliFeedback_58[CalibrationType])       //正在BAT分流器充电电流双点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_BSHUCHD != CalibrationCtrl_52[SysCorrCmdType]))             //非分流器充电电流双点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在分流器放电电流单点校准接收其他校准命令
    else if((eCORR_BSHUDHS == CaliFeedback_58[CalibrationType])       //正在BAT分流器放电电流单点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_BSHUDHS != CalibrationCtrl_52[SysCorrCmdType]))             //非分流器放电电流单点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在分流器放电电流双点校准接收其他校准命令
    else if((eCORR_BSHUDHD == CaliFeedback_58[CalibrationType])       //正在BAT分流器放电电流双点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_BSHUDHD != CalibrationCtrl_52[SysCorrCmdType]))             //非分流器放电电流双点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在分流器充电电流单点校准接收其他校准命令
    else if((eCORR_LSHUCHS == CaliFeedback_58[CalibrationType])       //正在LINK分流器充电电流单点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_LSHUCHS != CalibrationCtrl_52[SysCorrCmdType]))             //非分流器充电电流单点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在分流器充电电流双点校准接收其他校准命令
    else if((eCORR_LSHUCHD == CaliFeedback_58[CalibrationType])       //正在LINK分流器充电电流双点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_LSHUCHD != CalibrationCtrl_52[SysCorrCmdType]))             //非分流器充电电流双点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在分流器放电电流单点校准接收其他校准命令
    else if((eCORR_LSHUDHS == CaliFeedback_58[CalibrationType])       //正在LINK分流器放电电流单点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_LSHUDHS != CalibrationCtrl_52[SysCorrCmdType]))             //非分流器放电电流单点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    //正在分流器放电电流双点校准接收其他校准命令
    else if((eCORR_LSHUDHD == CaliFeedback_58[CalibrationType])       //正在LINK分流器放电电流双点校准
        && (CaliFeedback_58[CalibrationState] > 0)                    //已开始校准
        && (CaliFeedback_58[CalibrationState] < 100)                  //未完成校准
        && (eCORR_LSHUDHD != CalibrationCtrl_52[SysCorrCmdType]))             //非分流器放电电流双点校准命令
    {
        //校准失败
        CaliFeedback_58[CalibrationState] = 101;
    }
    /*命令正确*/
    else
    {
    	/*正在电流校准*/
		if((CalibrationCtrl_52[SysCorrCmdType] >= eCORR_HALOUTS)
			&& (CalibrationCtrl_52[SysCorrCmdType] <= eCORR_LSHUDHD))
		{
	    	/*当前电流校准状态为步骤*/
			step = CaliFeedback_58[CalibrationState];

	    	/*之前在电流零点校准*/
			if(eCORR_HANDZRO == CaliFeedback_58[CalibrationType])
			{
				/*保持在第一步*/
				step = 0;
			}
		}
    	/*非电流校准*/
		else
		{
			/*保持在第一步*/
			step = 0;
		}
    }

    //根据校准状态检测信息
    switch(step)
    {
        //未校准
        case 0:
            /*更新电流校准结果*/
        	UpdateOutCurrCorrResult();

            //下发开始电流校准命令
            if((CalibrationCtrl_52[SysCorrCmdType] >= eCORR_HALOUTS)
            	&& (CalibrationCtrl_52[SysCorrCmdType] <= eCORR_LSHUDHD))
            {
                CaliFeedback_58[CalibrationState] = 1;
                CaliFeedback_58[CalibrationType] = CalibrationCtrl_52[SysCorrCmdType];
                BitClr(CaliFeedback_58[CalibrationResult], CalibrationCtrl_52[SysCorrCmdType]);
            }
            break;

        //清除校准参数
        case 1:
            /*清除电流零飘*/
            ClrCurrZeroFinFlagAPI();

            /*霍尔充电电流校准*/
        	if((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_HALOUTD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//复位KB参数
                BitClr(sCurrCorrType, eOCCorr_HalOutCS);
                BitClr(sCurrCorrType, eOCCorr_HalOutCD);
                
                ClrEEPHVoltCurrKBInfo(3);

                sHalOutCMapK = OC_CORR_BLOWUP;
                sHalOutCMapB = 0;
        	}
            /*电池端分流器充电电流校准*/
        	else if((eCORR_BSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_BSHUCHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//复位KB参数
                BitClr(sCurrCorrType, eOCCorr_BShuChgS);
                BitClr(sCurrCorrType, eOCCorr_BShuChgD);

                ClrEEPHVoltCurrKBInfo(4);

                sShuChgBMapK = OC_CORR_BLOWUP;
                sShuChgBMapB = 0;
        	}
            /*电池端分流器放电电流校准*/
        	else if((eCORR_BSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_BSHUDHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//复位KB参数
                BitClr(sCurrCorrType, eOCCorr_BShuDhgS);
                BitClr(sCurrCorrType, eOCCorr_BShuDhgD);

                ClrEEPHVoltCurrKBInfo(5);

                sShuDhgBMapK = OC_CORR_BLOWUP;
                sShuDhgBMapB = 0;
        	}
            /*负载端分流器充电电流校准*/
        	else if((eCORR_LSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_LSHUCHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//复位KB参数
                BitClr(sCurrCorrType, eOCCorr_LShuChgS);
                BitClr(sCurrCorrType, eOCCorr_LShuChgD);

                ClrEEPHVoltCurrKBInfo(6);

                sShuChgLMapK = OC_CORR_BLOWUP;
                sShuChgLMapB = 0;
        	}
            /*负载端分流器放电电流校准*/
        	else if((eCORR_LSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_LSHUDHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//复位KB参数
                BitClr(sCurrCorrType, eOCCorr_LShuDhgS);
                BitClr(sCurrCorrType, eOCCorr_LShuDhgD);

                ClrEEPHVoltCurrKBInfo(7);

                sShuDhgLMapK = OC_CORR_BLOWUP;
                sShuDhgLMapB = 0;
        	}

            for(j = 0; j < OC_CORR_FIL_T; j++)
            {
                sLowCurr[j] = 0;
                sHighCurr[j] = 0;
            }

            sErrTime = 0;
            sFilTime = 0;
            CalibrationCtrl_52[SysCorrValue] = 0;
            CaliFeedback_58[CalibrationState] = 2;
            break;

        //等待第一个基准电流
        case 2:
            //下发基准电流
            if(0 != CalibrationCtrl_52[SysCorrValue])
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sLowStandC = 0;
                    CaliFeedback_58[CalibrationState] = 3;
                }
            }
            break;

        //采集电压和读取第1个基准电压
        case 3:
            /*霍尔输出电流校准*/
        	if((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_HALOUTD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取霍尔采样电压值(1mv)*/
        		tempC = GetHallVolt();
				sLowStandC = (f32)CalibrationCtrl_52[SysCorrValue];
        	}
            /*电池端分流器充电电流校准*/
        	else if((eCORR_BSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_BSHUCHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取分流器采样电压值(1mV)*/
        		tempC =GetBatShuntVolt();

                //记录基准电流(10mA)
                //sLowStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV转1mV)
				sLowStandC = 0 - (f32)CalibrationCtrl_52[SysCorrValue]/100;

				// //小电流≤-110A(1mV对应4A,0.01mV)
				// if(sLowStandC >= -2750)
				// {
				// 	sCorrSec = 0;
				// }
        	}
            /*电池端分流器放电电流校准*/
        	else if((eCORR_BSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_BSHUDHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		/*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取分流器采样电压值(1mV)*/
        		tempC = GetBatShuntVolt();

                //记录基准电流(10mA)
                //sLowStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV转1mV)
				sLowStandC = (f32)CalibrationCtrl_52[SysCorrValue]/100;

				// //小电流≤110A(1mV对应4A,0.01mV)
				// if(sLowStandC <= 2750)
				// {
				// 	sCorrSec = 0;
				// }
        	}
            /*负载端分流器充电电流校准*/
        	else if((eCORR_LSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_LSHUCHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取分流器采样电压值(1mV)*/
        		tempC =GetLinkShuntVolt();

                //记录基准电流(10mA)
                //sLowStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV转1mV)
				sLowStandC = 0 - (f32)CalibrationCtrl_52[SysCorrValue]/100;

				// //小电流≤-110A(1mV对应4A,0.01mV)
				// if(sLowStandC >= -2750)
				// {
				// 	sCorrSec = 0;
				// }
        	}
            /*负载端分流器放电电流校准*/
        	else if((eCORR_LSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_LSHUDHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		/*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取分流器采样电压值(1mV)*/
        		tempC = GetLinkShuntVolt();

                //记录基准电流(10mA)
                //sLowStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV转1mV)
				sLowStandC = (f32)CalibrationCtrl_52[SysCorrValue]/100;

				// //小电流≤110A(1mV对应4A,0.01mV)
				// if(sLowStandC <= 2750)
				// {
				// 	sCorrSec = 0;
				// }
        	}
            //采样值与基准值误差50A内
            //if(ABS(tempC, sLowStandC) < 5000)
			//采样值与基准值霍尔误差100mV内/分流器误差10mV内
			if(((ABS(tempC, sLowStandC) < 100)
				&& ((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
		        || (eCORR_HALOUTD == CalibrationCtrl_52[SysCorrCmdType])))
				|| (ABS(tempC, sLowStandC) < 10))
            {
               sLowCurr[sFilTime] = tempC;
            }
            //采样电路误差过大或模拟电流输出误差过大
            else
            {
                //等待3s
                sErrTime++;
                if(sErrTime >= 3)
                {
                    //等待超时则校准失败
                    sErrTime = 0;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                return;
            }

            sFilTime++;
            sErrTime = 0;

            //达到滤波采样次数则求平均值并进入下一步
            if(sFilTime >= OC_CORR_FIL_T)
            {
                //计算采样通道平均值
                sFilSumCurr = 0;

                for(j = 0; j < OC_CORR_FIL_T; j++)
                {
                    sFilSumCurr += sLowCurr[j];
                }

                sLowCurr[0] =  sFilSumCurr / OC_CORR_FIL_T;

                sFilTime = 0;

                //判断是单点校准还是双点校准
                if((eCORR_HALOUTS == CaliFeedback_58[CalibrationType])
	                || (eCORR_BSHUCHS == CaliFeedback_58[CalibrationType])
	                || (eCORR_BSHUDHS == CaliFeedback_58[CalibrationType])
	                || (eCORR_LSHUCHS == CaliFeedback_58[CalibrationType])
	                || (eCORR_LSHUDHS == CaliFeedback_58[CalibrationType])
                    )
                {
                    //单点校准进入KB值计算
                    CalibrationCtrl_52[SysCorrValue] = 0;
                    CaliFeedback_58[CalibrationState] = 6;
                }
                else
                {
                    //双点校准进入第2点采集
                    CalibrationCtrl_52[SysCorrValue] = 0;
                    CaliFeedback_58[CalibrationState] = 4;
                }
            }
            break;

        //等待第2个基准电压
        case 4:
        //区分HALL和分流器
        
        if ((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
            || (eCORR_HALOUTD == CalibrationCtrl_52[SysCorrCmdType]))
        {
            if((0 != CalibrationCtrl_52[SysCorrValue]) && ((CalibrationCtrl_52[SysCorrValue] > sLowStandC)))//原值
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sHighStandC = 0;
                    CaliFeedback_58[CalibrationState] = 5;
                }
            }
        }
        else{ 
            //下发基准电压(必须比第1个基准点电压大)
            if((0 != CalibrationCtrl_52[SysCorrValue]) && ((CalibrationCtrl_52[SysCorrValue] > sLowStandC*100)))//原值*100
            {
                //等待1s再采集
                sErrTime++;
                if(sErrTime >= 1)
                {
                    sErrTime = 0;
                    sFilTime = 0;
                    sHighStandC = 0;
                    CaliFeedback_58[CalibrationState] = 5;
                }
            }

        }
            break;

        //采集第2个基准电压
        case 5:
            /*霍尔输出电流校准*/
        	if((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_HALOUTD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取霍尔采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCADC_Hall);
                /*取霍尔采样电压值(1mV)*/
        		tempC = GetHallVolt();

//        		//充电负电流
//        		/if(tempC < 0)
//        		{
//					//记录基准电流(10mA)
//					sHighStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
//        		}
//        		else
//        		{
//					//记录基准电流(10mA)
//					sHighStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
//        		}
                //记录霍尔基准电压(1mV)
				sHighStandC = (f32)CalibrationCtrl_52[SysCorrValue];
        	}
            /*电池端分流器充电电流校准*/
        	else if((eCORR_BSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_BSHUCHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取霍尔采样电压值(1mV)*/
        		tempC = GetBatShuntVolt();

                //记录基准电流(10mA)
        		//sHighStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV转1mV)
        		sHighStandC = 0 - (f32)CalibrationCtrl_52[SysCorrValue]/100;
        	}
            /*电池端分流器放电电流校准*/
        	else if((eCORR_BSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_BSHUDHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		/*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取霍尔采样电压值(1mV)*/
        		tempC = GetBatShuntVolt();

                //记录基准电流(10mA)
        		//sHighStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV转1mV)
        		sHighStandC = (f32)CalibrationCtrl_52[SysCorrValue]/100;
        	}
            /*负载端分流器充电电流校准*/
            else if((eCORR_LSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_LSHUCHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
                /*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取霍尔采样电压值(1mV)*/
        		tempC = GetLinkShuntVolt();

                //记录基准电流(10mA)
        		//sHighStandC = 0 - (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV转1mV)
        		sHighStandC = 0 - (f32)CalibrationCtrl_52[SysCorrValue]/100;
        	}
            /*负载端分流器放电电流校准*/
        	else if((eCORR_LSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_LSHUDHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		/*取分流器采样电流值(10mA)*/
        		//tempC = GetGRealCurr10mAAPI(eCAFE_Shunt);
                /*取霍尔采样电压值(1mV)*/
        		tempC = GetLinkShuntVolt();

                //记录基准电流(10mA)
        		//sHighStandC = (s32)gGCalibrCtrl_89[eCorrCmd89_Value];
				//记录基准电流对应电压(0.01mV转1mV)
        		sHighStandC = (f32)CalibrationCtrl_52[SysCorrValue]/100;
        	}

            //采样值与基准值误差50A内
            //if(ABS(tempC, sHighStandC) < 5000)
			//采样值与基准值霍尔误差100mV内/分流器误差10mV内
			if(((ABS(tempC, sHighStandC) < 100)
				&& ((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
				|| (eCORR_HALOUTD == CalibrationCtrl_52[SysCorrCmdType])))
				|| (ABS(tempC, sHighStandC) < 10))
            {
                sHighCurr[sFilTime] =tempC;
            }
            //采样误差过大或模拟电池输出误差过大
            else
            {
                //等待3s
                sErrTime++;
                if(sErrTime >= 3)
                {
                    //等待超时则校准失败
                    sErrTime = 0;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                return;
            }

            sFilTime++;
            sErrTime = 0;

            //达到滤波采样次数则求平均值并进入下一步
            if(sFilTime >= OC_CORR_FIL_T)
            {
                //计算每个采样通道平均值
                sFilSumCurr = 0;

                for(j = 0; j < OC_CORR_FIL_T; j++)
                {
                    sFilSumCurr += sHighCurr[j];
                }

                sHighCurr[0] =  sFilSumCurr / OC_CORR_FIL_T;

                sFilTime = 0;

                //进入KB值计算
                CalibrationCtrl_52[SysCorrValue] = 0;
                CaliFeedback_58[CalibrationState] = 6;
            }
            break;

        //校准计算处理
        case 6:
            //判断是单点校准还是双点校准
        	if((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
                || (eCORR_BSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
                || (eCORR_BSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
                || (eCORR_LSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
                || (eCORR_LSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
                )
            {
                sHighStandC = sLowStandC;
                calcK = OC_CORR_BLOWUP;

                calcB = ((f32)sLowStandC - sLowCurr[0]) * OC_CORR_BLOWUP;
            }
            else
            {
                //防止两次基准值一致除零
                if (FABS(sHighCurr[0] - sLowCurr[0]) < 0.1f)
                {
                    CaliFeedback_58[CalibrationState] = 101;
                    break;
                }

                calcK = (((f32)sHighStandC - (f32)sLowStandC) * OC_CORR_BLOWUP) / (((f32)sHighCurr[0] - (f32)sLowCurr[0]));
                //calcB = ((((s32)sHighCurr[0] * sLowStandC * 1000) - ((s32)sLowCurr[0] * sHighStandC * 1000))/((s32)sHighCurr[0] - (s32)sLowCurr[0]));
                calcB = (f32)sHighStandC * OC_CORR_BLOWUP - (f32)sHighCurr[0] * calcK;
            }

            /*霍尔输出电流校准*/
        	if((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_HALOUTD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//检测霍尔电流采样校准是否正确
        	    //tempC = (GetGRealCurr10mAAPI(eCADC_Hall) * calcK  + calcB) / OC_CORR_BLOWUP;
        		//检测霍尔电压采样校准是否正确
        	    tempC = (GetHallVolt() * calcK  + calcB) / OC_CORR_BLOWUP;

                //电流误差超过0.8A
                //if(ABS(sHighStandC, tempC) > 80)
                //电压误差超过5mV
                if(ABS(sHighStandC, tempC) > 5)
                {
                    //校准失败
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                //校准正确
                else
                {
                    //记录霍尔充电电流校准KB值
                    sHalOutCMapK = calcK;
                    sHalOutCMapB = calcB;
                    RcdEEPHVoltCurrKBInfo(3);

                    /*单点校准*/
                    if(eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
                    {
                        BitSet(sCurrCorrType, eOCCorr_HalOutCS);
                    }
                    /*双点校准*/
                    else
                    {
                        BitSet(sCurrCorrType, eOCCorr_HalOutCD);
                    }

                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 100;
                }
        	}
            /*电池端分流器充电电流校准*/
        	else if((eCORR_BSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_BSHUCHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//检测分流器电流采样校准是否正确
        	    //tempC = (GetGRealCurr10mAAPI(eCAFE_Shunt) * calcK  + calcB) / OC_CORR_BLOWUP;
        		//检测分流器电压采样校准是否正确
        	    tempC = (GetBatShuntVolt() * calcK  + calcB) / OC_CORR_BLOWUP;

                //电流误差超过0.8A
                //if(ABS(sHighStandC, tempC) > 80)
				//电压误差超过0.5mV
				if(ABS(sHighStandC, tempC) > 0.5)
                {
                    //校准失败
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                //校准正确
                else
                {
    				// //小电流≤110A
    				// if(0 == sCorrSec)
    				// {
                    //     //记录分流器充电小电流校准KB值
                    //     sShuChgBMapK = calcK;
                    //     sShuChgBMapB = calcB;
                    //     RcdEEPHVoltCurrKBInfo(u8 chan);

                    //     //返回重新校准大电流
                    //     sCorrSec = 1;
	                //     CaliFeedback_58[CalibrationState] = 2;
    				// }
    				// else
    				// {
						//记录分流器充电大电流校准KB值
						sShuChgBMapK = calcK;
						sShuChgBMapB = calcB;
                        RcdEEPHVoltCurrKBInfo(4);
                        //sCorrSec = 0;

	                    /*单点校准*/
	                    if(eCORR_BSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_BShuChgS);
	                    }
	                    /*双点校准*/
	                    else
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_BShuChgD);
	                    }

	                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
	                    CaliFeedback_58[CalibrationState] = 100;
    				//}
                }
        	}
            /*电池端分流器放电电流校准*/
        	else if((eCORR_BSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_BSHUDHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//检测分流器电流采样校准是否正确
        	    //tempC = (GetGRealCurr10mAAPI(eCAFE_Shunt) * calcK  + calcB) / OC_CORR_BLOWUP;
        		//检测分流器电压采样校准是否正确
        	    tempC = (GetBatShuntVolt() * calcK  + calcB) / OC_CORR_BLOWUP;

                //电流误差超过0.8A
                //if(ABS(sHighStandC, tempC) > 80)
				//电压误差超过0.5mV
				if(ABS(sHighStandC, tempC) > 0.5)
                {
                    //校准失败;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                //校准正确
                else
                {
    				// //小电流≤110A
    				// if(0 == sCorrSec)
    				// {
					// 	//记录分流器放电小电流校准KB值
					// 	sShuDhgBMapK = calcK;
					// 	sShuDhgBMapB = calcB;
					// 	RcdEEPHVoltCurrKBInfo();;

                    //     //返回重新校准大电流
                    //     sCorrSec = 1;
	                //     CaliFeedback_58[CalibrationState] = 2;
    				// }
    				// else
    				// {
						//记录分流器放电大电流校准KB值
						sShuDhgBMapK = calcK;
						sShuDhgBMapB = calcB;
                        RcdEEPHVoltCurrKBInfo(5);
                        //sCorrSec = 0;

	                    /*单点校准*/
	                    if(eCORR_BSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_BShuDhgS);
	                    }
	                    /*双点校准*/
	                    else
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_BShuDhgD);
	                    }

	                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
	                    CaliFeedback_58[CalibrationState] = 100;
    				//}
                }
                /*负载端分流器充电电流校准*/
        	}else if((eCORR_LSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_LSHUCHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//检测分流器电流采样校准是否正确
        	    //tempC = (GetGRealCurr10mAAPI(eCAFE_Shunt) * calcK  + calcB) / OC_CORR_BLOWUP;
        		//检测分流器电压采样校准是否正确
        	    tempC = (GetLinkShuntVolt() * calcK  + calcB) / OC_CORR_BLOWUP;

                //电流误差超过0.8A
                //if(ABS(sHighStandC, tempC) > 80)
				//电压误差超过0.5mV
				if(ABS(sHighStandC, tempC) > 0.5)
                {
                    //校准失败
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                //校准正确
                else
                {
    				// //小电流≤110A
    				// if(0 == sCorrSec)
    				// {
                    //     //记录分流器充电小电流校准KB值
                    //     sShuChgBMapK = calcK;
                    //     sShuChgBMapB = calcB;
                    //     RcdEEPHVoltCurrKBInfo(4);;

                    //     //返回重新校准大电流
                    //     sCorrSec = 1;
	                //     CaliFeedback_58[CalibrationState] = 2;
    				// }
    				// else
    				// {
						//记录分流器充电大电流校准KB值
						sShuChgLMapK = calcK;
						sShuChgLMapB = calcB;
                        RcdEEPHVoltCurrKBInfo(6);
                        //sCorrSec = 0;

	                    /*单点校准*/
	                    if(eCORR_LSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_LShuChgS);
	                    }
	                    /*双点校准*/
	                    else
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_LShuChgD);
	                    }

	                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
	                    CaliFeedback_58[CalibrationState] = 100;
    				//}
                }
        	}
            /*负载端分流器放电电流校准*/
        	else if((eCORR_LSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
        		|| (eCORR_LSHUDHD == CalibrationCtrl_52[SysCorrCmdType]))
        	{
        		//检测分流器电流采样校准是否正确
        	    //tempC = (GetGRealCurr10mAAPI(eCAFE_Shunt) * calcK  + calcB) / OC_CORR_BLOWUP;
        		//检测分流器电压采样校准是否正确
        	    tempC = (GetLinkShuntVolt() * calcK  + calcB) / OC_CORR_BLOWUP;

                //电流误差超过0.8A
                //if(ABS(sHighStandC, tempC) > 80)
				//电压误差超过0.5mV
				if(ABS(sHighStandC, tempC) > 0.5)
                {
                    //校准失败;
                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
                    CaliFeedback_58[CalibrationState] = 101;
                }
                //校准正确
                else
                {
    				// //小电流≤110A
    				// if(0 == sCorrSec)
    				// {
					// 	//记录分流器放电小电流校准KB值
					// 	sShuDhgBMapK = calcK;
					// 	sShuDhgBMapB = calcB;
					// 	RcdEEPHVoltCurrKBInfo(4);

                    //     //返回重新校准大电流
                    //     sCorrSec = 1;
	                //     CaliFeedback_58[CalibrationState] = 2;
    				// }
    				// else
    				// {
						//记录分流器放电大电流校准KB值
						sShuDhgLMapK = calcK;
						sShuDhgLMapB = calcB;
                        RcdEEPHVoltCurrKBInfo(7);
                        //sCorrSec = 0;

	                    /*单点校准*/
	                    if(eCORR_LSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_LShuDhgS);
	                    }
	                    /*双点校准*/
	                    else
	                    {
	                        BitSet(sCurrCorrType, eOCCorr_LShuDhgD);
	                    }

	                    CalibrationCtrl_52[SysCorrCmdType] = eCORR_INVALID;
	                    CaliFeedback_58[CalibrationState] = 100;
    				//}
                }
        	}
            break;

        //校准成功
        case 100:
        //校准失败
        case 101:
            /*更新电流校准结果*/
        	UpdateOutCurrCorrResult();

            //下发开始电流校准命令
            if((eCORR_HALOUTS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_HALOUTD == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_BSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_BSHUCHD == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_BSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_BSHUDHD == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_LSHUCHS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_LSHUCHD == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_LSHUDHS == CalibrationCtrl_52[SysCorrCmdType])
            	|| (eCORR_LSHUDHD == CalibrationCtrl_52[SysCorrCmdType])
                )
            {
                CaliFeedback_58[CalibrationType] = CalibrationCtrl_52[SysCorrCmdType];
                CaliFeedback_58[CalibrationState] = 1;
            }
            break;

        default:
            break;
    }
}

/*=================================================================================================
** @Name      : void ClrEEPHVoltCurrKBInfo(u8 chan)
** @Input     : chan:kb保存通道 0:电池总压 1:PE电压  2:LINK电压 3:霍尔电流 4:电池端分流器充电电流 5:电池端分流器放电电流 6:LINK端分流器充电电流 7:LINK端分流器放电电流 8:霍尔零漂校准
** @Output    : void
** @Function  : 清除EEP高压采样校准KB值
** @The notes :
**===============================================================================================*/
void ClrEEPHVoltCurrKBInfo(u8 chan)
{
    u32 flag = 0;//参数有效不使能
	/*电池总压*/
	if(0 == chan )
	{
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.BatVoltKBFlag = flag;
        stParameter.fBatVoltK = HV_CORR_BLOWUP;
        stParameter.fBatVoltB = 0;
	}else if(1 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.PeVoltKBFlag = flag;
        stParameter.fPeVoltK = HV_CORR_BLOWUP;
        stParameter.fPeVoltB = 0;
        
    }else if(2 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.LinkVoltKBFlag = flag;
        stParameter.fLinkVoltK = HV_CORR_BLOWUP;
        stParameter.fLinkVoltB = 0;
        
    }else if(3 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.HallCurrKBFlag = flag;
        stParameter.fHallCurrentK = OC_CORR_BLOWUP;
        stParameter.fHallCurrentB = 0;
        
    }else if(4 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.BatShuntChgKBFlag = flag;
        stParameter.fBatShuntCurrentChgK = OC_CORR_BLOWUP;
        stParameter.fBatShuntCurrentChgB = 0;
    }else if(5 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.BatShuntDhgKBFlag = flag;
        stParameter.fBatShuntCurrentDhgK = OC_CORR_BLOWUP;
        stParameter.fBatShuntCurrentDhgB = 0;
    }else if(6 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.LinkShuntChgKBFlag = flag;
        stParameter.fLinkShuntCurrentChgK = OC_CORR_BLOWUP;
        stParameter.fLinkShuntCurrentChgB = 0;
    }else if(7 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.LinkShuntDhgKBFlag = flag;
        stParameter.fLinkShuntCurrentDhgK = OC_CORR_BLOWUP;
        stParameter.fLinkShuntCurrentDhgB = 0;
    }else if(8 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.HallZeroCorrFlag = flag;
    }
}

/*=============================================================================================
** @Name      : void ClrCurrZeroFinFlagAPI(void)
** @Input     : void
** @Output    : void
** @Function  : 清除电流零点校零标志
** @The notes : 重新校零
**===========================================================================================*/
void ClrCurrZeroFinFlagAPI(void)
{
    sCurrZeroFin = 0;
    sCurrZeroCorr = 0;
    // sHallZeroCorr = 0;
}

/*=============================================================================================
** @Name      : u8 SampGetCurrZeroFinFlagAPI(void)
** @Input     : void
** @Output    : 电流采样零飘校准完成标志[0未校准 1校准成功 2校准失败]
** @Function  : 获取电流采样零飘校准完成标志
** @The notes :
**===========================================================================================*/
u8 SampGetCurrZeroFinFlagAPI(void)
{
    return(sCurrZeroFin);
}

/*=================================================================================================
** @Name      : void UpdateOutCurrCorrResult(void)
** @Input     : void
** @Output    : void
** @Function  : 更新电流校准结果
** @The notes : 根据校准标志位
**===============================================================================================*/
void UpdateOutCurrCorrResult(void)
{
    /*电流零点已校准*/
	if(1 == SampGetCurrZeroFinFlagAPI())
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_HANDZRO));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_HANDZRO));
	}

    /*霍尔输出电流单点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_HalOutCS))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_HALOUTS));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_HALOUTS));
	}

    /*霍尔输出电流双点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_HalOutCD))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_HALOUTD));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_HALOUTD));
	}

    /*分流器充电电流单点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_BShuChgS))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_BSHUCHS));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_BSHUCHS));
	}

    /*分流器充电电流双点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_BShuChgD))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_BSHUCHD));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_BSHUCHD));
	}

    /*分流器放电电流单点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_BShuDhgS))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_BSHUDHS));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_BSHUDHS));
	}

    /*分流器放电电流双点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_BShuDhgD))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_BSHUDHD));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_BSHUDHD));
	}

    /*分流器充电电流单点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_LShuChgS))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_LSHUCHS));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_LSHUCHS));
	}

    /*分流器充电电流双点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_LShuChgD))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_LSHUCHD));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_LSHUCHD));
	}

    /*分流器放电电流单点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_LShuDhgS))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_LSHUDHS));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_LSHUDHS));
	}

    /*分流器放电电流双点已校准*/
	if(1 == BitGet(sCurrCorrType, eOCCorr_LShuDhgD))
	{
		BitSet(CaliFeedback_58[CalibrationResult], (eCORR_LSHUDHD));
	}
	/*未校准*/
	else
	{
		BitClr(CaliFeedback_58[CalibrationResult], (eCORR_LSHUDHD));
	}
}

/*=================================================================================================
** @Name      : void RcvGroupDebugOperCmdAPI(u8 index)
** @Input     : index:调试命令索引
** @Output    : void
** @Function  : 接收切换调试操作命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvGroupDebugOperCmdAPI(u8 index)
{
    /*调试确认命令处理*/
	if(index < DebugComfirm)
	{
		DebugFeedback_61[index] = SystemDebugCmd_60[index];
	}
}

/*=================================================================================================
** @Name      : void RcvGroupDebugAffirmCmdAPI(u8 port)
** @Input     : port:接收命令端口:[0,4]CAN口 [5,9]RS485口 10:按钮 11:内部
** @Output    : void
** @Function  : 接收调试确认命令处理函数
** @The notes : 通信口或按钮命令
**===============================================================================================*/
void RcvGroupDebugAffirmCmdAPI(u8 port)
{
	// u8 evnId = 0;

    /*调试确认命令处理*/
    TacSetGroupDebugAffirmCmdAPI(SystemDebugCmd_60[DebugComfirm]);
}

/*=================================================================================================
** @Name      : void TacSetGroupDebugAffirmCmdAPI(u8 cmd)
** @Input     : cmd:调试确认命令 1:功能调试确认/EEPROM读调试确认 2:EEPROM写调试确认
** @Output    : void
** @Function  : 调试确认命令处理函数
** @The notes : 通信口
**===============================================================================================*/
void TacSetGroupDebugAffirmCmdAPI(u8 cmd)
{
	/*确认功能调试/确认读EEPROM参数 */
    if(1 == cmd)
    {
        if(eMODE_Debug != GetSystemWorkModeArray(WorkMode))                 //非调试模式不得调试
        {
        	DebugFeedback_61[DebugFbResult] = eDBGRES_NOMOD;          //调试结果 2:未进入调试模式
        }

        else if(eDBGCMD_CTRLDRY == SystemDebugCmd_60[DebugType])        //干接点调试命令
        {
            if(TRUE == TacDebugOutDryDeal())                            //设置干接点调试处理成功
            {
                DebugFeedback_61[DebugFbResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_CTRLRLY == SystemDebugCmd_60[DebugType])        //继电器器调试命令
        {
            if(TRUE == TacDebugSwitchDeal())                           //设置继电器调试处理成功
            {
                DebugFeedback_61[DebugFbResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else if(eDBGCMD_RWEEPPRM == SystemDebugCmd_60[DebugType])        //EEP参数调试信息成功
        {
            if(TRUE == TacDebugReadEepDeal())                           //设置读 EEP参数调试信息成功
            {
                DebugFeedback_61[DebugFbResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }

    }
    /*EEP写调试确认*/
    else if(2 == cmd)
    {
        if(eMODE_Debug != GetSystemWorkModeArray(WorkMode))              //非调试模式不得调试
        {
            DebugFeedback_61[DebugFbResult] = eDBGRES_NOMOD;          //调试结果 2:未进入调试模式
        }
        else if(eDBGCMD_RWEEPPRM == SystemDebugCmd_60[DebugType])        //主控配置参数读写调试命令
        {
            if(TRUE == TacDebugWriteEepDeal())                         //主控EEP参数调试处理成功
            {
                DebugFeedback_61[DebugFbResult] = eDBGRES_SUCCE;      //调试结果 1:调试完成
            }
        }
        else                                                                //调试类型不正确
        {
            DebugFeedback_61[DebugFbResult] = eDBGRES_ERRPA;          //调试结果 3:设置参数错误
        }
    }
    else                                                                    //非调试确认命令
    {
        return;
    }


}

/*=================================================================================================
** @Name      : void TacDebugOutDryDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制输出干接点状态 DO调试
** @The notes : 接收输出干接点调试确认命令时调用
**===============================================================================================*/
u8 TacDebugOutDryDeal(void)
{
    // u8 slave = 0;
    u8 result = 0;

                     
        //控制端口和状态正确
        if((SystemDebugCmd_60[DebugOption] <= 2)            //两路do控制序号正确,干节点调试：0：所有干节点 1：干节点1 2:干节点2 
            && (SystemDebugCmd_60[DebugState] <= 1))        //do状态正确,干节点调试：0：断开 1：闭合
        {
            //输入DO控制信息
            if(SystemDebugCmd_60[DebugOption] == 1)
            {
                gControl.DOStateSet[DO_1] = (SystemDebugCmd_60[DebugState] == 1) ? 1 : 0;
                if(SystemDebugCmd_60[DebugState] == 1)
                {
                    CRELAY1_Contr_Close();
                }
                else
                {
                    CRELAY1_Contr_Open();
                }
            }else if(SystemDebugCmd_60[DebugOption] == 2)
            {
                gControl.DOStateSet[DO_2] = (SystemDebugCmd_60[DebugState] == 1) ? 1 : 0;
                if(SystemDebugCmd_60[DebugState] == 1)
                {
                    CRELAY2_Contr_Close();
                }
                else
                {
                    CRELAY2_Contr_Open();
                }
            }else if(SystemDebugCmd_60[DebugOption] == 0)
            {
                gControl.DOStateSet[DO_1] = (SystemDebugCmd_60[DebugState] == 1) ? 1 : 0;
                gControl.DOStateSet[DO_2] = (SystemDebugCmd_60[DebugState] == 1) ? 1 : 0;
                if(SystemDebugCmd_60[DebugState] == 1)
                {
                    CRELAY1_Contr_Close();
                    CRELAY2_Contr_Close();
                }
                else
                {
                    CRELAY1_Contr_Open();
                    CRELAY2_Contr_Open();
                }
            }
            result = TRUE;
        }
        //控制端口或状态错误
        else
        {
            DebugFeedback_61[DebugFbResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
            result = FALSE;
        }
    
    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugSwitchDeal(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式控制继电器状态
** @The notes : 接收继电器调试确认命令时调用
**===============================================================================================*/
u8 TacDebugSwitchDeal(void)
{
    // u8 slave = 0;
    u8 result = 0;

    if(GetSystemWorkModeArray(WorkMode)!= eMODE_Tooling)
    {
        if((SystemDebugCmd_60[DebugOption] <= 2)                       //继电器调试 0：所有继电器 1：PE_RLY 2：IMD_RLY
            && (SystemDebugCmd_60[DebugState] <= 1))                   //继电器调试：0：断开 1：闭合
        {
            //输入从控接触器控制信息
            //输入DO控制信息
            if(SystemDebugCmd_60[DebugOption] == 1)
            {
                if(SystemDebugCmd_60[DebugState] == 1)
                {
                    PE_RLY_Contr_Close();
                    //添加板上继电器切换至少间隔时间
                    ControlRLYSwitchTime();
                }
                else
                {
                    PE_RLY_Contr_Open();
                    //添加板上继电器切换至少间隔时间
                    ControlRLYSwitchTime();
                }
            }else if(SystemDebugCmd_60[DebugOption] == 2)
            {
                if(SystemDebugCmd_60[DebugState] == 1)
                {
                    IMD_RLY_Close();
                    //添加板上继电器切换至少间隔时间
                    ControlRLYSwitchTime();
                }
                else
                {
                    IMD_RLY_Open();
                    //添加板上继电器切换至少间隔时间
                    ControlRLYSwitchTime();
                  
                }
            }else if(SystemDebugCmd_60[DebugOption] == 0)
            {
                if(SystemDebugCmd_60[DebugState] == 1)
                {
                    IMD_RLY_Close();
                    PE_RLY_Contr_Close();
                    //添加板上继电器切换至少间隔时间
                    ControlRLYSwitchTime();
                }
                else
                {
                    PE_RLY_Contr_Open();
                    IMD_RLY_Open();
                    //添加板上继电器切换至少间隔时间
                    ControlRLYSwitchTime();
                    
                }
            }
            result = TRUE;
        }
        //控制端口或状态错误
        else
        {
            DebugFeedback_61[DebugFbResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
            result = FALSE;
        }
    }
    return(result);
}

/*=================================================================================================
** @Name      : void DebugReadEepData(void)
** @Input     : void
** @Output    : TRUE:调试成功 FALSE:调试失败
** @Function  : 调试模式读取EEPROM参数值
** @The notes : 接收EEPROM参数调试确认命令时调用
**===============================================================================================*/
u8 TacDebugReadEepDeal(void)
{
    u8 result = FALSE;
    u8 rdata[4] = {0};
    if((SystemDebugCmd_60[DebugOption] <= 0x7ff)                       //16k的eeprom最大地址
        && (SystemDebugCmd_60[DebugState] <= 2))                        //0:8位数据 1:16位数据 2:32位数据
    {
        if(I2C_EE_BufferRead(rdata,Eeprom_TestAddress_Start,4) == TRUE)
        {
            result = TRUE;
        }
        DebugFeedback_61[DebugFbEepReadL] = ((u16)rdata[1] << 8) | rdata[0];
        DebugFeedback_61[DebugFbEepReadH] = ((u16)rdata[3] << 8) | rdata[2];
        DebugFeedback_61[DebugFbResult] = eDBGRES_SUCCE;               //调试结果 1:成功
    }
    //调试参数错误
    else
    {
        DebugFeedback_61[DebugFbResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = FALSE;
    }

    return(result);
}

/*=================================================================================================
** @Name      : void TacDebugWriteEepDeal(void)
** @Input     : void
** @Output    : TRUE:写入成功 FALSE:写入失败
** @Function  : 调试模式写EEP参数值
** @The notes : 接收写EEP参数调试确认命令时调用
**===============================================================================================*/
u8 TacDebugWriteEepDeal(void)
{
    u8 result = 0;
    u8 wdata[4] = {0};
    

    if((SystemDebugCmd_60[DebugOption] <= 0x7ff)                       //16k的eeprom最大地址
        && (SystemDebugCmd_60[DebugState] <= 2))               //写参数EEP地址正确
    {
        //写EEP参数:EEP小端模式(低位在前)
        wdata[0] = SystemDebugCmd_60[DebugEepWriteL] & 0xff;
        wdata[1] = (SystemDebugCmd_60[DebugEepWriteL] >> 8) & 0xff;
        wdata[2] = SystemDebugCmd_60[DebugEepWriteH] & 0xff;
        wdata[3] = (SystemDebugCmd_60[DebugEepWriteH] >> 8) & 0xff;
        if(I2C_EE_PageWrite(wdata, Eeprom_TestAddress_Start, 4) == TRUE)
        {
            result = TRUE;
        }  
    }
    else                                                        //写参数EEP地址错误
    {
        DebugFeedback_61[DebugFbResult] = eDBGRES_ERRPA;               //调试结果 3:设置参数错误
        result = FALSE;
    }
    return(result);
}

/*=============================================================================================
** @Name      : void CalcCurrZeroCorr(s32 curr)
** @Input     : curr:空闲时采样霍尔电流零飘值 单位:1mv
** @Output    : void
** @Function  : 计算霍尔电流零飘值
** @The notes : 必须在高压接触器全断开(无电流)时调用,由上层决定，将空闲霍尔偏移成2.5V
**===========================================================================================*/
void CalcCurrZeroCorr(s32 curr)
{
    // u16 flag = 0;           //EEPROM标志
    s32 curZeroMv = 0;        //零飘值 1mA
    static u16 num = 0;     //积分次数
    // static u16 time = 0;    //误差次数（预留）
    static s32 curSum = 0;  //零飘积分
    static s32 curMax = 0;  //零飘最大值
    static s32 curMin = 0;  //零飘最小值


    /*手动电流校零*/
    if(eCORR_HANDZRO == CaliFeedback_58[CalibrationType])
    {
        //非调试模式不校准
		if( (eMODE_Debug == GetSystemWorkModeArray(WorkMode))            /*调试模式*/
			&& (eErr_B !=  DiagGetErrLastStateAPI(eERR_ID_HAL_OVE))      /*非故障状态*/
			&& (eErr_B !=  DiagGetErrLastStateAPI(eERR_ID_HAL_SUP))
            && (eErr_B !=  DiagGetErrLastStateAPI(eERR_ID_HAL_ADC)))     
		{
			if(0 == sCurrZeroFin)                                       /*未完成电流零飘值计算*/
			{
				//计算1s电流积分求平均
                if(num < 20)
                {
                    if((curr > curMax) || (0 == num))
                    {
                        curMax = curr;
                    }

                    if((curr < curMin) || (0 == num))
                    {
                        curMin = curr;
                    }

                    curSum += curr;
                    num++;
                }
                else
                {
                    curZeroMv = (curSum - curMax - curMin) / (num - 2);

                    if(((s32)(HALL_ZERO_CORR_0A - MAX_HALL_CORR_10MV) <= curZeroMv)
                        && (curZeroMv <= (s32)(HALL_ZERO_CORR_0A + MAX_HALL_CORR_10MV))) //计算正确±2A
                    {

                        sCurrZeroCorr = (s32)(HALL_ZERO_CORR_0A - curZeroMv);  //零飘值 1mv

                        RcdEEPHVoltCurrKBInfo(8);
                        //ParaWriteStoreCorrZeroCurr(1, (u16*)&sCurrZeroCorr, 1);//修改EEP值
                        //flag = PARA_CFG_FLAG_U16;
                        //ParaWriteStoreCorrZeroCurr(0, &flag, 1);        //修改EEP标志

                        //SetCurrSampExpFlagAPI(eCExp_ZroErr, 0, (curZeroMv / 100));//清除电流校零异常标志
                        sCurrZeroFin = 1;                               //电流校零完成
                    }
                    else                                                //计算错误重新计算
                    {
                        //SetCurrSampExpFlagAPI(eCExp_ZroErr, 1, (curZeroMv / 100));//记录电流校零异常标志
                        sCurrZeroFin = 2;                               //电流校零失败
                    }

                    curSum = 0;                                         //零飘求和值
                    curMax = 0;                                         //零飘最大值
                    curMin = 0;                                         //零飘最小值
                    // time = 0;                                   //误差次数（预留）
                    num = 0;
                }
			}

		}

    }
}

/*=================================================================================================
** @Name      : void RcdEEPHVoltCurrKBInfo(u8 chan)
** @Input     : chan:kb保存通道 0:电池总压 1:PE电压  2:LINK电压 3:霍尔电流 4:电池端分流器充电电流 5:电池端分流器放电电流 6:LINK端分流器充电电流 7:LINK端分流器放电电流 8:霍尔零漂校准
** @Output    : void
** @Function  : 记录校准KB值到EEP
** @The notes :
**===============================================================================================*/
void RcdEEPHVoltCurrKBInfo(u8 chan)
{
    u32 flag = 1;//参数有效使能
	/*电池总压*/
	if(0 == chan )
	{
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sSumVoltMapK, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sSumVoltMapB, Eeprom_Calibration_Add + 8 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.BatVoltKBFlag = flag;
        stParameter.fBatVoltK = sSumVoltMapK;
        stParameter.fBatVoltB = sSumVoltMapB;
        
	}else if(1 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sPegVoltMapK, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sPegVoltMapB, Eeprom_Calibration_Add + 8 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.PeVoltKBFlag = flag;
        stParameter.fPeVoltK = sPegVoltMapK;
        stParameter.fPeVoltB = sPegVoltMapB;
        
    }else if(2 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sLinVoltMapK, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sLinVoltMapB, Eeprom_Calibration_Add + 8 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.LinkVoltKBFlag = flag;
        stParameter.fLinkVoltK = sLinVoltMapK;
        stParameter.fLinkVoltB = sLinVoltMapB;
        
    }else if(3 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sHalOutCMapK, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sHalOutCMapB, Eeprom_Calibration_Add + 8 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.HallCurrKBFlag = flag;
        stParameter.fHallCurrentK = sHalOutCMapK;
        stParameter.fHallCurrentB = sHalOutCMapB;
        
    }else if(4 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sShuChgBMapK, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sShuChgBMapB, Eeprom_Calibration_Add + 8 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.BatShuntChgKBFlag = flag;
        stParameter.fBatShuntCurrentChgK = sShuChgBMapK;
        stParameter.fBatShuntCurrentChgB = sShuChgBMapB;
        
    }else if(5 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sShuDhgBMapK, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sShuDhgBMapB, Eeprom_Calibration_Add + 8 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.BatShuntDhgKBFlag = flag;
        stParameter.fBatShuntCurrentDhgK = sShuDhgBMapK;
        stParameter.fBatShuntCurrentDhgB = sShuDhgBMapB;
    }else if(6 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sShuChgLMapK, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sShuChgLMapB, Eeprom_Calibration_Add + 8 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.LinkShuntChgKBFlag = flag;
        stParameter.fLinkShuntCurrentChgK = sShuChgLMapK;
        stParameter.fLinkShuntCurrentChgB = sShuChgLMapB;
    }else if(7 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sShuDhgLMapK, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sShuDhgLMapB, Eeprom_Calibration_Add + 8 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.LinkShuntDhgKBFlag = flag;
        stParameter.fLinkShuntCurrentDhgK = sShuDhgLMapK;
        stParameter.fLinkShuntCurrentDhgB = sShuDhgLMapB;
    }else if(8 == chan)
    {
		/*修改EEP值*/
        I2C_EE_BufferWrite((u8 *) &flag, Eeprom_Calibration_Add + (chan*3*4), 4);
        I2C_EE_BufferWrite((u8 *) &sCurrZeroCorr, Eeprom_Calibration_Add + 4 + (chan*3*4), 4);
        /*更新当前参数值*/
        stParameter.HallZeroCorrFlag = flag;
        stParameter.fHallZeroCorr = sCurrZeroCorr;
    }

}
/*=============================================================================================
** @Static function definition
**===========================================================================================*/
