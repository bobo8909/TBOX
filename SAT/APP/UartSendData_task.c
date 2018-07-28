#include "UartSendData_task.h"

#define USARTSENDBUF 10
#define USARTRECVCANDATA 10

UARTSENDDATA gUartSendData[USARTSENDBUF] = {0};
#if 1
static void SwitchCANData(u8* src ,u8* SendVal,u16 srclen)
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
#else
static void SwitchCANData(u16* src ,u8* SendVal,u16 srclen)
{
	u16 i;
	u8 Tempbuf[40] = {0};
	
	for(i = 0; i < srclen; i++)
	{
		Tempbuf[2*i] = SendVal[i] >> 4;
		Tempbuf[2*i+1] = SendVal[i] & 0xf;
	}
#if 0
	printf("tempbuf:");
	for(i = 0;i<40;i++)
	{
		printf("%02x ",Tempbuf[i]);
	}
	printf("\r\n");
	
	printf("src:");
#endif
	for(i = 0; i< srclen ; i++)
	{
		if (Tempbuf[2*i+1] <= 0x09)
		{
			src[i] = Tempbuf[2*i+1] + '0';
		}
		else if((Tempbuf[2*i+1] >= 0x0a) && (Tempbuf[2*i+1] <= 0x0f))
		{
			src[i] = Tempbuf[2*i+1] + 0x37;
		}
		else
		{
			src[i] = 0xFF;
		}

		
		if (Tempbuf[2*i] <= 0x09)
		{
			src[i] |= ((u16)(Tempbuf[2*i] + '0')) << 8;
		}
		else if((Tempbuf[2*i] >= 0x0a) && (Tempbuf[2*i] <= 0x0f))
		{
			src[i] |= ((u16)(Tempbuf[2*i] + 0x37)) << 8;
		}
		else
		{
			src[i] |= 0xFF00;
		}
		//printf("|%02x,%02x|",Tempbuf[i],Tempbuf[i+1]);
		//src[i] = (((u16)Tempbuf[i+1]) << 8 | Tempbuf[i]);
		//printf("%0x ",src[i]);
	}   
	//printf("\r\n");
}

#endif
void ATCommSendCAN(void)
{
	u16 i = 0;
	static u8 CanRxCount = 0;
    static u8 UartRecvCount = 0;
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
		SwitchCANData(gUartSendData[UartRecvCount].UartSendBuf,gCanRxRawDataBuf[CanRxCount].Buf, 
							USART_SEND_LEN);
		
		memset(gCanRxRawDataBuf + CanRxCount, 0, sizeof(gCanRxRawDataBuf[0]));

		#if 0
		printf("uartdata:");
		for(i = 0; i < USART_SEND_LEN; i++)
		{
			printf("%02x ",gUartSendData[CanRxCount].UartSendBuf[i]);
		}
		printf("\r\n");
		#endif
		gUartSendData[UartRecvCount].UartSendFlag = 1;
		gCanRxRawDataBuf[CanRxCount].NewDataFlag = 0;
        
        UartRecvCount++;
        if(UartRecvCount == USARTRECVCANDATA)
        {
            UartRecvCount = 0;
        }
        
	}
    
    CanRxCount++;
    if(CanRxCount == RXMSG_LEN)
    {
        CanRxCount = 0;
    }

}


void UartSendData_task(void)
{
//	u16 i = 0,j = 0;
    static u8 UartSendDataCount = 0;
	//if(g_TIMFlag.bits.ATUartSendFlag == 1)
	{
		ATCommSendCAN();
		//g_TIMFlag.bits.ATUartSendFlag = 0;
	}
	
	if(gUartSendData[UartSendDataCount].UartSendFlag == 1)
	{
	    #if 0
		for(j = 0; j < USART_SEND_LEN; j++)
		{
			USART_SendData(USART1,(u16)gUartSendData[UartSendDataCount].UartSendBuf[j]);		
		}
        #else
        printf("send data\r\n");

        if(g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag == 1)
        {
            //printf("111\r\n");
            g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag = 0;
            USART2_Send_String(gUartSendData[UartSendDataCount].UartSendBuf);
            gN720TCPInitStep = N720TCPInitFinish;
        }
        #endif
		//printf("\r\n");
		memset(gUartSendData + UartSendDataCount,0,sizeof(gUartSendData[0]));
		//gUartSendData[UartSendDataCount].UartSendFlag = 0;
    }
    
    UartSendDataCount++;
    if(UartSendDataCount == USARTSENDBUF)
    {
        UartSendDataCount = 0;
    }
    
}

