/*
*发送数据的思路：理论上同一时间可能接收到多个ID发送过来的CAN数据，但是不能同时处理，
                因此需要在发送完一帧数据后（收到4G模块回复的发送成功），再接着发送下一帧
*
*/
#include "UartSendData_task.h"

#define USARTSENDBUF 10
#define USARTRECVCANDATA 10

UARTSENDDATA gUartSendData[USARTSENDBUF] = {0};
#if 1

/**********************************
 *函数名：static void SwitchCANData(u8* src ,u8* SendVal,u16 srclen)
 *函数功能：把16进制数转换成ASCII值
 *参数:(1)src:转换后的值存放的地址
        (2)SendVal:准备转换的值
        (3)srclen:需要转换的数据长度
 *返回值:none
***********************************/
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
/**********************************
 *函数名：void ATCommSendCAN(void)
 *函数功能：处理接收到的CAN原始数据并进行转换
 *参数:None
 *返回值:none
***********************************/
void ATCommSendCAN(void)
{
	u16 i = 0;
	static u8 CanRxCount = 0;
    static u8 UartRecvCount = 0;
    static u8 SendFlag = 0;

    if(g_N720TCPInitFlag.bits.bN720SendFinishFlag == 1)
    {
        g_N720TCPInitFlag.bits.bN720SendFinishFlag = 0;
        SendFlag = 0;
        memset(gCanRxRawDataBuf + CanRxCount, 0, sizeof(gCanRxRawDataBuf[0]));
        
        CanRxCount++;
        if(CanRxCount == RXMSG_LEN)
        {
            CanRxCount = 0;
        }
    }
    #if 1
	if((gCanRxRawDataBuf[CanRxCount].NewDataFlag == 1) && (SendFlag == 0))
	{
	    SendFlag = 1;
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
        
        gN720TCPInitStep = N720SendTCPSEND;
        
		SwitchCANData(gUartSendData[UartRecvCount].UartSendBuf,gCanRxRawDataBuf[CanRxCount].Buf, 
							sizeof(gUartSendData[UartRecvCount].UartSendBuf));
		
//		memset(gCanRxRawDataBuf + CanRxCount, 0, sizeof(gCanRxRawDataBuf[0]));

		#if 0
		printf("uartdata:%d;",sizeof(gUartSendData[UartRecvCount].UartSendBuf));
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
    
    #endif
}

/**********************************
 *函数名：void UartSendData_task(void)
 *函数功能：串口2发送数据任务：此任务是把接收到的CAN数据转换成ASCII值，然后再通过AT指令发送给N720
 *参数:None
 *返回值:none
***********************************/
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
        //printf("send data\r\n");
#if 1
        if(g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag == 1)
        {
            printf("111\r\n");
            g_N720TCPInitFlag.bits.bN720SendATStartSendCommandFlag = 0;
            //USART2_Send_String(gUartSendData[UartSendDataCount].UartSendBuf);
            USART2_Send_CANData(gUartSendData[UartSendDataCount].UartSendBuf);
            gN720TCPInitStep = N720TCPInitFinish;
            memset(gUartSendData + UartSendDataCount,0,sizeof(gUartSendData[0]));
        }
#endif
        #endif
		//printf("\r\n");
		//gUartSendData[UartSendDataCount].UartSendFlag = 0;
    }
    
    UartSendDataCount++;
    if(UartSendDataCount == USARTSENDBUF)
    {
        UartSendDataCount = 0;
    }
    
}

