
/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPUserHook.c
** @Founder      : WYC834
** @Date         : 2025.09.29
** @Function     : BSP用户服务函数模块
** @Instructions : 用户完善
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPUserHook.h"
#include "CANHandle.h"
#include "UsartHandle.h"
#include "ToolingCheck.h"
#include "LoadApp.h"
#include "can_uds_network_layer.h"


#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"
#include "can_uds_interface.h"
#include "SystemInfo.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void CANSendData_Hook(e_CANNUM CANNum, t_CANMSG msg)
** @Input     : CANNum:CAN通道 msg:报文信息
** @Output    : void
** @Function  : CAN发送任务嵌入函数
** @The notes :
**===========================================================================================*/
void CANSendData_Hook(e_CANNUM CANNum, t_CANMSG msg)
{
    (void)CANNum;
    (void)msg;
}

/*=============================================================================================
** @Name      : void CANRcvMsgHandle_Hook(e_CANNUM CANNum, t_CANMSG *pWriteMsg)
** @Input     : CANNum:CAN通道 pWriteMsg:CAN报文信息结构指针
** @Output    : void
** @Function  : CAN接收信息处理接口
** @The notes : 该函数已经在读CAN接收缓冲区调用      
**===========================================================================================*/
void CANRcvMsgHandle_Hook(e_CANNUM CANNum, t_CANMSG *pWriteMsg)
{       
    if(CANNum == eCAN1)
    {
        //1.
        CANLibRcvMsgHandle_API(*pWriteMsg,CANNum);

        //2. 升级报文ID:0b110514/15  
        if (pWriteMsg->ID == g_uds_config.rx_phy_id)
        {
            uds_recv_data_fristhandle(pWriteMsg);
        }

        //2.1 其他设备UDS升级报文检测(帧ID基址匹配但地址≠本机)
        {
            u32 idBase = pWriteMsg->ID & 0xFFFFFF00;
            u32 testerBase = REMOTE_TESTER_CAN_ID | (LOAD_DEV_ID << 8);
            u32 deviceBase = LOCAL_DEVICE_CAN_ID  | (LOAD_DEV_ID << 8);
            u8  addr = (u8)(pWriteMsg->ID & 0xFF);

            if(((idBase == testerBase) || (idBase == deviceBase))
             && (addr != (u8)SystemInfo_1[DeviceAddr]))
            {
                UpgSilence_RefreshOtherTimer();
            }
        }
        
        //3.工装通信状态检测
          if (ModbusReg[REG_TOOLING_CTRL][1] == 1)
        {
          ToolingCheckCANReviceAPI(*pWriteMsg,CANNum);
        }

    }
}

/*=============================================================================================
** @Name      : void USARTRcvMsgHandle_Hook(e_USARTNUM USARTNum, t_USARTMSG *rcvData)
** @Input     : SCINum:SCI通道 rcvMsg:接收到的数据
** @Output    : void
** @Function  : USART接收信息处理接口
** @The notes :
**===========================================================================================*/
void USARTRcvMsgHandle_Hook(e_USARTNUM USARTNum, t_USARTMSG *rcvData)
{
    if(USARTNum == eUSART6)
    {             
      Usart6RcvMsgHandle_API(rcvData);
    }

}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
#if 0


        // 4.test
        // TestCheckCANReviceAPI(*pWriteMsg,CANNum);
//test
void TestCheckCANReviceAPI(t_CANMSG msg, u8 canNum)
{

   //测试CAN通信 //如果接收到0e0f0001报文则回复0f0e0100 

   if (msg.ID == 0x0e0f0001)
   {

    t_CANMSG tmsg;
    tmsg.ID = 0x0f0e0100;
    tmsg.Len = 64;
    u8 Test_FDCanData[64] ={0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                             0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                            0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,};
    
    memcpy(tmsg.Data,Test_FDCanData,sizeof(Test_FDCanData));
 
    //清空接收缓存区数据
    memset(FDCANRData, 0, sizeof(FDCANRData));
 
    osDelay(100);
 
    //发送数据
   BSPCANSendQin(eCAN1,&tmsg);
    
   }

}
   #endif