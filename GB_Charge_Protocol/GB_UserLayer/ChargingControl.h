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

/* 充电阶段枚举 */
typedef enum {
    CHARGING_STAGE_HANDSHAKE = 0,     /* 握手阶段 */
    CHARGING_STAGE_PARAMETER,         /* 参数配置阶段 */
    CHARGING_STAGE_CHARGING,          /* 充电阶段 */
    CHARGING_STAGE_END                /* 结束阶段 */
} Charging_StageTypeDef;

/* 充电控制结构体 */
typedef struct {
    Charging_StageTypeDef current_stage;  /* 当前充电阶段 */
    bool charging_allowed;                /* 是否允许充电 */
    uint32_t stage_timeout;               /* 阶段超时时间 (ms) */
    uint32_t timestamp;                   /* 时间戳 */
} Charging_ControlTypeDef;

/* 充电参数结构体 */
typedef struct {
    /* 握手阶段参数 */
    GBT_BHM_Data bhm_data;
    GBT_CHM_Data chm_data;
    GBT_BRM_Data brm_data;
    GBT_CRM_Data crm_data;
    
    /* 参数配置阶段参数 */
    GBT_BCP_Data bcp_data;
    GBT_CML_Data cml_data;
    GBT_CTS_Data cts_data;
    GBT_BRO_Data bro_data;
    GBT_CRO_Data cro_data;
    
    /* 充电阶段参数 */
    GBT_BCL_Data bcl_data;
    GBT_BCS_Data bcs_data;
    GBT_BSM_Data bsm_data;
    GBT_CCS_Data ccs_data;
    GBT_BST_Data bst_data;
    GBT_CST_Data cst_data;
    
    /* 结束阶段参数 */
    GBT_BSD_Data bsd_data;
    GBT_CSD_Data csd_data;
    GBT_BEM_Data bem_data;
    GBT_CEM_Data cem_data;
    
    /* 数据报文参数 */
    GBT_BMT_Data bmt_data;
    GBT_BMV_Data bmv_data;
} Charging_ParametersTypeDef;

/* 全局变量声明 */
extern Charging_ControlTypeDef charging_control;
extern Charging_ParametersTypeDef charging_params;

/* 函数声明 */
void ChargingControl_Init(void);
Charging_StageTypeDef ChargingControl_Get_Charging_Stage(void);
void ChargingControl_Set_Charging_Stage(Charging_StageTypeDef stage);
void ChargingControl_Process(void);

/* 时间获取函数 */
#define get_current_timestamp() Hardware_Get_Timestamp()

/* 各阶段处理函数 */
void ChargingControl_Handshake_Stage(void);
void ChargingControl_Parameter_Stage(void);
void ChargingControl_Charging_Stage(void);
void ChargingControl_End_Stage(void);

#endif /* GB_USERLAYER_CHARGINGCONTROL_H_ */
