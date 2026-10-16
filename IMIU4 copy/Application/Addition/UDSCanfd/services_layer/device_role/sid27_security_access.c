/* ******************* 
 * can uds 27h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"

//--- 内部结构体定义 ---


//--- 内部变量定义 ---
struct SECURITY_CTRL security_ctrl = {
    .level = SECURITY_LEVEL0,
    .seed[0] = 0,
    .seed[1] = 0,
    .seed[2] = 0,
    .seed[3] = 0,
};


void sid27_security_access_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    unsigned char rsp_msg[8] = {0};
    unsigned long tmp_seed = 0,tmp_key = 0;

    if(cur_session_type == EXTENDED_SESSION ||
       cur_session_type == PROGRAMMING_SESSION){
        if(((msg_buff[1]&0x7f) == 0x01) ||
            ((msg_buff[1]&0x7f) == 0x09)){//请求种子
            tmp_seed = uds_get_seed();    //获取种子(4字节宽度)，获取方法由用户定义
            rsp_msg[0] = ds_security_access | UDS_SRV_RET_ID_OFFSET;
            rsp_msg[1] = msg_buff[1];
            rsp_msg[2] = security_ctrl.seed[0] = tmp_seed&0xff;
            rsp_msg[3] = security_ctrl.seed[1] = (tmp_seed>>8)&0xff;
            rsp_msg[4] = security_ctrl.seed[2] = (tmp_seed>>16)&0xff;
            rsp_msg[5] = security_ctrl.seed[3] = tmp_seed>>24;
            uds_ack_accept(rsp_msg,6);
            d_printf("sid27, security:%d\r\n",msg_buff[1]);
        }else if(((msg_buff[1]&0x7f) == 0x02) ||
                 ((msg_buff[1]&0x7f) == 0x0A)){//请求计算KEY
            tmp_key =  msg_buff[2];         //存储对方发来的KEY
            tmp_key += msg_buff[3]<<8;
            tmp_key += msg_buff[4]<<16;
            tmp_key += msg_buff[5]<<24;   
            tmp_seed =  security_ctrl.seed[0];       //自身创建的种子
            tmp_seed += security_ctrl.seed[1]<<8;
            tmp_seed += security_ctrl.seed[2]<<16;
            tmp_seed += security_ctrl.seed[3]<<24;  
            if(uds_seed_to_key(tmp_seed) == tmp_key){//通过解密算法判断种子计算的结果是否跟
                security_ctrl.level = SECURITY_LEVEL1;//切换安全等级
                rsp_msg[0] = ds_security_access | UDS_SRV_RET_ID_OFFSET;
                rsp_msg[1] = msg_buff[1];
                rsp_msg[2] = security_ctrl.seed[0];
                rsp_msg[3] = security_ctrl.seed[1];
                rsp_msg[4] = security_ctrl.seed[2];
                rsp_msg[5] = security_ctrl.seed[3];
                uds_ack_accept(rsp_msg,6);
                d_printf("sid27, security:%d\r\n",msg_buff[1]);
            }else{
                uds_ack_refuse(ds_security_access,nrc_invalid_key);//key error
            }
        }else{//
            uds_ack_refuse(ds_security_access,nrc_service_not_support_in_active_session);// style error
        }
    }else{//
        uds_ack_refuse(ds_security_access,nrc_service_not_support_in_active_session);// style error
    }
}