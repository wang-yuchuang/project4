/* ******************* 
 * can uds 28h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"


void sid28_communication_ctrl_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    unsigned char rsp_msg[2] = {0};

    rsp_msg[0] = ds_communication_ctrl | UDS_SRV_RET_ID_OFFSET;
    rsp_msg[1] = msg_buff[1];

    if(cur_session_type == PROGRAMMING_SESSION||
        cur_session_type == EXTENDED_SESSION){
        if((msg_buff[1]&0x7f) == COMMU_EN_RX_TX){//
            if((msg_buff[2] == 0x01) || (msg_buff[2] == 0x03))// only comm type is 01 or 03
                cur_communication_type = COMMU_EN_RX_TX;
            uds_ack_accept(rsp_msg,2);
        }else if((msg_buff[1]&0x7f) == COMMU_EN_RX_DIS_TX){
            if((msg_buff[2] == 0x01) || (msg_buff[2] == 0x03))
                cur_communication_type = COMMU_EN_RX_DIS_TX;
            uds_ack_accept(rsp_msg,2);
        }else if((msg_buff[1]&0x7f) == COMMU_DIS_RX_EN_TX){
            if((msg_buff[2] == 0x01) || (msg_buff[2] == 0x03))// only comm type is 01 or 03
                cur_communication_type = COMMU_DIS_RX_EN_TX;
            uds_ack_accept(rsp_msg,2);
        }else if((msg_buff[1]&0x7f) == COMMU_DIS_RX_TX){
            if((msg_buff[2] == 0x01) || (msg_buff[2] == 0x03))// only comm type is 01 or 03
                cur_communication_type = COMMU_DIS_RX_TX;
            uds_ack_accept(rsp_msg,2);
        }else{// unaccepct type
            uds_ack_refuse(ds_communication_ctrl,nrc_subfunction_not_support);
        }
    }else{
        uds_ack_refuse(ds_communication_ctrl,nrc_subfunction_not_support);
    }
}