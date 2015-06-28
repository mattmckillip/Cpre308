/**
 * @file      sched_test.c
 * @author    Jeramie Vens
 * @brief     Test the scheduler part of the system.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler/isu_sched.h"
#include "task/isu_task.h"
#include "common/isu_types.h"
#include "llist/isu_llist.h"
#include "common/isu_error.h"
#include "module/isu_module.h"

/// the test framework used for testing
struct TEST_FRAMEWORK
{
	/// the list of tasks
	isu_llist_t task_list;
	/// count of context switches
	int context_switches;
	/// the time the test finished
	long finished_time;
};

//-- prototypes --//
struct TEST_FRAMEWORK* init_test_framework();
void run_test_framework(struct TEST_FRAMEWORK* framework);
void print_test_framework(struct TEST_FRAMEWORK* framework, char* name);
void destroy_test_framework(struct TEST_FRAMEWORK* framework);

int main(int argc, char** argv)
{
	int i;
	if(argc < 2)
	{	
		printf("Please supply an algorithm\n");
		return -1;
	}	
	
	isu_sched_Init();
	
	// for each command line argument
	for(i = 1; i < argc; i++)
	{
		// attempt to install the scheduling algorithm
		isu_sched_algorithm_t* alg = isu_module_install(argv[i]);
		if(!alg)
		{
			// was not a valid algorithm, exit w/ error
			printf("error\n");
			return -1;
		}
		// install the algorithm
		isu_sched_InstallAlgorithm(alg);
	}
	
	isu_llist_t list = isu_sched_ListAlgorithm();
	char* str;
	struct TEST_FRAMEWORK* framework;
	// for each installed algorithm
	for_each(str, list)
	{
		printf("Found algorithm: %s\n", str);
		// set the algorithm as current
		isu_sched_SetAlgorithm(str);
		// initialize the test framework
		framework = init_test_framework();
		// test the algorith
		run_test_framework(framework);
		// print out the results
		print_test_framework(framework, str);
		// destroy the test framework
		destroy_test_framework(framework);
	}	
	
	// clean everything up
	isu_llist_destroy(list);
	isu_sched_Destroy();	
	for( i=1; i<argc; i++)
	{
		isu_module_uninstall(argv[i]);
	}
}

/**
 * @brief     Initialize a test
 * @return    The test framework
 */
struct TEST_FRAMEWORK* init_test_framework()
{
	int i;
	struct TEST_FRAMEWORK* framework = calloc(1, sizeof(struct TEST_FRAMEWORK));
	framework->task_list = isu_llist_create();
	if(!framework->task_list)
	{
		isu_print(PRINT_ERROR, "task_list is null");
		exit(-1);
	}
	isu_task_t task;
	srand(0);
	int start_time = 0;
	int priority = 0;
	int length = 0;
	char name[20];
	// create 50 new tasks
	for(i=0;i<50;i++)
	{
		start_time += rand() % 15;
		priority = rand() % 4;
		length = (rand() % ((5*(priority+1))))+1;
		sprintf(name, "%02d", i);
		task = isu_task_create(name, start_time, length, priority);
		isu_llist_push(framework->task_list, task, ISU_LLIST_TAIL);
	}
	return framework;
}

/**
 * @brief     Run the test
 * @param     framework
 *                 The test to run
 */
void run_test_framework(struct TEST_FRAMEWORK* framework)
{
	long time = 0;
	int remaining_tasks = isu_llist_count(framework->task_list);
	int on_tick_flag = 1;
	isu_task_t next_task=NULL, current_task=NULL, new_task=NULL;
	next_task = isu_llist_ittr_start(framework->task_list, ISU_LLIST_HEAD);
	printf("waiting for time %d\n", next_task->arrive_time);
	while(remaining_tasks)
	{
		on_tick_flag = 1;
		if(current_task)
		{
			current_task->remaining_time --;
			current_task->run_time ++;
		}
		while(next_task && next_task->arrive_time == time)
		{
			printf("task arrived at %d\n", time);
			new_task = isu_sched_OnTaskArrive(next_task);
			next_task = isu_llist_ittr_next(framework->task_list);
		}
		if(current_task)
		{
			if(current_task->remaining_time == 0)
			{
				printf("task finished at %d, %d left\n", time,remaining_tasks-1);
				current_task->finish_time = time;
				remaining_tasks --;
				new_task = isu_sched_OnTaskEnd();
				on_tick_flag = 0;
			}
		}
		if(on_tick_flag)
		{
			new_task = isu_sched_OnTick(time);
		}
		if(new_task != current_task)
		{
			printf("context switch at %d\n", time);
			if(new_task == NULL)
				printf("Going Idle\n");
			else
				printf("from %s to %s\n", current_task->name, new_task->name);
			framework->context_switches++;
			isu_task_add_stop_time(current_task, time);
			isu_task_add_start_time(new_task, time);
			current_task = new_task;
		}
		time++;
	}
	framework->finished_time = time;
}

/**
 * @brief     Print the results of the test to name.json and name.log
 * @param     framework
 *                 The test to print
 * @param     name
 *                 The name of the test
 * @details   The output .json file can be imported into WaveDrom to create a gantt
 *            chart of the test.
 */
void print_test_framework(struct TEST_FRAMEWORK* framework, char* name)
{
	float response_time_avg = 0, norm_response_time_avg = 0;
	char filename[100];
	strcpy(filename, name);
	strcat(filename, ".json");
	FILE *fp = fopen(filename, "w");
	fprintf(fp, "{signal: [\n");
	isu_task_t task;
	for_each(task, framework->task_list)
	{
		char* str = isu_task_to_json(task, framework->finished_time);
		fprintf(fp, "\t%s,\n", str);
		free(str);
	}
	fprintf(fp, "],\n\thead: {\n\t\ttick: -1\n\t}\n}\n");
	fclose(fp);
	strcpy(filename, name);
	strcat(filename, ".log");
	fp = fopen(filename, "w");
	for_each(task, framework->task_list)
	{
		int response_time = task->finish_time - task->arrive_time;
		float norm_response_time = response_time * 1.0 / task->run_time;
		fprintf(fp,"Task %s: Arrived: %d, Finshed: %d, Response: %d, Norm Response: %02.03f\n",
				task->name,
				task->arrive_time,
				task->finish_time,
				response_time,
				norm_response_time);
		response_time_avg += response_time;
		norm_response_time_avg += norm_response_time;
	}
	response_time_avg /= isu_llist_count(framework->task_list);
	norm_response_time_avg /= isu_llist_count(framework->task_list);
	fprintf(fp, "Average Response Time: %02.03f\nAverage Norm Response Time: %02.03f\n",
			response_time_avg,
			norm_response_time_avg);
	fprintf(fp, "Number Context Switches: %d\n", framework->context_switches);
	fclose(fp);
}

/**
 * @brief     Cleanup the test framework
 * @param     framework
 *                 The framework to clean up
 */
void destroy_test_framework(struct TEST_FRAMEWORK* framework)
{
	isu_task_t task;
	for_each(task, framework->task_list)
	{
		isu_task_destroy(task);
	}
	isu_llist_destroy(framework->task_list);
	free (framework);
}

