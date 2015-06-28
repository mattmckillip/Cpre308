#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define matrix_size 64

int array_a[matrix_size][matrix_size] = {{0}};
int array_b[matrix_size][matrix_size] = {{0}};
int array_c[matrix_size][matrix_size];

/*writes the data in array_c to C.dat in provided path*/
void writeMatrix(int mat[matrix_size][matrix_size], char* filename) {
  	int i,j;
	FILE *f;
    	f = fopen(filename, "w");
	
	if (f == NULL)
	{
	    printf("Error opening output file!\n");
	    exit(1);
	}

  	fprintf(f, "#N = %d\n", matrix_size);
  	for(i=0; i < matrix_size; i++){
    		for(j=0; j < matrix_size; j++)
     		 	fprintf(f, "%d ",  mat[i][j]);
    	fprintf(f, "\n");
  	}

	fclose(f);
}

/*reads data from input .dat files*/
void read_dat_file( char* filename, int arr[matrix_size][matrix_size]){
	char line[256];	
	char * pch;
	int test;
	int i = 0;
	int column_counter =0;
	int row_counter = 0;
	
	FILE* file = fopen(filename, "r");

	if (file == NULL)
	{
	    printf("Error opening input file!\n");
	    exit(1);
	}

	//ignore first line
	fgets(line, sizeof(line), file);

	//parse data
	fscanf (file, "%d", &i);    
	while (!feof (file)){  
		arr[row_counter][column_counter]=i;
		column_counter ++;
		if (0 == column_counter % matrix_size){	
			row_counter++;
			column_counter = 0;
		}			
		
	      	fscanf (file, "%d", &i);      
	}
	fclose (file);  
}


/*computes the vale of an entire row of the matrix*/
void *thread_worker(void *arg){
	int row = *((int *) arg);

	int i,j, sum = 0;

	for (i=0; i < matrix_size; i++){
		for (j=0; j < matrix_size; j++){
			sum += array_a[row][j]*array_b[j][i];
			
		}
		array_c[row][i] = sum;
		sum = 0;
	}
}


int main(int argc, char **argv){
	int err = 0;
	pthread_t *threads;
	int i, j;
  	int *p;

	// malloc size using the number of threads
	threads = (pthread_t *) malloc(matrix_size * sizeof(pthread_t));
	
	if (argc < 4)
	{
		printf("Not enough arguments");
		exit(-1);
	}
	/*
	argv[1] = first input file
	agrv[2] = second input file	
	argv[3] = path to output file
	*/
	read_dat_file(argv[1], array_a);
	read_dat_file(argv[2], array_b);
	
	char filename[100];
	strcpy (filename, "");
	strcat(filename, argv[3]);
	strcat(filename, "/C.dat");

	// start the threads
	for (i = 0; i < matrix_size; i++) {
		p = (int *) malloc(sizeof(int));
		*p = i;
	    	pthread_create(&threads[i], NULL, thread_worker, (void *)(p));
	}

	// join the threads
	for (i = 0; i < matrix_size; i++) {
		pthread_join(threads[i], NULL);
	}

	writeMatrix(array_c, filename);
	return 0;
}



