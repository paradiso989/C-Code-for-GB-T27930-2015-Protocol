/*
 * ChargingControl.h
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 * 
 */

#ifndef __CHARGINGCONTROL_H_
#define __CHARGINGCONTROL_H_

#include "ApplicationLayer.h"
#include "ErrorHandling.h"
#include "Hardware.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>  
#include <stdio.h>   

/* ���׶�ö�� */
typedef enum {
    CHARGING_STAGE_HANDSHAKE = 0,     /* ���ֽ׶� */
    CHARGING_STAGE_PARAMETER,         /* �������ý׶� */
    CHARGING_STAGE_CHARGING,          /* ���׶� */
    CHARGING_STAGE_END                /* �����׶� */
} Charging_StageTypeDef;

/* �����ƽṹ�� */
typedef struct {
    Charging_StageTypeDef current_stage;  /* ��ǰ���׶� */
    bool charging_allowed;                /* �Ƿ������� */
    uint32_t stage_timeout;               /* �׶γ�ʱʱ�� (ms) */
    uint32_t timestamp;                   /* ʱ��� */
} Charging_ControlTypeDef;

/* �������ṹ�� */
typedef struct {
    /* ���ֽ׶β��� */
    GBT_BHM_Data bhm_data;
    GBT_CHM_Data chm_data;
    GBT_BRM_Data brm_data;
    GBT_CRM_Data crm_data;
    
    /* �������ý׶β��� */
    GBT_BCP_Data bcp_data;
    GBT_CML_Data cml_data;
    GBT_CTS_Data cts_data;
    GBT_BRO_Data bro_data;
    GBT_CRO_Data cro_data;
    
    /* ���׶β��� */
    GBT_BCL_Data bcl_data;
    GBT_BCS_Data bcs_data;
    GBT_BSM_Data bsm_data;
    GBT_CCS_Data ccs_data;
    GBT_BST_Data bst_data;
    GBT_CST_Data cst_data;
    
    /* �����׶β��� */
    GBT_BSD_Data bsd_data;
    GBT_CSD_Data csd_data;
    GBT_BEM_Data bem_data;
    GBT_CEM_Data cem_data;
    
    /* ���ݱ��Ĳ��� */
    GBT_BMT_Data bmt_data;
    GBT_BMV_Data bmv_data;
} Charging_ParametersTypeDef;

/* ȫ�ֱ������� */
extern Charging_ControlTypeDef charging_control;
extern Charging_ParametersTypeDef charging_params;

/* �������� */
void ChargingControl_Init(void);
Charging_StageTypeDef ChargingControl_Get_Charging_Stage(void);
void ChargingControl_Set_Charging_Stage(Charging_StageTypeDef stage);
void ChargingControl_Process(void);

/* ʱ���ȡ���� */
#define get_current_timestamp() Hardware_Get_Timestamp()

/* ���׶δ����� */
void ChargingControl_Handshake_Stage(void);
void ChargingControl_Parameter_Stage(void);
void ChargingControl_Charging_Stage(void);
void ChargingControl_End_Stage(void);

#endif /* GB_USERLAYER_CHARGINGCONTROL_H_ */
