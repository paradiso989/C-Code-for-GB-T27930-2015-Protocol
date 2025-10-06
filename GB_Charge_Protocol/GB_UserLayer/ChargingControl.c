/*
 * ChargingControl.c
 *
 * Created on: 2025.7.24
 * Author: 83923
 * 
 */

#include "ChargingControl.h"


/* ȫ�ֱ������� */
Charging_ControlTypeDef charging_control;
Charging_ParametersTypeDef charging_params;



/**
 * @brief ������ģ���ʼ������
 */
void ChargingControl_Init(void) {
    // ��ʼ�����ƽṹ��
    memset(&charging_control, 0, sizeof(Charging_ControlTypeDef));
    charging_control.current_stage = CHARGING_STAGE_HANDSHAKE;
    charging_control.charging_allowed = false;
    charging_control.stage_timeout = 5000; // Ĭ��5�볬ʱ
    charging_control.timestamp = 0;
    
    // ��ʼ�������ṹ��
    memset(&charging_params, 0, sizeof(Charging_ParametersTypeDef));
    
    // ��ʼ��������ģ��
    ErrorHandling_Init();
    
    // ��ʼ��Ӳ���㶨ʱ��
    Hardware_Init();
}

/**
 * @brief ��ȡ��ǰ���׶�
 * @return Charging_StageTypeDef ��ǰ���׶�
 */
Charging_StageTypeDef ChargingControl_Get_Charging_Stage(void) {
    return charging_control.current_stage;
}

/**
 * @brief ���õ�ǰ���׶�
 * @param stage ���׶�
 */
void ChargingControl_Set_Charging_Stage(Charging_StageTypeDef stage) {
    charging_control.current_stage = stage;
}

/**
 * @brief ���ֽ׶δ�����
 */
void ChargingControl_Handshake_Stage(void) {
    Transport_StatusTypeDef status;
    uint8_t handshake_stage_timer = 0;
    // ��ʼ����ʱ��
    if (handshake_stage_timer == 0) {
        handshake_stage_timer = Hardware_Get_Timestamp();
    }
    
    // �Ƚ���CHM����ֱ�����յ����߳�ʱ�˳�
    do {
        status = ApplicationLayer_Read_CHM(&charging_params.chm_data);
        if (Hardware_Get_Timestamp() - handshake_stage_timer > 5000) { // 5�볬ʱ
            ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
						ErrorHandling_Error_Handler();
            handshake_stage_timer = 0; // ���ö�ʱ��
            return;
        }
    } while (status != TRANSPORT_STATUS_OK);
     
		printf("���ܵ���CHM����\n");
	
    // BHM���ķ��ͣ�����ʽ�ط�BHMֱ���յ�CRM��ʱ��ÿ���ط����250ms
    uint32_t bhm_start_time = Hardware_Get_Timestamp();
    uint32_t last_send_time = 0;
    
    do {
        uint32_t current_time = Hardware_Get_Timestamp();
        
        // �״η��ͻ�ﵽ250ms���ʱ�ط�
        if (current_time - last_send_time >= 250 || last_send_time == 0) {
            ApplicationLayer_Send_BHM(&charging_params.bhm_data);
            last_send_time = current_time;
        }
        
        // ����Ƿ��յ�CRM���Ĳ��ж�SPN2560�Ƿ�Ϊ0x00
        status = ApplicationLayer_Read_CRM(&charging_params.crm_data);
        if (status == TRANSPORT_STATUS_OK && charging_params.crm_data.Recognitio_Result == 0x00) {
            break; // �յ�CRM���ģ��˳�ѭ��
        }
        
        // ���5�볬ʱ
        if (Hardware_Get_Timestamp() - bhm_start_time > 10000) {
            ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
						ErrorHandling_Error_Handler();
            handshake_stage_timer = 0;
            return;
        }
        
    } while (1);
    
		printf("���ܵ���CRM����\n");
		
    // BRM���ķ��ͣ�����ʽ�ط�BRMֱ���յ�CRM��ʱ��ÿ���ط����250ms
    uint32_t brm_start_time = Hardware_Get_Timestamp();
    uint32_t brm_last_send_time = 0;
    
    do {
        uint32_t current_time = Hardware_Get_Timestamp();
        
        // �״η��ͻ�ﵽ250ms���ʱ�ط�
        if (current_time - brm_last_send_time >= 250 || brm_last_send_time == 0) {
            status = ApplicationLayer_Send_BRM(&charging_params.brm_data);
            if (status != TRANSPORT_STATUS_OK) {
                ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
								ErrorHandling_Error_Handler();
                handshake_stage_timer = 0;
                return;
            }
            brm_last_send_time = current_time;
        }
        
        // ����Ƿ��յ�CRM���Ĳ��ж�SPN2560�Ƿ�Ϊ0xAA
        status = ApplicationLayer_Read_CRM(&charging_params.crm_data);
        if (status == TRANSPORT_STATUS_OK && charging_params.crm_data.Recognitio_Result == 0xAA) {
            break; // �յ�CRM���ģ��˳�ѭ��
        }
        
        // ���5�볬ʱ
        if (Hardware_Get_Timestamp() - brm_start_time > 5000) {
            ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
						ErrorHandling_Error_Handler();
            handshake_stage_timer = 0;
            return;
        }
        
    } while (1);
    
    // ����������ֱ��Ķ��ɹ����ͺͽ��գ�����������ý׶�
    ChargingControl_Set_Charging_Stage(CHARGING_STAGE_PARAMETER);
    handshake_stage_timer = 0; // ���ö�ʱ��
}

/**
 * @brief �������ý׶δ�����
 */
void ChargingControl_Parameter_Stage(void) {
    Transport_StatusTypeDef status;

    // BCP���ķ��ͣ�����ʽ�ط�BCPֱ���յ�CML��ʱ��ÿ���ط����500ms
    uint32_t bcp_start_time = Hardware_Get_Timestamp();
    uint32_t bcp_last_send_time = 0;
    
    do {
        uint32_t current_time = Hardware_Get_Timestamp();
        
        // �״η��ͻ�ﵽ500ms���ʱ�ط�
        if (current_time - bcp_last_send_time >= 500 || bcp_last_send_time == 0) {
            status = ApplicationLayer_Send_BCP(&charging_params.bcp_data);
            if (status != TRANSPORT_STATUS_OK) {
                ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
                return;
            }
            bcp_last_send_time = current_time;
        }
        
        //CTS���Ľ��գ����վ��в�������(�����������˴���)
        status = ApplicationLayer_Read_CTS(&charging_params.cts_data);

        // ����Ƿ��յ�CML����
        status = ApplicationLayer_Read_CML(&charging_params.cml_data);
        if (status == TRANSPORT_STATUS_OK) {
            break; // �յ�CML���ģ��˳�ѭ��
        }
        
        // ���5�볬ʱ
        if (Hardware_Get_Timestamp() - bcp_start_time > 5000) {
            ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
					  ErrorHandling_Error_Handler();
            return;
        }
        
    } while (1);

    // BRO���ķ��ͣ�����ʽ�ط�BROֱ���յ�����������CRO��ʱ��ÿ���ط����250ms
    uint32_t bro_start_time = Hardware_Get_Timestamp();
    uint32_t bro_last_send_time = 0;
    
    do {
        uint32_t current_time = Hardware_Get_Timestamp();
        
        // �״η��ͻ�ﵽ250ms���ʱ�ط�
        if (current_time - bro_last_send_time >= 250 || bro_last_send_time == 0) {
            status = ApplicationLayer_Send_BRO(&charging_params.bro_data);
            if (status != TRANSPORT_STATUS_OK) {
                ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
								ErrorHandling_Error_Handler();
                return;
            }
            bro_last_send_time = current_time;
        }
        
        // ����Ƿ��յ�����������CRO����
        status = ApplicationLayer_Read_CRO(&charging_params.cro_data);
        if (status == TRANSPORT_STATUS_OK && charging_params.cro_data.charging_prepare_status == 0xAA) {
            break; // �յ�����������CRO���ģ��˳�ѭ��
        }
        
        // ���5�볬ʱ
        if (Hardware_Get_Timestamp() - bro_start_time > 5000) {
            ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
            return;
        }
        
    } while (1);

    // ������в������ñ��Ķ��ɹ����ͺͽ��գ�������׶�
    ChargingControl_Set_Charging_Stage(CHARGING_STAGE_CHARGING);


}

/**
 * @brief ���׶δ�����
 */
void ChargingControl_Charging_Stage(void) {
    Transport_StatusTypeDef status;
    
    // ���׶Σ�����ѭ���շ�BCL/BCS/CCS/BSM����
    uint32_t bcl_last_send_time = 0;
    uint32_t bcs_last_send_time = 0;
    uint32_t bsm_last_send_time = 0;
    uint32_t ccs_timeout_start = 0;  // CCS��ʱ��ʱ���
    bool waiting_for_ccs = false;    // �Ƿ����ڵȴ�CCS��Ӧ
    bool sending_bsm_periodically = false;  // �Ƿ����������Է���BSM����
    
    while (1) 
		{
        uint32_t current_time = Hardware_Get_Timestamp();
 
        // ÿ50ms����BCL����
        if (current_time - bcl_last_send_time >= 50 || bcl_last_send_time == 0) {
            // ����BCL����
            status = ApplicationLayer_Send_BCL(&charging_params.bcl_data);
            if (status != TRANSPORT_STATUS_OK) {
                ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
								ErrorHandling_Error_Handler();
                return;
            }
            
            // ����ǵ�һ�η���BCL�����յ�CCS���һ�η���BCL������CCS��ʱ��ʱ
            if (!waiting_for_ccs) {
                ccs_timeout_start = current_time;
                waiting_for_ccs = true;
            }
            
            bcl_last_send_time = current_time;
        }
        
        // ������ʽ���CCS��Ӧ
        if (waiting_for_ccs) {
            status = ApplicationLayer_Read_CCS(&charging_params.ccs_data);
            if (status == TRANSPORT_STATUS_OK) {
                sending_bsm_periodically = true;  // ��ʼ�����Է���BSM����
                // ����CCS�ȴ�״̬���´η���BCLʱ���¿�ʼ��ʱ
                waiting_for_ccs = false;
            }
            
            // ���1�볬ʱ
            if (current_time - ccs_timeout_start > 1000) {
                ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
                ErrorHandling_Error_Handler();
                return;
            }
        }
        
        // ÿ250ms����BCS����
        if (current_time - bcs_last_send_time >= 250 || bcs_last_send_time == 0) {
            // ����BCS����
            status = ApplicationLayer_Send_BCS(&charging_params.bcs_data);
            if (status != TRANSPORT_STATUS_OK) {
                ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
                ErrorHandling_Error_Handler();
                return;
            }
            bcs_last_send_time = current_time;
        }
        
        // ����յ�CCS���ģ���ÿ250ms����BSM����
        if (sending_bsm_periodically) {
            if (current_time - bsm_last_send_time >= 250 || bsm_last_send_time == 0) {
                // ����BSM����
                status = ApplicationLayer_Send_BSM(&charging_params.bsm_data);
                if (status != TRANSPORT_STATUS_OK) {
                    ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
                    ErrorHandling_Error_Handler();
                    //return;
                }
                bsm_last_send_time = current_time;
            }
        }
        
        // ����Ƿ��յ�CST���ģ���������飩
        status = ApplicationLayer_Read_CST(&charging_params.cst_data);
        if (status == TRANSPORT_STATUS_OK) {
            // �յ�CST���ģ���������׶�
            ChargingControl_Set_Charging_Stage(CHARGING_STAGE_END);
            return;
        }
        
        // ����Ƿ���Ҫ����������ֹ���
        if (charging_params.bsm_data.batVolState == 0x01 ||  // ��ص�ѹ����
            charging_params.bsm_data.batTempOverState == 0x01 ||  // ����¶ȹ���
            charging_params.bsm_data.batInsulationState == 0x01) { // ��ؾ�Ե����
            
            // BST���ķ��ͣ�����ʽ�ط�BSTֱ�����ͳɹ���ʱ
            uint32_t bst_start_time = Hardware_Get_Timestamp();
            do {
                status = ApplicationLayer_Send_BST(&charging_params.bst_data);
                if (status == TRANSPORT_STATUS_OK) {
                    break; // ���ͳɹ����˳�ѭ��
                }
                
                // ���5�볬ʱ
                if (Hardware_Get_Timestamp() - bst_start_time > 5000) {
                    ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
										ErrorHandling_Error_Handler();
                    return;
                }
            } while (1);
            
            // CST���Ľ��գ�����ʽ�ȴ�ֱ���յ�CST��ʱ
            uint32_t cst_start_time = Hardware_Get_Timestamp();
            do {
                status = ApplicationLayer_Read_CST(&charging_params.cst_data);
                if (status == TRANSPORT_STATUS_OK) {
                    break; // �յ�CST���ģ��˳�ѭ��
                }
                
                // ���5�볬ʱ
                if (Hardware_Get_Timestamp() - cst_start_time > 5000) {
                    ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
										ErrorHandling_Error_Handler();
                    return;
                }
            } while (1);
            
            // ��ֹ��磬��������׶�
            ChargingControl_Set_Charging_Stage(CHARGING_STAGE_END);
            return;
        }
    }
}

/**
 * @brief �����׶δ�����
 */
void ChargingControl_End_Stage(void) {
    Transport_StatusTypeDef status;
    bool end_condition_met = false;
    
    // �����׶Σ�����ʽ�ط�BSD���ģ�ÿ250ms��������10��
    uint32_t end_start_time = Hardware_Get_Timestamp();
    uint32_t bsd_last_send_time = 0;
    uint8_t bsd_send_count = 0;
    //uint8_t csd_receive_count = 0;
    
    while (bsd_send_count < 10) {
        uint32_t current_time = Hardware_Get_Timestamp();
        
        // ���5���ܳ�ʱ
        if (current_time - end_start_time > 5000) {
            ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
						ErrorHandling_Error_Handler();
            return;
        }

        // �״η��ͻ�ﵽ250ms���ʱ�ط�BSD
        if (current_time - bsd_last_send_time >= 250 || bsd_last_send_time == 0) {
            status = ApplicationLayer_Send_BSD(&charging_params.bsd_data);
            if (status != TRANSPORT_STATUS_OK) {
                ErrorHandling_Report_Error(ERROR_TYPE_APPLICATION, ERROR_CODE_APP_BSD_FAILED, NULL, 0);
								ErrorHandling_Error_Handler();
            } 
        }
        
        // ����Ƿ��յ�CSD����
        status = ApplicationLayer_Read_CSD(&charging_params.csd_data);
        if (status == TRANSPORT_STATUS_OK) {
            end_condition_met = true; // �յ�3��CSD�������������
            break; // �˳�ѭ��
        }
    }

    // ���δ����������������磬�ﵽ����ʹ�����δ�յ��㹻CSD�����򱨸泬ʱ
    if (!end_condition_met) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_COMM_TIMEOUT, NULL, 0);
				ErrorHandling_Error_Handler();
    }

    // ���õ����ֽ׶Σ�׼����һ�γ��
    ChargingControl_Set_Charging_Stage(CHARGING_STAGE_HANDSHAKE);
}

/**
 * @brief ��������������
 * ʵ�ּ��弴�õĳ������߼�
 */
void ChargingControl_Process(void) {  
	if(ErrorHandling_Has_Error() == false)
		{
			// ��鵱ǰ���׶β�������Ӧ�Ĵ�����
			switch (ChargingControl_Get_Charging_Stage()) {
					case CHARGING_STAGE_HANDSHAKE:
							printf("���ֽ׶ο�ʼ\n");
							ChargingControl_Handshake_Stage();
							break;
					
					case CHARGING_STAGE_PARAMETER:
							printf("�������ý׶ο�ʼ\n");
							ChargingControl_Parameter_Stage();
							break;
					
					case CHARGING_STAGE_CHARGING:
							printf("���׶ο�ʼ\n");
							ChargingControl_Charging_Stage();
							break;
					
					case CHARGING_STAGE_END:
							printf("�����׶ο�ʼ\n");
							ChargingControl_End_Stage();
							break;
					
					default:
							// ����׶�״̬�쳣�����õ����ֽ׶�
							//ChargingControl_Set_Charging_Stage(CHARGING_STAGE_HANDSHAKE);
							break;
			}
		}
}
