#include "N720_Driver.h"

/*初始化过程需要发送的AT指令*/
static u8  *ATCommandBuf[] =
			{
				"AT\r",
				"ATI\r",
				"AT+CCID\r",
				"AT+CPIN?\r",
				"AT+CSQ\r",
				"AT+CREG?\r",
				"AT+CGATT?\r",
                "AT$MYSYSINFO\r",
			};
u8 testarc[]="at+qcnversion\r";
/**********************************
*函数名：void N720PowerON(void)
*函数功能：N720模块开机
*参数:None
*返回值:none
***********************************/
void N720PowerON(void)
{
    N720POWERKEY = N720_ON;
    g_N720InitTIMFlag.bits.bN720PowerONFlag = 1;
}

/**********************************
*函数名：void N720PowerOFF(void)
*函数功能：N720模块关机
*参数:None
*返回值:none
***********************************/
void N720PowerOFF(void)
{
    N720POWERKEY = N720_ON;
    g_N720InitTIMFlag.bits.bN720PowerOFFFlag = 1;
}

/**********************************
*函数名：void N720PowerkeyReset(void)
*函数功能：N720模块重启，控制powerkey引脚
*参数:None
*返回值:none
***********************************/
void N720PowerkeyReset(void)
{
    N720PowerOFF();
    g_N720InitTIMFlag.bits.bN720PowerkeyResetFlag = 1;    
}
/**********************************
*函数名：void N720IOInit(void)
*函数功能：N720模块IO初始化
*参数:None
*返回值:none
***********************************/
void N720IOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	
 
    //modern powerkey io
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;	    		 // ???ú????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	  				 //推挽输出，IO口速度为2MHz
}


/**********************************
 *函数名：void N720Init(void)
 *函数功能：N720模块上电初始化流程
 *参数:None
 *返回值:none
***********************************/
void N720Init(void)
{
	#if 1
	if(g_N720InitTIMFlag.bits.bN720StartInitFlag == 1)
	{
		//printf("%s len:%d,%d",ATCommandBuf[0],sizeof(ATCommandBuf[0]),sizeof(ATCommandBuf[0][0]));
		//USART2_Send_String(ATCommandBuf[COMMAND_AT]);
		g_N720InitTIMFlag.bits.bN720SendATCommandFlag = 1;
		g_N720InitTIMFlag.bits.bN720StartInitFlag = 0;
	}

	if(g_N720InitTIMFlag.bits.bN720SendATCommandFlag == 1)
	{
        printf("send AT\r\n");
		USART2_Send_String(ATCommandBuf[COMMAND_AT]);
		g_N720InitTIMFlag.bits.bN720SendATCommandFlag = 0;
	}

	if(g_N720InitTIMFlag.bits.bN720SendATICommandFlag == 1)
	{
        printf("send ATI\r\n");
		USART2_Send_String(ATCommandBuf[COMMAND_ATI]);
		g_N720InitTIMFlag.bits.bN720SendATICommandFlag = 0;
	}

	if(g_N720InitTIMFlag.bits.bN720SendATCCIDCommandFlag == 1)
	{
        printf("send ATCCID\r\n");
		USART2_Send_String(ATCommandBuf[COMMAND_ATCCID]);
		g_N720InitTIMFlag.bits.bN720SendATCCIDCommandFlag = 0;
	}

	if(g_N720InitTIMFlag.bits.bN720SendATCPINCommandFlag == 1)
	{
        printf("send ATCPIN\r\n");
		USART2_Send_String(ATCommandBuf[COMMAND_ATCPIN]);
		g_N720InitTIMFlag.bits.bN720SendATCPINCommandFlag = 0;
	}
	
	if(g_N720InitTIMFlag.bits.bN720SendATCSQCommandFlag == 1)
	{
        printf("send ATCSQ\r\n");
		USART2_Send_String(ATCommandBuf[COMMAND_ATCSQ]);
        delay_ms(10);
        
		USART2_Send_String(testarc);
		g_N720InitTIMFlag.bits.bN720SendATCSQCommandFlag = 0;
	}
	
	if(g_N720InitTIMFlag.bits.bN720SendATCREGCommandFlag == 1)
	{
        printf("send ATCREG\r\n");
		USART2_Send_String(ATCommandBuf[COMMAND_ATCREG]);
		g_N720InitTIMFlag.bits.bN720SendATCREGCommandFlag = 0;
	}

	if(g_N720InitTIMFlag.bits.bN720SendATCGATTCommandFlag == 1)
	{
        printf("send ATCGATT\r\n");
		USART2_Send_String(ATCommandBuf[COMMAND_ATCGATT]);
		g_N720InitTIMFlag.bits.bN720SendATCGATTCommandFlag = 0;
	}
	
	if(g_N720InitTIMFlag.bits.bN720SendATMYSYSINFOCommandFlag == 1)
	{
        printf("send ATSYSINFO\r\n");
		USART2_Send_String(ATCommandBuf[COMMAND_ATMYSYSINFO]);
		g_N720InitTIMFlag.bits.bN720SendATMYSYSINFOCommandFlag = 0;
	}
	#endif
}

