//bsp:board support package    板级支持包
#include "bsp_SC8701.h"
#include "bsp_usart.h"
#include "bsp_iwdg.h"
#include "delay.h"
#include "bsp_OLED.h"


float InPutVoltage;
uint8_t SC8701_GearFlag;


void SC8701_GPIO_Config(void)
{
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  //打开AFIO时钟
	// GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);  //重映射
	// // //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);  //方法2：禁止JTAG、SWD调试

	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(SC8701_EN_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = SC8701_EN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SC8701_EN_PORT,&GPIO_InitStruct);

	GPIO_SetBits(SC8701_EN_PORT, SC8701_EN);  //设置为高电平
	// GPIO_ResetBits(SC8701_EN_PORT, SC8701_EN);


	RCC_APB2PeriphClockCmd(SC8701_FB_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = SC8701_FB;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SC8701_FB_PORT,&GPIO_InitStruct);

	// GPIO_SetBits(SC8701_FB_PORT, SC8701_FB);  //设置为高电平
	GPIO_ResetBits(SC8701_FB_PORT, SC8701_FB);
}


static void ADVANCE_TIM_GPIO_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// 输出比较通道 GPIO 初始化
	RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_CH1_PIN|ADVANCE_TIM_CH4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADVANCE_TIM_CH1_PORT, &GPIO_InitStructure);
}


static void ADVANCE_TIM_Mode_Config(void)
{
	// 开启定时器时钟,即内部时钟CK_INT=72M
	ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK,ENABLE);

	/*--------------------时基结构体初始化-------------------------*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// 自动重装载寄存器的值，累计TIM_Period+1个频率后产生一个更新或者中断
	TIM_TimeBaseStructure.TIM_Period = ADVANCE_TIM_PERIOD;	
	// 驱动CNT计数器的时钟 = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler = ADVANCE_TIM_PSC;	
	// 时钟分频因子 ，配置死区时间时需要用到
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// 计数器计数模式，设置为向上计数
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// 重复计数器的值，没用到不用管
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// 初始化定时器
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);

	/*--------------------输出比较结构体初始化-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// 配置为PWM模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 互补输出使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	// 设置占空比大小
	TIM_OCInitStructure.TIM_Pulse = ADVANCE_TIM_PULSE;
	// 输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// 互补输出通道电平极性配置
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	// 输出通道空闲电平极性配置
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// 互补输出通道空闲电平极性配置
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);

	//通道4
	TIM_OCInitStructure.TIM_Pulse = ADVANCE_TIM_CH4_PULSE;
	TIM_OC4Init(ADVANCE_TIM, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);


	/*-------------------刹车和死区结构体初始化-------------------*/ //关闭
	// 有关刹车和死区结构体的成员具体可参考BDTR寄存器的描述
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSRState_Disable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
	// 输出比较信号死区时间配置，具体如何计算可参考 BDTR:UTG[7:0]的描述
	// 这里配置的死区时间为152ns
	TIM_BDTRInitStructure.TIM_DeadTime = 11;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	// 当BKIN引脚检测到高电平的时候，输出比较信号被禁止，就好像是刹车一样
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);

	// 使能计数器
	TIM_Cmd(ADVANCE_TIM, ENABLE);	
	// 主输出使能，当使用的是通用定时器时，这句不需要
	TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
}

void SC8701_Init(void)
{
	SC8701_GPIO_Config();
	ADVANCE_TIM_GPIO_Config();
	ADVANCE_TIM_Mode_Config();
}


/// @brief 使能或失能SC8701
/// @param option CE_ENABLE or CE_DESABLE
void SC8701_CE_Control(enum_CE option)
{
	if(option)
	{
		GPIO_SetBits(SC8701_EN_PORT, SC8701_EN);
		// printf("失能SC8701\r\n");
	}
	else
	{
		GPIO_ResetBits(SC8701_EN_PORT, SC8701_EN);
		// printf("使能SC8701\r\n");
	}
}


/// @brief 读取使能
/// @param  NULL
/// @return 使能/失能
enum_CE SC8701_CE_Read(void)
{
	if(GPIO_ReadOutputDataBit(SC8701_EN_PORT, SC8701_EN))
	{
		return CE_DESABLE;
	}

	return CE_ENABLE;
}


/// @brief 挡位选择
/// @param option FB_HIGH or FB_LOW
void SC8701_FB_Control(enum_FB option)
{
	if(option)
	{
		GPIO_SetBits(SC8701_FB_PORT, SC8701_FB);
	}
	else
	{
		GPIO_ResetBits(SC8701_FB_PORT, SC8701_FB);
	}
}


/// @brief 读取挡位
/// @param  NULL
/// @return 挡位
enum_FB SC8701_FB_Read(void)
{
	if(GPIO_ReadOutputDataBit(SC8701_FB_PORT, SC8701_FB))
	{
		return FB_HIGH;
	}

	return FB_LOW;
}


/// @brief 调整输出电压
/// @param voltage 目标电压
void SC8701_Set_Voltage(uint16_t voltage)
{
	uint16_t DutyNm = 0;
	uint16_t tmp = 0;

	if(SC8701_GearFlag)  //双挡位模式
	{
		if(voltage <= 1000)
		{
			if(SC8701_FB_Read() == FB_HIGH && SC8701_CE_Read() == CE_ENABLE)
			{
				SC8701_CE_Control(CE_DESABLE);
				Delay_ms(300);
				SC8701_CE_Control(CE_ENABLE);
				SC8701_FB_Control(FB_LOW);  //挡位调节
				Delay_ms(10);
			}

			if(voltage < RATED_VOLTAGE_LOW * 0.166)  //电压最低只能输出硬件设定的1/6
			{
				TIM_SetCompare1(ADVANCE_TIM, 0);
			}
			else
			{
				tmp = voltage - (RATED_VOLTAGE_LOW * 0.166);
				DutyNm = 3600 * ((float)tmp / (RATED_VOLTAGE_LOW * 0.833));
				TIM_SetCompare1(ADVANCE_TIM, DutyNm);
			}
		}
		else
		{
			tmp = voltage - (RATED_VOLTAGE_HIGH * 0.166);
			DutyNm = 3600 * ((float)tmp / (RATED_VOLTAGE_HIGH * 0.833));
			TIM_SetCompare1(ADVANCE_TIM, DutyNm);

			if(SC8701_FB_Read() == FB_LOW && SC8701_CE_Read() == CE_ENABLE)
			{
				SC8701_CE_Control(CE_DESABLE);
				Delay_ms(300);
				SC8701_CE_Control(CE_ENABLE);
				SC8701_FB_Control(FB_HIGH);  //必须放在后面，否则在低档切换至高档时会输出很高的电压，因为PWM未改变而挡位更改了
			}
		}
	}
	else  //单挡位模式
	{
		if(SC8701_FB_Read() == FB_LOW)
		{
			SC8701_CE_Control(CE_DESABLE);
			Delay_ms(300);
			SC8701_FB_Control(FB_HIGH);
		}

		if(voltage < RATED_VOLTAGE_HIGH * 0.166)  //电压最低只能输出硬件设定的1/6
		{
			TIM_SetCompare1(ADVANCE_TIM, 0);
		}
		else
		{
			tmp = voltage - (RATED_VOLTAGE_HIGH * 0.166);
			DutyNm = 3600 * ((float)tmp / (RATED_VOLTAGE_HIGH * 0.833));
			TIM_SetCompare1(ADVANCE_TIM, DutyNm);
		}
	}
}


/// @brief 调整输出电流
/// @param voltage 目标电流
void SC8701_Set_Current(uint16_t current)
{
	uint16_t DutyNm = 0;

	DutyNm = 3600 * ((float)current / RATED_CURRENT);
	TIM_SetCompare4(ADVANCE_TIM, DutyNm);
}


/// @brief 依据输入的两个参数的百分比显示进度条
/// @param Tmp1 变化的参数
/// @param Tmp2 固定的参数
void ShowProgressBar(uint16_t Tmp1, uint16_t Tmp2)
{
	static uint8_t ShowFlag;
	uint8_t ratio = (uint8_t)(((float)Tmp1 / Tmp2) * 10);

	if(ratio == 0 && ShowFlag == 0)
	{
		ShowFlag = 1;
		OLED_ShowString (3, 1, "                ");
		OLED_ShowString (4, 1, "                ");
		OLED_ShowString (3, 3, "loading...");
		OLED_ShowBmpMini(4, 3, 0);
		OLED_ShowBmpMini(4, 4, 2);
		OLED_ShowBmpMini(4, 5, 2);
		OLED_ShowBmpMini(4, 6, 2);
		OLED_ShowBmpMini(4, 7, 2);
		OLED_ShowBmpMini(4, 8, 2);
		OLED_ShowBmpMini(4, 9, 2);
		OLED_ShowBmpMini(4, 10, 2);
		OLED_ShowBmpMini(4, 11, 2);
		OLED_ShowBmpMini(4, 12, 2);
		OLED_ShowBmpMini(4, 13, 2);
		OLED_ShowBmpMini(4, 14, 1);
	}
	else if(ratio > 0)
	{
		OLED_ShowBmpMini(4, 3 + ratio, 3);
	}

	if(ratio == 10)  //100%
	{
		ShowFlag = 0;
		OLED_ShowString (3, 1, "                ");
		OLED_ShowString (4, 1, "                ");
	}
}


/// @brief 缓慢调整输出电压
/// @param voltage 目标电压
void SC8701_Set_Voltage_Slow(uint16_t voltage)
{
	uint16_t Vout;
	uint16_t tmp;
	uint16_t TimeCount = 0;
	uint16_t TimeCount2 = 0;
	uint16_t i;

	if(GPIO_ReadInputDataBit(SC8701_FB_PORT, SC8701_FB))
	{
		Vout = RATED_VOLTAGE_HIGH * (0.166 + 0.833 * ((float)TIM_GetCapture1(ADVANCE_TIM) / (ADVANCE_TIM_PERIOD + 1)));
	}
	else
	{
		Vout = RATED_VOLTAGE_LOW * (0.166 + 0.833 * ((float)TIM_GetCapture1(ADVANCE_TIM) / (ADVANCE_TIM_PERIOD + 1)));
	}

	if(voltage > Vout)
	{
		TimeCount2 = TimeCount = ((voltage - Vout) / 100) * 5;
		tmp = voltage - Vout;
		
		while(TimeCount--)
		{
			ShowProgressBar(TimeCount2 - TimeCount, TimeCount2);
			IWDG_Feed();
			SC8701_Set_Voltage(Vout + (tmp * (float)((float)(TimeCount2 - TimeCount) / TimeCount2)));
			i=12000;
			while(i--) ;
		}
		SC8701_Set_Voltage(voltage);
	}
	else
	{
		SC8701_Set_Voltage(voltage);
	}
}


/// @brief 设置单双模式的标志
/// @param GearTmp 单0双1
void Set_SC8701_GearFlag(uint8_t GearTmp)
{
	SC8701_GearFlag = GearTmp;
}


/// @brief 读取单双模式的标志
/// @param  NULL
/// @return 单0双1
uint8_t Get_SC8701_GearFlag(void)
{
	return SC8701_GearFlag;
}
