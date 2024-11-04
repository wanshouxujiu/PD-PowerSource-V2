#ifndef __BSP_SC8701_H__
#define __BSP_SC8701_H__

#include "stm32f10x.h"

extern float InPutVoltage;

typedef enum
{
    CE_ENABLE  = 0,
    CE_DESABLE = 1,
}enum_CE;

typedef enum
{
    FB_LOW  = 0,
    FB_HIGH = 1,
}enum_FB;


#define            RATED_VOLTAGE_LOW             1010  //理论10.257V    影响电压调节
#define            RATED_VOLTAGE_HIGH            3415  //理论34.657V    影响电压调节
#define            RATED_CURRENT                 807   //理论最大电流

#define            MINIMUM_VOL                   205   //实际能达到的最小电压

#define            MAX_VOLTAGE                   3300
#define            MAX_CURRENT                   600

//功率限制（超限会导致芯片停止输出并发出嗡鸣声）
#define            MAX_POWER_9V                  180000    //18W
#define            MAX_POWER_12V                 180000    //18W
#define            MAX_POWER_15V                 225000    //22.5W
#define            MAX_POWER_20V                 650000    //若支持20V5A，可以设定为100


#define            SC8701_GEAR_SINGLE            0
#define            SC8701_GEAR_DOUBLE            1




#define            SC8701_EN                     GPIO_Pin_5    //降压芯片控制引脚
#define            SC8701_EN_PORT                GPIOB
#define            SC8701_EN_CLK                 RCC_APB2Periph_GPIOB

#define            SC8701_FB                     GPIO_Pin_12   //降压芯片挡位引脚
#define            SC8701_FB_PORT                GPIOA
#define            SC8701_FB_CLK                 RCC_APB2Periph_GPIOA

#define            ADVANCE_TIM                   TIM1
#define            ADVANCE_TIM_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define            ADVANCE_TIM_CLK               RCC_APB2Periph_TIM1
// PWM 信号的频率 F = TIM_CLK/{(ARR+1)*(PSC+1)}
#define            ADVANCE_TIM_PERIOD            (3600-1)
#define            ADVANCE_TIM_PSC               (1-1)  //72M
#define            ADVANCE_TIM_PULSE             0
#define            ADVANCE_TIM_CH4_PULSE         0

// TIM1 输出比较通道
#define            ADVANCE_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            ADVANCE_TIM_CH1_PORT          GPIOA
#define            ADVANCE_TIM_CH1_PIN           GPIO_Pin_8
#define            ADVANCE_TIM_CH4_PIN           GPIO_Pin_11




void SC8701_Init(void);
void SC8701_CE_Control(enum_CE option);
void SC8701_FB_Control(enum_FB option);
void SC8701_Set_Voltage(uint16_t voltage);
void SC8701_Set_Current(uint16_t current);
void SC8701_Set_Voltage_Slow(uint16_t voltage);
enum_CE SC8701_CE_Read(void);
enum_FB SC8701_FB_Read(void);

void Set_SC8701_GearFlag(uint8_t GearTmp);
uint8_t Get_SC8701_GearFlag(void);

#endif
