/**
 * @file      main.c
 * @author    Jeramie Vens
 * @date      2015-02-11: Created
 * @date      2015-02-15: Last updated
 * @brief     Emulate a print server system
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

// lab1: print server program as it is
// lab2: make as a daemon, install different printer backends
// lab3: write a kernel module that gets installed as a backend

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <semaphore.h>
#include "queue.h"
#include "print_job.h"
#include "printer.h"
#include <argp.h>
int starts_with(const char *a, const char *b);
int printer_print(printer_t printer, print_job_t* job);

/// program version string
const char *argp_program_version = "ISU CprE308 Print Server 0.1";
/// program bug address string
const char *argp_program_bug_address = "Jeramie Vens: <vens@iastate.edu>";
/// program documentation string
static char doc[] = "Print server -- For my class\vThis should be at the bottom";

//const int N1_KEY = 1;
//const int N2_KEY = 2;
// list of options supported
static struct argp_option options[] = 
{
	{"verbose", 'v', 0, 0, "Produce verbose output"},
	{"version", 'V', 0, 0, "display version information and exit"},
	{"n1", 1, "NUM1", 0, "the number of print queue 1 printers there are"},
	{"n2", 2, "NUM2", 0, "the number of print queue 2 printers there are"},
	{"quiet", 'q', 0, 0, "Don't produce any output"},
	{"silent", 's', 0, OPTION_ALIAS, 0},
	{"log-file", 'o', "FILE", 0, "The output log file"},
	{0}
};

/// arugment structure to store the results of command line parsing
struct arguments
{
	/// are we in verbose mode?
	int verbose_mode;
	/// name of the log file
	char* log_file_name;
	// The student should add anything else they wish here
	//...
	//number of pdf1 printers
	int n1;
	//number of pdf2 printers
	int n2;
};


/**
 * @brief     Callback to parse a command line argument
 * @param     key
 *                 The short code key of this argument
 * @param     arg
 *                 The argument following the code
 * @param     state
 *                 The state of the arg parser state machine
 * @return    0 if succesfully handeled the key, ARGP_ERR_UNKONWN if the key was uknown
 */
error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	switch(key)
	{
		case 'v':
			arguments->verbose_mode = 2;
			break;
		case 'V':
			arguments->verbose_mode = 1;
			break;
		case 'q':
			arguments->verbose_mode = 0;
			break;
		case 'o':
			arguments->log_file_name = arg;
			break;
		case 1:
			arguments->n1 = atoi(arg);
			break;
		case 2:
			arguments->n2 = atoi(arg);	
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/// The arg parser object
static struct argp argp = {&options, parse_opt, 0, doc};

/// Parameters to pass to the print spooler thread
struct PRINTER_SPOOLER_PARAM
{
	/// The print queue that this spooler can pull off of
	queue_t print_queue_list;
	/// The printer that this spooler can print to
	printer_t printer_driver;
	//struct containing arguments from main
	struct arguments arguments;
};

/**
 * @brief     This thread represents a print spooler which directly talks to a single printer
 * @param     param
 *                 The PRINT_SPOOL_PARAM this printer should pull its jobs from, casted to a void*
 * @return    NULL
 *
 * This function should loop forever.  The basic process is as follows:
 * 1) pop a print job from this threads queue
 * 2) if it is NULL return
 * 3) print to the log file that printing of the job is starting
 * 4) call the print method on the printer object of the param and check the return
 * 5) handel errors correctly
 * 6) destroy the print job and get the next
 */
void *spooler(void* param)
{
	struct PRINTER_SPOOLER_PARAM* params =  malloc(sizeof(struct PRINTER_SPOOLER_PARAM));
	params = (struct PRINTER_SPOOLER_PARAM*)param;
	queue_t queue = params->print_queue_list;
	printer_t printer = params->printer_driver;
	struct arguments arguments = params->arguments;

	queue_ends_t q_ends = QUEUE_HEAD;
	print_job_t* popped;
	FILE *file;
	char *str;
	while(1){
		popped = queue_pop(queue,  q_ends);
		if (popped == NULL){	
			printer_uninstall(printer);
			return NULL;
		}
	
		file = fopen(arguments.log_file_name, "a");

		if(file == NULL){
			fprintf(stderr, "%s: error opening log file\n", __func__);	
			exit(-1);
		}
		asprintf(&str, "Starting to print job %s\n", popped->job_name);
		fputs(str,file);
		fclose(file);
		printer_print(printer, popped);
	}
}



/**
 * @brief     A print server program
 * This program shall take postscript files with some special header information from stdin
 * and print them to a printer device.  For Lab 5 the only printer device you need to support
 * is the pdf_printer provided.  Keep in mind that in future labs you will be expected to
 * support additional printers, so keep your code modular.  All printers will support the
 * API shown in printer.h.  
 *
 * The program should take a number of command line arguments.  At a minimum the following
 * should be supported:
 * - -?, --help: display help information and exit
 * - -V, --version: display version information and exit
 * - -v, --verbose: display debugging information to stderr
 * - -q, --quiet: don't display any messages or outputs unless it is a fatal error
 * - -o, --log-file: use the given log file to print out when jobs start and finish printing
 * - -d, --daemon: future lab will implement this
 * - -c, --config: future lab will implement this
 * - -p, --printer: future lab will implement this
 * - -n1: the number of print queue 1 printers there are (future lab will remove this)
 * - -n2: the number of print queue 2 printers there are (future lab will remove this)
 *
 * The syntax of the postscrip file is as follows.  The file will be suplied through stdin for
 * this lab.  A future lab will change this to a different location, so keep in mind modularity
 * as you go.  Each job will start with header information.  Each line of header information
 * will start with a `#` followed by a keyword and an `=` sign.  You must support at minimum
 * the following commands
 * - #name=: The name of the print job.
 * - #driver=: The driver to print the job to.  For Lab 5 this will be either "pdf1" or "pdf2".
 * - #description=: A discription of the print job which will be included in the log file
 * After all of the header information will be a line starting with a `%`.  Any line following
 * from that line to the line containing `%EOF` should be interpreted as raw postscript data.
 * It should therefore all be copied into the `print_job->file` file.
 *
 * After the `%EOF` has been found a new header may begin for the next file, or if the program
 * is finished `#exit` will be supplied.
 *
 * The flow of the program should be as follows:
 * -# parse command line arguments
 * -# create two print queues using the `queue_create()` function
 * -# install n1 pdf_printer drivers called "pdf1-%d" and n2 pdf_printer drivers called "pdf2-%d"
 * -# create n1+n2 spooler param objects: the first n1 with one of the print queues and all the n1
 *    drivers, and the other n2 with the other print queue and all the n2 drivers
 * -# create n1+n2 spooler threads
 * -# create a new print job using `print_job_create()`
 * -# parse stdin to build the print job
 * -# all postscript data should be appended to the `print_job->file` file
 * -# when the entire job has been read the `print_job->file` file should be closed
 * -# push the print job onto the correct print queue (if the driver was pdf1 or pdf2)
 * -# parse the next print job
 * -# when `#exit` is recieved make sure to release all threads and join them
 * -# free all resources and exit
 *
 * When the program is run with valgrind it should not have ANY memory leaks.  The program
 * should also never cause a segfault for any input or reason.
 */
int main(int argc, char* argv[])
{
	// parse arguments.  Look at the man pages and section 25.3 of the GNU libc manual
	// found at https://www.gnu.org/software/libc/manual/pdf/libc.pdf for more information
	// on how to use argp to parse arguments.  An example is shown below to get you started
	struct arguments arguments;
	arguments.verbose_mode = 1;
	arguments.log_file_name = "";
	argp_parse(&argp, argc, argv, 0, 0, &arguments);
	int n1;
	int n2;
	// The student should fill in all of this section
	n1 = arguments.n1;
	n2 = arguments.n2;
	

	//create two print queues
	queue_t queue1 = queue_create();
	queue_t queue2 = queue_create();
	//printf("THIS SHOULD PRINT QUEUE COUNT: %s\n",queue2->count);

	char *printer_name;
	printer_name = malloc(sizeof(1000));
	int i = 0;

	//create and install pdf_printer drivers
	printer_t printers[n1+n2];
	
	for (i = 0; i < n1; i++){
		asprintf(&printer_name, "pdf1-%i", i);
		printers[i] = printer_install(NULL, printer_name);

	}

	for (i = n1; i < n1+n2; i++){
		asprintf(&printer_name, "pdf2-%i", i-n1);
		printers[i] = printer_install(NULL, printer_name);
	}

	//create spooler threads
	i = 0;
	int err;
	struct PRINTER_SPOOLER_PARAM *params; 
	
	pthread_t threads[n1+n2];
	
	for (i = 0; i < n1; i++){
		params = malloc(sizeof(struct PRINTER_SPOOLER_PARAM));
		(*params).print_queue_list=queue1;
		(*params).printer_driver=printers[i];
		(*params).arguments=arguments;
		err = pthread_create(&threads[i], NULL, spooler,(void*) params);
		if(err != 0){
			errno = err;
			perror("pthread_create");
			exit(1);
		}
	}

	for (i = n1; i < n1+n2; i++){
		params = malloc(sizeof(struct PRINTER_SPOOLER_PARAM));
		(*params).print_queue_list=queue2;
		(*params).printer_driver=printers[i];
		(*params).arguments=arguments;
		err = pthread_create(&threads[i], NULL, spooler, (void*) params);
		if(err != 0){
			errno = err;
			perror("pthread_create");
			exit(1);
		}
	}

	int start_file = 0;
	int found_driver = 0;
	int found_description = 0;
	int found_name = 0;
	int header_complete = 0;
	int exit = 0;
	char *buffer = NULL;
	size_t len = 1000;
	char * name;
	char * driver;
	char * description;
	FILE *file ;

	queue_ends_t q_ends = QUEUE_HEAD;
	print_job_t* p_job = print_job_create();

	//read input from stdin until #exit is called
	while(!exit){
		getline(&buffer, &len, stdin);
		if(starts_with(buffer, "#exit")){
			exit=1;
			break;
		}
		if (!header_complete){
			if(starts_with(buffer, "#name=")){
				//remove everything before '=:' and the \n
				name = strtok (buffer,"=");
				name = strtok (NULL, "=");
				name[ strlen(name) - 1 ] = '\0';
				
				p_job->job_name = malloc(sizeof(name));
				strcpy(p_job->job_name, name);

				found_name = 1;
				if (found_name && found_driver && found_description){
					header_complete = 1;
				}
			}
			else if(starts_with(buffer, "#device=")){
				//remove everything before '=:' and the \n
				driver = strtok (buffer,"=");
				driver = strtok (NULL, "=");
				driver[ strlen(driver) - 1 ] = '\0';
					
				p_job->driver_name = malloc(sizeof(driver));
				strcpy(p_job->driver_name, driver);

				found_driver = 1;
				if (found_name && found_driver && found_description){
					header_complete = 1;				
				}
			}
			else if(starts_with(buffer, "#description=")){
				//remove everything before '=:' and the \n
				description = strtok (buffer,"=");
				description = strtok (NULL, "=");
				description[strlen(description)-1]='\0';
		
				p_job->description = malloc(sizeof(description));
				strcpy(p_job->description, description);

				found_description = 1;
				if (found_name && found_driver && found_description){
					header_complete = 1;
				}
			}
		}
		else{
			if(starts_with(buffer,"%") ){
				if (start_file==0){
					//printf("Filename: %s\n",p_job->input_file_name);
					file = fopen(p_job->input_file_name, "a");
					if(file == NULL){
						printf("error opening file\n");	
					}
				}				
				start_file = 1;				
			}
			if(start_file){
				header_complete=0;
				found_description = 0;
				found_driver = 0;
				found_name = 0;

				while(1){				
					getline(&buffer, &len, stdin);
					if(starts_with(buffer, "#exit")){
						exit=1;
						break;
					}
					// we have reached the end of the file
					if (starts_with(buffer,"%EOF")){
						//fclose(file);
						// push to correct queue
						if (starts_with(p_job->driver_name, "pdf1")){
							queue_push(queue1, p_job, q_ends);
						}
						else if (starts_with(p_job->driver_name, "pdf2")){
							queue_push(queue2, p_job, q_ends);
						}
						else{
							printf("ERROR with driver");
						}
					
						//start new print job
						p_job = malloc(sizeof(print_job_t));
						p_job = print_job_create();
						start_file=0;
						break;
						
					}
					else{
						fputs(buffer,file);				
					}
				}
			}		
		}	
	}
	queue_release_threads(queue1, n1+n2);
	queue_release_threads(queue2, n1+n2);

	for (i=0; i< n1+n2;i++){
		pthread_join(threads[i], NULL);
	}
	queue_destroy(queue1);
	queue_destroy(queue2);
	//print_job_destroy(p_job);
	return 0;
}

/* Helper method to check if a starts with b */
int starts_with(const char *a, const char *b)
{
   	if(strncmp(a, b, strlen(b)) == 0) return 1;
   	return 0;
}








