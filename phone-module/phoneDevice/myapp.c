#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <mcp3422.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <math.h>
#include "com_arduino.h"

#define STU_NUM _IOW('c', 1, int)
int analogValue[5] = {
	0,
};

int main(int argc, char *argv[])
{
	int num;
	pthread_t tComArduino;
	pthread_create(&tComArduino, 0, com_arduino, 0);
	while (1)
	{
		for (num = 0; num < 5; num++)
			printf("master : %d, %d\n", num, analogValue[num]);
		sleep(1);
	}

	pthread_join(tComArduino, 0);

	/*
	int fd = open("/dev/mydev", O_RDWR);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	for(;;){
		printf("학생수 : ");
		scanf("%d",&num);
		ioctl(fd,STU_NUM,&num);
	}


	close(fd);
*/
}
