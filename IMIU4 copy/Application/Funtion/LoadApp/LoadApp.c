/*==========================================Copyright(c)===========================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information------------------------------------------
** @File Name    : LoadApp.c
** @Founder      : WYP40505
** @Date         : 2026.05.06
** @Function     : 在线升级通信包模块
** @Instructions : APP应用程序使用
**===============================================================================================*/

/*=================================================================================================
** @Header file definition
**===============================================================================================*/
#include "LoadApp.h"
#include "BSPConfig.h"
#include "BSPIRQ.h"
#include "SystemInfo.h"
#include "BSPCAN.h"
#include "SystemPara.h"
/*=================================================================================================
** @Global variable definition
**===============================================================================================*/
// static u8 sFlashBootPara[eBOOT_PARA_FLASH_ID_LEN] = {0};    /*在线升级BOOT参数用Flash保存*/

                              
// u8 gLoadWorkModel = 0; 
// u8 gLoadWorkState = 0;
/*=================================================================================================
** @Static function declaration
**===============================================================================================*/
/*=================================================================================================
** @Name      : static u32 BuildLoadAREMsgID(u8 type, u8 sDev, u8 sAddr)
** @Input     : type:报文类型 sDev:源设备类型[0:从控 1:高压模块 2:主控 3:堆控 0xff:所有设备] sAddr:源设备地址
** @Output    : 在线升级回复报文ID
** @Function  : 构建在线升级回复报文ID
** @The notes :
**===============================================================================================*/
// static u32 BuildLoadAREMsgID(u8 type, u8 sDev, u8 sAddr);

/*=================================================================================================
** @Name      : static void EnterBootPro(void)
** @Input     : void
** @Output    : void
** @Function  : 跳转进入BOOT程序
** @The notes :
**===============================================================================================*/
// static void EnterBootPro(void);
#if 0
/*=================================================================================================
** @Name      : static void EnterAppPro(void)
** @Input     : void
** @Output    : void
** @Function  : 跳转进入应用程序
** @The notes :
**===============================================================================================*/
static void EnterAppPro(void);
#endif



/*=================================================================================================
** @Interface function definition
**===============================================================================================*/
/*=================================================================================================
** @Name      : void BootLoaderInit(void)
** @Input     : void
** @Output    : void
** @Function  : 在线升级模块初始化
** @The notes : BOOT和APP有区别
**===============================================================================================*/
void APPLoaderInit(void)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    // app_procedure_t procedure_stuct;
    uint32_t PageError = 0;
    uint32_t write_addr = FLASH_USER_UPDATE_FLAG_ADDR;
    uint64_t *pdata = 0;
    uint8_t tmp_array[8] = {0x5A,0x5A,0,0,0,0,0,0};//初始化为5A5A
    
    u8 *p_read = (u8 *)FLASH_USER_UPDATE_FLAG_ADDR;

    //首次上电 或参数被擦除
    if ((0xFFu == p_read[eBOOT_UDS_IAP_FLAG_ID0]) && (0xFFu == p_read[eBOOT_UDS_IAP_FLAG_ID1]) && (0xFFu == p_read[eBOOT_UDS_IAP_TYPE_ID0]))
    {
        __disable_irq();  /* 关中断保护Flash操作，防止并发破坏 */
        HAL_FLASH_Unlock();
        EraseInitStruct.TypeErase     = FLASH_CR_PER;
        EraseInitStruct.Banks         = FLASH_BANK_2;       //要擦除的位置位于flash尾部，BANK2
        EraseInitStruct.Page          = FLASH_PAGE_NB-2;    //擦除起始的sector编号,倒数第二个
        EraseInitStruct.NbPages       = 1;                  //擦除的sector数量
    
        if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){
            HAL_FLASH_Lock();
        }
    
        //写入升级标志和文件大小
        pdata = (uint64_t*)&tmp_array[0];
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
                                  write_addr, 
                                  ((uint64_t)*pdata)) == HAL_OK){
        }else{
            HAL_FLASH_Lock();
        }
        HAL_FLASH_Lock();  /* 确保Flash被锁定 */
        __enable_irq();
    }
}
#if 0 
/*=================================================================================================
** @Name      : void CanRcvLoadMsgHandleAPI(t_CANMSG *msg, u8 canNum)
** @Input     : msg:接收到的报文 canNum:接收CAN通道号
** @Output    : void
** @Function  : 在线升级模块CAN报文接收处理
** @The notes : CAN接收回调函数调用
**===============================================================================================*/
void CanRcvLoadMsgHandleAPI(t_CANMSG *msg, u8 canNum)
{
    u16 dir = 0;        //帧方向 0:上行信息 1:下行信息
    u16 fun = 0;        //功能码
    u16 type = 0;       //信息类型
    u16 dDev = 0;       //目标设备类型
    u16 dAddr = 0;      //目标设备地址

    dAddr = msg->ID & 0xff;                 //目标设备地址
    dDev = (msg->ID >> 8) & 0xff;           //目标设备类型
    type = (msg->ID >> 16) & 0xff;          //信息类型
    dir = (msg->ID >> 24) & 0x01;           //传输方向
    fun = (msg->ID >> 25) & 0x0f;           //功能码

    //非CAN升级信息不处理不转发
    if((fun < LOAD_REQ_FUN) || (fun > LOAD_RUP_FUN)
        || (type < eSEARCHDEV_MSG) || (type > eLOADFIN_MSG))
    {
        return;
    }

    //上行信息(不处理)
    if(dir == 0)
    {
        return;
    }

    //下行信息但不是本设备信息也不是广播信息
    if((dAddr != SystemInfo_1[DeviceAddr]) && (dAddr != 0xff))
    {
        //不处理本条信息
        return;
    }
  

    //非自身设备类型
    if((dDev != (LOAD_DEV_ID % 10)) && (dDev != 0xff))
    {
        //不处理本条信息
        return;
    }

    //判断报文功能类型
    switch(type)
    {
        //广播查询设备报文
        case eSEARCHDEV_MSG:

			if ((msg->Data[1] == 0x10) && (msg->Data[2] == 0x02))
			{
				sFlashBootPara[eBOOT_UDS_IAP_FLAG_ID0] = 0x5A;
				sFlashBootPara[eBOOT_UDS_IAP_FLAG_ID1] = 0x5A;
				//sFlashBootPara[eBOOT_UDS_IAP_FLAG_ID2] = 0x5A;
				//sFlashBootPara[eBOOT_UDS_IAP_FLAG_ID3] = 0x5A;
				/*当前为APP需要记录本地升级模式/等待握手状态再跳转至BOOT*/
				sFlashBootPara[eBOOT_JUM_FLAG_MODE_ID] = eLoMod_LocBoot;
				sFlashBootPara[eBOOT_JUM_FLAG_STAT_ID] = eLoSta_HandWait;

				(void)BSPINTFLASHEraseSector(BOOT_PARA_BLOCK4_FLASH_ADDR);
				

				(void)BSPINTFLASHWriteByte(BOOT_PARA_BLOCK4_FLASH_ADDR, sFlashBootPara, eBOOT_PARA_FLASH_ID_LEN);
				

				/*跳转进入BOOT复位*/
				EnterBootPro();
			}
			else
			{
				/*回复自身的设备信息和状态信息，包括芯片类型/通讯口/设备识别码/软件版本号/工作模式/工作状态*/
				msg->ID = BuildLoadAREMsgID(eSEARCHDEV_MSG, (LOAD_DEV_ID % 10), (u8)SystemInfo_1[DeviceAddr]);
				msg->Data[0] = (u8)eSTM32G0B1;   /*芯片类型*/
				msg->Data[1] = canNum;
				msg->Data[2] = gLoadWorkModel;
				msg->Data[3] = gLoadWorkState;
				msg->Data[4] = (u8)(LOAD_DEV_ID & 0xff);
				msg->Data[5] = (u8)(LOAD_DEV_ID >> 8 & 0xff);
				msg->Data[6] = (u8)(LOAD_SW_VERSION & 0xff);
				msg->Data[7] = (u8)(LOAD_SW_VERSION >> 8 & 0xff);
				msg->Len = 8;
				(void)BSPCANSendMsg((e_CANNUM)canNum, msg);
			}

            break;
        default:
            break;
    }
}


/*=================================================================================================
** @Static function definition
**===============================================================================================*/
void ReturnIMIU4ID(void)
{
    t_CANMSG msg1;

    				/*回复自身的设备信息和状态信息，包括芯片类型/通讯口/设备识别码/软件版本号/工作模式/工作状态*/
				msg1.ID = BuildLoadAREMsgID(eSEARCHDEV_MSG, (LOAD_DEV_ID % 10), (u8)SystemInfo_1[DeviceAddr]);
				msg1.Data[0] = (u8)eSTM32G0B1;   /*芯片类型*/
				msg1.Data[1] = eCAN1;
				msg1.Data[2] = gLoadWorkModel;
				msg1.Data[3] = gLoadWorkState;
				msg1.Data[4] = (u8)(LOAD_DEV_ID & 0xff);
				msg1.Data[5] = (u8)(LOAD_DEV_ID >> 8 & 0xff);
				msg1.Data[6] = (u8)(LOAD_SW_VERSION & 0xff);
				msg1.Data[7] = (u8)(LOAD_SW_VERSION >> 8 & 0xff);
				msg1.Len = 8;
				(void)BSPCANSendMsg((e_CANNUM)eCAN1, &msg1);
}



/*=================================================================================================
** @Name      : static u32 BuildLoadAREMsgID(u8 type, u8 sDev, u8 sAddr)
** @Input     : type:报文类型 sDev:源设备类型[0:从控 1:高压模块 2:主控 3:堆控 0xff:所有设备] sAddr:源设备地址
** @Output    : 在线升级回复报文ID
** @Function  : 构建在线升级回复报文ID
** @The notes :
**===============================================================================================*/
static u32 BuildLoadAREMsgID(u8 type, u8 sDev, u8 sAddr)
{
    u32 id = 0;

    //合成功能码(回复14)/传输方向(上行0)/报文类型/目标设备地址/源设备地址
    id = ((u32)LOAD_AEQ_FUN << 25) | ((u32)0 << 24)
        | ((u32)type << 16) | ((u32)sDev << 8) | (u32)sAddr;
    return(id);
}


/*=================================================================================================
** @Name      : static void EnterBootPro(void)
** @Input     : void
** @Output    : void
** @Function  : 跳转进入BOOT程序
** @The notes :
**===============================================================================================*/
static void EnterBootPro(void)
{
    typedef void(*pt2Func)(void);   //Pointer to Functions, void argument
    //__disable_irq();
    BSP_DISABLE_IRQ();
    NVIC_SystemReset();  
}
#endif
#if 0
/*=================================================================================================
** @Name      : static void EnterAppPro(void)
** @Input     : void
** @Output    : void
** @Function  : 跳转进入应用程序
** @The notes :
**===============================================================================================*/
static void EnterAppPro(void)
{


    typedef void (*pt2Func)(void);  //Pointer to Functions, void argument
    pt2Func pJumpTo;
    u32 appStack = 0;               //应用程序堆栈地址
    u32 appEntry = 0;               //应用程序跳转地址

    //BSP_DISABLE_IRQ();
    // __disable_irq();
    appStack = *(u32*)APP_ENRTY_POINT_FLASH_ADDR;
    appEntry = *(u32*)(APP_ENRTY_POINT_FLASH_ADDR + 4);

    //初始化中断向量表地址
    SCB->VTOR = (u32)APP_ENRTY_POINT_FLASH_ADDR;
    //nvic_vector_table_set(APP_ENRTY_POINT_FLASH_ADDR, 0);
    //设置栈顶地址
    //Set stack pointers to the application stack pointer
    __set_MSP(appStack);
    __set_PSP(appStack);

    //跳转到app
    pJumpTo = (pt2Func)appEntry;
    pJumpTo();
}
#endif