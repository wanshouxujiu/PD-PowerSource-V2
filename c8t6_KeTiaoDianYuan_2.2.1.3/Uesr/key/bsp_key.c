#include "bsp_key.h"
#include "delay.h"

uint8_t KeyLongCount;

void KEY_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = KEY1_GPIO_PIN|KEY2_GPIO_PIN|KEY3_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_Init(KEY1_GPIO_PORT,&GPIO_InitStruct);
}


/// @brief 按键检测：短按（123）、双击（456）、长按（789）
/// @param  NULL
/// @return 0~9
uint8_t Key_Scan(void)
{
    uint8_t i = 0;
    uint16_t z = 0;
    uint16_t Time = 500;
    uint16_t TimeDouble = 150;
    uint16_t Pin[3] = {KEY1_GPIO_PIN, KEY2_GPIO_PIN, KEY3_GPIO_PIN};

    for (i = 0; i < 3; i++)
    {
        if(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, Pin[i]) == KEY_ON)
        {
            if(KeyLongCount < 50)
            {
                KeyLongCount = 0;
                return KEY_OFF;
            }

            //松手检测
            while(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, Pin[i]) == KEY_ON && (Time != 0))
            {
                Time--;
                z = 12000;       //这要根据系统时钟频率进行计算
                while(z--);
            }

            if(Time != 0)
            {
                while(GPIO_ReadInputDataBit(KEY1_GPIO_PORT, Pin[i]) == KEY_OFF && (TimeDouble != 0))
                {
                    TimeDouble--;
                    z = 12000;       //这要根据系统时钟频率进行计算
                    while(z--);
                }
                // printf("%d\r\n", TimeDouble);

                if(TimeDouble == 0)
                {
                    return i+1;
                }
                else
                {
                    Delay_ms(150);
                    return i+4;
                }
            }
            else if(Time == 0)
            {
                KeyLongCount = 0;
                return i+7;
            }
        } 
    }

    return KEY_OFF;
}
