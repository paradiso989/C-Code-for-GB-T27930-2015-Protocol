/*
 * TransportLayer.h
 *
 * Created on: 2025.7.24
 * Author: 83923
 *
 * Based On Open_SAE_J1939
 * https://github.com/DanielMartensson/Open-SAE-J1939
 */

#ifndef __TRANSPORTLAYER_H_
#define __TRANSPORTLAYER_H_

#include <stdint.h>
#include <stdbool.h>
#include "Hardware.h"
#include "ErrorHandling.h"
#include <string.h>

/* 传输层状态码 */
typedef enum {
    TRANSPORT_STATUS_OK = 0,
    TRANSPORT_STATUS_ERROR,
    TRANSPORT_STATUS_BUSY,
    TRANSPORT_STATUS_TIMEOUT,
    TRANSPORT_STATUS_INVALID_PARAM
} Transport_StatusTypeDef;

/* 单包传输相关定义 */
#define TP_PACKET_SIZE	8

/* 多包传输相关定义 */
#define TP_MAX_PACKET_SIZE     10      /* 最大多包数据长度 */
#define TP_PACKET_DATA_SIZE         7         /* 单包数据长度(第一字节是包序号,所以每包最多7个字节的数据) */

/* PGN定义 */
#define PGN_TP_CM              0xEC /* 传输协议连接管理PGN */
#define PGN_TP_DT              0xEB /* 传输协议数据传输PGN */

/* PGN到CAN ID的转换宏 (符合GB/T 27930协议) */
#define R 0x00         /* 保留位(1位): 0 */
#define DP 0x00        /* 数据页(1位): 0 */
#define PGN_TO_CAN_ID(priority, pgn, da, sa) ( ((priority) << 26) | (R << 25) | (DP << 24) | (pgn << 16) | (da << 8) | sa )

/* 参数说明: 
 * priority: 优先级(3位): 0-7, 0为最高
 * pgn: 参数组编号 (24位)
 * da: 目标地址 (8位)
 * sa: 源地址 (8位)
 */
#define CAN_ID_TO_PGN(can_id)      (((can_id) >> 16) & 0xFF)
#define CAN_ID_TO_DA(can_id)       (((can_id) >> 8) & 0xFF)
#define CAN_ID_TO_SA(can_id)       ((can_id) & 0xFF)
#define CAN_ID_TO_PRIORITY(can_id) (((can_id) >> 26) & 0x07)


/* 函数声明 */
void TransportLayer_Init(void);
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
Transport_StatusTypeDef TransportLayer_Send_Message(uint8_t priority, uint32_t pgn, uint8_t da, uint8_t sa, uint8_t *data, uint8_t len);
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
Transport_StatusTypeDef TransportLayer_Receive_Message(uint32_t *pgn, uint8_t *data, uint8_t *len, uint8_t *da, uint8_t *sa, uint8_t *priority);
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
Transport_StatusTypeDef TransportLayer_Send_MultiPacket(uint8_t priority, uint32_t pgn, uint8_t da, uint8_t sa, uint8_t *data, uint8_t len);
/*
 * 接收多包消息
 * 参数:
 *   priority: 优先级 (0-7, 0为最高)
 *   pgn: 参数组编号指针
 *   data: 数据指针
 *   len: 数据长度指针
 *   da: 目标地址 (用于发送CTS和确认报文)
 *   sa: 源地址 (用于发送CTS和确认报文)
 */
Transport_StatusTypeDef TransportLayer_Receive_MultiPacket(uint8_t priority, uint32_t *pgn, uint8_t *data, uint8_t *len, uint8_t da, uint8_t sa);
void TransportLayer_Error_Handler(void);

#endif /* GB_CHARGING_PROTOCOL_TRANSPORTLAYER_TRANSPORTLAYER_H_ */
