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

static const char VersionNumber[] = "2.2.1.3";  //�汾�ţ�ǰ��λΪӲ������λΪ���

static KEY_EVENT_ID EventID = EVENT_VOID;
static KEY_STATUS StatusID = STATUS_NO_READY;
static uint16_t VolRestrict;                      //Ԥ���ѹ
static uint16_t CurRestrict;                      //Ԥ�����
static uint8_t VolCurSetingFlag;                  //��ѹ���������б�־ 0Ϊ��ѹ 1Ϊ����
static uint8_t VolCursor;                         //���λ��
static uint8_t CurCursor;                         //���λ��
static uint8_t PredefinedCount = 2;               //ʹ�õ�Ԥ����ı�ʶ
static TIMER_ID TimeCountFlag = NO_TIMER;
static ENUM_OPTION_ID EnumFlag = ENUM_VERSIONS;
static uint16_t ExtraVol;                          //��չ��ѹ����
static uint32_t ExtraPower;                        //��չ���ʲ���
static POWER_SUPPLY_STATE_ID PDPowerSupplyState = POWER_SUPPLY_RUNNING;   //������Դ�Ĺ���״̬

uint8_t AttonityTimeCountFlag;      //���ʱ��־
uint16_t AttonityTimeCount;
uint8_t ExtinguishTimeCountFlag;    //Ϩ����ʱ��־
uint32_t ExtinguishTimeCount;
uint8_t EESaveAddr;                 //��ѹ���������� EEPROM �д洢��λ��


static void MenuShow_Versions(void);
static void MenuShow_Wait(void);
static void MenuShow_Vol(void);
static void MenuShow_Power(void);


struct Predefined_OutPut_S
{
    uint16_t PreVol;
    uint16_t PreCur;
};

//Ԥ�ȶ�������������
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


/// @brief ��ȡ���ò����ֽ�
/// @param  NULL
/// @return ��������ò���
uint16_t Read_SettingField(void)
{
    uint16_t Tmp;

    I2C_EE_BufferRead((void*)&Tmp, SETTING_FIELD_ADDR, 2);
    // printf("seting: %x\r\n", Tmp);

    return Tmp;
}


/// @brief �������ò���
/// @param Field ���ò���
void Write_SettingField(uint16_t Field)
{
    I2C_EE_BufferWrite((void*)&Field, SETTING_FIELD_ADDR, 2);
    Delay_ms(10);  //����ʱ������ΪEEPROMû��д�꣬�ᵼ�¿���
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

        OLED_ShowChinese(4, 15, 19);  //��
    }
    else if(direction == RIGHT)
    {
        FieldTmp |= 0x8000;

        OLED_ShowChinese(4, 15, 20);  //��
    }
    else if(direction == ONLY_SHOW)
    {
        switch (FieldTmp & 0x8000)
        {
        case 0x0000:
            OLED_ShowChinese(4, 15, 19);  //��
            break;
        case 0x8000:
            OLED_ShowChinese(4, 15, 20);  //��
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


/// @brief ��ʾѡ��汾��
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


/// @brief ��ʾѡ��ϵ�ȴ�
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


/// @brief ��ʾѡ���ѹ����
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


/// @brief ��ʾѡ����������
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


/// @brief �����������͸����ĵ�ѹ������������������*��ѹ<����ʣ��򷵻ظ��ĵ���
/// @param VolTmp �����ĵ�ѹ
/// @param CurTmp �����ĵ���
/// @return Ӧ���趨�ĵ���
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

/// @brief �ڵ㺯�� ��
/// @param  NULL
static void Node_Void(void)
{
	// printf("node 0\r\n");
}


/// @brief �ڵ㺯�� ׼����������״̬
/// @param  NULL
static void Node_Ready(void)
{
    ExtinguishTimeCountFlag = 1;
	KeyInputFSM_SetStatus(STATUS_RUNNING);
}


/// @brief �ڵ㺯�� �����ѹ������������
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

    VolCurSetingFlag = 0;  //�����ѹ����
    VolCursor = 11;
    ShowRestrict_Vol();
    
    AttonityTimeCountFlag = 1;  //��ʼ��ʱ
    ExtinguishTimeCount = 0;
    KeyInputFSM_SetStatus(STATUS_SETTING);  //״̬�л�
}


/// @brief �ڵ㺯�� �˳���ѹ��������ҳ��
/// @param  NULL
static void Node_QuitSetting(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowString(4, 1, "                ");

    AttonityTimeCountFlag = 0;  //ֹͣ��ʱ
    KeyInputFSM_SetStatus(STATUS_RUNNING);  //״̬�л�
}


/// @brief �ڵ㺯�� ����ѡ��Ҫ������λ
/// @param  NULL
static void Node_LeftMove(void)
{
    if((VolCurSetingFlag == 0) && (VolCursor != 10))
    {
        VolCursor--;
        if(VolCursor == 12)  //����С����
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

    AttonityTimeCount = 0;  //�޶���ʱ�����
}


/// @brief �ڵ㺯�� ����ѡ��Ҫ������λ
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


/// @brief �л����õ�ѹ���ߵ���
/// @param  NULL
static void Node_Change_Vol_Or_Cur(void)
{
    if(VolCurSetingFlag == 0)  //����������õ�ѹ
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


/// @brief �ڵ㺯�� ��ֵ��
/// @param  NULL
static void Node_Subtract(void)
{
    uint8_t SlowFlag = 0;

    if(VolCurSetingFlag == 0)  //��ѹ
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
    else if(VolCurSetingFlag)  //����
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

    //�趨��ѹ����
    if(SlowFlag && SC8701_CE_Read() == CE_ENABLE)
    {
        SC8701_CE_Control(CE_DESABLE);
        Delay_ms(300);
        SC8701_CE_Control(CE_ENABLE);
        SC8701_Set_Voltage(VolRestrict - 1000);
        SC8701_Set_Current(350);  //�������趨��һЩ����ֹ��ѹʧ��
        SC8701_Set_Voltage_Slow(VolRestrict);
        SC8701_Set_Current(CurRestrict);
    }
    else
    {
        SC8701_Set_Voltage(VolRestrict);
    }
    SC8701_Set_Current(CurRestrict);

    //��ʾ
    if(SlowFlag)  //��Ϊ��ʾ��������������
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
    if(VolCurSetingFlag == 0)  //��ѹ
    {
        //��ʾ����
        ShowRestrict_Vol();
    }
    else if(VolCurSetingFlag)  //����
    {
        //��ʾ����
        ShowRestrict_Cur();
    }

    //�����ѹ�����趨
    Save_VolRestrict_ToRom(VolRestrict);
    Save_CurRestrict_ToRom(CurRestrict);

    Delay_ms(10);  //����ʱ������ΪEEPROMû��д�꣬�ᵼ�¿���
    AttonityTimeCount = 0;
}


/// @brief �ڵ㺯�� ��ֵ��
/// @param  NULL
static void Node_Add(void)
{
    uint8_t SlowFlag = 0;

    if(VolCurSetingFlag == 0)  //��ѹ
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
            if(InPutVoltage == 5 && (VolRestrict - 100) <= 1000)  //�������޸�5V����״̬���޷�ͨ��������ѹ����10V
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
            if(InPutVoltage == 5 && (VolRestrict - 10) <= 1000)  //�������޸�5V����״̬���޷�ͨ��������ѹ����10V
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
            if(InPutVoltage == 5 && (VolRestrict - 1) <= 1000)  //�������޸�5V����״̬���޷�ͨ��������ѹ����10V
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
    else if(VolCurSetingFlag)  //����
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

    //��鹦��
    Check_Power();

    //�趨��ѹ����
    if(SlowFlag)
    {
        SC8701_Set_Voltage(VolRestrict - 1000);
        SC8701_Set_Current(350);  //�������趨��һЩ����ֹ��ѹʧ��
        SC8701_Set_Voltage_Slow(VolRestrict);
        SC8701_Set_Current(CurRestrict);
    }
    else
    {
        SC8701_Set_Voltage(VolRestrict);
    }
    SC8701_Set_Current(CurRestrict);

    //��ʾ
    if(SlowFlag)  //��Ϊ��ʾ��������������
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
    ShowRestrict();  //��ʾ����
    if(VolCurSetingFlag == 0)  //��ѹ
    {
        //��ʾ����
        ShowRestrict_Vol();
    }
    else if(VolCurSetingFlag)  //����
    {
        //��ʾ����
        ShowRestrict_Cur();
    }

    //�����ѹ�����趨
    Save_VolRestrict_ToRom(VolRestrict);
    Save_CurRestrict_ToRom(CurRestrict);

    AttonityTimeCount = 0;
    Delay_ms(10);  //����ʱ������ΪEEPROMû��д�꣬�ᵼ�¿���
}


/// @brief �ڵ㺯�� ������ѹ��������ĳ�ʱ����
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


/// @brief �ڵ㺯�� �������߹ر����
/// @param  NULL
static void Node_ON_OFF_OutPut(void)
{
    if(GPIO_ReadOutputDataBit(SC8701_EN_PORT, SC8701_EN) == CE_ENABLE)  //����ѿ���
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

        //��ѹ�趨����9V���п����������ѣ�����5V�������ѹ����������ʼ��ѹ��Ӧ����5V
        if(VolRestrict > 9)
        {
            SC8701_Set_Voltage(500);
            SC8701_Set_Current(350);  //�������趨��һЩ����ֹ����ʧ��
            SC8701_Set_Voltage_Slow(VolRestrict);
            SC8701_Set_Current(CurRestrict);
        }
    }

    ExtinguishTimeCount = 0;
}


/// @brief �ڵ㺯�� ����Ԥ��ģʽ
/// @param  NULL
static void Node_EnterPredefined(void)
{
    ShowPredefined();
    OLED_ShowString(4, 1, "<              >");
    OLED_ShowChinese(4, 7, 25);
    OLED_ShowChinese(4, 9, 26);

    AttonityTimeCountFlag = 1;  //��ʼ��ʱ
    ExtinguishTimeCount = 0;
    KeyInputFSM_SetStatus(STATUS_PREDEFINED);  //״̬�л�
}


/// @brief �ڵ㺯�� �˳�Ԥ��ģʽ���趨
/// @param  NULL
static void Node_QuitPredefined(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowString(4, 1, "                ");

    VolRestrict = Predefined_OutPut_Array[PredefinedCount].PreVol;
    CurRestrict = Predefined_OutPut_Array[PredefinedCount].PreCur;
    
    SC8701_CE_Control(CE_DESABLE);  //һ��Ҫ����ѹй�ų�ȥ����3��������ˣ�tnnd������������������б���
    Delay_ms(300);
    SC8701_CE_Control(CE_ENABLE);

    //�趨��ѹ����
    SC8701_CE_Control(CE_ENABLE);
    if(VolRestrict > 9)
    {
        SC8701_Set_Voltage(500);
        SC8701_Set_Current(350);  //�������趨��һЩ����ֹ����ʧ��
        SC8701_Set_Voltage_Slow(VolRestrict);
        SC8701_Set_Current(CurRestrict);
    }
    else
    {
        SC8701_Set_Voltage(VolRestrict);
    }
    SC8701_Set_Current(CurRestrict);

    //�����ѹ�����趨
    Save_VolRestrict_ToRom(VolRestrict);
    Save_CurRestrict_ToRom(CurRestrict);
    Delay_ms(10);  //����ʱ������ΪEEPROMû��д�꣬�ᵼ�¿���

    AttonityTimeCountFlag = 0;  //ֹͣ��ʱ
    KeyInputFSM_SetStatus(STATUS_RUNNING);  //״̬�л�
}


/// @brief �ڵ㺯�� �˳�Ԥ��ģʽ
/// @param  NULL
static void Node_QuitPredefined_NoSet(void)
{
    AttonityTimeCountFlag = 0;
    OLED_ShowString(3, 1, "                ");
    OLED_ShowString(4, 1, "                ");
    KeyInputFSM_SetStatus(STATUS_RUNNING);  //״̬�л�
}


/// @brief �ڵ㺯�� ����ѡ������Ԥ��
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


/// @brief �ڵ㺯�� ������ת����Ԥ��
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


/// @brief �ڵ㺯�� Ԥ��ģʽ�µĳ�ʱ����
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
        KeyInputFSM_SetStatus(STATUS_RUNNING);  //״̬�л�
        break;
    default:
        break;
    }
}


/// @brief �ڵ㺯�� �������ã��˵���ģʽ
/// @param  NULL
static void Node_EnterMenu(void)
{
    OLED_ShowChinese(3, 7, 17);
    OLED_ShowChinese(3, 9, 31);
    Delay_ms(500);

    MenuShowOption[EnumFlag]();
    OLED_ShowString(4, 1, "                ");
    OLED_ShowString(4, 5, (char*)VersionNumber);

    AttonityTimeCountFlag = 1;  //��ʼ��ʱ
    ExtinguishTimeCount = 0;
    KeyInputFSM_SetStatus(STATUS_MENU);  //״̬�л�
}


/// @brief �ڵ㺯�� �˳����ã��˵���ģʽ
/// @param  NULL
static void Node_QuitMenu(void)
{
    OLED_ShowString(3, 1, "                ");
    OLED_ShowString(4, 1, "                ");

    EnumFlag = ENUM_VERSIONS;
    AttonityTimeCountFlag = 0;  //ֹͣ��ʱ
    KeyInputFSM_SetStatus(STATUS_RUNNING);  //״̬�л�
}


/// @brief �ڵ㺯�� �����л�������
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


/// @brief �ڵ㺯�� ����ѡ��������
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


/// @brief �ڵ㺯�� ���ã��˵���ģʽ�µĳ�ʱ����
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
        KeyInputFSM_SetStatus(STATUS_RUNNING);  //״̬�л�
        break;
    default:
        break;
    }
}


/// @brief �ڵ㺯�� �����л�������Ĳ���
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


/// @brief �ڵ㺯�� �����л�������Ĳ���
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


/// @brief �ڵ㺯�� ����ػ�״̬
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

    KeyInputFSM_SetStatus(STATUS_SHUTDOWN);  //״̬�л�
}


/// @brief �ڵ㺯�� �˳��ػ�״̬
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

    KeyInputFSM_SetStatus(STATUS_RUNNING);  //״̬�л�
}


/// @brief �ڵ㺯�� ����ģʽ�µĳ�ʱ����
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

        KeyInputFSM_SetStatus(STATUS_EXTINGUISH);  //״̬�л�
        break;
    default:
        break;
    }
}


/// @brief �ڵ㺯�� �˳�Ϩ��״̬
/// @param  NULL
void Node_QuitExtinguish_Mode(void)
{
    PDPowerSupplyState = POWER_SUPPLY_RUNNING;

    OLED_Clock_Control(ENABLE);
    OLED_ShowString(1, 1, "I:");
	OLED_ShowString(1, 9, "P:");
	OLED_ShowString(2, 1, "U:");

    ExtinguishTimeCountFlag = 1;

    KeyInputFSM_SetStatus(STATUS_RUNNING);  //״̬�л�
}


/// @brief ��������״̬��
static uint8_t KeyArray[EVENT_NUM][STATUS_NUM] = 
{
    /*δ׼����       ������          ��������         Ԥ��ģʽ        �˵�ģʽ         �ػ�ģʽ         Ϩ��ģʽ*/
    {1,             0,              0,              0,              0 ,             0 ,             0 },  /* ���¼����� */
    {0,             0,              4,              13,             18,             0 ,             26},  /* KEY_1 ���� */
    {0,             2,              6,              12,             0 ,             0 ,             26},  /* KEY_2 ���� */
    {0,             10,             5,              14,             19,             0 ,             26},  /* KEY_3 ���� */
    {0,             2,              3,              12,             0 ,             0 ,             26},  /* KEY_4 ���� */
    {0,             0,              7,              13,             21,             0 ,             26},  /* ���������� */
    {0,             0,              8,              14,             22,             0 ,             26},  /* ���������� */
    {0,             11,             0,              0,              0 ,             0 ,             26},  /* KEY_1 ˫�� */
    {0,             16,             0,              0,              0 ,             0 ,             26},  /* KEY_2 ˫�� */
    {0,             0,              0,              0,              0 ,             0 ,             26},  /* KEY_3 ˫�� */
    {0,             23,             3,              27,             17,             24,             26},  /* KEY_1 ���� */
    {0,             0,              3,              27,             17,             0 ,             26},  /* KEY_2 ���� */
    {0,             0,              3,              27,             17,             0 ,             26},  /* KEY_3 ���� */
    {0,             25,             9,              15,             20,             0 ,             0 },  /* TimeOut */
};


/// @brief �ڵ㺯���б�
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


/// @brief �����¼�
/// @param event �¼�ID
void KeyInputFSM_SetEvent(KEY_EVENT_ID event)
{
    if(event < EVENT_NUM)
    {
        EventID = event;
    }
}


/// @brief ����״̬
/// @param status ״̬ID
void KeyInputFSM_SetStatus(KEY_STATUS status)
{
    if(status < STATUS_NUM)
    {
        StatusID = status;
    }
}


/// @brief ����״̬��
/// @param  NULL
void KeyInputFSM_Run(void)
{
    uint8_t NodeNum;

    if(EventID == EVENT_VOID)  //��������
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


/// @brief ��ȡԤ���ѹֵ
/// @param  NULL
/// @return Ԥ���ѹֵ
uint16_t Get_VolRestrict(void)
{
    return VolRestrict;
}


/// @brief ��ȡԤ�����ֵ
/// @param  NULL
/// @return Ԥ�����ֵ
uint16_t Get_CurRestrict(void)
{
    return CurRestrict;
}


/// @brief ���õ�ѹ������Ϊ25V����33V
/// @param parameter ������LIMIT_LOW ���� LIMIT_GIGH
void Set_Vol_UpperLimit(uint8_t parameter)
{
    if(parameter == LIMIT_HIGH)
    {
        ExtraVol = 800;
    }
    else if(parameter == LIMIT_LOW)
    {
        ExtraVol = 0;

        if(VolRestrict > 2500)  //���͵�ѹ
        {
            VolRestrict = 2500;
            SC8701_Set_Voltage(VolRestrict);
            Save_VolRestrict_ToRom(VolRestrict);
            Delay_ms(10);  //����ʱ������ΪEEPROMû��д�꣬�ᵼ�¿���
        }
    }
}


/// @brief ���ù��ʵ�����Ϊ65W����100W
/// @param parameter ������LIMIT_LOW ���� LIMIT_GIGH
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
