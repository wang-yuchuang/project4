
#ifndef __API_FOR_APPLICATION__
#define __API_FOR_APPLICATION__
#include "BSPGD25Q32E.h"

#define UPDATE_FLAG_BYTE0    0xAA
#define UPDATE_FLAG_BYTE1    0x55
#define UPDATE_FLAG_EXFLASH  0xA0
#define UPDATE_FLAG_INFLASH  0x50

#define FLASH_USER_UPDATE_FLAG_ADDR         ((uint32_t)(FLASH_BASE + FLASH_SIZE - FLASH_PAGE_SIZE*2))//将FLASH尾部的倒数第3块2K作为升级标志写入

// #define PROCEDURE_MAGICNUM          0x7b5a4980
// typedef struct {
//     unsigned long *magic_num;
//     void (*procedure_init)(void);   //app导入程序初始化函数入口，内部如果使用函数，均使用ramfunction函数
//     void (*procedure_run)(void);    //app导入程序运行函数入口，内部如果使用函数，均使用ramfunction函数
//     void (*procedure_exit)(void);   //app导入程序结束函数入口，内部如果使用函数，均使用ramfunction函数
//     unsigned long reserved[31];     //保留占位
// }app_procedure_t;

int write_update_flag_for_app(uint32_t file_size,uint32_t flash_addr,uint8_t save_to_ext, uint8_t use_procedure);
#endif