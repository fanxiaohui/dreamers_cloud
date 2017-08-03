#include "Config_init.h"

extern cJSON *root_config,*root_state;

int Config_init()
{
	root_config = stringFromJson(config_filename);
	if(root_config == NULL){
		printf("Read root_config Error\n");
		return -1;
	}
	//根据配置文件创建状态文件
	root_state = CreatestateJson(root_config);
	if(root_state == NULL){
		printf("Read root_state Error\n");
		cJSON_Delete(root_config);
		return -1;
	}	
	return 0;
}

