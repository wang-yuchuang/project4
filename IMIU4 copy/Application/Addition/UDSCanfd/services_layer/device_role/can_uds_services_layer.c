/* ******************* 
 * can uds
 **********************/
#include <string.h>
#include "network_layer/can_uds_network_layer.h"
#include "services_layer/device_role/can_uds_services_layer.h"
#include "services_layer/device_role/sid_internal_header.h"


enum{
	e2rom_app_sw_id_str = 0,
	e2rom_active_diag_session_id_str,
	e2rom_vehicle_spare_num_id_str,
	e2rom_vehicle_ecu_num_id_str,
	e2rom_vehicle_ecu_sw_ver_id_str,
	e2rom_sys_supplier_id_str,
	e2rom_ecu_manufacturing_id_str,
	e2rom_ecu_serial_num_id_str,
	e2rom_vin_id_str,
	e2rom_vehicle_ecu_hw_num_id_str,
	e2rom_sys_supplier_ecu_hw_id_str,
	e2rom_ecu_hw_ver_str,
	e2rom_sys_supplier_ecu_sw_num_id_str,
	e2rom_sys_supplier_ecu_sw_ver_id_str,
	e2rom_sys_name_or_engine_type_id_str,
	e2rom_ecu_install_date_id_str
};

#define RECV_DATA_COMMING_OVERTIME      500





// --- 
static unsigned char phy_func_mode = 0;//0 is phy mode, 1 is func mode
unsigned char cur_session_type = DEFAULT_SESSION;
static struct UDS_SEND_BUF uds_send_buf;
static struct SF_PACK single_pack_send_buf;
static struct MF_PACK multi_pack_send_buf;
struct SF_PACK single_pack_recv_buf;
struct MF_PACK multi_pack_recv_buf;
static struct FC_STATUS cur_fc_sta;
unsigned char cur_communication_type = COMMU_EN_RX_TX;
unsigned char dtc_update_flg = 0;
unsigned char uds_busy_flag = FALSE;
unsigned short dtc_count = 0;
unsigned long pg_memory_address = 0,pg_memory_size = 0;

void uds_start_send(void);
#if 0 //以下四个函数不区分单帧还是多帧，全部统一处理
void uds_solve_sf_data(struct SF_PACK *sf_pack);
void uds_solve_sf_error(struct SF_PACK *sf_pack,unsigned char err_type);
void uds_solve_mf_data(struct MF_PACK *mf_pack);
void uds_solve_mf_error(struct MF_PACK *mf_pack,unsigned char err_type);
#else
void uds_solve_recv_data(unsigned char *buff, unsigned short len);
void uds_solve_recv_error(unsigned char *buff, unsigned char err_type);
#endif
static void cpy_send_to_sfpack(void);
static void cpy_send_to_mfpack(void);



// --- 重置发送缓冲区 ---
void reset_uds_send_buf(void)
{
    uds_send_buf.sw = FALSE;
    memset(uds_send_buf.buf,0,MAX_SEND_BUF_LEN);
    uds_send_buf.len = 0;
}

void cpy_send_to_sfpack(void)
{
	unsigned short i = 0;
    unsigned short max_len = 0;

    if(uds_is_canfd()){
        max_len = 62;
    }else{
        max_len = 8;
    }

	if(uds_send_buf.len > max_len)
		single_pack_send_buf.data_len = max_len;
	else
		single_pack_send_buf.data_len = uds_send_buf.len;
	for(i = 0; i <single_pack_send_buf.data_len; i++)
	{
		single_pack_send_buf.data_buf[i] = uds_send_buf.buf[i];
	}
}

void cpy_send_to_mfpack(void)
{
	unsigned short i = 0;

	if(uds_send_buf.len > MAX_MF_PACK_LEN)
		multi_pack_send_buf.data_len = MAX_MF_PACK_LEN;
	else
		multi_pack_send_buf.data_len = uds_send_buf.len;
	for(i = 0; i <multi_pack_send_buf.data_len; i++)
	{
		multi_pack_send_buf.data_buf[i] = uds_send_buf.buf[i];
	}
}

void uds_start_send(void)
{
	uds_send_buf.sw = TRUE;
}

void uds_solve_recv_data(unsigned char *buff, unsigned short len)
{
    switch(buff[0])
    {// pack 0 is diagnostic service 's id
    case ds_diagnostic_session_ctrl://支持,SID10
        sid10_diagnostic_session_ctrl_solve(&buff[0],len);
        break;
    case ds_security_access://支持,SID27
        sid27_security_access_solve(&buff[0],len);
        break;
    case ds_tester_present://支持,SID3E
        sid3e_tester_present_solve(&buff[0],len);
        break;
    case ds_ecu_reset://支持,SID11
        sid11_ecu_reset_solve(&buff[0],len);
        break;
    case ds_communication_ctrl://支持,SID28
        sid28_communication_ctrl_solve(&buff[0],len);
        break;
    case ds_ctrl_dtc_setting://不支持,SID85
        uds_ack_refuse(ds_write_data_by_id,nrc_service_not_supported);
		break;
    case ds_read_data_by_id://支持,SID22
        sid22_read_data_by_id_solve(&buff[0],len);
        break;
    case ds_write_data_by_id://支持,SID2E
        sid2e_write_data_by_id_solve(&buff[0],len);
        break;
    case ds_read_dtc_info://不支持,SID14
        uds_ack_refuse(ds_write_data_by_id,nrc_service_not_supported);
        break;
    case ds_clr_diagnostic_info://不支持,SID14
        uds_ack_refuse(ds_write_data_by_id,nrc_service_not_supported);
        break;
    case ds_io_ctrl_by_id://不支持,SID2F
        uds_ack_refuse(ds_write_data_by_id,nrc_service_not_supported);
        break;
    case ds_routine_ctrl://支持,SID31
        sid31_routine_ctrl_solve(&buff[0],len);
        break;
    case ds_request_download://支持,SID34
        sid34_request_download_solve(&buff[0],len);
        break;
    case ds_request_upload://不支持,SID35
        uds_ack_refuse(ds_write_data_by_id,nrc_service_not_supported);
        break;
    case ds_transfer_data://支持,SID36
        sid36_transfer_data_solve(&buff[0],len);
        break;
    case ds_request_transferexit://支持,SID37
        sid37_transfer_exit_solve(&buff[0],len);
        break;
	}
}

void uds_solve_recv_error(unsigned char *buff, unsigned char err_type)
{
    if(err_type == ERR_SF_DL_WRR || 
       err_type == ERR_SF_DAT_WRR){// send len or data's style wrong  0x13
        uds_ack_refuse(buff[0],nrc_invalid_message_lenth_or_format);
    }
}

void uds_ack_accept(unsigned char *msg_buff,unsigned short len)
{
    uds_send_buf.len = len;
    for(int i=0;i<len;i++){
        uds_send_buf.buf[i] = msg_buff[i];
    }
    uds_start_send();
}


void uds_ack_refuse(unsigned char ds_type,unsigned char para)
{
    if(phy_func_mode == 1)//only phy mode solve error, when func do not send nay error
        return;
    // pack 0 is diagnostic service 's id
    uds_send_buf.len  = 3;
    uds_send_buf.buf[0] = 0x7f;
    uds_send_buf.buf[1] = ds_type;
    uds_send_buf.buf[2] = para;
    uds_start_send();
}

//接收CAN报文，处理单帧、首帧、连续帧
void uds_phy_recv(struct CAN_DATA *p_can_data,unsigned char reset)
{
    static unsigned char sta = 0;
    static unsigned long tick = 0;
    unsigned char frame_type = 0;
    unsigned char tmp_re = 0;

    if(reset == TRUE)
    {
        sta = 0;
        tick = 0;
        uds_busy_flag = FALSE;
    }
    switch(sta)
    {
    case 0:
        if(p_can_data == 0)//如果无数据输入,不做处理
            break;
        frame_type = check_uds_type(p_can_data);

        if(frame_type == SF_UDS_TYPE){// 单帧
            tmp_re = analyze_uds_SF(p_can_data, &single_pack_recv_buf);
            if(tmp_re == 0xff)
                uds_solve_recv_error(single_pack_recv_buf.data_buf,ERR_SF_DL_WRR);
            else
                uds_solve_recv_data(single_pack_recv_buf.data_buf,single_pack_recv_buf.data_len);
                 d_printf("SF_UDS_TYPE: %02x\r\n",single_pack_recv_buf.data_buf[0]);
            uds_busy_flag = FALSE;
        }
        else if(frame_type == FF_UDS_TYPE)// 首帧
        {
            uds_busy_flag = TRUE;
            tmp_re = analyze_uds_FF(p_can_data,&multi_pack_recv_buf);// 解析首帧，获取总数据长度
            //send FC
            send_uds_FC(FC_FS_CTS,0,g_uds_config.stmin);// continue send , there are no cf,STmin=50ms 发送流控帧
            tick = get_timer_tick();

            d_printf("over FC_FS_CTS tick: %02x\r\n",tick);
            sta = 1; // 进入多帧接收状态
        }
        else if(frame_type == CF_UDS_TYPE){// error
            uds_busy_flag = FALSE;
        }
        else if(frame_type == FC_UDS_TYPE){// if receive FC, update fc status
            analyze_uds_FC(p_can_data, &cur_fc_sta);
        }
        break;

    case 1://多帧接收
        //超时检查
        if((unsigned long)(get_timer_tick() - tick) > RECV_DATA_COMMING_OVERTIME){// if over 500ms,there is no data comming
            sta = 0;
            uds_busy_flag = FALSE;// 
            tick = get_timer_tick();
            d_printf("over 500ms tick: %02x\r\n",tick);
        }
        else
        {
            if(p_can_data == 0)//如果无数据输入,不做处理
                break;
            frame_type = check_uds_type(p_can_data);

            if(frame_type == SF_UDS_TYPE){// if it is single frame ,solve it ,  but this maybe some confuse ?????
                tmp_re = analyze_uds_SF(p_can_data, &single_pack_recv_buf);
                if(tmp_re == 0xff)
                    //uds_solve_sf_error(&single_pack_recv_buf,ERR_SF_DL_WRR);
                    uds_solve_recv_error(single_pack_recv_buf.data_buf,ERR_SF_DL_WRR);
                else
                    //uds_solve_sf_data(&single_pack_recv_buf);// solve data
                    uds_solve_recv_data(single_pack_recv_buf.data_buf,single_pack_recv_buf.data_len);
                     d_printf("SF_UDS_TYPE: %02x\r\n",single_pack_recv_buf.data_buf[0]);
            }
            else if(frame_type == CF_UDS_TYPE)// 收到连续帧
            {
                tmp_re = analyze_uds_CF(p_can_data,&multi_pack_recv_buf);// check  SN 
                if(tmp_re == ERR_SN_WRONG){// SN wrong 序列号错误
                    sta = 0;
                    uds_busy_flag = FALSE;
                    //### solve sn wrong###
                    //### need add code ####
                    // N_WRONG_SN N_UDSData.indication
                    //uds_solve_mf_error(&multi_pack_recv_buf,ERR_SN_WRONG);
                    uds_solve_recv_error(multi_pack_recv_buf.data_buf,ERR_SN_WRONG);
                }
                else if(tmp_re == ERR_RECV_END)
                {// all data has been received
                    //uds_solve_mf_data(&multi_pack_recv_buf);
                    // d_printf("case12:slove CF data: %02x\r\n",multi_pack_recv_buf.data_buf[0]);
                   
                    d_printf("case12:slove CF data: %02x\r\n",multi_pack_recv_buf.data_buf[0]);
                    uds_solve_recv_data(multi_pack_recv_buf.data_buf,multi_pack_recv_buf.data_len);// 处理完整数据
                    sta = 0;
                    uds_busy_flag = FALSE;
                }
                else{// SN is right
                    tick = get_timer_tick();// update timer
                }
            }
        }
        break;
    }
}

void uds_phy_send(unsigned char reset)
{
    static unsigned char sta = 0; // 发送状态：0=空闲,1=等待FC,2=准备发送CF,3=发送CF中
    static unsigned long tick = 0; 
    static unsigned long delay_tick = 0;
    unsigned short max_len = 0;
    if(reset == TRUE){
        sta = 0;
        tick = 0;
        reset_uds_send_buf();
    }
    if(uds_is_canfd()){//单帧有效数据字节为(64-2=62)
        max_len = 62;
    }else{
        max_len = 7;
    }

    switch(sta)
    {
    case 0:// 空闲状态，准备发送
        if(uds_send_buf.sw == TRUE){// only if uds_send_buf have data and open , just send 
            if(uds_send_buf.len <= max_len){// send single buff
                cpy_send_to_sfpack();// copy send_buf to sf_pack
                send_uds_SF(&single_pack_send_buf);
                reset_uds_send_buf();// if send over , reset send_buf
            }else{// multi buff
                cpy_send_to_mfpack();// copy send_buf to mf_pack
                send_uds_FF(&multi_pack_send_buf);
                if(uds_is_canfd()){
                    multi_pack_send_buf.cur_recv_idx = 58;
                }else{
                    multi_pack_send_buf.cur_recv_idx = 6;
                }
                tick = get_timer_tick();
                sta = 1;
                reset_fc_sta(&cur_fc_sta);//reset fc sta
            }
        }
        break;
    case 1:// wait for FC流控帧
        if((unsigned long)(get_timer_tick() - tick) > DEFAULT_STMIN_TIME){// if over 200ms, the FC is no used,standard is 150ms
            sta = 0;
            reset_uds_send_buf();
        }else if(cur_fc_sta.fs != FC_FS_UNUSED){// there is a new FC comming
            if(cur_fc_sta.fs == FC_FS_CTS){// continue
                sta = 2;
                if(cur_fc_sta.stmin <= 0x7f){
                    tick = get_timer_tick();
                    delay_tick = (cur_fc_sta.stmin>0)?cur_fc_sta.stmin:1;//直接指定STmin至少为1ms
                }else if(cur_fc_sta.stmin >= 0xf1 && cur_fc_sta.stmin <= 0xf9){
                    tick= get_timer_tick();
                    delay_tick = 1;//目前不支持微秒的延时，STmin指定为微秒级时，默认为1ms
                }else{
                    tick= get_timer_tick();
                    delay_tick = DEFAULT_STMIN_TIME;// 默认STmin指定由宏定义
                }
            }else if(cur_fc_sta.fs == FC_FS_WT){// wait next fc
                tick = get_timer_tick();//update timer
            }
            //else fs == FC_FS_OVFLW do not solve
        }
        break;
    case 2://等待发送第一个连续帧
        if(uds_send_buf.sw == TRUE){// only if uds_send_buf have data and open , just send 
            if(uds_send_buf.len <= max_len){// send single buff
                cpy_send_to_sfpack();// copy send_buf to sf_pack
                send_uds_SF(&single_pack_send_buf);
                reset_uds_send_buf();// if send over , reset send_buf
            }
        }
        if((unsigned long)(get_timer_tick() - tick) > delay_tick){// 
            sta = 3;
            send_uds_CF(&multi_pack_send_buf);
            multi_pack_send_buf.cur_sn++;// this time , cur_sn is not 0x0f
            multi_pack_send_buf.cur_recv_idx += 63;
            tick = get_timer_tick();// reset tick
        }
        break;
    case 3:// 发送连续帧中
        if(uds_send_buf.sw == TRUE){// only if uds_send_buf have data and open , just send 
            if(uds_send_buf.len <= max_len){// send single buff
                cpy_send_to_sfpack();// copy send_buf to sf_pack
                send_uds_SF(&single_pack_send_buf);
                reset_uds_send_buf();// if send over , reset send_buf
            }
        }
        if(multi_pack_send_buf.cur_recv_idx >= multi_pack_send_buf.data_len){// if all data has been sended,change sta
            sta = 0;
            reset_uds_send_buf();
        }else if((unsigned long)(get_timer_tick() - tick)> delay_tick){// 
            send_uds_CF(&multi_pack_send_buf);
            if(multi_pack_send_buf.cur_sn >= 0x0f)
                multi_pack_send_buf.cur_sn = 0;
            else
                multi_pack_send_buf.cur_sn++;
            multi_pack_send_buf.cur_recv_idx += 63;
            tick = get_timer_tick();// update timer
        }
        break;
    }
}
