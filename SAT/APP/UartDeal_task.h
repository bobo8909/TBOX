#ifndef __UARTDEAL_TASK_H
#define __UARTDEAL_TASK_H
#include "sys.h"

typedef enum 
{
    CMCC,CUCC,CTCC
}SIMCARDTYPE;
    
extern SIMCARDTYPE SimType;

void UartDeal_task(void);
void SendDataNoResponseTimerHandler(void);
#endif

