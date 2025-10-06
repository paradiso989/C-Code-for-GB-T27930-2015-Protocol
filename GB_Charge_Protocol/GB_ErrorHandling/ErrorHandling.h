/*
 * ErrorHandling.h
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 *
 */

#ifndef __ERRORHANDLING_H_
#define __ERRORHANDLING_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include "Hardware.h"

/* 错误类型定义 */
typedef enum {
    ERROR_TYPE_NONE = 0,              /* 无错误 */
    ERROR_TYPE_HARDWARE,              /* 硬件错误 */
    ERROR_TYPE_TRANSPORT,             /* 传输层错误 */
    ERROR_TYPE_APPLICATION,           /* 应用层错误 */
    ERROR_TYPE_TIMEOUT,               /* 超时错误 */
    ERROR_TYPE_INVALID_PARAM,         /* 参数错误 */
    ERROR_TYPE_COMMUNICATION          /* 通信错误 */
} Error_TypeTypeDef;

/* 错误代码定义 */
typedef enum {
    ERROR_CODE_NONE = 0,              /* 无错误 */
    ERROR_CODE_CAN_INIT_FAILED,       /* CAN初始化失败 */
    ERROR_CODE_CAN_TRANSMIT_FAILED,   /* CAN发送失败 */
    ERROR_CODE_CAN_RECEIVE_FAILED,    /* CAN接收失败 */
    ERROR_CODE_CAN_BUS_OFF,           /* CAN总线关闭 */
    ERROR_CODE_TP_TRANSMIT_FAILED,    /* 传输层发送失败 */
    ERROR_CODE_TP_RECEIVE_FAILED,     /* 传输层接收失败 */
    ERROR_CODE_TP_TIMEOUT,            /* 传输层超时 */
    ERROR_CODE_APP_INVALID_STAGE,     /* 应用层无效阶段 */
    ERROR_CODE_APP_MESSAGE_PROCESS_FAILED, /* 应用层消息处理失败 */
    ERROR_CODE_COMM_TIMEOUT,          /* 通信超时 */
    
    /* 应用层报文相关错误代码 */
    ERROR_CODE_APP_BRM_FAILED,        /* BRM报文处理失败 */
    ERROR_CODE_APP_BHM_FAILED,        /* BHM报文处理失败 */
    ERROR_CODE_APP_BCP_FAILED,        /* BCP报文处理失败 */
    ERROR_CODE_APP_BRO_FAILED,        /* BRO报文处理失败 */
    ERROR_CODE_APP_BCL_FAILED,        /* BCL报文处理失败 */
    ERROR_CODE_APP_BSM_FAILED,        /* BSM报文处理失败 */
    ERROR_CODE_APP_BCS_FAILED,        /* BCS报文处理失败 */
    ERROR_CODE_APP_BST_FAILED,        /* BST报文处理失败 */
    ERROR_CODE_APP_BMT_FAILED,        /* BMT报文处理失败 */
    ERROR_CODE_APP_BMV_FAILED,        /* BMV报文处理失败 */
    ERROR_CODE_APP_BSD_FAILED,        /* BSD报文处理失败 */
    ERROR_CODE_APP_BEM_FAILED,        /* BEM报文处理失败 */
    ERROR_CODE_APP_CRM_FAILED,        /* CRM报文处理失败 */
    ERROR_CODE_APP_CHM_FAILED,        /* CHM报文处理失败 */
    ERROR_CODE_APP_CML_FAILED,        /* CML报文处理失败 */
    ERROR_CODE_APP_CTS_FAILED,        /* CTS报文处理失败 */
    ERROR_CODE_APP_CRO_FAILED,        /* CRO报文处理失败 */
    ERROR_CODE_APP_CST_FAILED,        /* CST报文处理失败 */
    ERROR_CODE_APP_CCS_FAILED,        /* CCS报文处理失败 */
    ERROR_CODE_APP_CSD_FAILED,        /* CSD报文处理失败 */
    ERROR_CODE_APP_CEM_FAILED         /* CEM报文处理失败 */
} Error_CodeTypeDef;

/* 错误信息结构体 */
typedef struct {
    Error_TypeTypeDef type;           /* 错误类型 */
    Error_CodeTypeDef code;           /* 错误代码 */
    uint32_t timestamp;               /* 时间戳 */
    uint8_t data[8];                  /* 错误相关数据 */
    uint8_t data_len;                 /* 数据长度 */
} Error_InfoTypeDef;

/* 错误日志最大条数 */
#define ERROR_LOG_MAX_ENTRIES 50

/* 函数声明 */
void ErrorHandling_Init(void);
void ErrorHandling_Report_Error(Error_TypeTypeDef type, Error_CodeTypeDef code, uint8_t *data, uint8_t len);
void ErrorHandling_Get_Last_Error(Error_InfoTypeDef *error_info);
void ErrorHandling_Clear_Error(void);
bool ErrorHandling_Has_Error(void);
void ErrorHandling_Error_Handler(void);
uint32_t ErrorHandling_Get_Timestamp(void);
void ErrorHandling_Print_Error_Log(void);
void ErrorHandling_Clear_Error_Log(void);
int ErrorHandling_Get_Error_Log_Count(void);

#endif /* GB_CHARGING_PROTOCOL_ERRORHANDLING_ERRORHANDLING_H_ */
