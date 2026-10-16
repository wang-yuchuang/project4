
/* ******************* 
 * can uds 34h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"
extern unsigned long  WholePackCrc;
/// @brief 
struct TRANSFER_CTRL cur_transfer_ctrl = {
    .direction = TRANSFER_DOWNLOAD,
    .block_counter = 0,
    .last_block_counter = 0,
    .is_first_block = 1,
    .prog_addr = 0,
};


/// @brief SID34 请求下载
/// @param msg_buff 
/// @param msg_len 
void sid34_request_download_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    unsigned char rsp_msg[4];
    unsigned char mem_bytes_len = 0, addr_bytes_len = 0;
    pg_memory_address = 0;
    pg_memory_size = 0;

    rsp_msg[0] = ds_request_download | UDS_SRV_RET_ID_OFFSET;
    rsp_msg[1] = 0x20;
    rsp_msg[2] = (unsigned char)(MAX_NUMBERS_OF_BLOCK>>8);//MSB在前，大头序
    rsp_msg[3] = (unsigned char)(MAX_NUMBERS_OF_BLOCK);
    mem_bytes_len = (unsigned char)(msg_buff[2] >> 4);    //获取最大
    addr_bytes_len = (unsigned char)(msg_buff[2] & 0x0f);

    for(int i = 0; i < addr_bytes_len; i++){
        pg_memory_address |= ((unsigned long)msg_buff[3 + i] << (i * 8));
    }
    for(int i = 0; i < mem_bytes_len; i++){
        pg_memory_size |= ((unsigned long)msg_buff[3 + addr_bytes_len + i] << (i * 8));
    }

    if ((addr_bytes_len > 4) || (mem_bytes_len > 4)){ //memory宽度和地址宽度不能超过4字节
        uds_ack_refuse(ds_request_download, nrc_request_out_of_range);
    }else{
        d_printf("sid34, (%d,%d)\r\n",mem_bytes_len,addr_bytes_len);
        uds_ack_accept(rsp_msg,4);
        cur_transfer_ctrl.is_first_block = 1;
        cur_transfer_ctrl.last_block_counter = 0;
        cur_transfer_ctrl.block_counter = 0;
        cur_transfer_ctrl.prog_addr = 0;
        uds_crc_init(&WholePackCrc);

        d_printf("sid34, (%d)\r\n",cur_transfer_ctrl.is_first_block);
    }
}
