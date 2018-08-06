#include "UartDeal_task.h"

const char CPINResponeBuf[] = {"READY"};

/**********************************
 *函数名：void UartDeal_task(void)
 *函数功能：接收串口数据处理任务
 *参数:None
 *返回值:none
***********************************/
void UartDeal_task(void)
{
	u8 i = 0;
    
    char N720CSQX[2] = {0},N720CSQY[2] = {0};
    u8 N720CSQXValue = 0,N720CSQYValue = 0;
#if 0
	static u8 UartDealIndex = 0;

	if(gUARTData[UartDealIndex].USART_RX_Flag == 1)
	{
		gUARTData[UartDealIndex].USART_RX_Flag = 0;
		//USART_SendData(USART1,gUARTData[UartDealIndex].USART_RX_BUF);
	}
	UartDealIndex++;
	if(UartDealIndex == USART_REC_BUF)
	{
		UartDealIndex = 0;
	}
	#endif

	if(g_N720InitRecvFlag.bits.bN720RecvATInfoFlag == 1)
	{
		gN720InitStep = N720SendATI;
		g_N720InitRecvFlag.bits.bN720RecvATInfoFlag = 0;
	}
	
	if(g_N720InitRecvFlag.bits.bN720RecvATIInfoFlag == 1)
	{
		
		printf("ver:");
		for(i = 0;i < sizeof(gN720Info.SoftwareVerBuf);i++)
		{
			printf("%c",gN720Info.SoftwareVerBuf[i]);
		}
		printf("\r\n");
		
		gN720InitStep = N720SendATCCID;
		g_N720InitRecvFlag.bits.bN720RecvATIInfoFlag = 0;
	}

	if(g_N720InitRecvFlag.bits.bN720RecvATCCIDInfoFlag == 1)
	{
		printf("CCID:");
		for(i = 0;i < sizeof(gN720Info.CCIDBuf);i++)
		{
			printf("%c",gN720Info.CCIDBuf[i]);
		}
		printf("\r\n");
			
		gN720InitStep = N720SendATCPIN;		
		g_N720InitRecvFlag.bits.bN720RecvATCCIDInfoFlag = 0;
	}

	
	if(g_N720InitRecvFlag.bits.bN720RecvATCPINInfoFlag == 1)
	{
		printf("CPIN:");
		for(i = 0;i < sizeof(gN720Info.CPINBuf);i++)
		{
			printf("%c",gN720Info.CPINBuf[i]);
		}
		printf("\r\n");

		if(strstr((const char*)gN720Info.CPINBuf,CPINResponeBuf) != NULL)
		{
			gN720InitStep = N720SendATCSQ;
			g_N720InitRecvFlag.bits.bN720RecvATCPINInfoFlag = 0;
		}
	}

    if(g_N720InitRecvFlag.bits.bN720RecvATCSQInfoFlag == 1)
	{
		printf("CSQ:");
		for(i = 0;i < sizeof(gN720Info.CSQBuf);i++)
		{
			printf("%c",gN720Info.CSQBuf[i]);
		}
		printf("\r\n");
        memcpy(N720CSQX, gN720Info.CSQBuf + 1, 2);
        N720CSQXValue = atoi(N720CSQX);
        memcpy(N720CSQY, gN720Info.CSQBuf + 4, 2);
        N720CSQYValue = atoi(N720CSQY);
        if((N720CSQXValue <= 31) && (N720CSQXValue >= 12))
        {
            gN720InitStep = N720SendATCREG;
            CMDFailedCount = 0;
        }
		g_N720InitRecvFlag.bits.bN720RecvATCSQInfoFlag = 0;
	}

    if(g_N720InitRecvFlag.bits.bN720RecvATCREGInfoFlag == 1)
	{
		printf("CREG:");
		for(i = 0;i < sizeof(gN720Info.CREGBuf);i++)
		{
			printf("%c",gN720Info.CREGBuf[i]);
		}
		printf("\r\n");
		if((gN720Info.CREGBuf[3] == '1') ||(gN720Info.CREGBuf[3] == '5'))	
		{
		    gN720InitStep = N720SendATCGATT;
            CMDFailedCount = 0;
        }
		g_N720InitRecvFlag.bits.bN720RecvATCREGInfoFlag = 0;
	}

    if(g_N720InitRecvFlag.bits.bN720RecvATCGATTInfoFlag == 1)
	{
		printf("CGATT:");
		for(i = 0;i < sizeof(gN720Info.CGATTBuf);i++)
		{
			printf("%c",gN720Info.CGATTBuf[i]);
		}
		printf("\r\n");
		if(gN720Info.CGATTBuf[1] == '1')
		{
		    gN720InitStep = N720SendATMYSYSINFO;
            CMDFailedCount = 0;
        }
		g_N720InitRecvFlag.bits.bN720RecvATCGATTInfoFlag = 0;
	}

    if(g_N720InitRecvFlag.bits.bN720RecvATMYSYSINFOInfoFlag == 1)
	{
		printf("MYSYSINFO:");
		for(i = 0;i < sizeof(gN720Info.MYSYSINFOBuf);i++)
		{
			printf("%c",gN720Info.MYSYSINFOBuf[i]);
		}
		printf("\r\n");
			
		gN720InitStep = N720InitFinish;
        gN720TCPInitStep = N720SendTCPCGDCONT;
		g_N720InitRecvFlag.bits.bN720RecvATMYSYSINFOInfoFlag = 0;
		g_N720InitRecvFlag.bits.bN720InitFinish = 1;
	}

	if(g_N720TCPInitFlag.bits.bN720RecvATCGDCONTInfoFlag == 1)
	{
	    printf("recv cgdcont ok\r\n");
		//for(i = 0;i < sizeof(gN720Info.TCPCGDCONT);i++)
		//{
		//	printf("%c",gN720Info.TCPCGDCONT[i]);
		//}
		//printf("\r\n");
		gN720TCPInitStep = N720SendTCPXGAUTH;
		g_N720TCPInitFlag.bits.bN720RecvATCGDCONTInfoFlag = 0;
	}

    if(g_N720TCPInitFlag.bits.bN720RecvATTCPXGAUTHInfoFlag == 1)
    {        
        printf("recv XGUATH ok\r\n");
        
		gN720TCPInitStep = N720SendTCPXIIC;
        g_N720TCPInitFlag.bits.bN720RecvATTCPXGAUTHInfoFlag = 0;
    }
    
	if(g_N720TCPInitFlag.bits.bN720RecvATXIICInfoFlag == 1)
	{
	    printf("recv xiic\r\n");
		gN720TCPInitStep = N720SendTCPXIIC1;
		g_N720TCPInitFlag.bits.bN720RecvATXIICInfoFlag = 0;
        
        CMDFailedCount = 0;
        ReconnectCount = 0;
	}
    
	if(g_N720TCPInitFlag.bits.bN720RecvATXIIC1InfoFlag == 1)
	{
	    printf("xiic?:");
		for(i = 0;i < sizeof(gN720Info.TCPXIIC);i++)
		{
			printf("%c",gN720Info.TCPXIIC[i]);
		}
		printf("\r\n");
        if((gN720Info.TCPXIIC[0] != '0') &&(gN720Info.TCPXIIC[2] != '0'))
		{
		    gN720TCPInitStep = N720SendTCPCLOSE;
        }
		g_N720TCPInitFlag.bits.bN720RecvATXIIC1InfoFlag = 0;
	}
    
	if(g_N720TCPInitFlag.bits.bN720RecvATTCPCLOSEInfoFlag == 1)
	{
	    printf("TCPCLOSE:");
		for(i = 0;i < sizeof(gN720Info.TCPCLOSE);i++)
		{
			printf("%c",gN720Info.TCPCLOSE[i]);
		}
		printf("\r\n");
		gN720TCPInitStep = N720SendTCPSETUP;
		g_N720TCPInitFlag.bits.bN720RecvATTCPCLOSEInfoFlag = 0;
	}

	if( g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPInfoFlag == 1)
	{
	    printf("TCPSETUP OK\r\n");

		gN720TCPInitStep = N720SendTCPSETUPING;
		g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPInfoFlag = 0;
	}    

	if( g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPINGInfoFlag == 1)
	{
	    printf("TCPSETUP:");
		for(i = 0;i < sizeof(gN720Info.TCPSETUP);i++)
		{
			printf("%c",gN720Info.TCPSETUP[i]);
		}
		printf("\r\n");
		gN720TCPInitStep = N720SendTCPACK;
		g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPINGInfoFlag = 0;
	}    

    if(g_N720TCPInitFlag.bits.bN720RecvATTCPACKInfoFlag == 1)
    {
        printf("TCPACK:");
        for(i = 0;i < sizeof(gN720Info.TCPACK);i++)
        {
            printf("%c",gN720Info.TCPACK[i]);
        }
        printf("\r\n");
        gN720TCPInitStep = N720SendTCPSEND;
        g_N720TCPInitFlag.bits.bN720RecvATTCPACKInfoFlag = 0;
    }

    if(g_N720TCPInitFlag.bits.bN720RecvATTCPSENDInfoFlag == 1)
    {
        //printf("start send data\r\n");
        g_N720TCPInitTIMFlag.bits.bN720SendATPrepareSendCommandFlag = 1;
        g_N720TCPInitFlag.bits.bN720RecvATTCPSENDInfoFlag = 0;
        
        gN720TCPInitStep = N720SendTCPSTARTSEND;
    }

    if(g_N720TCPInitFlag.bits.bN720SendATSendDataSuccessCommandFlag == 1)
    {
        
        printf("Send data finish:");
        #if 0
        for(i = 0;i < sizeof(gN720Info.TCPFinish);i++)
        {
            printf("%c",gN720Info.TCPFinish[i]);
        }
        #endif
        printf("\r\n");
        g_N720TCPInitFlag.bits.bN720SendATSendDataSuccessCommandFlag = 0;
        
        gN720TCPInitStep = N720SendTCPSEND;
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag = 0;
    }

    if( g_N720InitRecvFlag.bits.bN720RecvCANDataFlag == 1)
    {
        g_N720InitRecvFlag.bits.bN720RecvCANDataFlag = 0;
    #if 0
        printf("recv data\r\n");
        for(i = 0;i < sizeof(gN720Info.TCPRecvCANData);i++)
        {
            printf("%c",gN720Info.TCPRecvCANData[i]);
        }
        printf("\r\n");
    #endif
    }
    
}
