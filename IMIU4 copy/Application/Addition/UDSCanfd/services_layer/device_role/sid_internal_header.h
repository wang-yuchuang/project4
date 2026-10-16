
/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2026-03-05 14:50:07
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2026-03-06 11:08:59
 * @FilePath: \IMIU4_Canfd_Uds_App\Application\Addition\UDSCanfd\services_layer\device_role\sid_internal_header.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/* *********************************
 * can uds 内部服务处理函数的头文件声明
 ***********************************/
#ifndef __SID_INTERNAL_HEADER_H__
#define __SID_INTERNAL_HEADER_H__
#include "SEGGER_RTT.h"
//宏定义
#define UDS_SRV_RET_ID_OFFSET           0x40

#ifndef d_printf
#define d_printf(...)       SEGGER_RTT_printf(0,__VA_ARGS__)
#endif

//用于安全访问的相关控制结构体
typedef enum {
    SECURITY_LEVEL0 = 0,
    SECURITY_LEVEL1 = 1,
} SECURITY_ACCESS_LEVEL_E;
struct SECURITY_CTRL {
    SECURITY_ACCESS_LEVEL_E level;
    unsigned char seed[4];
};

//用于数据传输中的通信控制结构体
typedef enum {
    TRANSFER_DOWNLOAD = 0,
    TRANSFER_UPLOAD = 1
} TRANSFER_DIRECTION_E;
struct TRANSFER_CTRL {
    TRANSFER_DIRECTION_E direction;     //记录当前传输方向
    unsigned char is_first_block;       //标识是否为首包
    unsigned char block_counter;        //记录接收到的当前block计数
    unsigned char last_block_counter;   //记录上一次接收到到的block，用于和当前block计数对比
    unsigned long prog_addr;            //记录当前要写入的逻辑编程地址，当持续收到传输数据包，变量会增长
};

//用于services层使用的内部变量声明
extern unsigned char cur_session_type;
extern unsigned long pg_memory_address,pg_memory_size;
extern struct TRANSFER_CTRL cur_transfer_ctrl;


//基于SID的每种服务数据的独立处理函数声明
void sid10_diagnostic_session_ctrl_solve(unsigned char *msg_buff,unsigned short msg_len);          // SID-10, 诊断会话控制
void sid11_ecu_reset_solve(unsigned char *msg_buff,unsigned short msg_len);                        // SID-11，ECU复位
void sid22_read_data_by_id_solve(unsigned char *msg_buff,unsigned short msg_len);                  // SID-22，根据数据ID读取数据
void sid2e_write_data_by_id_solve(unsigned char *msg_buff,unsigned short msg_len);                 // SID-2E，根据数据ID写入数据
void sid27_security_access_solve(unsigned char *msg_buff,unsigned short msg_len);                  // SID-27，安全访问
void sid28_communication_ctrl_solve(unsigned char *msg_buff,unsigned short msg_len);               // SID-28，通信控制
void sid31_routine_ctrl_solve(unsigned char *msg_buff,unsigned short msg_len);                     // SID-31，例程控制
void sid34_request_download_solve(unsigned char *msg_buff,unsigned short msg_len);                 // SID-34，请求下载 (tester -> device)
void sid36_transfer_data_solve(unsigned char *msg_buff,unsigned short msg_len);                    // SID-36，传输数据
void sid37_transfer_exit_solve(unsigned char *msg_buff,unsigned short msg_len);                    // SID-37，传输结束
void sid3e_tester_present_solve(unsigned char *msg_buff,unsigned short msg_len);                   // SID-3E，通知tester在线

//用于services层的公用函数
void uds_ack_accept(unsigned char *msg_buff,unsigned short len);
void uds_ack_refuse(unsigned char ds_type,unsigned char para);

#endif