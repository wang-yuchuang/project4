/* ******************* 
 * can uds 3eh服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"


void sid3e_tester_present_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    unsigned char rsp_msg[2] = {0};

    rsp_msg[0] = ds_tester_present | UDS_SRV_RET_ID_OFFSET;
    rsp_msg[1] = 0;
    uds_ack_accept(rsp_msg,2);
}