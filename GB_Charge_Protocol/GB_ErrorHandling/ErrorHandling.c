/*
 * ErrorHandling.c
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 *
 */

#include "ErrorHandling.h"


/* ȫ�ֱ��� */
static Error_InfoTypeDef last_error;
static Error_InfoTypeDef error_log[ERROR_LOG_MAX_ENTRIES];
static int error_log_count = 0;
static bool error_occurred = false;

/* ��������ʼ�� */
void ErrorHandling_Init(void) {
    // ��ʼ��������Ϣ
    memset(&last_error, 0, sizeof(Error_InfoTypeDef));
    memset(error_log, 0, sizeof(error_log));
    error_log_count = 0;
    error_occurred = false;
}

/* ������� */
void ErrorHandling_Report_Error(Error_TypeTypeDef type, Error_CodeTypeDef code, uint8_t *data, uint8_t len) {
    // ���������Ϣ
    last_error.type = type;
    last_error.code = code;
    last_error.timestamp = ErrorHandling_Get_Timestamp();
    
    // �����������
    if (data != NULL && len > 0) {
        uint8_t copy_len = (len > 8) ? 8 : len;
        memcpy(last_error.data, data, copy_len);
        last_error.data_len = copy_len;
    } else {
        last_error.data_len = 0;
    }
    
    // ��ӵ�������־
    if (error_log_count < ERROR_LOG_MAX_ENTRIES) {
        memcpy(&error_log[error_log_count], &last_error, sizeof(Error_InfoTypeDef));
        error_log_count++;
    } else {
        // ѭ����������Ĵ�����־
        for (int i = 0; i < ERROR_LOG_MAX_ENTRIES - 1; i++) {
            memcpy(&error_log[i], &error_log[i + 1], sizeof(Error_InfoTypeDef));
        }
        memcpy(&error_log[ERROR_LOG_MAX_ENTRIES - 1], &last_error, sizeof(Error_InfoTypeDef));
    }
    
    // ���ô����־
    error_occurred = true;
}

/* ��ȡ������Ĵ��� */
void ErrorHandling_Get_Last_Error(Error_InfoTypeDef *error_info) {
    if (error_info != NULL) {
        memcpy(error_info, &last_error, sizeof(Error_InfoTypeDef));
    }
}

/* ������� */
void ErrorHandling_Clear_Error(void) {
    memset(&last_error, 0, sizeof(Error_InfoTypeDef));
    error_occurred = false;
}

/* ����Ƿ��д��� */
bool ErrorHandling_Has_Error(void) {
    return error_occurred;
}

/* ��ȡϵͳʱ��� */
uint32_t ErrorHandling_Get_Timestamp(void) {
    return Hardware_Get_Timestamp();
}

/* ��ӡ������־ */
void ErrorHandling_Print_Error_Log(void) {
    printf("\n=== ������־ ===\n");
    for (int i = 0; i < error_log_count; i++) {
        printf("[%d] ʱ���: %lu, ����: %d, ����: %d\n", i+1, error_log[i].timestamp, error_log[i].type, error_log[i].code);
    }
    printf("===============\n\n");
}

/* ���������־ */
void ErrorHandling_Clear_Error_Log(void) {
    memset(error_log, 0, sizeof(error_log));
    error_log_count = 0;
}

/* ��ȡ������־���� */
int ErrorHandling_Get_Error_Log_Count(void) {
    return error_log_count;
}

/* �������� */
void ErrorHandling_Error_Handler(void) {
    // ���ݴ������͵�����Ӧ�Ĵ�����
    switch (last_error.type) {
        case ERROR_TYPE_HARDWARE:
            // Ӳ��������
            printf("[������] Ӳ������: ���� %d\n", last_error.code);
            break;
        case ERROR_TYPE_TRANSPORT:
            // ����������
            printf("[������] ��������: ���� %d\n", last_error.code);
            break;
        case ERROR_TYPE_APPLICATION:
            // Ӧ�ò������
            // ���ݾ���Ĵ��������д���
            switch (last_error.code) {
                case ERROR_CODE_APP_BRM_FAILED:
                    // BRM���Ĵ���ʧ��
                    printf("[������] BRM���Ĵ���ʧ�ܣ�������Ҫ��������\n");
                    break;
                case ERROR_CODE_APP_BHM_FAILED:
                    // BHM���Ĵ���ʧ��
                    printf("[������] BHM���Ĵ���ʧ�ܣ�������Ҫ��������\n");
                    break;
                case ERROR_CODE_APP_BCP_FAILED:
                    // BCP���Ĵ���ʧ��
                    printf("[������] BCP���Ĵ���ʧ�ܣ�������Ҫ�������ò���\n");
                    break;
                case ERROR_CODE_APP_BRO_FAILED:
                    // BRO���Ĵ���ʧ��
                    printf("[������] BRO���Ĵ���ʧ�ܣ�������Ҫ����ȷ��׼��״̬\n");
                    break;
                case ERROR_CODE_APP_BCL_FAILED:
                    // BCL���Ĵ���ʧ��
                    printf("[������] BCL���Ĵ���ʧ�ܣ�������Ҫ���·��ͳ�����\n");
                    break;
                case ERROR_CODE_APP_BSM_FAILED:
                    // BSM���Ĵ���ʧ��
                    printf("[������] BSM���Ĵ���ʧ�ܣ�������Ҫ���·��͵��״̬\n");
                    break;
                case ERROR_CODE_APP_BCS_FAILED:
                    // BCS���Ĵ���ʧ��
                    printf("[������] BCS���Ĵ���ʧ�ܣ�������Ҫ���·��ͳ��״̬\n");
                    break;
                case ERROR_CODE_APP_BST_FAILED:
                    // BST���Ĵ���ʧ��
                    printf("[������] BST���Ĵ���ʧ�ܣ�������Ҫ���·���ֹͣ�������\n");
                    break;
                case ERROR_CODE_APP_BMT_FAILED:
                    // BMT���Ĵ���ʧ��
                    printf("[������] BMT���Ĵ���ʧ�ܣ�������Ҫ���·����¶�����\n");
                    break;
                case ERROR_CODE_APP_BMV_FAILED:
                    // BMV���Ĵ���ʧ��
                    printf("[������] BMV���Ĵ���ʧ�ܣ�������Ҫ���·��͵�ѹ����\n");
                    break;
                case ERROR_CODE_APP_BSD_FAILED:
                    // BSD���Ĵ���ʧ��
                    printf("[������] BSD���Ĵ���ʧ�ܣ�������Ҫ���·���ͳ������\n");
                    break;
                case ERROR_CODE_APP_BEM_FAILED:
                    // BEM���Ĵ���ʧ��
                    printf("[������] BEM���Ĵ���ʧ�ܣ�������Ҫ���·��ʹ�����Ϣ\n");
                    break;
                case ERROR_CODE_APP_CRM_FAILED:
                    // CRM���Ĵ���ʧ��
                    printf("[������] CRM���Ĵ���ʧ�ܣ�������Ҫ���·���ȷ�ϱ���\n");
                    break;
                case ERROR_CODE_APP_CHM_FAILED:
                    // CHM���Ĵ���ʧ��
                    printf("[������] CHM���Ĵ���ʧ�ܣ�������Ҫ���·������ֱ���\n");
                    break;
                case ERROR_CODE_APP_CML_FAILED:
                    // CML���Ĵ���ʧ��
                    printf("[������] CML���Ĵ���ʧ�ܣ�������Ҫ���·�������������\n");
                    break;
                case ERROR_CODE_APP_CTS_FAILED:
                    // CTS���Ĵ���ʧ��
                    printf("[������] CTS���Ĵ���ʧ�ܣ�������Ҫ���·���ʱ��ͬ��\n");
                    break;
                case ERROR_CODE_APP_CRO_FAILED:
                    // CRO���Ĵ���ʧ��
                    printf("[������] CRO���Ĵ���ʧ�ܣ�������Ҫ����ȷ��׼��״̬\n");
                    break;
                case ERROR_CODE_APP_CST_FAILED:
                    // CST���Ĵ���ʧ��
                    printf("[������] CST���Ĵ���ʧ�ܣ�������Ҫ���·���ֹͣ�������\n");
                    break;
                case ERROR_CODE_APP_CCS_FAILED:
                    // CCS���Ĵ���ʧ��
                    printf("[������] CCS���Ĵ���ʧ�ܣ�������Ҫ���·��ͳ��״̬\n");
                    break;
                case ERROR_CODE_APP_CSD_FAILED:
                    // CSD���Ĵ���ʧ��
                    printf("[������] CSD���Ĵ���ʧ�ܣ�������Ҫ���·���ͳ������\n");
                    break;
                case ERROR_CODE_APP_CEM_FAILED:
                    // CEM���Ĵ���ʧ��
                    printf("[������] CEM���Ĵ���ʧ�ܣ�������Ҫ���·��ʹ�����Ϣ\n");
                    break;
                default:
                    // ����Ӧ�ò����
                    printf("[������] δ֪Ӧ�ò���󣬴���: %d\n", last_error.code);
                    break;
            }
            break;
        case ERROR_TYPE_TIMEOUT:
            // ��ʱ������
            printf("[������] ��ʱ����\n");
            break;
        case ERROR_TYPE_INVALID_PARAM:
            // ����������
            printf("[������] ��������\n");
            break;
        case ERROR_TYPE_COMMUNICATION:
            // ͨ�Ŵ�����
            printf("[������] ͨ�Ŵ���\n");
            break;
        default:
            // δ֪��������
            printf("[������] δ֪��������: %d\n", last_error.type);
            break;
    }
    
    // �������
    //ErrorHandling_Clear_Error();
}
