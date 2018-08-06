#include "CANrecv_task.h"





/**********************************
 *函数名：void CANrecv_task(void)
 *函数功能：接收CAN数据处理任务
 *参数:None
 *返回值:none
***********************************/
void CANrecv_task(void)
{
//	u8 busrec = 0;
#if 0
	if (CANRecvDataBuf[0].NewDataFlag == 1)
	{
	
		memcpy(&g_VCU2RecvVal,CANRecvDataBuf[0].Buf,8);

		CANRecvDataBuf[0].NewDataFlag = 0;		
	}

	if (CANRecvDataBuf[1].NewDataFlag == 1)
	{
	
		memcpy(&g_VCU3RecvVal,CANRecvDataBuf[1].Buf,8);
		
		CANRecvDataBuf[1].NewDataFlag = 0;
	}

	if (CANRecvDataBuf[2].NewDataFlag == 1)
	{
	
		memcpy(&g_VCU5RecvVal,CANRecvDataBuf[2].Buf,8);
		
		CANRecvDataBuf[2].NewDataFlag = 0;

	}
#endif	
}


