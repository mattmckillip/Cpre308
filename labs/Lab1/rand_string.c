/**
 * @file      rand_string.c
 * @author    Jeramie Vens
 * @author    Kris Hall
 * @date      12/21/14 - Created
 * @date      01/19/15 - Finished
 * @brief     This file contains a broken program to be debugged with GDB.
 * @detail    This program reads a line from stdin, generates random numbers
 *			  that is then used to pull characters from random
 *			  parts of the read string
 * @copyright MIT License (c) 2014-2015
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int arg, char** argv)
{
	char* string = NULL;
	string = malloc(10);
	int rand_array[10]; // 10 random numbers
	int i, n;
	int length = 10;
	size_t j=0;

	printf("Enter a line: ");
	n = getline(&string, &j, stdin);
	printf("You entered the %d character line %s", n, string);

	// generate an array of random indexes
	for(i=0; i<=length-1; i++)
	{
		rand_array[i] = rand() % (n-1);
	}
	
	printf("The randomized string is \"");
	for(i=0; i<=length-1; i++)
	{
		putchar(string[rand_array[i]]);
	}
	printf("\"\n");

	return 0;
}
