#include <stdio.h>
#include <stdlib.h>
#include "lib_printserver.h"

int main(int argc, char** argv)
{
	int i;
	int* handle;
	char* driver;
	char* job_name;
	char* description;
	char* data;
	driver = "pdf1";
	job_name = "test1";
	description = "this is a description";
	data = "% start of this file \n new line \n new line \n new line \n%EOF \nstuff goes here";
	i = printer_print(handle, driver, job_name, description, data);
	printf("returned: %d\n", i);

	driver = "pdf2";
	job_name = "test2";
	description = "this is a description";
	data = "% start of this file \n new line \n new line \n new line \n%EOF \nstuff goes here";
	i = printer_print(handle, driver, job_name, description, data);
	printf("returned: %d\n", i);

	/*int *num;
	printer_list_drivers(num);*/


	return 0;
}
