#include "stm32f10x.h"
#include "delay.h"


//���뼶����ʱ(����ʱ)
void Delay_ms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12000;       //��Ҫ����ϵͳʱ��Ƶ�ʽ��м���
      while(i--) ;
   }
}


//΢�뼶����ʱ(����ʱ)
void Delay_us(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12;       //��Ҫ����ϵͳʱ��Ƶ�ʽ��м���
      while(i--) ;
   }
}
