/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Calibration.h
** @Founder      : WYP40505
** @Date         : 20260228
** @Function     : IMIU4工装模块
** @Instructions : 
**===========================================================================================*/
#ifndef _CALIBRATION_H
#define _CALIBRATION_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/

#include "BSPTypeDef.h"
#include "Modbus.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern  u8 CURRENT_CALIBRATION_FLAG;
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define FLASH_CALIBRATION_BASE  0x00001000                  //flash校准参数存储基地址
#define FLASH_CALIBRATION_SIZE  sizeof(CalibrationParams)   //每个校准参数的大小
#define Tooling_OC         1000.0f  
#define PE_OC              (1/5606.2*2606.2)

//校准值类型
typedef enum 
{
    CAL_TYPE_HALL_CURRENT = 0,  //霍尔电流校准
    CAL_TYPE_PE_VOLTAGE,        //PE电压校准
    CAL_TYPE_BAT_VOLTAGE,       //BAT电压校准
    CAL_TYPE_CHG917_CURRENT,   //KA84917充电电流校准
    CAL_TYPE_DHG917_CURRENT,   //KA84917放电电流校准
    CAL_TYPE_RN8209_VOLTAGE,    //RN8209电压校准
    CAL_TYPE_GHG8209_CURRENT,    //RN8209充电电流校准
    CAL_TYPE_DHG8209_CURRENT,    //RN8209放电电流校准
    CAL_TYPE_MAX,
    CAL_TYPE_HALL_CURRENT_ZERO = 0xff //HALL校零值保存(置于CAL_TYPE_MAX之后,不影响数组大小)
}Calibration_Type;

//校准模式枚举
typedef enum
{
    CAL_MODE_NONE = 0,          //无动作
    CAL_MODE_DEFAULT,           //使用默认参数k=1,b=0
    CAL_MODE_READ_EXT,          //从外部存储读取参数
    CAL_MODE_SAVE_EXT,          //保存参数到外部存储并读取
}Calibration_Mode;

//校准状态枚举
typedef enum
{
    CAL_STATE_IDLE = 0,         //空闲状态
    CAL_STATE_WAIT_INPUT1,      //等待输入1完成
    CAL_STATE_WAIT_INPUT2,      //等待输入2完成
    CAL_STATE_CALCULATING,      //计算系数中
    CAL_STATE_TESTING,          //测试中
    CAL_STATE_COMPLETED,        //完成
    CAL_STATE_ERROR,            //错误
    CAL_STATE_ZEROFIN,         //校零          
}Calibration_State;

//校准配置结构体
typedef struct 
{
    Calibration_Type type;       //校准类型

    u16 reg_cal_mode;           //校准模式寄存器地址
    u16 reg_test_mode;          //测试模式寄存器地址

    u16 reg_inputz;             //校零值输入地址
    u16 reg_completez;          //校零值输入地址

    u16 reg_input1;             //输入1寄存器地址(低压)
    u16 reg_input1H;             //输入1寄存器地址H(高压)
    u16 reg_input1L;             //输入1寄存器地址L(高压)
    u16 reg_complete1;          //完成1寄存器地址

    u16 reg_input2;             //输入2寄存器地址(低压)
    u16 reg_input2H;             //输入2寄存器地址H(高压)
    u16 reg_input2L;             //输入2寄存器地址L(高压)
    u16 reg_complete2;          //完成2寄存器地址

    u16 reg_coeff_k_high;       //K系数高16位寄存器地址
    u16 reg_coeff_k_low;        //K系数低16位寄存器地址

    u16 reg_coeff_b_high;       //B系数高16位寄存器地址
    u16 reg_coeff_b_low;        //B系数低16位寄存器地址

    u16 reg_zero_offset_high;   // 校零值高16位寄存器地址
    u16 reg_zero_offset_low;    // 校零值低16位寄存器地址

    u16 reg_calibrated_value;    //校准后值寄存器地址(低压)

    u16 reg_measured_value;      //实际测量值寄存器地址(低压)

    u16 reg_calibrated_valueH;   //校准后值寄存器地址H(高压)
    u16 reg_calibrated_valueL;   //校准后值寄存器地址L(高压)
    
    u16 reg_measured_valueH;    //实际测量值寄存器地址H(高压)
    u16 reg_measured_valueL;    //实际测量值寄存器地址L(高压)


    f32 default_k;              //默认的K值
    f32 default_b;              //默认的B值

    f32 (*get_actual_value)(void); //获取实际值的函数指针
     f32 (*get_Zero_value)(void); //获取校零后实际值的函数指针
}CalibrationConfig;

//校准参数结构体
typedef struct 
{
    
    f32 k;                  //K
    f32 b;                  //B
    f32 zero_offset;        //校零值

    f32 inputZero;          //校零输入值
    f32 input1;             //输入值1
    f32 input2;             //输入值2

    f32 actualZero;         //实际校零值
    f32 actual1;            //实际值1
    f32 actual2;            //实际值2

    u8 inputZeroFlag;       //校零值输入完成标志
    u8 input1flag;          //实际值1完成标志
    u8 input2flag;          //实际值2完成标志

    u8 valid_flag1;          //计算单点校准参数有效标志
    u8 valid_flag2;          //计算双点校准参数有效标志
    u8 valid_flag_Zero;      //计算校零结果参数有效标志

    //校零测试
    f32 actualTest;         //测试值


}CalibrationParams;



//校准参数暂存
typedef struct 
{
    u32 BatVKBFlag;                 
    f32 fBatVK;                     
    f32 fBatVB;  

    u32 PeVKBFlag;                  
    f32 fPeVK;                      
    f32 fPeVB;    

    u32 LinkVKBFlag;                
    f32 fLinkVK;                    
    f32 fLinkVB;  

    u32 HallCKBFlag;                
    f32 fHallCK;                 
    f32 fHallCB;                 

    u32 BatShuntChgFlag;             
    f32 fBatShuntCChgK;          
    f32 fBatShuntCChgB;

    u32 BatShuntDhgFlag;             
    f32 fBatShuntCDhgK;          
    f32 fBatShuntCDhgB;

    u32 LinkShuntChgFlag;            
    f32 fLinkShuntCChgK;         
    f32 fLinkShuntCChgB; 

    u32 LinkShuntDhgFlag;            
    f32 fLinkShuntCDhgK;         
    f32 fLinkShuntCDhgB;         

    u32 HallZeroCFlag;              
    f32 fHallZero;                 

}CalibrationParamKB;

//共用体，浮点数和寄存器数据之间的转换
typedef union 
{
    f32 float_val;
    struct 
    {
        u16 high_word;
        u16 low_word;
    }words;
    struct
    {
        u8 byte0;
        u8 byte1;
        u8 byte2;
        u8 byte3;
    }bytes;
}FloatConverter;


/*=================================================================================================
** @Name      : void Calibration_Init(void)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 初始化校准模块
**===============================================================================================*/
void Calibration_Init(void);

/*=================================================================================================
** @Name      : void Calibration_Process(void)
** @Input     : void
** @Output    : void
** @Function  : 
** @The notes : 校准任务处理函数
**===============================================================================================*/
void Calibration_Process(void);



/*=================================================================================================
** @Name      : void CalibrationReadCoeffFromModbus(Calibration_Type type, f32* k, float32 *b)
** @Input     : 
** @Output    : 
** @Function  : 
** @The notes : 从MODBUS寄存器中读取校准系数
**===============================================================================================*/
void CalibrationReadCoeffFromModbus(Calibration_Type type, f32* k, float32 *b);


#endif
