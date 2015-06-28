/**
 * @addtogroup task
 * @{
 */
/**
 * @file      isu_task.c
 * @author    Jeramie Vens
 * @date      3/24/15 - Created
 * @brief     The main code that is needed for tasks
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist/isu_llist.h"
#include "isu_task.h"

// fix anoying errors 
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

/**
 * @brief     Create a new task
 * @param     name
 *                 The name of the task
 * @param     start_time
 *                 The time the task is started
 * @param     run_time
 *                 The estimated amount of time the task will take
 * @param     priority
 *                 The priority level of this task
 * @return    The task object or NULL if something goes wrong
 */
isu_task_t isu_task_create(char* name, int start_time, int run_time, int priority)
{
	isu_task_t task = calloc(1, sizeof(struct ISU_TASK_STRUCT));
	strcpy(task->name, name);
	task->arrive_time = start_time;
	task->remaining_time = run_time;
	task->priority = priority;
	task->start_times = isu_llist_create();
	task->stop_times = isu_llist_create();
	return task;
}

/**
 * @brief     Destroy a task object
 * @param     task
 *                 The task to destroy
 */
void isu_task_destroy(isu_task_t task)
{
	isu_llist_destroy(task->start_times);
	isu_llist_destroy(task->stop_times);
	free(task);
}

/** 
 * @brief     Add a start time to the start time list
 * @param     task
 *                 The task to add the time to
 * @param     time
 *                 The time to add
 */
void isu_task_add_start_time(isu_task_t task, int time)
{
	if(task)
		isu_llist_push(task->start_times, (void*)time, ISU_LLIST_TAIL);
}

/** 
 * @brief     Add a stop time to the start time list
 * @param     task
 *                 The task to add the time to
 * @param     time
 *                 The time to add
 */
void isu_task_add_stop_time(isu_task_t task, int time)
{
	if(task)
		isu_llist_push(task->stop_times, (void*)time, ISU_LLIST_TAIL);
}

/**
 * @brief     Creat a line to go into a json file to be imported by wavedrom to draw charts
 * @param     task
 *                 The task to work on
 * @param     total_time
 *                 The total time the chart should go to
 * @return    A malloced string containing the line
 */
char* isu_task_to_json(isu_task_t task, long total_time)
{
	int i;
	char* str = malloc(1024);
	str[0] = 0;
	sprintf(str, "{name: '%s', wave: \"z", task->name);
	long time = 0;
	int count = 0;
	int start = (int)isu_llist_ittr_start(task->start_times, ISU_LLIST_HEAD);
	int stop = (int)isu_llist_ittr_start(task->stop_times, ISU_LLIST_HEAD);
	for(time = 0; time < task->finish_time; time++)
	{
		if(time == start)
		{
			strcat(str, "2");
			count ++;
			start = (int)isu_llist_ittr_next(task->start_times);
		}
		else if(time == task->arrive_time)
		{
			strcat(str, "x");
		}
		else if(time == stop)
		{
			strcat(str, "x");
			stop = (int)isu_llist_ittr_next(task->stop_times);
		}
		else
		{
			strcat(str, ".");
		}
	}
	strcat(str, "z");
	for(time = task->finish_time + 1; time < total_time + 1; time++)
	{
		strcat(str, ".");
	}
	strcat(str, "\"}");
	return str;
}

/// @}

