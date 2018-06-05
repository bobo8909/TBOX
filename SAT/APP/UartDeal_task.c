#include "UartDeal_task.h"

void UartDeal_task(void)
{
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
}
