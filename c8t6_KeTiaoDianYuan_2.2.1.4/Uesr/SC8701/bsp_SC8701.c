//bsp:board support package    �弶֧�ְ�
#include "bsp_SC8701.h"
#include "bsp_usart.h"
#include "bsp_iwdg.h"
#include "delay.h"
#include "bsp_OLED.h"


float InPutVoltage;
uint8_t SC8701_GearFlag;


void SC8701_GPIO_Config(void)
{
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);  //��AFIOʱ��
	// GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);  //��ӳ��
	// // //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);  //����2����ֹJTAG��SWD����

	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(SC8701_EN_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = SC8701_EN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SC8701_EN_PORT,&GPIO_InitStruct);

	GPIO_SetBits(SC8701_EN_PORT, SC8701_EN);  //����Ϊ�ߵ�ƽ
	// GPIO_ResetBits(SC8701_EN_PORT, SC8701_EN);


	RCC_APB2PeriphClockCmd(SC8701_FB_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = SC8701_FB;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SC8701_FB_PORT,&GPIO_InitStruct);

	// GPIO_SetBits(SC8701_FB_PORT, SC8701_FB);  //����Ϊ�ߵ�ƽ
	GPIO_ResetBits(SC8701_FB_PORT, SC8701_FB);
}


static void ADVANCE_TIM_GPIO_Config(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// ����Ƚ�ͨ�� GPIO ��ʼ��
	RCC_APB2PeriphClockCmd(ADVANCE_TIM_CH1_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  ADVANCE_TIM_CH1_PIN|ADVANCE_TIM_CH4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADVANCE_TIM_CH1_PORT, &GPIO_InitStructure);
}


static void ADVANCE_TIM_Mode_Config(void)
{
	// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
	ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK,ENABLE);

	/*--------------------ʱ���ṹ���ʼ��-------------------------*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period = ADVANCE_TIM_PERIOD;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler = ADVANCE_TIM_PSC;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);

	/*--------------------����ȽϽṹ���ʼ��-------------------*/		
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	// ����ΪPWMģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// ���ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// �������ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
	// ����ռ�ձȴ�С
	TIM_OCInitStructure.TIM_Pulse = ADVANCE_TIM_PULSE;
	// ���ͨ����ƽ��������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// �������ͨ����ƽ��������
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	// ���ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	// �������ͨ�����е�ƽ��������
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(ADVANCE_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);

	//ͨ��4
	TIM_OCInitStructure.TIM_Pulse = ADVANCE_TIM_CH4_PULSE;
	TIM_OC4Init(ADVANCE_TIM, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(ADVANCE_TIM, TIM_OCPreload_Enable);


	/*-------------------ɲ���������ṹ���ʼ��-------------------*/ //�ر�
	// �й�ɲ���������ṹ��ĳ�Ա����ɲο�BDTR�Ĵ���������
	TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSRState_Disable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
	// ����Ƚ��ź�����ʱ�����ã�������μ���ɲο� BDTR:UTG[7:0]������
	// �������õ�����ʱ��Ϊ152ns
	TIM_BDTRInitStructure.TIM_DeadTime = 11;
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
	// ��BKIN���ż�⵽�ߵ�ƽ��ʱ������Ƚ��źű���ֹ���ͺ�����ɲ��һ��
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
	TIM_BDTRConfig(ADVANCE_TIM, &TIM_BDTRInitStructure);

	// ʹ�ܼ�����
	TIM_Cmd(ADVANCE_TIM, ENABLE);	
	// �����ʹ�ܣ���ʹ�õ���ͨ�ö�ʱ��ʱ����䲻��Ҫ
	TIM_CtrlPWMOutputs(ADVANCE_TIM, ENABLE);
}

void SC8701_Init(void)
{
	SC8701_GPIO_Config();
	ADVANCE_TIM_GPIO_Config();
	ADVANCE_TIM_Mode_Config();
}


/// @brief ʹ�ܻ�ʧ��SC8701
/// @param option CE_ENABLE or CE_DESABLE
void SC8701_CE_Control(enum_CE option)
{
	if(option)
	{
		GPIO_SetBits(SC8701_EN_PORT, SC8701_EN);
		// printf("ʧ��SC8701\r\n");
	}
	else
	{
		GPIO_ResetBits(SC8701_EN_PORT, SC8701_EN);
		// printf("ʹ��SC8701\r\n");
	}
}


/// @brief ��ȡʹ��
/// @param  NULL
/// @return ʹ��/ʧ��
enum_CE SC8701_CE_Read(void)
{
	if(GPIO_ReadOutputDataBit(SC8701_EN_PORT, SC8701_EN))
	{
		return CE_DESABLE;
	}

	return CE_ENABLE;
}


/// @brief ��λѡ��
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


/// @brief ��ȡ��λ
/// @param  NULL
/// @return ��λ
enum_FB SC8701_FB_Read(void)
{
	if(GPIO_ReadOutputDataBit(SC8701_FB_PORT, SC8701_FB))
	{
		return FB_HIGH;
	}

	return FB_LOW;
}


/// @brief ���������ѹ
/// @param voltage Ŀ���ѹ
void SC8701_Set_Voltage(uint16_t voltage)
{
	uint16_t DutyNm = 0;
	uint16_t tmp = 0;

	if(SC8701_GearFlag)  //˫��λģʽ
	{
		if(voltage <= 1000)
		{
			if(SC8701_FB_Read() == FB_HIGH && SC8701_CE_Read() == CE_ENABLE)
			{
				SC8701_CE_Control(CE_DESABLE);
				Delay_ms(300);
				SC8701_CE_Control(CE_ENABLE);
				SC8701_FB_Control(FB_LOW);  //��λ����
				Delay_ms(10);
			}

			if(voltage < RATED_VOLTAGE_LOW * 0.166)  //��ѹ���ֻ�����Ӳ���趨��1/6
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
				SC8701_FB_Control(FB_HIGH);  //������ں��棬�����ڵ͵��л����ߵ�ʱ������ܸߵĵ�ѹ����ΪPWMδ�ı����λ������
			}
		}
	}
	else  //����λģʽ
	{
		if(SC8701_FB_Read() == FB_LOW)
		{
			SC8701_CE_Control(CE_DESABLE);
			Delay_ms(300);
			SC8701_FB_Control(FB_HIGH);
		}

		if(voltage < RATED_VOLTAGE_HIGH * 0.166)  //��ѹ���ֻ�����Ӳ���趨��1/6
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


/// @brief �����������
/// @param voltage Ŀ�����
void SC8701_Set_Current(uint16_t current)
{
	uint16_t DutyNm = 0;

	DutyNm = 3600 * ((float)current / RATED_CURRENT);
	TIM_SetCompare4(ADVANCE_TIM, DutyNm);
}


/// @brief ������������������İٷֱ���ʾ������
/// @param Tmp1 �仯�Ĳ���
/// @param Tmp2 �̶��Ĳ���
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


/// @brief �������������ѹ
/// @param voltage Ŀ���ѹ
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


/// @brief ���õ�˫ģʽ�ı�־
/// @param GearTmp ��0˫1
void Set_SC8701_GearFlag(uint8_t GearTmp)
{
	SC8701_GearFlag = GearTmp;
}


/// @brief ��ȡ��˫ģʽ�ı�־
/// @param  NULL
/// @return ��0˫1
uint8_t Get_SC8701_GearFlag(void)
{
	return SC8701_GearFlag;
}
