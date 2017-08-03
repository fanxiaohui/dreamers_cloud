#ifndef _NET_H
#define _NET_H

#include "../Param.h"


/*
	���緢�ͺ���
	mode: 1 tcp_client_send
		  2 udp_client_broadcast
	ip:ip��ַ
	port:�˿ں�
	buf:��Ҫ���͵�����
	len:�������ݵĳ���
*/
void Internet_Tx(int mode, char *ip, char *port, char *buf,int len);

/*
	������ܺ���
	mode: 1 tcp_server_rece
		  2 tcp_client_rece
		  3 udp_server_rece
*/
void Internet_Rx(int mode, char *ip, char *port, char *buf);

#endif
