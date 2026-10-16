
/* ******************* 
 * can uds 36h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"
#include "BSPTypeDef.h"
#include "BSPIRQ.h"
#include "BSPKA84917_async.h"

unsigned long  WholePackCrc;
void sid36_transfer_data_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    //static unsigned char last_block_counter = 0;
    unsigned char rsp_msg[4] = {0};
    unsigned char is_crc_err = 0;
    unsigned long crc = 0,recv_crc = 0;
    
    if(msg_len < 2)//如果数据长度低于2，说明不是正常报文，不用解析
        return;
    rsp_msg[0] = ds_transfer_data | UDS_SRV_RET_ID_OFFSET; //0x76
    rsp_msg[1] = msg_buff[1];
    //cur_transfer_ctrl.last_block_counter = cur_transfer_ctrl.block_counter;//先记录上一次的
    cur_transfer_ctrl.block_counter = msg_buff[1];       //记录当前block

    if ((cur_transfer_ctrl.direction != TRANSFER_DOWNLOAD) && 
        (cur_transfer_ctrl.direction != TRANSFER_UPLOAD))
    {
        d_printf("sid36, refuse %x\r\n",nrc_request_sequence_error);
        uds_ack_refuse(ds_transfer_data, nrc_request_sequence_error);
    }
    else if ((unsigned char)(cur_transfer_ctrl.block_counter - cur_transfer_ctrl.last_block_counter) > 1)
    {
        d_printf("sid36, refuse %x,(%d,%d)\r\n",nrc_wrong_block_sequence_counter,
                                          cur_transfer_ctrl.block_counter,cur_transfer_ctrl.last_block_counter);
        uds_ack_refuse(ds_transfer_data, nrc_wrong_block_sequence_counter);
    }
    else
    {
        if(cur_transfer_ctrl.direction == TRANSFER_DOWNLOAD)
        {
             //如果是重复帧,正确回应
            if((cur_transfer_ctrl.last_block_counter == cur_transfer_ctrl.block_counter) && 
               !(cur_transfer_ctrl.is_first_block))
               {
                d_printf("sid36,last=(%d), now=(%d)\r\n",cur_transfer_ctrl.last_block_counter,cur_transfer_ctrl.block_counter);
            	
                uds_ack_accept(rsp_msg, 2);

                }
            else{
                d_printf("sid36, busy CRC CHECK\r\n");
                cur_transfer_ctrl.is_first_block = 0;//清除首包标志
                //uds_ack_refuse(ds_transfer_data, nrc_service_busy);
                if(g_uds_config.pack_with_crc){//如果crc附带功能打开，则先校验crc
                    msg_len -= 4;
                    crc = uds_crc(0xfffffffful,&msg_buff[2],msg_len-2);//由于最后四个字节是CRC校验值所以实际数据需要减去4字节
                    recv_crc  = msg_buff[msg_len+0];
                    recv_crc += (unsigned long)(msg_buff[msg_len+1]<<8);
                    recv_crc += (unsigned long)(msg_buff[msg_len+2]<<16);
                    recv_crc += (unsigned long)(msg_buff[msg_len+3]<<24);
                    if(crc != recv_crc){
                        is_crc_err = 1;
                        d_printf("crc err, addr[%04x], (%08x,%08x)\r\n",cur_transfer_ctrl.prog_addr,crc,recv_crc);
                        d_printf("crc err, %04x, %04x,%04x,%04x\r\n",msg_buff[2],msg_buff[msg_len+1],msg_buff[msg_len+2],msg_buff[msg_len+3]);
                    }else{
                        d_printf("crc correct, addr[%04x], (%08x)\r\n",cur_transfer_ctrl.prog_addr,crc);
                    }
                }
                if(is_crc_err)
                {//如果crc校验失败,返回单包校验错误使tester重发
                    uds_ack_refuse(ds_transfer_data, nrc_single_pack_crc_check_error);
                }
                else if(program_data_by_logic_addr(cur_transfer_ctrl.prog_addr,&msg_buff[2],(msg_len-2)) > 0){//编程完成
                    cur_transfer_ctrl.prog_addr += (msg_len-2);
                    cur_transfer_ctrl.last_block_counter = cur_transfer_ctrl.block_counter;//先记录上一次的
                    uds_wholepackcrc(&WholePackCrc,&msg_buff[2],msg_len-2);
                    uds_ack_accept(rsp_msg,2);
                    // d_printf("sid36, accpect, (%d,%d)\r\n",cur_transfer_ctrl.block_counter,cur_transfer_ctrl.last_block_counter);
                }else{                                                                               //编程失败
                    uds_ack_refuse(ds_transfer_data, nrc_general_programming_failure);
                }
            }
        }
    }

    if (irq_disabled) 
    {
       ENABLEIRQHANDLE(); 
    }
}
