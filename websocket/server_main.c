
#include "websocket_common.h"

#define		EPOLL_RESPOND_NUM		100		// epollͬʱ��Ӧ�¼�����

typedef int (*CallBackFun)(int fd, char *buf, unsigned int bufLen);

typedef struct{
    int fd;
    int client_fd_array[EPOLL_RESPOND_NUM][2];
    char ip[24];
    int port;
    char buf[1024];
    CallBackFun action;
}Websocket_Server;

//////////////////////////////////////////////////////////// Tool Function ///////////////////////////////////////////////////////////////////////////////

int arrayAddItem(int array[][2], int arraySize, int value)
{
    int i;
    for(i = 0; i < arraySize; i++)
    {
        if(array[i][1] == 0)
        {
            array[i][0] = value;
            array[i][1] = 1;
            return 0;
        }
    }
    return -1;
}

int arrayRemoveItem(int array[][2], int arraySize, int value)
{
    int i;
    for(i = 0; i < arraySize; i++)
    {
        if(array[i][0] == value)
        {
            array[i][0] = 0;
            array[i][1] = 0;
            return 0;
        }
    }
    return -1;
}

//////////////////////////////////////////////////////////// Call Back Function ///////////////////////////////////////////////////////////////////////////////

int call(CallBackFun function, int fd, char *buf, unsigned int bufLen)
{
    return function(fd, buf, bufLen);
}

//////////////////////////////////////////////////////////// Server Function ///////////////////////////////////////////////////////////////////////////////

void server_thread_fun(void *arge)
{
	int ret , i , j;
	int accept_fd;
	int socAddrLen;
	struct sockaddr_in acceptAddr;
	struct sockaddr_in serverAddr;
	//
	Websocket_Server *ws = (Websocket_Server *)arge;
	//
	memset(&serverAddr , 0 , sizeof(serverAddr)); 			// ���ݳ�ʼ��--����      
    serverAddr.sin_family = AF_INET; 						// ����ΪIPͨ��    
    //serverAddr.sin_addr.s_addr = inet_addr(ws->ip);		// ������IP��ַ
    serverAddr.sin_addr.s_addr = INADDR_ANY;				// ������IP��ַ
    serverAddr.sin_port = htons(ws->port); 					// �������˿ں�    
	//
	socAddrLen = sizeof(struct sockaddr_in);
	
	//------------------------------------------------------------------------------ socket init
	//socket init
	ws->fd = socket(AF_INET, SOCK_STREAM,0);  
    if(ws->fd <= 0)  
    {  
        printf("server cannot create socket !\r\n"); 
        exit(1);
    }    
    
    //����Ϊ����������
    ret = fcntl(ws->fd , F_GETFL , 0);
    fcntl(ws->fd , F_SETFL , ret | O_NONBLOCK);
    
    //bind sockfd & addr  
    while(bind(ws->fd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) < 0 )
        delayms(1);
    
    //listen sockfd   
    ret = listen(ws->fd, 0);  
    if(ret < 0) 
    {  
        printf("server cannot listen request\r\n"); 
        close(ws->fd); 
        exit(1);
    } 
    //------------------------------------------------------------------------------ epoll init
    // ����һ��epoll���  
    int epoll_fd;  
    epoll_fd = epoll_create(EPOLL_RESPOND_NUM);  
    if(epoll_fd < 0)
    {  
        printf("server epoll_create failed\r\n"); 
        exit(1);
    }  
    
    int nfds;                            // epoll�����¼������ĸ��� 
    struct epoll_event ev;             // epoll�¼��ṹ��  
    struct epoll_event events[EPOLL_RESPOND_NUM];
    ev.events = EPOLLIN|EPOLLET;  			// 	EPOLLIN		EPOLLET;    �����¼�����
    ev.data.fd = ws->fd;  
    // ��epollע��server_sockfd�����¼�  
    if(epoll_ctl(epoll_fd , EPOLL_CTL_ADD , ws->fd , &ev) < 0)
    {  
        printf("server epll_ctl : ws->fd register failed\r\n"); 
        close(epoll_fd);
        exit(1);
    }  
    
	//------------------------------------------------------------------------------ server receive
	printf("\r\n\r\n========== server start ! ==========\r\n\r\n");
    while(1)
    { 
    	// �ȴ��¼�����  
        nfds = epoll_wait(epoll_fd , events , EPOLL_RESPOND_NUM , -1);  	// -1��ʾ������������ֵΪ��ʱ
        if(nfds < 0)
        {  
            printf("server start epoll_wait failed\r\n"); 
            close(epoll_fd);
            exit(1);
        }  
       
        for(j = 0 ; j < nfds ; j++)
        { 
        	//===================epoll���� =================== 
            if ((events[j].events & EPOLLERR) || (events[j].events & EPOLLHUP))  
            {
            	    //printf("server epoll err !\r\n"); 
            	    //exit(1);
            	    printf("accept close : %d\r\n", events[j].data.fd);     // ��ͻ������ӳ���, �����Ͽ���ǰ ����
					// ��epollɾ��client_sockfd�����¼� 
					//ev.events = EPOLLIN|EPOLLET;
        			ev.data.fd = events[j].data.fd; 
				    if(epoll_ctl(epoll_fd , EPOLL_CTL_DEL , events[j].data.fd , &ev) < 0) 
				    {  
				        printf("server epoll_ctl : EPOLL_CTL_DEL failed !\r\n"); 
				        close(epoll_fd);
				        exit(1);
				    }
				    arrayRemoveItem(ws->client_fd_array, EPOLL_RESPOND_NUM, events[j].data.fd);    // �������޳�fd
					close(events[j].data.fd);	//�ر�ͨ��
            }
        	//===================��ͨ�������¼�===================
        	else if(events[j].data.fd == ws->fd)
        	{		
				//��Ѳ���ܽ������ͨ�� ����ͨ���ż�¼��accept_fd[]������
				accept_fd = accept(ws->fd, (struct sockaddr *)&acceptAddr, &socAddrLen);  
				if(accept_fd >= 0)  //----------���½��룬ͨ���ż�1----------
				{ 	
					// ��epollע��client_sockfd�����¼�    
					//ev.events = EPOLLIN|EPOLLET;
		    		ev.data.fd = accept_fd; 
					if(epoll_ctl(epoll_fd , EPOLL_CTL_ADD , accept_fd , &ev) < 0) 
					{  
						 printf("server epoll_ctl : EPOLL_CTL_ADD failed !\r\n"); 
						 close(epoll_fd);
						 exit(1);
					}
					//send(accept_fd , "OK\r\n" , 4 , MSG_NOSIGNAL);
					printf("server fd/%d : accept\r\n", accept_fd);
					arrayAddItem(ws->client_fd_array, EPOLL_RESPOND_NUM, accept_fd);   // ���fd������
				}
			}
			//===================���������¼�===================
			else if(events[j].events & EPOLLIN)
			{		
				//ret = recv(events[j].data.fd , ws->buf , sizeof(ws->buf) ,  MSG_NOSIGNAL); 			//  MSG_NOSIGNAL(������)  MSG_DONTWAIT  MSG_WAITALL
				memset(ws->buf, 0, sizeof(ws->buf));
				ret = call(ws->action, events[j].data.fd , ws->buf , sizeof(ws->buf));
				if(ret <= 0)		//----------ret<=0ʱ����쳣, �����Ƿ������������----------
				{
				    if(errno == EAGAIN || errno == EINTR)
                        ;
                    else
                    {
					    printf("accept close : %d\r\n", events[j].data.fd);
					    // ��epollɾ��client_sockfd�����¼�  
					    //ev.events = EPOLLIN|EPOLLET;
            			ev.data.fd = events[j].data.fd; 
				        if(epoll_ctl(epoll_fd , EPOLL_CTL_DEL , events[j].data.fd , &ev) < 0) 
				        {  
				            printf("server epoll_ctl : EPOLL_CTL_DEL failed !\r\n"); 
				            close(epoll_fd);
				            exit(1);
				        }
				        arrayRemoveItem(ws->client_fd_array, EPOLL_RESPOND_NUM, events[j].data.fd);    // �������޳�fd
					    close(events[j].data.fd);	//�ر�ͨ��
					}
				}
			}
			//===================���������¼�===================
			else if(events[j].events & EPOLLOUT)
			    ;
		}
    }  
    
	//�ر�epoll���
    close(epoll_fd);
    //�ر�socket
    close(ws->fd); 
    //�˳��߳�
    pthread_exit(NULL); 
}
//////////////////////////////////////////////////////////// Call Back Function ///////////////////////////////////////////////////////////////////////////////

int server_action(int fd, char *buf, unsigned int bufLen)
{
    int ret;
    ret = webSocket_recv(fd , buf , bufLen);    // ʹ��websocket recv
    if(ret > 0)
	{
		printf("server fd/%d : len/%d %s\r\n", fd, ret, buf);
		
		//===== ��������ݿͻ��˵���������, �ṩ��Ӧ�ķ��� =====
		
		if(strstr(buf, "connect") != NULL)     // �ɹ�����֮��, ������������
		    ret = webSocket_send(fd, "Hello !", strlen("Hello !"), false, WCT_TXTDATA);
		else if(strstr(buf, "Hello") != NULL)
		    ret = webSocket_send(fd, "I am Server_Test", strlen("I am Server_Test"), false, WCT_TXTDATA);
		else
		    ret = webSocket_send(fd, "You are carefree ...", strlen("You are carefree ..."), false, WCT_TXTDATA);
		// ... ...
		// ...
	}
	return ret;
}

//////////////////////////////////////////////////////////// Main Function ///////////////////////////////////////////////////////////////////////////////

int main(void)
{
    int ret;
    int i, client_fd;
    pthread_t sever_thread_id;
    Websocket_Server ws;
    
    //===== ��ʼ������������ =====
    memset(&ws, 0, sizeof(ws));
    //strcpy(ws.ip, "127.0.0.1");     
    ws.port = 9999;
    ws.action = &server_action;      // ��Ӧ�ͻ���ʱ, ��Ҫ����?
    
    //===== �����߳�, ��������� =====
    ret = pthread_create(&sever_thread_id, NULL, (void*)&server_thread_fun, (void *)(&ws));  // ���ݲ������߳�
	if(ret != 0)
	{
		printf("create server false !\r\n");
		exit(1);
	} 
	#if 0
	while(1)
	{
	    for(i = 0; i < EPOLL_RESPOND_NUM; i++)
	    {
	        if(ws.client_fd_array[i][1] != 0 && ws.client_fd_array[i][0] > 0)
	        {
	            client_fd = ws.client_fd_array[i][0];
	            /////////////////////////////////////////////////////////////////////////////////////////////   ���������������������������Ŀͻ��� ���͵��������
	            
	            ret = webSocket_send(client_fd, "\\O^O/  <-.<-  TAT  =.=#  -.-! ...", strlen("\\O^O/  <-.<-  TAT  =.=#  -.-! ..."), false, WCT_TXTDATA);
	            
	            /////////////////////////////////////////////////////////////////////////////////////////////
	        }
	    }
	    delayms(5000);
	}
	#endif
	//==============================
    pthread_join(sever_thread_id, NULL);     // �ȴ��̹߳ر�
    printf("server close !\r\n");
    return 0;
}







