#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define ARR_200Hz 9999
#define PSC_200Hz 35

#define ARR_1KHz 35999
#define PSC_1KHz 1

#define ARR_2KHz 35999
#define PSC_2KHz 0

#define ARR_2140Hz 33643
#define PSC_2140Hz 0

#define EncoderArr (5000*4-1)
#define EncoderPSC 0
typedef union
{
	u8 Byte;
	struct 
	{
		u8 CANFlag : 1;
		u8 SpeedFlag : 1;
		u8 LedFlag : 1;
		u8 TIMAngSensorAFlag : 1;
		u8 TIMAngSensorBFlag : 1;
		u8 EncoderFlag : 1;
		u8 DACSendFlag : 1;
		u8 b7 : 1;
	}bits;
}STRUCT_TIMFLAG;
extern STRUCT_TIMFLAG g_TIMFlag;

extern u16 AngSensorAMinus;
extern u16 AngSensorBMinus;

extern int PWM3OUT2;
extern int PWM3OUT4; 
extern int PWM4OUT2; 				
extern int PWM2OUT4; 
//extern u8 g_TIM2PluseCount;
//extern u8 g_CANSpeedFlag;
void TIM_INIT(void);
//u8 TIM6PluseFunc(void);

#endif
