/* ******************* 
// can uds的网络层接口
 **********************/
#include "network_layer/can_uds_network_layer.h"
#include "can_uds_arch.h"
#include "sid_internal_header.h"

enum{MF_STA_EMP = 0,MF_STA_REVING,MF_STA_FINISH};                           //多帧收发的状态枚举


unsigned char reset_flag = 0;
unsigned char dtc_status_mask = 0;



struct UDS_DEV_CONFIG g_uds_config;
unsigned char uds_send_flg = TRUE;



//
unsigned char send_uds_SF(struct SF_PACK *sf_pack)
{
    int ret = 0;
    unsigned char SF_DL = 0,i = 0;
    unsigned char databuf[64] = {0};
    unsigned char byte = 0,byte1 = 0;
    unsigned char sdl_max = 0;

    if(uds_is_canfd()){
        sdl_max = 62;
        memset(databuf,0x55,64);
    }else{
        sdl_max = 7;
        memset(databuf,0x55,8);
    }
        
    if(sf_pack->data_len > sdl_max)
        SF_DL = sdl_max;
    else
        SF_DL = sf_pack->data_len;

    for(i = 0; i < SF_DL;i++)
    {
        databuf[i] = sf_pack->data_buf[i];
    }
    
    uds_send_flg = TRUE;
    if(uds_is_canfd()){
        byte = 0x00;
        byte1 = SF_DL;
        ret = uds_send_data64(g_uds_config.tx_id,g_uds_config.tx_id_type,
                              byte,byte1,databuf[0],databuf[1],
                              databuf[2],databuf[3],databuf[4],databuf[5],
                              &databuf[6]);
    }else{
        byte = byte | (SF_DL&0x0f);
        ret = uds_send_data8(g_uds_config.tx_id,g_uds_config.tx_id_type,
                            byte,databuf[0],databuf[1],databuf[2],
                            databuf[3],databuf[4],databuf[5],databuf[6]);
    }
    
    return ((ret>0)?1:0);
}

unsigned char send_uds_FF(struct MF_PACK *mf_pack)//FF has total len
{
    int ret = 0;
    unsigned short FF_DL = 0, i = 0;
    unsigned char databuf[64] = {0};
    unsigned char byte0 = 0,byte1 = 0, byte2 = 0, byte3 = 0, byte4 = 0, byte5 = 0;
    unsigned char ff_left_bytes = 0;

    if(uds_is_canfd()){
        ff_left_bytes = 58;
    }else{
        ff_left_bytes = 6;
    }

    if(mf_pack->data_len > MAX_MF_PACK_LEN)
        return -1;
    else
        FF_DL = mf_pack->data_len;

    for(i = 0;i < ff_left_bytes;i++){// FF only have 6/59 bytes
        databuf[i] = mf_pack->data_buf[i];
    }
    
    mf_pack->cur_sn = 1;    //FF首帧，将SN状态清除，置位下一次CF为1
    uds_send_flg = TRUE;
    if(uds_is_canfd()){
        byte0 = 0x10 | (0x00);
        byte4 = ((FF_DL & 0x0f00)>>8);
        byte5 = (FF_DL & 0xff);
        ret = uds_send_data64(g_uds_config.tx_id,g_uds_config.tx_id_type,
                              byte0,byte1,byte2,byte3,
                              byte4,byte5,databuf[0],databuf[1],
                              &databuf[2]);
    }else{
        byte0 = 0x10 | ((FF_DL & 0x0f00)>>8);
        byte1 = (FF_DL & 0xff);
        ret = uds_send_data8(g_uds_config.tx_id,g_uds_config.tx_id_type,
                         byte0,byte1,databuf[0],databuf[1],
                         databuf[2],databuf[3],databuf[4],databuf[5]);
    }
    
    return ((ret>0)?1:0);
}

unsigned char send_uds_CF(struct MF_PACK *mf_pack)//CF do not have len each frame
{//对于是否CANFD，连续帧格式都是不变的，只是数据域扩展不同
    int ret = 0;
    unsigned i = 0;
    int tmp_len = 0;
    unsigned char databuf[64] = {0},byte = 0;

    if(uds_is_canfd()){
        memset(databuf,0x55,64);
    }else{
        memset(databuf,0x55,8);
    }

    if(mf_pack->cur_recv_idx < mf_pack->data_len)
        tmp_len = mf_pack->data_len - mf_pack->cur_recv_idx;
    if(uds_is_canfd()){
        if(tmp_len > 63)
            tmp_len = 63;
    }else{
        if(tmp_len > 7)
            tmp_len = 7;
    }
    

    for(i = 0; i < tmp_len;i++)
    {
        databuf[i] = mf_pack->data_buf[i+mf_pack->cur_recv_idx];
    }
    byte = 0x20 | ((mf_pack->cur_sn)&0x0f);
    uds_send_flg = TRUE;
    if(uds_is_canfd()){
        ret = uds_send_data64(g_uds_config.tx_id,g_uds_config.tx_id_type,
                              byte,databuf[0],databuf[1],databuf[2],
                              databuf[3],databuf[4],databuf[5],databuf[6],
                              &databuf[7]);
    }else{
        ret = uds_send_data8(g_uds_config.tx_id,g_uds_config.tx_id_type,
                            byte,databuf[0],databuf[1],databuf[2],
                            databuf[3],databuf[4],databuf[5],databuf[6]);
    }
    

    return ((ret>0)?1:0);
}

unsigned char send_uds_FC(unsigned char fs,unsigned char bs,unsigned char stmin)
{//对于是否CANFD，流控帧格式都是不变的，只是数据域扩展不同
    int ret = 0;
     unsigned char databuf[64] = {0};
    //memset(databuf,0x55,64);
    //unsigned char FS = 0,BS = 0,STmin = 0;

    uds_send_flg = TRUE;
    if(uds_is_canfd()){
        ret = uds_send_data64(g_uds_config.tx_id,g_uds_config.tx_id_type,
                                0x30 | (fs&0x0f),bs,stmin,0,
                                0,0,0,0,
                                &databuf[0]);//后续全部填0
    }else{
        ret = uds_send_data8(g_uds_config.tx_id,g_uds_config.tx_id_type,
                                0x30 | (fs&0x0f),bs,stmin,0,
                                0,0,0,0);
    }

    return ((ret>0)?1:0);
}

unsigned char check_uds_type(struct CAN_DATA *p_can_data)// return uds type
{
    if((p_can_data->idt.id == g_uds_config.rx_phy_id) ||
        (p_can_data->idt.id == g_uds_config.rx_fuc_id))
    {// if 
        switch(((p_can_data->data[0]&0xf0)>>4))
        {
        case 0x00:// SF 单帧
            return SF_UDS_TYPE;
        case 0x01:// FF 首帧
            return FF_UDS_TYPE;
        case 0x02:// CF 连续帧
            return CF_UDS_TYPE;
        case 0x03:// FC 流控帧
            return FC_UDS_TYPE;
        default:
            return -1;
        }
    }
    else
        return -1;
}

unsigned char analyze_uds_SF(struct CAN_DATA *p_can_data,struct SF_PACK *sf_pack)//return  SF_DL len
{
    unsigned char SF_DL = 0, i = 0;
    if(uds_is_canfd()){//canfd单帧的SDL位于BYTE1
        SF_DL = p_can_data->data[1];
        if(SF_DL > 62)
            return 0xff;//len error
        if(SF_DL == 0)
            return 0xff;//len is not equal 0
    }else{
        SF_DL = p_can_data->data[0] & 0x0f;
        if(SF_DL > 7)
            return 0xff;//len error
        if(SF_DL == 0)
            return 0xff;//len is not equal 0
    }
    if(uds_is_canfd())
    {
        for(i = 0;i < SF_DL; i++)
        {
            sf_pack->data_buf[i] = p_can_data->data[i+2];
        }
    }
    else
    {
        for(i = 0;i < SF_DL; i++)
        {
            sf_pack->data_buf[i] = p_can_data->data[i+1];
        }
    }

    sf_pack->data_len = SF_DL;
    return SF_DL;
}

unsigned short analyze_uds_FF(struct CAN_DATA *p_can_data,struct MF_PACK *mf_pack)// return FF_DL
{// when receive FF, should triggle CF
    unsigned short FF_DL = 0;
    unsigned char i = 0;
    if(uds_is_canfd()){//canfd首帧的SDL位于BYTE2/3/4/5
        FF_DL = (p_can_data->data[4] << 8) + (p_can_data->data[5]);
        for(i = 0;i < 58;i++){//剩余58个字节
            mf_pack->data_buf[i] = p_can_data->data[i+6];
        }
        mf_pack->cur_recv_idx = 58;
    }else{
        FF_DL = ((p_can_data->data[0]&0x0f) << 8) + p_can_data->data[1];
        for(i = 0;i < 6;i++){
            mf_pack->data_buf[i] = p_can_data->data[i+2];
        }
        mf_pack->cur_recv_idx = 6;
    }
    mf_pack->data_len = FF_DL;
    mf_pack->frame_sta = MF_STA_REVING;
    mf_pack->cur_sn = 1;// this is first frame ,so , set the first continue frame's sn to 1

    // /*test UDS
        d_printf("analyze_uds_FF, %04x",mf_pack->data_len);
    // test UDS*/

    return FF_DL;
}


unsigned char analyze_uds_CF(struct CAN_DATA *p_can_data,struct MF_PACK *mf_pack)// return sn
{// current do not solve over data length ###
    unsigned char SN = 0, i = 0;
    unsigned char data_len = 0;

    SN = (p_can_data->data[0]&0x0f);
    if(SN != mf_pack->cur_sn){
        return ERR_SN_WRONG;// CF SN ERROR
    }else{// checking sn auto increase
        if(mf_pack->cur_sn == 0x0f)
            mf_pack->cur_sn = 0;// 1;  when sn go to 0x0f , come to be 0
        else
            mf_pack->cur_sn++;
    }

    if(uds_is_canfd()){//连续帧只有数据长度不一样
        data_len = 63;
    }else{
        data_len = 7;
    }
    for(i = 0;i < data_len;i++){
        mf_pack->data_buf[mf_pack->cur_recv_idx] = p_can_data->data[i+1];
        mf_pack->cur_recv_idx++;
    }
    if(mf_pack->cur_recv_idx >= mf_pack->data_len){
        static unsigned long tick = 0;
        tick = get_timer_tick();// update timer
        d_printf("return ERR_RECV_END: %x\r\n",tick);
         return ERR_RECV_END;// all data has receive


    }else{
        return SN;// return correct sn
    }
}

unsigned char analyze_uds_FC(struct CAN_DATA *p_can_data,struct FC_STATUS *fc_sta)// return fs status,if error return (-1)
{//流控帧对于是否canfd，内容完全一致
    unsigned char FS = 0;

    fc_sta->bs = p_can_data->data[1];
    fc_sta->stmin = p_can_data->data[2];
    FS = p_can_data->data[0] & 0x0f;
    switch(FS)
    {
    case 0:
        fc_sta->fs = FS;
        return FC_FS_CTS;
    case 1:
        fc_sta->fs = FS;
        return FC_FS_WT;
    case 2:
        fc_sta->fs = FS;
        return FC_FS_OVFLW;
    default:
        fc_sta->fs = FC_FS_UNUSED;// is error
        return -1;
    }
}


void reset_sf_buf(struct SF_PACK *sf_pack)
{
    sf_pack->data_buf[0] = 0;
    sf_pack->data_len = 0;
}

void reset_mf_buf(struct MF_PACK *mf_pack)
{
    mf_pack->cur_recv_idx = 0;
    mf_pack->cur_sn = 0;
    mf_pack->data_buf[0] = 0;
    mf_pack->data_len = 0;
    mf_pack->frame_sta = MF_STA_EMP;
}

void reset_fc_sta(struct FC_STATUS *fc_sta)
{
    fc_sta->bs = 0;
    fc_sta->stmin = 0;
    fc_sta->fs = FC_FS_UNUSED;
}