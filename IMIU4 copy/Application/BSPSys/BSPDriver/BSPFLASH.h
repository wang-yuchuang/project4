/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPFLASH.c
** @Founder      : WYC834
** @Date         : 2025.09.03
** @Function     : BSPFLASH驱动函数模块
** @Instructions :
**===========================================================================================*/
#ifndef _BSPFLASH_H
#define _BSPFLASH_H
#include "stm32g0xx_hal.h"
/*=============================================================================================
** @Header file definition
**===========================================================================================*/

/*=============================================================================================
** @Macro definition
**===========================================================================================*/
/*
Flash特性：
256 Kbytes dual-bank
72-bit wide data read (64 bits plus 8 ECC bits)
72-bit wide data write (64 bits plus 8 ECC bits)
Page erase (2 Kbytes), bank (single-bank) erase, and mass (all-bank) erase
Bank 1：0x0800 0000-0x0801 FFFF     128k
Bank 2：0x0802 0000-0x0803 FFFF     128K
用户分区：
boot:           0x0800 0000-0x0800 9FFF   40K
app:            0x0800 A000-0x0802 2FFF   100K
app_bakup:      0x0802 3000-0x0803 BFFF   100K
boot_param:     0x0803 C000-0x0803 C7FF   2K            boot 参数
app_param:      0x0803 C800-0x0803 D000   2K            bms app应用参数
serial_num:     0x0803 D000-0x0803 D800   2K            生产序列号
*/
#define FLASH_ADDR_START            (FLASH_BASE)                        //flash起始地址
#define FLASH_ADDR_SIZE             (FLASH_SIZE)                        //flash总大小

#if 0
#define FLASH_BLOCK_NUM             (2)                                 //flash块数目(每块Flash大小见说明)

#define FLASH_BLOCK0_ADDR_START     (FLASH_BASE)                        //flash块0起始地址
#define FLASH_BLOCK0_SIZE           (0x400)                             //flash块0地址大小(共128K,用于BOOT代码)

#define FLASH_BLOCK1_ADDR_START     (FLASH_BLOCK0_ADDR_START+FLASH_BLOCK0_SIZE) //flash块1起始地址
#define FLASH_BLOCK1_SIZE           (0x400)                                     //flash块1地址大小(共100K,用于APP代码)

#define FLASH_BLOCK2_ADDR_START     (0x8023000)                       //flash块2起始地址
#define FLASH_BLOCK2_SIZE           (0x19000)                          //flash块2地址大小(共100K,用于APP代码)

#define FLASH_BLOCK3_ADDR_START     (0x807F000)                         //flash块3起始地址
#define FLASH_BLOCK3_SIZE           (0x800)                             //flash块3地址大小(共2K,用于BOOT参数)

#define FLASH_BLOCK4_ADDR_START     (0x807F800)                         //flash块4起始地址
#define FLASH_BLOCK4_SIZE           (0x800)                             //flash块4大小(共2K,用于APP参数)
#endif





#define FLASH_DISPART_NUM               (5U)                        //FLASH分区

#define FLASH_PART0_BOOT_ADDR           (0x08000000U)               //boot 起始地址   
#define FLASH_PART0_BOOT_SIZE           (0xA000U)                    //boot 限制大小 40K
#define FLASH_PART1_APP_ADDR            (FLASH_PART0_BOOT_ADDR + FLASH_PART0_BOOT_SIZE)               //app 起始地址   
#define FLASH_PART1_APP_SIZE            (0x34800U)                   //app 限制大小 210K
//** 没有备份区**//
#define FLASH_PART3_APP_PARAM_ADDR      (FLASH_PART1_APP_ADDR + FLASH_PART1_APP_SIZE)             //app参数 起始地址   
#define FLASH_PART3_APP_PARAM_SIZE      (2048U)                     //app参数 限制大小 2K

#define FLASH_PART4_BOOT_PARAM_ADDR     (FLASH_PART3_APP_PARAM_ADDR + FLASH_PART3_APP_PARAM_SIZE)             //boot参数 起始地址   
#define FLASH_PART4_BOOT_PARAM_SIZE     (2048U)                     //boot参数 限制大小 2K

#define FLASH_PART5_SERIALNUM_ADDR      (FLASH_PART4_BOOT_PARAM_ADDR + FLASH_PART4_BOOT_PARAM_SIZE)              //序列号 起始地址   
#define FLASH_PART5_SERIALNUM_SIZE      (2048U)                     //序列号 限制大小 2K


#define FLASH_PART1_APP_END_ADDR        (FLASH_PART1_APP_ADDR + FLASH_PART1_APP_SIZE - 1U)            //app 结束地址

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
typedef struct{
    u32 addr;
    u32 page_index;
    u32 bank_index;
}stru_flash_pos;

typedef enum{
    eFlash_part0 =0,
    eFlash_part1,
    // eFlash_part2,
    eFlash_part3,
    eFlash_part4,
    eFlash_partx,
}enum_flash_partnum;

/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPINTFLASHInit(void)
** @Input     : void
** @Output    : void
** @Function  : flash初始化
** @The notes :
**===========================================================================================*/
void BSPINTFLASHInit(void);

/*=============================================================================================
** @Name      : u32 BSPFlashErase(u32 addr,u32 len)
** @Input     : u32 addr：擦除起始地址 0x0800 0000~0x0803 FFFF
**              u32 len: 擦除长度（byte）,最小单位为页大小 FLASH_PAGE_SIZE
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除片内Flash指定地址长度
** @The notes : 
**===========================================================================================*/
u32 BSPFlashErase(u32 addr,u32 len);

/*=============================================================================================
** @Name      : void BSPFlashEraseChip(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除整个MCU整体FLASH
** @The notes : 用户慎用
**===========================================================================================*/
u8 BSPFlashEraseChip(void);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseBlock(u16 block)
** @Input     : u16 sector:FLASH块号[0,4]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除FLASH块
** @The notes : 原代码将block与分区概念混淆，因此需要纠正，此处的block对应的是flash分区
**===========================================================================================*/
u8 BSPINTFLASHEraseBlock(u16 block);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseSector(u32 addr)
** @Input     : addr:地址[0,131071](128K)
** @Output    : TRUE:扇区擦除成功 FALSE:扇区擦除错误
** @Function  : 擦除FLASH扇区(一页)
** @The notes : 1个扇区为512Byte(0.5K),地址必须是512倍数
**===========================================================================================*/
u8 BSPINTFLASHEraseSector(u32 addr);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWrite(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为8的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 由于stm32g0b1xx 内置flash最小cell为64bit，而本函数写入最小大小为单个字节，因此需要补全8字节；
**              注意，本操作前需要对内置flash进行擦除操作；
**===========================================================================================*/
u8 BSPINTFLASHWrite(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPFlashWriteHalfWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 新项目可删去，使用BSPINTFLASHWrite，本接口仅为兼容应用接口保留
**===========================================================================================*/
u8 BSPFlashWriteHalfWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPFlashWriteWord(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 新项目可删去，使用BSPINTFLASHWrite，本接口仅为兼容应用接口保留
**===========================================================================================*/
u8 BSPFlashWriteWord(u32 addr, u32 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPFlashWriteLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPFlashWriteLong(u32 addr, u32 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:读取地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadHalfWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按半字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadHalfWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len);

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按双字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len);

#endif
