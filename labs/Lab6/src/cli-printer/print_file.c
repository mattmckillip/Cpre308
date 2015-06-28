#include <mqueue.h>
#include "print_server_client.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if (argc < 2){
		printf("Please input a file");
		return -1;
	}
	char *filename = argv[1];
	int* handle;
	char* driver;
	char* job_name;
	char* description;
	char* data;
	char line[256];
	
	data = "";
	driver = "pdf1";
	job_name = filename;
	description = "sent from cli-printer";
	
	//read file
	FILE* file = fopen(filename, "r");
	if (file != NULL) 
	{
	    	while (fgets(line, sizeof(line), file)) 
		{
			asprintf(&data, "%s%s", data, line);
		}
	}
	fclose(file);
	int printer_return;
	printer_return = printer_print(handle, driver, job_name, description, data);
	if (printer_return == 0){
		printf("success\n");
	}
	else{
		printf("failure\n");
	}

}
