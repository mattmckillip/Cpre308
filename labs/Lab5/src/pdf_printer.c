/**
 * @file      pdf_printer.c
 * @author    Jeramie Vens
 * @date      2015-02-13: Created
 * @date      2015-02-14: Last updated
 * @brief     A virtual printer for the print sever program
 * @copyright MIT License (c) 2015
 */
 
/*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include "print_job.h"
#include "printer.h"


/// The name of this driver
const char* DRIVER_NAME = "pdf_printer";
/// The version of this driver
const char* DRIVER_VERSION = "0.1.0";

// prototypes
int printer_uninstall(printer_t printer);
int printer_print(printer_t printer, print_job_t* job);

// install a pdf printer
printer_t printer_install(void* future, char* printer_name)
{
	assert(printer_name);
	printer_t printer;	
	
	if((printer = (printer_t) calloc(1, sizeof(struct PRINTER_STRUCT))))
	{
		printer->printer_name = printer_name;
		printer->driver_name = DRIVER_NAME;
		printer->driver_version = DRIVER_VERSION;
		printer->print = printer_print;
		printer->uninstall = printer_uninstall;
		if(mkdir(printer_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) && errno != EEXIST)
		{
			fprintf(stderr, "%s: %s: failed to create output directory %s\n", DRIVER_NAME, __func__, printer_name);
		}
	}
	else
	{
		fprintf(stderr, "%s: %s: failed to allocate printer\n", DRIVER_NAME, __func__);
		exit(-1);
	}
	return printer;
}

/**
 * @brief     Uninstall this printer driver and clean up resources.
 * @param     printer
 *                 This printer driver
 * @return    0
 */
int printer_uninstall(printer_t printer)
{
	assert(printer);
	printer->printer_name ? free(printer->printer_name) : 0;
	free(printer);
	return 0;
}

/**
 * @brief     Convert the file from postscript format to portable display file format.
 * @param     printer
 *                 This printer object
 * @param     job
 *                 The print job to print
 * @return    0 on success, the exit status of ps2pdf on failure.
 */
int printer_print(printer_t printer, print_job_t* job)
{
	assert(printer);
	assert(job);
	
	// build the output file name
	char out_name[100];
	snprintf(out_name, 100, "%s/%s-%d.pdf", printer->printer_name, job->job_name, job->number);
	printf("%s\n",out_name);
	char* arg_list[] = {"ps2pdf", job->input_file_name, out_name, NULL};

	// fork the process
	pid_t child = fork();
	if(child == 0)
	{
		// the child should do the printing...
		// sleep because otherwise this goes to fast to see the effects of threads
		// but if it was a real printer there would be a delay anyway
		sleep(10);	
		
		// call the printer
		execvp(arg_list[0], arg_list);
		fprintf(stderr, "%s: %s: failed to exec ps2pdf\n", DRIVER_NAME, __func__);
		exit(-1);
	}
	else
	{
		int status;
		// wait for printing to finish and record the results
		waitpid(child, &status, 0);
		// return the exit status
		printf("waited for %s\n", out_name);
		return status;
	}	
}


