#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#include "stm32f10x.h"

#define    ADC_GPIO_APBxClock_FUN                 RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                           RCC_APB2Periph_GPIOB
#define    ADC_PORT                               GPIOB

#define    ADC_GPIO_APBxClock_FUN_INPUT           RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK_INPUT                     RCC_APB2Periph_GPIOA
#define    ADC_PORT_INPUT                         GPIOA

// 转换通道个数
#define    NOFCHANEL										 3

#define    ADC_PIN1                               GPIO_Pin_0
#define    ADC_CHANNEL1                           ADC_Channel_8

#define    ADC_PIN2                               GPIO_Pin_1
#define    ADC_CHANNEL2                           ADC_Channel_9

#define    ADC_PIN3                               GPIO_Pin_7
#define    ADC_CHANNEL3                           ADC_Channel_7


// ADC 编号选择
// 可以是 ADC1/2，如果使用ADC3，中断相关的要改成ADC3的
#define    ADC_APBxClock_FUN                      RCC_APB2PeriphClockCmd
#define    ADC_x                                  ADC1
#define    ADC_CLK                                RCC_APB2Periph_ADC1

// DMA 通道宏定义
#define    ADC_DMA_CLK                            RCC_AHBPeriph_DMA1
#define    ADC_DMA_CHANNEL                        DMA1_Channel1




void ADC_x_Init(void);
float ADC1_Get_Channel_7_Voltaged(void);
// float ADC1_Get_Channel_8_Voltaged(void);
// float ADC1_Get_Channel_9_Voltaged(void);
uint16_t ADC1_Get_Channel_8_Val(void);
uint16_t ADC1_Get_Channel_9_Val(void);




#endif
