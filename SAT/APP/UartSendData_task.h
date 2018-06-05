#ifndef __UARTSENDDATA_TASK_H
#define __UARTSENDDATA_TASK_H

#include "sys.h"


typedef struct 
{
	u8 UartSendBuf[USART_REC_LEN];
	u8 UartSendFlag;
}UARTSENDDATA;

void UartSendData_task(void);
#endif 
