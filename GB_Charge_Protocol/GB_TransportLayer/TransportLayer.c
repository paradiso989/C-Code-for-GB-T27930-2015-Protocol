/*
 * TransportLayer.c
 *
 * Created on: 2025.7.24
 * Author: 83923
 *
 * Based On Open_SAE_J1939
 * https://github.com/DanielMartensson/Open-SAE-J1939 
 */

#include "TransportLayer.h"

/* 全局变量 */
static uint8_t tp_cm_buffer[TP_PACKET_SIZE];
static uint8_t tp_dt_buffer[TP_MAX_PACKET_SIZE];

/* 内部函数声明 */
static Transport_StatusTypeDef send_tp_cm(uint8_t priority, uint8_t da, uint8_t sa, uint8_t control_byte, uint16_t total_size, uint8_t packet_count, uint32_t pgn);
static Transport_StatusTypeDef send_tp_dt(uint8_t priority, uint8_t da, uint8_t sa, uint8_t sequence, uint8_t *data, uint8_t len);

/* 传输层初始化 */
void TransportLayer_Init(void) {
    // 初始化传输层相关参数
    for (uint16_t i = 0; i < TP_PACKET_SIZE; i++) {
        tp_cm_buffer[i] = 0;
    }
    for (uint16_t i = 0; i < TP_MAX_PACKET_SIZE; i++) {
        tp_dt_buffer[i] = 0;
    }
}

/*
 * 发送单包消息
 * 参数:
 *   priority: 优先级 (0-7, 0为最高)
 *   pgn: 参数组编号
 *   da: 目标地址
 *   sa: 源地址
 *   data: 数据指针
 *   len: 数据长度
 */
Transport_StatusTypeDef TransportLayer_Send_Message(uint8_t priority, uint32_t pgn, uint8_t da, uint8_t sa, uint8_t *data, uint8_t len) {
    if (data == NULL || len > TP_PACKET_SIZE) {
        return TRANSPORT_STATUS_INVALID_PARAM;
    }
    
    // 构造CAN ID
    uint32_t can_id = PGN_TO_CAN_ID(priority, pgn, da, sa);
    
    // 通过硬件层发送
    return (Transport_StatusTypeDef)Hardware_CAN_Transmit(can_id, data, len);
}

/*
 * 接收单包消息
 * 参数:
 *   pgn: 参数组编号指针
 *   data: 数据指针
 *   len: 数据长度指针
 *   da: 目标地址指针 (可为NULL)
 *   sa: 源地址指针 (可为NULL)
 *   priority: 优先级指针 (可为NULL)
 */
Transport_StatusTypeDef TransportLayer_Receive_Message(uint32_t *pgn, uint8_t *data, uint8_t *len, uint8_t *da, uint8_t *sa, uint8_t *priority) {
    if (pgn == NULL || data == NULL || len == NULL) {
        return TRANSPORT_STATUS_INVALID_PARAM;
    }
    
    // 通过硬件层接收
    uint32_t can_id;
    CAN_StatusTypeDef status = Hardware_CAN_Receive(&can_id, data, len);
	
    // 从CAN ID中提取PGN、DA、SA和优先级
    if (status == CAN_STATUS_OK) {
        *pgn = CAN_ID_TO_PGN(can_id);
        if (da != NULL) *da = CAN_ID_TO_DA(can_id);
        if (sa != NULL) *sa = CAN_ID_TO_SA(can_id);
        if (priority != NULL) *priority = CAN_ID_TO_PRIORITY(can_id);
    }
    
    return (Transport_StatusTypeDef)status;
}

/*
 * 发送多包消息
 * 参数:
 *   priority: 优先级 (0-7, 0为最高)
 *   pgn: 参数组编号
 *   da: 目标地址
 *   sa: 源地址
 *   data: 数据指针
 *   len: 数据长度
 */
/**
 * @brief 多包发送函数 - 实现GB/T 27930协议的多包数据传输
 * 采用J1939-21传输协议标准，通过RTS/CTS握手机制实现可靠数据传输
 * @param priority 消息优先级(0-7，0为最高)
 * @param pgn 参数组编号
 * @param da 目标地址
 * @param sa 源地址
 * @param data 待发送数据缓冲区
 * @param len 数据长度
 * @return 传输状态
 */
Transport_StatusTypeDef TransportLayer_Send_MultiPacket(uint8_t priority, uint32_t pgn, uint8_t da, uint8_t sa, uint8_t *data, uint8_t len) {
    if (data == NULL || len == 0) 
	{
        return TRANSPORT_STATUS_INVALID_PARAM;
    }

    // 计算数据包数量(向上取整)
    uint8_t packet_count = (len + TP_PACKET_DATA_SIZE - 1) / TP_PACKET_DATA_SIZE;

    // 发送RTS(请求发送)报文 - 启动多包传输握手
    if (send_tp_cm(priority, da, sa, 0x10, len, packet_count, pgn) != TRANSPORT_STATUS_OK) {
        ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_ERROR;
    }

    // 等待CTS(允许发送)报文 - 超时时间1秒
    uint32_t received_pgn;
    uint8_t received_data[TP_PACKET_SIZE];
    uint32_t start_time = Hardware_Get_Timestamp();
    uint32_t timeout = 1000; // 1秒超时时间
    
    // 循环等待CTS响应
    while ((Hardware_Get_Timestamp() - start_time) < timeout) {
        uint32_t received_can_id;
        uint8_t temp_len;
        // 接收CAN数据
        if (Hardware_CAN_Receive(&received_can_id, received_data, &temp_len) == CAN_STATUS_OK) {
            // 从CAN ID提取PGN
            received_pgn = CAN_ID_TO_PGN(received_can_id);
            // 检查是否为CTS报文(控制字节0x11)
            if (received_pgn == PGN_TP_CM && received_data[0] == 0x11) {
                break; // 收到CTS，退出等待
            }
        }
    }

    // 检查是否超时
    if ((Hardware_Get_Timestamp() - start_time) >= timeout) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
		ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_TIMEOUT;
    }

     // 发送数据包 - 按序号发送所有分包
    for (uint8_t i = 0; i < packet_count; i++) 
	{
        uint8_t packet_data[TP_PACKET_DATA_SIZE];
        // 计算当前包长度(最后一包可能不满)
        uint8_t packet_len = (len - i * TP_PACKET_DATA_SIZE) > TP_PACKET_DATA_SIZE ? TP_PACKET_DATA_SIZE : (len - i * TP_PACKET_DATA_SIZE);
        // 复制数据到发送缓冲区
        for (uint8_t j = 0; j < packet_len; j++) {
            packet_data[j] = data[i * TP_PACKET_DATA_SIZE + j];
        }
        // 发送数据传输报文(DT)
        if (send_tp_dt(7, da, sa, i+1, packet_data, packet_len) != TRANSPORT_STATUS_OK) {
            ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
			ErrorHandling_Error_Handler();
            return TRANSPORT_STATUS_ERROR;
        }
    }

    // 等待确认报文(END) - 超时时间1秒
    start_time = Hardware_Get_Timestamp();
    timeout = 1000; // 1秒超时时间
    
    while ((Hardware_Get_Timestamp() - start_time) < timeout) {
        uint32_t received_can_id;
        uint8_t temp_len;
        if (Hardware_CAN_Receive(&received_can_id, received_data, &temp_len) == CAN_STATUS_OK) {
            received_pgn = CAN_ID_TO_PGN(received_can_id);
            // 检查是否为END报文(控制字节0x13)
            if (received_pgn == PGN_TP_CM && received_data[0] == 0x13) 
						{
                return TRANSPORT_STATUS_OK; // 传输成功
            }
        }
    }

    // 超时处理
    if ((Hardware_Get_Timestamp() - start_time) >= timeout) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
				ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_TIMEOUT;
    }

    ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
    return TRANSPORT_STATUS_ERROR;
}

/*
 * 接收多包消息
 * 参数:
 *   pgn: 参数组编号指针
 *   data: 数据指针
 *   len: 数据长度指针
 *   da: 目标地址 (用于发送CTS和确认报文)
 *   sa: 源地址 (用于发送CTS和确认报文)
 *   priority: 优先级 (0-7, 0为最高，用于发送CTS和确认报文)6
 
 */
Transport_StatusTypeDef TransportLayer_Receive_MultiPacket(uint8_t priority, uint32_t *pgn, uint8_t *data, uint8_t *len, uint8_t da, uint8_t sa) {
    if (pgn == NULL || data == NULL || len == NULL) {
        return TRANSPORT_STATUS_INVALID_PARAM;
    }
    
    // 接收连接管理报文 - RTS
    uint32_t received_pgn;
    uint8_t received_data[TP_PACKET_SIZE];

    // 等待RTS报文，超时时间为1000ms
    uint32_t start_time = Hardware_Get_Timestamp();
    uint32_t timeout = 1000; // 1秒超时时间
    
    while ((Hardware_Get_Timestamp() - start_time) < timeout) {
        uint32_t received_can_id;
        uint8_t temp_len;
        if (Hardware_CAN_Receive(&received_can_id, received_data, &temp_len) == CAN_STATUS_OK) {
            // 从CAN ID中提取PGN
            received_pgn = CAN_ID_TO_PGN(received_can_id);
            // 检查是否为RTS报文(控制字节0x10)
            if (received_pgn == PGN_TP_CM && received_data[0] == 0x10) {
                break;
            }
        }
    }

    // 检查是否超时
    if ((Hardware_Get_Timestamp() - start_time) >= timeout) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
				ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_TIMEOUT;
    }

    // 解析RTS报文内容
    uint32_t tp_pgn = (received_data[7] << 16) | (received_data[6] << 8) | received_data[5];
    uint16_t tp_data_len = (received_data[2] << 8) | received_data[1];
    uint8_t packet_count = received_data[3];

    // 发送CTS(允许发送)报文 - 响应RTS请求
    if (send_tp_cm(priority, da, sa, 0x11, tp_data_len, packet_count, tp_pgn) != TRANSPORT_STATUS_OK) {
        ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_ERROR;
    }

    // 接收数据包 - 按序号接收所有分包
    for (uint8_t i = 0; i < packet_count; i++) {
        uint32_t dt_pgn;
        uint8_t dt_data[TP_PACKET_SIZE];
        uint8_t dt_len;
        timeout = 0;

        // 等待数据包，超时时间为1000ms
        uint32_t start_time = Hardware_Get_Timestamp();
        uint32_t timeout = 1000; // 1秒超时时间
        
        while ((Hardware_Get_Timestamp() - start_time) < timeout) {
            uint32_t dt_can_id;
            uint8_t temp_len;
            if (Hardware_CAN_Receive(&dt_can_id, dt_data, &temp_len) == CAN_STATUS_OK) {
                // 从CAN ID中提取PGN
                dt_pgn = CAN_ID_TO_PGN(dt_can_id);
                dt_len = temp_len;
                // 检查是否为数据传输报文且序号匹配
                if (dt_pgn == PGN_TP_DT && dt_data[0] == (i + 1)) {
                    break;
                }
            }
        }

        // 检查是否超时
        if ((Hardware_Get_Timestamp() - start_time) >= timeout) {
            ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
						ErrorHandling_Error_Handler();
            return TRANSPORT_STATUS_TIMEOUT;
        }

        // 拷贝数据(跳过序号字节)
        for (uint8_t j = 0; j < dt_len - 1; j++) {
            tp_dt_buffer[i * (TP_PACKET_SIZE - 1) + j] = dt_data[1 + j];
        }

        // 数据包之间接收间隔为10ms
        uint32_t delay_start = Hardware_Get_Timestamp();
        while ((Hardware_Get_Timestamp() - delay_start) < 10); // 10ms间隔
    }

    // 接收END报文 - 确认传输完成
    uint32_t end_start_time = Hardware_Get_Timestamp();
    uint32_t end_timeout = 1000; // 1秒超时时间
    
    while ((Hardware_Get_Timestamp() - end_start_time) < end_timeout) {
        uint32_t received_can_id;
        uint8_t temp_len;
        if (Hardware_CAN_Receive(&received_can_id, received_data, &temp_len) == CAN_STATUS_OK) {
            // 从CAN ID中提取PGN
            received_pgn = CAN_ID_TO_PGN(received_can_id);
            // 检查是否为END报文(控制字节0x13)
            if (received_pgn == PGN_TP_CM && received_data[0] == 0x13) {
                break;
            }
        }
    }

    // 检查是否超时
    if ((Hardware_Get_Timestamp() - end_start_time) >= end_timeout) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
		ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_TIMEOUT;
    }

    // 发送确认报文
    if (send_tp_cm(priority, da, sa, 0x13, tp_data_len, packet_count, tp_pgn) != TRANSPORT_STATUS_OK) {
        ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_ERROR;
    }

    // 返回数据给上层
    *pgn = tp_pgn;
    *len = tp_data_len;
    for (uint16_t i = 0; i < tp_data_len; i++) {
        data[i] = tp_dt_buffer[i];
    }

    return TRANSPORT_STATUS_OK;
}

/* 发送连接管理报文 */
/* RTS格式：
 *   data[0] - 控制字节（RTS/CTS/EndOfMsgACK等）
 *   data[1-2] - 总消息大小（低位在前）
 *   data[3] - 数据包数量
 *   data[4] - 保留位（0xFF）
 *   data[5-7] - 打包消息的PGN（低位在前）
 */
static Transport_StatusTypeDef send_tp_cm(uint8_t priority, uint8_t da, uint8_t sa, uint8_t control_byte, uint16_t total_size, uint8_t packet_count, uint32_t pgn) {
    uint8_t data[TP_PACKET_SIZE];
    
    data[0] = control_byte;
    data[1] = total_size & 0xFF;
    data[2] = (total_size >> 8) & 0xFF;
    data[3] = packet_count;
    data[4] = 0xFF; // 保留字段
    data[5] = 0x00;
    data[6] = pgn;
    data[7] = 0x00;
    
    // 构造CM报文的CAN ID
    uint32_t cm_can_id = PGN_TO_CAN_ID(priority, PGN_TP_CM, da, sa);
    
    return (Transport_StatusTypeDef)Hardware_CAN_Transmit(cm_can_id, data, TP_PACKET_SIZE);
}

/* 发送数据传输报文 */
static Transport_StatusTypeDef send_tp_dt(uint8_t priority, uint8_t da, uint8_t sa, uint8_t sequence, uint8_t *data, uint8_t len) {
    uint8_t packet_data[TP_PACKET_SIZE];
    
    packet_data[0] = sequence;
	//从1开始存储
    for (uint8_t i = 0; i < len; i++) {
        packet_data[i+1] = data[i];
    }
    
    // 如果数据不足7字节，填充0xFF
    for (uint8_t i = len + 1; i <= TP_PACKET_DATA_SIZE; i++) {
        packet_data[i] = 0xFF;
    }
    
    // 构造DT报文的CAN ID
    uint32_t dt_can_id = PGN_TO_CAN_ID(priority, PGN_TP_DT, da, sa);
    
    return (Transport_StatusTypeDef)Hardware_CAN_Transmit(dt_can_id, packet_data, TP_PACKET_SIZE);
}

/* 传输层错误处理 */
void TransportLayer_Error_Handler(void) {
    // 传输层错误处理逻辑
    // 可以在这里添加更详细的错误处理
    Hardware_CAN_Error_Handler();
    
    // 报告传输层错误
    ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
	ErrorHandling_Error_Handler();
}
