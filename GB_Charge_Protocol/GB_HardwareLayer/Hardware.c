/*
 * CAN_Transmit_Receive.c
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 *
 */

#include "Hardware.h"

/* CAN�ӿ� */
static CAN_TypeDef* GB_Charging_CAN = CAN1;

/* ��ʱ���ӿ� */
static TIM_TypeDef* GB_Charging_TIMER = TIM2;
static uint32_t timestamp = 0;

/* Ӳ����ʼ�� */
void Hardware_Init(void) {
    /* ��ʼ��CAN */
    Hardware_CAN_Init();
    
    /* ��ʼ����ʱ�� */
    Hardware_Timer_Init();
}

/* CAN��ʼ�� */
void Hardware_CAN_Init(void) {
    RCC_APB2PeriphClockCmd(CAN_GPIO_CLK,ENABLE);
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	CAN_InitTypeDef CAN_InitStructure;
	CAN_InitStructure.CAN_TTCM = DISABLE;			/* ��ֹʱ�䴥��ģʽ��������ʱ���), T  */
	CAN_InitStructure.CAN_ABOM = DISABLE;			/* ��ֹ�Զ����߹رչ��� */
	CAN_InitStructure.CAN_AWUM = DISABLE;			/* ��ֹ�Զ�����ģʽ */
	CAN_InitStructure.CAN_NART = DISABLE;			/* ��ֹ�ٲö�ʧ��������Զ��ش����� */
	CAN_InitStructure.CAN_RFLM = DISABLE;			/* ��ֹ����FIFO����ģʽ */
	CAN_InitStructure.CAN_TXFP = DISABLE;			/* ��ֹ����FIFO���ȼ� */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	/* ����CANΪ�ػ�ģʽ */
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
	/* 
		CAN ������ = RCC_APB1Periph_CAN1 / Prescaler / (SJW + BS1 + BS2);
		
		SJW = synchronisation_jump_width 
		BS = bit_segment
		
		�����У�����CAN������Ϊ1Mbps		
		CAN ������ = 36M / 4 / (1 + 4 + 4) / = 1 Mbps		
	*/
	
	CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
	CAN_InitStructure.CAN_Prescaler = 16;
	CAN_Init(CANx, &CAN_InitStructure);
	//����Ĭ�Ϲ�����
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}

/* CAN�������� */
CAN_StatusTypeDef Hardware_CAN_Transmit(uint32_t id, uint8_t *data, uint8_t len) {
    CanTxMsg TxMessage;
	TxMessage.StdId = id;//��׼ID
	TxMessage.ExtId = id;//��չID
	TxMessage.IDE = CAN_Id_Extended;		//��׼֡������չ֡
	TxMessage.RTR = CAN_RTR_Data;	//ң��֡��������֡
	TxMessage.DLC = len;//���ݶγ���
	//���ݶ�����
	for (uint8_t i = 0; i < len; i ++)
	{
		TxMessage.Data[i] = data[i];
	}
	//���ͱ���
	uint8_t TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
	//��ʱ������
	uint32_t Timeout = 0;
	while (CAN_TransmitStatus(CAN1, TransmitMailbox) != CAN_TxStatus_Ok)
	{
		Timeout ++;
		if (Timeout > 10000)
		{
			return CAN_STATUS_TIMEOUT;
		}
	}
	return CAN_STATUS_OK;
}

/* CAN�������� */
CAN_StatusTypeDef Hardware_CAN_Receive(uint32_t *id, uint8_t *data, uint8_t *len) {
  CanRxMsg RxMessage;
	if( CAN_MessagePending(CAN1,CAN_FIFO0)==0) return CAN_STATUS_BUSY;		//û�н��յ�����,ֱ���˳�
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	if (RxMessage.IDE == CAN_Id_Standard)
	{
		*id = RxMessage.StdId;
	}
	else
	{
		*id = RxMessage.ExtId;
	}
	*len = RxMessage.DLC;
	for (uint8_t i = 0; i < RxMessage.DLC; i ++)
	{
		data[i] = RxMessage.Data[i];
	}
	return CAN_STATUS_OK;
}

/* CAN������ */
void Hardware_CAN_Error_Handler(void) {
    uint8_t error_code = CAN_GetLastErrorCode(GB_Charging_CAN);
    
    switch (error_code) {
        case CAN_ErrorCode_NoErr:
            // �޴���
            break;
        case CAN_ErrorCode_StuffErr:
            // λ������
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_FormErr:
            // ��ʽ����
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_ACKErr:
            // Ӧ�����
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_BitRecessiveErr:
            // λ���Դ���
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_BitDominantErr:
            // λ���Դ���
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_CRCErr:
            // CRCУ�����
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        default:
            // δ֪����
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
    }
    
    /* ��������־ */
    CAN_ClearFlag(GB_Charging_CAN, CAN_FLAG_EWG | CAN_FLAG_EPV | CAN_FLAG_BOF);
}

/* ��ʱ����ʼ�� */
void Hardware_Timer_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* ʹ��ͨ�ö�ʱ��ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
		/*��ʱ����Ԫ��ʼ��*/
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//ʱ�ӷ�Ƶ����ѡ��Ϊ����Ƶ��Ӱ�춨ʱ��ʱ��Ƶ��Ϊ72MHz
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//������ģʽѡ��Ϊ���ϼ���
		TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1; /* 1ms���� (72MHz/72000 = 1kHz) */
		TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; /* 72��Ƶ (72MHz/72 = 1MHz) */
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//�ظ������������߼���ʱ����Ч
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//���ṹ�������ֵ��TIM_TimeBaseInit������ʼ��TIM2��ʱ����Ԫ	
    
    /* ʹ�ܸ����ж� */
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);	
    TIM_ITConfig(GB_Charging_TIMER, TIM_IT_Update, ENABLE);
    
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;			//ѡ���ж�NVIC��TIM2ͨ��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//ָ��NVIC��·ʹ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//ָ��NVIC��·����ռ���ȼ�Ϊ2
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//ָ��NVIC��·����Ӧ���ȼ�Ϊ1
		NVIC_Init(&NVIC_InitStructure);								//���ṹ�������ֵ��NVIC_Init������ʼ��NVIC�Ĵ���
	
    /* ������ʱ�� */
    TIM_Cmd(GB_Charging_TIMER, ENABLE);
}

/* ��ȡʱ��� */
uint32_t Hardware_Get_Timestamp(void) {
    return timestamp;
}

/* ��ʱ���жϷ����� */
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(GB_Charging_TIMER, TIM_IT_Update) != RESET) {
        timestamp++;
        TIM_ClearITPendingBit(GB_Charging_TIMER, TIM_IT_Update);
    }
}
