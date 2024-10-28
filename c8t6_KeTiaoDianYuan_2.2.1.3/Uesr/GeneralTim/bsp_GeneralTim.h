#ifndef __BSP_GENERALTIM_H__
#define __BSP_GENERALTIM_H__

#include "stm32f10x.h"

/************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
#define            GENERAL_TIM_3                   TIM4
#define            GENERAL_TIM_APBxClock_FUN_3     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_CLK_3               RCC_APB1Periph_TIM4
#define            GENERAL_TIM_Period_3            (10000-1)  //10ms 
#define            GENERAL_TIM_Prescaler_3         (72-1)    //1M
#define            GENERAL_TIM_IRQ_3               TIM4_IRQn
#define            GENERAL_TIM_IRQHandler_3        TIM4_IRQHandler


/**************************函数声明********************************/

void GENERAL_TIM_Init(void);


#endif
