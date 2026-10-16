/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : ToolingCheck.h
** @Founder      : WYP40505
** @Date         : 20260228
** @Function     : IMIU4工装模块
** @Instructions : 
**===========================================================================================*/
#ifndef _TOOLINGCHECK_H
#define _TOOLINGCHECK_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include  "BSPCAN.h"
#include  "Modbus.h"
#include "SystemData.h"
#include "cmsis_os.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern SemaphoreHandle_t xFlashMutex;
extern u8 FDCANRData[eCANNUM][64];
extern volatile bool gCanfdRecivedFlag;
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
// 获取特定位的值
#define GET_DIDO_BIT(state, enum_value) \
    (((state) & g_didoGpio_bit_mask[enum_value]) ? 1 : 0)

/*=============================================================================================
** @Data type definition
**===========================================================================================*/

/*--------------------------------Address of  ModbusReg---------------------------------------*/
#if 1 // NEW VERSION - 精度升级(H/L双寄存器, 数据精度×1000)
typedef enum 
{
    // 工装检测控制与版本等相关寄存器 (0-9)
    REG_TOOLING_CTRL              = 0,   // 工装检测程序运行控制
    REG_VERSION_FLAG              = 1,   // 版本flag标志
    REG_HARDWARE_V                = 2,   // 硬件版本
    REG_BOOT_V                    = 3,   // boot软件版本
    REG_APP_V                     = 4,   // app软件版本
    REG_PARA_FLAG                 = 5,   // 参数flag标志

    REG_BOARD_ADDR                = 6,   // 板子地址标识 (1=工装校准使能CANFD通信测试, 其他=禁止)
    REG_TARGET_ADDR               = 7,   // 目标板地址标识 (1=工装校准使能CANFD通信测试, 其他=禁止)
    REG_RESERVED_8                = 8,   // 保留
    REG_RESERVED_9                = 9,   // 保留

    // 条码相关寄存器 (10-26)
    REG_BAR_WR                    = 10,  // 条码写入/读取
    REG_BAR                       = 11,  // 板条码内字符第1，2位

    REG_RESERVED_27               = 27,  // 保留
    REG_RESERVED_28               = 28,  // 保留
    REG_RESERVED_29               = 29,  // 保留
    
    // 输入/输出状态寄存器 (30-38)
    REG_SW                        = 30,  // 拨码开关
    REG_DI1                       = 31,  // DI1
    REG_HALL_P_FAULT              = 32,  // HALL_P_FAULT
    REG_DIAGNOSIS                 = 33,  // Diagnosis
    REG_RUN_LED                   = 34,  // RUN_LED灯
    REG_IMD_ALM_LED               = 35,  // IMD_ALM灯
    REG_C_RELAY_1                 = 36,  // CRELAY1
    REG_C_RELAY_2                 = 37,  // CRELAY2
    REG_PE_RELAY                  = 38,  // PE_RLY
    
    // 通信测试寄存器 (39-43)
    REG_E2PROM_FLASH_TEST         = 39,  // E2PRON\FLASH读写测试
    REG_KA84922_COMM_STATUS       = 40,  // 菊花链ka84922通信是否正常
    REG_KA84917_COMM_STATUS       = 41,  // 菊花链ka84917通信是否正常
    REG_RN8209_COMM_STATUS        = 42,  // rn8209通信是否正常
    REG_CANFD_COMM_STATUS         = 43,  // CANFD通信是否正常
    
    // 温度/电压/电流监控寄存器 (44-47)
    REG_NTC_TEMP_1                = 44,  // NTC温度1
    REG_NTC_TEMP_2                = 45,  // NTC温度2
    REG_VCC_5V                    = 46,  // VCC_5V
    REG_HALL_5V_SUPPLY            = 47,  // 霍尔供电5V电压
    
    // 霍尔电流校准相关寄存器 (48-65)
    REG_HALL_CURRENT_CAL_MODE     = 48,  // 霍尔电流校准系数获取/保存模式
    REG_HALL_CAL_TEST_MODE        = 49,  // 计算系数/校准测试模式切换

    REG_HALL_ZERO_INPUT           = 50,  // 工装接入监控板上的校零电压
    REG_HALL_ZERO_COMPLETE        = 51,  // 工装接入监控板校零电压输入完成

    REG_HALL_CAL_INPUT_1          = 52,  // 工装接入监控板上的电流/电压1
    REG_HALL_CAL_COMPLETE_1       = 53,  // 外部电流/电压输入完成1
    
    REG_HALL_CAL_INPUT_2          = 54,  // 工装接入监控板上的电流/电压2
    REG_HALL_CAL_COMPLETE_2       = 55,  // 外部电流/电压输入完成2
    
    REG_HALL_CAL_COEFF_K1_H       = 56,  // 霍尔校准系数K1H
    REG_HALL_CAL_COEFF_K1_L       = 57,  // 霍尔校准系数K1L
    REG_HALL_CAL_COEFF_B1_H       = 58,  // 霍尔校准系数B1H
    REG_HALL_CAL_COEFF_B1_L       = 59,  // 霍尔校准系数B1L
    REG_HALL_CAL_Z1_H             = 60,  // 霍尔校零值Z1H
    REG_HALL_CAL_Z1_L             = 61,  // 霍尔校零值Z1L
    REG_HALL_CALIBRATED_CURRENT   = 62,  // 校准后的霍尔电流
    REG_HALL_MEASURING_CURRENT    = 63,  // 实际测量的霍尔电流
    REG_RESERVED_64               = 64,  // 保留
    REG_RESERVED_65               = 65,  // 保留

    
    // 分流器温度和其他监控寄存器 (66-69)
    REG_SHUNT_TEMP_1                = 66,  // 分流器温度TEMP1
    REG_IMD_RELAY                   = 67, // IMD_RLY
    REG_HV_5V                       = 68, // HV_5V
    REG_HV_MINUS_5V_AD              = 69, // HV_-5V_AD
    
    // PE电压校准相关寄存器 (70-87)
    REG_PE_VOLTAGE_CAL_MODE         = 70,  // pe电压校准系数获取/保存模式
    REG_PE_CAL_TEST_MODE            = 71,  // 计算系数/校准测试模式切换
    REG_PE_CAL_INPUT_1H             = 72,  // 工装接入监控板上的电流/电压1H
    REG_PE_CAL_INPUT_1L             = 73,  // 工装接入监控板上的电流/电压1L
    REG_PE_CAL_COMPLETE_1           = 74,  // 外部电流/电压输入完成1
    REG_PE_CAL_INPUT_2H             = 75,  // 工装接入监控板上的电流/电压2H
    REG_PE_CAL_INPUT_2L             = 76,  // 工装接入监控板上的电流/电压2L
    REG_PE_CAL_COMPLETE_2           = 77,  // 外部电流/电压输入完成2
    REG_PE_CAL_COEFF_K1_H           = 78,  // pe电压校准系数K1H
    REG_PE_CAL_COEFF_K1_L           = 79,  // pe电压校准系数K1L
    REG_PE_CAL_COEFF_B1_H           = 80,  // pe电压校准系数B1H
    REG_PE_CAL_COEFF_B1_L           = 81,  // pe电压校准系数B1L
    REG_PE_CALIBRATED_VOLTAGEH      = 82,  // 校准后的pe电压H
    REG_PE_CALIBRATED_VOLTAGEL      = 83,  // 校准后的pe电压L
    REG_PE_MEASURING_VOLTAGEH       = 84,  // 实际测量的pe电压H
    REG_PE_MEASURING_VOLTAGEL       = 85,  // 实际测量的pe电压L
    REG_RESERVED_86                 = 86,  // 保留
    REG_RESERVED_87                 = 87,  // 保留
    
    // BAT电压校准 (88-103)
    REG_BAT_VOLTAGE_CAL_MODE        = 88,   // BAT电压校准系数获取/保存模式
    REG_BAT_CAL_TEST_MODE           = 89,   // 计算系数/校准测试模式切换
    REG_BAT_CAL_INPUT_1H            = 90,   // 工装接入监控板上的电流/电压1H
    REG_BAT_CAL_INPUT_1L            = 91,   // 工装接入监控板上的电流/电压1L
    REG_BAT_CAL_COMPLETE_1          = 92,   // 外部电流/电压输入完成1
    REG_BAT_CAL_INPUT_2H            = 93,   // 工装接入监控板上的电流/电压2H
    REG_BAT_CAL_INPUT_2L            = 94,   // 工装接入监控板上的电流/电压2L
    REG_BAT_CAL_COMPLETE_2          = 95,   // 外部电流/电压输入完成2
    REG_BAT_CAL_COEFF_K1_H          = 96,   // BAT电压校准系数K1H
    REG_BAT_CAL_COEFF_K1_L          = 97,   // BAT电压校准系数K1L
    REG_BAT_CAL_COEFF_B1_H          = 98,   // BAT电压校准系数B1H
    REG_BAT_CAL_COEFF_B1_L          = 99,   // BAT电压校准系数B1L
    REG_BAT_CALIBRATED_VOLTAGEH     = 100,  // 校准后的BAT电压H
    REG_BAT_CALIBRATED_VOLTAGEL     = 101,  // 校准后的BAT电压L
    REG_BAT_MEASURING_VOLTAGEH      = 102,  // 实际测量的BAT电压H
    REG_BAT_MEASURING_VOLTAGEL      = 103,  // 实际测量的BAT电压L
    REG_RESERVED_104                = 104,  // 保留
    REG_RESERVED_105                = 105,  // 保留
    
    // 917充电电流/电压校准 (106-123)
    REG_KA84917_CURRENT_CHG_MODE            = 106,  // 917充电校准系数获取/保存模式
    REG_KA84917_CHG_TEST_MODE               = 107,  // 计算系数/校准测试模式切换
    REG_KA84917_CHG_INPUT_1H                = 108,  // 工装接入监控板上的充电电压1H
    REG_KA84917_CHG_INPUT_1L                = 109,  // 工装接入监控板上的充电电压1L
    REG_KA84917_CHG_COMPLETE_1              = 110,  // 外部充电电压输入完成1
    REG_KA84917_CHG_INPUT_2H                = 111,  // 工装接入监控板上的充电电压2H
    REG_KA84917_CHG_INPUT_2L                = 112,  // 工装接入监控板上的充电电压2L
    REG_KA84917_CHG_COMPLETE_2              = 113,  // 外部充电电压输入完成2
    REG_KA84917_CHG_COEFF_K1_H              = 114,  // 917充电校准系数K1H
    REG_KA84917_CHG_COEFF_K1_L              = 115,  // 917充电校准系数K1L
    REG_KA84917_CHG_COEFF_B1_H              = 116,  // 917充电校准系数B1H
    REG_KA84917_CHG_COEFF_B1_L              = 117,  // 917充电校准系数B1L
    REG_KA84917_CALIBRATED_CHG_CURRENTH     = 118,  // 校准后的917充电电压H
    REG_KA84917_CALIBRATED_CHG_CURRENTL     = 119,  // 校准后的917充电电压L
    REG_KA84917_MEASURING_CHG_CURRENTH      = 120,  // 实际测量的917充电电压H
    REG_KA84917_MEASURING_CHG_CURRENTL      = 121,  // 实际测量的917充电电压L
    REG_RESERVED_122                        = 122,  // 保留
    REG_RESERVED_123                        = 123,  // 保留

    // 917放电电流/电压校准 (124-141)
    REG_KA84917_DISCHG_CURRENT_CAL_MODE       = 124, // 917放电校准系数获取/保存模式
    REG_KA84917_DISCHG_TEST_MODE              = 125, // 计算系数/校准测试模式切换
    REG_KA84917_DISCHG_INPUT_1H               = 126, // 工装接入监控板上的放电电压1H
    REG_KA84917_DISCHG_INPUT_1L               = 127, // 工装接入监控板上的放电电压1L
    REG_KA84917_DISCHG_COMPLETE_1             = 128, // 外部放电电压输入完成1
    REG_KA84917_DISCHG_INPUT_2H               = 129, // 工装接入监控板上的放电电压2H
    REG_KA84917_DISCHG_INPUT_2L               = 130, // 工装接入监控板上的放电电压2L
    REG_KA84917_DISCHG_COMPLETE_2             = 131, // 外部放电电压输入完成2
    REG_KA84917_DISCHG_COEFF_K1_H             = 132, // 917放电校准系数K1H
    REG_KA84917_DISCHG_COEFF_K1_L             = 133, // 917放电校准系数K1L
    REG_KA84917_DISCHG_COEFF_B1_H             = 134, // 917放电校准系数B1H
    REG_KA84917_DISCHG_COEFF_B1_L             = 135, // 917放电校准系数B1L
    REG_KA84917_CALIBRATED_DISCHG_CURRENTH    = 136, // 校准后的917放电电压H
    REG_KA84917_CALIBRATED_DISCHG_CURRENTL    = 137, // 校准后的917放电电压L
    REG_KA84917_MEASURING_DISCHG_CURRENTH     = 138, // 实际测量的917放电电压H
    REG_KA84917_MEASURING_DISCHG_CURRENTL     = 139, // 实际测量的917放电电压L
    REG_RESERVED_140                          = 140, // 保留
    REG_RESERVED_141                          = 141, // 保留

   // RN8209电压校准 (142-159)
    REG_RN8209_VOLTAGE_CAL_MODE             = 142,  // rn8209电压校准系数获取/保存模式
    REG_RN8209_VOLTAGE_CAL_TEST_MODE        = 143,  // 计算系数/校准测试模式切换
    REG_RN8209_VOLTAGE_CAL_INPUT_1H         = 144,  // 工装接入监控板上的电流/电压1H
    REG_RN8209_VOLTAGE_CAL_INPUT_1L         = 145,  // 工装接入监控板上的电流/电压1L
    REG_RN8209_VOLTAGE_CAL_COMPLETE_1       = 146,  // 外部电流/电压输入完成1
    REG_RN8209_VOLTAGE_CAL_INPUT_2H         = 147,  // 工装接入监控板上的电流/电压2H
    REG_RN8209_VOLTAGE_CAL_INPUT_2L         = 148,  // 工装接入监控板上的电流/电压2L
    REG_RN8209_VOLTAGE_CAL_COMPLETE_2       = 149,  // 外部电流/电压输入完成2
    REG_RN8209_VOLTAGE_CAL_COEFF_K1_H       = 150,  // rn8209电压校准系数K1H
    REG_RN8209_VOLTAGE_CAL_COEFF_K1_L       = 151,  // rn8209电压校准系数K1L
    REG_RN8209_VOLTAGE_CAL_COEFF_B1_H       = 152,  // rn8209电压校准系数B1H
    REG_RN8209_VOLTAGE_CAL_COEFF_B1_L       = 153,  // rn8209电压校准系数B1L
    REG_RN8209_VOLTAGE_CALIBRATEDH          = 154,  // 校准后的rn8209电压H
    REG_RN8209_VOLTAGE_CALIBRATEDL          = 155,  // 校准后的rn8209电压L
    REG_RN8209_MEASURING_VOLTAGEH           = 156,  // 实际测量的rn8209电压H
    REG_RN8209_MEASURING_VOLTAGEL           = 157,  // 实际测量的rn8209电压L
    REG_RESERVED_158                        = 158,  // 保留
    REG_RESERVED_159                        = 159,  // 保留

    // RN8209充电电流/电压校准 (160-177)
    REG_RN8209_CHG_CURRENT_CAL_MODE         = 160,    // rn8209充电校准系数获取/保存模式
    REG_RN8209_CHG_TEST_MODE                = 161,    // 计算系数/校准测试模式切换
    REG_RN8209_CHG_INPUT_1H                 = 162,    // 工装接入监控板上的充电电压1H
    REG_RN8209_CHG_INPUT_1L                 = 163,    // 工装接入监控板上的充电电压1L
    REG_RN8209_CHG_COMPLETE_1               = 164,    // 外部充电电压输入完成1
    REG_RN8209_CHG_INPUT_2H                 = 165,    // 工装接入监控板上的充电电压2H
    REG_RN8209_CHG_INPUT_2L                 = 166,    // 工装接入监控板上的充电电压2L
    REG_RN8209_CHG_COMPLETE_2               = 167,    // 外部充电电压输入完成2
    REG_RN8209_CHG_COEFF_K1_H               = 168,    // rn8209充电校准系数K1H
    REG_RN8209_CHG_COEFF_K1_L               = 169,    // rn8209充电校准系数K1L
    REG_RN8209_CHG_COEFF_B1_H               = 170,    // rn8209充电校准系数B1H
    REG_RN8209_CHG_COEFF_B1_L               = 171,    // rn8209充电校准系数B1L
    REG_RN8209_CALIBRATED_CHG_CURRENTH      = 172,    // 校准后的rn8209充电电压H
    REG_RN8209_CALIBRATED_CHG_CURRENTL      = 173,    // 校准后的rn8209充电电压L
    REG_RN8209_MEASURING_CHG_CURRENTH       = 174,    // 实际测量的rn8209充电电压H
    REG_RN8209_MEASURING_CHG_CURRENTL       = 175,    // 实际测量的rn8209充电电压L
    REG_RESERVED_176                        = 176,    // 保留
    REG_RESERVED_177                        = 177,    // 保留

    // RN8209放电电流/电压校准 (178-193)
    REG_RN8209_DISCHG_CURRENT_CAL_MODE      = 178, // rn8209放电校准系数获取/保存模式
    REG_RN8209_DISCHG_TEST_MODE             = 179, // 计算系数/校准测试模式切换
    REG_RN8209_DISCHG_INPUT_1H              = 180, // 工装接入监控板上的放电电压1H
    REG_RN8209_DISCHG_INPUT_1L              = 181, // 工装接入监控板上的放电电压1L
    REG_RN8209_DISCHG_COMPLETE_1            = 182, // 外部放电电压输入完成1
    REG_RN8209_DISCHG_INPUT_2H              = 183, // 工装接入监控板上的放电电压2H
    REG_RN8209_DISCHG_INPUT_2L              = 184, // 工装接入监控板上的放电电压2L
    REG_RN8209_DISCHG_COMPLETE_2            = 185, // 外部放电电压输入完成2
    REG_RN8209_DISCHG_COEFF_K1_H            = 186, // rn8209放电校准系数K1H
    REG_RN8209_DISCHG_COEFF_K1_L            = 187, // rn8209放电校准系数K1L
    REG_RN8209_DISCHG_COEFF_B1_H            = 188, // rn8209放电校准系数B1H
    REG_RN8209_DISCHG_COEFF_B1_L            = 189, // rn8209放电校准系数B1L
    REG_RN8209_CALIBRATED_DISCHG_CURRENTH   = 190, // 校准后的rn8209放电电压H
    REG_RN8209_CALIBRATED_DISCHG_CURRENTL   = 191, // 校准后的rn8209放电电压L
    REG_RN8209_MEASURING_DISCHG_CURRENTH    = 192, // 实际测量的rn8209放电电压H
    REG_RN8209_MEASURING_DISCHG_CURRENTL    = 193, // 实际测量的rn8209放电电压L
    
    // 保留寄存器区域 (194-253)
    REG_RESERVED_START            = 194,  
  
    // 寄存器总数
    REG_TOTAL_COUNT               = 254  

}ModbusRegAddr;
#else // OLD VERSION - preserved (原始版本)
typedef enum 
{
    // 工装检测控制与版本等相关寄存器 (0-9)
    REG_TOOLING_CTRL              = 0,   // 工装检测程序运行控制
    REG_VERSION_FLAG              = 1,   // 版本flag标志
    REG_HARDWARE_V                = 2,   // 硬件版本
    REG_BOOT_V                    = 3,   // boot软件版本
    REG_APP_V                     = 4,   // app软件版本
    REG_PARA_FLAG                 = 5,   // 参数flag标志

    REG_BOARD_ADDR                = 6,   // 板子地址标识 (0=使用拨码开关, 非0=Modbus设置的地址)
    REG_TARGET_ADDR               = 7,   // 目标板地址 (仅REG_BOARD_ADDR非0时有效)
    REG_RESERVED_8                = 8,   // 保留
    REG_RESERVED_9                = 9,   // 保留

    // 条码相关寄存器 (10-26)
    REG_BAR_WR                    = 10,  // 条码写入/读取
    REG_BAR                       = 11,  // 板条码内字符第1，2位

    REG_RESERVED_27               = 27,  // 保留
    REG_RESERVED_28               = 28,  // 保留
    REG_RESERVED_29               = 29,  // 保留
    
    // 输入/输出状态寄存器 (30-38)
    REG_SW                        = 30,  // 拨码开关
    REG_DI1                       = 31,  // DI1
    REG_HALL_P_FAULT              = 32,  // HALL_P_FAULT
    REG_DIAGNOSIS                 = 33,  // Diagnosis
    REG_RUN_LED                   = 34,  // RUN_LED灯
    REG_IMD_ALM_LED               = 35,  // IMD_ALM灯
    REG_C_RELAY_1                 = 36,  // CRELAY1
    REG_C_RELAY_2                 = 37,  // CRELAY2
    REG_PE_RELAY                  = 38,  // PE_RLY
    
    // 通信测试寄存器 (39-43)
    REG_E2PROM_FLASH_TEST         = 39,  // E2PRON\FLASH读写测试
    REG_KA84922_COMM_STATUS       = 40,  // 菊花链ka84922通信是否正常
    REG_KA84917_COMM_STATUS       = 41,  // 菊花链ka84917通信是否正常
    REG_RN8209_COMM_STATUS        = 42,  // rn8209通信是否正常
    REG_CANFD_COMM_STATUS         = 43,  // CANFD通信是否正常
    
    // 温度/电压/电流监控寄存器 (44-47)
    REG_NTC_TEMP_1                = 44,  // NTC温度1
    REG_NTC_TEMP_2                = 45,  // NTC温度2
    REG_VCC_5V                    = 46,  // VCC_5V
    REG_HALL_5V_SUPPLY            = 47,  // 霍尔供电5V电压
    
    // 霍尔电流校准相关寄存器 (48-65)
    REG_HALL_CURRENT_CAL_MODE     = 48,  // 霍尔电流校准系数获取/保存模式
    REG_HALL_CAL_TEST_MODE        = 49,  // 计算系数/校准测试模式切换

    REG_HALL_ZERO_INPUT           = 50,  // 工装接入监控板上的校零电压
    REG_HALL_ZERO_COMPLETE        = 51,  // 工装接入监控板校零电压输入完成

    REG_HALL_CAL_INPUT_1          = 52,  // 工装接入监控板上的电流/电压1
    REG_HALL_CAL_COMPLETE_1       = 53,  // 外部电流/电压输入完成1
    
    REG_HALL_CAL_INPUT_2          = 54,  // 工装接入监控板上的电流/电压2
    REG_HALL_CAL_COMPLETE_2       = 55,  // 外部电流/电压输入完成2
    
    REG_HALL_CAL_COEFF_K1_H       = 56,  // 霍尔校准系数K1H
    REG_HALL_CAL_COEFF_K1_L       = 57,  // 霍尔校准系数K1L
    REG_HALL_CAL_COEFF_B1_H       = 58,  // 霍尔校准系数B1H
    REG_HALL_CAL_COEFF_B1_L       = 59,  // 霍尔校准系数B1L
    REG_HALL_CAL_Z1_H             = 60,  // 霍尔校零值Z1H
    REG_HALL_CAL_Z1_L             = 61,  // 霍尔校零值Z1L
    REG_HALL_CALIBRATED_CURRENT   = 62,  // 校准后的霍尔电流
    REG_HALL_MEASURING_CURRENT    = 63,  // 实际测量的霍尔电流
    REG_RESERVED_64               = 64,  // 保留
    REG_RESERVED_65               = 65,  // 保留

    
    // 分流器温度和其他监控寄存器 (66-69)
    REG_SHUNT_TEMP_1                = 66,  // 分流器温度TEMP1
    REG_IMD_RELAY                   = 67, // IMD_RLY
    REG_HV_5V                       = 68, // HV_5V
    REG_HV_MINUS_5V_AD              = 69, // HV_-5V_AD
    
    // PE电压校准相关寄存器 (70-87)
    REG_PE_VOLTAGE_CAL_MODE         = 70,  // pe电压校准系数获取/保存模式
    REG_PE_CAL_TEST_MODE            = 71,  // 计算系数/校准测试模式切换
    REG_PE_CAL_INPUT_1H             = 72,  // 工装接入监控板上的电流/电压1H
    REG_PE_CAL_INPUT_1L             = 73,  // 工装接入监控板上的电流/电压1L
    REG_PE_CAL_COMPLETE_1           = 74,  // 外部电流/电压输入完成1
    REG_PE_CAL_INPUT_2H             = 75,  // 工装接入监控板上的电流/电压2H
    REG_PE_CAL_INPUT_2L             = 76,  // 工装接入监控板上的电流/电压2L
    REG_PE_CAL_COMPLETE_2           = 77,  // 外部电流/电压输入完成2
    REG_PE_CAL_COEFF_K1_H           = 78,  // pe电压校准系数K1H
    REG_PE_CAL_COEFF_K1_L           = 79,  // pe电压校准系数K1L
    REG_PE_CAL_COEFF_B1_H           = 80,  // pe电压校准系数B1H
    REG_PE_CAL_COEFF_B1_L           = 81,  // pe电压校准系数B1L
    REG_PE_CALIBRATED_VOLTAGEH      = 82,  // 校准后的pe电压H
    REG_PE_CALIBRATED_VOLTAGEL      = 83,  // 校准后的pe电压L
    REG_PE_MEASURING_VOLTAGEH       = 84,  // 实际测量的pe电压H
    REG_PE_MEASURING_VOLTAGEL       = 85,  // 实际测量的pe电压L
    REG_RESERVED_86                 = 86,  // 保留
    REG_RESERVED_87                 = 87,  // 保留
    
    // BAT电压校准 (88-103)
    REG_BAT_VOLTAGE_CAL_MODE        = 88,   // BAT电压校准系数获取/保存模式
    REG_BAT_CAL_TEST_MODE           = 89,   // 计算系数/校准测试模式切换
    REG_BAT_CAL_INPUT_1H            = 90,   // 工装接入监控板上的电流/电压1H
    REG_BAT_CAL_INPUT_1L            = 91,   // 工装接入监控板上的电流/电压1L
    REG_BAT_CAL_COMPLETE_1          = 92,   // 外部电流/电压输入完成1
    REG_BAT_CAL_INPUT_2H            = 93,   // 工装接入监控板上的电流/电压2H
    REG_BAT_CAL_INPUT_2L            = 94,   // 工装接入监控板上的电流/电压2L
    REG_BAT_CAL_COMPLETE_2          = 95,   // 外部电流/电压输入完成2
    REG_BAT_CAL_COEFF_K1_H          = 96,   // BAT电压校准系数K1H
    REG_BAT_CAL_COEFF_K1_L          = 97,   // BAT电压校准系数K1L
    REG_BAT_CAL_COEFF_B1_H          = 98,   // BAT电压校准系数B1H
    REG_BAT_CAL_COEFF_B1_L          = 99,   // BAT电压校准系数B1L
    REG_BAT_CALIBRATED_VOLTAGEH     = 100,  // 校准后的BAT电压H
    REG_BAT_CALIBRATED_VOLTAGEL     = 101,  // 校准后的BAT电压L
    REG_BAT_MEASURING_VOLTAGEH      = 102,  // 实际测量的BAT电压H
    REG_BAT_MEASURING_VOLTAGEL      = 103,  // 实际测量的BAT电压L
    REG_RESERVED_104                = 104,  // 保留
    REG_RESERVED_105                = 105,  // 保留
    
    // 917充电电流校准 (106-119)
    REG_KA84917_CURRENT_CHG_MODE            = 106,  // 917电流充电校准系数获取/保存模式
    REG_KA84917_CHG_TEST_MODE               = 107,  // 计算系数/校准测试模式切换
    REG_KA84917_CHG_INPUT_1                 = 108,  // 工装接入监控板上的充电电流
    REG_KA84917_CHG_COMPLETE_1              = 109,  // 外部充电电流输入完成1
    REG_KA84917_CHG_INPUT_2                 = 110,  // 工装接入监控板上的充电电流
    REG_KA84917_CHG_COMPLETE_2              = 111,  // 外部电流/电压输入完成2
    REG_KA84917_CHG_COEFF_K1_H              = 112,  // 917充电电流校准系数K1H
    REG_KA84917_CHG_COEFF_K1_L              = 113,  // 917充电电流校准系数K1L
    REG_KA84917_CHG_COEFF_B1_H              = 114,  // 917充电电流校准系数B1H
    REG_KA84917_CHG_COEFF_B1_L              = 115, // 917充电电流校准系数B1L
    REG_KA84917_CALIBRATED_CHG_CURRENT      = 116, // 校准后的917充电电流
    REG_KA84917_MEASURING_CHG_CURRENT       = 117, // 实际测量的917充电电流
    REG_RESERVED_118                        = 118, // 保留
    REG_RESERVED_119                        = 119, // 保留

    // 917放电电流校准 (120-133)
    REG_KA84917_DISCHG_CURRENT_CAL_MODE       = 120, // 917放电电流校准系数获取/保存模式
    REG_KA84917_DISCHG_TEST_MODE              = 121, // 计算系数/校准测试模式切换
    REG_KA84917_DISCHG_INPUT_1                = 122, // 工装接入监控板上的放电电流
    REG_KA84917_DISCHG_COMPLETE_1             = 123, // 外部放电电流输入完成1
    REG_KA84917_DISCHG_INPUT_2                = 124, // 工装接入监控板上的放电电流
    REG_KA84917_DISCHG_COMPLETE_2             = 125, // 外部放电电流输入完成2
    REG_KA84917_DISCHG_COEFF_K1_H             = 126, // 917放电电流校准系数K1H
    REG_KA84917_DISCHG_COEFF_K1_L             = 127, // 917放电电流校准系数K1L
    REG_KA84917_DISCHG_COEFF_B1_H             = 128, // 917放电电流校准系数B1H
    REG_KA84917_DISCHG_COEFF_B1_L             = 129, // 917放电电流校准系数B1L
    REG_KA84917_CALIBRATED_DISCHG_CURRENT     = 130, // 校准后的917放电电流
    REG_KA84917_MEASURING_DISCHG_CURRENT      = 131, // 实际测量的917放电电流
    REG_RESERVED_132                          = 132, // 保留
    REG_RESERVED_133                          = 133, // 保留

   // RN8209电压校准 (134-151)
    REG_RN8209_VOLTAGE_CAL_MODE             = 134,  // rn8209电压校准系数获取/保存模式
    REG_RN8209_VOLTAGE_CAL_TEST_MODE        = 135,  // 计算系数/校准测试模式切换
    REG_RN8209_VOLTAGE_CAL_INPUT_1H         = 136,  // 工装接入监控板上的电流/电压1H
    REG_RN8209_VOLTAGE_CAL_INPUT_1L         = 137,  // 工装接入监控板上的电流/电压1L
    REG_RN8209_VOLTAGE_CAL_COMPLETE_1       = 138,  // 外部电流/电压输入完成1
    REG_RN8209_VOLTAGE_CAL_INPUT_2H         = 139,  // 工装接入监控板上的电流/电压2H
    REG_RN8209_VOLTAGE_CAL_INPUT_2L         = 140,  // 工装接入监控板上的电流/电压2L
    REG_RN8209_VOLTAGE_CAL_COMPLETE_2       = 141,  // 外部电流/电压输入完成2
    REG_RN8209_VOLTAGE_CAL_COEFF_K1_H       = 142,  // rn8209电压校准系数K1H
    REG_RN8209_VOLTAGE_CAL_COEFF_K1_L       = 143,  // rn8209电压校准系数K1L
    REG_RN8209_VOLTAGE_CAL_COEFF_B1_H       = 144,  // rn8209电压校准系数B1H
    REG_RN8209_VOLTAGE_CAL_COEFF_B1_L       = 145,  // rn8209电压校准系数B1L
    REG_RN8209_VOLTAGE_CALIBRATEDH          = 146,  // 校准后的rn8209电压H
    REG_RN8209_VOLTAGE_CALIBRATEDL          = 147,  // 校准后的rn8209电压L
    REG_RN8209_MEASURING_VOLTAGEH           = 148,  // 实际测量的rn8209电压H
    REG_RN8209_MEASURING_VOLTAGEL           = 149,  // 实际测量的rn8209电压L
    REG_RESERVED_150                        = 150,  // 保留
    REG_RESERVED_151                        = 151,  // 保留

    // RN8209充电电流校准 (152-165)
    REG_RN8209_CHG_CURRENT_CAL_MODE         = 152,    // rn8209充电电流校准系数获取/保存模式
    REG_RN8209_CHG_TEST_MODE                = 153,    // 计算系数/校准测试模式切换
    REG_RN8209_CHG_INPUT_1                  = 154,    // 工装接入监控板上的充电电流
    REG_RN8209_CHG_COMPLETE_1               = 155,    // 外部电流/电压输入完成1
    REG_RN8209_CHG_INPUT_2                  = 156,    // 工装接入监控板上的充电电流
    REG_RN8209_CHG_COMPLETE_2               = 157,    // 外部充电电流/电压输入完成2
    REG_RN8209_CHG_COEFF_K1_H               = 158,    // rn8209充电电流校准系数K1H
    REG_RN8209_CHG_COEFF_K1_L               = 159,    // rn8209充电电流校准系数K1L
    REG_RN8209_CHG_COEFF_B1_H               = 160,    // rn8209充电电流校准系数B1H
    REG_RN8209_CHG_COEFF_B1_L               = 161,   // rn8209充电电流校准系数B1L
    REG_RN8209_CALIBRATED_CHG_CURRENT       = 162,   // 校准后的rn8209充电电流
    REG_RN8209_MEASURING_CHG_CURRENT        = 163,  // 实际测量的rn8209充电电流
    REG_RESERVED_164                        = 164,  // 保留
    REG_RESERVED_165                        = 165,  // 保留

    // RN8209放电电流校准 (166-177)
    REG_RN8209_DISCHG_CURRENT_CAL_MODE      = 166, // rn8209放电电流校准系数获取/保存模式
    REG_RN8209_DISCHG_TEST_MODE             = 167, // 计算系数/校准测试模式切换
    REG_RN8209_DISCHG_INPUT_1               = 168, // 工装接入监控板上的放电电流/电压1
    REG_RN8209_DISCHG_COMPLETE_1            = 169, // 外部放电电流/电压1输入完成1
    REG_RN8209_DISCHG_INPUT_2               = 170, // 工装接入监控板上的放电电流/电压2
    REG_RN8209_DISCHG_COMPLETE_2            = 171, // 外部放电电流/电压1输入完成2
    REG_RN8209_DISCHG_COEFF_K1_H            = 172, // rn8209放电电流校准系数K1H
    REG_RN8209_DISCHG_COEFF_K1_L            = 173, // rn8209放电电流校准系数K1L
    REG_RN8209_DISCHG_COEFF_B1_H            = 174, // rn8209放电电流校准系数B1H
    REG_RN8209_DISCHG_COEFF_B1_L            = 175, // rn8209放电电流校准系数B1L
    REG_RN8209_CALIBRATED_DISCHG_CURRENT    = 176, // 校准后的rn8209放电电流
    REG_RN8209_MEASURING_DISCHG_CURRENT     = 177,  // 实际测量的rn8209充电电流
    
    // 保留寄存器区域 (178-237)
    REG_RESERVED_START            = 178,
  
    // 寄存器总数
    REG_TOTAL_COUNT               = 238  

}ModbusRegAddr;
#endif

// 定义位掩码数组，DI DO GPIO索引对应枚举值
static const u16 g_didoGpio_bit_mask[] = {
    [Di1]           = 0x0001,  // bit0
    [Do1]           = 0x0002,  // bit1
    [Do2]           = 0x0004,  // bit2
    [Sw1]           = 0x0008,  // bit3
    [Sw2]           = 0x0010,  // bit4
    [Sw3]           = 0x0020,  // bit5
    [Sw4]           = 0x0040,  // bit6
    [RunLed]        = 0x0080,  // bit7
    [Almled]        = 0x0100,  // bit8
    [DaisyComFail]  = 0x0200,  // bit9
    [HallFailSignal]= 0x0400,  // bit10
    [PeRlyState]    = 0x0800,  // bit11
    [ImdRlyState]   = 0x1000,  // bit12
    [Ver1]          = 0x2000,  // bit13
    [Ver2]          = 0x4000,  // bit14
};



/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=================================================================================================
** @Name      : void ToolingCheckCanCommunTest(void)
** @Input     : void
** @Output    : void
** @Function  : 拨码为1的测试板测试工装通信发送数据
** @The notes : 连续发送工装测试报文
**===============================================================================================*/
void ToolingCheckCanCommunTest(void);

/*=================================================================================================
** @Name      : void ToolingCheckFunctionTask(void)  
** @Input     : void
** @Output    : void
** @Function  : 工装测试模块
** @The notes : 
**===============================================================================================*/
void ToolingCheckFunctionTask(void);

/*=================================================================================================
** @Name      : void Test_RVersion(void) 
** @Input     : void
** @Output    : void
** @Function  : 上传软硬件版本号到Modbus寄存器 包括版本标志和版本信息(只读)
** @The notes : 读取成功置为0xA5,失败为0xFF
**===============================================================================================*/
void Test_RVersion(void);

/*=================================================================================================
** @Name      : void Test_ParaFlag(void) 
** @Input     : void
** @Output    : void
** @Function  : 上传参数FLAG标志到Modbus寄存器 (只读)
** @The notes : 0x5a：表示能从外部存储读出参数     0：表示没有参数
**===============================================================================================*/
void Test_ParaFlag(void);

/*=================================================================================================
** @Name      : void Test_BarCode(void) 
** @Input     : void
** @Output    : void
** @Function  : 条码信息写入/读取
** @The notes : 
**===============================================================================================*/
void Test_BarCode(void);

/*=================================================================================================
** @Name      : void Test_SWAddr(void) 
** @Input     : void
** @Output    : void
** @Function  : 上传拨码开关
** @The notes : (最低4位有效)
**===============================================================================================*/
void Test_SWAddr(void);

/*=================================================================================================
** @Name      : void Test_DI1(void) 
** @Input     : void
** @Output    : void
** @Function  : DI1状态
** @The notes : (最低1位)
**===============================================================================================*/
void Test_DI1(void);

/*=================================================================================================
** @Name      : void Test_HallPFault(void) 
** @Input     : void
** @Output    : void
** @Function  : HALL_P_FAULT
** @The notes : (最低1位)
**===============================================================================================*/
void Test_HallPFault(void);

/*=================================================================================================
** @Name      : void Test_Diagnosis(void) 
** @Input     : void
** @Output    : void
** @Function  : Diagnosis
** @The notes : (最低1位)
**===============================================================================================*/
void Test_Diagnosis(void);

/*=================================================================================================
** @Name      : void Test_HallPFault(void) 
** @Input     : void
** @Output    : void
** @Function  : HALL_P_FAULT
** @The notes : (最低1位)
**===============================================================================================*/
void Test_HallPFault(void);

/*=================================================================================================
** @Name      : void Test_Diagnosis(void) 
** @Input     : void
** @Output    : void
** @Function  : Diagnosis
** @The notes : (最低1位)
**===============================================================================================*/
void Test_Diagnosis(void);

/*=================================================================================================
** @Name      : void Test_LEDALM(void) 
** @Input     : void
** @Output    : void
** @Function  : RUN_LED灯
** @The notes : (GPIO口状态)
**===============================================================================================*/
void Test_LEDALM(void);

/*=================================================================================================
** @Name      : void Test_IMDALM(void) 
** @Input     : void
** @Output    : void
** @Function  : IMD_ALM灯
** @The notes : (GPIO口状态)
**===============================================================================================*/
void Test_IMDALM(void);

/*=================================================================================================
** @Name      : void Test_CRELAY1Ctrl(void) 
** @Input     : void
** @Output    : void
** @Function  : CRELAY1
** @The notes : 继电器状态
**===============================================================================================*/
void Test_CRELAY1Ctrl(void);

/*=================================================================================================
** @Name      : void Test_CRELAY1Read(void) 
** @Input     : void
** @Output    : void
** @Function  : CRELAY1
** @The notes : 继电器状态
**===============================================================================================*/
void Test_CRELAY1Read(void);

/*=================================================================================================
** @Name      : void Test_CRELAY2Ctrl(void) 
** @Input     : void
** @Output    : void
** @Function  : CRELAY2
** @The notes : 继电器状态
**===============================================================================================*/
void Test_CRELAY2Ctrl(void);

/*=================================================================================================
** @Name      : void Test_CRELAY2Read(void) 
** @Input     : void
** @Output    : void
** @Function  : CRELAY2
** @The notes : 继电器状态
**===============================================================================================*/
void Test_CRELAY2Read(void);

/*=================================================================================================
** @Name      : void Test_PERLYCtrl(void) 
** @Input     : void
** @Output    : void
** @Function  : PERLY
** @The notes : (GPIO口状态)
**===============================================================================================*/
void Test_PERLYCtrl(void);

/*=================================================================================================
** @Name      : void Test_PERLYRead(void) 
** @Input     : void
** @Output    : void
** @Function  : PERLY
** @The notes : (GPIO口状态)
**===============================================================================================*/
void Test_PERLYRead(void);

/*=================================================================================================
** @Name      : void Test_Eep_Flash_WriteRead(void) 
** @Input     :
** @Output    : 
** @Function  : 测试EEPROM和Flash存储器的写入和读取功能
** @The notes : 
**===============================================================================================*/
void Test_Eep_Flash_WriteRead(void);

/*=================================================================================================
** @Name      : void Test_KA84917Commu(void)
** @Input     : void
** @Output    : void
** @Function  : KA84917和KA84922、RN8209通信是否正常
** @The notes : 初始化芯片然后写入测试数据，再将测试数据读取出来，比较判断，如果通信正常再测试RN8209
**===============================================================================================*/
void Test_KA84917Com(void);

void Test_RN8209Com(void);
/*=================================================================================================
** @Name      : void Test_CANFDCom(void) 
** @Input     : void
** @Output    : void
** @Function  : CANFD通信状态测试，向测试板发送数据，并等待回复数据，判断是否一致
** @The notes :
**===============================================================================================*/
void Test_CANFDCom(void);


/*=================================================================================================
** @Name      : void ToolingCheckCANReviceAPI(e_CANNUM CanNum, t_CANMSG* msg)
** @Input     : void
** @Output    : void
** @Function  : CANFD接收数据,接收测试板发送的数据
** @The notes :
**===============================================================================================*/
u8 ToolingCheckCANReviceAPI(t_CANMSG msg, u8 canNum);

/*=================================================================================================
** @Name      : void Test_NTC_TEMP(void) 
** @Input     : void
** @Output    : void
** @Function  : NTC温度值
** @The notes : 数值为真实值的10倍数，单位0.1°C
**===============================================================================================*/
void Test_NTC_TEMP(void);

/*=================================================================================================
** @Name      : void Test_VCC_5V(void) 
** @Input     : void
** @Output    : void
** @Function  : VCC_5V
** @The notes : 数值为真实值的10倍数，单位0.1°C
**===============================================================================================*/
void Test_VCC_5V(void);

/*=================================================================================================
** @Name      : void Test_HALL_5V(void) 
** @Input     : void
** @Output    : void
** @Function  : HALL_5V
** @The notes : 数值为真实值的10倍数，单位0.1°C
**===============================================================================================*/
void Test_HALL_5V(void);

/*=================================================================================================
** @Name      : void Test_ShuntTemp(void) 
** @Input     : void
** @Output    : void
** @Function  : 分流器温度TEMP1
** @The notes : 数值为真实值的10倍数，单位0.1°C
**===============================================================================================*/
void Test_ShuntTemp(void);

/*=================================================================================================
** @Name      : void Test_IMDRLYCtrl(void) 
** @Input     : void
** @Output    : void
** @Function  : IMDRLY
** @The notes : 0：断开继电器  1：闭合继电器
**===============================================================================================*/
void Test_IMDRLYCtrl(void);

/*=================================================================================================
** @Name      : void Test_IMDRLYRead(void) 
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 
**===============================================================================================*/
void Test_IMDRLYRead(void);

/*=================================================================================================
** @Name      : void Test_HV5V(void) 
** @Input     : void
** @Output    : void
** @Function  : HV_5V
** @The notes : 数值为真实值的10倍数，单位0.1V
**===============================================================================================*/
void Test_HV5V(void);

/*=================================================================================================
** @Name      : void Test_HV5VAD(void) 
** @Input     : void
** @Output    : void
** @Function  : HV_-5V_AD
** @The notes : 数值为真实绝对值的10倍数，单位0.1V
**===============================================================================================*/
void Test_HV_5VAD(void);

/*=================================================================================================
** @Name      : static u8 GetTargetAddr(u8 local_addr) 
** @Input     : local_addr 本设备拨码地址
** @Output    : target_addr 目标设备地址
** @Function  : CANFD通信状态测试，向测试板发送数据，并等待回复数据，判断是否一致
** @The notes :
**===============================================================================================*/
u8 GetTargetAddr(u8 local_addr);

/*=================================================================================================
** @Name      : void ToolingCheckInitEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 工装测试eep参数初始化
** @The notes : 如果EEP中有参数则读取存储的，没有则使用默认值赋值
**===============================================================================================*/
void ToolingCheckInitEEP(void);

void ToolingCheckInit(void);
u8 SafeFlash_Read_Parameter(u8* Data);
u8 SafeFlash_Write_Parameter(u8* Data);
u8 SafeFlash_Erase_Parameter(void);

u8 SafeTestFlash_Read_Parameter(u8* Data, u32 size);
u8 SafeTestFlash_Write_Parameter(u8* Data, u32 size);
u8 SafeTestFlash_Erase_Parameter(void);

#endif