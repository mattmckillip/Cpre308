/**
 * @file	isu_mem_req.h
 * @author	Kris Hall
 * @date	3/27/2015 - created
 * @date	3/28/2015 - finished
 * @brief	The main include file for page requests
 * @details	This file includes the memory request object type and the public task functions
 */

#ifndef ISU_MEM_REQ_H
#define ISU_MEM_REQ_H

typedef struct ISU_MEM_REQ_STRUCT *isu_mem_req_t;

isu_mem_req_t isu_mem_req_create(unsigned short addr);
void isu_mem_req_destroy(isu_mem_req_t req);
unsigned short isu_mem_req_get_address(isu_mem_req_t req);
char isu_mem_req_get_access_hit(isu_mem_req_t req);
void isu_mem_req_set_access_hit(isu_mem_req_t req, char hit);
void isu_mem_req_add_page(isu_mem_req_t req, int page_num);
unsigned long long isu_mem_req_get_req_time(isu_mem_req_t req);
void isu_mem_req_set_req_time(isu_mem_req_t req, unsigned long long t);
unsigned long long isu_mem_req_get_handle_time(isu_mem_req_t req);
void isu_mem_req_set_handle_time(isu_mem_req_t req, unsigned long long t);
char *isu_mem_req_to_json(isu_mem_req_t req);

#endif
