#include "CANsend_task.h"

#define SEND_CANID1 0x18DC11F7
#define SEND_CANID2 0x18DC22F7
#define SEND_CANID3 0x18DC33F7


CanRxMsg CANSendBuf[3] = {0,0,0,0,0,{0},0};

/*CANÊý¾Ý·¢ËÍ*/
void CANsend_task(void)
{
	
	if(g_TIMFlag.bits.CANFlag == 1)
	{		
		memcpy(CANSendBuf[0].Data, &g_BCM1SendVal, 8);
//		memcpy(CANSendBuf[1].Data, &g_BCM2SendVal, 8);
//		memcpy(CANSendBuf[2].Data, &g_BCM3SendVal, 8);
//		
		CanSendMsg(SEND_CANID1, CANSendBuf[0].Data, 8);
//		CanSendMsg(SEND_CANID2, CANSendBuf[1].Data, 8);
//		CanSendMsg(SEND_CANID3, CANSendBuf[2].Data, 8);
//			
		g_TIMFlag.bits.CANFlag = 0;		
//		printf("11");
		memset(CANSendBuf[0].Data, 0, 8);
//		memset(CANSendBuf[1].Data, 0, 8);
//		memset(CANSendBuf[2].Data, 0, 8);
	
	}

	CANErrPrintf();
}

