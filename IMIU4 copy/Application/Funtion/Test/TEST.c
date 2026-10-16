/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : TEST.c
** @Founder      :
** @Date         : 2026.05.25
** @Function     : 测试辅助模块 — 采样周期追踪 + 绝缘检测超时诊断工具
** @Instructions : RTT 命令: test <子命令>
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "Insulation.h"  /* INSULATION_TEST 宏定义在此,必须在 TEST.h 之前 */
#include "TEST.h"
#include "SystemPara.h"
#include "SystemData.h"
#include "SEGGER_RTT.h"
#include <string.h>
#include "BSPDIDO.h"
#include "SystemDiag.h"
#include "BSPKA84917_async.h"
#include <stdlib.h>
#include "DoControl.h"


/* ======================== 采样周期追踪 ======================== */

/* 每个追踪点的上次时间戳 */
static TickType_t gTrackLastTick[TRACK_MAX] = {0};
static u8 gTrackInitDone = 0;

/*
 * @Name     : Track_Sample
 * @Input    : id    — 追踪点ID (TRACK_MAIN_LOOP / TRACK_FULL_ROUND / TRACK_CALC_DONE)
 *             label — 打印标签
 * @Output   : 无
 * @Function : 记录采样周期时间戳, 计算与上次调用的时间间隔, 打印到RTT
 *             用于检测KA84917异步采样流程中各环节的实际耗时
 * @The notes : 在BSPKA84917_async.c的EvtFastSample中各关键点调用
 */
void Track_Sample(TrackerID id, const char *label)
{
    TickType_t now = xTaskGetTickCount();
    TickType_t dt_ms;

    if (id >= TRACK_MAX) return;

    if (!gTrackInitDone)
    {
        u8 i;
        for (i = 0; i < TRACK_MAX; i++)
        {
            gTrackLastTick[i] = now;
        }
        gTrackInitDone = 1;
    }

    dt_ms = (now - gTrackLastTick[id]) * portTICK_PERIOD_MS;
    gTrackLastTick[id] = now;

    SEGGER_RTT_printf(0, "[TRACK] %s  id=%d  dt=%dms  tick=%d\r\n",
                      label, (int)id, (int)dt_ms, (int)now);
}

/*=============================================================================================
** @Static variable definition - 绝缘检测诊断
**===========================================================================================*/
static u8  g_insu_mon_active = 0;        /* 监控运行标志 */
static u16 g_insu_mon_period = 500;      /* 监控周期 (ms) */

/* 电压注入状态 */
static u8    g_insu_inject_enabled = 0;
static float g_insu_inject_bat = 0.0f;
static float g_insu_inject_n2g = 0.0f;

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/

static const char *StateName(u8 s)
{
    switch (s) {
        case 0: return "START";
        case 1: return "PROCESS";
        case 2: return "STOP";
        case 3: return "END";
        case 4: return "OVERTIME";
        case 5: return "FAULT";
        default:return "???";
    }
}

static const char *ModeName(u8 m)
{
    switch (m) {
        case 0: return "AUTO";
        case 1: return "START";
        case 2: return "STOP";
        default:return "???";
    }
}

static const char *StepName(u8 s)
{
    switch (s) {
        case 0: return "STEP1(PE)";
        case 1: return "STEP2(IMD)";
        case 2: return "STEP3(CALC)";
        default:return "???";
    }
}

/*
 * 打印当前绝缘检测完整状态到 RTT
 * 输出: State, Mode, Step, 电压, 电阻, CAN反馈, 超时参数
 */
void Test_InsuDump(void)
{
    u8  state   = gs_InsulationDetectMng.ucInsulationState;
    u8  mode    = gs_InsulationDetectMng.ucInsulationMode;
    u8  step    = gs_InsulationDetectMng.ucInsulationCtrlStep;
    u8  vdelay  = gs_InsulationDetectMng.ucGetVoltDelay;
    u8  vstable = gs_InsulationDetectMng.ucVoltStable;
    u16 timeout = stParameter.u16ArraySet[InsuTimeLimit];
    u16 period  = stParameter.u16ArraySet[InsuSampPeriod];

    SEGGER_RTT_printf(0, "\r\n");
    SEGGER_RTT_printf(0, "+-- Insulation State -------------------------------------+\r\n");
    SEGGER_RTT_printf(0, "| State=%d(%s)  Mode=%d(%s)  Step=%s  VoltDelay=%d  VoltStable=%d\r\n",
                      state, StateName(state), mode, ModeName(mode),
                      StepName(step), vdelay, vstable);
    SEGGER_RTT_printf(0, "| BatVolt=0x%08X  N2GVolt=0x%08X  P2GVolt=0x%08X\r\n",
                      *(u32*)&gs_InsulationDetectMng.fBatVoltage,
                      *(u32*)&gs_InsulationDetectMng.fN2GVoltage,
                      *(u32*)&gs_InsulationDetectMng.fP2GVoltage);
    SEGGER_RTT_printf(0, "| N2GRes=0x%08X  P2GRes=0x%08X  CAN_Feedback=%d\r\n",
                      *(u32*)&gs_InsulationDetectMng.fN2GResistance,
                      *(u32*)&gs_InsulationDetectMng.fP2GResistance,
                      CtrlCmdFeedback_56[CtrlFeedback]);
    SEGGER_RTT_printf(0, "| InsuTimeLimit=%ds  InsuSampPeriod=%ds\r\n",
                      timeout, period);
    SEGGER_RTT_printf(0, "+----------------------------------------------------------+\r\n");
    SEGGER_RTT_printf(0, "\r\n");
}

/*
 * 设置绝缘检测超时时间 (InsuTimeLimit)
 * 测试时改为较小值(如3)可加速超时触发
 */
void Test_InsuSetTimeout(u16 val)
{
    u16 old = stParameter.u16ArraySet[InsuTimeLimit];
    stParameter.u16ArraySet[InsuTimeLimit] = val;
    SEGGER_RTT_printf(0, "[TEST] InsuTimeLimit: %d -> %d (sec)\r\n", old, val);
}

/*
 * 设置绝缘检测采样周期 (InsuSampPeriod)
 * 测试时设为0可跳过周期等待,立即执行
 */
void Test_InsuSetPeriod(u16 val)
{
    u16 old = stParameter.u16ArraySet[InsuSampPeriod];
    stParameter.u16ArraySet[InsuSampPeriod] = val;
    SEGGER_RTT_printf(0, "[TEST] InsuSampPeriod: %d -> %d (sec)\r\n", old, val);
}

/*
 * 设置绝缘检测模式
 * 0=AutoDetect, 1=StartDetect, 2=StopDetect
 * 设为1(StartDetect)可测试超时后永久停止行为
 */
void Test_InsuSetMode(u8 mode)
{
    if (mode > 2)
    {
        SEGGER_RTT_printf(0, "[TEST] Invalid mode: %d (0=Auto 1=Start 2=Stop)\r\n", mode);
        return;
    }
    u8 old = gs_InsulationDetectMng.ucInsulationMode;
    gs_InsulationDetectMng.ucInsulationMode = mode;
    SEGGER_RTT_printf(0, "[TEST] InsulationMode: %d(%s) -> %d(%s)\r\n",
                      old, ModeName(old), mode, ModeName(mode));
}

/*
 * 启动周期性状态监控 (每约500ms打印一次)
 * 需要将 Test_InsuMonPoll() 挂入循环任务才生效
 */
void Test_InsuMonStart(void)
{
    if (g_insu_mon_active)
    {
        SEGGER_RTT_printf(0, "[TEST] Monitoring is already active\r\n");
        return;
    }
    g_insu_mon_active = 1;
    SEGGER_RTT_printf(0, "[TEST] Insulation monitoring started (every %dms)\r\n", g_insu_mon_period);
}

/*
 * 停止周期性状态监控
 */
void Test_InsuMonStop(void)
{
    g_insu_mon_active = 0;
    SEGGER_RTT_printf(0, "[TEST] Insulation monitoring stopped\r\n");
}

/*
 * 监控轮询函数 — 在循环任务中周期性调用
 * 当 g_insu_mon_active=1 时, 每 g_insu_mon_period(500ms) 打印一次状态
 */
void Test_InsuMonPoll(void)
{
    static TickType_t last = 0;
    TickType_t now;

    if (!g_insu_mon_active) return;

    now = xTaskGetTickCount();
    if (last == 0 || ((now - last) * portTICK_PERIOD_MS) >= g_insu_mon_period)
    {
        last = now;
        Test_InsuDump();
    }
}

/* ======================== 继电器状态监控 ======================== */

void Test_PrintRelayState(void)
{
    u8 pe_gpio = PE_RLY_Pin_Read();
    u8 imd_state = GetIMD_RLY_State();
    u8 step = gs_InsulationDetectMng.ucInsulationCtrlStep;
    u8 state = gs_InsulationDetectMng.ucInsulationState;

    SEGGER_RTT_printf(0, "\r");
    SEGGER_RTT_printf(0, "+-- Relay State -------------------------------------------+\r\n");
    SEGGER_RTT_printf(0, "| State=%d(%s)  Step=%s\r\n",
                      state, StateName(state), StepName(step));
    SEGGER_RTT_printf(0, "| PE_RLY_GPIO=%d (%s)  IMD_RLY_STATE=%d (%s)\r\n",
                      pe_gpio, pe_gpio ? "CLOSED" : "OPEN",
                      imd_state, imd_state ? "CLOSED" : "OPEN");
    SEGGER_RTT_printf(0, "| AfterInsuPeRlyState=%d\r\n",
                      stParameter.u16ArraySet[AfterInsuPeRlyState]);
    SEGGER_RTT_printf(0, "+-----------------------------------------------------------+\r\n");
    SEGGER_RTT_printf(0, "\r\n");
}

/* ======================== 诊断告警读取 ======================== */

static const char *DiagStateName(e_ErrState s)
{
    switch (s) {
        case eErr_A:    return "NORMAL(A)";
        case eErr_B:    return "FAULT(B)";
        case eErr_AtoB: return "A->B";
        case eErr_BtoA: return "B->A";
        default:        return "???";
    }
}

void Test_PrintDiagState(void)
{
    e_ErrState insSta = DiagGetErrLastStateAPI(eERR_ID_INS_STA);
    e_ErrState insLow = DiagGetErrLastStateAPI(eERR_ID_INS_LOW);

    SEGGER_RTT_printf(0, "\r\n");
    SEGGER_RTT_printf(0, "+-- Insulation Diag State ---------------------------------+\r\n");
    SEGGER_RTT_printf(0, "| eERR_ID_INS_STA (volt unstable) : %d(%s)\r\n",
                      insSta, DiagStateName(insSta));
    SEGGER_RTT_printf(0, "| eERR_ID_INS_LOW (volt too low)  : %d(%s)\r\n",
                      insLow, DiagStateName(insLow));
    SEGGER_RTT_printf(0, "+-----------------------------------------------------------+\r\n");
    SEGGER_RTT_printf(0, "\r\n");
}

/* ======================== 阈值计算验证 ======================== */

static const char *ThresholdVerdict(float rn, float rp, float alarm, float fault)
{
    if (rn < 0 || rp < 0)         return "FAULT(neg)";
    if (rn < fault || rp < fault)  return "FAULT(<100R/V)";
    if (rn < alarm || rp < alarm)  return "ALARM(<500R/V)";
    return "NORMAL";
}

void Test_PrintThresholdStatus(void)
{
    float bat = gs_InsulationDetectMng.fBatVoltage;
    float rn = gs_InsulationDetectMng.fN2GResistance;
    float rp = gs_InsulationDetectMng.fP2GResistance;
    float rn_last = gs_InsulationDetectMng.fN2GResistanceLast;
    float rp_last = gs_InsulationDetectMng.fP2GResistanceLast;
    float alarm_limit = ALARM_LIMIT_RATE * bat;  /* 500欧/V */
    float fault_limit = FAULT_LIMIT_RATE * bat;  /* 100欧/V */

    SEGGER_RTT_printf(0, "\r\n");
    SEGGER_RTT_printf(0, "+-- Insulation Threshold ----------------------------------+\r\n");
    SEGGER_RTT_printf(0, "| BatVolt=%d.%dV\r\n",
                      (int)bat, (int)((bat - (int)bat) * 100));
    SEGGER_RTT_printf(0, "| AlarmLimit=%dR(500R/V)  FaultLimit=%dR(100R/V)\r\n",
                      (int)alarm_limit, (int)fault_limit);
    SEGGER_RTT_printf(0, "| RN=%d.%d  RP=%d.%d\r\n",
                      (int)rn, (int)((rn - (int)rn) * 100),
                      (int)rp, (int)((rp - (int)rp) * 100));
    SEGGER_RTT_printf(0, "| RN_last=%d.%d  RP_last=%d.%d\r\n",
                      (int)rn_last, (int)((rn_last - (int)rn_last) * 100),
                      (int)rp_last, (int)((rp_last - (int)rp_last) * 100));
    SEGGER_RTT_printf(0, "| Verdict: %s\r\n",
                      ThresholdVerdict(rn, rp, alarm_limit, fault_limit));
    SEGGER_RTT_printf(0, "+-----------------------------------------------------------+\r\n");
    SEGGER_RTT_printf(0, "\r\n");
}

/* ======================== 电压注入 ======================== */

void Test_InsuInjectOn(float batVolt, float n2gVolt)
{
    g_insu_inject_enabled = 1;
    g_insu_inject_bat = batVolt;
    g_insu_inject_n2g = n2gVolt;
    SEGGER_RTT_printf(0, "[TEST] Voltage inject ON: Bat=%d.%d N2G=%d.%d\r\n",
                      (int)batVolt, (int)((batVolt - (int)batVolt) * 100),
                      (int)n2gVolt, (int)((n2gVolt - (int)n2gVolt) * 100));
}

void Test_InsuInjectOff(void)
{
    g_insu_inject_enabled = 0;
    SEGGER_RTT_printf(0, "[TEST] Voltage inject OFF (use real samples)\r\n");
}

u8 Test_IsInjectEnabled(void)
{
    return g_insu_inject_enabled;
}

void Test_GetInjectedVoltage(float *pBat, float *pN2g)
{
    if (pBat) *pBat = g_insu_inject_bat;
    if (pN2g) *pN2g = g_insu_inject_n2g;
}

/* ======================== CAN报文测试 ======================== */

void Test_PrintCanCtrlCmd(void)
{
    SEGGER_RTT_printf(0, "\r\n");
    SEGGER_RTT_printf(0, "+-- CAN ID 55 Control Command -----------------------------+\r\n");
    SEGGER_RTT_printf(0, "| [0]UserCtrl    = %d  (%s)\r\n",
                      UserCtrlCmd_55[UserCtrl],
                      UserCtrlCmd_55[UserCtrl]==0 ? "Auto" :
                      UserCtrlCmd_55[UserCtrl]==1 ? "Start" : "Stop");
    SEGGER_RTT_printf(0, "| [1]DoCtrlChannl= %d  (0:All 1:DO1 2:DO2)\r\n",
                      UserCtrlCmd_55[DoCtrlChannl]);
    SEGGER_RTT_printf(0, "| [2]DoCtrlState = %d  (0:Open 1:Close)\r\n",
                      UserCtrlCmd_55[DoCtrlState]);
    SEGGER_RTT_printf(0, "| [3]DoCtrlConfirm= %d (1=Confirm)\r\n",
                      UserCtrlCmd_55[DoCtrlConfirm]);
    SEGGER_RTT_printf(0, "+-----------------------------------------------------------+\r\n");
    SEGGER_RTT_printf(0, "\r\n");
}

void Test_PrintCanFeedback(void)
{
    u8 fb = CtrlCmdFeedback_56[CtrlFeedback];
    SEGGER_RTT_printf(0, "\r\n");
    SEGGER_RTT_printf(0, "+-- CAN ID 56 Feedback Info -------------------------------+\r\n");
    SEGGER_RTT_printf(0, "| [0]CtrlFeedback = %d (%s)\r\n",
                      fb,
                      fb==0 ? "START" :
                      fb==1 ? "PROCESS" :
                      fb==2 ? "STOP" :
                      fb==3 ? "END" :
                      fb==4 ? "OVERTIME" : "FAULT");
    SEGGER_RTT_printf(0, "+-----------------------------------------------------------+\r\n");
    SEGGER_RTT_printf(0, "\r\n");
}

void Test_SimCanInsuMode(u8 mode)
{
    if (mode > 2)
    {
        SEGGER_RTT_printf(0, "[TEST] Invalid mode: %d (0=Auto 1=Start 2=Stop)\r\n", mode);
        return;
    }
    UserCtrlCmd_55[UserCtrl] = mode;
    gs_InsulationDetectMng.ucInsulationMode = mode;
    SEGGER_RTT_printf(0, "[TEST] CAN ID 55 simulated: UserCtrl=%d (%s)\r\n",
                      mode, ModeName(mode));
}

void Test_SimCanRelayCtrl(u8 channel, u8 state, u8 confirm)
{
    UserCtrlCmd_55[DoCtrlChannl] = channel;
    UserCtrlCmd_55[DoCtrlState] = state;
    UserCtrlCmd_55[DoCtrlConfirm] = confirm;
    SEGGER_RTT_printf(0, "[TEST] CAN ID 55 simulated: Relay Ch=%d State=%d Confirm=%d\r\n",
                      channel, state, confirm);
    
    /* 立即执行继电器控制逻辑(模拟CAN接收处理) */
    if (confirm == 1)
    {
        if (channel == 1)
        {
            gControl.DOStateSet[DO_1] = state;
        }
        else if (channel == 2)
        {
            gControl.DOStateSet[DO_2] = state;
        }
        else if (channel == 0)
        {
            gControl.DOStateSet[DO_1] = state;
            gControl.DOStateSet[DO_2] = state;
        }
        SEGGER_RTT_printf(0, "[TEST] Relay control executed: DO1=%d DO2=%d\r\n",
                          gControl.DOStateSet[DO_1], gControl.DOStateSet[DO_2]);
    }
}

/* ======================== RTT 命令解析 ======================== */

/*
 * RTT 命令入口, 解析 "test <子命令>" 并路由
 *
 * 支持命令:
 *   test insu_dump               打印绝缘状态
 *   test insu_timeout <秒>       设置超时时间
 *   test insu_period <秒>        设置采样周期
 *   test insu_mode <0|1|2>       设置模式 (0:Auto 1:Start 2:Stop)
 *   test insu_mon_start          启动周期监控
 *   test insu_mon_stop           停止周期监控
 */
void Test_RTTCommand(const char *cmd)
{
    const char *arg;

    /* 跳过 "test " 前缀 */
    if (strncmp(cmd, "test ", 5) != 0)
        return;
    arg = cmd + 5;

    /* test insu_dump */
    if (strcmp(arg, "insu_dump") == 0)
    {
        Test_InsuDump();
        return;
    }

    /* test insu_timeout <秒> */
    if (strncmp(arg, "insu_timeout ", 13) == 0)
    {
        u16 val = (u16)atoi(arg + 13);
        Test_InsuSetTimeout(val);
        return;
    }

    /* test insu_period <秒> */
    if (strncmp(arg, "insu_period ", 12) == 0)
    {
        u16 val = (u16)atoi(arg + 12);
        Test_InsuSetPeriod(val);
        return;
    }

    /* test insu_mode <0|1|2> */
    if (strncmp(arg, "insu_mode ", 10) == 0)
    {
        u8 mode = (u8)atoi(arg + 10);
        Test_InsuSetMode(mode);
        return;
    }

    /* test insu_mon_start */
    if (strcmp(arg, "insu_mon_start") == 0)
    {
        Test_InsuMonStart();
        return;
    }

    /* test insu_mon_stop */
    if (strcmp(arg, "insu_mon_stop") == 0)
    {
        Test_InsuMonStop();
        return;
    }

    /* test insu_relay */
    if (strcmp(arg, "insu_relay") == 0)
    {
        Test_PrintRelayState();
        return;
    }

    /* test insu_diag */
    if (strcmp(arg, "insu_diag") == 0)
    {
        Test_PrintDiagState();
        return;
    }

    /* test insu_threshold */
    if (strcmp(arg, "insu_threshold") == 0)
    {
        Test_PrintThresholdStatus();
        return;
    }

    /* test insu_inject_on <bat> <n2g> */
    if (strncmp(arg, "insu_inject_on ", 15) == 0)
    {
        float bat = (float)atof(arg + 15);
        /* 找第二个参数 */
        const char *p = arg + 15;
        while (*p && *p != ' ') p++;
        float n2g = (*p) ? (float)atof(p + 1) : 0.0f;
        Test_InsuInjectOn(bat, n2g);
        return;
    }

    /* test insu_inject_off */
    if (strcmp(arg, "insu_inject_off") == 0)
    {
        Test_InsuInjectOff();
        return;
    }

    /* test can_ctrl */
    if (strcmp(arg, "can_ctrl") == 0)
    {
        Test_PrintCanCtrlCmd();
        return;
    }

    /* test can_feedback */
    if (strcmp(arg, "can_feedback") == 0)
    {
        Test_PrintCanFeedback();
        return;
    }

    /* test can_set_mode <0|1|2> */
    if (strncmp(arg, "can_set_mode ", 13) == 0)
    {
        u8 mode = (u8)atoi(arg + 13);
        Test_SimCanInsuMode(mode);
        return;
    }

    /* test can_set_relay <channel> <state> <confirm> */
    if (strncmp(arg, "can_set_relay ", 14) == 0)
    {
        const char *p1 = arg + 14;
        u8 ch = (u8)atoi(p1);
        while (*p1 && *p1 != ' ') p1++;
        u8 st = (*p1) ? (u8)atoi(p1 + 1) : 0;
        if (*p1) p1++;
        while (*p1 && *p1 != ' ') p1++;
        u8 cf = (*p1) ? (u8)atoi(p1 + 1) : 0;
        Test_SimCanRelayCtrl(ch, st, cf);
        return;
    }

    /* 帮助信息 */
    SEGGER_RTT_printf(0, "[TEST] Available commands:\r\n");
    SEGGER_RTT_printf(0, "  test insu_dump               Print insulation state\r\n");
    SEGGER_RTT_printf(0, "  test insu_timeout <sec>      Set timeout limit (seconds)\r\n");
    SEGGER_RTT_printf(0, "  test insu_period <sec>       Set sampling period (seconds)\r\n");
    SEGGER_RTT_printf(0, "  test insu_mode <0|1|2>       Set mode (0:Auto 1:Start 2:Stop)\r\n");
    SEGGER_RTT_printf(0, "  test insu_mon_start          Start periodic monitoring\r\n");
    SEGGER_RTT_printf(0, "  test insu_mon_stop           Stop periodic monitoring\r\n");
    SEGGER_RTT_printf(0, "  test insu_relay              Print relay states\r\n");
    SEGGER_RTT_printf(0, "  test insu_diag               Print diagnostic states\r\n");
    SEGGER_RTT_printf(0, "  test insu_threshold          Print threshold calc result\r\n");
    SEGGER_RTT_printf(0, "  test insu_inject_on <bat> <n2g>  Enable voltage injection\r\n");
    SEGGER_RTT_printf(0, "  test insu_inject_off         Disable voltage injection\r\n");
    SEGGER_RTT_printf(0, "  test can_ctrl                Print CAN ID 55 command\r\n");
    SEGGER_RTT_printf(0, "  test can_feedback            Print CAN ID 56 feedback\r\n");
    SEGGER_RTT_printf(0, "  test can_set_mode <0|1|2>    Sim CAN set insulation mode\r\n");
    SEGGER_RTT_printf(0, "  test can_set_relay <ch> <st> <cf>  Sim CAN relay control\r\n");
}
