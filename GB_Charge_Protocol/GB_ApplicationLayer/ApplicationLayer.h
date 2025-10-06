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
 * �����˱�׼�й涨����ҪPGN
 */

/************** BMS -> Charger **************/
/* ������ֱ��� */
#define PGN_BRM                       0x02  /* BRM - BMS��ʶ���� (BMS -> ����) */
#define PGN_BHM                       0x27  /* BHM - BMS���ֱ��� (BMS -> ����) */

/* ���������ñ��� */
#define PGN_BCP                       0x06  /* BCP - BMS���������� (BMS -> ����) */
#define PGN_BRO                       0x09  /* BRO - BMS���׼���������� (BMS -> ����) */

/* ���׶α��� */
#define PGN_BCL                       0x10  /* BCL - BMS��������� (BMS -> ����) */ 
#define PGN_BSM                       0x13  /* BSM - BMS״̬��Ϣ���� (BMS -> ����) */
#define PGN_BCS                       0x11  /* BCS - BMS�ܳ��״̬���� (BMS -> ����) */
#define PGN_BST                       0x19  /* BST - BMSֹͣ��籨�� (BMS -> ����) */
#define PGN_BMT                       0x16  /* BMT - BMS����¶ȱ��� (BMS -> ����) */
#define PGN_BMV                       0x15  /* BMV - BMS��ص�ѹ���� (BMS -> ����) */

/* ���������� */
#define PGN_BSD                       0x1C  /* BSD - BMSͳ�����ݱ��� (BMS -> ����) */

/* �������� */
#define PGN_BEM                       0x1E  /* BEM - BMS������ (BMS -> ����) */

/************** Charger -> BMS **************/
/* ������ֱ��� */
#define PGN_CRM                       0x01  /* CRM - ������ʶ���� (���� -> BMS) */
#define PGN_CHM                       0x26  /* CHM - �������ֱ��� (���� -> BMS) */

/* ���������ñ��� */
#define PGN_CML                       0x08  /* CML - ������������������ (���� -> BMS) */
#define PGN_CTS                       0x07  /* CTS - ����ʱ��ͬ������ (���� -> BMS) */
#define PGN_CRO                       0x0A  /* CRO - �������׼���������� (���� -> BMS) */

/* ���׶α��� */
#define PGN_CST                       0x1A  /* CST - ����ֹͣ��籨�� (���� -> BMS) */
#define PGN_CCS                       0x12  /* CCS - �������״̬���� (���� -> BMS) */

/* ���������� */
#define PGN_CSD                       0x1D  /* CSD - ����ͳ�����ݱ��� (���� -> BMS) */

/* �������� */
#define PGN_CEM                       0x1F  /* CEM - ���������� (���� -> BMS) */


/* ������ַ�궨�� */
#define CHARGER_ADDRESS               0x56  /* ����Ĭ�ϵ�ַ */
/* BMSԴ��ַ�궨�� */
#define BMS_SOURCE_ADDRESS            0xF4  /* BMSĬ��Դ��ַ */

/* ���ģʽö�� */
typedef enum 
{
    GBT_Charging_Mode_Constant_Voltage = 1,  /* ��ѹ���ģʽ */
    GBT_Charging_Mode_Constant_Current = 2,  /* �������ģʽ */
} GBT_Charging_Mode;

/* ���״̬ö�� */
typedef enum
{
    GBT_Charging_Status_Pause = 0,   /* ��ͣ��� */
    GBT_Charging_Status_Allow = 1,   /* ������ */
} GBT_Charging_Status;

/* ���׼��״̬ö�� */
typedef enum
{
    GBT_Charging_Prepare_Status_Not_Ready = 0x00,  /* δ׼���� */
    GBT_Charging_Prepare_Status_Ready = 0xAA,      /* ׼���� */
    GBT_Charging_Prepare_Status_Invalid = 0xFF,    /* ��Ч״̬ */
} GBT_Charging_Prepare_Status;

/*
 * GB/T 27930 �������ݽṹ
 * ��Щ�ṹ�屣���˸��ֱ��ĵ�����
 */


/**
 * @brief BHM - ������ֱ���
 * PGN: 0x0027, 2�ֽ�, ��BMS����
 */
 typedef struct {
 	uint16_t batPermitChrgVolMax;       /* �������������ѹ (0.1V/bit) */
} GBT_BHM_Data;
 
/**
 * @brief CHM - �������ֱ���
 * PGN: 0x0026, 6�ֽ�, �ɳ�������
 */
 typedef struct {
    uint8_t charger_version[3];        /* ����ͨ��Э��汾 */
} GBT_CHM_Data;
 
/**
 * @brief CML - ������������������
 * PGN: 0x0800, 8�ֽ�, �ɳ�������
 */
typedef struct {
    uint16_t max_output_voltage;        /* ��������ѹ (0.1V/bit) */
    uint16_t min_output_voltage;        /* ��������ѹ (0.1V/bit) */
    uint16_t max_output_current;        /* ���������� (0.1A/bit, ƫ����-400A) */
    uint16_t min_output_current;        /* ���������� (0.1A/bit, ƫ����-400A) */
} GBT_CML_Data;

 /**
  * @brief BRM - ��ر�ʶ����
  * PGN: 0x0002, 49�ֽ�, ��BMS����(���Բ��������)
  */
 typedef struct {
    uint8_t bmsProVer[3];                 /* BMSЭ��汾 */
    uint8_t batType;                    /* ������� (03H:�������, 06H:��Ԫ�) */
    uint16_t batRateCapacity;           /* ��ض���� (0.1Ah/bit) */
    uint16_t batRateVol;                /* ��ض�ܵ�ѹ (0.1V/bit) */
    uint32_t manufacterName;            /* �������� */
    uint32_t batSeriNum;                /* ������ */
    uint8_t batYear;                    /* �������������(1985��ƫ����) */
    uint8_t batMonth;                   /* ������������� */
    uint8_t batDate;                    /* ������������� */
    uint32_t batChrgCount;              /* ��س����� */
    uint8_t batProperIdentify;          /* ��ز�Ȩ��ʶ */
    uint8_t reserver;                   /* Ԥ���ֽ� */
    uint16_t carVIN[17];                 /* ����ʶ����VIN */
    uint16_t bmsSoftWareVer[8];         /* BMS����汾 */
 } GBT_BRM_Data;

 /**
  * @brief CRM - ������ʶ����
  * PGN: 0x0001, 8�ֽ�, �ɳ�������
  */
 typedef struct {
     uint8_t Recognitio_Result;          /* ��ʶ��� (00H:���ܱ�ʶ, AAH:��ʶ�ɹ�) */
     uint8_t charger_serial[4];          /* �������к� */
     uint8_t region_code[3];             /* ��������������� */
 } GBT_CRM_Data;
 
 /**
  * @brief BCP - ��س���������
  * PGN: 0x0006, 13�ֽ�, ��BMS����(����������)
  */
 typedef struct {
    uint16_t batPermitChrgVolMax;       /* ��ص�������������ѹ (0.01V/bit) */
    uint16_t batPermitChrgCurMax;       /* ���������������� (0.1A/bit, ƫ����-400A) */
    uint16_t batPowerMax;               /* ��ر�������� (0.1kWh/bit) */
    uint16_t batPermitTotalChrgVolMax;  /* ����������ܵ�ѹ (0.1V/bit) */
    uint8_t  batPermitTemp;             /* ��������¶� (1��/bit, ƫ����-50��) */
    uint16_t batChrgState;              /* ��غɵ�״̬ (0.1%/bit) */
    uint16_t batCurrentVol;             /* ��ǰ��ص�ѹ (0.1V/bit) */
 } GBT_BCP_Data;
 
 /**
  * @brief CTS - ����ʱ��ͬ������
  * PGN: 0x0007, 7�ֽ�, �ɳ�������
  * ���ݸ�ʽ��ԭ����ΪBCD����,BMSһ��ֱ�ӽ���Ϊʮ����
  *  CTS���ĵ�7���ֽڷֱ��ʾ��
  *  �루BCD��ʽ��
  *  �֣�BCD��ʽ��
  *  ʱ��BCD��ʽ��
  *  �գ�BCD��ʽ��
  *  �£�BCD��ʽ��
  *  �꣨BCD��ʽ��
  *  ���ͣ�BCD��ʽ��
  */
 typedef struct {
     uint8_t year;                       /* �� */
     uint8_t month;                      /* �� */
     uint8_t day;                        /* �� */
     uint8_t hour;                       /* ʱ */
     uint8_t minute;                      /* �� */
     uint8_t second;                     /* �� */
     uint8_t century;                    /* ���� */
 } GBT_CTS_Data;
 
 /**
  * @brief BRO - ��س��׼����������
  * PGN: 0x0009, 1�ֽ�, ��BMS����
  */
 typedef struct {
    GBT_Charging_Prepare_Status charging_prepare_status; 
 } GBT_BRO_Data;
 
 /**
  * @brief CRO - �������׼����������
  * PGN: 0x000A, 1�ֽ�, �ɳ�������
  */
 typedef struct {
    GBT_Charging_Prepare_Status charging_prepare_status; 
 } GBT_CRO_Data;
 
/**
 * @brief BCL - ��س��������
 * PGN: 0x1000, 5�ֽ�, ��BMS�����Է���
 */
typedef struct {
	uint16_t charging_voltage_demand;   /* �������ѹ (0.1V/bit) */
	uint16_t charging_current_demand;   /* ��������� (0.1A/bit, ƫ����-400A) */
	uint8_t GBT_Charging_Mode;          /* ���ģʽ (1:��ѹ, 2:����) */
} GBT_BCL_Data;


 /**
  * @brief BCS - ��س��״̬����
  * PGN: 0x0011, 9�ֽ�, ��BMS����(����������)
  */
 typedef struct {
    uint16_t chrgVol;                   /* ����ѹ����ֵ (0.1V/bit) */
    uint16_t chrgCur;                   /* ����������ֵ (0.1A/bit, ƫ����-400A) */
    uint16_t batVolMax:12;              /* ��ߵ����ص�ѹ (0.01V/bit) */
    uint8_t batNum:4;                   /* ��ߵ�������� */
    uint8_t batSoc;                     /* ��ǰ�ɵ�״̬SOC% (1%/bit) */
    uint16_t residualChrgTime;          /* ����ʣ����ʱ�� (����) */
 } GBT_BCS_Data;
 
 /**
 * @brief CCS - �������״̬����
 * PGN: 0x1200, 8�ֽ�, �ɳ��������Է���
 */
 typedef struct {
     uint16_t output_voltage;            /* �����ѹ (0.1V/bit) */
     uint16_t output_current;            /* ������� (0.1A/bit, ƫ����-400A) */
     uint16_t total_charging_time;       /* �ۼƳ��ʱ�� (����) */
     uint8_t charging_status : 2;            /* �������״̬ (0:��ͣ, 1:����) */
 } GBT_CCS_Data;

 /**
 * @brief BSM - ���״̬��Ϣ����
 * PGN: 0x1300, 8�ֽ�, ��BMS�����Է���
 */
 typedef struct {
     uint8_t volMaxBatNumber;            /* ��ߵ����ص�ѹ���ڱ�� */
     uint8_t batTempMax;                 /* ��ߵ���¶� */
     uint8_t tempMaxBatNumber;           /* ����¶ȼ����� */
     uint8_t batTempMin;                 /* ��͵���¶� */
     uint8_t tempMinBatNumber;           /* ��͵���¶ȼ����� */

     // λ�ζ���
     uint8_t batVolState : 2;              /* �����ص�ѹ״̬ (00:����, 01:����, 10:����) */
     uint8_t batSocState : 2;              /* ��غɵ�״̬ (00:����, 01:����, 10:����) */
     uint8_t chrgCurState : 2;             /* ������״̬ (00:����, 01:����, 10:������) */
     uint8_t batTempOverState : 2;         /* ����¶ȹ���״̬ (00:����, 01:����, 10:������) */

     uint8_t batInsulationState : 2;       /* ��ؾ�Ե״̬ (00:����, 01:������, 10:������) */
     uint8_t batConnectState : 2;          /* ���������״̬ (00:����, 01:������, 10:������) */
     uint8_t chrgPermit : 2;               /* ������� (00:��ֹ, 01:����) */
     uint8_t reserved : 2;                 /* ����λ */
 } GBT_BSM_Data;

 /**
  * @brief BST - ���ֹͣ��籨��
  * PGN: 0x0019, 7�ֽ�, ��BMS����
  */
 typedef struct {
    uint8_t BMS_stop_reason;            /* BMS��ֹ���ԭ�� */
    uint8_t BMS_stop_fault_reason[2];   /* BMS��ֹ������ԭ�� */
    uint8_t BMS_stop_wrong_reason;      /* BMS��ֹ������ԭ�� */
} GBT_BST_Data;
 
 /**
  * @brief CST - ����ֹͣ��籨��
  * PGN: 0x001A, 4�ֽ�, �ɳ�������
  */
 typedef struct {
    uint8_t Charger_stop_reason;        /* ������ֹ���ԭ�� */
    uint8_t Charger_stop_fault_reason[2]; /* ������ֹ������ԭ�� */
    uint8_t Charger_stop_wrong_reason;  /* ������ֹ������ԭ�� */
} GBT_CST_Data;
 
 
 /**
  * @brief BEM - ��ش�����
  * PGN: 0x001E, 4�ֽ�, ��BMS����
  */
 typedef struct {
     uint8_t error_flags[4];            /* �����־λ */
} GBT_BEM_Data;
 
 /**
  * @brief CEM - ����������
  * PGN: 0x001F, 4�ֽ�, �ɳ�������
  */
 typedef struct {
     uint8_t error_flags[4];            /* �����־λ */
} GBT_CEM_Data;


 /**
  * @brief BMT - ����¶ȱ���
  * PGN: 0x0016, �ɱ䳤��, ��BMS����
  */
 typedef struct {
     uint8_t temp_probes_count;         /* �¶�̽������ */
     uint8_t temperatures[60];          /* �¶�ֵ���� (1��/bit, ƫ����-50��) */
 } GBT_BMT_Data;
 
 /**
  * @brief BMV - ��ص�ѹ����
  * PGN: 0x0015, �ɱ䳤��, ��BMS����
  */
 typedef struct {
     uint16_t total_cells;              /* �ܵ�ؽ��� */
     uint16_t voltages[50];             /* ��ѹֵ���� (0.01V/bit) */
 } GBT_BMV_Data;
 
 /**
  * @brief BSD - ���ͳ�����ݱ��� (PGN7168)
  * 6�ֽ�, ��BMS����
  */
 typedef struct {
    uint8_t soc;                      /* SPN3601: ��ֹ�ɵ�״̬SOC (0-100%) */
    uint16_t min_cell_voltage;        /* SPN3602: ����������͵�ѹ (0.01V/λ) */
    uint16_t max_cell_voltage;        /* SPN3603: ����������ߵ�ѹ (0.01V/λ) */
    int8_t min_battery_temp;          /* SPN3604: �����������¶� (-50~200��) */
    int8_t max_battery_temp;          /* SPN3605: �����������¶� (-50~200��) */
} GBT_BSD_Data;

/**
 * @brief CSD - ����ͳ�����ݱ��� (PGN7424)
 * 8�ֽ�, �ɳ�������
 */
typedef struct {
    uint16_t charging_time;           /* SPN3611: ���γ��ʱ�� (����) */
    uint16_t output_energy;           /* SPN3612: ������� (0.1kWh/λ) */
    uint32_t charger_id;              /* SPN3613: ������� (1λ, 1ƫ����) */
} GBT_CSD_Data;

/*
 * Ӧ�ò㺯������
 */

/* ���ֱ��ĺ��� */
Transport_StatusTypeDef ApplicationLayer_Send_BHM(GBT_BHM_Data* bhm_data);
Transport_StatusTypeDef ApplicationLayer_Read_CHM(GBT_CHM_Data* chm_data);

/* ��ʶ���ĺ��� (BMS�ӽ�) */
Transport_StatusTypeDef ApplicationLayer_Send_BRM(GBT_BRM_Data* brm_data);
Transport_StatusTypeDef ApplicationLayer_Read_CRM(GBT_CRM_Data* crm_data);

/* ���������ĺ��� (BMS�ӽ�) */
Transport_StatusTypeDef ApplicationLayer_Send_BCP(GBT_BCP_Data* bcp_data);
Transport_StatusTypeDef ApplicationLayer_Read_CML(GBT_CML_Data* cml_data);
Transport_StatusTypeDef ApplicationLayer_Read_CTS(GBT_CTS_Data* cts_data);

/* ���׼�����ĺ��� (BMS�ӽ�) */
Transport_StatusTypeDef ApplicationLayer_Send_BRO(GBT_BRO_Data* bro_data);
Transport_StatusTypeDef ApplicationLayer_Read_CRO(GBT_CRO_Data* cro_data);

/* ���״̬���ĺ��� (BMS�ӽ�) */
Transport_StatusTypeDef ApplicationLayer_Send_BCS(GBT_BCS_Data* bcs_data);
Transport_StatusTypeDef ApplicationLayer_Send_BCL(GBT_BCL_Data* bcl_data);
Transport_StatusTypeDef ApplicationLayer_Read_CCS(GBT_CCS_Data* ccs_data);
Transport_StatusTypeDef ApplicationLayer_Send_BSM(GBT_BSM_Data* bsm_data);

/* ֹͣ��籨�ĺ��� (BMS�ӽ�) */
Transport_StatusTypeDef ApplicationLayer_Send_BST(GBT_BST_Data* bst_data);
Transport_StatusTypeDef ApplicationLayer_Read_CST(GBT_CST_Data* cst_data);

/* ͳ�����ݱ��ĺ��� (BMS�ӽ�) */
Transport_StatusTypeDef ApplicationLayer_Send_BSD(GBT_BSD_Data* bsd_data);
Transport_StatusTypeDef ApplicationLayer_Read_CSD(GBT_CSD_Data* csd_data);

/* �����ĺ��� (BMS�ӽ�) */
Transport_StatusTypeDef ApplicationLayer_Send_BEM(GBT_BEM_Data* bem_data);
Transport_StatusTypeDef ApplicationLayer_Read_CEM(GBT_CEM_Data* cem_data);

/* ���ݱ��ĺ��� (BMS�ӽ�) */
Transport_StatusTypeDef ApplicationLayer_Send_BMT(GBT_BMT_Data* bmt_data);
Transport_StatusTypeDef ApplicationLayer_Send_BMV(GBT_BMV_Data* bmv_data);


#endif /* GB_APPLICATIONLAYER_H_ */
