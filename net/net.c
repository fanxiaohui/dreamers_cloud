#include "net.h"


int tcp_server_send(char *address,char *port,char *buf,int len)
{

	//创建网络流式套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}
	// 准备地址并绑定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = INADDR_ANY;
	
	// 设置套接字选项避免地址使用错误  
    int on=1;  
    if((setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("setsockopt failed");  
        return EXIT_FAILURE;  
    }  
	
	struct linger li;
	li.l_onoff = 1;
	li.l_linger = 5;
	setsockopt(sockfd,SOL_SOCKET,SO_LINGER,(char *)&li,sizeof(li));
	
	
	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		perror("bind");
		close(sockfd);// 关闭侦听套接字
		return -1;
	}
	// 启动侦听
	if(listen(sockfd,1024) ==  -1){
		perror("listen");
		close(sockfd);// 关闭侦听套接字
		return -1;
	}
	
	int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	// 等待并接收连接请求
	struct sockaddr_in addrcli = {};//有accept返回以后自动填写
	socklen_t addrlen = sizeof(addrcli);
	int connfd = -1;
	int flag = 2;
	while(flag){
		connfd = accept(sockfd,(struct sockaddr*)&addrcli,&addrlen);
		if(connfd < 0){
			if(errno==EAGAIN || errno == EWOULDBLOCK){
				sleep(1);
				//printf("accept 等待\n");
				//continue;
				//close(sockfd);// 关闭侦听套接字
				//close(connfd);// 关闭连接套接字
				printf("close 1\n");
				flag--;
				continue;
			}else{
				perror("accept:");
				close(sockfd);// 关闭侦听套接字
				close(connfd);// 关闭连接套接字
				printf("close 2\n");
				return -1;
			}
		}else{
			break;
		}	
		
	}

	printf("以接收客户机(%s:%u)的链接请求\n",
			 inet_ntoa(addrcli.sin_addr),
			 ntohs(addrcli.sin_port));
			 
	//发送响应
	if(send(connfd,buf,len,0) == -1){
	  close(sockfd);// 关闭侦听套接字
	  close(connfd);// 关闭连接套接字
	  perror("send");
	  return -1;
	}
	
	close(sockfd);// 关闭侦听套接字
	close(connfd);// 关闭连接套接字
	return 0;
}

int tcp_server_rece(char *address,char *port,char *buf)
{
	char tmp[1024];
	//创建网络流式套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}
	// 准备地址并绑定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = INADDR_ANY;
	
	// 设置套接字选项避免地址使用错误  
    int on=1;  
    if((setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("setsockopt failed");  
        return EXIT_FAILURE;  
    }  
	
	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		perror("bind");
		return -1;
	}
	// 启动侦听
	if(listen(sockfd,1024) == -1){
		perror("listen");
		return -1;
	}
	// 等待并接收连接请求
	struct sockaddr_in addrcli = {};//有accept返回以后自动填写
	socklen_t addrlen = sizeof(addrcli);
	int connfd = accept(sockfd,(struct sockaddr*)&addrcli,&addrlen);
	// 打印客户机信息
	printf("以接收客户机(%s:%u)的链接请求\n",
			 inet_ntoa(addrcli.sin_addr),
			 ntohs(addrcli.sin_port));

    // 与客户机通信
	//接收请求
	ssize_t rb = recv(connfd,tmp,sizeof(tmp),0);
	if(rb == -1){
	  perror("recv");
	  return -1;
	}
	if(rb == 0)
	{
		printf("客户机已关闭\n");
		return -1;
	}
	memcpy(buf,tmp,sizeof(tmp));
	close(sockfd);// 关闭侦听套接字
	close(connfd);// 关闭连接套接字
	
	return 0;
}

int tcp_client_send(char *address,char *port,char *buf,int len)
{
	//创建网络流式套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}
	//准备地址并连接
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(address);
	if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		close(sockfd);
		perror("connect");
		printf("Device not open!!!\n");
		return -1;
	}

	 //发送请求
	if(send(sockfd,buf,len,0) == -1){
	  close(sockfd);
	  perror("send");
	  return -1;
	}
	printf("Tcp Client quit!\n");
	//关闭套接字
	close(sockfd);
	return 0;
}

int tcp_client_rece(char *address,char *port,char *buf)
{
	char tmp[1024];
	//创建网络流式套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}
	//准备地址并连接
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(address);
	if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		perror("connect");
		return -1;
	}
	//接收响应
	ssize_t rb = recv(sockfd,tmp,sizeof(tmp),0);
	if(rb == -1){
	  perror("recv");
	  return -1;
	}
	memcpy(buf,tmp,sizeof(tmp));
	//关闭套接字
	close(sockfd);
	printf("TCP Client quit\n");
	return 0;
}

int udp_server_send(char *address,char *port,char *buf)
{
	  //创建网络数据报式的套接字
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}  
	//准备地址并绑定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		perror("bind");
		return -1;
	}
	
	char tmp[1024];
	struct sockaddr_in addrcli = {};//客户端的地址信息
	socklen_t addrlen = sizeof(addrcli);
	ssize_t rb = recvfrom(sockfd,tmp,sizeof(tmp),0,
		  (struct sockaddr*)&addrcli,&addrlen);
	if(rb == -1){
		perror("recvfrom");
		return -1;	
	}
	/*
	printf("接收到来自客户机(%s:%u)的%d个字节数据:\n",
		  inet_ntoa(addrcli.sin_addr),
		  ntohs(addrcli.sin_port),rb);*/
	//业务处理
	printf("> %s\n",tmp);
	//发送响应
	if(sendto(sockfd,buf,(strlen((char *)buf)+1)*sizeof(buf[0]),0,(struct sockaddr*)&addrcli,
			  addrlen) == -1){
		perror("snedto");
		return -1;
	}
	//关闭套接字
	close(sockfd); 
	return 0;
}

int udp_server_rece(char *address,char *port,char *buf)
{
	char tmp[1024];
	  //创建网络数据报式的套接字
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}  
	//准备地址并绑定
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) == -1){
		perror("bind");
		return -1;
	}
	
	//发送响应
	/*
	char tmp[] = "hello";
	if(sendto(sockfd,tmp,rb,0,(struct sockaddr*)&addrcli,
			  addrlen) == -1){
		perror("snedto");
		return -1;
	}
	*/
	 //接收请求
	struct sockaddr_in addrcli = {};//客户端的地址信息
	socklen_t addrlen = sizeof(addrcli);
	ssize_t rb = recvfrom(sockfd,tmp,sizeof(tmp),0,
		  (struct sockaddr*)&addrcli,&addrlen);
	if(rb == -1){
		perror("recvfrom");
		return -1;
	}
	/*
	printf("接收到来自客户机(%s:%u)的%d个字节数据:\n",
		  inet_ntoa(addrcli.sin_addr),
		  ntohs(addrcli.sin_port),rb);
		*/
	memcpy(buf,tmp,sizeof(tmp));
	//关闭套接字
	close(sockfd); 
	return 0;
}

int udp_client_broadcast(char *address,char *port,char *buf,int len)
{
	//创建网络数据报套接字
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}
	//打开广播选项
	int on = 1;//打开
	if(setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&on,
		  sizeof(on)) == -1){
		perror("setscokopt");
		return -1;
	}
	//准备服务器地址
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port));
	addr.sin_addr.s_addr = inet_addr(address);
	//printf("%d\n",(strlen((char *)buf)+1));
	//printf("%d\n",(strlen((char *)buf)+1)*sizeof(buf[0]));
	//广播数据
	if(sendto(sockfd,buf,len,0,(struct sockaddr*)&addr,
				sizeof(addr)) == -1){
	  perror("sendto");
	  return -1;
	}
	
	//关闭套接字
	close(sockfd);
	printf("UDP Broadcast quit\n");
	return 0;
}

void Internet_Tx(int mode, char *ip, char *port, char *buf,int len)
{
	switch(mode)
	{
		case 1: tcp_client_send(ip, port, buf,len); break;
		case 2: udp_client_broadcast(ip, port, buf,len);break;
		default:
			printf("input error");
				break;
	}
}

void Internet_Rx(int mode, char *ip, char *port, char *buf)
{
	switch(mode)
	{
		//case 1: tcp_server_rece(ip, port, buf); break;
		//case 2: tcp_client_rece(ip, port, buf); break;
		//case 3: udp_server_rece(ip, port, buf); break;
		//default:
		//	printf("input error");
		//		break;
	}
}
/*
void TCP_Server_test(char *ip, char *port)
{
	char temp[1024];
	Internet_Tx(1, ip, port, "01");
	printf("TX:01\n");
	Internet_Rx(1, ip, port, temp);
	printf("RX:%s\n",temp);
}

void TCP_Client_test(char *ip, char *port)
{
	char temp[1024];
	Internet_Tx(2, ip, port, "03");
	printf("TX:03\n");
	Internet_Rx(2, ip, port, temp);
	printf("RX:%s\n",temp);
}
void UDP_Server_test(char *ip, char *port)
{
	char temp[1024];
	Internet_Tx(3, ip, port, "05");
	printf("TX:05\n");
	Internet_Rx(3, ip, port, temp);
	printf("RX:%s\n",temp);
}
void UDP_Broadcast(char *ip, char *port)
{
	udp_client_broadcast(ip, port, "08");
}
*/


