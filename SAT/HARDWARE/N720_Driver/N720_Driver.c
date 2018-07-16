#include "N720_Driver.h"

u8 *ATCommandBuf[] =
			{
				"AT\r",
				"ATI\r",
				"AT+CCID\r",
				"AT+CPIN?\r",
				"AT+CSQ\r",
				"AT+CREG?\r",
				"AT+CGATT?\r",
				"AT$MYSYSINFO\r"
			};

			
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
