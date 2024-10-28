#ifndef __IWDG_H
#define	__IWDG_H


#include "stm32f10x.h"

void IWDG_Feed(void);
void IWDG_Config(uint8_t prv ,uint16_t rlv);
void IWDG_Init(void);
uint8_t IWDG_GetFlag(void);
void IWDG_CleanFlag(void);

#endif /* __IWDG_H */
