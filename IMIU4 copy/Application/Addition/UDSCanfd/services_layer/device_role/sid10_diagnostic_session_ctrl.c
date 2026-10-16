/* ******************* 
 * can uds 10h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"
#include "sid_internal_header.h"

void sid10_diagnostic_session_ctrl_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    unsigned char rsp_msg[6] = {0};

    rsp_msg[0] = ds_diagnostic_session_ctrl | UDS_SRV_RET_ID_OFFSET;
    rsp_msg[1] = msg_buff[1];
    rsp_msg[2] = 0x00;
    rsp_msg[3] = 0x32;
    rsp_msg[4] = 0x01;
    rsp_msg[5] = 0xf4;

    switch(cur_session_type)
    {
    case DEFAULT_SESSION:
    case EXTENDED_SESSION:
    case PROGRAMMING_SESSION:
        if((msg_buff[1]&0x7f) == EXTENDED_SESSION){//要求切换到扩展会话
            cur_session_type = EXTENDED_SESSION;
            uds_ack_accept(rsp_msg,6);
        }else if((msg_buff[1]&0x7f) == PROGRAMMING_SESSION){//要求切换到编程会话
            cur_session_type = PROGRAMMING_SESSION;
            uds_ack_accept(rsp_msg,6);
        }else if((msg_buff[1]&0x7f) == DEFAULT_SESSION){//要求切换到默认会话
            cur_session_type = DEFAULT_SESSION;
            uds_ack_accept(rsp_msg,6);
        }
        break;
    #if 0
    case EXTENDED_SESSION:
        if((msg_buff[1]&0x7f) == DEFAULT_SESSION){//要求切换到扩展会话
            cur_session_type = DEFAULT_SESSION;
            uds_ack_accept(rsp_msg,6);
        }else if((msg_buff[1]&0x7f) == PROGRAMMING_SESSION){//要求切换到编程会话
            cur_session_type = PROGRAMMING_SESSION;
            uds_ack_accept(rsp_msg,6);
        }else if((msg_buff[1]&0x7f) == EXTENDED_SESSION){//要求切换到默认会话
            cur_session_type = EXTENDED_SESSION;
            uds_ack_accept(rsp_msg,6);
        }
        break;
    case PROGRAMMING_SESSION:
        if((msg_buff[1]&0x7f) == DEFAULT_SESSION){//要求切换到扩展会话
            cur_session_type = DEFAULT_SESSION;
            uds_ack_accept(rsp_msg,6);
        }else if((msg_buff[1]&0x7f) == PROGRAMMING_SESSION){//要求切换到编程会话
            cur_session_type = PROGRAMMING_SESSION;
            uds_ack_accept(rsp_msg,6);
        else if((msg_buff[1]&0x7f) == EXTENDED_SESSION){//要求切换到默认会话
            cur_session_type = EXTENDED_SESSION;
            uds_ack_accept(rsp_msg,6);
        }
        break;
    #endif
    }
}