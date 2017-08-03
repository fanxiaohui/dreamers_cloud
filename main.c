#include "Param.h"

cJSON *root_config = NULL,*root_state= NULL;
//����
bool Locked_flag = false; 
//������
pthread_mutex_t mutex; 


int main()
{
	pthread_mutex_init(&mutex, NULL); 
	/*������LED��ʼ��*/

	/*�źų�ʼ��*/


	/*��ȡ�����ļ�������״̬�ļ�*/
	Config_init();
	
	
	/*���������߳�1.Websocket-Client 2.TCP-Server*/
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


