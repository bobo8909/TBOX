#include "led.h"

   
//LED IO初始化
void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOF, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	GPIO_SetBits(GPIOF,GPIO_Pin_6);						 //PB.5 输出高

}

/**********************************
 *函数名：void Led_task(void)
 *函数功能：心跳灯任务
 *参数:None
 *返回值:none
***********************************/
void Led_task(void)
{
	if(g_TIMFlag.bits.LedFlag == 1)
	{
		LED0 = LED_ON;
	}
	else
	{
		LED0 = LED_OFF;
	}
}

void LedTimerHandler(void)
{
    static u16 LEDCount = 0;
    
    LEDCount++;
    if(LEDCount == 499)
    {
        g_TIMFlag.bits.LedFlag = 1;
    }
    else if(LEDCount == 999)
    {
        g_TIMFlag.bits.LedFlag = 0;
        LEDCount = 0;
    }
}
