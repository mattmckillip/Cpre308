
#include "stdio.h"
#include "stdlib.h"

float average(int* buffer, int count)
{
	int i;
	long sum;
	for(i=0;i<count;i++)
	{
		sum += buffer[i];
	}
	return (float)sum / (float)count;
}

int main(int argc, char const *argv[])
{
	FILE* fp;
	int i = 0, j;
	int rv;
	int *buffer;
	int buffer_size;
	if(argc != 2)
	{
		fprintf(stderr, "Usage:\n\t%s csv-file\n", argv[0]);
		return -1;
	}

	fp = fopen(argv[1], "r");
	if(fp == NULL)
	{
		perror("fopen");
		return -1;
	}

	buffer_size = 10;
	buffer = (int*)malloc(buffer_size*sizeof(int));

	while(!feof(fp))
	{
		if(i >= buffer_size)
		{
			buffer_size += 10;
			buffer = (int*)realloc(buffer, buffer_size*sizeof(int));
		}
		rv = fscanf(fp, "%d, ", &buffer[i++]);
	}

	printf("Average = %f\n", average(buffer, i));

	fclose(fp);
	return 0;
}
