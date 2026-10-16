/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : BSPFLASH.c
** @Founder      : WYC834
** @Date         : 2025.09.03
** @Function     : BSPFLASH驱动函数模块
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "main.h"
#include "BSPTypeDef.h"
#include "BSPConfig.h"
#include "BSPMCG.h"
#include "BSPWDOG.h"
#include "BSPIRQ.h"
#include "BSPFLASH.h"
/*=============================================================================================
** @Global variable definition
**===========================================================================================*/
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/

HAL_StatusTypeDef HAL_FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);
/*=============================================================================================
** @Interface function definition
**===========================================================================================*/
/*=============================================================================================
** @Name      : void BSPINTFLASHInit(void)
** @Input     : void
** @Output    : void
** @Function  : flash初始化
** @The notes :
**===========================================================================================*/
void BSPINTFLASHInit(void)
{
    u32 flash_latency_set = 0,cnt = 0;
    //MyFLASH_Ptr = FLASH2_Init(NULL);
    if(HAL_RCC_GetHCLKFreq() <= 24000000){
        flash_latency_set = FLASH_LATENCY_0;
    }
    else if(HAL_RCC_GetHCLKFreq() <= 48000000){
        flash_latency_set = FLASH_LATENCY_1;
    }
    else if(HAL_RCC_GetHCLKFreq() <= 64000000){
        flash_latency_set = FLASH_LATENCY_2;
    }
    
    while(flash_latency_set != __HAL_FLASH_GET_LATENCY())
    {
        __HAL_FLASH_SET_LATENCY(flash_latency_set);
        HAL_Delay(10);
        if(cnt++ > 10)
        {
            Error_Handler();
        }
    }

#if(DEBUG_EN ==1)
    HAL_FLASHEx_EnableDebugger();
#endif
}

/*=============================================================================================
** @Name      : static u32 get_pagenum(u32 Addr)
** @Input     : stru_flash_pos *flash_pos：pull in addr,pull out bank number & page number
** @Output    : TRUE 成功, FALSE 失败
** @Function  : 根据flash地址获取块、页序号
** @The notes : 按规格书划分匹配页号
**===========================================================================================*/
static u32 get_flash_pos(stru_flash_pos *flash_pos)
{
    u32 pagenum = 0,temp_addr = 0;
    if(flash_pos->addr < FLASH_ADDR_START || flash_pos->addr >= (FLASH_ADDR_START+FLASH_ADDR_SIZE))
        return FALSE;
    temp_addr = flash_pos->addr - FLASH_ADDR_START;
    pagenum = temp_addr/FLASH_PAGE_SIZE;
    /*stm32g0b1xx flash page number 跨bank不连续*/
    if(pagenum > 127){
        flash_pos->bank_index = 1;
        flash_pos->page_index = pagenum - FLASH_PAGE_NB + 256;
    }
    else{

        flash_pos->bank_index = 0;
        flash_pos->page_index = pagenum;
    }

    return TRUE;
}

/*=============================================================================================
** @Name      : u32 BSPFlashErase(u32 addr,u32 len)
** @Input     : u32 addr：擦除起始地址 0x0800 0000~0x0803 FFFF
**              u32 len: 擦除长度（byte）,长度需要为页大小 FLASH_PAGE_SIZE 的整数倍
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除片内Flash指定地址长度
** @The notes : 
**===========================================================================================*/
u32 BSPFlashErase(u32 addr,u32 len)
{
    u8 result = TRUE;    
    
    //u32 secAddr = 0;
    //设置PageError
    uint32_t PageError = 0,margin_bank_pages = 0,erase_page_num = 0;
    
    FLASH_EraseInitTypeDef flash_opr;
    stru_flash_pos flash_pos = {0};
    // BSP_DISABLE_IRQ();
    __disable_irq();
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }

    if(addr < FLASH_ADDR_START ||
       addr > (FLASH_ADDR_START+FLASH_ADDR_SIZE)||
       (addr+len) > (FLASH_ADDR_START+FLASH_ADDR_SIZE) ||
       len < FLASH_PAGE_SIZE ||
       len % FLASH_PAGE_SIZE){
        return(FALSE);
       }
    erase_page_num = len/FLASH_PAGE_SIZE;   //计算需要擦除的页数量
    flash_opr.TypeErase = FLASH_TYPEERASE_PAGES;        //页擦除 
    flash_pos.addr = addr;
    /*根据flash地址获取块、页序号*/
    if(FALSE == get_flash_pos(&flash_pos)){
        return(FALSE);
    }
    else{
                                       
        /*擦除起始页在bank0*/
        if(0 == flash_pos.bank_index){
            /*bank0擦除页数量*/
            flash_opr.Banks = FLASH_BANK_1;
            flash_opr.Page = flash_pos.page_index;
            margin_bank_pages = FLASH_PAGE_NB - flash_pos.page_index;
            /*不跨bank区，需要一次擦操作*/
            if(margin_bank_pages > erase_page_num)
            {   
                flash_opr.NbPages = erase_page_num;
                HAL_FLASH_Unlock();
                __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
                if(HAL_OK != HAL_FLASHEx_Erase(&flash_opr, &PageError)){
                    HAL_FLASH_Lock();
                    return(FALSE);
                } 
                HAL_FLASH_Lock();
            }else/*跨bank区，需要两次擦操作*/
            {
                flash_opr.NbPages = margin_bank_pages;
                HAL_FLASH_Unlock();
                __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
                if(HAL_OK != HAL_FLASHEx_Erase(&flash_opr, &PageError)){
                    HAL_FLASH_Lock();
                    return(FALSE);
                }
                HAL_FLASH_Lock();
                /*传入剩余需要擦除起始页与页数量*/
                flash_opr.NbPages = erase_page_num - margin_bank_pages;
                flash_opr.Banks = FLASH_BANK_2;
                flash_opr.Page = 256;

                HAL_FLASH_Unlock();
                __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
                if(HAL_OK != HAL_FLASHEx_Erase(&flash_opr, &PageError)){
                    HAL_FLASH_Lock();
                    return(FALSE);
                } 
                HAL_FLASH_Lock();
            }
            


        }
        /*擦除起始页在bank1*/
        else if(1 == flash_pos.bank_index){
            /*stm32g0b1rct6 flash 页范围*/
            if((flash_pos.page_index + erase_page_num) > 383){
                return(FALSE);
            }
            else{
                flash_opr.Banks = FLASH_BANK_2;
                flash_opr.Page = flash_pos.page_index;
                flash_opr.NbPages = erase_page_num; 
                /*擦写bank1页*/
                HAL_FLASH_Unlock();
                __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
                if(HAL_OK != HAL_FLASHEx_Erase(&flash_opr, &PageError)){
                    result = FALSE;
                }
                HAL_FLASH_Lock(); 


            }   
        }

    }      

    
    // BSP_ENABLE_IRQ();
    __enable_irq();
    return(result); 
}

/*=============================================================================================
** @Name      : void BSPFlashEraseChip(void)
** @Input     : void
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除整个MCU整体FLASH
** @The notes : 用户慎用
**===========================================================================================*/
u8 BSPFlashEraseChip(void)
{
    u8 result = 0;    
    //u16 secNum = 0;
    //u32 secAddr = 0;
    //设置PageError
    uint32_t PageError = 0;
    FLASH_EraseInitTypeDef flash_opr;

    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }

    //计算扇区信息
    //secAddr = FLASH_ADDR_START;
    //secNum = (u16)(FLASH_PAGE_NB);//每bank的页数
    
    flash_opr.TypeErase = FLASH_TYPEERASE_MASS;//页擦除    
    flash_opr.Banks = FLASH_BANK_1;	        //擦除banks1地址

       		
    if(HAL_OK != HAL_FLASHEx_Erase(&flash_opr, &PageError)){
        return(FALSE);
    }
#ifdef FLASH_DBANK_SUPPORT
    flash_opr.Banks = FLASH_BANK_2;   //擦除banks2地址
    if(HAL_OK != HAL_FLASHEx_Erase(&flash_opr, &PageError)){
        return(FALSE);
    }
#endif  
    
    //BSP_ENABLE_IRQ();

    return(result);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseBlock(u16 block)
** @Input     : u16 sector:FLASH块号[0,4]
** @Output    : TRUE:成功 FALSE:失败
** @Function  : 擦除FLASH块
** @The notes : 原代码将block与分区概念混淆，因此需要纠正，此处的block对应的是flash分区
**===========================================================================================*/
u8 BSPINTFLASHEraseBlock(u16 block)
{    
    u8 result = TRUE;
    u32 addr = 0,erase_size = 0;
    enum_flash_partnum part_index = (enum_flash_partnum)block;
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }
    
    switch(part_index)
    {
        //第一块Flash(40k,BOOT程序块)
        case eFlash_part0:
            addr = FLASH_PART0_BOOT_ADDR;
            erase_size = FLASH_PART0_BOOT_SIZE;

            break;
            
		//第二块Flash(100k,APP程序块)
		case eFlash_part1:
			addr = FLASH_PART1_APP_ADDR;
			erase_size = FLASH_PART1_APP_SIZE;
			break;
		
        // //第三块Flash(100k,APP备份块)
        // case eFlash_part2:
        //     addr = FLASH_PART2_APP_BAK_ADDR;
        //     erase_size = FLASH_PART2_APP_BAK_SIZE;
            
        //     break;

		//第四块Flash(2k,BOOT参数块)
		case eFlash_part3:
			addr = FLASH_PART4_BOOT_PARAM_ADDR;
			erase_size = FLASH_PART4_BOOT_PARAM_SIZE;
			break;
            
        //第五块Flash(2k,APP参数块)
        case eFlash_part4:
            addr = FLASH_PART3_APP_PARAM_ADDR;
            erase_size = FLASH_PART3_APP_PARAM_SIZE;
            
            break;
            
        default:
            result = FALSE;
            break;
    }
    result = BSPFlashErase(addr,erase_size);
    return(result);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHEraseSector(u32 addr)
** @Input     : addr:擦除页首地址，必须为页大小整数倍
** @Output    : TRUE:扇区擦除成功 FALSE:扇区擦除错误
** @Function  : 擦除FLASH扇区(一页)
** @The notes : 
**===========================================================================================*/
u8 BSPINTFLASHEraseSector(u32 addr)
{
    u32 result = 0;
    result = BSPFlashErase(addr,FLASH_PAGE_SIZE);
    return result;
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWrite(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为8的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 由于stm32g0b1xx 内置flash最小cell为64bit，而本函数写入最小大小为单个字节，因此需要补全8字节；
**              注意，只支持页内操作即最大2048byte，调用本函数前需要对相关的页进行擦除操作；
**===========================================================================================*/
u8 BSPINTFLASHWrite(u32 addr, u8 *pData, u32 len)
{
    u8 ret = TRUE;
    u32 dw_cnt = 0,opr_len = 0,i = 0,fw_cnt = 0,wt_offsite = 0,flash_offsite = addr;
    u8 *p_alloc = NULL;
    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }
    /*越界、对齐、指针检查*/
    if(addr < FLASH_ADDR_START ||
        (addr+len) >= (FLASH_ADDR_START+FLASH_ADDR_SIZE)||
        addr%8 ||
        (pData == (void *)0) ||
        len > FLASH_PAGE_SIZE){
            return(FALSE);
        }

    if(len % 8) /*补全8 byte整数倍*/
    {
        opr_len = len+8-len%8;
    }
    else
    {
        opr_len = len;
    }

    p_alloc = (u8 *)malloc(opr_len);
    if(p_alloc == NULL)
        return FALSE;
    /*填充数据*/
    for(i = 0;i<opr_len; i++)
    {
        if(i < len)
            p_alloc[i] = pData[i];
        else
            p_alloc[i] = 0xFF;
    }
    /*分解写操作步骤*/
    /*是否需要快写：写入总长大于256 byte*/
    if(opr_len >= 256)
    {
        fw_cnt = opr_len/256;
        /* 快写：最小长度256 byte方式循环写入flash */
        for(i = 0; i<fw_cnt;i++)
        {
            HAL_FLASH_Unlock();
            if(HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_FAST,flash_offsite, (uint64_t)p_alloc[wt_offsite]))
            {
                ret = FALSE;
                goto error;
            }
            HAL_FLASH_Lock();
            wt_offsite += 256;
            flash_offsite +=256;
        }
        
    }
    /*计算双字写入次数*/
    dw_cnt = (opr_len-wt_offsite)/8;
    
    /* 以64bit方式循环写入flash */
    for(i = 0; i< dw_cnt;i++)
    {
        HAL_FLASH_Unlock();
        if(HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,flash_offsite, *(u64*)&p_alloc[wt_offsite]))
        {
            ret = FALSE;
            goto error;
        }
        HAL_FLASH_Lock();
        flash_offsite += 8;
        wt_offsite +=8;
    }

error:
    {
        HAL_FLASH_Lock();
        free(p_alloc);
    }

    return(ret);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 新项目可删去，使用BSPINTFLASHWrite，本接口仅为兼容应用接口保留
**===========================================================================================*/
u8 BSPINTFLASHWriteByte(u32 addr, u8 *pData, u32 len)
{
    u8 ret = TRUE;
    ret = BSPINTFLASHWrite(addr, pData, len);
    return(ret);
}

/*=============================================================================================
** @Name      : u8 BSPFlashWriteHalfWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 新项目可删去，使用BSPINTFLASHWrite，本接口仅为兼容应用接口保留
**===========================================================================================*/
u8 BSPFlashWriteHalfWord(u32 addr, u16 *pData, u32 len)
{
    u8 ret = TRUE;
    ret = BSPINTFLASHWrite(addr, (u8 *)pData, len*2);
    return(ret);
}

/*=============================================================================================
** @Name      : u8 BSPFlashWriteWord(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 新项目可删去，使用BSPINTFLASHWrite，本接口仅为兼容应用接口保留
**===========================================================================================*/
u8 BSPFlashWriteWord(u32 addr, u32 *pData, u32 len)
{
    u8 ret = TRUE;
    ret = BSPINTFLASHWrite(addr, (u8 *)pData, len*4);
    return(ret);
}

/*=============================================================================================
** @Name      : u8 BSPFlashWriteLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:写入地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:写入数据指针 len:写入数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式写入数据(8位)(不包含Flash擦除,写之前必须确保Flash已擦除)
** @The notes : 驱动内部没有数据缓冲,要求写入过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPFlashWriteLong(u32 addr, u32 *pData, u32 len)
{
    u8 ret = TRUE;
    ret = BSPINTFLASHWrite(addr, (u8 *)pData, len);
    return(ret);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len)
** @Input     : addr:读取地址[0,131071](128K)(必须为4的倍数,Flash一次写32位) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字节方式读出数据
** @The notes : 由于stm32g0b1xx flash操作支持2次指令32bit读取64bit数据
**===========================================================================================*/
u8 BSPINTFLASHReadByte(u32 addr, u8 *pData, u32 len)
{
    u8 ret = TRUE,*p_rd = NULL;
    u32 *p_flash = NULL,tmp_data[2] = {0};
    u32 opr64_cnt = 0,rd_cnt = 0,rd_len = 0,opr64_num = 0,opr32_cnt = 0;

    if(1 != gBSPFunCfg.Bits.FLASHEn)
    {
        return(FALSE);
    }
    
    /*越界、对齐、指针检查*/
    if(addr < FLASH_ADDR_START ||
        (addr+len) >= (FLASH_ADDR_START+FLASH_ADDR_SIZE)||
        addr%8 ||
        len <=0 ||
        (pData == (void *)0)){
            return(FALSE);
        }
    if(len % 8) /*取整操作次数+1*/
    {
        opr64_num = (len/8) +1;
    }
    else
    {
        opr64_num = (len/8);
    }
    p_flash = (u32 *)addr;
    p_rd = (u8 *)tmp_data;
    rd_len = 0;
    for(opr64_cnt = 0;opr64_cnt<opr64_num;opr64_cnt++)  /*flash 读操作*/
    {
        /*2次 32bit读flash操作*/
        tmp_data[0] = p_flash[opr32_cnt++];
        tmp_data[1] = p_flash[opr32_cnt++];

        for(rd_cnt = 0;rd_cnt<8;rd_cnt++)   /*传入flash读出的8字节*/
        {
            pData[rd_len++] = p_rd[rd_cnt]; 
            if(rd_len >= len)
                break;
        }
    }

    return ret;
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadHalfWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按半字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadHalfWord(u32 addr, u16 *pData, u32 len)
{
    return BSPINTFLASHReadByte(addr,(u8 *)pData,len*2);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadWord(u32 addr, u16 *pData, u32 len)
{
    return BSPINTFLASHReadByte(addr,(u8 *)pData,len*4);
}

/*=============================================================================================
** @Name      : u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len)
** @Input     : addr:读取FLASH地址([0,1048575](1M)) *pData:读出的数据指针 len:读取数据的个数
** @Output    : TRUE:成功 FALSE:失败
** @Function  : Flash按双字方式读出数据
** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
**===========================================================================================*/
u8 BSPINTFLASHReadLong(u32 addr, u32 *pData, u32 len)
{
    return BSPINTFLASHReadByte(addr,(u8 *)pData,len);
}

// /*=============================================================================================
// ** @Name      : s8 BSPFLASH_test(void)
// ** @Input     : none
// ** @Output    : TRUE:成功 FALSE:失败
// ** @Function  : Flash按双字方式读出数据
// ** @The notes : 驱动内部没有数据缓冲,要求读出过程数据指针对应的数据不能改变
// **===========================================================================================*/
// s32 BSPFLASH_test(void)
// {
//     u8 w_data[1032] = {0};
//     u8 r_data[1032] = {0};
//     u32 i = 0,*P_data = (u32 *)w_data;
//     /*擦除*/
//     if(FALSE == BSPFlashErase(FLASH_PART2_APP_BAK_ADDR,2048))
//         return FALSE;
//     /*写入*/
//     for(i = 0;i<1032/4;i++)
//     {
//         P_data[i] = i;
//     }
    
//     if(FALSE == BSPINTFLASHWriteByte(FLASH_PART2_APP_BAK_ADDR,w_data,1032))
//         return FALSE;
//     /*读取*/
//     if(FALSE == BSPINTFLASHReadByte(FLASH_PART2_APP_BAK_ADDR,r_data,1032))
//         return FALSE;
//     for(i = 0;i<1032;i++)
//     {
//         if(r_data[i] != w_data[i])
//         {
//             return FALSE;
//         }
//     }


//     return TRUE;
// }

/*=============================================================================================
** @Static function definition
**===========================================================================================*/





