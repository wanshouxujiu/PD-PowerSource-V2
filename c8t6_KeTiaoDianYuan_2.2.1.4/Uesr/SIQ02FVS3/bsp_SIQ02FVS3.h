#ifndef __BSP_SIQ02FVS3_H__
#define __BSP_SIQ02FVS3_H__

#include "stm32f10x.h"


#define SIQ02FVS3_KEY_OFF        1
#define SIQ02FVS3_KEY_ON         0
#define KEY4_ON                  10


#define SIQ02FVS3_KEY_GPIO_PIN               GPIO_Pin_15
#define SIQ02FVS3_KEY_GPIO_PORT              GPIOB
#define SIQ02FVS3_KEY_GPIO_CLK               RCC_APB2Periph_GPIOB

#define SIQ02FVS3_ECB_GPIO_PIN               GPIO_Pin_6
#define SIQ02FVS3_ECB_GPIO_PORT              GPIOA
#define SIQ02FVS3_ECB_GPIO_CLK               RCC_APB2Periph_GPIOA

#define SIQ02FVS3_ECA_GPIO_PIN               GPIO_Pin_4
#define SIQ02FVS3_ECA_GPIO_PORT              GPIOA
#define SIQ02FVS3_ECA_GPIO_CLK               RCC_APB2Periph_GPIOA




void SIQ02FVS3_Init(void);
uint8_t SIQ02FVS3_Key_Scan(void);


#endif
