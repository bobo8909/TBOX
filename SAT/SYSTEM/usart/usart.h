#ifndef __USART_H
#define __USART_H
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
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
		u8 bnull : 7;
	}bits;
}STRUCT_InitRECVFlag;
extern STRUCT_InitRECVFlag g_N720InitRecvFlag;

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
		u8 bnull : 7;
	}bits;
}STRUCT_TCPInitFlag;
extern STRUCT_InitRECVFlag g_N720TCPInitFlag;

typedef struct
{
	u8 SoftwareVerBuf[4];	//软件版本
	u8 CCIDBuf[20];
	u8 CPINBuf[10];
	u8 CSQBuf[6];
	u8 CREGBuf[4];
	u8 CGATTBuf[1];
	u8 MYSYSINFOBuf[4];
}STRUCT_N720_INFO;
extern STRUCT_N720_INFO gN720Info;


extern UARTData gUARTData[USART_REC_BUF]; 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void USART_Send_String(u8 *buf);

#endif


