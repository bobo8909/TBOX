#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0 PFout(6)// PB5

#define LED_ON 1
#define LED_OFF 0
void LED_Init(void);//≥ı ºªØ
void Led_task(void);
void LedTimerHandler(void);

		 				    
#endif
