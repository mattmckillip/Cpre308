/**
 * @file	isu_mem_req.c
 * @author	Kris Hall
 * @date	3/27/2015 - created
 * @brief	main code that is needed for memory requests
 */

#include <stdio.h>
#include <stdlib.h>
#include "llist/isu_llist.h"
#include "isu_mem_req.h"

/**
 * The main page request object type
 */
struct ISU_MEM_REQ_STRUCT{
	//the virtual address that is requested
	unsigned short mem_address;
	//whether or not the address requested is within the current set of pages
	char access_hit;
	//list of the current set of pages for data logging
	isu_llist_t pages;
	//request time
	unsigned long long req_time;
	//handled time
	unsigned long long handle_time;
};

/**
 * @brief	create a new memory request
 * @param	addr
 * 			the address that is to be requested
 * @return	the memory request or NULL if something horrible happens
 */
isu_mem_req_t isu_mem_req_create(unsigned short addr){
	isu_mem_req_t ret;
	ret = calloc(1, sizeof(struct ISU_MEM_REQ_STRUCT));
	ret->mem_address = addr;
	ret->pages = isu_llist_create();
	return ret;
}

/**
 * @brief	destroy a memory request object
 * @param	req
 * 			the memory request to destroy
 */
void isu_mem_req_destroy(isu_mem_req_t req){
	isu_llist_destroy(req->pages);
	free(req);
}
/**
 * @brief	gets the value of the variable `mem_address`
 * @param	req
 * 			the memory request to get the address from
 * @return	the value of `mem_address` stored in the variable req
 */
unsigned short isu_mem_req_get_address(isu_mem_req_t req){
	return req->mem_address;
}
/**
 * @brief	gets the value of the variable `access_hit`
 * @param	req
 * 			the page request to get the value of `access_hit` from
 */
char isu_mem_req_get_access_hit(isu_mem_req_t req){
	return req->access_hit;
}
/**
 * @brief	sets the value of the variable `access_hit`
 * @param	req
 * 			the page request to set the value of `access_hit`
 * @param	hit
 * 			whether or not the address requested is within the current set of pages
 */
void isu_mem_req_set_access_hit(isu_mem_req_t req, char hit){
	req->access_hit = hit;
}

/**
 * @brief	adds a page from the list of current pages to the data logging list of an isu_mem_req_t
 * @param	req
 * 			the memory request object to add the page to
 * @param	page_num
 * 			the page number to be added to the list
 */
void isu_mem_req_add_page(isu_mem_req_t req, int page_num){
	int *temp = calloc(1, sizeof(int));
	*temp = page_num;
	isu_llist_push(req->pages, (void *)temp, ISU_LLIST_TAIL);
}

/**
 * @brief	get the value of the variable `req_time`
 * @param	req
 * 			the page request to get the value of `req_time`
 * @return	the value of the time when the memory access is requested
 */
unsigned long long isu_mem_req_get_req_time(isu_mem_req_t req){
	return req->req_time;
}

/**
 * @brief	sets the value of the variable `req_time`
 * @param	req
 * 			the page request to set the value of `req_time`
 * @param	t
 * 			the value to set `req_time` to
 */
void isu_mem_req_set_req_time(isu_mem_req_t req, unsigned long long t){
	req->req_time = t;
}

/**
 * @brief	get the value of the variable `handle_time`
 * @param	req
 * 			the page request to get the value of `handle_time`
 * @return	the value of the time when the memory access is handled
 */
unsigned long long isu_mem_req_get_handle_time(isu_mem_req_t req){
	return req->handle_time;
}

/**
 * @brief	set the value of the variable `handle_time`
 * @param	req
 * 			the page request to set the value of `handle_time`
 * @param	t
 * 			the value to set `handle_time` to
 */
void isu_mem_req_set_handle_time(isu_mem_req_t req, unsigned long long t){
	req->handle_time = t;
}
/**
 * @brief	a
 * @param	req
 * 			the memory request object to grab page data from
 */
