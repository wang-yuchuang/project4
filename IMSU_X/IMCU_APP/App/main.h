
#ifndef __main_H
#define __main_H


#include "base_def.h"
#include "includes.h"

//865748

#include <time.h>
#include "interface.h"
#include "init1.h"
#include "setdata.h"
#include "History.h"
#include "mutex.h"
#include "uip_arp.h"

#include "EnergyOutputMng.h"

extern INT32 iAddCompensation;
extern UINT8 u8DebugVal;
#define WDOG_ENABLE					1 
#define SFT_WDOG_PERIOD				(2*60*(OS_TICKS_PER_SEC))
#define NET_PRINTF(...)				printf(__VA_ARGS__)
#define INVALID_DELAY				-100
#define CHARGE_LOG

//2020-02-22 lkf added 闁跨喕濡?鏉堢偓瀚归柨鐔告灮閹凤拷-D闁跨喐鏋婚幏鐑芥晸鐞涙?婂櫙閹烽攱寮归?愵剚瀚归柨鐔虹哺閹插??瀚归柨鐔虹哺閻у憡瀚归柨鐕傛嫹
#define UDP_EXCHANGE_EN			1

//20210208 lkf
#define	Debug_Print_En


#define OVER_TEMP_FROM_PT1000	1
#define OVER_TEMP_FROM_DI		0

//20220122 lk ACDC濡?锟犳晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔哄瀭EG濡?锟犳晸娴犲?剁礉闁跨喐鏋婚幏鐑芥晸閺傘倖瀚规禍娑㈡晸缂傚?搞?銒閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撶紒鎾茬瑝閸氾拷
#define	ACDC_IS_BEG_TYPE	0
#define ACDC_IS_PCS_TYPE	1
#define ACDC_IS_35KWPCS_TYPE  2                 //A2419闁跨喐鏋婚幏椋庢窗闁跨喐鏋婚幏铚傝礋V1.04闁跨喕濮ら張顒?沐礂闁跨喐鏋婚幏锟?  BEG濡?锟犳晸閺傘倖瀚?


#define EV_XXX_BUF_NUM	                                800

#define RECT_EFFICIENT		0.93
//鐗堟湰
#define mVERHIGH           1
#define mVERLOW			   05 

//setdata version
#define mSETDATAVERHIGH     1
#define mSETDATAVERLOW      3

#define RS485_SEND_CHECK_START		1
#define RS485_SEND_CHECK_STOP		0
#define RS485_SEND_TIME_OUT					1001
#define RS485_SEND_DELAY_START_COUNT		2003
#define RS485_SEND_DELAY_TIMER			2

#define PLUG_IN_CHARGING_MAX_SEC	70





#define PLC_IDL		0
#define PLC_PWM		1
#define PLC_CONN	2  //match
//#define PLC_MATCH	3
#define PLC_PRE		3
#define PLC_CHARGE  4
//#define	KAIPU_2017VER_TEST			1		//闁跨喐鏋婚幏鐑芥晸缁夊摜顣?閹风兘鏁撻惃鍡楁嫲椤?鎺撳?归挊鏇犵?慊闁跨喐鏋婚幏鐑芥晸閻ㄥ棗灏呴幏鐑芥晸閺傘倖???

#define	 SUPPORT_DYNAMIC		1
#define	 NO_SUPPORT_DYNAMIC		0

#define RELEASE_OTHER_RECT_DELAY_SEC	100
#define WAIT_UP_CTRL_GET_STOP_SEC		10

#define FREQ_POLL_CYCL					500		//闁跨喐鏋婚幏铚傜秴2ms
#define MULTIPLIER_BY_CYCL				(1000.00/(FREQ_POLL_CYCL*2.00))
//#define FAILS_REASON_UNKNOWN					1			//閺堫亞鐓￠崢鐔兼晸閺傘倖瀚?
//#define FAILS_REASON_E_STOP					2			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崑婊堟晸閺傘倖瀚?
//#define FAILS_REASON_CC1						3			//CC1闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
//#define FAILS_REASON_BMS_FAULT				4			//BMS闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
//#define FAILS_REASON_BMS_ERROR				5			//BMS闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
//#define FAILS_REASON_CELL_OVER_V				6			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹崵閻у憡瀚归柨鐔告灮閹风兘鏁撻敓锟?
//#define FAILS_REASON_CELL_OVER_T				7			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔兼應鎼达妇娅㈤幏鐑芥晸閺傘倖瀚?
//#define FAILS_REASON_AC_OVER					8			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹崵閿燂拷
//#define FAILS_REASON_AC_UNDER					9			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚瑰▎鐘插竾
//#define FAILS_REASON_AC_PHASE					10			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭紓娲?鏁撻弬銈嗗??
//#define FAILS_REASON_OUT_SHORT				11			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔兼應閿燂拷
//#define FAILS_REASON_SELF_CHECK				12			//闁跨喕濡?鐠囇勫?归柨鐔告灮閹风兘鏁撻弬銈嗗?? 闁跨喓娈曠涵閿嬪?归柨鐔告灮閹风兘鏁撻敓锟?
//#define FAILS_REASON_DC_OVER_V				13			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹崵閿燂拷
//#define FAILS_REASON_OUT_BREAK				14			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔兼應閿燂拷
//#define FAILS_REASON_APP						15			//APP闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
//#define FAILS_REASON_BRUSH					16			//閸掔兘鏁撻弬銈嗗?归崑婊堟晸閺傘倖瀚?
//#define FAILS_REASON_NET_ADMIN				17			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
//#define FAILS_REASON_SPEAR					18			//閺堫亪鏁撻弬銈嗗?归弸锟? 闁跨喐鏋婚幏閿嬬仚闁跨喕濞囩粵瑙勫?? 
//#define FAILS_REASON_BATT_FULL				19			//闁跨喐鏋婚幏鐑芥晸閹搭亞顣?閹风兘鏁撻弬銈嗗?? 闁跨喓娼鹃崙銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻敓锟?
//#define FAILS_REASON_NET_BREAK				20			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻敓锟?
//#define FAILS_REASON_INSULT					21			//闁跨喐鏋婚幏椋庣喘闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
//#define FAILS_REASON_VIN						22			//VIN闁跨喕顕犳稉宥呭爱闁跨喐鏋婚幏锟?
//#define FAILS_REASON_POLARITY					23			//闁跨喐鏋婚幏鐤?鍎伴柨鐔告灮閹风兘鏁撻敓锟?
//#define FAILS_REASON_STATION_SPEAR_DISBLE		24			//闁跨喐鏋婚幏鐑芥晸鐟欐帞娅㈤幏鐑芥晸闂冮?濆櫙閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻敓锟?
//#define FAILS_REASON_BMS_PROTOCOL				25			//閸楀繘鏁撴禒瀣╃瑝閸栧綊鏁撻弬銈嗗??
//#define FAILS_REASON_ROTATORY_ONE_WORK		26			//娑撯偓閺嬵亪鏁撻弬銈嗗?归柨鐔诲Ν閻у憡瀚归柨鐔告灮閹凤拷
//#define FAILS_REASON_SINGLE_ONE_WORK			27			//娑撯偓閺嬵亪鏁撻弬銈嗗?归柨鐔诲Ν閻у憡瀚归柨鐔告灮閹凤拷
//#define FAILS_REASON_ONE_SELF_WORK			28			//闁跨喐鏋婚幏閿嬬仚闁跨喐鏋婚幏鐑芥晸閼哄倻娅㈤幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
//#define FAILS_REASON_ORDER					29			//闁跨喐鏋婚幏閿嬬仚妫板嫮瀹抽柨鐔告灮閹凤拷



#if KAIPU_2017VER_TEST
	#define	BATT_OVER_VOLT_AlmDly	10
#else
	#define	BATT_OVER_VOLT_AlmDly	20
#endif

#define FINISH_CODE_NORMAL		1			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗??
#define FINISH_CODE_ABORT		2			//闁跨喎鐪剧敮绋夸粻闁跨喐鏋婚幏锟? 闁跨喕濡?鐠囇勫?归柨鐔侯仾閹插??瀚?	闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻柧甯?缍囬幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲浠犲?濓拷    閺堫亪鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔虹哺閻у憡瀚归柨鐔告灮閹烽攱鏋戦敍顓ㄧ骇閿燂拷
#define FINISH_CODE_EPO			3			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崑婊堟晸閺傘倖瀚归柨鐔告灮閹风兘鎸抽柨鐔告灮閹风兘鏁撻弬銈嗗?? 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崑婊勵剾
#define FINISH_CODE_CC1			4			//CC1 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭涵顕?沔晸鏉堝??娅㈤幏鐑芥晸閺傘倖瀚?
#define FINISH_CODE_BMS_FAILS	5			//bms闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?    闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崑婊勵剾
#define FINISH_CODE_BMS_ERROR	6			//bms闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?    闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崑婊勵剾
#define FINISH_CODE_CELL_OVER_V	7			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹崵閻у憡瀚归柨鐔煎徍閿濆繑瀚归柨鐕傛嫹
#define FINISH_CODE_CELL_OVER_T	8			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔兼應鎼达箓娼婚幏宄颁粻闁跨喐鏋婚幏锟?
#define FINISH_CODE_AC_OVER		9				//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲甯?
#define FINISH_CODE_AC_UNDER		10				//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚瑰▎鐘插竾
#define FINISH_CODE_AC_PHASE		11				//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭紓娲?鏁撻弬銈嗗??
#define FINISH_CODE_OUT_SHORT		12		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔兼應閿燂拷
#define FINISH_CODE_SELF_CHECK		13		//闁跨喕濡?鐠囇勫?归柨鐔告灮閹风兘鏁撻弬銈嗗?? 闁跨喓娈曠涵閿嬪?归柨鐔告灮閹风兘鏁撻敓锟?
#define FINISH_CODE_DC_OVER_V		14		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹崵閿燂拷
#define FINISH_CODE_OUT_BREAK		15		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔兼應閿燂拷
#define FINISH_CODE_APP				80		//APP闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define FINISH_CODE_BRUSH			81		//閸掔兘鏁撻弬銈嗗?归崑婊堟晸閺傘倖瀚?
#define FINISH_CODE_NET_ADMIN		82		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define FINISH_CODE_SPEAR			83		//閺堫亪鏁撻弬銈嗗?归弸锟? 闁跨喐鏋婚幏閿嬬仚闁跨喕濞囩粵瑙勫?? 
#define FINISH_CODE_BATT_FULL		84		//闁跨喐鏋婚幏鐑芥晸閹搭亞顒查幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
#define FINISH_CODE__NET_BREAK		85		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?? 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽攱绀傞柨蹇旀灮閹风兘鏁撶悰妤嬫嫹

#define TICKS_COUT_UNINT   OS_TICKS_PER_SEC

#define IS_PUTIAN_BMS		1
#define ISNT_PUTIAN_BMS		0
#define RELEASE_TIMER_STOP	0
#define RELEASE_TIMER_START	1
#define RELEASE_TIMER_FIRST_STABLE 10
#define RELEASE_TIMER_STEP_2	20
#define RELEASE_TIMER_STEP_3    30
#define RELEASE_TIMER_STEP_4	40
#define RELEASE_HALF_TIME		50
#define RELEASE_TIMER_STEP_6	60
#define RELEASE_TIMER_TIME_OUT	73

#define PARALLEL_SEM_VAL	(5*1000)
#define PARALLEL_SEM_FLAT		0
#define PARALLEL_SEM_EXECUTE	1
#define PARALLEL_SEM_NONE		2

#define ERR_BRM_00_DBDB_TIME_OUT		0x01
#define ERR_BRM_00_TIME_OUT				0x02
#define ERR_BCP_TIME_OUT				0x03
#define ERR_BCP_DBDBTIME_OUT			0x04
#define ERR_BRO_TIME_OUT				0x05
#define ERR_CRO_AA_TIME_OUT				0x06
#define ERR_BCS_BCL_TIME_OUT			0x07
/*************************************************************************************************
		BMS 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽攱姣婇悽鎿冪厜閹烽攱绔熼敓锟?
***************************************************************************************************/
//閻樿埖鈧?渚?沔晸閺傘倖??归柨鐔告灮閹凤拷

#define CHA_D1_ON_START					0
#define CHA_SEND_108_109				1
#define CHA_LOCK_INSULT					2
#define CHA_WAIT_EV_REALY_CURR			3
#define CHA_CHARGING_RCV_BCL_SEND_CCS	6
#define CHA_VEHICLE_NORMAL_STOP			7
#define CHA_VEHICLE_EMERGENCY_STOP		8
#define CHA_CHARGE_NORMAL_STOP			9
#define CHA_CHARGE_EMERGENCY_STOP		10


#define HANDSHAKE_SELF_CHECK		0
#define HANDSHAKE_START_CRM00		1
//#define HANDSHAKE_CRM_AA			2
#define CONFIG_RCV_BCP_OR_S_CRMAA	2
#define CONFIG_SEND_CML_RCV_BRO		3
#define CONFIG_SEND_CRO				4
#define CHARGING_RCV_BCS_OR_S_CRO	5
#define CHARGING_RCV_BCL_SEND_CCS	6
#define EV_TIME_OUT					7

#define EV_CEM_XXXX					8
#define EV_CST_BST					9
#define EV_BSD_CSD					10
#define MACHINE_ID_MAX_NUM			11


#define GLOADGUAL_S_STATE_CP_SLAC_SDP							0
#define GLOADGUAL_S_STATE_HLC_MSGI1_3							1
#define GLOADGUAL_S_STATE_PAYMENT_AUTHORIZATION_MSG4_5			2
#define GLOADGUAL_S_STATE_PARAMETER_DISCOVERY					3
#define GLOADGUAL_S_STATE_CABLE_CHECK		4
#define GLOADGUAL_S_STATE_PRE_CHARGE		5
#define GLOADGUAL_S_STATE_CHARGING				6
#define GLOADGUAL_S_TIME_OUT				7
#define GLOADGUAL_S_STATE_FAULT_STOP		8
#define GLOADGUAL_S_CST_BST				9
#define GLOADGUAL_S_STATE_FINISH			10



#define GRD_DIN_STATE_START					0
#define GRD_DIN_STATE_START_HLC_COMM		1
#define GRD_DIN_STATE_INIT_WAIT				2
#define GRD_DIN_STATE_INIT					3
#define GRD_DIN_STATE_CABLE_CHECK			4
#define GRD_DIN_STATE_PRE_CHARGE			5
#define GRD_DIN_STATE_CHARGING				6
#define GRD_DIN_EV_TIME_OUT					7
#define GRD_DIN_STATE_FAULT_STOP			8
#define GRD_DIN_EV_CST_BST					9
#define GRD_DIN_STATE_FINISH				10




#define CHARGING_STATE_START_HLC_COMM	1

#define GBT_STATE_START			0




//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚筍tuff CEM
#define CHARGING_RCV_BCL_SEND_CCS_2		200   //BCS闁跨喐鏋婚幏閿嬫??

#define CAN_DATA_SEND			1
#define CAN_DATA_STOP			0

#define EV_BSM_PAUSE		0xf2
#define EV_BSM_NO_RCV		0xf1
#define EV_BSM_NONE			0xf0
#define EV_BST_NONE			0xf0
#define EV_INVALID_RESULT	0xf0

#define BMS_VERSION_2011	0
#define BMS_VERSION_2015	1

#define EV_HAS_ELEC_LOCK	1

#define CELL_ALM_STOP		1
#define CELL_ALM_NONE		0
#define CELL_ALM_ONLY		2

#define SPEAR_IS_LOCK	1
#define SPEAR_IS_UNLOCK	0

#define HOLL_COEF_K			867.32
#define HOLL_COEF_B			782.7

#define V1000_COEF_K		988.468
#define V1000_COEF_B		(778.419)

#define NO_APPROVE_CHARGE	0		//闁跨喐鏋婚幏宄板櫙闁跨喐鏋婚幏鐑芥晸閿燂拷
#define APPROVE_CHARGE		1		//闁跨喐鏋婚幏鐑芥晸閿燂拷

#define SPEAR_ON_LINE		0x01
#define SPEAR_ON_LINE_KEY	0x02
#define SPEAR_NO_ON_LINE	0x00

#define EV_STOP_TIMER		-3

#define BMS_AUTO		0
#define BMS_TIME		1
#define BMS_ENERGY		2
#define BMS_MONEY		3
#define BMS_SOC			4

/************************************************ 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽攱鐨?閺嗩喚鏁侀…鎺撳?归柨鐕傛嫹 *******************************************************/
#define NET_SUPPORT_ZHIXIN	0
#define NET_SUPPORT_YUNSHAN	1

#define EV_ENTER_IC_CARD	1
#define EV_ENTER_APP		2
#define EV_ENTER_admin		3
#define EV_ENTER_IC_CARD_NET 4
#define EV_ENTER_VIN		5

#define EV_STAT_IDL		0
#define EV_STAT_READY_OK	1
#define EV_STAT_CHARGING	2
#define EV_STAT_FINISH		3
#define EV_STAT_START_FAIL	4
#define EV_STAT_ODER		5
#define EV_STAT_SYS_FAILT	6
#define EV_STAT_NEED_WAIT	7




//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽攱鐨?閺嗩喚鏁侀…鎺撳?归柨鐕傛嫹
#define EV_CommMode_WAIT_CC1_START	1
#define EV_CommMode_ALREADY_START	2
#define EV_CommMode_NONE	0

#define NO_ADJUST_ON_OFF	2
#define ADJUST_RECT_VOLT_CURR	1
#define NO_ADJUST_RECT_VOLT_CURR	0

#define TEST_CAN1			1
#define TEST_CAN2			2
#define TEST_485_UART4		4
#define TEST_485_UART5		5	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define TEST_UART1			1
#define TEST_UART2			2


#define BMS_IMMEDIATELY_TYPE	0		//闁跨喐鏋婚幏宄扮安闁跨喐鏋婚幏閿嬫綈  闁跨喓娈曠拋瑙勫?? 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define BMS_TIMMING_TYPE	1			//妫板嫮瀹抽柨鐔告灮閹风兘鏁撻敓锟? 闁跨喐鏋婚幏鐑芥晸鐟欐帟顕滈幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归弶澶愭晸閺傘倖瀚归柨鐔煎徍閿濆繑瀚归柨鐔告灮閹烽攱娼愰柨鐔告灮閹风兘鏁撻弬銈嗗?规０鍕?瀹抽柨鐔峰?欐导娆愬?归柨鐔告灮閹风柉顩﹂柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔虹哺绾?閿嬪?归柨鐔告灮閹烽攱膩闁跨噦鎷?
#define BMS_ENERGY_TYPE		8			//闁跨喐鏋婚幏宄扮安闁跨喐鏋婚幏閿嬫綈  闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define BMS_TIME_YUNSHAN_TYPE	9		//闁跨喐鏋婚幏宄扮安闁跨喐鏋婚幏閿嬫綈  闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归弶锟? 閺冨爼鏁撻幋鎺撴煙瀵?锟? 闁跨喐鏋婚幏鐑芥晸閿燂拷
#define BMS_MONEY_TYPE		10			//闁跨喐鏋婚幏宄扮安闁跨喐鏋婚幏閿嬫綈  闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归弶锟? 闁跨喐鏋婚幏椋庣稅缂佺儑鎷? 闁跨喐鏋婚幏鐑芥晸閿燂拷

#define BMS_SOC_TYPE		11

#define IS_SPEAR_CONNECT	0
#define IS_SPEAR_ISNT_CONNECT	1

#define TIME_IMMEDIATELY_TYPE	2
#define TIME_TIMMING_TYPE	3
#define ENEGY_IMMEDIATELY_TYPE	4
#define ENEGY_TIMMING_TYPE	5
#define MONEY_IMMEDIATELY_TYPE	6
#define MONEY_TIMMING_TYPE	7

#define ALM_LED_BLINK()	HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_7)

#define _LITTLE_ENDIAN_TYPE

#define VARIABLE_SET_BIT(Args,Pst) (Args|=(0x01<<Pst))				//闁跨喐鏋婚幏铚傜秴
#define VARIABLE_CLR_BIT(Args,Pst) (Args&=(~(0x01<<Pst)))			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define VARIABLE_CHK_BIT(Args,Pst) (Args&(0x01<<Pst))				//闁跨喐鏋婚幏鐑芥晸閿燂拷

#define BRUSH_COMFAIL_NUM  5

#define TASK_NAME_EN (0)

#define UART1_SEQUECE_NO	1
#define UART2_SEQUECE_NO	2
#define UART6_SEQUECE_NO	6
#define UART4_SEQUECE_NO	4
#define UART5_SEQUECE_NO	5

#define UART2_RECV_MAX_NUM	10

#define UART6_RECV_MAX_NUM	255
#define UART6_SEND_MAX_NUM	255

#define UART6_YDN23_RECV_MAX_NUM	200
#define UART6_YDN23_SEND_MAX_NUM	400

//#define UART1_RECV_MAX_NUM	100
//#define UART1_SEND_MAX_NUM	70
#define  CAN1_BUF_MAX_NUM	1
#define  CAN_A_B_BUF_MAX_NUM	100
#define  UART4_SEND_BUFF_MAX_NUM 300
#define  CAN_A_B_TEST_BUF_MAX_NUM	5
#define  MDBUS_BUFF_MAX_NUM	100

#define  RS485_UART5_RX_MAX_NUM	330
#define  RS485_UART5_TX_MAX_NUM	150

#define  RS485_UART4_RX_MAX_NUM	430
#define  RS485_UART4_TX_MAX_NUM	150

#define  RS485_UART4_5_RX_MAX_NUM	MIN(RS485_UART4_RX_MAX_NUM, RS485_UART5_RX_MAX_NUM)
#define  RS485_UART4_5_TX_MAX_NUM	MIN(RS485_UART4_TX_MAX_NUM, RS485_UART5_TX_MAX_NUM)

#define  UART4_RCV_ARRY_NUM	4
#define  CAN_FUN_CONDE_AUTO_RPT	5
#define  CAN_CCS_ADDR		2
#define  CAN_BROADCAST_ADDR	15

#define YDN23_IMCU_ADDR	0x01
#define YDN23_IMIU_ADDR	180

#define MDB_ISLT_ADDR  0x03
#define MDB_R_CMD	  0x03
#define MDB_W_CMD	  0x10

#define YDN23_IMCU_CID1_COPY	0xe1
#define YDN23_IMCU_CID1	0x50
#define YDN23_IMIU_CID1	0x60
#define YDN23_ADDR_SELF	1
#define YDN23_SOI_VAL	0x7e
#define YDN23_EOI_VAL	0x0d
#define YDN23_VER_VAL	0x21
#define YDN23_EID_VAL	0x41
#define YDN23_GET_ANLG	0x41
#define YDN23_GET_ALM	0x44
#define YDN23_GET_STAT	0x43
#define YDN23_CTRL_RECT 0x45
#define YDN23_ADJUST_RECT 0x80
#define YDN23_INVALID_VAL 0x00

#define YDN23_VER_PST	1
#define YDN23_ADDR_PST	3
#define YDN23_CID1_PST	5
#define YDN23_CID2_PST	7
#define YDN23_DATAINFO_PST	13

#define CAN_BRST_ADDR		15
#define CAN_DESTADDR_PST	22
#define CAN_SORCEADDR_PST	18
#define CAN_FUNC_CODE_PST	10
#define CAN_INFO_CODE_PST	0



#define LOCALFLAGFST      0x5A
#define LOCALFLAGSEC      0xA5
#define REMOTEFLAGFST     0xA5
#define REMOTEFLAGSEC     0x5A


//key values
#ifdef LCD_LINE_8
#define KEY_ESC   0x2e
#define KEY_ENT   0x27
#define KEY_UP    0x2b
#define KEY_DN    0x2d
#define KEY_NONE  0x2f
#define KEY_RESET (KEY_ESC & KEY_ENT)//0x26
#define KEY_BLUP  (KEY_UP & KEY_ENT)
#define KEY_BLDN  (KEY_DN & KEY_ENT)
#else
#define KEY_ESC   0x1f
#define KEY_ENT   0x3e
#define KEY_UP    0x3b
#define KEY_DN    0x37
#define KEY_LEFT  0x2f
#define KEY_RIGHT 0x3d
#define KEY_NONE  0x3f
#define KEY_RESET (KEY_ESC & KEY_ENT)//0x1e
#define KEY_BLUP  (KEY_UP & KEY_ENT)
#define KEY_BLDN  (KEY_DN & KEY_ENT)
#define KEY_ADVANCED  (KEY_RIGHT & KEY_ENT)
#endif

#define mRESETKEYTIME   20

#define mREPEATKEYTIME  3
#define mDOUBLEKEYTIME  10
#define m4TIMEKEYTIME   30
#define mNOPASSWORDTIME 750
#define mNOKEYALARMPOP  375     //375 * 0.16 = 1 Min
#define mNOKEYTIME      3000
#define mKEYONCE        1
#define mKEYREPEAT      2
#define mKEYDOUBLE      4
#define mKEY4TIME       8

#define YDN23_NONE  	0
#define YDN23_PROTOCOL  23
#define YDN23_HEAD      1
#define YDN23_START     0x7e
#define YDN23_END       0x0d
#define YDN23_OLD_ADD   '@'

#define SET_AC_DATAFLAG_SW_CHG      0
#define SET_RECT_DATAFLAG_SW_CHG    1
#define SET_DC_DATAFLAG_SW_CHG      2
#define SET_MPPT_DATAFLAG_SW_CHG    3


//SignalDisplayMode
#define DIRECT_NORMAL       1
#define DIRECT_REVERSE      2
#define INDIRECT_NORMAL     3
#define INDIRECT_REVERSE    4


//CTRLSIGNAL
#define CHARGE_CTRL         0
#define INSUL_CTRL          1
#define CHARGE_MODE_SET     2
#define RECT_ON_OFF_CTRL    3
#define FAN_FULL_CTRL       4
#define CONTACTOR_CTRL      5

//SetSignal


//SYSTEMINFO


//INSULINFO
#define INSUL_VOLT              0
#define INSUL_CURR              1
#define INSUL_P_G_RES           2
#define INSUL_N_G_RES           3

//AC/DC Meter INFO
//measure analog index


#define TEMP1_NUM      4
#define TEMP2_NUM      5
#define TEMP3_NUM      6

#define AC1VA_NUM      7
#define AC1VB_NUM      8
#define AC1VC_NUM      9
#define AC1IA_NUM      10
#define AC1IB_NUM      11
#define AC1IC_NUM      12

#define LOAD1FUSE_NUM  13
#define LOAD2FUSE_NUM  14
#define LOAD3FUSE_NUM  15
#define LOAD4FUSE_NUM  16
#define LOAD5FUSE_NUM  17
#define LOAD6FUSE_NUM  18
#define LOAD7FUSE_NUM  19

#define BATT1FUSE_NUM  20
#define BATT2FUSE_NUM  21
#define BATT3FUSE_NUM  22
#define BATT4FUSE_NUM  23

#define BMV1_NUM       24
#define BMV2_NUM       25
#define BMV3_NUM       26
#define BMV4_NUM       27
#define BMV5_NUM       28
#define BMV6_NUM       29
#define BMV7_NUM       30
#define BMV8_NUM       31
#define BMV9_NUM       32

#define LOADCURR_NUM   33
#define DCEMCURR1_NUM  34
#define DCEMCURR2_NUM  35
#define DCEMCURR3_NUM  36
#define DCEMCURR4_NUM  37
#define SIG_ID_BATT_SOC 38

#define ADMAXCOUNT      4096
#define BUSVREF         2.5    
#define DCEMHARDCOEFF   27.67
#define VOLTVREF        2     
#define CURRVREF        50   
#define LOADFUSECOEFF   26    
#define BATTFUSECOEFF   16   


#define mFANSELFTEST		1  
#define mFANPWRONTEST		2  
#define mFACTORYTEST		3  


#define mFAN1				0
#define hPOWER_PWM			1
#define AC_PWM				2

#define CHK_SEND_STAT(u8Stat) (u8Stat&(1<<0))
#define SET_SEND_STAT(u8Stat) (u8Stat|=(1<<0))
#define CLR_SEND_STAT(u8Stat) (u8Stat&=~(1<<0))

#define CHK_CONN_STAT(u8Stat) (u8Stat&(1<<1))
#define SET_CONN_STAT(u8Stat) (u8Stat|=1<<1)
#define CLR_CONN_STAT(u8Stat) (u8Stat&=~(1<<1))

#define CHK_DHCP_STAT(u8Stat) (u8Stat&(1<<2))
#define SET_DHCP_STAT(u8Stat) (u8Stat|=1<<2)
#define CLR_DHCP_STAT(u8Stat) (u8Stat&=~(1<<2))


#define CHK_SENDING_STAT(u8Stat) (u8Stat&(1<<3))
#define SET_SENDING_STAT(u8Stat) (u8Stat|=(1<<3))
#define CLR_SENDING_STAT(u8Stat) (u8Stat&=~(1<<3))
#define SET_SENDING_TIMEOUT(TimeOUt, Value) (TimeOUt = Value)

#define BMS_COMM_ERR_INVALID				0xff


#define CEM_CRM00_TIMEOUT					1	//HANDSHAKE_START_CRM00
#define CEM_RCV_BCP_OR_S_CRMAA_TIMEOUT		2	//CONFIG_RCV_BCP_OR_S_CRMAA
#define CEM_SEND_CML_RCV_BRO_TIMEOUT		3	//CONFIG_SEND_CML_RCV_BRO
#define CEM_RCV_BCS_OR_S_CRO_TIMEOUT		4	//CHARGING_RCV_BCS_OR_S_CRO
#define CEM_RCV_BCL_SEND_CCS_TIMEOUT		5	//CHARGING_RCV_BCL_SEND_CCS
#define CEM_CEM_XXXX_TIMEOUT				6	//EV_CEM_XXXX
#define CEM_CST_BST_TIMEOUT					7	//EV_CST_BST
#define CEM_BSD_CSD_TIMEOUT					8	//EV_BSD_CSD
#define CEM_OTHER_TIMEOUT					9	//ELSE
#define CRM00_LOCK_ERR						10	//闁跨喐鏋婚幏鐑芥晸鐟欐帪鎷? 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
#define SELF_TEST_ERR						11	//闁跨喓娈曠涵閿嬪?归柨鐔告灮閹风兘鏁撻敓锟?
#define CRO_BUS_VOLT_ERR					12	//CRO闁跨喖妯佺拋瑙勫?归柨鐔烘骄閸戙倖瀚归柨鐔告灮閹风兘鏁撴潏鍐?鎴风喊澶嬪?归柨鐔告灮閹凤拷
#define MCCB_CHECK_ERR						13	//濮ｅ秹鏁撻懘姘鳖暜閹峰嘲甯囬弽鈥冲櫙闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define CHARGE_ALM_BY_BMS					14	//闁跨喐鏋婚幏鐑芥晸缂佺儑鎷? 闁跨喎褰ㄩ崨濠咁劅閸嬫粓鏁撻弬銈嗗??
#define CHARGE_RECEIVE_BST					15	//闁跨喓笑绾板?嬪??		BST
#define CHARGE_BSM_ERR						16	//BSM  闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撶仦濠傜埗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define CHARGE_BUS_OVER_VOLT				17
#define SELF_TEST_CAR_VOLT_ERR				18	//闁跨喓娈曠涵閿嬪?归弮锟? 闁跨喐鏋婚幏鐑芥晸閸撹法顣?閹烽攱鐨?闁跨喓鐛ら惂鍛婂?归柨鐔告灮閹风兘鏁撻敓锟?30V  闁跨喐鏋婚幏閿嬫?? 娑撯偓闁跨喐鏋婚幏宄般亝椤?顔藉?归柨鐕傛嫹<-80V
#define SELF_TEST_INSULATION_ERR			19	//闁跨喓娈曠涵閿嬪?归弮锟? 闁跨喐鏋婚幏椋庣喘闁跨喕濮ょ拃锕傛晸閺傘倖瀚归柅姘舵晸閺傘倖瀚规径閬嶆晸閺傘倖瀚?
#define BMS_SOC_100							20	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define MANUANL_ALM_STOP					21	//闁跨喕顢滅拋瑙勫?归柨鐔告灮閹峰嘲绱? 鎼存柧璐熼柨鐔诲Г鐠?泔箓鏁撻弬銈嗗?归柨鐔告灮閹峰嘲浠犻柨鐔告灮閹凤拷  -- 閺堫亪鏁撻弬銈嗗?规０鍕?鏁撻弬銈嗗??
#define MANUAL_FIX_TIME_FINISH				22	//闁跨喕顢滅拋瑙勫?归柨鐔告灮閹峰嘲绱? 闁跨喐鏋婚幏閿嬫?傞柨鐔稿灊閸掍即鏁撻弬銈嗗?归柨鐔告灮閹凤拷   -- 閺堫亪鏁撻弬銈嗗?规０鍕?鏁撻弬銈嗗??
#define MANUAL_ENERGY_FINISH				23	//闁跨喕顢滅拋瑙勫?归柨鐔告灮閹峰嘲绱? 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗??    -- 閺堫亪鏁撻弬銈嗗?规０鍕?鏁撻弬銈嗗??
#define MANUAL_MONEY_FINISH					24	//闁跨喕顢滅拋瑙勫?归柨鐔告灮閹峰嘲绱? 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归悾瑙勬灮閹风兘鏁撻弬銈嗗?归柨鐕傛嫹     -- 閺堫亪鏁撻弬銈嗗?规０鍕?鏁撻弬銈嗗??
#define RECEIVE_STOP_CMD_BY_NET				25	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崑婊堟晸閺傘倖瀚?
#define MANUAL_SOC_FINISH					26	//闁跨喐鏋婚幏绋碠C闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风﹪				-- 閺堫亪鏁撻弬銈嗗?规０鍕?鏁撻弬銈嗗??
#define LUI_STOP_CMD						27	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔煎徍閿涘墎娅㈤幏鐑芥晸閺傘倖瀚?
#define MULTI_GUN_COMM_FAIL					28	//娑撯偓闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归弸顏堟晸閺傘倖瀚归弸顏堚偓姘舵晸閺傘倖瀚归柨鐔峰建鐠佽?勫??    -- 閺堫亪鏁撻弬銈嗗?规０鍕?鏁撻弬銈嗗??
#define CONSOLE_START_STOP_CMD					29	//闁跨喐鏋婚幏鐑芥晸閼哄倻灏ㄩ幏宄板酱閸嬫粓鏁撻弬銈嗗??  
#define ALL_RECT_COM_FAILS					30	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚瑰Ο锟犳晸閺傘倖瀚归柅姘舵晸閺傘倖瀚规径閬嶆晸閺傘倖瀚?
#define BRUSH_STOP_CMD						31	//閸掔兘鏁撻弬銈嗗?归崑婊堟晸閺傘倖瀚?
#define NET_BREAK_STOP						32	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚? APP闁跨喕濮ょ拃锕?沅粻闁跨喐鏋婚幏锟?
#define CHARGE_DOOR_ALM_BY_BMS				33	//闁跨喕鍓奸弬銈嗗?归柨鐔诲Г鐠?泔耿沅粻闁跨喐鏋婚幏锟?
#define CHARGE_ELE_LOCK_ALM_BY_BMS			34	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔诲Г鐠?泔??
#define CHARGE_SPEAR_CONNCT_ALM_BY_BMS		35	//闁跨喐鏋婚幏鐑芥晸鐟欐帞娅㈤幏鐑芥晸閺傘倖瀚归惀鍫濐熆閿燂拷
#define CHARGE_EPO_ALM_BY_BMS				36	//EPO
#define START_CHARGE_CC1_ALM				37	//cc1 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭涵顕?沔晸閺傘倖??? 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define BATT_VOLT_IS_NEGATIVE				38	//闁跨喐鏋婚幏閿嬬毉闁跨喓鐛ら惂鍛婂?归柨鐔活敎閿燂拷  -- 妫板嫰鏁撻弬銈嗗?归柨銉╂晸閿燂拷
#define CHARGE_ISLT_COMM_ALM				39	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?? 闁跨喐鏋婚幏椋庣喘 闁?姘舵晸閺傘倖瀚? 闁跨喕濮ょ拃锟?
#define CHARGE_SPD_ALM						40	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?? 闁跨喐鏋婚幏鐑芥晸闂冭泛鎲＄拃锟?
#define CHARGE_WH_COMM_ALM					41	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?? 闁跨喐鏋婚幏鐑芥晸闁版?跨厜閹风兘鏁撶紒鐐戝秵瀚归柨鐕傛嫹
#define CHARGE_CELL_VOLT_ALM				42	// 闁跨喐鏋婚幏锟?	闁跨喐鏋婚幏宄板竾闁跨喐鏋婚幏宄板竾闁跨喐鏋婚幏锟?
#define CHARGE_OVER_CURR_ALM				43	// 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define APP_CMD_ENTER_CHG_OVERTIME			44	// APP闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷90闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归張顏堟晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?瑰??蹇涙晸閺傘倖瀚圭痪缁㈠墾閹风兘鏁撻幓顓р偓顒婄骇閿燂拷
#define CHG_DURING_OVERTIME					45	// 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲鐤勯柨鐔虹哺缁涜?勫?归柨鐔告灮閹凤拷24鐏忓繑妞?
#define CARD_LEAVE_MONEY_NOT_ENOUGH			46	// 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归悾鍫滅串閹风兘鏁撻敓锟?
#define CHARGE_PAUSE_OVER_10_MIN			47	// 闁跨喐鏋婚幏宄颁粻10闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归弮鍫曟晸閹存帒鍩?
#define CHARGE_RING_FALT					48	// 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??
#define SELF_TEST_CAR_VOLT_ERR_MCCB_SHORT	49	//闁跨喓娈曠涵閿嬪?归弮锟? 闁跨喐鏋婚幏鐑芥晸閸撹法顣?閹烽攱鐨?闁跨喓鐛ら惂鍛婂?归柨鐔告灮閹风兘鏁撻敓锟?30V  (闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崫鐔兼晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻柧鏉垮殩閹风兘鏁撻敓锟??闁跨喐鏋婚幏锟?
#define SELF_TEST_CAR_VOLT_ERR_NEG_VOLT		50	//闁跨喓娈曠涵閿嬪?归柨鐔告灮閹风兘鏁撻敓锟? 闁跨喓娈曠涵閿嬪?归崜宥夋晸閺傘倖瀚归悰鎾呮嫹-20V闁跨喐鏋婚幏宄板竾  -- 闁跨喓娈曠涵閿嬪?归柨銉╂晸閿燂拷

#define CHARGE_RECEIVE_BST_SOC				51	//闁跨喓笑绾板?嬪?笲ST SOC闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??
#define CHARGE_RECEIVE_BST_VOLT				52	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚崚浼存晸閺傘倖瀚归崢瀣?鏁撻崐鐔风暰閸婏拷
#define CHARGE_RECEIVE_BST_CELL_VOLT		53	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚崚浼存晸閺傘倖瀚归柨鐔告灮閹风兘鏁撶粣鏍?娅㈤幏椋庢惥閵婃牭鎷?
#define CHARGE_RECEIVE_BST_CHARGER_ST		54	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归崑婊勵剾
#define CHARGE_RECEIVE_BST_INSUL_FALT		55	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏椋庣喘闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define CHARGE_RECEIVE_BST_CONTACTOR_OVER_TEMP	56	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐕傛嫹
#define CHARGE_RECEIVE_BST_DEVICE_OVER_TEMP	57	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚笲MS閸樼喖鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗??
#define CHARGE_RECEIVE_BST_CONTACTOR_FALT	58	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐕傛嫹
#define CHARGE_RECEIVE_BST_BATT_OVER_TEMP	59	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏閿嬬槵闁跨喐鏋婚幏鐑芥晸閿燂拷
#define CHARGE_RECEIVE_BST_HIGH_VOLT_CONTACTOR_FALT	60	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏宄板竾闁跨喐鏆?沌喊??嬪?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷
#define CHARGE_RECEIVE_BST_CC2_FALT			61	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?2闁跨喐鏋婚幏宄板竾闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define CHARGE_RECEIVE_BST_OTHER_FALT		62	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??
#define CHARGE_RECEIVE_BST_OVER_CURR		63	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??
#define CHARGE_RECEIVE_BST_VOLT_ABNORMAL	64	//闁跨喓笑绾板?嬪?笲ST 闁跨喐鏋婚幏宄板竾闁跨喎鐪剧敮锟?
#define CHARGE_GUN_OVER_TEM_ALM				65	//闁跨喐鏋婚幏鐑芥晸鐟欐帞娅㈤幏鐑芥晸閺傘倖瀚归柨鐕傛嫹
#define SELF_TEST_RECT_NOT_WORK_ALM			66	//闁跨喓娈曠涵閿嬪?归弮鑸的侀柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻敓锟?
#define SELF_TEST_DCMCCB_BREAK_ALM			67	//闁跨喓娈曠涵閿嬪?归弮鍫曟晸閺傘倖瀚归柨鐔告灮閹峰嘲鎽曢柨鐔告灮閹风兘鏁撻惌顐＄串閹风柉妯冮柨鐕傛嫹
#define CONTROLLER_FAIL_RESTART				68	//闁跨喐鏋婚幏閿嬬毉闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻敓锟?
#define CHARGE_NO_CURR_STOP					69	//闁跨喐鏋婚幏閿嬫?傞柨鐔告灮閹风兘鏁撻惈顐ゎ暜閹风兘鏁撻弬銈嗗?归崑婊堟晸閺傘倖瀚?
#define	STOP_REASON_BUS_VOLTAGE_ERROR		72	//闁跨喐鏋婚幏鐑芥晸鐟欐帗澧冮敐蹇斿?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔峰建绾板?嬪?归崢锟?
#define	STOP_REASON_REQ_VOLT_OVERLMT		73	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹崵閻у憡瀚归柨鐔告灮閹风兘鏁撴潪鍖℃嫹
#define STOP_REASON_EVDISCHG_ESBATT_ERROR	74	//闁跨喕鍓奸悽闈涘煂闁跨喐鏋婚幏鐑芥晸閺夋壆顣?閹峰嘲鍘愰柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔峰建鏉堢偓瀚归柨鐔告灮閹凤拷


#define	CONSOLE_STOP_CMD	 CONSOLE_START_STOP_CMD 

#define STOP_NOT_USED				0
#define STOP_PARAM_CFG_ERR			1			//1闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹峰嘲銇戦柨鐔告灮閹凤拷
#define STOP_CHARGING_ENABLE_ERR	2			// 2闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹哺閻у憡瀚归懝褰掓晸缂佺偞鍞婚幏鐑芥晸缁辩瘣ADEMO闁跨喐鏋婚幏锟?
#define STOP_OUTSIDE_VOLT_ERR		3			//3闁跨喐鏋婚幏閿嬬槤闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲甯囬柨鐔风溇鐢?鎼佹晸閺傘倖瀚笴HADEMO闁跨喐鏋婚幏锟?
#define STOP_LOCK_ERR				4			//4闁跨喐鏋婚幏鐑芥晸閻?顐㈠殩閹风兘鏁撻弬銈嗗?瑰?濄垽鏁撻弬銈嗗?归柨鐔活潡閿燂拷
#define STOP_UPPER_CTRL				5			//5闁跨喐鏋婚幏绋禤PER CTRL 閸嬫粓鏁撻弬銈嗗??
#define STOP_EV_NORMAL				6			//6闁跨喐鏋婚幏绋?V 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归崑婊堟晸閺傘倖瀚?
#define STOP_INSLT_ERR				7			//7) 闁跨喐鏋婚幏椋庣喘闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归弬鎰舵嫹
#define STOP_INSLT_TIME_OUT			8			//8) 闁跨喐鏋婚幏椋庣喘闁跨喐鏋婚幏椋庡纯椤戙儴鎽夐敐蹇斿?归柨鐕傛嫹
#define STOP_EV_RELAY_TIME_OUT		9			//9)闁跨喐鏋婚幏鐑芥晸閸撹法鎴风喊澶嬪?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔荤窛缁涜?勫?归弮鍫曟晸閺傘倖瀚笴HADEMO闁跨喐鏋婚幏锟?
#define STOP_CURR_TIME_OUT			10			//10)闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔虹哺閹插??瀚归柨鐔烘槬HADEMO闁跨喐鏋婚幏锟?
#define STOP_REAMIN_TIME_OVER		11			//11)Reamin time Over閸嬫粓鏁撻弬銈嗗?? 闁跨喐鏋婚幏绋?HADEMO闁跨喐鏋婚幏锟?
#define STOP_EV_SELF_ALM			12			//12)闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔诲Г鐠?泔耿沅粻闁跨喐鏋婚幏锟?
#define STOP_RING_FAULT				13			//13)闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归崑婊堟晸閺傘倖瀚?(妫板嫰鏁撻弬銈嗗??)
//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔风溇鐢?绋夸粻闁跨喐鏋婚幏锟?
#define STOP_BATT_OVERVOLT			14			//(14) Battery overvoltage (0: normal, 1: fault)
#define STOP_BATT_UNDERVOLT			15			//(15) Battery undervoltage (0: normal, 1: fault)
#define STOP_BATT_CURR_DEVIATION	16			//(16) Battery current deviation error (0: normal, 1: fault)
#define STOP_BATT_H_TEMP			17			//(17) High battery temperature (0: normal, 1: fault)
#define STOP_BATT_VOLT_DEVIATION	18			//(18) Battery voltage deviation error (0: normal, 1: fault)
#define STOP_ConnectorLockFault		19			//19)  ChargerConnectorLockFault
#define STOP_SHIFT_PST				20			//20闁跨喐鏋婚幏绋竐hicle shift position
#define STOP_EV_COMM_FAIL			21			//21闁跨喐鏋婚幏鐑芥晸閺傘倖瀚笶V闁?姘舵晸閺傘倖瀚规径閬嶆晸閺傘倖瀚?
#define STOP_UPPER_COMM_FAIL		22			//22闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹疯渹缍呴柨鐔告灮閹风兘鏁撻柊娈跨厜閹风兘鏁撶紒鐐戝秵瀚归柨鐕傛嫹
#define STOP_PLUG_IN_TIMEOUT		23			//23) 闁跨喐鏋婚幏閿嬬仚闁跨喐鏋婚幏閿嬫??
#define STOP_PRE_FAILS				24			//24闁跨喐鏋婚幏鐑筋暕闁跨喐鏋婚幏宄般亼闁跨喐鏋婚幏锟?

#define STOP_DOOR_FAILS				25			//25-Door Open Alarm
#define STOP_EPO_FAILS				26			//26-EPO Alarm
#define STOP_SPD_FAILS				27			//27-SPD Alarm
#define STOP_ALL_RECTIFIER_FAILS	28			//28-All Rectifier Fail Alarm
#define STOP_MAINS_FAILS			29			//29-Mains Fail Alarm
#define STOP_RECT_COMM_FAILS		30			//30- All Rectifier Communication Fail Alarm 
#define STOP_E_LOCK_FAILS			31			//31-E-Lock Failed
#define STOP_GUN_OVER_FAILS			32			//32Gun Over-Temp Alarm
#define STOP_OUT_SHORT_FAILS		33			//33-Output Short Circuit Alarm
#define STOP_PWM_FAILS				34			//25) PWM婢堕亶鏁撻弬銈嗗??
#define STOP_EV_ERROR_CODE			35			//EV ERROR CODE
#define STOP_LOW_LEVEL				36			//Low Leve TIMEOUT
#define STOP_HIGH_LEVEL				37			//High Leve TIMEOUT
#define STOP_AUTHENTICATION_TIMEOUT	38			//AUTHENTICATION TIMEOUT
#define STOP_PARAMDISCOVERY_TIMEOUT 39
#define STOP_Earth_ALM				40

#define STOP_CCS_LiquidGun_Imeu2_CommFail	41
#define STOP_CCS_LiquidGun_OilPumpFalt	42
#define STOP_SECC_COMM_FAIL			43

#define UART_DOG_COUNT_DISBL	0
#define UART_DOG_COUNT_START	1
#define UART_DOG_COUNT_OVER		90
#define UART_DOG_SEND_COUNT_OVER	500

#define IST_RECT				50
#define ALM_BRUSH				1
#define ALM_POLARITY			2
#define ALM_WH					3
#define ALM_SPD					4
#define ALM_CC1					5
#define ALM_DOOR				6
#define ALM_LOCK				7
#define ALM_BUS_VOLT			8
#define ALM_MCCB				9
#define ALM_BMS_ABORT_STOP		10
#define ALM_CELL_VOLT			11
#define ALM_RECT_STOP			12
#define ALM_CRM_TIMEOUT			13
#define ALM_BCP_TIMEOUT			14
#define ALM_BRO_TIMEOUT			15
#define ALM_BCS_TIMEOUT			16
#define ALM_BCL_TIMEOUT			17
#define ALM_INSLT_COMM			18
#define ALM_INSLT				19
#define ALM_RECT_OVER_TEMP		20
#define ALM_RECT_FAN			21
#define ALM_RECT_FAULT			22
#define ALM_RECT_PROTECT		23
#define ALM_EPO					24

#define FAULT_LOG_RAM_NUM		50
#define FAULT_LOG_VIEW_NUM		5

#define MENU_79_PLUG_IN_SPEAR		79
#define MENU_78_ORDER_NOTE			78
#define MENU_80_TAKE_OF_SPEAR		80
#define MENU_81_PLUG_IN_SPEAR		81		//81 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽?庛仛闁跨喐鏋婚幏閿嬬仚 闁跨喓鐓?閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔恍掗敍宀勬晸閺傘倖瀚归柨鐔活潡婵?鎴滅串閹风兘鏁撻弬銈嗗?归摀宥囥偡濮樻劙鏁撶槐绺奙E闁跨喐鏋婚幏鐑芥晸閼恒儻绱濋柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔活潡閻у憡瀚归懓宥夋晸鐞涙?佸敾閹烽攱妲ч柨鐔烘櫕閿涳拷
#define MENU_82_PLUG_IN_SPEAR		82
#define MENU_83_BusinessSelet		83
#define MENU_41_ALM_NOTE			41
#define MENU_10_HOME				10
#define MENU_123_APP_CHARGE			123
#define MENU_127_ADMIN_CHARGE		127
#define MENU_124_BRUSH_CHARGE		124
#define MENU_125_Charge_Result		125
#define MENU_126_CONFIM_STOP		126
#define MENU_92_PSWD				92
#define MENU_84_SELECT				84
#define MENU_85_ENERGY_SELECT		85
#define MENU_86_TIME_SELECT			86
#define MENU_87_REMOTE_START		87
#define MENU_88_TIME_SELECT			88
#define MENU_110_FAULT_VIEW			110

/************************************************ 濡?锟犳晸閺傘倖瀚归柨鐔告灮閹烽?庢緱閻㈡搩鐓夐幏鐑芥晸閿燂拷 *******************************************************/
#define CTRL_RECT_OPEN	1
#define CTRL_RECT_CLOSE	0

#define RECT_VoltageSum_VALUE			0
#define RECT_CurrentSum_VALUE			1

#define INVALID_PARAM_RECT			0

//rect alarm index
#define RECT_ACNotBalance_Alarm		4
#define Rect_LoadShare_Alarm		5

#define RECT_ACVoltHigh_Alarm		6
#define RECT_ACVoltLow_Alarm		7
#define Rect_ACLostPhase_Alarm		8
#define Rect_IDSame_Alarm			9
#define Rect_OutVoltHigh_Alarm		10
#define Rect_OverTemp_Alarm			11
#define Rect_FanFault_Alarm			12
#define Rect_Protect_Alarm			13
#define Rect_Fault_Alarm			14
#define Rect_OutPutShort_Alarm		15
#define Rect_CommBreak_Alarm		16


//rect status index
#define RECT_GroupInfo_ID	    0
#define RECT_ACLimitPower_STATUS    0
#define RECT_TempLimitPower_STATUS  1
#define RECT_LimitPower_STATUS      2
#define RECT_Close_STATUS           3
#define RECT_FanFullSpeed_STATUS    4
#define RECT_WalkIn_STATUS          5
#define RECT_HVD_STATUS             6
#define RECT_SequenceStart_STATUS   7


//rect alarm index
#define RECT_ACFailure_STATUS       8
#define RECT_OverTemp_STATUS        9
#define RECT_Fault_STATUS           10
#define RECT_Protect_STATUS         11
#define RECT_FanFault_STATUS        12
#define RECT_EEPROMFault_STATUS     13
#define RECT_LoadShare_STATUS       14
#define RECT_HVSD_STATUS            15
#define RECT_ACFailureAlarm_STATUS  16
#define RECT_NUMBER_SUM	            17

//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘顣╅柨鐔告灮閹凤拷  -----	100

//rect analog index
#define RECT_VoltageValue_VALUE  	100
#define RECT_RealCurrent_VALUE   	101
#define RECT_CurrentLimit_VALUE  	102
#define RECT_Temperature_VALUE   	103
#define RECT_ACVoltage_VALUE     	104
#define RECT_VoltageUp_VALUE     	105
#define RECT_CurrentValue_VALUE  	106
#define RECT_AC3PVoltageA_VALUE  	107
#define RECT_AC3PVoltageB_VALUE  	108
#define RECT_AC3PVoltageC_VALUE  	109
#define RECT_ALLTIME                110
#define RECT_POWER_VALUE            111

#define RECT_G_CURR_SUM				135
#define RECT_G_LAST_RECT_SUM		136

//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷 150--160
#define RECT_G_ON_OFF_ALL			150

#define RECT_BARCODE1_VALUE         160
#define RECT_BARCODE2_VALUE         161
#define RECT_BARCODE3_VALUE         162
#define RECT_BARCODE4_VALUE         163
#define RECT_BARCODE5_VALUE         164
#define RECT_BARCODE6_VALUE         165
#define RECT_BARCODE7_VALUE         166
#define RECT_BARCODE8_VALUE         167
#define RECT_BARCODE9_VALUE         168
#define RECT_BARCODE10_VALUE        169
#define RECT_BARCODE11_VALUE        170
#define RECT_BARCODE12_VALUE        171
#define RECT_BARCODE13_VALUE        172
#define RECT_BARCODE14_VALUE1		173
#define RECT_BARCODE15_VALUE        174
#define RECT_BARCODE16_VALUE        175
#define RECT_BARCODE17_VALUE        176
#define RECT_BARCODE18_VALUE        177
#define RECT_BARCODE19_VALUE        178
#define RECT_BARCODE20_VALUE        179
#define RECT_BARCODE21_VALUE        180
#define RECT_BARCODE22_VALUE        181
#define RECT_BARCODE23_VALUE        182
#define RECT_BARCODE24_VALUE        183
#define RECT_BARCODE25_VALUE        184
#define RECT_BARCODE26_VALUE        185
#define RECT_BARCODE27_VALUE        186
#define RECT_BARCODE28_VALUE        187
#define RECT_SERIAL0_VALUE          188

#define RECT_CURRLIMIT_SET          189

#define RECT_ESTOP_STATUS           190
#define RECT_AC_VOLT_TOTAL_ALM		191


//DCEM 閺堫亙濞囬柨鐔虹叓閺傘倖瀚归柨鐔告灮閹凤拷 LCD  閸掔娀鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷  闁跨喐鏋婚幏鐑芥晸鐟欙絽褰傞柨鐔告灮閹风兘鏁撴潪鑳?鎻?閹风兘鏁撻弬銈嗗??
#define DCEM_Voltage_VALUE          120
#define DCEM_Current1_VALUE         121
#define DCEM_Current2_VALUE         122
#define DCEM_Current3_VALUE         123
#define DCEM_Current4_VALUE         124
#define DCEM_KWH1_VALUE             125
#define DCEM_KWH2_VALUE             126
#define DCEM_KWH3_VALUE             127
#define DCEM_KWH4_VALUE             128
#define DCEM_KWatt1_VALUE           129
#define DCEM_KWatt2_VALUE           130
#define DCEM_KWatt3_VALUE           131
#define DCEM_KWatt4_VALUE           132
#define DCEM_DCEM2_VALUE            140

#define RECT_Voltage_VALUE			17
#define RECT_AC3PHAVoltage_VALUE    20
#define RECT_AC3PHBVoltage_VALUE    21
#define RECT_AC3PHCVoltage_VALUE    22


#define UNUSED_XXX_ID		0

/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭化鑽ょ埠闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?*/
#define SYSTYPE_24V_SET             4				/*4:24V/set*/
#define SYSTYPE_48V_100             5				/*5:48V/100*/
#define SYSTYPE_48V_300             6				/*6:48V/300*/
#define SYSTYPE_48V_500             7				/*7:48V/500*/
#define SYSTYPE_48V_1000            8				/*8:48V/1000*/
#define SYSTYPE_48V_SET             9				/*9:48V/set*/
/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭化鑽ょ埠闁跨喐鏋婚幏鐑芥晸閺傘倖瀚瑰Ο鈥崇础*/
#define SYSMODE_AC					0				/*闁跨喐鏋婚幏鐑芥晸閸欘偆鏁稿??鈧?闁跨喐鍩呯喊澶嬪?瑰┃鎰?膩瀵?锟?*/
#define SYSMODE_COOPERATE			1				/*闁跨喐鏋婚幏鐑芥晸閹搭亞顣?閹烽攱绨?+婢额亪鏁撻弬銈嗗?归柨鐔告緝閸戙倖瀚归柨鐔告灮閹烽攱濮囬柨鐔告灮閹峰嘲绱? 闁跨喐鏋婚幏鐑芥晸缂傜繝绱?閹风兘鏁撻弬銈嗗?瑰Ο鈥崇础*/
#define SYSMODE_MIX					2				/*闁跨喐鏋婚幏鐑芥晸閹搭亞顣?閹烽攱绨?濡?锟犳晸閺傘倖瀚?+MPPT濡?锟犳晸閺傘倖瀚? 闁跨喐鏋婚幏鐑芥晸娓氥儻鍚夐敓锟?*/
#define SYSMODE_SOLAR               3				/*闁跨喐鏋婚幏绋璓PT濡?锟犳晸閺傘倖瀚瑰Ο鈥崇础*/
#define SYSMODE_NUM                 4

/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔惰寧閿涱亝鏋婚幏鐑解攼闁跨喐鏋婚幏閿嬫嫚闁跨喐鏋婚幏鐑芥晸閿燂拷 */
#define CTRL_SAVE                   0				/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚规稉娲?鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?规径顏堟晸閺傘倖瀚归柨鐔告灮閹凤拷*/
#define CTRL_STABLY                 1				/*闁跨喖銈虹拋瑙勫?规稉娲?鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻幒銉ь劜閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐕傛嫹*/

#define  CUT_TYPE_CLOSE		0						//闁跨喐鏋婚幏鐑芥晸缁夊憡甯存潏鐐?瀚归柨鐔告灮閹凤拷   
#define  CUT_TYPE_OPEN		1						//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔稿复鏉堢偓瀚归柨鐔告灮閹凤拷

/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚瑰Ο锟犳晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??*/
#define RECTTYPE_200A               0
#define RECTTYPE_100A               1
#define RECTTYPE_75A                2
#define RECTTYPE_55A                3
#define RECTTYPE_50A                4
#define RECTTYPE_30A                5
#define RECTTYPE_15A                6
#define RECTTYPE_7_5A               7
#define RECTTYPE_37_5A              8
#define RECTTYPE_86A                9
#define RECTTYPE_43A                10

/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚规径顏堟晸閺傘倖瀚归柨鐔告灮閹烽攱膩瀵?蹇涙晸闁惧府缍囬幏鐑芥晸閺傘倖瀚瑰┃鎰版晸閺傘倖瀚归柨鐔峰建绾板?嬪?瑰?樻劙鏁撻弬銈嗗?归崡鍝ラ兇闁跨喐鏋婚幏鐑芥晸閿燂拷*/
#define HaveBattCurr               2
#define ENTER_BY_IC_CARD	0
#define ENTER_BY_PASSWORD	1
#define RUN_TYPE_IMMEDIATELY	0
#define RUN_TYPE_TIME		1
#define CHARGE_METOD_IS_BMS	0
#define CHARGE_METOD_IS_TIME	1
#define CHARGE_METOD_IS_ENERGY	2
#define CHARGE_METOD_IS_MONEY	3

#define TASK_STK_SIZE_CAN1			1200//1400
#define TASK_STK_SIZE_EEPROM		600
#define TASK_STK_SIZE_MODEM			600
#define TASK_STK_SIZE_MEASURE		500
#define TASK_STK_SIZE_ALARM			500
#define TASK_STK_SIZE_CAN2			800
#define TASK_STK_SIZE_SELF_TEST		500
#define TASK_STK_SIZE_RTC			500
#define TASK_STK_SIZE_CAN3_TX		500
#define TASK_STK_SIZE_CAN3			600
#define TASK_STK_SIZE_USB			800//1000
#define TASK_STK_SIZE_UART5			600
#define TASK_STK_SIZE_ETHERNET		800
#define TASK_STK_SIZE_I2C			800
#define TASK_STK_SIZE_DISPLAY		400
#define TASK_STK_SIZE_PWM			500
#define TASK_STK_SIZE_UART4			600
#define TASK_STK_NET_ZHIXIN			800
#define TASK_STK_SIZE_UART1			600
#define TASK_STK_SIZE_FLASH			800

#define IS_LOCK		1
#define IS_UN_LOCK	0
#define  IS_FIRST_RUNNING	1
#define  ISNT_FIRST_RUNNING	0


#define BY_E2PROM			1
#define BY_SRAM				0


#define	mENGLISH			0
#define mCHINESE            1

#define mCONFIG             0
#define mMENU               1

#define	mDISABLE	        0
#define	mENABLE		        1

#define mESHUTDOWN          1
#define mESTOP              2


#define mPOWERSPLIT         1

#define mESHUTDOWN_DI1      10
#define mESHUTDOWN_DI2      11
#define mESHUTDOWN_DI3      12
#define mESHUTDOWN_DI4      13
#define mESHUTDOWN_DI5      14
#define mESHUTDOWN_DI6      15
#define mESHUTDOWN_DI7      16
#define mESHUTDOWN_DI8      17
#define mESHUTDOWN_SPD      18

#define mESTOP_DI1          20
#define mESTOP_DI2          21
#define mESTOP_DI3          22
#define mESTOP_DI4          23
#define mESTOP_DI5          24
#define mESTOP_DI6          25
#define mESTOP_DI7          26
#define mESTOP_DI8          27
#define mESTOP_SPD          28

#define	mFALSE				0
#define	mTRUE				1

#define	mNONE				0
#define	mBATTCURR			1
#define	mLOADCURR		    2 
#define	mBATTERYTEMP	    1
#define	mAMBIENTTEMP	    2

//#define	mBATTERY2TEMP	3
//#define	mAMBIENT2TEMP	4

#define mTC1                0
#define mTC2                1
#define mTC3                2

#define mDHCP_NEED_INIT			0//闁跨喐鏋婚幏鐤?顩﹂柨鐔告灮閹风兘鏁撻弬銈嗗??
#define mDHCP_REQUESTING		1//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
#define mDHCP_CONFIGURED		2//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归弲鎺楁晸閿燂拷

#define mUART               0
#define mNET                1

#define mYDN23              0
#define mEEM                1
#define	mRSOC 	            2
#define mSOCTPE             3

#define	mMODEM				1
#define	mRS232				0

#define	m9600				0
#define	m4800				1
#define	m2400				2
#define	m1200				3
#define	m19200				4
#define	m38400				5
#define	m115200				6

#define	mECU				0
#define	mMANUAL				1
#define	mAUTO				2

//#define   mNONE           0
#define mAC1                1
#define mAC2                2
#define mACERROR            3

#define mDGRun				0
#define mATS1               1
#define mATS2               2
#define mATSAll             3

#define mMains				0
#define mDG1                1
#define mDG2                2
#define mBattery            3

#define mDEFAULT	0
#define mDEFAULT2	1

#define	mPERCENT			0
#define	mTIME				1
#define	mCAP				2

#define	mVOLTMODE			0
#define	mTIMEMODE			1

#define	mLEVEL				0//mono
#define	mPULSE				1//bistable

#define	mLOWVOLT			1
#define	mCUTBATT			2
#define mCUTLOAD            3
#define mCUTBOTH            4

//#define   mNONE                              0
#define	m1PHASE				1
#define	m3PHASE				2

#define	mOBSERVATION	    1
#define	mMAJOR				2
#define	mCRITICAL			3

#define	mLOWALARM			0
#define	mHIGHALARM		    1
  
#define	mLOWLEVEL			0
#define	mHIGHLEVEL		    1

#define	mNORMAL				0
#define	mACTIVE				1
#define mDEACTIVE           2

#define mFAULT              1

#define mSAVEPOWERDELAY     5

#define mNOLIMIT            1
#define mSMOOTHRECON        2

#define	mTRIMZERO			128

#define	mFLOATCHARGE	    0
#define	mTEMPCOMP			1
#define	mNEEDEC				2
#define	mAUTOEC				3
#define	mTIMEEC				4
#define	mAUTOTEST			5
#define	mAUTOSTABLETEST		6
#define	mSHORTTEST		    7

#define	mRELAY1				1
#define	mRELAY2				2
#define	mRELAY3				3
#define	mRELAY4				4
#define	mRELAY5				5
#define	mRELAY6				6
#define	mRELAY7				7
#define	mRELAY8				8

#define	mOPTION1            1
#define	mOPTION2            2
#define	mOPTION3            3
#define	mOPTION4            4
#define	mOPTION5            5    //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚笵I

#define mHOT                0 //闁跨喖銈洪弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗??
#define mAIR                1 //閻╂挳鈧?姘舵晸閺傘倖瀚归柨鐔告灮閹凤拷


#define mTempSensor1        0//闁跨喖鎽?鎼达箒鎻?閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷1
#define mTempSensor2        1 //闁跨喖鎽?鎼达箒鎻?閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷2

#define	m1PHAir				0//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归惄蹇涙晸閿燂拷
#define	m3PHAir				1//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归惄蹇涙晸閿燂拷


#define COUNTTIME           10  //i2c 闁?姘愁唵闁跨喎褰ㄧ拋瑙勫?归柨鐔峰建閺傤叀鎻?閹风兘鏁撻弬銈嗗??
#define LOCK                1   //menu闁跨喐鏋婚幏绌恊aderlock


#define mDAY                0   //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚? 
#define mNIGHT              1   //闁跨喐鏋婚幏宄邦檨
#define mPVFAULT            2   //PV闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?

#define EEM_PORT            5050
#define EEM_NO_INPUT        0
#define EEM_INPUT_DATA      1
#define EEM_INPUT_DATA_TAKEN 2
#define EEM_OUTPUT_PREPARE_UP 3

#define  SWITCH_IN_AC1      2    //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗??1
#define  SWITCH_IN_AC2      3    //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗??2
  
#define mReceive            0  
#define mTransmit           1

//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?瑰Ο鈥崇础
#define  MAINS_ACPOWER		0   //闁跨喎褰ㄧ喊澶嬪??
#define  DG_ACPOWER			1   //闁跨喖鍙烘导娆愬??
#define  MAINS_DG_ACPOWER	2   //闁跨喎褰ㄧ喊澶嬪??+闁跨喖鍙烘导娆愬??
#define  NONE_ACPOWER		3   //闁跨喓娼鹃弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷

//闁跨喖鍙烘导娆愬?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风柉妫旈柨鐕傛嫹
#define  MAINS_ON_SIGNAL	0   //闁跨喎褰ㄩ悽鍏哥返闁跨喐鏋婚幏鐑芥晸閼存氨灏ㄩ幏锟?
#define  DG_ON_SIGNAL		1   //闁跨喖鍙烘导娆愬?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔诲壖閻氬瓨瀚?
#define  DG_OILLEVEL_SIGNAL	2   //闁跨喖鍙烘导娆愬?归柨鐔告灮閹疯渹缍呴柨鐔诲壖閻氬瓨瀚?
#define  DG_FAULT_SIGNAL	3   //闁跨喖鍙烘导娆愬?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔诲壖閻氬瓨瀚?

//闁跨喖鍙烘导娆愬?归柨鐔告灮閹风兘鏁撻弬銈嗗?归悩鑸碘偓锟?
#define  DG_STOP            0
#define  DG_NEEDSTART       1
#define  DG_START           2
#define  DG_TEST            3


#define   DCEM_CTRL_LED_BLINK   10
#define   DCEM_CTRL_LED_NOBLINK 11
#define   DCEM_CTRL_CLEAR1      1
#define   DCEM_CTRL_CLEAR2      2
#define   DCEM_CTRL_CLEAR3      3
#define   DCEM_CTRL_CLEAR4      4
#define   DCEM_CTRL_DISABLE1    5
#define   DCEM_CTRL_DISABLE2    6
#define   DCEM_CTRL_DISABLE3    7
#define   DCEM_CTRL_DISABLE4    8
#define   DCEM_CTRL_ENABLE_ALL  9

//2011-05-06 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔惰寧閿涱亝鏋婚幏閿嬪珴闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔惰寧閿濆繑瀚瑰Δ锔规?勯幏鐑芥晸閺傘倖瀚归崢瀣?鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔烘骄閹插??瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?圭喊宀勬晸鐞涙?呮交閹风兘鏁撻弬銈嗗?归崢瀣?鏁撻弬銈嗗?归柨鐕傛嫹0.5V闁跨喐鏋婚幏鐑芥晸閻ㄥ棗鍤栭幏鐑芥晸閻?顐㈠殩閹风兘鏁撴潏鍐?顣?閹凤拷
#define RECT_FCVOLT_HIGHTHAN_LVDRSTVOLT 0.5

//濞戞煡鏁撻弬銈嗗?瑰Ο锟犳晸閺傘倖瀚归柨鐔告灮閹风ìED闁跨喓瀚涢崚鍡欘暜閹风柉澹婇柨鐔告灮閹峰嘲寮婚懝鏌ユ晸閺傘倖瀚归柨鐔煎徍閿濆繑瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?瑰??蹇涙晸閺傘倖瀚归崥锟?  NameData.AlarmLedType
#define  LED_TYPE_SINGLE       0
#define  LED_TYPE_DOUBLE       1

/************************************************/
/*E2PROM闁跨喐鏋婚幏宄版絻闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?*/
/*24C512:64K Bytes*/
/*闁跨喐鏋婚幏宄版絻闁跨喐鏋婚幏宄版纯:0x0000--0xffff*/
/************************************************/
/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??:闁跨喕顢滈崙銈嗗?归柨鐔告灮閹凤拷*/
#define	E2PROMCharData			0x0000				/*0x0000-0x0fff:4096闁跨喐鏋婚幏鐑芥晸鐞涙?佹灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷*/
#define E2PROMSysType			0x0000				/*闁跨喐鏋婚幏铚傜?撮柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷*/
/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??:闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??*/
#define	E2PROMFloatData			0x1000				/*0x1000-0x1fff:4096闁跨喕顢滈弬銈嗗??*/

/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻幑閿嬪敾閹风兘鏁撻弬銈嗗??1*/
/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??:闁跨喕顢滈崙銈嗗?归柨鐔告灮閹凤拷*/
#define	E2PROMCharData1		      0x2000
/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??:闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??*/
#define	E2PROMFloatData1	      0x3000

/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻幑閿嬪敾閹风兘鏁撻弬銈嗗??2*/
/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??:闁跨喕顢滈崙銈嗗?归柨鐔告灮閹凤拷*/
#define	E2PROMCharData2		      0x4000
/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??:闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??*/
#define	E2PROMFloatData2	      0x5000

/*default setdata*/
#define E2PROMDEFAULTCharData   0x6000
#define E2PROMDEFAULTFloatData  0x7000



#define FAULT_LOG_NUM_ADDR_H	34001
#define FAULT_LOG_NUM_ADDR_L	34002
#define FAULT_LOG_PST_ADDR_H	34003
#define FAULT_LOG_PST_ADDR_L	34004

#define FAULT_LOG_VAL_ADDR		34010		//0X84DA	(0xAEE0 - 0X84DA=0X2A06=10758闁跨喐鏋婚幏鐑芥晸鐞涙?佹灮閹凤拷)
#define MAX_FAULT_LOG_NUM		200
#define VIEW_FAULT_PER_FIVE		5




#define E2PROM_NEW_BOARDFlag1     0xbbe3
#define E2PROM_NEW_BOARDFlag2     0xbbe4

/*MAC闁跨喐鏋婚幏宄版絻6闁跨喐鏋婚幏鐑芥晸鐞涙?佹灮閹凤拷*/
#define E2PROMMACADD        0xbbe5       /*0xbbe5~0xbbea*/

/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲绻?*/
#define E2PROMFLAGADD1      0xbbeb
#define E2PROMFLAGADD2      0xbbec
/*闁跨喐鏋婚幏鐑芥晸閻?顐ｅ敾閹峰嘲绻?*/
#define	E2PROMSetFlag1		  0xbbed				/*0x55*/
#define	E2PROMSetFlag2		  0xbbee				/*0xaa*/

#define E2PROMDEFAULTFlag1  0xbbef        /*0x55*/
#define E2PROMDEFAULTFlag2  0xbbf0        /*0xaa*/

/*閺嶁?愠鍣?闁跨喐鏋婚幏宄扮箶*/
#define	E2PROM_U1_UPDATA_FLAG		  0xbbf1				/*0x55*/
#define	E2PROM_U2_UPDATA_FLAG		  0xbbf2				/*0xaa*/

/*闁跨喐鏋婚幏宄板蕉闁跨喐鏋婚幏鐑芥晸閹归攱鍞婚幏宄扮箶*/
#define	E2PROMEventFlag1	  0xbbf3				/*0x55*/
#define	E2PROMEventFlag2	  0xbbf4				/*0xaa*/

/*闁跨喕濮ら張锟?*/
//#define	E2PROMVersion1		  0xbbf5				/*闁跨喕濮ら張顒勬晸閺傘倖瀚规担锟?*/
//#define	E2PROMVersion2		  0xbbf6				/*闁跨喕濮ら張顒勬晸閺傘倖瀚规担锟?*/
//绾?顒勬晸閺傘倖瀚归柨鐔诲Г閺堬拷
#define E2PROM_HW_VER_HIGH	  0xbbf5				//A\B\C\...
#define E2PROM_HW_VER_LOW	  0xbbf6				//00\01\02...

/*閺冨爼鏁撻弬銈嗗??*/
#define	E2PROMTimeFlag1		  0xbbf7				/*0x55*/
#define	E2PROMTimeFlag2		  0xbbf8				/*0xaa*/

/*20210322-LKF闁跨喐鏋婚幏绋?oot闁跨喕濮ら張锟?*/
#define	E2PROM_BOOT_VER_VALID_FLAG	  0xbbfc //0xbbf9		//BOOT闁跨喕濮ら張顒勬晸鐟欐帒鍤栭幏鐑芥晸閺傘倖瀚归弫鍫ユ晸閺傘倖瀚硅箛妤呮晸閺傘倖瀚归柨鐔虹?峞prom闁跨喐鏋婚幏宄版絻
#define	E2PROM_BOOT_VERSION			  0xbbfd //0xbbfa		//BOOT闁跨喕濮ら張顒勬晸閺傘倖瀚归柨鐔虹?峞prom闁跨喐鏋婚幏宄版絻,閸欘亙濞囬柨鐔告灮閹疯渹绔撮柨鐔告灮閹风兘鏁撶悰妤佹灮閹凤拷,闁跨喐鏋婚幏宄板?燘OOT闁跨喍鑼庨悧鍫熸拱,闁跨喐鏋婚幏锟?1闁跨喐鏋婚幏宄邦潗闁跨喐鏋婚幏鐑芥晸閺傘倖瀚规稉娲?鏁撴潏鍐?澧楅張顒佺梾闁跨喐鏋婚幏宄板晸闁跨喐鏋婚幏鐑芥晸缁辩瓌OT闁跨喕濮ら張锟?,闁跨喐鏋婚幏宄板?犻柨鐔告灮閹疯渹璐焫0ff/0x00闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽攱鏅ラ崐锟?
#define	BOOT_VER_VALID_FLAG_Value	  0xA5			////BOOT闁跨喕濮ら張顒勬晸閺傘倖瀚归弫鍫ユ晸閺傘倖瀚硅箛妤?汊??锟?
#define BOOT_VER_HIGH_support_AppUpgarde_ExflashOver512k 2  //閺?泐垶鏁撻弬銈嗗?归柨鐔恍掗柈鈺?lash闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??512k闁跨喐鏋婚幏绋?OOT闁跨喕濮ら張顒勬晸閺傘倖瀚?



//#define E2PROMBootloaderUpdateFlag1     0xbbfd	
#define E2PROMBootloaderUpdateFlag1     0xbbfb	   //20230201 闁跨喐鏋婚幏宄版絻闁跨喐鏋婚幏椋庣崐闁跨喐鏋婚幏鐑芥晸閺傘倖瀚规稉锟?0xbbfb

#define E2PROMBootloaderUpdateFlag2     0xbbfe    //eeprom闁跨喐鏋婚幏宄版絻




#define	E2PROMNumber		    0xbbff				/*濡?锟犳晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?瑰Ο锟犳晸娴犲?╂丢婢堕亶鏁撻懞銉?顒?*/

//#define	E2PROMSPLASH		    0xbc00				/*0xbc00-0xbfff:1024闁跨喕顢滈弬銈嗗??*/

/*闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲缍?*/
#define	E2PROMRResetReasonPos 		0xbc00
#define	E2PROMRResetReason1	  		0xbc01			
#define	E2PROMRResetDogBit1	  		0xbc02				
#define	E2PROMRResetReason2	  		0xbc06			
#define	E2PROMRResetDogBit2	  		0xbc07
#define	E2PROMRResetReason3	  		0xbc0b			
#define	E2PROMRResetDogBit3	  		0xbc0c

#define	E2PROMRPowerON_1_Time	 	 0xbc40
#define	E2PROMRPowerON_2_Time	 	 0xbc44

//濡?锟犳晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撴笟銉с?銒閹风兘鏁撻弬銈嗗?归柨鐔哄珱閿濆繑瀚归崡鐘绘晸閺傘倖瀚?0xbc60~0xbc6F闁跨喐鏋婚幏宄版絻闁跨喐鏋婚幏宄版纯
#define	E2PROM_Rect_Upgade_BinFileName_U1	 	 0xbc60
#define	E2PROM_Rect_Upgade_BinFileName_U2	 	 0xbc68

/*another 64k eeprom for menu define*/
#define E2PROMMenuConfig      0x0000
#define E2PROMSystemName      E2PROMMenuConfig        /*32*60=1920 bytes*/
//#define E2PROMAliasName       E2PROMSystemName + sizeof(System_TypeDef) * MAX_SYSTEM_NUM/*3200 bytes*/
#define E2PROMMenuName        E2PROMSystemName + sizeof(System_TypeDef) * MAX_SYSTEM_NUM
#define E2PROMAliasName       0x780    //E2PROMMENUNAME + sizeof(Menu_TypeDef) * MAX_MANU_NUM
#define E2PROMLineConfig      E2PROMAliasName + sizeof(Alias_TypeDef) * MAX_ALIAS_NUM   /*15360 bytes*/
#define E2PROMScreenConfig    E2PROMLineConfig + sizeof(Line_TypeDef) * MAX_LINE_NUM    /*2720 bytes*/

#define E2PROMAlarmName       E2PROMScreenConfig + sizeof(ScreenCfg_TypeDef) * MAX_SCREEN_NUM  /*32768 bytes*/
#define E2PROMAnalogAlarmName E2PROMAlarmName + sizeof(Alarm_TypeDef) * mALARMNAMENUM   /*9408 bytes*/

//#define E2PROMNext            E2PROMAnalogAlarmName + sizeof(AnalogAlarm_TypeDef) * mAANUM /*65376=0xff60*/

#define E2PROMMENUDATA      0xffe0  /*闁跨喐鏋婚幏鐑芥晸閹恒儴褰嶇喊澶嬪?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗??*/

#define START_CHARGE_VAL	0
#define STOP_CHARGE_VAL		1

#define SPEAR_TYPE_IS_PULSE			0
#define SPEAR_TYPE_IS_VOLT			1
#define SPEAR_TYPE_IS_SOCKET		1
#define SPEAR_TYPE_NO_SOCKET		0



//闁跨喐鏆?沌喊??嬪??
#define __GREEN_LED_ON() do {} while (0);
#define __GREEN_LED_OFF() do {} while (0);

//A	闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define __A_YELLOW_LED_ON() do {\
} while (0);
#define __A_YELLOW_LED_OFF() do {\
} while (0);

//B	闁跨喐鏋婚幏鐑芥晸閺傘倖瀚?
#define __B_YELLOW_LED_ON() do {\
} while (0);
#define __B_YELLOW_LED_OFF() do {\
} while (0);


	// //A		闁跨喕濮ょ拃锕傛晸閺傘倖瀚?
	// #define __A_RED_LED_ON() do {\
	// 			BSP_DO_Ctrl(NEWDO14, DO_ON);\
	// } while (0);
	// #define __A_RED_LED_OFF() do {\
	// 			BSP_DO_Ctrl(NEWDO14, DO_OFF);\
	// } while (0)

	// //B		闁跨喕濮ょ拃锕傛晸閺傘倖瀚?   20190722--DO16闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔稿复鏉堢偓瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷
	// #define __B_RED_LED_ON() do {\
	// 			BSP_DO_Ctrl(NEWDO14, DO_ON);\
	// } while (0);
	// #define __B_RED_LED_OFF() do {\
	// 			BSP_DO_Ctrl(NEWDO14, DO_OFF);\
	// } while (0)



//闁跨喐鏋婚幏鐑芥晸閺夋壆顣?閹烽?庡煝闁跨喐鏋婚幏閿嬆侀柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹峰嘲鎽曢柨鐔告灮閹风兘鏁撻敓锟? 
#define __ENERGY_BATT_CHG_AC_ON() do {\
	gs_IMMU2_LCCU_Mng.u8ACInputMccbCtrl = DO_ON;\
	BSP_DO_Ctrl(NEWDO16, DO_ON);\
	BSP_DO_Ctrl(NEWDO16, DO_ON);\
	BSP_DO_Ctrl(NEWDO16, DO_ON);\
	} while (0);
#define __ENERGY_BATT_CHG_AC_OFF() do {\
	gs_IMMU2_LCCU_Mng.u8ACInputMccbCtrl = DO_OFF;\
	BSP_DO_Ctrl(NEWDO16, DO_OFF);\
	BSP_DO_Ctrl(NEWDO16, DO_OFF);\
	BSP_DO_Ctrl(NEWDO16, DO_OFF);\
	} while (0);

#define __ACLOAD_MCCB_ON() do {\
	gs_IMMU2_LCCU_Mng.u8ACOutputMccbCtrl = DO_ON;\
	BSP_DO_Ctrl(NEWDO15, DO_ON);\
	BSP_DO_Ctrl(NEWDO15, DO_ON);\
	BSP_DO_Ctrl(NEWDO15, DO_ON);\
	} while (0);
#define __ACLOAD_MCCB_OFF() do {\
	gs_IMMU2_LCCU_Mng.u8ACOutputMccbCtrl = DO_OFF;\
	BSP_DO_Ctrl(NEWDO15, DO_OFF);\
	BSP_DO_Ctrl(NEWDO15, DO_OFF);\
	BSP_DO_Ctrl(NEWDO15, DO_OFF);\
	} while (0);




#define __FAN_ON() do {\
	BSP_DO_Ctrl(NEWDO10, DO_ON);\
} while (0);

#define __FAN_OFF() do {\
	BSP_DO_Ctrl(NEWDO10, DO_OFF);\
} while (0);

#define D1_Ctrl_Off() do {\
	BSP_DO_Ctrl(NEWDO11, DO_OFF);\
} while (0);

#define D1_Ctrl_On() do {\
	BSP_DO_Ctrl(NEWDO11, DO_ON);\
} while (0);

#define D2_Ctrl_Off() do {\
	BSP_DO_Ctrl(NEWDO12, DO_OFF);\
} while (0);

#define D2_Ctrl_On() do {\
	BSP_DO_Ctrl(NEWDO12, DO_ON);\
} while (0);







//B		闁跨喕濮ょ拃锕傛晸閺傘倖瀚?
#define __AC_CHARGE_LED_ON() do {\
} while (0);
#define __AC_CHARGE_LED_OFF() do {\
} while (0);

//by lxr01202
#define __BATT_GRP_1_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[0] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[0] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[0] = DO_ON;\
	BSP_DO_Ctrl(NEWDO13, DO_ON);\
} while (0);
#define __BATT_GRP_1_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[0] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[0] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[0] = DO_OFF;\
	BSP_DO_Ctrl(NEWDO13, DO_OFF);\
} while (0);

#define __BATT_GRP_2_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[1] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[1] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[1] = DO_ON;\
	BSP_DO_Ctrl(NEWDO15, DO_ON);\
} while (0);
#define __BATT_GRP_2_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[1] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[1] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[1] = DO_OFF;\
	BSP_DO_Ctrl(NEWDO15, DO_OFF);\
} while (0);

#define __BATT_GRP_3_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[2] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[2] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[2] = DO_ON;\
	BSP_DO_Ctrl(NEWDO13, DO_ON);\
} while (0);
#define __BATT_GRP_3_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[2] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[2] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[2] = DO_OFF;\
	BSP_DO_Ctrl(NEWDO13, DO_OFF);\
} while (0);

#define __BATT_GRP_4_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[3] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[3] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[3] = DO_ON;\
	BSP_DO_Ctrl(NEWDO14, DO_ON);\
} while (0);
#define __BATT_GRP_4_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[3] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl_Delay[3] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8BatteryGrpXInputContactorCtrl[3] = DO_OFF;\
	BSP_DO_Ctrl(NEWDO14, DO_OFF);\
} while (0);

//by lxr01202
#define __MPPT_1_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[0] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl_Delay[0] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[0] = DO_ON;\
	BSP_DO_Ctrl(NEWDO12, DO_ON);\
} while (0);
#define __MPPT_1_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[0] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl_Delay[0] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[0] = DO_OFF;\
	BSP_DO_Ctrl(NEWDO12, DO_OFF);\
} while (0);

#define __MPPT_2_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[1] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl_Delay[1] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[1] = DO_ON;\
	BSP_DO_Ctrl(NEWDO14, DO_ON);\
} while (0);
#define __MPPT_2_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[1] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl_Delay[1] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[1] = DO_OFF;\
	BSP_DO_Ctrl(NEWDO14, DO_OFF);\
} while (0);



#define __MPPT_3_INPUT_ON() do {\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[2] = DO_ON;\
} while (0);
#define __MPPT_3_INPUT_OFF() do {\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[2] = DO_OFF;\
} while (0);

#define __MPPT_4_INPUT_ON() do {\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[3] = DO_ON;\
} while (0);
#define __MPPT_4_INPUT_OFF() do {\
	gs_IMMU2_LCCU_Mng.u8MPPTXInputContactorCtrl[3] = DO_OFF;\
} while (0);

#define __DCLOAD_1_INPUT_ON() do {\
	gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[0] = DO_ON;\
} while (0);
#define __DCLOAD_1_INPUT_OFF() do {\
	gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[0] = DO_OFF;\
} while (0);

#define __DCLOAD_2_INPUT_ON() do {\
	gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[1] = DO_ON;\
} while (0);
#define __DCLOAD_2_INPUT_OFF() do {\
	gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[1] = DO_OFF;\
} while (0);

#define __DCLOAD_3_INPUT_ON() do {\
	gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[2] = DO_ON;\
} while (0);
#define __DCLOAD_3_INPUT_OFF() do {\
	gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[2] = DO_OFF;\
} while (0);

#define __DCLOAD_4_INPUT_ON() do {\
	gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[3] = DO_ON;\
} while (0);
#define __DCLOAD_4_INPUT_OFF() do {\
	gs_IMMU2_LCCU_Mng.u8DCLoadXInputContactorCtrl[3] = DO_OFF;\
} while (0);



//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻幒銉ㄦ彧閹风兘鏁撻弬銈嗗??
#define __A_PV1_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[0] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] = DO_ON;\
} while (0);
#define __A_PV1_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[0] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[0] = DO_OFF;\
} while (0);
#define __A_PV2_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[1] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[1] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[1] = DO_ON;\
} while (0);
#define __A_PV2_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[1] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[1] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[1] = DO_OFF;\
} while (0);
#define __A_PV3_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[2] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[2] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[2] = DO_ON;\
} while (0);
#define __A_PV3_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[2] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[2] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[2] = DO_OFF;\
} while (0);
#define __A_PV4_INPUT_ON() do {\
	if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[3] == DO_OFF){\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[3] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[3] = DO_ON;\
} while (0);
#define __A_PV4_INPUT_OFF() do {\
	if(gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[3] == DO_ON){\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl_Delay[3] = ContactorCtrl_FeedbackChkDelay;}\
	gs_IMMU2_LCCU_Mng.u8PVInputMccbCtrl[3] = DO_OFF;\
} while (0);

#define	POWER_MODULE_PWRON_FOR_UPGRADE()	 do {\
} while (0);

#define __START_CHARGE_CMD() do {} while (0);
#define __STOP_CHARGE_CMD() do {} while (0);

//#define FRQ_VAL_TIME5			84000000
//#define PERIOD_TIME_5			8400
//#define PRESCALER_TIME_5		100
//#define PRESCALER_PER_SEC_TIME5	1000
//#define TIME5_T_VAL	((double)PERIOD_TIME_5 * PRESCALER_TIME_5)/(double)FRQ_VAL_TIME5)
//#define TIME5_250MS	 ((double)(250.000/1000.000)/(double)TIME5_T_VAL)
//#define TIME5_50MS	 ((double)(50.000/1000.000)/(double)TIME5_T_VAL)
//#define TIME5_500MS	 ((double)(500.000/1000.000)/(double)TIME5_T_VAL)
//#define TIME5_10MS	 ((double)(10.000/1000.000)/(double)TIME5_T_VAL)

#define FRQ_VAL_TIME5			84000000
//#define PERIOD_TIME_5			8400
#define PERIOD_TIME_5			1680  //2MS
#define PRESCALER_TIME_5		100
#define PRESCALER_PER_SEC_TIME5	1000

#define PERIOD_TIME_7			8400
#define PRESCALER_TIME_7		100
#define PRESCALER_PER_SEC_TIME7	1000
#define PER_CYCL_TIME			2

#define TIME5_T_VAL	 (double)(PERIOD_TIME_5 * PRESCALER_TIME_5)/(double)FRQ_VAL_TIME5

#define TIME5_50MS_COUNT	((((46 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5)))) //GBT闁跨喓鐓?绾板?嬪??
#define TIME5_250MS_COUNT   ((((250 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5)))) //GBT闁跨喓鐓?绾板?嬪??

//#define TIME5_50MS_COUNT_1	((((48 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
//#define TIME5_50MS_COUNT_2	((((50 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
//#define TIME5_50MS_COUNT_3	((((52 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
//#define TIME5_50MS_COUNT_4	((((54 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
//
#define TIME5_500MS_COUNT	((((496 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))	//GBT 闁跨喓鐓?绾板?嬪??
//#define TIME5_500MS_COUNT_1	((((500 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
//#define TIME5_500MS_COUNT_2	((((505 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
//#define TIME5_500MS_COUNT_3	((((510 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
//#define TIME5_500MS_COUNT_4	((((490 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
//#define TIME5_1000MS_COUNT_4	((((1003 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))


#define TIME5_91MS_COUNT	(((((92) * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
#define TIME5_100MS_COUNT	(((((100) * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
#define TIME5_102MS_COUNT	(((((107) * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))

#define TIME5_10MS_COUNT	(((((10) * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))
#define TIME5_2MS_COUNT	((((2 * (FRQ_VAL_TIME5/PRESCALER_PER_SEC_TIME5))/(PERIOD_TIME_5 * PRESCALER_TIME_5))))



typedef struct
{
	uint32_t iFlag; //闁跨喐鏋婚幏宄版姎闁跨喕顫楅崙銈嗗?归柨鐔告灮閹风兘鏁撻敓锟?  閺堫亪鏁撻弬銈嗗?归柨鐔告灮閹凤拷
	uint32_t iDCL;
	uint32_t Id;
	uint8_t Data[8];
	
} canmsgTXY;
typedef struct
{
	uint32_t iFlag; //闁跨喐鏋婚幏宄版姎闁跨喕顫楅崙銈嗗?归柨鐔告灮閹风兘鏁撻敓锟?  閺堫亪鏁撻弬銈嗗?归柨鐔告灮閹凤拷
	CanRxMsgTypeDef RxMsg;

} canmsgTXY2;


struct _TP_CM_RTS
{
	UINT8 PF;
	UINT8 CTLByte;			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔活敎閺傘倖瀚?
	UINT16 TotalMsgByteNum;		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽攱浼呴柨鐔活敎閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗??
	UINT8 TotalPkgNum;
	UINT32 PkgMsgNo;
	UINT8 PkgMsgNoLSB;
	UINT8 PkgMsgNoMID;
	UINT8 PkgMsgNoMSB;
};
typedef struct _TP_CM_RTS TP_CM_RTS_INFO;

struct _Uart4RecvInfo
{
	UINT8 uReceivFlag;			//1ok	0闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
	UINT8 uBuf[MDBUS_BUFF_MAX_NUM];
};
typedef struct _Uart4RecvInfo  UART4_RECV_INFO;

struct _Task_XXXInfo
{
	void          (*fTaskProc)(void *);
	uint16_t      u16Stk_Size;
	uint8_t       u8Priority;
	uint8_t       u8Id;
	OS_STK        *StkPtr;
};
typedef struct _Task_XXXInfo TASK_XXXInfo;


struct ___Sig_Addr
{
	UINT8	u8EType;
	UINT32	u16SigIdx;
	UINT32	u32E2pAddr;
	UINT8	u8SigType;//0 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷	1闁跨喕顢滈弬銈嗗?归柨鐔告灮閹凤拷		2 32娴ｅ秹鏁撻弬銈嗗?归柨鐔告灮閹凤拷
};
typedef struct ___Sig_Addr __SIG_E2P_ADDR;

struct _stFrameInfo
{
	BYTE byLabelDI3;
	BYTE byLabelDI2;
	BYTE byLabelDI1;
	BYTE byLabelDI0;	
	BYTE bySigForm;		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚圭亸蹇涙晸閺傘倖瀚归柨鐔告灮閹烽攱鐦濋柨鐔虹哺閺傘倖瀚归柨鐕傛嫹0闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹峰嘲鐨?闁跨喐鏋婚幏鐑芥晸婵?鎰剁礉1闁跨喐鏋婚幏鐑芥晸閺傘倖瀚规稉鈧?娴ｅ秴鐨?闁跨喐鏋婚幏鐑芥晸婵?鎰剁礉2闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹疯渹缍呯亸蹇涙晸閺傘倖瀚归柨鐔奉潟閿涳拷3,闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹疯渹缍呯亸蹇涙晸閺傘倖瀚归柨鐔告灮閹凤拷4,闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹疯渹缍呯亸蹇涙晸閺傘倖瀚归柨鐔告灮閹凤拷5
	BYTE byRcvLength;	//闁跨喐鍩呴幉瀣?瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归弫鍫ユ晸閼存氨灏ㄩ幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撶悰妤佹灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹凤拷4闁跨喕顢滈弬銈嗗?筨闁跨喐鏋婚幏鐤?鐦戦柨鐔告灮閹凤拷
	BYTE iRoughNo;	//闁跨喕濡?闁插洨銆嬮幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻崣顐ゎ暜閹疯渹缍呴柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归懙鏃堟晸妤楄　妲勯幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹疯渹缍呴柨鐔告灮閹风兘鏁撶憴鎺戝皡閹风兘鏁撻弬銈嗗?归柨鐔稿祹绾板?嬪?归柨鐔告灮閹峰嘲顫愰柨鐔告灮閹风兘鏁撻弬銈嗗?规担宥夋晸閺傘倖瀚?
	BYTE bySigSize;  //闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔诲壖閻氬瓨瀚归柨鐔活敎閼哄倻顒查幏鐑芥晸閺傘倖瀚?
	BYTE bySigNum;   //闁跨喕鍓奸崣鐑芥交閹风兘鏁撻弬銈嗗??
	BYTE byIsSigned;   //闁跨喕鍓奸悮瀛樺?归柨鐔活潡閸戙倖瀚归柨鐔峰建閸戙倖瀚归柨鐔告灮閹凤拷  0-闁跨喐鏋婚幏锟? 1-闁跨喐鏋婚幏锟?
	BYTE bySampleCycle;	//闁跨喓鍗崇涵閿嬪?归柨鐔告灮閹风兘鏁撻弬銈嗗?? 0-濮ｅ繐婀?汩柨鐔告灮閹风兘鏁撻弬銈嗗??  1\2\3...-闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归弶澶愭晸閺傘倖瀚归柨鐔峰?欑粵瑙勫?归柨鐕傛嫹
};
typedef struct _stFrameInfo stFrameInfo;

struct _stKEY_DATA
{
	int iMenuNo;
	int iFocus;
	int iMAIN_Menu10JumpNo;			//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹凤拷
	int iEVINFO_Menu11JumpNo;		//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹烽攱浼?
	int iHELP_Menu15JumpNo;
	int iNOCARDCHARGE_Menu34JumpNo;		//闁跨喓娼鹃崠鈩冨?归柨鐔告灮閹风兘鏁撻敓锟?
	int iMAINTAIN_MenuJumpNo;
	int iICPswd_MenuJumpNo;
	int iCharge_Menu24JumpNO;
	int iRECTINFO_Menu29JumpNo;
	int iRESULT_Menu27JumpNo;
	int iSELECT_Menu22JumpNo;
	int iBMS_TYPE_Menu18JumpNo;
	int iTIME_TYPE_Menu19JumpNo;
	int iENERGY_TYPE_Menu20JumpNo;
	int iMONEY_TYPE_Menu21JumpNo;
	int iMenu13JumpNo;
	int iMaintenaceLogInMenu32_JumpNo;
	int iMaintenaceMenu25_JumpNo;
	int iChargeRecordMenu28_JumpNo;
	int iHisAlmMenu14_JumpNo;
};
typedef struct _stKEY_DATA stKey_Data;

struct __BMS_TYPE_KEY_VAL
{
	int iKey_Day;
	int iKey_Hour;
	int iKey_Min;
};
typedef struct  __BMS_TYPE_KEY_VAL  BMS_Type_Key_Val;

struct __TIME_TYPE_KEY_VAL
{
	float fKeyVolt;
	float fKeyCurr;
	float  fKeyTime;
	int iKey_Day;
	int iKey_Hour;
	int iKey_Min;
};
typedef struct  __TIME_TYPE_KEY_VAL  TIME_Type_Key_Val;

struct __ENERGY_TYPE_KEY_VAL
{
	float fKeyVolt;
	float fKeyCurr;
	float  fKeyEnergy;
	int iKey_Day;
	int iKey_Hour;
	int iKey_Min;
};
typedef struct  __ENERGY_TYPE_KEY_VAL  ENERGY_Type_Key_Val;

struct __MONEY_TYPE_KEY_VAL
{
	float fKeyVolt;
	float fKeyCurr;
	float  fKeyMoney;
	int iKey_Day;
	int iKey_Hour;
	int iKey_Min;
};
typedef struct  __MONEY_TYPE_KEY_VAL  MONEY_Type_Key_Val;



#define NET_S_NUM	2

//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻悿銉ョ暰闁跨喕妫勬稉宥夋晸閺夌増鏋婚幏鐑芥晸閸欘偊娼婚幏鐑芥晸娓氥儻缍囬幏鐑芥晸閺傘倖瀚规稉娲?鏁撴?楃儤鏋婚幏鐑芥晸閺傘倖瀚归柨鐔虹叓绾板?嬪?归弮鍫曟晸閺傘倖瀚瑰?掔兘鏁撻弬銈嗗?归柨鐔稿疆椤忓孩瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔虹波閺嬪嫰鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻懞銉?绱掗柨鐔告灮閹凤拷
#define NET_R_NUM	10
//#define NET_BUF_DATA_L	1800	//MIN 2 
#define NET_BUF_DATA_L	1500	//MIN 2 
struct NetBuf
{
	UINT8 u8DataFlag;			//闁跨喐鏋婚幏鐑芥晸鏉炲尅鎷?0 鐠囨挳鏁撻弬銈嗗?归柨鐔虹崵閹惧懏瀚归崣鏍?鏁撻弬銈嗗?归柨鐔峰?欓敐蹇斿?归柨鐔告灮閹风兘鏁撻弬銈嗗?归崢瀣?鏁撻弬銈嗗?归柨鐔告灮閹凤拷  闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔诲▏椤忓孩瀚归柨鐔告灮閹峰嘲绶?瀹勪胶娅㈤幏閿嬬珶椤戔晪鎷?0闁跨喐鏋婚幏鐤?顕╅柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔哄瀭UF 闁跨喐鏋婚幏鐑芥晸鏉炲尅鎷?1鐠囨挳鏁撻弬銈嗗?归柨鐔告灮閹风āUF闁跨喐鏋婚幏宄板窗闁跨喓鐓?閿濆繑瀚?
	UINT16 u16SendNoBak;
	UINT16 u16Lengh;
	UINT8 u8Buf[NET_BUF_DATA_L];////闁跨喐鏋婚幏鐑芥晸閺傘倖瀚瑰畡浣烘?㈤幏閿嬬珶椤掑﹥瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?圭拠鏃堟晸缂佺偟銆嬮幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔稿疆椤忓孩瀚归柨鐔活敎閵堝繑瀚归柨鐔告灮閹风兘鏁撻敓锟?8192闁跨喕顢滈弬銈嗗??
};
typedef struct NetBuf __NET_BUF;



struct NET_TCP_INFO
{
	UINT8	u8SendIdx;
	UINT8   u8DHCPResult;
	UINT32	u32BreakCount;
	UINT16  u16ReDHCPCount;
	UINT8	u8NetIsBreakFlag;		//闁跨喐鏋婚幏鐑芥晸闁版?跨厜閹风兘鏁撻弬銈嗗?归崡鎼佹晸閺傘倖瀚归懓宥夋晸閺傘倖瀚归柨鐔笺偤閵夋?婃緛閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻敓锟? 闁跨喐鏋婚幏锟? 闁跨喐鏋婚幏宄扮秿 闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风柉鐓块柨鐔告灮閹风兘鏁撻敓锟?
	UINT8	iNetCreateSocketFlag;

	//bit 0闁跨喐鏋婚幏鐑芥晸閿燂拷 闁跨喕顫楅崙銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?圭憰渚?沔晸閺傘倖??归柨鐔告灮閹凤拷		闁跨喐鏋婚幏宄扮安5闁跨喐鏋婚幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?圭憰渚?沔晸閺傘倖??归柨鐔告灮閹凤拷
	//bit 1闁跨喐鏋婚幏鐑芥晸閿燂拷 闁跨喕顫楅崙銈嗗?归柨鐔虹崵閹惧懏瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗??		闁跨喐鏋婚幏宄扮安7
	//BIT6 闁跨喓笑绾板?嬪?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔稿祹閿濆繑瀚?
	//bit 2闁跨喐鏋婚幏鐑芥晸閿燂拷 闁跨喕顫楅崙銈嗗?笵HCP闁跨喓鍗抽惂鍛婂?归柨鐔告灮閹凤拷
	INT32 iSendTimeOUT;
	UINT8 u8MachineStat;

	UINT16 u16SendNo;
	__NET_BUF u8ReadBuf[NET_R_NUM];	//闁跨喐鏋婚幏鐑芥晸閺傘倖瀚瑰畡浣烘?㈤幏閿嬬珶椤掑﹥瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?圭拠鏃堟晸缂佺偟銆嬮幏鐑芥晸閺傘倖瀚归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔告灮閹风兘鏁撻弬銈嗗?归柨鐔稿疆椤忓孩瀚归柨鐔活敎閵堝繑瀚归柨鐔告灮閹风兘鏁撻敓锟?8192闁跨喕顢滈弬銈嗗??
	__NET_BUF u8SendBuf[NET_S_NUM];
};
typedef struct NET_TCP_INFO __NET_TCP_INFO;


#define CHAR_TYPE   sizeof(gSetDataInfo.SetData.ArrayData.CfgCharData)
#define FLOAT_TYPE  (sizeof(gSetDataInfo.SetData.ArrayData.CfgFloatData) / 4)
#define GetSetData(d1, d2)      gSetDataInfo.fGetSetData(d1, d2)
#define SetSetData(d1, d2)      gSetDataInfo.fSetSetData(d1, d2)
#define GetCharSetData(d1, d2)  gSetDataInfo.fGetSetData(d1, d2)
#define SetCharSetData(d1, d2)  gSetDataInfo.fSetSetData(d1, d2)
#define GetFloatSetData(d1, d2) gSetDataInfo.fGetSetData(d1 + CHAR_TYPE, d2)
#define SetFloatSetData(d1, d2) gSetDataInfo.fSetSetData(d1 + CHAR_TYPE, d2)
#define SetFloatSetData1(d1, d2) gSetDataInfo.fSetSetData1(d1 + CHAR_TYPE, d2)
#define GetDefaultCharSetData(d1, d2)  gSetDataInfo.fGetDefaultSetData(d1, d2)
#define SetDefaultCharSetData(d1, d2)  gSetDataInfo.fSetDefaultSetData(d1, d2)
#define GetDefaultFloatSetData(d1, d2) gSetDataInfo.fGetDefaultSetData(d1 + CHAR_TYPE, d2)
#define SetDefaultFloatSetData(d1, d2) gSetDataInfo.fSetDefaultSetData(d1 + CHAR_TYPE, d2)

void tcp_client_reconnect(void);
void tcp_client_connected(void);
void ask_dns_server_get_ip(void);
void tcp_client_reconnect_domainname(void);
//extern functions
void TASK_E2promProc(void* pdata);
void TASK_Uart6Proc(void* pdata);
void TASK_MeasureProc (void* pdata);
void TASK_AlmProc (void* pdata);
void TASK_CAN2Proc (void* pdata);
void TACK_Uart1Proc(void* pdata);
void TASK_SelfTestProc (void* pdata);
void TASK_RTCProc (void* pdata);
void TASK_CAN3_Proc (void* pdata);
void Task_USB_Proc (void* pdata);
void TASK_DisplayProc (void* pdata);
void TASK_Uart4Proc (void* pdata);
void TASK_Uart5Proc (void* pdata);
void TASK_EthernetProc(void* pdata);
void TAST_FanProc(void* pdata);
void TASK_CAN1Proc(void* pdata);
void TASK_FlashProc (void* pdata);
void TASK_Ethernet(void* pdata);


void InitEVInfo(void);
void EepromRecord_Init(void);

void CAN_1_GBT_SendCallBack(void);
void CAN_2_GBT_SendCallBack(void);
void CAN_2_SendCallBack(void);
void UART1_RecvCheck_CallBack(void);
void Can2_CHAdeMOBackCal(void);
void Can2_CCSBackCal(void);

//UINT8 SendCSD_A(void);
//UINT8 SendCST_A(void);
//UINT8 SendQuick_A(void);
//UINT8 SendCHM_A(void);
//UINT8 SendCRM_00_A(void);
//UINT8 SendCRM_AA_A(void);
//UINT8 SendCML_A(void);
//UINT8 SendCTS_A(void);
//UINT8 SendCRO_00_A(void);
//UINT8 SendCRO_AA_A(void);
//UINT8 SendCCS_A(void);
//UINT8 SendCEM_A(void);
void CAN_1_SendCallBack(void);

void TASK_NET_ZhixinProc(void* pdata);

void FaultLogIdxRefresh(void);

void InitIMIU_Uart5_E2prom(void);
void InitIMIU_Uart4_E2prom(void);

void InitWH_E2prom(void);

void API_CreateRecordLog(void);
/*void SetChargeSTART_CMD(UINT8 u8StartReason);*/
//void SetChargeSTOP_Cmd(UINT8 u8StopReason);
void GetChargeLogPstByFixIdx(UINT16 u16FixIdx, UINT16* u16Offset, UINT16* u16SectPst);
UINT8 GetUnRptPageIdxByE2prom(UINT16* u16Offset, UINT16* u16SectPst);
void ClrUnRptPageIdxAtE2prom(UINT16 u16Offset, UINT16 u16SectPst);
void Save_E2pRomValue(__XXX_UNION_VALUE Value,UINT8 u8IsByte,UINT32 E2PROM_ADDR);
//void Test_API(void);
void HMIKeyBack(void);
void HMIKeyClean(void);

void ClrFaultLOG(void);

//UINT8 u8CheckIMIU_1Alm(void);
//UINT8 u8CommIMIU_1Alm(void);
//UINT8 u8CheckIMIU_2Alm(void);
//UINT8 u8CommIMIU_2Alm(void);
void NET_Clean_SBuf(void);
void NET_Clean_RBuf(void);
UINT32 NET_Read(void *input_buf);
UINT8 NET_Send(UINT8* pBuf, UINT32 u32Len, UINT32 TimeOut10ms);

UINT8 TEST_Mode_485_Proc(UINT8 uartNo);
UINT8 TEST_Mode_DO_CAN_Proc(UINT8 canNO);
UINT8 u8RectIsAllClose(void);
UINT8 TEST_Mode_Uart_Proc(UINT8 uUartNo);
void Uart2TestMode(UINT8 u8IsTestMode);
void TxCAN1_TESTData(void);
void Parallel_A_StartDeal(void);
UINT8 Parallel_A_ChargeDeal(float fRCurr);

void Can2_InitBms_2Data(void);
UINT8 u8Can2_Send15ECC001(void);
UINT8 u8Can2_Send15ECC002(void);
UINT8 u8Can2_Send15ECC003(void);
UINT8 u8Can2_Send15ECC004(void);
UINT8 u8Can2_Send15ECC005(void);
UINT8 u8Can2_Send15ECC006(void);
UINT8 u8Can2_Send15ECC007(void);
UINT8 u8Can2_Send15ECC008(void);
UINT8 u8Can2_Send15ECC009(void);
UINT8 u8Can2_Send15ECC00A(void);
UINT8 u8Can2_Send15ECC00B(void);
UINT8 u8Can2_Send15ECC00C(void);
UINT8 u8Can2_Send15ECC00D(void);
UINT8 u8Can2_Send15ECC00E(void);

void PAGE_76_KeyBord_NETDeal(void);
UINT8 u8IsDoorOpen(void);
UINT8 u8IsEPOActive(void);

void B_Spear_RectOnOffCtrl(UINT8 u8IsOn);
void Clen_B_Alm(UINT16 u16Delay,UINT8 u8IsForce);
void B_MCCBCtrl(UINT8 u8IsConnect);
void Parallel_B_StartDeal(void);
void NotificationIMIU_2Check(void);
void NotificationIMIU_2Stop(void);
//UINT8 u8CheckIMIU_2Alm(void);
void B_AdjstVoltCurr(float Volt,float Curr);
UINT8 B_Charge_Out_Deal(UINT8 u8FirstRun, UINT8 PolarityAlmMask, UINT8 MccbCheck,UINT8 u8IsMinOut);
UINT8 B_FinishConditionDeal(void);
void CAN2_CHAdeMoDeal(void);
//UINT8 u8B_SpearLOCKDeal(UINT8 u8IsLock);
void Charge_2Ring(void);
void Charge_2ByBMS(void);
void CAN2_CCSDeal(void);
void CAN2_CCS_V30_Deal(void);
UINT8 u8_B_AlmDela(void);

void Clen_A_Alm(UINT16 u16Delay,UINT8 u8IsForce);
void Charge_1Ring(void);
void Charge_1ByManual(void);
void Charge_1ByBMS(void);
void A_AdjstVoltCurr(float Volt,float Curr);
UINT8 A_Charge_Out_Deal(UINT8 u8FirstRun, UINT8 PolarityAlmMask, UINT8 MccbCheck,UINT8 u8IsMinOut);
UINT8 A_FinishConditionDeal(void);
void A_MCCBCtrl(UINT8 u8IsConnect);
void A_Spear_RectOnOffCtrl(UINT8 u8IsOn);
//UINT8 u8IsSpear1Connect(void);
UINT8 u8_A_AlmDela(void);




void CAN1_CCSDeal(void);
void CAN1_GBT_Deal(void);
void CAN2_GBT_Deal(void);
void NotificationIMIU_1Check(void);
void NotificationIMIU_1Stop(void);
//UINT8 u8CheckIMIU_1Alm(void);

extern float gADCC2Val;
extern float gADCC1Val;
extern float gADTestFVal1;
extern float gADVoltFVal1;
extern float gADTestFVal2;
extern float gTestFVal3;

extern UINT32 u32CAN_1_PerioCount;
extern UINT8 u8FaultMode;
extern UINT32 spnNo;
extern INT32 gIFlag;
extern UINT8 gCSTbuf[4];
extern __XXX_UNION_VALUE BMS1unVal;
extern __XXX_UNION_VALUE BMS2unVal;
extern UINT8 HisNum;
extern UINT8 FaultNum;
//extern NOINIT stBMS_INFO	gEVBms_1_Info;
//extern NOINIT stBMSCommCtrlInfo gBms_1_CommInfo;
extern NOINIT UINT8 EVXXX_A_buf[EV_XXX_BUF_NUM + 10];
extern MACHINE_INFO BMS_1MachineIfo[MACHINE_ID_MAX_NUM + 20];
extern MACHINE_INFO BMS_2MachineIfo[MACHINE_ID_MAX_NUM + 20];
extern NOINIT UINT8 gUart4TXBuff[RS485_UART4_TX_MAX_NUM];

extern NOINIT MODBUS_RECV_INFO gMdbusRcvInfo[MDBUS_RCV_ARRY_NUM+4];
extern UINT8 gUart6ArryIdx;

extern NOINIT __NET_BUF		gDriveBuf;

extern NOINIT UINT8			gu8EVPSWD_MD5_32[64];
extern CAN_HandleTypeDef	Can_2_Handle;
extern CAN_HandleTypeDef	Can_1_Handle;
//extern int gMachineState;
extern NOINIT UINT16 Uart6_YDN23_RecvBuff[UART6_YDN23_RECV_MAX_NUM];
//extern NOINIT unsigned short Uart6RecvBuff[UART6_RECV_MAX_NUM];
extern NOINIT UINT8 gUART1SendData[UART1_SEND_MAX_NUM];
extern NOINIT UINT8 gUART1RecvData[UART1_RECV_MAX_NUM];
extern UINT8 gUart4ArryIdx;
extern UINT8 gUart4TimeOutFlag;
extern UINT16 gUart4TimeOutCout;
extern NOINIT CanRxMsgTypeDef CanRxMsgBuff_B[CAN_A_B_BUF_MAX_NUM+1];
extern NOINIT CanRxMsgTypeDef CanRxMsgBuff_A[CAN_A_B_BUF_MAX_NUM+1];
extern NOINIT CanRxMsgTypeDef CanRxMsgBuff_TEST_A[CAN_A_B_TEST_BUF_MAX_NUM];
extern NOINIT CanRxMsgTypeDef CanRxMsgBuff_TEST_B[CAN_A_B_TEST_BUF_MAX_NUM];
extern uint32_t pRecvCAN2Idx;
extern uint32_t pRecvTEST_CAN2Idx;
extern uint32_t pRecvCAN1Idx;
extern uint32_t pRecvTEST_CAN1Idx;
extern uint32_t pProcCAN2Buf;
extern uint32_t pProcCAN1Buf;
extern uint32_t gTestEXID;
extern NOINIT canmsgTXY2 gCan2RxMsgBuf[CAN_A_B_BUF_MAX_NUM+3];
extern NOINIT canmsgTXY2 gCan1RxMsgBuf[CAN_A_B_BUF_MAX_NUM+3];
extern NOINIT UINT8 gUart5RecvBuff[RS485_UART5_RX_MAX_NUM];
extern NOINIT UINT8 gUart4RecvBuff[RS485_UART4_RX_MAX_NUM];
extern NOINIT UINT16 gUart2RecvBuf[UART2_RECV_MAX_NUM];

extern SetDataExchangeInterface_TypeDef gSetDataInfo;
extern KeyInterface_TypeDef gs_Key;
extern RTCData_Type gRTCData;
extern CANCommData_TypeDef gs_CANComm;
extern CANTxData_TypeDef TxData[];
extern MeasureData_TypeDef gMeasureInfo;
extern SeltTest_TypeDef gSelfTest;
extern E2PROM_TypeDef  gs_E2PROM;
extern FanMng_TypeDef gFANMngInfo;
//extern NOINIT ScreenCfg_TypeDef         Screen[MAX_SCREEN_NUM];
//extern NOINIT LCDSource_TypeDef         LCDSource;
extern dv_LCD_TypeDef dv_LCD;
extern dv_RTC_TypeDef dv_RTC;
extern dv_Output_TypeDef dv_Output;
extern dv_Uart1_TypeDef dv_Uart1;
extern dv_Uart2_TypeDef dv_Uart2;
extern dv_Uart6_TypeDef  dv_Uart6;
extern dv_Uart4andUart5_TypeDef dv_Uart4;
extern dv_Uart4andUart5_TypeDef dv_Uart5;
extern dv_E2PROM_TypeDef dv_E2PROM;
extern dv_Adc_TypeDef dv_Adc;
extern dv_Adc_TypeDef dv_ADC_3;
extern dv_CAN_TypeDef dv_CAN1;
extern dv_Dog_TypeDef  dv_Dog;
extern dv_Capture_PWM_TypeDef  dv_Capture_PWM;
extern dv_I2C3_TypeDef dv_I2C3;
extern TASK_XXXInfo sTaskArray[TASK_MAX_NUM];
extern const UCHAR MeasureAddress[];
extern const __SIG_E2P_ADDR stArgsSigAddr[];

extern NOINIT __CHARGE_RECORD stChargeRecordFlash;
extern NOINIT __CHARGE_RECORD  stViewChargeRecordFlash;
extern NOINIT __ST_ZTE_FAULT_LOG_RAM gZTEFaultLog[FAULT_LOG_RAM_NUM];
extern NOINIT __ZTE_FAULT_LOG_IDX gZTEFaultLogIdx;
extern NOINIT __ZTE_FAULT_LOG gZTEFaultLogView[FAULT_LOG_VIEW_NUM + 2];
extern NOINIT __CHARGE_RECORD gEVChargeRecordFlashView;
extern __FLASH_CTRL_INFO gFlashOptInfo;
extern int ACFILTER_count;
extern UINT8 ACFILTER_IDX;
extern float AC_DMAValue[mFIELT_NUM];

extern NOINIT __CHARGE_RECORD gView_RecordFlash,gUnRPT_RecordFlash;
//TXY
extern float gTESTCAN2Val;

extern float fAC_PWM_VOLTAGE_QuickVal;
extern float fgCC1QuickVal;
extern float fgCC1QuickVal_2;
extern float fgCC1QuickVal_AC;
extern NOINIT ALM_DEAL gAlmDeal;
extern NOINIT __NET_TCP_INFO gNET_TCP_Info;
extern NOINIT stKey_Data gKey_Data;
extern dv_ethdev_TypeDef dv_ethdev;
extern EEMExchangeInterface_TypeDef gs_eem;
extern SNMPTRAPExchangeInterface_TypeDef   gs_trap;
extern UINT8 gCSTbuf[4];
extern UCHAR gOutPutSwitchState;
extern UCHAR gAllRectCtrlState ; //妫板嫰鏁撻弬銈嗗?归柨鐔稿复閸栤剝瀚?
extern UCHAR gFanFullCtrlState ;
extern UCHAR gContactorCtrlState;

//extern NOINIT stBMS_INFO		gEVBms_1_Info;
//extern NOINIT stBMS_INFO		gEVBms_2_Info;


extern NOINIT EV_CHARGE_INFO	gEvChargeInfo;
extern NOINIT BRUSH_INFO		gBrushInfo;

#define WIFI_R_BYTE_TIME_OUT	4
#define WIFI_R_BYTE_EMPTY	0
#define WIFI_R_BYTE_START_COUNT	1

//extern NOINIT WIFIDATA gWifiData;
extern NOINIT UINT16		gWifiReceiveFlag;
extern NOINIT UINT16		gWifiReceive_2_Flag;

extern __CHARGE_ARGS gTempChargeArgs;
extern __VIEW_RESULT_INFO gA_Result;
extern __VIEW_RESULT_INFO gB_Result;

extern NOINIT __CHARGE_RECORD SendRecord_1;
extern NOINIT __CHARGE_RECORD SendRecord_2;
extern NOINIT ChargeLogTable gChargeLogTable;
//extern NOINIT CHAdeMO  gCHAdeMo;
extern UINT8 g_u8Can3TxProtectDelay;
extern UINT32 u32g_A_OverVoltDelayTimes;
extern UINT32 u32g_B_OverVoltDelayTimes;
UINT8 u8_A_GUN_Is_OverVolt(void);
UINT8 u8_B_GUN_Is_OverVolt(void);
//float f_B_Bus_VoltageDeal(void);
//float f_A_Bus_VoltageDeal(void);

extern void FlashWrite_U2_Data(UINT8* pData, UINT32 u32Addr, UINT32 len);
extern void FlashWrite_U1_Data(UINT8* pData, UINT32 u32Addr, UINT32 len);
extern void FlashWrite_IMEU2_Data(UINT8* pData, UINT32 u32Addr, UINT32 len);
extern void FlashWrite_IMEU2_DataLen(UINT8* pData, UINT32 u32Addr, UINT32 len);
extern UINT8  FlashWaitClean_U2_Data(void);
extern UINT8  FlashWaitClean_U1_Data(void);
extern void FlashClean_U1_Data(void);
extern void FlashClean_U2_Data(void);
extern void FlashClean_IMEU2_Data(void);
extern dv_USB_TypeDef dv_USB;
extern void BSP_USBH_Process(void);
extern  void vUsbInit(void);
extern void RS485_UART4_SendDlay(UINT32 uDelayVal);
extern void RS485_UART5_SendDlay(UINT32 uDelayVal);
#endif // __main_H
// End of file


