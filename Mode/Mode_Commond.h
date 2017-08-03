#ifndef _MODE_COMMOND_
#define _MODE_COMMOND_

#include "../Param.h"



extern int mode_Recv_state(int fd,char *ip);
extern int mode_Recv_Command(int fd);
extern int Mode_Return_State(int fd);
extern int mode_Websocket_Recv_Command(int fd);

#endif

