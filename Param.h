#ifndef _PARAM_H_
#define _PARAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <linux/reboot.h>
#include <linux/tcp.h>
#include <time.h>
#include <sys/time.h>  

#include "./Cjson/cJSON.h"
#include "./Pthread/Pthread_Net.h"
#include "./Init/Config_init.h"
#include "./websocket/websocket_common.h"
#include "./Mode/Mode_Commond.h"
#include "./net/net.h"

#define config_filename "./Config/Config.json"

#define TCP_Server_Ip AF_INET
#define TCP_Server_Port 8990

#define Websocket_Client_Ip "192.168.1.37"
#define Websocket_Client_Port 9999

struct client{
	char ip[100];//客户端的IP地址
	int fds;//客户端的socket
};



#endif

