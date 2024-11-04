#include "bsp_SIQ02FVS3.h"
#include "delay.h"


void SIQ02FVS3_KEY_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(SIQ02FVS3_KEY_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = SIQ02FVS3_KEY_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SIQ02FVS3_KEY_GPIO_PORT,&GPIO_InitStruct);

    RCC_APB2PeriphClockCmd(SIQ02FVS3_ECB_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = SIQ02FVS3_ECB_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SIQ02FVS3_ECB_GPIO_PORT,&GPIO_InitStruct);

    RCC_APB2PeriphClockCmd(SIQ02FVS3_ECA_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = SIQ02FVS3_ECA_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SIQ02FVS3_ECA_GPIO_PORT,&GPIO_InitStruct);
}


static void SIQ02FVS3_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InItStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);        //����
	
	NVIC_InItStruct.NVIC_IRQChannel = EXTI4_IRQn;          //ͨ��������һһ��Ӧ��ֻ��һ����һһ��Ӧ
	NVIC_InItStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InItStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InItStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InItStruct);
}


static void SIQ02FVS3_EXTI_Config(void)
{
    //��ʼ��EXIT
	EXTI_InitTypeDef EXTI_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,  GPIO_PinSource4);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  //�½��ش���
	EXTI_Init(&EXTI_InitStruct);
}


void SIQ02FVS3_Init(void)
{
    SIQ02FVS3_KEY_GPIO_Config();
    SIQ02FVS3_EXTI_Config();
    SIQ02FVS3_NVIC_Config();
}


uint8_t SIQ02FVS3_Key_Scan(void)
{

    if(GPIO_ReadInputDataBit(SIQ02FVS3_KEY_GPIO_PORT, SIQ02FVS3_KEY_GPIO_PIN) == SIQ02FVS3_KEY_ON)
    {
        //���ּ��
		while(GPIO_ReadInputDataBit(SIQ02FVS3_KEY_GPIO_PORT, SIQ02FVS3_KEY_GPIO_PIN) == SIQ02FVS3_KEY_ON);
		Delay_ms(30);

        return KEY4_ON;
    }

    return 0;
}
