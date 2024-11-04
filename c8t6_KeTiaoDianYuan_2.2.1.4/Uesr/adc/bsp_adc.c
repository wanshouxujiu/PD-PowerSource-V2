#include "bsp_adc.h"

__IO uint16_t ADC_ConvertedValue[NOFCHANEL] = {0};
float Val;  //转换后的电压值

/**
  * @brief  配置ADC有关GPIO
  * @param  无
  * @retval 无
  */
static void ADCx_GPIO_Config()
{
	//======================================= GPIOB =======================================//
	//定义结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//开启时钟
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK, ENABLE);
	
	//引脚配置，一定要是模拟输入模式
	GPIO_InitStructure.GPIO_Pin = ADC_PIN1|ADC_PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	//初始化结构体
	GPIO_Init(ADC_PORT,&GPIO_InitStructure);


	//======================================= GPIOA =======================================//
	//开启时钟
	ADC_GPIO_APBxClock_FUN_INPUT(ADC_GPIO_CLK_INPUT, ENABLE);
	
	//引脚配置，一定要是模拟输入模式
	GPIO_InitStructure.GPIO_Pin = ADC_PIN3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	//初始化结构体
	GPIO_Init(ADC_PORT_INPUT,&GPIO_InitStructure);
}


/**
  * @brief  配置ADC有关的DMA模式
  * @param  无
  * @retval 无
  */
void ADCx_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	
	//打开时钟，DMA挂载于AHB总线上
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);
	
	//重置通道各项参数
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	//数据的来去
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC_x->DR));
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                         //外设选择为源头
	//数据的传输
	DMA_InitStruct.DMA_BufferSize = NOFCHANEL;                              //传输N次
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;                    //数组地址改为自增
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//模式
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;        //循环传输
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;    //优先级
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;           //M to M关
	
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStruct);
	
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
}


/**
  * @brief  配置ADC工作模式
  * @param  无
  * @retval 无
  */
static void ADCx_Mode_Config()
{
	//定义结构体
	ADC_InitTypeDef ADCInitStruct;
	
	//开启时钟
	ADC_APBxClock_FUN(ADC_CLK,ENABLE);
	
	ADCInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADCInitStruct.ADC_ScanConvMode = ENABLE;                          //多通道，开启扫描
	ADCInitStruct.ADC_ContinuousConvMode = ENABLE;                    //一直转换
	ADCInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADCInitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADCInitStruct.ADC_NbrOfChannel = NOFCHANEL;
	
	ADC_Init(ADC_x, &ADCInitStruct);
	
	//ADC时钟，8分频后为9M
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	//通道N，第N顺序，采样周期239.5
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL1, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL2, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC_x, ADC_CHANNEL3, 3, ADC_SampleTime_239Cycles5);
	
	//开启DMA请求
	ADC_DMACmd(ADC_x, ENABLE);
	
	ADC_Cmd(ADC_x, ENABLE);
	
	//校准
	ADC_StartCalibration(ADC_x);
	while(ADC_GetCalibrationStatus(ADC_x));
	
	//选择软件触发
	ADC_SoftwareStartConvCmd(ADC_x, ENABLE);
}


/**
  * @brief  整体初始化
  * @param  无
  * @retval 无
  */
void ADC_x_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_DMA_Config();
	ADCx_Mode_Config();
}


// /// @brief ADC1 的通道 8 的电压值获取
// /// @param  NULL
// /// @return 电压值
// float ADC1_Get_Channel_8_Voltaged(void)
// {
//     Val = (float )ADC_ConvertedValue[0]*(3.3/4096);

//     return Val;
// }


// /// @brief ADC1 的通道 9 的电压值获取
// /// @param  NULL
// /// @return 电压值
// float ADC1_Get_Channel_9_Voltaged(void)
// {
//     Val = (float )ADC_ConvertedValue[1]*(3.3/4096);

//     return Val;
// }


/// @brief ADC1 的通道 7 的电压值获取
/// @param  NULL
/// @return 电压值
float ADC1_Get_Channel_7_Voltaged(void)
{
    Val = (float )ADC_ConvertedValue[2]*(3.3/4096);

    return Val;
}


/// @brief ADC1 的通道 8 的计数值
/// @param  NULL
/// @return 计数值
uint16_t ADC1_Get_Channel_8_Val(void)
{
    return ADC_ConvertedValue[0];
}


/// @brief ADC1 的通道 9 的计数值
/// @param  NULL
/// @return 计数值
uint16_t ADC1_Get_Channel_9_Val(void)
{
    return ADC_ConvertedValue[1];
}
