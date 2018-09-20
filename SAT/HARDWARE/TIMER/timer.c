#include "timer.h"

#define DATA_SEND_DELAY 5000

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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM6, ENABLE);	//使能TIMx					 

}

static u16 DataSendCount = 0;
u8 DataSendFlag = 0;
u16 printCSQCount = 0;

void TIM6_IRQHandler(void)   //TIM2中断
{

	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)	//检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx更新中断标志
    #if 1
        if((g_N720TCPInitFlag.bits.bN720SendACKFinishFlag == 1)
        &&(g_N720GPSFalg.bits.bN720GPSOpenSuccessFlag == 1))
        {
            DataSendCount++;
    		if(DataSendCount == DATA_SEND_DELAY)
            {
                DataSendFlag = 1;
                DataSendCount = 0;
            }  
        }
        else
        {
            DataSendCount = 0;
        }
        
        SendDataNoResponseTimerHandler();
        
        if (g_N720TCPInitFlag.bits.bN720SendACKFinishFlag == 1)
        {
            printCSQCount++;
            if(printCSQCount == 2000)
            {
                printCSQCount = 0;
                g_N720InitTIMFlag.bits.bN720SendATCSQPrintFlag = 1;
            }
        }
        #endif
		/*can count*/
//        CANSendTimerHandler();
		/*LED count*/
        LedTimerHandler();

		/*N720*/
        N720InitTimerHandler();
        N720TCPInitTimerHandler();

        N720PowerTimerHandler();
        GPS_TimerHandler();

	}
}

/**********************************
 *函数名：void TIM_INIT(void)
 *函数功能：定时器初始化
 *参数:None
 *返回值:none
***********************************/
void TIM_INIT(void)
{
	TIM6_Init(999, 71);// 1kHz	1ms
}
