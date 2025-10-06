/*
 * ApplicationLayer.h
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 * 
 */

#ifndef __APPLICATIONLAYER_H_
#define __APPLICATIONLAYER_H_

#include "TransportLayer.h"
#include <string.h>  // For memcpy
#include "ErrorHandling.h"

/*
 * GB/T 27930-2015 PGNs
 * 定义了标准中规定的重要PGN
 */

/************** BMS -> Charger **************/
/* 充电握手报文 */
#define PGN_BRM                       0x02  /* BRM - BMS辨识报文 (BMS -> 充电机) */
#define PGN_BHM                       0x27  /* BHM - BMS握手报文 (BMS -> 充电机) */

/* 充电参数配置报文 */
#define PGN_BCP                       0x06  /* BCP - BMS充电参数报文 (BMS -> 充电机) */
#define PGN_BRO                       0x09  /* BRO - BMS充电准备就绪报文 (BMS -> 充电机) */

/* 充电阶段报文 */
#define PGN_BCL                       0x10  /* BCL - BMS充电需求报文 (BMS -> 充电机) */ 
#define PGN_BSM                       0x13  /* BSM - BMS状态信息报文 (BMS -> 充电机) */
#define PGN_BCS                       0x11  /* BCS - BMS总充电状态报文 (BMS -> 充电机) */
#define PGN_BST                       0x19  /* BST - BMS停止充电报文 (BMS -> 充电机) */
#define PGN_BMT                       0x16  /* BMT - BMS电池温度报文 (BMS -> 充电机) */
#define PGN_BMV                       0x15  /* BMV - BMS电池电压报文 (BMS -> 充电机) */

/* 充电结束后报文 */
#define PGN_BSD                       0x1C  /* BSD - BMS统计数据报文 (BMS -> 充电机) */

/* 充电错误报文 */
#define PGN_BEM                       0x1E  /* BEM - BMS错误报文 (BMS -> 充电机) */

/************** Charger -> BMS **************/
/* 充电握手报文 */
#define PGN_CRM                       0x01  /* CRM - 充电机辨识报文 (充电机 -> BMS) */
#define PGN_CHM                       0x26  /* CHM - 充电机握手报文 (充电机 -> BMS) */

/* 充电参数配置报文 */
#define PGN_CML                       0x08  /* CML - 充电机最大输出能力报文 (充电机 -> BMS) */
#define PGN_CTS                       0x07  /* CTS - 充电机时间同步报文 (充电机 -> BMS) */
#define PGN_CRO                       0x0A  /* CRO - 充电机充电准备就绪报文 (充电机 -> BMS) */

/* 充电阶段报文 */
#define PGN_CST                       0x1A  /* CST - 充电机停止充电报文 (充电机 -> BMS) */
#define PGN_CCS                       0x12  /* CCS - 充电机充电状态报文 (充电机 -> BMS) */

/* 充电结束后报文 */
#define PGN_CSD                       0x1D  /* CSD - 充电机统计数据报文 (充电机 -> BMS) */

/* 充电错误报文 */
#define PGN_CEM                       0x1F  /* CEM - 充电机错误报文 (充电机 -> BMS) */


/* 充电机地址宏定义 */
#define CHARGER_ADDRESS               0x56  /* 充电机默认地址 */
/* BMS源地址宏定义 */
#define BMS_SOURCE_ADDRESS            0xF4  /* BMS默认源地址 */

/* 充电模式枚举 */
typedef enum 
{
    GBT_Charging_Mode_Constant_Voltage = 1,  /* 恒压充电模式 */
    GBT_Charging_Mode_Constant_Current = 2,  /* 恒流充电模式 */
} GBT_Charging_Mode;

/* 充电状态枚举 */
typedef enum
{
    GBT_Charging_Status_Pause = 0,   /* 暂停充电 */
    GBT_Charging_Status_Allow = 1,   /* 允许充电 */
} GBT_Charging_Status;

/* 充电准备状态枚举 */
typedef enum
{
    GBT_Charging_Prepare_Status_Not_Ready = 0x00,  /* 未准备好 */
    GBT_Charging_Prepare_Status_Ready = 0xAA,      /* 准备好 */
    GBT_Charging_Prepare_Status_Invalid = 0xFF,    /* 无效状态 */
} GBT_Charging_Prepare_Status;

/*
 * GB/T 27930 报文数据结构
 * 这些结构体保存了各种报文的数据
 */


/**
 * @brief BHM - 电池握手报文
 * PGN: 0x0027, 2字节, 由BMS发送
 */
 typedef struct {
 	uint16_t batPermitChrgVolMax;       /* 电池最高允许充电电压 (0.1V/bit) */
} GBT_BHM_Data;
 
/**
 * @brief CHM - 充电机握手报文
 * PGN: 0x0026, 6字节, 由充电机发送
 */
 typedef struct {
    uint8_t charger_version[3];        /* 充电机通信协议版本 */
} GBT_CHM_Data;
 
/**
 * @brief CML - 充电机最大输出能力报文
 * PGN: 0x0800, 8字节, 由充电机发送
 */
typedef struct {
    uint16_t max_output_voltage;        /* 最高输出电压 (0.1V/bit) */
    uint16_t min_output_voltage;        /* 最低输出电压 (0.1V/bit) */
    uint16_t max_output_current;        /* 最高输出电流 (0.1A/bit, 偏移量-400A) */
    uint16_t min_output_current;        /* 最低输出电流 (0.1A/bit, 偏移量-400A) */
} GBT_CML_Data;

 /**
  * @brief BRM - 电池辨识报文
  * PGN: 0x0002, 49字节, 由BMS发送(可以不多包发送)
  */
 typedef struct {
    uint8_t bmsProVer[3];                 /* BMS协议版本 */
    uint8_t batType;                    /* 电池类型 (03H:磷酸铁锂, 06H:三元锂) */
    uint16_t batRateCapacity;           /* 电池额定容量 (0.1Ah/bit) */
    uint16_t batRateVol;                /* 电池额定总电压 (0.1V/bit) */
    uint32_t manufacterName;            /* 厂商名称 */
    uint32_t batSeriNum;                /* 电池序号 */
    uint8_t batYear;                    /* 电池生产日期年(1985年偏移量) */
    uint8_t batMonth;                   /* 电池生产日期月 */
    uint8_t batDate;                    /* 电池生产日期日 */
    uint32_t batChrgCount;              /* 电池充电次数 */
    uint8_t batProperIdentify;          /* 电池产权标识 */
    uint8_t reserver;                   /* 预留字节 */
    uint16_t carVIN[17];                 /* 车辆识别码VIN */
    uint16_t bmsSoftWareVer[8];         /* BMS软件版本 */
 } GBT_BRM_Data;

 /**
  * @brief CRM - 充电机辨识报文
  * PGN: 0x0001, 8字节, 由充电机发送
  */
 typedef struct {
     uint8_t Recognitio_Result;          /* 辨识结果 (00H:不能辨识, AAH:辨识成功) */
     uint8_t charger_serial[4];          /* 充电机序列号 */
     uint8_t region_code[3];             /* 充电机所在区域编码 */
 } GBT_CRM_Data;
 
 /**
  * @brief BCP - 电池充电参数报文
  * PGN: 0x0006, 13字节, 由BMS发送(必须多包发送)
  */
 typedef struct {
    uint16_t batPermitChrgVolMax;       /* 电池单体最高允许充电电压 (0.01V/bit) */
    uint16_t batPermitChrgCurMax;       /* 最高允许输出充电电流 (0.1A/bit, 偏移量-400A) */
    uint16_t batPowerMax;               /* 电池标称总能量 (0.1kWh/bit) */
    uint16_t batPermitTotalChrgVolMax;  /* 最高允许充电总电压 (0.1V/bit) */
    uint8_t  batPermitTemp;             /* 最高允许温度 (1℃/bit, 偏移量-50℃) */
    uint16_t batChrgState;              /* 电池荷电状态 (0.1%/bit) */
    uint16_t batCurrentVol;             /* 当前电池电压 (0.1V/bit) */
 } GBT_BCP_Data;
 
 /**
  * @brief CTS - 充电机时间同步报文
  * PGN: 0x0007, 7字节, 由充电机发送
  * 数据格式：原报文为BCD编码,BMS一侧直接解析为十进制
  *  CTS报文的7个字节分别表示：
  *  秒（BCD格式）
  *  分（BCD格式）
  *  时（BCD格式）
  *  日（BCD格式）
  *  月（BCD格式）
  *  年（BCD格式）
  *  世纪（BCD格式）
  */
 typedef struct {
     uint8_t year;                       /* 年 */
     uint8_t month;                      /* 月 */
     uint8_t day;                        /* 日 */
     uint8_t hour;                       /* 时 */
     uint8_t minute;                      /* 分 */
     uint8_t second;                     /* 秒 */
     uint8_t century;                    /* 世纪 */
 } GBT_CTS_Data;
 
 /**
  * @brief BRO - 电池充电准备就绪报文
  * PGN: 0x0009, 1字节, 由BMS发送
  */
 typedef struct {
    GBT_Charging_Prepare_Status charging_prepare_status; 
 } GBT_BRO_Data;
 
 /**
  * @brief CRO - 充电机充电准备就绪报文
  * PGN: 0x000A, 1字节, 由充电机发送
  */
 typedef struct {
    GBT_Charging_Prepare_Status charging_prepare_status; 
 } GBT_CRO_Data;
 
/**
 * @brief BCL - 电池充电需求报文
 * PGN: 0x1000, 5字节, 由BMS周期性发送
 */
typedef struct {
	uint16_t charging_voltage_demand;   /* 需求充电电压 (0.1V/bit) */
	uint16_t charging_current_demand;   /* 需求充电电流 (0.1A/bit, 偏移量-400A) */
	uint8_t GBT_Charging_Mode;          /* 充电模式 (1:恒压, 2:恒流) */
} GBT_BCL_Data;


 /**
  * @brief BCS - 电池充电状态报文
  * PGN: 0x0011, 9字节, 由BMS发送(必须多包发送)
  */
 typedef struct {
    uint16_t chrgVol;                   /* 充电电压测量值 (0.1V/bit) */
    uint16_t chrgCur;                   /* 充电电流测量值 (0.1A/bit, 偏移量-400A) */
    uint16_t batVolMax:12;              /* 最高单体电池电压 (0.01V/bit) */
    uint8_t batNum:4;                   /* 最高单体电池组号 */
    uint8_t batSoc;                     /* 当前荷电状态SOC% (1%/bit) */
    uint16_t residualChrgTime;          /* 估算剩余充电时间 (分钟) */
 } GBT_BCS_Data;
 
 /**
 * @brief CCS - 充电机充电状态报文
 * PGN: 0x1200, 8字节, 由充电机周期性发送
 */
 typedef struct {
     uint16_t output_voltage;            /* 输出电压 (0.1V/bit) */
     uint16_t output_current;            /* 输出电流 (0.1A/bit, 偏移量-400A) */
     uint16_t total_charging_time;       /* 累计充电时间 (分钟) */
     uint8_t charging_status : 2;            /* 充电允许状态 (0:暂停, 1:允许) */
 } GBT_CCS_Data;

 /**
 * @brief BSM - 电池状态信息报文
 * PGN: 0x1300, 8字节, 由BMS周期性发送
 */
 typedef struct {
     uint8_t volMaxBatNumber;            /* 最高单体电池电压所在编号 */
     uint8_t batTempMax;                 /* 最高电池温度 */
     uint8_t tempMaxBatNumber;           /* 最高温度检测点编号 */
     uint8_t batTempMin;                 /* 最低电池温度 */
     uint8_t tempMinBatNumber;           /* 最低电池温度检测点编号 */

     // 位段定义
     uint8_t batVolState : 2;              /* 单体电池电压状态 (00:正常, 01:过高, 10:过低) */
     uint8_t batSocState : 2;              /* 电池荷电状态 (00:正常, 01:过高, 10:过低) */
     uint8_t chrgCurState : 2;             /* 充电电流状态 (00:正常, 01:过流, 10:不可信) */
     uint8_t batTempOverState : 2;         /* 电池温度过高状态 (00:正常, 01:过高, 10:不可信) */

     uint8_t batInsulationState : 2;       /* 电池绝缘状态 (00:正常, 01:不正常, 10:不可信) */
     uint8_t batConnectState : 2;          /* 电池连接器状态 (00:正常, 01:不正常, 10:不可信) */
     uint8_t chrgPermit : 2;               /* 充电允许 (00:禁止, 01:允许) */
     uint8_t reserved : 2;                 /* 保留位 */
 } GBT_BSM_Data;

 /**
  * @brief BST - 电池停止充电报文
  * PGN: 0x0019, 7字节, 由BMS发送
  */
 typedef struct {
    uint8_t BMS_stop_reason;            /* BMS中止充电原因 */
    uint8_t BMS_stop_fault_reason[2];   /* BMS中止充电故障原因 */
    uint8_t BMS_stop_wrong_reason;      /* BMS中止充电错误原因 */
} GBT_BST_Data;
 
 /**
  * @brief CST - 充电机停止充电报文
  * PGN: 0x001A, 4字节, 由充电机发送
  */
 typedef struct {
    uint8_t Charger_stop_reason;        /* 充电机中止充电原因 */
    uint8_t Charger_stop_fault_reason[2]; /* 充电机中止充电故障原因 */
    uint8_t Charger_stop_wrong_reason;  /* 充电机中止充电错误原因 */
} GBT_CST_Data;
 
 
 /**
  * @brief BEM - 电池错误报文
  * PGN: 0x001E, 4字节, 由BMS发送
  */
 typedef struct {
     uint8_t error_flags[4];            /* 错误标志位 */
} GBT_BEM_Data;
 
 /**
  * @brief CEM - 充电机错误报文
  * PGN: 0x001F, 4字节, 由充电机发送
  */
 typedef struct {
     uint8_t error_flags[4];            /* 错误标志位 */
} GBT_CEM_Data;


 /**
  * @brief BMT - 电池温度报文
  * PGN: 0x0016, 可变长度, 由BMS发送
  */
 typedef struct {
     uint8_t temp_probes_count;         /* 温度探针数量 */
     uint8_t temperatures[60];          /* 温度值数组 (1℃/bit, 偏移量-50℃) */
 } GBT_BMT_Data;
 
 /**
  * @brief BMV - 电池电压报文
  * PGN: 0x0015, 可变长度, 由BMS发送
  */
 typedef struct {
     uint16_t total_cells;              /* 总电池节数 */
     uint16_t voltages[50];             /* 电压值数组 (0.01V/bit) */
 } GBT_BMV_Data;
 
 /**
  * @brief BSD - 电池统计数据报文 (PGN7168)
  * 6字节, 由BMS发送
  */
 typedef struct {
    uint8_t soc;                      /* SPN3601: 中止荷电状态SOC (0-100%) */
    uint16_t min_cell_voltage;        /* SPN3602: 单体蓄电池最低电压 (0.01V/位) */
    uint16_t max_cell_voltage;        /* SPN3603: 单体蓄电池最高电压 (0.01V/位) */
    int8_t min_battery_temp;          /* SPN3604: 动力电池最低温度 (-50~200℃) */
    int8_t max_battery_temp;          /* SPN3605: 动力电池最高温度 (-50~200℃) */
} GBT_BSD_Data;

/**
 * @brief CSD - 充电机统计数据报文 (PGN7424)
 * 8字节, 由充电机发送
 */
typedef struct {
    uint16_t charging_time;           /* SPN3611: 本次充电时间 (分钟) */
    uint16_t output_energy;           /* SPN3612: 输出能量 (0.1kWh/位) */
    uint32_t charger_id;              /* SPN3613: 充电机编号 (1位, 1偏移量) */
} GBT_CSD_Data;

/*
 * 应用层函数声明
 */

/* 握手报文函数 */
Transport_StatusTypeDef ApplicationLayer_Send_BHM(GBT_BHM_Data* bhm_data);
Transport_StatusTypeDef ApplicationLayer_Read_CHM(GBT_CHM_Data* chm_data);

/* 辨识报文函数 (BMS视角) */
Transport_StatusTypeDef ApplicationLayer_Send_BRM(GBT_BRM_Data* brm_data);
Transport_StatusTypeDef ApplicationLayer_Read_CRM(GBT_CRM_Data* crm_data);

/* 充电参数报文函数 (BMS视角) */
Transport_StatusTypeDef ApplicationLayer_Send_BCP(GBT_BCP_Data* bcp_data);
Transport_StatusTypeDef ApplicationLayer_Read_CML(GBT_CML_Data* cml_data);
Transport_StatusTypeDef ApplicationLayer_Read_CTS(GBT_CTS_Data* cts_data);

/* 充电准备报文函数 (BMS视角) */
Transport_StatusTypeDef ApplicationLayer_Send_BRO(GBT_BRO_Data* bro_data);
Transport_StatusTypeDef ApplicationLayer_Read_CRO(GBT_CRO_Data* cro_data);

/* 充电状态报文函数 (BMS视角) */
Transport_StatusTypeDef ApplicationLayer_Send_BCS(GBT_BCS_Data* bcs_data);
Transport_StatusTypeDef ApplicationLayer_Send_BCL(GBT_BCL_Data* bcl_data);
Transport_StatusTypeDef ApplicationLayer_Read_CCS(GBT_CCS_Data* ccs_data);
Transport_StatusTypeDef ApplicationLayer_Send_BSM(GBT_BSM_Data* bsm_data);

/* 停止充电报文函数 (BMS视角) */
Transport_StatusTypeDef ApplicationLayer_Send_BST(GBT_BST_Data* bst_data);
Transport_StatusTypeDef ApplicationLayer_Read_CST(GBT_CST_Data* cst_data);

/* 统计数据报文函数 (BMS视角) */
Transport_StatusTypeDef ApplicationLayer_Send_BSD(GBT_BSD_Data* bsd_data);
Transport_StatusTypeDef ApplicationLayer_Read_CSD(GBT_CSD_Data* csd_data);

/* 错误报文函数 (BMS视角) */
Transport_StatusTypeDef ApplicationLayer_Send_BEM(GBT_BEM_Data* bem_data);
Transport_StatusTypeDef ApplicationLayer_Read_CEM(GBT_CEM_Data* cem_data);

/* 数据报文函数 (BMS视角) */
Transport_StatusTypeDef ApplicationLayer_Send_BMT(GBT_BMT_Data* bmt_data);
Transport_StatusTypeDef ApplicationLayer_Send_BMV(GBT_BMV_Data* bmv_data);


#endif /* GB_APPLICATIONLAYER_H_ */
