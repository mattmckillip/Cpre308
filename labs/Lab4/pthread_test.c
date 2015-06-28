#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

void thread1Print(void){
	printf("I am thread 1\n");
}

void thread2Print(void){
	printf("I am thread 2\n");
}

int main(int argc, char **argv){
	int err = 0;
	pthread_t t1;
	pthread_t t2;

	err = pthread_create(&t1, NULL, (void *)thread1Print, NULL);
	if(err != 0){
		perror("pthread_create encountered an error");
		exit(1);
	}else{
		err = 0;
	}

	err = pthread_create(&t2, NULL, (void *)thread2Print, NULL);
	if(err != 0){
		perror("pthread_create encountered an error");
		exit(1);
	}else{
		err = 0;
	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("I am thread 0\n");
	return 0;
}
