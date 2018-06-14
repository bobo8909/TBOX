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
		u8 ATUartSendFlag : 1;
		u8 b4 : 1;
		u8 b5 : 1;
		u8 b6 : 1;
		u8 b7 : 1;
	}bits;
}STRUCT_TIMFLAG;
extern STRUCT_TIMFLAG g_TIMFlag;

typedef union
{
	u16 Byte;
	struct 
	{
		u8 bN720StartInitFlag : 1;
		u8 bN720SendATCommandFlag : 1;
		u8 bN720SendATICommandFlag : 1;
		u8 bN720SendATCCIDCommandFlag : 1;
		u8 bN720SendATCPINCommandFlag : 1;
		u8 bN720SendATCSQCommandFlag : 1;
		u8 bN720SendATCREGCommandFlag : 1;
		u8 bN720SendATCGATTCommandFlag : 1;
		u8 bN720SendATMYSYSINFOCommandFlag : 1;
		u8 b9 : 1;
		u8 bnull : 6;
	}bits;
}STRUCT_N720InitTIMFlag;
extern STRUCT_N720InitTIMFlag g_N720InitTIMFlag;

extern u16 AngSensorAMinus;
extern u16 AngSensorBMinus;

extern int PWM3OUT2;
extern int PWM3OUT4; 
extern int PWM4OUT2; 				
extern int PWM2OUT4; 

#define N720StartSend 0x00
#define N720SendAT 0x01
#define N720SendATI 0x02
#define N720SendATCCID 0x03
#define N720SendATCPIN 0x04
#define N720SendATCSQ 0x05
#define N720SendATCREG 0x06
#define N720SendATCGA 0x07
#define N720SendATMYSYSINFO 0x08
#define N720InitFinish 0xFE
extern u8 gN720InitStep;

//extern u8 g_TIM2PluseCount;
//extern u8 g_CANSpeedFlag;
void TIM_INIT(void);
//u8 TIM6PluseFunc(void);

#endif
