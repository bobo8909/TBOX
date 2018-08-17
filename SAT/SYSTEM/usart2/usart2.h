#ifndef __USART2_H
#define __USART2_H
#include "sys.h" 


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define USART_REC_BUF			64	  	
typedef struct 
{
	u8 USART_RX_BUF[USART_REC_LEN];
	u8 USART_RX_Flag;
}UARTData;

typedef union
{
	u16 Byte;
	struct 
	{
		u8 bN720RecvATInfoFlag : 1;
		u8 bN720RecvATIInfoFlag : 1;
		u8 bN720RecvATCCIDInfoFlag : 1;
		u8 bN720RecvATCPINInfoFlag : 1;
		u8 bN720RecvATCSQInfoFlag : 1;
		u8 bN720RecvATCREGInfoFlag : 1;
		u8 bN720RecvATCGATTInfoFlag : 1;
		u8 bN720RecvATMYSYSINFOInfoFlag : 1;
		u8 bN720InitFinish : 1;
		u8 bN720RecvCANDataFlag : 1;
		u8 bnull : 6;
	}bits;
}STRUCT_InitRECVFlag;
extern STRUCT_InitRECVFlag g_N720InitRecvFlag;

typedef union
{
	u16 Byte;
	struct 
	{
		u8 bN720RecvATCGDCONTInfoFlag : 1;
		u8 bN720RecvATXIICInfoFlag : 1;
		u8 bN720RecvATXIIC1InfoFlag : 1;
		u8 bN720RecvATTCPCLOSEInfoFlag : 1;
		u8 bN720RecvATTCPSETUPInfoFlag : 1;       
		u8 bN720RecvATTCPSETUPINGInfoFlag : 1;
		u8 bN720RecvATTCPACKInfoFlag : 1;
		u8 bN720RecvATTCPSENDInfoFlag : 1;
		u8 bN720RecvATTCPRECVInfoFlag : 1;
		u8 bN720TCPInitFinish : 1;
        u8 bN720RecvATTCPXGAUTHInfoFlag : 1;
        u8 bN720SendATStartSendCommandFlag : 1;
        u8 bN720SendATSendDataSuccessCommandFlag : 1;
        u8 bN720SendATRecvACKCommandFlag : 1;
		u8 bN720SendACKFinishFlag : 1;
        u8 bnull :1;
	}bits;
}STRUCT_TCPInitFlag;
extern STRUCT_TCPInitFlag g_N720TCPInitFlag;
typedef struct
{
	u8 SoftwareVerBuf[4];	//软件版本
	u8 CCIDBuf[21];
	u8 CPINBuf[10];
	u8 CSQBuf[6];
	u8 CREGBuf[4];
	u8 CGATTBuf[2];
	u8 MYSYSINFOBuf[6];
    u8 TCPXIIC[20];
    u8 TCPCLOSE[10];
    u8 TCPSETUP[10];
    u8 TCPCGDCONT[20];
    u8 TCPACK[15];
    u8 TCPFinish[10];
    u8 TCPRecvCANData[46];//链路编号(2byte)+','(1byte)+接收的数据长度40(2byte)+','(1byte)+数据(40byte) = 46
}STRUCT_N720_INFO;
extern STRUCT_N720_INFO gN720Info;


extern UARTData gUARTData[USART_REC_BUF]; 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart2_init(u32 bound);
void USART2_Send_String(u8 *buf);
void USART2_Send_CANData(u8 *buf);

#endif


