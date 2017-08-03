#ifndef _NET_H
#define _NET_H

#include "../Param.h"


/*
	网络发送函数
	mode: 1 tcp_client_send
		  2 udp_client_broadcast
	ip:ip地址
	port:端口号
	buf:需要发送的数据
	len:发送数据的长度
*/
void Internet_Tx(int mode, char *ip, char *port, char *buf,int len);

/*
	网络接受函数
	mode: 1 tcp_server_rece
		  2 tcp_client_rece
		  3 udp_server_rece
*/
void Internet_Rx(int mode, char *ip, char *port, char *buf);

#endif
