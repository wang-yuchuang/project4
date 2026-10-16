#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_gpio.h"
#include "api_for_application.h"
#include "stm32g0xx_hal_rcc.h"


/**
 * @brief 写入升级标志
 * 升级标志段包含几个内容：
 * [0~1]字节，AA55, 用于标记 存在升级数据，需要升级
 * [2]字节，外部FLASH标识  如果使用外部nor-flash作为升级数据暂存，则该段为外部nor-flash的升级数据存放起始地址，如果使用的是内部eflash作为升级数据暂存，boot将忽略该4字节
 * [4~7]字节，待升级数据的大小，小端排序
 * 
 * @param file_size ,升级数据(文件)的大小
 * @param flash_addr ,外部norflash存放升级数据的起始地址
 * @param save_to_ext ,升级文件暂存是否保存在外部flash, 1表示存在外部扩展的NORFLASH，0表示保存在内部eflash
 * @return int ，成功返回0，失败返回-1
 */
int write_update_flag_for_app(uint32_t file_size,uint32_t flash_addr,uint8_t save_to_ext, uint8_t use_procedure)
{//写入升级标志
    FLASH_EraseInitTypeDef EraseInitStruct;
    // app_procedure_t procedure_stuct;
    uint32_t PageError = 0;
    uint32_t write_addr = FLASH_USER_UPDATE_FLAG_ADDR;
    uint64_t *pdata = 0;
    uint8_t tmp_array[8] = {UPDATE_FLAG_BYTE0,UPDATE_FLAG_BYTE1,0,0,0,0,0,0};

    //标识升级暂存文件是否为外部FLASH, 值为三种状态（第三字节），00表示按原来设定，A0表示强制外部FLASH，50表示强制内部FLASH
    if(save_to_ext)//参数非0表示存在外部，否则为内部eflash
        tmp_array[2] = UPDATE_FLAG_EXFLASH;
    else
        tmp_array[2] = UPDATE_FLAG_INFLASH;

    //小端写入 
    tmp_array[4] = (uint8_t)(file_size & 0xff);
    tmp_array[5] = (uint8_t)((file_size>>8) & 0xff);
    tmp_array[6] = (uint8_t)((file_size>>16) & 0xff);
    tmp_array[7] = (uint8_t)((file_size>>24) & 0xff);
    HAL_FLASH_Unlock();
    EraseInitStruct.TypeErase     = FLASH_CR_PER;
    EraseInitStruct.Banks         = FLASH_BANK_2;       //要擦除的位置位于flash尾部，BANK2
    EraseInitStruct.Page          = FLASH_PAGE_NB-2;    //擦除起始的sector编号,倒数第2个
    EraseInitStruct.NbPages       = 1;                  //擦除的sector数量


    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){
        HAL_FLASH_Lock();
        return -1;
    }

    //写入升级标志和文件大小
    pdata = (uint64_t*)&tmp_array[0];
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
                              write_addr, 
                              ((uint64_t)*pdata)) == HAL_OK){
        write_addr += 8;
    }else{
        HAL_FLASH_Lock();
        return -1;
    }
    
    //写入外部flash存放升级文件的起始地址
    tmp_array[0] = (uint8_t)(flash_addr & 0xff);
    tmp_array[1] = (uint8_t)((flash_addr>>8) & 0xff);
    tmp_array[2] = (uint8_t)((flash_addr>>16) & 0xff);
    tmp_array[3] = (uint8_t)((flash_addr>>24) & 0xff);
    
    pdata = (uint64_t*)&tmp_array[0];
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
                              write_addr, 
                              ((uint64_t)*pdata)) == HAL_OK){
    }else{
        HAL_FLASH_Lock();
        return -1;
    }

    HAL_FLASH_Lock();
    return 0;

#if 0
    //写入外部flash的app操作行为结构体
    if(use_procedure){
        magic_num_in_ram = PROCEDURE_MAGICNUM;
        procedure_stuct.magic_num = &magic_num_in_ram;
        procedure_stuct.procedure_init = app_procedure_init;
        procedure_stuct.procedure_run = app_procedure_run;
        procedure_stuct.procedure_exit = app_procedure_exit;
        for(int i=0;i<32;i++){//对reserved位填0xff
            procedure_stuct.reserved[i] = 0xfffffffful;
        }
        pdata = (uint64_t*)&procedure_stuct;
        write_addr += 8;
        for(int i=0; i<sizeof(app_procedure_t); ){
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, 
                              write_addr, 
                              ((uint64_t)*pdata)) == HAL_OK){
            }else{
                HAL_FLASH_Lock();
                return -1;
            }
            i += 8;
            pdata += 1;
            write_addr += 8;
        }
    }
#endif
}

// 定义读写GPIO的函数
#if 0
void rf_gpio_setpinspeed(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t  Speed) @ "RAMCODE"
{
  MODIFY_REG(GPIOx->OSPEEDR, ((Pin * Pin) * GPIO_OSPEEDR_OSPEED0), ((Pin * Pin) * Speed));
}

void rf_gpio_setpinoutputtype(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t OutputType)  @ "RAMCODE"
{
  MODIFY_REG(GPIOx->OTYPER, PinMask, (PinMask * OutputType));
}

void rf_gpio_setpinpull(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Pull)  @ "RAMCODE"
{
  MODIFY_REG(GPIOx->PUPDR, ((Pin * Pin) * GPIO_PUPDR_PUPD0), ((Pin * Pin) * Pull));
}
#endif

#if 0

//常用的gpio操作接口
static ErrorStatus rf_gpio_init(GPIO_TypeDef *GPIOx, LL_GPIO_InitTypeDef *GPIO_InitStruct);
static int rf_gpio_read(GPIO_TypeDef *GPIOx, uint32_t PinMask);
static void rf_gpio_write(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t PinVal);
static void rt_gpio_toggle(GPIO_TypeDef *GPIOx, uint32_t PinMask);

/**
 * @brief magic_num_in_ram
 * 保存用于有效性判断的魔数
 */
unsigned long magic_num_in_ram @ "RAMCODE";
int count @ "RAMCODE";

/**
 * @brief app_procedure_init
 * 应用的procedure的初始化api
 */
void app_procedure_init(void)  @ "RAMCODE"
{//测试PC8，PWM1输出
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    
    LL_GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    rf_gpio_init(GPIOC,&gpio_init);
    
    gpio_init.Pin = GPIO_PIN_8;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    rf_gpio_init(GPIOD,&gpio_init);
    gpio_init.Pin = GPIO_PIN_8;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    rf_gpio_init(GPIOF,&gpio_init);

	rf_gpio_write(GPIOC,GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,1);
    rf_gpio_write(GPIOF, GPIO_PIN_8, 0);
    rf_gpio_write(GPIOD, GPIO_PIN_8, 1);
}

/**
 * @brief app_procedure_run
 * 应用的procedure的运行api
 */
void app_procedure_run(void)  @ "RAMCODE"
{//测试PC8，PWM1输出, boot会按照100us的频率调用一次，要输出1KZ的频率
   ;
}

/**
 * @brief app_procedure_exit
 * 应用的procedure的退出api
 */
void app_procedure_exit(void)  @ "RAMCODE"
{//退出时恢复PC8为高阻态
    LL_GPIO_InitTypeDef gpio_init;

    gpio_init.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    rf_gpio_init(GPIOC,&gpio_init);
 
}



void rf_set_pin_speed(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t  Speed)  @ "RAMCODE"
{
  MODIFY_REG(GPIOx->OSPEEDR, ((Pin * Pin) * GPIO_OSPEEDR_OSPEED0), ((Pin * Pin) * Speed));
}
void rf_set_pin_output_type(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t OutputType)  @ "RAMCODE"
{
  MODIFY_REG(GPIOx->OTYPER, PinMask, (PinMask * OutputType));
}

void rf_set_pin_pull(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Pull) @ "RAMCODE"
{
  MODIFY_REG(GPIOx->PUPDR, ((Pin * Pin) * GPIO_PUPDR_PUPD0), ((Pin * Pin) * Pull));
}

void rf_set_afPin_0_7(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Alternate) @ "RAMCODE"
{
  MODIFY_REG(GPIOx->AFR[0], ((((Pin * Pin) * Pin) * Pin) * GPIO_AFRL_AFSEL0),
             ((((Pin * Pin) * Pin) * Pin) * Alternate));
}

void rf_set_afPin_8_15(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Alternate) @ "RAMCODE"
{
  MODIFY_REG(GPIOx->AFR[1], (((((Pin >> 8U) * (Pin >> 8U)) * (Pin >> 8U)) * (Pin >> 8U)) * GPIO_AFRH_AFSEL8),
             (((((Pin >> 8U) * (Pin >> 8U)) * (Pin >> 8U)) * (Pin >> 8U)) * Alternate));
}

void rf_set_pin_mode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode) @ "RAMCODE"
{
  MODIFY_REG(GPIOx->MODER, ((Pin * Pin) * GPIO_MODER_MODE0), ((Pin * Pin) * Mode));
}
ErrorStatus rf_gpio_init(GPIO_TypeDef *GPIOx, LL_GPIO_InitTypeDef *GPIO_InitStruct)  @ "RAMCODE"
{
  uint32_t pinpos;
  uint32_t currentpin;

  /* ------------------------- Configure the port pins ---------------- */
  /* Initialize  pinpos on first pin set */
  pinpos = 0;

  /* Configure the port pins */
  while (((GPIO_InitStruct->Pin) >> pinpos) != 0x00u)
  {
    /* Get current io position */
    currentpin = (GPIO_InitStruct->Pin) & (0x00000001uL << pinpos);

    if (currentpin != 0x00u)
    {
      if ((GPIO_InitStruct->Mode == LL_GPIO_MODE_OUTPUT) || (GPIO_InitStruct->Mode == LL_GPIO_MODE_ALTERNATE))
      {
        /* Speed mode configuration */
        rf_set_pin_speed(GPIOx, currentpin, GPIO_InitStruct->Speed);
        /* Output mode configuration*/
        rf_set_pin_output_type(GPIOx, currentpin, GPIO_InitStruct->OutputType);
      }

      /* Pull-up Pull down resistor configuration*/
      rf_set_pin_pull(GPIOx, currentpin, GPIO_InitStruct->Pull);

      if (GPIO_InitStruct->Mode == LL_GPIO_MODE_ALTERNATE)
      {
        /* Speed mode configuration */
        if (currentpin < LL_GPIO_PIN_8)
        {
          rf_set_afPin_0_7(GPIOx, currentpin, GPIO_InitStruct->Alternate);
        }
        else
        {
          rf_set_afPin_8_15(GPIOx, currentpin, GPIO_InitStruct->Alternate);
        }
      }

      /* Pin Mode configuration */
      rf_set_pin_mode(GPIOx, currentpin, GPIO_InitStruct->Mode);
    }
    pinpos++;
  }

  return (SUCCESS);
}

// // LL_GPIO_PIN_X
int rf_gpio_read(GPIO_TypeDef *GPIOx, uint32_t PinMask)  @ "RAMCODE"
{
    uint32_t value = 0;
    
    value = LL_GPIO_ReadInputPort(GPIOx);
    if(value & PinMask)
        return 1;
    else
        return 0;
}

// LL_GPIO_PIN_X
void rf_gpio_write(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t PinVal)  @ "RAMCODE"
{
    uint32_t value = 0;

    value = (uint32_t)(READ_REG(GPIOx->IDR));
    if(PinVal){//输出高电平
        value |= PinMask;
    }else{//输出低电平
        value &= ~PinMask;
    }

	WRITE_REG(GPIOx->ODR, value);
}

void rf_toggle_pin(GPIO_TypeDef *GPIOx, uint32_t PinMask) @ "RAMCODE"
{
  uint32_t odr = READ_REG(GPIOx->ODR);
  WRITE_REG(GPIOx->BSRR, ((odr & PinMask) << 16u) | (~odr & PinMask));
}

void rt_gpio_toggle(GPIO_TypeDef *GPIOx, uint32_t PinMask)  @ "RAMCODE"
{
    rf_toggle_pin(GPIOx, PinMask);
}
#endif