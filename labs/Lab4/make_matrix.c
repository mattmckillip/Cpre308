#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
	int size, seed;
	FILE* file;
	int i, j;
	if (!(argc > 2 && (size = atoi(argv[1])) && argv[2]))
	{
		printf("call as\n\t%s <matrix size> <seed> [out file]\n", argv[0]);
		exit(0);
	}
	seed = atoi(argv[2]);
	if (argc > 3)
	{
		file = fopen(argv[3], "w");
	}
	else
	{
		file = stdout;
	}
	
	srand(seed);
	
<<<<<<< HEAD
	fprintf(file,"#N = %d\n", size);
=======
	fprintf(file,"N = %d\n", size);
>>>>>>> lab5-dev
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			fprintf(file,"%d ", rand()%10);
		}
		fprintf(file,"\n");
	}
	fclose(file);
	return 0;
}
