#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stm32f10x.h"


extern uint8_t KeyLongCount;


#define KEY_OFF        0
#define KEY_ON         1


#define KEY1_GPIO_PIN               GPIO_Pin_12
#define KEY1_GPIO_PORT              GPIOB
#define KEY1_GPIO_CLK               RCC_APB2Periph_GPIOB

#define KEY2_GPIO_PIN               GPIO_Pin_13
#define KEY3_GPIO_PIN               GPIO_Pin_14



void KEY_GPIO_Config(void);
uint8_t Key_Scan(void);


#endif
