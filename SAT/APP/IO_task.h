#ifndef __IO_TASK_H
#define __IO_TASK_H

#include "sys.h"

#define OFF 0
#define ON 1

#if 0
typedef enum 
{
	MANNED_DRIVING = 0, //manned
	AUTOMATIC_DRIVING , // 
}DRIVING_MODE;
#endif

u8 CheckDrivingMode(void);

void IO_task(void);

#endif
