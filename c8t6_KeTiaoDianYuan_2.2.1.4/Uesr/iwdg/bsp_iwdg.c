#include "bsp_iwdg.h"
#include "bsp_OLED.h"
#include "delay.h"

/*
 * ���� IWDG �ĳ�ʱʱ��
 * Tout = prv/40 * rlv (s)
 *      prv������[4,8,16,32,64,128,256]
 * prv:Ԥ��Ƶ��ֵ��ȡֵ���£�
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 * rlv:Ԥ��Ƶ��ֵ��ȡֵ��ΧΪ��0-0XFFF
 * �������þ�����
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s ��ʱ���
 */

void IWDG_Config(uint8_t prv ,uint16_t rlv)
{	
	// ʹ�� Ԥ��Ƶ�Ĵ���PR����װ�ؼĴ���RLR��д
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	
	// ����Ԥ��Ƶ��ֵ
	IWDG_SetPrescaler( prv );
	
	// ������װ�ؼĴ���ֵ
	IWDG_SetReload( rlv );
	
	// ����װ�ؼĴ�����ֵ�ŵ���������
	IWDG_ReloadCounter();
	
	// ʹ�� IWDG
	IWDG_Enable();	
}

// ι��
void IWDG_Feed(void)
{
	// ����װ�ؼĴ�����ֵ�ŵ��������У�ι������ֹIWDG��λ
	// ����������ֵ����0��ʱ������ϵͳ��λ
	IWDG_ReloadCounter();
}


//��ȡ���Ź�������־
uint8_t IWDG_GetFlag(void)
{
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		return 1;
	}

	return 0;
}


//������Ź�������־
void IWDG_CleanFlag(void)
{
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag();
	}
}


void IWDG_Init(void)
{
	// if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	// {
	// 	// OLED_ShowString (2, 7, "    ");
	// 	// OLED_ShowString (2, 7, "IWDG");
	// 	// Delay_ms(300);
	// 	// OLED_ShowString (2, 7, "    ");
		
	// 	RCC_ClearFlag();  //�����־
	// }

	IWDG_Config(IWDG_Prescaler_64, 1500);  //40000/64=625hz��1=0.0016s��0.0016*625=1s
}


/*********************************************END OF FILE**********************/
