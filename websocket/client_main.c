
#include "websocket_common.h"

char ip[] = "192.168.1.114";//"192.168.244.128";        // ����IP
int port = 9999;

int main(void)
{
    int ret, timeCount = 0;
    int fd;
    char buf[1024];
    //
    fd = webSocket_clientLinkToServer(ip, port, "/null");
    if(fd <= 0)
    {
        printf("client link to server failed !\r\n");
        return -1;
    }
    //
    sleep(1);
    //
    ret = webSocket_send(fd, "Hello !", strlen("Hello !"), true, WCT_TXTDATA);
    //
    printf("\r\n\r\n========== client start ! ==========\r\n\r\n");
    //
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        ret = webSocket_recv(fd, buf, sizeof(buf));
        if(ret > 0)
        {
            printf("client recv : len/%d %s\r\n", ret, buf);
            if(strstr(buf, "Hello") != NULL)
                ret = webSocket_send(fd, "I am Client_Test", strlen("I am Client_Test"), true, WCT_TXTDATA);
            else if(strstr(buf, "Server_Test") != NULL)
                ret = webSocket_send(fd, "I am carefree !", strlen("I am carefree !"), true, WCT_TXTDATA);
            else
                ;
            //
            if(ret <= 0)    // send���ظ���, �����ѶϿ�
            {
                close(fd);
                break;
            }
        }
        else    // ������, �Ƿ������ѶϿ�
        {
            if(errno == EAGAIN || errno == EINTR)
                ;
            else
            {
                close(fd);
                break;
            }
        }
        //
        delayms(10);
        timeCount += 10;
        //
        if(timeCount >= 4000)   /////////////////////////////////////////////////////////////////////////////  ÿ4s �ͻ��˿��������ﶨʱɧ��һ�·�����
        {
            timeCount = 0;
            ret = webSocket_send(fd, "#%^#@@@DTG%^&&+_)+(*^%!HHI", strlen("#%^#@@@DTG%^&&+_)+(*^%!HHI"), true, WCT_TXTDATA);
            if(ret <= 0)
            {
                close(fd);
                break;
            }
         }
    }
    printf("client close !\r\n");
    return 0;
}

