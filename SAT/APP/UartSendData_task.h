#ifndef __UARTSENDDATA_TASK_H
#define __UARTSENDDATA_TASK_H

#include "sys.h"

#define CANDATA_SEND_LEN 40

#define SEND_DATA_LEN  CANID_SUM * CANDATA_SEND_LEN

typedef struct 
{
	u8 UartSendBuf[CANDATA_SEND_LEN];
	u8 UartSendFlag;
}UARTSENDDATA;

void UartSendData_task(void);
#endif 
