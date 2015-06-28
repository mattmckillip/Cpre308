#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "llist/isu_llist.h"
#include "page_req/isu_mem_req.h"
#include "isu_mmu/isu_mmu.h"
#include "common/isu_types.h"
#include "common/isu_error.h"

struct TEST_FRAMEWORK{
	/// list of memory requests
	isu_llist_t mem_list;
	/// the current time
	unsigned long long current_time;
	/// the number of misses
	int misses;
};

struct TEST_FRAMEWORK *init_test_framework(int pattern);
void run_test_framework(struct TEST_FRAMEWORK *f, int mode, isu_mmu_t MMU);
void print_test_framework(struct TEST_FRAMEWORK *f, char *name);
void destroy_test_framework(struct TEST_FRAMEWORK *f);

int main(int argc, char **argv){
	/// create frame work
	/// 	create the list of memory requests
	/// 	create isu_mmu
	/// run the framework
	///	while the list of memory requests is not empty
	///		if the current time less than the time the request is handled
	///			increment the current time by 10
	///		when the current time is not less than the time the request in handled
	///			pop a memory request off the list
	///			handle the request
	///			update the value of request handled time
	///
	int i;
	int mode;
	int pattern;
	char *name = calloc(25, sizeof(char));
	if(name == NULL){
		perror("Malloc encountered an error");
		return -1;
	}
	//strncpy(name, "answers/", (size_t)8);
	if(argc < 3){
		printf("usage: mem_test <mode> <pattern>\n\n");
		printf("mode:\t\tspecifies which page replacement algorithm to use.\n");
		printf("\t\t0 - FIFO\n");
		printf("\t\t1 - LRU\n");
		printf("\t\t2 - clock\n");
		printf("pattern:\tspecifies which memory access pattern to test the\n");
		printf("\t\talgorithm with.\n");
		printf("\t\t0 - sequential\n");
		printf("\t\t1 - random\n");
		printf("\t\t2 - spatially local\n");
		return -1;
	}
	
	mode = atoi(argv[1]);
	/// might not be true if other page replacement algorithms are implemented
	if(mode < 0 || mode > 4){
		printf("Error: the value for `mode` is not within the acceptable range\n");
		return -1;
	}
	pattern = atoi(argv[2]);
	if(pattern < 0 || pattern > 2){
		printf("Error: the value for `pattern` is not within the acceptable range\n");
		return -1;
	}

	if(mode == 0){
		strncpy(name, "fifo-", (size_t)5);
	}else if(mode == 1){
		strncpy(name, "lru-", (size_t)4);
	}else{
		strncpy(name, "clock-", (size_t)6);
	}

	if(pattern == 0){
		strncat(name, "seqt", (size_t)4);
	}else if(pattern == 1){
		strncat(name, "rand", (size_t)4);
	}else{
		strncat(name, "spatl", (size_t)5);
	}
	
	struct TEST_FRAMEWORK *frame = init_test_framework(pattern);

	if(frame != NULL){
		isu_mmu_t test_MMU = isu_mmu_create(mode);
		run_test_framework(frame, mode, test_MMU);
		print_test_framework(frame, name);
		destroy_test_framework(frame);
	}else{
		printf("There was an error creating the test framework\n");
		return -1;
	}

	return 0;
}

struct TEST_FRAMEWORK *init_test_framework(int pattern){
	int i;
	int j;
	isu_mem_req_t temp;
	isu_llist_t past_elements;
	struct TEST_FRAMEWORK *ret;

	srand(12345);

	ret = calloc(1, sizeof(struct TEST_FRAMEWORK));

	ret->mem_list = isu_llist_create();
	ret->misses = 0;
	ret->current_time = 0;

	past_elements = isu_llist_create();

	for(i = 0; i < 1000; i++){
		switch(pattern){
		case 0:	temp = isu_mem_req_create((unsigned short)((i * 4096) % 65536));
			break;
		case 1: temp = isu_mem_req_create((unsigned short)(rand() % 65536));
			break;
		case 2: if(i < 5){
				temp = isu_mem_req_create((unsigned short)(rand() % 65536));
				isu_llist_push(past_elements, temp, ISU_LLIST_TAIL);
			}else{
				temp = isu_llist_ittr_start(past_elements, ISU_LLIST_TAIL);
				if((rand() % 100) < 80){
					j = (rand() % 5);
					while(j){
						temp = isu_llist_ittr_next(past_elements);
						j--;
					}
					temp = isu_mem_req_create(isu_mem_req_get_address(temp));
					isu_llist_pop(past_elements, ISU_LLIST_HEAD);
					isu_llist_push(past_elements, temp, ISU_LLIST_TAIL);
				}else{
					temp = isu_mem_req_create((unsigned short)(rand() % 65536));
					isu_llist_pop(past_elements, ISU_LLIST_HEAD);
					isu_llist_push(past_elements, temp, ISU_LLIST_TAIL);
				}
			}
			break;
		}
		isu_llist_push(ret->mem_list, temp, ISU_LLIST_TAIL);
	}

	return ret;
}

void run_test_framework(struct TEST_FRAMEWORK *f, int mode, isu_mmu_t MMU){
	/// while the memory request list isn't empty
	/// clear the reference bit
	/// get the next element in the memory request list
	/// handle memory request
	isu_mem_req_t t = (isu_mem_req_t)isu_llist_ittr_start(f->mem_list, ISU_LLIST_HEAD);
	while(t){
		if(mode != 2){
			isu_mmu_ref_clear(MMU);
		}
		isu_mmu_handle_req(MMU, t, &(f->current_time));
		t = isu_llist_ittr_next(f->mem_list);
	}
}

void print_test_framework(struct TEST_FRAMEWORK *f, char *name){
	/// open a file with the name `name`.log
	/// traverse the list of mem requests
	/// print to the opened file the line:
	/// memory address: %ud requested at time: %ull handled at time: %ull and was a hit/miss
	/// open another file with the name `name`.json
	/// traverse the list of mem requests
	/// print to the open json file
	strncat(name, ".log", (size_t)4);
	FILE *file = fopen(name, "w");
	isu_mem_req_t t = (isu_mem_req_t)isu_llist_ittr_start(f->mem_list, ISU_LLIST_HEAD);
	while(t){
		fprintf(file, "memory address: %u in page: %d requested at time: %llu handled at time: %llu and was a", isu_mem_req_get_address(t),
															(isu_mem_req_get_address(t) / 4096),
													   	 	isu_mem_req_get_req_time(t),
													    		isu_mem_req_get_handle_time(t));
		if(isu_mem_req_get_access_hit(t)){
			fprintf(file, " hit\n");
		}else{
			fprintf(file, " miss\n");
			f->misses++;
		}
		t = (isu_mem_req_t)isu_llist_ittr_next(f->mem_list);
	}
	fprintf(file, "1000 memory access requests were handled in %llu nanoseconds\n", f->current_time);
	fprintf(file, "Of the 1000 memory access requests, %d were misses, making it a hit rate of %f\n", f->misses, 1.f - ((float)(f->misses) / 1000.f));
	fclose(file);
	file = 0;
}

void destroy_test_framework(struct TEST_FRAMEWORK *f){
	/// go through the list and pop each element off the list
	/// free the elements poped from the list
	/// once all the elements are freed, free the list
	/// then free the framework
	/// set `f` to 0
	if(isu_llist_count){
		void *t = isu_llist_pop(f->mem_list, ISU_LLIST_HEAD);
		while(t){
			free(t);
			t = isu_llist_pop(f->mem_list, ISU_LLIST_HEAD);
		}
	}
	isu_llist_destroy(f->mem_list);
	free(f);
	f = 0;
}
