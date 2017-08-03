#include "Pthread_Net.h"
extern pthread_mutex_t mutex;
extern cJSON *root_config,*root_state;


void *TCP_Server_pthread(void *p)
//void TCP_Server_pthread()
{
	//套接字
	int sockfd;
	sockfd = TcpServerInit();
	if(sockfd < 0){
		perror("TcpServerInit");
		return;
	}
	TcpServerRun(sockfd);

	return;
}
void *Websocket_Client_pthread(void *p)
{
	if(WebsocketClientInit() < 0){
		perror("WebsocketClientInit");
		return;
	}
	return;
}
void *service_pthread(void *p)
{
	pthread_detach(pthread_self());
	struct client c = *(struct client*)p;
	int fd = c.fds;

	while(1){
		unsigned char buf[4096] = {0};
		memset(&buf, 0, sizeof(buf));
		int res = recv(fd,buf,3,0);
		if(buf[0] == 0x01){/*退出*/
			printf("Recv quite commond--pthread\n");
			close(fd);
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

	
	//mode_Recv_state(fd,c.ip);
	pthread_exit(0);
}


void *WebsocketAlarm_Stat(void *p)
{
	int fd = *(int *)p;
	int ret = 0;
	while(1){
		char *out = NULL;
		pthread_mutex_lock(&mutex);
		out = cJSON_Print(root_state);
		pthread_mutex_unlock(&mutex); 
		ret = webSocket_send(fd, out, strlen(out), true, WCT_TXTDATA);
		if(ret <= 0)    // send返回负数, 连接已断开
	    {
	    	printf("Websocket Send Data Error!\n");
			free(out);
	        close(fd);
	        break;
	    }
		free(out);
		
		delayms(5000);
	}
	pthread_exit(0);
}

void *WebsocketControl(void *p)
{
	char buf[1024];
	int fd = *(int *)p;
	int ret = 0;
	while(1){
		memset(buf, 0, sizeof(buf));
	    ret = webSocket_recv(fd, buf, sizeof(buf));
		if(ret > 0)
	    {
	    	printf("Recv: %s\n",buf);
			//mode_Websocket_Recv_Command(buf);
		}
		else
		{
			if(errno == EAGAIN || errno == EINTR)
		        ;
	        else
	        {
	            
				printf("Websocket Client Recv Error...\n");
				close(fd);
	            break;
	        }
		}
	}
	pthread_exit(0);
}


