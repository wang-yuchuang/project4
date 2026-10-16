#include "uip.h"
#include "resolv.h"
#include "uip_app_router.h"

#include "main.h"

extern SNMPTRAPExchangeInterface_TypeDef   gs_trap;
extern uip_ipaddr_t eem_ipaddr[2];

struct tcp_CLIENT_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 
typedef struct tcp_CLIENT_appstate __uip_tcp_appstate_t;

//u16_t u16lport;  //2.01T7 网络Alarm Report支持multiple sequences
//u16_t u16rport;  //Spain 远端端口
//u16_t u16lport_link;  
//u16_t u16rport_link;

void uip_router_udp_appcall(void)
{
#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])
	//switch(UDPBUF->destport) 
	if(uip_udp_conn->rport == HTONS(DNS_SERVER_PORT))
	{
		UIP_UDP_APPCALL_DNS();
	}
	else
	{
		switch (uip_udp_conn->lport)
		{
			case HTONS(DHCPC_CLIENT_PORT):
			{
			   extern void dhcpc_appcall(void);
			   dhcpc_appcall();
			}
			break;
			case HTONS(DHCPC_SERVER_PORT):
				{
					extern void dhcpc_appcall(void);
					dhcpc_appcall();
				}
				break;
			case HTONS(INFY_CHARGE_RECV_PORT):
				{
					extern void udp_exchange_appcall(void);
					udp_exchange_appcall();
				}
				break;
		}
	}
}

void uip_router_init(void){

  //httpd_init();
  eem_server_init();
  //extern void sninit(void);
  //sninit();
}

void uip_router_tcp_appcall(void)
{
   switch(uip_conn->lport) 
   {
   case HTONS(WEB_SERVER_PORT):
	   //httpd_appcall();
	   break;

   //case HTONS(EEM_SERVER_PORT+1):
   }
   //switch(uip_conn->rport)//远程连接9444端口
   {
	//case HTONS(ZHIXIN_CLIENT_PORT):
	if (uip_conn->rport == HTONS(ZHIXIN_CLIENT_PORT))
	{
		Net_Client_appcall();
	}
	//break;
   }
}

void eem_server_init(void)
{
  //uip_listen(HTONS(EEM_SERVER_PORT));
  uip_listen(HTONS(ZHIXIN_CLIENT_PORT));
}

extern void *uip_appdata;
extern void *uip_sappdata; 
extern u16_t uip_len, uip_slen;

void tcp_client_reconnect(void);
void tcp_client_aborted(void);
void tcp_client_timedout(void);
void tcp_client_closed(void);
void tcp_client_connected(void);
void tcp_client_acked(void);
void tcp_client_senddata(void);
INT8 u8GetSendData(void);


INT8 u8GetSendData(void)
{
	int iRst = -1;
	UINT8 i;
	UINT16 u16SendNo;
	UINT8 u8SendBufNo;
	UINT16 u16Idx = 0;
	__uip_tcp_appstate_t *s = (__uip_tcp_appstate_t *)&uip_conn->appstate;

	MutexLock(gEvChargeInfo.hMetexNet_W);
	u8SendBufNo = 0xff;
	u16SendNo = 0;
	for(i = 0; i < NET_S_NUM; i++)
	{
		if (TRUE == gNET_TCP_Info.u8SendBuf[i].u8DataFlag)
		{
			if(u8SendBufNo == 0xff)
			{
				u8SendBufNo = i;
				u16SendNo = gNET_TCP_Info.u8SendBuf[i].u16SendNoBak;
			}
			else if(gNET_TCP_Info.u8SendBuf[i].u16SendNoBak < u16SendNo)
			{
				if((u16SendNo != 0xffff) && (gNET_TCP_Info.u8SendBuf[i].u16SendNoBak != 0))
				{
					u8SendBufNo = i;
					u16SendNo = gNET_TCP_Info.u8SendBuf[i].u16SendNoBak;
				}
			}
		}
	}

	if(u8SendBufNo < NET_S_NUM)
	{	
		if (TRUE == gNET_TCP_Info.u8SendBuf[u8SendBufNo].u8DataFlag)
		{
			gNET_TCP_Info.u8SendBuf[u8SendBufNo].u8DataFlag = FALSE;
			memcpy(gDriveBuf.u8Buf, gNET_TCP_Info.u8SendBuf[u8SendBufNo].u8Buf, gNET_TCP_Info.u8SendBuf[u8SendBufNo].u16Lengh);
			gDriveBuf.u16Lengh = gNET_TCP_Info.u8SendBuf[u8SendBufNo].u16Lengh;
			s->textptr = gDriveBuf.u8Buf;
			s->textlen = gDriveBuf.u16Lengh;		//strlen((const char*)gNET_TCP_Info.u8SendBuf.u8Buf);
			MutexUnlock(gEvChargeInfo.hMetexNet_W);
			iRst = u8SendBufNo;
			return iRst;
		}
	}
	MutexUnlock(gEvChargeInfo.hMetexNet_W);
	return iRst;
}
/*---------------------------------------------------------------------------*/
void Net_Client_appcall(void)
{
	//UINT16 i = 0;
	UINT16 u16LVal = 0;
	UINT16 u16Idx = 0;
	//__uip_tcp_appstate_t *s = (__uip_tcp_appstate_t *)&uip_conn->appstate;
	//struct httpd_state *s = (struct httpd_state *)&(uip_conn->appstate);
	if(uip_aborted())
	{
		tcp_client_aborted();						//连接终止		标记未连接 然后重连！
		return;
	}
	if(uip_timedout())
	{
		tcp_client_timedout();						//连接超时		标记未连接 然后重连	
		return;
	}
	if(uip_closed())
	{
		tcp_client_closed();						//连接关闭		标记未连接 然后重连	
		return;
	}
	if(uip_connected())
	{
		tcp_client_connected();						//连接成功	
		CLR_SENDING_STAT(gNET_TCP_Info.u8MachineStat);			//清除发送状态！！ 停止计时
		SET_SENDING_TIMEOUT(gNET_TCP_Info.iSendTimeOUT, 800);		//5S超时！
	}
	
	if(uip_acked())
	{
		tcp_client_acked();						//发送的数据成功送达
		CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);			//清除标记！
		CLR_SENDING_STAT(gNET_TCP_Info.u8MachineStat);			//清除发送状态！！ 停止计时
		SET_SENDING_TIMEOUT(gNET_TCP_Info.iSendTimeOUT, 800);		//5S超时！
	}
	static UINT8 su16RIdx = 0;
	UINT16 u16BufIdx = 0;
	if (uip_newdata())
	{
		u16LVal = uip_len;
		if(uip_len > NET_BUF_DATA_L - 1)
		{
			u16LVal = NET_BUF_DATA_L - 1;
		}
		if(u16LVal > 0)
		{
			u16BufIdx = (su16RIdx)%NET_R_NUM;
			su16RIdx++;				//if NET_R_NUM == 4  Eg:0-->1-->2-->3-->0-->1-->2-->3.....
			if (su16RIdx == NET_R_NUM)
			{
				su16RIdx = 0;
			}

			MutexLock(gEvChargeInfo.hMetexNet_R);
			memcpy(gNET_TCP_Info.u8ReadBuf[u16BufIdx].u8Buf, uip_appdata, u16LVal);
			gNET_TCP_Info.u8ReadBuf[u16BufIdx].u16Lengh = u16LVal;
			gNET_TCP_Info.u8ReadBuf[u16BufIdx].u8DataFlag|=0x01;
			MutexUnlock(gEvChargeInfo.hMetexNet_R);
		}
	}
	//else if ((uip_poll() || uip_rexmit()))		//有数据需要发送！
	else if ((CHK_SEND_STAT(gNET_TCP_Info.u8MachineStat))&&(uip_poll() || uip_rexmit()))		//有数据需要发送！
	{
		//if (!CHK_SENDING_STAT(gNET_TCP_Info.u8MachineStat))//没有正在发送中！
		{
			for (u16Idx = 0; u16Idx < 5; u16Idx++)
			{
				if (u8GetSendData() >= 0)
				{
					SET_SENDING_STAT(gNET_TCP_Info.u8MachineStat);				//置为发送状态
					SET_SENDING_TIMEOUT(gNET_TCP_Info.iSendTimeOUT, 800);			//5S超时
					break;
				}
				OSTimeDly(2);
			}
		}
		//TimeOUT!!  没有数据将不发送
		tcp_client_senddata();
	}

	//当需要重发、新数据到达、数据包送达、连接建立时，通知uip发送数据 
	//if(uip_rexmit()||uip_newdata()||uip_connected()||uip_poll())//||uip_acked()
	//{
	//	tcp_client_senddata();
	//}	
}


void tcp_client_reconnect(void)
{
  __XXX_UNION_VALUE unVal;
	uip_ipaddr_t ipaddr;
	UINT8 p = 0;
        UINT8 ip[8];
	for(p=0;p<4;p++)					//get the  gateway
	{
		//GetCharSetData(SETDATA_PowerStarIP_NUM+p,&ip[p]);
                ExGetSigInfo(E_TYPE_SYS, ID_u8PowerStarIP_1 + p, &unVal, NULL, BY_SRAM);
                ip[p]= unVal.chtemp[0]; 
	}
	uip_ipaddr(&ipaddr,ip[0],ip[1],ip[2],ip[3]);			//设置IP为
	uip_connect(&ipaddr,htons(ZHIXIN_CLIENT_PORT)); 		//端口为
        
}
void tcp_client_reconnect_domainname(void)
{
	uip_connect(&gs_RemoteServerIp, htons(ZHIXIN_CLIENT_PORT)); 		//端口为
}

void ask_dns_server_get_ip(void)
{
	__XXX_UNION_VALUE unVal;
	uip_ipaddr_t ipaddr;
	char cServerName[33];
	UINT8 p = 0;
	UINT8 ip[8];
	for(p=0;p<4;p++)					//get the  gateway
	{
		//GetCharSetData(SETDATA_DNSServerIP_NUM+p,&ip[p]);
		 ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_DNSServerIP_NUM_1 + p, &unVal, NULL, BY_SRAM);
		 ip[p]= unVal.chtemp[0]; 
	}
	uip_ipaddr(&ipaddr,ip[0],ip[1],ip[2],ip[3]);			//设置IP为
	resolv_conf(&ipaddr); 		//端口为

	memset(cServerName, 0, sizeof(cServerName));
	for(p=0;p<32;p++)					
	{
		//GetCharSetData(SETDATA_RemoteServerDomainName_NUM+p,&cServerName[p]);
		 ExGetSigInfo(E_TYPE_SYS, ID_SETDATA_RemoteServerDomainName_NUM_1 + p, &unVal, NULL, BY_SRAM);
		 cServerName[p] = unVal.chtemp[0]; 
		 if(cServerName[p] == 0)
		 {
			 break;
		 }
	}

	resolv_query(cServerName);
}
//终止连接处理-----重连				    
void tcp_client_aborted(void)
{
	CLR_CONN_STAT(gNET_TCP_Info.u8MachineStat);		//标志没有连接
	CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);	
}


//连接超时处理----标记没有链接！
void tcp_client_timedout(void)
{
	CLR_CONN_STAT(gNET_TCP_Info.u8MachineStat);		//标志没有连接	  
	CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);
}

//连接关闭---重连！
void tcp_client_closed(void)
{
	CLR_CONN_STAT(gNET_TCP_Info.u8MachineStat);		//标志没有连接
	CLR_SEND_STAT(gNET_TCP_Info.u8MachineStat);
}

//连接建立---记录标记
void tcp_client_connected(void)
{
	SET_CONN_STAT(gNET_TCP_Info.u8MachineStat);				//标志连接成功
}
//发送的数据成功送达
void tcp_client_acked(void)
{			
	SET_CONN_STAT(gNET_TCP_Info.u8MachineStat);	
	__uip_tcp_appstate_t *s=(__uip_tcp_appstate_t *)&uip_conn->appstate;
	s->textlen=0;//发送清零
}
//发送数据给服务端
void tcp_client_senddata(void)
{
	__uip_tcp_appstate_t *s = (__uip_tcp_appstate_t *)&uip_conn->appstate;
	if(s->textlen>0)uip_send(s->textptr, s->textlen);//发送TCP数据包	 
}
