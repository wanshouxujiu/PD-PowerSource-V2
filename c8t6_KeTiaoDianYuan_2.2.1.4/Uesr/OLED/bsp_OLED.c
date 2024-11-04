#include "stm32f10x.h"
#include "OLED_Font.h"
#include "bsp_OLED.h"
#include "bsp_usart.h"
#include "delay.h"


__IO uint32_t  I2CTimeout = I2CT_FLAG_TIMEOUT;


/// @brief 超时处理
/// @param errorCode 错误代码
/// @return 0 表示错误
static  uint32_t I2C_TimeoutEvent(uint8_t errorCode)
{
  printf("I2C 等待超时! 错误代码: EV_%d\r\n", errorCode);
  
  return 0;
}


/*引脚初始化*/
void OLED_I2C_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
	//打开I2CGPIO时钟
	OLED_I2C_GPIO_APBxClkCmd(OLED_I2C_SCL_GPIO_CLK | OLED_I2C_SDA_GPIO_CLK, ENABLE);
	//打开I2C外设时钟
	OLED_I2C_APBxClkCmd(OLED_I2C_CLK, ENABLE);
	
	//I2C SCL
	GPIO_InitStructure.GPIO_Pin = OLED_I2C_SCL_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(OLED_I2C_SCL_GPIO_PORT,&GPIO_InitStructure);
	//I2C SDA
	GPIO_InitStructure.GPIO_Pin = OLED_I2C_SDA_GPIO_PIN;
	GPIO_Init(OLED_I2C_SDA_GPIO_PORT,&GPIO_InitStructure);
	
	//配置I2C工作参数
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;  //应答使能，基本大部分I2C都要应答
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  //地址长度设置为7bit
	I2C_InitStructure.I2C_ClockSpeed = OLED_I2C_BAUDRATE;    //波特率配置
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;       //占空比，一般可随意选择
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;               //选择I2C模式
	I2C_InitStructure.I2C_OwnAddress1 = STM32_I2C_OWN_ADDR;  //设置自身地址
	I2C_Init(OLED_I2C,&I2C_InitStructure);
	
	//最后使能I2C
	I2C_Cmd(OLED_I2C, ENABLE);
}


/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
uint8_t OLED_WriteCommand(uint8_t Command)
{
	//产生起始信号
	I2C_GenerateSTART(OLED_I2C, ENABLE);

	/* 检测EV5事件并清除标志位 */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_MODE_SELECT))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(5);
	}

	I2C_Send7bitAddress(OLED_I2C, OLED_ADDR, I2C_Direction_Transmitter);

	/* 检测EV6事件并清除标志位 */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(6);
	}

	I2C_SendData(OLED_I2C, 0x00);   //写命令

	/* 检测EV8事件并清除标志位 */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(8);
	}

	I2C_SendData(OLED_I2C, Command);

	/* 检测EV8_2事件并清除标志位 */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(82);
	}

	I2C_GenerateSTOP(OLED_I2C, ENABLE);
	Delay_us(5);  //快速模式下，不等待容易出错

	return 1;
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
uint8_t OLED_WriteData(uint8_t Data)
{
	//产生起始信号
	I2C_GenerateSTART(OLED_I2C, ENABLE);

	/* 检测EV5事件并清除标志位 */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_MODE_SELECT))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(5);
	}

	I2C_Send7bitAddress(OLED_I2C, OLED_ADDR, I2C_Direction_Transmitter);

	/* 检测EV6事件并清除标志位 */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(6);
	}

	I2C_SendData(OLED_I2C, 0x40);   //写命令

	/* 检测EV8事件并清除标志位 */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(8);
	}
	
	I2C_SendData(OLED_I2C, Data);

	/* 检测EV8_2事件并清除标志位 */
	I2CTimeout = I2CT_FLAG_TIMEOUT;  
	while(!I2C_CheckEvent(OLED_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))  
	{
	if((I2CTimeout--) == 0) return I2C_TimeoutEvent(82);
	}
	
	I2C_GenerateSTOP(OLED_I2C, ENABLE);
	Delay_us(5);  //快速模式下，不等待容易出错

	return 1;
}


//===================================以上为I2C底层构件=================================//


/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
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
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}


/**
  * @brief  OLED显示一个数字的阳码
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Num 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowNum_Bright(uint8_t Line, uint8_t Column, uint8_t Num)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16_Bright[Num][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16_Bright[Num][i + 8]);		//显示下半部分内容
	}
}


/**
  * @brief  OLED显示一个汉字
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Num 要显示的一个汉字序号
  * @retval 无
  */
void OLED_ShowChinese(uint8_t Line, uint8_t Column, uint8_t Num)
{      	
	uint8_t i;

	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 16; i++)
	{
		OLED_WriteData(OLED_F16x16[Num][i]);					//显示上半部分内容
	}

	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 16; i++)
	{
		OLED_WriteData(OLED_F16x16[Num][i + 16]);					//显示下半部分内容
	}
}


/**
  * @brief  OLED显示一个BMP格式的图像，64*64像素
  * @param  Line 行位置，范围：1~8
  * @param  Column 列位置，范围：1~128
  * @param  0：校徽
  * @retval 无
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
  * @brief  OLED显示一个8*16自定义图像
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Num 要显示的符号的序号，范围：0~255
  * @retval 无
  */
void OLED_ShowBmpMini(uint8_t Line, uint8_t Column, uint8_t Num)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_BMP_8x16[Num][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_BMP_8x16[Num][i + 8]);		//显示下半部分内容
	}
}


/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
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
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
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
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
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
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
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
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
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
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
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
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	uint16_t i, j, z;
	uint8_t OledInitFlag = 1;

	for (z = 0; z < 5; z++)
	{
		// printf("初始化次数计数: %d\r\n", z);
		for (i = 0; i < 1000; i++)			//上电延时
		{
			for (j = 0; j < 1000; j++);
		}
		
		OLED_I2C_Init();			//端口初始化
		
		if(OLED_WriteCommand(0xAE) == 0) OledInitFlag = 0;	//关闭显示
		
		if(OLED_WriteCommand(0xD5) == 0) OledInitFlag = 0;	//设置显示时钟分频比/振荡器频率
		if(OLED_WriteCommand(0x80) == 0) OledInitFlag = 0;
		
		if(OLED_WriteCommand(0xA8) == 0) OledInitFlag = 0;	//设置多路复用率
		if(OLED_WriteCommand(0x3F) == 0) OledInitFlag = 0;
		
		if(OLED_WriteCommand(0xD3) == 0) OledInitFlag = 0;	//设置显示偏移
		if(OLED_WriteCommand(0x00) == 0) OledInitFlag = 0;
		
		if(OLED_WriteCommand(0x40) == 0) OledInitFlag = 0;	//设置显示开始行
		
		#if SHOW_INVERSION
		if(OLED_WriteCommand(0xA0) == 0) OledInitFlag = 0;	//设置左右方向，0xA1正常 0xA0左右反置
		if(OLED_WriteCommand(0xC0) == 0) OledInitFlag = 0;	//设置上下方向，0xC8正常 0xC0上下反置
		#else
		if(OLED_WriteCommand(0xA1) == 0) OledInitFlag = 0;
		if(OLED_WriteCommand(0xC8) == 0) OledInitFlag = 0;
		#endif

		if(OLED_WriteCommand(0xDA) == 0) OledInitFlag = 0;	//设置COM引脚硬件配置
		if(OLED_WriteCommand(0x12) == 0) OledInitFlag = 0;  //0.96(128*64)寸这里为0x12，0.91(128*32)寸这里为0x02
		
		if(OLED_WriteCommand(0x81) == 0) OledInitFlag = 0;	//设置对比度控制
		if(OLED_WriteCommand(0xCF) == 0) OledInitFlag = 0;

		if(OLED_WriteCommand(0xD9) == 0) OledInitFlag = 0;	//设置预充电周期
		if(OLED_WriteCommand(0xF1) == 0) OledInitFlag = 0;

		if(OLED_WriteCommand(0xDB) == 0) OledInitFlag = 0;	//设置VCOMH取消选择级别
		if(OLED_WriteCommand(0x30) == 0) OledInitFlag = 0;

		if(OLED_WriteCommand(0xA4) == 0) OledInitFlag = 0;	//设置整个显示打开/关闭

		if(OLED_WriteCommand(0xA6) == 0) OledInitFlag = 0;	//设置正常/倒转显示

		if(OLED_WriteCommand(0x8D) == 0) OledInitFlag = 0;	//设置充电泵
		if(OLED_WriteCommand(0x14) == 0) OledInitFlag = 0;

		if(OLED_WriteCommand(0xAF) == 0) OledInitFlag = 0;	//开启显示
			
		OLED_Clear();				//OLED清屏

		if(OledInitFlag)
		{
			break;
		}
	}
}


/// @brief 控制IIC模块的时钟是否开启
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
