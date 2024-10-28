#ifndef __BSP_OLED_H__
#define __BSP_OLED_H__

#include "stm32f10x.h"


//等待超时时间
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)

#define STM32_I2C_OWN_ADDR                0x7f

//设备地址
#define OLED_ADDR                         0x78

//IIC
#define OLED_I2C                          I2C2
#define OLED_I2C_CLK                      RCC_APB1Periph_I2C2
#define OLED_I2C_APBxClkCmd               RCC_APB1PeriphClockCmd
#define OLED_I2C_BAUDRATE                 400000       //波特率

//IIC GPIO 引脚定义
#define OLED_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
#define OLED_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB

#define OLED_I2C_GPIO_APBxClkCmd          RCC_APB2PeriphClockCmd

#define OLED_I2C_SCL_GPIO_PORT            GPIOB
#define OLED_I2C_SCL_GPIO_PIN             GPIO_Pin_10

#define OLED_I2C_SDA_GPIO_PORT            GPIOB
#define OLED_I2C_SDA_GPIO_PIN             GPIO_Pin_11


#define SHOW_INVERSION                    1          //是否倒置显示


void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Num);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBMP(uint8_t Line, uint8_t Column, uint8_t Sign);
void OLED_ShowNum_Bright(uint8_t Line, uint8_t Column, uint8_t Num);
void OLED_ShowBmpMini(uint8_t Line, uint8_t Column, uint8_t Num);
void OLED_Clock_Control(uint8_t parameter);

#endif
