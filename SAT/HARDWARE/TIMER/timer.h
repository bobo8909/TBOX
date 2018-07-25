#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


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
        u8 bnull : 7;
	}bits;
}STRUCT_N720InitTIMFlag;
extern STRUCT_N720InitTIMFlag g_N720InitTIMFlag;

typedef union
{
	u16 Byte;
	struct 
	{
		u8 bN720SendATCGDCONTCommandFlag : 1;
		u8 bN720SendATXIICCommandFlag : 1;
        u8 bN720SendATXIIC1CommandFlag : 1;
        u8 bN720SendATTCPCLOSECommandFlag : 1;
        u8 bN720SendATTCPSETUPCommandFlag : 1;
        u8 bN720SendATTCPACKCommandFlag : 1;
        u8 bN720SendATTCPSENDCommandFlag : 1;
        u8 bN720SendATXGAUTHCommandFlag : 1;
        u8 bN720SendATPrepareSendCommandFlag : 1;
        u8 bNull : 7;
	}bits;
}STRUCT_N720TCPInitTIMFlag;
extern STRUCT_N720TCPInitTIMFlag g_N720TCPInitTIMFlag;

#define N720StartSend 0x00
#define N720SendAT 0x01
#define N720SendATI 0x02
#define N720SendATCCID 0x03
#define N720SendATCPIN 0x04
#define N720SendATCSQ 0x05
#define N720SendATCREG 0x06
#define N720SendATCGATT 0x07
#define N720SendATMYSYSINFO 0x08
#define N720InitFinish 0xFE
extern u8 gN720InitStep;

#define N720TCPStartSend 0x00
#define N720SendTCPCGDCONT 0x01
#define N720SendTCPXIIC 0x02
#define N720SendTCPXIIC1 0x03
#define N720SendTCPCLOSE 0x04
#define N720SendTCPSETUP 0x05
#define N720SendTCPSETUPING 0x06
#define N720SendTCPACK 0x07
#define N720SendTCPSEND 0x08
#define N720SendTCPXGAUTH 0x09
#define N720SendTCPSTARTSEND 0x0A
#define N720SendDataFinish 0x0B
#define N720TCPInitFinish 0xFE
extern u8 gN720TCPInitStep;

void TIM_INIT(void);

#endif
