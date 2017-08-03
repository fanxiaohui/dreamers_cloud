#include "Param.h"

cJSON *root_config = NULL,*root_state= NULL;
//上锁
bool Locked_flag = false; 
//互斥锁
pthread_mutex_t mutex; 


int main()
{
	pthread_mutex_init(&mutex, NULL); 
	/*按键、LED初始化*/

	/*信号初始化*/


	/*读取配置文件，创建状态文件*/
	Config_init();
	
	
	/*创建两个线程1.Websocket-Client 2.TCP-Server*/
	pthread_t pid1,pid2;
	int err1 = pthread_create(&pid1,NULL,TCP_Server_pthread,NULL);
	int err2 = pthread_create(&pid2,NULL,Websocket_Client_pthread,NULL);
	pthread_join (pid2, NULL);
	pthread_join (pid1, NULL);


	cJSON_Delete(root_config);
	cJSON_Delete(root_state);
	pthread_mutex_destroy(&mutex); 
	return 0;
}


