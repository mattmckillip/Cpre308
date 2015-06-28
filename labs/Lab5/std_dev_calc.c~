#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>

float Vals[1000];
pthread_mutex_t lock;

//typedef for the struct to be the argument for calcAverage
typedef struct{
	float **s;
	int tid;
}sumArgs;

//typedef for the struct to be the argument for calcStandardDev
typedef struct{
	float **s;
	float avg;
	int tid;
}stdArgs;

void *calcAverage(void *param){
	sumArgs *args = (sumArgs *)param;
	while(args->tid < 1000){
		pthread_mutex_lock(&lock);
		args->s[0][0] += Vals[args->tid];
		args->tid += 8;
		pthread_mutex_unlock(&lock);
	}
}

void *calcStandardDev(void *param){
	stdArgs *args = (stdArgs *)param;
	while(args->tid < 1000){
		pthread_mutex_lock(&lock);
		args->s[0][0] += powf((Vals[args->tid] - args->avg), 2.f);
		args->tid += 8;
		pthread_mutex_unlock(&lock);
	}
}


int main(int argc, char** argv){
	pthread_t threads[8];
	pthread_mutex_init(&lock, NULL);
	sumArgs *avgCalcArgs = malloc(8 * sizeof(sumArgs));
	
	if(avgCalcArgs == NULL){
		perror("Malloc encountered an error");
		exit(1);
	}

	stdArgs *stdCalcArgs = malloc(8 * sizeof(stdArgs));
	
	if(stdCalcArgs == NULL){
		perror("Malloc encountered an error");
		exit(1);
	}

	float *sum = malloc(sizeof(float));
	if(sum == NULL){
		perror("Malloc encountered an error");
		exit(1);
	}

	float avg;
	float e_sum = 0.f;
	float e_avg = 0.f;
	float e_std;
	int i;
	int err;

	sum[0] = 0.f;

	srand(7);

	for(i = 0; i < 1000; i++){
		Vals[i] = (float)(rand() % 15);
		printf("%f\n", Vals[i]);
	}

	for(i = 0; i < 8; i++){
		avgCalcArgs[i].s = &sum;
		avgCalcArgs[i].tid = i;
		err = pthread_create(&threads[i], NULL, calcAverage, (void *)&avgCalcArgs[i]);
		if(err != 0){
			errno = err;
			perror("pthread_create");
			exit(1);
		}
	}

	for(i = 0; i < 8; i++){
		err = pthread_join(threads[i], NULL);
		if(err != 0){
			errno = err;
			perror("pthread_join");
			exit(1);
		}
	}
	
	for(i = 0; i < 1000; i++){
		e_sum += Vals[i];
	}

	printf("e_sum: %f\tsum: %f\n", e_sum, sum[0]);

	avg = sum[0] / 1000.f;
	e_avg = e_sum / 1000.f;

	printf("e_avg: %f\t\tavg: %f\n", e_avg, avg);

	sum[0] = 0.f;
	e_sum = 0.f;
	for(i = 0; i < 8; i++){
		stdCalcArgs[i].s = &sum;
		stdCalcArgs[i].tid = i;
		stdCalcArgs[i].avg = avg;
		err = pthread_create(&threads[i], NULL, calcStandardDev, (void *)&stdCalcArgs[i]);
		if(err != 0){
			errno = err;
			perror("pthread_create");
			exit(1);
		}
	}

	for(i = 0; i < 8; i++){
		err = pthread_join(threads[i], NULL);
		if(err != 0){
			errno = err;
			perror("pthread_join");
			exit(1);
		}
	}

	for(i = 0; i < 1000; i++){
		e_sum += powf(Vals[i] - e_avg, 2.f);
	}

	printf("e_sum: %f\tsum: %f\n", e_sum, sum[0]);

	avg = sqrtf(sum[0] / 1000.f);
	e_avg = sqrtf(e_sum / 1000.f);

	printf("e_std: %f\tstd: %f\n", e_avg, avg);

	free(avgCalcArgs);
	free(stdCalcArgs);
	free(sum);

	pthread_mutex_destroy(&lock);

	return 0;
}

