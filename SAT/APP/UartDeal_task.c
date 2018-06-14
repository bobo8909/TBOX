#include "UartDeal_task.h"

void UartDeal_task(void)
{
	u8 i = 0;
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
		for(i = 0;i < sizeof(gN720Info.CCIDBuf);i++)
		{
			printf("%c",gN720Info.CCIDBuf[i]);
		}
		printf("\r\n");
			
		gN720InitStep = N720SendATCSQ;
		g_N720InitRecvFlag.bits.bN720RecvATCPINInfoFlag = 0;
	}
}
