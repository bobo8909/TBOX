#ifndef __UARTSENDDATA_TASK_H
#define __UARTSENDDATA_TASK_H

#include "sys.h"

#define USART_SEND_LEN 40
typedef struct 
{
	u8 UartSendBuf[USART_SEND_LEN];
	u8 UartSendFlag;
}UARTSENDDATA;

void UartSendData_task(void);
#endif 
