#include "stm32f10x.h"
#include "task.h"
#include "bsp_adc.h"
#include "bsp_usart.h"
#include "bsp_OLED.h"
#include "delay.h"
#include "bsp_SC8701.h"
#include "KeyInputFsm.h"
#include "bsp_key.h"
#include "bsp_i2c_ee.h"	
#include "bsp_SIQ02FVS3.h"
#include "bsp_iwdg.h"


#define  WRITE_FLAG_ADDR           0
#define  SAVE_FLAG_ADDR            1
#define  START_SAVE_ADDR           4


uint16_t Current;
uint16_t Voltage;
uint32_t Power;
uint8_t ShowTimeCount;   //状态闪烁显示时间间隔计数
uint8_t NormalCount;     //状态恢复正常所需的时间计数
uint16_t SlowTaskCount;  //间隔执行的任务的时间计数
uint8_t KeyNum;

uint8_t SettingParameter_wait;
uint8_t SettingParameter_Vol;
uint8_t SettingParameter_Power;


typedef enum            //这里枚举的值兼具优先级的功能
{
    Abnormal = 1,       //异常
    Overrun,            //超限
    Max_Cur_Tip,        //提示最大电流
    normal,             //正常
}State;
State state = normal;


float InPut_Voltage_Filter(void);
uint8_t Current_Filter(float* tmp);
uint8_t Voltage_Filter(float* tmp);
void Short_Circuit_Protection(void);
void Set_State(State state_Tmp);


//========================================    显示任务    ========================================//

/// @brief 显示：511实验室
/// @param  NULL
void Show_Init(void)
{
    Delay_ms(100);
    OLED_Clear();
    Delay_ms(100);
    OLED_ShowString(2, 4, "511");
    OLED_ShowChinese(2, 8, 0);
    OLED_ShowChinese(2, 10, 1);
    OLED_ShowChinese(2, 12, 2);
}


/// @brief 显示：清屏并显示界面
/// @param  NULL
void Show_UI(void)
{
    // OLED_Clear();
	OLED_ShowString(1, 1, "I:");
	OLED_ShowString(1, 9, "P:");
	OLED_ShowString(2, 1, "U:");
}


/// @brief 显示：清屏并显示 “短路, 请重启”
/// @param  NULL
void Show_Short_Circuit(void)
{
    OLED_Clear();
    OLED_ShowChinese(2, 3, 9);
    OLED_ShowChinese(2, 5, 10);
    OLED_ShowString(2, 7, ", ");
    OLED_ShowChinese(2, 9, 11);
    OLED_ShowChinese(2, 11, 22);
    OLED_ShowChinese(2, 13, 5);
}


void Show_State(void)
{
    static uint8_t flag = 0;

    if(NormalCount > 100)  //设置 n*10 ms后恢复normal
    {
        state = normal;
    }

    if(ShowTimeCount >= 30)
    {
        if(state == normal)
        {
            OLED_ShowString(2, 9, "Normal  ");
        }
        else
        {
            if(flag)
            {
                flag = 0;
                OLED_ShowString(2, 9, "        ");
            }
            else
            {
                flag = 1;

                switch (state)
                {
                case Abnormal:
                    OLED_ShowString(2, 9, "Abnormal");
                    break;

                case Overrun:
                    OLED_ShowString(2, 9, "Overrun");
                    break;

                case Max_Cur_Tip:
                    OLED_ShowString(2, 9, "MAX: 6A");
                    break;
                
                default:
                    break;
                }
            }
        }

        ShowTimeCount = 0;
    }
}


/// @brief 显示logo或者看门狗触发标志
/// @param  NULL
void Show_logo(void)
{
    if(SettingParameter_wait == 0)
    {
        if(IWDG_GetFlag())
        {
            OLED_ShowString (2, 7, "    ");
            OLED_ShowString (2, 7, "IWDG");
            Delay_ms(150);
            OLED_ShowString (2, 7, "    ");
        }
        else
        {
            OLED_ShowBMP(1, 32, 0);
            IWDG_Feed();
            Delay_ms(500);
            IWDG_Feed();
        }
    }
}


//===============================================================================================//


void Check_Setting(void)
{
    uint16_t FieldTmp = Read_SettingField();

    if((FieldTmp & 0x8000) == 0x0000)
    {
        SettingParameter_wait = 0;
    }
    else if((FieldTmp & 0x8000) == 0x8000)
    {
        SettingParameter_wait = 1;
    }

    if((FieldTmp & 0x4000) == 0x0000)
    {
        Set_Vol_UpperLimit(LIMIT_LOW);
        SettingParameter_Vol = 0;
    }
    else if((FieldTmp & 0x4000) == 0x4000)
    {
        Set_Vol_UpperLimit(LIMIT_HIGH);
        SettingParameter_Vol = 1;
    }

    if((FieldTmp & 0x2000) == 0x0000)
    {
        Set_Power_UpperLimit(LIMIT_LOW);
        SettingParameter_Power = 0;
    }
    else if((FieldTmp & 0x2000) == 0x2000)
    {
        Set_Power_UpperLimit(LIMIT_HIGH);
        SettingParameter_Power = 1;
    }
}


/// @brief 遍历式存储，延缓寿命
/// @param  NULL
void EE_Ergodic_Save(void)
{
    uint8_t Ergodic_flag = 0;
    uint16_t VolTmp = 0;
    uint16_t CurTmp = 0;

    I2C_EE_BufferRead(&Ergodic_flag, SAVE_FLAG_ADDR, 1);
    // printf("当前存储地址：%d\r\n", Ergodic_flag);
    
    if(Ergodic_flag == 0xFF || Ergodic_flag == 0x00)
    {
        EESaveAddr = START_SAVE_ADDR;
        I2C_EE_BufferWrite(&EESaveAddr, SAVE_FLAG_ADDR, 1);
    }
    else if(Ergodic_flag == 252)
    {
        I2C_EE_BufferRead((void*)&VolTmp, Ergodic_flag, DATA_SIZE);
        I2C_EE_BufferRead((void*)&CurTmp, Ergodic_flag + DATA_SIZE, DATA_SIZE);
        EESaveAddr = START_SAVE_ADDR;
        I2C_EE_BufferWrite((void*)&VolTmp, EESaveAddr, DATA_SIZE);
        I2C_EE_BufferWrite((void*)&CurTmp, EESaveAddr + DATA_SIZE, DATA_SIZE);

        I2C_EE_BufferWrite(&EESaveAddr, SAVE_FLAG_ADDR, 1);
    }
    else
    {
        I2C_EE_BufferRead((void*)&VolTmp, Ergodic_flag, DATA_SIZE);
        I2C_EE_BufferRead((void*)&CurTmp, Ergodic_flag + DATA_SIZE, DATA_SIZE);
        EESaveAddr = Ergodic_flag + (DATA_SIZE * 2);
        I2C_EE_BufferWrite((void*)&VolTmp, EESaveAddr, DATA_SIZE);
        I2C_EE_BufferWrite((void*)&CurTmp, EESaveAddr + DATA_SIZE, DATA_SIZE);

        I2C_EE_BufferWrite(&EESaveAddr, SAVE_FLAG_ADDR, 1);
    }
}


/// @brief 寻找EEPROM中保存的设定值
/// @param  NULL
void Search_Reserve_VolAndCur(void)
{
    uint8_t Write_flag = 0;
    uint16_t VolTmp = 0;
    uint16_t CurTmp = 0;

    I2C_EE_BufferRead(&Write_flag, WRITE_FLAG_ADDR, 1);
    EE_Ergodic_Save();

    if(Write_flag != 0xCD)  //如果没有写入过
    {
        Write_flag = 0xCD;
        I2C_EE_BufferWrite(&Write_flag, WRITE_FLAG_ADDR, 1);
        Save_VolRestrict_ToRom(500);
        Save_CurRestrict_ToRom(50);
        SC8701_Set_Current(50);
        SC8701_Set_Voltage(500);
        UpData_VolRestrictAndCurRestrict(500, 50);
    }
    else if(Write_flag == 0xCD)
    {
        VolTmp = Read_VolRestrict_FromRom();
        CurTmp = Read_CurRestrict_FromRom();

        CurTmp = SetMax_Current(VolTmp, CurTmp);  //判断电流是否超过功率限制

        if(VolTmp <= MAX_VOLTAGE && CurTmp <= MAX_CURRENT)
        {
            if(SettingParameter_Vol == 0 && VolTmp > 2500)
            {
                VolTmp = 2500;
            }
            UpData_VolRestrictAndCurRestrict(VolTmp, CurTmp);
            SC8701_Set_Current(CurTmp);
            SC8701_Set_Voltage(VolTmp);
        }
        else
        {
            UpData_VolRestrictAndCurRestrict(500, 50);
            SC8701_Set_Current(50);
            SC8701_Set_Voltage(500);
        }
    }
}


/// @brief 检查按键并触发事件
/// @param  NULL
void KeyInput_Detection(void)
{
    KeyNum = Key_Scan();
    if (KeyNum == 0)
    {
        KeyNum = SIQ02FVS3_Key_Scan();
    }

    switch (KeyNum)
    {
    case 0:
        break;
    case 1:
        KeyInputFSM_SetEvent(EVENT_KEY1);
        break;
    case 2:
        KeyInputFSM_SetEvent(EVENT_KEY2);
        break;
    case 3:
        KeyInputFSM_SetEvent(EVENT_KEY3);
        break;
    case 4:
        KeyInputFSM_SetEvent(EVENT_KEY1_DOUBLE);
        break;
    case 5:
        KeyInputFSM_SetEvent(EVENT_KEY2_DOUBLE);
        break;
    case 6:
        KeyInputFSM_SetEvent(EVENT_KEY3_DOUBLE);
        break;
    case 7:
        KeyInputFSM_SetEvent(EVENT_KEY1_LONG);
        break;
    case 8:
        KeyInputFSM_SetEvent(EVENT_KEY2_LONG);
        break;
    case 9:
        KeyInputFSM_SetEvent(EVENT_KEY3_LONG);
        break;
    case 10:
        KeyInputFSM_SetEvent(EVENT_KEY4);
        break;
    default:
        break;
    }
}


/// @brief 根据输入电压提示最大功率
/// @param InPut_Vol 输入电压
void Show_MaxPower(float InPut_Vol)
{
    static float LastVol = 0;
    
    if(InPut_Vol != LastVol)
    {
        LastVol = InPut_Vol;

        if(InPut_Vol == 5)
        {
            OLED_ShowChinese(4, 4, 3);
            OLED_ShowChinese(4, 6, 4);
            OLED_ShowChinese(4, 8, 23);
            OLED_ShowChinese(4, 10, 24);
        }
        else if(InPut_Vol == 9)
        {
            OLED_ShowString (3, 2, "            ");
            OLED_ShowString(4, 4, " MAX: 18W    ");
        }
        else if(InPut_Vol == 12)
        {
            OLED_ShowString (3, 2, "            ");
            OLED_ShowString(4, 4, " MAX: 18W    ");
        }
        else if(InPut_Vol == 15)
        {
            OLED_ShowString (3, 2, "            ");
            OLED_ShowString(4, 4, " MAX: 22.5W  ");
        }
        else if(InPut_Vol == 20)
        {
            OLED_ShowString (3, 2, "            ");
            if(SettingParameter_Power == 0)
            {
                OLED_ShowString(4, 4, " MAX: 65W    ");
            }
            else
            {
                OLED_ShowString(4, 4, " MAX: 100W   ");
            }
        }
    }
}


/// @brief 检测输入电压值、显示、使能
/// @param  NULL
void Detection_InPut_Voltage(void)
{
    uint8_t VoltageInteger = 0;  //整数位
    uint8_t i = 0;
    uint8_t count = 12;

    OLED_Clear();
    OLED_ShowChinese(2, 2, 11);  //请求电压
    OLED_ShowChinese(2, 4, 14);
    OLED_ShowChinese(2, 6, 7);
    OLED_ShowChinese(2, 8, 8);
    OLED_ShowString (2, 10, ":");

    if(IWDG_GetFlag())
    {
        count = 1;
        IWDG_CleanFlag();
    }
    else if(SettingParameter_wait)  //加快检测
    {
        count = 8;
    }

    for (i = 0; i < count; i++)     //显示n*200ms
    {
        IWDG_Feed();  //喂狗
        InPutVoltage = 10 * InPut_Voltage_Filter();  //乘10
        VoltageInteger = InPutVoltage;
        // printf("PA7的电压值: %f\r\n", Voltage);

        if(InPutVoltage < 8)
        {
            OLED_ShowChinese(3, 2, 7);
            OLED_ShowChinese(3, 4, 8);
            OLED_ShowChinese(3, 6, 15);
            OLED_ShowString (3, 8, " >= 9V");
        }

        if(InPutVoltage < 10)
        {
            OLED_ShowNum(2, 12, VoltageInteger, 1);
            OLED_ShowChar(2, 13, '.');
            OLED_ShowNum(2, 14, (InPutVoltage - VoltageInteger) * 100, 2);
            Delay_ms(200);
        }
        else
        {
            OLED_ShowNum(2, 11, VoltageInteger, 2);
            OLED_ShowChar(2, 13, '.');
            OLED_ShowNum(2, 14, (InPutVoltage - VoltageInteger) * 100, 2);
            Delay_ms(200);
        }

        if(InPutVoltage >= 4 && InPutVoltage <= 6)
        {
            InPutVoltage = 5;
        }
        else if(InPutVoltage >= 8 && InPutVoltage <= 10)
        {
            InPutVoltage = 9;
        }
        else if(InPutVoltage >= 11 && InPutVoltage <= 13)
        {
            InPutVoltage = 12;
        }
        else if(InPutVoltage >= 14 && InPutVoltage <= 16)
        {
            InPutVoltage = 15;
        }
        else if(InPutVoltage >= 19 && InPutVoltage <= 22)
        {
            InPutVoltage = 20;
        }
        else if(InPutVoltage > 22)  //错误的输入电压
        {
            OLED_Clear();
            OLED_ShowString (2, 4, "Fault_Vol");
            while (1)
            {
                IWDG_Feed();  //死循环
            }
        }

        Show_MaxPower(InPutVoltage);
    }

    Search_Reserve_VolAndCur();    //设置输出
}


/// @brief 获取电压值并显示
/// @param  NULL
void Detection_Voltage(void)
{
    float tmp;
    static uint8_t Overrun_Flag;

    if(Voltage_Filter(&tmp) == TURE)
    {
        Voltage = (tmp * 81 + 49)  / 100;  //线性回归_0
        // Voltage = (tmp * 82 + 190) / 100;  //线性回归_1
        // printf("输出电压值: %d\r\n", Voltage);

        if(Voltage < 1000)
        {
            OLED_ShowNum(2, 3, Voltage / 100, 1);
            OLED_ShowChar(2, 4, '.');
            OLED_ShowNum(2, 5, Voltage % 100, 2);
            OLED_ShowChar(2, 7, ' ');  //清除
        }
        else
        {
            OLED_ShowNum(2, 3, Voltage / 100, 2);
            OLED_ShowChar(2, 5, '.');
            OLED_ShowNum(2, 6, Voltage % 100, 2);
        }

        if((Get_VolRestrict() - Voltage) > 100 && (SC8701_CE_Read() == CE_ENABLE))  //偏差相差1V，定义为输出失败
        {
            Overrun_Flag++;
            if(Overrun_Flag >= 2)
            {
                Set_State(Overrun);
            }
        }
        else
        {
            Overrun_Flag = 0;
        }
    }
}


/// @brief 获取电流值并显示
/// @param  NULL
/// @return 数据是否更新
uint8_t Detection_Current(void)
{
    float tmp;
    uint8_t UpDataFlag = 0;
    uint16_t Current_Precise = 0;  //用于显示mA位

    if(Current_Filter(&tmp) == TURE)
    {
        UpDataFlag = 1;  //数据已更新
        Current_Precise = (float)(tmp * 16 - 27) / 10;  //线性回归
        Current = Current_Precise / 10;
        // printf("电流AD值: %f\r\n", tmp);
        
        if(Current_Precise < 5)
        {
            Current_Precise = 0;  //省得功率一直跳
        }
        
        OLED_ShowNum(1, 3, Current_Precise / 1000, 1);
        OLED_ShowChar(1, 4, '.');
        OLED_ShowNum(1, 5, Current_Precise % 1000, 3);

        if(Current > 350)
        {
            Set_State(Max_Cur_Tip);
        }
    }

    return UpDataFlag;
}


/// @brief 获取功率值并显示
/// @param  NULL
void Detection_Power(void)
{
    static uint32_t LastPowerVal = 0;

    Power = Voltage * Current;

    if(Power != LastPowerVal)
    {
        LastPowerVal = Power;

        if(Power < 100000)
        {
            OLED_ShowNum(1, 11, Power / 10000, 1);
            OLED_ShowChar(1, 12, '.');
            OLED_ShowNum(1, 13, (Power % 10000) / 100, 2);
            OLED_ShowChar(1, 15, ' ');  //清除
            OLED_ShowChar(1, 16, ' ');  //清除
        }
        else if(Power < 1000000)
        {
            OLED_ShowNum(1, 11, Power / 10000, 2);
            OLED_ShowChar(1, 13, '.');
            OLED_ShowNum(1, 14, (Power % 10000) / 100, 2);
            OLED_ShowChar(1, 16, ' ');  //清除
        }
        else
        {
            OLED_ShowNum(1, 11, Power / 10000, 3);
            OLED_ShowChar(1, 14, '.');
            OLED_ShowNum(1, 15, (Power % 10000) / 100, 2);
            Set_State(Abnormal);
        }
    }
}


/// @brief 浮点数冒泡排序，从小到大
/// @param arr 存有数据的数组地址
/// @param sz 数组长度
void bubble_sort(float* arr, uint8_t sz)
{
	int i = 0;
    int flag = 0;
	int j = 0;
    float tmp = 0;

	for (i = 0; i < sz - 1; i++)//确定冒泡趟数
	{
		flag = 1;
		j = 0;
		for (j = 0; j < sz - 1 - i; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
				flag = 0;       //优化，如果flag = 1说明不需要排或已经排完
			}
		}
		if (1 == flag)
		{
			break;
		}
	}
}


/// @brief 数冒泡排序，从小到大
/// @param arr 存有数据的数组地址
/// @param sz 数组长度
void bubble_sort_integer(uint16_t* arr, uint8_t sz)
{
	int i = 0;
    int flag = 0;
	int j = 0;
    uint16_t tmp = 0;

	for (i = 0; i < sz - 1; i++)//确定冒泡趟数
	{
		flag = 1;
		j = 0;
		for (j = 0; j < sz - 1 - i; j++)
		{
			if (arr[j] > arr[j + 1])
			{
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
				flag = 0;       //优化，如果flag = 1说明不需要排或已经排完
			}
		}
		if (1 == flag)
		{
			break;
		}
	}
}


/// @brief 输入电压ADC采样滤波，多次采样，排序后掐头去尾求平均
/// @param  NULL
/// @return 浮点平均值
float InPut_Voltage_Filter(void)
{
    float FilArr[FilterNum];
    uint8_t i = 0;
    float sum = 0;

    for (i = 0; i < FilterNum; i++)
    {
        Delay_ms(1);
        FilArr[i] = ADC1_Get_Channel_7_Voltaged();
    }
    
    bubble_sort(&FilArr[0], FilterNum);
    // for (i = 0; i < FilterNum; i++)
    // {
    //     printf("滤波：%d = %f\r\n", i, FilArr[i]);
    // }

    for (i = 0; i < FilterNum - 10; i++)
    {
        sum += FilArr[i + 5];
    }
    
    return (float)(sum / (FilterNum - 10));
}


// /// @brief 电压ADC采样滤波，多次采样，排序后掐头去尾求平均
// /// @param  tmp 存放数据的指针
// /// @return 是否完成
// uint8_t Voltage_Filter(float* tmp)
// {
//     static float FilArr[FilterNum];
//     static uint8_t i = 0;
//     static float sum = 0;

//     FilArr[i] = ADC1_Get_Channel_8_Voltaged();
//     i++;

//     if(i == FilterNum)
//     {
//         bubble_sort(&FilArr[0], FilterNum);
//         // for (i = 0; i < FilterNum; i++)
//         // {
//         //     printf("滤波：%d = %f\r\n", i, FilArr[i]);
//         // }

//         for (i = 0; i < FilterNum - 10; i++)
//         {
//             sum += FilArr[i + 5];
//         }
        
//         *tmp = (float)(sum / (FilterNum - 10));
//         i = 0;
//         sum = 0;
//         return TURE;
//     }
    
//     return FALSE;
// }


// /// @brief 电流ADC采样滤波，多次采样，排序后掐头去尾求平均
// /// @param  tmp 存放数据的指针
// /// @return 是否完成
// uint8_t Current_Filter(float* tmp)
// {
//     static float FilArr[FilterNum];
//     static uint8_t i = 0;
//     static float sum = 0;

//     FilArr[i] = ADC1_Get_Channel_9_Voltaged();
//     i++;

//     if(i == FilterNum)
//     {
//         bubble_sort(&FilArr[0], FilterNum);
//         // for (i = 0; i < FilterNum; i++)
//         // {
//         //     printf("滤波：%d = %f\r\n", i, FilArr[i]);
//         // }

//         for (i = 0; i < FilterNum - 10; i++)
//         {
//             sum += FilArr[i + 5];
//         }
        
//         *tmp = (float)(sum / (FilterNum - 10));
//         i = 0;
//         sum = 0;
//         return TURE;
//     }
    
//     return FALSE;
// }


/// @brief 电压ADC采样滤波，多次采样，排序后掐头去尾求平均
/// @param  tmp 存放数据的指针
/// @return 是否完成
uint8_t Voltage_Filter(float* tmp)
{
    static uint16_t FilArr[FilterNum];
    static uint8_t i = 0;
    static uint32_t sum = 0;

    FilArr[i] = ADC1_Get_Channel_8_Val();
    i++;

    if(i == FilterNum)
    {
        bubble_sort_integer(&FilArr[0], FilterNum);
        // for (i = 0; i < FilterNum; i++)
        // {
        //     printf("滤波：%d = %f\r\n", i, FilArr[i]);
        // }

        for (i = 0; i < FilterNum - 10; i++)
        {
            sum += FilArr[i + 5];
        }
        
        *tmp = (float)(sum / (FilterNum - 10));
        i = 0;
        sum = 0;
        return TURE;
    }
    
    return FALSE;
}


/// @brief 电流ADC采样滤波，多次采样，排序后掐头去尾求平均
/// @param  tmp 存放数据的指针
/// @return 是否完成
uint8_t Current_Filter(float* tmp)
{
    static uint16_t FilArr[FilterNum];
    static uint8_t i = 0;
    static uint32_t sum = 0;

    FilArr[i] = ADC1_Get_Channel_9_Val();
    i++;

    if(i == FilterNum)
    {
        bubble_sort_integer(&FilArr[0], FilterNum);
        // for (i = 0; i < FilterNum; i++)
        // {
        //     printf("滤波：%d = %d\r\n", i, FilArr[i]);
        // }

        for (i = 0; i < FilterNum - 10; i++)
        {
            sum += FilArr[i + 5];
        }
        
        *tmp = (float)(sum / (FilterNum - 10));
        i = 0;
        sum = 0;
        return TURE;
    }
    
    return FALSE;
}


/// @brief 短路保护
/// @param  NULL
void Short_Circuit_Protection(void)
{
    Show_Short_Circuit();
    SC8701_CE_Control(CE_DESABLE);  //立刻失能SC8701

    while(1)
    {
        IWDG_Feed();
    }
}


/// @brief 短路检测
/// @param  NULL
void Short_Circuit_Detection(void)
{
    uint8_t Count = 0;  //防止死循环

    if(ADC1_Get_Channel_9_Val() > 4000 && ADC1_Get_Channel_9_Val() > 4000)  //检测电流
    {   
        if(Get_PDPowerSupplyState() == POWER_SUPPLY_EXTINGUISH)
        {
            OLED_Clock_Control(ENABLE);
        }

        while((Detection_Current() == FALSE) && (Count < FilterNum))
        {
            Count++;
            Delay_ms(1);  //影响检测灵敏度
        }

        if(Current > 6)
        {
            Count = 0;
            Short_Circuit_Protection();  //触发短路保护
        }
    }
    else
    {
        if(Get_PDPowerSupplyState() == POWER_SUPPLY_EXTINGUISH)
        {
            OLED_Clock_Control(DISABLE);
        }

        Count = 0;
    }
}


/// @brief 设置状态
/// @param state_Tmp 要设置的状态
void Set_State(State state_Tmp)
{
    if(state_Tmp != normal)
    {
        NormalCount = 0;
    }

    if(state_Tmp < state)  //比较优先级
    {
        state = state_Tmp;
    }
}
