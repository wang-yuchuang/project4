/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.c
  * @brief   This file provides code for the configuration
  *          of the FDCAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "fdcan.h"

/* USER CODE BEGIN 0 */
#include "BSPDIDO.h"
extern void FDCAN_RxFifo0CallbackTypeDef(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 3;
  hfdcan1.Init.NominalSyncJumpWidth = 5;
  hfdcan1.Init.NominalTimeSeg1 = 14;
  hfdcan1.Init.NominalTimeSeg2 = 5;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 3;
  hfdcan1.Init.DataTimeSeg1 = 8;
  hfdcan1.Init.DataTimeSeg2 = 3;
  hfdcan1.Init.StdFiltersNbr = 0;              /*不使用标准帧过滤器（仅用扩展帧）*/
  hfdcan1.Init.ExtFiltersNbr = 3;              /*3个扩展帧过滤器：本机/广播/UDS*/
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }


    //  /* 使能TDC功能 */
if (HAL_FDCAN_EnableTxDelayCompensation(&hfdcan1) != HAL_OK)
{
   Error_Handler();
}
/* 配置TDC偏移量（以数据位时间为单位） */
  HAL_FDCAN_ConfigTxDelayCompensation(&hfdcan1,hfdcan1.Init.DataTimeSeg1 + 1 , 0x03);

  /*配置接收滤波 - 3个过滤器：本机地址/广播/UDS升级*/
  FDCAN_FilterTypeDef sFilterConfig;
  u8 devAddr = SWReadData() + 20;  /*拨码地址 + 20 = 本机设备地址(SystemInfo_1[DeviceAddr])*/
  
  sFilterConfig.IdType       = FDCAN_EXTENDED_ID;
  sFilterConfig.FilterType   = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  
  /*过滤器0：业务报文 - 发给本机的报文 (daddr == devAddr)*/
  sFilterConfig.FilterIndex  = 0;
  sFilterConfig.FilterID1    = ((u32)devAddr << 8);
  sFilterConfig.FilterID2    = 0x0000FF00;       /*掩码：只比较bits[15:8]即daddr字段*/
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  /*过滤器1：业务报文 - 广播报文 (daddr == 0xFF)*/
  sFilterConfig.FilterIndex  = 1;
  sFilterConfig.FilterID1    = 0x0000FF00;
  sFilterConfig.FilterID2    = 0x0000FF00;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  /*过滤器2：UDS升级报文 (精确匹配 rx_phy_id)*/
  sFilterConfig.FilterIndex  = 2;
  sFilterConfig.FilterID1    = 0x0B110500 | (u32)devAddr;  /*REMOTE_TESTER_CAN_ID | (LOAD_DEV_ID<<8) | devAddr*/
  sFilterConfig.FilterID2    = 0xFFFFFFFF;       /*掩码：全位精确匹配*/
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE)!= HAL_OK)
  {
    Error_Handler();
  }
  /*配置中断回调函数*/
  if (HAL_FDCAN_RegisterRxFifo0Callback(&hfdcan1,FDCAN_RxFifo0CallbackTypeDef) != HAL_OK)
  {
    Error_Handler();
  }
  if(HAL_FDCAN_ConfigInterruptLines(&hfdcan1, FDCAN_IT_GROUP_RX_FIFO0, FDCAN_INTERRUPT_LINE0) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
   Error_Handler();
  }
   /*使能LINE0中断*/
  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END FDCAN1_Init 2 */





}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PC4     ------> FDCAN1_RX
    PC5     ------> FDCAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_FDCAN1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(TIM16_FDCAN_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM16_FDCAN_IT0_IRQn);
    HAL_NVIC_SetPriority(TIM17_FDCAN_IT1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM17_FDCAN_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PC4     ------> FDCAN1_RX
    PC5     ------> FDCAN1_TX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4|GPIO_PIN_5);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM16_FDCAN_IT0_IRQn);
    HAL_NVIC_DisableIRQ(TIM17_FDCAN_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
