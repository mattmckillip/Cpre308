#include <stdio.h>

#include <stdlib.h>
#include <stdint.h>


#define MATRIX_SIZE 64

int main(int argc, char** argv)
{
	int i, j, k;
	
	if(argc < 4)
	{
		printf("Call using\n\t%s A.dat B.dat C.dat\n", argv[0]);
		return -1;
	}

	FILE* A = fopen(argv[1], "r");
	FILE* B = fopen(argv[2], "r");
	FILE* C = fopen(argv[3], "w");
	
	char buffer[100];
	fgets(buffer, 100, A);
	fgets(buffer, 100, B);
	
	int a[MATRIX_SIZE][MATRIX_SIZE];
	int b[MATRIX_SIZE][MATRIX_SIZE];
	int c[MATRIX_SIZE][MATRIX_SIZE];	
	
	for(i=0;i<MATRIX_SIZE;i++)
	{
		for(j=0;j<MATRIX_SIZE;j++)
		{
			fscanf(A, "%d ", &a[i][j]);
			fscanf(B, "%d ", &b[j][i]);
		}
	}
	
	for(i=0;i<MATRIX_SIZE;i++)
	{
		for(j=0;j<MATRIX_SIZE;j++)
		{
			c[i][j]=0;
			for(k=0;k<MATRIX_SIZE;k++)
			{
				c[i][j] += a[i][k]*b[j][k];
			}
		}
	}
	
	fprintf(C, "N = %d\n", MATRIX_SIZE);
	for(i=0;i<MATRIX_SIZE;i++)
	{
		for(j=0;j<MATRIX_SIZE;j++)
		{
			fprintf(C, "%d ", c[i][j]);
		}
		fprintf(C, "\n");
	}
}




