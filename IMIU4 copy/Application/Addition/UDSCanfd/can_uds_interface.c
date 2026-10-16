/*
 * @Author: WYP40505 wang.yaob@infypower.cn
 * @Date: 2026-04-16 09:03:06
 * @LastEditors: WYP40505 wang.yaob@infypower.cn
 * @LastEditTime: 2026-04-16 11:46:21
 * @FilePath: \IMIU4_Canfd_Uds_APP\Application\Addition\UDSCanfd\can_uds_interface.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

// can uds对外的接口实现
#include "can_uds_interface.h"
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/can_uds_services_layer.h"
#include "can_uds_arch.h"
#include "BSPCAN.h"
#include "Measure.h"
#include "SystemInfo.h"
#include "BSPTypeDef.h"
#include "sid_internal_header.h"
u16 CANfdudscount = 0;
extern unsigned char cur_session_type;
// --- 内部函数声明 ---
static void CANFDUDSCount(u16* count);
static void CANFDUDSCountCheck(void);
// --- 内部函数定义 --- 


// --- 外部函数定义 ---
void init_uds(void)
{
    
    g_uds_config.tx_id = LOCAL_DEVICE_CAN_ID |(LOAD_DEV_ID <<8) | (SystemInfo_1[DeviceAddr]);
    g_uds_config.tx_id_type = 1;//扩展帧
    g_uds_config.rx_phy_id = REMOTE_TESTER_CAN_ID |(LOAD_DEV_ID <<8) | (SystemInfo_1[DeviceAddr]);// 0b110514   0b110515
    g_uds_config.rx_phy_id_type = 1;//扩展帧
    g_uds_config.rx_fuc_id = REMOTE_TESTER_CAN_ID |(LOAD_DEV_ID <<8) | (SystemInfo_1[DeviceAddr]);
    g_uds_config.rx_fuc_id_type = 1;//扩展帧
    g_uds_config.stmin = 1;//设置stmin为50ms
    // g_uds_config.pack_with_crc = 1;//默认传输包带crc 31 36服务
	reset_uds_send_buf();
    // RefreshBootVersion();
}



void uds_proc_device(void)
{
    uds_phy_recv(0,FALSE);//仅运行流程
    uds_phy_send(FALSE);
    CANFDUDSCountCheck();
}

void uds_recv_data(struct CAN_DATA *p_can_data)
{
    uds_phy_recv(p_can_data,FALSE);
}


void uds_recv_data_fristhandle(t_CANMSG *msg)
{
    // /*test UDS
    static u8 update_first_id = 0;
    if (update_first_id == 0)
    {

        d_printf("update start 10");
        update_first_id = 1;
    }
    // test UDS*/


    struct CAN_DATA can_data;
    can_data.idt.id = msg->ID;
    can_data.idt.type = 1;//扩展帧
    can_data.dlc = msg->Len;
    CANFDUDSCount(&CANfdudscount);
    for(u8 i=0; i<can_data.dlc; i++ )
    {
        can_data.data[i] = msg->Data[i];
    }
    uds_recv_data(&can_data);
}

static void CANFDUDSCount(u16* count)
{
    *count = 15000;  //15s复位默认会话
}

static void CANFDUDSCountCheck(void)
{
    if(CANfdudscount >0)
    {
        CANfdudscount--;
    }
    if(CANfdudscount == 0)
    {
        cur_session_type = DEFAULT_SESSION;
    }

}