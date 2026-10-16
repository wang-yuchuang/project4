
#ifndef __BSPGD25Q32E_H
#define __BSPGD25Q32E_H

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "spi.h"
#include "BSPTypeDef.h"
/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/****************外部flash地址分配**********************************************************************************************************/
#define FlashMap_APP_BAK_ADDR            (OUT_FLASH_1_BLOCK_ADDR)                           //APP备份区起始地址
#define FlashMap_APP_BLOCK_NUM           (4)                                                //APP备份区 使用block数量
#define EXT_FLASH_BLOCK_SIZE             (0x10000U)                                         //block大小
#define FlashMap_APP_BAK_SIZE            (EXT_FLASH_BLOCK_SIZE * FlashMap_APP_BLOCK_NUM)    //APP备份区大小



#define FlashMap_Test                    (0x3D0000) //工装测试地址
#define FlashMap_Sysmsy                  (0x3F0000) //系统信息存储地址（包含条码）



/****************移植需要更改的内容，需要根据实际引脚更改***************/
#define GD25Qx_Enable()                  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
#define GD25Qx_Disable()                 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
/****************移植需要更改的内容，需要根据实际引脚更改***************/
/** 
  * @brief  GD25Q32E Configuration  
  */  
#define GD25Q32E_FLASH_SIZE                  0x400000  /* 32 MBits => 4MBytes */
#define GD25Q32E_BLOCK_SIZE                  0x10000   /* 64 blocks of 64KBytes */
#define GD25Q32E_SECTOR_SIZE                 0x1000    /* 1024 sectors of 4kBytes */
#define GD25Q32E_PAGE_SIZE                   0x100     /* 16384 pages of 256 bytes */

#define GD25Q32E_DUMMY_CYCLES_READ           4
#define GD25Q32E_DUMMY_CYCLES_READ_QUAD      10

#define GD25Q32E_BULK_ERASE_MAX_TIME         250000
#define GD25Q32E_SECTOR_ERASE_MAX_TIME       3000
#define GD25Q32E_SUBSECTOR_ERASE_MAX_TIME    800
#define GD25Qx_TIMEOUT_VALUE 1000
#define GD25Qx_GETSTATE_COUNT 6000


/** 
  * @brief  GD25Q32E Commands  
  */  
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define BLOCK_ERASE_CMD                      0xD8
#define CHIP_ERASE_CMD                       0xC7

/* 新增 */
#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75
#define GD25QXX_DUMMY_BYTE          0xAAU
#define GD25QXX_FLASH_WIP_TIMEOUT   40000U
#define GD25QXX_WIP_FLAG            0x01U
#define GD25QXX_READ_IDENTIFICATION 0x9FU
#define GD25QXX_READ_DATA_NORMAL    0x03U
#define GD25QXX_WR_ENABLE           0x06U
#define GD25QXX_PAGE_PROGRAM        0x02U
#define GD25QXX_RD_STATUSREG        0x05U
#define GD25QXX_ERASE_CHIP          0xc7U
#define GD25QXX_ERASE_BLOCK         0xd8U
#define GD25QXX_ERASE_SECTOR        0x20U

/* Flag Status Register */
#define GD25Q32E_FSR_OK                    ((uint8_t)0x00)    /*!< OK */
#define GD25Q32E_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define GD25Q32E_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */

#define GD25Qx_OK            ((uint8_t)0x00)
#define GD25Qx_ERROR         ((uint8_t)0x01)
#define GD25Qx_BUSY          ((uint8_t)0x02)
#define GD25Qx_TIMEOUT       ((uint8_t)0x03)


//新增
typedef struct {
  uint32_t identification;
  uint32_t size;
  uint32_t blockNum;
} Gd25qxxDeviceType;

//新增
// 一块大小为64K
typedef enum
{
    OUT_FLASH_1_BLOCK_ADDR = 0x010000U,
    OUT_FLASH_2_BLOCK_ADDR = 0x020000U,
    OUT_FLASH_3_BLOCK_ADDR = 0x030000U,
    OUT_FLASH_4_BLOCK_ADDR = 0x040000U,
    OUT_FLASH_5_BLOCK_ADDR = 0x050000U,
    OUT_FLASH_6_BLOCK_ADDR = 0x060000U,
    OUT_FLASH_7_BLOCK_ADDR = 0x070000U,
    OUT_FLASH_8_BLOCK_ADDR = 0x080000U,
    OUT_FLASH_9_BLOCK_ADDR = 0x090000U,
    OUT_FLASH_10_BLOCK_ADDR = 0x0A0000U,
    OUT_FLASH_11_BLOCK_ADDR = 0x0B0000U,
    OUT_FLASH_12_BLOCK_ADDR = 0x0C0000U,
    OUT_FLASH_13_BLOCK_ADDR = 0x0D0000U,
    OUT_FLASH_14_BLOCK_ADDR = 0x0E0000U,
    OUT_FLASH_15_BLOCK_ADDR = 0x0F0000U,
    OUT_FLASH_16_BLOCK_ADDR = 0x100000U,
    OUT_FLASH_17_BLOCK_ADDR = 0x110000U,
    OUT_FLASH_18_BLOCK_ADDR = 0x120000U,
    OUT_FLASH_19_BLOCK_ADDR = 0x130000U,
    OUT_FLASH_20_BLOCK_ADDR = 0x140000U,
    OUT_FLASH_21_BLOCK_ADDR = 0x150000U,
    OUT_FLASH_22_BLOCK_ADDR = 0x160000U,
    OUT_FLASH_23_BLOCK_ADDR = 0x170000U,
    OUT_FLASH_24_BLOCK_ADDR = 0x180000U,
    OUT_FLASH_25_BLOCK_ADDR = 0x190000U,
    OUT_FLASH_26_BLOCK_ADDR = 0x1A0000U,
    OUT_FLASH_27_BLOCK_ADDR = 0x1B0000U,
    OUT_FLASH_28_BLOCK_ADDR = 0x1C0000U,
    OUT_FLASH_29_BLOCK_ADDR = 0x1D0000U,
    OUT_FLASH_30_BLOCK_ADDR = 0x1E0000U,
    OUT_FLASH_31_BLOCK_ADDR = 0x1F0000U,
    OUT_FLASH_32_BLOCK_ADDR = 0x200000U,
    OUT_FLASH_33_BLOCK_ADDR = 0x210000U,
    OUT_FLASH_34_BLOCK_ADDR = 0x220000U,
    OUT_FLASH_35_BLOCK_ADDR = 0x230000U,
    OUT_FLASH_36_BLOCK_ADDR = 0x240000U,
    OUT_FLASH_37_BLOCK_ADDR = 0x250000U,
    OUT_FLASH_38_BLOCK_ADDR = 0x260000U,
    OUT_FLASH_39_BLOCK_ADDR = 0x270000U,
    OUT_FLASH_40_BLOCK_ADDR = 0x280000U,
    OUT_FLASH_41_BLOCK_ADDR = 0x290000U,
    OUT_FLASH_42_BLOCK_ADDR = 0x2A0000U,
    OUT_FLASH_43_BLOCK_ADDR = 0x2B0000U,
    OUT_FLASH_44_BLOCK_ADDR = 0x2C0000U,
    OUT_FLASH_45_BLOCK_ADDR = 0x2D0000U,
    OUT_FLASH_46_BLOCK_ADDR = 0x2E0000U,
    OUT_FLASH_47_BLOCK_ADDR = 0x2F0000U,
    OUT_FLASH_48_BLOCK_ADDR = 0x300000U,
    OUT_FLASH_49_BLOCK_ADDR = 0x310000U,
    OUT_FLASH_50_BLOCK_ADDR = 0x320000U,
    OUT_FLASH_51_BLOCK_ADDR = 0x330000U,
    OUT_FLASH_52_BLOCK_ADDR = 0x340000U,
    OUT_FLASH_53_BLOCK_ADDR = 0x350000U,
    OUT_FLASH_54_BLOCK_ADDR = 0x360000U,
    OUT_FLASH_55_BLOCK_ADDR = 0x370000U,
    OUT_FLASH_56_BLOCK_ADDR = 0x380000U,
    OUT_FLASH_57_BLOCK_ADDR = 0x390000U,
    OUT_FLASH_58_BLOCK_ADDR = 0x3A0000U,
    OUT_FLASH_59_BLOCK_ADDR = 0x3B0000U,
    OUT_FLASH_60_BLOCK_ADDR = 0x3C0000U,
    OUT_FLASH_61_BLOCK_ADDR = 0x3D0000U,
    OUT_FLASH_62_BLOCK_ADDR = 0x3E0000U,
    OUT_FLASH_63_BLOCK_ADDR = 0x3F0000U,
    OUT_FLASH_64_BLOCK_ADDR = 0x400000U
} Enum_OutFlashBlockAddrType;



extern bool irq_disabled;
/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Init(void)
** @Input     : void
** @Output    : void
** @Function  : 外部flash初始化
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Init(void);

/*=============================================================================================
** @Name      : void BSP_GD25Qx_Read_ID(uint8_t *ID)
** @Input     : ID:GD25Q32E的芯片ID
** @Output    : 
** @Function  : 返回GD25Q32E的ID
** @The notes : 
**===========================================================================================*/
void BSP_GD25Qx_Read_ID(uint8_t *ID);
/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
** @Input     : pData:读操作要写入的数组地址 ReadAddr：读地址 Size：读数组长度
** @Output    : 返回读操作是否成功
** @Function  : falsh读操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
** @Input     : pData:写操作要写入的数组地址 ReadAddr：写地址 Size：写数组长度
** @Output    : 返回写操作是否成功
** @Function  : falsh写操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Erase_SECTOR(uint32_t Address)
** @Input     : Address:擦除操作的扇区地址
** @Output    : 返回扇区擦除操作是否成功
** @Function  : falsh扇区擦除操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Erase_Sector(uint32_t Address);

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Erase_Block(uint32_t Address)
** @Input     : Address:擦除操作的块地址
** @Output    : 返回块擦除操作是否成功
** @Function  : falsh块擦除操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Erase_Block(uint32_t Address);

/*=============================================================================================
** @Name      : uint8_t BSP_GD25Qx_Erase_Chip(void)
** @Input     : 
** @Output    : 返回擦除操作是否成功
** @Function  : falsh擦除操纵
** @The notes : 
**===========================================================================================*/
uint8_t BSP_GD25Qx_Erase_Chip(void);




uint8 DRV_GD25QXX_ReadID(uint32 *id);

uint8 DRV_FLASH_ReadBytes(uint8 *pBuf, uint32 addr, uint32 numBytes);

uint8 DRV_GD25QXX_PageProgram(const uint8 *pBuf, uint32 addr, uint32 numBytes);

uint8 DRV_FLASH_WriteBytes(const uint8 *pBuf, uint32 addr, uint32 numBytes);

uint8 DRV_GD25QXX_EraseChip(void);

uint8 DRV_FLASH_EraseBlock(uint32 blockAddr);

uint8 DRV_FLASH_EraseSector(uint32 sectorAddr);
#endif /* __BSPGD25QXX_H */