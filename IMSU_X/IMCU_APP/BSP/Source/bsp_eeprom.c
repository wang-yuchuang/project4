#include "bsp_eeprom.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus; 

I2C_HandleTypeDef  heval_I2c;
I2C_HandleTypeDef  heval_I2c3;
static void     I2Cx_Init(void);
static HAL_StatusTypeDef I2Cx_WriteMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_ReadMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef  I2Cx_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);
static void     I2Cx_Error(uint8_t Addr);
static void I2C3_Error(uint8_t Addr);

uint32_t BSP_EEPROM_WritePage(uint8_t *pBuffer, uint16_t WriteAddr, uint8_t *NumByteToWrite);
void                EEPROM_IO_Init(void);
HAL_StatusTypeDef   EEPROM_IO_WriteData(uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
HAL_StatusTypeDef   EEPROM_IO_ReadData(uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
HAL_StatusTypeDef   EEPROM_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);
void     BSP_EEPROM_TIMEOUT_UserCallback(void);
HAL_StatusTypeDef RTC_IO_ReadData(uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
HAL_StatusTypeDef RTC_IO_WriteData(uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
HAL_StatusTypeDef I2C3_IO_WriteData(uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
HAL_StatusTypeDef I2C3_IO_ReadData(uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
uint32_t BSP_WaitIICStandbyState(uint8_t DeviceAddr); 
static HAL_StatusTypeDef I2C3_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);
uint32_t BSP_EEPROM_WritePage_x1(uint8_t *pBuffer, uint16_t WriteAddr, uint8_t *NumByteToWrite);

/*************************************************************************************************************************
 * @brief 初始化EEPROM硬件接口
 *
 * 该函数用于初始化EEPROM的IO接口，并设置写保护状态。
 * 函数会等待两个EEPROM设备(U1和X1)进入待机状态后返回。
 *
 * @return int 固定返回1表示初始化完成
 **************************************************************************************************************************/
int BSP_EepromInit(void)
{

  EEPROM_IO_Init();

	BSP_Eeprom_WP_Ctrl(EEPROM_WP_ENABLE);//1老监控！！

	BSP_EEPROM_WaitEepromStandbyState(EEPROM_DEV_ADDR_U1);
	//OSTimeDly(5u);
	BSP_EEPROM_WaitEepromStandbyState(EEPROM_DEV_ADDR_X1);
	return 1;
}

/****************************************************************************************************************************
 * @brief 控制EEPROM写保护引脚状态
 * @param u8CtrlStatus 引脚状态(GPIO_PIN_SET/GPIO_PIN_RESET)
 ****************************************************************************************************************************/
void BSP_Eeprom_WP_Ctrl(GPIO_PinState u8CtrlStatus)
{
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, u8CtrlStatus);
}

/*****************************************************************************************************************************
 * @brief 从EEPROM读取数据到缓冲区
 * @param pBuffer 指向存储读取数据的缓冲区
 * @param ReadAddr EEPROM中的起始读取地址
 * @param NumByteToRead 要读取的字节数
 * @retval EEPROM_OK 读取成功
 * @retval EEPROM_FAIL 读取失败
 ******************************************************************************************************************************/
uint32_t BSP_EEPROM_ReadBuffer(uint8_t *pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
  if (EEPROM_IO_ReadData(EEPROM_DEV_ADDR_U1, ReadAddr, pBuffer, NumByteToRead) != HAL_OK)
  {
    BSP_EEPROM_TIMEOUT_UserCallback();
    return EEPROM_FAIL;
  }
  return EEPROM_OK;
}

/*****************************************************************************************************************************
 * @brief 从EEPROM设备X1读取数据到缓冲区
 * @param pBuffer 指向存储读取数据的缓冲区指针
 * @param ReadAddr 读取起始地址
 * @param NumByteToRead 要读取的字节数
 * @retval EEPROM_OK 读取成功
 * @retval EEPROM_FAIL 读取失败
 ******************************************************************************************************************************/
uint32_t BSP_EEPROM_ReadBuffer_x1(uint8_t *pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
  if (EEPROM_IO_ReadData(EEPROM_DEV_ADDR_X1, ReadAddr, pBuffer, NumByteToRead) != HAL_OK)
  {
    BSP_EEPROM_TIMEOUT_UserCallback();
    return EEPROM_FAIL;
  }
  return EEPROM_OK;
}

/*****************************************************************************************************************************
 * @brief 向EEPROM写入数据缓冲区
 * @param pBuffer 要写入的数据缓冲区指针
 * @param WriteAddr 写入起始地址
 * @param NumByteToWrite 要写入的字节数
 * @return 操作状态 EEPROM_OK表示成功，其他值表示错误
 * @note 该函数会自动处理跨页写入的情况，确保数据正确写入EEPROM
 ******************************************************************************************************************************/
uint32_t BSP_EEPROM_WriteBuffer(uint8_t *pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t numofpage = 0, numofsingle = 0, count = 0;
  uint16_t addr = 0;
  uint8_t  dataindex = 0;
  uint32_t status = EEPROM_OK;

  addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - addr;
  numofpage =  NumByteToWrite / EEPROM_PAGESIZE;
  numofsingle = NumByteToWrite % EEPROM_PAGESIZE;
  
  if(addr == 0) 
  {
    if(numofpage == 0) 
    {
      /* Store the number of data to be written */
      dataindex = numofsingle;
      /* Start writing data */
      status = BSP_EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
      if(status != EEPROM_OK)
      {
        return status;
      }
    }
    else  
    {
      while(numofpage--)
      {
        /* Store the number of data to be written */
        dataindex = EEPROM_PAGESIZE;        
        status = BSP_EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
        
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }
      
      if(numofsingle!=0)
      {
        /* Store the number of data to be written */
        dataindex = numofsingle;          
        status = BSP_EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
      }
    }
  }
  else 
  {
    /* If NumByteToWrite < EEPROM_PAGESIZE */
    if(numofpage == 0) 
    {
      /* If the number of data to be written is more than the remaining space 
      in the current page: */
      if(NumByteToWrite > count)
      {
        /* Store the number of data to be written */
        dataindex = count;        
        /* Write the data contained in same page */
        status = BSP_EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
        
        /* Store the number of data to be written */
        dataindex = (NumByteToWrite - count);          
        /* Write the remaining data in the following page */
        status = BSP_EEPROM_WritePage((uint8_t*)(pBuffer + count), (WriteAddr + count), (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
      }      
      else      
      {
        /* Store the number of data to be written */
        dataindex = numofsingle;         
        status = BSP_EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
      }     
    }
    /* If NumByteToWrite > EEPROM_PAGESIZE */
    else
    {
      NumByteToWrite -= count;
      numofpage =  NumByteToWrite / EEPROM_PAGESIZE;
      numofsingle = NumByteToWrite % EEPROM_PAGESIZE;
      
      if(count != 0)
      {  
        /* Store the number of data to be written */
        dataindex = count;         
        status = BSP_EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(numofpage--)
      {
        /* Store the number of data to be written */
        dataindex = EEPROM_PAGESIZE;          
        status = BSP_EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;  
      }
      if(numofsingle != 0)
      {
        /* Store the number of data to be written */
        dataindex = numofsingle; 
        status = BSP_EEPROM_WritePage(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
      }
    }
  }  
                                   
  /* If all operations OK, return EEPROM_OK (0) */
  return EEPROM_OK;
}

/************************************************************************************************************************************
 * @brief 向EEPROM写入数据缓冲区
 * @param pBuffer 要写入的数据缓冲区指针
 * @param WriteAddr 写入起始地址
 * @param NumByteToWrite 要写入的字节数
 * @return 操作状态 EEPROM_OK表示成功，其他值表示错误
 * @note 该函数会自动处理跨页写入的情况，确保数据正确写入EEPROM
 ************************************************************************************************************************************/
uint32_t BSP_EEPROM_WriteBuffer_x1(uint8_t *pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t numofpage = 0, numofsingle = 0, count = 0;
  uint16_t addr = 0;
  uint8_t  dataindex = 0;
  uint32_t status = EEPROM_OK;

  addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - addr;
  numofpage =  NumByteToWrite / EEPROM_PAGESIZE;
  numofsingle = NumByteToWrite % EEPROM_PAGESIZE;
  
  if(addr == 0) 
  {
    if(numofpage == 0) 
    {
      /* Store the number of data to be written */
      dataindex = numofsingle;
      /* Start writing data */
      status = BSP_EEPROM_WritePage_x1(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
      if(status != EEPROM_OK)
      {
        return status;
      }
    }
    else  
    {
      while(numofpage--)
      {
        /* Store the number of data to be written */
        dataindex = EEPROM_PAGESIZE;        
        status = BSP_EEPROM_WritePage_x1(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
        
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }
      
      if(numofsingle!=0)
      {
        /* Store the number of data to be written */
        dataindex = numofsingle;          
        status = BSP_EEPROM_WritePage_x1(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
      }
    }
  }
  else 
  {
    /* If NumByteToWrite < EEPROM_PAGESIZE */
    if(numofpage == 0) 
    {
      /* If the number of data to be written is more than the remaining space 
      in the current page: */
      if(NumByteToWrite > count)
      {
        /* Store the number of data to be written */
        dataindex = count;        
        /* Write the data contained in same page */
        status = BSP_EEPROM_WritePage_x1(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
        
        /* Store the number of data to be written */
        dataindex = (NumByteToWrite - count);          
        /* Write the remaining data in the following page */
        status = BSP_EEPROM_WritePage_x1((uint8_t*)(pBuffer + count), (WriteAddr + count), (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
      }      
      else      
      {
        /* Store the number of data to be written */
        dataindex = numofsingle;         
        status = BSP_EEPROM_WritePage_x1(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
      }     
    }
    /* If NumByteToWrite > EEPROM_PAGESIZE */
    else
    {
      NumByteToWrite -= count;
      numofpage =  NumByteToWrite / EEPROM_PAGESIZE;
      numofsingle = NumByteToWrite % EEPROM_PAGESIZE;
      
      if(count != 0)
      {  
        /* Store the number of data to be written */
        dataindex = count;         
        status = BSP_EEPROM_WritePage_x1(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(numofpage--)
      {
        /* Store the number of data to be written */
        dataindex = EEPROM_PAGESIZE;          
        status = BSP_EEPROM_WritePage_x1(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;  
      }
      if(numofsingle != 0)
      {
        /* Store the number of data to be written */
        dataindex = numofsingle; 
        status = BSP_EEPROM_WritePage_x1(pBuffer, WriteAddr, (uint8_t*)(&dataindex));
        if(status != EEPROM_OK)
        {
          return status;
        }
      }
    }
  }  
                                   
  /* If all operations OK, return EEPROM_OK (0) */
  return EEPROM_OK;
}

/*****************************************************************************************************************************************************
 * @brief  向EEPROM写入一页数据
 * @param  pBuffer: 要写入的数据缓冲区指针
 * @param  WriteAddr: EEPROM内部写入地址
 * @param  NumByteToWrite: 要写入的字节数指针
 * @retval EEPROM_OK(0)表示成功，EEPROM_FAIL表示失败
 ******************************************************************************************************************************************************/
uint32_t BSP_EEPROM_WritePage(uint8_t *pBuffer, uint16_t WriteAddr, uint8_t *NumByteToWrite)
{
  uint32_t buffersize = *NumByteToWrite;
  uint32_t status = EEPROM_OK;
  
  /* Set the pointer to the Number of data to be written */
  if(EEPROM_IO_WriteData(EEPROM_DEV_ADDR_U1, WriteAddr, pBuffer, buffersize) != HAL_OK)  
  {
    BSP_EEPROM_TIMEOUT_UserCallback();
    status = EEPROM_FAIL;
  }
  
  while(BSP_EEPROM_WaitEepromStandbyState(EEPROM_DEV_ADDR_U1) != EEPROM_OK)
  {
    return EEPROM_FAIL;
  }

  /* If all operations OK, return EEPROM_OK (0) */
  return status;
}

/*****************************************************************************************************************************************************
 * @brief 向EEPROM设备X1写入一页数据
 * @param pBuffer 要写入的数据缓冲区指针
 * @param WriteAddr 写入起始地址
 * @param NumByteToWrite 要写入的字节数指针
 * @return EEPROM_OK(0)表示成功，EEPROM_FAIL表示失败
 * @note 该函数会等待EEPROM设备进入待机状态后再返回
 ******************************************************************************************************************************************************/
uint32_t BSP_EEPROM_WritePage_x1(uint8_t *pBuffer, uint16_t WriteAddr, uint8_t *NumByteToWrite)
{
  uint32_t buffersize = *NumByteToWrite;
  uint32_t status = EEPROM_OK;
  
  /* Set the pointer to the Number of data to be written */
  if(EEPROM_IO_WriteData(EEPROM_DEV_ADDR_X1, WriteAddr, pBuffer, buffersize) != HAL_OK)  
  {
    BSP_EEPROM_TIMEOUT_UserCallback();
    status = EEPROM_FAIL;
  }
  
  while(BSP_EEPROM_WaitEepromStandbyState(EEPROM_DEV_ADDR_X1) != EEPROM_OK)
  {
    return EEPROM_FAIL;
  }

  /* If all operations OK, return EEPROM_OK (0) */
  return status;
}

/**********************************************************************************************************************
 * @brief  等待EEPROM进入待机状态
 * @param  DeviceAddr: EEPROM设备地址
 * @retval EEPROM_OK: 操作成功
 *         EEPROM_TIMEOUT: 操作超时
 *********************************************************************************************************************/
uint32_t BSP_EEPROM_WaitEepromStandbyState(uint8_t DeviceAddr)
{
  /* Check if the maximum allowed number of trials has bee reached */
  if(EEPROM_IO_IsDeviceReady(DeviceAddr, EEPROM_MAX_TRIALS) != HAL_OK)
  {
    /* If the maximum number of trials has been reached, exit the function */
    BSP_EEPROM_TIMEOUT_UserCallback();
    return EEPROM_TIMEOUT;
  }
  return EEPROM_OK;
}
/*************************************************************************************************************************
 * @brief 等待IIC设备进入就绪状态
 * @param DeviceAddr 设备地址
 * @return 返回I2C设备是否就绪的状态
 * @note 该函数通过I2C3_IsDeviceReady接口检测设备是否就绪，最多尝试EEPROM_MAX_TRIALS次
 ************************************************************************************************************************/
uint32_t BSP_WaitIICStandbyState(uint8_t DeviceAddr)
{
  return (I2C3_IsDeviceReady(DeviceAddr, EEPROM_MAX_TRIALS));
}
__weak void BSP_EEPROM_TIMEOUT_UserCallback(void)
{
}

/**************************************************************************************************************************
 * @brief  初始化EEPROM的I/O接口
 * @note   该函数通过调用I2C初始化函数来配置EEPROM的通信接口
 **************************************************************************************************************************/
void EEPROM_IO_Init(void)
{
  I2Cx_Init();
}

/**************************************************************************************************************************
 * @brief 通过I2C向EEPROM写入数据
 * @param DevAddress 设备地址
 * @param MemAddress 内存地址
 * @param pBuffer 要写入的数据缓冲区指针
 * @param BufferSize 要写入的数据大小
 * @return HAL状态(OK/ERROR)
 **************************************************************************************************************************/
HAL_StatusTypeDef EEPROM_IO_WriteData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pBuffer, uint32_t BufferSize)
{
  return (I2Cx_WriteMultiple(DevAddress, MemAddress, 0x10, pBuffer, BufferSize));
}


/**************************************************************************************************************************
 * @brief 从EEPROM读取数据
 * @param DevAddress 设备地址
 * @param MemAddress 内存地址
 * @param pBuffer 数据缓冲区指针
 * @param BufferSize 缓冲区大小
 * @return HAL状态(成功/失败)
 **************************************************************************************************************************/
HAL_StatusTypeDef EEPROM_IO_ReadData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pBuffer, uint32_t BufferSize)
{
  return (I2Cx_ReadMultiple(DevAddress, MemAddress, 0x10, pBuffer, BufferSize));
}

/**************************************************************************************************************************
 * @brief  检查EEPROM设备是否就绪
 * @param  DevAddress 设备地址
 * @param  Trials 最大重试次数
 * @retval HAL_StatusTypeDef 返回设备状态(HAL_OK/HAL_ERROR)
 **************************************************************************************************************************/
HAL_StatusTypeDef EEPROM_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{
  return (I2Cx_IsDeviceReady(DevAddress, Trials));
}

/****************************************************************************************************************************
 * @brief  初始化I2C外设
 * @note   当I2C处于复位状态时进行初始化配置
 *         配置包括时钟速度、占空比、地址模式等参数
 *         如果I2C总线忙(被注释掉的代码)，会先反初始化再重新初始化
 ***************************************************************************************************************************/
static void I2Cx_Init(void)
{
  if (HAL_I2C_GetState(&heval_I2c) == HAL_I2C_STATE_RESET)
  {		
    heval_I2c.Instance = I2C3;
    heval_I2c.Init.ClockSpeed      = 100000;//原来400K 现在200K
    heval_I2c.Init.DutyCycle       = 0x00;
    heval_I2c.Init.OwnAddress1     = 0;
    heval_I2c.Init.AddressingMode  = 0x4000;
    heval_I2c.Init.DualAddressMode = 0x00;
    heval_I2c.Init.OwnAddress2     = 0;
    heval_I2c.Init.GeneralCallMode = 0x00;
    heval_I2c.Init.NoStretchMode   = 0x00;  

    /* Init the I2C */
    HAL_I2C_Init(&heval_I2c);
#if 0
		if (heval_I2c.Instance->SR2 & I2C_SR2_BUSY)
		{
			HAL_I2C_DeInit(&heval_I2c);
			OSTimeDly(500u);
			HAL_I2C_Init(&heval_I2c);
		}
#endif
  }
}


/***********************************************************************************************************************
 * @brief  通过I2C接口从指定设备地址和寄存器读取多个字节数据
 * @param  Addr 目标设备I2C地址
 * @param  Reg  要读取的寄存器地址
 * @param  MemAddress 存储器地址(用于I2C存储设备)
 * @param  Buffer 存放读取数据的缓冲区指针
 * @param  Length 要读取的数据长度
 * @retval HAL状态值(HAL_OK表示成功)
 ************************************************************************************************************************/
static HAL_StatusTypeDef I2Cx_ReadMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Read(&heval_I2c, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, I2C_TIMEOUT);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* I2C error occured */
    I2Cx_Error(Addr);
  }
  return status;
}

/*****************************************************************************************************************************
  * @brief  通过I2C3接口从指定地址读取多个字节数据
  * @param  Addr: 设备地址
  * @param  Reg: 寄存器地址
  * @param  MemAddress: 内存地址
  * @param  Buffer: 存储读取数据的缓冲区
  * @param  Length: 要读取的数据长度
  * @retval HAL状态
  ******************************************************************************************************************************/

static HAL_StatusTypeDef I2C3_ReadMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  status = HAL_I2C_Mem_Read(&heval_I2c3, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, I2C_TIMEOUT);

  if(status != HAL_OK)
  {
    I2C3_Error(Addr);
  }
  return status;
}

/*************************************************************************************************************************************
 * @brief  通过I2C接口向指定地址写入多个数据
 * @param  Addr: I2C设备地址
 * @param  Reg: 设备寄存器地址
 * @param  MemAddress: 内存地址偏移量
 * @param  Buffer: 要写入的数据缓冲区指针
 * @param  Length: 要写入的数据长度
 * @retval HAL状态值(HAL_OK表示成功)
 * @note   如果写入失败会自动尝试重新初始化I2C总线
 *************************************************************************************************************************************/
static HAL_StatusTypeDef I2Cx_WriteMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Write(&heval_I2c, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, I2C_TIMEOUT);
  
  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Re-Initiaize the I2C Bus */
    I2Cx_Error(Addr);
  }
  return status;
}

/*************************************************************************************************************************************
 * @brief  通过I2C3接口向指定地址写入多个数据
 * @param  Addr: 目标设备地址
 * @param  Reg: 寄存器地址
 * @param  MemAddress: 内存地址
 * @param  Buffer: 要写入的数据缓冲区
 * @param  Length: 要写入的数据长度
 * @retval HAL状态值(HAL_OK表示成功)
 *************************************************************************************************************************************/
static HAL_StatusTypeDef I2C3_WriteMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Write(&heval_I2c3, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, I2C_TIMEOUT);

  if(status != HAL_OK)
  {
    I2C3_Error(Addr);
  }
  BSP_WaitIICStandbyState(Addr);
  return status;
}

/*************************************************************************************************************************************
 * @brief  检查I2C设备是否就绪
 * @param  DevAddress 目标设备地址
 * @param  Trials 检测尝试次数
 * @retval HAL状态(HAL_OK/HAL_ERROR/HAL_BUSY/HAL_TIMEOUT)
 *************************************************************************************************************************************/
static HAL_StatusTypeDef I2Cx_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{
  return (HAL_I2C_IsDeviceReady(&heval_I2c, DevAddress, Trials, I2C_TIMEOUT));
}

/*************************************************************************************************************************************
 * @brief  检查I2C3设备是否就绪
 * @param  DevAddress 目标设备地址
 * @param  Trials 检测尝试次数
 * @retval HAL状态(HAL_OK/HAL_ERROR/HAL_BUSY/HAL_TIMEOUT)
 *************************************************************************************************************************************/
static HAL_StatusTypeDef I2C3_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{
  return (HAL_I2C_IsDeviceReady(&heval_I2c3, DevAddress, Trials, I2C_TIMEOUT));
}

/*************************************************************************************************************************************
 * @brief  I2C通信错误处理函数
 * @param  Addr: 发生错误的I2C设备地址
 * @note   该函数用于处理I2C通信错误，会先反初始化I2C总线，然后重新初始化
 *         属于内部实现细节，不对外暴露
 *************************************************************************************************************************************/
static void I2Cx_Error(uint8_t Addr)
{
  /* De-initialize the IOE comunication BUS */
  HAL_I2C_DeInit(&heval_I2c);
  
  /* Re-Initiaize the IOE comunication BUS */
  I2Cx_Init();
}
/*************************************************************************************************************************************
 * @brief  I2C3错误处理函数
 * @param  Addr: 发生错误的I2C设备地址
 * @note   该函数用于处理I2C3通信错误，当前实现仅进行I2C外设反初始化操作        
 *************************************************************************************************************************************/
static void I2C3_Error(uint8_t Addr)
{
  HAL_I2C_DeInit(&heval_I2c3);

  // I2C3_Init();
}
/*************************************************************************************************************************************
 * @brief  通过I2C向RTC设备写入数据
 * @param  DevAddress  目标设备地址
 * @param  MemAddress  内存地址
 * @param  pBuffer     数据缓冲区指针
 * @param  BufferSize  数据大小
 * @retval HAL状态码
 *************************************************************************************************************************************/
HAL_StatusTypeDef RTC_IO_WriteData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pBuffer, uint32_t BufferSize)
{
  return (I2Cx_WriteMultiple(DevAddress, MemAddress, 0x01, pBuffer, BufferSize));
}

/*************************************************************************************************************************************
 * @brief  通过I2C从RTC设备读取数据
 * @param  DevAddress  设备地址
 * @param  MemAddress  内存地址
 * @param  pBuffer     数据缓冲区指针
 * @param  BufferSize  缓冲区大小
 * @retval HAL状态(OK/ERROR)
 *************************************************************************************************************************************/
HAL_StatusTypeDef RTC_IO_ReadData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pBuffer, uint32_t BufferSize)
{
  return (I2Cx_ReadMultiple(DevAddress, MemAddress, 0x01, pBuffer, BufferSize));
}

/*************************************************************************************************************************************
 * @brief 写入RTC寄存器
 * @param Buffer 要写入的数据
 * @param WriteAddr 寄存器地址
 * @return 执行状态：EEPROM_OK 成功，EEPROM_FAIL 失败
 *************************************************************************************************************************************/
uint8_t BSP_WriteRtcReg(uint8_t Buffer, uint16_t WriteAddr)
{
  if (RTC_IO_WriteData(0x64, WriteAddr, &Buffer, 1) != HAL_OK)
  {
    return EEPROM_FAIL;
  }
  return EEPROM_OK;
}
/*************************************************************************************************************************************
 * @brief 从RTC寄存器读取数据
 * @param pBuffer 存储读取数据的缓冲区指针
 * @param ReadAddr 要读取的RTC寄存器地址
 * @return 读取状态：EEPROM_OK表示成功，EEPROM_FAIL表示失败
 *************************************************************************************************************************************/
uint8_t BSP_ReadRtcReg(uint8_t *pBuffer, uint16_t ReadAddr)
{
  if (RTC_IO_ReadData(0x64, ReadAddr, pBuffer, 1) != HAL_OK)
  {
    return EEPROM_FAIL;
  }
  return EEPROM_OK;
}
void I2C3_Init(void)
{
	//if(HAL_I2C_GetState(&heval_I2c3) == HAL_I2C_STATE_RESET)
	//{
	//	heval_I2c3.Instance = I2C3;
	//	heval_I2c3.Init.ClockSpeed      = 100000;
	//	heval_I2c3.Init.DutyCycle       = 0x00;
	//	heval_I2c3.Init.OwnAddress1     = 0;
	//	heval_I2c3.Init.AddressingMode  = 0x4000;
	//	heval_I2c3.Init.DualAddressMode = 0x00;
	//	heval_I2c3.Init.OwnAddress2     = 0;
	//	heval_I2c3.Init.GeneralCallMode = 0x00;
	//	heval_I2c3.Init.NoStretchMode   = 0x00;
	//	HAL_I2C_Init(&heval_I2c3);
	//}
}

/*************************************************************************************************************************************
 * @brief 通过I2C3向EEPROM写入数据
 * @param DevAddress 设备地址
 * @param MemAddress 内存地址
 * @param pBuffer 要写入的数据缓冲区指针
 * @param BufferSize 要写入的数据大小
 * @return HAL_StatusTypeDef 返回操作状态(HAL_OK表示成功)
 *************************************************************************************************************************************/
HAL_StatusTypeDef I2C3_IO_WriteData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pBuffer, uint32_t BufferSize)
{
  return (I2C3_WriteMultiple(DevAddress, MemAddress, 0x10, pBuffer, BufferSize));
}


/*************************************************************************************************************************************
 * @brief  通过I2C3从EEPROM读取数据
 * @param  DevAddress  设备地址
 * @param  MemAddress  内存地址
 * @param  pBuffer     数据缓冲区指针
 * @param  BufferSize  要读取的数据大小
 * @retval HAL状态码
 *************************************************************************************************************************************/
HAL_StatusTypeDef I2C3_IO_ReadData(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pBuffer, uint32_t BufferSize)
{
  return (I2C3_ReadMultiple(DevAddress, MemAddress, 0x10, pBuffer, BufferSize));
}


/*************************************************************************************************************************************
 * @brief  通过I2C3写入单个寄存器值
 * @param  DevAddress 设备地址
 * @param  pBuffer 要写入的数据缓冲区指针
 * @param  WriteAddr 寄存器地址
 * @retval EEPROM_OK 写入成功, EEPROM_FAIL 写入失败
 *************************************************************************************************************************************/
uint8_t BSP_WriteI2C3Reg(uint16_t DevAddress, uint8_t *pBuffer, uint16_t WriteAddr)
{
  if (I2C3_IO_WriteData(DevAddress, WriteAddr, pBuffer, 1) != HAL_OK)
  {
    return EEPROM_FAIL;
  }
  return EEPROM_OK;
}


/*************************************************************************************************************************************
 * @brief 从I2C3设备读取寄存器数据
 * @param DevAddress I2C设备地址
 * @param pBuffer 存储读取数据的缓冲区指针
 * @param ReadAddr 要读取的寄存器地址
 * @retval EEPROM_OK 读取成功
 * @retval EEPROM_FAIL 读取失败
 *************************************************************************************************************************************/
uint8_t BSP_ReadI2C3Reg(uint16_t DevAddress, uint8_t *pBuffer, uint16_t ReadAddr)
{
  if (I2C3_IO_ReadData(DevAddress, ReadAddr, pBuffer, 1) != HAL_OK)
  {
    return EEPROM_FAIL;
  }
  return EEPROM_OK;
}