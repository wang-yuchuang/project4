/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPConfig.c
** @Founder      : WYC834
** @Date         : 2025.8.15
** @Function     : 各驱动模块初始化接口
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPMCG.h"
#include "BSPDIDO.h"
#include "BSPGPIO.h"
#include "BSPWDOG.h"
#include "BSPDMA.h" 
#include "BSPTIM.h"
#include "BSPADC.h"
#include "BSPCAN.h"
#include "BSPSPI.h"
#include "BSPUSART.h"
#include "BSPIIC.h"
#include "BSPFLASH.h"
#include "BSPGD25Q32E.h"
#include "Insulation.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
volatile u_BSPFunCfg gBSPFunCfg;
volatile s_BSPParaCfg gBSPParaCfg;
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPUserCfgInit(void)
** @Input     : void
** @Output    : void
** @Function  : BSP驱动功能用户配置模块参数初始化
** @The notes : Main()函数调用，上电初始化
**===========================================================================================*/
void BSPUserCfgInit(void)
{
    /*读取驱动模块使能配置*/
    gBSPFunCfg.Bits.TIM6En = TIM6_FUN_EN;		/*定时器6使能配置*/
    gBSPFunCfg.Bits.TIM7En = TIM7_FUN_EN;		/*定时器7使能配置*/

    gBSPFunCfg.Bits.CAN1En = CAN1_FUN_EN;		/*CAN1使能配置*/

    gBSPFunCfg.Bits.SPI1En = SPI1_FUN_EN;		/*SPI1使能配置*/
    gBSPFunCfg.Bits.SPI3En = SPI3_FUN_EN;		/*SPI3使能配置*/
        
    gBSPFunCfg.Bits.USART6En = USART6_FUN_EN;		/*USART6使能配置*/
    gBSPFunCfg.Bits.IIC2En = IIC2_FUN_EN;		    /*IIC使能配置*/
    gBSPFunCfg.Bits.FLASHEn = FLASH_FUN_EN;

    gBSPFunCfg.Bits.ADCEn  = ADC_FUN_EN;         /*ADC使能配置*/

    gBSPFunCfg.Bits.DMAEn  = DMA_FUN_EN;         /*DMA使能配置*/
    gBSPFunCfg.Bits.GPIOEn = GPIO_FUN_EN;        /*GPIO使能配置*/
    gBSPFunCfg.Bits.DIDOEn  = DIDO_FUN_EN;       /*DIDO使能配置*/
    gBSPFunCfg.Bits.WDOGEn = WDOG_FUN_EN;        /*wdog使能配置*/

    /*读取驱动模块功能参数配置信息*/
    gBSPParaCfg.Can1Rate   = CAN1_BPS;           /*CAN1通信波特率(1kbps)*/
    gBSPParaCfg.Usart6Rate   = USART6_BPS;         /*USART6通信波特率(1Mbps)*/
    gBSPParaCfg.Spi1Rate   = SPI1_BPS;         /*SPI1通信波特率(1Mbps)*/
    gBSPParaCfg.Spi3Rate   = SPI3_BPS;         /*SPI2通信波特率(1Mbps)*/

}

/*=============================================================================================
** @Name      : void BSPDriverInit(void)
** @Input     : void
** @Output    : void
** @Function  : 所有底层驱动模块进行初始化
** @The notes : Main()函数调用，上电初始化
**===========================================================================================*/
void BSPDriverInit(void)
{
    BSPMCGInit();                                                               /*初始化时钟倍频*/                                                         
    if(1 == gBSPFunCfg.Bits.GPIOEn)                                              /*GPIO使能*/
    {
        BSPALLGPIOInit();                                                       /*初始化gpio口*/   
    }

    if(1 == gBSPFunCfg.Bits.DMAEn)                                              /*DMA使能*/
    {
        BSPDMAInit();                                                           /*初始化DMA*/      
    }
    
    if(1 == gBSPFunCfg.Bits.ADCEn)                                              /*ADC使能*/
    {
        BSPADCInit(eADC_MOD1);                                                  /*初始化ADC*/
    }

    if(1 == gBSPFunCfg.Bits.CAN1En)                                             /*CAN1使能*/
    {
        BSPCANInit(eCAN1);                                                      /*初始化CAN1*/
    }

    if(1 == gBSPFunCfg.Bits.SPI1En)                                             /*SPI_1使能*/
    {
       BSPSPIInit(eSPI1);                                                       /*初始化SPI_1*/
    }

    if(1 == gBSPFunCfg.Bits.SPI3En)                                             /*SPI_3使能*/
    {
       BSPSPIInit(eSPI3);                                                       /*初始化SPI_3*/
    }
    

    if(1 == gBSPFunCfg.Bits.USART6En)                                           /*USART_6使能*/
    {
        BSPUSARTInit(eUSART6);                                                  /*初始化USART_6*/
    }

    if(1== gBSPFunCfg.Bits.IIC2En)
    {
        BSPIICInit(eIIC2);
    }

    if(1 == gBSPFunCfg.Bits.TIM6En)                                             /*定时器TIM6使能*/
    {                                                                           /*定时器TIM6*/
        BSPTIMInit(eTIM6);                                                      /*初始化定时器TIM6*/
    }
    if(1 == gBSPFunCfg.Bits.TIM7En)                                             /*定时器TIM7使能*/
    {                                                                           /*定时器TIM7*/
        BSPTIMInit(eTIM7);                                                      /*初始化定时器TIM7*/
    }

    if(1 == gBSPFunCfg.Bits.FLASHEn)                                            /*内部FLASH使能*/
    {
        BSPINTFLASHInit();                                                      /*初始化内部FLASH*/
    }
    
    if(1 == gBSPFunCfg.Bits.WDOGEn)                                             /*内部看门狗模块使能*/
    {
        BSPWatchDogInit();                                                      /*初始化看门狗*/
    }
}

/*=============================================================================================
** @Name      : void DEVDriverInit(void)
** @Input     : void
** @Output    : void
** @Function  : 所有外围设备驱动模块初始化
** @The notes : Main()函数调用，上电初始化。必须在底层驱动初始化完成后调用
**===========================================================================================*/
void DEVDriverInit(void)
{


    BSP_GD25Qx_Init();/*外部falsh初始化*/ 
    
    DEVGPIOGetHWVersion();/*初始化硬件版本号*/ 
    
    Insulation_InitInsulationData();/*绝缘检测信息初始化*/  

    if(1 == gBSPFunCfg.Bits.USART6En)
    {
        BSPUSARTBufInit(); /*初始化USART发送/接收BUF*/ 
    }  
    BSPCanBufInit();    /*初始化CAN发送/接收BUF*/ 
    
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
