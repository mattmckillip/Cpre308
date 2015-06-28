#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char **argv)
{	
	pid_t child;
	child = fork();

	//check if the user has inputed a file
	if (argc < 2 && child){
		printf("Please input a file");
		return -1;
	}

	int status = 0;
	int i=0;
	char *filename = argv[1];
	char *extensions[] = {".doc", ".odt", ".png", ".txt", ".pdf", ".mp3"};
	char *programs[] = {"libreoffice", "libreoffice", "eog", "gedit", "evince", "vlc"};
	int flag = 0;
	char *input_extension = strrchr(filename, '.');

	//valid filetype
	if(child == 0){

		//check input filetype is valid
		for (i=0; i < 6; i++){
			if (strcmp(input_extension, extensions[i]) == 0){
				flag = 1;
			}
		}
		if (!flag){
			printf("anyopen does not support %s filetype", input_extension);
			return -1;
		}
		//child process, so open files
		for (i=0; i < 6; i++){
			if(ends_with(input_extension, extensions[i])){
				//start executable to run file
				execlp(programs[i],programs[i], filename , NULL);
				perror("Child process running  encountered an error");
				return -1;
			}
		}
		return -1;		
	}

	
	else if(child > 0){
		wait(&status);
		return 0;	
	}

	else{
		perror("fork");
		exit(-1);
	}
}

//helper method to determine if the filename ends with a supported filetype
int ends_with(const char* filetype, const char* extension)
{
	if(strcmp(filetype, extension) == 0)
	{
	    return 1;
	}
	return 0;
}
