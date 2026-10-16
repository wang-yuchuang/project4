/* ******************* 
// can uds需要重载实现的接口
 **********************/
#include "SystemInfo.h"
#include "BSPCAN.h"
#include "BSPGD25Q32E.h"
#include "BSPSPI.h"
#include "can_uds_arch.h"
#include "BSPIRQ.h"
#include "cmsis_os.h"
#include "DoControl.h"
#include "services_layer/device_role/sid_internal_header.h"
#include "api_for_application.h"
#include "sid_internal_header.h"

#include "BSPGD25Q32E.h"
#include "BSPSPI.h"
//在这里添加你需要依赖的头文件



St_FirmwareCanUdsUpgradeConfig pgst_firwareUdsUpgrade = 
{
	.externalUseBlockNum = FlashMap_APP_BLOCK_NUM,
	.externalFlashStartAddr = FlashMap_APP_BAK_ADDR,
	.storageType = emSTORAGE_EXTERNAL_FLASH,
};


//----
static unsigned long transform1(unsigned long seed);
static unsigned long transform2(unsigned long seed);


uint8_t ProFlash_buffer[1024] = {0};


int uds_send_data8(unsigned long id,unsigned char type,
                    unsigned char data0,unsigned char data1,unsigned char data2,unsigned char data3,
                    unsigned char data4,unsigned char data5,unsigned char data6,unsigned char data7)
{
    t_CANMSG	sCANTxItemInfo;
    sCANTxItemInfo.ID = id;
    sCANTxItemInfo.Len = 8;
    sCANTxItemInfo.Data[0] = data0;
    sCANTxItemInfo.Data[1] = data1;
    sCANTxItemInfo.Data[2] = data2;
    sCANTxItemInfo.Data[3] = data3;
    sCANTxItemInfo.Data[4] = data4;
    sCANTxItemInfo.Data[5] = data5;
    sCANTxItemInfo.Data[6] = data6;
    sCANTxItemInfo.Data[7] = data7;
    BSPCANSendQin(eCAN1,&sCANTxItemInfo);
    return 1;

}

//如果需要支持can-fd，则这个函数也需要实现
int uds_send_data64(unsigned long id,unsigned char type,
                    unsigned char data0,unsigned char data1,unsigned char data2,unsigned char data3,
                    unsigned char data4,unsigned char data5,unsigned char data6,unsigned char data7,
                    unsigned char *next_datas)
{
    t_CANMSG	sCANTxItemInfo;
    sCANTxItemInfo.ID = id;
    sCANTxItemInfo.Len = 64;
    sCANTxItemInfo.Data[0] = data0;
    sCANTxItemInfo.Data[1] = data1;
    sCANTxItemInfo.Data[2] = data2;
    sCANTxItemInfo.Data[3] = data3;
    sCANTxItemInfo.Data[4] = data4;
    sCANTxItemInfo.Data[5] = data5;
    sCANTxItemInfo.Data[6] = data6;
    sCANTxItemInfo.Data[7] = data7;
    for(u8 i =0;i<56; i++)
    {
        sCANTxItemInfo.Data[i+8] = next_datas[i];
    }
    BSPCANSendQin(eCAN1,&sCANTxItemInfo);
    return 1;
}


/*将UDS 0x36服务（传输数据）中，接收到的固件数据写入存储*/
int program_data_by_logic_addr(unsigned long addr,unsigned char *msg_buf,unsigned short lenth)
{

/*需要考虑加密压缩算法*/
    uint8_t ret = 0;
    uint32_t i = 0;
    // uint16_t write_lenth_padded = lenth;

d_printf("program_data_by_logic_addr,  %x\r\n",lenth);
    for (i = 0; i < lenth; i++)
    {
        ProFlash_buffer[i] = msg_buf[i];
    }
    if (lenth % FLASH_MIN_W_SZIE)
    {
        for( i =0; i< (FLASH_MIN_W_SZIE-(lenth % FLASH_MIN_W_SZIE)); i++)
                    ProFlash_buffer[lenth+i] = 0xff;
        lenth += (FLASH_MIN_W_SZIE - lenth % FLASH_MIN_W_SZIE);

    }
d_printf("external_flash,  %x\r\n",lenth);    


    if (pgst_firwareUdsUpgrade.storageType == emSTORAGE_EXTERNAL_FLASH)
    {
        static unsigned long tick_start = 0;
        static unsigned long tick_end = 0;
        tick_start = get_timer_tick();// update timer
        d_printf("write_external_flash_time_start,  %x\r\n",tick_start);    

                ret = segmentedWriteDataToOutFlash(ProFlash_buffer, pgst_firwareUdsUpgrade.externalFlashStartAddr + addr, lenth);

        tick_end = get_timer_tick();// update timer
        d_printf("write_external_flash_time_end,  %x\r\n",tick_end);   
        if(ret == FLASH_STATUS_ACCESS_OK)
        {   
            ret = 1;
            if(addr >FlashMap_APP_BAK_SIZE)
            {
                ret = 0;
            }
            return ret;
        }
    }

    ret = 0;
    return ret;
}


//写入外部存储
uint8_t ReadData[1024] = {0};
uint8_t segmentedWriteDataToOutFlash(uint8 *data, uint32 OutflashStartAddr, uint32 totalLength)
{
    uint32 offset = 0; // 初始化偏移量为0


    d_printf("segmentedWriteDataToOutFlash,  %x\r\n",totalLength); 
    // 循环直到所有数据都被写入
    while (totalLength > 0)
    {
        uint32 chunkSize = (totalLength > 64) ? 64 : totalLength; // 计算本次写入的块大小
        uint32 writeAddr = OutflashStartAddr + offset;                 // 计算写入的FLASH地址

        // 调用写入函数
       
          d_printf("DRV_FLASH_WriteBytes,  %x\r\n",writeAddr); 
        if (DRV_FLASH_WriteBytes(data + offset, writeAddr, chunkSize) != FLASH_STATUS_ACCESS_OK)
        {
              d_printf("DRV_FLASH_WriteBytes fail,  %x\r\n",writeAddr); 
            return FLASH_STATUS_ACCESS_FAIL;
        }

        (void)DRV_FLASH_ReadBytes(ReadData + offset, writeAddr, chunkSize);

        if (memcmp(ReadData + offset, data + offset, chunkSize) != 0)
        {
            return FLASH_STATUS_ACCESS_FAIL;
        }

        offset += chunkSize;      // 增加偏移量
        totalLength -= chunkSize; // 减少剩余长度
    }

    return FLASH_STATUS_ACCESS_OK;
}



/*擦除目标地址内容，用于SID31中*/
int uds_erase_app_data(void)
{
    unsigned char err  = 0;
    uint8 eraseSucceeCnt = 0;
    if (pgst_firwareUdsUpgrade.storageType == emSTORAGE_EXTERNAL_FLASH)
    {
        for (int i = 0; i < pgst_firwareUdsUpgrade.externalUseBlockNum; i++)
        {
             d_printf("uds_erase_app_data, refuse\n");
            // __disable_irq() 已移除: SPI1独占GD25Q32E无需关中断, 关闭会误杀FDCAN中断导致UDS帧丢失
            // BSP_DISABLE_IRQ();
			 if ((DRV_FLASH_EraseBlock(pgst_firwareUdsUpgrade.externalFlashStartAddr + i * 0x010000U) == FLASH_STATUS_ACCESS_OK))
			 {
				eraseSucceeCnt++;
			 }
            // __enable_irq()
            // BSP_ENABLE_IRQ();
			
		}
		if (eraseSucceeCnt != pgst_firwareUdsUpgrade.externalUseBlockNum)
		{
			err = 0;
        }else
        {
            err = 1;
        }
    }
    return err;
}

void uds_reset(void)
{//进入死循环，等待看门狗复位

    uds_program_finish();//写入升级标志
}

/*在升级数据全部接收完成后，写入升级标志，然后复位系统*/
int uds_program_finish(void)
{//编程结束后进行的操作，一般情况下用于更新升级标志
    uint32 flash_addr = 0;
    if (pgst_firwareUdsUpgrade.storageType == emSTORAGE_EXTERNAL_FLASH)
    {
        flash_addr = pgst_firwareUdsUpgrade.externalFlashStartAddr;
        write_update_flag_for_app(pg_memory_size,flash_addr,1,1);
    }
		CloseDOCmd();
		// Wait a moment for hard reset
		//BSPDelay(1000);
		osDelay(1000);
		// Add hard reset here
		//EnterAppPro();
		BSP_DISABLE_IRQ();
		NVIC_SystemReset(); 
    
}



//硬件始终配置为CAN-FD模式 (FDCAN_FRAME_FD_BRS)，直接返回1
int uds_is_canfd(void)
{
    return 1;
}

unsigned long uds_crc(unsigned long crc, const void *buffer, int size) 
{
    static const unsigned long rtable[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c,
    };

    const uint8_t *data = buffer;

    for (int i = 0; i < size; i++) {
        crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 0)) & 0xf];
        crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 4)) & 0xf];
    }

    return crc;
}

void uds_wholepackcrc(unsigned long *wholepackcrc,const void *buffer, int size)
{
    static const unsigned long rtable[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c,
    };

    const uint8_t *data = buffer;

    for (int i = 0; i < size; i++) {
        *wholepackcrc = (*wholepackcrc >> 4) ^ rtable[(*wholepackcrc ^ (data[i] >> 0)) & 0xf];
        *wholepackcrc = (*wholepackcrc >> 4) ^ rtable[(*wholepackcrc ^ (data[i] >> 4)) & 0xf];
    }

}

void uds_crc_init(unsigned long *wholepackcrc)
{
    *wholepackcrc = 0xFFFFFFFFuL;

    return;
} 


void uds_feed_wdt(void)
{//如果开启了看门狗，向此函数中添加喂狗程序片段

}

// --- 内部函数 ---
static unsigned long transform1(unsigned long seed) 
{
    unsigned char* b = (unsigned char*)&seed;
    for(int i=0; i<4; i++) {
        b[i] = (b[i] << 1) | (b[i] >> 7);
        b[i] ^= 0x1B; // 类似AES的轮常数[[9]]
    }
    return seed;
}

static unsigned long transform2(unsigned long seed) 
{
    unsigned long temp = (seed >> 24) |          // 取第一个字节
                   ((seed >> 8) & 0xFF00) | // 取第二个字节
                   ((seed << 8) & 0xFF0000)|// 取第三个字节
                   (seed << 24);            // 取第四个字节
    return temp ^ 0xA5A555AA;
}


/**/
unsigned long get_timer_tick(void)
{
    return xTaskGetTickCount();
}


unsigned long uds_get_seed(void)
{
    return get_timer_tick() ^ 0xAAff5A75;
}


unsigned long uds_seed_to_key(unsigned long input_seed)
{//seed到key的加密算法
    unsigned long key1 = transform1(input_seed);
    unsigned long key2 = transform2(input_seed);
    return key1 ^ key2;
}

void ENABLEIRQHANDLE(void)
{
     __enable_irq(); 
    // BSP_ENABLE_IRQ();
}