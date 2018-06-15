#include "timer.h"

#define DELAY10S 10000
#define DELAY1S 1000
/*global value*/
STRUCT_TIMFLAG g_TIMFlag = {0};
STRUCT_N720InitTIMFlag g_N720InitTIMFlag = {0};

/**
  * @brief  初始化TIM6定时器，
  * @param  u16 arr：自动重装载值
  * @param  U16 psc:预分频系数
  *
  * @retval None
  */

static void TIM6_Init(u16 arr,u16 psc)
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器2的初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断，允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	//从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM6, ENABLE);	//使能TIMx					 

}



static u16 LEDCount = 0;
static u8 CANSendCount = 0;
static u16 N720InitCount = 0;
u8 gN720InitStep = 0;

void TIM6_IRQHandler(void)	 //TIM2中断
{

	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)	//检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx更新中断标志

		
		/*can count*/
		if(g_TIMFlag.bits.CANFlag == 0)
		{
			CANSendCount++;
			if(CANSendCount == 99)
			{
				g_TIMFlag.bits.CANFlag = 1;
				g_TIMFlag.bits.ATUartSendFlag = 1;
				CANSendCount = 0;
			}
		}
		
		/*LED count*/
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

		/*N720 INIT*/
		if((g_N720InitTIMFlag.bits.bN720StartInitFlag == 0) && (gN720InitStep == N720StartSend))
		{
			N720InitCount++;
			if(N720InitCount == DELAY10S)
			{
				g_N720InitTIMFlag.bits.bN720StartInitFlag = 1;
				gN720InitStep = N720SendAT;
				N720InitCount = 0;
			}
		}
		
		if((g_N720InitTIMFlag.bits.bN720SendATCommandFlag == 0) && (gN720InitStep == N720SendAT))
		{
			N720InitCount++;
			if(N720InitCount == DELAY1S)
			{
				g_N720InitTIMFlag.bits.bN720SendATCommandFlag = 1;
				N720InitCount = 0;
			}
		}
		
		if((g_N720InitTIMFlag.bits.bN720SendATICommandFlag == 0) && (gN720InitStep == N720SendATI))
		{
			N720InitCount++;
			if(N720InitCount == DELAY1S)
			{
				g_N720InitTIMFlag.bits.bN720SendATICommandFlag = 1;
				N720InitCount = 0;
			}
		}

		if((g_N720InitTIMFlag.bits.bN720SendATCCIDCommandFlag == 0) && (gN720InitStep == N720SendATCCID))
		{
			N720InitCount++;
			if(N720InitCount == DELAY1S)
			{
				g_N720InitTIMFlag.bits.bN720SendATCCIDCommandFlag = 1;
				N720InitCount = 0;
			}
		}

		if((g_N720InitTIMFlag.bits.bN720SendATCPINCommandFlag == 0) && (gN720InitStep == N720SendATCPIN))
		{
			N720InitCount++;
			if(N720InitCount == DELAY1S)
			{
				g_N720InitTIMFlag.bits.bN720SendATCPINCommandFlag = 1;
				N720InitCount = 0;
			}
		}
        
		if((g_N720InitTIMFlag.bits.bN720SendATCSQCommandFlag == 0) && (gN720InitStep == N720SendATCSQ))
		{
			N720InitCount++;
			if(N720InitCount == DELAY1S)
			{
				g_N720InitTIMFlag.bits.bN720SendATCSQCommandFlag = 1;
				N720InitCount = 0;
			}
		}
        
        if((g_N720InitTIMFlag.bits.bN720SendATCREGCommandFlag == 0) && (gN720InitStep == N720SendATCREG))
		{
			N720InitCount++;
			if(N720InitCount == DELAY1S)
			{
				g_N720InitTIMFlag.bits.bN720SendATCREGCommandFlag = 1;
				N720InitCount = 0;
			}
		}

        
        if((g_N720InitTIMFlag.bits.bN720SendATCGATTCommandFlag == 0) && (gN720InitStep == N720SendATCGATT))
		{
			N720InitCount++;
			if(N720InitCount == DELAY1S)
			{
				g_N720InitTIMFlag.bits.bN720SendATCGATTCommandFlag = 1;
				N720InitCount = 0;
			}
		}

        if((g_N720InitTIMFlag.bits.bN720SendATMYSYSINFOCommandFlag == 0) && (gN720InitStep == N720SendATMYSYSINFO))
		{
			N720InitCount++;
			if(N720InitCount == DELAY1S)
			{
				g_N720InitTIMFlag.bits.bN720SendATMYSYSINFOCommandFlag = 1;
				N720InitCount = 0;
			}
		}
        
	}
}


void TIM_INIT(void)
{

	TIM6_Init(999, 71);// 1kHz	
}
