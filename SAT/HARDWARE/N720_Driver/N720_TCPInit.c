#include "N720_TCPInit.h"

static u8 *TCPInitCommandBuf[] =
{
    "AT+CGDCONT=1,\"IP\",\"UNINET\"\r",
    "AT+XIIC=1\r",
    "AT+XIIC?\r",
    "AT+TCPCLOSE=1\r",
    "AT+TCPSETUP=1,139.196.56.130,30102\r",
    "AT+TCPACK=1\r",
    "AT+TCPSEND=1,40\r",
    "AT+XGAUTH=1,1,\"card\",\"card\"\r"
};
//static u8 SendData[5][40] = {"1234567890123456789012345678901234567890",
//                                "1111111111111111111111111111111111111111",
//                                "2222222222222222222222222222222222222222",
//                                "3333333333333333333333333333333333333333",
//                                "4444444444444444444444444444444444444444"};

u8 gN720TCPInitStep = 0;
STRUCT_N720TCPInitTIMFlag g_N720TCPInitTIMFlag = {0};

/**********************************
*函数名：void N720TCPInitTimerHandler(void)
*函数功能：N720模块TCP初始化时的延时操作(包含发送数据)
*参数:None
*返回值:none
***********************************/
u16 N720TCPInitCount = 0;
void N720TCPInitTimerHandler(void)
{
    
    if((g_N720TCPInitTIMFlag.bits.bN720SendATCGDCONTCommandFlag == 0) && (gN720TCPInitStep == N720SendTCPCGDCONT))
    {
        N720TCPInitCount++;
        if(N720TCPInitCount == DELAY1S)
        {
            g_N720TCPInitTIMFlag.bits.bN720SendATCGDCONTCommandFlag = 1;
            N720TCPInitCount = 0;
        }
    }

    if((g_N720TCPInitTIMFlag.bits.bN720SendATXIICCommandFlag == 0) && (gN720TCPInitStep == N720SendTCPXIIC))
    {
        N720TCPInitCount++;
        if(N720TCPInitCount == DELAY1S)
        {
            g_N720TCPInitTIMFlag.bits.bN720SendATXIICCommandFlag = 1;
            N720TCPInitCount = 0;
        }
    }

    if((g_N720TCPInitTIMFlag.bits.bN720SendATXIIC1CommandFlag == 0) && (gN720TCPInitStep == N720SendTCPXIIC1))
    {
        N720TCPInitCount++;
        if(N720TCPInitCount == DELAY1S)
        {
            g_N720TCPInitTIMFlag.bits.bN720SendATXIIC1CommandFlag = 1;
            N720TCPInitCount = 0;

            CMDFailedCount++;
            if(CMDFailedCount == 20)
            {
                ReconnectCount++;
                if(ReconnectCount == 3)
                {
                    ReconnectCount = 0;
                    N720PowerkeyReset();
                    gN720InitStep = N720PrepareReset;
                }
                else
                {
                    CMDFailedCount = 0;
                    gN720InitStep = N720SendAT;
                }
            }
        }
    }

    
    if((g_N720TCPInitTIMFlag.bits.bN720SendATTCPCLOSECommandFlag == 0) && (gN720TCPInitStep == N720SendTCPCLOSE))
    {
        N720TCPInitCount++;
        if(N720TCPInitCount == DELAY1S)
        {
            g_N720TCPInitTIMFlag.bits.bN720SendATTCPCLOSECommandFlag = 1;
            N720TCPInitCount = 0;
        }
    }
#if 1      
    if((g_N720TCPInitTIMFlag.bits.bN720SendATTCPSETUPCommandFlag == 0) && (gN720TCPInitStep == N720SendTCPSETUP))
    {
        N720TCPInitCount++;
        if(N720TCPInitCount == DELAY1S)
        {
        
            g_N720TCPInitTIMFlag.bits.bN720SendATTCPSETUPCommandFlag = 1;
            N720TCPInitCount = 0;

        }
    }

    if(gN720TCPInitStep == N720SendTCPSETUPING)
    {
        N720TCPInitCount++;
        if(N720TCPInitCount == DELAY30S)
        {
            N720TCPInitCount = 0;
            CMDFailedCount++;
            if(CMDFailedCount == 3)
            {
                CMDFailedCount = 0;
                N720PowerkeyReset();
                gN720InitStep = N720PrepareReset;
            }
            else
            {
                gN720TCPInitStep = N720SendTCPSETUP;
                g_N720TCPInitTIMFlag.bits.bN720SendATTCPSETUPCommandFlag = 0;
            }
            
        }

    }
#endif
if((g_N720TCPInitTIMFlag.bits.bN720SendATXGAUTHCommandFlag == 0) && (gN720TCPInitStep == N720SendTCPXGAUTH))
{
    N720TCPInitCount++;
    if(N720TCPInitCount == DELAY1S)
    {
        g_N720TCPInitTIMFlag.bits.bN720SendATXGAUTHCommandFlag = 1;
        N720TCPInitCount = 0;
    }
}
#if 1
if((g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag == 0) && (gN720TCPInitStep == N720SendTCPACK))
{
    N720TCPInitCount++;
    if(N720TCPInitCount == DELAY1S)
    {
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag = 1;
        N720TCPInitCount = 0;
    }
}
#endif
    if(g_N720TCPInitTIMFlag.bits.bN720SendATPrepareSendCommandFlag == 1)
    {
         N720TCPInitCount++;
         if(N720TCPInitCount == SEND_DELAYMS)
         {
            N720TCPInitCount = 0;
            g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag = 1;
            g_N720TCPInitTIMFlag.bits.bN720SendATPrepareSendCommandFlag = 0;
         }
    }
}



/**********************************
 *函数名：void N720_TCPInit(void)
 *函数功能：N720模块内部协议栈TCP客户端连接流程
 *参数:None
 *返回值:none
***********************************/
void N720_TCPInit(void)
{
//    static u8 i = 0;
    if(g_N720TCPInitTIMFlag.bits.bN720SendATCGDCONTCommandFlag == 1)
    {
        printf("send ATCGDCONT\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATCGDCONT]);
        g_N720TCPInitTIMFlag.bits.bN720SendATCGDCONTCommandFlag = 0;
    }

    if(g_N720TCPInitTIMFlag.bits.bN720SendATXGAUTHCommandFlag == 1)
    {
        printf("send XGAUTH\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATTCPXGAUTH]);
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
    if(g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag == 1)
    {
        printf("send ATTCPACK\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATTCPACK]);
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag = 0;
    }

    #if 1
    if((gN720TCPInitStep == N720SendTCPSEND) && (g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag == 0))
    {
        printf("send ATTCPSEND\r\n");
        USART2_Send_String(TCPInitCommandBuf[COMMAND_ATTCPSEND]);
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag = 1;
    }
    #endif
    #if 0
    if(g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag == 1)
    {
        if(i == 5)
        {
            //i = 0;
            //gN720TCPInitStep = 0xFF;
            return ;
        }
        else
        {
            g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag = 0;
            printf("send data\r\n");
            USART2_Send_String(SendData[i++]);
            gN720TCPInitStep = N720TCPInitFinish;
        }
    }
    #endif
}
