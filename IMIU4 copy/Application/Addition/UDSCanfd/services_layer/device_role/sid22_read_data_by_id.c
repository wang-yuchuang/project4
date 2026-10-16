/* ******************* 
 * can uds 22h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"

unsigned char crc16_save[4] = {'0','0','0','0'};

void sid22_read_data_by_id_solve(unsigned char *msg_buff,unsigned short msg_len)
{//
    unsigned short read_did = 0;
    unsigned char rsp_msg[3+MAX_DID_CHARACTORS_NUM] = {0};//多三个字节，SID(1字节)+DID(2字节)

    if(cur_session_type == DEFAULT_SESSION ||
        cur_session_type == PROGRAMMING_SESSION ||
        cur_session_type == EXTENDED_SESSION){
        read_did = msg_buff[1];  //LSB
        read_did += (unsigned short)(msg_buff[2] << 8);
        rsp_msg[0] = ds_read_data_by_id | UDS_SRV_RET_ID_OFFSET;
        rsp_msg[1] = read_did&0x00ff;       //LSB
        rsp_msg[2] = (read_did&0xff00) >> 8;//MSB
        switch(read_did)
        {// para 3 is len, str is id+content
        case app_sw_id:
            memcpy(&rsp_msg[3],app_sw_version_str,sizeof(app_sw_version_str)-1);
            uds_ack_accept(rsp_msg,3 + sizeof(app_sw_version_str) - 1);
            d_printf("sid22, ack app_sw_id\r\n");
            break;
        case app_crc_value:
            memcpy(&rsp_msg[3],crc16_save,4);
            uds_ack_accept(rsp_msg,3 + 4);
            d_printf("sid22, ack app_crc_value\r\n");
            break;
        default:
            uds_ack_refuse(ds_read_data_by_id,nrc_request_out_of_range);
            break;
        }
    }else{
        uds_ack_refuse(ds_read_data_by_id,nrc_request_out_of_range);
    }
}