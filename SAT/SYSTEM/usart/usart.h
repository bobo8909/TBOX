#ifndef __USART_H
#define __USART_H
#include "sys.h" 


#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void USART_Send_String(u8 *buf);

#endif


