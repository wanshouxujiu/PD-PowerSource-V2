#ifndef __TASK_H__
#define __TASK_H__


#define             FilterNum           20   //将多少个数据放入滤波函数

#define             TURE                1
#define             FALSE               0


extern uint8_t ShowTimeCount;
extern uint8_t NormalCount;
extern uint16_t SlowTaskCount;


void Show_Init(void);
void Show_UI(void);
void Show_logo(void);
void Show_State(void);
void Detection_InPut_Voltage(void);
void Detection_Voltage(void);
uint8_t Detection_Current(void);
void Detection_Power(void);
void Short_Circuit_Detection(void);
void KeyInput_Detection(void);
void Check_Setting(void);


#endif
