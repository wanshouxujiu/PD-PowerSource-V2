#ifndef __KEYINPUTFSM_H__
#define __KEYINPUTFSM_H__

#include "stm32f10x.h"

#define  DATA_SIZE  2
#define  SETTING_FIELD_ADDR    2  //设置参数保存位置，2byte
#define  LIMIT_LOW             0
#define  LIMIT_HIGH            1

extern uint8_t AttonityTimeCountFlag;
extern uint16_t AttonityTimeCount;
extern uint8_t ExtinguishTimeCountFlag;
extern uint32_t ExtinguishTimeCount;
extern uint8_t EESaveAddr;

typedef enum
{
    EVENT_VOID,
	EVENT_KEY1,
    EVENT_KEY2,
    EVENT_KEY3,
    EVENT_KEY4,
    EVENT_TURN_LEFT,
    EVENT_TURN_RIGHT,
    EVENT_KEY1_DOUBLE,
    EVENT_KEY2_DOUBLE,
    EVENT_KEY3_DOUBLE,
    EVENT_KEY1_LONG,
    EVENT_KEY2_LONG,
    EVENT_KEY3_LONG,
    EVENT_TIMEOUT,
    EVENT_NUM,
} KEY_EVENT_ID;

typedef enum
{
	STATUS_NO_READY,
    STATUS_RUNNING,
    STATUS_SETTING,
    STATUS_PREDEFINED,
    STATUS_MENU,
    STATUS_SHUTDOWN,
    STATUS_EXTINGUISH,
    STATUS_NUM,
} KEY_STATUS;


typedef enum
{
    NO_TIMER,
    TIMER_ATTONITY_15S,
    TIMER_EXTINGUISH_10800S,
    TIMER_NUM,
} TIMER_ID;


typedef enum
{
    ENUM_VERSIONS,
    ENUM_WAIT,
    ENUM_VOL,
    ENUM_POWER,
    ENUM_OPTION_NUM,
} ENUM_OPTION_ID;


typedef enum
{
    POWER_SUPPLY_RUNNING,
    POWER_SUPPLY_SHUTDOWN,
    POWER_SUPPLY_EXTINGUISH,
} POWER_SUPPLY_STATE_ID;


void KeyInputFSM_SetEvent(KEY_EVENT_ID Num);
void KeyInputFSM_SetStatus(KEY_STATUS status);
void KeyInputFSM_Run(void);
void Save_VolRestrict_ToRom(uint16_t VolTmp);
void Save_CurRestrict_ToRom(uint16_t CurTmp);
uint16_t Read_VolRestrict_FromRom(void);
uint16_t Read_CurRestrict_FromRom(void);
void UpData_VolRestrictAndCurRestrict(uint16_t VolTmp, uint16_t CurTmp);
uint16_t SetMax_Current(uint16_t VolTmp, uint16_t CurTmp);
uint16_t Get_VolRestrict(void);
uint16_t Get_CurRestrict(void);
uint16_t Read_SettingField(void);
void Write_SettingField(uint16_t Field);
void Set_Vol_UpperLimit(uint8_t parameter);
void Set_Power_UpperLimit(uint8_t parameter);
POWER_SUPPLY_STATE_ID Get_PDPowerSupplyState(void);


#endif
