/**
 * @file	isu_mmu.h
 * @author	Kris Hall
 * @date	3/28/2015 - created
 * @date	3/29/2015 - overhaul, changed name, finished
 * @brief	header file for main memory class, emulates some functionality of a modern memory system(in terms of hierarchy)
 */

#ifndef ISU_MMU_H
#define ISU_MMU_H

#include "common/isu_types.h"
#include "page_req/isu_mem_req.h"

/**
 * @class	isu_mmu_t
 * @brief	The main memory object
 * @details	The main memory object that emulates the hierarchy and latency of memory of a more modern system
 * 		Assuming that the page size is 4KB and a 16-bit memory space(what is this, 1985?)
 */
typedef struct ISU_MMU_STRUCT *isu_mmu_t;

/**
 * @brief	constructs a new main memory object
 * @param	mode
 * 			
 * @return	the main memory of the test system or NULL if a failure occurs
 */
isu_mmu_t isu_mmu_create(int mode);

/**
 * @brief	destroys the main memory object
 * @param	mem
 * 			the main memory to be destroyed
 */
void isu_mmu_destroy(isu_mmu_t mem);

/**
 * @brief	handles a memory request
 * @param	mem
 * 			main memory to check in
 * @param	req
 * 			The page to check if in memory
 * @param	t
 * 			the current time when the memory request is to be handled
 * @return	0:
 * 			request handled successfully
 * @return	-1:
 * 			an error occured in page request handling
 */
int isu_mmu_handle_req(isu_mmu_t mem, isu_mem_req_t req, unsigned long long *t);

/**
 * @brief clears the reference bit for all pages
 * @param	mem
 * 			main memory to clear reference bit
 * @return	0:
 * 			reference bits cleared correctly
 */
int isu_mmu_ref_clear(isu_mmu_t mem);
#endif
