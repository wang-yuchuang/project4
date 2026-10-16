/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPKA84917_async.c
** @Founder      : WYP40505
** @Date         : 2026.05.06
** @Function     : KA84917驱动模块,MCU通过SPI与KA84920通信，再间接通过菊花链与KA84917通信
** @Instructions : 该模块需要用到rtos的延时函数osDelay()，故该模块的函数不能用于定时任务中
**===========================================================================================*/
#if 1
#ifndef __BSPKA84917_ASYNC_H
#define __BSPKA84917_ASYNC_H
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPTypeDef.h"
/*rtos延时必须头文件*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
/*rtos延时必须头文件*/
/*定时器ms延时必须头文件*/
#include "BSPTIM.h"
/*定时器ms延时必须头文件*/
/*=============================================================================================
** @Macro definition
**===========================================================================================*/
#define delayms(x) 				osDelay(x)      //定义带参数的rtos延时宏
// #define delayus(x) 				delay_us_tim(x)      //定义带参数的定时器延时宏
#define NO_MASK					(0xFFFFu)
#define M_MASK0(x)				(((ULONG)~(x)) & 0x0000FFFFu)							/* PRQA S 3453 */ /* permission for simplification with macro */
#define M_MASK1(x)				(((ULONG)(x) << 16u) | 0x0000FFFFu)						/* PRQA S 3453 */ /* permission for simplification with macro */
#define M_MASK(x,y)				((((ULONG)~(x)) & 0x0000FFFFu) | ((ULONG)(y) << 16u))	/* PRQA S 3453 */ /* permission for simplification with macro */
#define NOMASK					((ULONG)0x0000FFFFu)
#define ALLMASK					((ULONG)0x00000000u)



#define	DEVICEADDRESS_MAX			((UCHAR)2u)		/* device=2, KA84917address=1, KA84922address=2 */
#define	BMIC_REG_ADR_MAX_COMMON		((UCHAR)0xFFu)





#define SPI_NOERR					(UCHAR)(0xA0u)
#define SPI_ERR						(UCHAR)(0xA1u)
#define SPI_CONTINUE				(UCHAR)(0xA2u)
#define SPI_CRCERR					(UCHAR)(0xA3u)
#define SPI_TIME_OUT_ERR			(UCHAR)(0xA4u)
#define SPI_DAISY_ERR				(UCHAR)(0xA5u)

#define SPI_REQ_2BYTE_WRITE				(UCHAR)(3u)
#define SPI_REQ_2BYTE_READ				(UCHAR)(4u)
#define SPI_REQ_WRITE_INIT				(UCHAR)(5u)
#define SPI_REQ_CONTINUE_READ			(UCHAR)(6u)
#define SPI_REQ_REPEAT_READ				(UCHAR)(7u)
#define SPI_REQ_WAIT					(UCHAR)(8u)
#define	SPI_REQ_FIR_READ				(UCHAR)(9u)
#define	SPI_REQ_CVM_READ				(UCHAR)(10u)
#define	SPI_REQ_2BYTE_READ_TSR190_311	(UCHAR)(11u)
#define	SPI_REQ_ADFIX_READ_TSR192		(UCHAR)(12u)
#define	SPI_REQ_ADFIX_READ_TSR195		(UCHAR)(13u)
#define	SPI_REQ_ADFIX_READ_TSR196		(UCHAR)(14u)
#define SPI_REQ_FIFO_CONT_READ			(UCHAR)(15u)
#define SPI_REQ_RPT_CONT_READ			(UCHAR)(16u)






/* control register group 1 */
#define	BMIC_CNTADR_00	 	((UCHAR)0x00u)
#define	BMIC_CNTADR_01		((UCHAR)0x01u)
#define	BMIC_CNTWAKE 		((UCHAR)0x02u)
#define	BMIC_CNTSKIP_3		((UCHAR)0x03u)
#define	BMIC_CNTSKIP_4		((UCHAR)0x04u)
#define	BMIC_CNTSKIP_5		((UCHAR)0x05u)
#define	BMIC_CNTSKIP_6		((UCHAR)0x06u)
#define	BMIC_CNTSKIP_7		((UCHAR)0x07u)
#define	BMIC_CNTFILT 		((UCHAR)0x08u)
#define	BMIC_CNTADR_09 		((UCHAR)0x09u)

#define	BMIC_CNTADR_0B		((UCHAR)0x0Bu)
#define	BMIC_CNTADR_0C		((UCHAR)0x0Cu)
#define	BMIC_CNTADR_0D		((UCHAR)0x0Du)
#define	BMIC_OVUVHYS_ADR	((UCHAR)0x0Du)
#define	BMIC_UVMSK_ADR		((UCHAR)0x0Du)
#define	BMIC_CNTOVUVTH_ADR	((UCHAR)0x0Eu)
#define	BMIC_CNTGPADSEL_ADR0F	((UCHAR)0x0Fu)
#define	BMIC_CNTCVSEL_ADR10	((UCHAR)0x10u)
#define	BMIC_CNTADR_11		((UCHAR)0x11u)
#define	BMIC_CNTRSEL_ADR12	((UCHAR)0x12u)
#define	BMIC_CNTADR_13		((UCHAR)0x13u)
#define	BMIC_CNTADR_14		((UCHAR)0x14u)
#define	BMIC_CNTADR_15		((UCHAR)0x15u)
#define	BMIC_CNTADR_16		((UCHAR)0x16u)
#define	BMIC_CNTOTTH		((UCHAR)0x17u)
#define	BMIC_CNTUTTH		((UCHAR)0x18u)
#define	BMIC_CNTCBON19		((UCHAR)0x19u)
#define	BMIC_CNTCBON1A		((UCHAR)0x1Au)
#define	BMIC_CNTCBTIME1B	((UCHAR)0x1Bu)
#define	BMIC_CNTCBTIME1C	((UCHAR)0x1Cu)
#define	BMIC_CNTCBTIME1D	((UCHAR)0x1Du)
#define	BMIC_CNTCBTIME1E	((UCHAR)0x1Eu)
#define	BMIC_CNTCBTIME1F	((UCHAR)0x1Fu)
#define	BMIC_CNTCBTIME20	((UCHAR)0x20u)
#define	BMIC_CNTCBTIME21	((UCHAR)0x21u)
#define	BMIC_CNTCBTIME22	((UCHAR)0x22u)
#define	BMIC_CNTADR_23		((UCHAR)0x23u)

/* status register group 1 */
#define	BMIC_ST_REVID		((UCHAR)0x25u)
#define	BMIC_ST_OT14_1_F	((UCHAR)0x26u)
#define	BMIC_ST_UT14_1_F	((UCHAR)0x27u)
#define	BMIC_ST_OV16_1_F	((UCHAR)0x28u)
#define	BMIC_ST_OV25_17_F	((UCHAR)0x29u)
#define	BMIC_ST_UV16_1_F	((UCHAR)0x2Au)
#define	BMIC_ST_UV25_17_F	((UCHAR)0x2Bu)
#define	BMIC_STSADR_2C		((UCHAR)0x2Cu)
#define	BMIC_STSADR_2D		((UCHAR)0x2Du)
#define	BMIC_STSADR_2E		((UCHAR)0x2Eu)

/* data register group */
#define	BMIC_ACELL_ADR2F	((UCHAR)0x2Fu)
#define	BMIC_ACELL_ADR30	((UCHAR)0x30u)
#define	BMIC_ACELL_ADR31	((UCHAR)0x31u)
#define	BMIC_ACELL_ADR32	((UCHAR)0x32u)
#define	BMIC_ACELL_ADR33	((UCHAR)0x33u)
#define	BMIC_ACELL_ADR34	((UCHAR)0x34u)
#define	BMIC_ACELL_ADR35	((UCHAR)0x35u)
#define	BMIC_ACELL_ADR36	((UCHAR)0x36u)
#define	BMIC_ACELL_ADR37	((UCHAR)0x37u)
#define	BMIC_ACELL_ADR38	((UCHAR)0x38u)
#define	BMIC_ACELL_ADR39	((UCHAR)0x39u)
#define	BMIC_ACELL_ADR3A	((UCHAR)0x3Au)
#define	BMIC_ACELL_ADR3B	((UCHAR)0x3Bu)
#define	BMIC_ACELL_ADR3C	((UCHAR)0x3Cu)
#define	BMIC_ACELL_ADR3D	((UCHAR)0x3Du)
#define	BMIC_ACELL_ADR3E	((UCHAR)0x3Eu)
#define	BMIC_ACELL_ADR3F	((UCHAR)0x3Fu)
#define	BMIC_ACELL_ADR40	((UCHAR)0x40u)
#define	BMIC_ACELL_ADR41	((UCHAR)0x41u)
#define	BMIC_ACELL_ADR42	((UCHAR)0x42u)
#define	BMIC_ACELL_ADR43	((UCHAR)0x43u)
#define	BMIC_ACELL_ADR44	((UCHAR)0x44u)
#define	BMIC_ACELL_ADR45	((UCHAR)0x45u)
#define	BMIC_ACELL_ADR46	((UCHAR)0x46u)
#define	BMIC_ACELL_ADR47	((UCHAR)0x47u)
#define	BMIC_VBAT_ADR48		((UCHAR)0x48u)
#define	BMIC_GPIOA_ADR49	((UCHAR)0x49u)
#define	BMIC_GPIOA_ADR4A	((UCHAR)0x4Au)
#define	BMIC_GPIOA_ADR4B	((UCHAR)0x4Bu)
#define	BMIC_GPIOA_ADR4C	((UCHAR)0x4Cu)
#define	BMIC_GPIOA_ADR4D	((UCHAR)0x4Du)
#define	BMIC_GPIOA_ADR4E	((UCHAR)0x4Eu)
#define	BMIC_GPIOA_ADR4F	((UCHAR)0x4Fu)
#define	BMIC_GPIOA_ADR50	((UCHAR)0x50u)
#define	BMIC_GPIOA_ADR51	((UCHAR)0x51u)
#define	BMIC_GPIOA_ADR52	((UCHAR)0x52u)
#define	BMIC_GPIOA_ADR53	((UCHAR)0x53u)
#define	BMIC_GPIOA_ADR54	((UCHAR)0x54u)
#define	BMIC_GPIOA_ADR55	((UCHAR)0x55u)
#define	BMIC_GPIOA_ADR56	((UCHAR)0x56u)
#define	BMIC_VDD50A_ADR57	((UCHAR)0x57u)
#define	BMIC_VDD18A_ADR58	((UCHAR)0x58u)
#define	BMIC_DIETEMPA_ADR59	((UCHAR)0x59u)
#define	BMIC_MCELL_ADR5A	((UCHAR)0x5Au)
#define	BMIC_MCELL_ADR5B	((UCHAR)0x5Bu)
#define	BMIC_MCELL_ADR5C	((UCHAR)0x5Cu)
#define	BMIC_MCELL_ADR5D	((UCHAR)0x5Du)
#define	BMIC_MCELL_ADR5E	((UCHAR)0x5Eu)
#define	BMIC_MCELL_ADR5F	((UCHAR)0x5Fu)
#define	BMIC_MCELL_ADR60	((UCHAR)0x60u)
#define	BMIC_MCELL_ADR61	((UCHAR)0x61u)
#define	BMIC_MCELL_ADR62	((UCHAR)0x62u)
#define	BMIC_MCELL_ADR63	((UCHAR)0x63u)
#define	BMIC_MCELL_ADR64	((UCHAR)0x64u)
#define	BMIC_MCELL_ADR65	((UCHAR)0x65u)
#define	BMIC_MCELL_ADR66	((UCHAR)0x66u)
#define	BMIC_MCELL_ADR67	((UCHAR)0x67u)
#define	BMIC_MCELL_ADR68	((UCHAR)0x68u)
#define	BMIC_MCELL_ADR69	((UCHAR)0x69u)
#define	BMIC_MCELL_ADR6A	((UCHAR)0x6Au)
#define	BMIC_MCELL_ADR6B	((UCHAR)0x6Bu)
#define	BMIC_MCELL_ADR6C	((UCHAR)0x6Cu)
#define	BMIC_MCELL_ADR6D	((UCHAR)0x6Du)
#define	BMIC_MCELL_ADR6E	((UCHAR)0x6Eu)
#define	BMIC_MCELL_ADR6F	((UCHAR)0x6Fu)
#define	BMIC_MCELL_ADR70	((UCHAR)0x70u)
#define	BMIC_MCELL_ADR71	((UCHAR)0x71u)
#define	BMIC_MCELL_ADR72	((UCHAR)0x72u)
#define	BMIC_GPADM_ADR73	((UCHAR)0x73u)
#define	BMIC_GPADM_ADR74	((UCHAR)0x74u)
#define	BMIC_GPADM_ADR75	((UCHAR)0x75u)
#define	BMIC_GPADM_ADR76	((UCHAR)0x76u)
#define	BMIC_GPADM_ADR77	((UCHAR)0x77u)
#define	BMIC_GPADM_ADR78	((UCHAR)0x78u)
#define	BMIC_GPADM_ADR79	((UCHAR)0x79u)
#define	BMIC_GPADM_ADR7A	((UCHAR)0x7Au)
#define	BMIC_GPADM_ADR7B	((UCHAR)0x7Bu)
#define	BMIC_GPADM_ADR7C	((UCHAR)0x7Cu)
#define	BMIC_GPADM_ADR7D	((UCHAR)0x7Du)
#define	BMIC_GPADM_ADR7E	((UCHAR)0x7Eu)
#define	BMIC_GPADM_ADR7F	((UCHAR)0x7Fu)
#define	BMIC_GPADM_ADR80	((UCHAR)0x80u)
#define	BMIC_VDD50M_ADR81	((UCHAR)0x81u)
#define	BMIC_VDD18M_ADR82	((UCAHR)0x82u)
#define	BMIC_DIETEMPM_ADR83	((UCHAR)0x83u)

/* status register group 2 */
#define	BMIC_STSADR_84		((UCHAR)0x84u)

#define	BMIC_ST_CHIPINFO	((UCHAR)0x92u)
#define	BMIC_ST_CHIPINFO2	((UCHAR)0x93u)
#define	BMIC_ST_CHIPINFO3	((UCHAR)0x94u)
#define	BMIC_ST_CB16_1		((UCHAR)0x95u)
#define	BMIC_ST_CB25_17		((UCHAR)0x96u)
#define	BMIC_ST_GPIO14_1	((UCHAR)0x97u)

/* control register group 2 */
#define	BMIC_CNTGPIO98		((UCHAR)0x98u)
#define	BMIC_CNTGPIO99		((UCHAR)0x99u)
#define	BMIC_CNTGPIO9A		((UCHAR)0x9Au)
#define	BMIC_CNTGPIO9B		((UCHAR)0x9Bu)
#define	BMIC_CNTGPIO9C		((UCHAR)0x9Cu)
#define	BMIC_CNTADR_9D		((UCHAR)0x9Du)
#define	BMIC_CNTOTUT14_1_SET	((UCHAR)0x9Eu)
#define	BMIC_CNTDCI_SYNC	((UCHAR)0x9Fu)
#define	BMIC_CNTCB_UVTH		((UCHAR)0xA0u)
#define	BMIC_CNTADR_A1		((UCHAR)0xA1u)

/* status register group 3 */
#define	BMIC_ST_CBTIMEA2	((UCHAR)0xA2u)
#define	BMIC_ST_CBTIMEA3	((UCHAR)0xA3u)
#define	BMIC_ST_CBTIMEA4	((UCHAR)0xA4u)
#define	BMIC_ST_CBTIMEA5	((UCHAR)0xA5u)
#define	BMIC_ST_CBTIMEA6	((UCHAR)0xA6u)
#define	BMIC_ST_CBTIMEA7	((UCHAR)0xA7u)
#define	BMIC_ST_CBTIMEA8	((UCHAR)0xA8u)
#define	BMIC_ST_CBTIMEA9	((UCHAR)0xA9u)
#define	BMIC_ST_CBTIMEAA	((UCHAR)0xAAu)

/* control register group 3 */
#define	BMIC_CNTGRP_ID		((UCHAR)0xAFu)
#define	BMIC_CNTADR_B0		((UCHAR)0xB0u)
#define	BMIC_CNTADR_B1		((UCHAR)0xB1u)
#define	BMIC_CNTADR_B2		((UCHAR)0xB2u)

#define	BMIC_CNTADR_B4		((UCHAR)0xB4u)
#define	BMIC_CNTADR_B5		((UCHAR)0xB5u)
#define	BMIC_CNTIICRSTN		((UCHAR)0xB6u)
#define	BMIC_CNTADR_B7		((UCHAR)0xB7u)
#define	BMIC_CNTSENDIC		((UCHAR)0xB8u)
#define	BMIC_CNTADR_B9		((UCHAR)0xB9u)
#define	BMIC_CNTADR_BA		((UCHAR)0xBAu)
#define	BMIC_CNTADR_BB		((UCHAR)0xBBu)
#define	BMIC_CNTADR_BC		((UCHAR)0xBCu)
#define	BMIC_CNTADR_BD		((UCHAR)0xBDu)
#define	BMIC_CNTADR_BE		((UCHAR)0xBEu)
#define	BMIC_CNTADR_BF		((UCHAR)0xBFu)

/* status register group 4 */
#define	BMIC_STSADR_C0		((UCHAR)0xC0u)
#define	BMIC_STSADR_C1		((UCHAR)0xC1u)
#define	BMIC_STSADR_C2		((UCHAR)0xC2u)
#define	BMIC_STSADR_C3		((UCHAR)0xC3u)
#define	BMIC_STSADR_C4		((UCHAR)0xC4u)
#define	BMIC_STSADR_C5		((UCHAR)0xC5u)
#define	BMIC_STSADR_C6		((UCHAR)0xC6u)

/* registor for BMS define */
#define	BMIC_REG_95		((UCHAR)0x95u)
#define	BMIC_REG_A1		((UCHAR)0xA1u)
#define	BMIC_REG_B0		((UCHAR)0xB0u)
#define	BMIC_REG_BF		((UCHAR)0xBFu)

#define BMIC_CTRLTOPADD		((UCHAR)0x01u)			/* Top address of control register 1 */ 
#define BMIC_CTRLBTMADD		((UCHAR)0x23u)			/* Bottom address of control register 1 */
#define	BMIC_ACELL_BTM_ADR_MAX47	((UCHAR)0x47u)	/* Max address of (Data acquisition system) Cell voltage measurement result register */
#define	BMIC_MCELL_BTM_ADR_MAX72	((UCHAR)0x72u)	/* Max address of (Monitoring system) Cell voltage measurement result register */
#define	BMIC_GPIOM_ADR73	((UCHAR)0x73u)			/* Top address of GPADxM data register, DIETEMPM_AD */
#define	BMIC_GPIOM_BTM_ADR83	((UCHAR)0x83u)		/* Bottom address of GPADxM data register, DIETEMPM_AD */
#define BMIC_ABNSTSCOMM		((UCHAR)0x2Cu)			/* Abnormal communication status register */

#define BMIC_ABNSTSPOWER	((UCHAR)0x2Du)			/* Abnormal power and temperature status register */
#define BMIC_ABNSTSOSCL		((UCHAR)0x2Du)			/* Abnormal mux and oscl status register */
#define BMIC_ABNSTSBTMADD	((UCHAR)0x2Eu)			/* Bottom address of abnormal status register */
#define BMIC_VOLDATABTMADD	((UCHAR)0x83u)			/* Bottom address of abnormal data register */

/* registor for communication define */
#define BMIC_DEV_TYPE_COMMIC	((UCHAR)0x0u)
#define BMIC_DEV_TYPE_MEASIC	((UCHAR)0x1u)
#define BMIC_CONTSTSTOPADD	((UCHAR)0x25u)			/* Top address of continuous read register */

/****************************************************************************
 define macro bit definition
*****************************************************************************/
/* 0x00 setting */
/* 15 - 14 */
#define	BMIC_DCI_RATE_2560	((USHORT)0xC000u)		/* Daisy frequency 2.560MHz */
#define	BMIC_DCI_RATE_2276	((USHORT)0x8000u)		/* Daisy frequency 2.276MHz */
#define	BMIC_DCI_RATE_2048	((USHORT)0x4000u)		/* Daisy frequency 2.048MHz */
#define	BMIC_DCI_RATE_1024	((USHORT)0x0000u)		/* Daisy frequency 1.024MHz */

/* 0x01 setting */
/* bp15 */
#define	BMIC_MSET_SA_ON		((USHORT)0x8000u)		/* Stand-alone ON: Shift stand-alone mode */
#define	BMIC_MSET_SA_NO		((USHORT)0x0000u)		/* Stand-alone No: No shift stand-alone mode */
#define	BMIC_MSET_SA		((USHORT)0x8000u)		/* Stand-alone ON */
/* bp14 - 10 */
#define	BMIC_RTIME			((USHORT)0x7C00u)		/* Active mode recovery time:31mS full set*/
/* bp9 */
#define	BMIC_EXT_VBATSW_1	((USHORT)0x0200u)		/* EXT_VBATSW control 1: SW-ON */
#define	BMIC_EXT_VBATSW_0	((USHORT)0x0000u)		/* EXT_VBATSW control 0: SW-OFF */
/* bp8 */
#define	BMIC_TIMER_SET_1	((USHORT)0x0100u)		/* 1: Disable communication timer / Enable shutdown timer */
#define	BMIC_TIMER_SET_0	((USHORT)0x0000u)		/* 0: Enable communication timer / Disable shutdown timer (default) */
/* bp7 - 6 */
#define	BMIC_ADC_ON_11		((USHORT)0x00C0u)		/* Data acquisition system ON / Monitoring system ON */
													/* #define	BMIC_ADC_ON_10		((USHORT)0x0080u)	 Setting prohibited */
#define	BMIC_ADC_ON_01		((USHORT)0x0040u)		/* Data acquisition system ON /  Monitoring system OFF */
#define	BMIC_ADC_ON_00		((USHORT)0x0000u)		/* Data acquisition system OFF /  Monitoring system OFF (default) */
/* bp5 -4 */
#define	BMIC_CB_SET_BIT		((USHORT)0x0030u)		/* Cellbalancing ON setting BIT */
#define	BMIC_EVENCB_SET		((USHORT)0x0020u)		/* Cellbalancing ON of the even number with direct control */
#define	BMIC_ODDCB_SET		((USHORT)0x0010u)		/* Cellbalancing ON of the odd number with direct control */
#define	BMIC_OFFCB_SET		((USHORT)0x0000u)		/* Cellbalancing OFF with direct control */
#define	BMIC_CB_SET_MASK	((USHORT)~0x0030u)		/* Cellbalancing ON setting BIT Mask */
/* bp3 */
#define	BMIC_MSET_RST_ON	((USHORT)0x0000u)		/* Soft reset ON: Reset BM-IC */
#define	BMIC_MSET_RST_NO	((USHORT)0x0008u)		/* Soft reset No: Not reset BM-IC */
#define	BMIC_MSET_RST		((USHORT)0x0008u)		/* Soft reset ON */
/* bp2 */
#define	BMIC_MSET_SHDN_ON	((USHORT)0x0000u)		/* Shutdown ON: Shift to shutdown mode */
#define	BMIC_MSET_SHDN_NO	((USHORT)0x0004u)		/* Shutdown No: No shift to shutdown mode */
#define	BMIC_MSET_SHDN		((USHORT)0x0004u)		/* Shutdown ON */
/* bp1 */
#define	BMIC_MSET_SLP_ON	((USHORT)0x0002u)		/* Sleep ON: Shift to sleep mode */
#define	BMIC_MSET_SLP		((USHORT)0x0002u)		/* Sleep ON */
/* bp0 */
#define	BMIC_MSET_STB_ON	((USHORT)0x0001u)		/* Standby ON: Shift to standby mode */
#define	BMIC_MSET_STB		((USHORT)0x0001u)		/* Standby ON */
/* other */
#define	BMIC_MSET_INIT		((USHORT)0x000Cu)		/* Mode set initialization */

/* 0x02 setting */
#define	BMIC_WAKEUP			((USHORT)0x0A2Bu)		/* Wake-up command */
#define	BMIC_COMRESUME_STD	((USHORT)0x0C5Du)		/* Communication resuming command from standby */
#define	BMIC_COMRESUME_SLP	((USHORT)0xCC5Du)		/* Communication resuming command from sleep */

/* 0x03 - 0x07 skip register */
#define	BMIC_CVSKIP_SEL03	((USHORT)0x0000u)		/* No register skipped */
#define	BMIC_CVSKIP_SEL04	((USHORT)0x0000u)		/* No register skipped */
#define	BMIC_CVSKIP_SEL05	((USHORT)0x0000u)		/* No register skipped */
#define	BMIC_CVSKIP_SEL06	((USHORT)0x0000u)		/* No register skipped */
#define	BMIC_CVSKIP_SEL07	((USHORT)0x0000u)		/* No register skipped */

/* 0x08 setting */
/* bp11 */
#define	BMIC_DSP_CLKSEL_Act	((USHORT)0x0800u)		/* Active the dummy clock even if ADC_ON is set to "01" or "10" and ADC is OFF */
#define	BMIC_DSP_CLKSEL_Ina	((USHORT)0x0000u)		/* Inactive the dummy clock when ADC_ON is set to "01" or "10" and ADC is OFF */
/* bp10 */
#define	BMIC_FIL_FAST_ON	((USHORT)0x0400u)		/* AD-Filter fast saturate function */
#define	BMIC_FIL_FAST_OFF	((USHORT)0x0000u)		/* AD-Filter fast saturate function */
/* bp6 - 4 */
#define	BMIC_DIG_FIL_AOFF		((USHORT)0x0070u)	/* AD-Filter Cut-Off Setting A OFF */
#define	BMIC_DIG_FIL_SET_A39	((USHORT)0x0050u)	/* AD-Filter Cut-Off Setting A3.9Hz */
#define	BMIC_DIG_FIL_SET_A79	((USHORT)0x0040u)	/* AD-Filter Cut-Off Setting A7.9Hz */
#define	BMIC_DIG_FIL_SET_A120	((USHORT)0x0030u)	/* AD-Filter Cut-Off Setting A12.0Hz */
#define	BMIC_DIG_FIL_SET_A161	((USHORT)0x0020u)	/* AD-Filter Cut-Off Setting A16.1Hz */
#define	BMIC_DIG_FIL_SET_A334	((USHORT)0x0010u)	/* AD-Filter Cut-Off Setting A33.4Hz */
#define	BMIC_DIG_FIL_SET_AOFF	((USHORT)0x0000u)	/* AD-Filter Cut-Off Setting A OFF default */
/* bp1 - 0 */
#define	BMIC_AD_FILA_RESUME	((USHORT)0x0003u)		/* AD-Filter A resume */
#define	BMIC_AD_FILA_START	((USHORT)0x0002u)		/* AD-Filter A start(initial setting) */
#define	BMIC_AD_FILA_STOP	((USHORT)0x0001u)		/* AD-Filter A suspend */
#define	BMIC_AD_FILA_OFF	((USHORT)0x0000u)		/* AD-Filter A off */
#define	BMIC_AD_FILA_MASK	((USHORT)~0x0003u)		/* AD-Filter A BIT mask */

/* 0x09 setting */
/* bp15 - 14 */
#define	BMIC_CAL_RES_25		((USHORT)0xC000u)		/* External RC filter resistanc value setting: 2.5 kohm */
#define	BMIC_CAL_RES_10		((USHORT)0x8000u)		/* External RC filter resistanc value setting: 1.0 kohm */
#define	BMIC_CAL_RES_100	((USHORT)0x4000u)		/* External RC filter resistanc value setting: 10 kohm */
#define	BMIC_CAL_RES_47		((USHORT)0x0000u)		/* External RC filter resistanc value setting: 4.7 kohm */
/* bp12 */
#define	BMIC_EN_CMP_BB_VLD	((USHORT)0x1000u)		/* Monitorinig system bus bar adjacent cell voltage correciton setting: Valid */
#define	BMIC_EN_CMP_BB_INV	((USHORT)0x0000u)		/* Monitorinig system bus bar adjacent cell voltage correciton setting: Invalid */
/* bp11 */
#define	BMIC_SMP_SEL_ON		((USHORT)0x0800u)		/* random sampling ON */
#define	BMIC_SMP_SEL_OFF	((USHORT)0x0000u)		/* random sampling OFF */
#define	BMIC_SMP_SEL_MASK	((USHORT)~0x0800u)		/* random sampling MASK */
/* bp10 - 9 */
#define	BMIC_INR_DRVSET_48UA	((USHORT)0x0600u)	/* 48uA for cell disconnection detection */
#define	BMIC_INR_DRVSET_32UA	((USHORT)0x0400u)	/* 32uA for cell disconnection detection */
#define	BMIC_INR_DRVSET_16UA	((USHORT)0x0200u)	/* 16uA for cell disconnection detection */
#define	BMIC_INR_DRVSET_0UA		((USHORT)0x0000u)	/* 0uA for cell disconnection detection (default) */
#define	BMIC_INR_DRVSET_MASK	((USHORT)~0x0600u)	/* BIT mask */
#define	BMIC_INR_DRVSET_BIT		((USHORT)0x0600u)	/* BIT */
/* bp8 */
#define	BMIC_NEN_TSDL_ON	((USHORT)0x0000u)		/* High temperature 135 oC abnormal detection function ON byTSDL */
#define	BMIC_NEN_TSDL_OFF	((USHORT)0x0100u)		/* High temperature 135 oC abnormal detection function OFF  */
#define	BMIC_NEN_TSD_MASK	((USHORT)~0x0100u)		/* High temperature 135 oC abnormal detection function MASK */
/* bp7 */
#define BMIC_CLR_FLAG		((USHORT)0x0080u)		/* Status Register clear */
#define BMIC_UNCLR_FLAG		((USHORT)0x0000u)		/* Status Register unclear */
#define BMIC_CLR_FLAG_MASK	((USHORT)~0x0080u)		/* Status Register clear BIT mask */
/* bp6 */
#define	BMIC_DIAG2_SKIP_ON	((USHORT)0x0040u)		/* Communication data skip function diagnostic function ON */
#define	BMIC_DIAG2_SKIP_OFF	((USHORT)0x0000u)		/* Communication data skip function diagnostic function OFF */
#define	BMIC_DIAG2_SKIP_MASK	((USHORT)~0x0040u)		/* Communication data skip function diagnostic function MASK */
/* bp4 */
#define	BMIC_DIAG2_AD_OFF	((USHORT)0x0000u)		/* DIAD2_AD OFF */
#define	BMIC_DIAG2_AD_ON	((USHORT)0x0010u)		/* DIAD2_AD ON */
#define	BMIC_DIAG2_AD_MASK	((USHORT)~0x0010u)		/* DIAD2_AD BIT mask */
/* bp2 */
#define	BMIC_DIAG2_ALM_ON	((USHORT)0x0004u)		/* DIAG2_ALM ON */
#define	BMIC_DIAG2_ALM_OFF	((USHORT)0x0000u)		/* DIAG2_ALM OFF */
#define	BMIC_DIAG2_ALM_MASK	((USHORT)~0x0004u)		/* DIAG2_ALM BIT mask */
/* bp1 */
#define	BMIC_DIAG1_GP_ON	((USHORT)0x0002u)		/* DIAG1_GP ON */
#define	BMIC_DIAG1_GP_OFF	((USHORT)0x0000u)		/* DIAG1_GP OFF */
#define	BMIC_DIAG1_GP_MASK	((USHORT)~0x0002u)		/* DIAG1_GP BIT mask */
/* bp0 */
#define	 BMIC_DIAG1_VM_ON	((USHORT)0x0001u)		/* Measurement system free run sequence mode VDAC measurement function ON */
#define	 BMIC_DIAG1_VM_OFF	((USHORT)0x0000u)		/* Measurement system free run sequence mode VDAC measurement function OFF */
													/* GPIO and internal supply voltage mesurement */

#define	BMIC_DIAG_ON_ALL	((USHORT)0x0096u)		/* DIAG ON ALL BIT expected DIAG1, DIAG2, CLR_FLAG */
#define	BMIC_DIAG_OFF_ALL	((USHORT)0x0000u)		/* DIAG OFF ALL BIT */

/* 0x0B setting  **REMARK** this register must be set to bp13 */
/* bp12 - 11 */
#define BMIC_BST_MODE11		((USHORT)0x1800u)		/* SDO pin is pulled to High every two bytes of data received by daisy communication */
#define BMIC_BST_MODE01		((USHORT)0x0800u)		/* SDO pin is pulled to High after the first two bytes of data received by daisy communication */
#define BMIC_BST_MODE00		((USHORT)0x0000u)		/* SDO pin is pulled to High after all data received by daisy communication (default) */
/* bp9 - 8 */
#define	BMIC_DCI_IISO00		((USHORT)0x0000u)		/* Daisy communication current power setting 16mA */
#define	BMIC_DCI_IISO01		((USHORT)0x0100u)		/* Daisy communication current power setting 20mA */
#define	BMIC_DCI_IISO10		((USHORT)0x0200u)		/* Daisy communication current power setting  8mA */
#define	BMIC_DCI_IISO11		((USHORT)0x0300u)		/* Daisy communication current power setting 12mA */
/* bp7 */
#define	BMIC_SDO_DRV_M4A	((USHORT)0x0000u)		/* SDO pin drive capability setting 4mA(default) */
#define	BMIC_SDO_DRV_M2A	((USHORT)0x0080u)		/* SDO pin drive capability setting 2mA */
/* bp1 */
#define	BMIC_CVSKIP_ON		((USHORT)0x0002u)		/* Skip-function ON in continuous register read 0x0B->0x0002u */
#define	BMIC_CVSKIP_OFF		((USHORT)0x0000u)		/* Skip-function OFF in continuous register read 0x0B->0x0000u */
/* bp0 */
#define	SET_SPI_CMT_05		((USHORT)0x0000u)		/* Communication time-out setting : 0.5s(default) */
#define	SET_SPI_CMT_20		((USHORT)0x0001u)		/* Communication time-out setting : 2.0s */

/* 0x0C setting */
/* bp8 - 7 */
#define	BMIC_SA_DT_SET11	((USHORT)0x0180u)		/* Temperature change diagnosis Stand-alone mode : 10 to 17 intermittent cycle data */
#define	BMIC_SA_DT_SET10	((USHORT)0x0100u)		/* Temperature change diagnosis Stand-alone mode : 2 to 9 intermittent cycle data */
#define	BMIC_SA_DT_SET01	((USHORT)0x0080u)		/* Temperature change diagnosis Stand-alone mode : 1 intermittent cycle data */
#define	BMIC_SA_DT_SET00	((USHORT)0x0000u)		/* Temperature change diagnosis Stand-alone mode : off */
/* bp6 */
#define	BMIC_IIC_SET_EN		((USHORT)0x0040u)		/* Enable I2C I/F (GPIO10 and GPIO11 are assigned as SCL2 and SDA2, respectively) */
#define	BMIC_IIC_SET_DIS	((USHORT)0x0000u)		/* Disable (default) */
/* bp5 */
#define	BMIC_ALM_IE_EN		((USHORT)0x0020u)		/* Enable alarm input from upper slave device. (GPIO13 is assigned as ALMIN.) */
#define	BMIC_ALM_IE_DIS		((USHORT)0x0000u)		/* Alarm input disable */
/* bp4 */
#define	BMIC_ALM_SET_ON		((USHORT)0x0010u)		/* Alarm on */
#define	BMIC_ALM_SET_OFF	((USHORT)0x0000u)		/* Alarm off */
/* bp3 */
#define	BMIC_REG_SEL_ON		((USHORT)0x0008u)		/* REG_SEL on */
#define	BMIC_REG_SEL_OFF	((USHORT)0x0000u)		/* REG_SEL off */
/* bp2 */
#define	BMIC_NPD_ICONT1_ON	((USHORT)0x0004u)		/* NPD_ICONT1 on */
#define	BMIC_NPD_ICONT1_OFF	((USHORT)0x0000u)		/* NPD_ICONT1 off */
/* bp1 */
#define	BMIC_PD_REG50_OFF	((USHORT)0x0002u)		/* PD_REG50 off */
#define	BMIC_PD_REG50_ON	((USHORT)0x0000u)		/* PD_REG50 on(default) */
/* bp0 */
#define	BMIC_VDD50DRV_SET_ON	((USHORT)0x0001u)	/* VDD50DRV_SET on */
#define	BMIC_VDD50DRV_SET_OFF	((USHORT)0x0000u)	/* VDD50DRV_SET off */

/* 0x0D setting */
/* bp15 */
#define	BMIC_OVP_F_SET_NOSHIFT	((USHORT)0x8000u)	/* 1: no shift (default) */
#define	BMIC_OVP_F_SET_TOSHDN	((USHORT)0x0000u)	/* 0: To shutdown mode */
/* bp14 */
#define	BMIC_TSD_F_SET_NOSHIFT	((USHORT)0x4000u)	/* 1: no shift (default) */
#define	BMIC_TSD_F_SET_TOSHDN	((USHORT)0x0000u)	/* 0: To shutdown mode */
/* bp13 */
#define	BMIC_OSC_F_SET_NOSHIFT	((USHORT)0x2000u)	/* 1: no shift (default) */
#define	BMIC_OSC_F_SET_TOSHDN	((USHORT)0x0000u)	/* 0: To shutdown mode */
/* bp10 */
#define	BMIC_CVSEL_SET		((USHORT)0x0400u)		/* 1: Start measurement */
#define	BMIC_CVSEL_SET_OFF	((USHORT)0x0000u)		/* 0: Status before measurement start (default) */
/* bp9 - 8 */
#define	BMIC_AD_LATCH_SET_11	((USHORT)0x0300u)	/* Voltage measurement data register updating timing setting : Instant updating (default) and After 500us(typ.) updating */
#define	BMIC_AD_LATCH_SET_10	((USHORT)0x0200u)	/* Voltage measurement data register updating timing setting : 10: Synchronous updating */
#define	BMIC_AD_LATCH_SET_01	((USHORT)0x0100u)	/* Voltage measurement data register updating timing setting : Instant updating (default) and After 500us(typ.) updating */
#define	BMIC_AD_LATCH_SET_00	((USHORT)0x0000u)	/* Voltage measurement data register updating timing setting : Instant updating (default) */
/* bp7 (COM-IC) */
#define	BMIC_HBOUT_EN_EN	((USHORT)0x0080u)		/* Enable Heart beat signal output enable (for GPIO4 pin) */
#define	BMIC_HBOUT_EN_DIS	((USHORT)0x0000u)		/* Disable Heart beat signal output enable (for GPIO4 pin) */
/* bp6 - 5 */
#define	BMIC_OUVDLY_SET_4MS	((USHORT)0x0060u)		/* 11: 4ms (8counts) Set the count of OV/UV detection delay filter for the alarm pin */
#define	BMIC_OUVDLY_SET_2MS	((USHORT)0x0040u)		/* 10: 2ms (4counts) Set the count of OV/UV detection delay filter for the alarm pin */
#define	BMIC_OUVDLY_SET_1MS	((USHORT)0x0020u)		/* 01: 1ms (2counts) Set the count of OV/UV detection delay filter for the alarm pin */
#define	BMIC_OUVDLY_SET_0_5MS	((USHORT)0x0000u)	/* 00: 0.5ms (1counts) Set the count of OV/UV detection delay filter for the alarm pin */
/* bp3 */
#define	BMIC_UVMSK_ON		((USHORT)0x0008u)		/* UV Mask ON */
#define	BMIC_UVMSK_OFF		((USHORT)0x0000u)		/* UV Mask OFF */
/* bp2 */
#define	BMIC_OV_SEL			((USHORT)0x0000u)		/* Resource data setting for OV/UV judgment before digital filter  */
#define	BMIC_OV_SEL_AFTER	((USHORT)0x0004u)		/* Resource data setting for OV/UV judgment after digital filter  */
/* bp1 - 0 */
#define	BMIC_OUT_HYS_SET_11	((USHORT)0x0003u)		/* 11: 0.0mV: Selection of OV/UV comparator hysteresis width  */
#define	BMIC_OUT_HYS_SET_10	((USHORT)0x0002u)		/* 10: 24.4mV: Selection of OV/UV comparator hysteresis width  */
#define	BMIC_OUT_HYS_SET_01	((USHORT)0x0001u)		/* 01: 96.7mV: Selection of OV/UV comparator hysteresis width  */
#define	BMIC_OUT_HYS_SET_00	((USHORT)0x0000u)		/* 00: 48.8mV: Selection of OV/UV comparator hysteresis width  */

/* 0x0E setting */
/* bp15 - 8 */
#define BMIC_OVTH			((USHORT)0xE000u)		/* OV threshold level:4.375V */
/* bp6 - 0 */
#define BMIC_UVTH			((USHORT)0x0030u)		/* UV threshold level:1.875V */

/* 0x0F setting */
#define BMIC_ALLGPIOSEL		((USHORT)0x1FFFu)		/* GPIO all selected */
#define BMIC_GPIOMASK		((USHORT)0x03FFu)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD1SEL		((USHORT)0x0001u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD2SEL		((USHORT)0x0002u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD3SEL		((USHORT)0x0004u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD4SEL		((USHORT)0x0008u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD5SEL		((USHORT)0x0010u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD6SEL		((USHORT)0x0020u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD7SEL		((USHORT)0x0040u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD8SEL		((USHORT)0x0080u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD9SEL		((USHORT)0x0100u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD10SEL		((USHORT)0x0200u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD11SEL		((USHORT)0x0400u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD12SEL		((USHORT)0x0800u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD13SEL		((USHORT)0x1000u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_GPAD14SEL		((USHORT)0x2000u)		/* request of all GPIO(1-14) monitoring selected */
#define BMIC_NOTGPIOSEL		((USHORT)0x0000u)		/* request of no GPIO(1-4) monitoring selected */
#define BMIC_GPADODDSEL		((USHORT)0x1555u)		/* request of GPIO(ODD) monitoring selected */
#define BMIC_GPADEVENSEL	((USHORT)0x2AAAu)		/* request of GPIO(EVEN) monitoring selected */

/* 0x10 setting */
#define BMIC_ALLCELLSEL		((USHORT)0xFFFFu)		/* request of all cell monitoring selected */
#define BMIC_NOTCELLSEL		((USHORT)0x0000u)		/* request of no monitoring selected 14cells */
#define BMIC_CV16SEL_OFF	((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell15 */
#define BMIC_CV16SEL_ON		((USHORT)0x8000u)		/* ON	: Impedance measurement for Cell15 */
#define BMIC_CV15SEL_OFF	((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell15 */
#define BMIC_CV15SEL_ON		((USHORT)0x4000u)		/* ON	: Impedance measurement for Cell15 */
#define BMIC_CV14SEL_OFF	((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell14 */
#define BMIC_CV14SEL_ON		((USHORT)0x2000u)		/* ON	: Impedance measurement for Cell14 */
#define BMIC_CV13SEL_OFF	((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell13 */
#define BMIC_CV13SEL_ON		((USHORT)0x1000u)		/* ON	: Impedance measurement for Cell13 */
#define BMIC_CV12SEL_OFF	((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell12 */
#define BMIC_CV12SEL_ON		((USHORT)0x0800u)		/* ON	: Impedance measurement for Cell12 */
#define BMIC_CV11SEL_OFF	((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell11 */
#define BMIC_CV11SEL_ON		((USHORT)0x0400u)		/* ON	: Impedance measurement for Cell11 */
#define BMIC_CV10SEL_OFF	((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell10 */
#define BMIC_CV10SEL_ON		((USHORT)0x0200u)		/* ON	: Impedance measurement for Cell10 */
#define BMIC_CV9SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell9 */
#define BMIC_CV9SEL_ON		((USHORT)0x0100u)		/* ON	: Impedance measurement for Cell9 */
#define BMIC_CV8SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell8 */
#define BMIC_CV8SEL_ON		((USHORT)0x0080u)		/* ON	: Impedance measurement for Cell8 */
#define BMIC_CV7SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell7 */
#define BMIC_CV7SEL_ON		((USHORT)0x0040u)		/* ON	: Impedance measurement for Cell7 */
#define BMIC_CV6SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell6 */
#define BMIC_CV6SEL_ON		((USHORT)0x0020u)		/* ON	: Impedance measurement for Cell6 */
#define BMIC_CV5SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell5 */
#define BMIC_CV5SEL_ON		((USHORT)0x0010u)		/* ON	: Impedance measurement for Cell5 */
#define BMIC_CV4SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell4 */
#define BMIC_CV4SEL_ON		((USHORT)0x0008u)		/* ON	: Impedance measurement for Cell4 */
#define BMIC_CV3SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell3 */
#define BMIC_CV3SEL_ON		((USHORT)0x0004u)		/* ON	: Impedance measurement for Cell3 */
#define BMIC_CV2SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell2 */
#define BMIC_CV2SEL_ON		((USHORT)0x0002u)		/* ON	: Impedance measurement for Cell2 */
#define BMIC_CV1SEL_OFF		((USHORT)0x0000u)		/* OFF	: Impedance measurement for Cell1 */
#define BMIC_CV1SEL_ON		((USHORT)0x0001u)		/* ON	: Impedance measurement for Cell1 */

/* 0x11 setting */
/* bp15 */
#define	BMIC_VBATMUXSEL_ON	((USHORT)0x8000u)		/* ON switching of VBATMUX measurement */
#define	BMIC_VBATMUXSEL_OFF	((USHORT)0x0000u)		/* OFF switching of VBATMUX measurement */
/* bp14 */
#define	BMIC_VDD18MSEL_ON	((USHORT)0x4000u)		/* ON switching of VDD18M measurement */
#define	BMIC_VDD18MSEL_OFF	((USHORT)0x0000u)		/* OFF switching of VDD18M measurement */
/* bp13 */
#define	BMIC_VDD50MSEL_ON	((USHORT)0x2000u)		/* ON switching of VDD50M measurement */
#define	BMIC_VDD50MSEL_OFF	((USHORT)0x0000u)		/* OFF switching of VDD50M measurement */
/* bp12 */
#define	BMIC_DIETEMPMSEL_ON		((USHORT)0x1000u)	/* ON switching of DIETEMPMSEL measurement */
#define	BMIC_DIETEMPMSEL_OFF	((USHORT)0x0000u)	/* OFF switching of DIETEMPMSEL measurement */
/* bp11 */
#define	BMIC_VDD18ASEL_ON	((USHORT)0x0800u)		/* ON switching of VDD18A measurement */
#define	BMIC_VDD18ASEL_OFF	((USHORT)0x0000u)		/* OFF switching of VDD18A measurement */
/* bp10 */
#define	BMIC_VDD50ASEL_ON	((USHORT)0x0400u)		/* ON switching of VDD50A measurement */
#define	BMIC_VDD50ASEL_OFF	((USHORT)0x0000u)		/* OFF switching of VDD50A measurement */
/* bp9 */
#define	BMIC_DIETEMPASEL_ON		((USHORT)0x0200u)	/* ON switching of DIETEMPASEL measurement */
#define	BMIC_DIETEMPASEL_OFF	((USHORT)0x0000u)	/* OFF switching of DIETEMPASEL measurement */
/* bp8 */
#define	BMIC_CV25SEL_ON		((USHORT)0x0100u)		/* ON switching of Cell 25 voltage measurement */
#define	BMIC_CV25SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 25 voltage measurement */
/* bp7 */
#define	BMIC_CV24SEL_ON		((USHORT)0x0080u)		/* ON switching of Cell 24 voltage measurement */
#define	BMIC_CV24SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 24 voltage measurement */
/* bp6 */
#define	BMIC_CV23SEL_ON		((USHORT)0x0040u)		/* ON switching of Cell 23 voltage measurement */
#define	BMIC_CV23SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 23 voltage measurement */
/* bp5 */
#define	BMIC_CV22SEL_ON		((USHORT)0x0020u)		/* ON switching of Cell 22 voltage measurement */
#define	BMIC_CV22SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 22 voltage measurement */
/* bp4 */
#define	BMIC_CV21SEL_ON		((USHORT)0x0010u)		/* ON switching of Cell 21 voltage measurement */
#define	BMIC_CV21SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 21 voltage measurement */
/* bp3 */
#define	BMIC_CV20SEL_ON		((USHORT)0x0008u)		/* ON switching of Cell 20 voltage measurement */
#define	BMIC_CV20SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 20 voltage measurement */
/* bp2 */
#define	BMIC_CV19SEL_ON		((USHORT)0x0004u)		/* ON switching of Cell 19 voltage measurement */
#define	BMIC_CV19SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 19 voltage measurement */
/* bp1 */
#define	BMIC_CV18SEL_ON		((USHORT)0x0002u)		/* ON switching of Cell 18 voltage measurement */
#define	BMIC_CV18SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 18 voltage measurement */
/* bp0 */
#define	BMIC_CV17SEL_ON		((USHORT)0x0001u)		/* ON switching of Cell 17 voltage measurement */
#define	BMIC_CV17SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 17 voltage measurement */

#define	BMIC_CV25_17SEL_ON	((USHORT)0x01FFu)		/* ON switching of Cell 17-25 voltage measurement */
#define	BMIC_CV25_17SEL_OFF	((USHORT)0x0000u)		/* OFF switching of Cell 17-25 voltage measurement */

/* 0x12 setting */
#define	BMIC_RSEL16_25_25V	((USHORT)0x8000u)		/* Cell 16 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL16_0V_5V	((USHORT)0x0000u)		/* Cell 16 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL15_25_25V	((USHORT)0x4000u)		/* Cell 15 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL15_0V_5V	((USHORT)0x0000u)		/* Cell 15 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL14_25_25V	((USHORT)0x2000u)		/* Cell 14 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL14_0V_5V	((USHORT)0x0000u)		/* Cell 14 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL13_25_25V	((USHORT)0x1000u)		/* Cell 13 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL13_0V_5V	((USHORT)0x0000u)		/* Cell 13 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL12_25_25V	((USHORT)0x0800u)		/* Cell 12 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL12_0V_5V	((USHORT)0x0000u)		/* Cell 12 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL11_25_25V	((USHORT)0x0400u)		/* Cell 11 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL11_0V_5V	((USHORT)0x0000u)		/* Cell 11 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL10_25_25V	((USHORT)0x0200u)		/* Cell 10 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL10_0V_5V	((USHORT)0x0000u)		/* Cell 10 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL9_25_25V	((USHORT)0x0100u)		/* Cell 9 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL9_0V_5V	((USHORT)0x0000u)		/* Cell 9 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL8_25_25V	((USHORT)0x0080u)		/* Cell 8 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL8_0V_5V	((USHORT)0x0000u)		/* Cell 8 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL7_25_25V	((USHORT)0x0040u)		/* Cell 7 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL7_0V_5V	((USHORT)0x0000u)		/* Cell 7 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL6_25_25V	((USHORT)0x0020u)		/* Cell 6 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL6_0V_5V	((USHORT)0x0000u)		/* Cell 6 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL5_25_25V	((USHORT)0x0010u)		/* Cell 5 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL5_0V_5V	((USHORT)0x0000u)		/* Cell 5 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL4_25_25V	((USHORT)0x0008u)		/* Cell 4 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL4_0V_5V	((USHORT)0x0000u)		/* Cell 4 voltage measurement range setting 0V - 5V */

/* 0x13 setting */
#define	BMIC_RSEL20_25_25V	((USHORT)0x0008u)		/* Cell 20 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL20_0V_5V	((USHORT)0x0000u)		/* Cell 20 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL19_25_25V	((USHORT)0x0004u)		/* Cell 19 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL19_0V_5V	((USHORT)0x0000u)		/* Cell 19 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL18_25_25V	((USHORT)0x0002u)		/* Cell 18 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL18_0V_5V	((USHORT)0x0000u)		/* Cell 18 voltage measurement range setting 0V - 5V */
#define	BMIC_RSEL17_25_25V	((USHORT)0x0001u)		/* Cell 17 voltage measurement range setting -2.5V - 2.5V */
#define	BMIC_RSEL17_0V_5V	((USHORT)0x0000u)		/* Cell 17 voltage measurement range setting 0V - 5V */

/* 0x14 setting */
/* bp1 */
#define	BMIC_AD_SEL_FIR		((USHORT)0x0002u)		/* Measurement system output digital filter selection: FIR(SINC) */
#define	BMIC_AD_SEL_IIR		((USHORT)0x0000u)		/* Measurement system output digital filter selection: IIR */
/* bp0 */
#define	BMIC_ADCCONV_LATCH		((USHORT)0x0001u)	/* AD Latch ON */
#define	BMIC_ADCCONV_NOLATCH	((USHORT)0x0000u)	/* AD Latch OFF */

/* 0x15 setting */
/* bp13 */
#define	BMIC_CLR_LFC_ON		((USHORT)0x2000u)		/* ON	: Life Counter Clear */
#define	BMIC_CLR_LFC_OFF	((USHORT)0x0000u)		/* OFF	: Life Counter Clear */
/* bp1 */
#define	BMIC_ADDIAG_SET_ON	((USHORT)0x0002u)		/* ON	: Set value for diagnosis about communication data selection */
#define	BMIC_ADDIAG_SET_OFF	((USHORT)0x0000u)		/* OFF	: Set value for diagnosis about communication data selection */

/* 0x16 setting */
/* bp15 */
#define	BMIC_ALM_TMOUT_20	((USHORT)0x8000u)		/* Alarm timeout during Stand-alone mode 2s */
#define	BMIC_ALM_TMOUT_05	((USHORT)0x0000u)		/* Alarm timeout during Stand-alone mode 0.5s */
/* bp14 - 13 */
#define	BMIC_INTTIM_8		((USHORT)0x6000u)		/* Intermittent time during Stand-alone mode 8s */
#define	BMIC_INTTIM_2		((USHORT)0x4000u)		/* Intermittent time during Stand-alone mode 2s */
#define	BMIC_INTTIM_1		((USHORT)0x2000u)		/* Intermittent time during Stand-alone mode 1s */
#define	BMIC_INTTIM_01		((USHORT)0x0000u)		/* Intermittent time during Stand-alone mode 0.1s */
/* bp12 */
#define	BMIC_ALM_SHDN_TOSHDN	((USHORT)0x1000u)	/* Shutdown after alarm timeout during Stand-alone mode */
#define	BMIC_ALM_SHDN_NOSHIFT	((USHORT)0x0000u)	/* No shutdown after alarm timeout during Stand-alone mode */
/* bp11 */
#define	BMIC_CALM_OE_ON		((USHORT)0x0800u)		/* COM-Alarm output enable: Enable */
/* bp10 */
#define	BMIC_CALM_DIR_1		((USHORT)0x0400u)		/* COM-Alarm communication direction: From upper to lower communication */
#define	BMIC_CALM_DIR_0		((USHORT)0x0000u)		/* COM-Alarm communication direction: From lower to upper communication */
/* bp9 */
#define	BMIC_CALM_IE_ON		((USHORT)0x0200u)		/* COM-Alarm input enable: Enable */
/* bp9 (COM-IC) */
#define	BMIC_CALM_IE_U_EN	((USHORT)0x0200u)		/* COM-Alarm input enable (Upper daisy I/F): Enable */
/* bp8 (COM-IC) */
#define	BMIC_CALM_IE_L_EN	((USHORT)0x0100u)		/* COM-Alarm input enable (lower daisy I/F): Enable */
/* bp5 */
#define	BMIC_SHDNTIM_SEL_SA	((USHORT)0x0020u)		/* Stand-alone after shutdown timer time passed  */
#define	BMIC_SHDNTIM_SEL_SHDN	((USHORT)0x0000u)	/* Shutdown after shutdown timer time passed */
/* bp4 - 0 */
#define	BMIC_SHDNTIM_960	((USHORT)0x001Fu)		/* Shutdown timer time 960min */
#define	BMIC_SHDNTIM_2		((USHORT)0x0002u)		/* Shutdown timer time 2min */
#define	BMIC_SHDNTIM_1		((USHORT)0x0001u)		/* Shutdown timer time 1min */
#define	BMIC_SHDNTIM_0		((USHORT)0x0000u)		/* Shutdown timer time 0min */

/* 0x24 setting */
/* bp11 */
#define	BMIC_DT12_F_1		((USHORT)0x0800u)		/* GPIO12 voltage change abnormality detected */
#define	BMIC_DT12_F_0		((USHORT)0x0000u)		/* GPIO12 voltage change abnormality not detected */
/* bp10 */
#define	BMIC_DT11_F_1		((USHORT)0x0400u)		/* GPIO11 voltage change abnormality detected */
#define	BMIC_DT11_F_0		((USHORT)0x0000u)		/* GPIO11 voltage change abnormality not detected */
/* bp9 */
#define	BMIC_DT10_F_1		((USHORT)0x0200u)		/* GPIO10 voltage change abnormality detected */
#define	BMIC_DT10_F_0		((USHORT)0x0000u)		/* GPIO10 voltage change abnormality not detected */
/* bp8 */
#define	BMIC_DT9_F_1		((USHORT)0x0100u)		/* GPIO9 voltage change abnormality detected */
#define	BMIC_DT9_F_0		((USHORT)0x0000u)		/* GPIO9 voltage change abnormality not detected */
/* bp7 */
#define	BMIC_DT8_F_1		((USHORT)0x0080u)		/* GPIO8 voltage change abnormality detected */
#define	BMIC_DT8_F_0		((USHORT)0x0000u)		/* GPIO8 voltage change abnormality not detected */
/* bp6 */
#define	BMIC_DT7_F_1		((USHORT)0x0040u)		/* GPIO7 voltage change abnormality detected */
#define	BMIC_DT7_F_0		((USHORT)0x0000u)		/* GPIO7 voltage change abnormality not detected */
/* bp5 */
#define	BMIC_DT6_F_1		((USHORT)0x0020u)		/* GPIO6 voltage change abnormality detected */
#define	BMIC_DT6_F_0		((USHORT)0x0000u)		/* GPIO6 voltage change abnormality not detected */
/* bp4 */
#define	BMIC_DT5_F_1		((USHORT)0x0010u)		/* GPIO5 voltage change abnormality detected */
#define	BMIC_DT5_F_0		((USHORT)0x0000u)		/* GPIO5 voltage change abnormality not detected */
/* bp3 */
#define	BMIC_DT4_F_1		((USHORT)0x0008u)		/* GPIO4 voltage change abnormality detected */
#define	BMIC_DT4_F_0		((USHORT)0x0000u)		/* GPIO4 voltage change abnormality not detected */
/* bp2 */
#define	BMIC_DT3_F_1		((USHORT)0x0004u)		/* GPIO3 voltage change abnormality detected */
#define	BMIC_DT3_F_0		((USHORT)0x0000u)		/* GPIO3 voltage change abnormality not detected */
/* bp1 */
#define	BMIC_DT2_F_1		((USHORT)0x0002u)		/* GPIO2 voltage change abnormality detected */
#define	BMIC_DT2_F_0		((USHORT)0x0000u)		/* GPIO2 voltage change abnormality not detected */
/* bp0 */
#define	BMIC_DT1_F_1		((USHORT)0x0001u)		/* GPIO1 voltage change abnormality detected */
#define	BMIC_DT1_F_0		((USHORT)0x0000u)		/* GPIO1 voltage change abnormality not detected */

/* 0x25 setting */
#define BMIC_REVID_950		((USHORT)0xC000u)		/* KA84950 */
#define BMIC_REVID_930		((USHORT)0xC400u)		/* KA84930 */
#define BMIC_REVID_922		((USHORT)0xC800u)		/* KA84922 */
#define BMIC_REVID_917		((USHORT)0xCC00u)		/* KA84917 */

/* 0x2C Event driven trigger */
#define BMIC_EV_BUSY_ABNORMAL	((USHORT)0x8000u)	/* Event-driven trigger busy error detection history abnormal*/
#define BMIC_EV_BUSY_NORMAL		((USHORT)0x0000u)	/* Event-driven trigger busy error detection history normal */
/* 0x2C bit definition — 通信错误状态寄存器 (Section 6.2.5.2 Table 6.2-8) */
#define BMIC_COM_ERR_SPI_F        ((USHORT)0x2000u)   /* bit13: SPI通信CRC错误 */
#define BMIC_COM_ERR_DCI_TMOUT_F  ((USHORT)0x0800u)   /* bit11: Daisy通信超时 */
#define BMIC_COM_ERR_DCU_UDR_F    ((USHORT)0x0200u)   /* bit9:  Daisy上行缓冲区下溢 */
#define BMIC_COM_ERR_DCL_UDR_F    ((USHORT)0x0100u)   /* bit8:  Daisy下行缓冲区下溢 */
#define BMIC_COM_ERR_DCU_OVR_F    ((USHORT)0x0080u)   /* bit7:  Daisy上行缓冲区溢出 */
#define BMIC_COM_ERR_DCL_OVR_F    ((USHORT)0x0040u)   /* bit6:  Daisy下行缓冲区溢出 */
#define BMIC_COM_ERR_DCU_CRC_F    ((USHORT)0x0020u)   /* bit5:  Daisy上行CRC错误 */
#define BMIC_COM_ERR_DCL_CRC_F    ((USHORT)0x0010u)   /* bit4:  Daisy下行CRC错误 */
#define BMIC_COM_ERR_DCU_PERI_F   ((USHORT)0x0008u)   /* bit3:  Daisy上行信号周期错误 */
#define BMIC_COM_ERR_DCL_PERI_F   ((USHORT)0x0004u)   /* bit2:  Daisy下行信号周期错误 */
#define BMIC_COM_ERR_ALL_MASK     ((USHORT)0x2FFCu)   /* bit[13:2] 全部通信错误位掩码 */
/* 0x98 setting */
#define BMIC_GPIO1_3_OUT_HIGH		((USHORT)0x1004u)
#define BMIC_GPIO1_3_OUT_LOW		((USHORT)0x0000u)
#define BMIC_GPIO1_3_OEN_ENABLE		((USHORT)0x0802u)
#define BMIC_GPIO1_3_OEN_DISABLE	((USHORT)0x0000u)
#define BMIC_GPIO2_OEN_ENABLE		((USHORT)0x0040u)
#define BMIC_GPIO2_OEN_DISABLE		((USHORT)0x0000u)
#define BMIC_GPIO2_OUT_HIGH			((USHORT)0x0080u)
#define BMIC_GPIO2_OUT_LOW			((USHORT)0x0000u)

/* 0x99 setting */
#define BMIC_GPIO4_OUT_HIGH			((USHORT)0x0004u)
#define BMIC_GPIO4_OUT_LOW			((USHORT)0x0000u)
#define BMIC_GPIO4_OEN_ENABLE		((USHORT)0x0002u)
#define BMIC_GPIO4_OEN_DISABLE		((USHORT)0x0000u)
#define BMIC_GPIO5_OUT_HIGH			((USHORT)0x0080u)
#define BMIC_GPIO5_OUT_LOW			((USHORT)0x0000u)
#define BMIC_GPIO5_OEN_ENABLE		((USHORT)0x0040u)
#define BMIC_GPIO5_OEN_DISABLE		((USHORT)0x0000u)
#define BMIC_GPIO4_6_OEN_ENABLE		((USHORT)0x0802u)
#define BMIC_GPIO4_6_OEN_DISABLE	((USHORT)0x0000u)

/* 0x9A setting */
#define BMIC_GPIO7_9_OUT_HIGH		((USHORT)0x1004u)
#define BMIC_GPIO7_9_OUT_LOW		((USHORT)0x0000u)
#define BMIC_GPIO7_9_OEN_ENABLE		((USHORT)0x0802u)
#define BMIC_GPIO7_9_OEN_DISABLE	((USHORT)0x0000u)
#define BMIC_GPIO8_OEN_ENABLE		((USHORT)0x0040u)
#define BMIC_GPIO8_OEN_DISABLE		((USHORT)0x0000u)

/* 0x9B setting */
#define BMIC_GPIO11_OUT_HIGH		((USHORT)0x0080u)
#define BMIC_GPIO11_OUT_LOW			((USHORT)0x0000u)
#define BMIC_GPIO11_OEN_ENABLE		((USHORT)0x0040u)
#define BMIC_GPIO11_OEN_DISABLE		((USHORT)0x0000u)
#define BMIC_GPIO10_OUT_HIGH		((USHORT)0x0004u)
#define BMIC_GPIO10_OUT_LOW			((USHORT)0x0000u)
#define BMIC_GPIO10_OEN_ENABLE		((USHORT)0x0002u)
#define BMIC_GPIO10_OEN_DISABLE		((USHORT)0x0000u)


/* 0x9C setting */
#define BMIC_GPIO13_OUT_HIGH		((USHORT)0x0004u)
#define BMIC_GPIO13_OUT_LOW			((USHORT)0x0000u)
#define BMIC_GPIO13_OEN_ENABLE		((USHORT)0x0002u)
#define BMIC_GPIO13_OEN_DISABLE		((USHORT)0x0000u)
#define BMIC_GPIO14_OEN_ENABLE		((USHORT)0x0040u)
#define BMIC_GPIO14_OEN_DISABLE		((USHORT)0x0000u)

/* 0x9D setting */
/* bp14 - 12 */
#define	BMIC_DCI_OS_200MV	((USHORT)0x0000u)		/* Daisy RX offset voltage setting :  200mV(default) */
#define	BMIC_DCI_OS_250MV	((USHORT)0x1000u)		/* Daisy RX offset voltage setting :  250mV */
#define	BMIC_DCI_OS_300MV	((USHORT)0x2000u)		/* Daisy RX offset voltage setting :  300mV */
#define	BMIC_DCI_OS_350MV	((USHORT)0x3000u)		/* Daisy RX offset voltage setting :  350mV */
#define	BMIC_DCI_OS_400MV	((USHORT)0x4000u)		/* Daisy RX offset voltage setting :  400mV */
#define	BMIC_DCI_OS_450MV	((USHORT)0x5000u)		/* Daisy RX offset voltage setting :  450mV */
#define	BMIC_DCI_OS_500MV	((USHORT)0x6000u)		/* Daisy RX offset voltage setting :  500mV */
#define	BMIC_DCI_OS_550MV	((USHORT)0x7000u)		/* Daisy RX offset voltage setting :  550mV */
/* bp11 - 10 */
#define	BMIC_TSDL_TH_170	((USHORT)0x0C00u)		/* Over-temperature threshold for TSDL :  170C */
#define	BMIC_TSDL_TH_160	((USHORT)0x0800u)		/* Over-temperature threshold for TSDL :  160C */
#define	BMIC_TSDL_TH_150	((USHORT)0x0400u)		/* Over-temperature threshold for TSDL :  150C */
#define	BMIC_TSDL_TH_135	((USHORT)0x0000u)		/* Over-temperature threshold for TSDL :  135C */
/* bp9 - 8 */
#define	BMIC_UVATSW_UVTH_14V	((USHORT)0x0300u)	/* Under voltage threshold of VBATSW for CBVBATSW control : 14V */
#define	BMIC_UVATSW_UVTH_13V	((USHORT)0x0200u)	/* Under voltage threshold of VBATSW for CBVBATSW control : 13V */
#define	BMIC_UVATSW_UVTH_12V	((USHORT)0x0100u)	/* Under voltage threshold of VBATSW for CBVBATSW control : 12V */
#define	BMIC_UVATSW_UVTH_11V	((USHORT)0x0000u)	/* Under voltage threshold of VBATSW for CBVBATSW control : 11V(default) */
/* bp7 */
#define	BMIC_DCI_SYNCON		((USHORT)0x0008u)		/* ON :Latch timing sychronization between slave device */
#define	BMIC_DCI_SYNCOFF	((USHORT)0x0000u)		/* OFF:Latch timing sychronization between slave device */

/* 0x9F setting */
/* bp0 */
#define	BMIC_DCI_SYNC_ON	((USHORT)0x0001u)		/* Synchronized broadcast write ON */

/* 0xA1 setting */
/* bp2 */
#define	BMIC_CB_HIST_CLR_ON		((USHORT)0x0004u)	/* ON :Cell balance history reset */
#define	BMIC_CB_HIST_CLR_OFF	((USHORT)0x0000u)	/* OFF :Cell balance history reset */
/* bp1 */
#define	BMIC_CB_HIST_MODE_5M	((USHORT)0x0002u)	/* 5min/step :Resolution setting for cell balance history function */
#define	BMIC_CB_HIST_MODE_15M	((USHORT)0x0000u)	/* 15min/step :Resolution setting for cell balance history function */
/* bp0 */
#define	BMIC_CB_AUTOOFF_ON		((USHORT)0x0001u)	/* ON :Enable for cell balancing automatic OFF function */
#define	BMIC_CB_AUTOOFF_OFF		((USHORT)0x0000u)	/* OFF :Enable for cell balancing automatic OFF function */

/* 0xB0 setting */
/* bp15 - 13 */
#define	BMIC_RECFG_RECON	((USHORT)0xA000u)		/* Reconfiguration setting for Daisy I/F: Reconfiguration(101) */
#define	BMIC_RECFG_RECOFF	((USHORT)0x0000u)		/* Reconfiguration setting for Daisy I/F: Reconfiguration(Other than 101) */
/* bp12 */
#define	BMIC_NENDMY_FWD_NON	((USHORT)0x1000u)		/* Without dummy communication in advance of devices setted with COM1END or COM2END */
#define	BMIC_NENDMY_FWD_ON	((USHORT)0x0000u)		/* With dummy communication in advance of devices setted with COM1END or COM2END */
/* bp11 - 6 */
#define	BMIC_COM2END_SET	((USHORT)0x0000u)		/* Final device address setting of daisy IF2 Communication I/F */
/* bp5 - 0 */
#define	BMIC_COM1END_SET	((USHORT)0x0001u)		/* Final device address setting of daisy IF1 Communication I/F */

/* 0xB1 setting */
/* bp15 - 14 */
#define	BMIC_DSCN_CR_384UA	((USHORT)0xC000u)		/* 11: 384 micro A :Current source setting for disconnect detection */
#define	BMIC_DSCN_CR_256UA	((USHORT)0x8000u)		/* 10: 256 micro A :Current source setting for disconnect detection */
#define	BMIC_DSCN_CR_192UA	((USHORT)0x4000u)		/* 01: 192 micro A :Current source setting for disconnect detection */
#define	BMIC_DSCN_CR_64UA	((USHORT)0x0000u)		/* 01: 64 micro A :Current source setting for disconnect detection */
/* bp13 - 11 */
#define	BMIC_DSCN_TM_3929	((USHORT)0x3800u)		/* 111: 392.9 micro sec per cell :Current ON time setting for disconnect detection */
#define	BMIC_DSCN_TM_3128	((USHORT)0x3000u)		/* 110: 312.8 micro sec per cell :Current ON time setting for disconnect detection */
#define	BMIC_DSCN_TM_2327	((USHORT)0x2800u)		/* 101: 232.7 micro sec per cell :Current ON time setting for disconnect detection */
#define	BMIC_DSCN_TM_1526	((USHORT)0x2000u)		/* 100: 152.6 micro sec per cell :Current ON time setting for disconnect detection */
#define	BMIC_DSCN_TM_1126	((USHORT)0x1800u)		/* 011: 112.6 micro sec per cell :Current ON time setting for disconnect detection */
#define	BMIC_DSCN_TM_726	((USHORT)0x1000u)		/* 010: 72.6 micro sec per cell :Current ON time setting for disconnect detection */
#define	BMIC_DSCN_TM_325	((USHORT)0x0800u)		/* 001: 32.5 micro sec per cell :Current ON time setting for disconnect detection */
#define	BMIC_DSCN_TM_126	((USHORT)0x0000u)		/* 000: 12.6 micro sec per cell :Current ON time setting for disconnect detection */
/* bp10 - 9 */
#define	BMIC_FIR_DPT_M_64	((USHORT)0x0600u)		/* 11: 64 :Monitoring system SINC filter coefficient */
#define	BMIC_FIR_DPT_M_32	((USHORT)0x0400u)		/* 10: 32 :Monitoring system SINC filter coefficient */
#define	BMIC_FIR_DPT_M_16	((USHORT)0x0200u)		/* 01: 16 :Monitoring system SINC filter coefficient */
#define	BMIC_FIR_DPT_M_8	((USHORT)0x0000u)		/* 00: 8 :Monitoring system SINC filter coefficient */
/* bp8 - 7 */
#define	BMIC_FIR_SET_M_S3	((USHORT)0x0180u)		/* 11: SINC3 :Monitoring system SINC filter order setting */
#define	BMIC_FIR_SET_M_S2	((USHORT)0x0100u)		/* 10: SINC2 :Monitoring system SINC filter order setting */
#define	BMIC_FIR_SET_M_S1	((USHORT)0x0080u)		/* 01: SINC1 :Monitoring system SINC filter order setting */
#define	BMIC_FIR_SET_M_OFF	((USHORT)0x0000u)		/* 00: OFF :Monitoring system SINC filter order setting */
/* bp4 - 3 */
#define	BMIC_AD_SET_M_11	((USHORT)0x0018u)		/* 11: 12bit(cyclic 4bit) : Incremental bit number setting of monitoring ADC */
#define	BMIC_AD_SET_M_10	((USHORT)0x0010u)		/* 10: 10bit(cyclic 6bit) : Incremental bit number setting of monitoring ADC */
#define	BMIC_AD_SET_M_7		((USHORT)0x0008u)		/* 01: 7bit(cyclic 9bit) : Incremental bit number setting of monitoring ADC */
#define	BMIC_AD_SET_M_5		((USHORT)0x0000u)		/* 00: 5bit(cyclic 11bit) : Incremental bit number setting of monitoring ADC */
/* bp2 - 1 */
#define	BMIC_ADC_MODE_M_1X	((USHORT)0x0002u)		/* 1X: Bias ON in synchronization with the measurement system and immedeate measurement starts after setting EV_SET bit */
#define	BMIC_ADC_MODE_M_01	((USHORT)0x0001u)		/* 01: After setting the EV_SET bit, bias is turned on in synchronization with the mesurement system and measurement start */
#define	BMIC_ADC_MODE_M_00	((USHORT)0x0000u)		/* 00: Immediate bias is turned on and measurement start after setting EV_SET bit (default) */

/* 0xB2 setting */
/* bp15 - 11 */
#define	BMIC_EV_CELL_M_25_MAX	((USHORT)0xF800u)	/* 11111: cell 25 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_25_MIN	((USHORT)0xC800u)	/* 11001: cell 25 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_25		((USHORT)0xC000u)	/* 11000: cell 25 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_24		((USHORT)0xB800u)	/* 10111: cell 24 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_23		((USHORT)0xB000u)	/* 10110: cell 23 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_22		((USHORT)0xA100u)	/* 10101: cell 22 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_21		((USHORT)0xA000u)	/* 10100: cell 21 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_20		((USHORT)0x9800u)	/* 10011: cell 20 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_19		((USHORT)0x9000u)	/* 10010: cell 19 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_18		((USHORT)0x8800u)	/* 10001: cell 18 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_17		((USHORT)0x8000u)	/* 10000: cell 17 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_16		((USHORT)0x7800u)	/* 01111: cell 16 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_15		((USHORT)0x7000u)	/* 01110: cell 15 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_14		((USHORT)0x6800u)	/* 01101: cell 14 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_13		((USHORT)0x6000u)	/* 01100: cell 13 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_12		((USHORT)0x5800u)	/* 01011: cell 12 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_11		((USHORT)0x5000u)	/* 01010: cell 11 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_10		((USHORT)0x4800u)	/* 01001: cell 10 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_9		((USHORT)0x4000u)	/* 01000: cell 9 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_8		((USHORT)0x3800u)	/* 00111: cell 8 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_7		((USHORT)0x3000u)	/* 00110: cell 7 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_6		((USHORT)0x2800u)	/* 00101: cell 6 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_5		((USHORT)0x2000u)	/* 00100: cell 5 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_4		((USHORT)0x1800u)	/* 00011: cell 4 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_3		((USHORT)0x1000u)	/* 00010: cell 3 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_2		((USHORT)0x0800u)	/* 00001: cell 2 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
#define	BMIC_EV_CELL_M_1		((USHORT)0x0000u)	/* 00000: cell 1 :Target cell setting for single cell continuous measurement mode of monitoring ADC */
/* bp10 */
#define	BMIC_EV_DSCN_SKIP_OFF	((USHORT)0x0400u)	/* 1: No measure(no turn on the disconnection detection current source of cell terminal) :Cell disconnection detection sequence setting */
#define	BMIC_EV_DSCN_SKIP_ON	((USHORT)0x0000u)	/* 0: Turn on the disconnection detection current source for all Cn terminals :Cell disconnection detection sequence setting */
/* bp9 - 7 */
#define	BMIC_EV_MSEL_SINCELCON_MAX	((USHORT)0x0380u)	/* 111: single cell continuous measurement :Measurement target setting for monitoring ADC(event-driven sequence) */
#define	BMIC_EV_MSEL_SINCELCON_MIN	((USHORT)0x0280u)	/* 101: single cell continuous measurement :Measurement target setting for monitoring ADC(event-driven sequence) */
#define	BMIC_EV_MSEL_VDAC			((USHORT)0x0200u)	/* 100: VDAC measurement :Measurement target setting for monitoring ADC(event-driven sequence) */
#define	BMIC_EV_MSEL_CELGPINPWSP	((USHORT)0x0180u)	/* 011: cell, GPIO and  internal power supply voltage measurement :Measurement target setting for monitoring ADC(event-driven sequence) */
#define	BMIC_EV_MSEL_GPINPWSP		((USHORT)0x0100u)	/* 010: GPIO and internal power supply voltage measurement :Measurement target setting for monitoring ADC(event-driven sequence) */
#define	BMIC_EV_MSEL_CELVOL			((USHORT)0x0080u)	/* 001: cell voltage measurement :Measurement target setting for monitoring ADC(event-driven sequence) */
#define	BMIC_EV_MSEL_OFF			((USHORT)0x0000u)	/* 000: single cell continuous measurement :Measurement target setting for monitoring ADC(event-driven sequence) */
/* bp1 */
#define	BMIC_EV_DSCN_ON		((USHORT)0x0002u)		/* 1: cell disconnection detection sequence :Cell disconnection detection sequence setting */
#define	BMIC_EV_DSCN_OFF	((USHORT)0x0000u)		/* 0: generally :Cell disconnection detection sequence setting */
/* bp0 */
#define	BMIC_EV_SET_ON		((USHORT)0x0001u)		/* 1: start of monitoring system voltagge measurement event or cell disconnection detection sequence :Event trigger setting */
#define	BMIC_EV_SET_OFF		((USHORT)0x0000u)		/* 0: No event :Event trigger setting */

/*0xB4 setting */
/* bp15 - 13 */
#define	BMIC_CB_MSK2_M_360	((USHORT)0xE000u)		/* 111: 360 usec :Duration to prevent cell balancing before monitoring cell voltage measurement */
#define	BMIC_CB_MSK2_M_320	((USHORT)0xC000u)		/* 110: 320 usec :Duration to prevent cell balancing before monitoring cell voltage measurement */
#define	BMIC_CB_MSK2_M_280	((USHORT)0xA000u)		/* 101: 280 usec :Duration to prevent cell balancing before monitoring cell voltage measurement */
#define	BMIC_CB_MSK2_M_240	((USHORT)0x8000u)		/* 100: 240 usec :Duration to prevent cell balancing before monitoring cell voltage measurement */
#define	BMIC_CB_MSK2_M_200	((USHORT)0x6000u)		/* 011: 200 usec :Duration to prevent cell balancing before monitoring cell voltage measurement */
#define	BMIC_CB_MSK2_M_160	((USHORT)0x4000u)		/* 010: 160 usec :Duration to prevent cell balancing before monitoring cell voltage measurement */
#define	BMIC_CB_MSK2_M_120	((USHORT)0x2000u)		/* 001: 120 usec :Duration to prevent cell balancing before monitoring cell voltage measurement */
#define	BMIC_CB_MSK2_M_80	((USHORT)0x0000u)		/* 000:  80 usec :Duration to prevent cell balancing before monitoring cell voltage measurement */
/* bp8 */
#define	BMIC_EN_MSOCB_M_ON	((USHORT)0x0100u)		/* 1: Enable :Measurement directional cell balance of monitoring system enable */
#define	BMIC_EN_MSOCB_M_OFF	((USHORT)0x0000u)		/* 0: Disable:Measurement directional cell balance of monitoring system enable */
/* bp7 - 5 */
#define	BMIC_CB_MSK2_360	((USHORT)0x00E0u)		/* 111: 360 usec :Duration to prevent cell balancing before measurement cell voltage measurement */
#define	BMIC_CB_MSK2_320	((USHORT)0x00C0u)		/* 110: 320 usec :Duration to prevent cell balancing before measurement cell voltage measurement */
#define	BMIC_CB_MSK2_280	((USHORT)0x00A0u)		/* 101: 280 usec :Duration to prevent cell balancing before measurement cell voltage measurement */
#define	BMIC_CB_MSK2_240	((USHORT)0x0080u)		/* 100: 240 usec :Duration to prevent cell balancing before measurement cell voltage measurement */
#define	BMIC_CB_MSK2_200	((USHORT)0x0060u)		/* 011: 200 usec :Duration to prevent cell balancing before measurement cell voltage measurement */
#define	BMIC_CB_MSK2_160	((USHORT)0x0040u)		/* 010: 160 usec :Duration to prevent cell balancing before measurement cell voltage measurement */
#define	BMIC_CB_MSK2_120	((USHORT)0x0020u)		/* 001: 120 usec :Duration to prevent cell balancing before measurement cell voltage measurement */
#define	BMIC_CB_MSK2_80		((USHORT)0x0000u)		/* 000:  80 usec :Duration to prevent cell balancing before measurement cell voltage measurement */
/* bp0 */
#define	BMIC_EN_MSOCB_ON	((USHORT)0x0001u)		/* 1: Enable :Measurement directional cell balance of measurement sysytem enable */
#define	BMIC_EN_MSOCB_OFF	((USHORT)0x0000u)		/* 0: Disable:Measurement directional cell balance of measurement sysytem enable */

/* 0xB5 setting */
/* bp15 */
#define	BMIC_ALM_OE_ON		((USHORT)0x8000u)		/* 1: ON:ALMOUT terminal output enable */
#define	BMIC_ALM_OE_OFF		((USHORT)0x0000u)		/* 1: OFF:ALMOUT terminal output enable */
/* bp14 - 3 */
#define	BMIC_ALM_MASK_ADC		((USHORT)0x4000u)	/* 100000000000: not target alarm for ADC:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_SA		((USHORT)0x2000u)	/* 010000000000: not target alarm for SA:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_OTP		((USHORT)0x1000u)	/* 001000000000: not target alarm for OTP:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_TSD		((USHORT)0x0800u)	/* 000100000000: not target alarm for TSD:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_OSC		((USHORT)0x0400u)	/* 000010000000: not target alarm for OSC:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_VBATSW	((USHORT)0x0200u)	/* 000001000000: not target alarm for VBATSW:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_VDD18		((USHORT)0x0100u)	/* 000000100000: not target alarm for VDD18:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_VDD50		((USHORT)0x0080u)	/* 000000010000: not target alarm for VDD50:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_UT		((USHORT)0x0040u)	/* 000000001000: not target alarm for UT:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_OT		((USHORT)0x0020u)	/* 000000000100: not target alarm for OT:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_UV		((USHORT)0x0010u)	/* 000000000010: not target alarm for UV:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_OV		((USHORT)0x0008u)	/* 000000000001: not target alarm for OV:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_INIT		((USHORT)0x7FE0u)	/* 111111111100: not alarm target for all except UV and OV:Alarm sigunal mask setting */
#define	BMIC_ALM_MASK_ALLOFF	((USHORT)0x0000u)	/* 000000000000: alarm target for all:Alarm sigunal mask setting */
/* bp2 */
#define	BMIC_ALM_HB_EN_PULSE	((USHORT)0x0004u)	/* 1: Output heart beat pulse for SV state (all cell voltage is within OV/UV threshold):ALMOUT terminal heart beat pulse output enable */
#define	BMIC_ALM_HB_EN_LOW		((USHORT)0x0000u)	/* 0: Output low for SV state:ALMOUT terminal heart beat pulse output enable */
/* bp1 - 0 */
#define	BMIC_ALM_HB_PT_100MS	((USHORT)0x0003u)	/* 11: 100msec :ALMOUT terminal heart beat pulse period */
#define	BMIC_ALM_HB_PT_10MS		((USHORT)0x0002u)	/* 10: 10msec :ALMOUT terminal heart beat pulse period */
#define	BMIC_ALM_HB_PT_5MS		((USHORT)0x0001u)	/* 01: 5msec :ALMOUT terminal heart beat pulse period */
#define	BMIC_ALM_HB_PT_1MS		((USHORT)0x0000u)	/* 00: 1msec(default) :ALMOUT terminal heart beat pulse period */

/* Other command */
#define BMIC_COMMONSEND		((UCHAR)0x00u)
#define BMIC_DEVICETOPADD	((UCHAR)0x01u)
#define BMIC_GRPIDFIRST		((UCHAR)0x38u)
#define BMIC_GRPIDLAST		((UCHAR)0x3Fu)
#define BMIC_GRPID_950		((UCHAR)0x3Fu)
#define BMIC_GRPID_930		((UCHAR)0x3Eu)
#define BMIC_GRPID_917		((UCHAR)0x3Du)
#define BMIC_GRPID_922		((UCHAR)0x00u)
#define BMIC_ADDRESS_922	((UCHAR)0x02u)
#define BMIC_ADDRESS_917	((UCHAR)0x01u)
/* PMIC setting */
/* Control register group 1 */
#define	CMIC_CNTADR_0C	 	((UCHAR)0x0Cu)
#define	CMIC_CNTADR_17	 	((UCHAR)0x17u)
/* Status register group 1 */
#define	CMIC_STSADR_2F		((UCHAR)0x2Fu)
/* Control register group 2 */
#define	CMIC_CNTADR_30	 	((UCHAR)0x30u)
#define	CMIC_TX1500_31	 	((UCHAR)0x31u)
#define	CMIC_TX3116_32	 	((UCHAR)0x32u)
#define	CMIC_RX1500_33	 	((UCHAR)0x33u)
#define	CMIC_RX3116_34	 	((UCHAR)0x34u)
#define	CMIC_CNTADR_35	 	((UCHAR)0x35u)
#define	CMIC_CNTADR_48	 	((UCHAR)0x48u)
#define	CMIC_CNTADR_48	 	((UCHAR)0x48u)
#define	CMIC_CNTADR_5C	 	((UCHAR)0x5Cu)
#define	CMIC_CNTADR_5D	 	((UCHAR)0x5Du)
#define	CMIC_CNTADR_5E	 	((UCHAR)0x5Eu)
#define	CMIC_CNTADR_5F	 	((UCHAR)0x5Fu)
#define	CMIC_CNTADR_60	 	((UCHAR)0x60u)
#define	CMIC_CNTADR_61	 	((UCHAR)0x61u)
#define	CMIC_CNTADR_68	 	((UCHAR)0x68u)
#define	CMIC_CNTADR_69	 	((UCHAR)0x69u)
#define	CMIC_CNTADR_6A	 	((UCHAR)0x6Au)
/* Data register group 1 */
#define	CMIC_VDD50_ADR6D	((UCHAR)0x6Du)
#define	CMIC_GPIO_ADR88		((UCHAR)0x88u)
/* Control register group 3 */
#define	CMIC_CNTADR_9F	 	((UCHAR)0x9Fu)
/* Status register group 2 */
#define	CMIC_STSADR_C6		((UCHAR)0xC6u)
/* Data register group 2 */
#define	CMIC_FIFO_C_P_C7	((UCHAR)0xC7u)
#define	CMIC_FIFO_V_P_C8	((UCHAR)0xC8u)
#define	CMIC_FIFO_C_S_C9	((UCHAR)0xC9u)

/* 0x0C setting */
/* bp10 */
#define	CMIC_PWM3_SET_PWM	((USHORT)0x0400u)	/* PWM3   setting */
#define	CMIC_PWM3_SET_GPIO	((USHORT)0x0000u)	/* GPIO10 setting */
/* bp4 */
#define	CMIC_ALM_SET_ON		((USHORT)0x0010u)	/* Alarm on */
#define	CMIC_ALM_SET_OFF	((USHORT)0x0000u)	/* Alarm off */

/* 0x14 setting */
/* bp4 */
#define	CMIC_AUX_LATCH		((USHORT)0x0010u)	/* AUX latch ON */
#define	CMIC_AUX_NOLATCH	((USHORT)0x0000u)	/* AUX latch OFF */

/* 0x17 setting */
/* bp2 */
#define	CMIC_PLLCKSEL_PLL	((USHORT)0x0004u)	/* PLL selection */
#define	CMIC_PLLCKSEL_OSC	((USHORT)0x0000u)	/* Internal OSC selection */
/* bp1 */
#define	CMIC_PLL_NPD_ON		((USHORT)0x0002u)	/* PLL ON */
#define	CMIC_PLL_NPD_OFF	((USHORT)0x0000u)	/* PLL OFF */
/* bp0 */
#define	CMIC_EXOSC_NPD_ON	((USHORT)0x0001u)	/* XO ON */
#define	CMIC_EXOSC_NPD_OFF	((USHORT)0x0000u)	/* XO OFF */

/* 0x30 setting */
/* bp11 */
#define	CMIC_CSON_2_ON		((USHORT)0x0800u)	/* GCS2: Force ON */
#define	CMIC_CSON_2_OFF		((USHORT)0x0000u)	/* GCS2: Auto */
/* bp10 */
#define	CMIC_CSON_1_ON		((USHORT)0x0400u)	/* GCS1: Force ON */
#define	CMIC_CSON_1_OFF		((USHORT)0x0000u)	/* GCS1: Auto */
/* bp8 */
#define	CMIC_MAN_CS_ON		((USHORT)0x0100u)	/* GPSPI CS: Manual control */
#define	CMIC_MAN_CS_OFF		((USHORT)0x0000u)	/* GPSPI CS: Auto control */

/* bp1 - 0 */
#define	CMIC_TALK_GPSPI1ON	((USHORT)0x0001u)	/* GPSPI1 I/F: Transmission ON */
#define	CMIC_TALK_GPSPI2ON	((USHORT)0x0002u)	/* GPSPI2 I/F: Transmission ON */

/* 0x35 setting */
/* bp4 - 0 */
#define	CMIC_BIT_LENGTH_1_7	((USHORT)0x0007u)	/* GPSPI1 flame bits: 8bit */
#define	CMIC_BIT_LENGTH_1_32	((USHORT)0x001Fu)	/* GPSPI1 flame bits: 32bit */
/* bp10*/
#define	CMIC_BIT_GPSPI_CS1_P	((USHORT)0x0000u)	/* GPSPI1 I/F GCS1 polarity:  Positive */
#define	CMIC_BIT_GPSPI_CS1_N	((USHORT)0x0400u)	/* GPSPI1 I/F GCS1 polarity:  Negative */
/* bp9*/
#define	CMIC_BIT_GPSPI_CL1_EDGE ((USHORT)0x0000u)	/* GPSPI1 I/F GSCL1 shift/latch edge: 0:↑/↓    */
/* bp8*/
#define	CMIC_BIT_GPSPI_DO1_POLARITY ((USHORT)0x0000u)	/* GPSPI1 I/F GSDO1 value during idle: 0: HIGH*/
/* bp7*/
#define	CMIC_BIT_GPSPI_DI1_POLARITY ((USHORT)0x0000u)	/* GPSPI1 I/F GSDI1 shift edge setting: 0:↑/↓*/
/* bp6 - 5*/
#define	CMIC_BIT_GPSPI_RATE0 ((USHORT)0x0000u)       /* Communication speed of GPSPI1 I/F:  00: 3.4Mbps*/
#define	CMIC_BIT_GPSPI_RATE1 ((USHORT)0x0020u)       /* Communication speed of GPSPI1 I/F:  01: 2Mbps*/
#define	CMIC_BIT_GPSPI_RATE2 ((USHORT)0x0040u)       /* Communication speed of GPSPI1 I/F:  10: 1Mbps*/
#define	CMIC_BIT_GPSPI_RATE3 ((USHORT)0x0060u)       /* Communication speed of GPSPI1 I/F:  01: 0.256Mbps*/




/* 0x48 setting */
/* bp4 - 0 */
#define	CMIC_BIT_LENGTH_2_32	((USHORT)0x001Fu)	/* GPSPI2 flame bits: 32bit */

/* 0x5C setting */
/* bp15 */
#define	CMIC_NPD_C_P_ON		((USHORT)0x8000u)	/* Primary CS ADC ON */
#define	CMIC_NPD_C_P_OFF	((USHORT)0x0000u)	/* Primary CS ADC OFF */
/* bp14 */
#define	CMIC_NPD_V_P_ON		((USHORT)0x4000u)	/* Primary VBAT ADC ON */
#define	CMIC_NPD_V_P_OFF	((USHORT)0x0000u)	/* Primary VBAT ADC OFF */
/* bp13 */
#define	CMIC_NPD_C_S_ON		((USHORT)0x2000u)	/* Secondary CS ADC ON */
#define	CMIC_NPD_C_S_OFF	((USHORT)0x0000u)	/* Secondary CS ADC OFF */
/* bp12 */
#define	CMIC_NPD_OCD_P_ON	((USHORT)0x1000u)	/* Primary OCD ADC ON */
#define	CMIC_NPD_OCD_P_OFF	((USHORT)0x0000u)	/* Primary OCD ADC OFF */
/* bp11 */
#define	CMIC_NPD_OCD_S_ON	((USHORT)0x0800u)	/* Secondary OCD ADC ON */
#define	CMIC_NPD_OCD_S_OFF	((USHORT)0x0000u)	/* Secondary OCD ADC OFF */
/* bp10 */
#define	CMIC_NPD_AUX_ON		((USHORT)0x0400u)	/* AUX ADC ON */
#define	CMIC_NPD_AUX_OFF	((USHORT)0x0000u)	/* AUX ADC OFF */
/* bp7 - 6 */
#define	CMIC_AUXMODE_10		((USHORT)0x0080u)	/* VBATP-VBATN: AUX Measurement Mode */
#define	CMIC_AUXMODE_01		((USHORT)0x0040u)	/* Basic cycle: AUX Measurement Mode */
#define	CMIC_AUXMODE_00		((USHORT)0x0000u)	/* No Measure:  AUX Measurement Mode */
/* bp4 */
#define	CMIC_DSP_C_P_FR		((USHORT)0x0010u)	/* FR mode Primary CS */
#define	CMIC_DSP_C_P_EV		((USHORT)0x0000u)	/* EV mode Primary CS */
/* bp3 */
#define	CMIC_DSP_V_P_FR		((USHORT)0x0008u)	/* FR mode Primary VBAT */
#define	CMIC_DSP_V_P_EV		((USHORT)0x0000u)	/* EV mode Primary VBAT */
/* bp2 */
#define	CMIC_DSP_C_S_FR		((USHORT)0x0004u)	/* FR mode Secondary CS */
#define	CMIC_DSP_C_S_EV		((USHORT)0x0000u)	/* EV mode Secondary CS */
/* bp0 */
#define	CMIC_NSTP_PWM_OUT	((USHORT)0x0001u)	/* PWM1,2,3 Output Enable */
#define	CMIC_NSTP_PWM_LOW	((USHORT)0x0000u)	/* PWM1,2,3 Low Fix */

/* 0x5D setting */
/* bp15 - 14 */
#define	CMIC_CC_INTVL_P_102450	((USHORT)0xC000u)	/* 102.45ms Measurement time Primary CS & VBAT */
#define	CMIC_CC_INTVL_P_12850	((USHORT)0x8000u)	/* 12.85ms  Measurement time Primary CS & VBAT */
#define	CMIC_CC_INTVL_P_850		((USHORT)0x4000u)	/* 850us    Measurement time Primary CS & VBAT */
#define	CMIC_CC_INTVL_P_100		((USHORT)0x0000u)	/* 100us    Measurement time Primary CS & VBAT */
/* bp8 - 7 */
#define	CMIC_CC_INTVL_S_102450	((USHORT)0x0180u)	/* 102.45ms Measurement time Secondary CS */
#define	CMIC_CC_INTVL_S_12850	((USHORT)0x0100u)	/* 12.85ms  Measurement time Secondary CS */
#define	CMIC_CC_INTVL_S_850		((USHORT)0x0080u)	/* 850us    Measurement time Secondary CS */
#define	CMIC_CC_INTVL_S_100		((USHORT)0x0000u)	/* 100us    Measurement time Secondary CS */
/* other */
#define	CMIC_REG_5D_RESERVED1	((USHORT)0x3863u)	/* Reserved Must be set to "1" */

/* 0x5E setting */
/* bp15 - 14 */
#define	CMIC_GAIN_PROG_P_256	((USHORT)0xC000u)	/* Gain x256 Primary CS */
#define	CMIC_GAIN_PROG_P_64		((USHORT)0x8000u)	/* Gain x64 Primary CS */
#define	CMIC_GAIN_PROG_P_16		((USHORT)0x4000u)	/* Gain x16 Primary CS */
#define	CMIC_GAIN_PROG_P_4		((USHORT)0x0000u)	/* Gain x4  Primary CS */
/* bp14 - 12 */
#define	CMIC_GAIN_PROG_S_256	((USHORT)0x3000u)	/* Gain x256 Secondary CS */
#define	CMIC_GAIN_PROG_S_64		((USHORT)0x2000u)	/* Gain x64 Secondary CS */
#define	CMIC_GAIN_PROG_S_16		((USHORT)0x1000u)	/* Gain x16 Secondary CS */
#define	CMIC_GAIN_PROG_S_4		((USHORT)0x0000u)	/* Gain x4  Secondary CS */
/* bp0 */
#define	CMIC_EXOSC_CHDRV_5MA	((USHORT)0x0001u)	/* 5mA Drivability of XO */
#define	CMIC_EXOSC_CHDRV_1MA	((USHORT)0x0000u)	/* 1mA Drivability of XO */

/* 0x5F setting */
/* bp11 */
#define	CMIC_OCPWM3_EN_ON		((USHORT)0x0800u)	/* PWM3 duty change */
#define	CMIC_OCPWM3_EN_OFF		((USHORT)0x0000u)	/* PWM3 always 10% duty */
/* bp5 - 4 */
#define	CMIC_OCPWM3_FQ_2KHZ		((USHORT)0x0030u)	/* 2kHz   PWM3 frequency */
#define	CMIC_OCPWM3_FQ_10KHZ	((USHORT)0x0020u)	/* 10kHz  PWM3 frequency */
#define	CMIC_OCPWM3_FQ_100KHZ	((USHORT)0x0010u)	/* 100kHz PWM3 frequency */
#define	CMIC_OCPWM3_FQ_400KHZ	((USHORT)0x0000u)	/* 400kHz PWM3 frequency */

/* 0x60 setting */
/* bp15 - 12 */
#define	CMIC_OCNF_P_1600	((USHORT)0xF000u)	/* 1600us OC noise filter Primary OCD */
#define	CMIC_OCNF_P_1400	((USHORT)0xD000u)	/* 1400us OC noise filter Primary OCD */
#define	CMIC_OCNF_P_1200	((USHORT)0xB000u)	/* 1200us OC noise filter Primary OCD */
#define	CMIC_OCNF_P_1000	((USHORT)0x9000u)	/* 1000us OC noise filter Primary OCD */
#define	CMIC_OCNF_P_400		((USHORT)0x3000u)	/* 400us  OC noise filter Primary OCD */
#define	CMIC_OCNF_P_200		((USHORT)0x1000u)	/* 200us  OC noise filter Primary OCD */
#define	CMIC_OCNF_P_100		((USHORT)0x0000u)	/* 100us  OC noise filter Primary OCD */
/* bp11 - 10 */
#define	CMIC_OCTH_EN_P_DIS	((USHORT)0x0C00u)	/* Inactive OC comparator Primary OCD */
#define	CMIC_OCTH_EN_P_N	((USHORT)0x0800u)	/* Negative threshold enable OC comparator Primary OCD */
#define	CMIC_OCTH_EN_P_P	((USHORT)0x0400u)	/* Positive threshold enable OC comparator Primary OCD */
#define	CMIC_OCTH_EN_P_PAN	((USHORT)0x0000u)	/* Positive & Negative threshold enable OC comparator Primary OCD */
/* bp9 - 5 */
#define	CMIC_OCTHP_P_100	((USHORT)0x03C0u)	/* 100mV Positive OC threshold Primary OCD */
#define	CMIC_OCTHP_P_80		((USHORT)0x02C0u)	/* 80mV  Positive OC threshold Primary OCD */
#define	CMIC_OCTHP_P_50		((USHORT)0x0140u)	/* 50mV  Positive OC threshold Primary OCD */
#define	CMIC_OCTHP_P_25		((USHORT)0x0000u)	/* 25mV  Positive OC threshold Primary OCD */
/* bp4 - 0 */
#define	CMIC_OCTHN_P_100	((USHORT)0x001Fu)	/* 100mV Negative OC threshold Primary OCD */
#define	CMIC_OCTHN_P_80		((USHORT)0x0016u)	/* 80mV  Negative OC threshold Primary OCD */
#define	CMIC_OCTHN_P_50		((USHORT)0x000Au)	/* 50mV  Negative OC threshold Primary OCD */
#define	CMIC_OCTHN_P_25		((USHORT)0x0000u)	/* 25mV  Negative OC threshold Primary OCD */

/* 0x61 setting */
/* bp15 - 12 */
#define	CMIC_OCNF_S_1600	((USHORT)0xF000u)	/* 1600us OC noise filter Secondary OCD */
#define	CMIC_OCNF_S_1400	((USHORT)0xD000u)	/* 1400us OC noise filter Secondary OCD */
#define	CMIC_OCNF_S_1200	((USHORT)0xB000u)	/* 1200us OC noise filter Secondary OCD */
#define	CMIC_OCNF_S_1000	((USHORT)0x9000u)	/* 1000us OC noise filter Secondary OCD */
#define	CMIC_OCNF_S_400		((USHORT)0x3000u)	/* 400us  OC noise filter Secondary OCD */
#define	CMIC_OCNF_S_200		((USHORT)0x1000u)	/* 200us  OC noise filter Secondary OCD */
#define	CMIC_OCNF_S_100		((USHORT)0x0000u)	/* 100us  OC noise filter Secondary OCD */
/* bp11 - 10 */
#define	CMIC_OCTH_EN_S_DIS	((USHORT)0x0C00u)	/* Inactive OC comparator Secondary OCD */
#define	CMIC_OCTH_EN_S_N	((USHORT)0x0800u)	/* Negative threshold enable OC comparator Secondary OCD */
#define	CMIC_OCTH_EN_S_P	((USHORT)0x0400u)	/* Positive threshold enable OC comparator Secondary OCD */
#define	CMIC_OCTH_EN_S_PAN	((USHORT)0x0000u)	/* Positive & Negative threshold enable OC comparator Secondary OCD */
/* bp9 - 5 */
#define	CMIC_OCTHP_S_100	((USHORT)0x03C0u)	/* 100mV Positive OC threshold Secondary OCD */
#define	CMIC_OCTHP_S_80		((USHORT)0x02C0u)	/* 80mV  Positive OC threshold Secondary OCD */
#define	CMIC_OCTHP_S_50		((USHORT)0x0140u)	/* 50mV  Positive OC threshold Secondary OCD */
#define	CMIC_OCTHP_S_25		((USHORT)0x0000u)	/* 25mV  Positive OC threshold Secondary OCD */
/* bp4 - 0 */
#define	CMIC_OCTHN_S_100	((USHORT)0x001Eu)	/* 100mV Negative OC threshold Secondary OCD */
#define	CMIC_OCTHN_S_80		((USHORT)0x0016u)	/* 80mV  Negative OC threshold Secondary OCD */
#define	CMIC_OCTHN_S_50		((USHORT)0x000Au)	/* 50mV  Negative OC threshold Secondary OCD */
#define	CMIC_OCTHN_S_25		((USHORT)0x0000u)	/* 25mV  Negative OC threshold Secondary OCD */

/* 0x6 setting */
/* bp2 */
#define	CMIC_GPIO1_5_ENABLE_ADC		((USHORT)0x0F80u)	/* Disable GPIO10 measurement,Enable GPIO1-5 measurement */

/* 0x68 setting */
/* bp15 */
#define	CMIC_FR_ON_ON		((USHORT)0x8000u)	/* Start measurement of Primary CS, Secondary CS, VBAT */
#define	CMIC_FR_ON_OFF		((USHORT)0x0000u)	/* Stop  measurement of Primary CS, Secondary CS, VBAT */
/* bp14 */
#define	CMIC_AUX_ON_ON		((USHORT)0x4000u)	/* Start measurement of AUX */
#define	CMIC_AUX_ON_OFF		((USHORT)0x0000u)	/* Stop  measurement of AUX */
/* bp13 */
#define	CMIC_OCD_ON_P_ON	((USHORT)0x2000u)	/* Start measurement of Primary OSD */
#define	CMIC_OCD_ON_P_OFF	((USHORT)0x0000u)	/* Stop  measurement of Primary OSD */
/* bp12 */
#define	CMIC_OCD_ON_S_ON	((USHORT)0x1000u)	/* Start measurement of Secondary OSD */
#define	CMIC_OCD_ON_S_OFF	((USHORT)0x0000u)	/* Stop  measurement of Secondary OSD */

/* 0x69 setting */
/*bp4 - 0*/
#define CMIC_INIT_POINT_C_P  ((USHORT)0x0400u)
/* bp10 */
#define	CMIC_CLR_POINT_C_P	((USHORT)0x0400u)	/* Clear read pointer of FIFOOUT_C_P(0xC7) */
/* bp2 */
#define	CMIC_CLR_POINT_V_P	((USHORT)0x0004u)	/* Clear read pointer of FIFOOUT_V_P(0xC8) */
/* 0x6A setting */
/* bp10 */
#define	CMIC_CLR_POINT_C_S	((USHORT)0x0400u)	/* Clear read pointer of FIFOOUT_C_S(0xC9) */



typedef enum spi_status_e {
	SPI_STATUS_FAIL,  
	SPI_STATUS_OK,    
	SPI_STATUS_INIT,  
	SPI_STATUS_WAIT,
	SPI_NUM_STATUS
}spi_status_t;

typedef enum 
{
	EvtWake = 0,
	EvtInit917,
	EvtInit8209,
	EvtSys,
	EvtCheck,	
	EvtUpdate,
	EvtFastSample,  
	EvtSlowSample,  
	EvtVMmC7,
	EvtVMmC8,
	EvtVMmC9,
	Evt8209AdcIA,
	Evt8209AdcU,
	EvtError,	

}Event_ID;




#define KA84917_ARRAY_INDEX BMIC_ADDRESS_917-1
#define KA84922_ARRAY_INDEX BMIC_ADDRESS_922-1

#define SLOW_SAMPLE_DIVIDER  (1u)  

#define  RN8209_ENABLE()     vSPI_2byte_write_Mask(0x01, CMIC_CNTADR_30, CMIC_CSON_1_ON|CMIC_MAN_CS_ON, CMIC_CSON_1_ON|CMIC_MAN_CS_ON)
#define  RN8209_DISABLE()     vSPI_2byte_write_Mask(0x01, CMIC_CNTADR_30, CMIC_CSON_1_OFF|CMIC_MAN_CS_OFF, CMIC_CSON_1_ON|CMIC_MAN_CS_OFF)
/*=============================================================================================
** @Data type definition
**===========================================================================================*/
extern USHORT	usSPI_Reg_ReadData[DEVICEADDRESS_MAX][BMIC_REG_ADR_MAX_COMMON];
extern ULONG    usSPI_RegFifo_Data[3][32];
extern UCHAR 	wakeUpState;
extern u32 Rn8209AdData[2];

extern u8 gWakeUpStateInitFlag922;
extern u8 gWakeUpStateInitFlag917;

extern u8 FirstSampleFinishedFlag;
/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void KA84917_maintask(void);
** @Input     : void
** @Output    : void
** @Function  : KA84917主任务
** @The notes : 
**===========================================================================================*/
void KA84917_maintask(void);

u8 vSPI_2byte_write_NoWait(u8 ucdev, u8 ucreg, u16 usdata);
u8 vSPI_2byte_write_Mask_NoWait(u8 ucdev, u8 ucreg, u16 usdata, u16 usMask);
u8 vSPI_2byte_read_NoWait(u8 ucdev, u8 ucreg);
u8 SPI2_Send_Byte(u8 u8Byte, u8 *u8ReadByte);

u8 vSPI_2byte_write(u8 ucdev, u8 ucreg, u16 usdata);
u8 vSPI_2byte_write_Mask(u8 ucdev, u8 ucreg, u16 usdata, u16 usMask);
u8 vSPI_2byte_read(u8 ucdev, u8 ucreg);
u8 vSPI_Continue_Read(u8 ucdev, u8 ucreg, u8 size);
u8 vSPI_FifoContRead(u8 ucdev, u8 ucreg, u8 size);

void IMD_RLY_Open(void);
void IMD_RLY_Close(void);

/*=============================================================================================
** @Name      : u8 GetIMD_RLY_State(void)
** @Input     : void
** @Output    : IMD_RLY_STATE
** @Function  : 获取IMD_RLY状态
** @The notes : 工装代码调用
**===========================================================================================*/
u8 GetIMD_RLY_State(void);


/*=============================================================================================
** @Name      : u8 KA84917_DRV_Init(void)
** @Input     : void
** @Output    : TURE:初始化成功 FLASE:初始化失败
** @Function  : KA84917初始化配置
** @The notes : 
**===========================================================================================*/
u8 KA84917_DRV_Init(TickType_t xLastWakeTime);


#endif
#endif