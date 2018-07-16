#include "UartDeal_task.h"

const char CPINResponeBuf[] = {"READY"};


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
		g_N720InitRecvFlag.bits.bN720RecvATMYSYSINFOInfoFlag = 0;
		g_N720InitRecvFlag.bits.bN720InitFinish = 1;
	}
    
}
