#ifndef __N720_TCPINIT_H
#define __N720_TCPINIT_H
#include "sys.h"

#define COMMAND_ATCGDCONT 0
#define COMMAND_ATXIIC 1
#define COMMAND_ATXIIC1 2
#define COMMAND_ATTCPCLOSE 3
#define COMMAND_ATTCPSETUP 4
#define COMMAND_ATTCPACK 5
#define COMMAND_ATTCPSEND 6


void N720_TCPInit(void);

#endif
