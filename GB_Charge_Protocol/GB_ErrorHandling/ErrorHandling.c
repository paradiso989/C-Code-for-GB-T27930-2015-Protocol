/*
 * ErrorHandling.c
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 *
 */

#include "ErrorHandling.h"


/* 全局变量 */
static Error_InfoTypeDef last_error;
static Error_InfoTypeDef error_log[ERROR_LOG_MAX_ENTRIES];
static int error_log_count = 0;
static bool error_occurred = false;

/* 错误处理层初始化 */
void ErrorHandling_Init(void) {
    // 初始化错误信息
    memset(&last_error, 0, sizeof(Error_InfoTypeDef));
    memset(error_log, 0, sizeof(error_log));
    error_log_count = 0;
    error_occurred = false;
}

/* 报告错误 */
void ErrorHandling_Report_Error(Error_TypeTypeDef type, Error_CodeTypeDef code, uint8_t *data, uint8_t len) {
    // 保存错误信息
    last_error.type = type;
    last_error.code = code;
    last_error.timestamp = ErrorHandling_Get_Timestamp();
    
    // 保存错误数据
    if (data != NULL && len > 0) {
        uint8_t copy_len = (len > 8) ? 8 : len;
        memcpy(last_error.data, data, copy_len);
        last_error.data_len = copy_len;
    } else {
        last_error.data_len = 0;
    }
    
    // 添加到错误日志
    if (error_log_count < ERROR_LOG_MAX_ENTRIES) {
        memcpy(&error_log[error_log_count], &last_error, sizeof(Error_InfoTypeDef));
        error_log_count++;
    } else {
        // 循环覆盖最早的错误日志
        for (int i = 0; i < ERROR_LOG_MAX_ENTRIES - 1; i++) {
            memcpy(&error_log[i], &error_log[i + 1], sizeof(Error_InfoTypeDef));
        }
        memcpy(&error_log[ERROR_LOG_MAX_ENTRIES - 1], &last_error, sizeof(Error_InfoTypeDef));
    }
    
    // 设置错误标志
    error_occurred = true;
}

/* 获取最后发生的错误 */
void ErrorHandling_Get_Last_Error(Error_InfoTypeDef *error_info) {
    if (error_info != NULL) {
        memcpy(error_info, &last_error, sizeof(Error_InfoTypeDef));
    }
}

/* 清除错误 */
void ErrorHandling_Clear_Error(void) {
    memset(&last_error, 0, sizeof(Error_InfoTypeDef));
    error_occurred = false;
}

/* 检查是否有错误 */
bool ErrorHandling_Has_Error(void) {
    return error_occurred;
}

/* 获取系统时间戳 */
uint32_t ErrorHandling_Get_Timestamp(void) {
    return Hardware_Get_Timestamp();
}

/* 打印错误日志 */
void ErrorHandling_Print_Error_Log(void) {
    printf("\n=== 错误日志 ===\n");
    for (int i = 0; i < error_log_count; i++) {
        printf("[%d] 时间戳: %lu, 类型: %d, 代码: %d\n", i+1, error_log[i].timestamp, error_log[i].type, error_log[i].code);
    }
    printf("===============\n\n");
}

/* 清除错误日志 */
void ErrorHandling_Clear_Error_Log(void) {
    memset(error_log, 0, sizeof(error_log));
    error_log_count = 0;
}

/* 获取错误日志数量 */
int ErrorHandling_Get_Error_Log_Count(void) {
    return error_log_count;
}

/* 错误处理函数 */
void ErrorHandling_Error_Handler(void) {
    // 根据错误类型调用相应的处理函数
    switch (last_error.type) {
        case ERROR_TYPE_HARDWARE:
            // 硬件错误处理
            printf("[错误处理] 硬件错误: 代码 %d\n", last_error.code);
            break;
        case ERROR_TYPE_TRANSPORT:
            // 传输层错误处理
            printf("[错误处理] 传输层错误: 代码 %d\n", last_error.code);
            break;
        case ERROR_TYPE_APPLICATION:
            // 应用层错误处理
            // 根据具体的错误代码进行处理
            switch (last_error.code) {
                case ERROR_CODE_APP_BRM_FAILED:
                    // BRM报文处理失败
                    printf("[错误处理] BRM报文处理失败，可能需要重新握手\n");
                    break;
                case ERROR_CODE_APP_BHM_FAILED:
                    // BHM报文处理失败
                    printf("[错误处理] BHM报文处理失败，可能需要重新握手\n");
                    break;
                case ERROR_CODE_APP_BCP_FAILED:
                    // BCP报文处理失败
                    printf("[错误处理] BCP报文处理失败，可能需要重新配置参数\n");
                    break;
                case ERROR_CODE_APP_BRO_FAILED:
                    // BRO报文处理失败
                    printf("[错误处理] BRO报文处理失败，可能需要重新确认准备状态\n");
                    break;
                case ERROR_CODE_APP_BCL_FAILED:
                    // BCL报文处理失败
                    printf("[错误处理] BCL报文处理失败，可能需要重新发送充电参数\n");
                    break;
                case ERROR_CODE_APP_BSM_FAILED:
                    // BSM报文处理失败
                    printf("[错误处理] BSM报文处理失败，可能需要重新发送电池状态\n");
                    break;
                case ERROR_CODE_APP_BCS_FAILED:
                    // BCS报文处理失败
                    printf("[错误处理] BCS报文处理失败，可能需要重新发送充电状态\n");
                    break;
                case ERROR_CODE_APP_BST_FAILED:
                    // BST报文处理失败
                    printf("[错误处理] BST报文处理失败，可能需要重新发送停止充电命令\n");
                    break;
                case ERROR_CODE_APP_BMT_FAILED:
                    // BMT报文处理失败
                    printf("[错误处理] BMT报文处理失败，可能需要重新发送温度数据\n");
                    break;
                case ERROR_CODE_APP_BMV_FAILED:
                    // BMV报文处理失败
                    printf("[错误处理] BMV报文处理失败，可能需要重新发送电压数据\n");
                    break;
                case ERROR_CODE_APP_BSD_FAILED:
                    // BSD报文处理失败
                    printf("[错误处理] BSD报文处理失败，可能需要重新发送统计数据\n");
                    break;
                case ERROR_CODE_APP_BEM_FAILED:
                    // BEM报文处理失败
                    printf("[错误处理] BEM报文处理失败，可能需要重新发送错误信息\n");
                    break;
                case ERROR_CODE_APP_CRM_FAILED:
                    // CRM报文处理失败
                    printf("[错误处理] CRM报文处理失败，可能需要重新发送确认报文\n");
                    break;
                case ERROR_CODE_APP_CHM_FAILED:
                    // CHM报文处理失败
                    printf("[错误处理] CHM报文处理失败，可能需要重新发送握手报文\n");
                    break;
                case ERROR_CODE_APP_CML_FAILED:
                    // CML报文处理失败
                    printf("[错误处理] CML报文处理失败，可能需要重新发送最大输出能力\n");
                    break;
                case ERROR_CODE_APP_CTS_FAILED:
                    // CTS报文处理失败
                    printf("[错误处理] CTS报文处理失败，可能需要重新发送时间同步\n");
                    break;
                case ERROR_CODE_APP_CRO_FAILED:
                    // CRO报文处理失败
                    printf("[错误处理] CRO报文处理失败，可能需要重新确认准备状态\n");
                    break;
                case ERROR_CODE_APP_CST_FAILED:
                    // CST报文处理失败
                    printf("[错误处理] CST报文处理失败，可能需要重新发送停止充电命令\n");
                    break;
                case ERROR_CODE_APP_CCS_FAILED:
                    // CCS报文处理失败
                    printf("[错误处理] CCS报文处理失败，可能需要重新发送充电状态\n");
                    break;
                case ERROR_CODE_APP_CSD_FAILED:
                    // CSD报文处理失败
                    printf("[错误处理] CSD报文处理失败，可能需要重新发送统计数据\n");
                    break;
                case ERROR_CODE_APP_CEM_FAILED:
                    // CEM报文处理失败
                    printf("[错误处理] CEM报文处理失败，可能需要重新发送错误信息\n");
                    break;
                default:
                    // 其他应用层错误
                    printf("[错误处理] 未知应用层错误，代码: %d\n", last_error.code);
                    break;
            }
            break;
        case ERROR_TYPE_TIMEOUT:
            // 超时错误处理
            printf("[错误处理] 超时错误\n");
            break;
        case ERROR_TYPE_INVALID_PARAM:
            // 参数错误处理
            printf("[错误处理] 参数错误\n");
            break;
        case ERROR_TYPE_COMMUNICATION:
            // 通信错误处理
            printf("[错误处理] 通信错误\n");
            break;
        default:
            // 未知错误类型
            printf("[错误处理] 未知错误类型: %d\n", last_error.type);
            break;
    }
    
    // 清除错误
    //ErrorHandling_Clear_Error();
}
