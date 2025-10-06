/*
 * TransportLayer.c
 *
 * Created on: 2025.7.24
 * Author: 83923
 *
 * Based On Open_SAE_J1939
 * https://github.com/DanielMartensson/Open-SAE-J1939 
 */

#include "TransportLayer.h"

/* ȫ�ֱ��� */
static uint8_t tp_cm_buffer[TP_PACKET_SIZE];
static uint8_t tp_dt_buffer[TP_MAX_PACKET_SIZE];

/* �ڲ��������� */
static Transport_StatusTypeDef send_tp_cm(uint8_t priority, uint8_t da, uint8_t sa, uint8_t control_byte, uint16_t total_size, uint8_t packet_count, uint32_t pgn);
static Transport_StatusTypeDef send_tp_dt(uint8_t priority, uint8_t da, uint8_t sa, uint8_t sequence, uint8_t *data, uint8_t len);

/* ������ʼ�� */
void TransportLayer_Init(void) {
    // ��ʼ���������ز���
    for (uint16_t i = 0; i < TP_PACKET_SIZE; i++) {
        tp_cm_buffer[i] = 0;
    }
    for (uint16_t i = 0; i < TP_MAX_PACKET_SIZE; i++) {
        tp_dt_buffer[i] = 0;
    }
}

/*
 * ���͵�����Ϣ
 * ����:
 *   priority: ���ȼ� (0-7, 0Ϊ���)
 *   pgn: ��������
 *   da: Ŀ���ַ
 *   sa: Դ��ַ
 *   data: ����ָ��
 *   len: ���ݳ���
 */
Transport_StatusTypeDef TransportLayer_Send_Message(uint8_t priority, uint32_t pgn, uint8_t da, uint8_t sa, uint8_t *data, uint8_t len) {
    if (data == NULL || len > TP_PACKET_SIZE) {
        return TRANSPORT_STATUS_INVALID_PARAM;
    }
    
    // ����CAN ID
    uint32_t can_id = PGN_TO_CAN_ID(priority, pgn, da, sa);
    
    // ͨ��Ӳ���㷢��
    return (Transport_StatusTypeDef)Hardware_CAN_Transmit(can_id, data, len);
}

/*
 * ���յ�����Ϣ
 * ����:
 *   pgn: ��������ָ��
 *   data: ����ָ��
 *   len: ���ݳ���ָ��
 *   da: Ŀ���ַָ�� (��ΪNULL)
 *   sa: Դ��ַָ�� (��ΪNULL)
 *   priority: ���ȼ�ָ�� (��ΪNULL)
 */
Transport_StatusTypeDef TransportLayer_Receive_Message(uint32_t *pgn, uint8_t *data, uint8_t *len, uint8_t *da, uint8_t *sa, uint8_t *priority) {
    if (pgn == NULL || data == NULL || len == NULL) {
        return TRANSPORT_STATUS_INVALID_PARAM;
    }
    
    // ͨ��Ӳ�������
    uint32_t can_id;
    CAN_StatusTypeDef status = Hardware_CAN_Receive(&can_id, data, len);
	
    // ��CAN ID����ȡPGN��DA��SA�����ȼ�
    if (status == CAN_STATUS_OK) {
        *pgn = CAN_ID_TO_PGN(can_id);
        if (da != NULL) *da = CAN_ID_TO_DA(can_id);
        if (sa != NULL) *sa = CAN_ID_TO_SA(can_id);
        if (priority != NULL) *priority = CAN_ID_TO_PRIORITY(can_id);
    }
    
    return (Transport_StatusTypeDef)status;
}

/*
 * ���Ͷ����Ϣ
 * ����:
 *   priority: ���ȼ� (0-7, 0Ϊ���)
 *   pgn: ��������
 *   da: Ŀ���ַ
 *   sa: Դ��ַ
 *   data: ����ָ��
 *   len: ���ݳ���
 */
/**
 * @brief ������ͺ��� - ʵ��GB/T 27930Э��Ķ�����ݴ���
 * ����J1939-21����Э���׼��ͨ��RTS/CTS���ֻ���ʵ�ֿɿ����ݴ���
 * @param priority ��Ϣ���ȼ�(0-7��0Ϊ���)
 * @param pgn ��������
 * @param da Ŀ���ַ
 * @param sa Դ��ַ
 * @param data ���������ݻ�����
 * @param len ���ݳ���
 * @return ����״̬
 */
Transport_StatusTypeDef TransportLayer_Send_MultiPacket(uint8_t priority, uint32_t pgn, uint8_t da, uint8_t sa, uint8_t *data, uint8_t len) {
    if (data == NULL || len == 0) 
	{
        return TRANSPORT_STATUS_INVALID_PARAM;
    }

    // �������ݰ�����(����ȡ��)
    uint8_t packet_count = (len + TP_PACKET_DATA_SIZE - 1) / TP_PACKET_DATA_SIZE;

    // ����RTS(������)���� - ���������������
    if (send_tp_cm(priority, da, sa, 0x10, len, packet_count, pgn) != TRANSPORT_STATUS_OK) {
        ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_ERROR;
    }

    // �ȴ�CTS(������)���� - ��ʱʱ��1��
    uint32_t received_pgn;
    uint8_t received_data[TP_PACKET_SIZE];
    uint32_t start_time = Hardware_Get_Timestamp();
    uint32_t timeout = 1000; // 1�볬ʱʱ��
    
    // ѭ���ȴ�CTS��Ӧ
    while ((Hardware_Get_Timestamp() - start_time) < timeout) {
        uint32_t received_can_id;
        uint8_t temp_len;
        // ����CAN����
        if (Hardware_CAN_Receive(&received_can_id, received_data, &temp_len) == CAN_STATUS_OK) {
            // ��CAN ID��ȡPGN
            received_pgn = CAN_ID_TO_PGN(received_can_id);
            // ����Ƿ�ΪCTS����(�����ֽ�0x11)
            if (received_pgn == PGN_TP_CM && received_data[0] == 0x11) {
                break; // �յ�CTS���˳��ȴ�
            }
        }
    }

    // ����Ƿ�ʱ
    if ((Hardware_Get_Timestamp() - start_time) >= timeout) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
		ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_TIMEOUT;
    }

     // �������ݰ� - ����ŷ������зְ�
    for (uint8_t i = 0; i < packet_count; i++) 
	{
        uint8_t packet_data[TP_PACKET_DATA_SIZE];
        // ���㵱ǰ������(���һ�����ܲ���)
        uint8_t packet_len = (len - i * TP_PACKET_DATA_SIZE) > TP_PACKET_DATA_SIZE ? TP_PACKET_DATA_SIZE : (len - i * TP_PACKET_DATA_SIZE);
        // �������ݵ����ͻ�����
        for (uint8_t j = 0; j < packet_len; j++) {
            packet_data[j] = data[i * TP_PACKET_DATA_SIZE + j];
        }
        // �������ݴ��䱨��(DT)
        if (send_tp_dt(7, da, sa, i+1, packet_data, packet_len) != TRANSPORT_STATUS_OK) {
            ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
			ErrorHandling_Error_Handler();
            return TRANSPORT_STATUS_ERROR;
        }
    }

    // �ȴ�ȷ�ϱ���(END) - ��ʱʱ��1��
    start_time = Hardware_Get_Timestamp();
    timeout = 1000; // 1�볬ʱʱ��
    
    while ((Hardware_Get_Timestamp() - start_time) < timeout) {
        uint32_t received_can_id;
        uint8_t temp_len;
        if (Hardware_CAN_Receive(&received_can_id, received_data, &temp_len) == CAN_STATUS_OK) {
            received_pgn = CAN_ID_TO_PGN(received_can_id);
            // ����Ƿ�ΪEND����(�����ֽ�0x13)
            if (received_pgn == PGN_TP_CM && received_data[0] == 0x13) 
						{
                return TRANSPORT_STATUS_OK; // ����ɹ�
            }
        }
    }

    // ��ʱ����
    if ((Hardware_Get_Timestamp() - start_time) >= timeout) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
				ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_TIMEOUT;
    }

    ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
    return TRANSPORT_STATUS_ERROR;
}

/*
 * ���ն����Ϣ
 * ����:
 *   pgn: ��������ָ��
 *   data: ����ָ��
 *   len: ���ݳ���ָ��
 *   da: Ŀ���ַ (���ڷ���CTS��ȷ�ϱ���)
 *   sa: Դ��ַ (���ڷ���CTS��ȷ�ϱ���)
 *   priority: ���ȼ� (0-7, 0Ϊ��ߣ����ڷ���CTS��ȷ�ϱ���)6
 
 */
Transport_StatusTypeDef TransportLayer_Receive_MultiPacket(uint8_t priority, uint32_t *pgn, uint8_t *data, uint8_t *len, uint8_t da, uint8_t sa) {
    if (pgn == NULL || data == NULL || len == NULL) {
        return TRANSPORT_STATUS_INVALID_PARAM;
    }
    
    // �������ӹ����� - RTS
    uint32_t received_pgn;
    uint8_t received_data[TP_PACKET_SIZE];

    // �ȴ�RTS���ģ���ʱʱ��Ϊ1000ms
    uint32_t start_time = Hardware_Get_Timestamp();
    uint32_t timeout = 1000; // 1�볬ʱʱ��
    
    while ((Hardware_Get_Timestamp() - start_time) < timeout) {
        uint32_t received_can_id;
        uint8_t temp_len;
        if (Hardware_CAN_Receive(&received_can_id, received_data, &temp_len) == CAN_STATUS_OK) {
            // ��CAN ID����ȡPGN
            received_pgn = CAN_ID_TO_PGN(received_can_id);
            // ����Ƿ�ΪRTS����(�����ֽ�0x10)
            if (received_pgn == PGN_TP_CM && received_data[0] == 0x10) {
                break;
            }
        }
    }

    // ����Ƿ�ʱ
    if ((Hardware_Get_Timestamp() - start_time) >= timeout) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
				ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_TIMEOUT;
    }

    // ����RTS��������
    uint32_t tp_pgn = (received_data[7] << 16) | (received_data[6] << 8) | received_data[5];
    uint16_t tp_data_len = (received_data[2] << 8) | received_data[1];
    uint8_t packet_count = received_data[3];

    // ����CTS(������)���� - ��ӦRTS����
    if (send_tp_cm(priority, da, sa, 0x11, tp_data_len, packet_count, tp_pgn) != TRANSPORT_STATUS_OK) {
        ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
				ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_ERROR;
    }

    // �������ݰ� - ����Ž������зְ�
    for (uint8_t i = 0; i < packet_count; i++) {
        uint32_t dt_pgn;
        uint8_t dt_data[TP_PACKET_SIZE];
        uint8_t dt_len;
        timeout = 0;

        // �ȴ����ݰ�����ʱʱ��Ϊ1000ms
        uint32_t start_time = Hardware_Get_Timestamp();
        uint32_t timeout = 1000; // 1�볬ʱʱ��
        
        while ((Hardware_Get_Timestamp() - start_time) < timeout) {
            uint32_t dt_can_id;
            uint8_t temp_len;
            if (Hardware_CAN_Receive(&dt_can_id, dt_data, &temp_len) == CAN_STATUS_OK) {
                // ��CAN ID����ȡPGN
                dt_pgn = CAN_ID_TO_PGN(dt_can_id);
                dt_len = temp_len;
                // ����Ƿ�Ϊ���ݴ��䱨�������ƥ��
                if (dt_pgn == PGN_TP_DT && dt_data[0] == (i + 1)) {
                    break;
                }
            }
        }

        // ����Ƿ�ʱ
        if ((Hardware_Get_Timestamp() - start_time) >= timeout) {
            ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
						ErrorHandling_Error_Handler();
            return TRANSPORT_STATUS_TIMEOUT;
        }

        // ��������(��������ֽ�)
        for (uint8_t j = 0; j < dt_len - 1; j++) {
            tp_dt_buffer[i * (TP_PACKET_SIZE - 1) + j] = dt_data[1 + j];
        }

        // ���ݰ�֮����ռ��Ϊ10ms
        uint32_t delay_start = Hardware_Get_Timestamp();
        while ((Hardware_Get_Timestamp() - delay_start) < 10); // 10ms���
    }

    // ����END���� - ȷ�ϴ������
    uint32_t end_start_time = Hardware_Get_Timestamp();
    uint32_t end_timeout = 1000; // 1�볬ʱʱ��
    
    while ((Hardware_Get_Timestamp() - end_start_time) < end_timeout) {
        uint32_t received_can_id;
        uint8_t temp_len;
        if (Hardware_CAN_Receive(&received_can_id, received_data, &temp_len) == CAN_STATUS_OK) {
            // ��CAN ID����ȡPGN
            received_pgn = CAN_ID_TO_PGN(received_can_id);
            // ����Ƿ�ΪEND����(�����ֽ�0x13)
            if (received_pgn == PGN_TP_CM && received_data[0] == 0x13) {
                break;
            }
        }
    }

    // ����Ƿ�ʱ
    if ((Hardware_Get_Timestamp() - end_start_time) >= end_timeout) {
        ErrorHandling_Report_Error(ERROR_TYPE_TIMEOUT, ERROR_CODE_TP_TIMEOUT, NULL, 0);
		ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_TIMEOUT;
    }

    // ����ȷ�ϱ���
    if (send_tp_cm(priority, da, sa, 0x13, tp_data_len, packet_count, tp_pgn) != TRANSPORT_STATUS_OK) {
        ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
		ErrorHandling_Error_Handler();
        return TRANSPORT_STATUS_ERROR;
    }

    // �������ݸ��ϲ�
    *pgn = tp_pgn;
    *len = tp_data_len;
    for (uint16_t i = 0; i < tp_data_len; i++) {
        data[i] = tp_dt_buffer[i];
    }

    return TRANSPORT_STATUS_OK;
}

/* �������ӹ����� */
/* RTS��ʽ��
 *   data[0] - �����ֽڣ�RTS/CTS/EndOfMsgACK�ȣ�
 *   data[1-2] - ����Ϣ��С����λ��ǰ��
 *   data[3] - ���ݰ�����
 *   data[4] - ����λ��0xFF��
 *   data[5-7] - �����Ϣ��PGN����λ��ǰ��
 */
static Transport_StatusTypeDef send_tp_cm(uint8_t priority, uint8_t da, uint8_t sa, uint8_t control_byte, uint16_t total_size, uint8_t packet_count, uint32_t pgn) {
    uint8_t data[TP_PACKET_SIZE];
    
    data[0] = control_byte;
    data[1] = total_size & 0xFF;
    data[2] = (total_size >> 8) & 0xFF;
    data[3] = packet_count;
    data[4] = 0xFF; // �����ֶ�
    data[5] = 0x00;
    data[6] = pgn;
    data[7] = 0x00;
    
    // ����CM���ĵ�CAN ID
    uint32_t cm_can_id = PGN_TO_CAN_ID(priority, PGN_TP_CM, da, sa);
    
    return (Transport_StatusTypeDef)Hardware_CAN_Transmit(cm_can_id, data, TP_PACKET_SIZE);
}

/* �������ݴ��䱨�� */
static Transport_StatusTypeDef send_tp_dt(uint8_t priority, uint8_t da, uint8_t sa, uint8_t sequence, uint8_t *data, uint8_t len) {
    uint8_t packet_data[TP_PACKET_SIZE];
    
    packet_data[0] = sequence;
	//��1��ʼ�洢
    for (uint8_t i = 0; i < len; i++) {
        packet_data[i+1] = data[i];
    }
    
    // ������ݲ���7�ֽڣ����0xFF
    for (uint8_t i = len + 1; i <= TP_PACKET_DATA_SIZE; i++) {
        packet_data[i] = 0xFF;
    }
    
    // ����DT���ĵ�CAN ID
    uint32_t dt_can_id = PGN_TO_CAN_ID(priority, PGN_TP_DT, da, sa);
    
    return (Transport_StatusTypeDef)Hardware_CAN_Transmit(dt_can_id, packet_data, TP_PACKET_SIZE);
}

/* ���������� */
void TransportLayer_Error_Handler(void) {
    // �����������߼�
    // ������������Ӹ���ϸ�Ĵ�����
    Hardware_CAN_Error_Handler();
    
    // ���洫������
    ErrorHandling_Report_Error(ERROR_TYPE_TRANSPORT, ERROR_CODE_TP_TRANSMIT_FAILED, NULL, 0);
	ErrorHandling_Error_Handler();
}
