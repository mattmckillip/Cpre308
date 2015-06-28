/**
 * @file      part-b.c
 * @author    Jeramie Vens
 * @date      12/21/14 - Created
 * @brief     This file contains a broken program to be debugged with GDB.
 * @detail    This program allocates a buffer, reads upto 1024 characters from
 *			  STDIN and then prints the buffer to STDOUT.
 * @copyright MIT License (c) 2014-2015
 */

/// Include standard i/o and standard library
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
	char *buffer; // pointer to the buffer

	buffer = malloc(1<<31); // allocate a new buffer

	fgets(buffer, 1024, stdin); // get upto 1024 characters from STDIN
	printf("%s\n", buffer); // print the buffer to STDOUT

	return 0;
}