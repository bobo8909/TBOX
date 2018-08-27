#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"

CANRXBUF CANRecvDataBuf[3] = {0, {0}, 0};
CANRXBUF CANRecvBCMBuf = {0, {0}, 0};
CANRXBUF CANRecvEBSBuf = {0, {0}, 0};

STRUCT_VCU_2 g_VCU2RecvVal = {0};
STRUCT_VCU_3 g_VCU3RecvVal = {0};
STRUCT_VCU_5 g_VCU5RecvVal = {0};
STRUCT_BCM_1 g_RecvBCM1Val = {0};

STRUCT_BCM_1 g_BCM1SendVal = {0};
STRUCT_BCM_2 g_BCM2SendVal = {0};
STRUCT_BCM_3 g_BCM3SendVal = {0};
STRUCT_EBS_1 g_EBS1RecvVal = {0};

STRUCT_CAN_ERR_FLAG g_CANErrFlag = {0};



									  
//////////////////////////////////////////////////////////////////////////////////
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;  tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败;


u8 CanModeInit(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{

	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif

//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

//  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
    
#if 0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
    GPIO_Init(GPIOA, &GPIO_InitStructure);		//初始化IO
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化IO
#else
    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽
    GPIO_Init(GPIOB, &GPIO_InitStructure);      //初始化IO

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化IO

#endif
 	//CAN单元设置
	CAN_InitStructure.CAN_TTCM = DISABLE;						 //非时间触发通信模式  //
	CAN_InitStructure.CAN_ABOM = DISABLE;						 //软件自动离线管理	 //
  	CAN_InitStructure.CAN_AWUM = DISABLE;						 //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)//
  	CAN_InitStructure.CAN_NART = DISABLE;						 	//报文自动传送 //
  	CAN_InitStructure.CAN_RFLM = DISABLE;						 //报文不锁定,新的覆盖旧的 // 
  	CAN_InitStructure.CAN_TXFP = DISABLE;						 //优先级由报文标识符决定 //
  	CAN_InitStructure.CAN_Mode = mode;	         //模式设置： mode:0,普通模式;1,回环模式; //
  	//设置波特率
  	CAN_InitStructure.CAN_SJW = tsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1 = tbs1; //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2 = tbs2;//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler = brp;            //分频系数(Fdiv)为brp+1	//
  	CAN_Init(CAN1, &CAN_InitStructure);            // 初始化CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber = 0;	  //过滤器0
   	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;//CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)CANID_BMS_1 << 3) >> 16) & 0xFFFF;
  	CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)CANID_BMS_1 << 3) & 0xFFFF) | CAN_Id_Extended | CAN_RTR_Data;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (((u32)CANID_BMS_7 << 3) >> 16) & 0xFFFF;
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (((u32)CANID_BMS_7 << 3) & 0xFFFF) | CAN_Id_Extended | CAN_RTR_Data;;
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化


	CAN_FilterInitStructure.CAN_FilterNumber = 1;	  //过滤器1
   	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;//CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)CANID_VCU_5 << 3) >> 16) & 0xFFFF;
  	CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)CANID_VCU_5 << 3) & 0xFFFF) | CAN_Id_Extended | CAN_RTR_Data;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (((u32)CANID_EBS_1 << 3) >> 16) & 0xFFFF;
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (((u32)CANID_EBS_1 << 3) & 0xFFFF) | CAN_Id_Extended | CAN_RTR_Data;;
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器1关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //激活过滤器1
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
	
#if CAN_RX0_INT_ENABLE

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.	
	
  	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

#if 0
	CAN_ITConfig(CAN1,CAN_IT_EWG|CAN_IT_EPV|CAN_IT_BOF|CAN_IT_LEC|CAN_IT_ERR,ENABLE);

  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif	
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//使能RX0中断

CanRxMsg RxMessage;
CANRXRAWDATA gCanRxRawDataBuf[RXMSG_LEN]={0};
//static u8 RxMsgCount = 0;
//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CAN_Receive(CAN1, 0, &RxMessage);

    
	if(RxMessage.ExtId == CANID_BMS_1)
	{
		memcpy(gCanRxRawDataBuf[CAN_INDEX0].Buf,&RxMessage,sizeof(RxMessage));		
		gCanRxRawDataBuf[CAN_INDEX0].NewDataFlag = 1;

    }

    
	if(RxMessage.ExtId == CANID_BMS_7)
	{
		memcpy(gCanRxRawDataBuf[CAN_INDEX1].Buf,&RxMessage,sizeof(RxMessage));		
		gCanRxRawDataBuf[CAN_INDEX1].NewDataFlag = 1;
    }
}

#endif

//can发送一组数据
//u32CanId : 数据帧的can id;
//u8len:数据长度(最大为8);				     
//ptrMsg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 CanSendMsg(uint32_t u32CanId, u8* ptrMsg, u8 u8Len)
{	
    u8 mbox;
    u16 i = 0;
    CanTxMsg TxMessage;
    TxMessage.StdId = 0;					 // 标准标识符 
    TxMessage.ExtId = u32CanId;				   // 设置扩展标示符 
    TxMessage.IDE = CAN_Id_Extended; // 扩展帧
    TxMessage.RTR = CAN_RTR_Data;		 // 数据帧
    TxMessage.DLC = u8Len;						// 要发送的数据长度
    for(i = 0; i < u8Len; i++)
    TxMessage.Data[i] = ptrMsg[i];			          
    mbox = CAN_Transmit(CAN1, &TxMessage);   
    i=0;
    while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed)&&(i<0XFFF)) //等待发送结束
    {
      i++;	
    }
    if(i>=0XFFF)return 1;
    return 0;		

}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 CanReceiveMsg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}

void CAN1_SCE_IRQHandler(void)
{
	
	unsigned char Rx_Error_Cnt;
	unsigned char Tx_Error_Cnt;
	unsigned char Last_Error;
	
	if(CAN_GetITStatus(CAN1, CAN_IT_EWG) == SET) 	   //错误警告中断
	{
		Tx_Error_Cnt = CAN_GetLSBTransmitErrorCounter(CAN1);
		Rx_Error_Cnt = CAN_GetReceiveErrorCounter(CAN1);
		
		if((Tx_Error_Cnt >= 96) || (Rx_Error_Cnt >= 96))
		{
			g_CANErrFlag.Byte1.bits.CAN_ErrorWarning_Flag = 1;
		}

		CAN_ClearITPendingBit(CAN1, CAN_IT_EWG);
	}
	
	if(CAN_GetITStatus(CAN1, CAN_IT_EPV) == SET) 	   //错误被动中断
	{
		Tx_Error_Cnt = CAN_GetLSBTransmitErrorCounter(CAN1);
		Rx_Error_Cnt = CAN_GetReceiveErrorCounter(CAN1);
		
		if((Tx_Error_Cnt > 127)||(Rx_Error_Cnt > 127))
		{
			g_CANErrFlag.Byte1.bits.CAN_ErrorPassive_Flag = 1;
		}
		CAN_ClearITPendingBit(CAN1, CAN_IT_EPV); 	   
	}
	
	if(CAN_GetITStatus(CAN1, CAN_IT_BOF) == SET) 	   //离线中断
	{
		g_CANErrFlag.Byte1.bits.CAN_ErrorBusoff_Flag = 1;
		
		CAN_ClearITPendingBit(CAN1, CAN_IT_BOF);
	}
	
	if(CAN_GetITStatus(CAN1, CAN_IT_LEC) == SET) 	   //上次错误号中断
	{
		Last_Error = CAN_GetLastErrorCode(CAN1);
		if(Last_Error == 0x1) //位填充错误
		{
			g_CANErrFlag.Byte0.bits.CAN_ErrorCode_StuffErr_Flag = 0x01;
		}
		if(Last_Error == 0x2) //格式错误
		{
			g_CANErrFlag.Byte0.bits.CAN_ErrorCode_FormErr_Flag = 0x01;
		}
		if(Last_Error == 0x3) //确认(ACK)错误
		{
			g_CANErrFlag.Byte0.bits.CAN_ErrorCode_ACKErr_Flag = 0x01;
		}
		if(Last_Error == 0x4) //隐性位错误
		{
			g_CANErrFlag.Byte0.bits.CAN_ErrorCode_BitRecessiveErr_Flag = 0x01;
		}
		if(Last_Error == 0x5) //显性位错误
		{
			g_CANErrFlag.Byte0.bits.CAN_ErrorCode_BitDominantErr_Flag = 0x01;
		}
		if(Last_Error == 0x6) //CRC错误
		{
			g_CANErrFlag.Byte0.bits.CAN_ErrorCode_CRCErr_Flag = 0x01;
		}
		CAN_ClearITPendingBit(CAN1, CAN_IT_LEC);
	}

	/*这里还不确定什么时候能够调用*/
	//if(CAN_GetITStatus(CAN1, CAN_IT_ERR) == SET) 	   //错误中断
	//{
	//	CanModeInit(CAN_SJW_1tq, CAN_BS2_2tq, CAN_BS1_3tq, 24, CAN_Mode_Normal); //CAN节点重启
	//	CAN_ClearITPendingBit(CAN1, CAN_IT_ERR);
	//}
}


void CANErrPrintf(void)
{
	if(g_CANErrFlag.Byte1.bits.CAN_ErrorWarning_Flag == 0x01)
	{
		g_CANErrFlag.Byte1.bits.CAN_ErrorWarning_Flag = 0;
		printf("CAN_IT_EWG\r\n");	
	}

	if(g_CANErrFlag.Byte1.bits.CAN_ErrorPassive_Flag == 0x01)
	{
		g_CANErrFlag.Byte1.bits.CAN_ErrorPassive_Flag  = 0;
		printf("CAN_IT_EPV\r\n"); 	 
	}

	if(g_CANErrFlag.Byte1.bits.CAN_ErrorBusoff_Flag == 0x01)
	{
		g_CANErrFlag.Byte1.bits.CAN_ErrorBusoff_Flag = 0;
		
		CanModeInit(CAN_SJW_1tq, CAN_BS2_2tq, CAN_BS1_3tq, 24, CAN_Mode_Normal);//CAN节点重启
		printf("busoff\r\n"); 
	}


	if(g_CANErrFlag.Byte0.bits.CAN_ErrorCode_StuffErr_Flag == 0x01) //位填充错误
	{
		g_CANErrFlag.Byte0.bits.CAN_ErrorCode_StuffErr_Flag = 0;
		printf("CAN_ErrorCode_StuffErr_Flag\r\n");
	}
	if(g_CANErrFlag.Byte0.bits.CAN_ErrorCode_FormErr_Flag == 0x01) //格式错误
	{
		g_CANErrFlag.Byte0.bits.CAN_ErrorCode_FormErr_Flag = 0;
		printf("CAN_ErrorCode_FormErr\r\n");
	}
	if(g_CANErrFlag.Byte0.bits.CAN_ErrorCode_ACKErr_Flag == 0x01) //确认(ACK)错误
	{
		g_CANErrFlag.Byte0.bits.CAN_ErrorCode_ACKErr_Flag = 0;
		printf("CAN_ErrorCode_ACKErr\r\n");
	}
	if(g_CANErrFlag.Byte0.bits.CAN_ErrorCode_BitRecessiveErr_Flag == 0x01) //隐性位错误
	{
		g_CANErrFlag.Byte0.bits.CAN_ErrorCode_BitRecessiveErr_Flag = 0;
		printf("CAN_ErrorCode_BitRecessiveErr\r\n");
	}
	if(g_CANErrFlag.Byte0.bits.CAN_ErrorCode_BitDominantErr_Flag == 0x01) //显性位错误
	{
		g_CANErrFlag.Byte0.bits.CAN_ErrorCode_BitDominantErr_Flag = 0;
		printf("CAN_ErrorCode_BitDominantErr\r\n");
	}
	if(g_CANErrFlag.Byte0.bits.CAN_ErrorCode_CRCErr_Flag == 0x01) //CRC错误
	{
		g_CANErrFlag.Byte0.bits.CAN_ErrorCode_CRCErr_Flag = 0;
		printf("CAN_ErrorCode_CRCErr\r\n");
	}


}
