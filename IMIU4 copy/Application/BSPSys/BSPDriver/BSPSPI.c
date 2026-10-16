/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPSPI.c
** @Founder      : WYC834
** @Date         : 2025.09.08
** @Function     : SPI通讯驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "main.h"
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPIRQ.h"
#include "BSPSPI.h"
#include "spi.h"


/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
static u8 sSpiWorkState[eSPINUM] = {0};
static u8 sSpiRcvData[eSPINUM] = {0};

M_SPI_t GdSpiTab[eSPINUM] = 
{
    {
//        .spi = (u32)&SPI1,
//        .dma = (u32)&DMA1,
//        .dmaTxCh=DMA_CH2,
//        .dmaRxCh=DMA_CH1,
		.dmaTxDone = true,
		.dmaRxDone = true,
    },
    {
//        .spi = (u32)&SPI2,
//        .dma = (u32)&DMA1,
//        .dmaTxCh=DMA_CH4,
//        .dmaRxCh=DMA_CH3,
		.dmaTxDone = true,
		.dmaRxDone = true,
    }
};

/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPSPIInit(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : 初始化结果 TRUE:成功 FALSE:失败
** @Function  : SPI驱动模块初始化
** @The notes : BSP调用
**===========================================================================================*/
void BSPSPIInit(e_SPINum spiNum)
{    
    if(spiNum >= eSPINUM)
    {
        return;
    }
    if(((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return;
    }
    sSpiWorkState[spiNum] = 0;
    switch(spiNum)
    {
        case eSPI1:
            MX_SPI1_Init();
            break;
              
        case eSPI3:       
            MX_SPI3_Init();
            break;
                      
        default:
            break;
    }
}

/*=============================================================================================
** @Name      : u8 BSPSPIGetWorkState(e_SPINum spiNum)
** @Input     : sspiNum:SPI通道号
** @Output    : SPI工作状态 0:空闲 1:忙
** @Function  : 获取SPI工作状态
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPSPIGetWorkState(e_SPINum spiNum)
{
    //模块未使能不进行初始化
    if(((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(0xff);
    }
    
    if(spiNum >= eSPINUM)
    {
        return(0xff);
    }
    
    return(sSpiWorkState[spiNum]);
}

/*=============================================================================================
** @Name      : u8 BSPSPIDmaSendRcvData(e_SPINum spiNum, u8 * tdata, u8 *rdata, u8 size)
** @Input     : sspiNum:SPI通道号 *tdata:需要发送的数据 *rdata:需要接收的数据 size: 数据的大小
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SPI通过DMA发送和接收数据
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPSPIDmaSendRcvData(e_SPINum spiNum, u8 *tdata, u8 *rdata, u8 size)
{
    //模块未使能不进行初始化
    if(((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(FALSE);
    }
    if(1 != gBSPFunCfg.Bits.DMAEn)                                              /*DMA使能*/
    {
        return(FALSE);                                                             
    } 
    switch(spiNum)
    {
        /*SPI1发送*/
        case eSPI1:
            /*SPI正忙*/
            if(1 == sSpiWorkState[spiNum])
            {
                //return(FALSE);
            }
            
            /*SPI切换为忙*/
            sSpiWorkState[spiNum] = 1;
            if(HAL_OK !=HAL_SPI_TransmitReceive_DMA(&hspi1,tdata,rdata,size))
            {
                return (FALSE);
            }
            /*SPI切换为空闲*/
            sSpiWorkState[spiNum] = 0;
            break;
     
        /*SPI3发送*/
        case eSPI3:
            /*SPI正忙*/
            if(1 == sSpiWorkState[spiNum])
            {
                //return(FALSE);
            }
            
            /*SPI切换为忙*/
            sSpiWorkState[spiNum] = 1;
            if(HAL_OK !=HAL_SPI_TransmitReceive_DMA(&hspi3,tdata,rdata,size))
            {
                return (FALSE);
            }
            /*SPI切换为空闲*/
            sSpiWorkState[spiNum] = 0;
            break; 
            
        default:
            return(FALSE);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPSPISendRcvData(e_SPINum spiNum, u8 *tdata, u8 *rdata, u8 size)
** @Input     : sspiNum:SPI通道号 *tdata 需要发送的数据 *rdata 接收的数据 size:数据大小
** @Output    : TRUE:成功 FALSE:失败
** @Function  : SPI直接发送和接收数据
** @The notes : 用户调用
**===========================================================================================*/
u8 BSPSPISendRcvData(e_SPINum spiNum, u8 *tdata, u8 *rdata, u8 size)
{
    //模块未使能不进行初始化
    if(((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(FALSE);
    }
    if(1 != gBSPFunCfg.Bits.DMAEn)                                              /*DMA使能*/
    {
        return(FALSE);                                                             
    } 
    switch(spiNum)
    {
        /*SPI1发送*/
        case eSPI1:
            /*SPI正忙*/
            if(1 == sSpiWorkState[spiNum])
            {
                //return(FALSE);
            }
            
            /*SPI切换为忙*/
            sSpiWorkState[spiNum] = 1;
            if(HAL_OK !=HAL_SPI_TransmitReceive(&hspi1,tdata,rdata,size,100))
            {
                return (FALSE);
            }
            /*SPI切换为空闲*/
            sSpiWorkState[spiNum] = 0;
            break;
     
        /*SPI3发送*/
        case eSPI3:
            /*SPI正忙*/
            if(1 == sSpiWorkState[spiNum])
            {
                //return(FALSE);
            }
            
            /*SPI切换为忙*/
            sSpiWorkState[spiNum] = 1;
            if(HAL_OK !=HAL_SPI_TransmitReceive(&hspi3,tdata,rdata,size,100))
            {
                return (FALSE);
            }
            /*SPI切换为空闲*/
            sSpiWorkState[spiNum] = 0;
            break; 
            
        default:
            return(FALSE);
    }
    
    return(TRUE);
}

/*=============================================================================================
** @Name      : u16 BSPSPIGetRegData(e_SPINum spiNum)
** @Input     : spiNum:SPI通道号
** @Output    : void
** @Function  : 获得SPI接收到寄存器的数据
** @The notes : 用户调用
**===========================================================================================*/
u16 BSPSPIGetRegData(e_SPINum spiNum)
{
    //模块未使能不进行初始化
    if(((eSPI1 == spiNum) && (1 != gBSPFunCfg.Bits.SPI1En))
        || ((eSPI3 == spiNum) && (1 != gBSPFunCfg.Bits.SPI3En)))
    {
        return(0xffff);
    }
    //检测是否已进行数据接收
    switch(spiNum)
    {
        //SPI1接收
        case eSPI1:
            //未进行接收
            if(1 == sSpiWorkState[spiNum])
            {
                //读取接收数据
                //sSpiRcvData[spiNum] = SPI_receive1(0);
                
                //SPI切换为空闲
                sSpiWorkState[spiNum] = 0;
            }
            break;

        //SPI3接收
        case eSPI3:
            //未进行接收
            if(1 == sSpiWorkState[spiNum])
            {
                //读取接收数据
                //sSpiRcvData[spiNum] = SPI_receive1(1);
                
                //SPI切换为空闲
                sSpiWorkState[spiNum] = 0;
            }
            break;  

        default:
            return(0xffff);
    }
    
    return(sSpiRcvData[spiNum]);
}

/*=============================================================================================
** @Name      : u8 BSPSPIRcvData(e_SPINum spiNum, u8 *data, u8 size)
** @Input     : spiNum:SPI通道号 data:接收的数据 size:数据大小
** @Output    : 是否接收成功
** @Function  : 获取SPI接收数据
** @The notes :
**===========================================================================================*/
u8 BSPSPIRcvData(e_SPINum spiNum, u8 *data, u8 size)
{
    if(eSPI1 == spiNum)
    {   
        if(1 == sSpiWorkState[spiNum])
        {
            return(FALSE);
        }
        /*SPI切换为忙*/
        sSpiWorkState[spiNum] = 1;
        if(HAL_OK != HAL_SPI_Receive(&hspi1, data, size, 1000))
        {
            // Error_Handler();      
            return (FALSE);          
        }
        /*SPI切换为空闲*/
        sSpiWorkState[spiNum] = 0;  
    }
    else if(eSPI3 == spiNum)
    {
        if(1 == sSpiWorkState[spiNum])
        {
            return(FALSE);
        }
        /*SPI切换为忙*/
        sSpiWorkState[spiNum] = 1;
        if(HAL_OK != HAL_SPI_Receive(&hspi3, data, size, 1000))
        {
            // Error_Handler(); 
            return (FALSE);           
        }
        /*SPI切换为空闲*/
        sSpiWorkState[spiNum] = 0;
    }
    return(TRUE);
}

/*=============================================================================================
** @Name      : u8 BSPSPISendData(e_SPINum spiNum, u8 *data ,u8 size)
** @Input     : spiNum:SPI通道号  data:需要发送的数据 size:数据大小
** @Output    : 发送是否成功
** @Function  : SPI发送数据
** @The notes :
**===========================================================================================*/
u8 BSPSPISendData(e_SPINum spiNum, u8 *data ,u8 size)
{

    if(eSPI1 == spiNum)
    {     
        if(1 == sSpiWorkState[spiNum])
        {
            return(FALSE);
        } 
        /*SPI切换为忙*/
        sSpiWorkState[spiNum] = 1;      
        if(HAL_OK !=HAL_SPI_Transmit(&hspi1, data, size, 1000))
        {
            // Error_Handler();   
            return (FALSE);  
        }
        /*SPI切换为空闲*/
        sSpiWorkState[spiNum] = 0;
    } 
    else if(eSPI3 == spiNum)
    {    
        if(1 == sSpiWorkState[spiNum])
        {
            return(FALSE);
        }
        /*SPI切换为忙*/
        sSpiWorkState[spiNum] = 1;
        if(HAL_OK !=HAL_SPI_Transmit(&hspi3, data, size, 1000))
        {
            // Error_Handler(); 
            return (FALSE);   
        }
        /*SPI切换为空闲*/
        sSpiWorkState[spiNum] = 0;
    }	

    return(TRUE);

}


//GD25q32e
uint8 DRV_SPI2_SendByteBurst(uint8 *u8SendByte, uint8 *u8ReadByte, uint16 u16Size)
{
    uint8 u8SpiStatus;

    if (HAL_SPI_TransmitReceive(&hspi1, u8SendByte, u8ReadByte, u16Size, SPI_TIMEOUT) != HAL_OK)
    {
        u8SpiStatus = FLASH_STATUS_ACCESS_FAIL;
        HAL_SPI_DeInit(&hspi1);
        HAL_SPI_Init(&hspi1);
    }
    u8SpiStatus = FLASH_STATUS_ACCESS_OK;
    return u8SpiStatus;
}

uint8 DRV_SPI2_SendByte(uint8 u8Byte, uint8 *u8ReadByte)
{
    uint8 u8Dummy = 0;
    uint8 u8SpiStatus;
    if (HAL_SPI_TransmitReceive(&hspi1, (uint8 *)(&u8Byte), (uint8 *)(&u8Dummy), 1, SPI_TIMEOUT) != HAL_OK)
    {
        u8SpiStatus = FLASH_STATUS_ACCESS_FAIL;
        HAL_SPI_DeInit(&hspi1);
        HAL_SPI_Init(&hspi1);
    }
    u8SpiStatus = FLASH_STATUS_ACCESS_OK;
    *u8ReadByte = u8Dummy;
    return u8SpiStatus;
}
/*=============================================================================================
** @Static function definition
**===========================================================================================*/
