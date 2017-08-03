#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <linux/tty.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
	int fd;
	int ret;
	char *name = "hello,world\n";
	char buf[1024];
	fd = open("/dev/ttyUSB0",O_RDWR);
	if(fd < 0){
		printf("---open serial failed \n");
		return 0;
	}
	write(fd,name,strlen(name));
	write(fd,name,strlen(name));
	while(1)
	{
		ret = read(fd,buf,1024);
		buf[ret] = '\0';
		printf("%d,%s\n",ret,buf);
		
	}
	close(fd);
	return 0;
}

