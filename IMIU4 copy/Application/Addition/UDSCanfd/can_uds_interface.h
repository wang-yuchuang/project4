
#ifndef __CAN_UDS_INTERFACE_H__
#define __CAN_UDS_INTERFACE_H__
#include "can_uds_arch.h"
#include "BSPCAN.h"
#define REMOTE_TESTER_CAN_ID                0x0B110000//远端诊断仪（监控）的CAN-ID
#define LOCAL_DEVICE_CAN_ID                 0x0C110000//自身作为device设备的CAN-ID
#define LOAD_DEV_ID                         (0x05)    /*设备识别码由拨码地址区分(设备类型:  5:IMIU4)*/


/**
 * @brief 初始化uds协议栈，初始化uds相关结构，在调用所有uds流程前调用一次
 * 
 */
void init_uds(void);

/**
 * @brief 回调处理uds的数据, ecu端调用此函数
 * 
 */
void uds_proc_device(void);

/**
 * @brief 回调处理uds的数据, tester端调用此函数
 * 
 */
void uds_proc_tester(void);

/**
 * @brief 收到UDS需要的CAN数据后，调用此函数接收
 * 
 * @param p_can_data ，收到的CAN帧结构体指针
 */
void uds_recv_data(struct CAN_DATA *p_can_data);
void uds_recv_data_fristhandle(t_CANMSG *msg);
#endif