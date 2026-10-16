
/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Measure.c
** @Founder      : WYP40505
** @Date         : 2026.05.06
** @Function     : 数据采集模块
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "Measure.h"
#include "BSPADC.h"
#include "BSPDIDO.h"
#include "SystemData.h"
#include "BSPKA84917_async.h"
// #include "BSPKA84917.h"
#include "drv_rn8209_interface.h"
#include "SystemPara.h"
#include "SystemInfo.h"
#include "SystemDebug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "SystemDiag.h"
#include "BSPTypeDef.h"
#include "SEGGER_RTT.h"
#include "CurrFliterCalc.h"
#include "../Test/TEST.h"
#include "spi_async.h"
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define NTC_T_TAB_LEN  191U
#define ADC_SAMP_TIMES   8       //ADC通道采样次数(滤波次数)


#define AD_MAX_AD 0x3FFFFU
#define AD_MAX_VOLT 1.0f
#define VOLT_RATE    ((6720.0f+3.16f)/3.16f)
#define CURRENT_RATE 9
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static const uint32 sNTCBalTTabR[NTC_T_TAB_LEN] =    //NTC(10K)温度阻值取值表[BAL_T_TAB_LEN],[-40~150]
{
    193278,183037,173310,164116,155454,147302,139635,132422,125631,119234,//-40℃__-31℃
    113203,107511,102136,97058,92257,87717,83422,79357,75511,71870,//-30℃__-21℃
    68424,65161,62071,59146,56376,53752,51266,48911,46679,44564,//-20℃__-11℃
    42557,40655,38849,37135,35508,33963,32494,31098,29770,28507,//-10℃__-1℃
    27305,26160,25070,24031,23041,22096,21196,20336,19516,18733,//0℃__9℃
    17985,17271,16588,15936,15312,14716,14145,13600,13078,12579,//10℃__19℃
    12101,11643,11205,10786,10384,10000,9631,9278,8940,8615,//20℃__29℃
    8305,8007,7721,7447,7184,6932,6690,6458,6236,6021,//30℃__39℃
    5815,5617,5428,5245,5070,4902,4740,4584,4435,4291,//40℃__49℃
    4152,4019,3890,3766,3647,3532,3422,3315,3213,3114,//50℃__59℃
    3018,2926,2837,2751,2669,2589,2512,2437,2365,2296,//60℃__69℃
    2229,2164,2101,2040,1982,1925,1870,1817,1766,1717,//70℃__79℃
    1669,1622,1577,1534,1492,1451,1411,1373,1336,1300,//80℃__89℃
    1266,1232,1199,1168,1137,1108,1079,1051,1024,998,//90℃__99℃
    972,948,924,901,878,857,835,815,795,776,//100℃__109℃
    757,739,721,704,687,671,655,640,625,611,//110℃__119℃
    597,583,570,557,545,533,521,509,498,487,//120℃__129℃
    477,466,456,446,437,427,418,410,401,392,//130℃__139℃
    384,376,368,361,353,346,339,332,325,318,//140℃__149℃
    312//150℃
};
static f32 HV_REF = 4.015;


/*=============================================================================================
** @Static function declaration
**===========================================================================================*/


/*=============================================================================================
** @Name      : static f32 CalcHallCurrent(f32 volt, f32 ZeroVolt, f32 VoltRange, f32 CurrRange)
** @Input     : void
** @Output    : 校准前HALL电流计算结果
** @Function  : 校准前Hall电流计算
** @The notes : 
**===========================================================================================*/
static f32 CalcHallCurrent(f32 volt, f32 ZeroVolt, f32 VoltRange, f32 CurrRange);

/*=============================================================================================
** @Name      : static f32 CalcCalibHallCurrent(f32 volt, f32 ZeroVolt, f32 VoltRange, f32 CurrRange,f32 VoltRange_mV,f32 CurrRange_A)
** @Input     : void
** @Output    : 校准后HALL电流计算结果
** @Function  : 校准后Hall电流计算
** @The notes : 判断HALL电压电流是否超量程，超量程电流清零
**===========================================================================================*/
static f32 CalcCalibHallCurrent(f32 volt, f32 ZeroVolt, f32 VoltRange, f32 CurrRange);

/*=============================================================================================
** @Name      : static f32 CalcShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
** @Input     : ShuntVolt VoltRange CurrRange
** @Output    : 校准前分流器电流计算结果
** @Function  : 校准前分流器电流计算
** @The notes : 
**===========================================================================================*/
static f32 CalcShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange);

/*=============================================================================================
** @Name      : static f32 CalcCalibBatShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
** @Input     : ShuntVolt VoltRange CurrRange
** @Output    : 校准后分流器电流计算结果
** @Function  : 校准后分流器电流计算
** @The notes : 判断BAT分流器电压电流是否超量程，超量程电流清零
**===========================================================================================*/
static f32 CalcCalibBatShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange);

/*=============================================================================================
** @Name      : static f32 CalcCalibLinkShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
** @Input     : ShuntVolt VoltRange CurrRange
** @Output    : 校准后分流器电流计算结果
** @Function  : 校准后分流器电流计算
** @The notes : 判断Link分流器电压电流是否超量程，超量程电流清零
**===========================================================================================*/
static f32 CalcCalibLinkShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange);

/*=============================================================================================
** @Name      : static f32 CalibVoltCheck(f32 Volt, u32 KBFlag, f32 K, f32 B)
** @Input     : 电压值 校准标志 K B
** @Output    : 校准后电压
** @Function  : 计算校准电压
** @The notes : 判断是否满足校准条件 计算校准后电压
**===========================================================================================*/
static f32 CalibVoltCheck(f32 Volt, u32 KBFlag, f32 K, f32 B);


/*==================================================================
** @Name      : static u16 Measure_SearchNtcTempTabByR(UINT32 data, const uint32 u8Table[],u8 tablesize)
** @Input     : data:ntc电阻值，u8Table：ntc电阻与温度关系表
** @Output    : 
** @Function  : 获取ntc阻值对应温度下标
** @The notes : 
**================================================================*/
static u16 Measure_SearchNtcTempTabByR(UINT32 data, const uint32 u8Table[],u8 tablesize);

/*=============================================================================================
** @Name      :static void BoardReadInfoSync(void)
** @Input     : 
** @Output    : 
** @Function  : 板载拨码读取、继电器板检测、以及DO信息同步
** @The notes : 
**===========================================================================================*/
static void BoardReadInfoSync(void);

static float32 GetVoltViaAd(uint32 u32Ad);
static float32 GetCurrentViaAd(uint32 u32Ad);

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/

/*==========================================================================
**   采样计算任务 (计算逻辑已迁入 KA84917_maintask)
**   每周期: KA84917_maintask 内完成采样+快路径计算
**   每80ms: KA84917_maintask 内完成全量计算
**========================================================================*/
void KA84917_Rn8209_MeasureTask(void)
{
    KA84917_maintask();//采样+计算任务 (采样后立即计算)
}


/*==========================================================================
** @Name      : void CalcFast3Results(void)
** @Function  : 仅计算快路径3个分流器电流 (从原Ka84917VoltCurrentCalculate和
**              Rn8209VoltCurrentCalculate中拆分, 计算逻辑不变)
**   计算结果: BatShuntCurrent1, BatShuntCurrent2, LinkShuntCurrent
**========================================================================*/
void CalcFast3Results(void)
{
    /* 通信异常时填充默认值 */
    if ((wakeUpState == 0) || (ucSPI_GetComError() != SPI_NOERR))
    {
        InPutSystemDataArray(BatShuntCurrentL, 0);
        InPutSystemDataArray(BatShuntCurrentH, 0);
        InPutSystemDataArray(LinkShuntCurrentL, 0);
        InPutSystemDataArray(LinkShuntCurrentH, 0);
        BitSet16(SystemData_51[DidoGpioState], DaisyComFail);
        return;
    }
    if (FirstSampleFinishedFlag == 0)
    {
        InPutSystemDataArray(BatShuntCurrentL, 0);
        InPutSystemDataArray(BatShuntCurrentH, 0);
        InPutSystemDataArray(LinkShuntCurrentL, 0);
        InPutSystemDataArray(LinkShuntCurrentH, 0);
        return;
    }
    BitClr16(SystemData_51[DidoGpioState], DaisyComFail);

    
    /* 变量声明 */
    f32 fShuntVolt1 = 0, fShuntCurrent1 = 0;
    f32 fShuntVolt2 = 0, fShuntCurrent2 = 0;
    u32 u32ShuntCurrentAdSum1 = 0, u32ShuntCurrentAd1 = 0;
    u32 u32ShuntCurrentAdSum2 = 0, u32ShuntCurrentAd2 = 0;
    u8 u8ShuntCurrentCount1 = 0, u8ShuntCurrentCount2 = 0;
    s32 s32fShuntCurrent1 = 0, s32fShuntCurrent2 = 0, s32HallCurrent = 0;

    f32 fShuntVoltRange, fShuntCurrRange;
    fShuntVoltRange = stParameter.u16ArraySet[ShuntVoltSampRange];
    fShuntCurrRange = stParameter.u16ArraySet[ShuntCurrentSampRange]*10;

    /*=== 1. 电池端分流器电流1 (from FIFO[0] C7) ===*/
    for(int i = 0; i < 16; i++)
    {
        if(usSPI_RegFifo_Data[0][i] != 0)
        {
            u32ShuntCurrentAdSum1 += (usSPI_RegFifo_Data[0][i]>>16);
            u8ShuntCurrentCount1++;
        }
    }
    u32ShuntCurrentAd1 = (u8ShuntCurrentCount1 != 0)
        ? (u32ShuntCurrentAdSum1/u8ShuntCurrentCount1) : 0x8000;

    if(0X8000 == (u32ShuntCurrentAd1 & 0x8000))//为正压
    {
        fShuntVolt1 = u32ShuntCurrentAd1 & 0x7fff;
        fShuntVolt1 = fShuntVolt1*0.000007629f*1000.0f;
    }
    else//为负压
    {
        fShuntVolt1 = u32ShuntCurrentAd1 & 0x7fff;
        fShuntVolt1 = (fShuntVolt1*0.000007629f-0.25f)*1000.0f;
    }
    InPutBatShuntVolt(fShuntVolt1);//单位mv

    fShuntCurrent1 = CalcShuntCurrent(fShuntVolt1, fShuntVoltRange, fShuntCurrRange);
    InPutBatShuntCurrent(fShuntCurrent1);

    if(fShuntVolt1 > 0)//大于0，放电
    {
        fShuntVolt1 = CalibVoltCheck(fShuntVolt1, stParameter.BatShuntDhgKBFlag, stParameter.fBatShuntCurrentDhgK, stParameter.fBatShuntCurrentDhgB);
    }
    else//小于0，充电
    {
        fShuntVolt1 = CalibVoltCheck(fShuntVolt1, stParameter.BatShuntChgKBFlag, stParameter.fBatShuntCurrentChgK, stParameter.fBatShuntCurrentChgB);
    }
    InPutfAdjustBatShuntVolt(fShuntVolt1);//单位mv
    fShuntCurrent1 = CalcCalibBatShuntCurrent(fShuntVolt1, fShuntVoltRange, fShuntCurrRange);
    InPutfAdjustBatShuntCurrent(fShuntCurrent1);
    s32fShuntCurrent1 = (s32)(fShuntCurrent1*1000);//mA

    /*=== 2. 电池端分流器电流2 (from FIFO[2] C9) ===*/
    for(int i = 0; i < 16; i++)
    {
        if(usSPI_RegFifo_Data[2][i] != 0)
        {
            u32ShuntCurrentAdSum2 += (usSPI_RegFifo_Data[2][i]>>16);
            u8ShuntCurrentCount2++;
        }
    }
    u32ShuntCurrentAd2 = (u8ShuntCurrentCount2 != 0)
        ? (u32ShuntCurrentAdSum2/u8ShuntCurrentCount2) : 0x8000;

    if(0x8000 == (u32ShuntCurrentAd2 & 0x8000))//为正压
    {
        fShuntVolt2 = u32ShuntCurrentAd2 & 0x7fff;
        fShuntVolt2 = fShuntVolt2*0.000007629f*1000.0f;
    }
    else//为负压
    {
        fShuntVolt2 = u32ShuntCurrentAd2 & 0x7fff;
        fShuntVolt2 = (fShuntVolt2*0.000007629f-0.25f)*1000.0f;
    }
    InPutfBatShuntVoltBackup(fShuntVolt2);//单位mv

    fShuntCurrent2 = CalcShuntCurrent(fShuntVolt2, fShuntVoltRange, fShuntCurrRange);
    InPutfBatShuntCurrentBackup(fShuntCurrent2);

    if(fShuntVolt2 > 0)//大于0，放电
    {
        fShuntVolt2 = CalibVoltCheck(fShuntVolt2, stParameter.BatShuntDhgKBFlag, stParameter.fBatShuntCurrentDhgK, stParameter.fBatShuntCurrentDhgB);
    }
    else//小于0，充电
    {
        fShuntVolt2 = CalibVoltCheck(fShuntVolt2, stParameter.BatShuntChgKBFlag, stParameter.fBatShuntCurrentChgK, stParameter.fBatShuntCurrentChgB);
    }
    InPutfAdjustBatShuntVoltBackup(fShuntVolt2);//单位mv
    fShuntCurrent2 = CalcCalibBatShuntCurrent(fShuntVolt2, fShuntVoltRange, fShuntCurrRange);
    InPutfAdjustBatShuntCurrentBackup(fShuntCurrent2);
    s32fShuntCurrent2 = (s32)(fShuntCurrent2*1000.0f);//mA

    /* 分流器数据选择: 与霍尔偏差小的作为有效值 */
    s32HallCurrent = (s32)(GetfAdjustHallCurrent()*1000.0f);
    if(ABS(s32HallCurrent,s32fShuntCurrent2) >= ABS(s32HallCurrent,s32fShuntCurrent1))
    {
        InPutSystemDataArray(BatShuntCurrentL,(s16)(s32fShuntCurrent1&0xffff));
        InPutSystemDataArray(BatShuntCurrentH,(s16)((s32fShuntCurrent1>>16)&0xffff));
    }
    else
    {
        InPutSystemDataArray(BatShuntCurrentL,(s16)(s32fShuntCurrent2&0xffff));
        InPutSystemDataArray(BatShuntCurrentH,(s16)((s32fShuntCurrent2>>16)&0xffff));
    }

    /* 分流器自检: 两路分流器差异检查 */
    if(ABS(s32fShuntCurrent1, s32fShuntCurrent2) <= BATSHUNT_SELF_TEST)
    {
        DiagSetErrNowStateAPI(eERR_ID_CUR_SAM, eErr_A);
    }
    else
    {
        DiagSetErrNowStateAPI(eERR_ID_CUR_SAM, eErr_B);
    }

    /* 分流器采样值范围检查 */
    if((fShuntVolt2 >= BATSHUNT_SAMP_MIN && fShuntVolt2 <= BATSHUNT_SAMP_MAX)
        || (fShuntVolt1 >= BATSHUNT_SAMP_MIN && fShuntVolt1 <= BATSHUNT_SAMP_MAX))
    {
        DiagSetErrNowStateAPI(eERR_ID_SHU_BSA, eErr_A);
    }
    else
    {
        DiagSetErrNowStateAPI(eERR_ID_SHU_BSA, eErr_B);
    }

    /*电池端分流器与霍尔采样偏差过大*/
    if(ABS(s32HallCurrent, s32fShuntCurrent2) >= ABS(s32HallCurrent, s32fShuntCurrent1))
    {
        if(ABS(s32HallCurrent, s32fShuntCurrent1) < BATSHUNT_HALL_DIFF)
            DiagSetErrNowStateAPI(eERR_ID_SHU_DIF, eErr_A);
        else
            DiagSetErrNowStateAPI(eERR_ID_SHU_DIF, eErr_B);
    }
    else
    {
        if(ABS(s32HallCurrent, s32fShuntCurrent2) < BATSHUNT_HALL_DIFF)
            DiagSetErrNowStateAPI(eERR_ID_SHU_DIF, eErr_A);
        else
            DiagSetErrNowStateAPI(eERR_ID_SHU_DIF, eErr_B);
    }

    /*=== 3. 负载端分流器电流 (from Rn8209AdData[1] IA) ===*/
    if(Rn8209AdData[1] != 0)
    {
        f32 f32ShuntVolt, f32ShuntCurrent, f32ShuntAdjustVolt, f32ShuntAdjustCurrent;
        s32 s32LinkShuntCurrent = 0;

        f32ShuntVolt = GetCurrentViaAd(Rn8209AdData[1]);
        InPutLinkShuntVolt(f32ShuntVolt);

        f32ShuntCurrent = CalcShuntCurrent(f32ShuntVolt, fShuntVoltRange, fShuntCurrRange);
        InPutLinkShuntCurrent(f32ShuntCurrent);

        if(f32ShuntVolt > 0)//大于0，放电
        {
            f32ShuntAdjustVolt = CalibVoltCheck(f32ShuntVolt, stParameter.LinkShuntDhgKBFlag, stParameter.fLinkShuntCurrentDhgK, stParameter.fLinkShuntCurrentDhgB);
        }
        else//小于0，充电
        {
            f32ShuntAdjustVolt = CalibVoltCheck(f32ShuntVolt, stParameter.LinkShuntChgKBFlag, stParameter.fLinkShuntCurrentChgK, stParameter.fLinkShuntCurrentChgB);
        }

        f32ShuntAdjustCurrent = CalcCalibLinkShuntCurrent(f32ShuntAdjustVolt, fShuntVoltRange, fShuntCurrRange);
        InPutfAdjustLinkShuntVolt(f32ShuntAdjustVolt);
        InPutfAdjustLinkShuntCurrent(f32ShuntAdjustCurrent);

        s32LinkShuntCurrent = (s32)(f32ShuntAdjustCurrent*1000);
        InPutSystemDataArray(LinkShuntCurrentL,(s16)(s32LinkShuntCurrent&0xffff));
        InPutSystemDataArray(LinkShuntCurrentH,(s16)((s32LinkShuntCurrent>>16)&0xffff));

        /* 负载端分流器采样范围检查 */
        if((f32ShuntVolt >= LINKSHUNT_SAMP_MIN) && (f32ShuntVolt <= LINKSHUNT_SAMP_MAX))
            DiagSetErrNowStateAPI(eERR_ID_SHU_LSA, eErr_A);
        else
            DiagSetErrNowStateAPI(eERR_ID_SHU_LSA, eErr_B);
    }
}





/*=============================================================================================
** @Name      : void TASK_MeasureProcTask (void)
** @Input     : void
** @Output    : void
** @Function  : 低压采样任务
** @The notes :
**===========================================================================================*/
void TASK_MeasureProcTask (void)
{
    BSPADCGetSampData();//ADC
    TempCalculate();
    HallCalculate();
    GetSystemInfoTask();

}

/*=============================================================================================
** @Name      : void GetSystemInfoTask (void)
** @Input     : void
** @Output    : void
** @Function  : 系统信息获取任务
** @The notes :
**===========================================================================================*/
void GetSystemInfoTask(void)
{
    RefreshDI();//读取DI1
    RefreshHall_Pfault();//Hall_Pfault
    BoardReadInfoSync();//获取拨码值计算地址、获取系统运行时间  
}

/*=============================================================================================
** @Name      : void TempADSample(void)
** @Input     : void
** @Output    : void
** @Function  : 温度计算任务
** @The notes :
**===========================================================================================*/
void TempCalculate(void)
{
    f32 ftemp=0,fResist=0,fvolt=0;
    u16 index=0;

    /*1. ntc1温度计算*/
    if (adc_value_buffer[Ch8Value] >= 4040)//认为断路 
    {
        InPutNtcTemp(0,-50);//断路温度-50度
        InPutSystemDataArray(NtcTemp1,(s16)(-500));//传输数据温度输入

    }
    else if(adc_value_buffer[Ch8Value] <= 100)//认为短路
    {
        InPutNtcTemp(0,206);//短路温度206度
        InPutSystemDataArray(NtcTemp1,(s16)(2060));//传输数据温度输入
    }
    else//NTC温度计算
    {
        fvolt = ((3.3*adc_value_buffer[Ch8Value])/4095)*1000;
        fResist = 10000 * fvolt / (3300.0f - fvolt);
        index = Measure_SearchNtcTempTabByR((u32)fResist,sNTCBalTTabR,NTC_T_TAB_LEN);
        if((index > 0) && (index < (NTC_T_TAB_LEN-1)))
        {
            ftemp = index - 40 + (sNTCBalTTabR[index] - fResist)/(sNTCBalTTabR[index]-sNTCBalTTabR[index+1]);//精确到浮点型
        }else
        {
            ftemp = index -40;
        }
        InPutNtcTemp(0,ftemp);
        InPutSystemDataArray(NtcTemp1,(s16)(ftemp*10));//传输温度电压输入
    }
    
   /*2. ntc2温度计算*/
    if (adc_value_buffer[Ch9Value] >= 4040)//认为断路 
    {
        InPutNtcTemp(1,-50);//断路温度-50度
        InPutSystemDataArray(NtcTemp2,(s16)(-500));//传输数据电压输入

    }
    else if(adc_value_buffer[Ch9Value] <= 100)//认为短路
    {
        InPutNtcTemp(1,206);//短路温度206度
        InPutSystemDataArray(NtcTemp2,(s16)(2060));//传输数据电压输入
    }
    else//NTC温度计算
    {
        fvolt = ((3.3*adc_value_buffer[Ch9Value])/4095)*1000;
        fResist = 10000 * fvolt / (3300.0f - fvolt);
        index = Measure_SearchNtcTempTabByR((u32)fResist,sNTCBalTTabR,NTC_T_TAB_LEN);
        if((index > 0) && (index < (NTC_T_TAB_LEN-1)))
        {
            ftemp = index - 40 + (sNTCBalTTabR[index] - fResist)/(sNTCBalTTabR[index]-sNTCBalTTabR[index+1]);//精确到浮点型
        }else
        {
            ftemp = index -40;
        }
        InPutNtcTemp(1,ftemp);
        InPutSystemDataArray(NtcTemp2,(s16)(ftemp*10));//传输数据电压输入
    }
}
/*=============================================================================================
** @Name      : void HallCalculate(void)
** @Input     : void
** @Output    : void
** @Function  : 计算霍尔电流、霍尔供电自检、5v供电自检
** @The notes : 用户调用
**===========================================================================================*/
void HallCalculate(void)
{
    f32 fcurrent = 0,  fvolt= 0;
    s32 s32HallCurrent;

    f32 fZeroVolt=0, fVoltRange=0, fCurrRange = 0;
    /*获取零点电压、霍尔电压单向范围、霍尔电流单向量程*/
    fZeroVolt  = (f32)stParameter.u16ArraySet[HallSignalZeroVolt]*100; //(0.1V) 2500
    fVoltRange = (f32)stParameter.u16ArraySet[HallVoltSampRange]; //霍尔电压单向范围 (0.1V) 
    fCurrRange = (f32)stParameter.u16ArraySet[HallCurrentSampRange];//霍尔电流单向范围 (10A)
 
        /*1.1霍尔ad值告警检测*/
        if(adc_value_buffer[Ch10Value] >= HALL_AD_MAX)
        {
            DiagSetErrNowStateAPI(eERR_ID_HAL_ADC,eErr_B);
        } else{
            DiagSetErrNowStateAPI(eERR_ID_HAL_ADC,eErr_A);}
        
        if(adc_value_buffer[Ch10Value]> 1e-6f)
        {
            fvolt = 3300.0f*adc_value_buffer[Ch10Value]/4095.0f/2.0f*3.0f;//HALL电压计算 
        }
   
        /*1.2霍尔零漂校准*/
        // CalcCurrZeroCorr((s32)fvolt);//A

        InPutHallVolt(fvolt);                               //未校准HALL电压 单位mv
        // /*校准前HALL电流计算 */
        fcurrent = CalcHallCurrent(fvolt, fZeroVolt, fVoltRange, fCurrRange);
        InPutHallCurrent(((f32)fcurrent));                  //未校准HALL电流

/*hall工装未校准仍然使用原始测量值计算结果*/
        // /*1.3 校准电压电流计算*/
        // if(1 == stParameter.HallZeroCorrFlag)//判断是否需要霍尔校零 
        // {
        //     if (FABS(stParameter.fHallZeroCorr)> 1e-6f)
        //     {
        //         //f32 corr = stParameter.fHallZeroCorr;
        //         if (FABS(stParameter.fHallZeroCorr) < 25.0f)//判断霍尔校零值是否有效
        //         {
        //             fvolt = fvolt + stParameter.fHallZeroCorr;
        //     }
        //     }
        // }
        
        InPutfAdjustZeroHallVolt(((f32)fvolt));//Hall校零后电压 单位mv
        // fvolt = CalibVoltCheck(fvolt, stParameter.HallCurrKBFlag, stParameter.fHallCurrentK, stParameter.fHallCurrentB);//双点校准
        
        InPutfAdjustHallVolt(((f32)fvolt));//校准后电压

         /*1.4霍尔过载告警检测*/
         if(fvolt >= HALL_OVERL_RANGEL && fvolt <= HALL_OVERL_RANGEH)
         {
             DiagSetErrNowStateAPI(eERR_ID_HAL_OVE,eErr_A);
         }else{
             DiagSetErrNowStateAPI(eERR_ID_HAL_OVE,eErr_B);
 
             fvolt = 0;//霍尔过载告警电压数据清零
         }
        
        /*1.4 校准后电压电流量程检测*/
        /*HALL电流计算 单向电压电流范围量程判断*/
        //fcurrent = CalcCalibHallCurrent(fvolt, fZeroVolt, fVoltRange, fCurrRange);
        InPutfAdjustHallCurrent(((f32)fcurrent));//校准后电流


        InPutSystemDataArray(HallSignalSamp,(s16)fvolt);//HALL信号采样量输入 mV
        s32HallCurrent = (s32)(fcurrent*1000);//mA
        InPutSystemDataArray(HallCurrentL,(s16)(s32HallCurrent & 0xffff));//霍尔电流输入
        InPutSystemDataArray(HallCurrentH,(s16)((s32HallCurrent>>16) & 0xffff));//霍尔电流输入
      


    /*2. 霍尔供电5V*/
    if(adc_value_buffer[Ch11Value] > 1e-6f)
    {
        fvolt = 3.3f*adc_value_buffer[Ch11Value]/4095.0f*2.0f;
    
    }
    InPutHall5Volt(fvolt);
    InPutSystemDataArray(Hall5vSamp,(s16)(fvolt*10));//霍尔5v电压输入 0.1V
    if(fvolt >= HALL5V_RANGEL && fvolt <= HALL5V_RANGEH) //霍尔供电告警检测
    {
        DiagSetErrNowStateAPI(eERR_ID_HAL_SUP,eErr_A);
    }
    else
    {
        DiagSetErrNowStateAPI(eERR_ID_HAL_SUP,eErr_B);
        fvolt = 0;//HALL供电5v超限电压清零
    }


    /*3. Vdd供电5V*/
    if(adc_value_buffer[Ch15Value] > 1e-6f)
    {
        fvolt = 3.3f*adc_value_buffer[Ch15Value]/4095.0f*2.0f;
    }
    InPutVcc5Volt(fvolt);
    InPutSystemDataArray(Vdd5VSamp,(s16)(fvolt*10));//vdd5v电压输入 0.1V
     //vdd5v供电告警检测
     if(fvolt >= HALLVDD5V_RANGEL && fvolt <= HALLVDD5V_RANGEH)
     {
         DiagSetErrNowStateAPI(eERR_ID_BOA_S5V,eErr_A);
     }
     else
     {
         DiagSetErrNowStateAPI(eERR_ID_BOA_S5V,eErr_B);
     }
}

/*=============================================================================================
** @Name      : void Ka84917VoltCurrentCalculate(void)
** @Input     : void
** @Output    : void
** @Function  : KA84917相关电压以及电流计算任务
** @The notes : 电池电压、电池端分流器电压、电池端分流器电流、高压供电5V、PE对地电压、NTC温度
**===========================================================================================*/
void Ka84917VoltCurrentCalculate(void)
{
    //未初始化成功、菊花链通信故障时，应上传默认值
    if ((wakeUpState == 0)
    || (ucSPI_GetComError() != SPI_NOERR ))
    {
        /* 填充所有高压侧数据默认值 */
        InPutSystemDataArray(BatVolt,           0);
        InPutSystemDataArray(BatShuntCurrentL,  0);
        InPutSystemDataArray(BatShuntCurrentH,  0);
        InPutSystemDataArray(Ka84917P5V,        0);
        InPutSystemDataArray(Ka84917N5V,        0);
        InPutSystemDataArray(ShuntTemp,         0);
        //复位gSystemData
        InPutfAdjustBatVolt(0);
        InPutBatVolt(0);
        InPutPeGndVolt(0);
        InPutfAdjustPeGndVolt(0);
        InPutNpeVolt(0);
        /* 置位菊花链通信故障标志 */
        BitSet16(SystemData_51[DidoGpioState], DaisyComFail);
        return;  // ← 跳过后续计算
    }
    if (FirstSampleFinishedFlag == 0)//初始化成功后但数据采样未更新一轮
    {
         /* 填充所有高压侧数据默认值 */
         InPutSystemDataArray(BatVolt,           0);
         InPutSystemDataArray(BatShuntCurrentL,  0);
         InPutSystemDataArray(BatShuntCurrentH,  0);
         InPutSystemDataArray(Ka84917P5V,        0);
         InPutSystemDataArray(Ka84917N5V,        0);
         InPutSystemDataArray(ShuntTemp,         0);

         //复位gSystemData
        // InPutBatVolt(0);
        InPutfAdjustBatVolt(0);
        // InPutBatShuntCurrent(0);
        // InPutfAdjustBatShuntCurrent(0);
        // InPutfBatShuntCurrentBackup(0);
        // InPutfAdjustBatShuntCurrentBackup(0);

        // InPutHvP5Volt(0);
        // InPutHvN5Volt(0);
        // InPutPeGndVolt(0);
        InPutfAdjustPeGndVolt(0);
        // InPutNpeVolt(0);
         return;
    }
    
    /* 通信正常，清除 DaisyComFail */
    BitClr16(SystemData_51[DidoGpioState], DaisyComFail);

    f32 fResist=0,fvolt=0,ftemp=0,fBatDividVolt = 0,
    fBatVolt = 0,
    fHvP5vVolt=0,
    fHvN5vVolt=0,
    fNpeVolt=0,
    fPeGndVolt=0;
    u32 u32PeVoltAdSum = 0;
    u8 u8PeVoltCount = 0,index=0;
 

    /*1.电池端电压BatVolt 128*/
    fBatDividVolt = usSPI_Reg_ReadData[0][0x80]*0.000083923f;//128
    fBatVolt=(fBatDividVolt-HV_REF*1.1f/11.1f)*(11.1f/10.0f)*(6736.0f/16.0f);//BAT电压计算
    InPutBatVolt(fBatVolt);//校准前电池电压 

   //电压滤波 if ((GetFilterValue(fBatVolt, &fBatAverageVolt, &stBatVoltFilter) == TRUE)) {}
   
    /* 开路时原始值在[-8V, 0V]范围内，不做校准直接输出0V，避免校准偏置B导致虚报电压 */
    /* 开路零位屏蔽暂时关闭，直接走校准
    if(fBatVolt >= BAT_VOLT_ZERO_RANGEL && fBatVolt <= BAT_VOLT_ZERO_RANGEH)
    {
        fBatVolt = 0.0f;
    }
    else
    {
        fBatVolt = CalibVoltCheck(fBatVolt, stParameter.BatVoltKBFlag, stParameter.fBatVoltK, stParameter.fBatVoltB);//双点校准
    }
    */
    fBatVolt = CalibVoltCheck(fBatVolt, stParameter.BatVoltKBFlag, stParameter.fBatVoltK, stParameter.fBatVoltB);//双点校准

   
    InPutfAdjustBatVolt(fBatVolt);//校准后电池电压
    InPutSystemDataArray(BatVolt,(s16)(fBatVolt*10));//0.1V
    //Bat端高压告警检测
    if(fBatVolt >= BAT_VOLT_RANGEL && fBatVolt <= BAT_VOLT_RANGEH)
    {
        DiagSetErrNowStateAPI(eERR_ID_BAT_VOL,eErr_A);
    }
    else
    {
        DiagSetErrNowStateAPI(eERR_ID_BAT_VOL,eErr_B);
    }

    /* 分流器电流计算已移至 CalcFast3Results() 中统一处理, 此处仅保留高压侧电压/温度/NPE计算 */
    /*4.电池高压+5v自检 */
    if(usSPI_Reg_ReadData[0][0x7F] != 0)
    {
        fHvP5vVolt = usSPI_Reg_ReadData[0][0x7F]*0.000083923f/10.0f*20.0f;//127
    }
    InPutHvP5Volt(fHvP5vVolt);
    InPutSystemDataArray(Ka84917P5V,(s16)(fHvP5vVolt*10));//传输数据电压输入

    //电池高压5v自检告警检测
    if(fHvP5vVolt >= BATSHUNT_5V_RANGEL && fHvP5vVolt <= BATSHUNT_5V_RANGEH)
    {
        DiagSetErrNowStateAPI(eERR_ID_917_P5V,eErr_A);
    }
    else
    {
        DiagSetErrNowStateAPI(eERR_ID_917_P5V,eErr_B);
    }

    /*5.电池高压-5v自检 */
    if(usSPI_Reg_ReadData[0][0x81] != 0)
    {
        fHvN5vVolt = (usSPI_Reg_ReadData[0][0x81]*0.000083923f-(fHvP5vVolt*(10.0f*10.0f/(10.0f+10.0f)/(10.0f*10.0f/(10.0f+10.0f)+5.6f))))/(10.0f*5.6f/(10.0f+5.6f)/(10.0f*5.6f/(10.0f+5.6f)+10.0f));//有偏移，约为1.037V //129
        fHvN5vVolt = -1*fHvN5vVolt;//取绝对值
    }
    InPutHvN5Volt(fHvN5vVolt);
    InPutSystemDataArray(Ka84917N5V,(s16)(fHvN5vVolt*10));//传输数据电压输入
    //电池高压-5v自检告警检测
    if(fHvN5vVolt >= BATSHUNT_5V_RANGEL && fHvN5vVolt <= BATSHUNT_5V_RANGEH)
    {
        DiagSetErrNowStateAPI(eERR_ID_917_N5V,eErr_A);
    }
    else
    {
        DiagSetErrNowStateAPI(eERR_ID_917_N5V,eErr_B);
    }

    /*6.NPE_AD */
    for(int i = 0; i < 16; i++)
    {
            if(usSPI_RegFifo_Data[1][i] != 0)
            {
                u32PeVoltAdSum += (usSPI_RegFifo_Data[1][i]>>16);
                u8PeVoltCount++;
            }  
    }
    if(u8PeVoltCount != 0 )
    {
        fNpeVolt = u32PeVoltAdSum*0.000083923f/u8PeVoltCount;
        InPutNpeVolt(fNpeVolt);        // VNPE_AD
      fPeGndVolt = fNpeVolt/6.2f*2606.2f; //VRD
    }else
    {
    fPeGndVolt = 0;
    }
    /*校准前PEGndVolt*/
    InPutPeGndVolt(fPeGndVolt);

    /* PE对地电压绝对值在1V以内，不做校准直接输出0V */
    /* 开路零位屏蔽暂时关闭，直接走校准
    if(fPeGndVolt >= -PE_VOLT_ZERO_RANGE && fPeGndVolt <= PE_VOLT_ZERO_RANGE)
    {
        fPeGndVolt = 0.0f;
    }
    else
    {
        fPeGndVolt = CalibVoltCheck(fPeGndVolt, stParameter.PeVoltKBFlag, stParameter.fPeVoltK, stParameter.fPeVoltB);//双点校准
    }
    */
    fPeGndVolt = CalibVoltCheck(fPeGndVolt, stParameter.PeVoltKBFlag, stParameter.fPeVoltK, stParameter.fPeVoltB);//双点校准

    /*校准后PEGNDVolt*/
    InPutfAdjustPeGndVolt(fPeGndVolt);
    InPutSystemDataArray(PeGndVolt,(s16)(fPeGndVolt*10));//传输数据电压输入

    /*7.电池端分流器温度 125*/
    if (usSPI_Reg_ReadData[0][0x7D] >= 59000)//认为断路 //125
    {
        InPutBatShuntTemp(-50);//断路温度-50度
        InPutSystemDataArray(ShuntTemp,(s16)(-500));//传输数据电压输入

    }
    else if(usSPI_Reg_ReadData[0][0x7D] <= 100)//认为短路
    {
        // InPutBatShuntTemp(206);//短路温度206度
        // InPutSystemDataArray(ShuntTemp,(s16)(2060));//传输数据电压输入
        InPutBatShuntTemp(206);//短路温度206度
        InPutSystemDataArray(ShuntTemp,(s16)(2060));//传输数据电压输入
    }
    else
    {
        //NTC温度计算
        fvolt = (usSPI_Reg_ReadData[0][0x7D]*0.000083923f)*1000.0f;//单位mv
        fResist = 10000.0f * fvolt / (fHvP5vVolt*1000.0f - fvolt);
        index = Measure_SearchNtcTempTabByR((u32)fResist,sNTCBalTTabR,NTC_T_TAB_LEN);
        if((index > 0) && (index < (NTC_T_TAB_LEN-1)))
        {
            ftemp = index - 40 + (sNTCBalTTabR[index] - fResist)/(sNTCBalTTabR[index]-sNTCBalTTabR[index+1]);//精确到浮点型
        }else
        {
            ftemp = index - 40;
        }
        InPutBatShuntTemp(ftemp);
        InPutSystemDataArray(ShuntTemp,(s16)(ftemp*10));//传输数据电压输入
    }
    //  SEGGER_RTT_printf(0, "Reg80=0x%04X  Reg7D=0x%04X  BatDiv=%.3f\r\n",
    // usSPI_Reg_ReadData[0][0x80],
    // usSPI_Reg_ReadData[0][0x7D],
    // (double)(usSPI_Reg_ReadData[0][0x80] * 0.000083923f));
    
}

 //  for(int i = 0; i < 16; i++)
    //  {
    //     if(0 != usSPI_RegFifo_Data[1][i])
    //     {
    //         u32PeVoltAdSum += (usSPI_RegFifo_Data[1][i]>>16);
    //         u8PeVoltCount++;
    //     }    
    //  }
    // if(0 != u8PeVoltCount)
    // {
    //     fBatDividVolt = u32PeVoltAdSum/u8PeVoltCount*0.000083923;
    // }else
    // {
    //     fBatDividVolt = 0;
    // }    
    //fBatVolt=(fBatDividVolt-HV_REF*1.1/11.1)*(11.1/10)*(2586.2/6.2);
    // fBatAverageVolt = fBatVolt;

    // fNpeVolt = usSPI_Reg_ReadData[0][0x80]*0.000083923;
    //fPeGndVolt = fNpeVolt/6.2*2606.2;


/*=============================================================================================
** @Name      : void Rn8209VoltCurrentCalculate(void)
** @Input     : void
** @Output    : void
** @Function  : RN8209相关电压以及电流计算任务
** @The notes : 
**===========================================================================================*/
void Rn8209VoltCurrentCalculate(void)
{
//未初始化成功、菊花链通信故障时，应上传默认值
 if ((wakeUpState == 0)
 || (ucSPI_GetComError() != SPI_NOERR ))
 {
     /* 填充所有高压侧数据默认值 */
     InPutSystemDataArray(LinkVolt,(s16)(0));//外端电压输入
     InPutSystemDataArray(LinkShuntCurrentL,(s16)(0));//外端电流输入
     InPutSystemDataArray(LinkShuntCurrentH,(s16)(0));//外端电流输入
    /* 同步复位 gSystemData */
    // InPutfAdjustLinkVolt(0);
    // InPutLinkVolt(0);
    // InPutfAdjustLinkShuntCurrent(0);
    // InPutLinkShuntCurrent(0);   
     return;  // ← 跳过后续计算
 }

    float32 f32Volt=0,f32AdjustVolt=0; 

/*1.Link端电压 */
    if(Rn8209AdData[0] != 0)
    {
        f32Volt = GetVoltViaAd(Rn8209AdData[0]);
        /*校准前LinkVolt*/
            InPutLinkVolt(f32Volt);

            /* 开路时原始值在[-8V, 0V]范围内，不做校准直接输出0V，避免校准偏置B导致虚报电压 */
            /* 开路零位屏蔽暂时关闭，直接走校准
            if(f32Volt >= LINK_VOLT_ZERO_RANGEL && f32Volt <= LINK_VOLT_ZERO_RANGEH)
            {
                f32AdjustVolt = 0.0f;
            }
            else
            {
                f32AdjustVolt = CalibVoltCheck(f32Volt, stParameter.LinkVoltKBFlag, stParameter.fLinkVoltK, stParameter.fLinkVoltB);//双点校准
            }
            */
            f32AdjustVolt = CalibVoltCheck(f32Volt, stParameter.LinkVoltKBFlag, stParameter.fLinkVoltK, stParameter.fLinkVoltB);//双点校准
            
            /*校准后LinkVolt*/
            InPutfAdjustLinkVolt(f32AdjustVolt);
            InPutSystemDataArray(LinkVolt,(s16)(f32AdjustVolt*10));//外端电压输入
    }

    //Link端高压告警检测
    if(f32AdjustVolt >= LINK_VOLT_RANGEL && f32AdjustVolt <= LINK_VOLT_RANGEH) //电压-5-1800之间认为是正常
    {
        DiagSetErrNowStateAPI(eERR_ID_LIN_VOL,eErr_A);
    }
    else
    {
        DiagSetErrNowStateAPI(eERR_ID_LIN_VOL,eErr_B);
    }

    /* 负载端分流器电流计算已移至 CalcFast3Results() 中统一处理 */
}


/*=============================================================================================
** @Name      : void RefreshDI(void)
** @Input     : void
** @Output    : void
** @Function  : 读取DI1
** @The notes : 用户调用
**===========================================================================================*/
void RefreshDI(void)
{
    InPutDiState(DI1_Pin_Read());
    if(DI1_Pin_Read())
    {
        BitSet16(SystemData_51[DidoGpioState],Di1);//记录状态,高电平为1
    }
    else
    {
        BitClr16(SystemData_51[DidoGpioState],Di1);//记录状态
    }
}

/*=============================================================================================
** @Name      : void RefreshHall_Pfault(void)
** @Input     : void
** @Output    : void
** @Function  : 读取HALL_PFAULT
** @The notes : 用户调用
**===========================================================================================*/
void RefreshHall_Pfault(void)
{
    if(HALL_PFAULT_Pin_Read())
    {
        BitSet16(SystemData_51[DidoGpioState],HallFailSignal);//记录状态,高电平为1
    }
    else
    {
        BitClr16(SystemData_51[DidoGpioState],HallFailSignal);//记录状态
    }
}



/*=============================================================================================
** @Static function definition
**===========================================================================================*/

/*=============================================================================================
** @Name      : static f32 CalibVoltCheck(f32 Volt, u32 KBFlag, f32 K, f32 B)
** @Input     : 电压值 校准标志 K B
** @Output    : 校准后电压
** @Function  : 计算校准电压
** @The notes : 判断是否满足校准条件 计算校准后电压
**===========================================================================================*/
static f32 CalibVoltCheck(f32 Volt, u32 KBFlag, f32 K, f32 B)
{
    if((1 == KBFlag) && (IsCoefValid(K, B) == TRUE))//判断kb值是否有效
    {
        Volt = (f32)(Volt*K + B);
    }else
    {
        Volt = (f32)(Volt);
    }
    return Volt;
}

/*=============================================================================================
** @Name      : static f32 CalcHallCurrent(f32 volt, f32 ZeroVolt, f32 VoltRange, f32 CurrRange)
** @Input     : void
** @Output    : 校准前HALL电流计算结果
** @Function  : 校准前Hall电流计算
** @The notes : 
**===========================================================================================*/
static f32 CalcHallCurrent(f32 volt, f32 ZeroVolt, f32 VoltRange, f32 CurrRange)
{
   
    f32 current = 0.0f;

    if((FABS(volt) > 1e-6f)
    &&(FABS(ZeroVolt) > 1e-6f)
    &&(FABS(VoltRange) > 1e-6f)
    &&(FABS(CurrRange) > 1e-6f))
    {
        current = (volt - ZeroVolt) * CurrRange / VoltRange / 10.0f;  /* 计算电流 A */ 
        return current;
    }
    return 0;
}

/*=============================================================================================
** @Name      : static f32 CalcCalibHallCurrent(f32 volt, f32 ZeroVolt, f32 VoltRange, f32 CurrRange
** @Input     : void
** @Output    : 校准后HALL电流计算结果
** @Function  : 校准后Hall电流计算
** @The notes : 判断HALL电压电流是否超量程，超量程电流清零
**===========================================================================================*/
static f32 CalcCalibHallCurrent(f32 volt, f32 ZeroVolt, f32 VoltRange, f32 CurrRange)
{
    f32 VoltOffest = 0;
    f32 current = 0.0f;


    f32 VoltRange_mV = VoltRange * 100.0f;
    f32 CurrRange_A = CurrRange * 10.0f;
    
    if ((FABS(volt) > 1e-6f) 
     && (FABS(ZeroVolt) > 1e-6f)  
     && (FABS(VoltRange) > 1e-6f) 
     && (FABS(CurrRange) > 1e-6f))
    {
        VoltOffest = volt - ZeroVolt;//

        if ((FABS(VoltOffest) <= VoltRange_mV))//判断电压范围 mV
        {
            current = (volt - ZeroVolt) * CurrRange / VoltRange / 10.0f;  /* 计算电流 A */ 

            if ((FABS(current) >= HALL_C_SAMP_RANGDEL)&& (FABS(current) <= CurrRange_A))
            {
                return current;
            }
            
        }
    }
    return 0;

}
/*=============================================================================================
** @Name      : static f32 CalcShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
** @Input     : ShuntVolt VoltRange CurrRange
** @Output    : 校准前分流器电流计算结果
** @Function  : 校准前分流器电流计算
** @The notes : 
**===========================================================================================*/
static f32 CalcShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
{
    f32 ShuntCurrent = 0;

    if((FABS(ShuntVolt) > 1e-6f)
    &&(FABS(VoltRange) > 1e-6f)
    &&(FABS(CurrRange) > 1e-6f))
    {
        ShuntCurrent = ShuntVolt * CurrRange / VoltRange;//单位A
        return ShuntCurrent;
    }
    return 0;
}
/*=============================================================================================
** @Name      : static f32 CalcCalibBatShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
** @Input     : ShuntVolt VoltRange CurrRange
** @Output    : 校准后分流器电流计算结果
** @Function  : 校准后分流器电流计算
** @The notes : 判断BAT分流器电压电流是否超量程，超量程电流清零
**===========================================================================================*/
static f32 CalcCalibBatShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
{
    f32 ShuntCurrent = 0;

    if((FABS(ShuntVolt) > 1e-6f)
    &&(FABS(VoltRange) > 1e-6f)
    &&(FABS(CurrRange) > 1e-6f))
    {
        if ((FABS(ShuntVolt) <= VoltRange))
        {
            ShuntCurrent = ShuntVolt * CurrRange / VoltRange;//单位A

        if ((FABS(ShuntCurrent) >= BATSHUNT_C_SAMP_RANGDEL) && (FABS(ShuntCurrent) <= CurrRange))
            {
                return ShuntCurrent;
            }
        }
    }

    return 0;
}
/*=============================================================================================
** @Name      : static f32 CalcCalibLinkShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
** @Input     : ShuntVolt VoltRange CurrRange
** @Output    : 校准后分流器电流计算结果
** @Function  : 校准后分流器电流计算
** @The notes : 判断Link分流器电压电流是否超量程，超量程电流清零
**===========================================================================================*/
static f32 CalcCalibLinkShuntCurrent(f32 ShuntVolt, f32 VoltRange, f32 CurrRange)
{
    f32 ShuntCurrent = 0;

    if((FABS(ShuntVolt) > 1e-6f)
        &&(FABS(VoltRange) > 1e-6f)
        &&(FABS(CurrRange) > 1e-6f))
    {
        if ((FABS(ShuntVolt) <= VoltRange))
        {
            ShuntCurrent = ShuntVolt * CurrRange / VoltRange;//单位A mV

        if ((FABS(ShuntCurrent) >= LINKSHUNT_C_SAMP_RANGDEL) && (FABS(ShuntCurrent) <= CurrRange))
        {
            return ShuntCurrent;
        }
            
        }
    }
    return 0;
}





/*=============================================================================================
** @Name      :static void BoardReadInfoSync(void)
** @Input     : 
** @Output    : 
** @Function  : 获取拨码值计算地址、获取系统运行时间   
** @The notes : 
**===========================================================================================*/
static void BoardReadInfoSync(void)
{
    InPutSwAddr(SWReadData());
    InPutSystemInfoArray(DeviceAddr ,(u16)(GetSwAddr()+20));
   
    u32 run_timers = GetRunTimers();
   
    InPutSystemInfoArray(RunTimesL ,(u16)(run_timers & 0xffff));
    InPutSystemInfoArray(RunTimesH ,(u16)((run_timers >> 16) & 0xffff));
}

/*==================================================================
** @Name      : static u16 Measure_SearchNtcTempTabByR(UINT32 data, const uint32 u8Table[],u8 tablesize)
** @Input     : data:ntc电阻值，u8Table：ntc电阻与温度关系表, tablesize:表长
** @Output    : 
** @Function  : 获取ntc阻值对应温度下标
** @The notes : 
**================================================================*/
static u16 Measure_SearchNtcTempTabByR(UINT32 data, const uint32 u8Table[],u8 tablesize)
{
	u16 low = 0,high = tablesize-1,mid;
    if(tablesize == 1 || data > u8Table[0])
    {
        return 0;
    }
    while(low <= high)
    {
        mid = (low + high)/2;
        if(data <= u8Table[mid] && data > u8Table[mid+1])
        {
            return mid;
        }else if(data > u8Table[mid])
        {
            high = mid -1;
        }else
        {
            low = mid + 1;
        }
    }
	return(tablesize-1);
}

static float32 GetVoltViaAd(uint32 u32Ad)
{
    uint32 u32AdDealVal;
    float32 f32Volt;

    u32AdDealVal = u32Ad & 0x7FFFFU;
    if (u32AdDealVal & 0x40000U) { // 负数
        u32AdDealVal = (~(u32AdDealVal - 1U))& 0x3FFFFU;
        f32Volt = AD_MAX_VOLT * u32AdDealVal / AD_MAX_AD * VOLT_RATE;
        f32Volt = -f32Volt;
    } else { //正数
        f32Volt = AD_MAX_VOLT * u32AdDealVal / AD_MAX_AD * VOLT_RATE;
    }
    return f32Volt;
}

static float32 GetCurrentViaAd(uint32 u32Ad)
{
    uint32 u32AdDealVal;
    float32 f32CurrentVolt;
    

    u32AdDealVal = u32Ad & 0x7FFFFU;
    if (u32AdDealVal & 0x40000U) { // 负数
        u32AdDealVal = (~(u32AdDealVal - 1U))& 0x3FFFFU;
        f32CurrentVolt = AD_MAX_VOLT * u32AdDealVal / AD_MAX_AD*1000/CURRENT_RATE;//单位mv
        f32CurrentVolt = -f32CurrentVolt;
    } else { //正数
        f32CurrentVolt = AD_MAX_VOLT * u32AdDealVal / AD_MAX_AD*1000/CURRENT_RATE;//单位mv
    }

    return f32CurrentVolt;
}

// //引用返回pf32Output
// static bool GetFilterValue(float32 f32Input, float32 *pf32Output, St_FilterType *pstFilter)
// {
//     uint8 i;
//     float32 f32Sum = 0;
//     if (pstFilter->u8InputCount == 0) {
//         pstFilter->f32MaxVal = -10000.0f;
//         pstFilter->f32MinVal = 10000.0f;
//     }
//     if (f32Input > pstFilter->f32MaxVal) {
//         pstFilter->f32MaxVal = f32Input;
//     }
//     if (f32Input < pstFilter->f32MinVal) {
//         pstFilter->f32MinVal = f32Input;
//     }
//     pstFilter->f32InputVal[pstFilter->u8InputCount] = f32Input;
//     pstFilter->u8InputCount++;
//     if (pstFilter->u8InputCount == 5U) {
//         for (i = 0; i < 5U; i++) {
//             f32Sum += pstFilter->f32InputVal[i];
//         }
//         f32Sum -= pstFilter->f32MaxVal;
//         f32Sum -= pstFilter->f32MinVal;
//         *pf32Output = f32Sum / 3U;
//         pstFilter->u8InputCount = 0;
//         return TRUE;
//     } else {
//         return FALSE;
//     }
// }

// static bool IsCoefValid(float32 f32K, float32 f32B)
// {
//     if ((f32K > 0.8f) && (f32K < 1.3f) && (f32B > -50.0f) && (f32B < 50.0f)) {//此次b值偏移有时候超过10V
//         return TRUE;
//     } else {
//         return FALSE;
//     }
// }