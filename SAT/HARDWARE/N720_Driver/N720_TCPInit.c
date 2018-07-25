#include "N720_TCPInit.h"

static u8 *TCPInitCommandBuf[] =
{
    "AT+CGDCONT=1,\"IP\",\"UNINET\"\r",
    "AT+XIIC=1\r",
    "AT+XIIC?\r",
    "AT+TCPCLOSE=1\r",
    "AT+TCPSETUP=1,139.196.56.130,30102\r",
    "AT+TCPACK=1\r",
    "AT+TCPSEND=1,10\r"
};
static u8 SendData[5][10] = {"1234567890","1111111111","2222222222","3333333333","4444444444"};

void N720_TCPInit(void)
{
    static u8 i = 1;
    if(g_N720TCPInitTIMFlag.bits.bN720SendATCGDCONTCommandFlag == 1)
    {
        printf("send ATCGDCONT\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATCGDCONT]);
        g_N720TCPInitTIMFlag.bits.bN720SendATCGDCONTCommandFlag = 0;
    }

    if(g_N720TCPInitTIMFlag.bits.bN720SendATXGAUTHCommandFlag == 1)
    {
        printf("send XGAUTH\r\n");
        USART2_Send_String("AT+XGAUTH=1,1,\"card\",\"card\"\r");
        g_N720TCPInitTIMFlag.bits.bN720SendATXGAUTHCommandFlag = 0;
    }
    
    if(g_N720TCPInitTIMFlag.bits.bN720SendATXIICCommandFlag == 1)
    {
        printf("send ATXIIC\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATXIIC]);
        g_N720TCPInitTIMFlag.bits.bN720SendATXIICCommandFlag = 0;
    }
    
    if(g_N720TCPInitTIMFlag.bits.bN720SendATXIIC1CommandFlag == 1)
    {
        printf("send ATXIIC1\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATXIIC1]);
        g_N720TCPInitTIMFlag.bits.bN720SendATXIIC1CommandFlag = 0;
    }
    
    if(g_N720TCPInitTIMFlag.bits.bN720SendATTCPCLOSECommandFlag == 1)
    {
        printf("send ATTCPCLOSE\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATTCPCLOSE]);
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPCLOSECommandFlag = 0;
    }
    
    if(g_N720TCPInitTIMFlag.bits.bN720SendATTCPSETUPCommandFlag == 1)
    {
        printf("send ATTCPSETUP\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATTCPSETUP]);
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPSETUPCommandFlag = 0;
    }

#if 0
    if(g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag == 1)
    {
        printf("send ATTCPACK\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATTCPACK]);
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag = 0;
    }
#endif
    if((gN720TCPInitStep == N720SendTCPACK) && (g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag == 0))
    {
        printf("send ATTCPACK\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATTCPACK]);
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag = 1;

    }
    
    if((gN720TCPInitStep == N720SendTCPSEND) && (g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag == 0))
    {
        printf("send ATTCPSEND\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATTCPSEND]);
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag = 1;
    }

    
    if(g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag == 1)
    {
        printf("send data\r\n");
        g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag = 0;
        USART2_Send_String(SendData[0]);
        if(i == 5)
            i = 0;
        gN720TCPInitStep = N720TCPInitFinish;
    }
}
