#include "bsp_usart.h"

// static void NVIC_Configuration(void)
// {
// 	NVIC_InitTypeDef NVIC_InitStructure;
	
// 	//优先级分组
// 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
// 	//中断源，优先级，使能
// 	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	
// 	NVIC_Init(&NVIC_InitStructure);
// }

void USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//打开串口GPIO时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	//打开串口外设时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_CLK, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT,&GPIO_InitStruct);
	
	//配置串口
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(DEBUG_USARTx,&USART_InitStructure);
	
	// //配置中断优先级
	// NVIC_Configuration();
	
	//打开接收非空中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, DISABLE);
	
	//最后使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);
}

void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t Data)
{
	USART_SendData(pUSARTx, Data);
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
}

void Usart_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t Data)
{
	uint8_t temp_h = 0;
	uint8_t temp_l = 0;
	
	temp_h = (Data & 0xFF00) >> 8;
	temp_l = Data & 0xFF;
	
	USART_SendData(pUSARTx, temp_h);
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
	USART_SendData(pUSARTx, temp_l);
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
}

void Usart_SendArray(USART_TypeDef* pUSARTx, uint8_t* Array, uint8_t Num)
{
	uint8_t i = 0;
	
	//循环发送一个字节
	for(i = 0; i < Num; i++)
	{
		USART_SendData(pUSARTx, Array[i]);
		while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
	}
	
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);
}

void Usart_Sendstr(USART_TypeDef* pUSARTx, uint8_t* str)
{
	uint8_t i = 0;
	
	do
	{
		USART_SendData(pUSARTx, str[i]);
		while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);
		i++;
	}while(str[i]);
	
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);
}

//重定向c库函数printf到串C ]，重定向后可使用printf函数
int fputc(int ch, FILE* f)
{
	/*发送一个字节数据到串口*/
	USART_SendData(DEBUG_USARTx,(uint8_t)ch);
	/*等待发送完毕*/
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
	
	return(ch);
}
