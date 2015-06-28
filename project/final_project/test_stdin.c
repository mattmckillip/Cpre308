#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	char *buffer = NULL;
	int read;
	unsigned int len;
	read = getline(&buffer, &len, stdin);
	if (read != -1){
		printf("test_stdin Output: %s\n", buffer);
	}
	else{
		printf("NO line read ...\n");
	}
	//printf("Size read: %d\nlen: %d\b",read, len);
	free(buffer);
	return 0;
	


}
