#include "UartDeal_task.h"

const char CPINResponeBuf[] = {"READY"};
const char TCPACKBuf[] = {"1,OK"};

const char ICCIDCMCC[] = {"898600"};//移动
const char ICCIDCMCC1[] = {"898602"};//移动
const char ICCIDCMCC2[] = {"898604"};//移动
const char ICCIDCMCC3[] = {"898607"};//移动

const char ICCIDCUCC[] = {"898601"};//联通
const char ICCIDCUCC1[] = {"898606"};//联通
const char ICCIDCUCC2[] = {"898609"};//联通

const char ICCIDCTCC[] = {"898603"};//电信
const char ICCIDCTCC1[] = {"898611"};//电信

static u16 DataResendCount = 0;

static u8 printfRecvData[757] = {0};
SIMCARDTYPE SimType = CMCC;

/**********************************
 *函数名：void UartDeal_task(void)
 *函数功能：接收串口数据处理任务
 *参数:None
 *返回值:none
***********************************/
void UartDeal_task(void)
{
	u16 i = 0;
    
    char N720CSQX[2] = {0},N720CSQY[2] = {0};
    u8 N720CSQXValue = 0,N720CSQYValue = 0;

    static u8 TCPSetupCount = 0;
#if 0
	static u8 UartDealIndex = 0;

	if(gUARTData[UartDealIndex].USART_RX_Flag == 1)
	{
		gUARTData[UartDealIndex].USART_RX_Flag = 0;
		//USART_SendData(USART1,gUARTData[UartDealIndex].USART_RX_BUF);
	}
	UartDealIndex++;
	if(UartDealIndex == USART_REC_BUF)
	{
		UartDealIndex = 0;
	}
	#endif

	if(g_N720InitRecvFlag.bits.bN720RecvATInfoFlag == 1)
	{
		gN720InitStep = N720SendATI;
		g_N720InitRecvFlag.bits.bN720RecvATInfoFlag = 0;
	}
	
	if(g_N720InitRecvFlag.bits.bN720RecvATIInfoFlag == 1)
	{
		
		printf("ver:");
		for(i = 0;i < sizeof(gN720Info.SoftwareVerBuf);i++)
		{
			printf("%c",gN720Info.SoftwareVerBuf[i]);
		}
		printf("\r\n");
		
		gN720InitStep = N720SendATCCID;
        N720TCPInitCount = 0;
		g_N720InitRecvFlag.bits.bN720RecvATIInfoFlag = 0;
	}

	if(g_N720InitRecvFlag.bits.bN720RecvATCCIDInfoFlag == 1)
	{
		printf("CCID:");
		for(i = 0;i < sizeof(gN720Info.CCIDBuf);i++)
		{
			printf("%c",gN720Info.CCIDBuf[i]);
		}
		printf("\r\n");

        if((strstr((const char*)gN720Info.CCIDBuf,ICCIDCMCC) != NULL)
            ||(strstr((const char*)gN720Info.CCIDBuf,ICCIDCMCC1) != NULL)
            ||(strstr((const char*)gN720Info.CCIDBuf,ICCIDCMCC2) != NULL)
            ||(strstr((const char*)gN720Info.CCIDBuf,ICCIDCMCC3) != NULL))
        {
            SimType = CMCC;
            printf("CMCC\r\n");
        }
        else if((strstr((const char*)gN720Info.CCIDBuf,ICCIDCUCC) != NULL)
            ||(strstr((const char*)gN720Info.CCIDBuf,ICCIDCUCC1) != NULL)
            ||(strstr((const char*)gN720Info.CCIDBuf,ICCIDCUCC2) != NULL))
        {
            SimType = CUCC;
            printf("CUCC\r\n");
        }
        else if((strstr((const char*)gN720Info.CCIDBuf,ICCIDCTCC) != NULL)
            ||(strstr((const char*)gN720Info.CCIDBuf,ICCIDCTCC1) != NULL))
        {
            SimType = CTCC;
            printf("CTCC\r\n");
        }

		gN720InitStep = N720SendATCPIN;		
        N720TCPInitCount = 0;
		g_N720InitRecvFlag.bits.bN720RecvATCCIDInfoFlag = 0;
	}

	
	if(g_N720InitRecvFlag.bits.bN720RecvATCPINInfoFlag == 1)
	{
		printf("CPIN:");
		for(i = 0;i < sizeof(gN720Info.CPINBuf);i++)
		{
			printf("%c",gN720Info.CPINBuf[i]);
		}
		printf("\r\n");

		if(strstr((const char*)gN720Info.CPINBuf,CPINResponeBuf) != NULL)
		{
			gN720InitStep = N720SendATCSQ;
            N720TCPInitCount = 0;
			g_N720InitRecvFlag.bits.bN720RecvATCPINInfoFlag = 0;
		}
	}

    if(g_N720InitRecvFlag.bits.bN720RecvATCSQInfoFlag == 1)
	{
		printf("CSQ:");
		for(i = 0;i < sizeof(gN720Info.CSQBuf);i++)
		{
			printf("%c",gN720Info.CSQBuf[i]);
		}
		printf("\r\n");
        memcpy(N720CSQX, gN720Info.CSQBuf + 1, 2);
        N720CSQXValue = atoi(N720CSQX);
        memcpy(N720CSQY, gN720Info.CSQBuf + 4, 2);
        N720CSQYValue = atoi(N720CSQY);
        memset(gN720Info.CSQBuf,0,sizeof(gN720Info.CSQBuf)/sizeof(gN720Info.CSQBuf[0]));
        printf("sizeof:%d\r\n",sizeof(gN720Info.CSQBuf)/sizeof(gN720Info.CSQBuf[0]));
        if( g_N720TCPInitFlag.bits.bN720SendACKFinishFlag == 0)
        {
            if((N720CSQXValue <= 31) && (N720CSQXValue >= 12))
            {
                gN720InitStep = N720SendATCREG;
                CMDFailedCount = 0;
                N720TCPInitCount = 0;
            }
        }
		g_N720InitRecvFlag.bits.bN720RecvATCSQInfoFlag = 0;
	}

    if(g_N720InitRecvFlag.bits.bN720RecvATCREGInfoFlag == 1)
	{
		printf("CREG:");
		for(i = 0;i < sizeof(gN720Info.CREGBuf);i++)
		{
			printf("%c",gN720Info.CREGBuf[i]);
		}
		printf("\r\n");
		if((gN720Info.CREGBuf[3] == '1') ||(gN720Info.CREGBuf[3] == '5'))	
		{
		    gN720InitStep = N720SendATCGATT;
            CMDFailedCount = 0;
            N720TCPInitCount = 0;
        }
		g_N720InitRecvFlag.bits.bN720RecvATCREGInfoFlag = 0;
	}

    if(g_N720InitRecvFlag.bits.bN720RecvATCGATTInfoFlag == 1)
	{
		printf("CGATT:");
		for(i = 0;i < sizeof(gN720Info.CGATTBuf);i++)
		{
			printf("%c",gN720Info.CGATTBuf[i]);
		}
		printf("\r\n");
		if(gN720Info.CGATTBuf[1] == '1')
		{
		    gN720InitStep = N720SendATMYSYSINFO;
            CMDFailedCount = 0;
            N720TCPInitCount = 0;
        }
		g_N720InitRecvFlag.bits.bN720RecvATCGATTInfoFlag = 0;
	}

    if(g_N720InitRecvFlag.bits.bN720RecvATMYSYSINFOInfoFlag == 1)
	{
		printf("MYSYSINFO:");
		for(i = 0;i < sizeof(gN720Info.MYSYSINFOBuf);i++)
		{
			printf("%c",gN720Info.MYSYSINFOBuf[i]);
		}
		printf("\r\n");
			
		gN720InitStep = N720InitFinish;
        N720TCPInitCount = 0;
        gN720TCPInitStep = N720SendTCPCGDCONT;
		g_N720InitRecvFlag.bits.bN720RecvATMYSYSINFOInfoFlag = 0;
		g_N720InitRecvFlag.bits.bN720InitFinish = 1;
	}

	if(g_N720TCPInitFlag.bits.bN720RecvATCGDCONTInfoFlag == 1)
	{
	    printf("recv cgdcont ok\r\n");
		//for(i = 0;i < sizeof(gN720Info.TCPCGDCONT);i++)
		//{
		//	printf("%c",gN720Info.TCPCGDCONT[i]);
		//}
		//printf("\r\n");
		gN720TCPInitStep = N720SendTCPXGAUTH;
        N720TCPInitCount = 0;
		g_N720TCPInitFlag.bits.bN720RecvATCGDCONTInfoFlag = 0;
	}

    if(g_N720TCPInitFlag.bits.bN720RecvATTCPXGAUTHInfoFlag == 1)
    {        
        printf("recv XGUATH ok\r\n");
        
		gN720TCPInitStep = N720SendTCPXIIC;
        N720TCPInitCount = 0;
        g_N720TCPInitFlag.bits.bN720RecvATTCPXGAUTHInfoFlag = 0;
    }
    
	if(g_N720TCPInitFlag.bits.bN720RecvATXIICInfoFlag == 1)
	{
	    printf("recv xiic\r\n");
		gN720TCPInitStep = N720SendTCPXIIC1;
        N720TCPInitCount = 0;
		g_N720TCPInitFlag.bits.bN720RecvATXIICInfoFlag = 0;
        
        CMDFailedCount = 0;
        ReconnectCount = 0;
	}
    
	if(g_N720TCPInitFlag.bits.bN720RecvATXIIC1InfoFlag == 1)
	{
	    printf("xiic?:");
		for(i = 0;i < sizeof(gN720Info.TCPXIIC);i++)
		{
			printf("%c",gN720Info.TCPXIIC[i]);
		}
		printf("\r\n");
        if((gN720Info.TCPXIIC[0] != '0') &&(gN720Info.TCPXIIC[2] != '0'))
		{
		    gN720TCPInitStep = N720SendTCPCLOSE;
            N720TCPInitCount = 0;
        }
		g_N720TCPInitFlag.bits.bN720RecvATXIIC1InfoFlag = 0;
	}
    
	if(g_N720TCPInitFlag.bits.bN720RecvATTCPCLOSEInfoFlag == 1)
	{
	    printf("TCPCLOSE:");
		for(i = 0;i < sizeof(gN720Info.TCPCLOSE);i++)
		{
			printf("%c",gN720Info.TCPCLOSE[i]);
		}
		printf("\r\n");
		gN720TCPInitStep = N720SendTCPSETUP;
        N720TCPInitCount = 0;
		g_N720TCPInitFlag.bits.bN720RecvATTCPCLOSEInfoFlag = 0;
	}

	if( g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPInfoFlag == 1)
	{
	    printf("TCPSETUP OK\r\n");

		gN720TCPInitStep = N720SendTCPSETUPING;
        N720TCPInitCount = 0;
		g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPInfoFlag = 0;
	}    

	if( g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPINGInfoFlag == 1)
	{
	    printf("TCPSETUP:");
		for(i = 0;i < sizeof(gN720Info.TCPSETUP);i++)
		{
			printf("%c",gN720Info.TCPSETUP[i]);
		}
		printf("\r\n");

        /*判断返回值是否为“OK”：
            如果返回值有“OK”，则进入下一步，发送TCPACK,如果返回"fail"则重新发送TCPSETUP;
            TCPSETUP发送3次错误后就重启
        */
		if(strstr((const char*)gN720Info.TCPSETUP,TCPACKBuf) != NULL)
		{
		    gN720TCPInitStep = N720SendTCPACK;
        }
        else
        {
            TCPSetupCount++;
            if(TCPSetupCount == 3)
            {
                TCPSetupCount = 0;
                N720PowerkeyReset();
                return ;
            }
    		gN720TCPInitStep = N720SendTCPSETUP;
            
        }
        N720TCPInitCount = 0;
        g_N720TCPInitFlag.bits.bN720RecvATTCPSETUPINGInfoFlag = 0;
	}    

    if(g_N720TCPInitFlag.bits.bN720RecvATTCPACKInfoFlag == 1)
    {
        printf("TCPACK:");
        for(i = 0;i < sizeof(gN720Info.TCPACK);i++)
        {
            printf("%c",gN720Info.TCPACK[i]);
        }
        printf("\r\n");

        gN720TCPInitStep = N720WaitRecvData;
        N720TCPInitCount = 0;
        g_N720TCPInitFlag.bits.bN720RecvATTCPACKInfoFlag = 0;
        g_N720TCPInitFlag.bits.bN720SendACKFinishFlag = 1;
        
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag = 0;
    }

    if(g_N720TCPInitFlag.bits.bN720RecvATTCPSENDInfoFlag == 1)
    {
        //printf("start send data\r\n");
        g_N720TCPInitTIMFlag.bits.bN720SendATPrepareSendCommandFlag = 1;
        g_N720TCPInitFlag.bits.bN720RecvATTCPSENDInfoFlag = 0;
        
        //gN720TCPInitStep = N720SendTCPSTARTSEND;
    }

    if((g_N720TCPInitFlag.bits.bN720SendATSendDataSuccessCommandFlag == 1)
        /*&&(g_N720InitRecvFlag.bits.bN720RecvCANDataFlag == 1)*/)
    {
        
        printf("Send data finish:");
        #if 0
        for(i = 0;i < sizeof(gN720Info.TCPFinish);i++)
        {
            printf("%c",gN720Info.TCPFinish[i]);
        }
        #endif
        printf("\r\n");
        g_N720TCPInitFlag.bits.bN720SendATSendDataSuccessCommandFlag = 0;

        
        g_N720TCPInitTIMFlag.bits.bN720SendATDataNoResponseFlag = 0;
        DataResendCount = 0;
        
        //gN720TCPInitStep = N720SendTCPSEND;
        g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag = 0;
        
        g_N720TCPInitFlag.bits.bN720SendFinishFlag = 1;
        //g_N720InitRecvFlag.bits.bN720RecvCANDataFlag  = 0;
    }
#if 1
    if( g_N720InitRecvFlag.bits.bN720RecvCANDataFlag == 1)
    {
        memcpy(printfRecvData,gTCPRecvCANData,sizeof(gTCPRecvCANData));
        memset(gTCPRecvCANData,0,sizeof(gTCPRecvCANData));
        g_N720InitRecvFlag.bits.bN720RecvCANDataFlag = 0;
    #if 1
        printf("recv data:\r\n");
        #if 0
        for(i = 0;i < sizeof(gN720Info.TCPRecvCANData);i++)
        {
            //printf("%c",gN720Info.TCPRecvCANData[i]);
            printf("%c",gTCPRecvCANData[i]);
        }
        #else
        for(i = 0;i < sizeof(printfRecvData);i++)
        {
            //printf("%c",gN720Info.TCPRecvCANData[i]);
            printf("%c",printfRecvData[i]);
        }

        #endif
        printf("\r\n");
    #endif
    }
#endif
}

void SendDataNoResponseTimerHandler(void)
{
    if(g_N720TCPInitTIMFlag.bits.bN720SendATDataNoResponseFlag == 1)
    {
        DataResendCount++;
        if(DataResendCount == 10000)
        {
            printf("xxx\r\n");
            DataResendCount = 0;
            g_N720TCPInitTIMFlag.bits.bN720SendATDataNoResponseFlag = 0;
//            g_N720TCPInitTIMFlag.bits.bN720SendATTCPSENDCommandFlag = 0;
            g_N720TCPInitTIMFlag.bits.bN720SendATTCPCLOSECommandFlag = 0;
            g_N720TCPInitFlag.bits.bN720SendACKFinishFlag = 0;
            
            gN720TCPInitStep = N720SendTCPCLOSE;
        }
    }
}
