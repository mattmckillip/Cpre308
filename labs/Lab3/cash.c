#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{	
	/* check if there is an argument */
	if (argc < 2)
	{
		printf("No argument\n");
	}
	int count = 0;
        char const* const fileName = argv[1];
	char line[256];
	char *ch;
	char * pch;
	char *temp;
	int x = 0;
	char *slash = "/";
	char cwd[1024];
	int in_path = 0;
	int status = 0;
	int background_flag=0;
	int break_flag=0;
	
	//establish processes for opening executable and running background command
	pid_t background_process;

    	FILE* file = fopen(fileName, "r");
	if (file != NULL) 
	{
	    	while (fgets(line, sizeof(line), file)) 
		{
			/* Determine of script file */			
			if (count == 0)
			{
				/* break and return if the file isn't CASH */
				if(strcmp(line, "#! /bin/cash\n") != 0)
				{
					perror("Not a cash script file");
					exit(-1);
				}
			}
			
			/* throw out lines with no code */
			if(is_empty(line))
			{
				count += 1;
			}
			else{
				/* Remove the comments allowing partial line comments */
				for(x = 0; x < sizeof(line); x++)
				{
					if(line[x] == '#') 
					{
						line[x] = '\n';
						line[x + 1] = '\0';
						break;
				    	}
				}

				/* don't use the lines that were comments */
				if(!is_empty(line))
				{		
					//printf("ID: %d\n", background_process);
					if (ends_with(line, "&\n"))
					{
						background_process = fork();
						background_flag = 1;
					}
					
					if (starts_with(line, "cd"))
					{
						if ((background_process  > 0) || ((background_process == 0) && (background_flag))){
							/* remove 'cd ' from line */
							char *b = &line[3];
							/* remove new line character causing chdir error */
	 						b[ strlen(b) - 1 ] = '\0';

							chdir(b);
						}
					
					}
					else if (starts_with(line, "pwd"))
					{	
						if ((background_process>0) || ((background_process == 0) && (background_flag))){	
							/* save cwd to char cwd[1024] and print out to user */
							if (getcwd(cwd, sizeof(cwd)) != NULL)
							{
								printf("%s\n", cwd);
							}
						}
					}
					else if (starts_with(line, "export"))
					{
						if ((background_process>0) || ((background_process==0) && (background_flag))){
							export_parse(line);
						}
					}
					else if (starts_with(line, "echo"))
					{
						if ((background_process>0) || ((background_process==0) && (background_flag))){
							system(line);
						}
					}
					else
					{
						if ((background_process>0) || ((background_process==0) && (background_flag))){
	
							/* Attempt to open an executable in path or given path*/
							// given path 
							char *conatins = strstr(line, "/");
							if (conatins)
							{	
								open_given_path_exec(line);
							}
							//check path for executable
							else{
									
								open_path_exec(line);
							}
						}
						
					}
					
				}

				//wait for background prcess to end and print status
				if(background_process > 0 && background_flag){
					waitpid(background_process, &status, 0);
					printf("background_process is done, status is: %d\n", status);
					
				}
				
				background_flag=0;
				
			
				count += 1;
			}
	   	}

		fclose(file);
    	}
	else
	{
		perror("Invalid filename\n");
	}	
    return 0;
}


/* Helper method to check if a string is only whitespace */
int is_empty(const char *s)
{
	while (*s != '\0') 
	{
    		if (!isspace(*s))
      		return 0;
    		s++;
  	}
  	return 1;
}


/* Helper method to check if a starts with b */
int starts_with(const char *a, const char *b)
{
   	if(strncmp(a, b, strlen(b)) == 0) return 1;
   	return 0;
}

int ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

/* Helper method to handle export statements */
int export_parse(char *line)
{
	char * pch;
	char *name;
	char *value;
	char *temp;

	/* Split on whitespace to get the name=value string */
	pch = strtok (line," ");
	while (pch != NULL)
	{	temp = pch;
	    	pch = strtok (NULL, " ");
	}

	/* Get the name and the value for setenv call */
	name = strtok (temp,"=");
	value = strtok (NULL,"=");

	/* set environment variable with overwrite*/
	setenv(name, value, 1);
}

int open_path_exec(char *line)
{
	char seps[]   = ":";
	char *token;
	char *path;
	char str[80];
	int len;

	//remove /n from executable commant
	len = strlen(line);
	if( line[len-1] == '\n' )
	    line[len-1] = 0;

	// tokenize path checking for executable location
	path = getenv("PATH");

	token = strtok( path, seps );
	
	while( token != NULL )
		{

			//append the executable name at the end of path to fit arguments for execl
			strcpy (str, "");
			strcat(str, token);
			strcat(str, "/");
			strcat(str,line);
			
			//attempt to run
			execl(str, line, NULL);
			
			//Get next token: 
			token = strtok( NULL, seps );
	  	}
	
	
}


int open_given_path_exec(char *line)
{
	char seps[]   = "/";
	char *token;
	char *second_last_token;
	char str[80];
	int len;

	char first_arg[80];

	
	//remove /n from executable command
	len = strlen(line);
	if( line[len-1] == '\n' )
	    line[len-1] = 0;

	//copy line to save its value to use in the execl command
	strcpy(first_arg, line);

	// tokenize path checking for executable location
	token = strtok( line, seps );

	// get executable name after last '/'
	while( token != NULL )
		{
			//Get next token: 
			second_last_token = token;
			token = strtok( NULL, seps );
			
	  	}
	execl(first_arg, second_last_token, NULL);
	
}


