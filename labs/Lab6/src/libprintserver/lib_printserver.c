/**
 * @file      lib_hello.c
 * @author    Jeramie Vens
 * @date      2015-02-24: Last updated
 * @brief     Library example source
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
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include "lib_printserver.h"
#include "../print-server/printer.h"

#define DEBUG=1
/**
 * @brief     Send a print job to the print server daemon program.
 * @details   This function should send the given job to the print server program using
 *            your chosen method of IPC.
 * @param     handle
 *                 You may optionally implement this for extra credit.  If used this should return
 *                 a unique number that represents this print job and can be used to get information
 *                 about the job with other function calls.  If you choose not to implement this
 *                 you can safely ignore this param.
 * @param     driver
 *                 The name of the driver to print the job to.  Required.
 * @param     job_name
 *                 A name given to the job.  Required
 * @param     description
 *                 A discription of the job.  Optional, must handel being set to NULL if no
 *                 description is given.
 * @param     data
 *                 The actual print job to be printed in Postscript format.  Must start
 *                 with a "%" sign and end with "%EOF".
 * @return    This function should return 0 if the print server accepts the job as being valid.
 *            Note, this should return as soon as the print server accepts the job, but it should
 *            NOT wait for the server to finish printing the job.  It should return a number < 0
 *            on error.
 */
int printer_print(int* handle, char* driver, char* job_name, char* description, char* data){
	// Create message queue to send the information to the daemon printer
	mqd_t msg_queue = mq_open("/CprE308-Printer-Queue", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP, NULL);
	if (msg_queue == -1){
		perror("mq_open\n");
		return -1;
	}
	
	//build the string
	char to_send[256];
	char *delim;
	delim = "*";

	snprintf(to_send, sizeof to_send, "%s%s%s%s%s%s%s", driver, delim, job_name, delim,description, delim, data);

	// Send the data to the printer
	if( mq_send(msg_queue, to_send, strlen(to_send), 10)){
		perror("mq_send");
		return -1;
	}

	// Wait for message back
	// Initialize some buffers to receive the message
	struct mq_attr attr;
	char *buffer;
	if(mq_getattr(msg_queue, &attr)){
		perror("mq_getattr\n");
		return -1;
	}
	
	buffer = malloc(attr.mq_msgsize);
	if (buffer == NULL){
		perror("malloc");
		return -1;
	}

	int priority;
	// Receive the message
	size_t size = mq_receive(msg_queue, buffer, attr.mq_msgsize, &priority);
	if (size == -1){
		perror("mq_receive");
		return -1;
	}
	
	// Unlink message queue
	mq_unlink("/CprE308-Printer Queue");


#ifdef DEBUG
	printf("library buffer:%d\n", *buffer);
#endif
	return *buffer;
}



/**
 * @brief     List the currently installed printer drivers from the print server
 * @details   This function should query the print server for a list of currently installed drivers
 *            and return them as a NULL terminated array of printer_driver_t objects.
 * @param     number
 *                 Returns the number of printer drivers currently installed in the print server daemon
 * @return    An array of number printer_driver_t* objects followed by NULL
 * @example
 *
 * int num;
 * printer_driver* list[] = printer_list_driver(&num);
 * printf("printer_name=%s", list[0]->printer_name);
 *
 */
printer_driver_t** printer_list_drivers(int *number){
	// Create message queue to send the information to the daemon printer
	// Create message queue to send the information to the daemon printer
	mqd_t msg_queue = mq_open("/CprE308-Printer-Queue", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP, NULL);
	if (msg_queue == -1){
		perror("mq_open\n");
		return -1;
	}
	
	//build the string
	char to_send[256]="drivers";


	// Send the data to the printer
	if( mq_send(msg_queue, to_send, strlen(to_send), 10)){
		perror("mq_send");
		return -1;
	}

	// Wait for message back
	// Initialize some buffers to receive the message
	struct mq_attr attr;
	attr.mq_msgsize = 1024;

	printer_driver_t *buffer[5];
	if(mq_getattr(msg_queue, &attr)){
		perror("mq_getattr\n");
		return -1;
	}
	
	//buffer = malloc(sizeof(buffer));
	if (buffer == NULL){
		perror("malloc");
		return -1;
	}

	int priority;
	// Receive the message
	size_t size = mq_receive(msg_queue, buffer, attr.mq_msgsize, &priority);
	if (size == -1){
		perror("mq_receive");
		return -1;
	}
	
	// Unlink message queue
	mq_unlink("/CprE308-Printer Queue");

	// update the number of drivers
	number = sizeof(buffer)/sizeof(buffer[0])-1;
	return buffer;
}

