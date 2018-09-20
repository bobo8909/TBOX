#ifndef __UARTSENDDATA_TASK_H
#define __UARTSENDDATA_TASK_H

#include "sys.h"

#define GPS_SEND_DATA_LEN 110
#define CANID_SUM 16
#define CANDATA_SEND_LEN (CANDATA_BUF_LEN * 2)
#define CANDATA_BUF_LEN 20
#define SEND_DATA_LEN  (CANID_SUM * CANDATA_SEND_LEN + GPS_SEND_DATA_LEN)



typedef struct 
{
	u8 UartSendBuf[CANDATA_SEND_LEN];
	u8 UartSendFlag;
}UARTSENDDATA;

extern u8 DataSendBuf[SEND_DATA_LEN];

void SwitchSendData(u8* src ,u8* SendVal,u16 SendValLen);
void UartSendData_task(void);
#endif 
