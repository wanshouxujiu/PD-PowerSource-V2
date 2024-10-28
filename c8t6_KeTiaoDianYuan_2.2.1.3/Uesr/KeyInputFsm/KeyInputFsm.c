#include "KeyInputFsm.h"
#include "bsp_usart.h"
#include "bsp_OLED.h"
#include "bsp_SC8701.h"
#include "bsp_i2c_ee.h"
#include "delay.h"
#include "bsp_iwdg.h"

#define         PREDEFINED_NUM      3
#define         LEFT                0
#define         RIGHT               1
#define         ONLY_SHOW           2

static const char VersionNumber[] = "2.2.1.3";  //版本号：前两位为硬件后两位为软件

static KEY_EVENT_ID EventID = EVENT_VOID;
static KEY_STATUS StatusID = STATUS_NO_READY;
static uint16_t VolRestrict;                      //预设电压
static uint16_t CurRestrict;                      //预设电流
static uint8_t VolCurSetingFlag;                  //电压电流设置中标志 0为电压 1为电流
static uint8_t VolCursor;                         //光标位置
static uint8_t CurCursor;                         //光标位置
static uint8_t PredefinedCount = 2;               //使用的预设组的标识
static TIMER_ID TimeCountFlag = NO_TIMER;
static ENUM_OPTION_ID EnumFlag = ENUM_VERSIONS;
static uint16_t ExtraVol;                          //拓展电压部分
static uint32_t ExtraPower;                        //拓展功率部分
static POWER_SUPPLY_STATE_ID PDPowerSupplyState = POWER_SUPPLY_RUNNING;   //整个电源的工作状态

uint8_t AttonityTimeCountFlag;      //活动计时标志
uint16_t AttonityTimeCount;
uint8_t ExtinguishTimeCountFlag;    //熄屏计时标志
uint32_t ExtinguishTimeCount;
uint8_t EESaveAddr;                 //电压电流数据在 EEPROM 中存储的位置


static void MenuShow_Versions(void);
static void MenuShow_Wait(void);
static void MenuShow_Vol(void);
static void MenuShow_Power(void);


struct Predefined_OutPut_S
{
    uint16_t PreVol;
    uint16_t PreCur;
};

//预先定义的输出参数组
struct Predefined_OutPut_S Predefined_OutPut_Array[PREDEFINED_NUM] = \
{
    330,
    50,

    500,
    100,

    1200,
    150
};


void(*MenuShowOption[ENUM_OPTION_NUM])(void) = 
{
    MenuShow_Versions,
    MenuShow_Wait,
    MenuShow_Vol,
    MenuShow_Power,
};


/// @brief 读取设置参数字节
/// @param  NULL
/// @return 保存的设置参数
uint16_t Read_SettingField(void)
{
    uint16_t Tmp;

    I2C_EE_BufferRead((void*)&Tmp, SETTING_FIELD_ADDR, 2);
    // printf("seting: %x\r\n", Tmp);

    return Tmp;
}


/// @brief 保存设置参数
/// @param Field 设置参数
void Write_SettingField(uint16_t Field)
{
    I2C_EE_BufferWrite((void*)&Field, SETTING_FIELD_ADDR, 2);
    Delay_ms(10);  //不延时可能因为EEPROM没有写完，会导致卡死
}


static void Menu_Set_Wait(uint8_t direction)
{
    uint16_t FieldTmp = Read_SettingField();

    OLED_ShowString(4, 1, "                ");
    OLED_ShowChinese(4, 1, 32);
    OLED_ShowChinese(4, 3, 33);
    OLED_ShowChar(4, 5,  ':');

    if(direction == LEFT)
    {
        FieldTmp &= 0x7FFF;

        OLED_ShowChinese(4, 15, 19);  //开
    }
    else if(direction == RIGHT)
    {
        FieldTmp |= 0x8000;

        OLED_ShowChinese(4, 15, 20);  //关
    }
    else if(direction == ONLY_SHOW)
    {
        switch (FieldTmp & 0x8000)
        {
        case 0x0000:
            OLED_ShowChinese(4, 15, 19);  //开
            break;
        case 0x8000:
            OLED_ShowChinese(4, 15, 20);  //关
            break;
        default:
            break;
        }
    }
    Write_SettingField(FieldTmp);
}


static void Menu_Set_Vol(uint8_t direction)
{
    uint16_t FieldTmp = Read_SettingField();

    OLED_ShowString(4, 1, "                ");
    OLED_ShowChinese(4, 1, 32);
    OLED_ShowChinese(4, 3, 33);
    OLED_ShowChar(4, 5,  ':');

    if(direction == LEFT)
    {
        FieldTmp &= 0xBFFF;
        Write_SettingField(FieldTmp);
        Set_Vol_UpperLimit(LIMIT_LOW);

        OLED_ShowString(4, 14, "25V");
    }
    else if(direction == RIGHT)
    {
        FieldTmp |= 0x4000;
        Write_SettingField(FieldTmp);
        Set_Vol_UpperLimit(LIMIT_HIGH);

        OLED_ShowString(3, 1, "Must be operated");
        OLED_ShowString(4, 1, "on load  !!!");
        IWDG_Feed();
        Delay_ms(2000);
        IWDG_Feed();
        OLED_ShowString(3, 1, "                ");
        OLED_ShowString(4, 1, "                ");

        MenuShow_Vol();
        OLED_ShowChinese(4, 1, 32);
        OLED_ShowChinese(4, 3, 33);
        OLED_ShowChar(4, 5,  ':');
        OLED_ShowString(4, 14, "33V");
    }
    else if(direction == ONLY_SHOW)
    {
        switch (FieldTmp & 0x4000)
        {
        case 0x0000:
            OLED_ShowString(4, 14, "25V");
            break;
        case 0x4000:
            OLED_ShowString(4, 14, "33V");
            break;
        default:
            break;
        }
    }
}


static void Menu_Set_Power(uint8_t direction)
{
    uint16_t FieldTmp = Read_SettingField();

    OLED_ShowString(4, 1, "                ");
    OLED_ShowChinese(4, 1, 32);
    OLED_ShowChinese(4, 3, 33);
    OLED_ShowChar(4, 5,  ':');

    if(direction == LEFT)
    {
        FieldTmp &= 0xDFFF;
        Write_SettingField(FieldTmp);
        Set_Power_UpperLimit(LIMIT_LOW);

        OLED_ShowString(4, 14, "65W");
    }
    else if(direction == RIGHT)
    {
        FieldTmp |= 0x2000;
        Write_SettingField(FieldTmp);
        Set_Power_UpperLimit(LIMIT_HIGH);

        OLED_ShowString(4, 13, "100W");
    }
    else if(direction == ONLY_SHOW)
    {
        switch (FieldTmp & 0x2000)
        {
        case 0x0000:
            OLED_ShowString(4, 14, "65W");
            break;
        case 0x2000:
            OLED_ShowString(4, 13, "100W");
            break;
        default:
            break;
        }
    }
}


/// @brief 显示选项：版本号
/// @param  NULL
static void MenuShow_Versions(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowChar(3, 1,  '<');
    OLED_ShowChar(3, 16, '>');
    OLED_ShowChinese(3, 6, 0);
    OLED_ShowChinese(3, 8, 1);
    OLED_ShowChinese(3, 10, 2);
}


/// @brief 显示选项：上电等待
/// @param  NULL
static void MenuShow_Wait(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowChar(3, 1,  '<');
    OLED_ShowChar(3, 16, '>');
    OLED_ShowChinese(3, 5, 13);
    OLED_ShowChinese(3, 7, 28);
    OLED_ShowChinese(3, 9, 29);
    OLED_ShowChinese(3, 11, 30);
}


/// @brief 显示选项：电压限制
/// @param  NULL
static void MenuShow_Vol(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowChar(3, 1,  '<');
    OLED_ShowChar(3, 16, '>');
    OLED_ShowChinese(3, 5, 5);
    OLED_ShowChinese(3, 7, 8);
    OLED_ShowChinese(3, 9, 16);
    OLED_ShowChinese(3, 11, 27);
}


/// @brief 显示选项：最大功率限制
/// @param  NULL
static void MenuShow_Power(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowChar(3, 1,  '<');
    OLED_ShowChar(3, 16, '>');
    OLED_ShowChinese(3, 5, 3);
    OLED_ShowChinese(3, 7, 4);
    OLED_ShowChinese(3, 9, 16);
    OLED_ShowChinese(3, 11, 27);
}


void ShowPredefined(void)
{
    OLED_ShowChinese(3, 1, 17);
    OLED_ShowChinese(3, 3, 26);
    OLED_ShowString(3, 5, ": ");

    if(Predefined_OutPut_Array[PredefinedCount].PreVol >= 1000)
    {
        OLED_ShowNum(3, 7, Predefined_OutPut_Array[PredefinedCount].PreVol / 100, 2);
        OLED_ShowChar(3, 9, '.');
        OLED_ShowNum(3, 10, (Predefined_OutPut_Array[PredefinedCount].PreVol % 100) / 10, 1);
        OLED_ShowChar(3, 11, 'V');
    }
    else
    {
        OLED_ShowChar(3, 7, ' ');
        OLED_ShowNum(3, 8, Predefined_OutPut_Array[PredefinedCount].PreVol / 100, 1);
        OLED_ShowChar(3, 9, '.');
        OLED_ShowNum(3, 10, (Predefined_OutPut_Array[PredefinedCount].PreVol % 100) / 10, 1);
        OLED_ShowChar(3, 11, 'V');
    }

    OLED_ShowNum(3, 13, Predefined_OutPut_Array[PredefinedCount].PreCur / 100, 1);
    OLED_ShowChar(3, 14, '.');
    OLED_ShowNum(3, 15, (Predefined_OutPut_Array[PredefinedCount].PreCur % 100) / 10, 1);
    OLED_ShowChar(3, 16, 'A');
}


/// @brief 依据最大功率求和给出的电压设置最大电流，若电流*电压<最大功率，则返回给的电流
/// @param VolTmp 给出的电压
/// @param CurTmp 给出的电流
/// @return 应该设定的电流
uint16_t SetMax_Current(uint16_t VolTmp, uint16_t CurTmp)
{
    switch ((uint8_t)InPutVoltage)
    {
    case 9:
        if((VolTmp * CurTmp) > MAX_POWER_9V)
        {  
            CurTmp = MAX_POWER_9V / VolTmp;
        }
        break;
    case 12:
        if((VolTmp * CurTmp) > MAX_POWER_12V)
        {  
            CurTmp = MAX_POWER_12V / VolTmp;
        }
        break;
    case 15:
        if((VolTmp * CurTmp) > MAX_POWER_15V)
        {  
           CurTmp = MAX_POWER_15V / VolTmp; 
        }
        break;
    case 20:
        if((VolTmp * CurTmp) > ((MAX_POWER_20V) + ExtraPower))
        {  
            CurTmp = ((MAX_POWER_20V) + ExtraPower) / VolTmp;
        }
        break;
    default:
        break;
    }

    return CurTmp;
}


void Check_Power(void)
{
    if(VolCurSetingFlag == 0)
    {
        switch ((uint8_t)InPutVoltage)
        {
        case 5:
            break;
        case 9:
            if((VolRestrict * CurRestrict) > MAX_POWER_9V)
            {  
                CurRestrict = MAX_POWER_9V / VolRestrict;
            }
            break;
        case 12:
            if((VolRestrict * CurRestrict) > MAX_POWER_12V)
            {  
                CurRestrict = MAX_POWER_12V / VolRestrict;
            }
            break;
        case 15:
            if((VolRestrict * CurRestrict) > MAX_POWER_15V)
            {  
                CurRestrict = MAX_POWER_15V / VolRestrict;
            }
            break;
        case 20:
            if((VolRestrict * CurRestrict) > ((MAX_POWER_20V) + ExtraPower))
            {  
                CurRestrict = ((MAX_POWER_20V) + ExtraPower) / VolRestrict;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch ((uint8_t)InPutVoltage)
        {
        case 5:
            break;
        case 9:
            if((VolRestrict * CurRestrict) > MAX_POWER_9V)
            {  
                VolRestrict = MAX_POWER_9V / CurRestrict;
            }
            break;
        case 12:
            if((VolRestrict * CurRestrict) > MAX_POWER_12V)
            {  
                VolRestrict = MAX_POWER_12V / CurRestrict;
            }
            break;
        case 15:
            if((VolRestrict * CurRestrict) > MAX_POWER_15V)
            {  
                VolRestrict = MAX_POWER_15V / CurRestrict;
            }
            break;
        case 20:
            if((VolRestrict * CurRestrict) > ((MAX_POWER_20V) + ExtraPower))
            {  
                VolRestrict = ((MAX_POWER_20V) + ExtraPower) / CurRestrict;
            }
            break;
        default:
            break;
        }
    }
}


void Check_TimeOut(void)
{
    if(AttonityTimeCountFlag == 1)
    {
        if(AttonityTimeCount >= 1500)
        {
            TimeCountFlag = TIMER_ATTONITY_15S;
            KeyInputFSM_SetEvent(EVENT_TIMEOUT);
        }
    }
    else
    {
        AttonityTimeCount = 0;
    }

    if(ExtinguishTimeCountFlag == 1)
    {
        if(ExtinguishTimeCount >= 1080000 && StatusID == STATUS_RUNNING)
        {
            TimeCountFlag = TIMER_EXTINGUISH_10800S;
            KeyInputFSM_SetEvent(EVENT_TIMEOUT);
        }
    }
    else
    {
        ExtinguishTimeCount = 0;
    }
}


void UpData_VolRestrictAndCurRestrict(uint16_t VolTmp, uint16_t CurTmp)
{
    VolRestrict = VolTmp;
    CurRestrict = CurTmp;
}


void Save_VolRestrict_ToRom(uint16_t VolTmp)
{
    I2C_EE_BufferWrite((void*)&VolTmp, EESaveAddr, sizeof(VolTmp));
}


void Save_CurRestrict_ToRom(uint16_t CurTmp)
{
    I2C_EE_BufferWrite((void*)&CurTmp, EESaveAddr + 2, sizeof(CurTmp));
}


uint16_t Read_VolRestrict_FromRom(void)
{
    uint16_t VolTmp;

    I2C_EE_BufferRead((void*)&VolTmp, EESaveAddr, sizeof(VolRestrict));

    return VolTmp;
}


uint16_t Read_CurRestrict_FromRom(void)
{
    uint16_t CurTmp;

    I2C_EE_BufferRead((void*)&CurTmp, EESaveAddr + 2, sizeof(CurRestrict));

    return CurTmp;
}


void ShowRestrict(void)
{
    OLED_ShowString(3, 10, "       ");
    OLED_ShowString(4, 10, "       ");

    OLED_ShowNum(3, 10, VolRestrict / 100, 2);
    OLED_ShowChar(3, 12, '.');

    OLED_ShowNum(3, 13, VolRestrict % 100, 2);

    OLED_ShowNum(4, 11, CurRestrict / 100, 1);
    OLED_ShowChar(4, 12, '.');
    OLED_ShowNum(4, 13, CurRestrict % 100, 2);
}


void ShowVolRestrict(void)
{
    OLED_ShowString(3, 10, "       ");

    OLED_ShowNum(3, 10, VolRestrict / 100, 2);
    OLED_ShowChar(3, 12, '.');
    OLED_ShowNum(3, 13, VolRestrict % 100, 2);
}


void ShowCurRestrict(void)
{
    OLED_ShowString(4, 10, "       ");

    OLED_ShowNum(4, 11, CurRestrict / 100, 1);
    OLED_ShowChar(4, 12, '.');
    OLED_ShowNum(4, 13, CurRestrict % 100, 2);
}


void ShowRestrict_Vol(void)
{
    if(VolCursor == 10)
    {
        OLED_ShowNum_Bright(3, VolCursor, VolRestrict / 1000);
    }
    else if(VolCursor == 11)
    {
        OLED_ShowNum_Bright(3, VolCursor, (VolRestrict % 1000) / 100);
    }
    else if(VolCursor == 13)
    {
        OLED_ShowNum_Bright(3, VolCursor, (VolRestrict % 100) / 10);
    }
    else if(VolCursor == 14)
    {
        OLED_ShowNum_Bright(3, VolCursor, VolRestrict % 10);
    }
}


void ShowRestrict_Cur(void)
{
    if(CurCursor == 11)
    {
        OLED_ShowNum_Bright(4, CurCursor, CurRestrict / 100);
    }
    else if(CurCursor == 13)
    {
        OLED_ShowNum_Bright(4, CurCursor, (CurRestrict % 100) / 10);
    }
    else if(CurCursor == 14)
    {
        OLED_ShowNum_Bright(4, CurCursor, CurRestrict % 10);
    }
}

/// @brief 节点函数 空
/// @param  NULL
static void Node_Void(void)
{
	// printf("node 0\r\n");
}


/// @brief 节点函数 准备进入运行状态
/// @param  NULL
static void Node_Ready(void)
{
    ExtinguishTimeCountFlag = 1;
	KeyInputFSM_SetStatus(STATUS_RUNNING);
}


/// @brief 节点函数 进入电压电流调整界面
/// @param  NULL
static void Node_EnterSetting(void)
{
    OLED_ShowChinese(3, 1, 7);
    OLED_ShowChinese(3, 3, 8);
    OLED_ShowChinese(3, 5, 17);
    OLED_ShowChinese(3, 7, 18);
    OLED_ShowChar(3, 9, ':');

    OLED_ShowChinese(4, 1, 5);
    OLED_ShowChinese(4, 3, 6);
    OLED_ShowChinese(4, 5, 16);
    OLED_ShowChinese(4, 7, 18);
    OLED_ShowChar(4, 9, ':');

    ShowRestrict();

    VolCurSetingFlag = 0;  //进入电压设置
    VolCursor = 11;
    ShowRestrict_Vol();
    
    AttonityTimeCountFlag = 1;  //开始计时
    ExtinguishTimeCount = 0;
    KeyInputFSM_SetStatus(STATUS_SETTING);  //状态切换
}


/// @brief 节点函数 退出电压电流调整页面
/// @param  NULL
static void Node_QuitSetting(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowString(4, 1, "                ");

    AttonityTimeCountFlag = 0;  //停止计时
    KeyInputFSM_SetStatus(STATUS_RUNNING);  //状态切换
}


/// @brief 节点函数 向左选择要调整的位
/// @param  NULL
static void Node_LeftMove(void)
{
    if((VolCurSetingFlag == 0) && (VolCursor != 10))
    {
        VolCursor--;
        if(VolCursor == 12)  //跳过小数点
        {
            VolCursor--;
        }

        ShowRestrict();
        ShowRestrict_Vol();
    }
    else if(VolCurSetingFlag && (CurCursor != 11))
    {
        CurCursor--;
        if(CurCursor == 12)
        {
            CurCursor--;
        }

        ShowRestrict();
        ShowRestrict_Cur();
    }

    AttonityTimeCount = 0;  //无动作时间计数
}


/// @brief 节点函数 向右选择要调整的位
/// @param  NULL
static void Node_RightMove(void)
{
    if((VolCurSetingFlag == 0) && (VolCursor != 14))
    {
        VolCursor++;
        if(VolCursor == 12)
        {
            VolCursor++;
        }

        ShowRestrict();
        ShowRestrict_Vol();
    }
    else if(VolCurSetingFlag && (CurCursor != 14))
    {
        CurCursor++;
        if(CurCursor == 12)
        {
            CurCursor++;
        }

        ShowRestrict();
        ShowRestrict_Cur();
    }

    AttonityTimeCount = 0;
}


/// @brief 切换设置电压或者电流
/// @param  NULL
static void Node_Change_Vol_Or_Cur(void)
{
    if(VolCurSetingFlag == 0)  //如果正在设置电压
    {
        CurCursor = 11;
        VolCurSetingFlag = 1;
        ShowRestrict();
        ShowRestrict_Cur();
    }
    else if(VolCurSetingFlag)
    {
        VolCursor = 11;
        VolCurSetingFlag = 0;
        ShowRestrict();
        ShowRestrict_Vol();
    }

    AttonityTimeCount = 0;
}


/// @brief 节点函数 数值减
/// @param  NULL
static void Node_Subtract(void)
{
    uint8_t SlowFlag = 0;

    if(VolCurSetingFlag == 0)  //电压
    {
        if(VolCursor == 10 && VolRestrict >= (MINIMUM_VOL + 1000))
        {
            VolRestrict -= 1000;
            SlowFlag = 1;
        }
        else if(VolCursor == 11 && VolRestrict >= (MINIMUM_VOL + 100))
        {
            VolRestrict -= 100;
            if(VolRestrict > 1500)
            {
                SlowFlag = 1;
            }
        }
        else if(VolCursor == 13 && VolRestrict >= (MINIMUM_VOL + 10))
        {
            VolRestrict -= 10;
        }
        else if(VolCursor == 14 && VolRestrict >= (MINIMUM_VOL + 1))
        {
            VolRestrict -= 1;
        }
        else
        {
            VolRestrict = MINIMUM_VOL;
        }
    }
    else if(VolCurSetingFlag)  //电流
    {
        if(CurCursor == 11 && CurRestrict >= 100)
        {
            CurRestrict -= 100;
        }
        else if(CurCursor == 13 && CurRestrict >= 10)
        {
            CurRestrict -= 10;
        }
        else if(CurCursor == 14 && CurRestrict >= 1)
        {
            CurRestrict -= 1;
        }
        else
        {
            CurRestrict = 0;
        }
    }

    //设定电压电流
    if(SlowFlag && SC8701_CE_Read() == CE_ENABLE)
    {
        SC8701_CE_Control(CE_DESABLE);
        Delay_ms(300);
        SC8701_CE_Control(CE_ENABLE);
        SC8701_Set_Voltage(VolRestrict - 1000);
        SC8701_Set_Current(350);  //电流先设定高一些，防止升压失败
        SC8701_Set_Voltage_Slow(VolRestrict);
        SC8701_Set_Current(CurRestrict);
    }
    else
    {
        SC8701_Set_Voltage(VolRestrict);
    }
    SC8701_Set_Current(CurRestrict);

    //显示
    if(SlowFlag)  //因为显示进度条被擦除了
    {
        OLED_ShowChinese(3, 1, 7);
        OLED_ShowChinese(3, 3, 8);
        OLED_ShowChinese(3, 5, 17);
        OLED_ShowChinese(3, 7, 18);
        OLED_ShowChar(3, 9, ':');

        OLED_ShowChinese(4, 1, 5);
        OLED_ShowChinese(4, 3, 6);
        OLED_ShowChinese(4, 5, 16);
        OLED_ShowChinese(4, 7, 18);
        OLED_ShowChar(4, 9, ':');
    }
    ShowRestrict();
    if(VolCurSetingFlag == 0)  //电压
    {
        //显示阳码
        ShowRestrict_Vol();
    }
    else if(VolCurSetingFlag)  //电流
    {
        //显示阳码
        ShowRestrict_Cur();
    }

    //保存电压电流设定
    Save_VolRestrict_ToRom(VolRestrict);
    Save_CurRestrict_ToRom(CurRestrict);

    Delay_ms(10);  //不延时可能因为EEPROM没有写完，会导致卡死
    AttonityTimeCount = 0;
}


/// @brief 节点函数 数值加
/// @param  NULL
static void Node_Add(void)
{
    uint8_t SlowFlag = 0;

    if(VolCurSetingFlag == 0)  //电压
    {
        if(VolCursor == 10 && VolRestrict <= (1500 + ExtraVol))
        {
            if(SC8701_CE_Read() == CE_ENABLE)
            {
                SlowFlag = 1;
            }
            VolRestrict += 1000;
        }
        else if(VolCursor == 11 && VolRestrict <= (2400 + ExtraVol))
        {
            VolRestrict += 100;
            if(InPutVoltage == 5 && (VolRestrict - 100) <= 1000)  //补丁：修复5V输入状态下无法通过滚轮升压超过10V
            {
                SC8701_CE_Control(CE_DESABLE);
                Delay_ms(300);
                SC8701_CE_Control(CE_ENABLE);
                SlowFlag = 1;
            }
        }
        else if(VolCursor == 13 && VolRestrict <= (2490 + ExtraVol))
        {
            VolRestrict += 10;
            if(InPutVoltage == 5 && (VolRestrict - 10) <= 1000)  //补丁：修复5V输入状态下无法通过滚轮升压超过10V
            {
                SC8701_CE_Control(CE_DESABLE);
                Delay_ms(300);
                SC8701_CE_Control(CE_ENABLE);
                SlowFlag = 1;
            }
        }
        else if(VolCursor == 14 && VolRestrict <= (2499 + ExtraVol))
        {
            VolRestrict += 1;
            if(InPutVoltage == 5 && (VolRestrict - 1) <= 1000)  //补丁：修复5V输入状态下无法通过滚轮升压超过10V
            {
                SC8701_CE_Control(CE_DESABLE);
                Delay_ms(300);
                SC8701_CE_Control(CE_ENABLE);
                SlowFlag = 1;
            }
        }
        // else
        // {
        //     if(SC8701_CE_Read() == CE_ENABLE)
        //     {
        //         SlowFlag = 1;
        //     }

        //     if(ExtraVol != 0)
        //     {
        //         VolRestrict = 2500 + ExtraVol;
        //     }
        //     else
        //     {
        //         VolRestrict = 2500;
        //     }
        // }
    }
    else if(VolCurSetingFlag)  //电流
    {
        if(CurCursor == 11 && CurRestrict <= 500)
        {
            CurRestrict += 100;
        }
        else if(CurCursor == 13 && CurRestrict <= 590)
        {
            CurRestrict += 10;
        }
        else if(CurCursor == 14 && CurRestrict <= 599)
        {
            CurRestrict += 1;
        }
        else
        {
            CurRestrict = 600;
        }
    }

    //检查功率
    Check_Power();

    //设定电压电流
    if(SlowFlag)
    {
        SC8701_Set_Voltage(VolRestrict - 1000);
        SC8701_Set_Current(350);  //电流先设定高一些，防止升压失败
        SC8701_Set_Voltage_Slow(VolRestrict);
        SC8701_Set_Current(CurRestrict);
    }
    else
    {
        SC8701_Set_Voltage(VolRestrict);
    }
    SC8701_Set_Current(CurRestrict);

    //显示
    if(SlowFlag)  //因为显示进度条被擦除了
    {
        OLED_ShowChinese(3, 1, 7);
        OLED_ShowChinese(3, 3, 8);
        OLED_ShowChinese(3, 5, 17);
        OLED_ShowChinese(3, 7, 18);
        OLED_ShowChar(3, 9, ':');

        OLED_ShowChinese(4, 1, 5);
        OLED_ShowChinese(4, 3, 6);
        OLED_ShowChinese(4, 5, 16);
        OLED_ShowChinese(4, 7, 18);
        OLED_ShowChar(4, 9, ':');
    }
    ShowRestrict();  //显示数据
    if(VolCurSetingFlag == 0)  //电压
    {
        //显示阳码
        ShowRestrict_Vol();
    }
    else if(VolCurSetingFlag)  //电流
    {
        //显示阳码
        ShowRestrict_Cur();
    }

    //保存电压电流设定
    Save_VolRestrict_ToRom(VolRestrict);
    Save_CurRestrict_ToRom(CurRestrict);

    AttonityTimeCount = 0;
    Delay_ms(10);  //不延时可能因为EEPROM没有写完，会导致卡死
}


/// @brief 节点函数 调整电压电流界面的超时处理
/// @param  NULL
static void Node_SettingTimeOut(void)
{
    switch (TimeCountFlag)
    {
    case TIMER_ATTONITY_15S:
        AttonityTimeCountFlag = 0;
        AttonityTimeCount = 0;
        TimeCountFlag = NO_TIMER;
        Node_QuitSetting();
        break;
    default:
        break;
    }
}


/// @brief 节点函数 开启或者关闭输出
/// @param  NULL
static void Node_ON_OFF_OutPut(void)
{
    if(GPIO_ReadOutputDataBit(SC8701_EN_PORT, SC8701_EN) == CE_ENABLE)  //如果已开启
    {
        SC8701_CE_Control(CE_DESABLE);
        OLED_ShowChinese(4, 7, 20);
        OLED_ShowChinese(4, 9, 21);
        Delay_ms(200);
        OLED_ShowString (4, 7, "    ");
    }
    else
    {
        OLED_ShowChinese(4, 7, 19);
        OLED_ShowChinese(4, 9, 13);
        Delay_ms(200);
        OLED_ShowString (4, 7, "    ");
        SC8701_CE_Control(CE_ENABLE);

        //电压设定超过9V，有可能启动困难，对于5V的输入电压，则启动开始电压不应超过5V
        if(VolRestrict > 9)
        {
            SC8701_Set_Voltage(500);
            SC8701_Set_Current(350);  //电流先设定高一些，防止启动失败
            SC8701_Set_Voltage_Slow(VolRestrict);
            SC8701_Set_Current(CurRestrict);
        }
    }

    ExtinguishTimeCount = 0;
}


/// @brief 节点函数 进入预设模式
/// @param  NULL
static void Node_EnterPredefined(void)
{
    ShowPredefined();
    OLED_ShowString(4, 1, "<              >");
    OLED_ShowChinese(4, 7, 25);
    OLED_ShowChinese(4, 9, 26);

    AttonityTimeCountFlag = 1;  //开始计时
    ExtinguishTimeCount = 0;
    KeyInputFSM_SetStatus(STATUS_PREDEFINED);  //状态切换
}


/// @brief 节点函数 退出预设模式并设定
/// @param  NULL
static void Node_QuitPredefined(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowString(4, 1, "                ");

    VolRestrict = Predefined_OutPut_Array[PredefinedCount].PreVol;
    CurRestrict = Predefined_OutPut_Array[PredefinedCount].PreCur;
    
    SC8701_CE_Control(CE_DESABLE);  //一定要将电压泄放出去，打坏3个充电器了，tnnd。建议用绿联充电器有保护
    Delay_ms(300);
    SC8701_CE_Control(CE_ENABLE);

    //设定电压电流
    SC8701_CE_Control(CE_ENABLE);
    if(VolRestrict > 9)
    {
        SC8701_Set_Voltage(500);
        SC8701_Set_Current(350);  //电流先设定高一些，防止启动失败
        SC8701_Set_Voltage_Slow(VolRestrict);
        SC8701_Set_Current(CurRestrict);
    }
    else
    {
        SC8701_Set_Voltage(VolRestrict);
    }
    SC8701_Set_Current(CurRestrict);

    //保存电压电流设定
    Save_VolRestrict_ToRom(VolRestrict);
    Save_CurRestrict_ToRom(CurRestrict);
    Delay_ms(10);  //不延时可能因为EEPROM没有写完，会导致卡死

    AttonityTimeCountFlag = 0;  //停止计时
    KeyInputFSM_SetStatus(STATUS_RUNNING);  //状态切换
}


/// @brief 节点函数 退出预设模式
/// @param  NULL
static void Node_QuitPredefined_NoSet(void)
{
    AttonityTimeCountFlag = 0;
    OLED_ShowString(3, 1, "                ");
    OLED_ShowString(4, 1, "                ");
    KeyInputFSM_SetStatus(STATUS_RUNNING);  //状态切换
}


/// @brief 节点函数 向左选择其他预设
/// @param  NULL
static void Node_Predefined_Subtract(void)
{
    if(PredefinedCount == 0)
    {
        PredefinedCount = PREDEFINED_NUM - 1;
    }
    else
    {
        PredefinedCount--;
    }

    ShowPredefined();
    AttonityTimeCount = 0;
}


/// @brief 节点函数 向右旋转其他预设
/// @param  NULL
static void Node_Predefined_Add(void)
{
    if(PredefinedCount == PREDEFINED_NUM - 1)
    {
        PredefinedCount = 0;
    }
    else
    {
        PredefinedCount++;
    }

    ShowPredefined();
    AttonityTimeCount = 0;
}


/// @brief 节点函数 预设模式下的超时处理
/// @param  NULL
static void Node_PredefinedTimeOut(void)
{
    switch (TimeCountFlag)
    {
    case TIMER_ATTONITY_15S:
        TimeCountFlag = NO_TIMER;
        AttonityTimeCountFlag = 0;
        AttonityTimeCount = 0;
        OLED_ShowString(3, 1, "                ");
        OLED_ShowString(4, 1, "                ");
        KeyInputFSM_SetStatus(STATUS_RUNNING);  //状态切换
        break;
    default:
        break;
    }
}


/// @brief 节点函数 进入设置（菜单）模式
/// @param  NULL
static void Node_EnterMenu(void)
{
    OLED_ShowChinese(3, 7, 17);
    OLED_ShowChinese(3, 9, 31);
    Delay_ms(500);

    MenuShowOption[EnumFlag]();
    OLED_ShowString(4, 1, "                ");
    OLED_ShowString(4, 5, (char*)VersionNumber);

    AttonityTimeCountFlag = 1;  //开始计时
    ExtinguishTimeCount = 0;
    KeyInputFSM_SetStatus(STATUS_MENU);  //状态切换
}


/// @brief 节点函数 退出设置（菜单）模式
/// @param  NULL
static void Node_QuitMenu(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowString(4, 1, "                ");

    EnumFlag = ENUM_VERSIONS;
    AttonityTimeCountFlag = 0;  //停止计时
    KeyInputFSM_SetStatus(STATUS_RUNNING);  //状态切换
}


/// @brief 节点函数 向左切换设置项
/// @param  NULL
static void Node_MenuOption_Left(void)
{
    if(EnumFlag > 0)
    {
        EnumFlag--;
    }
    else
    {
        EnumFlag = ENUM_POWER;
    }

    MenuShowOption[EnumFlag]();
    switch (EnumFlag)
    {
    case ENUM_VERSIONS:
        OLED_ShowString(4, 1, "                ");
        OLED_ShowString(4, 5, (char*)VersionNumber);
        break;
    case ENUM_WAIT:
        Menu_Set_Wait(ONLY_SHOW);
        break;
    case ENUM_VOL:
        Menu_Set_Vol(ONLY_SHOW);
        break;
    case ENUM_POWER:
        Menu_Set_Power(ONLY_SHOW);
        break;
    default:
        break;
    }

    AttonityTimeCount = 0;
}


/// @brief 节点函数 向右选择设置项
/// @param  NULL
static void Node_MenuOption_Right(void)
{
    if(EnumFlag < ENUM_POWER)
    {
        EnumFlag++;
    }
    else
    {
        EnumFlag = ENUM_VERSIONS;
    }

    MenuShowOption[EnumFlag]();
    switch (EnumFlag)
    {
    case ENUM_VERSIONS:
        OLED_ShowString(4, 1, "                ");
        OLED_ShowString(4, 5, (char*)VersionNumber);
        break;
    case ENUM_WAIT:
        Menu_Set_Wait(ONLY_SHOW);
        break;
    case ENUM_VOL:
        Menu_Set_Vol(ONLY_SHOW);
        break;
    case ENUM_POWER:
        Menu_Set_Power(ONLY_SHOW);
        break;
    default:
        break;
    }

    AttonityTimeCount = 0;
}


/// @brief 节点函数 设置（菜单）模式下的超时处理
/// @param  NULL
static void Node_MenuTimeOut(void)
{
    switch (TimeCountFlag)
    {
    case TIMER_ATTONITY_15S:
        TimeCountFlag = NO_TIMER;
        AttonityTimeCountFlag = 0;
        AttonityTimeCount = 0;
        OLED_ShowString(3, 1, "                ");
        OLED_ShowString(4, 1, "                ");
        KeyInputFSM_SetStatus(STATUS_RUNNING);  //状态切换
        break;
    default:
        break;
    }
}


/// @brief 节点函数 向左切换设置项的参数
/// @param  NULL
static void Node_MenuOption_Subtract(void)
{
    switch (EnumFlag)
    {
    case ENUM_VERSIONS:
        OLED_ShowString(4, 1, "                ");
        OLED_ShowString(4, 5, (char*)VersionNumber);
        break;
    case ENUM_WAIT:
        Menu_Set_Wait(LEFT);
        break;
    case ENUM_VOL:
        Menu_Set_Vol(LEFT);
        break;
    case ENUM_POWER:
        Menu_Set_Power(LEFT);
        break;
    default:
        break;
    }

    AttonityTimeCount = 0;
}


/// @brief 节点函数 向右切换设置项的参数
/// @param  NULL
static void Node_MenuOption_Add(void)
{
    switch (EnumFlag)
    {
    case ENUM_VERSIONS:
        OLED_ShowString(4, 1, "                ");
        OLED_ShowString(4, 5, (char*)VersionNumber);
        break;
    case ENUM_WAIT:
        Menu_Set_Wait(RIGHT);
        break;
    case ENUM_VOL:
        Menu_Set_Vol(RIGHT);
        break;
    case ENUM_POWER:
        Menu_Set_Power(RIGHT);
        break;
    default:
        break;
    }

    AttonityTimeCount = 0;
}


/// @brief 节点函数 进入关机状态
/// @param  NULL
void Node_EnterShutDowm_Mode(void)
{
    PDPowerSupplyState = POWER_SUPPLY_SHUTDOWN;

    OLED_Clear();
    OLED_ShowBMP(1, 32, 1);
    Delay_ms(500);

    OLED_Clear();
    OLED_Clock_Control(DISABLE);
    SC8701_CE_Control(CE_DESABLE);

    ExtinguishTimeCountFlag = 0;
    ExtinguishTimeCount = 0;

    KeyInputFSM_SetStatus(STATUS_SHUTDOWN);  //状态切换
}


/// @brief 节点函数 退出关机状态
/// @param  NULL
void Node_QuitShutDowm_Mode(void)
{
    PDPowerSupplyState = POWER_SUPPLY_RUNNING;

    OLED_Clock_Control(ENABLE);
    OLED_ShowBMP(1, 32, 1);
    Delay_ms(500);
    OLED_Clear();
    
    OLED_ShowString(1, 1, "I:");
	OLED_ShowString(1, 9, "P:");
	OLED_ShowString(2, 1, "U:");
    
    ExtinguishTimeCountFlag = 1;

    KeyInputFSM_SetStatus(STATUS_RUNNING);  //状态切换
}


/// @brief 节点函数 运行模式下的超时处理
/// @param  NULL
static void Node_RunningTimeOut(void)
{
    switch (TimeCountFlag)
    {
    case TIMER_EXTINGUISH_10800S:
        TimeCountFlag = NO_TIMER;
        ExtinguishTimeCountFlag = 0;
        ExtinguishTimeCount = 0;

        PDPowerSupplyState = POWER_SUPPLY_EXTINGUISH;
        OLED_Clear();
        OLED_Clock_Control(DISABLE);

        KeyInputFSM_SetStatus(STATUS_EXTINGUISH);  //状态切换
        break;
    default:
        break;
    }
}


/// @brief 节点函数 退出熄屏状态
/// @param  NULL
void Node_QuitExtinguish_Mode(void)
{
    PDPowerSupplyState = POWER_SUPPLY_RUNNING;

    OLED_Clock_Control(ENABLE);
    OLED_ShowString(1, 1, "I:");
	OLED_ShowString(1, 9, "P:");
	OLED_ShowString(2, 1, "U:");

    ExtinguishTimeCountFlag = 1;

    KeyInputFSM_SetStatus(STATUS_RUNNING);  //状态切换
}


/// @brief 按键输入状态机
static uint8_t KeyArray[EVENT_NUM][STATUS_NUM] = 
{
    /*未准备好       运行中          进入设置         预设模式        菜单模式         关机模式         熄屏模式*/
    {1,             0,              0,              0,              0 ,             0 ,             0 },  /* 无事件发生 */
    {0,             0,              4,              13,             18,             0 ,             26},  /* KEY_1 触发 */
    {0,             2,              6,              12,             0 ,             0 ,             26},  /* KEY_2 触发 */
    {0,             10,             5,              14,             19,             0 ,             26},  /* KEY_3 触发 */
    {0,             2,              3,              12,             0 ,             0 ,             26},  /* KEY_4 触发 */
    {0,             0,              7,              13,             21,             0 ,             26},  /* 编码器左旋 */
    {0,             0,              8,              14,             22,             0 ,             26},  /* 编码器右旋 */
    {0,             11,             0,              0,              0 ,             0 ,             26},  /* KEY_1 双击 */
    {0,             16,             0,              0,              0 ,             0 ,             26},  /* KEY_2 双击 */
    {0,             0,              0,              0,              0 ,             0 ,             26},  /* KEY_3 双击 */
    {0,             23,             3,              27,             17,             24,             26},  /* KEY_1 长按 */
    {0,             0,              3,              27,             17,             0 ,             26},  /* KEY_2 长按 */
    {0,             0,              3,              27,             17,             0 ,             26},  /* KEY_3 长按 */
    {0,             25,             9,              15,             20,             0 ,             0 },  /* TimeOut */
};


/// @brief 节点函数列表
static void (*NodeFnArr[])(void) =
{
    /* 0 */   Node_Void,
    /* 1 */   Node_Ready,
    /* 2 */   Node_EnterSetting,
    /* 3 */   Node_QuitSetting,
    /* 4 */   Node_LeftMove,
    /* 5 */   Node_RightMove,
    /* 6 */   Node_Change_Vol_Or_Cur,
    /* 7 */   Node_Subtract,
    /* 8 */   Node_Add,
    /* 9 */   Node_SettingTimeOut,
    /* 10 */  Node_ON_OFF_OutPut,
    /* 11 */  Node_EnterPredefined,
    /* 12 */  Node_QuitPredefined,
    /* 13 */  Node_Predefined_Subtract,
    /* 14 */  Node_Predefined_Add,
    /* 15 */  Node_PredefinedTimeOut,
    /* 16 */  Node_EnterMenu,
    /* 17 */  Node_QuitMenu,
    /* 18 */  Node_MenuOption_Left,
    /* 19 */  Node_MenuOption_Right,
    /* 20 */  Node_MenuTimeOut,
    /* 21 */  Node_MenuOption_Subtract,
    /* 22 */  Node_MenuOption_Add,
    /* 23 */  Node_EnterShutDowm_Mode,
    /* 24 */  Node_QuitShutDowm_Mode,
    /* 25 */  Node_RunningTimeOut,
    /* 26 */  Node_QuitExtinguish_Mode,
    /* 27 */  Node_QuitPredefined_NoSet,
};


/// @brief 设置事件
/// @param event 事件ID
void KeyInputFSM_SetEvent(KEY_EVENT_ID event)
{
    if(event < EVENT_NUM)
    {
        EventID = event;
    }
}


/// @brief 设置状态
/// @param status 状态ID
void KeyInputFSM_SetStatus(KEY_STATUS status)
{
    if(status < STATUS_NUM)
    {
        StatusID = status;
    }
}


/// @brief 运行状态机
/// @param  NULL
void KeyInputFSM_Run(void)
{
    uint8_t NodeNum;

    if(EventID == EVENT_VOID)  //按键优先
    {
        Check_TimeOut();
    }
    
    NodeNum = KeyArray[EventID][StatusID];

    if(NodeNum != 0)
    {
        KeyInputFSM_SetEvent(EVENT_VOID);
    }

    NodeFnArr[NodeNum]();
}


/// @brief 获取预设电压值
/// @param  NULL
/// @return 预设电压值
uint16_t Get_VolRestrict(void)
{
    return VolRestrict;
}


/// @brief 获取预设电流值
/// @param  NULL
/// @return 预设电流值
uint16_t Get_CurRestrict(void)
{
    return CurRestrict;
}


/// @brief 设置电压的上限为25V或者33V
/// @param parameter 参数：LIMIT_LOW 或者 LIMIT_GIGH
void Set_Vol_UpperLimit(uint8_t parameter)
{
    if(parameter == LIMIT_HIGH)
    {
        ExtraVol = 800;
    }
    else if(parameter == LIMIT_LOW)
    {
        ExtraVol = 0;

        if(VolRestrict > 2500)  //降低电压
        {
            VolRestrict = 2500;
            SC8701_Set_Voltage(VolRestrict);
            Save_VolRestrict_ToRom(VolRestrict);
            Delay_ms(10);  //不延时可能因为EEPROM没有写完，会导致卡死
        }
    }
}


/// @brief 设置功率的上限为65W或者100W
/// @param parameter 参数：LIMIT_LOW 或者 LIMIT_GIGH
void Set_Power_UpperLimit(uint8_t parameter)
{
    if(parameter == LIMIT_HIGH)
    {
        ExtraPower = 350000;
    }
    else if(parameter == LIMIT_LOW)
    {
        ExtraPower = 0;
    }
}


POWER_SUPPLY_STATE_ID Get_PDPowerSupplyState(void)
{
    return PDPowerSupplyState;
}
