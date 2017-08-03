#include "Mode_Commond.h"

extern cJSON *root_config,*root_state;
extern bool Locked_flag;
extern pthread_mutex_t mutex;

int mode_Recv_state(int fd,char *ip)
{
  	unsigned char buf[100] = {0};
	char devicename[100] = {0};
	memset(&buf, 0, sizeof(buf));
	int res = recv(fd,buf,13,0);
	if(res < 0)
	{
		printf("The reception contract failed\n");
		return -1;
	}
	int kk = 0;
	for(kk = 0;kk < 13;kk++)
		printf("%x ",buf[kk]);
	printf("\n");
	printf("buf[11]:%x\n",buf[11]);
	printf("buf[12]:%x\n",buf[12]);
	if((buf[11] != 0x1f) || (buf[12] != 0xff))
	{
		printf("Check Error!\n");
		return -1;
	}
	switch(buf[0])
	{
		case 0x01://电脑
					strcpy(devicename,"Computer");
					break;
		case 0x02://投影仪
					strcpy(devicename,"Projector");
					break;
		case 0x03://强电
					strcpy(devicename,"StrongPower");
					break;
		case 0x04://弱电
					strcpy(devicename,"WeakPower");
					break;
		default:	printf("input error！！！\n");
					return -1;;
	}

	int flag_i = 0;	//循环量
	int device_id = -1; //设备号
	pthread_mutex_lock(&mutex);
	while(1)
	{
		char *device_ip = ReadJsonString(root_config,devicename,flag_i,"Device_ip");
		if(device_ip == NULL){
			printf("IP address not found\n");
			return -1;
		}
		//Error
		if(strcmp(device_ip,ip/*,(strlen((char *)bites)+1)*sizeof(bites[0])*/) == 0){
			device_id = ReadJsonInt(root_config,devicename,flag_i,"id");
			if(device_id == -1){
				printf("Can not find device id:%d\n",flag_i);
				return -1;
			}
			break;
		}
		flag_i++;
	}

	printf("devicename:%s\n id:%d\n",devicename,device_id);

	//使能开启
	if(WriteJsonInt(root_state,devicename,device_id,"Enable",1) == -1){
		printf("Write Json Int Error\n");
		return -1;
	}
	//报警状态
	unsigned int alarm_info = buf[1];
	if(WriteJsonInt(root_state,devicename,device_id,"Alarm",alarm_info) == -1){
		printf("Write Json Int Error\n");
		return -1;
	}
	//开关状态
	unsigned int power = buf[4];
	if(WriteJsonInt(root_state,devicename,device_id,"Statue",power) == -1){
		printf("Write Json Int Error\n");
		return -1;
	}
	//运行时间
	unsigned int runtime = ((buf[5] << 16)&0xff0000) | ((buf[6] << 8)&0xff00) | (buf[7]&0xff);
	if(WriteJsonInt(root_state,devicename,device_id,"Runtime",runtime) == -1){
		printf("Write Json Int Error\n");
		return -1;
	}
	//开机次数
	unsigned int runtec = (buf[8] << 8) | buf[9];
	if(WriteJsonInt(root_state,devicename,device_id,"Boot",runtec) == -1){
		printf("Write Json Int Error\n");
		return -1;
	}
	//剩余寿命
	unsigned int lasttime = buf[10];
	if(WriteJsonInt(root_state,devicename,device_id,"Lasttime",lasttime) == -1){
		printf("Write Json Int Error\n");
		return -1;
	}
	pthread_mutex_unlock(&mutex); 
	//printf("out:\n%s\n",cJSON_Print(root_state));
	return 0;
}

void Change(unsigned char *pbDest,unsigned  char *pbSrc, int nLen)
{
	char h1,h2;
	char s1,s2;
	int i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
		s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
		s2 -= 7;

		pbDest[i] = s1*16 + s2;
	}
}

int mode_Recv_Command(int fd)
{
	unsigned char buf[4096] = {0};
	char devicename[100] = {0};
	char tmp[100] = {0};
	char *ip = NULL,*port = NULL;
	/*
		检测上锁标志位有无置位
		有则返回置位信息
	*/
	pthread_mutex_lock(&mutex);
	
	if(Locked_flag){
		unsigned char send_data[5] = {0};
		send_data[0] = 0x55;
		send_data[1] = 0x02;
		send_data[2] = 0x02;
		send_data[3] = 0x1f;
		send_data[4] = 0xff;
		int ret = send(fd,send_data,14,MSG_NOSIGNAL);
		
		if((ret < 0) && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)){
			send(fd,send_data,14,0);
			Locked_flag = false;
		}else if((ret <= 0) && (errno != EINTR) && (errno != EWOULDBLOCK) && (errno != EAGAIN)){
			printf("send Locked Error!\n");
		}
		else{
			Locked_flag = false;
		}
	}
	
	pthread_mutex_unlock(&mutex); 
	
	memset(&buf, 0, sizeof(buf));
	int res = recv(fd,buf,7,0);
	if(res < 0)
	{
		printf("The reception contract failed\n");
		return -1;
	}
	if((buf[5] != 0x1f) || (buf[6] != 0xff))
	{
		printf("Check Error!\n");
		return -1;
	}
	switch(buf[0])
	{
		case 0x01://电脑
					strcpy(devicename,"Computer");
					break;
		case 0x02://投影仪
					strcpy(devicename,"Projector");
					break;
		case 0x03://强点
					strcpy(devicename,"StrongPower");
					break;
		case 0x04://弱电
					strcpy(devicename,"WeakPower");
					break;
		default:	printf("input error！！！\n");
					return -1;;
	}
	printf("devicename: %s\n",devicename);
	//针对某类设备的总开关
	if(buf[0] != 0xff && buf[1] == 0xff)
	{
		printf("A\n");
		switch(buf[2])//总开关
		{
			case 0x01://总开
						//power_on_device(fd,devicename);
					break;
			case 0x02://总关
						//power_off_device(fd,devicename);
					break;
			case 0x00://默认
					break;
			default:
					printf("input error！！！\n");
					return -1;
		}
	}
	else
	{
		//总开关
		if(buf[2] != 0x00){
			printf("B\n");
			switch(buf[2])
			{
				case 0x01://总开
							//power_on(fd);
							return 0;
						break;
				case 0x02://总关
							//power_off(fd);
							return 0;
						break;
				case 0x00://默认
						break;
				default:
						printf("input error！！！\n");
						return -1;
			}
		}
		//指定设备开关
		if(buf[2] == 0x00){
			printf("C\n");
			switch(buf[3])
			{
				case 0x01://开
						strcpy(tmp,"poweron");
						break;
				case 0x02://关
						strcpy(tmp,"poweroff");
						break;
				case 0x00://默认
						break;
				default:
						printf("input error！！！\n");
						return -1;
			}
			
			pthread_mutex_lock(&mutex);
			
			int control_flag = ReadJsonInt(root_config,devicename,buf[1] - 1,"Control_flag");
			if(control_flag == 1){
				ip = ReadJsonString(root_config,devicename,buf[1] - 1,"Control_ip");
				if(ip == NULL) return -1;
				port = ReadJsonString(root_config,devicename,buf[1] - 1,"Control_port");
				if(port == NULL) return -1;
			}
			else if(control_flag == 0)
			{
				ip = ReadJsonString(root_config,devicename,buf[1] - 1,"Device_ip");
				if(ip == NULL) return -1;
				port = ReadJsonString(root_config,devicename,buf[1] - 1,"Device_port");
				if(port == NULL) return -1;
			}
			else
			{
				printf("control_flag Error\n");
				return -1;
			}
			char *content = ReadJsonString(root_config,devicename,buf[1] - 1,tmp);
			if(content == NULL) return -1;
			
			pthread_mutex_unlock(&mutex); 
			
			int content_len = strlen(content)/2;
			if(content_len == 0) return -1;
			//printf("len:%d\n",content_len);
			unsigned char bites[content_len];
			bzero(bites,content_len);
			Change(bites,content,content_len);
			printf("connect ip:%s,port:%s,send:%s\n",ip,port,content);
			int iii = 0;
			for(iii = 0;iii< content_len;iii++)
				printf("bites: %x\n",bites[iii]);
			if(buf[4] == 0x01)
			{
				printf("TCP\n");
				Internet_Tx(1,ip,port,bites,content_len);
			}
			if(buf[4] == 0x02)
			{
				printf("UDP\n");
				Internet_Tx(2,ip,port,bites,content_len);
			}
		}
	}
	return 0;
}

int Mode_Return_State(int fd)
{
	unsigned char buf[4096] = {0};
	memset(&buf, 0, sizeof(buf));
	int res = recv(fd,buf,2,0);
	if(res < 0)
	{
		printf("The reception contract failed\n");
		return -1;
	}
	printf("buf[11]:%x\n",buf[0]);
	printf("buf[12]:%x\n",buf[1]);
	if((buf[0] != 0x1f) || (buf[1] != 0xff))
	{
		printf("Check Error!\n");
		return -1;
	}
	
	char *out = NULL;
	pthread_mutex_lock(&mutex);
	out = cJSON_Print(root_state);
	pthread_mutex_unlock(&mutex); 
	if(send(fd,out,(strlen((char *)out)+1)*sizeof(out[0]),0) == -1){
	  close(fd);// 关闭连接套接字
	  perror("send");
	  return -1;
	}
	free(out);
	out = NULL;
}
int WebsocketRecv(char *buf)
{
	char devicename[100] = {0};
	char tmp[100] = {0};
	char *ip = NULL,*port = NULL;
	switch(buf[3])
	{
		case 0x01://电脑
					strcpy(devicename,"Computer");
					break;
		case 0x02://投影仪
					strcpy(devicename,"Projector");
					break;
		case 0x03://强点
					strcpy(devicename,"StrongPower");
					break;
		case 0x04://弱电
					strcpy(devicename,"WeakPower");
					break;
		default:	printf("input error！！！\n");
					return -1;;
	}
	printf("devicename: %s\n",devicename);
	//针对某类设备的总开关
	if(buf[3] != 0xff && buf[4] == 0xff)
	{
		printf("A\n");
		switch(buf[5])//总开关
		{
			case 0x01://总开
						//power_on_device(fd,devicename);
					break;
			case 0x02://总关
						//power_off_device(fd,devicename);
					break;
			case 0x00://默认
					break;
			default:
					printf("input error！！！\n");
					return -1;
		}
	}
	else if(buf[3] == 0xff && buf[4] == 0xff)
	{
		//总开关
		if(buf[5] != 0x00){
			printf("B\n");
			switch(buf[5])
			{
				case 0x01://总开
							//power_on(fd);
							return 0;
						break;
				case 0x02://总关
							//power_off(fd);
							return 0;
						break;
				case 0x00://默认
						break;
				default:
						printf("input error！！！\n");
						return -1;
			}
		}
	}
	else
	{
		//指定设备开关
		if(buf[5] == 0x00){
			printf("C\n");
			switch(buf[3])
			{
				case 0x01://开
						strcpy(tmp,"poweron");
						break;
				case 0x02://关
						strcpy(tmp,"poweroff");
						break;
				case 0x00://默认
						break;
				default:
						printf("input error！！！\n");
						return -1;
			}

			pthread_mutex_lock(&mutex);
			
			int control_flag = ReadJsonInt(root_config,devicename,buf[4] - 1,"Control_flag");
			if(control_flag == 1){
				ip = ReadJsonString(root_config,devicename,buf[4] - 1,"Control_ip");
				if(ip == NULL) return -1;
				port = ReadJsonString(root_config,devicename,buf[4] - 1,"Control_port");
				if(port == NULL) return -1;
			}
			else if(control_flag == 0)
			{
				ip = ReadJsonString(root_config,devicename,buf[4] - 1,"Device_ip");
				if(ip == NULL) return -1;
				port = ReadJsonString(root_config,devicename,buf[4] - 1,"Device_port");
				if(port == NULL) return -1;
			}
			else
			{
				printf("control_flag Error\n");
				return -1;
			}
			char *content = ReadJsonString(root_config,devicename,buf[4] - 1,tmp);
			if(content == NULL) return -1;

			pthread_mutex_unlock(&mutex);
			
			int content_len = strlen(content)/2;
			if(content_len == 0) return -1;
			//printf("len:%d\n",content_len);
			unsigned char bites[content_len];
			bzero(bites,content_len);
			Change(bites,content,content_len);
			printf("connect ip:%s,port:%s,send:%s\n",ip,port,content);
			if(buf[7] == 0x01)
			{
				printf("TCP\n");
				Internet_Tx(1,ip,port,bites,content_len);
			}
			if(buf[7] == 0x02)
			{
				printf("UDP\n");
				Internet_Tx(2,ip,port,bites,content_len);
			}
			if(buf[8] == 0x02)
			{
				printf("Locked\n");
				Locked_flag = true;
			}
		}
	}
	return 0;
}
int mode_Websocket_Recv_Command(int fd)
{
	int ret;
	char buf[1024] = {0};
	while(1){
		memset(buf, 0, sizeof(buf));
	    ret = webSocket_recv(fd, buf, sizeof(buf));
		if(ret > 0)
	    {
	    	//printf("Recv: %s\n",buf);
			int content_len = strlen(buf)/2;
			if(content_len == 0) return -1;
			//printf("len:%d\n",content_len);
			unsigned char bites[content_len];
			bzero(bites,content_len);
			Change(bites,buf,content_len);
			int iii = 0;
			for(iii = 0;iii< content_len;iii++)
				printf("bites: %x\n",bites[iii]);
			if((bites[0] == 0x55) && (bites[1] == 0x02) && (bites[2] == 0x01) && (bites[9] == 0x1f) && (bites[10] == 0xff))
			{
				WebsocketRecv(bites);
			}
			else
			{	
				printf("Websocket Server Send Error...\n");
				continue;
			}
		}
		else
		{
			if(errno == EAGAIN || errno == EINTR)
		        ;
	        else
	        {
	            
				printf("Websocket Client Recv Error...\n");
	            break;
	        }
		}
	}
}

