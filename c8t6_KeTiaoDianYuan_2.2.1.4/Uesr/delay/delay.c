#include "stm32f10x.h"
#include "delay.h"


//毫秒级的延时(粗延时)
void Delay_ms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12000;       //这要根据系统时钟频率进行计算
      while(i--) ;
   }
}


//微秒级的延时(粗延时)
void Delay_us(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12;       //这要根据系统时钟频率进行计算
      while(i--) ;
   }
}
