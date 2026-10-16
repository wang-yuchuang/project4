/**
  ******************************************************************************
  * @file    bsp_usb.c
  * @author  Zuo Zhenglai (derek)
  * @version V1.0.0
  * @date    15-Nov-2014
  * @brief   USB host Firmware Upgrade file
  ******************************************************************************
 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp_usb.h"
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBH_HandleTypeDef hUSBHost;
#define	hUsbHostFS hUSBHost
FW_ApplicationTypeDef Appli_state = APPLICATION_DISCONNECT;

/* Private function prototypes -----------------------------------------------*/
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);


/* Private functions ---------------------------------------------------------*/

/******************************************************************************************************************************
 * @brief 初始化USB主机控制器
 * 
 * 该函数用于初始化USB主机控制器，注册MSC类设备，并启动USB主机服务。
 * 调用后，系统将能够检测和管理连接的USB大容量存储设备。
 ******************************************************************************************************************************/
void vUsbInit(void)
{	
  USBH_Init(&hUSBHost, USBH_UserProcess, 0);
  USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
  USBH_Start(&hUSBHost);
}

/******************************************************************************************************************************
 * @brief  USB主机处理函数
 * @note   调用USBH_Process处理USB主机事务
 * @param  None
 * @retval None
 ******************************************************************************************************************************/
void BSP_USBH_Process(void)
{
  USBH_Process(&hUSBHost);
}

/******************************************************************************************************************************
 * @brief  USB主机用户处理回调函数
 * @param  phost: USB主机控制器句柄
 * @param  id: 用户事件ID
 * @note   根据不同的事件ID更新应用状态(Appli_state)
 *         处理的事件包括：配置选择、断开连接、类激活、设备连接等
 ******************************************************************************************************************************/
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
  switch (id)
  { 
  case HOST_USER_SELECT_CONFIGURATION:
    break;
    
  case HOST_USER_DISCONNECTION:
    Appli_state = APPLICATION_DISCONNECT;
    break;
    
  case HOST_USER_CLASS_ACTIVE:
    Appli_state = APPLICATION_READY;
    break;
    
  case HOST_USER_CONNECTION:
    Appli_state = APPLICATION_CONNECT;    
    break;
     
  default:
    break; 
  }
}

/************************ (C) COPYRIGHT INFY POWER  *****END OF FILE****/
