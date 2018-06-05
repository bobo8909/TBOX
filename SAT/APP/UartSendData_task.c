#include "UartSendData_task.h"

#define USARTSENDBUF 16
#define USARTSENDCANDATA 0
UARTSENDDATA gUartSendData[USARTSENDBUF] = {{0},0};

static void SwitchSendToCPUUartData(u8* src,u8* SendVal,u16 srclen)
{
    u16 i;
     for(i = 0;i<srclen;i++)
    {
        src[2*i] = SendVal[i]>>4;
        src[2*i+1] = SendVal[i]&0xf;
    }
      for(i = 0;i<srclen*2;i++)
     {
         if(src[i] < 10)
         {
             src[i] = src[i] + '0';
         }
         else
         {
             src[i] = src[i] + 0x37;
         }
     }   

}

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

