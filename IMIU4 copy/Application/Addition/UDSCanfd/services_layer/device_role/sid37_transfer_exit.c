/* ******************* 
 * can uds 37h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"

void sid37_transfer_exit_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    unsigned char rsp_msg[2] = {0};

    rsp_msg[0] = ds_request_transferexit | UDS_SRV_RET_ID_OFFSET;

    cur_transfer_ctrl.block_counter = rsp_msg[1];

    if ((cur_transfer_ctrl.direction != TRANSFER_DOWNLOAD) && 
        (cur_transfer_ctrl.direction != TRANSFER_UPLOAD)){
        uds_ack_refuse(ds_request_transferexit, nrc_request_sequence_error);
    }else if(cur_transfer_ctrl.block_counter == (cur_transfer_ctrl.last_block_counter+1)){//如果block_counter为最后一包，给出正确回应
        uds_ack_accept(rsp_msg,1);
    }else {//否则，说明block_counter的序号是错的，给出否定响应
        uds_ack_refuse(ds_request_transferexit, nrc_wrong_block_sequence_counter);
    }
}