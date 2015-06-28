/**
 * @file	SMASH_Helper.c
 * @author	Matt McKillip and Lee Robinson
 * @date	4/08/2015 - Created
 * @brief	Helper methods for SMASH.c
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SMASH_Helper.h"

int starts_with(const char *a, const char *b)
{
   	if(strncmp(a, b, strlen(b)) == 0) return 1;
   	return 0;
}

void printUsageMessage()
{
	printf("Argument supplied!\n\n");
	printf("usage: No arguments must be supplied to start SMASH shell\n");
	printf("The supported SMASH commands are:\n\n");
	printf("cd\tchange the directory\n");
	printf("pwd\tshows the present working directory\n");
	printf("export\tset environment variables\n");
	printf("echo\tprint output to shell\n");
	printf("ls\tview contents of current directory\n");
	printf("clear\tclear contents of screen\n");
	printf("ps\tview running processes\n");
	printf("kill\tkill running process by PID\n");
	printf("less\tquickly view contents of a file\n");
	printf("/path/\trun any program found in $PATH\n");
	printf("quit\texit shell\n\n");
}

int file_select(const struct direct *entry)
{
    if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)){
        return 0;
	}    
	else{
        return 1;
	}
}

int nameSort(const struct dirent** file1, const struct dirent** file2){

    char* a = (*file1) -> d_name;
    char* b = (*file2) -> d_name;
    return strcasecmp(a,b);
}
 

char *sgets(char *s, int n, const char **strp){
    if(**strp == '\0')return NULL;
    int i;
    for(i=0;i<n-1;++i, ++(*strp)){
        s[i] = **strp;
        if(**strp == '\0')
            break;
        if(**strp == '\n'){
            s[i+1]='\0';
            ++(*strp);
            break;
        }
    }
    if(i==n-1)
        s[i] = '\0';
    return s;
}
