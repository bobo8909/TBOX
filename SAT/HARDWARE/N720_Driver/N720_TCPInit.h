#ifndef __N720_TCPINIT_H
#define __N720_TCPINIT_H
#include "sys.h"

#define COMMAND_ATCGDCONT 0
#define COMMAND_ATXIIC 1
#define COMMAND_ATXIIC1 2
#define COMMAND_ATTCPCLOSE 3
#define COMMAND_ATTCPSETUP 4
#define COMMAND_ATTCPACK 5
#define COMMAND_ATTCPSEND 6
#define COMMAND_ATTCPXGAUTH 7 
#define COMMAND_ATCGDCONTCUCC 8
#define COMMAND_ATCGDCONTCTCC 9
#define COMMAND_ATTCPXGAUTHCUCC_CTCC 10

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
#define N720WaitRecvData 0xFD
#define N720TCPInitFinish 0xFE
extern u8 gN720TCPInitStep;

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
        u8 bN720SendATDataNoResponseFlag : 1;   //发送"tcpsend"指令后如果收不到"tcpsend ok"的回复
        u8 bNull : 6;
	}bits;
}STRUCT_N720TCPInitTIMFlag;
extern STRUCT_N720TCPInitTIMFlag g_N720TCPInitTIMFlag;
extern u16 N720TCPInitCount;

void N720_TCPInit(void);
void N720TCPInitTimerHandler(void);

#endif
