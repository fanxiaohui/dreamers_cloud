#include <stdio.h>  
#include <stdlib.h>  
#include "cJSON.h"  
// 被解析的JSON数据包  
char text[] = "{\"timestamp\":\"2013-11-19T08:50:11\",\"value\":1}";  





/************获取json文件内容******************/
int main2()
{
	cJSON *root,*people,*name,*item;
	char *out;
	root = dofile("1.json");
	out = cJSON_Print(root);
	printf("root:\n%s\n",out);
	
	
	people = cJSON_GetObjectItem(root,"people");
	printf("people:\n%s\n", cJSON_Print(people));
	int len = cJSON_GetArraySize(people);
	printf("len:%d\n",len);//数组长度
	item = cJSON_GetArrayItem(people,0);
	printf("item0:\n%s\n", cJSON_Print(item));
	printf("\npeople->firstname:%s\n",cJSON_GetObjectItem(item,"firstname")->valuestring);
	printf("\npeople->firstname:%s\n",cJSON_GetObjectItem(item,"lastname")->valuestring);
	printf("\npeople->firstname:%d\n",cJSON_GetObjectItem(item,"email")->valueint);
	item = cJSON_GetArrayItem(people,1);
	printf("item1:\n%s\n", cJSON_Print(item));
	item = cJSON_GetArrayItem(people,2);
	printf("item2:\n%s\n", cJSON_Print(item));
	name = cJSON_GetObjectItem(root,"name");
	printf("name:\n%s\n", cJSON_Print(name));
	item = cJSON_GetArrayItem(name,0);
	printf("item0:\n%s\n", cJSON_Print(item));
	item = cJSON_GetArrayItem(name,1);
	printf("item1:\n%s\n", cJSON_Print(item));
	item = cJSON_GetArrayItem(name,2);
	printf("item2:\n%s\n", cJSON_Print(item));
	free(out);
	cJSON_Delete(root);
	return 0;
	
}

/*************增加json文件内容****************/
int main3()
{
	cJSON *root,*people,*name,*item;
	char *out;
	root = dofile("1.json");
	if(root != NULL){
		out = cJSON_Print(root);
		printf("root:\n%s\n",out);
		
		people = cJSON_GetObjectItem(root,"people");
		printf("people:\n%s\n", cJSON_Print(people));
		
		item = cJSON_GetArrayItem(people,0);
		printf("item:\n%s\n", cJSON_Print(item));
		cJSON *New = cJSON_CreateObject();
		cJSON_AddStringToObject(New,"firstname","brett");
		cJSON_AddStringToObject(New,"lastname","McLaus");
		cJSON_AddNumberToObject(New,"email",200);
		cJSON_ReplaceItemInArray (people,0,New);
	
		out = cJSON_Print(root);
		FILE* fp = fopen("1.json","wb");
		if(fp == NULL){
			perror("open filename ");
			return -1;					
		}
		int writelen = fwrite(out,sizeof(char),strlen(out),fp);			
		if(writelen < 0){
			perror("fwrite ");
			return -1;
		}
	}
	return 0;
}

int main4()
{
	char buf[100] = {0};
	get_cjson_Array_valueString("./Control_bad.json","pr",0,"name",buf);
	printf("get:%s\n",buf);

     return 0;
}

int main11(void)
{
	cJSON *root;
	root = stringFromJson("./Control_bad.json");
	if(root == NULL){
		printf("Read File Error\n");
		//return -1;
	}
	
	//printf("out:\n%s\n",cJSON_Print(root));
	
	char *value_str = ReadJsonString(root,"we",1,"name");
	if(value_str == NULL){
		printf("Read Json Value_string Error\n");
		//return -1;
	}else
		printf("value_str: %s\n",value_str);
	
	int value_int = ReadJsonInt(root,"we",0,"name");
	if(value_int == -1){
		printf("Read Json Value_int Error\n");
	}else
		printf("value_int: %d\n",value_int);
	
	if(WriteJsonInt(root,"we",0,"name",1) == -1){
		printf("Write Json Int Error\n");
	}
	//printf("out:\n%s\n",cJSON_Print(root));
	
	//CreatestateJson(root,"sp");
	//printf("out:\n%s\n",cJSON_Print(root));
	
	value_int = ReadJsonInt(root,"sp",1,"adadaaaaaa");
	if(value_int == -1){
		printf("Read Json Value_int Error\n");
	}else
		printf("value_int: %d\n",value_int);
	return 0;
}

int main(void)
{
	cJSON *root;
	root = stringFromJson("./Control_bad.json");
	if(root == NULL){
		printf("Read File Error\n");
		//return -1;
	}
	printf("out:\n%s\n",cJSON_Print(CreatestateJson(root)));
}











































