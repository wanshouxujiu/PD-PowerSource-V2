#include "stm32f10x.h"
#include "OLED_Font.h"
#include "bsp_OLED.h"
#include "bsp_usart.h"
#include "delay.h"


__IO uint32_t  I2CTimeout = I2CT_FLAG_TIMEOUT;


/// @brief ��ʱ����
/// @param errorCode �������
/// @return 0 ��ʾ����
static  uint32_t I2C_TimeoutEvent(uint8_t errorCode)
{
  printf("I2C �ȴ���ʱ! �������: EV_%d\r\n", errorCode);
  
  return 0;
}


/*���ų�ʼ��*/
void OLED_I2C_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
	//��I2CGPIOʱ��
	OLED_I2C_GPIO_APBxClkCmd(OLED_I2C_SCL_GPIO_CLK | OLED_I2C_SDA_GPIO_CLK, ENABLE);
	//��I2C����ʱ��
	OLED_I2C_APBxClkCmd(OLED_I2C_CLK, ENABLE);
	
	//I2C SCL
	GPIO_InitStructure.GPIO_Pin = OLED_I2C_SCL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OLED_I2C_SCL_GPIO_PORT,&GPIO_InitStructure);
	//I2C SDA
	GPIO_InitStructure.GPIO_Pin = OLED_I2C_SDA_GPIO_PIN;
	GPIO_Init(OLED_I2C_SDA_GPIO_PORT,&GPIO_InitStructure);
	
	//����I2C��������
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;  //Ӧ��ʹ�ܣ������󲿷�I2C��ҪӦ��
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  //��ַ��������Ϊ7bit
	I2C_InitStructure.I2C_ClockSpeed = OLED_I2C_BAUDRATE;    //����������
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;       //ռ�ձȣ�һ�������ѡ��
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;               //ѡ��I2Cģʽ
	I2C_InitStructure.I2C_OwnAddress1 = STM32_I2C_OWN_ADDR;  //���������ַ
	I2C_Init(OLED_I2C,&I2C_InitStructure);
	
	//���ʹ��I2C
	I2C_Cmd(OLED_I2C, ENABLE);
}


/**
  * @brief  OLEDд����
  * @param  Command Ҫд�������
  * @retval ��
  */
uint8_t OLED_WriteCommand(uint8_t Command)
{
	//������ʼ�ź�
	I2C_GenerateSTART(OLED_I2C, ENABLE);

	/* ���EV5�¼��������־λ */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_MODE_SELECT))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(5);
	}

	I2C_Send7bitAddress(OLED_I2C, OLED_ADDR, I2C_Direction_Transmitter);

	/* ���EV6�¼��������־λ */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(6);
	}

	I2C_SendData(OLED_I2C, 0x00);   //д����

	/* ���EV8�¼��������־λ */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(8);
	}

	I2C_SendData(OLED_I2C, Command);

	/* ���EV8_2�¼��������־λ */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(82);
	}

	I2C_GenerateSTOP(OLED_I2C, ENABLE);
	Delay_us(5);  //����ģʽ�£����ȴ����׳���

	return 1;
}

/**
  * @brief  OLEDд����
  * @param  Data Ҫд�������
  * @retval ��
  */
uint8_t OLED_WriteData(uint8_t Data)
{
	//������ʼ�ź�
	I2C_GenerateSTART(OLED_I2C, ENABLE);

	/* ���EV5�¼��������־λ */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_MODE_SELECT))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(5);
	}

	I2C_Send7bitAddress(OLED_I2C, OLED_ADDR, I2C_Direction_Transmitter);

	/* ���EV6�¼��������־λ */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(6);
	}

	I2C_SendData(OLED_I2C, 0x40);   //д����

	/* ���EV8�¼��������־λ */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(8);
	}
	
	I2C_SendData(OLED_I2C, Data);

	/* ���EV8_2�¼��������־λ */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(82);
	}
	
	I2C_GenerateSTOP(OLED_I2C, ENABLE);
	Delay_us(5);  //����ģʽ�£����ȴ����׳���

	return 1;
}


//===================================����ΪI2C�ײ㹹��=================================//


/**
  * @brief  OLED���ù��λ��
  * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
  * @retval ��
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//����Yλ��
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//����Xλ�ø�4λ
	OLED_WriteCommand(0x00 | (X & 0x0F));			//����Xλ�õ�4λ
}

/**
  * @brief  OLED����
  * @param  ��
  * @retval ��
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED��ʾһ���ַ�
  * @param  Line ��λ�ã���Χ��1~4
  * @param  Column ��λ�ã���Χ��1~16
  * @param  Char Ҫ��ʾ��һ���ַ�����Χ��ASCII�ɼ��ַ�
  * @retval ��
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//���ù��λ�����ϰ벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//��ʾ�ϰ벿������
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//���ù��λ�����°벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//��ʾ�°벿������
	}
}


/**
  * @brief  OLED��ʾһ�����ֵ�����
  * @param  Line ��λ�ã���Χ��1~4
  * @param  Column ��λ�ã���Χ��1~16
  * @param  Num Ҫ��ʾ��һ���ַ�����Χ��ASCII�ɼ��ַ�
  * @retval ��
  */
void OLED_ShowNum_Bright(uint8_t Line, uint8_t Column, uint8_t Num)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//���ù��λ�����ϰ벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16_Bright[Num][i]);			//��ʾ�ϰ벿������
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//���ù��λ�����°벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16_Bright[Num][i + 8]);		//��ʾ�°벿������
	}
}


/**
  * @brief  OLED��ʾһ������
  * @param  Line ��λ�ã���Χ��1~4
  * @param  Column ��λ�ã���Χ��1~16
  * @param  Num Ҫ��ʾ��һ���������
  * @retval ��
  */
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Num)
{      	
	uint8_t i;

	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//���ù��λ�����ϰ벿��
	for (i = 0; i < 16; i++)
	{
		OLED_WriteData(OLED_F16x16[Num][i]);					//��ʾ�ϰ벿������
	}

	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//���ù��λ�����°벿��
	for (i = 0; i < 16; i++)
	{
		OLED_WriteData(OLED_F16x16[Num][i + 16]);					//��ʾ�°벿������
	}
}


/**
  * @brief  OLED��ʾһ��BMP��ʽ��ͼ��64*64����
  * @param  Line ��λ�ã���Χ��1~8
  * @param  Column ��λ�ã���Χ��1~128
  * @param  0��У��
  * @retval ��
  */
void OLED_ShowBMP(uint8_t Line, uint8_t Column, uint8_t Sign)
{      	
	uint8_t i,j;
	
	for (i = 0; i < 8; i++)
	{
		OLED_SetCursor((Line - 1) + i, (Column - 1));
		
		for (j = 0; j < 64; j++)
		{
			OLED_WriteData(OLED_BMP_64x64[Sign][j + (i * 64)]);
		}
	}
}


/**
  * @brief  OLED��ʾһ��8*16�Զ���ͼ��
  * @param  Line ��λ�ã���Χ��1~4
  * @param  Column ��λ�ã���Χ��1~16
  * @param  Num Ҫ��ʾ�ķ��ŵ���ţ���Χ��0~255
  * @retval ��
  */
void OLED_ShowBmpMini(uint8_t Line, uint8_t Column, uint8_t Num)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//���ù��λ�����ϰ벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_BMP_8x16[Num][i]);			//��ʾ�ϰ벿������
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//���ù��λ�����°벿��
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_BMP_8x16[Num][i + 8]);		//��ʾ�°벿������
	}
}


/**
  * @brief  OLED��ʾ�ַ���
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  String Ҫ��ʾ���ַ�������Χ��ASCII�ɼ��ַ�
  * @retval ��
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED�η�����
  * @retval ����ֵ����X��Y�η�
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED��ʾ���֣�ʮ���ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~4294967295
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED��ʾ���֣�ʮ���ƣ�����������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��-2147483648~2147483647
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED��ʾ���֣�ʮ�����ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~0xFFFFFFFF
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~8
  * @retval ��
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED��ʾ���֣������ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~1111 1111 1111 1111
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~16
  * @retval ��
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}


/**
  * @brief  OLED��ʼ��
  * @param  ��
  * @retval ��
  */
void OLED_Init(void)
{
	uint16_t i, j, z;
	uint8_t OledInitFlag = 1;

	for (z = 0; z < 5; z++)
	{
		// printf("��ʼ����������: %d\r\n", z);
		for (i = 0; i < 1000; i++)			//�ϵ���ʱ
		{
			for (j = 0; j < 1000; j++);
		}
		
		OLED_I2C_Init();			//�˿ڳ�ʼ��
		
		if(OLED_WriteCommand(0xAE) == 0) OledInitFlag = 0;	//�ر���ʾ
		
		if(OLED_WriteCommand(0xD5) == 0) OledInitFlag = 0;	//������ʾʱ�ӷ�Ƶ��/����Ƶ��
		if(OLED_WriteCommand(0x80) == 0) OledInitFlag = 0;
		
		if(OLED_WriteCommand(0xA8) == 0) OledInitFlag = 0;	//���ö�·������
		if(OLED_WriteCommand(0x3F) == 0) OledInitFlag = 0;
		
		if(OLED_WriteCommand(0xD3) == 0) OledInitFlag = 0;	//������ʾƫ��
		if(OLED_WriteCommand(0x00) == 0) OledInitFlag = 0;
		
		if(OLED_WriteCommand(0x40) == 0) OledInitFlag = 0;	//������ʾ��ʼ��
		
		#if SHOW_INVERSION
		if(OLED_WriteCommand(0xA0) == 0) OledInitFlag = 0;	//�������ҷ���0xA1���� 0xA0���ҷ���
		if(OLED_WriteCommand(0xC0) == 0) OledInitFlag = 0;	//�������·���0xC8���� 0xC0���·���
		#else
		if(OLED_WriteCommand(0xA1) == 0) OledInitFlag = 0;
		if(OLED_WriteCommand(0xC8) == 0) OledInitFlag = 0;
		#endif

		if(OLED_WriteCommand(0xDA) == 0) OledInitFlag = 0;	//����COM����Ӳ������
		if(OLED_WriteCommand(0x12) == 0) OledInitFlag = 0;  //0.96(128*64)������Ϊ0x12��0.91(128*32)������Ϊ0x02
		
		if(OLED_WriteCommand(0x81) == 0) OledInitFlag = 0;	//���öԱȶȿ���
		if(OLED_WriteCommand(0xCF) == 0) OledInitFlag = 0;

		if(OLED_WriteCommand(0xD9) == 0) OledInitFlag = 0;	//����Ԥ�������
		if(OLED_WriteCommand(0xF1) == 0) OledInitFlag = 0;

		if(OLED_WriteCommand(0xDB) == 0) OledInitFlag = 0;	//����VCOMHȡ��ѡ�񼶱�
		if(OLED_WriteCommand(0x30) == 0) OledInitFlag = 0;

		if(OLED_WriteCommand(0xA4) == 0) OledInitFlag = 0;	//����������ʾ��/�ر�

		if(OLED_WriteCommand(0xA6) == 0) OledInitFlag = 0;	//��������/��ת��ʾ

		if(OLED_WriteCommand(0x8D) == 0) OledInitFlag = 0;	//���ó���
		if(OLED_WriteCommand(0x14) == 0) OledInitFlag = 0;

		if(OLED_WriteCommand(0xAF) == 0) OledInitFlag = 0;	//������ʾ
			
		OLED_Clear();				//OLED����

		if(OledInitFlag)
		{
			break;
		}
	}
}


/// @brief ����IICģ���ʱ���Ƿ���
/// @param parameter ENABLE or DISABLE
void OLED_Clock_Control(uint8_t parameter)
{
	if(parameter)
	{
		OLED_I2C_APBxClkCmd(OLED_I2C_CLK, ENABLE);
	}
	else
	{
		OLED_I2C_APBxClkCmd(OLED_I2C_CLK, DISABLE);
	}
}
