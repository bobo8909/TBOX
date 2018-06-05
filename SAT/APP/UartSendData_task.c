#include "UartSendData_task.h"

#define USARTSENDBUF 16
#define USARTSENDCANDATA 0
UARTSENDDATA gUartSendData[USARTSENDBUF] = {{0},0};

void ATCommSendCAN(u8 *buf)
{
//	char Head[] = ""; 
}



void UartSendData_task(void)

{
	u8 i = 0;
	for(i = 0; i < USART_REC_BUF; i ++)
	{
		if(gUartSendData[i].UartSendFlag == 1)
		{
			USART_SendData(USART1,gUartSendData[i].UartSendBuf);			
			return ;
		}
	}
}

