#ifndef _N720_DRIVER_H
#define _N720_DRIVER_H

#include "sys.h"

#define COMMAND_AT 0
#define COMMAND_ATI 1
#define COMMAND_ATCCID 2
#define COMMAND_ATCPIN 3
#define COMMAND_ATCSQ 4
#define COMMAND_ATCREG 5
#define COMMAND_ATCGATT 6
#define COMMAND_ATMYSYSINFO 7

#define N720POWERKEY PFout(4)
#define N720_ON 1
#define N720_OFF 0



void N720Init(void);
void N720IOInit(void);
void N720PowerON(void);
void N720PowerOFF(void);
void N720PowerkeyReset(void);


#endif
