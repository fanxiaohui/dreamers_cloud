#include "Net_init.h"
//#include "../Param.h"
int TcpServerInit()
{
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1)
	{
		perror("socket1");
		return -1;
	}
	//准备地址
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
    addr.sin_family = TCP_Server_Ip;
    addr.sin_port = htons(TCP_Server_Port);
    addr.sin_addr.s_addr = INADDR_ANY;
	//防止端口复用
	int   opt   =   1; 
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt)) < 0)
	{
		perror("setsocket");
		return -1;
	}
	//绑定地址
	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
        perror("bind");
        return -1;
    }
	//监听
	if(listen(sockfd,100) == -1){
        perror("listen");
        return -1;
    }
	printf("TCP Server Start.....\n");
	return sockfd;
}

int TcpServerRun(int sockfd)
{
	struct sockaddr_in revAddr = {};
	struct client c;
	while(1)
	{
		memset(&revAddr, 0, sizeof(revAddr));
	    socklen_t len = sizeof(revAddr);
	    int fd = accept(sockfd,(struct sockaddr*)&revAddr,&len);
	    if(fd == -1){                    /////出现错误后，尝试在这里重启
			perror("accept");
	        printf("Server Net Init error...\n");
	        return -1;
	    }
		printf("To receive a client(%s:%u)connection request\n",
					 inet_ntoa(revAddr.sin_addr),
					 ntohs(revAddr.sin_port));

		c.fds = fd;
		strncpy(c.ip, inet_ntoa(revAddr.sin_addr),strlen(inet_ntoa(revAddr.sin_addr)));

		unsigned char buf[2] = {0};
		memset(&buf, 0, sizeof(buf));
		int res = recv(fd,buf,1,0);
		if(buf[0] == 0x01){/*退出*/
			printf("Recv quite commond\n");
			close(fd);
			break;
		}
		
		pthread_t pid;
		int err = pthread_create(&pid,NULL,service_pthread,(void *)&c);
		if(err != 0)
		{
			printf("can't create thread: %s\n",strerror(err));
			close(fd); 
            return -1;
		}
		#if 0
		bool quit = false;
		while(1){
			unsigned char buf[4096] = {0};
			memset(&buf, 0, sizeof(buf));
			int res = recv(fd,buf,3,0);
			if(buf[0] == 0x01){/*退出*/
				printf("Recv quite commond\n");
				close(fd);
				quit = true;
				break;
			}
			printf("0\n");
			if((buf[0] == 0x55) &&(buf[1] == 0x01) && (buf[2] == 0x01))
			{
				/*设备到中控反馈状态 */
				printf("1\n");
				mode_Recv_state(fd,c.ip);
				
			}
			else if((buf[0] == 0x55) &&(buf[1] == 0x02))
			{
				if(buf[2] == 0x01)
				{/*服务器/IPad到中控控制设备*/
					mode_Recv_Command(fd);
					printf("2\n");
				}
				else if(buf[2] == 0x02)
				{/*IPad到中控获取设备状态*/
					printf("3\n");
					Mode_Return_State(fd);
				}
				else
				{
					printf("Recv Data Error\n");
					close(fd);
					break;
				}
			}
			else
			{
				printf("Recv Data Error\n");
				close(fd);
				break;
			}
		}
		if(quit){
			break;
		}
		#endif
	}
	return 0;
}

int WebsocketClientInit()
{
	int fd = -1,ret;
	pthread_t pid;
	char buf[1024];
	char ip[] = Websocket_Client_Ip;//"192.168.244.128";        // 本机IP
	int port = Websocket_Client_Port;
	while(fd <= 0)
	{
		printf("websocket client Start \n");
		fd = webSocket_clientLinkToServer(ip, port, "/null");
		if(fd <= 0){
			//printf("Websocket Client Connect Error...\n");
			continue;
		}
		sleep(1);
		printf("Websocket Client Start...\n");
		ret = pthread_create(&pid, NULL, WebsocketAlarm_Stat,(void*)&fd);  // 传递参数到线程
		if(ret != 0)
		{
			printf("create server false !\r\n");
			goto RETURN_QUIT;
		} 
		mode_Websocket_Recv_Command(fd);
		pthread_join (pid, NULL);
		printf("websocket Server quit\n");
		fd = -1;
	}

	#if 0

	pthread_t pid1;
	ret = pthread_create(&pid1, NULL, WebsocketControl,(void*)&fd);  // 传递参数到线程
	if(ret != 0)
	{
		printf("create server false !\r\n");
		goto RETURN_QUIT;
	} 
	//#else
	while(1){
		memset(buf, 0, sizeof(buf));
	    ret = webSocket_recv(fd, buf, sizeof(buf));
		if(ret > 0)
	    {
	    	printf("Recv: %s\n",buf);
		}
		else
		{
			if(errno == EAGAIN || errno == EINTR)
		        ;
	        else
	        {
	            
				printf("Websocket Client Send Error...\n");
	            break;
	        }
		}
	}
	
	#endif
RETURN_QUIT:
	close(fd);
	pthread_join (pid, NULL);
	//pthread_join (pid1, NULL);
	return 0;
}


