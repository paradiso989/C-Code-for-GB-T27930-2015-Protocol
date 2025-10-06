/*
 * Hardware.h
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 *
 *  This example is based on STM32F103ZET6
 */

#ifndef __HARDWARE_H_
#define __HARDWARE_H_

#include "stm32f10x.h"
#include "ErrorHandling.h"

//CAN1����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	0		//0,��ʹ��;1,ʹ��.

#define CANx 					CAN1
#define CAN_CLK					RCC_APB1Periph_CAN1

/************���Ŷ���***************/
#define CAN_TX_GPIO_PROT  		GPIOA
#define CAN_TX_GPIO_PIN   		GPIO_Pin_12

#define  CAN_RX_GPIO_PORT     GPIOA
#define  CAN_RX_GPIO_PIN      GPIO_Pin_11

#define CAN_GPIO_CLK		RCC_APB2Periph_GPIOA

/* CAN״̬�� */
typedef enum {
    CAN_STATUS_OK = 0,
    CAN_STATUS_ERROR,
    CAN_STATUS_BUSY,
    CAN_STATUS_TIMEOUT
} CAN_StatusTypeDef;

/* �������� */
void Hardware_Init(void);
void Hardware_CAN_Init(void);
CAN_StatusTypeDef Hardware_CAN_Transmit(uint32_t id, uint8_t *data, uint8_t len);
CAN_StatusTypeDef Hardware_CAN_Receive(uint32_t *id, uint8_t *data, uint8_t *len);
void Hardware_CAN_Error_Handler(void);

/* ��ʱ����غ������� */
void Hardware_Timer_Init(void);
uint32_t Hardware_Get_Timestamp(void);


#endif /* GB_CHARGING_PROTOCOL_HARDWARE_HARDWARE_H_ */
