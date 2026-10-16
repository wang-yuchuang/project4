/*==========================================Copyright(c)=======================================
**                                  Shenzhen Infypower Co., Ltd.
**                                  Power conversion Department
**                                       BMS Product Line
**
**---------------------------------------File Information--------------------------------------
** @File Name    : Modbus.c
** @Founder      : WYP40505
** @Date         : 20260228
** @Function     : MODBUS
** @Instructions :
**===========================================================================================*/

/*=============================================================================================
** @Header file definition
**===========================================================================================*/
#include "Modbus.h"
#include "BSPUSART.h"
/*=============================================================================================
** @Macro definition
**===========================================================================================*/

/*=============================================================================================
** @Data type definition
**===========================================================================================*/
stc_modbus  pstcModbus;
u8 ModbusReg[MAX_HOLDING_REGS][2];
/*=============================================================================================
** @Static function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      :static u8 Modbus_BuildResponse(stc_modbus *p_modbus, u8 *tx_buf)
** @Input     : 
** @Output    : 
** @Function  :
** @The notes : 
**===========================================================================================*/
static u8* Modbus_ErrorResponse(stc_modbus *p_modbus, u8 *tx_buf);

/*=============================================================================================
** @Name      :static u8 Modbus_BuildResponse(stc_modbus *p_modbus, u8 *tx_buf)
** @Input     : 
** @Output    : 
** @Function  :输出请求和要返回的数据，输出缓存区
** @The notes : 
**===========================================================================================*/
static u8 Modbus_BuildResponse(stc_modbus *p_modbus, u8 *tx_buf);

/*=============================================================================================
** @Name      : static u8 Modbus_ExecuteOperation(stc_modbus *p_modbus)
** @Input     : 
** @Output    : 
** @Function  : 执行操作,根据解析后得到的modbus请求，执行相应的寄存器读写操作
** @The notes : 
**===========================================================================================*/
static u8 Modbus_ExecuteOperation(stc_modbus *p_modbus);

/*=============================================================================================
** @Name      : static u8 Modbus_ParsingFrame(u8 *rx_buf, u8 rx_len, stc_modbus *p_modbus)
** @Input     : 
** @Output    : 
** @Function  : modbus功能码解析 
** @The notes : 
**===========================================================================================*/
static u8 Modbus_ParsingFrame(u8 *rx_buf, u8 rx_len, stc_modbus *p_modbus);

/*=============================================================================================
** @Name      : static u8 Modbus_RTUProcess(u8 *rx_buf, stc_modbus *p_modbus);
** @Input     : void
** @Output    :   
** @Function  : modbus基础数据解析
** @The notes : 
**===========================================================================================*/
static u8 Modbus_RTUProcess(u8 *rx_buf, stc_modbus *p_modbus);

/*=============================================================================================
** @Name      : static u16 ModbusCrc(u8 *from, u8 *to);
** @Input     : *from:crc校验数据开始 *to:crc校验数据结束 
** @Output    : crc计算后的数值
** @Function  : USART的crc校验
** @The notes : 用户调用
**===========================================================================================*/
static u16 ModbusCrc(u8 *data, u8 size);



/*=============================================================================================
** @Interface function declaration
**===========================================================================================*/
/*=============================================================================================
** @Name      : void Modbus_ToolingInterface(t_USARTMSG *pmsgData)
** @Input     : 串口接收数据
** @Output    : void
** @Function  : 上位机通信主函数，解析报文并返回回复上位机的报文
** @The notes : 
**===========================================================================================*/
void Modbus_ToolingInterface(t_USARTMSG *pmsgData)
{
	if (pmsgData == NULL || pmsgData->len == 0) 
	{
        return;
    }
	//1. 解析数据
	u8 status = Modbus_RTUProcess(pmsgData->data,&pstcModbus);

	//根据解析结果进行读写操作 并 生成回复报文
	if (status == MODBUS_OK)
	{
		//2. 读写操作
		Modbus_ExecuteOperation(&pstcModbus);
	}

	// 3.生成回复报文
	u8 bufReply[300];
    u8 lenReply = 0;
	if (status == MODBUS_OK)
	{
         lenReply = Modbus_BuildResponse(&pstcModbus, bufReply);
	}
	//错误回复报文
	else 
	{// 构建错误响应
        u8 *errorReply = Modbus_ErrorResponse(&pstcModbus, pmsgData->data);
        if (errorReply != NULL) 
		{
            memcpy(bufReply, errorReply, 5);
            lenReply = 5;
		}
	}

	if (lenReply > 0)
	{
		BSPUSARTSendQin(eUSART6, bufReply, lenReply); 
	}
}


/*=============================================================================================
** @Name      : u8 Test_LengthRecData(u8*pData)	
** @Input     : 报文数据
** @Output    : 报文长度
** @Function  : modbus报文数据长度解析
** @The notes : 
**===========================================================================================*/
u8 Test_LengthRecData(u8*pData)		
{
	if(pData[1] == MODBUS_READ_HOLD_REG)		//读报文长度
		{
			return 8;					
		}
	if (pData[1] == MODBUS_WRITE_SIN_HOLD_REG) 
		{
			return 8;		
		}
		
	if(pData[1] == MODBUS_WRITE_MULT_HOLD_REG)    //写报文长度
		{
			return (pData[4]<<8|pData[5])*2+9;
		}	
        return false;
}

/*=============================================================================================
** @Name      : static u8 Modbus_RTUProcess(void)
** @Input     : void
** @Output    : Modbus状态  
** @Function  : modbus基础数据解析
** @The notes : 
**===========================================================================================*/
static u8 Modbus_RTUProcess(u8 *rx_buf, stc_modbus *p_modbus)
{
	memset(p_modbus, 0, sizeof(stc_modbus));
	//获取报文完整长度
	u8 rx_len;
	rx_len = Test_LengthRecData(rx_buf);
	
	//1.基本报文长度检查
	if (rx_len < 4)//地址+功能码+CRC最小长度
	{
		p_modbus->error_code = MODBUS_ERR_DATA;
		return MODBUS_ERR_DATA;
	}
	
	//2. 解析报文
	p_modbus->address = rx_buf[0];
	p_modbus->functionCode = rx_buf[1];
	p_modbus->reg_addr = (rx_buf[2] << 8) | rx_buf[3];
	p_modbus->data_len = rx_len;
	p_modbus->crc = (rx_buf[rx_len - 2] << 8) | rx_buf[rx_len - 1];

	//3. 地址是否正确
	if ((p_modbus->address != Slave1_Address))
	{
		p_modbus->error_code = MODBUS_ERR_ADDRESS;
		return MODBUS_ERR_ADDRESS;
	}

	//4. 检测寄存器地址是否正确
	if (p_modbus->reg_addr >= MAX_HOLDING_REGS)
	{
		p_modbus->error_code = MODBUS_ERR_REG_ADDR;
		return MODBUS_ERR_REG_ADDR;
	}
	
	//5. CRC校验是否正确
	u16 calc_crc  =  ModbusCrc(rx_buf, rx_len - 2);
	if (calc_crc != p_modbus->crc)
	{
		p_modbus->error_code = MODBUS_ERR_CRC;
		return MODBUS_ERR_CRC;
	}

	//根据功能码解析数据域
	return Modbus_ParsingFrame(rx_buf,rx_len,p_modbus);

}

/*=============================================================================================
** @Name      : static u8 Modbus_ParsingFrame(u8 *rx_buf, u8 rx_len, stc_modbus *p_modbus)
** @Input     : 报文数据 报文数据长度 modbus结构体
** @Output    : Modbus状态  
** @Function  : modbus功能码解析 
** @The notes : 
**===========================================================================================*/
static u8 Modbus_ParsingFrame(u8 *rx_buf, u8 rx_len, stc_modbus *p_modbus)
{
	u8 func_code = p_modbus->functionCode;

	switch (func_code)
	{
	//0x03
	case MODBUS_READ_HOLD_REG: 
		if (rx_len != 8) // 固定长度：地址+功能码+起始地址(2)+数量(2)+CRC(2)
		{
			p_modbus->error_code = MODBUS_ERR_DATA;
			return MODBUS_ERR_DATA;
		}
		p_modbus->reg_addr = (rx_buf[2] << 8) | rx_buf[3];
		p_modbus->reg_num  = (rx_buf[4] << 8) | rx_buf[5];
		p_modbus->data_len = 0;  // 请求无数据
		//对读取地址添加边界检查
		if (p_modbus->reg_addr > MAX_HOLDING_REGS)
		{
			p_modbus->error_code = MODBUS_ERR_REG_ADDR;
			return MODBUS_ERR_REG_ADDR;
		}
		//对读取数量添加边界检查
		if((p_modbus->reg_num == 0) || (p_modbus->reg_num > MAX_HOLDING_REGS))
		{
			p_modbus->error_code = MODBUS_ERR_REG_NUM;
			return MODBUS_ERR_REG_NUM;
		}
		break;

	//0x06
	case MODBUS_WRITE_SIN_HOLD_REG:
		 if (rx_len != 8) {
			p_modbus->error_code = MODBUS_ERR_DATA;
                return MODBUS_ERR_DATA;
            }
            p_modbus->reg_addr = (rx_buf[2] << 8) | rx_buf[3];
            p_modbus->reg_num = 1;
            // 存储要写入的数据
            p_modbus->registers[0][0] = rx_buf[4];  // 存储到临时缓冲区索引0
			p_modbus->registers[0][1] = rx_buf[5];
            p_modbus->data_len = 2;
            break;
	//0x10 
	case MODBUS_WRITE_MULT_HOLD_REG://写多个保持寄存器
			p_modbus->reg_addr = (rx_buf[2] << 8) | rx_buf[3];
			p_modbus->reg_num = (rx_buf[4] << 8) | rx_buf[5];
			p_modbus->data_len = rx_buf[6];  // 字节数
	
	// 检查数据长度是否匹配
	if (rx_len != (9 + p_modbus->data_len)) {
		p_modbus->error_code = MODBUS_ERR_DATA;
		return MODBUS_ERR_DATA;
	}
	for (u16 j = 0; j < p_modbus->reg_num; j++) {
		p_modbus->registers[j][0] = rx_buf[7 + j * 2];  // 第j个寄存器的高字节
		p_modbus->registers[j][1] = rx_buf[8 + j * 2];  // 第j个寄存器的低字节
	}
	break;

	default:
	p_modbus->error_code = MODBUS_ERR_FUNC_CODE;
	return MODBUS_ERR_FUNC_CODE;
	}

	return MODBUS_OK;

}
/*=============================================================================================
** @Name      : static u8 Modbus_ExecuteOperation(stc_modbus *p_modbus)
** @Input     : modbus结构体
** @Output    : modbus状态
** @Function  : 执行操作,根据解析后得到的modbus请求，执行相应的寄存器读写操作
** @The notes : 
**===========================================================================================*/
static u8 Modbus_ExecuteOperation(stc_modbus *p_modbus)
{
	u8 func_code = p_modbus->functionCode;
	u16 reg_addr  = p_modbus->reg_addr;
	u16 reg_num   = p_modbus->reg_num;

	if (p_modbus->error_code != MODBUS_OK)
	{
		return p_modbus->error_code;
	}

	switch (func_code)
	{
		//读保持寄存器03
	case MODBUS_READ_HOLD_REG:
		for (u16 i = 0; i < reg_num; i++) {
			u16 actual_reg_addr = reg_addr + i;
			if (actual_reg_addr < MAX_HOLDING_REGS) {  
				p_modbus->registers[i][0] = ModbusReg[actual_reg_addr][0];
				p_modbus->registers[i][1] = ModbusReg[actual_reg_addr][1];
			} else {
				// 寄存器地址无效
				p_modbus->registers[i][0] = 0;
				p_modbus->registers[i][1] = 0;
			}
		}
		p_modbus->data_len = reg_num * 2;
	break;
		
	case MODBUS_WRITE_SIN_HOLD_REG:
		// 写入单个保持寄存器06
		ModbusReg[reg_addr][0] = p_modbus->registers[0][0];
		ModbusReg[reg_addr][1] = p_modbus->registers[0][1];
		
		// 更新寄存器值到p_modbus->registers（用于响应）
		p_modbus->registers[0][0] = ModbusReg[reg_addr][0];
		p_modbus->registers[0][1] = ModbusReg[reg_addr][1];
		p_modbus->data_len = 2;
		break;
		
	case MODBUS_WRITE_MULT_HOLD_REG:
		// 写入连续保持寄存器10
		for (u16 i = 0; i < reg_num; i++) {
			u16 actual_reg_addr = reg_addr + i;
			ModbusReg[actual_reg_addr][0] = p_modbus->registers[i][0];
			ModbusReg[actual_reg_addr][1] = p_modbus->registers[i][1];
		}
		// 对于0x10功能码，响应中不需要寄存器数据
		p_modbus->data_len = 0;
		break;
	
	default:
		p_modbus->error_code = MODBUS_ERR_FUNC_CODE;
		return MODBUS_ERR_FUNC_CODE;
	}
	return p_modbus->error_code;
}

/*=============================================================================================
** @Name      : static u8 Modbus_BuildResponse(stc_modbus *p_modbus, u8 *bufReply)
** @Input     : modbus结构体 
** @Output    : 回复报文长度pos
** @Function  : 输出请求和要返回的数据，输出缓存区
** @The notes : 
**===========================================================================================*/
static u8 Modbus_BuildResponse(stc_modbus *p_modbus, u8 *bufReply)
{
    u8 pos = 0;
    
    // 1. 地址和功能码
    bufReply[pos++] = p_modbus->address;
    
    // 检查是否有错误
    if (p_modbus->error_code != MODBUS_OK) {
        bufReply[pos++] = p_modbus->functionCode | 0x80;  // 异常响应
        bufReply[pos++] = p_modbus->error_code;
    } else {
        bufReply[pos++] = p_modbus->functionCode;
        
        // 2. 根据功能码填充数据
        switch (p_modbus->functionCode) {
            case MODBUS_READ_HOLD_REG:  // 读保持寄存器响应
					bufReply[pos++] = p_modbus->data_len;  // 字节数
                // 复制寄存器数据
                for (u16 i = 0; i < p_modbus->reg_num; i++) {
                    bufReply[pos++] = p_modbus->registers[i][0];  // 高字节
                    bufReply[pos++] = p_modbus->registers[i][1];  // 低字节
                }
                break;
                
            case MODBUS_WRITE_SIN_HOLD_REG:  // 写单个寄存器响应
				bufReply[pos++] = (u8)(p_modbus->reg_addr >> 8);
				bufReply[pos++] = (u8)(p_modbus->reg_addr & 0xFF);
				bufReply[pos++] = p_modbus->registers[0][0];  // 高字节
				bufReply[pos++] = p_modbus->registers[0][1];  // 低字节
                break;
                
            case MODBUS_WRITE_MULT_HOLD_REG:  // 写多个寄存器响应
				bufReply[pos++] = (u8)(p_modbus->reg_addr >> 8);
				bufReply[pos++] = (u8)(p_modbus->reg_addr & 0xFF);
				bufReply[pos++] = (u8)(p_modbus->reg_num >> 8);
				bufReply[pos++] = (u8)(p_modbus->reg_num & 0xFF);
                break;
        }
    }
    
    // 3. 计算CRC
    u16 crc = ModbusCrc(bufReply, pos);
    bufReply[pos++] = (u8)((crc >> 8)& 0xFF);
    bufReply[pos++] = (u8)(crc)& 0xFF;
    
    return pos;  // 返回帧长度
}

/*=============================================================================================
** @Name      :static u8 Modbus_BuildResponse(stc_modbus *p_modbus, u8 *tx_buf)
** @Input     : modbus结构体 报文数据
** @Output    : 部分错误状态回复数据
** @Function  : 错误回应
** @The notes : 
**===========================================================================================*/
static u8* Modbus_ErrorResponse(stc_modbus *p_modbus, u8 *tx_buf)
{
	static u8 reply[5];
	u16 crc;
	switch (p_modbus->error_code)
	{
	case MODBUS_ERR_FUNC_CODE:
		reply[0] = p_modbus->address;
		reply[1] = p_modbus->functionCode | 0x80;		//最高位置一
		reply[2] = p_modbus->error_code;
		crc = ModbusCrc(reply, 3);	
		reply[3] =  (crc >> 8) & 0xFF;
		reply[4] =	crc & 0xFF;
		return reply;
		
	case MODBUS_ERR_REG_ADDR:
		reply[0] = p_modbus->address;
		reply[1] = p_modbus->functionCode | 0x80;		//最高位置一
		reply[2] = p_modbus->error_code;
		crc = ModbusCrc(reply, 3);	
		reply[3] =  (crc >> 8) & 0xFF;
		reply[4] =	crc & 0xFF;
		return reply;	
		
	case MODBUS_ERR_CRC:
		reply[0] = p_modbus->address;
		reply[1] = p_modbus->functionCode | 0x80;		//最高位置一
		reply[2] = p_modbus->error_code;
		crc = ModbusCrc(reply, 3);	
		reply[3] =  (crc >> 8) & 0xFF;
		reply[4] =	crc & 0xFF;
		return reply;	
		
	default:
	reply[0] = 0;
		return reply;
	}
}

/*=============================================================================================
** @Name      : static u16 ModbusCrc(u8 *from, u8 *to);
** @Input     : *from:crc校验数据开始 *to:crc校验数据结束 
** @Output    : crc计算后的数值
** @Function  : USART的crc校验
** @The notes : 用户调用
**===========================================================================================*/
static u16 ModbusCrc(u8 *data, u8 size)
{
    u8 uchCRCHi = 0xFF;  	
    u8 uchCRCLo = 0xFF; 	
    u8 uIndex; 	

	
    while (size--) 				 
    {
        uIndex = uchCRCHi ^ *data++; 	
        uchCRCHi = uchCRCLo ^ modbus_crc_hi_[uIndex]; 
        uchCRCLo = modbus_crc_lo_[uIndex]; 
    }
    return (((u16)uchCRCHi << 8) | uchCRCLo);
	
}

// u16 GetModbusCRC(u8* pData,u8 rx_len,stc_modbus *pModbus)
// {
// 	u16 crc = 0;
// 	crc = pData[rx_len - 2] | pData[rx_len - 1];
// 	u16 calc_crc = ModbusCrc(pData,rx_len);
// 	if (crc != calc_crc)
// 	{
// 		pModbus->error_code = MODBUS_ERR_CRC;
// 	}
// }