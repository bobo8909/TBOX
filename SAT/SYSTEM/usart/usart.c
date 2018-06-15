#include "sys.h"
#include "usart.h"	  

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
	  
UARTData gUARTData[USART_REC_BUF] = {{0},0};	  
  
void uart_init(u32 bound)
{
  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
     
    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

/*N720 AT指令接收*/
#define ATCMDSIZEOF(A) (sizeof(A)/sizeof(A[0]))

STRUCT_InitRECVFlag g_N720InitRecvFlag = {0};


u8 const ATCMDATIHeadBuf[]= {"\r\nNEOWAY\r\nN720\r\n"};
u8 const ATCMDATCCIDHeadBuf[] = {"\r\n+CCID:"};
u8 const ATCMDATCPINHeadBuf[] = {"+CPIN:\r\nOK\r\n"};		
u8 const ATCMDATCSQHeadBuf[] = {"+CSQ:"};
u8 const ATCMDATCREGHeadBuf[] = {"+CREG:"};
u8 const ATCMDATCGATTHeadBuf[] = {"+CGATT:"};
u8 const ATCMDATMYSYSINFOHeadBuf[] = {"\r\n$MYSYSINFO:"};

u8 const ATCMDTailBuf[] = {"\r\nOK\r\n"};
	
static u8 ATCMDHeadCount = 0;
static u8 ATCMDTailCount = 0;	
static u8 ATRecvStep = 0;
static u8 ATDataCount = 0;
STRUCT_N720_INFO gN720Info = {0};

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART1);	//读取接收到的数据

		
		/*4G模块初始化,接收到发送AT命令后的回复*/
		switch(gN720InitStep)
		{
			case  N720SendAT:
	        {
		        if(ATCMDTailBuf[ATCMDTailCount] == Res)
		        {
		            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
		            {
						//printf("44:%d\r\n",(sizeof(ATCMDTailBuf)/sizeof(ATCMDTailBuf[0])-2));
		                ATCMDTailCount = 0;
						//gN720InitStep = N720SendATI;
						g_N720InitRecvFlag.bits.bN720RecvATInfoFlag = 1;
						return ;
		            }
		            else
		            {
		                ATCMDTailCount++;
		            }
		        }
		        else
		        {
		            ATCMDTailCount = 0;
		        }
			}
			break;
			
			case N720SendATI:
			{
				switch(ATRecvStep)
				{
					case 0:
						if(ATCMDATIHeadBuf[ATCMDHeadCount] == Res)
						{
							if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDATIHeadBuf)-2))
							{
								ATCMDHeadCount = 0;
								ATRecvStep = 1;
							}
							else
							{
								ATCMDHeadCount++;
							}
						}
						else
						{
							ATCMDHeadCount = 0;
						}
						break;
					case 1:
						if(ATCMDTailBuf[ATCMDTailCount] == Res)
						{
							//ATDataCount = 0;
							ATRecvStep = 2;
							//printf("aa:%x\r\n",Res);
						}
						else
						{
							gN720Info.SoftwareVerBuf[ATDataCount++] = Res;
							//printf("%2x",Res);
						}
						
						break;
					case 2:
						//printf("bb:%x\r\n",Res);
						
						if(ATCMDTailBuf[++ATCMDTailCount] == Res)
						{
							if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
							{
								g_N720InitRecvFlag.bits.bN720RecvATIInfoFlag = 1;
								ATRecvStep = 0;
								ATCMDTailCount = 0;
								ATCMDHeadCount = 0;
								ATDataCount = 0;
								//printf("22\r\n");
							}
							//printf("33\r\n");
						}
						else
						{
							ATRecvStep = 0;
							ATCMDTailCount = 0;
							ATCMDHeadCount = 0;
							ATDataCount = 0;
							//printf("44\r\n");
						}
						break;
					default:
						break;
				}
			}
			break;

			case N720SendATCCID:
			{
				switch(ATRecvStep)
				{
					case 0:
						if(ATCMDATCCIDHeadBuf[ATCMDHeadCount] == Res)
						{
							if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDATCCIDHeadBuf)-2))
							{
								ATCMDHeadCount = 0;
								ATRecvStep = 1;
							}
							else
							{
								ATCMDHeadCount++;
							}
						}
						else
						{
							ATCMDHeadCount = 0;
						}
						break;
					case 1:
						if(ATCMDTailBuf[ATCMDTailCount] == Res)
						{
							ATRecvStep = 2;
						}
						else
						{
							gN720Info.CCIDBuf[ATDataCount++] = Res;
						}
						
						break;
					case 2:
						if(ATCMDTailBuf[++ATCMDTailCount] == Res)
						{
							if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
							{
								g_N720InitRecvFlag.bits.bN720RecvATCCIDInfoFlag = 1;
								ATRecvStep = 0;
								ATCMDTailCount = 0;
								ATCMDHeadCount = 0;
								ATDataCount = 0;
							}
						}
						else
						{
							ATRecvStep = 0;
							ATCMDTailCount = 0;
							ATCMDHeadCount = 0;
							ATDataCount = 0;
							//printf("44\r\n");
						}
						break;
					default:
						break;
				}
			}
			break;
			case N720SendATCPIN:
			{
				switch(ATRecvStep)
				{
					case 0:
						if(ATCMDATCPINHeadBuf[ATCMDHeadCount] == Res)
						{
							if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDATCPINHeadBuf)-2))
							{
								ATCMDHeadCount = 0;
								ATRecvStep = 1;
							}
							else
							{
								ATCMDHeadCount++;
							}
						}
						else
						{
							ATCMDHeadCount = 0;
						}
						break;
					case 1:
						if(ATCMDTailBuf[ATCMDTailCount] == Res)
						{
							ATRecvStep = 2;
						}
						else
						{
							gN720Info.CPINBuf[ATDataCount++] = Res;
						}
						
						break;
					case 2:
						if(ATCMDTailBuf[++ATCMDTailCount] == Res)
						{
							if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
							{
								g_N720InitRecvFlag.bits.bN720RecvATCPINInfoFlag = 1;
								ATRecvStep = 0;
								ATCMDTailCount = 0;
								ATCMDHeadCount = 0;
								ATDataCount = 0;
							}
						}
						else
						{
							ATRecvStep = 0;
							ATCMDTailCount = 0;
							ATCMDHeadCount = 0;
							ATDataCount = 0;
							//printf("44\r\n");
						}
						break;
					default:
						break;
				}
			}
			break;

            case N720SendATCSQ:
            {
                switch(ATRecvStep)
                {
                    case 0:
                        if(ATCMDATCSQHeadBuf[ATCMDHeadCount] == Res)
                        {
                            if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDATCSQHeadBuf)-2))
                            {
                                ATCMDHeadCount = 0;
                                ATRecvStep = 1;
                            }
                            else
                            {
                                ATCMDHeadCount++;
                            }
                        }
                        else
                        {
                            ATCMDHeadCount = 0;
                        }
                        break;
                    case 1:
                        if(ATCMDTailBuf[ATCMDTailCount] == Res)
                        {
                            ATRecvStep = 2;
                        }
                        else
                        {
                            gN720Info.CSQBuf[ATDataCount++] = Res;
                        }
                        
                        break;
                    case 2:
                        if(ATCMDTailBuf[++ATCMDTailCount] == Res)
                        {
                            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
                            {
                                g_N720InitRecvFlag.bits.bN720RecvATCSQInfoFlag = 1;
                                ATRecvStep = 0;
                                ATCMDTailCount = 0;
                                ATCMDHeadCount = 0;
                                ATDataCount = 0;
                            }
                        }
                        else
                        {
                            ATRecvStep = 0;
                            ATCMDTailCount = 0;
                            ATCMDHeadCount = 0;
                            ATDataCount = 0;
                            //printf("44\r\n");
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
			
            case N720SendATCREG:
            {
                switch(ATRecvStep)
                {
                    case 0:
                        if(ATCMDATCREGHeadBuf[ATCMDHeadCount] == Res)
                        {
                            if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDATCREGHeadBuf)-2))
                            {
                                ATCMDHeadCount = 0;
                                ATRecvStep = 1;
                            }
                            else
                            {
                                ATCMDHeadCount++;
                            }
                        }
                        else
                        {
                            ATCMDHeadCount = 0;
                        }
                        break;
                    case 1:
                        if(ATCMDTailBuf[ATCMDTailCount] == Res)
                        {
                            ATRecvStep = 2;
                        }
                        else
                        {
                            gN720Info.CREGBuf[ATDataCount++] = Res;
                        }
                        
                        break;
                    case 2:
                        if(ATCMDTailBuf[++ATCMDTailCount] == Res)
                        {
                            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
                            {
                                g_N720InitRecvFlag.bits.bN720RecvATCREGInfoFlag = 1;
                                ATRecvStep = 0;
                                ATCMDTailCount = 0;
                                ATCMDHeadCount = 0;
                                ATDataCount = 0;
                            }
                        }
                        else
                        {
                            ATRecvStep = 0;
                            ATCMDTailCount = 0;
                            ATCMDHeadCount = 0;
                            ATDataCount = 0;
                            //printf("44\r\n");
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
    		case N720SendATCGATT:
    			{
    				switch(ATRecvStep)
    				{
    					case 0:
    						if(ATCMDATCGATTHeadBuf[ATCMDHeadCount] == Res)
    						{
    							if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDATCGATTHeadBuf)-2))
    							{
    								ATCMDHeadCount = 0;
    								ATRecvStep = 1;
    							}
    							else
    							{
    								ATCMDHeadCount++;
    							}
    						}
    						else
    						{
    							ATCMDHeadCount = 0;
    						}
    						break;
    					case 1:
    						if(ATCMDTailBuf[ATCMDTailCount] == Res)
    						{
    							ATRecvStep = 2;
    						}
    						else
    						{
    							gN720Info.CGATTBuf[ATDataCount++] = Res;
    						}
    						
    						break;
    					case 2:
    						if(ATCMDTailBuf[++ATCMDTailCount] == Res)
    						{
    							if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
    							{
    								g_N720InitRecvFlag.bits.bN720RecvATCGATTInfoFlag = 1;
    								ATRecvStep = 0;
    								ATCMDTailCount = 0;
    								ATCMDHeadCount = 0;
    								ATDataCount = 0;
    							}
    						}
    						else
    						{
    							ATRecvStep = 0;
    							ATCMDTailCount = 0;
    							ATCMDHeadCount = 0;
    							ATDataCount = 0;
    							//printf("44\r\n");
    						}
    						break;
    					default:
    						break;
    				}
    			}
    			break;

    			case N720SendATMYSYSINFO:
    				{
    					switch(ATRecvStep)
    					{
    						case 0:
    							if(ATCMDATMYSYSINFOHeadBuf[ATCMDHeadCount] == Res)
    							{
    								if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDATMYSYSINFOHeadBuf)-2))
    								{
    									ATCMDHeadCount = 0;
    									ATRecvStep = 1;
    								}
    								else
    								{
    									ATCMDHeadCount++;
    								}
    							}
    							else
    							{
    								ATCMDHeadCount = 0;
    							}
    							break;
    						case 1:
    							if(ATCMDTailBuf[ATCMDTailCount] == Res)
    							{
    								ATRecvStep = 2;
    							}
    							else
    							{
    								gN720Info.MYSYSINFOBuf[ATDataCount++] = Res;
    							}
    							
    							break;
    						case 2:
    							if(ATCMDTailBuf[++ATCMDTailCount] == Res)
    							{
    								if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
    								{
    									g_N720InitRecvFlag.bits.bN720RecvATMYSYSINFOInfoFlag = 1;
    									ATRecvStep = 0;
    									ATCMDTailCount = 0;
    									ATCMDHeadCount = 0;
    									ATDataCount = 0;
    								}
    							}
    							else
    							{
    								ATRecvStep = 0;
    								ATCMDTailCount = 0;
    								ATCMDHeadCount = 0;
    								ATDataCount = 0;
    								//printf("44\r\n");
    							}
    							break;
    						default:
    							break;
    					}
    				}
    				break;
            
    			default:
    				break;
		}
     } 
} 
#endif	


void USART_Send_String(u8 *buf)
{
	//printf("   buf:0x%x,0x%x  ",buf[0],buf[1]);
	u16 Len = 0;
	while(1)
	{
		//printf("11\r\n");
		USART_SendData(USART1,(u16)buf[Len]);
		Len++;
		if(buf[Len] == '\0')
			return;
	}
}

#if 0
void RecvATData(u8 Res,u8* ATHeadBuf,STRUCT_N720InitTIMFlag g_N720InitTIMFlag,u8 InitStep)
{
	switch(ATRecvStep)
	{
		case 0:
			if(ATHeadBuf[ATCMDHeadCount] == Res)
			{
				if(ATCMDHeadCount == (ATCMDSIZEOF(ATHeadBuf)-2))
				{
					ATCMDHeadCount = 0;
					ATRecvStep = 1;
				}
				else
				{
					ATCMDHeadCount++;
				}
			}
			else
			{
				ATCMDHeadCount = 0;
			}
			break;
		case 1:
			if(ATCMDTailBuf[ATCMDTailCount] == Res)
			{
				//ATDataCount = 0;
				ATRecvStep = 2;
				//printf("aa:%x\r\n",Res);
			}
			else
			{
				gN720Info.CCIDBuf[ATDataCount++] = Res;
				//printf("%2x",Res);
			}
			
			break;
		case 2:
			//printf("bb:%x\r\n",Res);
			
			if(ATCMDTailBuf[++ATCMDTailCount] == Res)
			{
				if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
				{
					g_N720InitTIMFlag.bits.bN720SendATCPINCommandFlag = 1;
					gN720InitStep = N720SendATCPIN;
					ATRecvStep = 0;
					ATCMDTailCount = 0;
					ATCMDHeadCount = 0;
					ATDataCount = 0;
					//printf("22\r\n");
				}
				//printf("33\r\n");
			}
			else
			{
				ATRecvStep = 0;
				ATCMDTailCount = 0;
				ATCMDHeadCount = 0;
				ATDataCount = 0;
				//printf("44\r\n");
			}
			break;
		default:
			break;
	}

}
#endif
