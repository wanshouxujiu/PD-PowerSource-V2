#include "stm32f10x.h"
#include "bsp_usart.h"
#include "bsp_OLED.h"
#include "bsp_adc.h"
#include "task.h"
#include "delay.h"
#include "bsp_GeneralTim.h"
#include "stm32f10x_it.h"
#include "bsp_SC8701.h"
#include "bsp_key.h"
#include "bsp_SIQ02FVS3.h"
#include "KeyInputFsm.h"
#include "bsp_i2c_ee.h"
#include "bsp_iwdg.h" 


void PowerSupply_Runing(void);
void PowerSupply_ShutDown(void);
void PowerSupply_Extinguish(void);


int main()
{
	//���Ź���ʼ��
	IWDG_Init();
	//���ڳ�ʼ��
	USART_Config();
	// printf("\r\n��ʼ����...\r\n");
	printf("\r\n��ඣ��ܺ�����𣿿����һ�û��ú���˵ʲô...\r\n");
	
	//OLED��ʼ��
	OLED_Init();
	//EEPROM��ʼ��(AT24C02)
	I2C_EE_Init();
	//���������
	Check_Setting();
	//��ʾ��У��
	Show_logo();
	//ADC��ʼ��
	ADC_x_Init();
	//��ʱ����ʼ��
	GENERAL_TIM_Init();
	//������ʼ��
	KEY_GPIO_Config();
	//��������ʼ��
	SIQ02FVS3_Init();
	//SC8701 �������ų�ʼ��
	SC8701_Init();
	//��ʾ�����ѹ
	Detection_InPut_Voltage();
	OLED_Clear();
	//��ʾUI
	Show_UI();

	printf("��ʼ����� \r\n");
	while(1)
	{
		switch (Get_PDPowerSupplyState())
		{
		case POWER_SUPPLY_RUNNING:
			PowerSupply_Runing();      //��������
			break;
		case POWER_SUPPLY_SHUTDOWN:
			PowerSupply_ShutDown();    //�ػ�
			break;
		case POWER_SUPPLY_EXTINGUISH:
			PowerSupply_Extinguish();  //����
			break;
		default:
			break;
		}
	}
}


void PowerSupply_Runing(void)
{
	IWDG_Feed();                //ι��
	Short_Circuit_Detection();  //��·���
	KeyInput_Detection();       //����
	KeyInputFSM_Run();          //����״̬��
	
	if(Get_PDPowerSupplyState() == POWER_SUPPLY_RUNNING)
	{
		if(TaskTimeCount >= 1)
		{
			Detection_Voltage();
			Detection_Current();
			Detection_Power();
			TaskTimeCount = 0;
		}

		if(SlowTaskCount >= 500)
		{
			Show_UI();
			SlowTaskCount = 0;
		}

		Show_State();
	}
}


void PowerSupply_ShutDown(void)
{
	IWDG_Feed();                //ι��
	KeyInput_Detection();       //����
	KeyInputFSM_Run();          //����״̬��
}


void PowerSupply_Extinguish(void)
{
	IWDG_Feed();                //ι��
	Short_Circuit_Detection();  //��·���
	KeyInput_Detection();       //����
	KeyInputFSM_Run();          //����״̬��
}
