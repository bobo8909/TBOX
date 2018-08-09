#include "CANsend_task.h"

#define SEND_CANID1 0x18DC11F7
#define SEND_CANID2 0x18DC22F7
#define SEND_CANID3 0x18DC33F7


CanRxMsg CANSendBuf[3] = {0,0,0,0,0,{0},0};

/**********************************
 *函数名：void CANsend_task(void)
 *函数功能：CAN数据发送任务
 *参数:None
 *返回值:none
***********************************/
void CANsend_task(void)
{
	
	if(g_TIMFlag.bits.CANFlag == 1)
	{		
		g_TIMFlag.bits.CANFlag = 0;		
		memcpy(CANSendBuf[0].Data, &g_BCM1SendVal, 8);
//		memcpy(CANSendBuf[1].Data, &g_BCM2SendVal, 8);
//		memcpy(CANSendBuf[2].Data, &g_BCM3SendVal, 8);
//		
		CanSendMsg(SEND_CANID1, CANSendBuf[0].Data, 8);
//		CanSendMsg(SEND_CANID2, CANSendBuf[1].Data, 8);
//		CanSendMsg(SEND_CANID3, CANSendBuf[2].Data, 8);
//			
//		printf("11");
		memset(CANSendBuf[0].Data, 0, 8);
//		memset(CANSendBuf[1].Data, 0, 8);
//		memset(CANSendBuf[2].Data, 0, 8);
	
	}

	//CANErrPrintf();
}


void CANSendTimerHandler(void)
{
    static u8 CANSendCount = 0;

    if(g_TIMFlag.bits.CANFlag == 0)
    {
        CANSendCount++;
        if(CANSendCount == 100)
        {
            CANSendCount = 0;
            g_TIMFlag.bits.CANFlag = 1;
            //g_TIMFlag.bits.ATUartSendFlag = 1;
        }
    }

}
