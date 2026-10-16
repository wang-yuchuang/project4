/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : ToolingCheck.c
** @Founder      : WYP40505
** @Date         : 20260228
** @Function     : IMIU4工装模块
** @Instructions : 
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "Modbus.h"
#include "ToolingCheck.h"
#include "BSPGD25Q32E.h"
#include "SystemInfo.h"
#include "BSPCAN.h"
#include "BSPUSART.h"
#include "BSPDIDO.h"
#include "BSPKA84917_async.h"
#include "drv_rn8209.h"
#include "drv_rn8209_type.h"
#include "drv_rn8209_interface.h"
#include "Calibration.h"
#include "SystemPara.h"
#include "SystemData.h"
#include "EEPROM.h"
#include "BSPWDOG.h"

/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
u8 FDCANRData[eCANNUM][64]; //接收数据
volatile bool gCanfdRecivedFlag  = false;
SemaphoreHandle_t xFlashMutex = NULL;

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/

/*=================================================================================================
** @Name      : void ToolingCheckFunctionTask(void)  
** @Input     : void
** @Output    : void
** @Function  : 工装测试模块
** @The notes : 
**===============================================================================================*/
void ToolingCheckFunctionTask(void)
{
    static u8 MutexInit = 0 ;

    if (MutexInit != 1)
    {
        ToolingCheckInit();
        ToolingCheckInitEEP();
        
        MutexInit = 1;
    }
    
    Test_CANFDCom();                      //CANFD通信状态                    (两个板子测试)
    Test_KA84917Com();                   //KA84917通信是否正常   //KA84922通信是否正常
    Test_RN8209Com();                       //RN8209通信是否正常
    Test_Eep_Flash_WriteRead();            //E2PRON \ FLASH读写测试      (地址)
    BSPWatchDogRefresh();              //喂狗: 通信测试+Flash操作后

    Test_SWAddr();                  //上传拨码开关
    Test_DI1();                     //上传DI1状态
    Test_HallPFault();              // HALL_P_FAULT
    Test_Diagnosis();               // Diagnosis
    Test_LEDALM();                  //上传LED灯
    Test_IMDALM();                  //上传ALM灯状态
    //RW
    Test_CRELAY1Ctrl();             //两路DO状态 CRELAY1
    Test_CRELAY1Read();
    Test_CRELAY2Ctrl();             //两路DO状态 CRELAY2
    Test_CRELAY2Read();
    Test_PERLYCtrl();               //PE_RLY
    Test_PERLYRead(); 
    BSPWatchDogRefresh();              //喂狗: 继电器测试后
    
    Test_NTC_TEMP();                //NTC温度
    //RW
    Test_VCC_5V();                  //VCC5V
    Test_HALL_5V();                 //霍尔供电5V电压
    Test_ShuntTemp();               //分流器温度
    Test_IMDRLYCtrl();              //IMD_RLY
    Test_IMDRLYRead();
    Test_HV5V();                   //高压5V
    Test_HV_5VAD();                //高压-5V
    BSPWatchDogRefresh();              //喂狗: ADC/电压测试后

    Test_RVersion();                //上传软硬件版本号
    //RW
    Test_ParaFlag();                //参数FLAG标志
    Test_BarCode();                 //写入读取的序列号
    BSPWatchDogRefresh();              //喂狗: 条码读写后，校准任务前
    //校准任务
    Calibration_Process();              // 霍尔电流校准 PE电压校准 电池电压校准 KA84917电流校准 RN8209电压校准 RN8209电流校准
    
    BSPWatchDogRefresh();              //喂狗: 校准任务后
}


#if 1
/*=================================================================================================
** @Name      : void Test_RVersion(void) 
** @Input     : void
** @Output    : void
** @Function  : 上传软硬件版本号到Modbus寄存器 包括版本标志和版本信息(只读)
** @The notes : 读取成功置为0x5A,失败为0xFF
**===============================================================================================*/
#if 1
void Test_RVersion(void) 
{
    //版本Flag读取
     u8 version_flag = ModbusReg[REG_VERSION_FLAG][1];
    // 1. 如果已经成功（0x5A）直接返回,寄存器中有 有效数据; 失败（0xFF），返回空数据
    if (version_flag == 0x5A)
    {
        return;
    }
    else if(version_flag == 0xFF)
    {
        ModbusReg[REG_HARDWARE_V][1] = 0;
        ModbusReg[REG_HARDWARE_V][0] = 0;
        ModbusReg[REG_BOOT_V][1] = 0;
        ModbusReg[REG_BOOT_V][0] = 0;
        ModbusReg[REG_APP_V][1] = 0;
        ModbusReg[REG_APP_V][0] = 0;
    }

    //2.初始化时flag = 0，从eeprom中读取数据，如果eeprom中无数据从flash内读取版本号信息到寄存器，成功后置为0x5A,若失败则为0xff
    else if (version_flag == 0)
    {
        u8 read_success = 0;
        u8 valueEEP = 0;
        u8 valueFlash = 0;
    
        // 1. 先尝试从EEPROM读取
    
        // 读取完整数据到stParameter
        valueEEP = I2C_EE_BufferRead((u8*)&stParameter, Eeprom_ParaAddress_Start, sizeof(stParameter));
        if (valueEEP == TRUE)
        {
            read_success = 1;
        }
        // 2. 如果EEPROM没有有效数据，再尝试Flash
        else
        {
                // 读取完整数据到stParameter
                valueFlash =  SafeFlash_Read_Parameter((u8*)&stParameter);
                if (valueFlash == GD25Qx_OK)
                {
                    read_success = 1;
                }
        }
        
        // 3. 如果任一存储设备读取成功，更新寄存器
        if (read_success == 1)
        {
            // 更新Modbus寄存器
            ModbusReg[REG_HARDWARE_V][0] = (stParameter.u16HardwareVersion >> 8) & 0xFF;
            ModbusReg[REG_HARDWARE_V][1] = stParameter.u16HardwareVersion & 0xFF;
            ModbusReg[REG_BOOT_V][0]     = (stParameter.u16BootVersion >> 8) & 0xFF;
            ModbusReg[REG_BOOT_V][1]     = stParameter.u16BootVersion & 0xFF;
            ModbusReg[REG_APP_V][0]      = (stParameter.u16AppVersion >> 8) & 0xFF;
            ModbusReg[REG_APP_V][1]      = stParameter.u16AppVersion & 0xFF;
            ModbusReg[REG_VERSION_FLAG][1] = 0x5A;
        }
        else
        {
            // 两个存储设备都没有读取成功
            ModbusReg[REG_VERSION_FLAG][1] = 0xFF;
        }
    }

    //3. 如果flag = 1，从寄存器写入外部存储然后再读取
    else if (version_flag == 1)
    {
        u8 EEPWriteflag = 0;
        u8 FlashWriteflag = 0;

        // 从Modbus寄存器读取版本数据
        stParameter.u16HardwareVersion = (ModbusReg[REG_HARDWARE_V][0] << 8) | ModbusReg[REG_HARDWARE_V][1];
        stParameter.u16BootVersion = (ModbusReg[REG_BOOT_V][0] << 8) | ModbusReg[REG_BOOT_V][1];
        stParameter.u16AppVersion = (ModbusReg[REG_APP_V][0] << 8) | ModbusReg[REG_APP_V][1];

        if (TRUE == I2C_EE_BufferWrite((u8*)&stParameter,Eeprom_ParaAddress_Start,sizeof(stParameter)))
        {
           static Parameter_TypeDef verifyEEP;
            I2C_EE_BufferRead((u8 *)&verifyEEP,Eeprom_ParaAddress_Start,sizeof(verifyEEP));//读取eeprom中数据
            if (memcmp(&stParameter, &verifyEEP, sizeof(stParameter)) == 0)
            {
                EEPWriteflag = 1;
            }
            else
            {
                EEPWriteflag = 0;
            }
        }
    
            // 写入Flash
            if( SafeFlash_Erase_Parameter() == GD25Qx_OK)
            {
                if (SafeFlash_Write_Parameter((u8*)&stParameter) == GD25Qx_OK)
                {
                    // 写入成功，立即读取验证
                  static  Parameter_TypeDef verifyData;
                    if (SafeFlash_Read_Parameter((u8*)&verifyData) == GD25Qx_OK)
                    {
                        // 验证写入的数据
                        if (memcmp(&stParameter, &verifyData, sizeof(stParameter)) == 0)
                        {
                           FlashWriteflag = 1;  
                        }
                        else
                        {
                            FlashWriteflag = 0;
                        }
                    }
                }
            }
            if ((FlashWriteflag == 1) || (EEPWriteflag == 1))
            {
               // 验证成功，更新Modbus寄存器
               I2C_EE_BufferRead((u8 *)&stParameter,Eeprom_ParaAddress_Start,sizeof(stParameter));//读取eeprom中数据
               ModbusReg[REG_HARDWARE_V][0] = (stParameter.u16HardwareVersion >> 8) & 0xFF;
               ModbusReg[REG_HARDWARE_V][1] = stParameter.u16HardwareVersion & 0xFF;
               ModbusReg[REG_BOOT_V][0]     = (stParameter.u16BootVersion>> 8) & 0xFF;
               ModbusReg[REG_BOOT_V][1]     = stParameter.u16BootVersion & 0xFF;
               ModbusReg[REG_APP_V][0]      = (stParameter.u16AppVersion>> 8) & 0xFF;
               ModbusReg[REG_APP_V][1]      = stParameter.u16AppVersion & 0xFF;
               ModbusReg[REG_VERSION_FLAG][1] = 0x5A;  // 设置成功标志
            }
            else
            {
                ModbusReg[REG_VERSION_FLAG][1] = 0xFF;  // 
            }
            
    }
    else
    {
        ModbusReg[REG_VERSION_FLAG][1] = 0xFF;  // 
    }
}
# endif
/*=================================================================================================
** @Name      : void Test_ParaFlag(void) 
** @Input     : void
** @Output    : void
** @Function  : 上传参数FLAG标志到Modbus寄存器 (只读)
** @The notes : 0x5a：表示能从外部存储读出参数     0：表示没有参数
**===============================================================================================*/
void Test_ParaFlag(void) 
{
    ModbusReg[REG_PARA_FLAG][1] = stParameter.WriteFlag;
}

/*=================================================================================================
** @Name      : void Test_BarCode(void) 
** @Input     : void
** @Output    : void
** @Function  : 条码信息写入/读取
** @The notes : 
**===============================================================================================*/
#if 1
void Test_BarCode(void)
{
    u8 control_cmd = ModbusReg[REG_BAR_WR][1];
    //1. 无动作
    if (control_cmd == 0)
    {
        return;
    }

     // 2. 只读后清零
    else if (control_cmd == 1)
    {
        
        u8 rAdd = REG_BAR;
        u8 read_success = 0;
        
        // 先尝试从EEPROM读取
        if (TRUE == I2C_EE_BufferRead((u8*)&SystemSerialNum_2, Eeprom_SerialNum_Add, sizeof(SystemSerialNum_2)))
        {
            read_success = 1;
        }
        // EEPROM读取失败，尝试从Flash读取
        else if (GD25Qx_OK == SafeFlash_Read_Parameter((u8*)&stParameter))
        {
            // 从stParameter复制序列号到SystemSerialNum_2
        u16 copy_len = (SerialNum_len < sizeof(stParameter.SerialNumber)) ? 
            SerialNum_len : sizeof(stParameter.SerialNumber);
        memcpy(SystemSerialNum_2, stParameter.SerialNumber, copy_len);
            read_success = 1;
        }
        
        // 如果读取成功，填充Modbus寄存器
        if (read_success)
        {
            u8 rflag = 0;
            // 计算需要填充的寄存器数量
            u8 reg_count = (SerialNum_len + 1) / 2;  // 23 -> 12
            
            for (u8 i = 0; i < reg_count; i++)
            {
                for (u8 j = 0; j < 2; j++)
                {
                    if (rflag < SerialNum_len)
                    ModbusReg[rAdd + i][j] = SystemSerialNum_2[rflag++];
                }
            }
            // 清零命令寄存器
            ModbusReg[REG_BAR_WR][1] = 0;
            
        }
        else
        {
            // 读取失败，清空寄存器
            u8 reg_count = (SerialNum_len + 1) / 2;
            for (u8 i = 0; i < reg_count; i++)
            {
                ModbusReg[rAdd + i][0] = 0;
                ModbusReg[rAdd + i][1] = 0;
            }
        }
    }


    //3. 写入后读取，清零
    else if (control_cmd == 2)
    {
        u8 wflag = 0;
        u8 rflag = 0;
        u8 rAdd = REG_BAR;
        u8 wAdd = REG_BAR;
        //
        for ( u8 i = 0; i < ((SerialNum_len + 1) /2); i++)
          {
            for ( u8 j = 0; j < 2; j++)
            {
                if (wflag < SerialNum_len)
                SystemSerialNum_2[wflag++] = ModbusReg[wAdd+i][j];
            }
          }

        // 如果数据不足，填充0
            while (wflag < SerialNum_len)
            {
                SystemSerialNum_2[wflag++] = 0;
            }
          
        //写入后读取
        if (TRUE == RcvDeviceSerialParaCfgAPI())
        {
                for ( u8 i = 0; i < ((SerialNum_len + 1) /2) ; i++)
                {
                    for ( u8 j = 0; j < 2; j++)
                    {
                        if (rflag < SerialNum_len)
                        ModbusReg[rAdd+i][j] = SystemSerialNum_2[rflag++];
                    }
                }
                ModbusReg[REG_BAR_WR][1] = 0;
        }
    }

    else
    {
        return;
    }
}


#endif


/*=================================================================================================
** @Name      : void Test_SWAddr(void) 
** @Input     : void
** @Output    : void
** @Function  : 上传拨码开关
** @The notes : (只读 最低4位有效)
**===============================================================================================*/
void Test_SWAddr(void)
{
    u8 SW = SWReadData();
    ModbusReg[REG_SW][1] = SW & 0x0F;
} 

/*=================================================================================================
** @Name      : void Test_DI1(void) 
** @Input     : void
** @Output    : void
** @Function  : DI1状态
** @The notes : (只读 最低1位)
**===============================================================================================*/
void Test_DI1(void)
{
    ModbusReg[REG_DI1][1] = (DI1_Pin_Read()) & 0x01;
}

/*=================================================================================================
** @Name      : void Test_HallPFault(void) 
** @Input     : void
** @Output    : void
** @Function  : HALL_P_FAULT
** @The notes : (只读 最低1位)
**===============================================================================================*/
void Test_HallPFault(void)
{
    ModbusReg[REG_HALL_P_FAULT][1] = (HALL_PFAULT_Pin_Read()) & 0x01;
}

/*=================================================================================================
** @Name      : void Test_Diagnosis(void) 
** @Input     : void
** @Output    : void
** @Function  : Diagnosis
** @The notes : (只读 最低1位)
**===============================================================================================*/
void Test_Diagnosis(void)
{
    ModbusReg[REG_DIAGNOSIS][1] = (DIAGNOSIS_Pin_Read()) & 0x01;
}

/*=================================================================================================
** @Name      : void Test_IMDALM(void) 
** @Input     : void
** @Output    : void
** @Function  : IMD_ALM灯
** @The notes : (最低1位)
**===============================================================================================*/
void Test_IMDALM(void) 
{
    ModbusReg[REG_IMD_ALM_LED][1] = (IMD_ALM_Pin_Read()) & 0x01;
}

/*=================================================================================================
** @Name      : void Test_LEDALM(void) 
** @Input     : void
** @Output    : void
** @Function  : RUN_LED灯
** @The notes : (GPIO口状态)
**===============================================================================================*/
void Test_LEDALM(void) 
{
    ModbusReg[REG_RUN_LED][1] = (LED_RUN_Pin_Read())  & 0x01;
}

/*=================================================================================================
** @Name      : void Test_CRELAY1Ctrl(void) 
** @Input     : void
** @Output    : void
** @Function  : CRELAY1
** @The notes : 继电器1控制
**===============================================================================================*/
void Test_CRELAY1Ctrl(void) 
{
        if (ModbusReg[REG_C_RELAY_1][1] == 0)
        {
            CRELAY1_Contr_Open();
        }
        else if(ModbusReg[REG_C_RELAY_1][1] == 1)
        {
            CRELAY1_Contr_Close();
        }
}

/*=================================================================================================
** @Name      : void Test_CRELAY1Read(void) 
** @Input     : void
** @Output    : void
** @Function  : CRELAY1
** @The notes : 继电器1状态读取
**===============================================================================================*/
void Test_CRELAY1Read(void) 
{ 
   ModbusReg[REG_C_RELAY_1][1] = CRELAY1_Pin_Read();
}


/*=================================================================================================
** @Name      : void Test_CRELAY2Ctrl(void) 
** @Input     : void
** @Output    : void
** @Function  : CRELAY2
** @The notes : 继电器2
**===============================================================================================*/
void Test_CRELAY2Ctrl(void) 
{
        if (ModbusReg[REG_C_RELAY_2][1] == 0)
        {
            CRELAY2_Contr_Open();
            
        }
        else if(ModbusReg[REG_C_RELAY_2][1] == 1)
        {
            CRELAY2_Contr_Close();
        }
}

/*=================================================================================================
** @Name      : void Test_CRELAY2Read(void) 
** @Input     : void
** @Output    : void
** @Function  : CRELAY2
** @The notes : 继电器2
**===============================================================================================*/
void Test_CRELAY2Read(void) 
{
    ModbusReg[REG_C_RELAY_2][1] = CRELAY2_Pin_Read();
}

/*=================================================================================================
** @Name      : void Test_PERLYCtrl(void) 
** @Input     : void
** @Output    : void
** @Function  : PERLY
** @The notes : 
**===============================================================================================*/
void Test_PERLYCtrl(void) 
{
        if (ModbusReg[REG_PE_RELAY][1] == 0)
        {
            PE_RLY_Contr_Open();
        }
        else if (ModbusReg[REG_PE_RELAY][1] == 1)
        {
            PE_RLY_Contr_Close();
        }
}


/*=================================================================================================
** @Name      : void Test_PERLYRead(void) 
** @Input     : void
** @Output    : void
** @Function  : PERLY
** @The notes : 
**===============================================================================================*/
void Test_PERLYRead(void) 
{
    ModbusReg[REG_PE_RELAY][1] = PE_RLY_Pin_Read();
}

/*=================================================================================================
** @Name      : void Test_IMDRLYCtrl(void) 
** @Input     : void
** @Output    : void
** @Function  : IMDRLY
** @The notes : 0：断开继电器  1：闭合继电器
**===============================================================================================*/
void Test_IMDRLYCtrl(void) 
{
    if (ModbusReg[REG_IMD_RELAY][1] == 0)
    {
        IMD_RLY_Open();
        
    }
    else if (ModbusReg[REG_IMD_RELAY][1] == 1)
    {
        IMD_RLY_Close();
        
    }
}

/*=================================================================================================
** @Name      : void Test_IMDRLYRead(void) 
** @Input     : void
** @Output    : void
** @Function  : IMDRLY
** @The notes : 读取继电器状态
**===============================================================================================*/
void Test_IMDRLYRead(void)
{
    ModbusReg[REG_IMD_RELAY][1] = GetIMD_RLY_State();
}

/*=================================================================================================
** @Name      : void Test_Eep_Flash_WriteRead(void) 
** @Input     : void
** @Output    : void
** @Function  : 测试EEPROM和Flash存储器的写入和读取功能
** @The notes : 无EEP2驱动，当前读写状态置为1
**===============================================================================================*/
void Test_Eep_Flash_WriteRead(void)
{
    u8 eeprom1_ok = 0;
    u8 eeprom2_ok = 0;
    u8 flash_ok = 0;

    //检测EEPROM存储器的写入和读取功能
    u8 wdata[4] = {0x04,0x03,0x02,0x01};
    u8 rdata[4] = {0};
    //数据写入后读取

    if ((ModbusReg[REG_E2PROM_FLASH_TEST][1] & 0x02) != 0x02)
    {
        if(I2C_EE_PageWrite(wdata, Eeprom_TestAddress_Start, sizeof(wdata)) == TRUE)
        {
            I2C_EE_BufferRead(rdata,Eeprom_TestAddress_Start, sizeof(rdata));

            if (memcmp(wdata,rdata, sizeof(wdata)) == 0)
            {
                eeprom1_ok = 1;
            }
            else
            {
                eeprom1_ok = 0;
            }
        } else
        {
            eeprom1_ok = 0;
        }
    }
    else
    {
        eeprom1_ok = 1;
    }

    //检测EEP2 
    eeprom2_ok = 1;
    // u8 mdata[4] = {0x03,0x02,0x01,0x00};
    // u8 ndata[4] = {0};
    // //避免重复测试读写操作eeprom
    // if ((ModbusReg[REG_E2PROM_FLASH_TEST][1] & 0x04) != 0x04)
    // {
    //     if (I2C_EE_PageWrite(mdata,0x00,sizeof(mdata)) == true)
    //     {
    //         I2C_EE_BufferRead(ndata,0x00,sizeof(ndata));
    //     }

    //     if (memcmp(mdata, ndata, sizeof(mdata))== 0)
    //     {
    //         eeprom2_ok = 1; 
    //     }
    //     else{
    //         eeprom2_ok = 0;
    //     }
    // }
    // else
    // {
    //     eeprom2_ok = 1;
    // }
    
    //检测Flash存储器的写入和读取功能
    if ((ModbusReg[REG_E2PROM_FLASH_TEST][1] & 0x08) != 0x08)
    {
        u8 Test_write_data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}; 
        u8 Test_read_data[8] = {0};
        u32 size = sizeof(Test_write_data);
        if (SafeTestFlash_Erase_Parameter() == GD25Qx_OK)
        {
            if (SafeTestFlash_Write_Parameter(Test_write_data,size) == GD25Qx_OK)
            {
                if (SafeTestFlash_Read_Parameter(Test_read_data,size) == GD25Qx_OK)
                {
                    //Flash数据读写正确
                    if (memcmp(Test_read_data,Test_write_data,sizeof(Test_write_data)) == 0)
                    {
                        flash_ok = 1;
                        //测试完成后擦除数据
                        SafeTestFlash_Erase_Parameter();
                    }
                    else{
                        flash_ok = 0;}
                }
                else{
                    flash_ok = 0;}
            }
            else{
                flash_ok = 0;}
        }
        else{
            flash_ok = 0;}
    }
    else{
        flash_ok = 1;}

     // 设置状态位
    u8 status_byte = ModbusReg[REG_E2PROM_FLASH_TEST][1];
    
     // 先清除相关位（bit0-bit3）
    status_byte &= ~0x0F;
    
     // 根据检测结果设置各个状态位
     
    if (eeprom1_ok) 
    {
        status_byte |= (1 << 1);  // bit1 = 1
    }
    if (eeprom2_ok) 
    {
        status_byte |= (1 << 2);  // bit2 = 1
    }
    if (flash_ok) 
    {
        status_byte |= (1 << 3);  // bit3 = 1
    }
    
    if (eeprom1_ok && eeprom2_ok && flash_ok)
    {
        // 检测完成且均成功标志,将bit0 置为1
        status_byte |= (1 << 0);      // bit0 = 1
    }
    ModbusReg[REG_E2PROM_FLASH_TEST][1] = status_byte;
}

/*===========================================================================================================================
通信状态测试相关
**===================================================================================================================================*/

/*=================================================================================================
** @Name      : void Test_KA84917Commu(void)
** @Input     : void
** @Output    : void
** @Function  : KA84917和KA84922是否正常
** @The notes : 
**===============================================================================================*/
void Test_KA84917Com(void)
{
    static u8 InitFlag917 = 0;
    static u8 InitFlag922 = 0;
    
    if(wakeUpState == 1)
    {
        InitFlag917 = gWakeUpStateInitFlag917;
        InitFlag922 = gWakeUpStateInitFlag922;
    }
    else
    {
        InitFlag917 = 0;
        InitFlag922 = 0;
    }
  

    ModbusReg[REG_KA84917_COMM_STATUS][1] = InitFlag917;
    ModbusReg[REG_KA84922_COMM_STATUS][1] = InitFlag922;
}

/*=================================================================================================
** @Name      : void Test_RN8209Com(void)
** @Input     : void
** @Output    : void
** @Function  : RN8209通信是否正常
** @The notes : 
**===============================================================================================*/
void Test_RN8209Com(void)
{
    static u8 InitFlag8209 = 0;

    if (Rn8209InitSuccessFlag == 1)
    {
      InitFlag8209 = 1;
    }
    else
    {
     InitFlag8209 = 0;
    }
    ModbusReg[REG_RN8209_COMM_STATUS][1] = InitFlag8209;
}

/*=================================================================================================
** @Name      :  u8 GetTargetAddr(u8 local_addr) 
** @Input     : local_addr 本设备拨码地址
** @Output    : target_addr 目标设备地址
** @Function  : 根据本机地址判断目标地址
** @The notes :
**===============================================================================================*/
 u8 GetTargetAddr(u8 local_addr)
{
    u8 target_addr;
    switch (local_addr)
    {
    case 0x00:
        target_addr = 0x0f;
        break;
    case 0x0f:
        target_addr = 0x00;
        break;
    default:
        target_addr = 0xFF;
        break;
    }
    return target_addr;
}
/*=================================================================================================
** @Name      : void Test_CANFDCom(void) 
** @Input     : void
** @Output    : void
** @Function  : CANFD通信状态测试，向测试板发送数据，并等待回复数据，判断是否一致
** @The notes :
**===============================================================================================*/
void Test_CANFDCom(void) 
{
    BSPCANInit(eCAN1);

    /* 工装校准门控：REG_BOARD_ADDR和REG_TARGET_ADDR必须同时为1才允许CANFD通信测试 */
    if ((ModbusReg[REG_BOARD_ADDR][1] != 1) || (ModbusReg[REG_TARGET_ADDR][1] != 1))
    {
        ModbusReg[REG_CANFD_COMM_STATUS][1] = 0x00;
        return;
    }

    u8 send_status = 0;    // 发送状态
    u8 recv_status = 0;    // 接收状态
    u8 canfd_status = 0;

    u8 local_addr, target_addr;
    local_addr = (SystemInfo_1[DeviceAddr] & 0xff) - 20;
    target_addr = GetTargetAddr(local_addr);

    //判断地址是否有效
    if (((local_addr == target_addr) && (local_addr == 0))
        || (target_addr == 0xFF))
    {
        ModbusReg[REG_CANFD_COMM_STATUS][1] = 0x00;
        return;
    }


    t_CANMSG tmsg;
    tmsg.ID = 0x0A0B0000 | (u32)(local_addr << 8) | (u32)(target_addr);
    tmsg.Len = 64;
    u8 Test_FDCanData[64] ={0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
                            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
                            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
    
    memcpy(tmsg.Data,Test_FDCanData,sizeof(Test_FDCanData));

    //清空接收缓存区数据
    gCanfdRecivedFlag = 0;
    memset(FDCANRData, 0, sizeof(FDCANRData));

    osDelay(100);

    //发送数据
    if (BSPCANSendQin(eCAN1,&tmsg) == TRUE)
    {
        send_status = 1;
    }
    else
    {
        send_status = 0;
    }

    //发送后等待回复
    u32 timeout_ms = 100;
   u32 start_time = HAL_GetTick();
    while (gCanfdRecivedFlag == 0)
    {
       if (HAL_GetTick() - start_time >= timeout_ms)
       {
            break;
       }
       osDelay(1);
   }

    //2.测试接收数据状态
    //接收数据标志为1，如果接收的数据和发送数据相同，则接收数据正常
    if ((gCanfdRecivedFlag == 1) && (memcmp(Test_FDCanData, FDCANRData, sizeof(Test_FDCanData)) == 0))
    {
        recv_status = 1;
    }
    else
    {
        recv_status = 0;
    }

    canfd_status = 0;
    if (send_status == 1) canfd_status |= 0x01;  // 设置bit0
    if (recv_status == 1) canfd_status |= 0x02;  // 设置bit1
    
    // 3. 更新Modbus寄存器
    ModbusReg[REG_CANFD_COMM_STATUS][1] = canfd_status;
}

/*=================================================================================================
** @Name      : void ToolingCheckCANReviceAPI(e_CANNUM CanNum, t_CANMSG* msg)
** @Input     : void
** @Output    : void
** @Function  : CANFD接收数据,接收测试板发送的数据
** @The notes :
**===============================================================================================*/
u8 ToolingCheckCANReviceAPI(t_CANMSG msg, u8 canNum)
{
    /* 工装校准门控：REG_BOARD_ADDR和REG_TARGET_ADDR必须同时为1才允许接收CANFD数据 */
    if ((ModbusReg[REG_BOARD_ADDR][1] != 1) || (ModbusReg[REG_TARGET_ADDR][1] != 1))
    {
        return 0;
    }

    //本机地址由拨码开关决定
    u8 local_addr = (SystemInfo_1[DeviceAddr] & 0xff) - 20;

    // 检查协议前缀（高16位是否为0x0A0B）
    if((msg.ID & 0xFFFF0000) != 0x0A0B0000)
    {
        return 0; 
    }

    // 1. 解析ID中的地址信息
    // u8 src_addr = (msg.ID >> 8) & 0xFF;   // 源地址（发送方）
    u8 dst_addr = msg.ID & 0xFF;          // 目标地址（接收方）

    //目标地址等于本机地址
    if (dst_addr != local_addr)
    {
       return 0;
    }
    
    //写入数据
    if(canNum == eCAN1)
    {
        for(int i = 0; i < msg.Len; i++)
            {
                FDCANRData[canNum][i] = msg.Data[i]; //存储接收数据
            }       
            gCanfdRecivedFlag = 1;
        }
        else
        {
            gCanfdRecivedFlag = 0;
        }
        
    return gCanfdRecivedFlag;
}

/*=================================================================================================
** @Name      : void ToolingCheckCanCommunTest(void)
** @Input     : void
** @Output    : void
** @Function  : 拨码为1的测试板测试工装通信发送数据
** @The notes : 连续发送工装测试报文
**===============================================================================================*/
void ToolingCheckCanCommunTest(void)
{
    /* 工装校准门控：REG_BOARD_ADDR和REG_TARGET_ADDR必须同时为1才允许CANFD通信测试 */
    if ((ModbusReg[REG_BOARD_ADDR][1] != 1) || (ModbusReg[REG_TARGET_ADDR][1] != 1))
    {
        ModbusReg[REG_CANFD_COMM_STATUS][1] = 0x00;
        return;
    }

    u8 local_addr, target_addr;
    local_addr = (SystemInfo_1[DeviceAddr] & 0xff) - 20;
    target_addr = GetTargetAddr(local_addr);

    //非工装测试报文
    if ((target_addr != 0x00)|| (target_addr == 0xFF))
    {
        ModbusReg[REG_CANFD_COMM_STATUS][1] = 0x00;
        return;
    }
   
    //未拨码
    if ((local_addr == target_addr) && (local_addr == 0))
    {
        ModbusReg[REG_CANFD_COMM_STATUS][1] = 0x00;
        return;
    }


    t_CANMSG tmsg;
    tmsg.ID = 0x0A0B0000 | (u32)(local_addr << 8) | (u32)(target_addr);
    tmsg.Len = 64;
    u8 Test_FDCanData[64] ={0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
                            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
                            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                            0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
    
    memcpy(tmsg.Data,Test_FDCanData,sizeof(Test_FDCanData));

    //清空接收缓存区数据
    gCanfdRecivedFlag = 0;
    memset(FDCANRData, 0, sizeof(FDCANRData));

    osDelay(100);


    
    //发送数据
   BSPCANSendQin(eCAN1,&tmsg);

    
}

/*=================================================================================================
** @Name      : void Test_NTC_TEMP(void) 
** @Input     : void
** @Output    : void
** @Function  : NTC温度值
** @The notes : 数值为真实值的10倍数，单位0.1°C
**===============================================================================================*/
void Test_NTC_TEMP(void)
{
    ModbusReg[REG_NTC_TEMP_1][0] =  ((u16)((gSystemData.fNtcTemp[0])*10) >> 8) & 0xFF;
    ModbusReg[REG_NTC_TEMP_1][1] = ((u16)((gSystemData.fNtcTemp[0])*10) ) & 0xFF;
    ModbusReg[REG_NTC_TEMP_2][0] = ((u16)((gSystemData.fNtcTemp[1])*10) >> 8) & 0xFF;
    ModbusReg[REG_NTC_TEMP_2][1] = ((u16)((gSystemData.fNtcTemp[1])*10) ) & 0xFF;
    
    // ModbusReg[REG_NTC_TEMP_1][0] = (u8)(SystemData_51[NtcTemp1] >> 8) & 0xFF;
    // ModbusReg[REG_NTC_TEMP_1][1] = (u8)(SystemData_51[NtcTemp1]) & 0xFF;
    // ModbusReg[REG_NTC_TEMP_2][0] = (u8)(SystemData_51[NtcTemp2] >> 8) & 0xFF;
    // ModbusReg[REG_NTC_TEMP_2][1] = (u8)(SystemData_51[NtcTemp2]) & 0xFF;
}

/*=================================================================================================
** @Name      : void Test_VCC_5V(void) 
** @Input     : void
** @Output    : void
** @Function  : VCC_5V
** @The notes : 数值为真实值的10倍数，单位0.1°C
**===============================================================================================*/
void Test_VCC_5V(void)
{
    ModbusReg[REG_VCC_5V][0] = ((u16)((gSystemData.fVcc5Volt)*10) >> 8) & 0xFF;
    ModbusReg[REG_VCC_5V][1] = ((u16)((gSystemData.fVcc5Volt)*10) ) & 0xFF;
}

/*=================================================================================================
** @Name      : void Test_HALL_5V(void) 
** @Input     : void
** @Output    : void
** @Function  : HALL_5V
** @The notes : 数值为真实值的10倍数，单位0.1°C
**===============================================================================================*/
void Test_HALL_5V(void) 
{
    ModbusReg[REG_HALL_5V_SUPPLY][0] = ((u16)((gSystemData.fHall5Volt)*10) >> 8) & 0xFF;
    ModbusReg[REG_HALL_5V_SUPPLY][1] = ((u16)((gSystemData.fHall5Volt)*10)) & 0xFF;

    // ModbusReg[REG_HALL_5V_SUPPLY][0] = (u8)(SystemData_51[Hall5vSamp] >> 8) & 0xFF;
    // ModbusReg[REG_HALL_5V_SUPPLY][1] = (u8)(SystemData_51[Hall5vSamp]) & 0xFF;
}

/*=================================================================================================
** @Name      : void Test_ShuntTemp(void) 
** @Input     : void
** @Output    : void
** @Function  : 分流器温度TEMP1
** @The notes : 数值为真实值的10倍数，单位0.1°C
**===============================================================================================*/
void Test_ShuntTemp(void) 
{
    ModbusReg[REG_SHUNT_TEMP_1][0] = ((u16)((gSystemData.fBatShuntTemp)*10) >> 8) & 0xFF;
    ModbusReg[REG_SHUNT_TEMP_1][1] = ((u16)((gSystemData.fBatShuntTemp)*10)) & 0xFF;

    // ModbusReg[REG_SHUNT_TEMP_1][0] = (u8)(SystemData_51[ShuntTemp] >> 8) & 0xFF;
    // ModbusReg[REG_SHUNT_TEMP_1][1] = (u8)(SystemData_51[ShuntTemp]) & 0xFF;
}



/*=================================================================================================
** @Name      : void Test_HV5V(void) 
** @Input     : void
** @Output    : void
** @Function  : HV_5V
** @The notes : 数值为真实值的10倍数，单位0.1V
**===============================================================================================*/
void Test_HV5V(void) 
{
    ModbusReg[REG_HV_5V][0] = ((u16)((gSystemData.fHvP5Volt)*10) >> 8) & 0xFF;
    ModbusReg[REG_HV_5V][1] = ((u16)((gSystemData.fHvP5Volt)*10)) & 0xFF;
    
    // ModbusReg[REG_HV_5V][0] = (u8)(SystemData_51[Ka84917P5V] >> 8) & 0xFF;
    // ModbusReg[REG_HV_5V][1] = (u8)(SystemData_51[Ka84917P5V]) & 0xFF;
}
/*=================================================================================================
** @Name      : void Test_HV5VAD(void) 
** @Input     : void
** @Output    : void
** @Function  : HV_-5V_AD
** @The notes : 数值为真实绝对值的10倍数，单位0.1V
**===============================================================================================*/
void Test_HV_5VAD(void) 
{
    ModbusReg[REG_HV_MINUS_5V_AD][0] = ((u16)((gSystemData.fHvN5Volt)*10) >> 8) & 0xFF;
    ModbusReg[REG_HV_MINUS_5V_AD][1] = ((u16)((gSystemData.fHvN5Volt)*10)) & 0xFF;

    // ModbusReg[REG_HV_MINUS_5V_AD][0] = (u8)(SystemData_51[Ka84917N5V] >> 8) & 0xFF;
    // ModbusReg[REG_HV_MINUS_5V_AD][1] = (u8)(SystemData_51[Ka84917N5V]) & 0xFF;
}
#endif

/*===========================================================================================================================
    eep操作相关
**===================================================================================================================================*/
/*=================================================================================================
** @Name      : void ToolingCheckInitEEP(void)
** @Input     : void
** @Output    : void
** @Function  : 工装测试eep参数初始化
** @The notes : 如果EEP中有参数则读取存储的，没有则使用默认值赋值
**===============================================================================================*/
void ToolingCheckInitEEP(void)
{
    ParameterNow_102[ParameterCigFlag] = 23131;
    ParameterNow_102[CanComRate] = 3;
    ParameterNow_102[RS485ComRate] = 0;
    ParameterNow_102[CanCrolVersion] = 1;
    ParameterNow_102[CurrentSampChannel] = 2;
    ParameterNow_102[ShuntVoltSampRange] = 75;//单位1mv
    ParameterNow_102[ShuntCurrentSampRange] = 40;//单位10A
    ParameterNow_102[HallSignalZeroVolt] = 25;//单位0.1V
    ParameterNow_102[HallVoltSampRange] = 20;//单位0.1V
    ParameterNow_102[HallCurrentSampRange] = 40;//单位10A
    ParameterNow_102[CurrentFilterMethod] = 0;
    ParameterNow_102[CurrentFilterTimes] = 0;
    ParameterNow_102[AfterInsuPeRlyState] = 1;
    ParameterNow_102[InsuSampPeriod] = 21600;
    ParameterNow_102[InsuTimeLimit] = 5;
    ParameterNow_102[RlySwiftInterTime] = 500;//单位MS

    static Parameter_TypeDef gSystemParaBuff;
    u8* rbuff = (u8 *) &gSystemParaBuff;
    u8* parabuff = (u8 *) &stParameter;
    I2C_EE_BufferRead(rbuff,Eeprom_ParaAddress_Start,sizeof(stParameter));//读取eeprom中数据
    BSPWatchDogRefresh();              //喂狗: EEPROM读取后
    if(gSystemParaBuff.WriteFlag == 0x5a)//eeprom中有数据，读取
    {
        //读取eeprom中配置系统信息
        I2C_EE_BufferRead(parabuff,Eeprom_ParaAddress_Start,sizeof(stParameter));//读取eeprom中数据
        
        ParameterNow_102[ParameterCigFlag] = stParameter.u16ArraySet[ParameterCigFlag];
        ParameterNow_102[CanComRate] = stParameter.u16ArraySet[CanComRate];
        ParameterNow_102[RS485ComRate] = stParameter.u16ArraySet[RS485ComRate];
        ParameterNow_102[CanCrolVersion] = stParameter.u16ArraySet[CanCrolVersion];
        ParameterNow_102[CurrentSampChannel] = stParameter.u16ArraySet[CurrentSampChannel];
        ParameterNow_102[ShuntVoltSampRange] = stParameter.u16ArraySet[ShuntVoltSampRange];//单位1mv
        ParameterNow_102[ShuntCurrentSampRange] = stParameter.u16ArraySet[ShuntCurrentSampRange];//单位10A
        ParameterNow_102[HallSignalZeroVolt] = stParameter.u16ArraySet[HallSignalZeroVolt];//单位0.1V
        ParameterNow_102[HallVoltSampRange] = stParameter.u16ArraySet[HallVoltSampRange];//单位0.1V
        ParameterNow_102[HallCurrentSampRange] = stParameter.u16ArraySet[HallCurrentSampRange];//单位10A
        ParameterNow_102[CurrentFilterMethod] = stParameter.u16ArraySet[CurrentFilterMethod];
        ParameterNow_102[CurrentFilterTimes] = stParameter.u16ArraySet[CurrentFilterTimes];
        ParameterNow_102[AfterInsuPeRlyState] = stParameter.u16ArraySet[AfterInsuPeRlyState];
        ParameterNow_102[InsuSampPeriod] = stParameter.u16ArraySet[InsuSampPeriod];
        ParameterNow_102[InsuTimeLimit] = stParameter.u16ArraySet[InsuTimeLimit];
        ParameterNow_102[RlySwiftInterTime] = stParameter.u16ArraySet[RlySwiftInterTime];//单位MS
        for(int i = 0;i<SerialNum_len;i++)
        {
            SystemSerialNum_2[i] = stParameter.SerialNumber[i];//初始化条码当前值
        }
    }else//eeprom无数据，尝试从外部flash读取备份
    {
        u8 flashid[2] = {0,0};
        BSP_GD25Qx_Read_ID(flashid);
        //读flah配置系统信息
        BSP_GD25Qx_Read(rbuff,FlashMap_Sysmsy,sizeof(stParameter));
        BSPWatchDogRefresh();          //喂狗: Flash读取后
        if(gSystemParaBuff.WriteFlag == 0x5a)//外部flash中有数据，读取
        {
            BSP_GD25Qx_Read(parabuff,FlashMap_Sysmsy,sizeof(stParameter));
            ParameterNow_102[ParameterCigFlag] = stParameter.u16ArraySet[ParameterCigFlag];
            ParameterNow_102[CanComRate] = stParameter.u16ArraySet[CanComRate];
            ParameterNow_102[RS485ComRate] = stParameter.u16ArraySet[RS485ComRate];
            ParameterNow_102[CanCrolVersion] = stParameter.u16ArraySet[CanCrolVersion];
            ParameterNow_102[CurrentSampChannel] = stParameter.u16ArraySet[CurrentSampChannel];
            ParameterNow_102[ShuntVoltSampRange] = stParameter.u16ArraySet[ShuntVoltSampRange];//单位1mv
            ParameterNow_102[ShuntCurrentSampRange] = stParameter.u16ArraySet[ShuntCurrentSampRange];//单位10A
            ParameterNow_102[HallSignalZeroVolt] = stParameter.u16ArraySet[HallSignalZeroVolt];//单位0.1V
            ParameterNow_102[HallVoltSampRange] = stParameter.u16ArraySet[HallVoltSampRange];//单位0.1V
            ParameterNow_102[HallCurrentSampRange] = stParameter.u16ArraySet[HallCurrentSampRange];//单位10A
            ParameterNow_102[CurrentFilterMethod] = stParameter.u16ArraySet[CurrentFilterMethod];
            ParameterNow_102[CurrentFilterTimes] = stParameter.u16ArraySet[CurrentFilterTimes];
            ParameterNow_102[AfterInsuPeRlyState] = stParameter.u16ArraySet[AfterInsuPeRlyState];
            ParameterNow_102[InsuSampPeriod] = stParameter.u16ArraySet[InsuSampPeriod];
            ParameterNow_102[InsuTimeLimit] = stParameter.u16ArraySet[InsuTimeLimit];
            ParameterNow_102[RlySwiftInterTime] = stParameter.u16ArraySet[RlySwiftInterTime];//单位MS
            
            for(int i = 0;i<SerialNum_len;i++)
            {
                SystemSerialNum_2[i] = stParameter.SerialNumber[i];//初始化条码当前值
            }
        }
    }

        //首次上电eep和flash中都没有数据，工装需使用这些参数，赋值默认值
        stParameter.u16ArraySet[ParameterCigFlag] = 23131;
        stParameter.u16ArraySet[CanComRate] = 3;
        stParameter.u16ArraySet[RS485ComRate] = 0;
        stParameter.u16ArraySet[CanCrolVersion] = 1;
        stParameter.u16ArraySet[CurrentSampChannel] = 2;
        stParameter.u16ArraySet[ShuntVoltSampRange] = 75;//单位1mv
        stParameter.u16ArraySet[ShuntCurrentSampRange] = 40;//单位10A
        stParameter.u16ArraySet[HallSignalZeroVolt] = 25;//单位0.1V
        stParameter.u16ArraySet[HallVoltSampRange] = 20;//单位0.1V
        stParameter.u16ArraySet[HallCurrentSampRange] = 40;//单位10A
        stParameter.u16ArraySet[CurrentFilterMethod] = 0;
        stParameter.u16ArraySet[CurrentFilterTimes] = 0;
        stParameter.u16ArraySet[AfterInsuPeRlyState] = 1;
        stParameter.u16ArraySet[InsuSampPeriod] = 21600;//s
        stParameter.u16ArraySet[InsuTimeLimit] = 5;//工装代码未写入此参数 以及FREERTOS栈空间过小 can测试项更改拨码值
        stParameter.u16ArraySet[RlySwiftInterTime] = 500;//单位MS
    
    
        I2C_EE_BufferWrite(parabuff,Eeprom_ParaAddress_Start,sizeof(stParameter));
        
        
        BSPWatchDogRefresh();          //喂狗: Flash擦写前
        if(0 == BSP_GD25Qx_Erase_Block(FlashMap_Sysmsy))
        {
            if(0 == BSP_GD25Qx_Write(parabuff,FlashMap_Sysmsy,sizeof(stParameter)))//版本号存于flash
            {
                if(0 == BSP_GD25Qx_Read(parabuff,FlashMap_Sysmsy,sizeof(stParameter)))
                {
                        
                }
            }
        }
        BSPWatchDogRefresh();          //喂狗: Flash擦写后
}






/*===========================================================================================================================
    FLASH操作相关
**===================================================================================================================================*/
/*=================================================================================================
** @Name      : void ToolingCheckInit(void)
** @Input     : void
** @Output    : void
** @Function  : 工装测试模块互斥量初始化
** @The notes : 
**===============================================================================================*/
void ToolingCheckInit(void)
{
    xFlashMutex = xSemaphoreCreateMutex();
}
/*=================================================================================================
** @Name      : u8 SafeTestFlash_Read_Parameter(u8* Data, u32 size)
** @Input     : Data:读操作要写入的数组地址 size 数组大小
** @Output    : 读取结果
** @Function  : flash读操作添加互斥锁和临界区保护
** @The notes : 操作测试数据
**===============================================================================================*/
u8 SafeTestFlash_Read_Parameter(u8* Data, u32 size)
{
    if (xSemaphoreTake(xFlashMutex,pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return GD25Qx_ERROR;
    }
    BSPWatchDogRefresh();              //喂狗: SPI Flash读取前
    u8 result = BSP_GD25Qx_Read(Data,FlashMap_Test,size);
    xSemaphoreGive(xFlashMutex);
    return result;
}
/*=================================================================================================
** @Name      : u8 SafeTestFlash_Write_Parameter(u8* Data, u32 size)
** @Input     : Data:写操作要写入的数组地址 size 数组大小
** @Output    : 写入结果
** @Function  : flash写操作添加互斥锁和临界区保护
** @The notes : 操作测试数据
**===============================================================================================*/
u8 SafeTestFlash_Write_Parameter(u8* Data, u32 size)
{
    if (xSemaphoreTake(xFlashMutex,pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return GD25Qx_ERROR;
    }
    // taskENTER_CRITICAL();
    u8 result = BSP_GD25Qx_Write(Data,FlashMap_Test,size);
    // taskEXIT_CRITICAL();
    xSemaphoreGive(xFlashMutex);
    return result;
}
/*=================================================================================================
** @Name      : u8 SafeTestFlash_Erase_Parameter(void)
** @Input     : void
** @Output    : 擦除结果
** @Function  : flash擦除操作添加互斥锁和临界区保护
** @The notes : 操作测试数据
**===============================================================================================*/
u8 SafeTestFlash_Erase_Parameter(void)
{
    if (xSemaphoreTake(xFlashMutex,pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return GD25Qx_ERROR;
    }
    // taskENTER_CRITICAL();
    u8 result = BSP_GD25Qx_Erase_Sector(FlashMap_Test);
    // taskEXIT_CRITICAL();
    xSemaphoreGive(xFlashMutex);
    return result;
}
/*=================================================================================================
** @Name      : u8 SafeFlash_Read_Parameter(u8* Data)
** @Input     : Data:读操作要写入的数组地址
** @Output    : 读取结果
** @Function  : flash读操作添加互斥锁和临界区保护
** @The notes : 操作flash参数
**===============================================================================================*/
u8 SafeFlash_Read_Parameter(u8* Data)
{
    if (xSemaphoreTake(xFlashMutex,pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return GD25Qx_ERROR;
    }
    // taskENTER_CRITICAL();
    u8 result = BSP_GD25Qx_Read(Data,FlashMap_Sysmsy,sizeof(stParameter));
    // taskEXIT_CRITICAL();
    xSemaphoreGive(xFlashMutex);
    return result;
}
/*=================================================================================================
** @Name      : u8 SafeFlash_Write_Parameter(u8* Data)
** @Input     : Data:写操作要写入的数组地址
** @Output    : 写入结果
** @Function  : flash写操作添加互斥锁和临界区保护
** @The notes : 操作flash参数
**===============================================================================================*/
u8 SafeFlash_Write_Parameter(u8* Data)
{
    if (xSemaphoreTake(xFlashMutex,pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return GD25Qx_ERROR;
    }
    // taskENTER_CRITICAL();
    u8 result = BSP_GD25Qx_Write(Data,FlashMap_Sysmsy,sizeof(stParameter));
    // taskEXIT_CRITICAL();
    xSemaphoreGive(xFlashMutex);
    return result;
}
/*=================================================================================================
** @Name      : u8 SafeFlash_Erase_Parameter(void)
** @Input     : void
** @Output    : 擦除结果
** @Function  : flash擦除操作添加互斥锁和临界区保护
** @The notes : 操作flash参数
**===============================================================================================*/
u8 SafeFlash_Erase_Parameter(void)
{
    if (xSemaphoreTake(xFlashMutex,pdMS_TO_TICKS(100)) != pdTRUE)
    {
        return GD25Qx_ERROR;
    }
    // taskENTER_CRITICAL();
    u8 result = BSP_GD25Qx_Erase_Sector(FlashMap_Sysmsy);
    // taskEXIT_CRITICAL();
    xSemaphoreGive(xFlashMutex);
    return result;
}

