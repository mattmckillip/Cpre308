/**
 * @addtomodule task Tasks
 * @brief     Operating system tasks which are halfway between threads and processes.
 * @details   The task system of the operating system.  A task is something
 *            halfway between a thread and a process.  Since this OS has no
 *            differentiation between kernel and userspace care must be taken
 *            to protect address spaces of each task.
 * @{
 */
/**
 * @file      isu_task.h
 * @author    Jeramie Vens
 * @date      3/15/15 - Created placeholder file
 * @date      3/17/15 - Implemented basic functionality and added doc
 * @brief     The main include file for tasks on the os.
 * @details   This file includes the task object type and the public task functions.
 */
 
#ifndef ISU_TASK_H
#define ISU_TASK_H

/**
 * The main task object type
 */
struct ISU_TASK_STRUCT
{
	/// the name of this task
	char name[20];
	/// the amount of time this task has been running for
	int run_time;
	/// the estimatted remaining time this task will run for
	int remaining_time;
	/// the time this task finished
	int finish_time;
	/// the time this task arrived
	int arrive_time;
	/// the priority level of this task
	int priority;
	/// a list of the times that this task was started.  Used for data logging
	isu_llist_t start_times;
	/// a list of the times that this task was stopped.  Used for data logging
	isu_llist_t stop_times;	
};
/// The task object type that other parts of the OS should use
typedef struct ISU_TASK_STRUCT* isu_task_t;

// define the idel task to be NULL
#define isu_task_idleTask NULL

isu_task_t isu_task_create(char* name, int start_time, int run_time, int priority);
void isu_task_destroy(isu_task_t task);
void isu_task_add_start_time(isu_task_t task, int time);
void isu_task_add_stop_time(isu_task_t task, int time);
char* isu_task_to_json(isu_task_t task, long total_time);

#endif

/// @}

