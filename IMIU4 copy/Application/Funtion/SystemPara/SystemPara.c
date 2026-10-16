/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Parameter.c
** @Founder      : WYC834
** @Date         : 2025.11.3
** @Function     : 自身参数模块
** @Instructions : 系统自身所有参数
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "SystemPara.h"
#include "SystemInfo.h"
#include "BSPGD25Q32E.h"
#include "SystemData.h"
#include "SystemPara.h"
#include "EEPROM.h"
#include "ToolingCheck.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
Parameter_TypeDef stParameter;
u16 ParameterNow_102[Parameter_Len];
u16 ParameterSet_107[Parameter_Len];
/*=======================================序列号信息===============================================*/
u8 SystemSerialNum_2[SerialNum_len] =
{
    0x0                             /*设备序列号信息(最高字符在低位)*/
};
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
void InitParameterArray(void)
{
    stParameter.u16ArraySet[ParameterCigFlag] = 23131;
    stParameter.u16ArraySet[CanComRate] = 3;
    stParameter.u16ArraySet[RS485ComRate] = 0;
    stParameter.u16ArraySet[CanCrolVersion] = 1;
    stParameter.u16ArraySet[CurrentSampChannel] = 2;
    stParameter.u16ArraySet[ShuntVoltSampRange] = 75;//单位1mv
    stParameter.u16ArraySet[ShuntCurrentSampRange] = 30;//单位10A
    stParameter.u16ArraySet[HallSignalZeroVolt] = 25;//单位0.1V
    stParameter.u16ArraySet[HallVoltSampRange] = 20;//单位0.1V
    stParameter.u16ArraySet[HallCurrentSampRange] = 30;//单位10A
    stParameter.u16ArraySet[CurrentFilterMethod] = 0;
    stParameter.u16ArraySet[CurrentFilterTimes] = 0;
    stParameter.u16ArraySet[AfterInsuPeRlyState] = 1;
    stParameter.u16ArraySet[InsuSampPeriod] = 21600;//s
    stParameter.u16ArraySet[InsuTimeLimit] = 10;//工装代码未写入此参数 以及FREERTOS栈空间过小 can测试项更改拨码值
    stParameter.u16ArraySet[RlySwiftInterTime] = 500;//单位MS

    stParameter.BatVoltKBFlag = 0;
    stParameter.fBatVoltK = 1.0f;
    stParameter.fBatVoltB = 0.0f;

    stParameter.PeVoltKBFlag = 0;
    stParameter.fPeVoltK = 1.0f;
    stParameter.fPeVoltB = 0.0f;

    stParameter.LinkVoltKBFlag = 0;
    stParameter.fLinkVoltK = 1.0f;
    stParameter.fLinkVoltB= 0.0f;

    stParameter.HallCurrKBFlag = 0;
    stParameter.fHallCurrentK = 1.0f;
    stParameter.fHallCurrentB= 0.0f;

    stParameter.BatShuntChgKBFlag = 0;
    stParameter.fBatShuntCurrentChgK = 1.0f;
    stParameter.fBatShuntCurrentChgB= 0.0f;


    stParameter.BatShuntDhgKBFlag = 0;
    stParameter.fBatShuntCurrentDhgK = 1.0f;
    stParameter.fBatShuntCurrentDhgB= 0.0f;
  
    stParameter.BatShuntDhgKBFlag = 0;
    stParameter.fBatShuntCurrentDhgK = 1.0f;
    stParameter.fBatShuntCurrentDhgB= 0.0f;
    
    stParameter.LinkShuntChgKBFlag = 0;
    stParameter.fLinkShuntCurrentChgK = 1.0f;
    stParameter.fLinkShuntCurrentChgB= 0.0f;
   
    stParameter.LinkShuntDhgKBFlag = 0;
    stParameter.fLinkShuntCurrentDhgK = 1.0f;
    stParameter.fLinkShuntCurrentDhgB= 0.0f;
  
    stParameter.HallZeroCorrFlag = 0;
    stParameter.fHallZeroCorr= 0.0f;

    //init ParameterNow_102
    ParameterNow_102[ParameterCigFlag] = 23131;
    ParameterNow_102[CanComRate] = 3;
    ParameterNow_102[RS485ComRate] = 0;
    ParameterNow_102[CanCrolVersion] = 1;
    ParameterNow_102[CurrentSampChannel] = 2;
    ParameterNow_102[ShuntVoltSampRange] = 75;//单位1mv
    ParameterNow_102[ShuntCurrentSampRange] = 30;//单位10A
    ParameterNow_102[HallSignalZeroVolt] = 25;//单位0.1V
    ParameterNow_102[HallVoltSampRange] = 20;//单位0.1V
    ParameterNow_102[HallCurrentSampRange] = 30;//单位10A
    ParameterNow_102[CurrentFilterMethod] = 0;
    ParameterNow_102[CurrentFilterTimes] = 0;
    ParameterNow_102[AfterInsuPeRlyState] = 1;
    ParameterNow_102[InsuSampPeriod] = 21600;
    ParameterNow_102[InsuTimeLimit] = 10;
    ParameterNow_102[RlySwiftInterTime] = 500;//单位MS



    static Parameter_TypeDef gSystemParaBuff;
    u8* rbuff = (u8 *) &gSystemParaBuff;
    u8* parabuff = (u8 *) &stParameter;
    I2C_EE_BufferRead(rbuff,Eeprom_ParaAddress_Start,sizeof(stParameter));//读取eeprom中数据
    if(gSystemParaBuff.WriteFlag == 0x5a)//eeprom中有数据，读取
    {
        //读取eeprom中配置系统信息
        I2C_EE_BufferRead(parabuff,Eeprom_ParaAddress_Start,sizeof(stParameter));//读取eeprom中数据
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
        if(gSystemParaBuff.WriteFlag == 0x5a)//外部flash中有数据，读取
        {
            BSP_GD25Qx_Read(parabuff,FlashMap_Sysmsy,sizeof(stParameter));
            
            for(int i = 0;i<SerialNum_len;i++)
            {
                SystemSerialNum_2[i] = stParameter.SerialNumber[i];//初始化条码当前值
            }
        }
    }
//数据同步
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

//软件版本检测
    if(((stParameter.u16AppVersion & 0xff) != SOFTWARE_VERSIONL) || (((stParameter.u16AppVersion >> 8) & 0xff) != SOFTWARE_VERSIONH))
    {
        u16 AppSoftVersion = (SOFTWARE_VERSIONH << 8)|SOFTWARE_VERSIONL;
        stParameter.u16AppVersion = AppSoftVersion;
        I2C_EE_BufferWrite(parabuff,Eeprom_ParaAddress_Start,sizeof(stParameter));//版本号存于eeprom
        
        
        if(0 == BSP_GD25Qx_Erase_Block(FlashMap_Sysmsy))
        {
            if(0 == BSP_GD25Qx_Write(parabuff,FlashMap_Sysmsy,sizeof(stParameter)))//版本号存于flash
            {
                if(0 == BSP_GD25Qx_Read(parabuff,FlashMap_Sysmsy,sizeof(stParameter)))
                {
                        
                }
            }
        }
    }

//硬件版本检测
    if (SystemInfo_1[HardwareVersion] != (stParameter.u16HardwareVersion & 0xff))
    {
        u16 HwVersion = SystemInfo_1[HardwareVersion];
        stParameter.u16HardwareVersion = HwVersion;

        I2C_EE_BufferWrite(parabuff,Eeprom_ParaAddress_Start,sizeof(stParameter));//版本号存于eeprom

        if (0 == BSP_GD25Qx_Erase_Block(FlashMap_Sysmsy))
        {
            if (0 == BSP_GD25Qx_Write(parabuff,FlashMap_Sysmsy,sizeof(stParameter)))//版本号存于flash
            {
                if(0 == BSP_GD25Qx_Read(parabuff,FlashMap_Sysmsy,sizeof(stParameter)))
                {
                        
                }
            }
            
        }
        
    }
//     //工装参数错误写入修正
// //工装代码未写入此参数 以及FREERTOS栈空间过小 can测试项更改拨码值
// stParameter.u16ArraySet[InsuSampPeriod] = 21600;
// stParameter.u16ArraySet[InsuTimeLimit] = 5;//工装代码未写入此参数 以及FREERTOS栈空间过小 can测试项更改拨码值
// stParameter.u16ArraySet[RlySwiftInterTime] = 500;//单位MS
 //ParameterNow_102[InsuSampPeriod] = 21600;
 //ParameterNow_102[InsuTimeLimit] = 20;
 //ParameterNow_102[RlySwiftInterTime] = 500;

   // stParameter.BatShuntChgKBFlag = 1;
    //stParameter.fBatShuntCurrentChgK = 0.99720707f;
    //stParameter.fBatShuntCurrentChgB= -0.1844978f;
    //stParameter.BatShuntDhgKBFlag = 0;
    //stParameter.fBatShuntCurrentDhgK = 0.99720707f;
    //stParameter.fBatShuntCurrentDhgB=  -0.1844978f;
    
  //   stParameter.HallCurrKBFlag = 0;
   //  stParameter.fHallCurrentK = 1.0f;
   //  stParameter.fHallCurrentB= 0.0f;

   //  stParameter.HallZeroCorrFlag = 0;
   // stParameter.fHallZeroCorr= 0.0f;
 
    //stParameter.LinkShuntChgKBFlag = 0;
    //stParameter.fLinkShuntCurrentChgK = 1.0f;
    //stParameter.fLinkShuntCurrentChgB= 0.0f;
   
  // stParameter.LinkShuntDhgKBFlag = 0;
   //stParameter.fLinkShuntCurrentDhgK = 1.0f;
   //stParameter.fLinkShuntCurrentDhgB= 0.0f;
//    stParameter.u16ArraySet[InsuTimeLimit] = 10;
//        stParameter.u16ArraySet[ShuntCurrentSampRange] = 30;//单位10A
//  I2C_EE_BufferWrite(parabuff,Eeprom_ParaAddress_Start,sizeof(stParameter));//读取eeprom中数据 
//        if(0 == BSP_GD25Qx_Erase_Block(FlashMap_Sysmsy))
//        {
//            if(0 == BSP_GD25Qx_Write(parabuff,FlashMap_Sysmsy,sizeof(stParameter)))//版本号存于flash
//            {
//                if(0 == BSP_GD25Qx_Read(parabuff,FlashMap_Sysmsy,sizeof(stParameter)))
//                {
//                        
//                }
//            }
//        }
}  
/* =================================================================================================
** @Name      : u8 RcvParaSetFlashSaveAPI()
** @Input     : 
** @Output    : 设置成功或失败
** @Function  : 参数设置外部flash保存处理函数
** @The notes : 
**===============================================================================================*/
u8 RcvParaSetFlashSaveAPI()
{
    for(int j = 0;j<Parameter_Len;j++)
    {
        stParameter.u16ArraySet[j] = ParameterNow_102[j];
    }
    u8* parabuff = (u8 *) &stParameter;
    if(0 == SafeFlash_Erase_Parameter())
    {
        if(0 == SafeFlash_Write_Parameter(parabuff))
        {
            if(0 == SafeFlash_Read_Parameter(parabuff))
            {
                return TRUE;
            }else
            {
                return FALSE;
            }
        }else
        {
            return FALSE;
        }
    }else 
    {
        return FALSE;
    }

}



/*=================================================================================================
** @Name      : u8 RcvDeviceSerialParaCfgAPI()
** @Input     : 
** @Output    : 设置成功或失败
** @Function  : 设备序列号参数设置命令处理函数
** @The notes : 
**===============================================================================================*/
u8 RcvDeviceSerialParaCfgAPI()
{
    u8 cfgResult = TRUE;
    for(int i = 0;i<SerialNum_len;i++)
    {
        stParameter.SerialNumber[i] = SystemSerialNum_2[i];//当前值复制到设置值
    }

    if(TRUE == I2C_EE_BufferWrite(SystemSerialNum_2,Eeprom_SerialNum_Add,sizeof(SystemSerialNum_2)))//序列号存于eeprom中
    {
        stParameter.WriteFlag = 0x5a;
        u8* buff = (u8 *) &stParameter;//外部flash备份存储

        if(0 == SafeFlash_Erase_Parameter())
        {
            if(0 == SafeFlash_Write_Parameter(buff))
            {
                if(0 == SafeFlash_Read_Parameter(buff))
                {
                    for(int i = 0;i<SerialNum_len;i++)
                    {
                        if(stParameter.SerialNumber[i] != SystemSerialNum_2[i])
                        {
                            cfgResult = FALSE;
                        }
                    }
                }else
                {
                    cfgResult = FALSE;   
                }
            }else
            {
                cfgResult = FALSE;
            }
        }else
        {
            cfgResult = FALSE;
        }
    }else
    {
        cfgResult = FALSE;
    }


    return(cfgResult);

}

/*=================================================================================================
** @Name      : u8 RcvSystemGenParaCfgAPI(u8 index, u8 port)
** @Input     : index:参数字段索引号 port:接收命令端口:[0,4]CAN口 [5,9]RS485口
** @Output    : 设置成功或失败
** @Function  : 系统通用参数设置命令处理函数
** @The notes : 通信口设置
**===============================================================================================*/
u8 RcvSystemGenParaCfgAPI(u8 index, u8 port)
{
    u16 para = 0;

    /*索引正确*/
    if(index < Parameter_Len)
    {
    	para = ParameterSet_107[index];

        /*写入成功*/
   	if(TRUE == ParaWriteStoreSystemGenParaInfo(index, para))
       {
   		/*显示区索引*/
           if(index < Parameter_Len)
           {
               stParameter.u16ArraySet[index] = para;/*同步更新RAM参数*/
           	   ParameterNow_102[index] = para;
           }
           else
           {
                return(FALSE);
           }

           return(TRUE);
       }
    }

    return(FALSE);
}

/*=================================================================================================
** @Name      : u8 ParaWriteStoreSystemGenParaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据 len:写入数据长度(个数)
** @Output    : 写入成功或失败
** @Function  : 向存储区写入系统设备通用参数信息
** @The notes : 写入内部EEPROM,请求备份到外部EEPROM和内部FLASH
**===============================================================================================*/
u8 ParaWriteStoreSystemGenParaInfo(u8 index, u16 data)
{
	u8 result  = 0;

    /*直接写入内部EEPROM*/
	result = WriteIntEepSystemGenParaInfo(index, data);
    return(result);
}

/*=================================================================================================
** @Name      : u8 WriteIntEepSystemGenParaInfo(u8 index, u16 data)
** @Input     : index:参数索引 data:需要写入的数据 
** @Output    : 写入成功或失败
** @Function  : 向内部EEPROM写入系统设备通用参数信息,单个参数写入
** @The notes :
**===============================================================================================*/
u8 WriteIntEepSystemGenParaInfo(u8 index, u16 data)
{
        u8 databuff[2];
        databuff[0] = (u8)data & 0xff;
        databuff[1] = (u8)(data >> 8) & 0xff;

    /*输入参数无效*/
    if((index > Parameter_Len))
    {
        return(FALSE);
    }

	/*检测参数有效性*/
    /*参数非有效值*/
    if(FALSE == ParaCheckSGenParaEffect(ParameterCigFlag + index, data))
    {
        return(FALSE);
    }
    
	/*向内部EEPROM写入参数值*/
	(void)I2C_EE_BufferWrite(databuff, Eeprom_Parameter_Add + (2 * index), 2);

    return(TRUE);
}

/*=================================================================================================
** @Name      : u8 ParaCheckSGenParaEffect(u32 addr, u32 para)
** @Input     : addr:参数地址 para:参数值
** @Output    : TRUE:有效合法 FALSE:非法无效
** @Function  : 检测系统通用参数值是否有效
** @The notes : U16
**===============================================================================================*/
u8 ParaCheckSGenParaEffect(u32 addr, u32 para)
{
    /*根据地址判断有效性*/
	switch(addr)
	{
		/*系统通用参数配置标志*/
		case ParameterCigFlag:
			/*不需检有效性*/
			return(TRUE);

		
		case CanComRate:
			if(para <= 3)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		case RS485ComRate:
			if(para <= 4)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		case CanCrolVersion:
			if(para <= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		case CurrentSampChannel:
			if(para <= 2)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

		case ShuntVoltSampRange:
			if(para <= 200 && para >= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;
		case ShuntCurrentSampRange:
			if(para <= 200 && para >= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

        case HallSignalZeroVolt:
			if(para <= 200 && para >= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

        case HallVoltSampRange:
			if(para <= 200 && para >= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

        case HallCurrentSampRange:
			if(para <= 200 && para >= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

        case CurrentFilterMethod:
			if(para <= 3)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

        case CurrentFilterTimes:
			if(para <= 200 && para >= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;

        case AfterInsuPeRlyState:
			if(para <= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;
        
        case InsuSampPeriod:
			if(para <= 64800 && para >= 1)
			{
				/*参数合法*/
				return(TRUE);
			}
			break;
            
        case InsuTimeLimit:
            if (para <= 20 && para >= 1)
            {
               /*参数合法*/
				return(TRUE);
            }
            break;

        case RlySwiftInterTime:
			if(para <= 10000 && para >= 10)
			{
				/*参数合法*/
				return(TRUE);
			}
		default:
			break;
	}

    return(FALSE);
}
/*=============================================================================================
** @Static function definition
**===========================================================================================*/
