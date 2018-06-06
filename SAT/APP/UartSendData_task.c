#include "UartSendData_task.h"

#define USARTSENDBUF 1
#define USARTSENDCANDATA 0

UARTSENDDATA gUartSendData[USARTSENDBUF] = {0};

static void SwitchSendToCPUUartData(u8* src ,u8* SendVal,u16 srclen)
{
	u16 i;
	for(i = 0; i < srclen; i++)
	{
		src[2*i] = SendVal[i] >> 4;
		src[2*i+1] = SendVal[i] & 0xf;
	}
	
	for(i = 0; i< srclen * 2; i++)
	{
		if (src[i] <= 0x09)
		{
			src[i] = src[i] + '0';
		}
		else if((src[i] >= 0x0a) && (src[i] <= 0x0f))
		{
			src[i] = src[i] + 0x37;
		}
		else
		{
			src[i] = 0xFF;
		}
	}   

}

void ATCommSendCAN(void)
{
	u16 i = 0;
	static u8 CanRxCount = 0;
	if(gCanRxRawDataBuf[CanRxCount].NewDataFlag == 1)
	{
		#if 0
		printf("can.extid = %x;can.dlc=%x,can.fmi=%x,can.ide=%x,can.rtr=%x,can.stdid=%x\r\n",
			RxMessage.ExtId,RxMessage.DLC,RxMessage.FMI,RxMessage.IDE,RxMessage.RTR,RxMessage.StdId);
		
		printf("gcanrx:");
		for(i = 0; i < RXMSG_LEN; i++)
		{
			printf("%02x ",gCanRxRawDataBuf[CanRxCount].Buf[i]);
		}
		printf("\r\n");
		#endif
		SwitchSendToCPUUartData(gUartSendData[USARTSENDCANDATA].UartSendBuf,gCanRxRawDataBuf[CanRxCount].Buf, 
							USART_SEND_LEN);
		
		memset(gCanRxRawDataBuf[CanRxCount].Buf, 0, RXMSG_LEN);

		#if 0
		printf("uartdata:");
		for(i = 0; i < USART_SEND_LEN; i++)
		{
			printf("%02x ",gUartSendData[USARTSENDCANDATA].UartSendBuf[i]);
		}
		printf("\r\n");
		#endif
		gUartSendData[USARTSENDCANDATA].UartSendFlag = 1;
		gCanRxRawDataBuf[CanRxCount].NewDataFlag = 0;
	}
	
	CanRxCount++;
	if(CanRxCount == RXMSG_LEN)
	{
		CanRxCount = 0;
	}
}



void UartSendData_task(void)

{
	u16 i = 0,j = 0;
	//if(g_TIMFlag.bits.ATUartSendFlag == 1)
	{
		ATCommSendCAN();
		//g_TIMFlag.bits.ATUartSendFlag = 0;
	}
	
	for(i = 0; i < USARTSENDBUF; i ++)
	{
		if(gUartSendData[i].UartSendFlag == 1)
		{
			for(j = 0; j < USART_SEND_LEN; j++)
			{
				USART_SendData(USART1,(u16)gUartSendData[i].UartSendBuf[j]);		
			}
			printf("\r\n");
			memset(gUartSendData[i].UartSendBuf,0,USART_SEND_LEN);
			gUartSendData[i].UartSendFlag = 0;
			return ;
		}
	}
}

