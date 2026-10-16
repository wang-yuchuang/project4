/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPDIDO.c
** @Founder      : WYC834
** @Date         : 2025.8.21
** @Function     : DIDO驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPDIDO_H
#define _BSPDIDO_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
#include "BSPGPIO.h"
#include "SystemInfo.h"
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
//外部控制
#define RS485_PORT                 ePOR_C           
#define RS485_PIN                  ePIN_2

#define CRELAY1_PORT                 ePOR_B           
#define CRELAY1_PIN                  ePIN_7

#define CRELAY2_PORT                 ePOR_B          
#define CRELAY2_PIN                  ePIN_8

#define PE_RLY_PORT                  ePOR_C           
#define PE_RLY_PIN                   ePIN_10

#define IMD_ALM_PORT                 ePOR_D          
#define IMD_ALM_PIN                  ePIN_0

#define RUN_LED_PORT                 ePOR_C           
#define RUN_LED_PIN                  ePIN_9

#define SPI_CS_PORT                  ePOR_A           
#define SPI_CS_PIN                   ePIN_15

#define Flash_SPI_NSS_PORT           ePOR_A           
#define Flash_SPI_NSS_PIN            ePIN_4

#define SPI3_MISO_PORT           ePOR_C           
#define SPI3_MISO_PIN            ePIN_11

//外部反馈
#define VRE0_PORT                    ePOR_A           
#define VRE0_PIN                     ePIN_9

#define VRE1_PORT                    ePOR_A          
#define VRE1_PIN                     ePIN_8

#define SCH0_PORT                    ePOR_C           
#define SCH0_PIN                     ePIN_6

#define SCH1_PORT                    ePOR_C          
#define SCH1_PIN                     ePIN_7

#define DI_1_PORT                    ePOR_B          
#define DI_1_PIN                     ePIN_15

#define SW1_PORT                     ePOR_A           /*拨码开关1*/
#define SW1_PIN                      ePIN_0

#define SW2_PORT                     ePOR_A           /*拨码开关2*/
#define SW2_PIN                      ePIN_1

#define SW3_PORT                     ePOR_A           /*拨码开关3*/
#define SW3_PIN                      ePIN_2

#define SW4_PORT                     ePOR_A           /*拨码开关4*/
#define SW4_PIN                      ePIN_3


#define  RS485_SendEnable()  BSPGPIOSetPortState(RS485_PORT, RS485_PIN, eIOHIG)
#define  RS485_ReceiveEnable()  BSPGPIOSetPortState(RS485_PORT, RS485_PIN, eIOLOW)

#define CRELAY1_Open()   BSPGPIOSetPortState(CRELAY1_PORT, CRELAY1_PIN, eIOLOW)//打开继电器
#define CRELAY1_Close()  BSPGPIOSetPortState(CRELAY1_PORT, CRELAY1_PIN, eIOHIG)//闭合继电器

#define CRELAY2_Open()   BSPGPIOSetPortState(CRELAY2_PORT, CRELAY2_PIN, eIOLOW)
#define CRELAY2_Close()  BSPGPIOSetPortState(CRELAY2_PORT, CRELAY2_PIN, eIOHIG)

#define PE_RLY_Open()    BSPGPIOSetPortState(PE_RLY_PORT, PE_RLY_PIN, eIOLOW)
#define PE_RLY_Close()   BSPGPIOSetPortState(PE_RLY_PORT, PE_RLY_PIN, eIOHIG)

#define IMD_ALM_ON()     BSPGPIOSetPortState(IMD_ALM_PORT, IMD_ALM_PIN, eIOLOW)
#define IMD_ALM_OFF()    BSPGPIOSetPortState(IMD_ALM_PORT, IMD_ALM_PIN, eIOHIG)
#define IMD_ALM_Toggle() BSPGPIOTogglePortData(IMD_ALM_PORT, IMD_ALM_PIN)

#define LED_RUN_ON()     BSPGPIOSetPortState(RUN_LED_PORT, RUN_LED_PIN, eIOLOW)
#define LED_RUN_OFF()    BSPGPIOSetPortState(RUN_LED_PORT, RUN_LED_PIN, eIOHIG)
#define LED_RUN_Toggle() BSPGPIOTogglePortData(RUN_LED_PORT, RUN_LED_PIN)

#define SPI_CS_Disable()  BSPGPIOSetPortState(SPI_CS_PORT, SPI_CS_PIN, eIOLOW)
#define SPI_CS_Enable() BSPGPIOSetPortState(SPI_CS_PORT, SPI_CS_PIN,eIOHIG)

#define Flash_SPI_NSS_Enable()   BSPGPIOSetPortState(Flash_SPI_NSS_PORT, Flash_SPI_NSS_PIN, eIOLOW)
#define Flash_SPI_NSS_Disable()  BSPGPIOSetPortState(Flash_SPI_NSS_PORT, Flash_SPI_NSS_PIN, eIOHIG)

#define DI1_Pin_Read()   BSPGPIOGetPortState(DI_1_PORT, DI_1_PIN)
#define VRE0_Pin_Read()  BSPGPIOGetPortState(VRE0_PORT, VRE0_PIN)
#define VRE1_Pin_Read()  BSPGPIOGetPortState(VRE1_PORT, VRE1_PIN)   

#define SCH0_Pin_Read()  BSPGPIOGetPortState(SCH0_PORT, SCH0_PIN)
#define SCH1_Pin_Read()  BSPGPIOGetPortState(SCH1_PORT, SCH1_PIN)

#define CRELAY1_Pin_Read()  BSPGPIOGetPortState(CRELAY1_PORT, CRELAY1_PIN)
#define CRELAY2_Pin_Read()  BSPGPIOGetPortState(CRELAY2_PORT, CRELAY2_PIN)
#define LED_RUN_Pin_Read()  BSPGPIOGetPortState(RUN_LED_PORT, RUN_LED_PIN)
#define IMD_ALM_Pin_Read()  BSPGPIOGetPortState(IMD_ALM_PORT, IMD_ALM_PIN)
#define PE_RLY_Pin_Read()  BSPGPIOGetPortState(PE_RLY_PORT, PE_RLY_PIN)

#define HALLPFAULT_PORT              ePOR_A
#define HALLPFAULT_PIN               ePIN_11

#define DIAGNOSIS_PORT              ePOR_A
#define DIAGNOSIS_PIN               ePIN_10

#define HALL_PFAULT_Pin_Read()  BSPGPIOGetPortState(HALLPFAULT_PORT, HALLPFAULT_PIN)
#define DIAGNOSIS_Pin_Read()  BSPGPIOGetPortState(DIAGNOSIS_PORT, DIAGNOSIS_PIN)





/*输入端口序号*/
typedef enum
{
    ePIn_HWVER0   	 = 0, 		/*硬件版本号端口1*/
    ePIn_HWVER1   	 = 1, 		/*硬件版本号端口2*/ 
	eINPUTPORT_Len
}e_INPUTPORT;


// 定义 SW 位掩码
#define SW_BIT_MASK  ((1 << Sw1) | (1 << Sw2) | (1 << Sw3) | (1 << Sw4))  // 0x0078
/*=============================================================================================
** @Data type definition
**===========================================================================================*/

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/

/*=============================================================================================
** @Name      : u8 SWReadData(void)
** @Input     : 
** @Output    : 读取拨码地址
** @Function  : 读取拨码地址
** @The notes : 用户调用
**===========================================================================================*/
u8 SWReadData(void);

/*=============================================================================================
** @Name      : void CRELAY1_Contr_Open(void)
** @Input     : void
** @Output    : 打开DO1 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void CRELAY1_Contr_Open(void);

/*=============================================================================================
** @Name      : void CRELAY1_Contr_Close(void)
** @Input     : void
** @Output    : 闭合DO1 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void CRELAY1_Contr_Close(void);

/*=============================================================================================
** @Name      : void CRELAY2_Contr_Open(void)
** @Input     : void
** @Output    : 打开DO2 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void CRELAY2_Contr_Open(void);

/*=============================================================================================
** @Name      : void CRELAY2_Contr_Close(void)
** @Input     : void
** @Output    : 闭合DO2 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void CRELAY2_Contr_Close(void);

/*=============================================================================================
** @Name      : void PE_RLY_Contr_Open(void)
** @Input     : void
** @Output    : 打开PE_RLY 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void PE_RLY_Contr_Open(void);

/*=============================================================================================
** @Name      : void PE_RLY_Contr_Close(void)
** @Input     : void
** @Output    : 闭合PE_RLY 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void PE_RLY_Contr_Close(void);


/*=============================================================================================
** @Name      : void PE_RLY_Contr_Close(void)
** @Input     : void
** @Output    : 获取硬件版本信息
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void DEVGPIOGetHWVersion(void);

#endif