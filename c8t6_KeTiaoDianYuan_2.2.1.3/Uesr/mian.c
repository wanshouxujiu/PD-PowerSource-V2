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
	//看门狗初始化
	IWDG_Init();
	//串口初始化
	USART_Config();
	// printf("\r\n初始化中...\r\n");
	printf("\r\n哈喽，很好奇对吗？可是我还没想好和你说什么...\r\n");
	
	//OLED初始化
	OLED_Init();
	//EEPROM初始化(AT24C02)
	I2C_EE_Init();
	//检查设置项
	Check_Setting();
	//显示：校徽
	Show_logo();
	//ADC初始化
	ADC_x_Init();
	//定时器初始化
	GENERAL_TIM_Init();
	//按键初始化
	KEY_GPIO_Config();
	//编码器初始化
	SIQ02FVS3_Init();
	//SC8701 控制引脚初始化
	SC8701_Init();
	//显示输入电压
	Detection_InPut_Voltage();
	OLED_Clear();
	//显示UI
	Show_UI();

	printf("初始化完成 \r\n");
	while(1)
	{
		switch (Get_PDPowerSupplyState())
		{
		case POWER_SUPPLY_RUNNING:
			PowerSupply_Runing();      //正常运行
			break;
		case POWER_SUPPLY_SHUTDOWN:
			PowerSupply_ShutDown();    //关机
			break;
		case POWER_SUPPLY_EXTINGUISH:
			PowerSupply_Extinguish();  //休眠
			break;
		default:
			break;
		}
	}
}


void PowerSupply_Runing(void)
{
	IWDG_Feed();                //喂狗
	Short_Circuit_Detection();  //短路检测
	KeyInput_Detection();       //按键
	KeyInputFSM_Run();          //运行状态机
	
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
	IWDG_Feed();                //喂狗
	KeyInput_Detection();       //按键
	KeyInputFSM_Run();          //运行状态机
}


void PowerSupply_Extinguish(void)
{
	IWDG_Feed();                //喂狗
	Short_Circuit_Detection();  //短路检测
	KeyInput_Detection();       //按键
	KeyInputFSM_Run();          //运行状态机
}
