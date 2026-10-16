
/* ******************* 
 * can uds 11h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"
#include "cmsis_os.h"
#include "BSPCAN.h"

void sid11_ecu_reset_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    //只能直接调用BSP发送
    unsigned char i;
    t_CANMSG txMsg;
    txMsg.ID = g_uds_config.tx_id;
    txMsg.Len = 64;
    txMsg.Data[0] = 0;
    txMsg.Data[1] = 2;
    txMsg.Data[2] = ds_ecu_reset | UDS_SRV_RET_ID_OFFSET;
    txMsg.Data[3] = msg_buff[1];
    for(i = 4;i<64;i++)
    {
        txMsg.Data[i] = 0x55;
    }

    if((msg_buff[1]&0x7f) == 0x01){// 硬件复位
        cur_session_type = DEFAULT_SESSION;
        //uds_ack_accept(rsp_msg,2);
        //osDelay(1000);
        BSPCANSendMsg((e_CANNUM)eCAN1, &txMsg);
        uds_reset();
    }else if((msg_buff[1]&0x7f) == 0x02){// 软件复位
        cur_session_type = DEFAULT_SESSION;
        BSPCANSendMsg((e_CANNUM)eCAN1, &txMsg);
        uds_reset();
    }else{
        uds_ack_refuse(ds_ecu_reset,nrc_invalid_key);//key error
    }
}