/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPGD25Q32E.c
** @Founder      : WYC834
** @Date         : 2024.06.21
** @Function     : GD25Q32E驱动模块
** @Instructions :
**===========================================================================================*/
/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "BSPGD25Q32E.h"
#include "BSPSPI.h"
#include "FreeRTOS.h"
#include "task.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
extern SPI_HandleTypeDef hspi1;

static Gd25qxxDeviceType g_stGd25qDevice = 
{
    .blockNum = DRV_GD25Q32_BLOCK_NUM,
    .size = DRV_GD25Q32_SIZE,
    .identification = DRV_GD25Q32_IDENTIFICATION,
};
const uint8 dummy_chars_tx[256] = {0xff};
uint8 dummy_chars_rx[256];
bool irq_disabled = false;
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : static uint8_t BSP_GD25Qx_GetStatus(void)
** @Input     : void
** @Output    : GD25Q32E此刻状态
** @Function  : 读取GD25Q32E状态
** @The notes : 
**===========================================================================================*/
static uint8_t BSP_GD25Qx_GetStatus(void);

/*=============================================================================================
** @Name      : static void    BSP_GD25Qx_Reset(void)
** @Input     : void
** @Output    : void
** @Function  : 重置flash
** @The notes : 
**===========================================================================================*/
static void   BSP_GD25Qx_Reset(void);

static uint8 Gd25qxxWriteEnable(void);

static uint8 Gd25qxxWaitForIdle(void);
/*=============================================================================================
** @Interface function definition
**===========================================================================================*/

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Init(void)
** @Input     : void
** @Output    : void
** @Function  : 外部flash初始化
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Init(void)
{ 
    uint8_t status;
    uint32_t timeout = 0;
    //重置flash
    BSP_GD25Qx_Reset();
    do{
        status = BSP_GD25Qx_GetStatus();
        timeout++;
        if(timeout > GD25Qx_GETSTATE_COUNT)
        {
            return GD25Qx_TIMEOUT;
        }
      
    }while(status != GD25Qx_OK);
    return GD25Qx_OK;
}

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_WriteEnable(void)
** @Input     : void
** @Output    : GD25Q32E写使能是否成功
** @Function  : 返回GD25Q32E写使能
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_WriteEnable(void)
{
    uint8_t cmd[] = {WRITE_ENABLE_CMD};
    uint32_t count = 0;
    /*Select the FLASH: Chip Select low */
    GD25Qx_Enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&hspi1, cmd, 1, GD25Qx_TIMEOUT_VALUE);    
    /*Deselect the FLASH: Chip Select high */
    GD25Qx_Disable();

    /* Wait the end of Flash writing */
    while(BSP_GD25Qx_GetStatus() == GD25Qx_BUSY)
    {
        /* Check for the Timeout */
        count++;
        if(count > GD25Qx_GETSTATE_COUNT)
        {        
            return GD25Qx_TIMEOUT;
        }
    }

    return GD25Qx_OK;
}


/*=============================================================================================
** @Name      : void BSP_GD25Qx_Read_ID(uint8_t *ID)
** @Input     : ID:GD25Q32E的芯片ID
** @Output    : 
** @Function  : 返回GD25Q32E的ID
** @The notes : 
**===========================================================================================*/
void BSP_GD25Qx_Read_ID(uint8_t *ID)
{
    uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};

    GD25Qx_Enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&hspi1, cmd, 4, GD25Qx_TIMEOUT_VALUE);    
    /* Reception of the data */
    HAL_SPI_Receive(&hspi1,ID, 2, GD25Qx_TIMEOUT_VALUE);
    
    //BSPSPIDmaSendRcvData(eSPI1,cmd,ID,4);  
    GD25Qx_Disable();

}

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
** @Input     : pData:读操作要写入的数组地址 ReadAddr：读地址 Size：读数组长度
** @Output    : 返回读操作是否成功
** @Function  : falsh读操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
    uint8_t cmd[4];

    /* Configure the command */
    cmd[0] = READ_CMD;
    cmd[1] = (uint8_t)(ReadAddr >> 16);
    cmd[2] = (uint8_t)(ReadAddr >> 8);
    cmd[3] = (uint8_t)(ReadAddr);

    GD25Qx_Enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&hspi1, cmd, 4, GD25Qx_TIMEOUT_VALUE);    
    /* Reception of the data */
    if (HAL_SPI_Receive(&hspi1, pData,Size,GD25Qx_TIMEOUT_VALUE) != HAL_OK)
    {
      return GD25Qx_ERROR;
    }
    GD25Qx_Disable();
    return GD25Qx_OK;
}

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
** @Input     : pData:写操作要写入的数组地址 ReadAddr：写地址 Size：写数组长度
** @Output    : 返回写操作是否成功
** @Function  : falsh写操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
    uint8_t cmd[4];
    uint32_t count = 0;
    uint32_t end_addr, current_size, current_addr;
    /* Calculation of the size between the write address and the end of the page */
  current_addr = 0;

  while (current_addr <= WriteAddr)//判断地址属于哪一扇区开始
  {
    current_addr += GD25Q32E_PAGE_SIZE;//0x100-> 256 bytes
  }
  current_size = current_addr - WriteAddr;

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the adress variables *///写入地址大小范围
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;

  /* Perform the write page by page */
  do
  {
        /* Configure the command */
        cmd[0] = PAGE_PROG_CMD;
        cmd[1] = (uint8_t)(current_addr >> 16);
        cmd[2] = (uint8_t)(current_addr >> 8);
        cmd[3] = (uint8_t)(current_addr);

        /* Enable write operations */
        BSP_GD25Qx_WriteEnable();

        GD25Qx_Enable();
    /* Send the command */
    if (HAL_SPI_Transmit(&hspi1,cmd, 4, GD25Qx_TIMEOUT_VALUE) != HAL_OK)
    {
      return GD25Qx_ERROR;
    }

    /* Transmission of the data */
    if (HAL_SPI_Transmit(&hspi1, pData,current_size, GD25Qx_TIMEOUT_VALUE) != HAL_OK)
    {
      return GD25Qx_ERROR;
    }
            GD25Qx_Disable();
        /* Wait the end of Flash writing */
        while(BSP_GD25Qx_GetStatus() == GD25Qx_BUSY)
        {
            /* Check for the Timeout */
            count++;
            if(count > GD25Qx_GETSTATE_COUNT)
            {        
                return GD25Qx_TIMEOUT;
            }
        }

    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + GD25Q32E_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : GD25Q32E_PAGE_SIZE;
  } while (current_addr < end_addr);


    return GD25Qx_OK;
}

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Erase_Sector(uint32_t Address)
** @Input     : Address:擦除操作的扇区地址
** @Output    : 返回扇区擦除操作是否成功
** @Function  : falsh扇区擦除操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Erase_Sector(uint32_t Address)
{
    uint8_t cmd[4];
    uint32_t count = 0;
    cmd[0] = SECTOR_ERASE_CMD;
    cmd[1] = (uint8_t)(Address >> 16);
    cmd[2] = (uint8_t)(Address >> 8);
    cmd[3] = (uint8_t)(Address);

    /* Enable write operations */
    BSP_GD25Qx_WriteEnable();

    /*Select the FLASH: Chip Select low */
    GD25Qx_Enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&hspi1, cmd, 4, GD25Qx_TIMEOUT_VALUE);    
    /*Deselect the FLASH: Chip Select high */
    GD25Qx_Disable();
    /* Wait the end of Flash writing */
    while(BSP_GD25Qx_GetStatus() != GD25Qx_OK)
    {
        /* Check for the Timeout */
        count++;
        if(count > GD25Qx_GETSTATE_COUNT)
        {     
            return GD25Qx_TIMEOUT;
        }
    }
    return GD25Qx_OK;
}



/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Erase_Block(uint32_t Address)
** @Input     : Address:擦除操作的块地址
** @Output    : 返回块擦除操作是否成功
** @Function  : falsh块擦除操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Erase_Block(uint32_t Address)
{
    uint8_t cmd[4];
    uint32_t count = 0;
    cmd[0] = BLOCK_ERASE_CMD;
    cmd[1] = (uint8_t)(Address >> 16);
    cmd[2] = (uint8_t)(Address >> 8);
    cmd[3] = (uint8_t)(Address);

    /* Enable write operations */
    BSP_GD25Qx_WriteEnable();

    /*Select the FLASH: Chip Select low */
    GD25Qx_Enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&hspi1, cmd, 4, GD25Qx_TIMEOUT_VALUE);    

    /*Deselect the FLASH: Chip Select high */
    GD25Qx_Disable();
    while(BSP_GD25Qx_GetStatus() != GD25Qx_OK)
    {
        /* Check for the Timeout */
        count++;
        if(count > GD25Qx_GETSTATE_COUNT)
        {      
            return GD25Qx_TIMEOUT;
        }
    }

    return GD25Qx_OK;
}


/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Erase_Chip(void)
** @Input     : 
** @Output    : 返回擦除操作是否成功
** @Function  : falsh擦除操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Erase_Chip(void)
{
    uint8_t cmd[4];
    uint32_t count = 0;
    cmd[0] = CHIP_ERASE_CMD;

    /* Enable write operations */
    BSP_GD25Qx_WriteEnable();

    /*Select the FLASH: Chip Select low */
    GD25Qx_Enable();
    /* Send the read ID command */
    HAL_SPI_Transmit(&hspi1, cmd, 1, GD25Qx_TIMEOUT_VALUE);    
    GD25Qx_Disable();
    /* Wait the end of Flash writing */
    while(BSP_GD25Qx_GetStatus() != GD25Qx_OK)
    {
        /* Check for the Timeout */
        count++;
        if(count > GD25Qx_GETSTATE_COUNT)
        {    
            /*Deselect the FLASH: Chip Select high */
            return GD25Qx_TIMEOUT;
        }
    }
    /*Deselect the FLASH: Chip Select high */
    
    return GD25Qx_OK;
}

/*=============================================================================================
** @Static function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : static void    BSP_GD25Qx_Reset(void)
** @Input     : void
** @Output    : void
** @Function  : 重置flash
** @The notes : 
**===========================================================================================*/
static void BSP_GD25Qx_Reset(void)
{
    uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};

    GD25Qx_Enable();
    /* Send the enable reset command */
    HAL_SPI_Transmit(&hspi1, &cmd[0], 1, GD25Qx_TIMEOUT_VALUE);    
    GD25Qx_Disable();
    
    GD25Qx_Enable();
    /* Send the reset command */
    HAL_SPI_Transmit(&hspi1, &cmd[1], 1, GD25Qx_TIMEOUT_VALUE);    
    GD25Qx_Disable();

}

/*=============================================================================================
** @Name      : static uint8_t BSP_GD25Qx_GetStatus(void)
** @Input     : void
** @Output    : GD25Q32E此刻状态
** @Function  : 读取GD25Q32E状态
** @The notes : 
**===========================================================================================*/
static uint8_t BSP_GD25Qx_GetStatus(void)
{
    uint8_t cmd[4] = {READ_STATUS_REG1_CMD,READ_STATUS_REG2_CMD,READ_STATUS_REG3_CMD,0x00};
    
    uint8_t status;
    GD25Qx_Enable();
    /* Send the read status command */
    HAL_SPI_Transmit(&hspi1, cmd, 1, GD25Qx_TIMEOUT_VALUE);    
    /* Reception of the data */
    HAL_SPI_Receive(&hspi1,&status, 1, GD25Qx_TIMEOUT_VALUE);
    GD25Qx_Disable();

    /* Check the value of the register */
    if(status == GD25Q32E_FSR_OK)
    {      
        return GD25Qx_OK;
    }
    else
    {
        return GD25Qx_BUSY;
    }        
}






/*移植IMAP平台外部flash读写*/

static uint8 Gd25qxxWriteEnable(void)
{
    uint8 u8RetVal = FLASH_STATUS_ACCESS_FAIL;
    uint8 u8Temp;
    GD25Qx_Enable();
    if (DRV_SPI2_SendByte(GD25QXX_WR_ENABLE, &u8Temp) == FLASH_STATUS_ACCESS_OK)
    {
        u8RetVal = FLASH_STATUS_ACCESS_OK;
    }
    GD25Qx_Disable();
    return u8RetVal;
}

static uint8 Gd25qxxWaitForIdle(void)
{
    uint8 flashstatus = 0;
    uint16 timeout;
    uint8 u8RetVal = FLASH_STATUS_ACCESS_FAIL;
    uint8 u8SpiStatus = FLASH_STATUS_ACCESS_OK;
    GD25Qx_Enable();
    timeout = GD25QXX_FLASH_WIP_TIMEOUT;
    if (DRV_SPI2_SendByte(GD25QXX_RD_STATUSREG, &flashstatus) == FLASH_STATUS_ACCESS_OK)
    {
        while ((flashstatus & GD25QXX_WIP_FLAG) == GD25QXX_WIP_FLAG)
        {
            timeout--;
            if (timeout == 0)
            {
                break;
            }
            u8SpiStatus = DRV_SPI2_SendByte(GD25QXX_RD_STATUSREG, &flashstatus);
            if (u8SpiStatus != FLASH_STATUS_ACCESS_OK)
            {
                break;
            }      
            vTaskDelay(1);
        }
  
        if ((u8SpiStatus == FLASH_STATUS_ACCESS_OK) && (timeout > 0))
        {
            u8RetVal = FLASH_STATUS_ACCESS_OK;
        }
    }
    else
    {
        u8RetVal = FLASH_STATUS_WAIT;
    }
    GD25Qx_Disable();
    return u8RetVal;
}


uint8 DRV_GD25QXX_ReadID(uint32 *id)
{
    uint8 u8Temp, u8Temp1, u8Temp2;
    uint8 u8RetVal = FLASH_STATUS_ACCESS_FAIL;


    *id = 0;
    GD25Qx_Enable();
    if (DRV_SPI2_SendByte(GD25QXX_READ_IDENTIFICATION, &u8Temp) == FLASH_STATUS_ACCESS_OK)
    {
        if (DRV_SPI2_SendByte(GD25QXX_DUMMY_BYTE, &u8Temp) == FLASH_STATUS_ACCESS_OK)
        {
            if (DRV_SPI2_SendByte(GD25QXX_DUMMY_BYTE, &u8Temp1) == FLASH_STATUS_ACCESS_OK)
            {
                if (DRV_SPI2_SendByte(GD25QXX_DUMMY_BYTE, &u8Temp2) == FLASH_STATUS_ACCESS_OK)
                {
                    *id = (u8Temp << 16U) | (u8Temp1 << 8U) | u8Temp2;
                    u8RetVal = FLASH_STATUS_ACCESS_OK;
                }
            }
        }
    }
    GD25Qx_Disable();
    return u8RetVal;
}

uint8 DRV_FLASH_ReadBytes(uint8 *pBuf, uint32 addr, uint32 numBytes)
{
    uint8 u8RetVal = FLASH_STATUS_ACCESS_FAIL;
    uint8 u8Temp;
    uint8 u8SpiStatus;
    // uint32 delta;

    if ((0 == numBytes))
    {
        return FLASH_STATUS_ACCESS_FAIL;
    }

    GD25Qx_Enable();
    if (DRV_SPI2_SendByte(GD25QXX_READ_DATA_NORMAL, &u8Temp) == FLASH_STATUS_ACCESS_OK)
    {
        // Send High nibble
        if (DRV_SPI2_SendByte(((addr & 0xFF0000U) >> 16U), &u8Temp) == FLASH_STATUS_ACCESS_OK)
        {
            // Send Medium nibble
            if (DRV_SPI2_SendByte(((addr & 0xFF00U) >> 8U), &u8Temp) == FLASH_STATUS_ACCESS_OK)
            {
                if (DRV_SPI2_SendByte((addr & 0xFFU), &u8Temp) == FLASH_STATUS_ACCESS_OK)
                {

                    u8SpiStatus = DRV_SPI2_SendByteBurst((uint8 *)dummy_chars_tx, pBuf, numBytes);
                    if (u8SpiStatus == FLASH_STATUS_ACCESS_OK)
                    {
                        u8RetVal = FLASH_STATUS_ACCESS_OK;
                    }
                }
            }
        }
    }
    GD25Qx_Disable();
    return u8RetVal;
}

uint8 DRV_GD25QXX_PageProgram(const uint8 *pBuf, uint32 addr, uint32 numBytes)
{
    uint8 u8RetVal = FLASH_STATUS_ACCESS_FAIL;
    uint8 u8Temp;
    uint8 u8SpiStatus;

    if ((0 == numBytes) || (pBuf == NULL))
    {
        return FLASH_STATUS_ACCESS_FAIL;
    }
    //__disable_irq();
    //BSP_DISABLE_IRQ();
    irq_disabled = true; 
    if (Gd25qxxWriteEnable() == FLASH_STATUS_ACCESS_OK)
    {
        GD25Qx_Enable();
        if (DRV_SPI2_SendByte(GD25QXX_PAGE_PROGRAM, &u8Temp) == FLASH_STATUS_ACCESS_OK)
        {
            if (DRV_SPI2_SendByte(((addr & 0xFF0000U) >> 16U), &u8Temp) == FLASH_STATUS_ACCESS_OK)
            {
                if (DRV_SPI2_SendByte(((addr & 0xFF00U) >> 8U), &u8Temp) == FLASH_STATUS_ACCESS_OK)
                {
                    if (DRV_SPI2_SendByte((addr & 0xFFU), &u8Temp) == FLASH_STATUS_ACCESS_OK)
                    {
                        u8SpiStatus = DRV_SPI2_SendByteBurst((uint8 *)pBuf, dummy_chars_rx, numBytes);
                        if (u8SpiStatus == FLASH_STATUS_ACCESS_OK)
                        {
                            u8RetVal = FLASH_STATUS_ACCESS_OK;
                        }
                    }
                }
            }
        }
        GD25Qx_Disable();
        u8RetVal = Gd25qxxWaitForIdle();
    }
    // __enable_irq();
    // BSP_ENABLE_IRQ();

   irq_disabled = false; 
    return u8RetVal;
}

uint8 DRV_FLASH_WriteBytes(const uint8 *pBuf, uint32 addr, uint32 numBytes)
{
    uint32 wrAdd;
    uint16 wrBytes;
    uint8 u8RetVal = FLASH_STATUS_ACCESS_FAIL;

    if ((0 == numBytes) || (pBuf == NULL))
    {
        return FLASH_STATUS_ACCESS_FAIL;
    }
    if (g_stGd25qDevice.size <= (addr + numBytes))
    {
        return FLASH_STATUS_INVALID_ADD;
    }
    wrAdd = ((addr / DRV_GD25Q32_PAGE_SIZE) + 1U) * DRV_GD25Q32_PAGE_SIZE;
    if (wrAdd >= (addr + numBytes))
    {
        u8RetVal = DRV_GD25QXX_PageProgram(pBuf, addr, numBytes);
    }
    else
    {
        wrBytes = wrAdd - addr;
        wrAdd = addr;
        u8RetVal = DRV_GD25QXX_PageProgram(pBuf, wrAdd, wrBytes);
        if (u8RetVal == FLASH_STATUS_ACCESS_OK)
        {
            numBytes -= wrBytes;
            wrAdd += wrBytes;
            pBuf += wrBytes;
            while (numBytes > DRV_GD25Q32_PAGE_SIZE)
            {
                u8RetVal = DRV_GD25QXX_PageProgram(pBuf, wrAdd, DRV_GD25Q32_PAGE_SIZE);
                if (u8RetVal == FLASH_STATUS_ACCESS_FAIL)
                {
                    return u8RetVal;
                }
                numBytes -= DRV_GD25Q32_PAGE_SIZE;
                wrAdd += DRV_GD25Q32_PAGE_SIZE;
                pBuf += DRV_GD25Q32_PAGE_SIZE;
            }
            if (numBytes)
            {
                u8RetVal = DRV_GD25QXX_PageProgram(pBuf, wrAdd, numBytes);
            }
        }
    }
  //  HAL_Delay(5);
    return u8RetVal;
}

uint8 DRV_GD25QXX_EraseChip(void)
{
    uint8 u8Temp;

    if (Gd25qxxWriteEnable() == FLASH_STATUS_ACCESS_OK)
    {
        GD25Qx_Enable();
        if (DRV_SPI2_SendByte(GD25QXX_ERASE_CHIP, &u8Temp) == FLASH_STATUS_ACCESS_OK)
        {
            GD25Qx_Disable();
            return Gd25qxxWaitForIdle();
        }
        else
        {
            GD25Qx_Disable();
            return FLASH_STATUS_ACCESS_FAIL;
        }
    }
    else
    {
        return FLASH_STATUS_ACCESS_FAIL;
    }
}

uint8 DRV_FLASH_EraseBlock(uint32 blockAddr)
{
    uint8 u8Temp;

    if (g_stGd25qDevice.size <= blockAddr)
    {
        return FLASH_STATUS_INVALID_ADD;
    }
    if (Gd25qxxWriteEnable() == FLASH_STATUS_ACCESS_OK)
    {
        GD25Qx_Enable();
        if (DRV_SPI2_SendByte(GD25QXX_ERASE_BLOCK, &u8Temp) == FLASH_STATUS_ACCESS_OK)
        {
            if (DRV_SPI2_SendByte(((blockAddr & 0xFF0000U) >> 16U), &u8Temp) == FLASH_STATUS_ACCESS_OK)
            {
                if (DRV_SPI2_SendByte(((blockAddr & 0xFF00U) >> 8U), &u8Temp) == FLASH_STATUS_ACCESS_OK)
                {
                    if (DRV_SPI2_SendByte((blockAddr & 0xFFU), &u8Temp) == FLASH_STATUS_ACCESS_OK)
                    {
                        GD25Qx_Disable();
                        return Gd25qxxWaitForIdle();
                    }
                }
            }
        }
        GD25Qx_Disable();
    }
    return FLASH_STATUS_ACCESS_FAIL;
}

uint8 DRV_FLASH_EraseSector(uint32 sectorAddr)
{
    uint8 u8Temp;

    if (g_stGd25qDevice.size <= sectorAddr)
    {
        return FLASH_STATUS_INVALID_ADD;
    }
    if (Gd25qxxWriteEnable() == FLASH_STATUS_ACCESS_OK)
    {
        GD25Qx_Enable();
        if (DRV_SPI2_SendByte(GD25QXX_ERASE_SECTOR, &u8Temp) == FLASH_STATUS_ACCESS_OK)
        {
            if (DRV_SPI2_SendByte(((sectorAddr & 0xFF0000U) >> 16U), &u8Temp) == FLASH_STATUS_ACCESS_OK)
            {
                if (DRV_SPI2_SendByte(((sectorAddr & 0xFF00U) >> 8U), &u8Temp) == FLASH_STATUS_ACCESS_OK)
                {
                    if (DRV_SPI2_SendByte((sectorAddr & 0xFFU), &u8Temp) == FLASH_STATUS_ACCESS_OK)
                    {
                        GD25Qx_Disable();
                        return Gd25qxxWaitForIdle();
                    }
                }
            }
        }
        GD25Qx_Disable();
    }
    return FLASH_STATUS_ACCESS_FAIL;
}
