
#ifndef _WEBSOCKET_COMMON_H_
#define _WEBSOCKET_COMMON_H_

#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>     // ʹ�� malloc, calloc�ȶ�̬�����ڴ淽��
#include <stdbool.h>
#include <time.h>       // ��ȡϵͳʱ��

#include <errno.h>
#include <netinet/in.h>  
#include <fcntl.h>                      // socket���÷�����ģʽ
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/un.h> 
#include <sys/epoll.h>  // epoll��������������Ӻͽ��մ���

#include <pthread.h>    // ʹ�ö��߳�

// websocket����data[0]�б����ݰ�����    ����0x81 = 0x80 | 0x1 Ϊһ��txt�������ݰ�
typedef enum{
    WCT_MINDATA = -20,      // 0x0����ʶһ���м����ݰ�
    WCT_TXTDATA = -19,      // 0x1����ʶһ��txt�������ݰ�
    WCT_BINDATA = -18,      // 0x2����ʶһ��bin�������ݰ�
    WCT_DISCONN = -17,      // 0x8����ʶһ���Ͽ������������ݰ�
    WCT_PING = -16,     // 0x8����ʶһ���Ͽ������������ݰ�
    WCT_PONG = -15,     // 0xA����ʾһ��pong�������ݰ�
    WCT_ERR = -1,
    WCT_NULL = 0
}Websocket_CommunicationType;

int webSocket_clientLinkToServer(char *ip, int port, char *interface_path);
int webSocket_serverLinkToClient(int fd, char *recvBuf, unsigned int bufLen);

int webSocket_send(int fd, unsigned char *data, unsigned int dataLen, bool mod, Websocket_CommunicationType type);
int webSocket_recv(int fd, unsigned char *data, unsigned int dataMaxLen);

void delayms(unsigned int ms);

#endif

