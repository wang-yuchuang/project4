#include "main.h"
#include "Uart6_new.h"
#include "EtherNet.h"
#include "HistoryEvent.h"

#define TOPWAY_SOI_VAL			0xAA
#define TOPWAY_EOI_VAL1			0xCC
#define TOPWAY_EOI_VAL2			0x33
#define TOPWAY_EOI_VAL3			0xC3
#define TOPWAY_EOI_VAL4			0x3C

#define TOPWAY_CMD_PST			1

UINT8 gUart6ArryIdx = 0;
HMI_INFO gHMIData;

extern UINT8 u8A_SpearLOCKDeal(UINT8 u8IsLock);


//gs_DisplayCtrl.u8ChkPageFlag
enum PAGE_CHK_FLAG_Def
{
	PAGE_CHK_FLAG_Ok = 0,		//已发送查询命令且收到回复，屏的页面已更新OK
	PAGE_CHK_FLAG_NeedChk,		//需要发送查询页面命令
	PAGE_CHK_FLAG_SentCmd,		//已发送查询命令
};


/************************************************************************************************/
//屏显示内容定义
/************************************************************************************************/
//每屏各信息段刷新标志位定义
enum ScreemSigReflashFlagDef
{
	SCREEM_SIG_ALL_OFF = 0x0000,
	SCREEM_SIG_STR1_ON = 0x0001,		//1-表示需要刷新字串1
	SCREEM_SIG_STR2_ON = 0x0002,		//1-表示需要刷新字串2
	SCREEM_SIG_STR3_ON = 0x0004,		//1-表示需要刷新字串3
	SCREEM_SIG_STR4_ON = 0x0008,		//1-表示需要刷新字串4
	SCREEM_SIG_STR5_ON = 0x0010,		//1-表示需要刷新字串5
	SCREEM_SIG_STR6_ON = 0x0020,		//1-表示需要刷新字串6
	SCREEM_SIG_16BIT1_ON = 0x0040,		//1-表示需要刷新第一个16位信号段
	SCREEM_SIG_16BIT2_ON = 0x0080,		//1-表示需要刷新第二个16位信号段
	SCREEM_SIG_32BIT_ON = 0x0100,		//1-表示需要刷新32位信号段

	SCREEM_SIG_ALL_ON = 0x01FF,
};

typedef struct TopwayScreem_
{
	unsigned char u8_PageNo;
	unsigned char u8_IsHighPriority;		//是否高优先级页面 0-不是  1-是; 例如维护界面(参数设置、状态查询等) 不能被主机自动跳走，而需要人为触摸按键退出  
	unsigned short u16_ReflashFlag;			//刷新标志   enum ScreemSigReflashFlagDef	
	unsigned char u8_StrSigNumber;			//字符串信号个数
	unsigned long u32_StrSigAddr[6];		//支持6个字符串
	unsigned char u8_16bitSigSeg1Number;	//16位信号段1个数
	unsigned long u32_16bitSigSeg1Addr;	    //16位信号段1起始地址
	unsigned char u8_16bitSigSeg2Number;	//16位信号段2个数
	unsigned long u32_16bitSigSeg2Addr;	    //16位信号段2起始地址
	unsigned char u8_32bitSigNumber;		//32位信号个数
	unsigned long u32_32bitSigAddr;			//32位信号起始地址
	
}TopwayScreem_Def;

const TopwayScreem_Def TopwayScreem[] =
{
	//双枪待机
	{	TOPWAY_SCREEM_PAGE_INIT,    
		0,							
		SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_STR3_ON|SCREEM_SIG_STR4_ON|SCREEM_SIG_STR5_ON|SCREEM_SIG_STR6_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON, 
		6, TOPWAY_STR_ADDR_GUN_A_QR_CODE, TOPWAY_STR_ADDR_GUN_B_QR_CODE, 
			TOPWAY_STR_ADDR_GUN_A_QR_CODE_2,TOPWAY_STR_ADDR_GUN_A_RESERVE_ID,
			TOPWAY_STR_ADDR_GUN_B_QR_CODE_2,TOPWAY_STR_ADDR_GUN_B_RESERVE_ID,			
		14, TOPWAY_16BIT_ADDR_START_GUN_A_STAT, 14, TOPWAY_16BIT_ADDR_START_GUN_B_STAT, 
		0, 0},
	//双枪充电中
	{	TOPWAY_SCREEM_PAGE_BOTH_CHG, 
		0, 
		SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		2, TOPWAY_STR_ADDR_GUN_A_CARD_ID, TOPWAY_STR_ADDR_GUN_B_CARD_ID, 0,0,0,0, 
		14, TOPWAY_16BIT_ADDR_CHG_GUN_A_CHG_MODE, 14, TOPWAY_16BIT_ADDR_CHG_GUN_B_CHG_MODE, 
		0, 0}, 
	//双枪结算中
	{	TOPWAY_SCREEM_PAGE_BOTH_STOP, 
		0, 
		SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		0, 0, 0, 0,0,0,0, 
		7, TOPWAY_16BIT_ADDR_STOP_GUN_A_MONEY, 7, TOPWAY_16BIT_ADDR_STOP_GUN_B_MONEY, 
		0, 0}, 
	//A待机B充电
	{	TOPWAY_SCREEM_PAGE_A_IDLE_B_CHG, 
		0, 
		SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_STR3_ON|SCREEM_SIG_STR4_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		4, TOPWAY_STR_ADDR_GUN_A_QR_CODE,  TOPWAY_STR_ADDR_GUN_B_CARD_ID, 
		   TOPWAY_STR_ADDR_GUN_A_QR_CODE_2,TOPWAY_STR_ADDR_GUN_A_RESERVE_ID,0,0,
		14, TOPWAY_16BIT_ADDR_START_GUN_A_STAT, 14, TOPWAY_16BIT_ADDR_CHG_GUN_B_CHG_MODE, 
		0, 0}, 
	//A充电B待机
	{	TOPWAY_SCREEM_PAGE_A_CHG_B_IDLE, 
		0,
		SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON| SCREEM_SIG_STR3_ON|SCREEM_SIG_STR4_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		4, TOPWAY_STR_ADDR_GUN_A_CARD_ID,  TOPWAY_STR_ADDR_GUN_B_QR_CODE, 
			TOPWAY_STR_ADDR_GUN_B_QR_CODE_2,TOPWAY_STR_ADDR_GUN_B_RESERVE_ID,0,0,  	
		14, TOPWAY_16BIT_ADDR_CHG_GUN_A_CHG_MODE, 14, TOPWAY_16BIT_ADDR_START_GUN_B_STAT, 
		0, 0}, 
	//A待机B结算
	{	TOPWAY_SCREEM_PAGE_A_IDLE_B_STOP, 
		0, 
		SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_STR3_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		3, TOPWAY_STR_ADDR_GUN_A_QR_CODE,  TOPWAY_STR_ADDR_GUN_A_QR_CODE_2,TOPWAY_STR_ADDR_GUN_A_RESERVE_ID,0,0, 0,
		14, TOPWAY_16BIT_ADDR_START_GUN_A_STAT, 7, TOPWAY_16BIT_ADDR_STOP_GUN_B_MONEY, 
		0, 0}, 
	//A结算B待机
	{	TOPWAY_SCREEM_PAGE_A_STOP_B_IDLE, 
		0, 
		SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_STR3_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		3, TOPWAY_STR_ADDR_GUN_B_QR_CODE,  TOPWAY_STR_ADDR_GUN_B_QR_CODE_2,TOPWAY_STR_ADDR_GUN_B_RESERVE_ID,0,0, 0,
		7, TOPWAY_16BIT_ADDR_STOP_GUN_A_MONEY, 14, TOPWAY_16BIT_ADDR_START_GUN_B_STAT, 
		0, 0}, 
	//A结算B充电
	{	TOPWAY_SCREEM_PAGE_A_STOP_B_CHG, 
		0, 
		SCREEM_SIG_STR1_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		1, TOPWAY_STR_ADDR_GUN_B_CARD_ID, 0, 0,0,0,0, 
		7, TOPWAY_16BIT_ADDR_STOP_GUN_A_MONEY, 14, TOPWAY_16BIT_ADDR_CHG_GUN_B_CHG_MODE, 
		0, 0}, 
	//A充电B结算
	{	TOPWAY_SCREEM_PAGE_A_CHG_B_STOP, 
		0, 
		SCREEM_SIG_STR1_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		1, TOPWAY_STR_ADDR_GUN_A_CARD_ID,  0, 0,0,0,0,  
		14, TOPWAY_16BIT_ADDR_CHG_GUN_A_CHG_MODE, 7, TOPWAY_16BIT_ADDR_STOP_GUN_B_MONEY, 
		0, 0}, 

	//刷卡连接后台验证界面
	{	TOPWAY_SCREEM_PAGE_BRUSH_CONFIRM_NET, 
		0, 
		SCREEM_SIG_STR1_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON|SCREEM_SIG_32BIT_ON,
		1, TOPWAY_STR_ADDR_CARD_CERTIFY_ID, 0, 0,0,0,0,  
		2, TOPWAY_16BIT_ADDR_CARD_CERTIFY_STAT, 1, TOPWAY_16BIT_ADDR_GUN_SELECT, 
		1, TOPWAY_32BIT_ADDR_CARD_LEAVE_MONEY_FROM_HOST}, 

		//屏保
	{	TOPWAY_SCREEM_PAGE_SCREEM_SAVER, 
		1, 
		SCREEM_SIG_ALL_OFF,
		0, 0,  0, 0,0,0,0, 
		0, 0, 0, 0, 
		0, 0}, 

	//提示插入枪
	{TOPWAY_SCREEM_PAGE_NOTICE_PLUGIN_SPEAR, 0, SCREEM_SIG_ALL_OFF,
		0, 0,  0, 0,0,0,0, 
		0, 0, 0, 0, 
		0, 0}, 

	//密码输入
	{TOPWAY_SCREEM_PAGE_PASSWORD_INPUT, 1, SCREEM_SIG_STR1_ON | SCREEM_SIG_16BIT1_ON,
		1, TOPWAY_STR_ADDR_PASSWORD, 0, 0,0,0,0, 
		2, TOPWAY_16BIT_ADDR_PASSWORD_INPUT_USER_LEVEL, 0, 0, 
		0, 0}, 

	//A枪详细信息
	{TOPWAY_SCREEM_PAGE_DETAILS_A1, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		17, TOPWAY_16BIT_ADDR_DETAILS_GUN_A_RECT_NUMBER, 0, 0, 
		1, TOPWAY_32BIT_ADDR_GUN_A_METER_VALUE}, 
	//A枪详细信息2
	{TOPWAY_SCREEM_PAGE_DETAILS_A2, 1, SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		2, TOPWAY_STR_ADDR_GUN_A_BSM_BYTE6_7, TOPWAY_STR_ADDR_GUN_A_BST_BYTES, 0,0,0,0, 
		12, TOPWAY_16BIT_ADDR_CHG_GUN_A_CHG_MODE, 13, TOPWAY_16BIT_ADDR_DETAILS_GUN_A_RES_P2PE, 
		0, 0}, 
	//B枪详细信息
	{TOPWAY_SCREEM_PAGE_DETAILS_B1, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		17, TOPWAY_16BIT_ADDR_DETAILS_GUN_B_RECT_NUMBER, 0, 0, 
		1, TOPWAY_32BIT_ADDR_GUN_B_METER_VALUE}, 
	//B枪详细信息2
	{TOPWAY_SCREEM_PAGE_DETAILS_B2, 1, SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		2, TOPWAY_STR_ADDR_GUN_B_BSM_BYTE6_7, TOPWAY_STR_ADDR_GUN_B_BST_BYTES, 0,0,0,0, 
		12, TOPWAY_16BIT_ADDR_CHG_GUN_B_CHG_MODE, 13, TOPWAY_16BIT_ADDR_DETAILS_GUN_B_RES_P2PE, 
		0, 0}, 	
	//环网信息1
	{TOPWAY_SCREEM_PAGE_DETAILS_RING1, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		22, TOPWAY_16BIT_ADDR_RING_OnlineTotal, 0, 0, 
		0, 0}, 
	//环网信息2
	{TOPWAY_SCREEM_PAGE_DETAILS_RING2, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		64, TOPWAY_16BIT_ADDR_RING_Node1_LockStat, 0, 0, 
		0, 0}, 
	//环网信息3
	{TOPWAY_SCREEM_PAGE_DETAILS_RING3, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		64, TOPWAY_16BIT_ADDR_RING_Node9_LockStat, 0, 0, 
		0, 0}, 
	//超时事件
	{TOPWAY_SCREEM_PAGE_DETAILS_OT_EVENT, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		0, 0, 0, 0,0,0,0, 
		9, TOPWAY_16BIT_ADDR_DETAILS_GUN_A_OT_EVENT1, 9, TOPWAY_16BIT_ADDR_DETAILS_GUN_B_OT_EVENT1, 
		0, 0}, 

	//系统告警
	{TOPWAY_SCREEM_PAGE_ALARMS_SYS, 1, SCREEM_SIG_16BIT1_ON,
		0, 0, 0, 0,0,0,0, 
		15, TOPWAY_16BIT_ADDR_ALARM_SYS_AC_HIGH, 0, 0, 
		0, 0}, 
	//A枪告警
	{TOPWAY_SCREEM_PAGE_ALARMS_GUN_A, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		16, TOPWAY_16BIT_ADDR_ALARM_GUN_A_INSUL_FAIL, 0, 0, 
		0, 0}, 
	//B枪告警
	{TOPWAY_SCREEM_PAGE_ALARMS_GUN_B, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		16, TOPWAY_16BIT_ADDR_ALARM_GUN_B_INSUL_FAIL, 0, 0, 
		0, 0}, 

	//系统参数1
	{TOPWAY_SCREEM_PAGE_PARAS_SYS1, 1, SCREEM_SIG_STR1_ON|SCREEM_SIG_16BIT1_ON,
		1, TOPWAY_STR_ADDR_STATION_ID,  0, 0,0,0,0, 
		18, TOPWAY_16BIT_ADDR_PARA_SYS_VOLT_HIGH, 0, 0, 
		0, 0}, 

	//系统参数2  网络参数
	{TOPWAY_SCREEM_PAGE_PARAS_SYS2, 1, SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_STR3_ON|SCREEM_SIG_16BIT1_ON,
		3, TOPWAY_STR_ADDR_RECORD_MAC,  TOPWAY_STR_ADDR_DOMAIN_NAME,  TOPWAY_STR_ADDR_NetFrameHead, 0, 0,0,
		32, TOPWAY_16BIT_ADDR_PARA_SYS_NET_MASK, 0, 0, 
		0, 0}, 

	//A枪参数1
	{TOPWAY_SCREEM_PAGE_PARAS_A1, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		18, TOPWAY_16BIT_ADDR_PARA_GUN_A_ELOCK_EN, 0, 0, 
		0, 0}, 
	//A枪参数2
	{TOPWAY_SCREEM_PAGE_PARAS_A2, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		12, TOPWAY_16BIT_ADDR_PARA_GUN_A_T1, 0, 0, 
		0, 0}, 
	//B枪参数1
	{TOPWAY_SCREEM_PAGE_PARAS_B1, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		18, TOPWAY_16BIT_ADDR_PARA_GUN_B_ELOCK_EN, 0, 0, 
		0, 0}, 
	//B枪参数2
	{TOPWAY_SCREEM_PAGE_PARAS_B2, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		12, TOPWAY_16BIT_ADDR_PARA_GUN_B_T1, 0, 0, 
		0, 0}, 

	//费率参数 1
	{TOPWAY_SCREEM_PAGE_PARAS_FEE1, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		2, TOPWAY_16BIT_ADDR_PARA_FEE_MODE, 0, 0, 
		16, TOPWAY_16BIT_ADDR_PARA_SERVICE_PRICE}, 
	//费率参数 2
	{TOPWAY_SCREEM_PAGE_PARAS_FEE2, 1, SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		0, 0, 0, 0, 
		18, TOPWAY_16BIT_ADDR_PARA_TIME15_PRICE}, 
	//费率参数 3
	{TOPWAY_SCREEM_PAGE_PARAS_FEE3, 1, SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		0, 0, 0, 0, 
		16, TOPWAY_16BIT_ADDR_PARA_TIME33_PRICE}, 

	//充电记录
	{TOPWAY_SCREEM_PAGE_RECORD_1, 1, SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		2, TOPWAY_STR_ADDR_RECORD_BILL_ID,  TOPWAY_STR_ADDR_RECORD_CARD_ID,  0,0,0,0, 
		30, TOPWAY_16BIT_ADDR_RECORD_TOTAL_NUMBER, 0, 0, 
		4, TOPWAY_32BIT_ADDR_RECORD_START_METER_VALUE}, 
	
	//测试充电
	/*{TOPWAY_SCREEM_PAGE_TEST, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 
		2, TOPWAY_16BIT_ADDR_GUN_A_TEST_CHG_CMD, 0, 0, 
		0, 0}, */

	//A枪校准
	{TOPWAY_SCREEM_PAGE_ADJUST_A, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		3, TOPWAY_16BIT_ADDR_PARA_ADJUST_MODE, 0, 0, 
		8, TOPWAY_32BIT_ADDR_GUN_A_ADJUST_REQ_VOLT}, 

	//B枪校准
	{TOPWAY_SCREEM_PAGE_ADJUST_B, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		3, TOPWAY_16BIT_ADDR_PARA_ADJUST_MODE, 0, 0, 
		8, TOPWAY_32BIT_ADDR_GUN_B_ADJUST_REQ_VOLT}, 


	//TP1000 校准！！
	{TOPWAY_SCREEM_PAGE_ADJUST_TP1000, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		0, 0, 0, 0, 
		8, TOPWAY_32BIT_ADDR_GUN_AB_TP1000_POINT_1}, 


	//模式选择 自动充满
	{TOPWAY_SCREEM_PAGE_CHGMODE_SEL_AUTO, 0, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		7, TOPWAY_16BIT_ADDR_GUN_SELECT, 0, 0, 
		2, TOPWAY_32BIT_ADDR_SET_CHG_MONEY}, 
	//模式选择 按时间
	{TOPWAY_SCREEM_PAGE_CHGMODE_SEL_TIME, 0, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		7, TOPWAY_16BIT_ADDR_GUN_SELECT, 0, 0, 
		2, TOPWAY_32BIT_ADDR_SET_CHG_MONEY}, 
	//模式选择 按电量
	{TOPWAY_SCREEM_PAGE_CHGMODE_SEL_ENERGY, 0, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		7, TOPWAY_16BIT_ADDR_GUN_SELECT, 0, 0, 
		2, TOPWAY_32BIT_ADDR_SET_CHG_MONEY}, 
	//模式选择 按金额
	{TOPWAY_SCREEM_PAGE_CHGMODE_SEL_MONEY, 0, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		7, TOPWAY_16BIT_ADDR_GUN_SELECT, 0, 0, 
		2, TOPWAY_32BIT_ADDR_SET_CHG_MONEY}, 
	//模式选择 按SOC
	{TOPWAY_SCREEM_PAGE_CHGMODE_SEL_SOC, 0, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		7, TOPWAY_16BIT_ADDR_GUN_SELECT, 0, 0, 
		2, TOPWAY_32BIT_ADDR_SET_CHG_MONEY}, 	

	//关于
	{TOPWAY_SCREEM_PAGE_ABOUT, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_STR1_ON,
		1, TOPWAY_STR_ADDR_HARDWARE_VER,  0, 0,0,0,0, 
		2, TOPWAY_16BIT_ADDR_ABOUT_SOFTWARE_VER, 0, 0, 
		0, 0}, 

	//维护操作
	{TOPWAY_SCREEM_PAGE_PARAS_SYS3, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		1, TOPWAY_16BIT_ADDR_DEBUG_ENERGY_SYS_ConverSource, 0, 0, 
		0, 0}, 
	//系统参数 环网参数
	{TOPWAY_SCREEM_PAGE_PARAS_SYS4, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		11, TOPWAY_16BIT_ADDR_PARA_SYS_RING_EN, 0, 0, 
		0, 0}, 
	//系统参数2
	{TOPWAY_SCREEM_PAGE_PARAS_SYS5, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		18, TOPWAY_16BIT_ADDR_PARA_SYS_IS_SINGLE_GUN_MODE, 0, 0, 
		0, 0}, 

	//模块信息1
	{TOPWAY_SCREEM_PAGE_DETAILS_RECT1, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		68, TOPWAY_16BIT_ADDR_RECT1_Stat, 0, 0, 
		0, 0}, 
	//模块信息2
	{TOPWAY_SCREEM_PAGE_DETAILS_RECT2, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		68, TOPWAY_16BIT_ADDR_RECT9_Stat, 0, 0, //加了模块版本信息
		0, 0}, 
	//密码设置
	{TOPWAY_SCREEM_PAGE_PARAS_PWD_SET, 1, SCREEM_SIG_STR1_ON,
		1, TOPWAY_STR_ADDR_USER3_PASSWORD,  0, 0,0,0,0, 
		0, 0, 0, 0, 
		0, 0}, 
	//重启记录
	{TOPWAY_SCREEM_PAGE_PARAS_RESET_RECORD, 1, SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_STR3_ON|SCREEM_SIG_16BIT1_ON,
		3, TOPWAY_STR_ADDR_RESET_REASON1,  TOPWAY_STR_ADDR_RESET_REASON2, TOPWAY_STR_ADDR_RESET_REASON3,0,0,0, 
		10, TOPWAY_16BIT_ADDR_ABOUT_POWER_TIMES, 0, 0, 
		0, 0}, 
	//调测控制
	{TOPWAY_SCREEM_PAGE_PARAS_DEBUG, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		20, TOPWAY_16BIT_ADDR_DEBUG_ONOFF, 0, 0, 
		0, 0}, 

	//模组信息1
	{TOPWAY_SCREEM_PAGE_DETAILS_RECT_GRP, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		56, TOPWAY_16BIT_ADDR_RECT_GRP1_RatePwr, 0, 0, 
		0, 0}, 
	//储能电池信息1
	{TOPWAY_SCREEM_PAGE_DETAILS_ES_BATT, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON,
		0, 0,  0, 0,0,0,0, 
		16, TOPWAY_16BIT_ADDR_ES_BATT1_RatePwr, 8, TOPWAY_16BIT_ADDR_ES_AC_INPUT_PWR_LMT, 
		0, 0}, 
	//系统参数3
	{TOPWAY_SCREEM_PAGE_PARAS_SYS6, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_32BIT_ON,
		0, 0,  0, 0,0,0,0, 
		24, TOPWAY_16BIT_ADDR_PARA_SYS_Grp1_ConvtrNum, 0, 0, 
		2, TOPWAY_32BIT_ADDR_PARA_EV_DISCHG_STOP_VOLT_COEFF}, 
	//系统参数4
		{TOPWAY_SCREEM_PAGE_PARAS_SYS_PAGE4, 1, SCREEM_SIG_16BIT1_ON|SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON,
		2, TOPWAY_STR_ADDR_ATTRIBITS,  TOPWAY_STR_ADDR_PWM_Paras, 0,0,0,0, 
		24, TOPWAY_16BIT_ADDR_PARA_SYS_IMIU1_MASK, 0, 0, 
		0, 0}, 
	//WIFI设置参数
	/*{TOPWAY_SCREEM_PAGE_PARAS_WIFI, 1, SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_16BIT1_ON,
		2, TOPWAY_STR_ADDR_WIFI_SSID,  TOPWAY_STR_ADDR_WIFI_PSWD, 0,0,0,0, 
		16, TOPWAY_16BIT_ADDR_PARAM3_WIFI_MODE, 0, 0, 
		0, 0}, */
	//调测控制
	{TOPWAY_SCREEM_PAGE_PARAS_DEBUG2, 1, SCREEM_SIG_16BIT1_ON,
		0, 0,  0, 0,0,0,0, 
		32, TOPWAY_16BIT_ADDR_DEBUG_GRP_CTRL_VOLT, 0, 0, 
		0, 0}, 

	

	/***********************************************************************/
	/*单枪	A 界面*/
	/***********************************************************************/
	//单枪待机
	{	TOPWAY_SCREEM_PAGE_INIT_Single,    
		0,							
		SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_STR3_ON|SCREEM_SIG_16BIT1_ON, 
		3, TOPWAY_STR_ADDR_GUN_A_QR_CODE, TOPWAY_STR_ADDR_GUN_A_QR_CODE_2, 
		TOPWAY_STR_ADDR_GUN_A_RESERVE_ID,0,	0,0,			
		14, TOPWAY_16BIT_ADDR_START_GUN_A_STAT, 0, 0, 
		0, 0},
	//单枪充电中
	{	TOPWAY_SCREEM_PAGE_CHG_Single, 
		0, 
		SCREEM_SIG_STR1_ON|SCREEM_SIG_16BIT1_ON,
		1, TOPWAY_STR_ADDR_GUN_A_CARD_ID, 0, 0,0,0,0, 
		14, TOPWAY_16BIT_ADDR_CHG_GUN_A_CHG_MODE, 0, 0, 
		0, 0}, 
	//单枪结算中
	{	TOPWAY_SCREEM_PAGE_STOP_Single, 
		0, 
		SCREEM_SIG_16BIT1_ON,
		0, 0, 0, 0,0,0,0, 
		7, TOPWAY_16BIT_ADDR_STOP_GUN_A_MONEY, 0, 0, 
		0, 0}, 

	/***********************************************************************/
	/*单枪	B 界面*/
	/***********************************************************************/
	//单枪待机
	{	TOPWAY_B_SCREEM_PAGE_INIT_Single,    
	0,							
	SCREEM_SIG_STR1_ON|SCREEM_SIG_STR2_ON|SCREEM_SIG_STR3_ON|SCREEM_SIG_16BIT1_ON, 
	3, TOPWAY_STR_ADDR_GUN_B_QR_CODE, TOPWAY_STR_ADDR_GUN_B_QR_CODE_2, 
	TOPWAY_STR_ADDR_GUN_B_RESERVE_ID,0,	0,0,			
	14, TOPWAY_16BIT_ADDR_START_GUN_B_STAT, 0, 0, 
	0, 0},
	//单枪充电中
	{	TOPWAY_B_SCREEM_PAGE_CHG_Single, 
	0, 
	SCREEM_SIG_STR1_ON|SCREEM_SIG_16BIT1_ON,
	1, TOPWAY_STR_ADDR_GUN_B_CARD_ID, 0, 0,0,0,0, 
	14, TOPWAY_16BIT_ADDR_CHG_GUN_B_CHG_MODE, 0, 0, 
	0, 0}, 
	//单枪结算中
	{	TOPWAY_B_SCREEM_PAGE_STOP_Single, 
	0, 
	SCREEM_SIG_16BIT1_ON,
	0, 0, 0, 0,0,0,0, 
	7, TOPWAY_16BIT_ADDR_STOP_GUN_B_MONEY, 0, 0, 
	0, 0}, 

	//交流
	{TOPWAY_SCREEM_PAGE_AC_CHARGER, 1, SCREEM_SIG_16BIT1_ON,
	0, 0, 0, 0,0,0,0, 
	2, TOPWAY_16BIT_ADDR_VIEW_AC_START_STOP, 0, 0, 
	0, 0}, 

};
static unsigned char s_ucTopwayScreemPageNum = sizeof(TopwayScreem) / sizeof(TopwayScreem_Def); 


/************************************************************************************************/
//显示控制处理数据结构变量
/************************************************************************************************/
//typedef struct DisplayCtrl_
//{
//	unsigned char u8CurrPageNo;
//	unsigned char u8LastPageNo;
//	unsigned char u8IsNewPage;
//	unsigned char u8TopwayScreem_Index;  //TopwayScreem[]的下标
//
//
//	unsigned char u8PageNeedReflashFlag;	//需要刷新的标志   enum ScreemSigReflashFlagDef
//	unsigned char u8PageReflashDoneFlag;	//已刷新的标志   enum ScreemSigReflashFlagDef
//
//	unsigned char u8TxIsBusy;				//上一包数据还未发完
//
//}DisplayCtrl_Def;

DisplayCtrl_Def gs_DisplayCtrl = {0};  

/************************************************************************************************/
// A、B枪 充电、结算、待机组合显示屏，跳转逻辑关系定义   unsigned char GetGunAGunBDispPageNo(void)
/************************************************************************************************/
typedef struct GunA_GunB_Disp_Page_Jump_
{
	unsigned char u8GunADispStat;   //对应 gs_DisplayCtrl.u8GunDispStat[0]
	unsigned char u8GunBDispStat;	//对应 gs_DisplayCtrl.u8GunDispStat[1]
	unsigned char u8_PageNo;	
}GunA_GunB_Disp_Page_Jump_Def;
static GunA_GunB_Disp_Page_Jump_Def s_GunA_GunB_Disp_Page_Jump[] =
{
	{GUN_DISP_STAT_Standby, GUN_DISP_STAT_Standby, TOPWAY_SCREEM_PAGE_INIT},
	{GUN_DISP_STAT_Chg, GUN_DISP_STAT_Chg, TOPWAY_SCREEM_PAGE_BOTH_CHG},
	{GUN_DISP_STAT_Stop, GUN_DISP_STAT_Stop, TOPWAY_SCREEM_PAGE_BOTH_STOP},
	{GUN_DISP_STAT_Standby, GUN_DISP_STAT_Chg, TOPWAY_SCREEM_PAGE_A_IDLE_B_CHG},
	{GUN_DISP_STAT_Chg, GUN_DISP_STAT_Standby, TOPWAY_SCREEM_PAGE_A_CHG_B_IDLE},
	{GUN_DISP_STAT_Standby, GUN_DISP_STAT_Stop, TOPWAY_SCREEM_PAGE_A_IDLE_B_STOP},
	{GUN_DISP_STAT_Stop, GUN_DISP_STAT_Standby, TOPWAY_SCREEM_PAGE_A_STOP_B_IDLE},
	{GUN_DISP_STAT_Stop, GUN_DISP_STAT_Chg, TOPWAY_SCREEM_PAGE_A_STOP_B_CHG},
	{GUN_DISP_STAT_Chg, GUN_DISP_STAT_Stop, TOPWAY_SCREEM_PAGE_A_CHG_B_STOP},
};
static unsigned char s_ucGunAGunBPageJumpLogicNum = sizeof(s_GunA_GunB_Disp_Page_Jump) / sizeof(GunA_GunB_Disp_Page_Jump_Def); 

//单枪模式
typedef struct GunA_Disp_Page_Jump_
{
	unsigned char u8GunADispStat;   //对应 gs_DisplayCtrl.u8GunDispStat[0]
	unsigned char u8_PageNo;	
}GunA_Disp_Page_Jump_Def;
static GunA_Disp_Page_Jump_Def s_GunA_Disp_Page_Jump[] =
{
	{GUN_DISP_STAT_Standby, TOPWAY_SCREEM_PAGE_INIT_Single},
	{GUN_DISP_STAT_Chg, TOPWAY_SCREEM_PAGE_CHG_Single},
	{GUN_DISP_STAT_Stop, TOPWAY_SCREEM_PAGE_STOP_Single},	
};
static unsigned char s_ucGunAPageJumpLogicNum = sizeof(s_GunA_Disp_Page_Jump) / sizeof(GunA_Disp_Page_Jump_Def); 

static GunA_Disp_Page_Jump_Def s_GunB_Disp_Page_Jump[] =
{
	{GUN_DISP_STAT_Standby, TOPWAY_B_SCREEM_PAGE_INIT_Single},
	{GUN_DISP_STAT_Chg, TOPWAY_B_SCREEM_PAGE_CHG_Single},
	{GUN_DISP_STAT_Stop, TOPWAY_B_SCREEM_PAGE_STOP_Single},	
};
static unsigned char s_ucGunBPageJumpLogicNum = sizeof(s_GunB_Disp_Page_Jump) / sizeof(GunA_Disp_Page_Jump_Def); 

/************************************************************************************************/
//密码定义  unsigned char CheckPasswordLevel(unsigend char *pPwdBuf, unsigned char u8Lenth)
/************************************************************************************************/
//typedef struct Password_Table_
//{
//	unsigned char u8UserLevel;   
//	unsigned char u8Password[6];
//}Password_Table_Def;
//static Password_Table_Def s_PasswordTable[] =
//{
//	{1, "151719"},
//	{2, "151719"},
//	{3, "151719"},
//	{4, "865748"},
//};
//static unsigned char s_PasswordTableNum = sizeof(s_PasswordTable) / sizeof(Password_Table_Def); 
/************************************************************************************************/
//
/************************************************************************************************/


NOINIT MODBUS_RECV_INFO gMdbusRcvInfo[MDBUS_RCV_ARRY_NUM + 4];
NOINIT UINT8 u8AnalayseRbuf[MDBUS_BYTES_MAX_NUM + 50];
NOINIT UINT8 u8MdbSendbuf[MDBUS_SEND_BYTES_MAX_NUM+20];

unsigned char TopwayPasswordBuf[TOPWAY_PASSWORD_BUF_LENTH+1];	//用户密码

//////////////////
//初始化变量
//////////////////
void InitDisplayVariables(void)
{
	unsigned char i;
	gEvChargeInfo.u8IMMUCommAlm = FALSE;
	gs_DisplayCtrl.u8IsNewPage = 1;
	gs_DisplayCtrl.u8ChkPageFlag = 0;
	gs_DisplayCtrl.u8ChkPageFlagTimeOut = 0;

// 	if((gEvChargeInfo.u8IsSingleGunMode == TRUE) && (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_A_GUN))
// 	{
// 		gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_INIT_Single;
// 	}
// 	else if ((gEvChargeInfo.u8IsSingleGunMode == TRUE) && (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_B_GUN))
// 	{
// 		gs_DisplayCtrl.u8CurrPageNo = TOPWAY_B_SCREEM_PAGE_INIT_Single;
// 	}
// 	else
	{
		gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_INIT;
	}

	gs_DisplayCtrl.u16PageNeedReflashFlag = SCREEM_SIG_ALL_ON;
	gs_DisplayCtrl.u16PageReflashDoneFlag = 0;
	gs_DisplayCtrl.u8TxIsBusy = 0;

	gs_DisplayCtrl.IsScreemOk = FALSE;

	gs_DisplayCtrl.u16DisplayReflashDelay = 0;

	gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Standby;
	gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Standby;

	gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 0;
	gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 0;

	gs_DisplayCtrl.u8AdminChgKeyDelay[0] = 0;
	gs_DisplayCtrl.u8AdminChgKeyDelay[1] = 0;

	gs_DisplayCtrl.u8PasswordFailReturnPage = 0;
	gs_DisplayCtrl.u8PasswordOkJumpPage = 0;

	gs_DisplayCtrl.u8UserLevel = 1;
	gs_DisplayCtrl.u8PasswordIsOk = 0;

	gs_DisplayCtrl.u8SysTimeReflashPeriod = 0;

	gs_DisplayCtrl.u8WaitSpearOkTimeout = 0;
	gs_DisplayCtrl.u8WaitChgModeSelTimeout = 0;

	gs_DisplayCtrl.u8IsDispCardLeaveMoney = 0;
	gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[0] = 0;
	gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[1] = 0;

	gs_DisplayCtrl.u16ScreemSaverDelay = 0;
	gs_DisplayCtrl.u8ScreemSavingReturnPage = 0;
	gs_DisplayCtrl.IsScreemSaving = 0;
	gs_DisplayCtrl.u8PasswordPageOverTimer = 0;

	//gs_DisplayCtrl.u8AccountRequestStep = ACCOUNT_REQUEST_STEP_Init;
	//gs_DisplayCtrl.u8BrushWaitHostConfirmTimeout = 0;
	
	for(i = 0; i < TOPWAY_PASSWORD_BUF_LENTH; i++)
	{
		TopwayPasswordBuf[i] = '*';
	}
	TopwayPasswordBuf[i] = 0;
}

//检查密码等级，正确返回等级，否则，返回0
unsigned char CheckPasswordLevel(unsigned char *pPwdBuf, unsigned char u8Lenth)
{
	unsigned char i, j, uctemp, ucSuccess, k, u8UserLevel;

	if(u8Lenth == 0)
	{
		return 0;
	}

	memset(TopwayPasswordBuf, 0, sizeof(TopwayPasswordBuf));

	if(u8Lenth > TOPWAY_PASSWORD_BUF_LENTH)
		u8Lenth = TOPWAY_PASSWORD_BUF_LENTH;
	strncpy((char*)TopwayPasswordBuf, (const char*)pPwdBuf, u8Lenth);
	
	//for(k = 0; k < s_PasswordTableNum; k++)
	//{
	//	i = s_PasswordTableNum - k - 1;
	//	uctemp = strnlen( (const char*)(s_PasswordTable[i].u8Password), 6);
	//	if(uctemp == u8Lenth)
	//	{
	//		ucSuccess = TRUE;
	//		for(j = 0; j < u8Lenth; j++)
	//		{
	//			if(pPwdBuf[j] != s_PasswordTable[i].u8Password[j])
	//			{
	//				ucSuccess = FALSE;
	//				break;
	//			}
	//		}

	//		if(ucSuccess == TRUE)
	//		{
	//			//return s_PasswordTable[i].u8UserLevel;
	//			if(gs_DisplayCtrl.u8UserLevel == s_PasswordTable[i].u8UserLevel)
	//			{
	//				return s_PasswordTable[i].u8UserLevel;
	//			}
	//		}
	//	}
	//}
	for(k = 0; k < 4; k++)
	{
		i = 4 - k - 1;
		uctemp = strnlen( (const char*)(gEvChargeInfo.u16UserPassword[i]), 6);
		if(uctemp == u8Lenth)
		{
			ucSuccess = TRUE;
			for(j = 0; j < u8Lenth; j++)
			{
				if(pPwdBuf[j] != gEvChargeInfo.u16UserPassword[i][j])
				{
					ucSuccess = FALSE;
					break;
				}
			}

			if(ucSuccess == TRUE)
			{
				u8UserLevel = i+1;
				if(gs_DisplayCtrl.u8UserLevel == u8UserLevel)
				{
					return u8UserLevel;
				}
			}
		}
	}

	

	return  0;
}

//返回TopwayScreem[]的下标
unsigned char GetTopwayScreemTableIdx(unsigned char u8PageNo)
{
	unsigned char i;
	static unsigned char s_u8Index = 0;
	if(TopwayScreem[s_u8Index].u8_PageNo != u8PageNo)
	{
		for(i = 0; i < s_ucTopwayScreemPageNum; i++)
		{
			if(TopwayScreem[i].u8_PageNo == u8PageNo)
			{
				s_u8Index = i;
				break;
			}
		}
	}

	return s_u8Index;
}

//A、B枪 充电、结算、待机组合显示屏，跳转处理
unsigned char GetGunAGunBDispPageNo(void)
{
	unsigned char i;

	if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_WaitSpearOk)
		|| (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_WaitSpearOk))
	{		
		return TOPWAY_SCREEM_PAGE_NOTICE_PLUGIN_SPEAR;
	}

	if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_ChgModeSel)
		|| (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_ChgModeSel))
	{
		return TOPWAY_SCREEM_PAGE_CHGMODE_SEL_AUTO;
	}	

	if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_BrushWaitHostConfirm)
		|| (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_BrushWaitHostConfirm))
	{
		return TOPWAY_SCREEM_PAGE_BRUSH_CONFIRM_NET;
	}	

	//单枪模式
// 	if((gEvChargeInfo.u8IsSingleGunMode == TRUE) && (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_A_GUN))
// 	{
// 		for(i = 0; i < s_ucGunAPageJumpLogicNum; i++)
// 		{
// 			if(s_GunA_Disp_Page_Jump[i].u8GunADispStat == gs_DisplayCtrl.u8GunDispStat[0])
// 			{
// 				return s_GunA_Disp_Page_Jump[i].u8_PageNo;
// 			}
// 		}
// 
// 		return TOPWAY_SCREEM_PAGE_INIT_Single;
// 	}
// 	else if((gEvChargeInfo.u8IsSingleGunMode == TRUE) && (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_B_GUN))
// 	{
// 		for(i = 0; i < s_ucGunBPageJumpLogicNum; i++)
// 		{
// 			if(s_GunB_Disp_Page_Jump[i].u8GunADispStat == gs_DisplayCtrl.u8GunDispStat[1])
// 			{
// 				return s_GunB_Disp_Page_Jump[i].u8_PageNo;
// 			}
// 		}
// 		return TOPWAY_B_SCREEM_PAGE_INIT_Single;
// 	}
// 	else
	{
		for(i = 0; i < s_ucGunAGunBPageJumpLogicNum; i++)
		{
			if((s_GunA_GunB_Disp_Page_Jump[i].u8GunADispStat == gs_DisplayCtrl.u8GunDispStat[0])
				&& (s_GunA_GunB_Disp_Page_Jump[i].u8GunBDispStat == gs_DisplayCtrl.u8GunDispStat[1]))
			{
				return s_GunA_GunB_Disp_Page_Jump[i].u8_PageNo;
			}
		}

		return TOPWAY_SCREEM_PAGE_INIT;
	}	
}

//屏握手
void Cmd0x30Handshake(void)
{
	unsigned short u16Lenth;

	u8MdbSendbuf[0] = TOPWAY_SOI_VAL;
	u8MdbSendbuf[1] = 0x30;
	u16Lenth = 2;
	
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL1;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL2;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL3;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL4;	

	dv_Uart6.fSendDataTXY(u8MdbSendbuf, u16Lenth);

	//OSTimeDly(30);
}

//查询屏序号
void Cmd0x32GetPageNo(void)
{
	unsigned short  u16Lenth;

	u8MdbSendbuf[0] = TOPWAY_SOI_VAL;
	u8MdbSendbuf[1] = 0x32;
	u16Lenth = 2;

	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL1;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL2;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL3;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL4;

	//OSTimeDly(30);
	dv_Uart6.fSendDataTXY(u8MdbSendbuf, u16Lenth);
}

//设置页面
void Cmd0x70SetPageNo(unsigned char u8PageNo)
{
	unsigned short u16Lenth;

	u8MdbSendbuf[0] = TOPWAY_SOI_VAL;
	u8MdbSendbuf[1] = 0x70;
	u16Lenth = 2;
	u8MdbSendbuf[u16Lenth++] = 0;
	u8MdbSendbuf[u16Lenth++] = u8PageNo;

	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL1;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL2;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL3;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL4;

	//OSTimeDly(30);
	dv_Uart6.fSendDataTXY(u8MdbSendbuf, u16Lenth);
}


//写字符串
unsigned char WriteStringBuf[128];
void Cmd0x42WriteString(unsigned long u32StartAddr)
{
	unsigned char i, uclenth;
	unsigned short u16Lenth;

	u8MdbSendbuf[0] = TOPWAY_SOI_VAL;
	u8MdbSendbuf[1] = 0x42;
	u16Lenth = 2;

	u8MdbSendbuf[u16Lenth++] = (u32StartAddr>>24)&0xff;
	u8MdbSendbuf[u16Lenth++] = (u32StartAddr>>16)&0xff;
	u8MdbSendbuf[u16Lenth++] = (u32StartAddr>>8)&0xff;
	u8MdbSendbuf[u16Lenth++] = (u32StartAddr)&0xff;

	memset(WriteStringBuf, 0, sizeof(WriteStringBuf));

	TopwayGetStringSignal(u32StartAddr, WriteStringBuf);

	uclenth = strnlen((char const*)WriteStringBuf, 128);

	for(i = 0; i < uclenth; i++)
	{
		u8MdbSendbuf[u16Lenth++] = WriteStringBuf[i];
	}
	u8MdbSendbuf[u16Lenth++] = 0;

	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL1;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL2;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL3;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL4;

	//OSTimeDly(30);
	dv_Uart6.fSendDataTXY(u8MdbSendbuf, u16Lenth);
}

//多个16、32、64位数据填写
enum IntergerBitNumberModeDef
{
	INT_16BIT = 0,
	INT_32BIT,
	INT_64BIT,
};
void Cmd0x82WriteMultiInteger(unsigned long u32StartAddr, unsigned char BitNumMode, unsigned char SigNum)
{
	unsigned char i;
	unsigned long u32Value;
	unsigned short u16Value, u16Lenth;

	u8MdbSendbuf[0] = TOPWAY_SOI_VAL;
	u8MdbSendbuf[1] = 0x82;
	//if(SigNum > 0)
	{
		u8MdbSendbuf[2] = (u32StartAddr>>24)&0xff;
		u8MdbSendbuf[3] = (u32StartAddr>>16)&0xff;
		u8MdbSendbuf[4] = (u32StartAddr>>8)&0xff;
		u8MdbSendbuf[5] = (u32StartAddr)&0xff;
		u8MdbSendbuf[6] = SigNum;
		u16Lenth = 7;

		if(BitNumMode == INT_16BIT)
		{
			for(i = 0; i < SigNum; i++)
			{
				u16Value = u16TopwayGet16bitsSignal(u32StartAddr + i*2);
				u8MdbSendbuf[u16Lenth++] = (u16Value>>8)&0xff;
				u8MdbSendbuf[u16Lenth++] = (u16Value)&0xff;
			}
		}
		else if(BitNumMode == INT_32BIT)
		{
			for(i = 0; i < SigNum; i++)
			{
				u32Value = u32TopwayGet32bitsSignal(u32StartAddr + i*4);
				u8MdbSendbuf[u16Lenth++] = (u32Value>>24)&0xff;
				u8MdbSendbuf[u16Lenth++] = (u32Value>>16)&0xff;
				u8MdbSendbuf[u16Lenth++] = (u32Value>>8)&0xff;
				u8MdbSendbuf[u16Lenth++] = (u32Value)&0xff;
			}
		}
		else if(BitNumMode == INT_64BIT)
		{

		}
		
	}
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL1;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL2;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL3;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL4;

	//OSTimeDly(30);
	dv_Uart6.fSendDataTXY(u8MdbSendbuf, u16Lenth);
}
//更新背光
enum BackLightOnOffDef
{
	BackLightOn = 0,
	BackLightOff,
};
void Cmd0x5FWriteBacklight(unsigned char u8CtrlSig)
{
	unsigned short u16Lenth;

	u8MdbSendbuf[0] = TOPWAY_SOI_VAL;
	u8MdbSendbuf[1] = 0x5F;
	u16Lenth = 2;

	if(u8CtrlSig == BackLightOff)
	{
		u8MdbSendbuf[u16Lenth++] = 0;
	}
	else
	{
		u8MdbSendbuf[u16Lenth++] = 63;
	}

	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL1;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL2;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL3;
	u8MdbSendbuf[u16Lenth++] = TOPWAY_EOI_VAL4;

	dv_Uart6.fSendDataTXY(u8MdbSendbuf, u16Lenth);
}
//////////////////
//刷新页面数据
//////////////////
#define IsNeedReflash(flag, n)			(((flag & n)!=0))  //判断FLAG的位N是否置位
#define IsReflashNotDone(flag, n)		(((flag & n)==0))  //判断FLAG的位N是否为0
#define SetReflashDoneFlag(flag, n)		(flag |= n)		   //置位FLAG的位N
#define ClrReflashDoneFlag(flag, n)		(flag &= (~n))	   //清除FLAG的位N

//需要定期刷新系统时间及系统告警状态标志
#define  CYCLE_REPORT_SIGNAL_START_ADDR		TOPWAY_16BIT_ADDR_SYSTEM_ALARM_STAT
#define  CYCLE_REPORT_SIGNAL_NUM	17

void ReflashPageData(unsigned char u8PageNo)
{
	unsigned char i, isNewPage;
	TopwayScreem_Def *pPage;
	static unsigned char s_u8TopwayScreem_Index = 0;
	static unsigned char s_u8ScreemStrReflashStep = 0;

	isNewPage = FALSE;

	//检查是否需要重新查找TopwayScreem[]
	if(TopwayScreem[s_u8TopwayScreem_Index].u8_PageNo != u8PageNo)
	{
		for(i = 0; i < s_ucTopwayScreemPageNum; i++)
		{
			if(TopwayScreem[i].u8_PageNo == u8PageNo)
			{
				s_u8TopwayScreem_Index = i;
				break;
			}
		}
		isNewPage = TRUE;
	}
	//纠错
	if(s_u8TopwayScreem_Index >= s_ucTopwayScreemPageNum)
	{	
		s_u8TopwayScreem_Index = 0; 
	}

	pPage = (TopwayScreem_Def *) &TopwayScreem[s_u8TopwayScreem_Index];

	//刷新标志
	if(isNewPage == TRUE)
	{		
		gs_DisplayCtrl.u16PageReflashDoneFlag = 0;
		gs_DisplayCtrl.u16DisplayReflashDelay = 0;
	}

	gs_DisplayCtrl.u16PageNeedReflashFlag = pPage->u16_ReflashFlag ;

	//刷新字串1
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR1_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR1_ON))
	{		
		if((pPage->u8_StrSigNumber < 1) || (pPage->u32_StrSigAddr[0] == 0x00000000))  //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR1_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x42WriteString(pPage->u32_StrSigAddr[0]);			
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR1_ON);
		}
	}
	
	//刷新字串2
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR2_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR2_ON))
	{			
		if((pPage->u8_StrSigNumber < 2) || (pPage->u32_StrSigAddr[1] == 0))  //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR2_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x42WriteString(pPage->u32_StrSigAddr[1]);
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR2_ON);
		}
	}

	//刷新字串3
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR3_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR3_ON))
	{		
		if((pPage->u8_StrSigNumber < 3) || (pPage->u32_StrSigAddr[2] == 0x00000000))  //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR3_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x42WriteString(pPage->u32_StrSigAddr[2]);			
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR3_ON);
		}
	}

	//刷新字串4
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR4_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR4_ON))
	{			
		if((pPage->u8_StrSigNumber < 4) || (pPage->u32_StrSigAddr[3] == 0))  //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR4_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x42WriteString(pPage->u32_StrSigAddr[3]);
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR4_ON);
		}
	}


	//刷新字串5
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR5_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR5_ON))
	{		
		if((pPage->u8_StrSigNumber < 5) || (pPage->u32_StrSigAddr[4] == 0x00000000))  //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR5_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x42WriteString(pPage->u32_StrSigAddr[4]);			
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR5_ON);
		}
	}

	//刷新字串6
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR6_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR6_ON))
	{			
		if((pPage->u8_StrSigNumber < 6) || (pPage->u32_StrSigAddr[5] == 0))  //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR6_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x42WriteString(pPage->u32_StrSigAddr[5]);
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR6_ON);
		}
	}


	//刷新16位数据段1
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_16BIT1_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_16BIT1_ON))	
	{
		if((pPage->u8_16bitSigSeg1Number == 0) || (pPage->u32_16bitSigSeg1Addr == 0))   //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_16BIT1_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x82WriteMultiInteger(pPage->u32_16bitSigSeg1Addr, INT_16BIT, pPage->u8_16bitSigSeg1Number);
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_16BIT1_ON);
		}
	}

	//刷新16位数据段2
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_16BIT2_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_16BIT2_ON))	
	{
		if((pPage->u8_16bitSigSeg2Number == 0) || (pPage->u32_16bitSigSeg2Addr == 0))  //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_16BIT2_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x82WriteMultiInteger(pPage->u32_16bitSigSeg2Addr, INT_16BIT, pPage->u8_16bitSigSeg2Number);
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_16BIT2_ON);
		}
	}

	//刷新32位数据段
	if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_32BIT_ON) 
		&& IsReflashNotDone(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_32BIT_ON))		
	{
		if((pPage->u8_32bitSigNumber == 0) || (pPage->u32_32bitSigAddr == 0))  //排除异常
		{
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_32BIT_ON);
		}
		else if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;
			Cmd0x82WriteMultiInteger(pPage->u32_32bitSigAddr, INT_32BIT, pPage->u8_32bitSigNumber);
			SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_32BIT_ON);
		}
	}

	//启动刷新计时器
	if(gs_DisplayCtrl.u16PageNeedReflashFlag == gs_DisplayCtrl.u16PageReflashDoneFlag)
	{
		if(gs_DisplayCtrl.u16DisplayReflashDelay == 0)
		{
			gs_DisplayCtrl.u16DisplayReflashDelay = 1; 
		}
		else if(gs_DisplayCtrl.u16DisplayReflashDelay > DISP_REFLASH_PERIOD)
		{
			gs_DisplayCtrl.u16PageReflashDoneFlag = 0;			//清空已刷新标志
			gs_DisplayCtrl.u16DisplayReflashDelay = 0;

			//一个页面字符串超过3个，则分两次刷新
			if(pPage->u8_StrSigNumber > 3)
			{				
				if(s_u8ScreemStrReflashStep == 0)
				{
					if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR4_ON)) 
					{
						SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR4_ON);
					}
					if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR5_ON)) 
					{
						SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR5_ON);
					}
					if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR6_ON)) 
					{
						SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR6_ON);
					}

					s_u8ScreemStrReflashStep = 1;
				}
				else
				{
					if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR1_ON)) 
					{
						SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR1_ON);
					}
					if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR2_ON)) 
					{
						SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR2_ON);
					}
					if(IsNeedReflash(gs_DisplayCtrl.u16PageNeedReflashFlag, SCREEM_SIG_STR3_ON)) 
					{
						SetReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, SCREEM_SIG_STR3_ON);
					}

					s_u8ScreemStrReflashStep = 0;
				}				
			}
		}	

		//为防止页面没有切换成功，每次更换页面后，做一次回查处理,  在所有数据都更新一遍后才做此回查处理
		if((gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_NeedChk) && (gs_DisplayCtrl.u8TxIsBusy == FALSE))
		{
			gs_DisplayCtrl.u8TxIsBusy = TRUE;			
			Cmd0x32GetPageNo();
			gs_DisplayCtrl.u8ChkPageFlag = PAGE_CHK_FLAG_SentCmd;
			gs_DisplayCtrl.u8ChkPageFlagTimeOut = 1;
		}
		else if(gs_DisplayCtrl.u8SysTimeReflashPeriod > 0)   //定期刷新时间
		{
			if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
			{
				gs_DisplayCtrl.u8TxIsBusy = TRUE;
				Cmd0x82WriteMultiInteger(CYCLE_REPORT_SIGNAL_START_ADDR, INT_16BIT, CYCLE_REPORT_SIGNAL_NUM);
				gs_DisplayCtrl.u8SysTimeReflashPeriod = 0;
			}
		}
	}
}
//////////////////
// 上电开机，先与屏握手，检查屏返回握手回复
//////////////////
void CheckScreemHandshkeResponse(void)
{
	UINT8 ArryIdx = 0;

	for (ArryIdx = 0; ArryIdx < MDBUS_RCV_ARRY_NUM; ArryIdx++)
	{
		if (TRUE == gMdbusRcvInfo[ArryIdx].uReceivFlag)
		{
			//0x30命令 握手数据返回
			if((gMdbusRcvInfo[ArryIdx].u16ReceivLenth == 23)
				&& (gMdbusRcvInfo[ArryIdx].uBuf[1] == 0x30))
			{
				gs_DisplayCtrl.IsScreemOk = TRUE;
			}

			gMdbusRcvInfo[ArryIdx].uReceivFlag = FALSE;
		}
	}	
}

//////////////////
//
//////////////////

void TopwayKeyEventDeal(void)
{
#define XXXX_BUF_ARRY_MAX_NUM  100
	
	unsigned long u32SigAddr;
	unsigned char i, uctemp, ucScreemNo, ucAdjustMode;
	UINT16 u16Value;
	__XXX_UNION_VALUE untemp;
	__16BITS_UNION_VALUE untemp_16bits;
#ifdef	Debug_Print_En     
        UINT8 u8DataArry[XXXX_BUF_ARRY_MAX_NUM];
	UINT8 u8Idx = 0;
	UINT8* p= NULL;
#endif        
	UINT8 ArryIdx = 0;
	static UINT32 u32CheckUart6Times = 0;
        UNUSED(u32CheckUart6Times);

	for (ArryIdx = 0; ArryIdx < MDBUS_RCV_ARRY_NUM; ArryIdx++)
	{
		if (TRUE == gMdbusRcvInfo[ArryIdx].uReceivFlag)
		{
			u32SigAddr = ((unsigned long)gMdbusRcvInfo[ArryIdx].uBuf[2]<<24) + ((unsigned long)gMdbusRcvInfo[ArryIdx].uBuf[3]<<16) + ((unsigned long)gMdbusRcvInfo[ArryIdx].uBuf[4]<<8) + (gMdbusRcvInfo[ArryIdx].uBuf[5]);

			//0. 0x77命令 键盘输入数据返回 -  字符串
			if((gMdbusRcvInfo[ArryIdx].uBuf[1] == 0x77) && (u32SigAddr < 0x00010000) && (gMdbusRcvInfo[ArryIdx].u16ReceivLenth > 10))
			{
				gs_DisplayCtrl.u16ScreemSaverDelay = 0;  //屏保计时器清0

				//密码输输入
				if(u32SigAddr == TOPWAY_STR_ADDR_PASSWORD)
				{
					if(gMdbusRcvInfo[ArryIdx].u16ReceivLenth <= 17)
					{	
						uctemp = CheckPasswordLevel( &gMdbusRcvInfo[ArryIdx].uBuf[6], (gMdbusRcvInfo[ArryIdx].u16ReceivLenth-11));
						
						//if((uctemp > 0) && (uctemp == gs_DisplayCtrl.u8UserLevel))
						if(uctemp > 0)
						{
							gs_DisplayCtrl.u8PasswordIsOk = 1;
							ClrReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, (SCREEM_SIG_16BIT1_ON));  //需要马上刷新
						}
					}
				}
				//帧起始域输入
				else if(u32SigAddr == TOPWAY_STR_ADDR_NetFrameHead)
				{
					if(gMdbusRcvInfo[ArryIdx].u16ReceivLenth <= 15)
					{	
						untemp_16bits.chtemp[0] = TwoAscii2Hex(gMdbusRcvInfo[ArryIdx].uBuf[6], gMdbusRcvInfo[ArryIdx].uBuf[7]);
						untemp_16bits.chtemp[1] = TwoAscii2Hex(gMdbusRcvInfo[ArryIdx].uBuf[8], gMdbusRcvInfo[ArryIdx].uBuf[9]);
						gs_EthAppCommMng.u16NetFrameHead = untemp_16bits.u16Temp; 
						untemp.u32Value = untemp_16bits.u16Temp; 
						ExSetSigInfo(E_TYPE_SYS, ID_NetFrameHead_NUM, untemp, NULL, BY_E2PROM);
					}
				}
				//桩号
				else if(u32SigAddr == TOPWAY_STR_ADDR_STATION_ID)
				{
					memset(WriteStringBuf, 0, sizeof(WriteStringBuf));

					if(gMdbusRcvInfo[ArryIdx].u16ReceivLenth <= 27) //最多16个
					{
						uctemp = gMdbusRcvInfo[ArryIdx].u16ReceivLenth - 11;
						for(i = 0; i < uctemp; i++)
						{
							untemp.chtemp[0] = gMdbusRcvInfo[ArryIdx].uBuf[6+i];
							ExSetSigInfo(E_TYPE_SYS, ID_u8StationCode_No1 + i, untemp, NULL, BY_E2PROM);
						}
						for(; i < 32; i++)  //补充0
						{
							untemp.chtemp[0] = 0;
							ExSetSigInfo(E_TYPE_SYS, ID_u8StationCode_No1 + i, untemp, NULL, BY_E2PROM);
						}
					}					
				}
				
				//修改 用户密码
				else if(u32SigAddr == TOPWAY_STR_ADDR_USER3_PASSWORD)
				{
					memset(WriteStringBuf, 0, sizeof(WriteStringBuf));

					if(gMdbusRcvInfo[ArryIdx].u16ReceivLenth <= 17)
					{	
						//大等于3才可以修改，目前是三个等级同一个密码
						if(gs_DisplayCtrl.u8UserLevel >= 3)
						{
							uctemp = gMdbusRcvInfo[ArryIdx].u16ReceivLenth - 11;
							for(i = 0; i < uctemp && i < 6; i++)
							{
								untemp.chtemp[0] = gMdbusRcvInfo[ArryIdx].uBuf[6+i];
								ExSetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1 + i, untemp, NULL, BY_E2PROM);
								ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1 + i, untemp, NULL, BY_E2PROM);
								ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1 + i, untemp, NULL, BY_E2PROM);
							}
							for(; i < 6; i++)  //补充0
							{
								untemp.chtemp[0] = 0;
								ExSetSigInfo(E_TYPE_SYS, ID_u8OffLinePwsd1 + i, untemp, NULL, BY_E2PROM);
								ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L2_1 + i, untemp, NULL, BY_E2PROM);
								ExSetSigInfo(E_TYPE_SYS, ID_UserPassword_L3_1 + i, untemp, NULL, BY_E2PROM);
							}
						}
					}
				}
				//域名
				else if(u32SigAddr == TOPWAY_STR_ADDR_DOMAIN_NAME)
				{
					memset(WriteStringBuf, 0, sizeof(WriteStringBuf));

					if(gMdbusRcvInfo[ArryIdx].u16ReceivLenth <= 43)  //最多32个
					{
						uctemp = gMdbusRcvInfo[ArryIdx].u16ReceivLenth - 11;
						for(i = 0; i < uctemp; i++)
						{
							untemp.chtemp[0] = gMdbusRcvInfo[ArryIdx].uBuf[6+i];
							ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_RemoteServerDomainName_NUM_1 + i, untemp, NULL, BY_E2PROM);
						}
						if(i < 32)  //补充一个0
						{
							untemp.chtemp[0] = 0;
							ExSetSigInfo(E_TYPE_SYS, ID_SETDATA_RemoteServerDomainName_NUM_1 + i, untemp, NULL, BY_E2PROM);
						}
					}					
				}
			}
			//1. 0x77命令 键盘输入数据返回 - 16bits
			else if((gMdbusRcvInfo[ArryIdx].u16ReceivLenth == 12)
				&& (gMdbusRcvInfo[ArryIdx].uBuf[1] == 0x77))
			{		
				gs_DisplayCtrl.u16ScreemSaverDelay = 0;  //屏保计时器清0

				u16Value = ((unsigned short)gMdbusRcvInfo[ArryIdx].uBuf[6]<<8) + (gMdbusRcvInfo[ArryIdx].uBuf[7]);

				//参数设置
				if( ((u32SigAddr >= TOPWAY_16BIT_SYS_PARA_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_16BIT_SYS_PARA_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_16BIT_SYS_PARA_ADDR_RANGE_START_2) && (u32SigAddr <= TOPWAY_16BIT_SYS_PARA_ADDR_RANGE_END_2))
					|| ((u32SigAddr >= TOPWAY_16BIT_GUN_A_PARA_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_16BIT_GUN_A_PARA_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_16BIT_GUN_B_PARA_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_16BIT_GUN_B_PARA_ADDR_RANGE_END)) 
					|| ((u32SigAddr >= TOPWAY_16BIT_FEE_PARA_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_16BIT_FEE_PARA_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_16BIT_ADDR_PARAM3_START) && (u32SigAddr <= TOPWAY_16BIT_ADDR_PARAM3_END)))
				{
					TopwaySetCfgSignal(u32SigAddr, u16Value, 0);
				}	
				//校准参数，校准期间不能修改
				else if((u32SigAddr >= TOPWAY_16BIT_ADJUST_PARA_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_16BIT_ADJUST_PARA_ADDR_RANGE_END))
				{
					if((gEvChargeInfo.u8_AutoTestStep > 0)
						|| (gEvChargeInfo.u8_A_TestMode == TRUE) 
						|| (gEvChargeInfo.u8_B_TestMode == TRUE))
					{						
					}
					else
					{
						TopwaySetCfgSignal(u32SigAddr, u16Value, 0);
					}
				}
				//调测命令
				else if((u32SigAddr >= TOPWAY_16BIT_DEBUG_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_16BIT_DEBUG_ADDR_RANGE_END))
				{
					TopwayDebugSignal(u32SigAddr, u16Value);
				}
				//储能系统调测命令
				else if((u32SigAddr >= TOPWAY_16BIT_ADDR_DEBUG_ENERGY_SYS_RANGE_START) && (u32SigAddr <= TOPWAY_16BIT_ADDR_DEBUG_ENERGY_SYS_RANGE_END))
				{
					TopwayDebugSignal_EnergySys(u32SigAddr, u16Value);
				}
				
				//测试按键按下
				/*else if(u32SigAddr == TOPWAY_16BIT_ADDR_TEST_KEY)
				{
					Put_BrushSem();
				}*/
				//系统时间设置
				else if((u32SigAddr >= TOPWAY_16BIT_ADDR_SYSTEM_TIME_Y) && (u32SigAddr <= TOPWAY_16BIT_ADDR_SYSTEM_TIME_S))
				{
					TopwaySetSysTime(u32SigAddr, u16Value);
				}
				else
				{
					switch(u32SigAddr)
					{
						//跳转页面处理
						case TOPWAY_16BIT_ADDR_CURR_PAGE_NO:
							for(i = 0; i < s_ucTopwayScreemPageNum; i++)
							{
								if(TopwayScreem[i].u8_PageNo == u16Value)
								{
									gs_DisplayCtrl.u8CurrPageNo = u16Value;
									gs_DisplayCtrl.u8LastPageNo = gs_DisplayCtrl.u8CurrPageNo;  //因是屏主动下发的，所以不需要再上报新屏号给屏
									gs_DisplayCtrl.u8ChkPageFlag = PAGE_CHK_FLAG_Ok;
									break;
								}
							}	

							//跳转入充电记录，获取第一条记录
							if(u16Value == TOPWAY_SCREEM_PAGE_RECORD_1)
							{
								GetRecordDeal(GetRecordDeal_Init);
							}
							break;

						//枪选择
						case TOPWAY_16BIT_ADDR_GUN_SELECT:
							if(u16Value < 3)
							{
								if((u16Value == 1) && (gs_EthAppCommMng.u8OrderChargeCMD[0] == TRUE))
								{
									u16Value = 0;
								}
								else if((u16Value == 2) && (gs_EthAppCommMng.u8OrderChargeCMD[1] == TRUE))
								{
									u16Value = 0;
								}
								gEvChargeInfo.u8GunSelect = u16Value;
							}
							ClrReflashDoneFlag(gs_DisplayCtrl.u16PageReflashDoneFlag, (SCREEM_SIG_16BIT1_ON|SCREEM_SIG_16BIT2_ON));  //需要马上刷新
							break;

						//按键处理： 确定 充电模式选择 
						case TOPWAY_16BIT_ADDR_KEY_CHG_MODE_SELECT:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{
// 								if(gEvChargeInfo.u8GunSelect == 1)
// 								{
// 									gTempChargeArgs.u8ChargeMode = u16Value;	
// 
// 									if ((gEvChargeInfo.u8EV_SpearPst[GUN_NO_A] == SPEAR_ON_LINE)
// 										 && (gs_GunCtrlReq[GUN_NO_A].GunChgPhase == GunChgPhase_Init))
// 									{
// 										gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Chg;
// 										if(gEvChargeInfo.u8_A_LogInMother == EV_ENTER_IC_CARD)
// 										{
// 											gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 1;
// 										}
// 										gEvChargeInfo.u8GunSelect = 0;
// 
// 										gs_IMMU2_LCCU_Mng.u8GunTaskType[GUN_NO_A] = GUN_TASK_TYPE_Chg2Ev;
// 										//gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_A] = ES_BATT_TASK_TYPE_Batt2Ev;
// 
// 										A_SetChargeSTART_Cmd(gEvChargeInfo.u8_A_LogInMother);
// 
// 
// 										
// 										/*__AC_MIN_MCCB_ON();
// 										__AC_MIN_MCCB_ON();
// 										
// 										gEvChargeInfo.u8_Indicate_A_STARTChargeFlag = TRUE;
// 										if (gEvChargeInfo.u8_ParamTotalMCCBDelay == 0)
// 										{
// 											gEvChargeInfo.u8_A_STARTChargeDelay = POWER_ON_RELACATION_12_TIME_OUT;
// 										}
// 										else
// 										{
// 											gEvChargeInfo.u8_A_STARTChargeDelay = POWER_ON_RELACATION_TIMER_1_START;
// 										}*/
// 									}
// 									else
// 									{
// 										gs_DisplayCtrl.u8WaitSpearOkTimeout = WAIT_SPEAR_OK_TIME_MAX;
// 										gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_WaitSpearOk;
// 									}
// 
// 								}
// 								else if(gEvChargeInfo.u8GunSelect == 2)
// 								{
// 									gTempChargeArgs.u8ChargeMode = u16Value;	
// 
// 									if ((gEvChargeInfo.u8EV_SpearPst[GUN_NO_B] == SPEAR_ON_LINE)
// 										&& (gs_GunCtrlReq[GUN_NO_B].GunChgPhase == GunChgPhase_Init))
// 									{
// 										gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Chg;
// 										if(gEvChargeInfo.u8_B_LogInMother == EV_ENTER_IC_CARD)
// 										{
// 											gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 1;
// 										}
// 										gEvChargeInfo.u8GunSelect = 0;
// 
// 										gs_IMMU2_LCCU_Mng.u8GunTaskType[GUN_NO_B] = GUN_TASK_TYPE_Chg2Ev;
// 										//gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_A] = ES_BATT_TASK_TYPE_Batt2Ev;
// 
// 										B_SetChargeSTART_Cmd(gEvChargeInfo.u8_B_LogInMother);
// 
// 
// 										/*__AC_MIN_MCCB_ON();
// 										__AC_MIN_MCCB_ON();										
// 										gEvChargeInfo.u8_Indicate_B_STARTChargeFlag = TRUE;
// 										if (gEvChargeInfo.u8_ParamTotalMCCBDelay == 0)
// 										{
// 											gEvChargeInfo.u8_B_STARTChargeDelay = POWER_ON_RELACATION_12_TIME_OUT;
// 										}
// 										else
// 										{
// 											gEvChargeInfo.u8_B_STARTChargeDelay = POWER_ON_RELACATION_TIMER_1_START;
// 										}*/
// 									}
// 									else
// 									{
// 										gs_DisplayCtrl.u8WaitSpearOkTimeout = WAIT_SPEAR_OK_TIME_MAX;
// 										gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_WaitSpearOk;
// 									}
// 								}
								gs_DisplayCtrl.u8IsDispCardLeaveMoney = 0;
							}
							break;

						//按键处理： 取消 充电模式选择
						case TOPWAY_16BIT_ADDR_KEY_CHG_MODE_CANCEL:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{
								if(gEvChargeInfo.u8GunSelect == 1)
								{
									gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Standby;
									gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 0;
								}
								else if(gEvChargeInfo.u8GunSelect == 2)
								{
									gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Standby;
									gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 0;
								}

								gEvChargeInfo.u8GunSelect = 0;
								gs_DisplayCtrl.u8IsDispCardLeaveMoney = 0;
							}
							break;

						//按键处理：取消 充电机连接提示界面
						case TOPWAY_16BIT_ADDR_KEY_SPEAR_CON_CANCEL:
							if(gEvChargeInfo.u8GunSelect == 1)
							{
								gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Standby;
								gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 0;
							}
							else if(gEvChargeInfo.u8GunSelect == 2)
							{
								gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Standby;
								gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 0;
							}

							gEvChargeInfo.u8GunSelect = 0;					
							break;

						//按键处理：确认返回 A枪结算页面 确认返回键
						case TOPWAY_16BIT_ADDR_KEY_GUN_A_STOP_CONL: 
							gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Standby;
							break;

						//按键处理：确认返回 B枪结算页面 确认返回键
						case TOPWAY_16BIT_ADDR_KEY_GUN_B_STOP_CONL: 
							gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Standby;
							break;

						//按键：A枪密码启动充电按键
						case TOPWAY_16BIT_ADDR_KEY_GUN_A_ADMIN_START_KEY:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{
								
							}
							else
							{
								gs_DisplayCtrl.u8AdminChgKeyDelay[0] = 0;
							}
							gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[0] = 0; //触摸屏了，可以把钱不够的提示信息清掉
							break;
						//按键：B枪密码启动充电按键
						case TOPWAY_16BIT_ADDR_KEY_GUN_B_ADMIN_START_KEY:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{							
					
							}
							else
							{
								gs_DisplayCtrl.u8AdminChgKeyDelay[1] = 0;
							}
							gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[1] = 0; //触摸屏了，可以把钱不够的提示信息清掉
							break;

						//按键：A枪密码启动充电后的停止按键
						case TOPWAY_16BIT_ADDR_KEY_GUN_A_ADMIN_STOP_KEY:
							/*A_SetChargeSTOP_Cmd(LUI_STOP_CMD);*/
							gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Stop;
#ifdef	Debug_Print_En
							u8Idx = 0;
							p = "*****A stop by UI****";
							memset(u8DataArry, 0, XXXX_BUF_ARRY_MAX_NUM);
							u8Idx += sprintf((char*)u8DataArry,"%s",p);
							WriteDebugInfo(u8DataArry, u8Idx);
#endif
							break;
						//按键：B枪密码启动充电后的停止按键
						case TOPWAY_16BIT_ADDR_KEY_GUN_B_ADMIN_STOP_KEY:
							/*B_SetChargeSTOP_Cmd(LUI_STOP_CMD);*/
							gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Stop;
#ifdef	Debug_Print_En					
							p = "******B Gun Stop By UI******";
							memset(u8DataArry, 0, XXXX_BUF_ARRY_MAX_NUM);
							u8Idx += sprintf((char*)u8DataArry,"%s",p);
							WriteDebugInfo(u8DataArry, u8Idx);
#endif
							break;

						//充电时间设置
						case TOPWAY_16BIT_ADDR_SET_CHG_TIME:
							gTempChargeArgs.fTimeArgs = u16Value;	
							break;

						//充电电量设置
						case TOPWAY_16BIT_ADDR_SET_CHG_ENERGY:
							gTempChargeArgs.fEnergeArgs = u16Value;	
							break;

						//充电SOC设置
						case TOPWAY_16BIT_ADDR_SET_CHG_SOC:
							gTempChargeArgs.fSOCArgs = u16Value;	
							break;

						//用户等级设置
						case TOPWAY_16BIT_ADDR_PASSWORD_INPUT_USER_LEVEL:
							gs_DisplayCtrl.u8UserLevel = u16Value;	
							break;

						//按键： 密码登录界面 
						case TOPWAY_16BIT_ADDR_KEY_PASSWORD_INPUT:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{
								gs_DisplayCtrl.u8PasswordPageOverTimer = 0;

								if(gs_DisplayCtrl.u8PasswordIsOk)  //密码Ok
								{
									gs_DisplayCtrl.u8PasswordIsOk = 0;

									gs_DisplayCtrl.u8CurrPageNo = gs_DisplayCtrl.u8PasswordOkJumpPage;
				
// 									if(gs_DisplayCtrl.u8PasswordOkJumpPage == TOPWAY_SCREEM_PAGE_CHGMODE_SEL_AUTO)
// 									{
// 										if(gEvChargeInfo.u8GunSelect == 2)
// 										{
// 											gEvChargeInfo.u8_B_LogInMother = EV_ENTER_admin;
// 											//禁止模式选择则直接进入充电
// 											//if(gEvChargeInfo.u8ChgModeSelEn == 1) 
// 											{										
// 												gEvChargeInfo.u8ChargeMode[GUN_NO_B] = 0;	
// 												gTempChargeArgs.u8ChargeMode = 0;
// 
// 												if ((gEvChargeInfo.u8EV_SpearPst[GUN_NO_B] == SPEAR_ON_LINE)
// 													&& (gs_GunCtrlReq[GUN_NO_B].GunChgPhase == GunChgPhase_Init))
// 												{
// 													gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Chg;
// 
// 													gEvChargeInfo.u8GunSelect = 0;
// 
// 													gs_IMMU2_LCCU_Mng.u8GunTaskType[GUN_NO_B] = GUN_TASK_TYPE_Chg2Ev;
// 													//gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_B] = ES_BATT_TASK_TYPE_Batt2Ev;
// 
// 													B_SetChargeSTART_Cmd(gEvChargeInfo.u8_B_LogInMother);
// 												}
// 												else
// 												{
// 													gs_DisplayCtrl.u8WaitSpearOkTimeout = WAIT_SPEAR_OK_TIME_MAX;
// 													gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_WaitSpearOk;
// 												}
// 											}
// 											/*else
// 											{
// 												gs_DisplayCtrl.u8WaitChgModeSelTimeout = WAIT_CHG_MODE_SEL_TIME_MAX;
// 												gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_ChgModeSel;
// 												gEvChargeInfo.u8ChargeMode[GUN_NO_B] = 0;
// 											}			*/									
// 										}
// 										else //if(gEvChargeInfo.u8GunSelect == 1)
// 										{
// 											gEvChargeInfo.u8_A_LogInMother = EV_ENTER_admin;
// 											//禁止模式选择则直接进入充电
// 											//if(gEvChargeInfo.u8ChgModeSelEn == 1) 
// 											{										
// 												gEvChargeInfo.u8ChargeMode[GUN_NO_A] = 0;	
// 												gTempChargeArgs.u8ChargeMode = 0;
// 
// 												if ((gEvChargeInfo.u8EV_SpearPst[GUN_NO_A] == SPEAR_ON_LINE)
// 													&& (gs_GunCtrlReq[GUN_NO_A].GunChgPhase == GunChgPhase_Init))
// 												{
// 													gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Chg;
// 
// 													gEvChargeInfo.u8GunSelect = 0;
// 
// 													gs_IMMU2_LCCU_Mng.u8GunTaskType[GUN_NO_A] = GUN_TASK_TYPE_Chg2Ev;
// 													//gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_A] = ES_BATT_TASK_TYPE_Batt2Ev;
// 
// 													A_SetChargeSTART_Cmd(gEvChargeInfo.u8_A_LogInMother);
// 												}
// 												else
// 												{
// 													gs_DisplayCtrl.u8WaitSpearOkTimeout = WAIT_SPEAR_OK_TIME_MAX;
// 													gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_WaitSpearOk;
// 												}
// 											}
// 											/*else
// 											{
// 												gs_DisplayCtrl.u8WaitChgModeSelTimeout = WAIT_CHG_MODE_SEL_TIME_MAX;
// 												gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_ChgModeSel;
// 												gEvChargeInfo.u8ChargeMode[GUN_NO_A] = 0;
// 											}*/
// 										}	
// 										gTempChargeArgs.u8ChargeMode = 0;
// 										gTempChargeArgs.fTimeArgs = 30;
// 										gTempChargeArgs.fEnergeArgs = 20;
// 										gTempChargeArgs.fMoney = 20;
// 										gTempChargeArgs.fSOCArgs = 80;
// 									}							
								}
								else  //取消
								{
									gs_DisplayCtrl.u8CurrPageNo = gs_DisplayCtrl.u8PasswordFailReturnPage;
								}
								gs_DisplayCtrl.u8IsNewPage = TRUE;
							}
							break;

						//按键：进入参数设置的按键
						case TOPWAY_16BIT_ADDR_KEY_ENTER_PARAS:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{
								ucScreemNo = GetTopwayScreemTableIdx(gs_DisplayCtrl.u8CurrPageNo);
								if(TopwayScreem[ucScreemNo].u8_IsHighPriority)  //已进入了维护屏（包括密码屏）中，不再响应此键，因为可能用户快速点击了两个键，前一个屏还没切进去又来按键
								{
								}
								else 
								{
									gs_DisplayCtrl.u8PasswordOkJumpPage = TOPWAY_SCREEM_PAGE_PARAS_SYS1;
									gs_DisplayCtrl.u8PasswordFailReturnPage = gs_DisplayCtrl.u8CurrPageNo;
									gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_PASSWORD_INPUT;
									gs_DisplayCtrl.u8PasswordPageOverTimer = 1;
									gs_DisplayCtrl.u8IsNewPage = TRUE;
								}
							}
							break;

						//按键：从系统信息查询界面返回   从参数设置界面返回也是同一键
						case TOPWAY_16BIT_ADDR_KEY_RETURN_FROM_SYS_INFO:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{
								ucScreemNo = GetTopwayScreemTableIdx(gs_DisplayCtrl.u8CurrPageNo);
								if(TopwayScreem[ucScreemNo].u8_IsHighPriority == 0)  //已已退出了维护屏（包括密码屏）中，不再响应此键，因为可能用户快速点击了两个键，前一个屏还没切进去又来按键
								{
								}
								else
								{
									gs_DisplayCtrl.u8CurrPageNo = gs_DisplayCtrl.u8PasswordFailReturnPage;
									gs_DisplayCtrl.u8IsNewPage = TRUE;	
								}
							}
							break;

						//按键：进入系统信息查询界面
						case TOPWAY_16BIT_ADDR_KEY_ENTER_SYS_INFO:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{
								ucScreemNo = GetTopwayScreemTableIdx(gs_DisplayCtrl.u8CurrPageNo);
								if(TopwayScreem[ucScreemNo].u8_IsHighPriority)  //已进入了维护屏（包括密码屏）中，不再响应此键，因为可能用户快速点击了两个键，前一个屏还没切进去又来按键
								{
								}
								else
								{
									gs_DisplayCtrl.u8PasswordFailReturnPage = gs_DisplayCtrl.u8CurrPageNo;
									if(u16Value == TOPWAY_SCREEM_PAGE_ALARMS_SYS)
									{
										gs_DisplayCtrl.u8CurrPageNo = u16Value;
									}
									else
									{
										gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_DETAILS_A1;
									}
									gs_DisplayCtrl.u8IsNewPage = TRUE;	
								}
							}
							break;

						//按键： 充电记录中的前一条、后一条 按键
						case TOPWAY_16BIT_ADDR_KEY_RECORD_NEXT:
							GetRecordDeal(GetRecordDeal_Next);
							gs_DisplayCtrl.u16PageReflashDoneFlag = 0;
							gs_DisplayCtrl.u16DisplayReflashDelay = 0;
							break;
						case TOPWAY_16BIT_ADDR_KEY_RECORD_PREV:
							GetRecordDeal(GetRecordDeal_Prev);
							gs_DisplayCtrl.u16PageReflashDoneFlag = 0;
							gs_DisplayCtrl.u16DisplayReflashDelay = 0;
							break;

						//校准启动按钮
						case TOPWAY_16BIT_ADDR_KEY_ADJUST_GUN_A:
							/*if((gEvChargeInfo.u8IsSingleGunMode == FALSE) || (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_A_GUN))*/
							{
								//只有A/B都空闲待机态才允许校准
								if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_Standby) 
									&& (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_Standby))
								{
									ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_AdjustMode_NUM, &untemp, NULL, BY_E2PROM);
									ucAdjustMode = untemp.chtemp[0];

									if(gEvChargeInfo.u8_AutoTestStep > 0)
									{
										//自动校准中，不处理
									}
									else if(ucAdjustMode == 0)  //自动测试
									{
										//若在校准中，则不处理
										if ((gEvChargeInfo.u8_A_TestMode == TRUE) || (gEvChargeInfo.u8_B_TestMode == TRUE))
										{

										}
										else
										{
											gEvChargeInfo.u8_AutoTestStep = 1;
											gEvChargeInfo.u8_A_TestMode = TRUE;
										}
									}
									else  //手动测试
									{
										//若B还在测试，则关闭
										if (gEvChargeInfo.u8_B_TestMode == TRUE)
										{
											gEvChargeInfo.u8_ManualTestStep = 0;
											gEvChargeInfo.u8_B_TestMode = FALSE;
										//gs_CANComm.u8GRP_RectOnOff[GRP_F_DCDC] = RECT_OFF;
											gs_CANComm.fSetRectCtlStatus(GRP_2_ACDC, RECT_OFF);
											//__B_DC_MCCB_OFF();
											BSP_DO_Ctrl(NEWDO2, DO_OFF);
										}

										if (gEvChargeInfo.u8_A_TestMode == TRUE)
										{
											gEvChargeInfo.u8_ManualTestStep = 0;
											gEvChargeInfo.u8_A_TestMode = FALSE;
											//gs_CANComm.fSetRectCtlStatus(GRP_A, RECT_OFF);
										gs_CANComm.u8GRP_RectOnOff[GRP_1_ACDC] = RECT_OFF;
											//__A_DC_MCCB_OFF();
											BSP_DO_Ctrl(NEWDO1, DO_OFF);
										}
										else
										{
											gEvChargeInfo.u8_A_TestMode = TRUE;
											gEvChargeInfo.u8_ManualTestStep = 1;
											//20180322 lkf 移到 AutoAdjustDeal()函数中处理
											//gEvChargeInfo.fBattSet_A_Volt = 0;
											//gEvChargeInfo.fBattSet_B_Volt = 0;
											//gs_CANComm.fSetRectCtlVoltCurr(GRP_A, gEVBms_1_Info.fBMSRequireVolt, gEVBms_1_Info.fBMSRequireCurr);
											//gs_CANComm.fSetRectCtlStatus(GRP_A, RECT_ON);
											//__A_DC_MCCB_ON();
										}
									}
								}
							}
							break;
						case TOPWAY_16BIT_ADDR_KEY_ADJUST_GUN_B:

							/*if((gEvChargeInfo.u8IsSingleGunMode == FALSE) || (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_B_GUN)) //双枪模式才支持*/
							{
								//只有A/B都空闲待机态才允许校准
								if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_Standby) 
									&& (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_Standby))
								{
									ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_AdjustMode_NUM, &untemp, NULL, BY_E2PROM);
									ucAdjustMode = untemp.chtemp[0];								

									if(gEvChargeInfo.u8_AutoTestStep > 0)
									{
										//自动校准中，不处理
									}
									else if(ucAdjustMode == 0)  //自动测试
									{
										//若在校准中，则不处理
										if ((gEvChargeInfo.u8_A_TestMode == TRUE) || (gEvChargeInfo.u8_B_TestMode == TRUE))
										{

										}
										else
										{
											gEvChargeInfo.u8_AutoTestStep = 1;
											gEvChargeInfo.u8_B_TestMode = TRUE;
										}
									}
									else  //手动测试
									{
										//若A还在测试，则关闭
										if (gEvChargeInfo.u8_A_TestMode == TRUE)
										{
											gEvChargeInfo.u8_ManualTestStep = 0;
											gEvChargeInfo.u8_A_TestMode = FALSE;
											gs_CANComm.fSetRectCtlStatus(GRP_A, RECT_OFF);
											//gs_CANComm.u8GRP_RectOnOff[GRP_E_DCDC] = RECT_OFF;
											//__A_DC_MCCB_OFF();
											BSP_DO_Ctrl(NEWDO1, DO_OFF);
										}
										if (gEvChargeInfo.u8_B_TestMode == TRUE)
										{						
											gEvChargeInfo.u8_ManualTestStep = 0;
											gEvChargeInfo.u8_B_TestMode = FALSE;
											gs_CANComm.fSetRectCtlStatus(GRP_B, RECT_OFF);
											//gs_CANComm.u8GRP_RectOnOff[GRP_F_DCDC] = RECT_OFF;
											//__B_DC_MCCB_OFF();
											BSP_DO_Ctrl(NEWDO2, DO_OFF);
										}
										else
										{
											gEvChargeInfo.u8_B_TestMode = TRUE;
											gEvChargeInfo.u8_ManualTestStep = 1;
											//20180322 lkf 移到 AutoAdjustDeal()函数中处理
											//gEvChargeInfo.fBattSet_A_Volt = 0;
											//gEvChargeInfo.fBattSet_B_Volt = 0;
											//gs_CANComm.fSetRectCtlVoltCurr(GRP_B, gEVBms_2_Info.fBMSRequireVolt, gEVBms_2_Info.fBMSRequireCurr);
											//gs_CANComm.fSetRectCtlStatus(GRP_B, RECT_ON);
											//__B_DC_MCCB_ON();
										}
									}
								}
							}
							break;

						case TOPWAY_16BIT_ADDR_KEY_CLEAR_RECORD:
							if(gs_DisplayCtrl.u8UserLevel >= 3)
							{
								gFlashOptInfo.u8CleanRecordsFlag = TRUE;
							}							
							break;

						case TOPWAY_16BIT_ADDR_KEY_REFLASH_SCREEM:
							gs_DisplayCtrl.u8IsNewPage = TRUE;
							if(gs_DisplayCtrl.IsScreemSaving == TRUE)   //已经在屏保状态
							{						
								if(gs_DisplayCtrl.u8CurrPageNo == TOPWAY_SCREEM_PAGE_SCREEM_SAVER)
								{
									if(gs_DisplayCtrl.u8ScreemSavingReturnPage != TOPWAY_SCREEM_PAGE_SCREEM_SAVER)
									{
										gs_DisplayCtrl.u8CurrPageNo = gs_DisplayCtrl.u8ScreemSavingReturnPage;
									}
									else
									{
// 										if((gEvChargeInfo.u8IsSingleGunMode == TRUE) && (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_A_GUN))
// 										{
// 											gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_INIT_Single;
// 										}
// 										else if ((gEvChargeInfo.u8IsSingleGunMode == TRUE) && (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_B_GUN))
// 										{
// 											gs_DisplayCtrl.u8CurrPageNo = TOPWAY_B_SCREEM_PAGE_INIT_Single;
// 										}
// 										else
										{
											gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_INIT;
										}									
									}
								}
							}
							break;		
						case TOPWAY_16BIT_ADDR_KEY_UNLOCK_SPEAR_A:
							
							break;
						case TOPWAY_16BIT_ADDR_KEY_UNLOCK_SPEAR_B:
							
							break;

						case TOPWAY_16BIT_ADDR_KEY_RETURN_FROM_CARD_INFY:
							if(gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_BrushWaitHostConfirm)
								uctemp = GUN_NO_A;
							else if(gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_BrushWaitHostConfirm)
								uctemp = GUN_NO_B;
							else
								uctemp = 0xff;

							if(uctemp <= GUN_NO_B)
							{
								gs_DisplayCtrl.u8GunDispStat[uctemp] = GUN_DISP_STAT_Standby;
								gs_DisplayCtrl.u8IsDispCardLeaveMoney = 0;
								gBrushInfo.u8AccountRequestStep = ACCOUNT_REQUEST_STEP_Init;
								gBrushInfo.u8BrushWaitHostConfirmTimeout = 0;
							}
							break;

						case TOPWAY_16BIT_ADDR_KEY_CARD_WRITE_FAIL_QUIT:
							/*if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_Stop) && (gs_DisplayCtrl.u8NeedBrushJiesuan[0] == 1))
							{		
								gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 0;
							}
							if((gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_Stop) && (gs_DisplayCtrl.u8NeedBrushJiesuan[1] == 1))
							{		
								gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 0;
							}*/
							//20220627 lkf 借用来读取flash事件

							if(gs_HistEventTable.u16TotalNum > gs_HistoryEventMng.u16HistoryEventNum)
							{
								gs_HistAlmWriteToUdisc.u8ReadFromFlash_Flag = 1;	
							}

							break;
						case TOPWAY_16BIT_ADDR_KEY_CLEAR_RESET_RECORD:
							if(gs_DisplayCtrl.u8UserLevel >= 3)
							{
								gs_E2PROM.fSetClearEventFlag();
							}
							break;
						case TOPWAY_16BIT_ADDR_KEY_RESET:
							if((gs_DisplayCtrl.u8UserLevel >= 3)
								&&(gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == STOP_CHARGE_VAL)
								&& (gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == STOP_CHARGE_VAL))
							{
								/*if(gSelfTest.StopFeedDogReason == 0)
								{
									gSelfTest.StopFeedDogReason = 2;
									gSelfTest.u32DogBit_Bak = gSelfTest.u32DogBit;
									gs_E2PROM.fSetHistoryEventUpdate();
								}*/
								dv_Output.fOpenBeep();
								dv_Output.fOpenBeep();
								OSTimeDly(700);
								OSTimeDly(700);
								OSTimeDly(700);
								OSTimeDly(700);
								OSTimeDly(700);
								Trap();
							}
							break;

						case TOPWAY_16BIT_ADDR_KEY_AC_START_STOP:
							if (gEvChargeInfo.u8AC_StartCMD == TRUE)
							{
								gEvChargeInfo.u8AC_StartCMD = FALSE;
							}
							else
							{
								gEvChargeInfo.u8AC_StartCMD = TRUE;
							}
							break;

						case TOPWAY_16BIT_ADDR_KEY_ENTER_AC_MENU:
							//页面确认后才响应
							if(gs_DisplayCtrl.u8ChkPageFlag == PAGE_CHK_FLAG_Ok)
							{
								ucScreemNo = GetTopwayScreemTableIdx(gs_DisplayCtrl.u8CurrPageNo);
								if(TopwayScreem[ucScreemNo].u8_IsHighPriority)  //已进入了维护屏（包括密码屏）中，不再响应此键，因为可能用户快速点击了两个键，前一个屏还没切进去又来按键
								{
								}
								else
								{
									gs_DisplayCtrl.u8PasswordFailReturnPage = gs_DisplayCtrl.u8CurrPageNo;
									if(u16Value == TOPWAY_SCREEM_PAGE_ALARMS_SYS)
									{
										gs_DisplayCtrl.u8CurrPageNo = u16Value;
									}
									else
									{
										gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_AC_CHARGER;
									}
									gs_DisplayCtrl.u8IsNewPage = TRUE;	
								}
							}	
							break;

						default:                                                 
							break;

					}  //end： switch(u32SigAddr)
				
				}			
			}

			//2. 0x77命令 键盘输入数据返回 -32bits
			else if((gMdbusRcvInfo[ArryIdx].u16ReceivLenth == 14)
				&& (gMdbusRcvInfo[ArryIdx].uBuf[1] == 0x77))
			{
				gs_DisplayCtrl.u16ScreemSaverDelay = 0;  //屏保计时器清0

				//u32SigAddr = ((unsigned long)gMdbusRcvInfo[ArryIdx].uBuf[2]<<24) + ((unsigned long)gMdbusRcvInfo[ArryIdx].uBuf[3]<<16) + ((unsigned long)gMdbusRcvInfo[ArryIdx].uBuf[4]<<8) + (gMdbusRcvInfo[ArryIdx].uBuf[5]);
				untemp.chtemp[3] = gMdbusRcvInfo[ArryIdx].uBuf[6];
				untemp.chtemp[2] = gMdbusRcvInfo[ArryIdx].uBuf[7];
				untemp.chtemp[1] = gMdbusRcvInfo[ArryIdx].uBuf[8];
				untemp.chtemp[0] = gMdbusRcvInfo[ArryIdx].uBuf[9];

				//充电金额设置
				 if(u32SigAddr == TOPWAY_32BIT_ADDR_SET_CHG_MONEY)
				{
					gTempChargeArgs.fMoney = untemp.ftemp;	
				}
				//参数设置
				else if(((u32SigAddr >= TOPWAY_32BIT_FEE_PARA_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_32BIT_FEE_PARA_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_32BIT_FEE2_PARA_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_32BIT_FEE2_PARA_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_32BIT_PARA_SYS_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_32BIT_PARA_SYS_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_32BIT_PARA_A_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_32BIT_PARA_A_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_32BIT_PARA_B_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_32BIT_PARA_B_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_32BIT_PARA_3_START) && (u32SigAddr <= TOPWAY_32BIT_PARA_3_END)))
				{
					TopwaySetCfgSignal(u32SigAddr, untemp.u32Value, 1);
				}
				//校准命令
				else if(((u32SigAddr >= TOPWAY_32BIT_ADJUST_A_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_32BIT_ADJUST_A_ADDR_RANGE_END))
					|| ((u32SigAddr >= TOPWAY_32BIT_ADJUST_B_ADDR_RANGE_START) && (u32SigAddr <= TOPWAY_32BIT_ADJUST_B_ADDR_RANGE_END)))
				{
					if(gEvChargeInfo.u8_AutoTestStep > 0)
					{
						//自动校准期间不允许修改
					}
					//只有A/B都空闲待机态才允许校准
					else if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_Standby) && (gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_Standby))
					{
						TopwaySetAdjustSignal(u32SigAddr, untemp.u32Value);
					}
				}
			}
			//3. 0x32命令 查询页面返回
			else if((gMdbusRcvInfo[ArryIdx].u16ReceivLenth == 8)
				&& (gMdbusRcvInfo[ArryIdx].uBuf[1] == 0x32))
			{
				if((gs_DisplayCtrl.IsScreemSaving == TRUE)
					|| (gs_DisplayCtrl.u8CurrPageNo == TOPWAY_SCREEM_PAGE_SCREEM_SAVER))
				{	
					//屏保计时器不清零
				}
				else 
				{
					gs_DisplayCtrl.u16ScreemSaverDelay = 0;  //屏保计时器清0
				}

				u16Value = ((unsigned short)gMdbusRcvInfo[ArryIdx].uBuf[2]<<8) + (gMdbusRcvInfo[ArryIdx].uBuf[3]);
				if(u16Value != gs_DisplayCtrl.u8CurrPageNo)
				{
					gs_DisplayCtrl.u8IsNewPage = TRUE;
					gs_DisplayCtrl.u8ChkPageFlag = PAGE_CHK_FLAG_NeedChk;
				}
				else
				{
					gs_DisplayCtrl.u8ChkPageFlag = PAGE_CHK_FLAG_Ok;
				}
			}
			//4. 0x30命令 握手数据返回
			else if((gMdbusRcvInfo[ArryIdx].u16ReceivLenth == 23)
				&& (gMdbusRcvInfo[ArryIdx].uBuf[1] == 0x30))
			{
				gs_DisplayCtrl.u16ScreemSaverDelay = 0;  //屏保计时器清0

				gs_DisplayCtrl.IsScreemOk = TRUE;
			}


			gMdbusRcvInfo[ArryIdx].uReceivFlag = FALSE;
		}
	}
}


void EnterChgModeSelPage(unsigned char u8GunNo, unsigned char LoginMode, float fCardLeaveMoney)
{
	//禁止模式选择则直接进入充电
	//if(gEvChargeInfo.u8ChgModeSelEn == 1) 
	{							
// 		if(u8GunNo == GUN_NO_A)
// 		{
// 			gEvChargeInfo.u8_A_LogInMother = LoginMode;		
// 			gEvChargeInfo.u8ChargeMode[GUN_NO_A] = 0;
// 			gTempChargeArgs.u8ChargeMode = 0;
// 			gEvChargeInfo.fCardLeaveMoney[0] = fCardLeaveMoney;	
// 			if ((gEvChargeInfo.u8EV_SpearPst[GUN_NO_A] == SPEAR_ON_LINE)
// 				&& (gs_GunCtrlReq[GUN_NO_A].GunChgPhase == GunChgPhase_Init))
// 			{
// 				gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Chg;
// 
// 				gEvChargeInfo.u8GunSelect = 0;
// 
// 				gs_IMMU2_LCCU_Mng.u8GunTaskType[GUN_NO_A] = GUN_TASK_TYPE_Chg2Ev;
// 				//gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_A] = ES_BATT_TASK_TYPE_Batt2Ev;
// 
// 				A_SetChargeSTART_Cmd(gEvChargeInfo.u8_A_LogInMother);
// 			}
// 			else
// 			{
// 				gs_DisplayCtrl.u8WaitSpearOkTimeout = WAIT_SPEAR_OK_TIME_MAX;
// 				gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_WaitSpearOk;
// 			}
// 		}
// 		else //if(u8GunNo == GUN_NO_B)
// 		{
// 			gEvChargeInfo.u8_B_LogInMother = LoginMode;		
// 			gEvChargeInfo.u8ChargeMode[GUN_NO_B] = 0;
// 			gTempChargeArgs.u8ChargeMode = 0;
// 			gEvChargeInfo.fCardLeaveMoney[1] = fCardLeaveMoney;	
// 			if ((gEvChargeInfo.u8EV_SpearPst[GUN_NO_B] == SPEAR_ON_LINE)
// 				&& (gs_GunCtrlReq[GUN_NO_B].GunChgPhase == GunChgPhase_Init))
// 			{
// 				gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Chg;
// 
// 				gEvChargeInfo.u8GunSelect = 0;
// 
// 				gs_IMMU2_LCCU_Mng.u8GunTaskType[GUN_NO_B] = GUN_TASK_TYPE_Chg2Ev;
// 				//gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_B] = ES_BATT_TASK_TYPE_Batt2Ev;
// 
// 				B_SetChargeSTART_Cmd(gEvChargeInfo.u8_B_LogInMother);
// 			}
// 			else
// 			{
// 				gs_DisplayCtrl.u8WaitSpearOkTimeout = WAIT_SPEAR_OK_TIME_MAX;
// 				gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_WaitSpearOk;
// 			}
// 		}
	}//end: if(gEvChargeInfo.u8ChgModeSelEn == 1) 
	
	/*else
	{
		gs_DisplayCtrl.u8WaitChgModeSelTimeout = WAIT_CHG_MODE_SEL_TIME_MAX;
		gTempChargeArgs.u8ChargeMode = 0;
		gTempChargeArgs.fTimeArgs = 30;
		gTempChargeArgs.fEnergeArgs = 20;
		gTempChargeArgs.fMoney = 20;
		gTempChargeArgs.fSOCArgs = 80;

		if((LoginMode == EV_ENTER_IC_CARD)||(LoginMode == EV_ENTER_IC_CARD_NET))
			gs_DisplayCtrl.u8IsDispCardLeaveMoney = 1;
		else
			gs_DisplayCtrl.u8IsDispCardLeaveMoney = 0;

		if(u8GunNo == GUN_NO_A)
		{
			gEvChargeInfo.u8GunSelect = 1;
			gEvChargeInfo.u8_A_LogInMother = LoginMode;		
			gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_ChgModeSel;
			gEvChargeInfo.u8ChargeMode[GUN_NO_A] = 0;
			gEvChargeInfo.fCardLeaveMoney[0] = fCardLeaveMoney;		
		}
		else
		{
			gEvChargeInfo.u8GunSelect = 2;
			gEvChargeInfo.u8_B_LogInMother = LoginMode;
			gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_ChgModeSel;
			gEvChargeInfo.u8ChargeMode[GUN_NO_B] = 0;
			gEvChargeInfo.fCardLeaveMoney[1] = fCardLeaveMoney;
		}
	}*/
}



//gEvChargeInfo.u8GunSelectTimeOut
#define GUN_SELECT_TIMEOUT		60  //S
void GunSelectTimeOutDeal(void)
{
	static unsigned char su8GunSelectStat = 0xff;

	if(gs_DisplayCtrl.u8CurrPageNo == TOPWAY_SCREEM_PAGE_INIT)
	{
		if((gEvChargeInfo.u8GunSelect == 0) || (gEvChargeInfo.u8GunSelect != su8GunSelectStat))
		{
			gEvChargeInfo.u8GunSelectTimeOut = 0;
			su8GunSelectStat = gEvChargeInfo.u8GunSelect;
		}
		else if((gs_DisplayCtrl.u8GunDispStat[0] != GUN_DISP_STAT_Standby)
			|| (gs_DisplayCtrl.u8GunDispStat[1] != GUN_DISP_STAT_Standby))
		{
			gEvChargeInfo.u8GunSelectTimeOut = 0;
		}
		else if((gs_EthAppCommMng.u8OrderChargeCMD[0] == TRUE) || (gs_EthAppCommMng.u8OrderChargeCMD[1] == TRUE))
		{
			gEvChargeInfo.u8GunSelectTimeOut = 0;
		}
		else if ((gEvChargeInfo.u8SysAlm == TRUE) || (gEvChargeInfo.u8Gun_Alm[GUN_NO_A] == TRUE) || (gEvChargeInfo.u8Gun_Alm[GUN_NO_B] == TRUE))
		{
			gEvChargeInfo.u8GunSelectTimeOut = 0;
		}		
		else if(gEvChargeInfo.u8GunSelectTimeOut == 0)
		{
			gEvChargeInfo.u8GunSelectTimeOut = 1;
		}
		else if(gEvChargeInfo.u8GunSelectTimeOut > GUN_SELECT_TIMEOUT)
		{
			gEvChargeInfo.u8GunSelectTimeOut = 0;
			gEvChargeInfo.u8GunSelect = 0;
		}
	}
	else
	{
		gEvChargeInfo.u8GunSelectTimeOut = 0;
	}
}

void PlugInChargingDisplayDela(UINT8 u8GunNo)
{
	if(u8GunNo ==  1)
	{
		gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Chg;
		gEvChargeInfo.u8_A_LogInMother = EV_ENTER_admin;
		gEvChargeInfo.u8GunSelect = 0;
	}
	if(u8GunNo ==  2)
	{
		gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Chg;
		gEvChargeInfo.u8GunSelect = 1;
		gEvChargeInfo.u8_B_LogInMother = EV_ENTER_admin;
	}
}
void TopwayPageDeal(void)
{
	unsigned char i, ucGunNo;
	static unsigned char s_ucCurrentPageNo = 0;

	//密码输入界面超时判断
	if(gs_DisplayCtrl.u8PasswordPageOverTimer > 0)
	{
		if(gs_DisplayCtrl.u8PasswordPageOverTimer > PASSWORD_PAGE_TIME_OUT)
		{
			gs_DisplayCtrl.u8CurrPageNo = gs_DisplayCtrl.u8PasswordFailReturnPage;
			gs_DisplayCtrl.u8IsNewPage = TRUE;
			gs_DisplayCtrl.u8PasswordPageOverTimer = 0;
			gs_DisplayCtrl.u8PasswordIsOk = 0;
		}
	}

	//插枪检查
//	if(gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_WaitSpearOk)
//	{
// 		if ((gEvChargeInfo.u8EV_SpearPst[GUN_NO_A] == SPEAR_ON_LINE)
// 			&& (gs_GunCtrlReq[GUN_NO_A].GunChgPhase == GunChgPhase_Init))
// 		{
// 			gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Chg;
// 			if(gEvChargeInfo.u8_A_LogInMother == EV_ENTER_IC_CARD)
// 			{
// 				gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 1;
// 			}
// 			gEvChargeInfo.u8GunSelect = 0;
// 
// 			gs_IMMU2_LCCU_Mng.u8GunTaskType[GUN_NO_A] = GUN_TASK_TYPE_Chg2Ev;
// 			//gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_A] = ES_BATT_TASK_TYPE_Batt2Ev;
// 
// 			A_SetChargeSTART_Cmd(gEvChargeInfo.u8_A_LogInMother);
// 			
// 			/*__AC_MIN_MCCB_ON();
// 			__AC_MIN_MCCB_ON();
// 			gEvChargeInfo.u8_Indicate_A_STARTChargeFlag = TRUE;
// 			if (gEvChargeInfo.u8_ParamTotalMCCBDelay == 0)
// 			{
// 				gEvChargeInfo.u8_A_STARTChargeDelay = POWER_ON_RELACATION_12_TIME_OUT;
// 			}
// 			else
// 			{
// 				gEvChargeInfo.u8_A_STARTChargeDelay = POWER_ON_RELACATION_TIMER_1_START;
// 			}*/
// 		}
// 		else if(gs_DisplayCtrl.u8WaitSpearOkTimeout == 0)
// 		{
// 			gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Standby;
// 			gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 0;
// 			gEvChargeInfo.u8GunSelect = 0;
// 		}
// 	}
// 	else if(gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_WaitSpearOk)
// 	{
// 		if ((gEvChargeInfo.u8EV_SpearPst[GUN_NO_B] == SPEAR_ON_LINE)
// 			&& (gs_GunCtrlReq[GUN_NO_B].GunChgPhase == GunChgPhase_Init))
// 		{
// 			gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Chg;
// 			if(gEvChargeInfo.u8_B_LogInMother == EV_ENTER_IC_CARD)
// 			{
// 				gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 1;
// 			}
// 			gEvChargeInfo.u8GunSelect = 0;
// 
// 			gs_IMMU2_LCCU_Mng.u8GunTaskType[GUN_NO_B] = GUN_TASK_TYPE_Chg2Ev;
// 			//gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_A] = ES_BATT_TASK_TYPE_Batt2Ev;
// 
// 			B_SetChargeSTART_Cmd(gEvChargeInfo.u8_B_LogInMother);
// 
// 			/*__AC_MIN_MCCB_ON();
// 			__AC_MIN_MCCB_ON();		
// 			gEvChargeInfo.u8_Indicate_B_STARTChargeFlag = TRUE;
// 			if (gEvChargeInfo.u8_ParamTotalMCCBDelay == 0)
// 			{
// 				gEvChargeInfo.u8_B_STARTChargeDelay = POWER_ON_RELACATION_12_TIME_OUT;
// 			}
// 			else
// 			{
// 				gEvChargeInfo.u8_B_STARTChargeDelay = POWER_ON_RELACATION_TIMER_1_START;
// 			}*/
// 		}
// 		else if(gs_DisplayCtrl.u8WaitSpearOkTimeout == 0)
// 		{
// 			gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Standby;
// 			gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 0;
// 			gEvChargeInfo.u8GunSelect = 0;
// 		}
//	}	

	//充电模式选择页面超时检查
	if(gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_ChgModeSel)
	{
		if(gs_DisplayCtrl.u8WaitChgModeSelTimeout == 0)
		{
			gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Standby;
			gs_DisplayCtrl.u8NeedBrushJiesuan[0] = 0;
			gEvChargeInfo.u8GunSelect = 0;
			gs_DisplayCtrl.u8IsDispCardLeaveMoney = 0;
		}
		gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[0] = 0;   //进入选择界面了，可以把钱不够的提示信息清掉
	}
	else if(gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_ChgModeSel)
	{		
		if(gs_DisplayCtrl.u8WaitChgModeSelTimeout == 0)
		{
			gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Standby;
			gs_DisplayCtrl.u8NeedBrushJiesuan[1] = 0;
			gEvChargeInfo.u8GunSelect = 0;
			gs_DisplayCtrl.u8IsDispCardLeaveMoney = 0;
		}
		gs_DisplayCtrl.u8CardMoneyNotEnoughNoticeDelay[1] = 0; //进入选择界面了，可以把钱不够的提示信息清掉
	}	

	//卡验证界面超时处理
	if((gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_BrushWaitHostConfirm)
		||(gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_BrushWaitHostConfirm))
	{
		//注释：同时只能一把枪进入刷卡确认页面
		if(gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_BrushWaitHostConfirm)
			ucGunNo = GUN_NO_A;
		else 
			ucGunNo = GUN_NO_B;

		//各阶段超时判断
		if(
			((gBrushInfo.u8AccountRequestStep == ACCOUNT_REQUEST_STEP_1) && (gBrushInfo.u8BrushWaitHostConfirmTimeout > BRUSH_WAIT_HOST_CONFIRM_STEP_1_TIME))
			|| ((gBrushInfo.u8AccountRequestStep == ACCOUNT_REQUEST_STEP_2) && (gBrushInfo.u8BrushWaitHostConfirmTimeout > BRUSH_WAIT_HOST_CONFIRM_STEP_2_TIME))
			|| ((gBrushInfo.u8AccountRequestStep == ACCOUNT_REQUEST_STEP_4) && (gBrushInfo.u8BrushWaitHostConfirmTimeout > BRUSH_WAIT_HOST_CONFIRM_STEP_4_TIME))
			)
		{
			gs_DisplayCtrl.u8GunDispStat[ucGunNo] = GUN_DISP_STAT_Standby;

			gs_DisplayCtrl.u8IsDispCardLeaveMoney = 0;

			gBrushInfo.u8AccountRequestStep = ACCOUNT_REQUEST_STEP_Init;
			gBrushInfo.u8BrushWaitHostConfirmTimeout = 0;
		}
		else if(gBrushInfo.u8AccountRequestStep == ACCOUNT_REQUEST_STEP_3)  //后台回复金额满足要求，可以启动充电
		{			
			EnterChgModeSelPage(ucGunNo, EV_ENTER_IC_CARD_NET, gBrushInfo.fCardMoneyFormNet);
			gBrushInfo.u8AccountRequestStep = ACCOUNT_REQUEST_STEP_Init;
			gBrushInfo.u8BrushWaitHostConfirmTimeout = 0;
		}
	}

	//刷卡启动或停止
// 	if (Pop_BrushSem())
// 	{
// 		gs_DisplayCtrl.u16ScreemSaverDelay = 0;  //屏保计时器清0
// 
// 		BrushStartOrStopDeal();
// 	}

	//网络启动充电，需要进入充电显示界面
	if(gEvChargeInfo.u8A_NetStartSem) 
	{
		gEvChargeInfo.u8A_NetStartSem = FALSE;
		gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Chg;
	}
	if(gEvChargeInfo.u8B_NetStartSem) 
	{
		gEvChargeInfo.u8B_NetStartSem = FALSE;
		gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Chg;
	}

	//网络停止充电 或 CAN任务因故障 或 充满而停止充电
// 	if (Pop_A_StopSem())
// 	{
// 		if(gs_DisplayCtrl.u8GunDispStat[0] == GUN_DISP_STAT_Chg)
// 		{
// 			gs_DisplayCtrl.u8GunDispStat[0] = GUN_DISP_STAT_Stop;
// 		}
// 	}
// 	if (Pop_B_StopSem())
// 	{
// 		if(gs_DisplayCtrl.u8GunDispStat[1] == GUN_DISP_STAT_Chg)
// 		{
// 			gs_DisplayCtrl.u8GunDispStat[1] = GUN_DISP_STAT_Stop;
// 		}
// 	}

	//双枪待机画面选中某枪超过一定时间后若还没进行下一步,则取消选中
	GunSelectTimeOutDeal();

	//正常充电过程 屏序号自动跳转处理	
	if(TopwayScreem[s_ucCurrentPageNo].u8_PageNo != gs_DisplayCtrl.u8CurrPageNo)  //检查是否需要重新查找TopwayScreem[]， s_ucCurrentPageNo用于快速定位TopwayScreem[]下标
	{
		for(i = 0; i < s_ucTopwayScreemPageNum; i++)
		{
			if(TopwayScreem[i].u8_PageNo == gs_DisplayCtrl.u8CurrPageNo)
			{
				s_ucCurrentPageNo = i;
				break;
			}
		}
	}
	if(TopwayScreem[s_ucCurrentPageNo].u8_IsHighPriority == 0)  //0-非维护界面,由主机自己更新屏号；1-维护界面，由屏自己更新屏号
	{
		gs_DisplayCtrl.u8CurrPageNo = GetGunAGunBDispPageNo();
	}		
}

//////////////////
//屏保处理
//////////////////
void TopwayScreemSaverDeal(void)
{
	static unsigned char s_u8SentCount_EnterSaver = 0;
	static unsigned char s_u8SentCount_QuitSaver = 0;
	static unsigned char s_u8CloseBlackLightFlag = 0;
	//以下几种状态下不屏保
	if ((gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == START_CHARGE_VAL)
		|| (gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == START_CHARGE_VAL)
		|| (gs_DisplayCtrl.u8PasswordPageOverTimer > 0)   //密码输入界面
		|| (gs_DisplayCtrl.u8WaitSpearOkTimeout > 0)	  //等待枪插入界面
		|| (gs_DisplayCtrl.u8WaitChgModeSelTimeout > 0)	  //充电模式选择界面
		)
	{
		gs_DisplayCtrl.u16ScreemSaverDelay = 0;
		s_u8SentCount_EnterSaver = 0;
	}

	if(gs_DisplayCtrl.u16ScreemSaverDelay >= SCREEM_SAVER_DELAY_TIME)
	{
		gs_DisplayCtrl.u16ScreemSaverDelay = SCREEM_SAVER_DELAY_TIME;
		gs_DisplayCtrl.u8PasswordPageOverTimer = 0;   //加个清0操作，其实可以不用

		if(gs_DisplayCtrl.IsScreemSaving == FALSE)
		{
			if(gs_DisplayCtrl.u8TxIsBusy == FALSE) 
			{				
				gs_DisplayCtrl.u8TxIsBusy = TRUE;

				s_u8CloseBlackLightFlag = TRUE;
				Cmd0x5FWriteBacklight(BackLightOff);

				s_u8SentCount_QuitSaver = 0;
				s_u8SentCount_EnterSaver++;
				if(s_u8SentCount_EnterSaver > 1)
				{
					s_u8SentCount_EnterSaver = 0;
					gs_DisplayCtrl.IsScreemSaving = TRUE;
				}
				
				if(gs_DisplayCtrl.u8CurrPageNo != TOPWAY_SCREEM_PAGE_SCREEM_SAVER)
				{
					gs_DisplayCtrl.u8ScreemSavingReturnPage = gs_DisplayCtrl.u8CurrPageNo;
				}
				gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_SCREEM_SAVER;
				gs_DisplayCtrl.u8IsNewPage = TRUE;
			}
		}
	}
	else 
	{
		if((gs_DisplayCtrl.IsScreemSaving == TRUE)
			|| (s_u8CloseBlackLightFlag == TRUE))
		{
			if(gs_DisplayCtrl.u8TxIsBusy == FALSE) 
			{				
				gs_DisplayCtrl.u8TxIsBusy = TRUE;

				Cmd0x5FWriteBacklight(BackLightOn);				

				s_u8SentCount_QuitSaver++;
				s_u8SentCount_EnterSaver = 0;
				if(s_u8SentCount_QuitSaver > 1)
				{
					s_u8SentCount_QuitSaver = 0;
					gs_DisplayCtrl.IsScreemSaving = FALSE;
					s_u8CloseBlackLightFlag = FALSE;
				}

				if(gs_DisplayCtrl.u8CurrPageNo == TOPWAY_SCREEM_PAGE_SCREEM_SAVER)
				{
					if(gs_DisplayCtrl.u8ScreemSavingReturnPage != TOPWAY_SCREEM_PAGE_SCREEM_SAVER)
					{
						gs_DisplayCtrl.u8CurrPageNo = gs_DisplayCtrl.u8ScreemSavingReturnPage;
					}
					else
					{
// 						if((gEvChargeInfo.u8IsSingleGunMode == TRUE) &&  (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_A_GUN))
// 						{
// 							gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_INIT_Single;
// 						}
// 						else if ((gEvChargeInfo.u8IsSingleGunMode == TRUE) &&  (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_B_GUN))
// 						{
// 							gs_DisplayCtrl.u8CurrPageNo = TOPWAY_B_SCREEM_PAGE_INIT_Single;
// 						}
// 						else
						{
							gs_DisplayCtrl.u8CurrPageNo = TOPWAY_SCREEM_PAGE_INIT;
						}
					}
				}	
				gs_DisplayCtrl.u8IsNewPage = TRUE;
			}
		}
	}
}

UINT8 IsTopway(void)
{
	if(gs_DisplayCtrl.IsScreemOk == TRUE
		&& gEvChargeInfo.u8RcvYdn23Flag == FALSE)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//////////////////
//
//////////////////
void TASK_Uart6Proc (void* pdata)
{
	unsigned char u8PowerOnFlag = 1;
	unsigned char u8temp;
	int i = 0;
	for (i = 0; i < MDBUS_RCV_ARRY_NUM; i++)
	{
		gMdbusRcvInfo[i].uReceivFlag = FALSE;
	}
	gEvChargeInfo.u8IMMUCommAlm = FALSE;
	UNUSED(pdata);
	dv_Uart6.fSetUart6BaudRate();			//19200 E 8 1
	dv_Uart6.fEnableUart6Rx();
	OSTimeDly(800);
	gSelfTest.fSetTaskID(Task_ID_Console);
	OSTimeDly(800);
	gSelfTest.fSetTaskID(Task_ID_Console);
	OSTimeDly(800);
	gSelfTest.fSetTaskID(Task_ID_Console);
	OSTimeDly(800);
	gSelfTest.fSetTaskID(Task_ID_Console);
	gSelfTest.fSetTaskID(Task_ID_Console);
	OSTimeDly(800);
	gSelfTest.fSetTaskID(Task_ID_Console);
	//InitHMIData();
	gEvChargeInfo.u32_UART6_DogTimer = 0;
	dv_Uart6.fSetUart6BaudRate();			//19200 E 8 1
	dv_Uart6.fEnableUart6Rx();
	OSTimeDly(800);


	//Cmd0x5FWriteBacklight(BackLightOn);
	//Cmd0x5FWriteBacklight(BackLightOn);
	//Cmd0x5FWriteBacklight(BackLightOn);
	//Cmd0x5FWriteBacklight(BackLightOn);

	//InitMdbusBuf();
	//RefreshQRCode();
	gEvChargeInfo.u8RectifierUpgrateMode = FALSE;

	InitDisplayVariables();
	gEvChargeInfo.i32Uart6_RCV_YND23_DogCount = UART_DOG_COUNT_DISBL;					//0 没有接上级监控
	gEvChargeInfo.i32Send_Init_Uart6_DogCount = UART_DOG_COUNT_START;
	for(;;)
	{
		
		gSelfTest.fSetTaskID(Task_ID_Console);
		//if(gEvChargeInfo.u32_UART6_DogTimer > (500*600))//2MS加一次 收到YDN23数据，开始计时，如果10分钟没有任何YDN23数据收到，复位
		//{
		//	gSelfTest.fSetStopFeedDog();
		//	gEvChargeInfo.u32_UART6_DogTimer = 0;
		//}

		if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
		{
			OSTimeDly(50);
			continue;
		}
		
		
		if(gs_DisplayCtrl.IsScreemOk == FALSE)
		{
			//后台调试
			if(gEvChargeInfo.u8RcvYdn23Flag == FALSE)
			{
				if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
				{
					//dv_Uart6.fSetUart6BaudRate();			//19200 E 8 1
					//dv_Uart6.fEnableUart6Rx();
					//gSelfTest.fSetTaskID(Task_ID_Console);
					//OSTimeDly(500);
					Cmd0x30Handshake();
					gSelfTest.fSetTaskID(Task_ID_Console);
					OSTimeDly(500);
                    gSelfTest.fSetTaskID(Task_ID_Console);
				}

				CheckScreemHandshkeResponse();
			}

			AutoAdjustDeal();
			//Test_API();//如果计费单元将CHADEMO 送没连接
		}
		else if(u8PowerOnFlag)
		{		
			if((u8PowerOnFlag == 1) && (gs_DisplayCtrl.u8TxIsBusy == FALSE))  //先上送一次时间
			{
				gs_DisplayCtrl.u8TxIsBusy = TRUE;
				Cmd0x82WriteMultiInteger(CYCLE_REPORT_SIGNAL_START_ADDR, INT_16BIT, CYCLE_REPORT_SIGNAL_NUM);
				gs_DisplayCtrl.u8SysTimeReflashPeriod = 0;
				u8PowerOnFlag = 2;
			}
			else if((u8PowerOnFlag == 2)  && (gs_DisplayCtrl.u8TxIsBusy == FALSE))  //开背光
			{
				gs_DisplayCtrl.u8TxIsBusy = TRUE;
				Cmd0x5FWriteBacklight(BackLightOn);
				u8PowerOnFlag = 3;
			}
			else if((u8PowerOnFlag == 3)  && (gs_DisplayCtrl.u8TxIsBusy == FALSE))  //初始化首屏
			{
				gs_DisplayCtrl.u8TxIsBusy = TRUE;

// 				if((gEvChargeInfo.u8IsSingleGunMode == TRUE) && (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_A_GUN))
// 				{
// 					u8temp = TOPWAY_SCREEM_PAGE_INIT_Single;
// 				}
// 				else if ((gEvChargeInfo.u8IsSingleGunMode == TRUE) && (gEvChargeInfo.u8IsSingleGunUseNo == SINGLE_MODE_USE_B_GUN))
// 				{
// 					u8temp = TOPWAY_B_SCREEM_PAGE_INIT_Single;
// 				}
// 				else
				{
					u8temp = TOPWAY_SCREEM_PAGE_INIT;
				}

				Cmd0x70SetPageNo(u8temp);
				gs_DisplayCtrl.u8CurrPageNo = u8temp;
				gs_DisplayCtrl.u8LastPageNo = u8temp;
				u8PowerOnFlag = 0;
			}
		}
		else 
		{	
			if(gEvChargeInfo.u8RcvYdn23CMDCount >= 100)
			{
				gs_DisplayCtrl.IsScreemOk = FALSE;
				gEvChargeInfo.u8RcvYdn23CMDCount = 0;
			}

			//屏下发数据处理
			TopwayKeyEventDeal();
			
			
			//校准处理
			AutoAdjustDeal();
			
			//屏跳转逻辑处理
			TopwayPageDeal();
			
			
			//主机逻辑更改了屏号，需要上报给屏，进行同步
			if((gs_DisplayCtrl.u8LastPageNo != gs_DisplayCtrl.u8CurrPageNo)
				|| (gs_DisplayCtrl.u8IsNewPage == TRUE))
			{
				if(gs_DisplayCtrl.u8TxIsBusy == FALSE)
				{
					gs_DisplayCtrl.u8TxIsBusy = TRUE;
					Cmd0x70SetPageNo(gs_DisplayCtrl.u8CurrPageNo);
					gs_DisplayCtrl.u8LastPageNo = gs_DisplayCtrl.u8CurrPageNo;
					gs_DisplayCtrl.u8IsNewPage = FALSE;
					gs_DisplayCtrl.u8ChkPageFlag = PAGE_CHK_FLAG_NeedChk;
				}
			}
			//刷新屏显示内容
			else 
			{			
				ReflashPageData(gs_DisplayCtrl.u8CurrPageNo);
			}	

			//屏保处理
			TopwayScreemSaverDeal();
		}
		
		OSTimeDly(10);

	}
}