#include "sys.h"
#include "usart2.h"	  


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
 
	  
UARTData gUARTData[USART_REC_BUF] = {{0},0};	  
  
void uart2_init(u32 bound)
{
  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
     
    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART2, ENABLE);                    //使能串口1 

}

/*N720 AT指令接收*/
#define ATCMDSIZEOF(A) (sizeof(A)/sizeof(A[0]))

STRUCT_InitRECVFlag g_N720InitRecvFlag = {0};
STRUCT_TCPInitFlag g_N720TCPInitFlag = {0};

u8 const ATCMDATIHeadBuf[]= {"NEOWAY\r\nN720\r\n"};
u8 const ATCMDATCCIDHeadBuf[] = {"+CCID:"};
u8 const ATCMDATCPINHeadBuf[] = {"+CPIN:"};		
u8 const ATCMDATCSQHeadBuf[] = {"+CSQ:"};
u8 const ATCMDATCREGHeadBuf[] = {"+CREG:"};
u8 const ATCMDATCGATTHeadBuf[] = {"+CGATT:"};
u8 const ATCMDATMYSYSINFOHeadBuf[] = {"$MYSYSINFO:"};

u8 const ATCMDXIICHeadBuf[] = {"+XIIC:    "};
u8 const ATCMDTCPCLOSEHeadBuf[] = {"+TCPCLOSE:"};
u8 const ATCMDTCPSETUPHeadBuf[] = {"+TCPSETUP:"};
u8 const ATCMDTCPACKHeadBuf[] = {"+TCPACK:"};
u8 const ATCMDTCPSENDHeadBuf[] = {">"};
u8 const ATCMDTCPSENDDataHeadBuf[] = {"\r\nOK\r\n+TCPSEND:"};
u8 const ATCMDTCPSENDDataHeadBuf1[] = {"\r\nOK\r\n\r\n+TCPSEND:"};

u8 const ATCMDCGDCONTHeadBuf[] = {"AT+CGDCONT="};
u8 const ATCMDTailBuf[] = {"\r\nOK\r\n"};
u8 const ATCMDTCPTailBuf[] = {"\r\r\nOK\r\n"};
u8 const ATCMDRNTailBuf[] = {"\r\n"};
u8 const ATCMDRecvHeadBuf[] = {"+TCPRECV:"};
static u8 ATCMDHeadCount = 0;
static u8 ATCMDTailCount = 0;	
static u8 ATRecvStep = 0;
static u8 ATDataCount = 0;
STRUCT_N720_INFO gN720Info = {0};

static u8 ATRecvDataStep = 0;
static u8 ATRecvHeadCount = 0;
static u8 ATRecvTailCount = 0;
static u8 ATRecvDataCount = 0;

//static u8 ATRecvStatusStep = 0;
static u8 ATRecvStatusHeadCount = 0;
//static u8 ATRecvStatusTailCount = 0;
u8 const ATRecvStatusBuf[] = {"+TCPSEND: 1,OPERATION EXPIRED"};
u8 const ATRecvStatusBuf1[] = {"+TCPSEND: 1,FAIL"};
void N720InitRecvData(u8 Res)
{
	/*4G模块初始化,接收到发送AT命令后的回复*/
	if(g_N720InitRecvFlag.bits.bN720InitFinish == 0)
	{
		switch(gN720InitStep)
		{
			case  N720SendAT:
	        {
		        if(ATCMDTCPTailBuf[ATCMDTailCount] == Res)
		        {
		            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTCPTailBuf)-2))
		            {
						//printf("44:%d\r\n",(sizeof(ATCMDTailBuf)/sizeof(ATCMDTailBuf[0])-2));
		                ATCMDTailCount = 0;
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
//								printf("n");
							}
						}
						else
						{
							ATCMDHeadCount = 0;
//							printf("%x",Res);
						}
						break;
					case 1:
						if(ATCMDTailBuf[ATCMDTailCount] == Res)
						{
							//ATDataCount = 0;
							ATRecvStep = 2;
//							printf("x");
						}
						else
						{
							gN720Info.SoftwareVerBuf[ATDataCount++] = Res;
//							printf("%2x",Res);
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
//								printf("22\r\n");
							}
//							printf("33\r\n");
						}
						else
						{
							ATRecvStep = 0;
							ATCMDTailCount = 0;
							ATCMDHeadCount = 0;
							ATDataCount = 0;
//							printf("44\r\n");
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

void N720TCPInitRecvData(u8 Res)
{
    if(g_N720InitRecvFlag.bits.bN720InitFinish == 1)
    {
        switch (gN720TCPInitStep)
        {      
            case N720SendTCPCGDCONT:
            #if 0
                switch(ATRecvStep)
                {
                    case 0:
                        if(ATCMDCGDCONTHeadBuf[ATCMDHeadCount] == Res)
                        {
                            if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDCGDCONTHeadBuf)-2))
                            {
                                ATCMDHeadCount = 0;
                                ATRecvStep = 1;
                                //printf("11");
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
                        if(ATCMDTCPTailBuf[ATCMDTailCount] == Res)
                        {
                            ATRecvStep = 2;
                            //printf("22");
                        }
                        else
                        {
                            gN720Info.TCPCGDCONT[ATDataCount++] = Res;
                        }
                        
                        break;
                    case 2:
                        
                        if(ATCMDTCPTailBuf[++ATCMDTailCount] == Res)
                        {
                            //printf("33");
                            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
                            {
                                g_N720TCPInitFlag.bits.bN720RecvATCGDCONTInfoFlag = 1;

                               // printf("44");
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
                        }
                        break;
                    default:
                        break;
                }    
            break;
            #endif
            case N720SendTCPXIIC:
            case N720SendTCPSETUP:
            case N720SendTCPXGAUTH:
            {
//                    printf("333\r\n");
                if(ATCMDTCPTailBuf[ATCMDTailCount] == Res)
                {
                    //printf("1:%d\r\n",ATCMDTailCount);
                    if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTCPTailBuf)-2))
                    {
                        //printf("1111\r\n");
                        ATCMDTailCount = 0;
                        if(gN720TCPInitStep == N720SendTCPCGDCONT)
                        {
                            g_N720TCPInitFlag.bits.bN720RecvATCGDCONTInfoFlag = 1;
                        }
                        else if(gN720TCPInitStep == N720SendTCPXIIC)
                        {
                            g_N720TCPInitFlag.bits.bN720RecvATXIICInfoFlag = 1;
                        }
                        else if(gN720TCPInitStep == N720SendTCPSETUP)
                        {
                            g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPInfoFlag = 1;
                        }
                        else if(gN720TCPInitStep == N720SendTCPXGAUTH)
                        {
                            g_N720TCPInitFlag.bits.bN720RecvATTCPXGAUTHInfoFlag = 1;
                        }
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
            case N720SendTCPXIIC1:
                switch(ATRecvStep)
                {
                    case 0:
                        if(ATCMDXIICHeadBuf[ATCMDHeadCount] == Res)
                        {
                            if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDXIICHeadBuf)-2))
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
                            gN720Info.TCPXIIC[ATDataCount++] = Res;
                        }
                        
                        break;
                    case 2:
                        
                        if(ATCMDTailBuf[++ATCMDTailCount] == Res)
                        {
                            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
                            {
                                g_N720TCPInitFlag.bits.bN720RecvATXIIC1InfoFlag = 1;
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
                        }
                        break;
                    default:
                        break;
                }    
            break;
            case N720SendTCPCLOSE:
                
                g_N720TCPInitFlag.bits.bN720RecvATTCPCLOSEInfoFlag = 1;
        #if 0
                switch(ATRecvStep)
                {
                    case 0:
                        if(ATCMDTCPCLOSEHeadBuf[ATCMDHeadCount] == Res)
                        {
                            if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDTCPCLOSEHeadBuf)-2))
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
                            gN720Info.TCPCLOSE[ATDataCount++] = Res;
                        }
                        
                        break;
                    case 2:
                        
                        if(ATCMDTailBuf[++ATCMDTailCount] == Res)
                        {
                            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDTailBuf)-2))
                            {
                                g_N720TCPInitFlag.bits.bN720RecvATTCPCLOSEInfoFlag = 1;
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
                        }
                        break;
                    default:
                        break;
                }
            #endif
                break;
            case N720SendTCPSETUPING:
                switch(ATRecvStep)
                {
                    case 0:
                        if(ATCMDTCPSETUPHeadBuf[ATCMDHeadCount] == Res)
                        {
                            if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDTCPSETUPHeadBuf)-2))
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
                        if(ATCMDRNTailBuf[ATCMDTailCount] == Res)
                        {
                            ATRecvStep = 2;
                        }
                        else
                        {
                            gN720Info.TCPSETUP[ATDataCount++] = Res;
                        }
                        
                        break;
                    case 2:                         
                        if(ATCMDRNTailBuf[++ATCMDTailCount] == Res)
                        {
                            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDRNTailBuf)-2))
                            {
                                g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPINGInfoFlag = 1;
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
                        }
                        break;
                    default:
                        break;
                }
            break;
            case N720SendTCPACK:
                switch(ATRecvStep)
                {
                    case 0:
                        if(ATCMDTCPACKHeadBuf[ATCMDHeadCount] == Res)
                        {
                            if(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDTCPACKHeadBuf)-2))
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
                        if(ATCMDRNTailBuf[ATCMDTailCount] == Res)
                        {
                            ATRecvStep = 2;
                        }
                        else
                        {
                            gN720Info.TCPACK[ATDataCount++] = Res;
                        }
                        
                        break;
                    case 2:                         
                        if(ATCMDRNTailBuf[++ATCMDTailCount] == Res)
                        {
                            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDRNTailBuf)-2))
                            {
                                g_N720TCPInitFlag.bits.bN720RecvATTCPACKInfoFlag = 1;
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
                        }
                        break;
                    default:
                        break;
                }
            break;
            case N720SendTCPSEND:
                if('>' == Res)
                {
                    g_N720TCPInitFlag.bits.bN720RecvATTCPSENDInfoFlag = 1;
                }
            case N720TCPInitFinish:
                switch(ATRecvStep)
                {
                    case 0:
                        if((ATCMDTCPSENDDataHeadBuf[ATCMDHeadCount] == Res)
                            ||(ATCMDTCPSENDDataHeadBuf1[ATCMDHeadCount] == Res))
                        {
                            if((ATCMDHeadCount == (ATCMDSIZEOF(ATCMDTCPSENDDataHeadBuf)-2))
                                ||(ATCMDHeadCount == (ATCMDSIZEOF(ATCMDTCPSENDDataHeadBuf1)-2)))
                            {
                                //printf("AA");
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
                        //printf("BB");
                        if(ATCMDRNTailBuf[ATCMDTailCount] == Res)
                        {
                            ATRecvStep = 2;
                        }
                        else
                        {
                            gN720Info.TCPFinish[ATDataCount++] = Res;
                        }
                        
                        break;
                    case 2:                         
                        //printf("CC");
                        if(ATCMDRNTailBuf[++ATCMDTailCount] == Res)
                        {
                            if(ATCMDTailCount == (ATCMDSIZEOF(ATCMDRNTailBuf)-2))
                            {
                                g_N720TCPInitFlag.bits.bN720SendATSendDataSuccessCommandFlag = 1;
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
                        }
                        break;
                    default:
                        break;
                }
            break;
        }
    }

}

void N720RecvCANData(u8 Res)
{
    switch(ATRecvDataStep)
    {
        case 0:
            if(ATCMDRecvHeadBuf[ATRecvHeadCount] == Res)
            {
                if(ATRecvHeadCount == (ATCMDSIZEOF(ATCMDRecvHeadBuf)-2))
                {
                    ATRecvHeadCount = 0;
                    ATRecvDataStep = 1;
                }
                else
                {
                    ATRecvHeadCount++;
//                              printf("n");
                }
            }
            else
            {
                ATRecvHeadCount = 0;
//                          printf("%x",Res);
            }
            break;
        case 1:
            if(ATCMDRNTailBuf[ATRecvTailCount] == Res)
            {
                //ATDataCount = 0;
                ATRecvDataStep = 2;
//                          printf("x");
            }
            else
            {
                gN720Info.TCPRecvCANData[ATRecvDataCount++] = Res;
//                          printf("%2x",Res);
            }
            
            break;
        case 2:
            //printf("bb:%x\r\n",Res);
            
            if(ATCMDRNTailBuf[++ATRecvTailCount] == Res)
            {
                if(ATRecvTailCount == (ATCMDSIZEOF(ATCMDRNTailBuf)-2))
                {
                    g_N720InitRecvFlag.bits.bN720RecvCANDataFlag = 1;
                    ATRecvDataStep = 0;
                    ATRecvTailCount = 0;
                    ATRecvHeadCount = 0;
                    ATRecvDataCount = 0;
                    return ;
//                              printf("22\r\n");
                }
//                          printf("33\r\n");
            }
            else
            {
                ATRecvDataStep = 0;
                ATRecvTailCount = 0;
                ATRecvHeadCount = 0;
                ATRecvDataCount = 0;
//                          printf("44\r\n");
            }
            break;
        default:
            break;
    }

}

void N720RecvStatus(u8 Res)
{
    if((ATRecvStatusBuf[ATRecvStatusHeadCount] == Res)||(ATRecvStatusBuf1[ATRecvStatusHeadCount] == Res))
    {
        if((ATRecvStatusHeadCount == (ATCMDSIZEOF(ATRecvStatusBuf)-2))
            ||(ATRecvStatusHeadCount == (ATCMDSIZEOF(ATRecvStatusBuf1)-2)))
        {
            ATRecvStatusHeadCount = 0;
            printf("zzz\r\n");
            
            gN720TCPInitStep = N720SendTCPCGDCONT;
            g_N720TCPInitFlag.bits.bN720SendACKFinishFlag = 0;
            g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag = 0;
            //g_N720TCPInitTIMFlag.bits.bN720SendATTCPACKCommandFlag = 0;
            //g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag = 0;
        }
        else
        {
            ATRecvStatusHeadCount++;
        }
    }
    else
    {
        ATRecvStatusHeadCount = 0;
    }       
}

void USART2_IRQHandler(void)                	//串口1中断服务程序
{
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
    	Res = USART_ReceiveData(USART2);	//读取接收到的数据
        N720RecvCANData(Res);
        N720InitRecvData(Res);
        N720TCPInitRecvData(Res);
        N720RecvStatus(Res);
    }
} 


void USART2_Send_String(u8 *buf)
{
	//printf("   buf:0x%x,0x%x  ",buf[0],buf[1]);
	u16 Len = 0;
	while(1)
	{
		//printf("11\r\n");
        //USART_SendData(USART1,(u16)buf[Len]);
#if 1
		USART_SendData(USART2,(u16)buf[Len]);
		Len++;
		if(buf[Len] == '\0')
			return;
        #else
		USART_SendData(USART2,40);
        #endif
	}
}

void USART2_Send_CANData(u8 *buf)
{
	u8 Len = 0;
	while(1)
	{
		USART_SendData(USART2,(u16)buf[Len]);
		Len++;
		if(Len == USART_SEND_LEN)
			return;
	}
}
