/**
 * @file      print_job.c
 * @author    Jeramie Vens
 * @date      2015-02-14: Created
 * @date      2015-02-14: Last updated
 * @brief     A print job which can be passed to a printer by the server
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
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "print_job.h"
char *str;
/**
 * @brief     Create a new print job object
 * @return    The new object
 */
print_job_t *print_job_create(void)
{
	static int number = 0;
	int file_number;
	// allocate the memory
	print_job_t *job = (print_job_t*) calloc(1, sizeof(print_job_t));
	if(!job)
	{
		fprintf(stderr, "%s: Failed to allocate new print job\n", __func__);
		exit(-1);
	}
	// create the temporary file name
	strncpy(job->input_file_name, "/tmp/" NETID "-printjob-XXXXXX.ps", 50);
	// open the file
	file_number = mkstemps(job->input_file_name, 3);
	if(file_number < 0)
	{
		perror("create_print_job");
		exit(-1);
	}
	// open the file stream
	job->file = fdopen(file_number, "w");
	if(!job->file)
	{
		fprintf(stderr, "%s: Failed to create new temporary file\n", __func__);
		exit(-1);
	}
	// save a job number to the job
	job->number = number;
	number++;
	// return the job
	return job;
}



/**
 * @brief     Destroy a print job object and clean allocated resources
 * @param     job
 *                 The print job to destroy
 * @return    NULL
 */
print_job_t* print_job_destroy(print_job_t *job) 
{
	//printf("Destroying print job\n");
	assert(job);
	// if the temp file is still open, close it
	job->file ? fclose(job->file) : 0;
	if(job->input_file_name)
	{
		// if the temp file still exists on the file system, remove it
		if(remove(job->input_file_name))
		{
			fprintf(stderr, "%s: Failed to remove file %s\n", __func__, job->input_file_name);
		}
	}
	// free the strings
	job->job_name ? free(job->job_name):0;
	job->description ? free(job->description):0;
	// free the job
	free(job);
	free(str);
	// return NULL
	return NULL;
}


/**
 * @brief     Format a print job into a nice string format
 * @param     job
 *                 The print job to convert to string
 * @return    A dynamically allocated string which MUST be freed after use.
 */
char* print_job_tostring(print_job_t* job)
{
	#warning not implemented
	// @todo fix
	str = malloc(sizeof(1000));
	//asprintf(&str, "Input file name: %s\nOutput file name: %s\nDriver name: %s\nJob name: %s\nDescription : %s\nJob time: %s\nJob number: %s\n",job->input_file_name, job->output_file_name, job->driver_name, job->job_name, job->description, job->arrival_time, job->number);
	asprintf(&str, "Input file name: %s\nOutput file name: %s\nDriver name: %s\nJob name: %s\nDescription : %s\nJob time: TODO\nJob number: %d\n",job->input_file_name, job->output_file_name, job->driver_name, job->job_name, job->description, job->number);
	return str;
}




