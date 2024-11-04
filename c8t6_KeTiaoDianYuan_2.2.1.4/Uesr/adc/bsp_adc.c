#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue[NOFCHANEL] = {0};
float Val;  //ת����ĵ�ѹֵ

/**
  * @brief  ����ADC�й�GPIO
  * @param  ��
  * @retval ��
  */
static void ADCx_GPIO_Config()
{
	//======================================= GPIOB =======================================//
	//����ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//����ʱ��
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE);
	
	//�������ã�һ��Ҫ��ģ������ģʽ
	GPIO_InitStructure.GPIO_Pin = ADC_PIN1|ADC_PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	//��ʼ���ṹ��
	GPIO_Init(ADC_PORT,&GPIO_InitStructure);


	//======================================= GPIOA =======================================//
	//����ʱ��
	ADC_GPIO_APBxClock_FUN_INPUT(ADC_GPIO_CLK_INPUT, ENABLE);
	
	//�������ã�һ��Ҫ��ģ������ģʽ
	GPIO_InitStructure.GPIO_Pin = ADC_PIN3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	//��ʼ���ṹ��
	GPIO_Init(ADC_PORT_INPUT,&GPIO_InitStructure);
}


/**
  * @brief  ����ADC�йص�DMAģʽ
  * @param  ��
  * @retval ��
  */
void ADCx_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	//��ʱ�ӣ�DMA������AHB������
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);
	
	//����ͨ���������
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	//���ݵ���ȥ
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC_x->DR));
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                         //����ѡ��ΪԴͷ
	//���ݵĴ���
	DMA_InitStruct.DMA_BufferSize = NOFCHANEL;                              //����N��
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;                    //�����ַ��Ϊ����
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//ģʽ
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;        //ѭ������
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;    //���ȼ�
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;           //M to M��
	
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
	
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
}


/**
  * @brief  ����ADC����ģʽ
  * @param  ��
  * @retval ��
  */
static void ADCx_Mode_Config()
{
	//����ṹ��
	ADC_InitTypeDef ADCInitStruct;
	
	//����ʱ��
	ADC_APBxClock_FUN(ADC_CLK,ENABLE);
	
	ADCInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADCInitStruct.ADC_ScanConvMode = ENABLE;                          //��ͨ��������ɨ��
	ADCInitStruct.ADC_ContinuousConvMode = ENABLE;                    //һֱת��
	ADCInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADCInitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADCInitStruct.ADC_NbrOfChannel = NOFCHANEL;
	
	ADC_Init(ADC_x, &ADCInitStruct);
	
	//ADCʱ�ӣ�8��Ƶ��Ϊ9M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//ͨ��N����N˳�򣬲�������239.5
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL1, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL2, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL3, 3, ADC_SampleTime_239Cycles5);
	
	//����DMA����
	ADC_DMACmd(ADC_x, ENABLE);
	
	ADC_Cmd(ADC_x, ENABLE);
	
	//У׼
	ADC_StartCalibration(ADC_x);
	while(ADC_GetCalibrationStatus(ADC_x));
	
	//ѡ���������
	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}


/**
  * @brief  �����ʼ��
  * @param  ��
  * @retval ��
  */
void ADC_x_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_DMA_Config();
	ADCx_Mode_Config();
}


// /// @brief ADC1 ��ͨ�� 8 �ĵ�ѹֵ��ȡ
// /// @param  NULL
// /// @return ��ѹֵ
// float ADC1_Get_Channel_8_Voltaged(void)
// {
//     Val = (float )ADC_ConvertedValue[0]*(3.3/4096);

//     return Val;
// }


// /// @brief ADC1 ��ͨ�� 9 �ĵ�ѹֵ��ȡ
// /// @param  NULL
// /// @return ��ѹֵ
// float ADC1_Get_Channel_9_Voltaged(void)
// {
//     Val = (float )ADC_ConvertedValue[1]*(3.3/4096);

//     return Val;
// }


/// @brief ADC1 ��ͨ�� 7 �ĵ�ѹֵ��ȡ
/// @param  NULL
/// @return ��ѹֵ
float ADC1_Get_Channel_7_Voltaged(void)
{
    Val = (float )ADC_ConvertedValue[2]*(3.3/4096);

    return Val;
}


/// @brief ADC1 ��ͨ�� 8 �ļ���ֵ
/// @param  NULL
/// @return ����ֵ
uint16_t ADC1_Get_Channel_8_Val(void)
{
    return ADC_ConvertedValue[0];
}


/// @brief ADC1 ��ͨ�� 9 �ļ���ֵ
/// @param  NULL
/// @return ����ֵ
uint16_t ADC1_Get_Channel_9_Val(void)
{
    return ADC_ConvertedValue[1];
}
