/*
 * ApplicationLayer.c
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 * 
 */

#include "ApplicationLayer.h"


/* ���ֱ��ĺ��� */

/**
 * @brief ����BHM��������֣�����
 * @param bhm_data ָ��BHM���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BHM(GBT_BHM_Data* bhm_data) {
    /* BHM PGN: 0x0027, 2 bytes total */
    uint8_t data[2];

    data[0] = (uint8_t)(bhm_data->batPermitChrgVolMax);
    data[1] = (uint8_t)(bhm_data->batPermitChrgVolMax >> 8);
    
    /* ���͵�����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_Message(0x06, PGN_BHM, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 2);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BHM_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ��ȡ������CHM���������֣�����
 * @param chm_data ָ��CHM���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ��ȡ״̬
 */
Transport_StatusTypeDef ApplicationLayer_Read_CHM(GBT_CHM_Data* chm_data) {
     /* BHM PGN: 0x0026, 3 bytes total */
	uint32_t pgn;
    uint8_t data[3];
    uint8_t len;
    uint8_t da, sa, priority;
    
    /* �Ӵ�����ȡ��Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Receive_Message(&pgn, data, &len, &da, &sa, &priority);
	if (status == TRANSPORT_STATUS_OK) {
        /* ���PGN�Ƿ�ƥ�� */
        if (pgn == PGN_CHM) 
				{
								/* �������� */
								chm_data->charger_version[0] = data[0];
								chm_data->charger_version[1] = data[1];
								chm_data->charger_version[2] = data[2];
								return TRANSPORT_STATUS_OK;
				}
        return TRANSPORT_STATUS_ERROR;
    }
    return status;
}

/* ��ʶ���ĺ��� (BMS�ӽ�) */

/**
 * @brief ����BRM����ر�ʶ������ (��ǿ�ƶ������)
 * @param brm_data ָ��BRM���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BRM(GBT_BRM_Data* brm_data) {
    /* BRM PGN: 0x0200, 49 bytes total */
    uint8_t data[8];

    // ��������
    memset(data, 0, 8);
    memcpy(&data[0], &brm_data->bmsProVer, 3);
    data[3] = brm_data->batType;
    data[4] = (uint8_t)(brm_data->batRateCapacity);
    data[5] = (uint8_t)(brm_data->batRateCapacity >> 8);
    data[6] = (uint8_t)(brm_data->batRateVol);
    data[7] = (uint8_t)(brm_data->batRateVol >> 8);
    /*��Ϊ�˱��Ĳ�ǿ�ƶ������, ��ʵ��ѡ��ֻ���ͱ�������, ͬʱ�ṩ������֧��*/
//    memcpy(&data[8], &brm_data->manufacterName, 4);
//    memcpy(&data[12], &brm_data->batSeriNum, 4);
//    data[16] = brm_data->batYear;
//    data[17] = brm_data->batMonth;
//    data[18] = brm_data->batDate;
//    memcpy(&data[19], &brm_data->batChrgCount, 3);
//    data[22] = brm_data->batProperIdentify;
//    data[23] = brm_data->reserver;
//    memcpy(&data[24], brm_data->carVIN, 17);
//    memcpy(&data[41], brm_data->bmsSoftWareVer, 8);
    
    /* ���Ͷ����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_Message(0x07, PGN_BRM, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 8);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BRM_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ��ȡ������CRM��������ʶ������
 * @param crm_data ָ��CRM���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ��ȡ״̬
 */
Transport_StatusTypeDef ApplicationLayer_Read_CRM(GBT_CRM_Data* crm_data) {
    uint32_t pgn;
    uint8_t data[8];
    uint8_t len;
    uint8_t da, sa, priority;
    
    /* �Ӵ�����ȡ�����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Receive_Message(&pgn, data, &len, &da, &sa, &priority);
    if (status == TRANSPORT_STATUS_OK) {
        /* ���PGN�Ƿ�ƥ�� */
        if (pgn == PGN_CRM) 
		{
            /* �������� */
            crm_data->Recognitio_Result = data[0];
						printf("%d\n",data[0]);
            for(uint8_t i = 1;i<=4;i++)
			{
				crm_data->charger_serial[i] = data[i];
			}
			for(uint8_t i = 5;i<8;i++)
			{
				crm_data->region_code[i] = data[i];
			}
            
            return TRANSPORT_STATUS_OK;
        }
        return TRANSPORT_STATUS_ERROR;
    }
    return status;
}


/* ���������ĺ��� (BMS�ӽ�) */

/**
 * @brief ����BCP����س����������� (������)
 * @param bcp_data ָ��BCP���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BCP(GBT_BCP_Data* bcp_data) {
    /* BCP PGN: 0x0600, 13 bytes total */
    uint8_t data[13];
    
    data[0] = (uint8_t)(bcp_data->batPermitChrgVolMax);
    data[1] = (uint8_t)(bcp_data->batPermitChrgVolMax >> 8);
    data[2] = (uint8_t)(400-(bcp_data->batPermitChrgCurMax)*10);
    data[3] = (uint8_t)((400-(bcp_data->batPermitChrgCurMax)*10) >> 8);
    data[4] = (uint8_t)(bcp_data->batPowerMax);
    data[5] = (uint8_t)(bcp_data->batPowerMax >> 8);
    data[6] = (uint8_t)(bcp_data->batPermitTotalChrgVolMax);
    data[7] = (uint8_t)(bcp_data->batPermitTotalChrgVolMax >> 8);
    data[8] = bcp_data->batPermitTemp + 50; // Offset -50
    data[9] = (uint8_t)(bcp_data->batChrgState);
    data[10] = (uint8_t)(bcp_data->batChrgState >> 8);
    data[11] = (uint8_t)(bcp_data->batCurrentVol);
    data[12] = (uint8_t)(bcp_data->batCurrentVol >> 8);

    /* ���Ͷ����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_MultiPacket(0x07, PGN_BCP, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 13);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BCP_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ��ȡ������CML����������������������
 * @param cml_data ָ��CML���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ��ȡ״̬
 */
Transport_StatusTypeDef ApplicationLayer_Read_CML(GBT_CML_Data* cml_data) {
    uint32_t pgn;
    uint8_t data[8];
    uint8_t len;
    uint8_t da, sa, priority;
    
    /* �Ӵ�����ȡ��Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Receive_Message(&pgn, data, &len, &da, &sa, &priority);
    if (status == TRANSPORT_STATUS_OK) {
        /* ���PGN�Ƿ�ƥ�� */
        if (pgn == PGN_CML) {
            /* �������� */
            cml_data->max_output_voltage = (data[1] << 8) | data[0];
            cml_data->min_output_voltage = (data[3] << 8) | data[2];
            cml_data->max_output_current = (data[5] << 8) | data[4];
            cml_data->min_output_current = (data[7] << 8) | data[6];
            return TRANSPORT_STATUS_OK;
        }
        return TRANSPORT_STATUS_ERROR;
    }
    return status;
}

/**
 * @brief ��ȡ������CTS������ʱ��ͬ��������
 * @param cts_data ָ��CTS���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ��ȡ״̬
 */
Transport_StatusTypeDef ApplicationLayer_Read_CTS(GBT_CTS_Data* cts_data) {
    uint32_t pgn;
    uint8_t data[8];
    uint8_t len;
    uint8_t da, sa, priority;
    
    /* �Ӵ�����ȡ��Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Receive_Message(&pgn, data, &len, &da, &sa, &priority);
    if (status == TRANSPORT_STATUS_OK) {
        /* ���PGN�Ƿ�ƥ�� */
        if (pgn == PGN_CTS) {
            /* ����BCD�������ݲ�ת��Ϊʮ���� */
            cts_data->second = ((data[0] >> 4) & 0x0F) * 10 + (data[0] & 0x0F);   /* �� (BCDתʮ����) */
            cts_data->minute = ((data[1] >> 4) & 0x0F) * 10 + (data[1] & 0x0F);   /* �� (BCDתʮ����) */
            cts_data->hour = ((data[2] >> 4) & 0x0F) * 10 + (data[2] & 0x0F);     /* ʱ (BCDתʮ����) */
            cts_data->day = ((data[3] >> 4) & 0x0F) * 10 + (data[3] & 0x0F);      /* �� (BCDתʮ����) */
            cts_data->month = ((data[4] >> 4) & 0x0F) * 10 + (data[4] & 0x0F);    /* �� (BCDתʮ����) */
            cts_data->year = ((data[5] >> 4) & 0x0F) * 10 + (data[5] & 0x0F);     /* �� (BCDתʮ����) */
            cts_data->century = ((data[6] >> 4) & 0x0F) * 10 + (data[6] & 0x0F);  /* ���� (BCDתʮ����) */
            return TRANSPORT_STATUS_OK;
        }
        return TRANSPORT_STATUS_ERROR;
    }
    return status;
}

/* ���׼�����ĺ��� (BMS�ӽ�) */

/**
 * @brief ����BRO����س��׼������������
 * @param bro_data ָ��BRO���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BRO(GBT_BRO_Data* bro_data) {
    /* BRO PGN: 0x0009, 1 byte total */
    uint8_t data[1];

    data[0] = bro_data->charging_prepare_status;
    
    /* ���͵�����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_Message(0x04, PGN_BRO, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 1);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BRO_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ��ȡ������CRO���������׼������������
 * @param cro_data ָ��CRO���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ��ȡ״̬
 */
Transport_StatusTypeDef ApplicationLayer_Read_CRO(GBT_CRO_Data* cro_data) {
    uint32_t pgn;
    uint8_t data[8];
    uint8_t len;
    uint8_t da, sa, priority;
    
    /* �Ӵ�����ȡ��Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Receive_Message(&pgn, data, &len, &da, &sa, &priority);
    if (status == TRANSPORT_STATUS_OK) {
        /* ���PGN�Ƿ�ƥ�� */
        if (pgn == PGN_CRO) {
            /* �������� */
            cro_data->charging_prepare_status = data[0];
            return TRANSPORT_STATUS_OK;
        }
        return TRANSPORT_STATUS_ERROR;
    }
    return status;
}

/* ���׶α��ĺ��� (BMS�ӽ�) */

/**
 * @brief ����BCS����س��״̬������(������)
 * @param bcs_data ָ��BCS���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BCS(GBT_BCS_Data* bcs_data) {
    /* BCS PGN: 0x0011, 9 bytes total */
    uint8_t data[9];

    data[0] = (uint8_t)(bcs_data->chrgVol);
    data[1] = (uint8_t)(bcs_data->chrgVol >> 8);
    data[2] = (uint8_t)(bcs_data->chrgCur);
    data[3] = (uint8_t)(bcs_data->chrgCur >> 8);
    data[4] = (uint8_t)(bcs_data->batVolMax);
    data[5] = (uint8_t)((bcs_data->batVolMax >> 8) & 0x0F) | ((bcs_data->batNum & 0x0F) << 4);
    data[6] = bcs_data->batSoc;
    data[7] = (uint8_t)(bcs_data->residualChrgTime);
    data[8] = (uint8_t)(bcs_data->residualChrgTime >> 8);
    
    /* ���Ͷ����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_MultiPacket(0x07, PGN_BCS, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 9);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BCS_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ����BCL����س�����󣩱���
 * @param bcl_data ָ��BCL���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BCL(GBT_BCL_Data* bcl_data) {
    /* BCL PGN: 0x1000, 5 bytes total */
    uint8_t data[5];

    data[0] = (uint8_t)(bcl_data->charging_voltage_demand);
    data[1] = (uint8_t)(bcl_data->charging_voltage_demand >> 8);
    data[2] = (uint8_t)((400-bcl_data->charging_current_demand)*10);
    data[3] = (uint8_t)(((400-bcl_data->charging_current_demand)*10) >> 8);
    data[4] = bcl_data->GBT_Charging_Mode;

    /* ���͵�����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_Message(0x06, PGN_BCL, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 5);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BCL_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ��ȡ������CCS���������״̬������
 * @param ccs_data ָ��CCS���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ��ȡ״̬
 */
Transport_StatusTypeDef ApplicationLayer_Read_CCS(GBT_CCS_Data* ccs_data) {
    /* CCS PGN: 0x1200, 8 bytes total */
    uint32_t pgn;
    uint8_t data[8];
    uint8_t len;
    uint8_t da, sa, priority;

    /* �Ӵ�����ȡ��Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Receive_Message(&pgn, data, &len, &da, &sa, &priority);
    if (status == TRANSPORT_STATUS_OK) {
        /* ���PGN�Ƿ�ƥ�� */
        if (pgn == PGN_CCS) {
            /* �������� */
            ccs_data->output_voltage = (data[1] << 8) | data[0];
            ccs_data->output_current = (data[3] << 8) | data[2];
            ccs_data->total_charging_time = (data[5] << 8) | data[4];
            ccs_data->charging_status = data[6] & 0x03;
            return TRANSPORT_STATUS_OK;
        }
        return TRANSPORT_STATUS_ERROR;
    }
    return status;
}

/**
 * @brief ����BSM�����״̬��Ϣ������
 * @param bsm_data ָ��BSM���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BSM(GBT_BSM_Data* bsm_data) {
    /* BSM PGN: 0x1300, 8 bytes total */
    uint8_t data[8];

    data[0] = bsm_data->volMaxBatNumber;
    data[1] = bsm_data->batTempMax;
    data[2] = bsm_data->tempMaxBatNumber;
    data[3] = bsm_data->batTempMin;
    data[4] = bsm_data->tempMinBatNumber;

    // λ�δ��
    data[5] = (bsm_data->batVolState & 0x03) |
        ((bsm_data->batSocState & 0x03) << 2) |
        ((bsm_data->chrgCurState & 0x03) << 4) |
        ((bsm_data->batTempOverState & 0x03) << 6);

    data[6] = (bsm_data->batInsulationState & 0x03) |
        ((bsm_data->batConnectState & 0x03) << 2) |
        ((bsm_data->chrgPermit & 0x03) << 4) |
        ((bsm_data->reserved & 0x03) << 6);

    /* ���͵�����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_Message(0x06, PGN_BSM, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 8);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BSM_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}
/* ֹͣ��籨�ĺ��� (BMS�ӽ�) */

/**
 * @brief ����BST��BMSֹͣ��磩���� (���)
 * @param bst_data ָ��BST���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BST(GBT_BST_Data* bst_data) {
    /* BST PGN: 0x0019, 7 bytes total */
    uint8_t data[7];

    data[0] = bst_data->BMS_stop_reason;
    memcpy(&data[1], bst_data->BMS_stop_fault_reason, 2);
    data[3] = bst_data->BMS_stop_wrong_reason;
    
    /* ���Ͷ����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_MultiPacket(0x04, PGN_BST, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 7);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BST_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ��ȡ������CST������ֹͣ��磩���� (���)
 * @param cst_data ָ��CST���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ��ȡ״̬
 */
Transport_StatusTypeDef ApplicationLayer_Read_CST(GBT_CST_Data* cst_data) {
    uint32_t pgn;
    uint8_t data[8];
    uint8_t len;
    uint8_t da, sa, priority;
    
    /* �Ӵ�����ȡ��Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Receive_Message(&pgn, data, &len, &da, &sa, &priority);
    if (status == TRANSPORT_STATUS_OK) {
        /* ���PGN�Ƿ�ƥ�� */
        if (pgn == PGN_CST) {
            /* �������� */
            cst_data->Charger_stop_reason = data[0];
            memcpy(cst_data->Charger_stop_fault_reason, &data[1], 2);
            cst_data->Charger_stop_wrong_reason = data[3];
            return TRANSPORT_STATUS_OK;
        }
        return TRANSPORT_STATUS_ERROR;
    }
    return status;
}

/* ͳ�����ݱ��ĺ��� (BMS�ӽ�) */

/**
 * @brief ����BSD�����ͳ�����ݣ����� (���)
 * @param bsd_data ָ��BSD���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BSD(GBT_BSD_Data* bsd_data) {
    /* BSD PGN: 0x001C, 7 bytes total */
    uint8_t data[7];

    data[0] = bsd_data->soc;  /* SPN3601: SOCֵ (0-100%) */
    
    /* SPN3602: ����������͵�ѹ (0.01V/λ) */
    data[1] = (uint8_t)(bsd_data->min_cell_voltage);
    data[2] = (uint8_t)(bsd_data->min_cell_voltage >> 8);
    
    /* SPN3603: ����������ߵ�ѹ (0.01V/λ) */
    data[3] = (uint8_t)(bsd_data->max_cell_voltage);
    data[4] = (uint8_t)(bsd_data->max_cell_voltage >> 8);
    
    /* SPN3604: ����¶� (-50��ƫ��) */
    data[5] = bsd_data->min_battery_temp + 50;
    
    /* SPN3605: ����¶� (-50��ƫ��) */
    data[6] = bsd_data->max_battery_temp + 50;  /* ��չ��7�ֽ���ƥ���׼ */
    
    /* ���Ͷ����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_MultiPacket(0x06, PGN_BSD, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 7);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BSD_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ��ȡ������CSD������ͳ�����ݣ����� (���)
 * @param csd_data ָ��CSD���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ��ȡ״̬
 */
Transport_StatusTypeDef ApplicationLayer_Read_CSD(GBT_CSD_Data* csd_data) {
    uint32_t pgn;
    uint8_t data[8];
    uint8_t len;
    
    /* �Ӵ�����ȡ�����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Receive_MultiPacket(0x06, &pgn, data, &len, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS);
    if (status == TRANSPORT_STATUS_OK) {
        /* ���PGN�Ƿ�ƥ�� */
        if (pgn == PGN_CSD) {
            /* �������� */
            csd_data->charging_time = (data[1] << 8) | data[0];  /* SPN3611: ���ʱ�� */
            csd_data->output_energy = (data[3] << 8) | data[2];  /* SPN3612: ������� */
            
            /* SPN3613: ������� (4�ֽ�, 1ƫ����) */
            csd_data->charger_id = ((uint32_t)data[7] << 24) | ((uint32_t)data[6] << 16) | 
                                 ((uint32_t)data[5] << 8) | data[4];
            csd_data->charger_id -= 1;  /* Ӧ��1λƫ���� */
            
            return TRANSPORT_STATUS_OK;
        }
        return TRANSPORT_STATUS_ERROR;
    }
    return status;
}

/* �����ĺ��� (BMS�ӽ�) */

/**
 * @brief ����BEM����ش��󣩱���
 * @param bem_data ָ��BEM���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BEM(GBT_BEM_Data* bem_data) {
    /* BEM PGN: 0x001E, 4 bytes total */
    uint8_t data[4];

    memcpy(data, bem_data->error_flags, 4);
    
    /* ���͵�����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_Message(0x02, PGN_BEM, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 4);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BEM_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

///**
// * @brief ��ȡ������CEM���������󣩱��� (���)
// * @param cem_data ָ��CEM���ݽṹ��ָ��
// * @return Transport_StatusTypeDef ��ȡ״̬
// */
//Transport_StatusTypeDef ApplicationLayer_Read_CEM(GBT_CEM_Data* cem_data) {
//    uint32_t pgn;
//    uint8_t data[8];
//    uint8_t len;
//    uint8_t da, sa, priority;
//    
//    /* �Ӵ�����ȡ�����Ϣ */
//    Transport_StatusTypeDef status = TransportLayer_Receive_MultiPacket(0x02, &pgn, data, &len, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS);
//    if (status == TRANSPORT_STATUS_OK) {
//        /* ���PGN�Ƿ�ƥ�� */
//        if (pgn == PGN_CEM) {
//            /* �������� */
//            cem_data->Charger_error_code = data[0];
//            memcpy(cem_data->Charger_error_reason, &data[1], 2);
//            cem_data->Charger_error_level = data[3];
//            return TRANSPORT_STATUS_OK;
//        }
//        return TRANSPORT_STATUS_ERROR;
//    }
//    return status;
//}

/* �ݲ��� */

/**
 * @brief ����BMT������¶ȣ����� (���)
 * @param bmt_data ָ��BMT���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BMT(GBT_BMT_Data* bmt_data) {
    /* BMT PGN: 0x0016, �ɱ䳤�� */
    uint8_t data[61]; // 1 byte for count + up to 60 bytes for temperatures

    data[0] = bmt_data->temp_probes_count;
    memcpy(&data[1], bmt_data->temperatures, bmt_data->temp_probes_count);
    
    /* ���Ͷ����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_MultiPacket(0x07, PGN_BMT, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 1 + bmt_data->temp_probes_count);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BMT_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}

/**
 * @brief ����BMV����ص�ѹ������ (���)
 * @param bmv_data ָ��BMV���ݽṹ��ָ��
 * @return Transport_StatusTypeDef ����״̬
 */
Transport_StatusTypeDef ApplicationLayer_Send_BMV(GBT_BMV_Data* bmv_data) {
    /* BMV PGN: 0x0015, �ɱ䳤�� */
    uint8_t data[101]; // 2 bytes for total cells + up to 100 bytes for voltages (50 * 2)

    data[0] = (uint8_t)(bmv_data->total_cells);
    data[1] = (uint8_t)(bmv_data->total_cells >> 8);
    
    // Pack voltage values
    for (int i = 0; i < bmv_data->total_cells && i < 50; i++) {
        data[2 + i * 2] = (uint8_t)(bmv_data->voltages[i]);
        data[3 + i * 2] = (uint8_t)(bmv_data->voltages[i] >> 8);
    }
    
    /* ���Ͷ����Ϣ */
    Transport_StatusTypeDef status = TransportLayer_Send_MultiPacket(0x07, PGN_BMV, CHARGER_ADDRESS, BMS_SOURCE_ADDRESS, data, 2 + bmv_data->total_cells * 2);
    if (status != TRANSPORT_STATUS_OK) {
        // ������
        ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BMV_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return status;
    }
    return TRANSPORT_STATUS_OK;
}
