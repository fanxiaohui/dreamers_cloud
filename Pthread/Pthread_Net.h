#ifndef _PTHREAD_NET_
#define _PTHREAD_NET_

#include "../Param.h"


extern void *TCP_Server_pthread(void *p);

extern void *Websocket_Client_pthread(void *p);

extern void *service_pthread(void *p);

extern void *WebsocketAlarm_Stat(void *p);

extern void *WebsocketControl(void *p);


#endif


