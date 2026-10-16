
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_DIDO_H
#define __BSP_DIDO_H
#include "main.h"
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
//TXY
#include "stm32f4xx_hal_rcc.h"
#include     "base_def.h"
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

#define DISLCD_LRST    GPIOE->BSRRH = GPIO_PIN_3 
#define ENLCD_LRST     GPIOE->BSRRL = GPIO_PIN_3
#define ENLCD_BL       GPIOE->BSRRH = GPIO_PIN_4
#define DISLCD_BL      GPIOE->BSRRL = GPIO_PIN_4



typedef enum 
{
	NEWDO1 = 0,					//A母排直流输出
	NEWDO2 = 1,					//B母排直流输出
	NEWDO3 = 2,					//???
	NEWDO4 = 3,					//并机继电器
	NEWDO5 = 4,					//B枪电子锁
	NEWDO6 = 5,					//锁开关？？
	NEWDO7 = 6,					//方向
	NEWDO8 = 7,					//GBT电源开关和 交流总输入的开关
	NEWDO9 = 8,					//AC枪 交流开关
	NEWDO10 = 9,				//风扇
	NEWDO11 = 10,				//CHADEMO 的DO1
	NEWDO12 = 11,				//CHADEMO 的DO2		D2_Ctrl_Off
	NEWDO13 = 12,				//A 充电灯			__A_YELLOW_LED_ON
	NEWDO14 = 13,				//告警灯			__A_RED_LED_ON
	NEWDO15 = 14,				//B充电灯			__B_YELLOW_LED_ON
	NEWDO16 = 15,				//AC 充电灯			 __AC_CHARGE_LED_ON

}NEWDoChannel_TypeDef;


#define MCCB_CONNECT				1
#define MCCB_DISCONNECT				0

typedef enum 
{
  DO_OFF = 0,
  DO_ON = 1,
}DoStatus_TypeDef;

//4051通道选择
typedef enum 
{
  AD_4051_PT1 = 0, 
  AD_4051_PT2, 
  AD_4051_PT3, 
  AD_4051_PT4, 
  AD_4051_Temp0,
  AD_4051_Temp1, 
  AD_4051_Temp2, 
  AD_4051_Temp3,
  AD_4051_MAX
}AD4051Channel_TypeDef;

//硬件DO脚位
typedef enum 
{
	//CSDO1
	RELAY1 = 0,					
	RELAY2 = 1,					
	RELAY8 = 2,					
	RELAY9 = 3,					
	RELAY10 = 4,					
	RELAY11 = 5,
	//CSDO2
	RELAY4 = 8+0,					
	RELAY12 = 8+1,					
	RELAY13 = 8+2,					
	RELAY14 = 8+3,			
	RELAY15 = 8+4,	
	RELAY16 = 8+5,	
	RELAY17 = 8+6,	
	RELAY18 = 8+7,	

	//CSDO3
	LED1 = 16+0,					
	LED2 = 16+1,					
	LED3 = 16+2,					
	LED4 = 16+3,			
	LED5 = 16+4,	
	LED6 = 16+5,	
	LED7 = 16+6,	
	
	//CSDO4
	AD_S1 = 24+0,					
	AD_S2 = 24+1,					
	AD_S3 = 24+2,							
	RELAY3 = 24+3,			
	RELAY5 = 24+4,	
	RELAY6 = 24+5,	
	RELAY7 = 24+6,	

	//CSDO5
	INSUL_RES_RELAY1 = 32+0,					
	INSUL_PE_RELAY1 = 32+1,					
	INSUL_RES_RELAY2 = 32+2,					
	INSUL_PE_RELAY2 = 32+3,	

	DO_CHANNEL_INVALID = 0xFF,
}DoChannel_TypeDef;

//DO功能
typedef enum 
{
	DO_FUNCTION_NONE = 0,
	SupportRelay_Grp6,					//6# Support Relays
	SupportRelay_Grp7,					//7# Support Relays
	BMS_PS_LEVEL_SEL_A,					//BMS power 12V/24V select
	BMS_PS_LEVEL_SEL_B,					//BMS power 12V/24V select
	SupportRelay_Grp11,					//11# Support Relays
	SupportRelay_Grp12,					//12# Support Relays
	SupportRelay_Grp1,					//1# Support Relays
	Reserve,										//Reserve
	SupportRelay_Grp4,					//4# Support Relays
	SupportRelay_Grp2,					//2# Support Relays
	SupportRelay_Grp3,					//3# Support Relays
	SupportRelay_Grp9,					//9# Support Relays
	SupportRelay_Grp10,					//10# Support Relays
	Blue_LED,										//
	ALARM_LED_2,								//
	RED_LED,										//
	FALT_LED_2,									//
	GREEN_LED,									//
	RUN_LED_2,									//
	
	SupportRelay_Grp8,					//8# Support Relays
	//GUN_ELOCK_A_2,

	Transformer_scrambler_fan,	//Transformer Scrambler Fan
	//GUN_ELOCK_B_2,

	INSULT_RES_SW_A,						//绝缘电阻臂投切-A
	INSULT_PE_SW_A,							//绝缘PE投切

	INSULT_RES_SW_B,						//绝缘电阻臂投切-B
	INSULT_PE_SW_B,							//绝缘PE投切

	SupportRelay_Grp5,					//5# Support Relays

	DO_Function_Max,

}DOFunction_TypeDef;

typedef struct _tagDOFunctionMap tagDOFunctionMap;
struct _tagDOFunctionMap
{
	unsigned char u8IsValid;		//是否启用
	DoChannel_TypeDef u8DoNo[2];		//可以关联两个继电器
	unsigned char u8IsActionReverse;	//是否要反向输出 1-DO要反向 0-不反向
	//void *(DoFunction)(struct _tagDOFunctionMap *pDOFunctionMap, DoStatus_TypeDef u8DoStatus);
	void (*DoFunction)(DOFunction_TypeDef DoFuncType, DoStatus_TypeDef u8DoStatus);
};
enum 
{
	DO_CS_NO_1 = 0,
	DO_CS_NO_2,
	DO_CS_NO_3,
	DO_CS_NO_4,
	DO_CS_NO_5,
	DO_CS_NUM_MAX,
};    // CS选通通道数量，对应 DoChannel_TypeDef

#define	AD4051SEL_MAX_DELAY		20
typedef struct _tagDoDiCtrlMng
{
	unsigned char u8DoCtrlStat[DO_CS_NUM_MAX];
	unsigned char u8AD4051SelStat;			//AD 4051选通单独一个变量
	unsigned char u8AD4051SelStatBack;		//AD 4051选通单独一个变量 
	unsigned short u16AD4051SelDelay;
	
	unsigned char u8DoActionFlag;

	tagDOFunctionMap DOFunctionMap[DO_Function_Max];
}tagDoCtrlMng;


extern tagDoCtrlMng gs_DoCtrlMng;


#define __A_DC_MCCB_ON() do {\
} while (0);

#define __B_DC_MCCB_ON() do {\
} while (0);

//并机控制！！！
#define __PARALLEL_ON() do {\
			gs_EnergyOutputMng.ParallelRelayStat = MCCB_CONNECT;\
} while (0);

//并机控制！！！
#define __PARALLEL_ON_B() do {\
} while (0);

#define __PARALLEL_OFF_B() do {\
} while (0);


void InitDoCtrlMng(void);

//void BSP_LvdCtrl(LvdChannel_TypeDef u8LvdChannel,LvdStatus_TypeDef u8LvdStatus);
void BSP_InitGpioF(void);
//TXY
void BSP_Run_LED_ON(void);
void BSP_Run_LED_OFF(void);
void BSP_Alm_LED_ON(void);
void BSP_Alm_LED_OFF(void);
void BSP_vGpioEXIT5_Init(void);
void BSP_vGpioEXIT1Init(void);
//void BSP_DO_Ctrl(NEWDoChannel_TypeDef iDONo, DoStatus_TypeDef u8DoStatus);
void BSP_vGpioInit(void);
void BSP_vGpioReInit(void);
void BSP_vCLOCKInit(void);
GPIO_PinState u8GetPFOStatus(void);
GPIO_PinState BSP_u8GetPFOStatus(void);
void BSP_u1_heartbeat_led_toggle(void);
void BSP_vBuzzer_On(void);
void BSP_vBuzzer_Off(void);

unsigned char BSP_u8GetCSDI1Status(void);
unsigned char BSP_u8GetCSDI2Status(void);

void HAL_GPIO_EXTI_5_Callback(uint16_t GPIO_Pin);
//GPIO_PinState BSP_u8GetFAN_ALM1Status(void);
//GPIO_PinState BSP_u8GetFAN_ALM2Status(void);
//GPIO_PinState BSP_u8GetFAN_ALM3Status(void);
//GPIO_PinState BSP_u8GetFAN_ALM4Status(void);
//GPIO_PinState BSP_u8GetFAN_ALM5Status(void);
//GPIO_PinState BSP_u8GetFAN_ALM6Status(void);
//GPIO_PinState BSP_u8GetSSD5Status(void);
//GPIO_PinState BSP_u8GetSD55Status(void);

GPIO_PinState BSP_u8GetVer0(void);
GPIO_PinState BSP_u8GetVer1(void);
GPIO_PinState BSP_u8GetSW1Status(void);
GPIO_PinState BSP_u8GetSW2Status(void);
GPIO_PinState BSP_u8GetSW3Status(void);
GPIO_PinState BSP_u8GetSW4Status(void);

void TESTXXXXXXX(int iPort, int pinNo, int val);
void BSP_vFeedFan(void);
void BSP_WDGInit(void);
void BSP_WDGRefresh(void);

void InitDoCtrlMng_PowerOn(void);
void AD_4051_ChanelSel(AD4051Channel_TypeDef u8ChannelNo);
void BSP_DO_Ctrl(DoChannel_TypeDef iDONo, DoStatus_TypeDef u8DoStatus);
void DO_Function(DOFunction_TypeDef DoFuncType, DoStatus_TypeDef u8DoStatus);
void GBT_GUN_SPEAR_LOCK_PulseType(DOFunction_TypeDef DoFuncType, DoStatus_TypeDef u8DoStatus);
#endif