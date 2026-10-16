#include "bsp_dido.h"
#include "bsp_fsmc.h"
#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */


uint8_t g_u8LvdStatusBuk=0;

static GPIO_InitTypeDef  GPIO_InitStruct;
IWDG_HandleTypeDef IwdgHandle;

tagDoCtrlMng gs_DoCtrlMng = {0};

void BSP_DO_Ctrl(DoChannel_TypeDef iDONo, DoStatus_TypeDef u8DoStatus);

/*********************************************************************************************************************
 * @brief 控制数字输出功能
 * 
 * 根据指定的功能类型和状态控制数字输出通道。支持反向动作配置，
 * 当u8IsActionReverse为1时，实际输出状态与输入状态相反。
 * 
 * @param DoFuncType 数字输出功能类型
 * @param u8DoStatus 期望的输出状态(DO_ON/DO_OFF)
 * 
 * @note 1. 仅当功能映射有效且通道号有效时执行操作
 *       2. 通过BSP_DO_Ctrl函数实现最终输出控制
 **********************************************************************************************************************/
void DO_Function(DOFunction_TypeDef DoFuncType, DoStatus_TypeDef u8DoStatus)
{

	unsigned char ucDoMode;
	struct _tagDOFunctionMap *pDOFunctionMap = &gs_DoCtrlMng.DOFunctionMap[DoFuncType];

	if(pDOFunctionMap->u8IsValid == 0)
	{
		return;
	}

	if(pDOFunctionMap->u8DoNo[0] == DO_CHANNEL_INVALID)
	{
		return;
	}

	ucDoMode = u8DoStatus;

	if(pDOFunctionMap->u8IsActionReverse == 1)
	{
		if(u8DoStatus == DO_ON)
		{
			ucDoMode = DO_OFF;
		}
		else
		{
			ucDoMode = DO_ON;
		}
	}	
	BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[0], (DoStatus_TypeDef)ucDoMode);
}

/*****************************************************************************************************************************************************
 * @brief 控制枪锁脉冲类型
 *
 * 根据输入的DO功能类型和状态，控制枪锁的脉冲输出模式。
 *
 * @param DoFuncType DO功能类型
 * @param u8DoStatus DO状态(DO_ON/DO_OFF)
 *
 * @note
 * 1. 当u8DoStatus为DO_ON时，执行锁枪操作：
 *    - 先激活主DO通道，延时20ms
 *    - 然后对辅助DO通道进行3次200ms间隔的脉冲输出
 *    - 最后关闭主DO通道
 *
 * 2. 当u8DoStatus为DO_OFF时，执行解锁操作：
 *    - 先关闭主DO通道，延时20ms
 *    - 然后对辅助DO通道进行3次200ms间隔的脉冲输出
 *    - 最后确保主DO通道关闭
 *
 * 函数会先检查DO功能映射是否有效，以及DO通道号是否有效，
 * 任一条件不满足则直接返回。
 **********************************************************************************************************************************************************/
void GBT_GUN_SPEAR_LOCK_PulseType(DOFunction_TypeDef DoFuncType, DoStatus_TypeDef u8DoStatus)
{
	struct _tagDOFunctionMap *pDOFunctionMap = &gs_DoCtrlMng.DOFunctionMap[DoFuncType];

	if(pDOFunctionMap->u8IsValid == 0)
	{
		return;
	}

	if((pDOFunctionMap->u8DoNo[0] == DO_CHANNEL_INVALID)
		|| (pDOFunctionMap->u8DoNo[1] == DO_CHANNEL_INVALID))
	{
		return;
	}

	if(u8DoStatus == DO_ON)//锁枪
	{
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[0], DO_ON);
		OSTimeDly(20);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_OFF);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_ON);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_OFF);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_ON);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_OFF);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[0], DO_OFF);
	}	
	else//解锁
	{
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[0], DO_OFF);
		OSTimeDly(20);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_ON);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_OFF);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_ON);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_OFF);
		OSTimeDly(200);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_ON);
		OSTimeDly(200);	
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[1], DO_OFF);
		BSP_DO_Ctrl(pDOFunctionMap->u8DoNo[0], DO_OFF);
	}
}

/***************************************************************************************************************************
 * @brief 初始化数字输出控制管理器（上电时调用）
 *
 * 该函数用于在系统上电时初始化数字输出控制管理器的状态和功能映射。
 * 主要完成以下操作：
 * 1. 清零所有数字输出控制状态
 * 2. 初始化AD4051选择状态
 * 3. 初始化所有数字输出功能映射表项为无效状态
 * 4. 配置特定的数字输出功能映射，包括：
 *    - 支持继电器组(1-12)
 *    - BMS电源等级选择(A/B)
 *    - 各种LED指示灯控制
 *    - 变压器风扇控制
 *    - 绝缘电阻/PE投切控制
 ****************************************************************************************************************************/
void InitDoCtrlMng_PowerOn(void)
{
	unsigned char i;

	for(i = 0; i < DO_CS_NUM_MAX; i++)
	{
		gs_DoCtrlMng.u8DoCtrlStat[i] = 0;
	}
	gs_DoCtrlMng.u8AD4051SelStat = 0;

	for(i = 0; i < DO_Function_Max; i++)
	{
		gs_DoCtrlMng.DOFunctionMap[i].u8IsValid = 0;
		gs_DoCtrlMng.DOFunctionMap[i].u8DoNo[0] = DO_CHANNEL_INVALID;
		gs_DoCtrlMng.DOFunctionMap[i].u8DoNo[1] = DO_CHANNEL_INVALID;
		gs_DoCtrlMng.DOFunctionMap[i].u8IsActionReverse = 0;
		gs_DoCtrlMng.DOFunctionMap[i].DoFunction = &DO_Function;
	}

	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp6].u8IsValid = 1;				//6# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp6].u8DoNo[0] = RELAY1;
	
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp7].u8IsValid = 1;				//7# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp7].u8DoNo[0] = RELAY2;
	
	gs_DoCtrlMng.DOFunctionMap[BMS_PS_LEVEL_SEL_A].u8IsValid = 1;					//A 国标枪12V/24V辅助电源等级选择	gs_DoCtrlMng.DOFunctionMap[BMS_PS_LEVEL_SEL_A].u8DoNo[0] = RELAY10;
	gs_DoCtrlMng.DOFunctionMap[BMS_PS_LEVEL_SEL_A].u8DoNo[0] = RELAY10;
	
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp11].u8IsValid = 1;					//11# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp11].u8DoNo[0] = RELAY11;
	
	gs_DoCtrlMng.DOFunctionMap[BMS_PS_LEVEL_SEL_B].u8IsValid = 1;					//B 国标枪12V/24V辅助电源等级选择	gs_DoCtrlMng.DOFunctionMap[BMS_PS_LEVEL_SEL_B].u8DoNo[0] = RELAY8;
	gs_DoCtrlMng.DOFunctionMap[BMS_PS_LEVEL_SEL_B].u8DoNo[0] = RELAY8;
	
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp12].u8IsValid = 1;					//12# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp12].u8DoNo[0] = RELAY9;
	
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp1].u8IsValid = 1;						//1# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp1].u8DoNo[0] = RELAY12;

	gs_DoCtrlMng.DOFunctionMap[Reserve].u8IsValid = 1;											//Reserve
	gs_DoCtrlMng.DOFunctionMap[Reserve].u8DoNo[0] = RELAY4;

	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp4].u8IsValid = 1;						//4# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp4].u8DoNo[0] = RELAY15;

	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp2].u8IsValid = 1;						//2# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp2].u8DoNo[0] = RELAY13;

	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp3].u8IsValid = 1;						//3# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp3].u8DoNo[0] = RELAY14;

	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp9].u8IsValid = 1;						//9# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp9].u8DoNo[0] = RELAY17;

	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp10].u8IsValid = 1;						//10# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp10].u8DoNo[0] = RELAY18;

	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp5].u8IsValid = 1;						//5# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp5].u8DoNo[0] = RELAY16;
	
	gs_DoCtrlMng.DOFunctionMap[Blue_LED].u8IsValid = 1;							//蓝灯控制
	gs_DoCtrlMng.DOFunctionMap[Blue_LED].u8DoNo[0] = LED1;
	gs_DoCtrlMng.DOFunctionMap[ALARM_LED_2].u8IsValid = 1;					//告警指示灯2
	gs_DoCtrlMng.DOFunctionMap[ALARM_LED_2].u8DoNo[0] = LED2;
	gs_DoCtrlMng.DOFunctionMap[RED_LED].u8IsValid = 1;							//红灯控制
	gs_DoCtrlMng.DOFunctionMap[RED_LED].u8DoNo[0] = LED3;
	gs_DoCtrlMng.DOFunctionMap[FALT_LED_2].u8IsValid = 1;					//故障指示灯2	
	gs_DoCtrlMng.DOFunctionMap[FALT_LED_2].u8DoNo[0] = LED4;
	gs_DoCtrlMng.DOFunctionMap[GREEN_LED].u8IsValid = 1;					//绿灯控制
	gs_DoCtrlMng.DOFunctionMap[GREEN_LED].u8DoNo[0] = LED5;
	gs_DoCtrlMng.DOFunctionMap[RUN_LED_2].u8IsValid = 1;					//运行指示灯2	
	gs_DoCtrlMng.DOFunctionMap[RUN_LED_2].u8DoNo[0] = LED6;

	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp8].u8IsValid = 1;					//8# Support Relays
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp8].u8DoNo[0] = RELAY3;
	gs_DoCtrlMng.DOFunctionMap[SupportRelay_Grp8].u8DoNo[1] = RELAY5;

	gs_DoCtrlMng.DOFunctionMap[Transformer_scrambler_fan].u8IsValid = 1;					//Transformer Scrambler Fan
	gs_DoCtrlMng.DOFunctionMap[Transformer_scrambler_fan].u8DoNo[0] = RELAY6;
	gs_DoCtrlMng.DOFunctionMap[Transformer_scrambler_fan].u8DoNo[1] = RELAY7;

	gs_DoCtrlMng.DOFunctionMap[INSULT_RES_SW_A].u8IsValid = 1;					//绝缘电阻臂投切-A	
	gs_DoCtrlMng.DOFunctionMap[INSULT_RES_SW_A].u8DoNo[0] = INSUL_RES_RELAY1;

	gs_DoCtrlMng.DOFunctionMap[INSULT_PE_SW_A].u8IsValid = 1;					//绝缘PE投切-A	
	gs_DoCtrlMng.DOFunctionMap[INSULT_PE_SW_A].u8DoNo[0] = INSUL_PE_RELAY1;

	gs_DoCtrlMng.DOFunctionMap[INSULT_RES_SW_B].u8IsValid = 1;					//绝缘电阻臂投切-B	
	gs_DoCtrlMng.DOFunctionMap[INSULT_RES_SW_B].u8DoNo[0] = INSUL_RES_RELAY2;

	gs_DoCtrlMng.DOFunctionMap[INSULT_PE_SW_B].u8IsValid = 1;					//绝缘PE投切-B	
	gs_DoCtrlMng.DOFunctionMap[INSULT_PE_SW_B].u8DoNo[0] = INSUL_PE_RELAY2;
}

/*******************************************************************************************************************************
 * @brief 选择AD4051的通道
 *
 * 该函数用于选择AD4051多路复用器的输入通道。它会先检查通道号是否有效，
 * 然后通过修改DO控制状态寄存器来切换通道，并更新当前选择的通道状态。
 *
 * @param u8ChannelNo 要选择的通道号，范围必须小于等于AD_4051_Temp3
 *
 * @note 该函数是线程安全的，内部使用了互斥锁保护共享资源
 ********************************************************************************************************************************/
void AD_4051_ChanelSel(AD4051Channel_TypeDef u8ChannelNo)
{
	unsigned char uctmep;

	if(u8ChannelNo > AD_4051_Temp3)
	{
		return;
	}

	MutexLock(gEvChargeInfo.hMetex_DoCtrl);

	uctmep = gs_DoCtrlMng.u8DoCtrlStat[DO_CS_NO_4] & 0xF8;
	gs_DoCtrlMng.u8DoCtrlStat[DO_CS_NO_4] = uctmep | u8ChannelNo;

	CSDO4 = gs_DoCtrlMng.u8DoCtrlStat[DO_CS_NO_4];

	gs_DoCtrlMng.u8AD4051SelStat = u8ChannelNo;

	/*switch(u8CSNO)
	{
		case AD_4051_PT1:
			break;
		case AD_4051_PT2:
			break;
		case AD_4051_PT3:
			break;
		case AD_4051_PT4:
			break;
		case AD_4051_Temp0:
			break;
		case AD_4051_Temp1:
			break;
		case AD_4051_Temp2:
			break;
		case AD_4051_Temp3:
			break;
		default:
			break;
	}*/
	MutexUnlock(gEvChargeInfo.hMetex_DoCtrl);
}

/****************************************************************************************************
	协议中控制和上报DO序号      电路注释		 对应凤凰端子输出
	DO1				--	CRELAY1     		-- 母排A---  j6-1
	DO2				--	CRELAY2     		-- 母排B---  j6-5
	DO3				--	CRELAY3     		-- A电子锁方向 未引出。
	DO4				--	CRELAY4     		-- 并机 ---  j6-2
	DO5				--	CRELAY5     		-- A电子锁控制---J12-24
	DO6				--	CRELAY6     		-- B电子锁控制---J12-68
	DO9	    		--	CRELAY9     		-- B枪 BMS供电---J3-68脚
	DO10			--	CRELAY10    		-- 电源、预约指示灯---J12-57脚
	DO11    		--	CRELAY11    		-- A枪BMS电源选择---J3-13脚
	DO12			--	CRELAY12    		-- B枪BMS电源选择---J3-57脚
	DO13			--	CRELAY13    		-- A充电灯---J13-24脚
	DO14			--	CRELAY14    		-- A告警灯---J13-68脚
	DO7	    		--	CRELAY7     		-- B枪电子锁方向---未引出
	DO8				--	CRELAY8     		-- A枪 BMS供电---J3-24脚
	DO15			--	CRELAY15    		-- B充电灯---J13-13脚
	DO16			--	CRELAY16			-- B告警灯---J13-57脚
******************************************************************************************************/
UINT8 xxx1, xxx2;
/******************************************************************************************************
 * @brief 控制数字输出通道状态
 *
 * @param iDONo 数字输出通道号
 * @param u8DoStatus 输出状态(DO_ON/DO_OFF)
 *
 * @note 该函数会通过互斥锁保护对全局DO控制状态的修改，
 *       并根据通道号计算对应的芯片选择号和通道位，
 *       最后更新对应DO控制寄存器的值。
 *       特殊处理RELAY8-RELAY11通道的OFF状态。
 ******************************************************************************************************/
void BSP_DO_Ctrl(DoChannel_TypeDef iDONo, DoStatus_TypeDef u8DoStatus)
{
	return;
	unsigned char u8CSNO, u8ChannelNo;

	if ((iDONo == RELAY10) ||
		 (iDONo == RELAY8)	||
		 (iDONo == RELAY11) ||
		 (iDONo == RELAY9))
	{
		if (u8DoStatus == DO_OFF)
		{
			u8DoStatus = DO_OFF;
		}
	}

	if(u8DoStatus > DO_ON)
	{
		return;
	}
	else
	{
		u8CSNO = iDONo/8;
		if(u8CSNO >= DO_CS_NUM_MAX)
		{
			return;
		}
		u8ChannelNo = iDONo%8;

		MutexLock(gEvChargeInfo.hMetex_DoCtrl);
		
		gs_DoCtrlMng.u8DoCtrlStat[u8CSNO] &=(~((uint8_t)0x01<<u8ChannelNo));
		gs_DoCtrlMng.u8DoCtrlStat[u8CSNO] |= (u8DoStatus <<u8ChannelNo);

		switch(u8CSNO)
		{
			case 0:
				CSDO1 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			case 1:
				CSDO2 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			case 2:
				CSDO3 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			case 3:
				CSDO4 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			case 4:
				CSDO5 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			default:
				break;
		}

		
		MutexUnlock(gEvChargeInfo.hMetex_DoCtrl);
	}
}
/*********************************************************************************************************
 * @brief 控制数字输出通道状态
 *
 * @param iDONo 数字输出通道号
 * @param u8DoStatus 输出状态(DO_ON/DO_OFF)
 *
 * @note 该函数会处理特定继电器通道(RELAY8-RELAY11)的OFF状态，
 *       并通过互斥锁保护对全局DO控制状态的修改，最后更新对应的CSDO寄存器。
 *       如果通道号或状态无效，则直接返回。
 **********************************************************************************************************/
void ESPCU_BSP_DO_Ctrl(DoChannel_TypeDef iDONo, DoStatus_TypeDef u8DoStatus)
{

	unsigned char u8CSNO, u8ChannelNo;

	if ((iDONo == RELAY10) ||
		 (iDONo == RELAY8)	||
		 (iDONo == RELAY11) ||
		 (iDONo == RELAY9))
	{
		if (u8DoStatus == DO_OFF)
		{
			u8DoStatus = DO_OFF;
		}
	}

	if(u8DoStatus > DO_ON)
	{
		return;
	}
	else
	{
		u8CSNO = iDONo/8;
		if(u8CSNO >= DO_CS_NUM_MAX)
		{
			return;
		}
		u8ChannelNo = iDONo%8;

		MutexLock(gEvChargeInfo.hMetex_DoCtrl);
		
		gs_DoCtrlMng.u8DoCtrlStat[u8CSNO] &=(~((uint8_t)0x01<<u8ChannelNo));
		gs_DoCtrlMng.u8DoCtrlStat[u8CSNO] |= (u8DoStatus <<u8ChannelNo);

		switch(u8CSNO)
		{
			case 0:
				CSDO1 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			case 1:
				CSDO2 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			case 2:
				CSDO3 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			case 3:
				CSDO4 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			case 4:
				CSDO5 = gs_DoCtrlMng.u8DoCtrlStat[u8CSNO];
				break;
			default:
				break;
		}

		
		MutexUnlock(gEvChargeInfo.hMetex_DoCtrl);
              
	}
}





//void BSP_InitGpioF(void)
//{
//	GPIO_InitStruct.Pin = 0;			//0x2048
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	//0x01;
//	GPIO_InitStruct.Speed = 0x02;
	/***********************************************************
	PF	0x0400
		GPIO_Pin_10                ((uint16_t)0x0400) UART6-CTL	
    ***********************************************************/
//	GPIO_InitStruct.Pin = GPIO_Pin_10;
//	//TXY TEST 
//	GPIO_InitStruct.Pin |= GPIO_Pin_8;		//CPWM2
//	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
//}



/*********************************************************************************************************
 * @brief 初始化系统时钟和外设时钟
 *
 * 该函数用于使能所有使用到的外设时钟，包括：
 * - USART1/2/3/6
 * - ADC1/3
 * - TIM2/3/5/7/8/9/10/11/13/14
 * - CAN1/2
 * - I2C1/3
 * - GPIOA-G
 * - DMA1/2
 * - ETH相关时钟
 * - FSMC
 * - UART4/5
 * - SPI3
 *
 * 函数通过调用预定义的宏来设置RCC寄存器，使能各外设时钟。
 **********************************************************************************************************/
void BSP_vCLOCKInit(void)
{
	__USART1_CLK_ENABLE();

	__USART2_CLK_ENABLE();

	__USART3_CLK_ENABLE();
	//TXY
	//RCC->APB2ENR |= 0x00000020;	
	__USART6_CLK_ENABLE();

	//RCC->APB2ENR |= 0x00000100;	
	__ADC1_CLK_ENABLE();

	__ADC3_CLK_ENABLE();


	__TIM8_CLK_ENABLE();

	__TIM7_CLK_ENABLE();

	//RCC->APB1ENR |= 0x00000002;	
	__TIM3_CLK_ENABLE();

	__TIM5_CLK_ENABLE();

	//RCC->APB1ENR |= 0x00000080;	
	__TIM13_CLK_ENABLE();

	//RCC->APB1ENR |= 0x00000001;
	__TIM2_CLK_ENABLE();

	//RCC->APB2ENR |= 0x00010000;
	__TIM9_CLK_ENABLE();

	//RCC->APB2ENR |= 0x00020000;
	__TIM10_CLK_ENABLE();

	//RCC->APB2ENR |= 0x00040000;
	__TIM11_CLK_ENABLE();

	//RCC->APB1ENR |= 0x00000100;
	__TIM14_CLK_ENABLE();

	//RCC->APB1ENR |= 0x02000000;
	__CAN1_CLK_ENABLE();

	//TXY 
	__CAN2_CLK_ENABLE();		//(RCC->APB1ENR |= (RCC_APB1ENR_CAN2EN))

	//RCC->APB1ENR |= 0x00200000;
	__I2C1_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x00000001;
	__GPIOA_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x00000002;
	__GPIOB_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x00000004;
	__GPIOC_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x00000008;
	__GPIOD_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x00000010;
	__GPIOE_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x00000020;
	__GPIOF_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x00000040;
	__GPIOG_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x00400000;
	__DMA2_CLK_ENABLE();

	__DMA1_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x02000000;
	__ETHMACEN_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x04000000;
	__ETHMACTXEN_CLK_ENABLE();

	//RCC->AHB1ENR |= 0x08000000;
	__ETHMACRXEN_CLK_ENABLE();

	//RCC->AHB3ENR |= 0x01;
	__FSMC_CLK_ENABLE();//Note #define STM32F407xx	1

	//RCC->APB1ENR |= 0x00080000;
	__UART4E_CLK_ENABLE();

	//RCC->APB1ENR |= 0x00100000;
	__UART5E_CLK_ENABLE();

	//RCC->APB1ENR |= 0x00800000;
	__I2C3_CLK_ENABLE();

	__SPI3_CLK_ENABLE();
}

/*********************************************************************************************************
 * @brief  初始化外部中断1 (PB1引脚)
 * @note   配置PB1为下降沿触发中断，无上拉/下拉，优先级为2:1
 *         并使能EXTI1中断
 *********************************************************************************************************/
void BSP_vGpioEXIT1Init(void)
{
	unsigned long   PinVal;
	PinVal = GPIO_Pin_1;	
	__GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = PinVal;
	//PB1
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;				//下降沿触发    因为有数据了  MCP2515是给了一个低电平
	GPIO_InitStruct.Pull = GPIO_NOPULL;							//
	GPIO_InitStruct.Speed = 0x02;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 1);			//ISR_CIM_EXTI1
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/*********************************************************************************************************
 * @brief 初始化GPIO外部中断5
 *
 * 配置GPIOE的Pin5为下降沿触发的外部中断，并设置中断优先级为2。
 * 使用HAL库函数进行GPIO初始化和NVIC配置。
 *
 * @note 中断服务函数需另行实现
 *********************************************************************************************************/
void BSP_vGpioEXIT5_Init(void)
{
	unsigned long PinVal;
	PinVal = GPIO_Pin_5;	
	__GPIOE_CLK_ENABLE();
	GPIO_InitStruct.Pin = PinVal;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;				
	GPIO_InitStruct.Pull = GPIO_NOPULL;	
	GPIO_InitStruct.Speed = 0x02;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 2);					//ISR_CIM_EXTI9_5
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/*********************************************************************************************************
 * @brief  GPIO外部中断5回调函数
 * @param  GPIO_Pin 触发中断的GPIO引脚号
 * @note   此函数为空实现，需根据实际需求添加中断处理逻辑
 *********************************************************************************************************/
void HAL_GPIO_EXTI_5_Callback(uint16_t GPIO_Pin)
{
	return;
}

/*********************************************************************************************************
 * @brief 重新初始化GPIO配置
 * 
 * 该函数用于重新配置多个GPIO引脚的工作模式：
 * 1. 配置GPIOC的Pin8为推挽输出模式
 * 2. 配置GPIOF的Pin11和Pin9为输入模式
 * 3. 配置GPIOD的Pin13为输入模式
 * 4. 配置GPIOA的Pin8为输入模式
 * 
 * 注意：函数会修改GPIO_InitStruct结构体中的Mode、Pull、Speed和Pin参数
*********************************************************************************************************/
void BSP_vGpioReInit(void)
{
	/***************************************************************************************************
				OUTPUT
	***************************************************************************************************/
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;								//0x01;
	GPIO_InitStruct.Pull = 0x01;											//
	GPIO_InitStruct.Speed = 0x02;
	//GPIO_InitStruct.Pin = GPIO_Pin_8 | GPIO_Pin_2;	
	GPIO_InitStruct.Pin = GPIO_Pin_8;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);									//PC2

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;									//0x00;
	GPIO_InitStruct.Pull = 0x00;
	GPIO_InitStruct.Speed = 0x02;											//	
	GPIO_InitStruct.Pin = GPIO_Pin_11 | GPIO_Pin_9;			
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_Pin_13;  
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_Pin_8;  
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);   
}

/********************************************************************************************************************
 * @brief  GPIO初始化函数
 * 
 * 该函数用于初始化所有GPIO引脚，包括输入和输出配置。函数按照不同的GPIO组（GPIOA-GPIOG）
 * 分别进行初始化，设置引脚模式（输出/输入）、上下拉电阻、速度等参数。
 * 
 * 输出引脚配置为推挽输出模式，输入引脚配置为浮空输入模式。
 * 每个GPIO组的初始化都包含详细的引脚功能注释，说明每个引脚的具体用途。
 * 
 * @note 该函数通常在系统初始化阶段调用，用于配置所有GPIO外设。
 *********************************************************************************************************************/
void BSP_vGpioInit(void)
{
	unsigned long   PinVal;
	/***********************************************************
	PD	0x2048
		GPIO_Pin_3	((uint16_t)0x0008)	  UART4 

		GPIO_Pin_13  SPI2-NSS  
		GPIO_Pin_6  i2c3-wp
	***********************************************************/
	PinVal = 0;
	PinVal = GPIO_Pin_3 | GPIO_Pin_13| GPIO_Pin_6;
	GPIO_InitStruct.Pin = PinVal;			//0x2048
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	//
	GPIO_InitStruct.Pull = 0x01;			//	
	GPIO_InitStruct.Speed = 0x02;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


	/***********************************************************
	PG	0x11c0
		GPIO_Pin_7                 ((uint16_t)0x0080)  BUZZER
		GPIO_Pin_6                 ((uint16_t)0x0040)  LED_CTL

		GPIO_Pin_8                 UART5-CTRL
		GPIO_Pin_12					flash_wp  (SPI2_WP)
	***********************************************************/
	PinVal = 0;
	PinVal = GPIO_Pin_12 | GPIO_Pin_7 |GPIO_Pin_8;
	GPIO_InitStruct.Pin =PinVal;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	PinVal = 0;
	PinVal = GPIO_Pin_6;
	GPIO_InitStruct.Pin = PinVal; 		//0x2048
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //
	GPIO_InitStruct.Pull = 0x01;			//	
	GPIO_InitStruct.Speed = 0x02;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/***********************************************************
	PE	0x18
		             
		GPIO_Pin_2		SPI2-NSS			
	
	***********************************************************/
	PinVal = 0;
	PinVal =  GPIO_Pin_2;
	GPIO_InitStruct.Pin = PinVal; 
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct); //U1-ALM-LED
	/***********************************************************
	PF	0x0400
	GPIO_Pin_8		 CPWM2
	
	GPIO_Pin_11   RS485-3 CTRL
	GPIO_Pin_6	
	***********************************************************/
	PinVal = 0;
	PinVal =   GPIO_Pin_8 |GPIO_Pin_11|GPIO_Pin_6;
	GPIO_InitStruct.Pin = PinVal;	
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	/***********************************************************
	PC	0x04
	
		GPIO_Pin_8					CPWM1

		GPIO_Pin_14					can3-reset
	***********************************************************/
	PinVal = 0;
	PinVal =   GPIO_Pin_8 | GPIO_Pin_14;	
	GPIO_InitStruct.Pin = PinVal;	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/***********************************************************
	PB	0x04
		GPIO_Pin_11				uart3-RX (RS485-3)
		GPIO_Pin_10				uart3-TX

		GPIO_Pin_2                 BOOT1				
		PWM1 PWM2	GPIO_Pin_12					CAN2-RX
	***********************************************************/
	GPIO_InitStruct.Pin = GPIO_Pin_2;	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/***********************************************************
	PA
		GPIO_Pin_15      SPI3_NSS!!         
	
	***********************************************************/
	//PA15   SPI3_NSS!!
	GPIO_InitStruct.Pin = GPIO_Pin_15;  
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);   
/***********************************END		OUTPUT	***************************************************/


/***********************************START	INPUT	**************************************************
************************************START	INPUT	**************************************************
**********************************************************************************************************/

	/***********************************************************
	PE	0x04
		GPIO_Pin_6                 SW1
		GPIO_Pin_5					f_cp
		GPIO_Pin_4					ver1
		GPIO_Pin_3					ver0	

		
	***********************************************************/
	GPIO_InitStruct.Pin = GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 ;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;	//0x00;
	GPIO_InitStruct.Pull = 0x00;
	GPIO_InitStruct.Speed = 0x02;//50M
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/***********************************************************
	PB	
		GPIO_Pin_10/11				(RS485-3)

		GPIO_Pin_7				SW4
		GPIO_Pin_0				ad_curr
		GPIO_Pin_1				SPI3_INT
		GPIO_Pin_2                 BOOT1
	***********************************************************/
	PinVal = 0;
	PinVal =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_7;
	GPIO_InitStruct.Pin = PinVal;  
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/***********************************************************
	PG	0x8400

		
		GPIO_Pin_15		SW3
		GPIO_Pin_10		((uint16_t)0x0400) PFO
	***********************************************************/
	PinVal = 0;
	PinVal = GPIO_Pin_15 | GPIO_Pin_10 ;
	GPIO_InitStruct.Pin = PinVal;  
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/***********************************************************
	PF	0x800
		
		GPIO_Pin_9           ACGUN IN
		GPIO_Pin_7			AD_ALL

		GPIO_Pin_10			CONPW1_AD
	***********************************************************/
	GPIO_InitStruct.Pin = GPIO_Pin_10|GPIO_Pin_7|GPIO_Pin_9;			
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/***********************************************************
	PC	0x2000
		GPIO_Pin_13               SW2
		GPIO_Pin_9				 - IMSU-X I2C3 - SDA
		GPIO_Pin_14					can3-reset
		GPIO_Pin_15					DI_PE
		

		GPIO_Pin_0  AC_CC_AD
		GPIO_Pin_2  BMSPW1-AD
		GPIO_Pin_3   BMSPW2-AD
	***********************************************************/
	GPIO_InitStruct.Pin = GPIO_Pin_13 | GPIO_Pin_15|GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3;	 //PC 0
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);   

	/***********************************************************
	PD	
		GPIO_Pin_12               pcb_c_di
		

	***********************************************************/
	GPIO_InitStruct.Pin = GPIO_Pin_12;  
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/***********************************************************
	PA	
		GPIO_Pin_8                 - IMSU-X I2C3 - CLKD
		GPIO_Pin_5  f_voltCA
		GPIO_Pin_6  ChadeMo_AD
		GPIO_Pin_4  GUN2
		GPIO_Pin_3  GUN1
		GPIO_Pin_0  f_voltCB
	***********************************************************/
	GPIO_InitStruct.Pin = GPIO_Pin_5|GPIO_Pin_6| GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_0; 
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*****************************************************************************************************************
 * @brief  获取PFO状态
 * @retval GPIO_PinState 返回PFO引脚的电平状态
 ****************************************************************************************************************/
GPIO_PinState BSP_u8GetPFOStatus(void)
{
	return HAL_GPIO_ReadPin(GPIOG, 0x0400);
}

/****************************************************************************************************************
 * @brief  翻转心跳LED状态
 * @note   该函数通过调用HAL库函数翻转GPIOG的第6引脚状态
****************************************************************************************************************/
void BSP_u1_heartbeat_led_toggle(void)
{
	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_6);//0x40
}
/****************************************************************************************************************
 * @brief  切换风扇控制引脚状态
 * @note   该函数通过翻转GPIOB的PIN2引脚电平来控制风扇启停
****************************************************************************************************************/
void BSP_vFeedFan(void)
{
	HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_2);//0x04
}
/****************************************************************************************************************
 * @brief 打开蜂鸣器
 * 
 * 该函数通过设置GPIOG的第7引脚为高电平来打开蜂鸣器。
****************************************************************************************************************/
void BSP_vBuzzer_On(void)
{
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_SET);//0x80
}
/****************************************************************************************************************
 * @brief 关闭蜂鸣器
 * 
 * 该函数通过将GPIOG的第7引脚设置为低电平来关闭蜂鸣器。
****************************************************************************************************************/
void BSP_vBuzzer_Off(void)
{
	HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_RESET);//0x80
}

/****************************************************************************************************************
 * @brief 关闭运行状态LED灯
 *
 * 该函数通过控制GPIOG的第6引脚输出低电平来关闭运行状态指示灯。
 ****************************************************************************************************************/
void BSP_Run_LED_OFF(void)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
}
/****************************************************************************************************************
 * @brief  点亮运行状态LED灯
 * @note   通过设置GPIOG的第6引脚为高电平来点亮LED
 ****************************************************************************************************************/
void BSP_Run_LED_ON(void)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
}
/****************************************************************************************************************
 * @brief 关闭报警LED
 *
 * 该函数用于控制GPIOE的PIN2引脚输出低电平，从而关闭报警LED。
 ****************************************************************************************************************/
void BSP_Alm_LED_OFF(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);

}
/****************************************************************************************************************
 * @brief 打开报警LED
 * 
 * 该函数通过设置GPIOF的PIN6为高电平来点亮报警LED。
 ****************************************************************************************************************/
void BSP_Alm_LED_ON(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);

}
/****************************************************************************************************************
 * @brief 根据端口号和引脚号设置GPIO输出值
 * @param iPort 端口号 (0:GPIOB, 1:GPIOF, 2:GPIOC)
 * @param pinNo 引脚号 (未使用，保留参数)
 * @param val 要设置的值 (0:低电平，1:高电平)
 ****************************************************************************************************************/
void TESTXXXXXXX(int iPort, int pinNo, int val)
{
	if (0 == iPort)
	{
		 	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,(GPIO_PinState)val);
		//HAL_GPIO_WritePin(GPIOF,GPIO_PIN_6,(GPIO_PinState)val);                 //by lxr01202
	}

	if (1 == iPort)
	{
		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_8,(GPIO_PinState)val);
	}
	if (2 == iPort)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,(GPIO_PinState)val);
	}
}





/****************************************************************************************************************
 * @brief  获取Ver0引脚状态
 * @retval GPIO_PinState 返回GPIOE_PIN3的当前状态
 ****************************************************************************************************************/
GPIO_PinState BSP_u8GetVer0(void)
{
	return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
}




/****************************************************************************************************************
 * @brief  获取版本1信号状态
 * @retval GPIO_PinState 返回GPIOE_PIN4的引脚状态
 ****************************************************************************************************************/
GPIO_PinState BSP_u8GetVer1(void)
{
	return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
}



/****************************************************************************************************************
 * @brief  获取SW1开关状态
 * @retval GPIO_PinState 返回GPIO引脚状态(GPIO_PIN_SET或GPIO_PIN_RESET)
 ****************************************************************************************************************/
GPIO_PinState BSP_u8GetSW1Status(void)
{
	return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
}


/****************************************************************************************************************
 * @brief  获取SW2开关状态
 * @retval GPIO_PinState 返回SW2对应的GPIO引脚状态
 ****************************************************************************************************************/
GPIO_PinState BSP_u8GetSW2Status(void)
{
	return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);//0x0800
}



/****************************************************************************************************************
 * @brief  获取SW3开关状态
 * @retval GPIO_PinState 返回SW3的GPIO引脚状态
 ****************************************************************************************************************/
GPIO_PinState BSP_u8GetSW3Status(void)
{
	return HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_15); // 0x2000
}



/****************************************************************************************************************
 * @brief  获取SW4开关状态
 * @retval GPIO_PinState 返回SW4的GPIO引脚状态（GPIO_PIN_SET/GPIO_PIN_RESET）
 ****************************************************************************************************************/
GPIO_PinState BSP_u8GetSW4Status(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
}



/****************************************************************************************************************
 * @brief 获取CSDI1状态
 *
 * 该函数用于读取CSDI1的当前状态值。
 *
 * @return unsigned char 返回CSDI1的当前状态值
 ****************************************************************************************************************/
unsigned char BSP_u8GetCSDI1Status(void)
{
	unsigned char ucDIValue;
	ucDIValue = CSDI1;
	return ucDIValue;
}



/****************************************************************************************************************
 * @brief 获取CSDI2状态值
 * 
 * 该函数用于读取CSDI2的当前状态值并返回。
 * 
 * @return unsigned char 返回CSDI2的当前状态值
 ****************************************************************************************************************/
unsigned char BSP_u8GetCSDI2Status(void)
{
	unsigned char ucDIValue;
	ucDIValue = CSDI2;
	return ucDIValue;
}
void BSP_vFeedDogLed(void)
{
	HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_6);//0x40
}

GPIO_PinState BSP_u8GetDI1Status(void)
{
	return HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_11);//0x0800
}

GPIO_PinState BSP_u8GetDI2Status(void)
{
	return HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13);//0x2000
}

GPIO_PinState BSP_u8GetDI3Status(void)
{
	if(u8Is1000VHardwareA00())
	{
		return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6);//A04  从PF9 变为 PD6  
	}
	else
	{
		return HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_9);//0x2000
	}
}

GPIO_PinState BSP_u8GetDI4Status(void)
{
	return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);//0x2000
}

GPIO_PinState BSP_u8GetDI5Status(void)
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);//0x2000
}

GPIO_PinState BSP_u8GetDI6Status(void)
{
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);//0x2000
}

GPIO_PinState BSP_u8GetDI7Status(void)
{
	return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13);//0x2000
}

GPIO_PinState BSP_u8GetDI8Status(void)
{
	return HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_12);//0x2000
}

GPIO_PinState BSP_u8GetDI9Status(void)
{
	return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15);//0x2000
}

GPIO_PinState BSP_u8GetDI10Status(void)
{
	return HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0);//0x2000
}
#if 0
GPIO_PinState BSP_u8GetFAN_ALM1Status(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
}

GPIO_PinState BSP_u8GetFAN_ALM2Status(void)
{
	return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
}

GPIO_PinState BSP_u8GetFAN_ALM3Status(void)
{
	return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
}

GPIO_PinState BSP_u8GetFAN_ALM4Status(void)
{
	return HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6);
}

GPIO_PinState BSP_u8GetFAN_ALM5Status(void)
{
	return HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7);
}

GPIO_PinState BSP_u8GetFAN_ALM6Status(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
}

GPIO_PinState BSP_u8GetSSD5Status(void)
{
	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
}

GPIO_PinState BSP_u8GetSD55Status(void)
{
	return HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
}

#endif

/****************************************************************************************************************
 * @brief 初始化并启动独立看门狗(IWDG)
 * @note  配置看门狗预分频为32，重载值为0xFFF
 *        调用HAL库函数完成初始化和启动操作
 ****************************************************************************************************************/
void BSP_WDGInit(void)
{
	IwdgHandle.Instance = IWDG;
	IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;
	IwdgHandle.Init.Reload    = 0xFFF;//1600;
	HAL_IWDG_Init(&IwdgHandle);
	HAL_IWDG_Start(&IwdgHandle);
}


/****************************************************************************************************************

 * @brief 看门狗定时器刷新函数

 * @details 该函数用于刷新看门狗定时器，防止系统看门狗复位

 ****************************************************************************************************************/
void BSP_WDGRefresh(void)
{
	HAL_IWDG_Refresh(&IwdgHandle);
}
