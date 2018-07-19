#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <wiringPi.h>
#include <mcp3422.h>

#include "info.h"
#include "../include/parson.h"
#include "com_arduino.h"

#define GLOBAL	"global"
#define PID		"phone"
#define BUFFER_SIZE	1024
#define BASE 	50

#define STU_NUM _IOW('c', 1, int)

static int fd;
pthread_t tComArduino;
pthread_t tSegment;
static char pid_buffer[BUFFER_SIZE];
static char cmd_buffer[BUFFER_SIZE];
static int std_num=5;
static int student[5];
static int std_cur_num = 0;

int analogValue[5] = {
	        0,
};

enum CMD {
	HANDSHAKE = 0,
	GET_STATUS,
};

/*
 *
	input : full packet string(json format)
	sample : {"key1","value1" : "key2","value2"}
*/
int read_json(char* json_packet)
{
	int ret = 0;
	/*Init json object*/
	JSON_Value *rootValue;
	JSON_Object *rootObject;

	rootValue = json_parse_string(json_packet);
	rootObject = json_value_get_object(rootValue);

	strncpy(pid_buffer, json_object_get_string(rootObject, "pid"), BUFFER_SIZE);
	strncpy(cmd_buffer, json_object_get_string(rootObject, "cmd"), BUFFER_SIZE);

	if (strcmp(pid_buffer, PID) != 0 && strcmp(pid_buffer, GLOBAL) !=0) {
	//	printf("ERROR PID : %s\n",pid_buffer);
		ret = -1;
		goto OUT;
	}

	if (strcmp(cmd_buffer, "handshake") == 0) {
		ret = HANDSHAKE;
	}else if (strcmp(cmd_buffer, "get_status") == 0) {
		ret = GET_STATUS;
	}

	/*Get value function*/
	printf("[key : %s] [data : %s]\n", "pid", json_object_get_string(rootObject, "pid"));
	printf("[key : %s] [data : %s]\n", "cmd", json_object_get_string(rootObject, "cmd"));


OUT:
	/*free memory*/
	json_value_free(rootValue);

	/* APPENDIX
	//Get array value 
	int i;
	JSON_Array *array = json_object_get_array(rootObject, "key#");
	for(i=0; i<json_array_get_count(array); i++)
	{
	printf("%s\n", json_array_get_string(array, i);	
	}
	 */

	return ret;
}

/*
   make json packet & print
 */
void* setSegmentStuNum(void* arg)
{
	int i;
	while(1){
		std_cur_num = 0;
		for(i=0;i<std_num;i++)
		{
			if(analogValue[i] >= BASE)	
				std_cur_num++;
		}
	//	printf("std_cur_num : %d\n",std_cur_num);
		ioctl(fd,STU_NUM,&std_cur_num);
	}
}

void getStudentInfo(){
	int i, j;
	for(j=0; j<3; j++){
		for(i=0; i<std_num; i++) {
			if(j==0){
				if(analogValue[i]>=BASE) {
					student[i] = 1;
				}
				else student[i] = 0;
			}
			else {
				if(student[i]==1) {
					if(analogValue[i]<BASE)
						student[i] = 0;
				}
			}
		}
		//수정하기
		delay(100);
	}
}
void response(char *send_buf,int cmd)
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	char *buf;

	/*init empty json packet*/
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);

	int i;
	/*add key & value*/
	json_object_set_string(rootObject, "pid", pid_buffer);
	json_object_set_string(rootObject, "cmd", cmd_buffer);
	switch(cmd)
	{
		case HANDSHAKE: 
			json_object_set_string(rootObject, "module", PID);
			break;

		case GET_STATUS:
			getStudentInfo();
			json_object_set_number(rootObject, "std_num", std_num);
			for(i=0; i<std_num; i++){
				char a[10];
				sprintf(a,"std_%d",i+1);
				json_object_set_number(rootObject, a, student[i]); 
			}
			break;
	}

	/*get full string of json packet */
	buf =  json_serialize_to_string(rootValue);
	strncpy(send_buf, buf, BUFFER_SIZE);
	printf("result json : %s\n", buf);

	//free memory
	json_free_serialized_string(buf);
	json_value_free(rootValue);
}

void phone_module_init(struct info_t *info)
{
	printf("[%s] Phone Module is initialized!!!\n", __func__);
	fd = open("/dev/mydev", O_RDWR);
	if(fd == -1){
		perror("phone_module_init() open");
		exit(-1);
	}
	pthread_create(&tComArduino, 0 ,com_arduino, 0);
	pthread_create(&tSegment,0,setSegmentStuNum,0);
}

void phone_module(struct info_t *info)
{
	char send_buffer[BUFFER_SIZE];

	printf("[%s] RECV: %s!\n", __func__, info->receive_msg);

	int ret = read_json(info->receive_msg);
	if (ret < 0) {
		return;
	}

	switch (ret) {
		case HANDSHAKE:
			response(send_buffer,HANDSHAKE);
			info->send(info,send_buffer);
			break;
		case GET_STATUS:
			response(send_buffer, GET_STATUS);
			info->send(info, send_buffer);
			break;
		default:
			break;
	}
}

//exit : pthread_join(tComArduino, 0);
//         pthread_join(tSegment,0);
