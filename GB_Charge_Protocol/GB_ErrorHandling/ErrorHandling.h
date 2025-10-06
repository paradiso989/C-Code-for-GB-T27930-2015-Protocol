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

/* �������Ͷ��� */
typedef enum {
    ERROR_TYPE_NONE = 0,              /* �޴��� */
    ERROR_TYPE_HARDWARE,              /* Ӳ������ */
    ERROR_TYPE_TRANSPORT,             /* �������� */
    ERROR_TYPE_APPLICATION,           /* Ӧ�ò���� */
    ERROR_TYPE_TIMEOUT,               /* ��ʱ���� */
    ERROR_TYPE_INVALID_PARAM,         /* �������� */
    ERROR_TYPE_COMMUNICATION          /* ͨ�Ŵ��� */
} Error_TypeTypeDef;

/* ������붨�� */
typedef enum {
    ERROR_CODE_NONE = 0,              /* �޴��� */
    ERROR_CODE_CAN_INIT_FAILED,       /* CAN��ʼ��ʧ�� */
    ERROR_CODE_CAN_TRANSMIT_FAILED,   /* CAN����ʧ�� */
    ERROR_CODE_CAN_RECEIVE_FAILED,    /* CAN����ʧ�� */
    ERROR_CODE_CAN_BUS_OFF,           /* CAN���߹ر� */
    ERROR_CODE_TP_TRANSMIT_FAILED,    /* ����㷢��ʧ�� */
    ERROR_CODE_TP_RECEIVE_FAILED,     /* ��������ʧ�� */
    ERROR_CODE_TP_TIMEOUT,            /* ����㳬ʱ */
    ERROR_CODE_APP_INVALID_STAGE,     /* Ӧ�ò���Ч�׶� */
    ERROR_CODE_APP_MESSAGE_PROCESS_FAILED, /* Ӧ�ò���Ϣ����ʧ�� */
    ERROR_CODE_COMM_TIMEOUT,          /* ͨ�ų�ʱ */
    
    /* Ӧ�ò㱨����ش������ */
    ERROR_CODE_APP_BRM_FAILED,        /* BRM���Ĵ���ʧ�� */
    ERROR_CODE_APP_BHM_FAILED,        /* BHM���Ĵ���ʧ�� */
    ERROR_CODE_APP_BCP_FAILED,        /* BCP���Ĵ���ʧ�� */
    ERROR_CODE_APP_BRO_FAILED,        /* BRO���Ĵ���ʧ�� */
    ERROR_CODE_APP_BCL_FAILED,        /* BCL���Ĵ���ʧ�� */
    ERROR_CODE_APP_BSM_FAILED,        /* BSM���Ĵ���ʧ�� */
    ERROR_CODE_APP_BCS_FAILED,        /* BCS���Ĵ���ʧ�� */
    ERROR_CODE_APP_BST_FAILED,        /* BST���Ĵ���ʧ�� */
    ERROR_CODE_APP_BMT_FAILED,        /* BMT���Ĵ���ʧ�� */
    ERROR_CODE_APP_BMV_FAILED,        /* BMV���Ĵ���ʧ�� */
    ERROR_CODE_APP_BSD_FAILED,        /* BSD���Ĵ���ʧ�� */
    ERROR_CODE_APP_BEM_FAILED,        /* BEM���Ĵ���ʧ�� */
    ERROR_CODE_APP_CRM_FAILED,        /* CRM���Ĵ���ʧ�� */
    ERROR_CODE_APP_CHM_FAILED,        /* CHM���Ĵ���ʧ�� */
    ERROR_CODE_APP_CML_FAILED,        /* CML���Ĵ���ʧ�� */
    ERROR_CODE_APP_CTS_FAILED,        /* CTS���Ĵ���ʧ�� */
    ERROR_CODE_APP_CRO_FAILED,        /* CRO���Ĵ���ʧ�� */
    ERROR_CODE_APP_CST_FAILED,        /* CST���Ĵ���ʧ�� */
    ERROR_CODE_APP_CCS_FAILED,        /* CCS���Ĵ���ʧ�� */
    ERROR_CODE_APP_CSD_FAILED,        /* CSD���Ĵ���ʧ�� */
    ERROR_CODE_APP_CEM_FAILED         /* CEM���Ĵ���ʧ�� */
} Error_CodeTypeDef;

/* ������Ϣ�ṹ�� */
typedef struct {
    Error_TypeTypeDef type;           /* �������� */
    Error_CodeTypeDef code;           /* ������� */
    uint32_t timestamp;               /* ʱ��� */
    uint8_t data[8];                  /* ����������� */
    uint8_t data_len;                 /* ���ݳ��� */
} Error_InfoTypeDef;

/* ������־������� */
#define ERROR_LOG_MAX_ENTRIES 50

/* �������� */
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
