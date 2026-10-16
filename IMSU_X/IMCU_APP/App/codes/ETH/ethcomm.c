/*
 *****************************************************************************
 *                                                                           *
 *  Filename:   Ethernet.c                                                   *
 *  Date:       May 1, 2007                                                  *
 *  Description:net comm                                                     *
 *  Version:    V1.00                                                        *
 *  Hardware:                                                                *
 *  Remarks:    For NGC application                                          *
 *                                                                           *
 *****************************************************************************
 */

#include "httpd.h"
#include "httpd-fs.h"
//#include "httpd-cgi.h"
#include "http-strings.h"
#include "main.h"
#include "ethdev.h"
#include "uip_arp.h"

//#include "interface.h"
#include "uip_arp.h"
#include "uip.h"
#include "timer.h"
#include "resolv.h"
#include "EtherNet.h"
//#include "MQTTClient.h"
#include "Upgrade.h"

uip_ipaddr_t gs_RemoteServerIp;
unsigned char g_ucRemoteServerIpIsOk = 0;
unsigned long  Trap_TrapNum=0;

NOINIT __NET_BUF gDriveBuf;
struct timer periodic_timer, arp_timer;
#define	PERIODIC_TIMER_UPGRADE	100		//*2ms   20210709 固件升级期间加快回报速度

UCHAR mac[10];
#define   RESET_CHIP_NUM1  35000
#define   RESET_CHIP_NUM2    1000 
//15000次没有接收到数据，就复位，以此做计数器
 long reset_netchip = 0;

//1000次有接收数据却没有发送数据，就复位，以此做计数器
long reset_netchip_send = 0; 

UCHAR ucARPOnce = 0;

UCHAR g_ucDHCPAddressPrepared = mDHCP_NEED_INIT;

SNMPTRAPExchangeInterface_TypeDef   gs_trap;

uip_ipaddr_t eem_ipaddr[2];
void ReInitNetPort(void);
void Init_udp_exchange_app(void);

//读取ip，mask，gateway
static void ethInit(void)
{
	UCHAR ip[4];
	int p;
	uip_ipaddr_t ipaddr[2];
        //__XXX_UNION_VALUE unVal;
	struct uip_udp_conn *udp_conn_ret;

	//following init the parameter of other part of the network
	memset(&gs_trap,0,sizeof(gs_trap));	

	for(p=0;p<4;p++)//get the  ip
	{
		//GetCharSetData(SETDATA_IP_NUM+p,&ip[p]);
           // ExGetSigInfo(E_TYPE_SYS, ID_u8IP_1 + p, &unVal, NULL, BY_SRAM);
			//ip[p] = unVal.chtemp[0]; 
			ip[p] = gEvChargeInfo.u8IP_1[p];
           
	}
	uip_ipaddr(ipaddr, ip[0],ip[1],ip[2],ip[3]);
	uip_sethostaddr(ipaddr);//set the ip

	//NET_PRINTF(" Init Net IP=%d %d %d %d ",ip[0],ip[1],ip[2],ip[3]);

	for(p=0;p<4;p++)//get the  netmask
	{
		//GetCharSetData(SETDATA_Subnetmask_NUM+p,&ip[p]);
           // ExGetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_1 + p, &unVal, NULL, BY_SRAM);
           // ip[p] = unVal.chtemp[0]; 
		   ip[p] = gEvChargeInfo.u8Subnetmask[p];
                
	}
	uip_ipaddr(ipaddr, ip[0],ip[1],ip[2],ip[3]);
	uip_setnetmask(ipaddr);//set the netmasik
	//NET_PRINTF(" Init Net Mask=%d %d %d %d \n",ip[0],ip[1],ip[2],ip[3]);
	for(p=0;p<4;p++)//get the  gateway
	{
		//GetCharSetData(SETDATA_Gateway_NUM+p,&ip[p]);
                //ExGetSigInfo(E_TYPE_SYS, ID_u8Gateway_1 + p, &ip[p]);
              //  ExGetSigInfo(E_TYPE_SYS, ID_u8Gateway_1 + p, &unVal, NULL, BY_SRAM);
              //   ip[p] = unVal.chtemp[0]; 
		ip[p] = gEvChargeInfo.u8Gateway[p];
	}
	uip_ipaddr(ipaddr, ip[0],ip[1],ip[2],ip[3]);
	uip_setdraddr(ipaddr);
	//NET_PRINTF(" Init Net Gateway=%d %d %d %d \n",ip[0],ip[1],ip[2],ip[3]);
	for(p=0;p<4;p++)//get the  eem_ip1
	{
                // ExGetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_1 + p, &unVal, NULL, BY_SRAM);
                // ip[p] = unVal.chtemp[0]; 
		ip[p] = gEvChargeInfo.u8PowerStarIP[p];
				 
	}
	uip_ipaddr(eem_ipaddr[0], ip[0],ip[1],ip[2],ip[3]);
	//NET_PRINTF(" Init Net Powerstar1=%d %d %d %d \n",ip[0],ip[1],ip[2],ip[3]);

	for(p=0;p<4;p++)//get the  eem_ip1
	{
		//ExGetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_1 + p, &unVal, NULL, BY_SRAM);
		//ip[p] = unVal.chtemp[0]; 
		ip[p] = 0;
	}
	uip_ipaddr(eem_ipaddr[1], ip[0],ip[1],ip[2],ip[3]);
	//NET_PRINTF(" Init Net Powerstar2=%d %d %d %d \n",ip[0],ip[1],ip[2],ip[3]);

	for(p=0;p<4;p++)//get the trap ip
	{
		//ExGetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_1 + p, &unVal, NULL, BY_SRAM);
		//ip[p] = unVal.chtemp[0];
		ip[p] = 0;
	}
	gs_trap.uip_trap[0][0]=(ip[1])*256+ip[0];
	gs_trap.uip_trap[0][1]=ip[3]*256+ip[2];

	for(p=0;p<4;p++)//get the trap ip
	{
		//ExGetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_1 + p, &unVal, NULL, BY_SRAM);
		//ip[p] = unVal.chtemp[0]; 
		ip[p] = 0;
	}
	gs_trap.uip_trap[1][0]=(ip[1])*256+ip[0];
	gs_trap.uip_trap[1][1]=ip[3]*256+ip[2];

	uip_ipaddr(ipaddr,255,255,255,255);
	udp_conn_ret = uip_udp_new(ipaddr,HTONS(0));
	if(udp_conn_ret != NULL)
	{
		uip_udp_bind(udp_conn_ret,HTONS(161));
	}
}

//static void DHCPethInit(void);
//static void DHCPethInit(void)
//{
//	UCHAR ip[4];
//	int p;
//	uip_ipaddr_t ipaddr[2];
//
//	struct uip_udp_conn *udp_conn_ret;
//
//	//following init the parameter of other part of the network
//	memset(&gs_trap,0,sizeof(gs_trap));	
//
//	for(p=0;p<4;p++)//get the  ip
//	{
//		GetCharSetData(SETDATA_IP_NUM+p,&ip[p]);
//	}
//	uip_ipaddr(ipaddr, gEvChargeInfo.stDHCPNetInfo.u8IP[0],gEvChargeInfo.stDHCPNetInfo.u8IP[1],gEvChargeInfo.stDHCPNetInfo.u8IP[2],gEvChargeInfo.stDHCPNetInfo.u8IP[3]);
//	uip_sethostaddr(ipaddr);//set the ip
//
//	//NET_PRINTF(" Init Net IP=%d %d %d %d ",ip[0],ip[1],ip[2],ip[3]);
//
//	for(p=0;p<4;p++)//get the  netmask
//	{
//		GetCharSetData(SETDATA_Subnetmask_NUM+p,&ip[p]);
//	}
//	uip_ipaddr(ipaddr, ip[0],ip[1],ip[2],ip[3]);
//	uip_setnetmask(ipaddr);//set the netmasik
//	//NET_PRINTF(" Init Net Mask=%d %d %d %d \n",ip[0],ip[1],ip[2],ip[3]);
//	for(p=0;p<4;p++)//get the  gateway
//	{
//		GetCharSetData(SETDATA_Gateway_NUM+p,&ip[p]);
//	}
//	uip_ipaddr(ipaddr, ip[0],ip[1],ip[2],ip[3]);
//	uip_setdraddr(ipaddr);
//	//NET_PRINTF(" Init Net Gateway=%d %d %d %d \n",ip[0],ip[1],ip[2],ip[3]);
//	for(p=0;p<4;p++)//get the  eem_ip1
//	{
//		GetCharSetData(SETDATA_PowerStarIP_NUM+p,&ip[p]);
//	}
//	uip_ipaddr(eem_ipaddr[0], ip[0],ip[1],ip[2],ip[3]);
//	//NET_PRINTF(" Init Net Powerstar1=%d %d %d %d \n",ip[0],ip[1],ip[2],ip[3]);
//
//	for(p=0;p<4;p++)//get the  eem_ip1
//	{
//		GetCharSetData(SETDATA_PowerStarIP_NUM+6+p,&ip[p]);
//	}
//	uip_ipaddr(eem_ipaddr[1], ip[0],ip[1],ip[2],ip[3]);
//	//NET_PRINTF(" Init Net Powerstar2=%d %d %d %d \n",ip[0],ip[1],ip[2],ip[3]);
//
//	for(p=0;p<4;p++)//get the trap ip
//	{
//		GetCharSetData(SETDATA_NMSIP_NUM+p,&ip[p]);
//	}
//	gs_trap.uip_trap[0][0]=(ip[1])*256+ip[0];
//	gs_trap.uip_trap[0][1]=ip[3]*256+ip[2];
//
//	for(p=0;p<4;p++)//get the trap ip
//	{
//		GetCharSetData(SETDATA_NMSIP_NUM+6+p,&ip[p]);
//	}
//	gs_trap.uip_trap[1][0]=(ip[1])*256+ip[0];
//	gs_trap.uip_trap[1][1]=ip[3]*256+ip[2];
//
//	uip_ipaddr(ipaddr,255,255,255,255);
//	udp_conn_ret = uip_udp_new(ipaddr,HTONS(0));
//	if(udp_conn_ret != NULL)
//	{
//		uip_udp_bind(udp_conn_ret,HTONS(161));
//	}
//}
//

void  DHCPset_ip(void );
void  DHCPset_ip(void )
{
	//int i;
	uip_ipaddr_t ipaddr[2];

	//UCHAR ip1[4];

	// set the ip
	uip_ipaddr(ipaddr, gEvChargeInfo.stDHCPNetInfo.u8IP[0],gEvChargeInfo.stDHCPNetInfo.u8IP[1],gEvChargeInfo.stDHCPNetInfo.u8IP[2],gEvChargeInfo.stDHCPNetInfo.u8IP[3]);
	uip_sethostaddr(ipaddr);

	//set the mask
	uip_ipaddr(ipaddr, gEvChargeInfo.stDHCPNetInfo.u8Mask[0],gEvChargeInfo.stDHCPNetInfo.u8Mask[1],gEvChargeInfo.stDHCPNetInfo.u8Mask[2],gEvChargeInfo.stDHCPNetInfo.u8Mask[3]);
	uip_setnetmask(ipaddr);

	//set the draddr
	uip_ipaddr(ipaddr, gEvChargeInfo.stDHCPNetInfo.u8Gateway[0],gEvChargeInfo.stDHCPNetInfo.u8Gateway[1],gEvChargeInfo.stDHCPNetInfo.u8Gateway[2],gEvChargeInfo.stDHCPNetInfo.u8Gateway[3]);
	uip_setdraddr(ipaddr);

}

static UINT8 u8IsLowLevelBreak(void)
{
	if(gNET_TCP_Info.u32BreakCount >= 360)
	{
		gNET_TCP_Info.u8DHCPResult = FALSE;
		g_ucDHCPAddressPrepared = mDHCP_NEED_INIT;
		gs_EthAppCommMng.ui16DHCPReconfigTimeOUT  = 0;
		gNET_TCP_Info.iNetCreateSocketFlag = TRUE;
		gNET_TCP_Info.u32BreakCount =0;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
void resolv_found(char *name, u16_t *ipaddr)
{
	if(ipaddr == NULL)
	{

	}
	else
	{
		uip_ipaddr_copy(&gs_RemoteServerIp, ipaddr); 
		g_ucRemoteServerIpIsOk = 1;
	}
}
/************************************************/
/*函数名称:set_ip  			                */
/*函数功能:	从flash中读出配置，进行设置        */
/*入口参数:									                  */
/*出口参数:?									                  */
/*资源:           					          */
/*子函数:	         								              */
/*更改日期:2007.5.25      						          */
/************************************************/
// void  set_ip(void )
// {
// 	int i;
// 	//__XXX_UNION_VALUE unVal;
// 	uip_ipaddr_t ipaddr[2]; 
// 	UCHAR ip1[4];

// 	for(i=0;i<4;i++)//get the  ip
// 	{
// 		//GetCharSetData(SETDATA_IP_NUM+i,&ip1[i]);
// 		//ExGetSigInfo(E_TYPE_SYS, ID_u8IP_1 + i, &unVal, NULL, BY_E2PROM);
// 		//ip1[i] = unVal.chtemp[0];
// 		ip1[i] = gEvChargeInfo.u8IP_1[i];
// 	}
// 	// set the ip
// 	uip_ipaddr(ipaddr, ip1[0],ip1[1],ip1[2],ip1[3]);
// 	uip_sethostaddr(ipaddr);


// 	for(i=0;i<4;i++)//get the mask
//     {
//       //GetCharSetData(SETDATA_Subnetmask_NUM+i,&ip1[i]);
// 		//ExGetSigInfo(E_TYPE_SYS, ID_u8Subnetmask_1 + i, &unVal, NULL, BY_E2PROM);
// 		//ip1[i] = unVal.chtemp[0];
// 		ip1[i] = gEvChargeInfo.u8Subnetmask[i];
//     }
// 	//set the mask
// 	uip_ipaddr(ipaddr, ip1[0],ip1[1],ip1[2],ip1[3]);
// 	uip_setnetmask(ipaddr);


// 	for(i=0;i<4;i++)//get the gateway
// 	{
// 		//GetCharSetData(SETDATA_Gateway_NUM+i,&ip1[i]);
// 		//ExGetSigInfo(E_TYPE_SYS, ID_u8Gateway_1 + i, &unVal, NULL, BY_E2PROM);
// 		//ip1[i] = unVal.chtemp[0];
// 		ip1[i] = gEvChargeInfo.u8Gateway[i];
// 	}
// 	//set the draddr
// 	uip_ipaddr(ipaddr, ip1[0],ip1[1],ip1[2],ip1[3]);
// 	uip_setdraddr(ipaddr);

// }
/************************************************/
/*函数名称:TaskofWebComm()		                */
/*函数功能:网络任务							    */
/*入口参数:									    */
/*出口参数:无									*/
/*资源:           					            */
/*子函数:	         							*/
/*更改日期:2007.5.31      						*/
/************************************************/
#define UIPBUF ((struct uip_eth_hdr *)&uip_buf[0])
static void net_poll(void)
{
	int i;
	__XXX_UNION_VALUE unVal;
	UINT8 ucDHCPClientEnable = DISABLE;
	UINT8 ucServerIsDomainName = 0;
	static unsigned char s_uNeedReconnectFlag = 0;
	//get the input
	uip_len = dv_ethdev.fNetWorkDeviceRead();

	if (gNET_TCP_Info.iNetCreateSocketFlag == TRUE)
	{

		gNET_TCP_Info.iNetCreateSocketFlag = FALSE;
		ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_RemoteServerIsDomainName_NUM, &unVal, NULL, BY_E2PROM);
		ucServerIsDomainName = unVal.chtemp[0];
		//GetCharSetData(SETDATA_DHCPCLientEnable_NUM, &ucDHCPClientEnable);
		//ExGetSigInfo(E_TYPE_SYS, ID_NET_DHCP, &unVal, NULL, BY_E2PROM);
		ucDHCPClientEnable = gEvChargeInfo.u8DHCPClientEn;
		if (ucDHCPClientEnable == DISABLE)
		{
			ReInitNetPort();
			uip_len =0;
			set_ip();
			uip_init();
			Init_udp_exchange_app();
			if(ucServerIsDomainName)
			{
				ask_dns_server_get_ip();
				s_uNeedReconnectFlag = 1;
			}
			else
			{
				tcp_client_reconnect();
				s_uNeedReconnectFlag = 0;
			}
		}
		else
		{
			if (TRUE == gNET_TCP_Info.u8DHCPResult)
			{
				uip_len =0;
				DHCPset_ip();
				uip_init();
				Init_udp_exchange_app();
				if(ucServerIsDomainName)
				{
					ask_dns_server_get_ip();
					s_uNeedReconnectFlag = 1;
				}
				else
				{
					tcp_client_reconnect();
					s_uNeedReconnectFlag = 0;
				}
			}
		}
		return;
	}
	if(s_uNeedReconnectFlag)
	{
		if(g_ucRemoteServerIpIsOk)
		{		
			tcp_client_reconnect_domainname();
			s_uNeedReconnectFlag = 0;
		}
	}

	if(uip_len == 0)
	{
		if (u8IsLowLevelBreak())
		{
			g_ucRemoteServerIpIsOk = 0;
			resolv_init();
			return;
		}
	}
	else
	{
		gNET_TCP_Info.u32BreakCount = 0;
	}
    
	if(uip_len > 0)
	{
		reset_netchip = 0;
		//if it is arp packet
		if(UIPBUF->type == htons(UIP_ETHTYPE_ARP))
		{
			gSelfTest.fSetTaskID(Tasko_ID_ethernet);
			uip_arp_arpin();
			if (uip_len > 0)
			{
				reset_netchip_send = 0;               
				dv_ethdev.fNetWorkDeviceSend();
				gSelfTest.fSetTaskID(Tasko_ID_ethernet);
			}
		}
		//if it is ip packet
		else if(UIPBUF->type == htons(UIP_ETHTYPE_IP))
		{
			uip_arp_ipin();
			uip_input();
			if (uip_len > 0)
			{              
				gSelfTest.fSetTaskID(Tasko_ID_ethernet);
				uip_arp_out();              
				reset_netchip_send = 0;				                
				dv_ethdev.fNetWorkDeviceSend();
				gSelfTest.fSetTaskID(Tasko_ID_ethernet);
			}
		}
	}
	else if(timer_expired(&periodic_timer))
	{
		gSelfTest.fSetTaskID(Tasko_ID_ethernet);
		timer_reset(&periodic_timer);
		for(i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);
			if(uip_len > 0)
			{
				uip_arp_out();			//加以太网头结构，在主动连接时可能要构造ARP请求
				dv_ethdev.fNetWorkDeviceSend();
			}
		}
#if UIP_UDP	//UIP_UDP 
		//轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个
	/*	for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//处理UDP通信事件
			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				dv_ethdev.fNetWorkDeviceSend();//发送数据到以太网
			}
		}*/
#endif 
		gSelfTest.fSetTaskID(Tasko_ID_ethernet);
		/* Call the ARP timer function every 10 seconds. */
		if(timer_expired(&arp_timer)) {
			timer_reset(&arp_timer);
			uip_arp_timer();
			ucARPOnce = 0;
		}
	}
}




void ReInitNetPort(void)
{
	UCHAR mac[6];
	UCHAR i;
	__XXX_UNION_VALUE unVal;
	//init  the network
	for(i=0;i<6;i++)
	{
		//GetCharSetData(SETDATA_Mac_NUM+i,&mac[i]);
		ExGetSigInfo(E_TYPE_SYS, ID_u8Mac_1 + i, &unVal, NULL, BY_SRAM);
		 mac[i]= unVal.chtemp[0];
		uip_ethaddr.addr[5-i] = mac[i];
	}
	dv_ethdev.fInitNetPort();
	uip_init();
	uip_arp_init();
	ethInit();
	uip_router_init();
	//timer_set(&periodic_timer, ((CLOCK_SECOND * OS_TICKS_PER_SEC) / 2));
	//20210709 升级固件程序期间提升回包速度
	if(gs_UpGradeCtrl.u8UpgradeStat == UpgradeStatDef_Start)
	{
		timer_set(&periodic_timer, (PERIODIC_TIMER_UPGRADE));		//*2ms  = 200ms
	}
	else
	{
		//timer_set(&periodic_timer, ((CLOCK_SECOND * OS_TICKS_PER_SEC) / 2));
		timer_set(&periodic_timer, (300/ 2));
	}

	timer_set(&arp_timer, ((CLOCK_SECOND * OS_TICKS_PER_SEC) * 10));

	Init_udp_exchange_app();
}

void netIP_Deal(void);
void netIP_Deal(void)									//DHCP static!
{
	__XXX_UNION_VALUE unVal;
	UCHAR ucDHCPClientEnable = mENABLE;
	//UCHAR mac[10];
	UCHAR i = 0;

	gSelfTest.fSetTaskID(Tasko_ID_ethernet);
	//GetCharSetData(SETDATA_DHCPCLientEnable_NUM, &ucDHCPClientEnable);
	//ExGetSigInfo(E_TYPE_SYS, ID_NET_DHCP, &unVal, NULL, BY_E2PROM);
	ucDHCPClientEnable = gEvChargeInfo.u8DHCPClientEn;
	if (ucDHCPClientEnable == mDISABLE)				//不使能则更新设置IP
	{
		set_ip();
		g_ucDHCPAddressPrepared = mDHCP_NEED_INIT;
	//	ucDHCPClientEnable = mENABLE;           //by lxr01202
	}
	else
	{
		if (g_ucDHCPAddressPrepared == mDHCP_NEED_INIT)
		{
			ReInitNetPort();

			//Re-init to avoid UDP connections fully occupied.
			uip_init();
			Init_udp_exchange_app();
			uip_arp_init();
			ethInit();
			uip_router_init();
			for(i=0;i<6;i++)
			{
				//GetCharSetData(SETDATA_Mac_NUM+i,&mac[5-i]);
				ExGetSigInfo(E_TYPE_SYS, ID_u8Mac_1 + i, &unVal, NULL, BY_SRAM);
				mac[5-i]= unVal.chtemp[0]; 
			}
			extern void dhcpc_init(const void *mac_addr, int mac_len);
			dhcpc_init(mac, 6);
			g_ucDHCPAddressPrepared = mDHCP_REQUESTING;
		}
		else if(g_ucDHCPAddressPrepared == mDHCP_REQUESTING)
		{
			gSelfTest.fSetTaskID(Tasko_ID_ethernet);
			extern void dhcpc_appcall(void);
			dhcpc_appcall();
			g_ucDHCPAddressPrepared = mDHCP_CONFIGURED;		//让void uip_router_udp_appcall(void)继续DHCP！
		}
	}
}

void netInitMac(void);

//给全局变量uip_ethaddr  赋值
void netInitMac(void)
{
  __XXX_UNION_VALUE unVal;
	UCHAR i = 0;
	mac[5] = 0x00;
	mac[4] = 0x09;
	mac[3] = 0xf5;
	mac[2] = 0x0c;
	mac[1] = 0xE2;
	mac[0] = 0x02;
	for(i=0;i<6;i++)
	{
		//SetCharSetData(SETDATA_Mac_NUM+i,&mac[i]);
		ExGetSigInfo(E_TYPE_SYS, ID_u8Mac_1 + i, &unVal, NULL, BY_SRAM);
		mac[i]= unVal.chtemp[0];
		uip_ethaddr.addr[5-i] = mac[i];
	}
}

void NET_Clean_RBuf(void)
{
	UCHAR i = 0;
	for (i = 0; i < NET_R_NUM; i++)
	{
		gNET_TCP_Info.u8ReadBuf[i].u8DataFlag = 0;
		gNET_TCP_Info.u8ReadBuf[i].u16Lengh = 0;
	}
}
void NET_Clean_SBuf(void)
{
	UINT8 u32Idx = 0;
	UINT32 u32Temp = 0;
	gNET_TCP_Info.u8SendIdx = 0;
	MutexLock(gEvChargeInfo.hMetexNet_W);
	for(u32Idx = 0; u32Idx < NET_S_NUM; u32Idx++)
	{
		gNET_TCP_Info.u8SendBuf[u32Idx].u8DataFlag = FALSE;
		for (u32Temp = 0; u32Temp < NET_BUF_DATA_L/2; u32Temp++)
		{
			gNET_TCP_Info.u8SendBuf[u32Idx].u8Buf[u32Temp] = 0;//BUF 清 0
		}
	}
	MutexUnlock(gEvChargeInfo.hMetexNet_W);
	return;
}

//TRUE Send OK,   FALSE Send Fails
UINT8 NET_Send(UINT8* pBuf, UINT32 u32Len, UINT32 TimeOut10ms)
{
	UINT8  u8Send2Pack = FALSE;
	unsigned short u16SecondPackLenth = 0;
	UINT8  u8FindEmptyBuf = FALSE;
	UINT8  u8BufIdx = 0;
	UINT32 u32DataIdx = 0;
	UINT32 u32Idx = 0;
	TimeOut10ms = MAX(TimeOut10ms, 1);				//保证1以上
	for (u32Idx = 0; u32Idx < TimeOut10ms; u32Idx++)
	{
		gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
		if (!CHK_CONN_STAT(gNET_TCP_Info.u8MachineStat))
		{
			return FALSE;
		}
		else if (CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))
		{
			OSTimeDly(20);
		}
		else
		{
			break;
		}
	}
	SET_SEND_STAT(gNET_TCP_Info.u8MachineStat);
	//先找到一个空的 BUF 往里填数据。

	if(u32Len > g_i32TcpMssLenth)
	{
		u8Send2Pack = TRUE;

		u16SecondPackLenth = u32Len - g_i32TcpMssLenth;
		u32Len = g_i32TcpMssLenth;
	}
	for (u32Idx = 0; u32Idx < TimeOut10ms; u32Idx++)
	{
		OSTimeDly(10);
		for(u8BufIdx = 0; u8BufIdx < NET_S_NUM; u8BufIdx++)
		{	
			gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
			MutexLock(gEvChargeInfo.hMetexNet_W);
			if (FALSE == gNET_TCP_Info.u8SendBuf[u8BufIdx].u8DataFlag)
			{
				u8FindEmptyBuf = TRUE;
				for (u32DataIdx = 0; (u32DataIdx < u32Len) && (u32DataIdx < NET_BUF_DATA_L); u32DataIdx++)
				{
					gNET_TCP_Info.u8SendBuf[u8BufIdx].u8Buf[u32DataIdx] = pBuf[u32DataIdx];
				}
				gNET_TCP_Info.u8SendBuf[u8BufIdx].u16SendNoBak = gNET_TCP_Info.u16SendNo;
				gNET_TCP_Info.u16SendNo++;

				gNET_TCP_Info.u8SendBuf[u8BufIdx].u8DataFlag = TRUE;
				gNET_TCP_Info.u8SendBuf[u8BufIdx].u16Lengh = u32Len;
				MutexUnlock(gEvChargeInfo.hMetexNet_W);
				//return TRUE;
				break;
			}
			MutexUnlock(gEvChargeInfo.hMetexNet_W);
		}
		if(u8FindEmptyBuf == TRUE)
		{
			break;
		}
		OSTimeDly(10);
	}

	if(u8Send2Pack)
	{
		u8Send2Pack = FALSE;

		for (u32Idx = 0; u32Idx < TimeOut10ms; u32Idx++)
		{
			gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
			if (!CHK_CONN_STAT(gNET_TCP_Info.u8MachineStat))
			{
				return FALSE;
			}
			else if (CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))
			{
				OSTimeDly(20);
			}
			else
			{
				break;
			}
		}
		SET_SEND_STAT(gNET_TCP_Info.u8MachineStat);
		//先找到一个空的 BUF 往里填数据。

		if(u16SecondPackLenth > g_i32TcpMssLenth)
		{
			u8Send2Pack = TRUE;
			u16SecondPackLenth -= g_i32TcpMssLenth;
			u32Len = g_i32TcpMssLenth;
		}
		else 
		{
			u32Len = u16SecondPackLenth;
		}

		u8FindEmptyBuf = FALSE;
		
		for (u32Idx = 0; u32Idx < TimeOut10ms; u32Idx++)
		{
			OSTimeDly(10);
			for(u8BufIdx = 0; u8BufIdx < NET_S_NUM; u8BufIdx++)
			{	
				gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
				MutexLock(gEvChargeInfo.hMetexNet_W);
				if (FALSE == gNET_TCP_Info.u8SendBuf[u8BufIdx].u8DataFlag)
				{
					u8FindEmptyBuf = TRUE;
					for (u32DataIdx = 0; (u32DataIdx < u32Len) && (u32DataIdx < NET_BUF_DATA_L); u32DataIdx++)
					{
						gNET_TCP_Info.u8SendBuf[u8BufIdx].u8Buf[u32DataIdx] = pBuf[u32DataIdx+g_i32TcpMssLenth];
					}
					gNET_TCP_Info.u8SendBuf[u8BufIdx].u16SendNoBak = gNET_TCP_Info.u16SendNo;
					gNET_TCP_Info.u16SendNo++;
					gNET_TCP_Info.u8SendBuf[u8BufIdx].u8DataFlag = TRUE;
					gNET_TCP_Info.u8SendBuf[u8BufIdx].u16Lengh = u32Len;
					MutexUnlock(gEvChargeInfo.hMetexNet_W);
					//return TRUE;
					break;
				}
				MutexUnlock(gEvChargeInfo.hMetexNet_W);
			}
			if(u8FindEmptyBuf == TRUE)
			{
				break;
			}
			OSTimeDly(10);
		}
	}

	if(u8Send2Pack)
	{
		u8Send2Pack = FALSE;

		for (u32Idx = 0; u32Idx < TimeOut10ms; u32Idx++)
		{
			gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
			if (!CHK_CONN_STAT(gNET_TCP_Info.u8MachineStat))
			{
				return FALSE;
			}
			else if (CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))
			{
				OSTimeDly(20);
			}
			else
			{
				break;
			}
		}
		SET_SEND_STAT(gNET_TCP_Info.u8MachineStat);
		//先找到一个空的 BUF 往里填数据。

		u32Len = u16SecondPackLenth;	

		u8FindEmptyBuf = FALSE;

		for (u32Idx = 0; u32Idx < TimeOut10ms; u32Idx++)
		{
			OSTimeDly(10);
			for(u8BufIdx = 0; u8BufIdx < NET_S_NUM; u8BufIdx++)
			{	
				gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
				MutexLock(gEvChargeInfo.hMetexNet_W);
				if (FALSE == gNET_TCP_Info.u8SendBuf[u8BufIdx].u8DataFlag)
				{
					u8FindEmptyBuf = TRUE;
					for (u32DataIdx = 0; (u32DataIdx < u32Len) && (u32DataIdx < NET_BUF_DATA_L); u32DataIdx++)
					{
						gNET_TCP_Info.u8SendBuf[u8BufIdx].u8Buf[u32DataIdx] = pBuf[u32DataIdx+g_i32TcpMssLenth*2];
					}
					gNET_TCP_Info.u8SendBuf[u8BufIdx].u16SendNoBak = gNET_TCP_Info.u16SendNo;
					gNET_TCP_Info.u16SendNo++;

					gNET_TCP_Info.u8SendBuf[u8BufIdx].u8DataFlag = TRUE;
					gNET_TCP_Info.u8SendBuf[u8BufIdx].u16Lengh = u32Len;
					MutexUnlock(gEvChargeInfo.hMetexNet_W);
					//return TRUE;
					break;
				}
				MutexUnlock(gEvChargeInfo.hMetexNet_W);
			}
			if(u8FindEmptyBuf == TRUE)
			{
				break;
			}
			OSTimeDly(10);
		}
	}


	if(u8FindEmptyBuf == TRUE)
	{
		//for (u32Idx = 0; u32Idx < TimeOut10ms; u32Idx++)
		for (u32Idx = 0; u32Idx < 3000; u32Idx++)
		{
			OSTimeDly(10);
			gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
			if (!CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))
			{
				return TRUE;
			}			
		}
	}

	
	return FALSE;
	//UINT16 u16Idx = 0;
	////Wait Last send OK
	//for (u16Idx = 0; u16Idx< 100;u16Idx++)
	//{
	//	if (CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))
	//	{
	//		OSTimeDly(5);
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
	//SET_SEND_STAT(gNET_TCP_Info.u8MachineStat);
	//11111111
	//for (u16Idx = 0; u16Idx < 300; u16Idx++)
	//{
	//	OSTimeDly(20);
	//	gSelfTest.fSetTaskID(Tasko_ID_ETHERNET_App);
	//	if (!CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))
	//	{
	//		break;
	//	}
	//}
}


//一次只能全读取出来！ 
UINT32 NET_Read(void *input_buf)
{	
	UINT32 u32Len = 0;
	INT8 uIdx = 0;
	MutexLock(gEvChargeInfo.hMetexNet_R);
	for (uIdx = NET_R_NUM - 1; uIdx >= 0 ; uIdx--)
	{
		if (gNET_TCP_Info.u8ReadBuf[uIdx].u8DataFlag & 0x01)
		{
			memcpy(input_buf, gNET_TCP_Info.u8ReadBuf[uIdx].u8Buf, NET_BUF_DATA_L-1);
			u32Len = (UINT16)gNET_TCP_Info.u8ReadBuf[uIdx].u16Lengh;
			memset(gNET_TCP_Info.u8ReadBuf[uIdx].u8Buf, 0, NET_BUF_DATA_L/2);
			gNET_TCP_Info.u8ReadBuf[uIdx].u8DataFlag = 0;
			MutexUnlock(gEvChargeInfo.hMetexNet_R);
			return u32Len;
		}
	}
	MutexUnlock(gEvChargeInfo.hMetexNet_R);
	return 0;
}



extern u16_t uip_len, uip_slen;


const UCHAR NodeStatMap[5][2] = 
{
	_RingNodeStat_Idle, 0x00,
	_RingNodeStat_Slave, 0x01,
	_RingNodeStat_SubGun, 0x02,
	_RingNodeStat_MasterGun, 0x03,
	_RingNodeStat_NotAvailable, 0xff,
};
int NodeStatMapIndex(UCHAR uDir, UCHAR Input)
{
	UCHAR i,uctemp;

	uctemp = sizeof(NodeStatMap)/2;

	for(i = 0; i < uctemp; i++)
	{
		if(uDir == 0)
		{
			if(NodeStatMap[i][0] == Input)
			{
				return NodeStatMap[i][1];
			}			
		}
		else
		{
			if(NodeStatMap[i][1] == Input)
			{
				return NodeStatMap[i][0];
			}			
		}
	}

	return -1;
}

enum tagUDP_Ring_Cmd_def
{
	UDP_Ring_Cmd_Slave = 0x01,
	UDP_Ring_Cmd_Master = 0x02,
}; 

enum tagUDP_Ring_Master_Cmd_def
{
	UDP_Ring_Master_Cmd_Adj_P = 0x01,
	//UDP_Ring_Master_Cmd_Adj_N = 0x02,
	UDP_Ring_Master_Cmd_AckSlaveGunJoin = 0x10,
	UDP_Ring_Master_Cmd_ReqSlaveJoin = 0x20,
	UDP_Ring_Master_Cmd_ReqQuit = 0xff,
}; 

void NodeInfoSendFloat(UCHAR *Input, float fInput)
{
	union
	{
		float			ftemp;						          /*浮点数*/
		unsigned long	Ltemp;
		unsigned char	chtemp[4];					    /*字节型数*/
	}unTempVal;

	unTempVal.ftemp = fInput;

	*Input++ = unTempVal.chtemp[0];
	*Input++ = unTempVal.chtemp[1];
	*Input++ = unTempVal.chtemp[2];
	*Input++ = unTempVal.chtemp[3];
}
float NodeInfoGetFloat(UCHAR *Input)
{
	union
	{
		float			ftemp;						          /*浮点数*/
		unsigned long	Ltemp;
		unsigned char	chtemp[4];					    /*字节型数*/
	}unTempVal;

	unTempVal.chtemp[0] = *Input++;
	unTempVal.chtemp[1] = *Input++;
	unTempVal.chtemp[2] = *Input++;
	unTempVal.chtemp[3] = *Input++;

	return unTempVal.ftemp;
}

void PackNodeInfoToSend(UCHAR u8Direction, UCHAR *Input, RING_NODE* pNodeMe, UCHAR u8GunNo)
{
	union
	{
		short			i16temp;						          /*浮点数*/
		unsigned short	u16temp;
		unsigned char	chtemp[2];					    /*字节型数*/
	}unTempVal;
	
	if(u8Direction == 0)
	{
		unTempVal.u16temp = (UINT16)(pNodeMe->fMaxMergeCurr_ACDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.u16temp = (UINT16)(pNodeMe->fOutVolt_ACDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.u16temp = (UINT16)(pNodeMe->fInputVolt_ACDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.i16temp = (INT16)(pNodeMe->fOutCurr_ACDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.i16temp = (INT16)(pNodeMe->fInputCurr_ACDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.u16temp = (UINT16)(pNodeMe->fMaxAlowPwr_ACDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.u16temp = (UINT16)(pNodeMe->fMaxMergeCurr_DCDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.u16temp = (UINT16)(pNodeMe->fOutVolt_DCDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.u16temp = (UINT16)(pNodeMe->fInputVolt_DCDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.i16temp = (INT16)(pNodeMe->fOutCurr_DCDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.i16temp = (INT16)(pNodeMe->fInputCurr_DCDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.u16temp = (UINT16)(pNodeMe->fMaxAlowPwr_DCDC*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];

		unTempVal.u16temp = (UINT16)(pNodeMe->fGunLmtCurr*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];
		//预留1个信号
		*Input++ = pNodeMe->u8ConvtorCtrlOutStat;
		if((pNodeMe->emRingNodeState == _RingNodeStat_MasterGun)  //主枪
			&&(gEvChargeInfo.u8_EvMaxVoltLevel_IsValid[u8GunNo] == 1))
		{
			*Input++ = gEvChargeInfo.u8_EvMaxVoltLevel[u8GunNo];
		}
		else
		{
			*Input++ = 0;  //pNodeMe->u8Reserve;
		}

		if(pNodeMe->fConvtExtVolt < 0)
			unTempVal.u16temp = 0;
		else
			unTempVal.u16temp = (UINT16)(pNodeMe->fConvtExtVolt*10);
		*Input++ = unTempVal.chtemp[0];
		*Input++ = unTempVal.chtemp[1];
		//预留
		*Input++ = 0;
		*Input++ = 0;
	}
	else if(u8Direction == 1)
	{
		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fMaxMergeCurr_ACDC = (float)unTempVal.u16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fOutVolt_ACDC = (float)unTempVal.u16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fInputVolt_ACDC = (float)unTempVal.u16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fOutCurr_ACDC = (float)unTempVal.i16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fInputCurr_ACDC = (float)unTempVal.i16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fMaxAlowPwr_ACDC = (float)unTempVal.u16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fMaxMergeCurr_DCDC = (float)unTempVal.u16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fOutVolt_DCDC = (float)unTempVal.u16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fInputVolt_DCDC = (float)unTempVal.u16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fOutCurr_DCDC = (float)unTempVal.i16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fInputCurr_DCDC = (float)unTempVal.i16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fMaxAlowPwr_DCDC = (float)unTempVal.u16temp/10;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fGunLmtCurr = (float)unTempVal.u16temp/10;	

		//预留一个信号		
		pNodeMe->u8ConvtorCtrlOutStat =  *Input++;
		pNodeMe->u8Reserve = *Input++;

		unTempVal.chtemp[0] = *Input++;
		unTempVal.chtemp[1] = *Input++;
		pNodeMe->fConvtExtVolt = (float)unTempVal.u16temp/10;	

		//预留2个字节
		Input++;
		Input++;


		pNodeMe->fOutVolt = MAX(pNodeMe->fOutVolt_DCDC,pNodeMe->fOutVolt_ACDC);
		pNodeMe->fOutCurr = pNodeMe->fOutCurr_ACDC + pNodeMe->fOutCurr_DCDC;
	}	
}

void PrepareRingExchangeInfo(void)
{
	UINT8 i, u8GunNo, uctemp, ucNodeStat, ucSlaveNum, u8CtrlAction;
	UINT8 *pDatabuf, *pSlaveNumPos, *pFrameLenPos;
	CAN_RING_DATA *pRing_Rough, *pAnother_Ring_Rough;
	UINT16 u16SendLenth, ii, u16temp;
	union
	{
		short			i16temp;						          /*浮点数*/
		unsigned short	u16temp;
		unsigned char	chtemp[2];					    /*字节型数*/
	}unTempVal;

	time_t_ring tmNow = time_ring(NULL);

	MutexLock(gs_RingExchangeInfo.hMutex);

	pDatabuf = gs_RingExchangeInfo.Sendbuf;
	pFrameLenPos = pDatabuf;
	pDatabuf += 2;
	
	for(u8GunNo = 0; u8GunNo < 2; u8GunNo++)
	{
		if(u8GunNo == 1)
		{
			pRing_Rough = g_pRing_B;
			pAnother_Ring_Rough = g_pRing_A;
		}
		else
		{
			pRing_Rough = g_pRing_A;
			pAnother_Ring_Rough = g_pRing_B;
		}		

		ucNodeStat = pRing_Rough->pNodeMe->emRingNodeState;

		/* 一.主枪 */
		if(ucNodeStat == _RingNodeStat_MasterGun)
		{
			*pDatabuf++ = UDP_Ring_Cmd_Master;
			*pDatabuf++ = pRing_Rough->pNodeMe->byNodeAddress;
			*pDatabuf++ = NodeStatMapIndex(0, ucNodeStat);
			*pDatabuf++ = pRing_Rough->pNodeMe->emMergeStatus;
			
			//
			PackNodeInfoToSend(0, pDatabuf, pRing_Rough->pNodeMe, u8GunNo);
			pDatabuf += 32;

			NodeInfoSendFloat(pDatabuf, pRing_Rough->pNodeMe->fkWhReading);
			pDatabuf += 4;



			//*pDatabuf++ = pRing_Rough->pNodeMe->nMasterGunNo;
			//*pDatabuf++ = pRing_Rough->pNodeMe->nLockedMasterID;
			pSlaveNumPos = pDatabuf;
			pDatabuf += 1;

			ucSlaveNum = 0;
			//1.主机处于查找从枪阶段
			if(pRing_Rough->emCurrState == RING_STATE_MasterGun_FindSlaveGun)
			{
				for(i = 0; i < MAX_SUPPORTED_RING_NODE; i++)
				{
					//判断是否该辅助枪正在申请加入
					if((g_RingAllInfo.allNodes[i].emRingNodeState == _RingNodeStat_Idle)
						&& (g_RingAllInfo.allNodes[i].u8AppendInfoByte[0] == _SlaveRingCmd_ReqBeAuxGun)
						&& (g_RingAllInfo.allNodes[i].u8AppendInfoByte[1] == pRing_Rough->pNodeMe->byNodeAddress))
					{
						//本桩内，不发出去
						if(g_RingAllInfo.allNodes[i].byNodeAddress == pAnother_Ring_Rough->pNodeMe->byNodeAddress)
						{
							if(pAnother_Ring_Rough->pNodeMe->emRingNodeState == _RingNodeStat_Idle)
							{
								//确认成为辅助枪
								if(pAnother_Ring_Rough->slave.u8ReqBeSlaveGun_ToMasterGun)								
								{					
									if((pRing_Rough->pNodeMe->byNodeAddress == pAnother_Ring_Rough->slave.u8ReqBeWhoseSlaveGun)
										&& (pAnother_Ring_Rough->slave.u8BeSlaveGunAck_FromMasterGun == FALSE))  //避免同时收到
									{
										pAnother_Ring_Rough->slave.u8BeSlaveGunAck_FromMasterGun = TRUE;
									}								
								}
							}
							continue;
						}

						*pDatabuf++ = g_RingAllInfo.allNodes[i].byNodeAddress;
						*pDatabuf++ = UDP_Ring_Master_Cmd_AckSlaveGunJoin+pRing_Rough->pNodeMe->byNodeAddress;
						//
						NodeInfoSendFloat(pDatabuf, 0);   //需求电压
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, 0);  //需求电流（ACDC）
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, 0);  //需求电流（DCDC）
						pDatabuf += 4;
						*pDatabuf++ = 0;		//模块开关控制（ACDC）
						*pDatabuf++ = 0;		//模块开关控制（DCDC）
						*pDatabuf++ = 0;		//环网接触器开关控制
						*pDatabuf++ = 0;		//输出接触器开关控制

						ucSlaveNum++;
					}						
				}
			}
			//2.主枪在配置阶段
			//else if(pRing_Rough->emCurrState == RING_STATE_MasterGun_BeforeCharge)
			//{
			//	for(i = 0; i < pRing_Rough->master.nRingGunTotal; i++)
			//	{
			//		*pDatabuf++ = pRing_Rough->master.SlaveGunBuf[i].pNode->byNodeAddress;
			//		*pDatabuf++ = UDP_Ring_Master_Cmd_Adj_P;
			//		//
			//		NodeInfoSendFloat(pDatabuf, pRing_Rough->master.SlaveGunBuf[i].fVolt2Sent);   //需求电压
			//		pDatabuf += 4;
			//		NodeInfoSendFloat(pDatabuf, pRing_Rough->master.SlaveGunBuf[i].fCurr2Sent_ACDC);  //需求电流（ACDC）
			//		pDatabuf += 4;
			//		NodeInfoSendFloat(pDatabuf, pRing_Rough->master.SlaveGunBuf[i].fCurr2Sent_DCDC);  //需求电流（DCDC）
			//		pDatabuf += 4;
			//		*pDatabuf++ = pRing_Rough->master.SlaveGunBuf[i].emSetPwrOnOff_ACDC;		//模块开关控制（ACDC）
			//		*pDatabuf++ = pRing_Rough->master.SlaveGunBuf[i].emSetPwrOnOff_DCDC;		//模块开关控制（DCDC）
			//		*pDatabuf++ = pRing_Rough->master.SlaveGunBuf[i].emSetRingRelayOnOff;		//环网接触器开关控制
			//		*pDatabuf++ = pRing_Rough->master.SlaveGunBuf[i].emSetOutRelayOnOff;		//输出接触器开关控制

			//		ucSlaveNum++;
			//	}
			//}
			//3.主枪已进入充电阶段
			else if((pRing_Rough->emCurrState == RING_STATE_MasterGun_Charging)
				||(pRing_Rough->emCurrState == RING_STATE_MasterGun_Wait_Merging)
				||(pRing_Rough->emCurrState == RING_STATE_MasterGun_Wait_Cancelling)
				||(pRing_Rough->emCurrState == RING_STATE_MasterGun_Stopping)
				)
			{
				//3.1 给辅助枪发送命令
				for(i = 0; i < pRing_Rough->master.nRingAuxGunTotal; i++)
				{
					//本桩内，不发出去
					if(pRing_Rough->master.sRingGuns[1+i].pNode->byNodeAddress == pAnother_Ring_Rough->pNodeMe->byNodeAddress)
					{
						if((pAnother_Ring_Rough->pNodeMe->emRingNodeState == _RingNodeStat_SubGun)
							&& (pAnother_Ring_Rough->slave.pMasterGun->byNodeAddress == pRing_Rough->pNodeMe->byNodeAddress))
						{
							pAnother_Ring_Rough->slave.u8RcvMasterGunCommOvertimeCnt = 0;
							pAnother_Ring_Rough->slave.tm_LasReceive_Cmd_FromMaster = time_ring(NULL);

							if(pRing_Rough->master.sRingGuns[1+i].CtrlQuitRing)
							{
								pAnother_Ring_Rough->slave.fReqVolt = 0;
								pAnother_Ring_Rough->slave.fReqCurr_ACDC = 0;
								pAnother_Ring_Rough->slave.fReqCurr_DCDC = 0;
								pAnother_Ring_Rough->slave.emSetPwrOnOff_ACDC = 0;
								pAnother_Ring_Rough->slave.emSetPwrOnOff_DCDC = 0;
								pAnother_Ring_Rough->slave.u8InfoQuitRing_FromMasterGun = TRUE;
							}
							else 
							{
								pAnother_Ring_Rough->slave.fReqVolt = pRing_Rough->master.sRingGuns[1+i].fVolt2Sent;
								pAnother_Ring_Rough->slave.fReqCurr_ACDC = pRing_Rough->master.sRingGuns[1+i].fCurr2Sent_ACDC;
								pAnother_Ring_Rough->slave.fReqCurr_DCDC = pRing_Rough->master.sRingGuns[1+i].fCurr2Sent_DCDC;
								pAnother_Ring_Rough->slave.emSetPwrOnOff_ACDC = pRing_Rough->master.sRingGuns[1+i].emSetPwrOnOff_ACDC;
								pAnother_Ring_Rough->slave.emSetPwrOnOff_DCDC = pRing_Rough->master.sRingGuns[1+i].emSetPwrOnOff_DCDC;

								//20201214 lkf 环网接触器闭合增加延时
								//pAnother_Ring_Rough->slave.emSetRingRelayOnOff = pRing_Rough->master.sRingGuns[1+i].emSetRingRelayOnOff;
								u8CtrlAction = MCCB_DISCONNECT;
								if((pRing_Rough->master.sRingGuns[1+i].emSetRingRelayOnOff == MCCB_CONNECT)
									&& ((tmNow - pRing_Rough->master.sRingGuns[1+i].tmlastCtrlRingRelayCon) > (SLAVE_MERGE_OUTPUT_DELAY/2)))
								{
									u8CtrlAction = MCCB_CONNECT;
								}
								pAnother_Ring_Rough->slave.emSetRingRelayOnOff = u8CtrlAction;
								
								pAnother_Ring_Rough->slave.emSetOutRelayOnOff = pRing_Rough->master.sRingGuns[1+i].emSetOutRelayOnOff;
								pAnother_Ring_Rough->slave.bReceivedAdj_FromMasterGun = TRUE;
							}
						}
						continue;
					}

					*pDatabuf++ = pRing_Rough->master.sRingGuns[1+i].pNode->byNodeAddress;
					if(pRing_Rough->master.sRingGuns[1+i].CtrlQuitRing)
					{							
						*pDatabuf++ = UDP_Ring_Master_Cmd_ReqQuit;
						NodeInfoSendFloat(pDatabuf, 0);   //需求电压
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, 0);  //需求电流（ACDC）
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, 0);  //需求电流（DCDC）
						pDatabuf += 4;
						*pDatabuf++ = 0;		//模块开关控制（ACDC）
						*pDatabuf++ = 0;		//模块开关控制（DCDC）
						*pDatabuf++ = 0;		//环网接触器开关控制
						*pDatabuf++ = 0;		//输出接触器开关控制
					}
					else
					{
						*pDatabuf++ = UDP_Ring_Master_Cmd_Adj_P;
						//
						NodeInfoSendFloat(pDatabuf, pRing_Rough->master.sRingGuns[1+i].fVolt2Sent);   //需求电压
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, pRing_Rough->master.sRingGuns[1+i].fCurr2Sent_ACDC);  //需求电流（ACDC）
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, pRing_Rough->master.sRingGuns[1+i].fCurr2Sent_DCDC);  //需求电流（DCDC）
						pDatabuf += 4;
						*pDatabuf++ = pRing_Rough->master.sRingGuns[1+i].emSetPwrOnOff_ACDC;		//模块开关控制（ACDC）
						*pDatabuf++ = pRing_Rough->master.sRingGuns[1+i].emSetPwrOnOff_DCDC;		//模块开关控制（DCDC）
						
						//20201214 lkf 环网接触器闭合增加延时
						//*pDatabuf++ = pRing_Rough->master.sRingGuns[1+i].emSetRingRelayOnOff;		//环网接触器开关控制
						u8CtrlAction = MCCB_DISCONNECT;
						if((pRing_Rough->master.sRingGuns[1+i].emSetRingRelayOnOff == MCCB_CONNECT)
							&& ((tmNow - pRing_Rough->master.sRingGuns[1+i].tmlastCtrlRingRelayCon) > (SLAVE_MERGE_OUTPUT_DELAY/2)))
						{
							u8CtrlAction = MCCB_CONNECT;
						}
						*pDatabuf++ = u8CtrlAction;

						*pDatabuf++ = pRing_Rough->master.sRingGuns[1+i].emSetOutRelayOnOff;			//输出接触器开关控制
					}

					ucSlaveNum++;
				}// for(i = 0; i < pRing_Rough->master.nRingAuxGunTotal; i++)

				//3.2 给主枪的从机发送命令
				for(i = 0; i < pRing_Rough->master.nSlaveTotal; i++)
				{
					//本桩内，不发出去
					if(pRing_Rough->master.slaves[i]->pNode->byNodeAddress == pAnother_Ring_Rough->pNodeMe->byNodeAddress)
					{
						if((pAnother_Ring_Rough->pNodeMe->emRingNodeState == _RingNodeStat_Slave)
							&& (pAnother_Ring_Rough->pNodeMe->nMasterGunNo == pRing_Rough->pNodeMe->byNodeAddress))
						{
							pAnother_Ring_Rough->slave.u8RcvMasterGunCommOvertimeCnt = 0;
							pAnother_Ring_Rough->slave.tm_LasReceive_Cmd_FromMaster = time_ring(NULL);

							if(pRing_Rough->master.slaves[i]->CtrlQuitRing)
							{
								pAnother_Ring_Rough->slave.fReqVolt = 0;
								pAnother_Ring_Rough->slave.fReqCurr_ACDC = 0;
								pAnother_Ring_Rough->slave.fReqCurr_DCDC = 0;
								pAnother_Ring_Rough->slave.emSetPwrOnOff_ACDC = 0;
								pAnother_Ring_Rough->slave.emSetPwrOnOff_DCDC = 0;
								pAnother_Ring_Rough->slave.u8InfoQuitRing_FromMasterGun = TRUE;
							}
							else 
							{
								pAnother_Ring_Rough->slave.fReqVolt = pRing_Rough->master.slaves[i]->fVolt2Sent;
								pAnother_Ring_Rough->slave.fReqCurr_ACDC = pRing_Rough->master.slaves[i]->fCurr2Sent_ACDC;
								pAnother_Ring_Rough->slave.fReqCurr_DCDC = pRing_Rough->master.slaves[i]->fCurr2Sent_DCDC;
								pAnother_Ring_Rough->slave.emSetPwrOnOff_ACDC = pRing_Rough->master.slaves[i]->emSetPwrOnOff_ACDC;
								pAnother_Ring_Rough->slave.emSetPwrOnOff_DCDC = pRing_Rough->master.slaves[i]->emSetPwrOnOff_DCDC;

								
								//20201214 lkf 环网接触器闭合增加延时
								//pAnother_Ring_Rough->slave.emSetRingRelayOnOff = pRing_Rough->master.slaves[i]->emSetRingRelayOnOff;
								u8CtrlAction = MCCB_DISCONNECT;
								if((pRing_Rough->master.slaves[i]->emSetRingRelayOnOff == MCCB_CONNECT)
									&& ((tmNow - pRing_Rough->master.slaves[i]->tmlastCtrlRingRelayCon) > (SLAVE_MERGE_OUTPUT_DELAY/2)))
								{
									u8CtrlAction = MCCB_CONNECT;
								}
								pAnother_Ring_Rough->slave.emSetRingRelayOnOff = u8CtrlAction;

								pAnother_Ring_Rough->slave.emSetOutRelayOnOff = pRing_Rough->master.slaves[i]->emSetOutRelayOnOff;
								pAnother_Ring_Rough->slave.bReceivedAdj_FromMasterGun = TRUE;
							}
						}
						continue;
					}


					*pDatabuf++ = pRing_Rough->master.slaves[i]->pNode->byNodeAddress;
					if(pRing_Rough->master.slaves[i]->CtrlQuitRing)
					{							
						*pDatabuf++ = UDP_Ring_Master_Cmd_ReqQuit;
						NodeInfoSendFloat(pDatabuf, 0);   //需求电压
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, 0);  //需求电流（ACDC）
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, 0);  //需求电流（DCDC）
						pDatabuf += 4;
						*pDatabuf++ = 0;		//模块开关控制（ACDC）
						*pDatabuf++ = 0;		//模块开关控制（DCDC）
						*pDatabuf++ = 0;		//环网接触器开关控制
						*pDatabuf++ = 0;		//输出接触器开关控制
					}
					else
					{
						*pDatabuf++ = UDP_Ring_Master_Cmd_Adj_P;
						//
						NodeInfoSendFloat(pDatabuf, pRing_Rough->master.slaves[i]->fVolt2Sent);   //需求电压
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, pRing_Rough->master.slaves[i]->fCurr2Sent_ACDC);  //需求电流（ACDC）
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, pRing_Rough->master.slaves[i]->fCurr2Sent_DCDC);  //需求电流（DCDC）
						pDatabuf += 4;
						*pDatabuf++ = pRing_Rough->master.slaves[i]->emSetPwrOnOff_ACDC;		//模块开关控制（ACDC）
						*pDatabuf++ = pRing_Rough->master.slaves[i]->emSetPwrOnOff_DCDC;		//模块开关控制（DCDC）
						
						
						//20201214 lkf 环网接触器闭合增加延时
						//*pDatabuf++ = pRing_Rough->master.slaves[i]->emSetRingRelayOnOff;		//环网接触器开关控制
						u8CtrlAction = MCCB_DISCONNECT;
						if((pRing_Rough->master.slaves[i]->emSetRingRelayOnOff == MCCB_CONNECT)
							&& ((tmNow - pRing_Rough->master.slaves[i]->tmlastCtrlRingRelayCon) > (SLAVE_MERGE_OUTPUT_DELAY/2)))
						{
							u8CtrlAction = MCCB_CONNECT;
						}
						*pDatabuf++ = u8CtrlAction;

						*pDatabuf++ = pRing_Rough->master.slaves[i]->emSetOutRelayOnOff;			//输出接触器开关控制
					}

					ucSlaveNum++;
				}//for(i = 0; i < pRing_Rough->master.nSlaveTotal; i++)

				//3.3 新并入从机命令
				if(pRing_Rough->emCurrState == RING_STATE_MasterGun_Wait_Merging)
				{
					//本桩内，不发出去
					if(pRing_Rough->master.pSlave_InHandling->pNode->byNodeAddress == pAnother_Ring_Rough->pNodeMe->byNodeAddress)
					{
						if((pAnother_Ring_Rough->slave.u8RcvJoinReq_FromMaster == FALSE)
							&& (pAnother_Ring_Rough->pNodeMe->emRingNodeState == _RingNodeStat_Idle))
						{
							pAnother_Ring_Rough->slave.u8RcvJoinReq_MasterNo = pRing_Rough->master.pSlave_InHandling->u8MasterNodeAddr;
							pAnother_Ring_Rough->slave.u8RcvJoinReq_MasterGunNo = pRing_Rough->pNodeMe->byNodeAddress;
							pAnother_Ring_Rough->slave.u8RcvJoinReq_FromMaster = TRUE;
						}						
					}
					else
					{
						*pDatabuf++ = pRing_Rough->master.pSlave_InHandling->pNode->byNodeAddress;
						*pDatabuf++ = UDP_Ring_Master_Cmd_ReqSlaveJoin + pRing_Rough->master.pSlave_InHandling->u8MasterNodeAddr;
						NodeInfoSendFloat(pDatabuf, 0);   //需求电压
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, 0);  //需求电流（ACDC）
						pDatabuf += 4;
						NodeInfoSendFloat(pDatabuf, 0);  //需求电流（DCDC）
						pDatabuf += 4;
						*pDatabuf++ = 0;		//模块开关控制（ACDC）
						*pDatabuf++ = 0;		//模块开关控制（DCDC）
						*pDatabuf++ = 0;		//环网接触器开关控制
						*pDatabuf++ = 0;		//输出接触器开关控制

						ucSlaveNum++;
					}
				}				

			}// else if(pRing_Rough->emCurrState == RING_STATE_MasterGun_Charging)

			//从机数量
			*pSlaveNumPos = ucSlaveNum;


		} // if(ucNodeStat == _RingNodeStat_MasterGun)
		/* 二.从机 */
		else
		{
			*pDatabuf++ = UDP_Ring_Cmd_Slave;
			*pDatabuf++ = pRing_Rough->pNodeMe->byNodeAddress;
			*pDatabuf++ = NodeStatMapIndex(0, ucNodeStat);
			*pDatabuf++ = pRing_Rough->pNodeMe->emMergeStatus;

			PackNodeInfoToSend(0, pDatabuf, pRing_Rough->pNodeMe, u8GunNo);
			pDatabuf += 32;

			NodeInfoSendFloat(pDatabuf, pRing_Rough->pNodeMe->fkWhReading);
			pDatabuf += 4;

			if((ucNodeStat == _RingNodeStat_Slave)
				|| (ucNodeStat == _RingNodeStat_SubGun))
			{
				*pDatabuf++ = pRing_Rough->pNodeMe->nMasterGunNo;
				*pDatabuf++ = pRing_Rough->pNodeMe->nLockedMasterID;
			}
			else
			{
				*pDatabuf++ = 0;
				*pDatabuf++ = 0;
			}
			
			*pDatabuf++ = pRing_Rough->pNodeMe->u8AppendInfoByte[0];
			*pDatabuf++ = pRing_Rough->pNodeMe->u8AppendInfoByte[1];
			
		}
	}// for(u8GunNo = 0; u8GunNo < 2; u8GunNo++)

	//主桩增加交流接触器控制状态
	if(g_u8IMMU_LCCU_COMM_Addr == 1)
	{
		*pDatabuf++ = g_u8IMMU_LCCU_COMM_Addr;
		*pDatabuf++ = (UCHAR)gEvChargeInfo.u8ACInputContactorCtrlStat;
		*pDatabuf++ = (UCHAR)gEvChargeInfo.u8ACInputContactorCloseAlmDelayFlag;
		*pDatabuf++ = (UCHAR)gEvChargeInfo.u8DCDCPwrSupplyStat;
		*pDatabuf++ = (UCHAR)gEvChargeInfo.u8DCDCPwrOnAlmDelayFlag;
		*pDatabuf++ = (UCHAR)gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_A];
	}
	else
	{
		*pDatabuf++ = g_u8IMMU_LCCU_COMM_Addr;		
		*pDatabuf++ = 0;
		*pDatabuf++ = 0;
		*pDatabuf++ = 0;
		*pDatabuf++ = 0;
		*pDatabuf++ = 0;		
	}	
	//广播当前运行状态 - 从机通知主机需要交流输入,各监控也可以据此决定风扇是否起转
	if ((gEvChargeInfo.u8_A_TestMode == TRUE)
		|| (gEvChargeInfo.u8_B_TestMode == TRUE)
		|| (gEvChargeInfo.u8_Charge_CMD[GUN_NO_A] == START_CHARGE_VAL)
		|| (gEvChargeInfo.u8_Charge_CMD[GUN_NO_B] == START_CHARGE_VAL))
	{
		*pDatabuf++ = 1;
	}
	else if((g_u8IMMU_LCCU_COMM_Addr != 1)
		&&(gs_CANComm.u8IsAllCommFail == TRUE)  
				)
	{
		//从机无正常模块，让主机开启AC接触器
		*pDatabuf++ = 1;
	}
	else
	{
		*pDatabuf++ = 0;
	}

	//广播当前温度
	unTempVal.i16temp = (signed short)gEvChargeInfo.fEV_InterTemp;
	if(gs_CANComm.bRectTemperatureIsValid == 1)
	{
		if(gs_CANComm.fMaxRectTemperature > gEvChargeInfo.fEV_InterTemp)
		{
			unTempVal.i16temp = (signed short)gs_CANComm.fMaxRectTemperature;
		}
	}
	*pDatabuf++ = unTempVal.chtemp[0];
	*pDatabuf++ = unTempVal.chtemp[1];

	//20211118 lkf 增加门禁
	uctemp = 0;
	if(gEvChargeInfo.u8DoorAlm)
	{
		uctemp |= 0x01;
	}
	if(gEvChargeInfo.u8SPDAlm)
	{
		uctemp |= 0x02;
	}
	*pDatabuf++ = uctemp;
		
	u16SendLenth = (UINT16)(pDatabuf - gs_RingExchangeInfo.Sendbuf + 1);
	*pFrameLenPos++ = (UCHAR)(u16SendLenth>>8);
	*pFrameLenPos = (UCHAR)(u16SendLenth);

	uctemp = 0;
	u16temp = u16SendLenth-1;
	for(ii = 0; ii < u16temp; ii++)
	{
		uctemp += gs_RingExchangeInfo.Sendbuf[ii];
	}
	*pDatabuf++ = uctemp;

	gs_RingExchangeInfo.u16SendDataLenth = u16SendLenth;

	MutexUnlock(gs_RingExchangeInfo.hMutex);
}

void UDP_Ring_Info(void)
{
	uip_ipaddr_t addr;
//	UINT16 u16SendLenth, ii, u16temp;

	struct uip_udp_conn udp_conn_trap;
	uip_udp_conn = &udp_conn_trap;	

	uip_sappdata = uip_appdata = &uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN];  //udp数据存放期起始地址
	uip_len = uip_slen = 0;

	uip_udp_conn->lport = HTONS(INFY_CHARGE_SEND_PORT);
	uip_udp_conn->rport = HTONS(INFY_CHARGE_RECV_PORT);
	uip_udp_conn->ttl = UIP_TTL;

	uip_ipaddr(addr, 255,255,255,255);
	uip_ipaddr_copy(uip_udp_conn->ripaddr, &addr);

	if(gs_RingExchangeInfo.u16SendDataLenth == 0)
	{
		return;
	}
		
	MutexLock(gs_RingExchangeInfo.hMutex);
	uip_send(gs_RingExchangeInfo.Sendbuf, gs_RingExchangeInfo.u16SendDataLenth);
	MutexUnlock(gs_RingExchangeInfo.hMutex);
	
	uip_process(UIP_UDP_SEND_CONN);

	if (uip_len > 0)
	{              
		gSelfTest.fSetTaskID(Tasko_ID_ethernet);
		uip_arp_out();              
		reset_netchip_send = 0;				                
		dv_ethdev.fNetWorkDeviceSend();
		gSelfTest.fSetTaskID(Tasko_ID_ethernet);

		OSTimeDly(20);
	}
};

extern struct uip_udp_conn *
uip_udp_listen(uip_ipaddr_t *ripaddr, u16_t rport, u16_t lport);
void Init_udp_exchange_app(void)
{
//2020-02-22 lkf added
#if UDP_EXCHANGE_EN
	uip_ipaddr_t addr;
	struct uip_udp_conn *conn;

	uip_ipaddr(addr, 255,255,255,255);
	conn = uip_udp_listen(&addr, HTONS(INFY_CHARGE_SEND_PORT),  HTONS(INFY_CHARGE_RECV_PORT));
	if(conn != NULL) 
	{
	}
#endif
}



void udp_exchange_appcall(void)
{
	//2020-02-22 lkf added
//#if UDP_EXCHANGE_EN	
	int itemp;
	UCHAR ucCmdType, i, j, ucSlaveNum, uctemp, ucMasterGunNo;
	UINT16 u16RcvDataLen, u16temp, ii;
	uip_ipaddr_t addr;
	unsigned char u8LccuAddr, *pData, *ptemp, *pEnd;
	CAN_RING_DATA* pRing_Rough;	
	union
	{
		short			i16temp;						          /*浮点数*/
		unsigned short	u16temp;
		unsigned char	chtemp[2];					    /*字节型数*/
	}unTempVal;

	//uip_ipaddr(addr, gEvChargeInfo.u8IP_1[0],gEvChargeInfo.u8IP_1[1],gEvChargeInfo.u8IP_1[2],202);
	//uip_ipaddr(addr, 192,168 ,137,7);
	//uip_ipaddr(addr, 192,168 ,100,201);

#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])
	if((UDPBUF->udplen) < (HTONS(36*2+8)))
	{
		//长度不满足
		return;
	}
	u16temp = UDPBUF->udplen - 8;
	//uip_sappdata //数据区
	//if(uip_ipaddr_cmp(BUF->srcipaddr, addr))

	pData = (unsigned char *)uip_sappdata;
	u16RcvDataLen = ((UINT16)pData[0]<<8) + (UINT16)pData[1];
	if((u16temp < u16RcvDataLen)
		|| (u16RcvDataLen < 3))
	{
		return;
	}

	//检查校验和	
	uctemp = 0;
	u16RcvDataLen -= 1;
	ptemp = (unsigned char *)uip_sappdata;
	for(ii = 0; ii < u16RcvDataLen; ii++)
	{
		uctemp += ptemp[ii];
	}
	if(uctemp != ptemp[ii])
	{
		return;
	}

	//信息数据长度,及起始地址
	u16RcvDataLen -= 2;
	pData += 2;	  
	pEnd = pData+u16RcvDataLen;  //终点地址
	for(j = 0; j < 2; j++)
	{
		//每个节点信息总共40个字节: 包括从 命令号 ~ 电表读数 这些范围内的字节
		if((pData + 40) > pEnd)
		{
			return;  //长度不对
		}

		ucCmdType = pData[0];
		if((ucCmdType != UDP_Ring_Cmd_Master)
			&& (ucCmdType != UDP_Ring_Cmd_Slave))
		{
			//命令格式不对
			return;
		}

		pData++;
		if((pData[0] < 1) || (pData[0] > MAX_SUPPORTED_RING_NODE))
		{
			//地址超范围
			return;
		}
			
		if(pData[0] == g_pRing_A->pNodeMe->byNodeAddress)
		{
			//本机？
			return;
		}
		ucMasterGunNo = pData[0];
		u8LccuAddr = pData[0] - 1;
		pData++;
		

		//节点状态
		itemp = NodeStatMapIndex(1, pData[0]);
		if(itemp == -1)
		{
			return;
		}
		g_RingAllInfo.allNodes[u8LccuAddr].emRingNodeState = (tagRingNodeStat_Def)itemp;
		pData++;

		g_RingAllInfo.allNodes[u8LccuAddr].emMergeStatus = (RINGNODE_MERGE_STAT)pData[0];
		pData++;

		g_RingAllInfo.allNodes[u8LccuAddr].bCommFail = (BOOL)0;
		g_RingAllInfo.allNodes[u8LccuAddr].bIsExist = (BOOL)1;
		g_RingAllInfo.allNodes[u8LccuAddr].tmLastRefresh = time_ring(NULL);
		
				
		PackNodeInfoToSend(1, pData, &g_RingAllInfo.allNodes[u8LccuAddr], j);
		pData += 32;

		g_RingAllInfo.allNodes[u8LccuAddr].fkWhReading = NodeInfoGetFloat(pData);
		pData += 4;
		
		//1. 辅助枪或从机发出的报文 
		if(ucCmdType == UDP_Ring_Cmd_Slave)
		{
			//从机命令 信息总共4个字节: 包括从 节点状态 ~附带信息2 这些范围内的字节
			if((pData + 4) > pEnd)
			{				
				return;  //长度不对
			}

			g_RingAllInfo.allNodes[u8LccuAddr].nMasterGunNo = pData[0];
			pData++;
			g_RingAllInfo.allNodes[u8LccuAddr].nLockedMasterID = pData[0];
			pData++;
			g_RingAllInfo.allNodes[u8LccuAddr].u8AppendInfoByte[0] = pData[0];
			pData++;
			g_RingAllInfo.allNodes[u8LccuAddr].u8AppendInfoByte[1] = pData[0];
			pData++;
		}
		//2.由主枪发出的报文
		else  
		{
			//主机命令： 从机数量1个字节
			if((pData + 1) > pEnd)
			{				
				return;  //长度不对
			}
			ucSlaveNum = pData[0];  //从机数量
			pData++;
			if(ucSlaveNum > MAX_RING_SLAVE_NUM)
			{
				return;
			}		
#define SLAVE_CTRL_INFO_FromMaster_Len	18			
			//主机命令：从机x节点地址 ~ 输出接触器开关控制 共18个字节
			if((pData + ucSlaveNum*SLAVE_CTRL_INFO_FromMaster_Len) > pEnd)
			{				
				return;  //长度不对
			}
			ptemp = pData;
			for(i = 0; i < ucSlaveNum; i++)
			{
				//发给A枪或B枪
				if((pData[0] == g_pRing_A->pNodeMe->byNodeAddress)
					|| (pData[0] == g_pRing_B->pNodeMe->byNodeAddress))
				{
					if(pData[0] == g_pRing_A->pNodeMe->byNodeAddress)
						pRing_Rough = g_pRing_A;
					else
						pRing_Rough = g_pRing_B;
					pData++;

					//2.1 本枪还处于空闲态
					if(pRing_Rough->pNodeMe->emRingNodeState == _RingNodeStat_Idle)
					{
						//确认成为辅助枪
						if((pRing_Rough->slave.u8ReqBeSlaveGun_ToMasterGun)
							&& ((pData[0] & 0xf0) == UDP_Ring_Master_Cmd_AckSlaveGunJoin))
						{
							uctemp = pData[0] & 0x0f;
							if((uctemp == pRing_Rough->slave.u8ReqBeWhoseSlaveGun)
								&& (pRing_Rough->slave.u8BeSlaveGunAck_FromMasterGun == FALSE))  //避免同时收到
							{
								pRing_Rough->slave.u8BeSlaveGunAck_FromMasterGun = TRUE;
							}
							pData++;
						}
						//接受成为从机
						else if((pData[0] & 0xf0) == UDP_Ring_Master_Cmd_ReqSlaveJoin)
						{				
							uctemp = pData[0] & 0x0f;
							if(((uctemp > 0) && (uctemp <= MAX_SUPPORTED_RING_NODE))
								&& (pRing_Rough->slave.u8RcvJoinReq_FromMaster == FALSE))  //避免同时收到
							{
								pRing_Rough->slave.u8RcvJoinReq_MasterNo = uctemp;
								pRing_Rough->slave.u8RcvJoinReq_MasterGunNo = ucMasterGunNo;
								pRing_Rough->slave.u8RcvJoinReq_FromMaster = TRUE;
							}
							pData++;
						}
					}  //  2.1

					//2.2 本枪已是辅助枪或从机
					else if(((pRing_Rough->pNodeMe->emRingNodeState == _RingNodeStat_SubGun)
							|| (pRing_Rough->pNodeMe->emRingNodeState == _RingNodeStat_Slave))
						&& (pRing_Rough->slave.pMasterGun->byNodeAddress == ucMasterGunNo))
					{
						pRing_Rough->slave.u8RcvMasterGunCommOvertimeCnt = 0;
						pRing_Rough->slave.tm_LasReceive_Cmd_FromMaster = time_ring(NULL);

						//充电控制命令
						if(pData[0] == UDP_Ring_Master_Cmd_Adj_P)
						{
							pData++;
							pRing_Rough->slave.fReqVolt = NodeInfoGetFloat(pData);
							pData += 4;
							pRing_Rough->slave.fReqCurr_ACDC = NodeInfoGetFloat(pData);
							pData += 4;
							pRing_Rough->slave.fReqCurr_DCDC = NodeInfoGetFloat(pData);
							pData += 4;
							pRing_Rough->slave.emSetPwrOnOff_ACDC = pData[0];
							pData++;
							pRing_Rough->slave.emSetPwrOnOff_DCDC = pData[0];
							pData++;
							pRing_Rough->slave.emSetRingRelayOnOff = pData[0];
							pData++;
							pRing_Rough->slave.emSetOutRelayOnOff = pData[0];
							pData++;
							pRing_Rough->slave.bReceivedAdj_FromMasterGun = TRUE;
						}
						//主枪通知退出
						else if(pData[0] == UDP_Ring_Master_Cmd_ReqQuit)
						{
							pData++;
							pRing_Rough->slave.fReqVolt = 0;
							pRing_Rough->slave.fReqCurr_ACDC = 0;
							pRing_Rough->slave.fReqCurr_DCDC = 0;
							pRing_Rough->slave.emSetPwrOnOff_ACDC = 0;
							pRing_Rough->slave.emSetPwrOnOff_DCDC = 0;
							pRing_Rough->slave.u8InfoQuitRing_FromMasterGun = TRUE;
						}						
					} // 2.2本枪已是辅助枪或从机
					//2.3 其它状态
					else  
					{
						
					}
				}	// 发给本桩的 
					// if((pData[0] == g_pRing_A->pNodeMe->byNodeAddress)
					//   || (pData[0] == g_pRing_B->pNodeMe->byNodeAddress))

				//不是发给本桩的
				else
				{					
				}

				ptemp += SLAVE_CTRL_INFO_FromMaster_Len;
				pData = ptemp;
			} // for(i = 0; i < ucSlaveNum; i++)
		}  // 2.由主枪发出的报文		
	}	// for(j = 0; j < 2; j++)

	//检查是否有桩下发的其它数据
	if((pData + 10) > pEnd)
	{				
		return;  //长度不对
	}	

	if(pData[0] == 1)
	{
		gEvChargeInfo.u8ACInputContactorCtrlStat_FromMaster = pData[1];
		gEvChargeInfo.u8ACInputContactorCloseAlmDelayFlag_FromMaster = pData[2];
		gEvChargeInfo.u8DCDCPwrSupplyStat_FromMaster = pData[3];
		gEvChargeInfo.u8DCDCPwrOnAlmDelayFlag_FromMaster = pData[4];
		gs_IMMU2_LCCU_Mng.u8EsBattTaskType[GUN_NO_A] = pData[5];
		gs_LccuExchangeInfo[0].u8RunStatForACContactor =  pData[6];
		
		unTempVal.chtemp[0] = pData[7];
		unTempVal.chtemp[1] = pData[8];
		gs_LccuExchangeInfo[0].i16Temparature = unTempVal.i16temp;

		gs_LccuExchangeInfo[0].u8DoorOpenAlm = pData[9];

		gs_LccuExchangeInfo[0].u8CommAlmStat = 0;
		gs_LccuExchangeInfo[0].u8CommAlmDelay = 0;
	}
	else if((pData[0] > 1) && (pData[0] <= LCCU_NUMBER_RING))
	{
		gs_LccuExchangeInfo[pData[0]-1].u8RunStatForACContactor =  pData[6];	

		unTempVal.chtemp[0] = pData[7];
		unTempVal.chtemp[1] = pData[8];
		gs_LccuExchangeInfo[pData[0]-1].i16Temparature = unTempVal.i16temp;

		gs_LccuExchangeInfo[pData[0]-1].u8CommAlmStat = 0;
		gs_LccuExchangeInfo[pData[0]-1].u8CommAlmDelay = 0;

		gs_LccuExchangeInfo[pData[0]-1].u8DoorOpenAlm = pData[9];

	}

	
	
		

//#endif
}
/************************************************/
/*软件名称:TaskofEthernet        				        */
/*软件功能:网络通讯          					          */
/*软件版本:1.0									                */
/*版本升级:										                  */
/*适用系统:           							            */
/*入口参数:										                  */
/*出口参数:										                  */
/*资源:   	               						          */
/*子函数:              							            */
/*设计日期:										                  */
/*设计人员: 									                  */
/*更改日期:			      						              */
/*更改人员: 									                  */
/************************************************/
static unsigned char s_u8UpdateStatBak = 0xff;
void TASK_Ethernet (void* pdata)
{

	OSTimeDly(100);
	UINT8 u8ItvTime = 0;
	UINT8 uctemp;
	

	netInitMac();
	CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);			//无数据可发
	CLR_CONN_STAT(gNET_TCP_Info.u8MachineStat);			//无链接！
	NET_Clean_RBuf();
	// init the network
	netInitMac();
	dv_ethdev.fInitNetPort();
	uip_init();	
	Init_udp_exchange_app();
	uip_arp_init();
	resolv_init();

	ethInit();
	uip_router_init();
	timer_set(&periodic_timer, (300/ 2));
	timer_set(&arp_timer, ((CLOCK_SECOND * OS_TICKS_PER_SEC) * 10));

	OSTimeDly(50);

	for(;;)
	{
		gSelfTest.fSetTaskID(Tasko_ID_ethernet);

		if (gEvChargeInfo.u8NET_Mask == TRUE)
		{
			OSTimeDly(300);
			gNET_TCP_Info.iNetCreateSocketFlag = TRUE;
			gEvChargeInfo.i32APP_PipeWTDOG = 0;
			CLR_CONN_STAT(gNET_TCP_Info.u8MachineStat);
			CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);
			
			continue;
		}

		//20210709 升级固件程序期间提升回包速度
		uctemp = gs_UpGradeCtrl.u8UpgradeStat;
		if(s_u8UpdateStatBak != uctemp)
		{
			s_u8UpdateStatBak = uctemp;
			if(s_u8UpdateStatBak == UpgradeStatDef_Start)
			{
				timer_set(&periodic_timer, (PERIODIC_TIMER_UPGRADE));		//*2ms  = 200ms
			}
			else
			{
				//timer_set(&periodic_timer, ((CLOCK_SECOND * OS_TICKS_PER_SEC) / 2));
				timer_set(&periodic_timer, (300/ 2));
			}
		}

		if(gMeasureInfo.ucAddressDelay <5)
		{
			OSTimeDly(100);
			continue;
		}

		netIP_Deal();

		net_poll();
		
		//u8ItvTime++;
		//if (u8ItvTime > 254)
		//{
		//	u8ItvTime = 0;
		//}
		//if (u8ItvTime%10==0)
		
			OSTimeDly(20);			// 100mS

		
	
//2020-02-22 lkf added
#if UDP_EXCHANGE_EN	
		/*u8ItvTime++;
		if (u8ItvTime > 9)
		{
			u8ItvTime = 0;
			//UDP_Exchange_Info();
			UDP_Ring_Info();
		}	*/
		if(gs_RingExchangeInfo.u16SendDelayCnt == 0)
		{
			UDP_Ring_Info();
			gs_RingExchangeInfo.u16SendDelayCnt = RING_EXCHANGE_UDP_PERIOD;
		}
#endif

		gSelfTest.fSetTaskID(Tasko_ID_ethernet);
	}
}
