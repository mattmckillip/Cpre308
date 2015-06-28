/**
 * @file      malloc_test.c
 * @author    Jeramie Vens
 * @date      1/4/15
 * @brief     A simple program to show how Valgrind works
 * @detail    This program creates a buffer overflow and a memory leak to show
 *            how Valgrind can detect these errors
 * @copyright MIT License (c) 2014-2015
 */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char const *argv[])
{
	char* p = malloc(10); // malloc a buffer of 10 chars
	p[12] = '!'; // buffer overflow
	p = malloc(6); // memory leak
	p[2] = 'h'; // valid write
	free(p); // free the memory used by p
	return 0;
}