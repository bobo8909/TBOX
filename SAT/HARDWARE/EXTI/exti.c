#include "exti.h"


//外部中断0初始化
void EXTI0_Init(void)
{
 
   	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

   //GPIOE.0	  中断线以及中断初始化配置 上升沿触发   //车速脉冲数读取
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
   
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}

#if 0
void EXTI1_Init(void)
{
 
   	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

   //GPIOE.1	  中断线以及中断初始化配置 上升沿触发   //车速脉冲数读取
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource1);
   
  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}

void EXTI2_Init(void)
{
 
   	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

   //GPIOE.2	  中断线以及中断初始化配置 上升沿触发   //车速脉冲数读取
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource2);
   
  	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x04;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}
#endif
//外部中断0服务程序 
u16 SpeedCount = 0;
//static u16 PluseCount = 0;
void EXTI0_IRQHandler(void)
{
//	if (EXTI_GetITStatus(EXTI_Line0) != RESET)//判断某个线上的中断是否发生 
//	{
//		g_StructExtiFlag.bits.SpeedStartFlag = 1;
//		if(g_TIMFlag.bits.SpeedFlag == 1)
//		{
//			 SpeedCount = PluseCount;
//			 PluseCount = 0;
//			 g_TIMFlag.bits.SpeedFlag = 0;
//			 g_StructExtiFlag.bits.SpeedCalFlag = 1;
//		}
//		PluseCount++;
//		EXTI_ClearITPendingBit(EXTI_Line0);	//清除 LINE 上的中断标志位
//	}	  
}

u8 SpeedPluseFunc(void)
{
	u8 SpeedPluse = 0;
	
	SpeedPluse = SpeedCount;
	SpeedCount = 0;	
	//printf("TIMCount = %d\r\n",TIMPluse);
	return SpeedPluse;
}

#if 0
//外部中断1服务程序
u16 AngSensorACount = 0;
void EXTI1_IRQHandler(void)
{
	static u16 APluseCount = 0;
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)//判断某个线上的中断是否发生 
	{
		
		if(g_StructExtiFlag.bits.ExtiAngSensorBFlag == 0)
		{
			g_StructExtiFlag.bits.ExtiAngSensorAFlag = 1;
			g_StructExtiFlag.bits.ExtiAngSensorAStartFlag = 1;
			if(g_TIMFlag.bits.TIMAngSensorAFlag == 1)
			{
				AngSensorACount = APluseCount;
				APluseCount = 0;
				g_TIMFlag.bits.TIMAngSensorAFlag = 0;
				
				g_StructExtiFlag.bits.ExtiAngSensorACalFlag = 1;
			}
			
			APluseCount++;
			AngSensorAMinus = 20;

		}
		EXTI_ClearITPendingBit(EXTI_Line1);	//清除 LINE 上的中断标志位
	}	  
}
u8 APluseFunc(void)
{
	u8 APluse = 0;
	
	APluse = AngSensorACount;
	SpeedCount = 0;	
	printf("APluse = %d\r\n",APluse);
	return APluse;
}

//外部中断1服务程序 
u16 AngSensorBCount = 0;

void EXTI2_IRQHandler(void)
{
	static u16 BPluseCount = 0;

	if (EXTI_GetITStatus(EXTI_Line2) != RESET)//判断某个线上的中断是否发生 
	{
		if(g_StructExtiFlag.bits.ExtiAngSensorAFlag == 0)
		{
			g_StructExtiFlag.bits.ExtiAngSensorBFlag = 1;
			g_StructExtiFlag.bits.ExtiAngSensorBStartFlag = 1;
			if(g_TIMFlag.bits.TIMAngSensorBFlag == 1)
			{
				AngSensorBCount = BPluseCount;
				BPluseCount = 0;
				g_TIMFlag.bits.TIMAngSensorBFlag = 0;
				g_StructExtiFlag.bits.ExtiAngSensorBCalFlag = 1;
			}
			
			BPluseCount++;
			AngSensorAMinus = 20;
		
		}
		EXTI_ClearITPendingBit(EXTI_Line2);	//清除 LINE 上的中断标志位
	}	  
}

 
u8 BPluseFunc(void)
{
	u8 BPluse = 0;
	
	BPluse = AngSensorBCount;
	SpeedCount = 0;	
	printf("BPluse = %d\r\n",BPluse);
	return BPluse;
}
#endif



 
