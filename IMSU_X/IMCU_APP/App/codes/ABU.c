/*******************************************************************************
 *  Includes
 */
#include "ABU.h"

/*******************************************************************************
 *  Constants
 */
#define R_PROTO_SELF_ADDR		gs_CANComm.bySelfAddress
#define R_PRPTO_DEVNO_0A	0x0A
#define R_PRPTO_DEVNO_0B	0x0B


/*******************************************************************************
 *  Typedef
 */

/*******************************************************************************
 *  Macros
 */

/*******************************************************************************
 *  Global Variables
 */
ABU_t ABU_info;
ABU_t *ABU = &ABU_info;

/*******************************************************************************
 *  External Variables
 */
extern CanRxMsgTypeDef RxMsg[CAN_3_MAX_BUFF];

/*******************************************************************************
 *  External Functions
 */
extern void ClenRBuff(void);
extern void DealRawCanMsgToIMMU2(canmsg * pRcvMsg, UINT8 u8GSNo, UINT8 IsSend);

/*******************************************************************************
 *  Local Variables
 */


/*******************************************************************************
 *  Public Functions Declaration
 */


/*******************************************************************************
 *  Local Functions Declaration
 */
void ABU_ReadOutputVolt_0x23(UINT8 ch);
void ABU_ReadOutputCurr_0x23(void);
void ABU_ReadACVoltage_0x23(UINT8 ch);
void ABU_ReadACCurrent_0x23(UINT8 ch);
void ABU_ReadACFrequency_0x23(void);
void ABU_ReadActivePower_0x23(UINT8 ch);
void ABU_ReadReactivePower_0x23(UINT8 ch);
VOID ABU_ReadPowerFactor_0x23(UINT8 ch);
void ABU_Read_status(UINT8 Byte7, UINT8 Byte6);
void ABU_Read_alm(UINT8 Byte7, UINT8 Byte6);
void ABU_ReadPowerTemperature_0x23(void);
void PowerModule_GetRealBarCode(coTime BarCode, UINT32 *DataBuf);
void ABU_state_ALM_init(void);


/*******************************************************************************
 *  Local Functions Definition
 * 
 */

//by lxr01202   新增ABU状态和告警初始化
void ABU_state_ALM_init(void)
{
//1112 
	ABU->ContravariantStat2.Data = 0x00;
	ABU->ContravariantStat3.Data = 0x00;
//1113
	ABU->ContravariantAlm0.Data = 0x00;
	ABU->ContravariantAlm1.Data = 0x00;
	ABU->ContravariantAlm2.Data = 0x00;
	ABU->ContravariantAlm3.Data = 0x00;
//1114
	ABU->ContravariantAlm4.Data = 0x00;
	ABU->ContravariantAlm5.Data = 0x00;
	ABU->ContravariantAlm6.Data = 0x00;
	ABU->ContravariantAlm7.Data = 0x00;
//1115
	ABU->_0x1115Byte0_info.Data = 0x00;
	ABU->_0x1115Byte1_info.Data = 0x00;
	ABU->_0x1115Byte2_info.Data = 0x00;
	ABU->_0x1115Byte3_info.Data = 0x00;
}

void ABU_Unpack_0x23(INT8U i)
{
	INT8U uIdx = 0u;
	co_dword dword;
	INT8U Ptr[8] = {0};

	for (uIdx = 0u; uIdx < 8; uIdx++)
	{
		Ptr[uIdx] = (INT8U)RxMsg[i].Data[uIdx];
	}
	
	if (Ptr[0] == 0x11)                    //byte0   0x11
	{
		switch (Ptr[1])
		{
			case 0x01:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x11_info.PowerModule_DCVolt = dword.v;              //模块直流侧电压
				}
				break;

			case 0x02:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x11_info.PowerModule_DCCurr = dword.l;           //模块直流侧电流
				}
				break;

			case 0x06:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x11_info.PowerModule_AmbientTemp = dword.l;          //模块环境温度
				}
				break;

			case 0x10:                                              //0x23  0x11  0x10  模块状态2
				{
			



						ABU->Status3.Data = Ptr[4];            //当前串并联模式设置  1:自动切换 2：手动设置并联模式 3：手动设置串联模式
						ABU->Status4.Data = Ptr[5];           //0：限功率状态 1：ID冲突 3：DC侧系统不均流 4：AC缺相 5：AC电压不平衡 6：AC欠压 7：AC过压 8：AC关机1/开机0
						ABU->Status5.Data = Ptr[6];                     
						ABU->Status6.Data = Ptr[7];                     

				 }
				break;

			case 0x11:

						ABU->Status7.Data = Ptr[5];                     
						ABU->Status8.Data = Ptr[6];                    
						ABU->Status9.Data = Ptr[7];                     
            
				break;

			case 0x16:                                                      //模块逆变状态	
				{
					ABU->ContravariantStat2.Data = Ptr[5];
					ABU->ContravariantStat1.Data = Ptr[6];
					ABU->ContravariantStat0.Data = Ptr[7];
				}
				break;

			case 0x17:
				{
					ABU->ContravariantAlm3.Data = Ptr[4];                        //模块3逆变报警
					ABU->ContravariantAlm2.Data = Ptr[5];						//模块2逆变报警
					ABU->ContravariantAlm1.Data = Ptr[6];
					ABU->ContravariantAlm0.Data = Ptr[7];
				}
				break;

			case 0x18:
				{
					ABU->ContravariantAlm7.Data = Ptr[4];
					ABU->ContravariantAlm6.Data = Ptr[5];
					ABU->ContravariantAlm5.Data = Ptr[6];
					ABU->ContravariantAlm4.Data = Ptr[7];
				}
				break;

			case 0x19:
				{
					ABU->_0x1115Byte2_info.Data = Ptr[5];
					ABU->_0x1115Byte1_info.Data = Ptr[6];
					ABU->_0x1115Byte0_info.Data = Ptr[7];					
				}
				break;

			case 0x22:
				{
					dword.v = 0u;
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->DC_Version = dword.w[0];
					ABU->IsGetABUVersion |= 0x01;
				}
				break;

			case 0x23:
				{
					dword.v = 0u;
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->PFC_Version = dword.w[0];
					ABU->IsGetABUVersion |= 0x02;
				}
				break;
		}
	}
	else if (Ptr[0] == 0x21)
	{
		switch (Ptr[1])
		{
			case 0x01:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.AC_VoltAB = dword.v;                                  //交流A相电压
				}
				break;

			case 0x02:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.AC_VoltBC = dword.v;
				}
				break;

			case 0x03:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.AC_VoltCA = dword.v;
				}
				break;

			case 0x04:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.AC_CurrAB = dword.l;
				}
				break;

			case 0x05:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.AC_CurrBC = dword.l;
				}
				break;

			case 0x06:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.AC_CurrCA = dword.l;
				}
				break;

			case 0x07:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.AC_Freq = dword.l;                      //交流频率
				}
				break;

			case 0x08:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.TotalActivePower = dword.l;
				}
				break;

			case 0x09:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.ActivePowerAB = dword.l;
				}
				break;

			case 0x0A:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.ActivePowerBC = dword.l;
				}
				break;

			case 0x0B:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.ActivePowerCA = dword.l;
				}
				break;

			case 0x0C:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.TotalReactivePower = dword.l;
				}
				break;

			case 0x0D:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.ReactivePowerAB = dword.l;
				}
				break;

			case 0x0E:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.ReactivePowerBC = dword.l;
				}
				break;

			case 0x0F:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.ReactivePowerCA = dword.l;
				}
				break;

			case 0x14:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.TotalPowerFactor = dword.v;
				}
				break;

			case 0x15:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.PowerFactorAB = dword.v;
				}
				break;

			case 0x16:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.PowerFactorBC = dword.v;
				}
				break;

			case 0x17:
				{
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x21_info.PowerFactorCA = dword.v;
				}
				break;
		}
	}
	else if (Ptr[0] == 0x31)
	{
		switch (Ptr[1])
		{
			case 0x12:
				{
					dword.v = 0u;
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x31_info.OutputVoltAB = dword.v;
				}
				break;

			case 0x13:
				{
					dword.v = 0u;
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x31_info.OutputVoltBC = dword.v;
				}
				break;

			case 0x14:
				{
					dword.v = 0u;
					dword.b[3] = Ptr[4];
					dword.b[2] = Ptr[5];
					dword.b[1] = Ptr[6];
					dword.b[0] = Ptr[7];
					ABU->ABU_0x31_info.OutputVoltCA = dword.v;
				}
				break;
		}
	}
}


void ABU_CommFailed(void)
{
	ABU->CommFailed = !FALSE;
	ABU->CommFailedLast = !FALSE;
	ABU->IsGetABUVersion = FALSE;
	ABU->IsGetBarCode = FALSE;
	ABU->UnrecvTimes = 0u;
}

void PowerModule_GetRealBarCode(coTime BarCode, UINT32 *DataBuf)
{
	UINT8 i = 0u;
	UINT16 SoftWare_Version = 0u;

	SoftWare_Version = ((UINT8)DataBuf[6] << 8) + (UINT8)DataBuf[7];
	ABU->BarCodeString[11] = BarCode % 10 + '0';
	
	for (i = 1u; i < 12u; i++)
	{
		BarCode /= 10u;	
		ABU->BarCodeString[11-i] = (BarCode % 10) + '0';
	}

	ABU->BarCodeString[i++] = (UINT8)DataBuf[0];
	ABU->BarCodeString[i++] = SoftWare_Version / 1000 + '0';
	ABU->BarCodeString[i++] = ((SoftWare_Version / 100) % 10) + '0';
	ABU->BarCodeString[i++] = ((SoftWare_Version / 10)  % 10) + '0';
	ABU->BarCodeString[i++] = (SoftWare_Version 				 % 10) + '0';
}


void ABUUnPackData(UINT8 u8RcvMsgNum)                     //处理接收到的CAN总线消息，并将其解包后发送给IMMU2
{
	UINT8 i = 0u;
	UINT8 u8RcvCmd, u8RcvDstAddr, u8RcvSrcAddr;
	union
	{
		unsigned char uctemp[4];
		UINT32 u32temp;
	}uniontemp;
	
	if(u8RcvMsgNum > CAN_3_MAX_BUFF)
	{
		return;
	}
	
	for (i = 0u; i < u8RcvMsgNum; i++)                                //使用一个for循环遍历所有接收到的消息
	{
		//u8RcvErrCode = ((RxMsg[i].ExtId>>26)&0x07);	
		u8RcvCmd = ((RxMsg[i].ExtId>>16)&0x3f);	                        //对于每条消息，从扩展ID（ExtId）中提取命令、目标地址和源地址等信息
		//u8RcvDevNo = ((RxMsg[i].ExtId>>22)&0x0f);
		u8RcvDstAddr = ((RxMsg[i].ExtId>>8)&0xff);
		u8RcvSrcAddr = (RxMsg[i].ExtId & 0xff);

		//Deal raw CAN to IMMU2
		canmsg SendMsg;                                            //创建一个新的canmsg结构体SendMsg，并将相关信息填充进去
		SendMsg.Id = RxMsg[i].ExtId;
		uniontemp.uctemp[0] = RxMsg[i].Data[0];                    
		uniontemp.uctemp[1] = RxMsg[i].Data[1];
		uniontemp.uctemp[2] = RxMsg[i].Data[2];
		uniontemp.uctemp[3] = RxMsg[i].Data[3];
		SendMsg.Data[0] = uniontemp.u32temp;                    //使用联合体uniontemp将接收到的数据字节重新组合成32位整数，并存储到SendMsg.Data[0]中
		uniontemp.uctemp[0] = RxMsg[i].Data[4];
		uniontemp.uctemp[1] = RxMsg[i].Data[5];
		uniontemp.uctemp[2] = RxMsg[i].Data[6];
		uniontemp.uctemp[3] = RxMsg[i].Data[7];
		SendMsg.Data[1] = uniontemp.u32temp;                    //使用联合体uniontemp将接收到的数据字节重新组合成32位整数，并存储到SendMsg.Data[1]中
		DealRawCanMsgToIMMU2(&SendMsg, 0x0C, FALSE);

		if ((u8RcvDstAddr != R_PROTO_SELF_ADDR) || (u8RcvSrcAddr != ABU_Addr))
		{
			continue;
		}

		ABU->UnrecvTimes = 0u;
		ABU->CommFailed = FALSE;

		switch (u8RcvCmd)
		{
			case 0x0B:
				{
					ABU->BarCode = ((coTime)RxMsg[i].Data[1] << 32) | ((coTime)RxMsg[i].Data[2] << 24) | ((coTime)RxMsg[i].Data[3] << 16) | ((coTime)RxMsg[i].Data[4] << 8) | (coTime)RxMsg[i].Data[5];
					PowerModule_GetRealBarCode(ABU->BarCode, (UINT32 *)RxMsg[i].Data);
					ABU->IsGetBarCode = !FALSE;
				}
				break;
			
			case 0x23:
				{
					ABU_Unpack_0x23(i);
				}
				break;
		}
	}
}

void ABU_PackSendMsg(UINT8 u8DevNo, UINT8 u8Cmd, UINT32 Dh, UINT32 Dl)
{
	canmsg SendMsg;
	SendMsg.Id = (u8DevNo << 22) + (u8Cmd << 16) + (ABU_Addr << 8) + R_PROTO_SELF_ADDR;

	SendMsg.Data[0] = Dh;
	SendMsg.Data[1] = Dl;
	gSelfTest.fSetTaskID(TASK_ID_CAN3);
	gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	ClenRBuff();
	gs_CANComm.fTxCAN3Buff(&SendMsg, 8);

	//Deal raw CAN to IMMU2
	DealRawCanMsgToIMMU2(&SendMsg, 0x0C, TRUE);
}

/*********************************************************************************************
 * @brief 读取 ABU 软件版本信息
 * @details 通过 CAN 总线向 ABU 设备发送读取软件版本请求，并等待响应
 * 
 * @param ch 通道号，0 或 1，用于选择不同的 ABU 设备
 * 
 * @note 函数会等待 CFG_ABU_RESPONSE_TIME 时间以接收响应
 * @note 如果接收到响应消息，将调用 ABUUnPackData 解析数据
 *********************************************************************************************/
void ABU_ReadSoftWareVersion(INT8U ch)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	dword.b[3] = 0x11;
	if (ch == 0u)
	{
		dword.b[2] = 0x22;
	}
	else if (ch == 1u)
	{
		dword.b[2] = 0x23;
	}
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_ReadBarCode_0x0B(void)
{
	UINT8 u8RcvMsgNum = 0u;

	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x0B, 0, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}


void ABU_ReadPowerTemperature_0x23(void)     //0x23  0x11  0x06
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	dword.b[3] = 0x11;
	dword.b[2] = 0x06;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}


void ABU_ReadPowerModuleStatus_0x23(void)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	dword.b[3] = 0x11;
	dword.b[2] = 0x10;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}



void ABU_ReadOutputVolt_0x23(UINT8 ch)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	dword.b[3] = 0x31;
	if (ch == eType_AB)
	{
		dword.b[2] = 0x12;
	}
	else if (ch == eType_BC)
	{
		dword.b[2] = 0x13;
	}
	else if (ch == eType_CA)
	{
		dword.b[2] = 0x14;
	}
	
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_ReadOutputCurr_0x23(void)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	dword.b[3] = 0x11;
	dword.b[2] = 0x02;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);
	}
}

void ABU_ReadACVoltage_0x23(UINT8 ch)               //0x23  0x21  0x01  02  03   交流A相相电压  B相相电压  C相相电压
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	if (ch == eType_AB)
	{
		dword.b[2] = 0x01;
	}
	else if (ch == eType_BC)
	{
		dword.b[2] = 0x02;
	}
	else if (ch == eType_CA)
	{
		dword.b[2] = 0x03;
	}
	else
	{
		return;
	}
	
	dword.b[3] = 0x21;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_ReadACCurrent_0x23(UINT8 ch)             //0x23 0x21  0x04   交流A相电流  交流B相电流  交流C相电流
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	if (ch == eType_AB)
	{
		dword.b[2] = 0x04;
	}
	else if (ch == eType_BC)
	{
		dword.b[2] = 0x05;
	}
	else if (ch == eType_CA)
	{
		dword.b[2] = 0x06;
	}
	else
	{
		return;
	}
	
	dword.b[3] = 0x21;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_ReadACFrequency_0x23(void)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	dword.b[3] = 0x21;
	dword.b[2] = 0x07;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_ReadActivePower_0x23(UINT8 ch)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	if (ch == eType_Total)
	{
		dword.b[2] = 0x08;
	}
	else if (ch == eType_AB)
	{
		dword.b[2] = 0x09;
	}
	else if (ch == eType_BC)
	{
		dword.b[2] = 0x0A;
	}
	else if (ch == eType_CA)
	{
		dword.b[2] = 0x0B;
	}
	else
	{
		return;
	}
	
	dword.b[3] = 0x21;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_ReadReactivePower_0x23(UINT8 ch)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	if (ch == eType_Total)
	{
		dword.b[2] = 0x0C;
	}
	else if (ch == eType_AB)
	{
		dword.b[2] = 0x0D;
	}
	else if (ch == eType_BC)
	{
		dword.b[2] = 0x0E;
	}
	else if (ch == eType_CA)
	{
		dword.b[2] = 0x0F;
	}
	else
	{
		return;
	}
	
	dword.b[3] = 0x21;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_ReadPowerFactor_0x23(UINT8 ch)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	if (ch == eType_Total)
	{
		dword.b[2] = 0x14;
	}
	else if (ch == eType_AB)
	{
		dword.b[2] = 0x15;
	}
	else if (ch == eType_BC)
	{
		dword.b[2] = 0x16;
	}
	else if (ch == eType_CA)
	{
		dword.b[2] = 0x17;
	}
	else
	{
		return;
	}
	
	dword.b[3] = 0x21;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_Read_status(UINT8 Byte7, UINT8 Byte6)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	dword.b[3] = Byte7;
	dword.b[2] = Byte6;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_Read_alm(UINT8 Byte7, UINT8 Byte6)
{
	UINT8 u8RcvMsgNum = 0u;
	co_dword dword;

	dword.v = 0u;
	dword.b[3] = Byte7;
	dword.b[2] = Byte6;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		ABUUnPackData(u8RcvMsgNum);	
	}
}

void ABU_Set_OnOff(UINT8 Cmd)       //0x24 0x11 0x10   模块开关机
{
	co_dword dword;
	co_dword dword1;

	dword.v = 0u;
	dword.b[3] = 0x11;
	dword.b[2] = 0x10;

	dword1.v = 0u;
	if (Cmd == eType_Off)
	{
		dword1.b[0] = 0xA1;
	}
	else
	{
		dword1.b[0] = 0xA0;
	}

	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x24, dword.v, dword1.v);
}

void ABU_Set_OnOffGridModeSwitchAuto(UINT8 Cmd)
{
	co_dword dword;
	co_dword dword1;

	dword.v = 0u;
	dword.b[3] = 0x21;
	dword.b[2] = 0x19;

	dword1.v = 0u;
	dword1.b[0] = Cmd;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x24, dword.v, dword1.v);
}

void ABU_Set_OnOffGridMode(UINT8 Cmd)                 //0x24 0x21 0x10   设置工作模式
{
	co_dword dword;
	co_dword dword1;

	dword.v = 0u;
	dword.b[3] = 0x21;
	dword.b[2] = 0x10;

	dword1.v = 0u;
	dword1.b[0] = Cmd;
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x24, dword.v, dword1.v);
}

void ABU_CheckABUIsConfig(void)
{
	UINT8 i = 0u;
	UINT8 u8RcvMsgNum = 0u;
	UINT8 u8RcvDstAddr, u8RcvSrcAddr;
	co_dword dword;

	if(ABU->u8ConfigDelay > 60)
	{
		ABU->u8ConfigDelay = 0;
	}
	else
	{
		return;
	}

	//check ABU soft version 
	dword.v = 0u;
	dword.b[3] = 0x11;
	dword.b[2] = 0x22;
	
	ABU_PackSendMsg(R_PRPTO_DEVNO_0A, 0x23, dword.v, 0);
	
	OSTimeDly(CFG_ABU_RESPONSE_TIME);
	u8RcvMsgNum = gs_CANComm.fRxCAN3Buff(CAN_3_MAX_BUFF, RxMsg);
	if (u8RcvMsgNum > 0)
	{
		if(u8RcvMsgNum > CAN_3_MAX_BUFF)
		{
			return;
		}

		for (i = 0u; i < u8RcvMsgNum; i++)
		{
			u8RcvDstAddr = ((RxMsg[i].ExtId>>8)&0xff);
			u8RcvSrcAddr = (RxMsg[i].ExtId & 0xff);

			if ((u8RcvDstAddr != R_PROTO_SELF_ADDR) || (u8RcvSrcAddr != ABU_Addr))
			{
				ABU->ConfigFailed = FALSE;
				continue;
			}

			ABU->ConfigFailed = TRUE;
		}
	}
	else
	{
		ABU->ConfigFailed = FALSE;
	}
}

/*******************************************************************************
 *  Public Functions Definition
 */

void ABU_Comm(void)
{
	static UINT8 s_u8ReadABUInfoStep = 0;

	//模块在升级
	if(gEvChargeInfo.u8RectUpgradeFlag == TRUE)
	{
		return;
	}

	//ABU Disable
	if (!gEvChargeInfo.u16AttributesBitsMap_2.BIT_FIELD.ABU1Exist)
	{
		ABU->CommFailed = FALSE;
		ABU->CommFailedLast = FALSE;
		ABU->IsGetABUVersion = FALSE;
		ABU->IsGetBarCode = FALSE;
		ABU->UnrecvTimes = 0u;

		//check ABU is config
		ABU_CheckABUIsConfig();

		return;
	}
	else
	{
		ABU->ConfigFailed = FALSE;
	}

	if(gEvChargeInfo.u8GetABUInfoDelay != 0)
	{
		return;
	}
	else
	{
		gEvChargeInfo.u8GetABUInfoDelay = gEvChargeInfo.u8GetABUInfoInterval;
	}


	//by lxr01202 若读取的ABU版本为0，则持续读取，直到读取到为止
	if ((!(ABU->IsGetABUVersion & 0x01))||(!ABU->DC_Version))
    {
            ABU_ReadSoftWareVersion(0u);
            return;
    }

    if ((!(ABU->IsGetABUVersion & 0x02))||(!ABU->PFC_Version))
    {
            ABU_ReadSoftWareVersion(1u);
            return;
    }
#if 0
	if (!ABU->IsGetBarCode)
	{
		ABU_ReadBarCode_0x0B();
	}
#endif

	if (ABU->UnrecvTimes >= 5u)
	{
		ABU_CommFailed();
	}

	ABU->UnrecvTimes++;
	ABU_ReadOutputVolt_0x23(eType_AB);
	ABU_ReadOutputVolt_0x23(eType_BC);
	ABU_ReadOutputVolt_0x23(eType_CA);
	ABU_ReadPowerTemperature_0x23();
	
	ABU_ReadACVoltage_0x23(eType_AB);
	ABU_ReadACVoltage_0x23(eType_BC);
	ABU_ReadACVoltage_0x23(eType_CA);
	
	//这里为了降低CAN负载率，单步读取
	s_u8ReadABUInfoStep++;
	if(s_u8ReadABUInfoStep >= 16)
	{
		s_u8ReadABUInfoStep = 0;
	}

	if(s_u8ReadABUInfoStep == 0)
	{
		ABU_ReadACCurrent_0x23(eType_AB);
	}
	else if(s_u8ReadABUInfoStep == 1)
	{
		ABU_ReadACCurrent_0x23(eType_BC);
	}
	else if(s_u8ReadABUInfoStep == 2)
	{
		ABU_ReadACCurrent_0x23(eType_CA);
	}
	else if(s_u8ReadABUInfoStep == 3)
	{
		ABU_ReadACFrequency_0x23();
	}
	else if(s_u8ReadABUInfoStep == 4)
	{
		ABU_ReadActivePower_0x23(eType_Total);
	}
	else if(s_u8ReadABUInfoStep == 5)
	{
		ABU_ReadActivePower_0x23(eType_AB);
	}
	else if(s_u8ReadABUInfoStep == 6)
	{
		ABU_ReadActivePower_0x23(eType_BC);
	}
	else if(s_u8ReadABUInfoStep == 7)
	{
		ABU_ReadActivePower_0x23(eType_CA);
	}
	else if(s_u8ReadABUInfoStep == 8)
	{
		ABU_ReadReactivePower_0x23(eType_Total);
	}
	else if(s_u8ReadABUInfoStep == 9)
	{
		ABU_ReadReactivePower_0x23(eType_AB);
	}
	else if(s_u8ReadABUInfoStep == 10)
	{
		ABU_ReadReactivePower_0x23(eType_BC);
	}
	else if(s_u8ReadABUInfoStep == 11)
	{
		ABU_ReadReactivePower_0x23(eType_CA);
	}
	else if(s_u8ReadABUInfoStep == 12)
	{
		ABU_ReadPowerFactor_0x23(eType_Total);
	}
	else if(s_u8ReadABUInfoStep == 13)
	{
		ABU_ReadPowerFactor_0x23(eType_AB);
	}
	else if(s_u8ReadABUInfoStep == 14)
	{
		ABU_ReadPowerFactor_0x23(eType_BC);
	}
	else if(s_u8ReadABUInfoStep == 15)
	{
		ABU_ReadPowerFactor_0x23(eType_CA);
	}
	
	ABU_state_ALM_init();

	ABU_Read_status(0x11, 0x16);
	ABU_Read_alm(0x11, 0x17);
	ABU_Read_alm(0x11, 0x18);
	ABU_Read_alm(0x11, 0x19);

	if (ABU->ControlInfo.ControlBitmap.bit.OnOff)
	{
		ABU_Set_OnOff(ABU->ControlInfo.OnOff);
		ABU_Set_OnOff(ABU->ControlInfo.OnOff);
		ABU_Set_OnOff(ABU->ControlInfo.OnOff);

		ABU->ControlInfo.ControlBitmap.bit.OnOff = 0;
	}

	if (ABU->ControlInfo.ControlBitmap.bit.OnOffGridSwitchModeAuto)
	{
		ABU_Set_OnOffGridModeSwitchAuto(ABU->ControlInfo.OnOffGridSwitchModeAuto);
		ABU_Set_OnOffGridModeSwitchAuto(ABU->ControlInfo.OnOffGridSwitchModeAuto);
		ABU_Set_OnOffGridModeSwitchAuto(ABU->ControlInfo.OnOffGridSwitchModeAuto);
		ABU->ControlInfo.ControlBitmap.bit.OnOffGridSwitchModeAuto = 0;
	}

	if (ABU->ControlInfo.ControlBitmap.bit.OnOffGridMode)
	{
		ABU_Set_OnOffGridMode(ABU->ControlInfo.OnOffGridMode);
		ABU_Set_OnOffGridMode(ABU->ControlInfo.OnOffGridMode);
		ABU_Set_OnOffGridMode(ABU->ControlInfo.OnOffGridMode);
		ABU->ControlInfo.ControlBitmap.bit.OnOffGridMode = 0;
	}

	if ((ABU->CommFailed == FALSE) && (ABU->CommFailedLast == !FALSE))
	{
		ABU_Set_OnOff(eType_On);
		ABU_Set_OnOff(eType_On);
		ABU_Set_OnOff(eType_On);
		ABU->CommFailedLast = ABU->CommFailed;
	}
}





