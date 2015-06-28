#include <print_server_client.h>

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


	driver = "pdf1";
	job_name = filename;
	description = "sent from cli-printer";
	
	//read file
	FILE* file = fopen(fileName, "r");
	if (file != NULL) 
	{
	    	while (fgets(line, sizeof(line), file)) 
		{
			
			asprintf(&data, line);
		}
	}
	printf("line: %s\n");
	fclose(file);

	i = printer_print(handle, driver, job_name, description, data);

}
