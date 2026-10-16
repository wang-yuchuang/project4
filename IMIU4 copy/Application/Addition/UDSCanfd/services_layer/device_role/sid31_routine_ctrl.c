/* ******************* 
 * can uds 31h服务的实现
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "services_layer/device_role/can_uds_services_layer.h"
#include "can_uds_arch.h"

extern unsigned long  WholePackCrc; 

extern St_FirmwareCanUdsUpgradeConfig pgst_firwareUdsUpgrade;

enum
{
    UDS_ROUTINE_CTRL_START = 0x01,		   // 启动例程
    UDS_ROUTINE_CTRL_STOP = 0x02,		   // 停止例程
    UDS_ROUTINE_CTRL_REQUEST_RESULT = 0x03 // 结束例程后获取结果值
};

#define ROUTINE_ID_ERASE_MEM        0xFF00      //擦除flash(擦除目标存储的flash)
#define ROUTINE_ID_CHECK_MEM        0x0202      //启动整包校验
#define ROUTINE_ID_TRANS_CRC_SET    0x0A01      //传输是否带CRC校验设置


static void routine_erase_memory(unsigned char *msg_buff,unsigned char *rsp_buff);
static void routine_check_memory(unsigned char *msg_buff,unsigned char *rsp_buff);
static void routine_transfer_crc_set(unsigned char *msg_buff,unsigned char *rsp_buff);


void sid31_routine_ctrl_solve(unsigned char *msg_buff,unsigned short msg_len)
{
    unsigned char rsp_msg[8];
    unsigned char subfunction = 0;
    unsigned short routine_id = 0;
    //unsigned char mem_bytes_len = 0, addr_bytes_len = 0;

    subfunction = msg_buff[1]&0x7f;
    routine_id = msg_buff[2] << 8 | msg_buff[3];

    rsp_msg[0] = ds_routine_ctrl | UDS_SRV_RET_ID_OFFSET;
    rsp_msg[1] = msg_buff[1];
    rsp_msg[2] = msg_buff[2];
    rsp_msg[3] = msg_buff[3];
    rsp_msg[4] = 0;//这个目前无效

    switch(subfunction)
    {
    case UDS_ROUTINE_CTRL_START:
        switch(routine_id)
        {
        case ROUTINE_ID_ERASE_MEM:
            routine_erase_memory(&msg_buff[4],&rsp_msg[0]);
            break;
        case ROUTINE_ID_CHECK_MEM:
            routine_check_memory(&msg_buff[4],&rsp_msg[0]);
            break;
        case ROUTINE_ID_TRANS_CRC_SET:
            routine_transfer_crc_set(&msg_buff[4],&rsp_msg[0]);
            break;
        default:
            uds_ack_refuse(ds_routine_ctrl, nrc_request_out_of_range);
            break;
        }
        break;
    case UDS_ROUTINE_CTRL_STOP:
        switch(routine_id)
        {
        case ROUTINE_ID_ERASE_MEM:
        case ROUTINE_ID_CHECK_MEM:
        case ROUTINE_ID_TRANS_CRC_SET:
            uds_ack_accept(rsp_msg,5);//由于这几个控制功能没有持续执行的，所以停止命令就立即返回成功即可
            break;
        default:
            uds_ack_refuse(ds_routine_ctrl, nrc_request_out_of_range);
            break;
        }
        break;
    case UDS_ROUTINE_CTRL_REQUEST_RESULT:
        switch(routine_id)
        {
        case ROUTINE_ID_ERASE_MEM:
        case ROUTINE_ID_CHECK_MEM:
        case ROUTINE_ID_TRANS_CRC_SET:
            uds_ack_refuse(ds_routine_ctrl, nrc_request_sequence_error);//由于这几个控制功能没有并没有可以查询的结果，因此也是不支持的
            break;
        default:
            uds_ack_refuse(ds_routine_ctrl, nrc_request_out_of_range);
            break;
        }
        break;
    default:
        uds_ack_refuse(ds_routine_ctrl, nrc_subfunction_not_support);
        break;
    }
}


//------内部函数 

static void routine_erase_memory(unsigned char *msg_buff,unsigned char *rsp_buff)
{//msg_buff(0~3)(4~7)分别是擦除起始地址和擦除长度，这里仅作升级用，不用这两个字段，擦除功能即表示擦除目标暂存区域
    if(uds_erase_app_data() == 0){
        uds_ack_refuse(ds_routine_ctrl, nrc_general_programming_failure);//擦除失败
    }else{
        uds_ack_accept(rsp_buff,5);//擦除成功，回复之前已填好的数据
    }
}

static void routine_check_memory(unsigned char *msg_buff,unsigned char *rsp_buff)
{//进行全包crc处理
    unsigned long crc32_temp;
    crc32_temp = msg_buff[3] << 24 | msg_buff[2] << 16 | msg_buff[1] << 8 | msg_buff[0];
	if (crc32_temp == WholePackCrc)
	{
		uds_ack_accept(rsp_buff,5);//校验成功，回复之前已填好的数据
	}
	else
	{
        uds_ack_refuse(ds_routine_ctrl, nrc_whole_pack_crc_check_error);//擦除失败
	}
    //全包校验，待填充
    //uds_program_finish();//写入升级标志
  
}

static void routine_transfer_crc_set(unsigned char *msg_buff,unsigned char *rsp_buff)
{
    if(msg_buff[0]){//非0，表示使能子包crc
        g_uds_config.pack_with_crc = 1;
    }else{
        g_uds_config.pack_with_crc = 0;
    }
    uds_ack_accept(rsp_buff,5);//回复之前已填好的数据
}