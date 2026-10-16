#ifndef ETHERNET_H
#define ETHERNET_H


#define CHK_SEND_STAT(u8Stat) (u8Stat&(1<<0))
#define SET_SEND_STAT(u8Stat) (u8Stat|=(1<<0))
#define CLR_SEND_STAT(u8Stat) (u8Stat&=~(1<<0))

#define CHK_CONN_STAT(u8Stat) (u8Stat&(1<<1))
#define SET_CONN_STAT(u8Stat) (u8Stat|=1<<1)
#define CLR_CONN_STAT(u8Stat) (u8Stat&=~(1<<1))

#define CHK_DHCP_STAT(u8Stat) (u8Stat&(1<<2))
#define SET_DHCP_STAT(u8Stat) (u8Stat|=1<<2)
#define CLR_DHCP_STAT(u8Stat) (u8Stat&=~(1<<2))

#define NEED_CREAD_SOCKET 1
#define NEED_NOT_CREAD_SOCKET 0
#define BUF_NO_DATA	-1
#define FIND_SPECIFIED_CMD 0
#define BUF_HAS_DADA	1

#define SCMD_LastCHARGE_INFO_ACK 201
#define SCMD_HEARDBEAT_ACK	101
#define SCMD_STAT_RSP_ACK	103			//(CMD=103)服务器应答充电桩状态信息包
#define SCMD_REGIST_ACK		105
#define SCMD_SET_INT_PRG_1	1			//(CMD=1)后台服务器下发充电桩整形工作参数
#define SCMD_SET_CHAR_PRG_3	3			//(CMD=3)后台服务器下发充电桩字符型工作参数
#define SCMD_CTRL_5		5			//(CMD=5)后台服务器下发充电桩控制命令
#define SCMD_CTRL_CHARGE_7	7			//(CMD=7)后台服务器下发充电桩开启充电控制命令
#define SCMD_UPGRADE_1101		1101		//CMD=1101)后台服务器向充电桩下发升级指令
#define SCMD_SET_FARE_1301		1301		//CMD=1301）后台下发固定电费计价策略信息
#define SCMDPeakFlatValleyFARE_1303	1303		//（CMD=1303）后台下发峰平谷电费计价策略信息
#define SCMD_FARE_INFO_1305		1305		//（CMD=1305）后台下发当前充电用户充电服务费信息
#define SCMD_BMS_INFO_ACK	301			//(CMD=301)服务器应答充电桩上报BMS信息
#define SCMD_CMD_204		204			//发送命令 204
#define CMD_EV_INFO_ACK		1103

#define CMD_SET_CHAR_PRG_ACK	4		//(CMD=4)充电桩参数字符形设置/查询应答
#define CMD_CTRL_ACK		6		//(CMD=6)充电桩对后台控制命令应答
#define CMD_CTRL2_CHARGE_ACK	8		//(CMD=8) 充电桩对后台下发的充电桩开启充电控制应答
#define CMD_UPGRADE_ACK		1102		//(CMD=1102)充电桩对服务器下发升级指令的应答
#define CMD_SET_FARE_ACK	1302		//（CMD=1302）充电桩回应后台下发的固定电费计价测率信息
#define CMDPeakFlatValleyFARE_ACK	1304	//（CMD=1304）充电桩响应后台下发峰平谷电费计价策略信息
#define CMD_FARE_INFO_ACK		1306	//（CMD=1306）充电桩响应后台下发当前充电用户充电服务费信息
#define CMD_HEARDBEAT		102		//(CMD=102)充电桩上传心跳包信息
#define CMD_REGIST		106		//(CMD=106)充电桩签到信息上报
#define CMD_STAT_RSP		104		//(CMD=104)充电桩状态信息包上报
#define CMD_LastCHARGE_INFO	202		//(CMD=202)充电桩上报最新一次充电信息
#define CMD_BMS_INFO		302		//(CMD=302)充电桩上报BMS信息
#define CMD_EV_INFO		1104		//(CMD=1104） 充电桩主动上报桩的程序、中间件、广告文件的版本信息
#define CMD_CMD_204_ACK		203		

#define SCMD_REPORT_401		401
#define CMD_REPORT_402		402		//充电桩上报历史的充电记录

#define RSP_MODE_IS_AUTO	2		//自动应答
#define RSP_MODE_IS_ACK		1

#define Y_SOI			gs_EthAppCommMng.u16NetFrameHead
#define Y_VERSION		0x10

#define Y_SOI_PST		0
#define Y_LENGH_PST		2
#define Y_VERSION_PST		4
#define Y_SEQ_NO_PST		5
#define Y_CMD_PST		6
#define Y_DATA_PST		8


//后台服务器下发充电桩字符型工作参数	具体信号！！！
#define CMD3_PRG_EV_SerialNum	1
#define CMD3_PRG_Tick		2
#define CMD3_PRG_AdminPSWD1	3
#define CMD3_PRG_AdminPSWD2	4	//操作员密码
#define CMD3_PRG_MACReserve	5
#define CMD3_PRG_BoxNumber	6	//箱变编码
#define CMD3_PRG_2BarCode	7	//二维码
#define CMD3_PRG_RESERVE1	8
#define CMD3_PRG_RESERVE2	9
#define CMD3_PRG_Cost2BarCode	10

#define CMD5_SIG_MAX_NUM	16

#define Data_HI(w)  ((UINT8)((UINT16)(w) >> 8))
#define Data_LO(w)  ((UINT8)((UINT16)(w) &0xff))

struct strCMD5_SigList
{
	UINT8 u8CMDAddr;			//区分出来是哪个信号
	UINT8 u8ByteNum;			//该信号占几个字节
};
typedef struct strCMD5_SigList  CMD5_SIG_LIST;



struct _EthAppCommMng
{
	INT16 i16CommFailTimes;			//通信失败计数值

	UINT8  ucSetCodeByHost[2][128];
	UINT8  ucSetIdentify[2][16];


	UINT16 u16EndInfoCount[2];			//结算信息
	INT32 i32RegistCount;				//未收到校时指令后，每隔15S，登录一下。
	UINT16 u16HeardBeatCount;			//心跳周期
	UINT16 u16HeartBeatInterval;
	UINT8 u8CMD104Result;
	INT32 i32StatCMD104Count[2];
	UINT32 i32StatCMD104Interval;
	INT32 i32BMSInfoCMD302Count[2];
	UINT16 u16302SeqNo[2];
	UINT8 u8CMD104Flag[2];
	UINT8 u8CMD202Flag[2];
	UINT8 ucCMD202Count[2];
	UINT8 ucCMD402Count;
	UINT16 u16NetFrameHead;
	//刷卡或扫码开启电桩（解锁充电枪）时立即上报一次（协议第7字段值为1）
	//控制104报文 充电机状态变化上传
	//UINT8 u8NET_StartChargeFlag104;			//手机APP充电
	UINT8 u8Brush_StartChargeFlag104;		//刷卡开始充电
	INT32 i32OrderTimeOUTCount[2];
	INT16 i16OrderTimeOUT[2];				//预约超时时间	用于回传 不用于记数
	UINT8 u8StopMother202;				//刷卡结束条件
	UINT8 u8Before_CardMoney[4];
	UINT8 u8After_CardMoney[4];
	UINT8 u8START_Charge_FAILS_104;			//开始充电启动失败
	//UINT8 u8SpearStatChange104;			//枪状态发生改变
	//UINT8 u8EVSelfFailt104;				//EV故障 无法充电了！
	UINT8 u8WorkStat[2];
	UCHAR  ucEndStateUpdataCount[2];
	UINT32 u32YUNSHANAlm[2];
	INT8  I8CommModeStartFlag;		//1需要 开始充电  2 已经 点击过开始充电 
	UINT8 u8OrderChargeCMD[2];			//0-无预约（无效）	1-预约有效
	UINT8 u8OrderSetTime[2][8];		//预约开始时间
	UINT8 u8OrderTimeOut[2];
	UINT8 u8OffLineFlag;			//0-不允许 1-允许
	UINT8 u8StartChargeTime[8];		//开始充电时间
	UINT8 u8EndChargeTime[8];		//结束充电时间
	UINT32 u32Reserve;		
	float fOffLineAllowPower;

	UINT16 u16RegistInterval;		//签到间隔 单位分钟
	UINT8  u8HeartBeatInterval;		//心跳上报周期
	UINT8  u8HeartBeatTimeOutTimes;		//心跳超时次数
	
	struct tm  LastChargeStartTime;
	struct tm  LastChargeENDTime;
	struct tm  PowerOnTime;			//最后一次启动时间

	//UINT8 u8ServerIsExist;			//SOCKET 连接结果
	UINT8 u8RegistResult;			//登录结果
	UINT8 u8TickFlag;				//校时标识	TRUE 收到校时	
	UINT8 u8RcvFlagOfCodeByHost[2];	//屏显示二维码 标识	TRUE 收到
	UINT8 u8RcvFlagOfIdentifyID[2];	//屏显示识别码 标识	TRUE 收到

	
	INT32 i32EV_FLASH_RecordDelay;


	UINT8 u8APPInfo_A[32];    //预定号/APP账号
	UINT8 u8APPInfo_B[32];    //预定号/APP账号

	double fServerCARDLeaveMoney;			//服务器记录 剩余金额

	UINT16 ui16DHCPReconfigTimeOUT;
};
typedef struct _EthAppCommMng EthAppCommMng;
extern EthAppCommMng gs_EthAppCommMng;


#define CMD202			0
#define CMD402			1

#endif

