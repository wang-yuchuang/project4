#include "main.h"
#include "can_ring.h"
#include "Uart6_new.h"

RING_SALVE	s_RingSlavesBuf[2][MAX_RING_SLAVE_NUM];	//存放动态产生的从机信息

#define R_PRPTO_DEVNO_0C		0x0C //for ring
#define R_PROTO_RING_BROADCAST	0x1F

CanRxMsgTypeDef RxMsg_Ring[CAN_3_MAX_BUFF];

RING_ALL_INFO			g_RingAllInfo; 

CAN_RING_DATA*			g_pRing_A;
CAN_RING_DATA*			g_pRing_B;

Lccu_Exchange_Info_Def gs_LccuExchangeInfo[LCCU_NUMBER_RING] = {0};


NOINIT RingExchangeInfo_Def gs_RingExchangeInfo;

#ifdef	Debug_Print_En    
unsigned char g_u8AfterAddDecSlavePrintCurrrentChangeDelay = 0;
#endif
/************************************************/
//函数名称:										
//函数功能:										
//更改日期:      								
/************************************************/
time_t_ring time_ring(unsigned char ucflag)
{
	return (time_t_ring)g_RingAllInfo.u32RingTimeSec;
}
