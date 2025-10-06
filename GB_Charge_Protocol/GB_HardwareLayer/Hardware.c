/*
 * CAN_Transmit_Receive.c
 *
 *  Created on: 2025.7.24
 *  Author: 83923
 *
 */

#include "Hardware.h"

/* CAN接口 */
static CAN_TypeDef* GB_Charging_CAN = CAN1;

/* 定时器接口 */
static TIM_TypeDef* GB_Charging_TIMER = TIM2;
static uint32_t timestamp = 0;

/* 硬件初始化 */
void Hardware_Init(void) {
    /* 初始化CAN */
    Hardware_CAN_Init();
    
    /* 初始化定时器 */
    Hardware_Timer_Init();
}

/* CAN初始化 */
void Hardware_CAN_Init(void) {
    RCC_APB2PeriphClockCmd(CAN_GPIO_CLK,ENABLE);
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	CAN_InitTypeDef CAN_InitStructure;
	CAN_InitStructure.CAN_TTCM = DISABLE;			/* 禁止时间触发模式（不生成时间戳), T  */
	CAN_InitStructure.CAN_ABOM = DISABLE;			/* 禁止自动总线关闭管理 */
	CAN_InitStructure.CAN_AWUM = DISABLE;			/* 禁止自动唤醒模式 */
	CAN_InitStructure.CAN_NART = DISABLE;			/* 禁止仲裁丢失或出错后的自动重传功能 */
	CAN_InitStructure.CAN_RFLM = DISABLE;			/* 禁止接收FIFO加锁模式 */
	CAN_InitStructure.CAN_TXFP = DISABLE;			/* 禁止传输FIFO优先级 */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	/* 设置CAN为回环模式 */
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
	/* 
		CAN 波特率 = RCC_APB1Periph_CAN1 / Prescaler / (SJW + BS1 + BS2);
		
		SJW = synchronisation_jump_width 
		BS = bit_segment
		
		本例中，设置CAN波特率为1Mbps		
		CAN 波特率 = 36M / 4 / (1 + 4 + 4) / = 1 Mbps		
	*/
	
	CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;
	CAN_InitStructure.CAN_Prescaler = 16;
	CAN_Init(CANx, &CAN_InitStructure);
	//配置默认过滤器
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

/* CAN发送数据 */
CAN_StatusTypeDef Hardware_CAN_Transmit(uint32_t id, uint8_t *data, uint8_t len) {
    CanTxMsg TxMessage;
	TxMessage.StdId = id;//标准ID
	TxMessage.ExtId = id;//扩展ID
	TxMessage.IDE = CAN_Id_Extended;		//标准帧还是扩展帧
	TxMessage.RTR = CAN_RTR_Data;	//遥控帧还是数据帧
	TxMessage.DLC = len;//数据段长度
	//数据段内容
	for (uint8_t i = 0; i < len; i ++)
	{
		TxMessage.Data[i] = data[i];
	}
	//发送报文
	uint8_t TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
	//超时处理函数
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

/* CAN接收数据 */
CAN_StatusTypeDef Hardware_CAN_Receive(uint32_t *id, uint8_t *data, uint8_t *len) {
  CanRxMsg RxMessage;
	if( CAN_MessagePending(CAN1,CAN_FIFO0)==0) return CAN_STATUS_BUSY;		//没有接收到数据,直接退出
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

/* CAN错误处理 */
void Hardware_CAN_Error_Handler(void) {
    uint8_t error_code = CAN_GetLastErrorCode(GB_Charging_CAN);
    
    switch (error_code) {
        case CAN_ErrorCode_NoErr:
            // 无错误
            break;
        case CAN_ErrorCode_StuffErr:
            // 位填充错误
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_FormErr:
            // 格式错误
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_ACKErr:
            // 应答错误
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_BitRecessiveErr:
            // 位隐性错误
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_BitDominantErr:
            // 位显性错误
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        case CAN_ErrorCode_CRCErr:
            // CRC校验错误
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
        default:
            // 未知错误
            ErrorHandling_Report_Error(ERROR_TYPE_HARDWARE, ERROR_CODE_CAN_BUS_OFF, NULL, 0);
					ErrorHandling_Error_Handler();
            break;
    }
    
    /* 清除错误标志 */
    CAN_ClearFlag(GB_Charging_CAN, CAN_FLAG_EWG | CAN_FLAG_EPV | CAN_FLAG_BOF);
}

/* 定时器初始化 */
void Hardware_Timer_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 使能通用定时器时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
		/*定时器单元初始化*/
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频因子选择为不分频，影响定时器时钟频率为72MHz
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式选择为向上计数
		TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1; /* 1ms周期 (72MHz/72000 = 1kHz) */
		TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; /* 72分频 (72MHz/72 = 1MHz) */
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，仅高级定时器有效
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//将结构体参数赋值给TIM_TimeBaseInit函数初始化TIM2定时器单元	
    
    /* 使能更新中断 */
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);	
    TIM_ITConfig(GB_Charging_TIMER, TIM_IT_Update, ENABLE);
    
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;			//选择中断NVIC的TIM2通道
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//指定NVIC线路的抢占优先级为2
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
		NVIC_Init(&NVIC_InitStructure);								//将结构体参数赋值给NVIC_Init函数初始化NVIC寄存器
	
    /* 启动定时器 */
    TIM_Cmd(GB_Charging_TIMER, ENABLE);
}

/* 获取时间戳 */
uint32_t Hardware_Get_Timestamp(void) {
    return timestamp;
}

/* 定时器中断服务函数 */
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(GB_Charging_TIMER, TIM_IT_Update) != RESET) {
        timestamp++;
        TIM_ClearITPendingBit(GB_Charging_TIMER, TIM_IT_Update);
    }
}
