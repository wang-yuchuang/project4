/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Calibration.c
** @Founder      : WYP40505
** @Date         : 20260228
** @Function     : IMIU4工装模块
** @Instructions : 
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "Calibration.h"
#include "BSPTypeDef.h"
#include "ToolingCheck.h"
#include "SystemInfo.h"
#include "SystemData.h"
#include "Modbus.h"
#include "BSPGD25Q32E.h"
#include "BSPSPI.h"
#include "SystemPara.h"
#include "SystemDebug.h"
#include "EEPROM.h"
#include "BSPKA84917_async.h"
#include "BSPDIDO.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static CalibrationConfig g_configs[CAL_TYPE_MAX];     //校准寄存器索引
static CalibrationParams g_params[CAL_TYPE_MAX];      //校准参数存储
static Calibration_State  g_states[CAL_TYPE_MAX] = {CAL_STATE_IDLE}; //校准状态
CalibrationParamKB ParamKB;
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=================================================================================================
** @Name      : static void FloatToRegisters(float32 value, u16* high_word, u16* low_word)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 浮点数转换为寄存器数据
**===============================================================================================*/
static void FloatToRegisters(float32 value, u16* high_word, u16* low_word);
/*=================================================================================================
** @Name      : static float32 RegistersToFloat(u16 high_word, u16 low_word)
** @Input     :
** @Output    : 
** @Function  : 
** @The notes : 寄存器数据转换为浮点数
**===============================================================================================*/
static f32 RegistersToFloat(u16 high_word, u16 low_word);

/*=================================================================================================
** @Name      : const CalibrationConfig* CalibrationGetConfig(Calibration_Type type)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 获取校准配置
**===============================================================================================*/
const CalibrationConfig* CalibrationGetConfig(Calibration_Type type);

/*=================================================================================================
** @Name      : static u8 ReadCalibrationFromEEPFlash(Calibration_Type type, CalibrationParams* params)
** @Input     : type：params：
** @Output    : 读取是否成功
** @Function  : 
** @The notes : 从外部存储读取参数
**===============================================================================================*/
static u8 ReadCalibrationFromEEPFlash(Calibration_Type type, CalibrationParams* params);


/*=================================================================================================
** @Name      : static u8 SaveCalibrationToParamKB(Calibration_Type type, const CalibrationParams* params)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 更新校准参数到CalibrationParamKB
**===============================================================================================*/
static u8 SaveCalibrationToParamKB(Calibration_Type type, const CalibrationParams* params);

/*=================================================================================================
** @Name      : static u8 SaveCalibrationToEEPFlash(u8 type)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 保存校准参数到外部存储
**===============================================================================================*/
static u8 SaveCalibrationToEEPFlash(u8 type);
/*=================================================================================================
** @Name      : static void ProcessCalibrationMode(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 处理校准模式
**===============================================================================================*/
static void ProcessCalibrationMode(Calibration_Type type);

/*=================================================================================================
** @Name      : static void ProcessTestMode(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 处理测试模式 状态切换
**===============================================================================================*/
static void ProcessTestMode(Calibration_Type type);

/*=================================================================================================
** @Name      : static void CalculateSinglePointCalibration(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 执行单点校准计算
**===============================================================================================*/
static void CalculateSinglePointCalibration(Calibration_Type type);

/*=================================================================================================
** @Name      : static void CalculateTwoPointCalibration(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 执行双点校准计算
**===============================================================================================*/
static void CalculateTwoPointCalibration(Calibration_Type type);

/*=================================================================================================
** @Name      : static void CalculateTwoPointCalibration(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 校准后使用校准系数测试
**===============================================================================================*/
static f32 ApplyCalibration(Calibration_Type type, f32 raw_value);

/*=================================================================================================
** @Name      : static void ProcessCalibration_State(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 处理校准状态
**===============================================================================================*/
static void ProcessCalibration_State(Calibration_Type type);

/*=================================================================================================
** @Name      : static void CalibrationUpdateCoeffToModbus(Calibration_Type type, f32 k, f32 b ,f32 z)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 将校准系数更新到MODBUS寄存器中
**===============================================================================================*/
static void CalibrationUpdateCoeffToModbus(Calibration_Type type, f32 k, f32 b ,f32 z);


/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=================================================================================================
** @Name      : void Calibration_Init(void)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 初始化校准模块
**===============================================================================================*/
void Calibration_Init(void)
{
    //初始化霍尔电流校准配置
    g_configs[CAL_TYPE_HALL_CURRENT].type                   = CAL_TYPE_HALL_CURRENT;

    g_configs[CAL_TYPE_HALL_CURRENT].reg_cal_mode           = REG_HALL_CURRENT_CAL_MODE;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_test_mode          = REG_HALL_CAL_TEST_MODE;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_inputz             = REG_HALL_ZERO_INPUT;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_completez          = REG_HALL_ZERO_COMPLETE;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_input1             = REG_HALL_CAL_INPUT_1;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_complete1          = REG_HALL_CAL_COMPLETE_1;    
    g_configs[CAL_TYPE_HALL_CURRENT].reg_input2             = REG_HALL_CAL_INPUT_2;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_complete2          = REG_HALL_CAL_COMPLETE_2;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_coeff_k_high       = REG_HALL_CAL_COEFF_K1_H;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_coeff_k_low        = REG_HALL_CAL_COEFF_K1_L;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_coeff_b_high       = REG_HALL_CAL_COEFF_B1_H;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_coeff_b_low        = REG_HALL_CAL_COEFF_B1_L;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_zero_offset_high   = REG_HALL_CAL_Z1_H;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_zero_offset_low    = REG_HALL_CAL_Z1_L;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_calibrated_value   = REG_HALL_CALIBRATED_CURRENT;
    g_configs[CAL_TYPE_HALL_CURRENT].reg_measured_value     = REG_HALL_MEASURING_CURRENT;

    g_configs[CAL_TYPE_HALL_CURRENT].default_k = 1.0f;
    g_configs[CAL_TYPE_HALL_CURRENT].default_b = 0.0f;
    g_configs[CAL_TYPE_HALL_CURRENT].get_actual_value = GetfAdjustZeroHallVolt;//校零后电压
    g_configs[CAL_TYPE_HALL_CURRENT].get_Zero_value = GetHallVolt;//校零前电压
    

    //初始化PE电压校准配置
    g_configs[CAL_TYPE_PE_VOLTAGE].type                   = CAL_TYPE_PE_VOLTAGE;

    g_configs[CAL_TYPE_PE_VOLTAGE].reg_cal_mode           = REG_PE_VOLTAGE_CAL_MODE;  
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_test_mode          = REG_PE_CAL_TEST_MODE ;    
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_input1H            = REG_PE_CAL_INPUT_1H ; 
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_input1L            = REG_PE_CAL_INPUT_1L ;     
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_complete1          = REG_PE_CAL_COMPLETE_1 ;   
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_input2H            = REG_PE_CAL_INPUT_2H ; 
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_input2L            = REG_PE_CAL_INPUT_2L ;     
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_complete2          = REG_PE_CAL_COMPLETE_2 ;   
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_coeff_k_high       = REG_PE_CAL_COEFF_K1_H ;   
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_coeff_k_low        = REG_PE_CAL_COEFF_K1_L ;   
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_coeff_b_high       = REG_PE_CAL_COEFF_B1_H ;   
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_coeff_b_low        = REG_PE_CAL_COEFF_B1_L ;     
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_calibrated_valueH  = REG_PE_CALIBRATED_VOLTAGEH;
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_calibrated_valueL  = REG_PE_CALIBRATED_VOLTAGEL;
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_measured_valueH    = REG_PE_MEASURING_VOLTAGEH;
    g_configs[CAL_TYPE_PE_VOLTAGE].reg_measured_valueL    = REG_PE_MEASURING_VOLTAGEL;
   
    g_configs[CAL_TYPE_PE_VOLTAGE].default_k = 1.0f;
    g_configs[CAL_TYPE_PE_VOLTAGE].default_b = 0.0f;
    g_configs[CAL_TYPE_PE_VOLTAGE].get_actual_value = GetPeGndVolt;//GetPeGndVolt



    //初始化BAT电压校准配置
    g_configs[CAL_TYPE_BAT_VOLTAGE].type                    = CAL_TYPE_BAT_VOLTAGE;

    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_cal_mode            = REG_BAT_VOLTAGE_CAL_MODE;  
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_test_mode           = REG_BAT_CAL_TEST_MODE;     
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_input1H             = REG_BAT_CAL_INPUT_1H;
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_input1L             = REG_BAT_CAL_INPUT_1L;
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_complete1           = REG_BAT_CAL_COMPLETE_1;    
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_input2H             = REG_BAT_CAL_INPUT_2H; 
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_input2L             = REG_BAT_CAL_INPUT_2L;  
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_complete2           = REG_BAT_CAL_COMPLETE_2;   
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_coeff_k_high        = REG_BAT_CAL_COEFF_K1_H;   
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_coeff_k_low         = REG_BAT_CAL_COEFF_K1_L;   
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_coeff_b_high        = REG_BAT_CAL_COEFF_B1_H;   
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_coeff_b_low         = REG_BAT_CAL_COEFF_B1_L; 
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_calibrated_valueH   = REG_BAT_CALIBRATED_VOLTAGEH;
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_calibrated_valueL   = REG_BAT_CALIBRATED_VOLTAGEL;
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_measured_valueH     = REG_BAT_MEASURING_VOLTAGEH;
    g_configs[CAL_TYPE_BAT_VOLTAGE].reg_measured_valueL     = REG_BAT_MEASURING_VOLTAGEL;
  
    
    g_configs[CAL_TYPE_BAT_VOLTAGE].default_k = 1.0f;
    g_configs[CAL_TYPE_BAT_VOLTAGE].default_b = 0.0f;
    g_configs[CAL_TYPE_BAT_VOLTAGE].get_actual_value = GetBatVolt; //GetBatVolt


    // 初始化KA84917充电电流校准配置
    g_configs[CAL_TYPE_CHG917_CURRENT].type                    = CAL_TYPE_CHG917_CURRENT;

    g_configs[CAL_TYPE_CHG917_CURRENT].reg_cal_mode            =  REG_KA84917_CURRENT_CHG_MODE;        
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_test_mode           =  REG_KA84917_CHG_TEST_MODE;                          
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_input1H             =  REG_KA84917_CHG_INPUT_1H;
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_input1L             =  REG_KA84917_CHG_INPUT_1L;
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_complete1           =  REG_KA84917_CHG_COMPLETE_1 ;               
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_input2H             =  REG_KA84917_CHG_INPUT_2H;
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_input2L             =  REG_KA84917_CHG_INPUT_2L;
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_complete2           =  REG_KA84917_CHG_COMPLETE_2 ;               
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_coeff_k_high        =  REG_KA84917_CHG_COEFF_K1_H;                
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_coeff_k_low         =  REG_KA84917_CHG_COEFF_K1_L;             
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_coeff_b_high        =  REG_KA84917_CHG_COEFF_B1_H;              
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_coeff_b_low         =  REG_KA84917_CHG_COEFF_B1_L;  
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_calibrated_valueH    = REG_KA84917_CALIBRATED_CHG_CURRENTH;
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_calibrated_valueL    = REG_KA84917_CALIBRATED_CHG_CURRENTL;
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_measured_valueH      = REG_KA84917_MEASURING_CHG_CURRENTH;
    g_configs[CAL_TYPE_CHG917_CURRENT].reg_measured_valueL      = REG_KA84917_MEASURING_CHG_CURRENTL;
    
    g_configs[CAL_TYPE_CHG917_CURRENT].default_k = 1.0f;
    g_configs[CAL_TYPE_CHG917_CURRENT].default_b = 0.0f;
    g_configs[CAL_TYPE_CHG917_CURRENT].get_actual_value = GetBatShuntVolt;//BatShuntVolt

     // 初始化KA84917放电电流校准配置
    g_configs[CAL_TYPE_DHG917_CURRENT].type                    = CAL_TYPE_DHG917_CURRENT;

    g_configs[CAL_TYPE_DHG917_CURRENT].reg_cal_mode            =  REG_KA84917_DISCHG_CURRENT_CAL_MODE;      
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_test_mode           =  REG_KA84917_DISCHG_TEST_MODE;                             
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_input1H              =  REG_KA84917_DISCHG_INPUT_1H;
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_input1L              =  REG_KA84917_DISCHG_INPUT_1L;
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_complete1           =  REG_KA84917_DISCHG_COMPLETE_1;                  
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_input2H              =  REG_KA84917_DISCHG_INPUT_2H;
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_input2L              =  REG_KA84917_DISCHG_INPUT_2L;
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_complete2           =  REG_KA84917_DISCHG_COMPLETE_2 ;                 
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_coeff_k_high        =  REG_KA84917_DISCHG_COEFF_K1_H;                  
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_coeff_k_low         =  REG_KA84917_DISCHG_COEFF_K1_L;               
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_coeff_b_high        =  REG_KA84917_DISCHG_COEFF_B1_H;                 
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_coeff_b_low         =  REG_KA84917_DISCHG_COEFF_B1_L;
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_calibrated_valueH    = REG_KA84917_CALIBRATED_DISCHG_CURRENTH;
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_calibrated_valueL    = REG_KA84917_CALIBRATED_DISCHG_CURRENTL;
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_measured_valueH      = REG_KA84917_MEASURING_DISCHG_CURRENTH;
    g_configs[CAL_TYPE_DHG917_CURRENT].reg_measured_valueL      = REG_KA84917_MEASURING_DISCHG_CURRENTL;
 
          
    
    g_configs[CAL_TYPE_DHG917_CURRENT].default_k = 1.0f;
    g_configs[CAL_TYPE_DHG917_CURRENT].default_b = 0.0f;
    g_configs[CAL_TYPE_DHG917_CURRENT].get_actual_value = GetBatShuntVolt;//BATShunt
 

     // 初始化RN8209电压校准配置
    g_configs[CAL_TYPE_RN8209_VOLTAGE].type                     = CAL_TYPE_RN8209_VOLTAGE;

    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_cal_mode             =  REG_RN8209_VOLTAGE_CAL_MODE;  
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_test_mode            =  REG_RN8209_VOLTAGE_CAL_TEST_MODE;
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_input1H              =  REG_RN8209_VOLTAGE_CAL_INPUT_1H ;
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_input1L              =  REG_RN8209_VOLTAGE_CAL_INPUT_1L ;              
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_complete1            =  REG_RN8209_VOLTAGE_CAL_COMPLETE_1 ;         
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_input2H              =  REG_RN8209_VOLTAGE_CAL_INPUT_2H;
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_input2L              =  REG_RN8209_VOLTAGE_CAL_INPUT_2L;       
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_complete2            =  REG_RN8209_VOLTAGE_CAL_COMPLETE_2;          
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_coeff_k_high         =  REG_RN8209_VOLTAGE_CAL_COEFF_K1_H;          
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_coeff_k_low          =  REG_RN8209_VOLTAGE_CAL_COEFF_K1_L;       
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_coeff_b_high         =  REG_RN8209_VOLTAGE_CAL_COEFF_B1_H;        
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_coeff_b_low          =  REG_RN8209_VOLTAGE_CAL_COEFF_B1_L;
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_calibrated_valueH    =  REG_RN8209_VOLTAGE_CALIBRATEDH; 
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_calibrated_valueL    =  REG_RN8209_VOLTAGE_CALIBRATEDL;
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_measured_valueH      =  REG_RN8209_MEASURING_VOLTAGEH;
    g_configs[CAL_TYPE_RN8209_VOLTAGE].reg_measured_valueL      =  REG_RN8209_MEASURING_VOLTAGEL;

    g_configs[CAL_TYPE_RN8209_VOLTAGE].default_k = 1.0f;
    g_configs[CAL_TYPE_RN8209_VOLTAGE].default_b = 0.0f;
    g_configs[CAL_TYPE_RN8209_VOLTAGE].get_actual_value = GetLinkVolt;//LinkVolt
  

    // 初始化RN8209充电电流校准配置
    g_configs[CAL_TYPE_GHG8209_CURRENT].type                 = CAL_TYPE_GHG8209_CURRENT;

    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_cal_mode         =  REG_RN8209_CHG_CURRENT_CAL_MODE;      
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_test_mode        =  REG_RN8209_CHG_TEST_MODE;                           
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_input1H          =  REG_RN8209_CHG_INPUT_1H;
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_input1L          =  REG_RN8209_CHG_INPUT_1L;
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_complete1        =  REG_RN8209_CHG_COMPLETE_1 ;         
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_input2H          =  REG_RN8209_CHG_INPUT_2H;
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_input2L          =  REG_RN8209_CHG_INPUT_2L;
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_complete2        =  REG_RN8209_CHG_COMPLETE_2;           
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_coeff_k_high     =  REG_RN8209_CHG_COEFF_K1_H ;          
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_coeff_k_low      =  REG_RN8209_CHG_COEFF_K1_L;           
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_coeff_b_high     =  REG_RN8209_CHG_COEFF_B1_H;           
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_coeff_b_low      =  REG_RN8209_CHG_COEFF_B1_L;   
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_calibrated_valueH = REG_RN8209_CALIBRATED_CHG_CURRENTH;
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_calibrated_valueL = REG_RN8209_CALIBRATED_CHG_CURRENTL;
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_measured_valueH   = REG_RN8209_MEASURING_CHG_CURRENTH;
    g_configs[CAL_TYPE_GHG8209_CURRENT].reg_measured_valueL   = REG_RN8209_MEASURING_CHG_CURRENTL;
   
    g_configs[CAL_TYPE_GHG8209_CURRENT].default_k = 1.0f;
    g_configs[CAL_TYPE_GHG8209_CURRENT].default_b = 0.0f;
    g_configs[CAL_TYPE_GHG8209_CURRENT].get_actual_value = GetLinkShuntVolt;//LinkShuntVolt

    // 初始化RN8209充电电流校准配置
    g_configs[CAL_TYPE_DHG8209_CURRENT].type                 = CAL_TYPE_DHG8209_CURRENT;

    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_cal_mode         =  REG_RN8209_DISCHG_CURRENT_CAL_MODE;     
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_test_mode        =  REG_RN8209_DISCHG_TEST_MODE;                          
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_input1H          =  REG_RN8209_DISCHG_INPUT_1H ;
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_input1L          =  REG_RN8209_DISCHG_INPUT_1L ;
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_complete1        =  REG_RN8209_DISCHG_COMPLETE_1;         
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_input2H          =  REG_RN8209_DISCHG_INPUT_2H;
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_input2L          =  REG_RN8209_DISCHG_INPUT_2L;
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_complete2        =  REG_RN8209_DISCHG_COMPLETE_2 ;         
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_coeff_k_high     =  REG_RN8209_DISCHG_COEFF_K1_H ;         
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_coeff_k_low      =  REG_RN8209_DISCHG_COEFF_K1_L ;         
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_coeff_b_high     =  REG_RN8209_DISCHG_COEFF_B1_H ;;         
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_coeff_b_low      =  REG_RN8209_DISCHG_COEFF_B1_L;         
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_calibrated_valueH =  REG_RN8209_CALIBRATED_DISCHG_CURRENTH;
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_calibrated_valueL =  REG_RN8209_CALIBRATED_DISCHG_CURRENTL;
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_measured_valueH   = REG_RN8209_MEASURING_DISCHG_CURRENTH;
    g_configs[CAL_TYPE_DHG8209_CURRENT].reg_measured_valueL   = REG_RN8209_MEASURING_DISCHG_CURRENTL;

    g_configs[CAL_TYPE_DHG8209_CURRENT].default_k = 1.0f;
    g_configs[CAL_TYPE_DHG8209_CURRENT].default_b = 0.0f;
    g_configs[CAL_TYPE_DHG8209_CURRENT].get_actual_value = GetLinkShuntVolt;//LinkShuntVolt


    //初始化校准参数
    for (u16 i = 0; i < CAL_TYPE_MAX; i++)
    {
        g_params[i].k = g_configs[i].default_k;
        g_params[i].b = g_configs[i].default_b;
        g_params[i].zero_offset = 0.0f;

        g_params[i].input1 = 0.0f;
        g_params[i].input2 = 0.0f;
        g_params[i].inputZero = 0.0f;

        g_params[i].actual1 = 0.0f;
        g_params[i].actual2 = 0.0f;
        g_params[i].actualZero = 0.0f;
        g_params[i].actualTest = 0.0f;
        

        g_params[i].input1flag = 0;
        g_params[i].input2flag = 0;
        g_params[i].inputZeroFlag = 0;

        g_params[i].valid_flag1 = 0;
        g_params[i].valid_flag2 = 0;
        g_params[i].inputZeroFlag = 0;
    }
}

/*=================================================================================================
** @Name      : void Calibration_Process(void)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 校准任务处理函数
**===============================================================================================*/
void Calibration_Process(void)
{
    // 只初始化一次
    static u8 cal_init_done = 0;
    if (cal_init_done == 0)
    {
        Calibration_Init();
        cal_init_done = 1;
    }
    
    for (u16 i = 0; i < CAL_TYPE_MAX; i++)
    {
        //校准模式
        ProcessCalibrationMode((Calibration_Type)i);

        //测试模式
        ProcessTestMode((Calibration_Type)i);

        //校准状态
        ProcessCalibration_State((Calibration_Type)i);
    }
}

/*=================================================================================================
** @Name      : void CalibrationReadCoeffFromModbus(Calibration_Type type, f32* k, float32 *b)
** @Input     : 校准类型 k b 
** @Output    : void
** @Function  : 
** @The notes : 从MODBUS寄存器中读取校准系数
**===============================================================================================*/
void CalibrationReadCoeffFromModbus(Calibration_Type type, f32* k, float32 *b)
{
    const CalibrationConfig* config = CalibrationGetConfig(type);
    if (config == NULL || k == NULL || b == NULL)
    {
        return;
    }
    //从modbus寄存器中读取数据
    u16 k_high = (ModbusReg[config->reg_coeff_k_high][0] << 8)|ModbusReg[config->reg_coeff_k_high][1];
    u16 k_low  = (ModbusReg[config->reg_coeff_k_low][0] << 8)|ModbusReg[config->reg_coeff_k_low][1];
    u16 b_high = (ModbusReg[config->reg_coeff_b_high][0] << 8)|ModbusReg[config->reg_coeff_b_high][1];
    u16 b_low  = (ModbusReg[config->reg_coeff_b_low][0] << 8)|ModbusReg[config->reg_coeff_b_low][1];

    *k = RegistersToFloat(k_high, k_low);
    *b = RegistersToFloat(b_high, b_low);
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=================================================================================================
** @Name      : static void FloatToRegisters(float32 value, u16* high_word, u16* low_word)
** @Input     : void
** @Output    : 
** @Function  : 
** @The notes : 浮点数转换为寄存器数据
**===============================================================================================*/
static void FloatToRegisters(float32 value, u16* high_word, u16* low_word)
{
    FloatConverter converter;
    converter.float_val = value;
    // 小端序转换
    *low_word = (converter.bytes.byte1 << 8) | converter.bytes.byte0;
    *high_word = (converter.bytes.byte3 << 8) | converter.bytes.byte2;
}
/*=================================================================================================
** @Name      : static float32 RegistersToFloat(u16 high_word, u16 low_word)
** @Input     :
** @Output    : 
** @Function  : 
** @The notes : 寄存器数据转换为浮点数
**===============================================================================================*/
static f32 RegistersToFloat(u16 high_word, u16 low_word)
{
    FloatConverter converter;
    // 小端序转换
    converter.bytes.byte0 = low_word & 0xFF;
    converter.bytes.byte1 = (low_word >> 8) & 0xFF;
    converter.bytes.byte2 = high_word & 0xFF;
    converter.bytes.byte3 = (high_word >> 8) & 0xFF;
    return converter.float_val;
}
/*=================================================================================================
** @Name      : const CalibrationConfig* CalibrationGetConfig(Calibration_Type type)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 获取校准配置
**===============================================================================================*/
const CalibrationConfig* CalibrationGetConfig(Calibration_Type type)
{
    if (type < CAL_TYPE_MAX)
    {
        return &g_configs[type];
    }
    return NULL;
}


/*=================================================================================================
** @Name      : static u8 ReadCalibrationFromEEPFlash(Calibration_Type type, CalibrationParams* params)
** @Input     : type：params：
** @Output    : 读取是否成功
** @Function  : 
** @The notes : 从外部存储读取参数
**===============================================================================================*/
static u8 ReadCalibrationFromEEPFlash(Calibration_Type type, CalibrationParams* params)
{
    u8 result = false;
    u8 para_pass;

    //测试eep和flash读取参数

    static Parameter_TypeDef gSystemParaE;
    // u8* rbuff = (u8 *) &gSystemParaE;

     // 判断flash和EEP中参数是否相同
    if (true == I2C_EE_BufferRead((u8*)&gSystemParaE,Eeprom_ParaAddress_Start,sizeof(gSystemParaE)))
    {
        if (GD25Qx_OK ==SafeFlash_Read_Parameter((u8*)&stParameter))
        {     
            //判断参数是否相同    
            if ((stParameter.BatVoltKBFlag == gSystemParaE.BatVoltKBFlag)
            &&(stParameter.fBatVoltK == gSystemParaE.fBatVoltK)
            &&(stParameter.fBatVoltB == gSystemParaE.fBatVoltB)
            &&(stParameter.PeVoltKBFlag == gSystemParaE.PeVoltKBFlag)
            &&(stParameter.fPeVoltK == gSystemParaE.fPeVoltK)
            &&(stParameter.fPeVoltB == gSystemParaE.fPeVoltB)
            &&(stParameter.LinkVoltKBFlag == gSystemParaE.LinkVoltKBFlag)
            &&(stParameter.fLinkVoltK == gSystemParaE.fLinkVoltK)
            &&(stParameter.fLinkVoltB == gSystemParaE.fLinkVoltB)
            &&(stParameter.HallCurrKBFlag == gSystemParaE.HallCurrKBFlag)
            &&(stParameter.fHallCurrentK == gSystemParaE.fHallCurrentK)
            &&(stParameter.fHallCurrentB == gSystemParaE.fHallCurrentB)
            &&(stParameter.BatShuntChgKBFlag == gSystemParaE.BatShuntChgKBFlag)
            &&(stParameter.fBatShuntCurrentChgK == gSystemParaE.fBatShuntCurrentChgK)
            &&(stParameter.fBatShuntCurrentChgB == gSystemParaE.fBatShuntCurrentChgB)
            &&(stParameter.BatShuntDhgKBFlag == gSystemParaE.BatShuntDhgKBFlag)
            &&(stParameter.fBatShuntCurrentDhgK == gSystemParaE.fBatShuntCurrentDhgK)
            &&(stParameter.fBatShuntCurrentDhgB == gSystemParaE.fBatShuntCurrentDhgB)
            &&(stParameter.LinkShuntChgKBFlag == gSystemParaE.LinkShuntChgKBFlag)         
            &&(stParameter.fLinkShuntCurrentChgK == gSystemParaE.fLinkShuntCurrentChgK)
            &&(stParameter.fLinkShuntCurrentChgB == gSystemParaE.fLinkShuntCurrentChgB)
            &&(stParameter.LinkShuntDhgKBFlag == gSystemParaE.LinkShuntDhgKBFlag)
            &&(stParameter.fLinkShuntCurrentDhgK == gSystemParaE.fLinkShuntCurrentDhgK)
            &&(stParameter.fLinkShuntCurrentDhgK == gSystemParaE.fLinkShuntCurrentDhgK)
            &&(stParameter.fLinkShuntCurrentDhgB == gSystemParaE.fLinkShuntCurrentDhgB)
            &&(stParameter.HallZeroCorrFlag == gSystemParaE.HallZeroCorrFlag)
            &&(stParameter.fHallZeroCorr == gSystemParaE.fHallZeroCorr))
            {
                para_pass = 1;
            }
            
        }
    }

    //参数flag有效，并且flash与eep中参数相同
    if((ModbusReg[REG_PARA_FLAG][1] == 0x5A)&& (para_pass == 1))
    {
        switch(type) 
        {
            case CAL_TYPE_HALL_CURRENT:
                params->k = stParameter.fHallCurrentK;
                params->b = stParameter.fHallCurrentB;
                params->zero_offset = stParameter.fHallZeroCorr;
                result = true;
            break;

            case CAL_TYPE_PE_VOLTAGE:
                params->k = stParameter.fPeVoltK;
                params->b = stParameter.fPeVoltB; 
                result = true;
            break;
    
            case CAL_TYPE_BAT_VOLTAGE:
                params->k = stParameter.fBatVoltK;
                params->b = stParameter.fBatVoltB;
                result = true;
            break;

            case CAL_TYPE_CHG917_CURRENT:
                params->k = stParameter.fBatShuntCurrentChgK;
                params->b = stParameter.fBatShuntCurrentChgB;
                result = true;
            break;

            case CAL_TYPE_DHG917_CURRENT:
                params->k = stParameter.fBatShuntCurrentDhgK;
                params->b = stParameter.fBatShuntCurrentDhgB;
                result = true;
            break;

            case CAL_TYPE_RN8209_VOLTAGE:
                params->k = stParameter.fLinkVoltK;
                params->b = stParameter.fLinkVoltB;
                result = true;
            break;

            case CAL_TYPE_GHG8209_CURRENT:
                params->k = stParameter.fLinkShuntCurrentChgK;
                params->b = stParameter.fLinkShuntCurrentChgB;
                result = true;
            break;

            case CAL_TYPE_DHG8209_CURRENT:
                params->k = stParameter.fLinkShuntCurrentDhgK;
                params->b = stParameter.fLinkShuntCurrentDhgB;
                result = true;
            break;
    
            default:
            result = false;
        }
    }
    return result;
}


/*=================================================================================================
** @Name      : static u8 SaveCalibrationToEEPFlash(u8 type)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 保存校准参数到外部存储
**===============================================================================================*/
static u8 SaveCalibrationToEEPFlash(u8 type)
{
    u8 cfgResultF = FALSE;
    u8 cfgResultE = FALSE;
    u8 finalResult = 0x00;

    static Parameter_TypeDef gSystemParaBuff;
    static Parameter_TypeDef verifyBuff;
    // u8* rbuff = (u8 *) &gSystemParaBuff;

    // 读取现有参数eep
    if (true == I2C_EE_BufferRead((u8*)&gSystemParaBuff,Eeprom_ParaAddress_Start,sizeof(stParameter)))
    {
        switch (type)
        {
                //HALL校零值保存
        case CAL_TYPE_HALL_CURRENT_ZERO:
            stParameter.HallZeroCorrFlag           = ParamKB.HallZeroCFlag;
            stParameter.fHallZeroCorr              = ParamKB.fHallZero;
            break;

         case CAL_TYPE_HALL_CURRENT:
            //读取HALL校零值保存stParameter
            stParameter.HallZeroCorrFlag           = ParamKB.HallZeroCFlag;
            stParameter.fHallZeroCorr              = ParamKB.fHallZero;
            stParameter.HallCurrKBFlag              = ParamKB.HallCKBFlag;
            stParameter.fHallCurrentK               = ParamKB.fHallCK;
            stParameter.fHallCurrentB               = ParamKB.fHallCB;
            break;

        case CAL_TYPE_PE_VOLTAGE:
            stParameter.PeVoltKBFlag                = ParamKB.PeVKBFlag;
            stParameter.fPeVoltK                    = ParamKB.fPeVK;
            stParameter.fPeVoltB                    = ParamKB.fPeVB;
            break;

        case CAL_TYPE_BAT_VOLTAGE:
            stParameter.BatVoltKBFlag               = ParamKB.BatVKBFlag;
            stParameter.fBatVoltK                   = ParamKB.fBatVK;
            stParameter.fBatVoltB                   = ParamKB.fBatVB;
            break;

        case CAL_TYPE_CHG917_CURRENT:
            stParameter.BatShuntChgKBFlag           = ParamKB.BatShuntChgFlag;
            stParameter.fBatShuntCurrentChgK        = ParamKB.fBatShuntCChgK;
            stParameter.fBatShuntCurrentChgB        = ParamKB.fBatShuntCChgB;
            break;

        case CAL_TYPE_DHG917_CURRENT:
            stParameter.BatShuntDhgKBFlag          = ParamKB.BatShuntDhgFlag;
            stParameter.fBatShuntCurrentDhgK       = ParamKB.fBatShuntCDhgK;
            stParameter.fBatShuntCurrentDhgB       = ParamKB.fBatShuntCDhgB;
            break;

        case CAL_TYPE_RN8209_VOLTAGE:
            stParameter.LinkVoltKBFlag             =  ParamKB.LinkVKBFlag;
            stParameter.fLinkVoltK                 =  ParamKB.fLinkVK;
            stParameter.fLinkVoltB                 =  ParamKB.fLinkVB;
            break;

        case CAL_TYPE_GHG8209_CURRENT:
            stParameter.LinkShuntChgKBFlag          = ParamKB.LinkShuntChgFlag;
            stParameter.fLinkShuntCurrentChgK       = ParamKB.fLinkShuntCChgK;
            stParameter.fLinkShuntCurrentChgB       = ParamKB.fLinkShuntCChgB;
            break;

        case CAL_TYPE_DHG8209_CURRENT:
            stParameter.LinkShuntDhgKBFlag          = ParamKB.LinkShuntDhgFlag;
            stParameter.fLinkShuntCurrentDhgK       = ParamKB.fLinkShuntCDhgK;
            stParameter.fLinkShuntCurrentDhgB       = ParamKB.fLinkShuntCDhgB;
            break;
        
        default:
            break;
        }
            }
    
    else//eep 无效
    {
        if (GD25Qx_OK !=SafeFlash_Read_Parameter((u8*)&stParameter))
        {
            //flash 和 eep都无效
            return 0x00;
        }
    }

    //校准单个数据保存一次FLASH
    if (stParameter.WriteFlag == 0x5A)
    {
        if ((stParameter.HallZeroCorrFlag == 1)
        || (stParameter.HallCurrKBFlag == 1)
        ||(stParameter.BatVoltKBFlag == 1)
        ||(stParameter.PeVoltKBFlag == 1)
        ||(stParameter.LinkVoltKBFlag == 1)
        ||(stParameter.BatShuntDhgKBFlag == 1)
        ||(stParameter.BatShuntChgKBFlag == 1)
        ||(stParameter.LinkShuntChgKBFlag == 1)
        ||(stParameter.LinkShuntDhgKBFlag == 1))
        {
            if(GD25Qx_OK == SafeFlash_Erase_Parameter())
            {
                if(GD25Qx_OK == SafeFlash_Write_Parameter((u8*)&stParameter))
                {
                     // 写入后立即回读验证
                    memset(&verifyBuff, 0, sizeof(verifyBuff));
                    if(GD25Qx_OK == SafeFlash_Read_Parameter((u8*)&verifyBuff))
                    {
                        // 比较写入的数据和读回的数据
                        if(memcmp(&stParameter, &verifyBuff, sizeof(stParameter)) == 0)
                        {
                            cfgResultF = TRUE;
                        }
                        else
                        {
                            // 验证失败，可以尝试重新写入或记录错误
                            cfgResultF = FALSE;
                        }
                    }
                }
            }  
        } 
    }

    //所有校准数据测试完成之后再保存eep
    if (stParameter.WriteFlag == 0x5A)
    {
        if ((stParameter.HallZeroCorrFlag == 1)
        && (stParameter.HallCurrKBFlag == 1)
        &&(stParameter.BatVoltKBFlag == 1)
        &&(stParameter.PeVoltKBFlag == 1)
        &&(stParameter.LinkVoltKBFlag == 1)
        &&(stParameter.BatShuntDhgKBFlag == 1)
        &&(stParameter.BatShuntChgKBFlag == 1)
        &&(stParameter.LinkShuntChgKBFlag == 1)
        &&(stParameter.LinkShuntDhgKBFlag == 1))
        {
            if (true == I2C_EE_BufferWrite((u8*)&stParameter, Eeprom_ParaAddress_Start, sizeof(stParameter)))
            {
                // 写入后立即回读验证
                memset(&verifyBuff, 0, sizeof(verifyBuff));
                if (true == I2C_EE_BufferRead((u8*)&verifyBuff, Eeprom_ParaAddress_Start, sizeof(stParameter)))
                {
                    if(memcmp(&stParameter, &verifyBuff, sizeof(stParameter)) == 0)
                    {
                        cfgResultE = TRUE;
                    }
                }
            }  
        }
    }

     // 构造返回结果
     finalResult = 0x00;
     if (cfgResultF == TRUE)
     {
         finalResult |= 0x01; // bit0 = 1 表示Flash保存成功
     }
     if (cfgResultE == TRUE)
     {
         finalResult |= 0x02; // bit1 = 1 表示EEPROM保存成功
     }
     
     return finalResult;
}


/*=================================================================================================
** @Name      : static u8 SaveCalibrationToParamKB(Calibration_Type type, const CalibrationParams* params)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 更新校准参数到CalibrationParamKB
**===============================================================================================*/
static u8 SaveCalibrationToParamKB(Calibration_Type type, const CalibrationParams* params)
{
    u8 cfgResult = FALSE;
   
    // 2. 根据校准类型更新对应参数
    switch(type) 
    {
        //HALL校零值保存
        case CAL_TYPE_HALL_CURRENT_ZERO:
        ParamKB.HallZeroCFlag    = 1;
        ParamKB.fHallZero        = params->zero_offset;
        break;

        case CAL_TYPE_HALL_CURRENT:
        ParamKB.HallCKBFlag  = 1;
        ParamKB.fHallCK      = params->k;
        ParamKB.fHallCB     = params->b;
        ParamKB.HallZeroCFlag    = 1;
        ParamKB.fHallZero        = params->zero_offset;
        break;

        case CAL_TYPE_PE_VOLTAGE:
        ParamKB.PeVKBFlag = 1;
        ParamKB.fPeVK       = params->k;
        ParamKB.fPeVB       = params->b;
        break;

        case CAL_TYPE_BAT_VOLTAGE:
        ParamKB.BatVKBFlag      = 1; 
        ParamKB.fBatVK      = params->k;
        ParamKB.fBatVB      = params->b;
        break;

        case CAL_TYPE_CHG917_CURRENT:
        ParamKB.BatShuntChgFlag = 1;
        ParamKB.fBatShuntCChgK  = params->k;
        ParamKB.fBatShuntCChgB  = params->b;
        break;

        case CAL_TYPE_DHG917_CURRENT:
        ParamKB.BatShuntDhgFlag = 1;
        ParamKB.fBatShuntCDhgK  = params->k;
        ParamKB.fBatShuntCDhgB  = params->b;
        break;

        case CAL_TYPE_RN8209_VOLTAGE:
        ParamKB.LinkVKBFlag = 1;
        ParamKB.fLinkVK     = params->k;
        ParamKB.fLinkVB     = params->b;
        break;

        case CAL_TYPE_GHG8209_CURRENT:
        ParamKB.LinkShuntChgFlag = 1;
        ParamKB.fLinkShuntCChgK     = params->k;
        ParamKB.fLinkShuntCChgB     = params->b;
        break;

        case CAL_TYPE_DHG8209_CURRENT:
        ParamKB.LinkShuntDhgFlag = 1;
        ParamKB.fLinkShuntCDhgK     = params->k;
        ParamKB.fLinkShuntCDhgB     = params->b;
        break;

        default:
        cfgResult = FALSE;
        return cfgResult;
    }
    
    return cfgResult;
}
/*=================================================================================================
** @Name      : static void ProcessCalibrationMode(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 处理校准模式
**===============================================================================================*/
static void ProcessCalibrationMode(Calibration_Type type)
{
    u8 cal_mode = ModbusReg[g_configs[type].reg_cal_mode][1];
    CalibrationParams* params = &g_params[type];

    if (cal_mode == 0)
    {
      
        return;
    }

    switch (cal_mode)
    {
    case CAL_MODE_DEFAULT: //使用默认的参数 k = 1， b = 0
        params->k = g_configs[type].default_k;
        params->b = g_configs[type].default_b;
        params->valid_flag1 = 1;
        break;

    case CAL_MODE_READ_EXT: //从外部存储读取
        if (ReadCalibrationFromEEPFlash(type,params))
        {
            //读取成功，更新寄存器
            CalibrationUpdateCoeffToModbus(type,params->k,params->b,params->zero_offset);
        }
        break;

    case CAL_MODE_SAVE_EXT: //保存到外部存储并读取

        //保存到Flash
        if (0 != SaveCalibrationToEEPFlash(type))
        {
            // 保存成功，从Flash读取验证
            CalibrationParams verify_params;
            //保存成功后从FALSH中读取验证
            if (ReadCalibrationFromEEPFlash(type,&verify_params))
            {
                //如果验证成功，更新寄存器
                CalibrationUpdateCoeffToModbus(type,verify_params.k,verify_params.b,verify_params.zero_offset);
            }
        }
        //清空校准模式寄存器
        ModbusReg[g_configs[type].reg_cal_mode][1] = 0;
        break;
    }
}
/*=================================================================================================
** @Name      : static void ProcessTestMode(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 处理测试模式 状态切换
**===============================================================================================*/
static void ProcessTestMode(Calibration_Type type)
{
    u8 test_mode = ModbusReg[g_configs[type].reg_test_mode][1];
    CalibrationParams* params = &g_params[type];
    if (test_mode == 0)
    {
        return;
    }
    switch (test_mode)
    {
    case 1: //计算系数，单点校准
        if (g_states[type] == CAL_STATE_IDLE)
        {
            g_states[type] = CAL_STATE_WAIT_INPUT1;
        }
        break;
    case 2: //计算系数，双点校准
        if (g_states[type] == CAL_STATE_IDLE)
        {
            g_states[type] = CAL_STATE_WAIT_INPUT1;
        }
        break;
    case 3: //校准后的测试
            g_states[type] = CAL_STATE_TESTING;
        break;
    
    case 4: // HALL校零计算（仅对HALL电流有效）
        if (type == CAL_TYPE_HALL_CURRENT)
        {
            g_states[type] = CAL_STATE_ZEROFIN;
        }
        break;
    case 5: // 不进行HALL校准，只读取HALL当前校准前的电压值
        if (type == CAL_TYPE_HALL_CURRENT)
        {
            params->actualTest = g_configs[type].get_actual_value();
            u16 test_value =  (u16)(params->actualTest);
            ModbusReg[g_configs[type].reg_measured_value][0] = (test_value >> 8) & 0xff;
            ModbusReg[g_configs[type].reg_measured_value][1] = (test_value) & 0xff;
            //默认值
            params->k = g_configs[type].default_k;
            params->b = g_configs[type].default_b;
            params->zero_offset = 0;
            //校准参数写入ParamKB
            SaveCalibrationToParamKB(type,params);
            //更新系数到寄存器
            CalibrationUpdateCoeffToModbus(type, g_params[type].k, g_params[type].b, g_params[type].zero_offset);
            //保存到Flash
            if (0 != SaveCalibrationToEEPFlash(type))
            {
                // 保存成功，从Flash读取验证
                CalibrationParams verify_params;
                //保存成功后从FALSH中读取验证
                if (ReadCalibrationFromEEPFlash(type,&verify_params))
                {
                    //如果验证成功，更新寄存器
                    CalibrationUpdateCoeffToModbus(type,verify_params.k,verify_params.b,verify_params.zero_offset);
                }
            }
            //清空校准模式寄存器
            ModbusReg[g_configs[type].reg_test_mode][1] = 0;
        }
        break;
    }
}

/*=================================================================================================
** @Name      : static void CalculateSinglePointCalibration(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 执行单点校准计算
**===============================================================================================*/
static void CalculateSinglePointCalibration(Calibration_Type type)
{
    CalibrationParams* params = &g_params[type];
    // 单点校准公式: y = x + b, k = 1, 只计算偏移量
    if (FABS(params->input1) > 1e-6f)
    {
        params->k = 1.0f;
        params->b = params->input1 - params->actual1;
    }
    else
    {
        params->k = 1.0f;
        params->b = 0.0f;
    }
    params->valid_flag1 = 1;
}


/*=================================================================================================
** @Name      : static void HALLZeroCalibration(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : Hall校零
**===============================================================================================*/
static void HALLZeroCalibration(Calibration_Type type)
{
    CalibrationParams* params = &g_params[type];
    // HALL校零只计算偏移量
    if (params->inputZeroFlag != 0)
    {
        params->zero_offset = params->inputZero - params->actualZero;
    }
    else
    {
        params->zero_offset= 0.0f;
    }
    params->valid_flag_Zero = 1;
}

/*=================================================================================================
** @Name      : static void CalculateTwoPointCalibration(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 执行双点校准计算
**===============================================================================================*/
static void CalculateTwoPointCalibration(Calibration_Type type)
{
    CalibrationParams* params = &g_params[type];

    if (params->input2 == params->input1)
    {
        CalculateSinglePointCalibration(type);
        return;
    }
    // 双点校准公式: y = k*x + b
    // 使用两点(x1,y1)和(x2,y2)计算k和b
    if (params->input2 != params->input1)
    {
        // 计算斜率 k = (y2 - y1) / (x2 - x1)
        params->k = (params->input2 - params->input1) / ( params->actual2 - params->actual1);
        // 计算偏移 b = y1 - k*x1
        params->b = params->input1 - params->k * params->actual1;
    }
    else
    {
        CalculateSinglePointCalibration(type);
    }
    params->valid_flag2 = 1;
}

/*=================================================================================================
** @Name      : static void CalculateTwoPointCalibration(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 获得校准系数后计算校准后的电流电压值
**===============================================================================================*/
static f32 ApplyCalibration(Calibration_Type type, f32 raw_value)
{
    //根据不同电压电流采样类型     获取对应值
    // const CalibrationParams* params = &g_params[type];
    f32 calibrated_data = 0;
    //flash参数有效标识
    if (ModbusReg[REG_PARA_FLAG][1] == 0x5A)
    {
        switch (type)
        {
        case CAL_TYPE_HALL_CURRENT:
            calibrated_data = GetfAdjustHallVolt();
            return calibrated_data;
            break;

        case CAL_TYPE_PE_VOLTAGE:
            calibrated_data = GetfAdjustPeGndVolt();
            return calibrated_data;
            break;

        case CAL_TYPE_BAT_VOLTAGE:
            calibrated_data = GetfAdjustBatVolt();
            return calibrated_data;
            break;

        case CAL_TYPE_CHG917_CURRENT:
            calibrated_data = GetfAdjustBatShuntVolt();
            return calibrated_data;
            break;

        case CAL_TYPE_DHG917_CURRENT:
            calibrated_data = GetfAdjustBatShuntVolt();
            return calibrated_data;
            break;

        case CAL_TYPE_RN8209_VOLTAGE:
            calibrated_data = GetfAdjustLinkVolt();
            return calibrated_data;
            break;
        case CAL_TYPE_GHG8209_CURRENT:
            calibrated_data = GetfAdjustLinkShuntVolt();
            return calibrated_data;
            break;

        case CAL_TYPE_DHG8209_CURRENT:
            calibrated_data = GetfAdjustLinkShuntVolt();
            return calibrated_data;
            break;

        default:
            return raw_value;
            break;
        }
    }
    return raw_value;
}
/*=================================================================================================
** @Name      : static void ProcessCalibration_State(Calibration_Type type)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 处理校准状态
**===============================================================================================*/
static void ProcessCalibration_State(Calibration_Type type)
{
    CalibrationParams* params = &g_params[type];
    u8 test_mode = ModbusReg[g_configs[type].reg_test_mode][1];

    switch (g_states[type])
    {
        case CAL_STATE_WAIT_INPUT1: 
        //外部输入完成标志1
        if (ModbusReg[g_configs[type].reg_complete1][1] == 1)
        {
            params->input1flag = 1;

            //PE校准需要闭合继电器
            if ((type == CAL_TYPE_PE_VOLTAGE)&& (g_states[type] == CAL_STATE_WAIT_INPUT1))
            {
                PE_RLY_Contr_Close();
            }
            
            // 分别获取输入值1
            // 获取实际测量值1  保存到寄存器中 上位机读取

            //（高压部分）
            if ((type == CAL_TYPE_BAT_VOLTAGE)||(type == CAL_TYPE_RN8209_VOLTAGE))
            {

                u16 high1 = (ModbusReg[g_configs[type].reg_input1H][0] << 8) | ModbusReg[g_configs[type].reg_input1H][1];
                u16 low1 = (ModbusReg[g_configs[type].reg_input1L][0] << 8) | ModbusReg[g_configs[type].reg_input1L][1];     
                u32 value1 = ((u32)high1 << 16) | low1;
                float32 input1  = (float32)(s32)value1 / Tooling_OC;
                params->input1 = input1;

                osDelay(500);
                
                params->actual1 = g_configs[type].get_actual_value();
                s32 reg_value =  (s32)(params->actual1 *Tooling_OC);

                ModbusReg[g_configs[type].reg_measured_valueH][0] = (reg_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueH][1] = (reg_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][0] = (reg_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][1] = (reg_value >> 0) & 0xff;
            }
            //PE校准部分
            else if (type == CAL_TYPE_PE_VOLTAGE)
            {
                 u16 high1 = (ModbusReg[g_configs[type].reg_input1H][0] << 8) | ModbusReg[g_configs[type].reg_input1H][1];
                 u16 low1 = (ModbusReg[g_configs[type].reg_input1L][0] << 8) | ModbusReg[g_configs[type].reg_input1L][1];     
                 u32 value1 = ((u32)high1 << 16) | low1;
                 float32 input1  = (float32)(s32)value1 * PE_OC / Tooling_OC;
                 params->input1 = input1;
 
                 osDelay(500);
                //获取实际测量值1  保存到寄存器中 上位机读取
                 params->actual1 = g_configs[type].get_actual_value();
                 s32 reg_value =  (s32)(params->actual1 *Tooling_OC);
 
                 ModbusReg[g_configs[type].reg_measured_valueH][0] = (reg_value >> 24) & 0xff;
                 ModbusReg[g_configs[type].reg_measured_valueH][1] = (reg_value >> 16) & 0xff;
                 ModbusReg[g_configs[type].reg_measured_valueL][0] = (reg_value >> 8) & 0xff;
                 ModbusReg[g_configs[type].reg_measured_valueL][1] = (reg_value >> 0) & 0xff;
            }
           
            //分流器等低压 (H/L双寄存器精度升级)
            else
            {
                u16 high1 = (ModbusReg[g_configs[type].reg_input1H][0] << 8) | ModbusReg[g_configs[type].reg_input1H][1];
                u16 low1  = (ModbusReg[g_configs[type].reg_input1L][0] << 8) | ModbusReg[g_configs[type].reg_input1L][1];
                u32 value1 = ((u32)high1 << 16) | low1;
                params->input1 = (float32)(s32)value1 / Tooling_OC;

                osDelay(500);
                //获取实际测量值1  保存到寄存器中 上位机读取
                params->actual1 = g_configs[type].get_actual_value();
                s32 reg_value =  (s32)(params->actual1 * Tooling_OC);
                ModbusReg[g_configs[type].reg_measured_valueH][0] = (reg_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueH][1] = (reg_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][0] = (reg_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][1] = (reg_value >> 0) & 0xff;
            }
            //清空完成标志
            ModbusReg[g_configs[type].reg_complete1][1] = 0;

        }
        else
        {
            params->input1flag = 0;
        }

        if ((test_mode == 1))
        {
            if (params->input1flag == 1){
                g_states[type] = CAL_STATE_CALCULATING;}
            else{
                g_states[type] = CAL_STATE_IDLE;}
        }
        else if ((test_mode == 2))
        {
            if (params->input1flag == 1)
            {
                g_states[type] = CAL_STATE_WAIT_INPUT2;
            }
            else{
                g_states[type] = CAL_STATE_IDLE;
                ModbusReg[g_configs[type].reg_complete1][1] = 0;}
        }
        break;

    case CAL_STATE_WAIT_INPUT2:
        //等待外部输入完成标志2
        if (ModbusReg[g_configs[type].reg_complete2][1] == 1)
        {
            params->input2flag = 1;

            //PE校准需要闭合继电器
            if ((type == CAL_TYPE_PE_VOLTAGE) && (g_states[type] == CAL_STATE_WAIT_INPUT2))
            {
                PE_RLY_Contr_Close();
            }

            //获取输入值2
            //获取实际测量值2  保存到寄存器中 上位机读取

            //（高压）
            if ((type == CAL_TYPE_BAT_VOLTAGE)||(type == CAL_TYPE_RN8209_VOLTAGE))
            {
                u16 high2 = (ModbusReg[g_configs[type].reg_input2H][0] << 8) | ModbusReg[g_configs[type].reg_input2H][1];
                u16 low2 = (ModbusReg[g_configs[type].reg_input2L][0] << 8) | ModbusReg[g_configs[type].reg_input2L][1];
                u32 value2 = ((u32)high2 << 16) | low2;
                float32 input2 =  (float32)(s32)value2 / Tooling_OC ;
                params->input2 =input2;

                osDelay(500);
                params->actual2 = g_configs[type].get_actual_value();
                s32 reg_value =  (s32)(params->actual2 *Tooling_OC);
                ModbusReg[g_configs[type].reg_measured_valueH][0] = (reg_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueH][1] = (reg_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][0] = (reg_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][1] = (reg_value >> 0) & 0xff;
            }
            //PE
            else if (type == CAL_TYPE_PE_VOLTAGE)
            {
                u16 high2 = (ModbusReg[g_configs[type].reg_input2H][0] << 8) | ModbusReg[g_configs[type].reg_input2H][1];
                u16 low2 = (ModbusReg[g_configs[type].reg_input2L][0] << 8) | ModbusReg[g_configs[type].reg_input2L][1];
                u32 value2 = ((u32)high2 << 16) | low2;
                float32 input2 =  (float32)(s32)value2 * PE_OC / Tooling_OC;
                params->input2 =input2;

                osDelay(500);
                params->actual2 = g_configs[type].get_actual_value();
                s32 reg_value =  (s32)(params->actual2 *Tooling_OC);
                ModbusReg[g_configs[type].reg_measured_valueH][0] = (reg_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueH][1] = (reg_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][0] = (reg_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][1] = (reg_value >> 0) & 0xff;
            }
            //分流器低压 (H/L双寄存器精度升级)
            else
            {
                u16 high2 = (ModbusReg[g_configs[type].reg_input2H][0] << 8) | ModbusReg[g_configs[type].reg_input2H][1];
                u16 low2  = (ModbusReg[g_configs[type].reg_input2L][0] << 8) | ModbusReg[g_configs[type].reg_input2L][1];
                u32 value2 = ((u32)high2 << 16) | low2;
                params->input2 = (float32)(s32)value2 / Tooling_OC;

                osDelay(500);
                params->actual2 = g_configs[type].get_actual_value();
                s32 reg_value =  (s32)(params->actual2 * Tooling_OC);
                ModbusReg[g_configs[type].reg_measured_valueH][0] = (reg_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueH][1] = (reg_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][0] = (reg_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][1] = (reg_value >> 0) & 0xff;
            }

            //清空完成标志
            ModbusReg[g_configs[type].reg_complete2][1] = 0;
            
            if (test_mode == 2)
            {
                // 两个校准值输入结束，开始双点校准计算
                g_states[type] = CAL_STATE_CALCULATING;
            }
        }
        else{
            //无input2，等待输入电压2
            params->input2flag = 0;

            if (test_mode == 2)
            {
                // 保持当前状态，继续等待输入
                g_states[type] = CAL_STATE_WAIT_INPUT2;
            }
            else
            {
                // 如果不是双点校准模式，才回到IDLE
                g_states[type] = CAL_STATE_IDLE;
                ModbusReg[g_configs[type].reg_complete2][1] = 0;
            }
        }
        break;


    case CAL_STATE_CALCULATING:

        if ((test_mode == 1) && (params->input1flag == 1)) //单点校准
        {
            CalculateSinglePointCalibration(type);
        }
        else if ((test_mode == 2) && (params->input2flag == 1)) //双点校准
        {
            CalculateTwoPointCalibration(type);
        }
        else 
        {
            // 如果条件不满足，返回空闲状态
            g_states[type] = CAL_STATE_IDLE;
            break;
        }
        //校准参数写入ParamKB
        SaveCalibrationToParamKB(type,params);
        //更新系数到寄存器
        CalibrationUpdateCoeffToModbus(type, g_params[type].k, g_params[type].b, g_params[type].zero_offset);
    
        // 清空测试模式寄存器
        ModbusReg[g_configs[type].reg_test_mode][1] = 0;
        
        // 重置输入标志
        params->input1flag = 0;
        params->input2flag = 0;
        g_states[type] = CAL_STATE_COMPLETED;
        break;

    case CAL_STATE_TESTING:
        //校准后测试
        if ((params->valid_flag1 == 1) || (params->valid_flag2 == 1))
        {
            //获取当前的实际值
            osDelay(200);
            f32 current_value = g_configs[type].get_actual_value();

            osDelay(200);
            // 计算校准后的值 
            f32 calibrated_value = ApplyCalibration(type,current_value);
            
            //高压
            if ((type == CAL_TYPE_PE_VOLTAGE)||(type == CAL_TYPE_BAT_VOLTAGE)||(type == CAL_TYPE_RN8209_VOLTAGE))
            {
                //校准后值保存到寄存器中
                u32 reg_value =  (u32)(calibrated_value * Tooling_OC);
                ModbusReg[g_configs[type].reg_calibrated_valueH][0] = (reg_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_calibrated_valueH][1] = (reg_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_calibrated_valueL][0] = (reg_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_calibrated_valueL][1] = (reg_value) & 0xff;

                //获取实际测量值  //保存到寄存器中 上位机读取
                osDelay(200);
                params->actualTest = g_configs[type].get_actual_value();
                u32 test_value =  (u32)(params->actualTest  * Tooling_OC);
                ModbusReg[g_configs[type].reg_measured_valueH][0] = (test_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueH][1] = (test_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][0] = (test_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][1] = (test_value) & 0xff;
            }
            else
            {
                //校准后值保存到寄存器中 (H/L双寄存器精度升级)
                s32 reg_value =  (s32)(calibrated_value * Tooling_OC);
                ModbusReg[g_configs[type].reg_calibrated_valueH][0] = (reg_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_calibrated_valueH][1] = (reg_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_calibrated_valueL][0] = (reg_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_calibrated_valueL][1] = (reg_value) & 0xff;

                //获取实际测量值  //保存到寄存器中 上位机读取
                osDelay(200);
                params->actualTest = g_configs[type].get_actual_value();
                s32 test_value =  (s32)(params->actualTest * Tooling_OC);
                ModbusReg[g_configs[type].reg_measured_valueH][0] = (test_value >> 24) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueH][1] = (test_value >> 16) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][0] = (test_value >> 8) & 0xff;
                ModbusReg[g_configs[type].reg_measured_valueL][1] = (test_value) & 0xff;
            }

        //清空测试模式寄存器
        ModbusReg[g_configs[type].reg_test_mode][1] = 0;
            }
            
        g_states[type] = CAL_STATE_IDLE;
        break;

        //校零 仅霍尔电流
        case CAL_STATE_ZEROFIN:
        {
            if (type == CAL_TYPE_HALL_CURRENT)
            {

            // 获取实际测量值（应该是零电压时的测量值）
            params->actualZero = g_configs[type].get_Zero_value();
            
            //保存到寄存器中
            u16 reg_value =  (u16)(params->actualZero);
            ModbusReg[g_configs[type].reg_measured_value][0] = (reg_value >> 8) & 0xff;
            ModbusReg[g_configs[type].reg_measured_value][1] = (reg_value) & 0xff;

            //获取输入值
            params->inputZeroFlag = ModbusReg[REG_HALL_ZERO_COMPLETE][1];
            params->inputZero = (ModbusReg[REG_HALL_ZERO_INPUT][0] << 8) | ModbusReg[REG_HALL_ZERO_INPUT][1];

            //hall校零值计算
            if ( ModbusReg[REG_HALL_CAL_TEST_MODE][1] == 4)
            {
                HALLZeroCalibration(type);
            }


            // 更新寄存器中的校零值
            u16 z_high, z_low;
            FloatToRegisters(g_params[type].zero_offset, &z_high, &z_low);
            
            ModbusReg[g_configs[type].reg_zero_offset_high][0] = (z_high >> 8) & 0xFF;
            ModbusReg[g_configs[type].reg_zero_offset_high][1] = z_high & 0xFF;
            
            ModbusReg[g_configs[type].reg_zero_offset_low][0] = (z_low >> 8) & 0xFF;
            ModbusReg[g_configs[type].reg_zero_offset_low][1] = z_low & 0xFF;
            
            //hall校零后  将校零值写入结构体
            SaveCalibrationToParamKB(CAL_TYPE_HALL_CURRENT_ZERO,params);

            // 清空测试模式寄存器
            ModbusReg[REG_HALL_ZERO_COMPLETE][1] = 0;
            
            g_states[type] = CAL_STATE_COMPLETED;
            ModbusReg[REG_HALL_CAL_TEST_MODE][1] = 0;
            }
            break;
        }

    case CAL_STATE_COMPLETED:
        //保持完成状态一段时间，然后回到空闲
        g_states[type] = CAL_STATE_IDLE;
        break;

    case CAL_STATE_ERROR:
        g_states[type] = CAL_STATE_IDLE;
        break;
    
    default:
        break;
    }
}



/*=================================================================================================
** @Name      : static void CalibrationUpdateCoeffToModbus(Calibration_Type type, f32 k, f32 b ,f32 z)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 将校准系数更新到MODBUS寄存器中
**===============================================================================================*/
static void CalibrationUpdateCoeffToModbus(Calibration_Type type, f32 k, f32 b ,f32 z)
{
    const CalibrationConfig* config = CalibrationGetConfig(type);
    if (config == NULL)
    {
        return;
    }
    if (type == CAL_TYPE_HALL_CURRENT)
    {
         // 更新寄存器中的校零值
         u16 z_high, z_low;
         FloatToRegisters(z, &z_high, &z_low);
         
         ModbusReg[g_configs[type].reg_zero_offset_high][0] = (z_high >> 8) & 0xFF;
         ModbusReg[g_configs[type].reg_zero_offset_high][1] = z_high & 0xFF;
         
         ModbusReg[g_configs[type].reg_zero_offset_low][0] = (z_low >> 8) & 0xFF;
         ModbusReg[g_configs[type].reg_zero_offset_low][1] = z_low & 0xFF;
    }

    u16 k_high, k_low, b_high, b_low;

    //将浮点数转换为u16
    FloatToRegisters(k, &k_high, &k_low);
    FloatToRegisters(b, &b_high, &b_low);

    //更新Modbus寄存器
    ModbusReg[config->reg_coeff_k_high][0] = (k_high >> 8) & 0xFF;
    ModbusReg[config->reg_coeff_k_high][1] = k_high & 0xFF;
    
    ModbusReg[config->reg_coeff_k_low][0] = (k_low >> 8) & 0xFF;
    ModbusReg[config->reg_coeff_k_low][1] = k_low & 0xFF;
    
    ModbusReg[config->reg_coeff_b_high][0] = (b_high >> 8) & 0xFF;
    ModbusReg[config->reg_coeff_b_high][1] = b_high & 0xFF;
    
    ModbusReg[config->reg_coeff_b_low][0] = (b_low >> 8) & 0xFF;
    ModbusReg[config->reg_coeff_b_low][1] = b_low & 0xFF;
}

