/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : TEST.h
** @Founder      :
** @Date         : 2026.05.25
** @Function     : 测试辅助模块 — 采样周期追踪 + 绝缘检测诊断工具
** @Instructions : 编译时启用 INSULATION_TEST_ENABLE 宏
**                 RTT 命令: test <子命令>
**===========================================================================================*/
#ifndef _TEST_H
#define _TEST_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "FreeRTOS.h"
#include "task.h"

/*=============================================================================================
** @Macro definition
**===========================================================================================*/

//#define async_TEST_ENABLE  /* 启用高压采样周期测试 */
// #define INSULATION_TEST 	//绝缘监测相关测试
/*=============================================================================================
** @Data type definition - 测量点ID
**===========================================================================================*/
typedef enum {
    TRACK_MAIN_LOOP = 0,  /* 主循环SPI完成 */
    TRACK_FULL_ROUND,      /* 全量SPI完成 */
    TRACK_CALC_DONE,       /* 计算完成 */
    TRACK_MAX
} TrackerID;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/

/* ======================== 采样周期追踪 ======================== */
void Track_Sample(TrackerID id, const char *label);

/* ======================== 绝缘检测诊断工具 ======================== */

/*
 * RTT 命令入口, 解析 "test <子命令>" 并路由
 */
void Test_RTTCommand(const char *cmd);
/*
 * 打印当前绝缘检测完整状态到 RTT
 * 输出: State, Mode, Step, 电压, 电阻, CAN反馈, 超时参数
 */
void Test_InsuDump(void);

/*
 * 设置绝缘检测超时时间 (InsuTimeLimit)
 * @param val : 秒 (0~65535, 默认5s)
 * 测试时改为较小值(如3)可加速超时触发
 */
void Test_InsuSetTimeout(u16 val);

/*
 * 设置绝缘检测采样周期 (InsuSampPeriod)
 * @param val : 秒 (0=每次执行, 默认180s)
 * 测试时设为0可跳过周期等待,立即执行
 */
void Test_InsuSetPeriod(u16 val);

/*
 * 设置绝缘检测模式
 * @param mode : 0=AutoDetect, 1=StartDetect, 2=StopDetect
 * 设为1(StartDetect)可测试超时后永久停止行为
 */
void Test_InsuSetMode(u8 mode);

/*
 * 启动周期性状态监控 (每约500ms打印一次)
 * 需要将 Test_InsuMonPoll() 挂入循环任务才生效
 */
void Test_InsuMonStart(void);

/*
 * 停止周期性状态监控
 */
void Test_InsuMonStop(void);

/*
 * 监控轮询函数 — 在循环任务中(如 PriorityBelowNormalTask)周期性调用
 * 当 g_insu_mon_active=1 时, 每 g_insu_mon_period(500ms) 打印一次状态
 */
void Test_InsuMonPoll(void);

/* 打印当前PE/IMD继电器状态到RTT */
void Test_PrintRelayState(void);

/* 打印绝缘检测相关诊断状态 */
void Test_PrintDiagState(void);

/* 打印当前绝缘电阻与告警/故障阈值比较结果 */
void Test_PrintThresholdStatus(void);

/* 开启电压注入模式 */
void Test_InsuInjectOn(float batVolt, float n2gVolt);

/* 关闭电压注入模式 */
void Test_InsuInjectOff(void);

/* 查询电压注入模式是否启用 */
u8 Test_IsInjectEnabled(void);

/* 获取注入的电压值 */
void Test_GetInjectedVoltage(float *pBat, float *pN2g);

/* 打印CAN ID 55控制命令当前值 */
void Test_PrintCanCtrlCmd(void);

/* 打印CAN ID 56反馈信息当前值 */
void Test_PrintCanFeedback(void);

/* 模拟CAN ID 55发送绝缘检测控制命令 */
void Test_SimCanInsuMode(u8 mode);

/* 模拟CAN ID 55发送继电器控制命令 */
void Test_SimCanRelayCtrl(u8 channel, u8 state, u8 confirm);

/* ======================== 继电器状态变化检测 ======================== */

/*
 * 启动继电器状态变化监控
 * 每次继电器状态改变时自动打印
 */
void Test_RelayMonStart(void);

/*
 * 停止继电器状态变化监控
 */
void Test_RelayMonStop(void);

/* ======================== 诊断告警变化检测 ======================== */

/*
 * 启动诊断告警变化监控
 * 每次诊断状态改变时自动打印
 */
void Test_DiagMonStart(void);

/*
 * 停止诊断告警变化监控
 */
void Test_DiagMonStop(void);

/* ======================== 电压不稳定自动模拟 ======================== */

/*
 * 启动电压不稳定自动模拟测试
 * 按设定间隔自动交替注入 (base) 和 (base+delta) 两个电压值
 * 模拟电压不稳定场景
 * @param base_bat: 基准母线电压 (如 100)
 * @param base_n2g: 基准PE对地电压 (如 50)
 * @param delta_bat: 母线电压变化量 (如 2, 使差值 > 1V阈值)
 * @param interval_ms: 交替间隔毫秒 (如 2000)
 */
void Test_InstabStart(float base_bat, float base_n2g, float delta_bat, u16 interval_ms);

/*
 * 停止电压不稳定模拟
 */
void Test_InstabStop(void);

/*
 * 电压不稳定模拟轮询函数 - 在循环任务中调用
 */
void Test_InstabPoll(void);

/* ======================== RTT 命令扩展 ======================== */
/*
 * 新增命令:
 *   test relay_mon_start        启动继电器变化监控
 *   test relay_mon_stop         停止继电器变化监控
 *   test diag_mon_start         启动诊断变化监控
 *   test diag_mon_stop          停止诊断变化监控
 *   test instab_start <bat> <n2g> <delta> <interval_ms>
 *                               启动电压不稳定模拟
 *   test instab_stop            停止电压不稳定模拟
 */

#endif /* _TEST_H */
