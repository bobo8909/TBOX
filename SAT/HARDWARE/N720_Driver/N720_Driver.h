#ifndef _N720_DRIVER_H
#define _N720_DRIVER_H

#include "sys.h"

#define DELAY10S 10000
#define DELAY1S 1000
#define SEND_DELAYMS 50
#define DELAY_POWERON 250
#define DELAY_POWEROFF 2500
#define DELAY_POWERRESET 5000
#define ATFAILEDCOUNT 15

#define COMMAND_AT 0
#define COMMAND_ATI 1
#define COMMAND_ATCCID 2
#define COMMAND_ATCPIN 3
#define COMMAND_ATCSQ 4
#define COMMAND_ATCREG 5
#define COMMAND_ATCGATT 6
#define COMMAND_ATMYSYSINFO 7

#define N720POWERKEY PFout(4)
#define N720_ON 1
#define N720_OFF 0

extern u8 CMDFailedCount;
extern u8 ReconnectCount;

#define N720StartSend 0x00
#define N720SendAT 0x01
#define N720SendATI 0x02
#define N720SendATCCID 0x03
#define N720SendATCPIN 0x04
#define N720SendATCSQ 0x05
#define N720SendATCREG 0x06
#define N720SendATCGATT 0x07
#define N720SendATMYSYSINFO 0x08
#define N720PrepareReset 0xFD
#define N720InitFinish 0xFE
extern u8 gN720InitStep;


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
		u8 bN720PowerONFlag : 1;
		u8 bN720PowerOFFFlag : 1;        
        u8 bN720PowerkeyResetFlag : 1;
        u8 bNull : 4;
	}bits;
}STRUCT_N720InitTIMFlag;
extern STRUCT_N720InitTIMFlag g_N720InitTIMFlag;

void N720Init(void);
void N720IOInit(void);
void N720PowerON(void);
void N720PowerOFF(void);
void N720PowerkeyReset(void);
void N720InitTimerHandler(void);
void N720PowerTimerHandler(void);


#endif
