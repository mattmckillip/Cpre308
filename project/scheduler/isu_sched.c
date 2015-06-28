/** 
 * @addtogroup scheduler
 * @{
 */
/**
 * @file      isu_sched.c
 * @author    Jeramie Vens
 * @date      3/15/16 - Created
 * @date      3/16/15 - Made the algorithms handle dynamic loading and unloaded
 * @date      3/17/15 - Documentation added
 * @date      3/23/15 - Moved into the new project repository, added On* functions
 * @brief     This is the main source code for the scheduler.
 * @details   This file has all of the source code needed to install and use a
 *            scheduling algorithm.
 * @todo      Return error from each function if they are called before init.
 * @todo      Handle periodic scheduling algorithms.
 */

#include <stdio.h>
#include <stdlib.h>
#include "common/isu_types.h"
#include "common/isu_error.h"
#include "llist/isu_llist.h"
#include "task/isu_task.h"
#include "isu_sched_algorithm.h"

/// The list of installed algorithms
isu_llist_t algorithm_list = NULL;

/// The algorithm currently being used
isu_sched_algorithm_t* current_algorithm = NULL;
/// The instanciated algorithm object
void* current_algorithm_obj = NULL;

/**
 * @private
 * The currently running task on the system.  NULL for idle.
 */ 
static isu_task_t running_task = NULL;

/**
 * @private
 * The current time on the quantum timer used for Round Robin type algorithms.
 */
static long quantum_time = 0;


/**
 * @brief     Initialize the scheduler system
 * @return    ISU_RET_SUCCESS:
 *                 The scheduler system successfully initalized.
 * @return    ERROR_CODE:
 *                 The error code that cause intiallization to fail.
 * @details   This function is used to initialize and set up the scheduler so
 *            it can be used.
 */
isu_return_t isu_sched_Init(void)
{
	isu_print(PRINT_DEBUG, "Initializing the scheduler system");
	algorithm_list = isu_llist_create(); // create a list of algorithms
	if(algorithm_list)
	{
		isu_print(PRINT_DEBUG, "Successfully initialized the scheduler system");
		return ISU_RET_SUCCESS;
	}
	isu_print(PRINT_ERROR, "Failed to initialize the scheduler system");
	return GET_ERRNO;
}

/**
 * @brief     Destroy the scheduler system
 * @return    ISU_RET_SUCCESS:
 *                 Successfully destroyed the schecduler system.
 * @details   This function is used to destroy the scheduler system and free
 *            resources being used by it.
 */
isu_return_t isu_sched_Destroy()
{
	isu_print(PRINT_DEBUG, "Destroying the scheduler system");
	if(current_algorithm)
	{
		isu_print(PRINT_DEBUG, "Stopping current scheduler");
		isu_llist_t task_list = current_algorithm->stop(current_algorithm_obj);
		if(task_list)
		{
			isu_task_t task;
			for_each(task, task_list)
			{
				isu_print(PRINT_DEBUG, "Killing tasks");
				isu_task_destroy(task);
			}
			isu_print(PRINT_DEBUG, "Destroying task list");
			isu_llist_destroy(task_list);
		}
		isu_print(PRINT_DEBUG, "Destroying algorithm");
		current_algorithm->destruct(current_algorithm_obj);
	}
	if(algorithm_list)
	{
		isu_print(PRINT_DEBUG, "Destroying list of algorithems");
		isu_llist_destroy(algorithm_list);
	}
	isu_print(PRINT_DEBUG, "Succesfully destroyed the scheduler system");
	return ISU_RET_SUCCESS;
}

/**
 * @brief     Install a new scheduler algorithem
 * @param     scheduler
 *                 The scheduler algorithm to install.
 * @return    ISU_RET_SUCCESS:
 *                 The algorithm was successfully installed.
 * @return    ISU_RET_EUSAGE:
 *                 The scheduler system was not intialized by isu_sched_init()
 *                 before calling this function.
 * @return    ISU_RET_EPARAM:
 *                 The passed in parameter is not a valid scheduler algorithm.
 * @return    Other Error:
 *                 The linked list library returned error.
 * @details   Add the given scheduler algorithm to the list of installed
 *            scheduling algorithms so it can be used by the scheduling
 *            system.
 */
isu_return_t isu_sched_InstallAlgorithm(isu_sched_algorithm_t *scheduler)
{
	if(algorithm_list)
	{
		if(scheduler)
		{
			isu_print(PRINT_DEBUG, "Installing new scheduler algorithm");
			return isu_llist_push(algorithm_list, scheduler, ISU_LLIST_TAIL);
		}
		else
		{
			isu_print(PRINT_ERROR, "Invalid param scheduler");
			return ISU_RET_EPARAM;
		}
	}
	else
	{
		isu_print(PRINT_ERROR, "Scheduler system not initialized");
		return ISU_RET_EUSAGE;
	}
}

/**
 * @private
 * @brief     Get the scheduler algorithm from the list of schedulers.
 * @param     name
 *                 The name of the scheduler to get.
 * @return    The scheduler algorithm or NULL if it could not be found.
 */
inline isu_sched_algorithm_t* isu_sched_GetAlgorithmObj(char* name)
{
	isu_sched_algorithm_t* algorithm;
	for_each(algorithm, algorithm_list)
	{
		if(strcmp(name, algorithm->name) == 0)
		{
			return algorithm;
		}
	}
	return NULL;
}

/**
 * @brief     Uninstall a scheduler algorithem
 * @param     name
 *                 The name of the scheduling algorithm to uninstall.
 * @return    ISU_RET_SUCCESS:
 *                 The algorithm was succesfully uninstalled.
 * @return    ISU_RET_EPARAM:
 *                 The algorithm could not be found in the list of installed 
 *                 algorithms.
 * @remark    If the algorithm to be uninstalled is the current algorithm
 *            the list of ready and running tasks will be lost and all of those
 *            tasks will be forced to be destroyed.
 * @details   This uninstalls an installed scheduling algorithm and cleans up
 *            any resources that the algorithm holds.
 */
isu_return_t isu_sched_UninstallAlgorithm(char *name)
{
	isu_sched_algorithm_t* algorithm;
	isu_print(PRINT_DEBUG, "Uninstalling scheduling algorithm \"%s\"", name);
	algorithm = isu_sched_GetAlgorithmObj(name);
	if(!algorithm)
	{
		// Could not find algorithm
		isu_print(PRINT_WARN, "Could not find scheduling algorithm \"%s\"", name);
		return ISU_RET_EPARAM;
	}
	if(algorithm == current_algorithm)
	{
		// This algorithm is in use
		// lets stop it and throw away the tasks
		// maybe not the best solution, but it works
		isu_print(PRINT_DEBUG, "Uninstalling currently used algorithm");
		isu_llist_t task_list;
		task_list = current_algorithm->stop(current_algorithm_obj);
		current_algorithm->destruct(current_algorithm_obj);
		current_algorithm = NULL;
		isu_task_t task;
		for_each(task, task_list)
		{
			isu_print(PRINT_NOTICE, "Killing ready task");
			isu_task_destroy(task);
		}
		isu_llist_destroy(task_list);
	}
	isu_print(PRINT_DEBUG, "Successfully uninstalled scheduling algorithm");
	return ISU_RET_SUCCESS;
}

/**
 * @brief     Set the current scheduler algorithem.
 * @param     name
 *                 The name of the algorithm to set as currently used.
 * @return    ISU_RET_SUCCESS:
 *                 The scheduler algorithm was successfully set.
 * @return    ISU_RET_EPARAM:
 *                 The given algorithm name could not be found in the list of
 *                 installed algorithms.
 * @return    Other Error:
 *                 The algorithms start method returned error.
 * @details   This will set the given algorithm name as the currently being
 *            used algorithm.
 */
int isu_sched_SetAlgorithm(char *name)
{
	isu_llist_t task_list;
	isu_print(PRINT_DEBUG, "Setting scheduler algorithm to %s", name);
	isu_sched_algorithm_t* algorithm;
	algorithm = isu_sched_GetAlgorithmObj(name);
	if(algorithm)
	{
		if(current_algorithm)
		{
			isu_print(PRINT_DEBUG, "Stopping running algorithm");
			task_list = current_algorithm->stop(current_algorithm_obj);
			isu_print(PRINT_DEBUG, "Destructing running algorithm object");
			current_algorithm->destruct(current_algorithm_obj);
		}
		else
		{
			task_list = NULL;
		}
		current_algorithm = algorithm;
		isu_print(PRINT_DEBUG, "Creating new algorithm object");
		current_algorithm_obj = current_algorithm->construct();
		isu_print(PRINT_DEBUG, "Starting new scheduler");
		return current_algorithm->start(current_algorithm_obj, task_list);
	}
	isu_print(PRINT_ERROR, "Could not find scheduler algorithm");
	return ISU_RET_EPARAM;
}

/**
 * @brief     Get the current scheduler algorithem name
 * @return    The name of the currently selected algorithm or NULL if on is not
 *            currently set.
 */
char const* isu_sched_GetAlgorithm()
{
	isu_print(PRINT_DEBUG, "Getting currently used scheduling algorithem");
	if(current_algorithm)
		return current_algorithm->name;
	else
		return NULL;
}

/**
 * @brief     Get a list of currently installed algorithems
 * @return    A linked list of currently installed algorithms or NULL if error.
 * @details   This will get a list of the names of currently installed
 *            scheduling algorithms.  Each element of the list is a const char 
 *            array.
 */
isu_llist_t isu_sched_ListAlgorithm()
{
	isu_llist_t list;
	isu_sched_algorithm_t* algorithm;
	
	isu_print(PRINT_DEBUG, "Getting list of installed scheduler algorithms");
	list = isu_llist_create();
	if(!list)
	{
		isu_print(PRINT_DEBUG, "Could not create linked list object");
		return NULL;
	}
	
	for_each(algorithm, algorithm_list)
	{
		if(isu_llist_push(list, (void*)algorithm->name, ISU_LLIST_TAIL))
		{
			isu_print(PRINT_DEBUG, "Error pushing algorithm name onto list");
			isu_llist_destroy(list);
			return NULL; 
		}
	}
	return list;
}


/**
 * @brief     Function which is registered as a callback for each system tick.
 * @param     time
 *                 The current system time
 * @return    The task that should next be run or NULL to run idle task.
 * @details   This function should be called periodically by a system tick.  If
 *            the loaded algorithm supports switching tasks on a system tick it
 *            will get a new task, otherwise it will return the running task.
 */
isu_task_t isu_sched_OnTick(int time)
{
	static int last_time = 0;
	if(current_algorithm->quantum == 0)
		return running_task;
	quantum_time ++;// (time - last_time);
	last_time = time;
	if((quantum_time % current_algorithm->quantum) == 0)
	{
		running_task = current_algorithm->get_next_task(current_algorithm_obj, 0);
	}
	return running_task;
}

/**
 * @brief     Function which is registered as a callback for when a task finishes.
 * @return    The next task that should be run or NULL to run idle task.
 * @details   Whenever a task finishes this will remove it from the ready queue
 *            and choose a new task to run on the system.
 */
isu_task_t isu_sched_OnTaskEnd()
{
	running_task = current_algorithm->get_next_task(current_algorithm_obj, 1);
	if(current_algorithm->reset_quantum)
		quantum_time = 0;
	return running_task;
}

/**
 * @brief     Function which is registered as a callback for when a new task arrives.
 * @param     task
 *                 The task that arrives
 * @return    The next task that should run.
 */
isu_task_t isu_sched_OnTaskArrive(isu_task_t task)
{
	current_algorithm->add_task(current_algorithm_obj, task);
	if(current_algorithm->is_premptive || running_task == NULL)
	{
		running_task = current_algorithm->get_next_task(current_algorithm_obj, 0);
		if(current_algorithm->reset_quantum)
			quantum_time = -1;
	}
	return running_task;
}
