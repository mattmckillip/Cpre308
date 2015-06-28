/**
 * @file	SMASH.c
 * @author	Matt McKillip and Lee Robinson
 * @date	4/08/2015 - Created
 * @brief	SMASH Shell
 */

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "SMASH_Helper.h"
#include "SMASH_Helper.c"

// Global Variables
char *running_programs;
int background_flag = 0;
int remove_pid = 0;
pid_t current_pid;
pthread_t t1;
char *pthread_program;

/**
 * @brief	Signal handler that prints when the process exited
 * @param	entry
 *				The signal to handle
 */
void handler(int sig)
{
	pid_t pid; 
	pid = wait(NULL); 
	if (1 || background_flag){
		printf("\n[Pid %d exited]\n", pid);
		fflush(stdout);
		background_flag = 0;
		remove_pid = 1;
		current_pid = pid;
	}
}

// Method to be run by pthread
// will execute the program stored in the pthread_program array
void runProcessInThread(void){
	int status = 0;
	pid_t child = fork();
	if(child == 0){
		setpgid(0, 0);
		char *args[] = {pthread_program, NULL};
		int returnVal = execvp(pthread_program, args);
		
		// If the return value from execvp wasn't 0, there was an error
		if(returnVal != 0){
			perror("runProcessInThread: Child process encountered an error");
			return -1;
		}
	}
	else{
		waitpid(child, &status, WNOHANG);
		char *formatted;
		asprintf (&formatted, "[%d] %s\n", child, pthread_program);
		strcat(running_programs, formatted);
		printf("[%d] %d %s from pthread\n", child, status, pthread_program);
	}

}

int main (int argc, char **argv)
{
    int runInBackground = 0;
    int status = 0;
    char *program_str;
	char pathname[MAXPATHLEN];
	pid_t child;
	running_programs = (char *) malloc(100);
	pthread_t t1;

    // Argument supplied
    if(argc > 1){
		printUsageMessage();
		exit(-1);
    }

	printf("SMASH-1.0$ ");
	char line[100];
	while (fgets(line, sizeof line, stdin)) {
		
		signal(SIGCHLD, handler);
		if(remove_pid == 1){

			char *pid_str;
			asprintf (&pid_str, "%s%d", "", current_pid);

			char buff[100];
			const char **p = &running_programs;
			char *new_running_programs;

			while(NULL!=sgets(buff, sizeof(buff), p)){

				if (strstr(buff, pid_str) != NULL){
					asprintf (&new_running_programs, "%s", "");
				}
				else{
					strcat(new_running_programs, buff);
				}
			}

			// Add the remainging processes back to the list
			asprintf (&running_programs, "%s", new_running_programs);
			remove_pid = 0;

		}

		// KILL COMMAND
		if (starts_with(line, "kill")){

			char *pid_str = line;
			pid_str += 5;
			pid_str = strtok(pid_str, "\n");
			pid_t PID = atoi(pid_str);

			kill(PID, SIGKILL);
	
		}

		// CD COMMAND
		else if (starts_with(line, "cd")){
			char* subLine = line;
			subLine += 3;
			subLine = strtok(subLine, "\n");
			chdir(subLine);
		}

		// PWD COMMAND
		else if (starts_with(line, "pwd")){
			char buff[100];
			size_t len = sizeof buff;
			getcwd(buff, len);
			printf(strcat(buff, "\n"));
		}

		// EXPORT COMMAND
		/*
		We should add support for variables. For example
		> Test="hello"
		> export Test
		> echo Test
			"hello"
		*/
		else if (starts_with(line, "export")){
			char* subLine = line;
			subLine += 7;
			char *ch;
			char *var;
			char *val;
			ch = strtok(subLine, "=");
			var = ch;
			ch = strtok(NULL, "=");
			val = ch;

			setenv(var,val,1);
		}

		// ECHO COMMAND
		else if (starts_with(line, "echo")){
			char *ln;
			ln = strtok(line, " ");
			
			while (ln != NULL) {
			if (strstr(ln, "echo") == NULL && strstr(ln, "$") == NULL){
				if (strstr(ln, "\n") != NULL){
					printf("%s", ln);
				}
				else{
				printf("%s ", ln);
				}
			}
			if (strstr(ln, "$") != NULL){
				ln = strtok(ln, "\n");
				ln++;
				printf("%s", getenv(ln));
			}
		  	ln = strtok(NULL, " ");
			}
		}

		// LESS COMMAND
		else if (starts_with(line, "less")){

			char* subLine = line;
			int less_command_buffer = 5;
			subLine += less_command_buffer;
			subLine = strtok(subLine, "\n");
			char ch;
			//read file

			FILE* file = fopen(subLine, "r");

			if( file != NULL )
			{
				while( ( ch = fgetc(file) ) != EOF )
			  		printf("%c",ch);
		    	fclose(file);
			}
			else{
				printf("%s: No such file or directory\n", subLine);
			}
		}

		// LS COMMAND
		else if (starts_with(line, "ls")){

			int count, i;
			struct direct **files;
		 
			if(!getcwd(pathname, sizeof(pathname))){
				perror("Error getting path name\n");	
				return -1;
			}

			count = scandir(pathname, &files, file_select, nameSort);
		 
			if(count <= 0){
				perror("No files in this directory\n");	
				return -1;
			}

			for (i = 1; i < count+1; ++i){
				printf("%s\n",files[i-1]->d_name);
			}			
		}

		// CLEAR COMMAND (Does't work when running program in background)
		else if (starts_with(line, "clear")){
			// Might need to do differently, throws PID of -1
			printf("\033[2J\033[1;1H");			
		}

		// PS COMMAND
		else if (starts_with(line, "ps")){
			printf(running_programs);		
		}

		// QUIT COMMAND
		else if (starts_with(line, "quit")){
			exit(1);		
		}
		

		// PIPING
		/*
		Handle >, >>, <, |
		*/
		// command1 | command2
		else if (strstr(line, "|") != NULL){
			char* substring = "|";
			//char* write_type = "w";
			// handle a space before and after the > operator
			if (strstr(line, " | ") != NULL){
				substring = " | ";
			}

			char* subLine = line;
			subLine = strtok(subLine, "\n");
			char *program1 = strtok(subLine, substring);
			char* program2 = strtok(NULL, substring);
			char *args1[] = {program1, NULL};
			char *args2[] = {program2, NULL};
			char *temp_file = "temp.txt";
			// fork to run the file
			pid_t pipe_pid = fork();
	
			if(pipe_pid == 0)
			{
				FILE* output_file = fopen(temp_file, "w");
				dup2(fileno(output_file), STDOUT_FILENO);
				
				fclose(output_file);
				execvp(program1, args1);
				//TODO handle bad program
			}
			else
			{
				waitpid(pipe_pid, &status, WNOHANG);
				// fork to run the file
				pid_t pipe_pid2 = fork();

				if(pipe_pid2 == 0)
				{
					FILE* input_file = fopen(temp_file, "r");
					dup2(fileno(input_file), STDIN_FILENO);
					execvp(program2, args2);
					//TODO handle bad program
				}
				else
				{
					// ...
				}
			}
		}
		//output to a file
		else if (strstr(line, ">") != NULL){
			char* substring = ">";
			char* write_type = "w";
			// handle a space before and after the > operator
			if (strstr(line, " > ") != NULL){
				substring = " > ";
			}
			// >> operator appends text to the file instead of overwriting
			else if (strstr(line, " >> ") != NULL){
				substring = " >> ";
				write_type = "a";
			}
			else if (strstr(line, ">>") != NULL){
				substring = ">>";
				write_type = "a";
			}

			char* subLine = line;
			subLine = strtok(subLine, "\n");
			char *program = strtok(subLine, substring);
			char* file = strtok(NULL, substring);
			char *args[] = {program, NULL};

			// fork to run the file
			pid_t pipe_pid = fork();

			if (pipe_pid == -1)
			{
				// ...
			}
			else if(pipe_pid == 0)
			{
				FILE* output_file = fopen(file, write_type);
				
				dup2(fileno(output_file), STDOUT_FILENO);
				fclose(output_file);
				execvp(program, args);
				//TODO handle bad program
			}
			else
			{
				// ...
			}
		}
		//input to a file
		else if (strstr(line, "<") != NULL){
			char* substring = "<";
			// handle a space before and after the > operator
			if (strstr(line, " < ") != NULL){
				substring = " < ";
			}

			char* subLine = line;
			subLine = strtok(subLine, "\n");
			char *program = strtok(subLine, substring);
			char* file = strtok(NULL, substring);
			char *args[] = {program, NULL};

			// fork to run the file
			pid_t pipe_pid = fork();

			if (pipe_pid == -1)
			{
				// ...
			}
			else if(pipe_pid == 0)
			{
				FILE* input_file = fopen(file, "r");
				dup2(fileno(input_file), STDIN_FILENO);
				fclose(input_file);
				execvp(program, args);
				//TODO handle bad program
			}
			else
			{
				waitpid(pipe_pid, &status, WNOHANG);
			}
		}


		// PROGRAM
		else if (starts_with(line, "./") || starts_with(line, "/")){
			int isThread = 0;
			
			// if the user specifies they want to run the process as a separate thread
			if (strstr(line, "-thread") != NULL){
				isThread = 1;
			}
			if(isThread){
				int err = 0;
				char* subLine = line;
				subLine = strtok(subLine, "\n");
				
				//remove -thread from the string. Now subLine only contains program
				int thread_buffer = 8;
				subLine[strlen(subLine)-thread_buffer] = 0;
				asprintf(&pthread_program, "%s", subLine);
				asprintf (&program_str, "%s", subLine);

				//run the pthread
				err = pthread_create(&t1, NULL, (void *)runProcessInThread, NULL);
				
				//check the output
				if(err != 0){
					perror("pthread_create encountered an error");
					exit(1);
				}else{
					err = 0;
				}
			}
			
			// run process in same thread
			else{
				char* subLine = line;
				subLine = strtok(subLine, "\n");
				runInBackground = 0;
			
				if (strstr(subLine, "&") != NULL){
					subLine = strtok(subLine, " ");
					runInBackground = 1;
					background_flag = 1;
				}

				asprintf (&program_str, "%s", subLine);
				child = fork();

				if(child == 0){
					break;
				}
			
				if(runInBackground == 0){
					wait(&status);
				}
				else{
					waitpid(child, &status, WNOHANG);
					printf("[%d] %d %s\n", child, status, program_str);

					char *formatted;
					asprintf (&formatted, "[%d] %s\n", child, program_str);
		
					strcat(running_programs, formatted);
				}
			}
		}

		else{
			char* subLine = line;
			subLine = strtok(subLine, "\n");
			if (subLine != NULL){
			
				printf("SMASH: %s: command not found\n", subLine);
			}
		}
		
		printf("SMASH-1.0$ ");
	}
	
    

	if(child == 0){
		setpgid(0, 0);
		char *args[] = {program_str, NULL};
		int returnVal = execvp(program_str, args);
	
		// If the return value from execvp wasn't 0, there was an error
		if(returnVal != 0){
			perror("Child process encountered an error");
			return -1;
		}
    }
    else{
        perror("fork");
		exit(-1);
    }
	
	free(running_programs);
    return 0;
}
