/*
 * TransportLayer.h
 *
 * Created on: 2025.7.24
 * Author: 83923
 *
 * Based On Open_SAE_J1939
 * https://github.com/DanielMartensson/Open-SAE-J1939
 */

#ifndef __TRANSPORTLAYER_H_
#define __TRANSPORTLAYER_H_

#include <stdint.h>
#include <stdbool.h>
#include "Hardware.h"
#include "ErrorHandling.h"
#include <string.h>

/* �����״̬�� */
typedef enum {
    TRANSPORT_STATUS_OK = 0,
    TRANSPORT_STATUS_ERROR,
    TRANSPORT_STATUS_BUSY,
    TRANSPORT_STATUS_TIMEOUT,
    TRANSPORT_STATUS_INVALID_PARAM
} Transport_StatusTypeDef;

/* ����������ض��� */
#define TP_PACKET_SIZE	8

/* ���������ض��� */
#define TP_MAX_PACKET_SIZE     10      /* ��������ݳ��� */
#define TP_PACKET_DATA_SIZE         7         /* �������ݳ���(��һ�ֽ��ǰ����,����ÿ�����7���ֽڵ�����) */

/* PGN���� */
#define PGN_TP_CM              0xEC /* ����Э�����ӹ���PGN */
#define PGN_TP_DT              0xEB /* ����Э�����ݴ���PGN */

/* PGN��CAN ID��ת���� (����GB/T 27930Э��) */
#define R 0x00         /* ����λ(1λ): 0 */
#define DP 0x00        /* ����ҳ(1λ): 0 */
#define PGN_TO_CAN_ID(priority, pgn, da, sa) ( ((priority) << 26) | (R << 25) | (DP << 24) | (pgn << 16) | (da << 8) | sa )

/* ����˵��: 
 * priority: ���ȼ�(3λ): 0-7, 0Ϊ���
 * pgn: �������� (24λ)
 * da: Ŀ���ַ (8λ)
 * sa: Դ��ַ (8λ)
 */
#define CAN_ID_TO_PGN(can_id)      (((can_id) >> 16) & 0xFF)
#define CAN_ID_TO_DA(can_id)       (((can_id) >> 8) & 0xFF)
#define CAN_ID_TO_SA(can_id)       ((can_id) & 0xFF)
#define CAN_ID_TO_PRIORITY(can_id) (((can_id) >> 26) & 0x07)


/* �������� */
void TransportLayer_Init(void);
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
Transport_StatusTypeDef TransportLayer_Send_Message(uint8_t priority, uint32_t pgn, uint8_t da, uint8_t sa, uint8_t *data, uint8_t len);
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
Transport_StatusTypeDef TransportLayer_Receive_Message(uint32_t *pgn, uint8_t *data, uint8_t *len, uint8_t *da, uint8_t *sa, uint8_t *priority);
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
Transport_StatusTypeDef TransportLayer_Send_MultiPacket(uint8_t priority, uint32_t pgn, uint8_t da, uint8_t sa, uint8_t *data, uint8_t len);
/*
 * ���ն����Ϣ
 * ����:
 *   priority: ���ȼ� (0-7, 0Ϊ���)
 *   pgn: ��������ָ��
 *   data: ����ָ��
 *   len: ���ݳ���ָ��
 *   da: Ŀ���ַ (���ڷ���CTS��ȷ�ϱ���)
 *   sa: Դ��ַ (���ڷ���CTS��ȷ�ϱ���)
 */
Transport_StatusTypeDef TransportLayer_Receive_MultiPacket(uint8_t priority, uint32_t *pgn, uint8_t *data, uint8_t *len, uint8_t da, uint8_t sa);
void TransportLayer_Error_Handler(void);

#endif /* GB_CHARGING_PROTOCOL_TRANSPORTLAYER_TRANSPORTLAYER_H_ */
