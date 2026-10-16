/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPDIDO.c
** @Founder      : WYC834
** @Date         : 2025.8.21
** @Function     : DIDO驱动函数模块
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "main.h"
#include "BSPConfig.h"
#include "BSPDIDO.h"
#include "BSPGPIO.h"
#include "gpio.h"
#include "BSPTypeDef.h"
#include "SystemData.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static u8 BSPGPIOGetHWVersion(e_INPUTPORT num)
** @Input     : u8
** @Output    : 获取
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
static u8 BSPGPIOGetHWVersion(e_INPUTPORT num);
/*=============================================================================================
** @Interface function definition
**===========================================================================================*/

/*=============================================================================================
** @Name      : u8 SWReadData(void)
** @Input     : 
** @Output    : 读取拨码地址
** @Function  : 读取拨码地址
** @The notes : 用户调用
**===========================================================================================*/
u8 SWReadData(void)
{
    if(1 != gBSPFunCfg.Bits.DIDOEn)
    {
        return 0;
    }
    u8 sw4 = BSPGPIOGetPortState(SW4_PORT, SW4_PIN);
    u8 sw3 = BSPGPIOGetPortState(SW3_PORT, SW3_PIN);
    u8 sw2 = BSPGPIOGetPortState(SW2_PORT, SW2_PIN);
    u8 sw1 = BSPGPIOGetPortState(SW1_PORT, SW1_PIN);
    

    SystemData_51[DidoGpioState] &= ~SW_BIT_MASK;//清空bit3-bit6

    //记录拨码状态
    if(sw1) SystemData_51[DidoGpioState] |= (1 << Sw1);
    if(sw2) SystemData_51[DidoGpioState] |= (1 << Sw2);
    if(sw3) SystemData_51[DidoGpioState] |= (1 << Sw3);
    if(sw4) SystemData_51[DidoGpioState] |= (1 << Sw4);

    //拨码值
    u8 sw  = 0;
    sw |= (sw1 << 0);
    sw |= (sw2 << 1);
    sw |= (sw3 << 2);
    sw |= (sw4 << 3);

    return sw;
}


/*=============================================================================================
** @Name      : void CRELAY1_Contr_Open(void)
** @Input     : void
** @Output    : 打开DO1 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void CRELAY1_Contr_Open(void)
{ 
    CRELAY1_Open();//0
    InPutDoState(0,0);
    BitClr16(SystemData_51[DidoGpioState],Do1);//记录状态
}

/*=============================================================================================
** @Name      : void CRELAY1_Contr_Close(void)
** @Input     : void
** @Output    : 闭合DO1 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void CRELAY1_Contr_Close(void)
{
    CRELAY1_Close();//1
    InPutDoState(0,1);
    BitSet16(SystemData_51[DidoGpioState],Do1);//记录状态
}

/*=============================================================================================
** @Name      : void CRELAY2_Contr_Open(void)
** @Input     : void
** @Output    : 打开DO2 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void CRELAY2_Contr_Open(void)
{ 
    CRELAY2_Open();//0
    InPutDoState(1,0);
    BitClr16(SystemData_51[DidoGpioState],Do2);//记录状态
}

/*=============================================================================================
** @Name      : void CRELAY2_Contr_Close(void)
** @Input     : void
** @Output    : 闭合DO2 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void CRELAY2_Contr_Close(void)
{
    CRELAY2_Close();//1
    InPutDoState(1,1);
    BitSet16(SystemData_51[DidoGpioState],Do2);//记录状态
}


/*=============================================================================================
** @Name      : void PE_RLY_Contr_Open(void)
** @Input     : void
** @Output    : 打开PE_RLY 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void PE_RLY_Contr_Open(void)
{
    PE_RLY_Open();//0
    BitClr16(SystemData_51[DidoGpioState],PeRlyState);//记录状态
}

/*=============================================================================================
** @Name      : void PE_RLY_Contr_Close(void)
** @Input     : void
** @Output    : 闭合PE_RLY 记录状态
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void PE_RLY_Contr_Close(void)
{
    PE_RLY_Close();//1
    BitSet16(SystemData_51[DidoGpioState],PeRlyState);//记录状态
}

/*=============================================================================================
** @Name      : void PE_RLY_Contr_Close(void)
** @Input     : void
** @Output    : 获取硬件版本信息
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
void DEVGPIOGetHWVersion(void)
{
    u8 state = 0;
    u8 i = 0;

    for ( i = ePIn_HWVER0; i < eINPUTPORT_Len; i++)
    {
        if (1 == BSPGPIOGetHWVersion((e_INPUTPORT)i))
        {
            BitSet16(state,(i - ePIn_HWVER0));
        }
    }
    InPutSystemInfoArray(HardwareVersion,state);

    if (1 == (state & 0x01) )
    {
        BitSet16(SystemData_51[DidoGpioState],Ver1);//记录状态
    }
    else if(2 == (state & 0x02))
    {
        BitSet16(SystemData_51[DidoGpioState],Ver2);//记录状态
    }
}


/*=============================================================================================
** @Name      : static u8 BSPGPIOGetHWVersion(e_INPUTPORT num)
** @Input     : u8
** @Output    : 获取
** @Function  : 
** @The notes : 用户调用
**===========================================================================================*/
static u8 BSPGPIOGetHWVersion(e_INPUTPORT num)
{
    u8 state = 0;
    switch (num)
    {
        case ePIn_HWVER0:
            state = VRE0_Pin_Read();
            return state;
            break;
        
        case ePIn_HWVER1:
            state = VRE1_Pin_Read();
            return state;
            break;

        default:
            break;
    }
     return state;
}


