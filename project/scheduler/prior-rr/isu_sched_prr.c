/*
 * @addtogroup scheduler_algorithms Scheduler Algorithms
 * @{
 * @addtogroup sched_prr Priority Round Robbin
 * @brief     Priority round robbin scheduling algorithm
 * @details   This algorithm will give each task a set amount of time to run
 *            and then cycle to the next task.  Tasks are always chosen such
 *            that a task with the highest priority always runs next.
 *            Priorities are defined such that the smaller the number, the 
 *            higher the priority of the task.  It is non-premptive, but does
 *            use they system tick.
 * @{
 * @file      isu_sched_prr.c
 * @author    Jeramie Vens
 * @date      3/24/15 - Created
 * @brief     Priority Round Robbin scheduling algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include "scheduler/isu_sched.h"
#include "scheduler/isu_sched_algorithm.h"
#include "task/isu_task.h"
#include "llist/isu_llist.h"
#include "common/isu_types.h"
#include "common/isu_error.h"

/// @def NUM_PRIORITIES
/// The number of priority levels supported
#define NUM_PRIORITIES 4
/// @def QUANTUM
/// The amount of time to give each process on the system
#define QUANTUM 5
///for debug printing
#define DEBUG 1

//-- Prototypes --//
void* prr_construct(void);
isu_return_t prr_destruct(void* this);
isu_return_t prr_start(void* this, isu_llist_t task_list);
isu_llist_t prr_stop(void* this);
isu_return_t prr_add_task(void* this, isu_task_t task);
isu_task_t prr_get_next_task(void* this, int remove);

/**
 * instantiated algorithm object.
 */
typedef struct PRR_OBJECT_STRUCT
{
	/// The list of tasks that are in the ready state
	isu_llist_t task_list[NUM_PRIORITIES];
	/// The cuprrently running task
	isu_task_t running_task;
	/// Flag if the itterator has been started on the task list
	int ittr_flag[NUM_PRIORITIES];
}prr_obj_t;

/// The name of this scheduling algorithm
static const char const* this_name = "prr";

/// @class prr_algorithm_t
/// @brief The static class object which of this scheduling algorithm
/// @extends isu_sched_algorithm_t
typedef struct FCFS_CLASS
{
	/// The base class of the algorithm
	isu_sched_algorithm_t base;
}prr_algorithm_t;

static prr_algorithm_t this_static;
/**
 * @brief     Install the prr algorithm module.
 * @return    The static class object containing the class methods
 * @details   This function is called when the library is installed using the 
 *            isu_module_install() function.
 */
isu_sched_algorithm_t const* install()
{
	this_static.base.construct = prr_construct;
	this_static.base.destruct = prr_destruct;
	this_static.base.start = prr_start;
	this_static.base.stop = prr_stop;
	this_static.base.add_task = prr_add_task;
	this_static.base.get_next_task = prr_get_next_task;
	this_static.base.name = this_name;
	this_static.base.is_premptive = 0;
	this_static.base.quantum = QUANTUM;
	this_static.base.reset_quantum = 1;
	return (isu_sched_algorithm_t*)&this_static;
}

/**
 * @brief     Uninstall the prr algorithm module.
 * @details   This function is called when the module is being uninstalled by
 *            the isu_module_uninstall() function.
 */
void uninstall(void)
{
	isu_print(PRINT_DEBUG, "Uninstalling the %s module", this_name);
	// this module doesn't have anything to clean up so just return without doing anything.	
}

/**
 * @brief     Construct a new prr algorithm object.
 * @memberof  prr_algorithm_t
 * @see       isu_sched_algorithm_t::construct()
 */
void* prr_construct(void)
{
	isu_print(PRINT_DEBUG, "Constructing new %s scheduler algorithm", this_name);
	prr_obj_t *this = calloc(1, sizeof(prr_obj_t));
	if(!this)
	{
		isu_print(PRINT_ERROR, "Could not allocate memory for new object");
	}
	return this;
}

/**
 * @brief     Destrcutor for prr algorithm object
 * @memberof  prr_algorithm_t
 * @see       isu_sched_algorithm_t::destruct()
 */
isu_return_t prr_destruct(void* this)
{
	if(!this)
	{
		isu_print(PRINT_ERROR, "Called with null object");
		return ISU_RET_EPARAM;
	}
	free(this);
	return ISU_RET_SUCCESS;
}

/**
 * @brief     Start the algorithm.
 * @memberof  prr_algorithm_t
 * @see       isu_sched_algorithm_t::start()
 */
isu_return_t prr_start(void* _this, isu_llist_t task_list)
{
	prr_obj_t *this = _this;
	isu_return_t rv;
	int i;
	isu_print(PRINT_DEBUG, "Starting scheduler algorithm %s", this_name);
	if(!this)
	{
		isu_print(PRINT_ERROR, "Called with null object");
		return ISU_RET_EPARAM;
	}
	if(this->task_list[0])
	{
		isu_print(PRINT_ERROR, "Algorithm already started");
		return ISU_RET_EUSAGE;
	}
	for(i=0; i<NUM_PRIORITIES; i++)
	{
		this->task_list[i] = isu_llist_create();
		this->ittr_flag[i] = 0;
	}
	if(task_list)
	{
		isu_print(PRINT_DEBUG, "Using old task list");
		isu_task_t task;
		while(task = isu_llist_pop(task_list, ISU_LLIST_HEAD))
		{
			if(rv = isu_llist_push(this->task_list[task->priority], task, ISU_LLIST_TAIL))
			{
				isu_print(PRINT_ERROR, "Failed to push task onto new task list");
				return rv;
			}
		}
	}
	isu_print(PRINT_DEBUG, "Finished starting scheduler algorithm %s", this_name);
	return ISU_RET_SUCCESS;
}

/**
 * @brief     Stop the algorithm
 * @memberof  prr_algorithm_t
 * @see       isu_sched_algorithm_t::stop()
 */
isu_llist_t prr_stop(void* _this)
{
	prr_obj_t *this = _this;
	isu_llist_t list = NULL;
	isu_return_t rv;
	int i;
	isu_print(PRINT_DEBUG, "Stopping scheduler algorithm %s", this_name);
	if(!this)
	{
		isu_print(PRINT_ERROR, "Called with null object");
		//return ISU_RET_EPARAM;
		return NULL;
	}
	list = isu_llist_create();
	for(i=0;i<NUM_PRIORITIES;i++)
	{
		if(isu_llist_count(this->task_list[i]) > 0)
		{
			isu_print(PRINT_DEBUG, "Saving task list for priority %d", i);
			isu_task_t task;
			while(task = isu_llist_pop(this->task_list[i], ISU_LLIST_HEAD))
			{
				if(rv = isu_llist_push(list, task, ISU_LLIST_TAIL))
				{
					isu_print(PRINT_ERROR, "Failed to push task onto new task list");
				}
			}
			isu_llist_destroy(this->task_list[i]);
			this->task_list[i] = NULL;
		}
		else
		{
			isu_llist_destroy(this->task_list[i]);
			this->task_list[i] = NULL;
		}
	}
	if(isu_llist_count(list) == 0)
	{
		isu_llist_destroy(list);
		list = NULL;
	}
	isu_print(PRINT_DEBUG, "Successfully stopped scheduler %s", this_name);
	return list;
}

/**
 * @brief     Add a new task to the ready list
 * @memberof  prr_algorithm_t
 * @see       isu_sched_algorithm_t::add_task()
 */
isu_return_t prr_add_task(void* _this, isu_task_t task)
{
	prr_obj_t *this = _this;
	isu_return_t rv;
	isu_print(PRINT_DEBUG, "Adding new task to scheduler algorithm %s", this_name);
	if(!this || !task)
	{
		isu_print(PRINT_ERROR, "Called with null object");
		return ISU_RET_EPARAM;
	}
	if(!this->task_list)
	{
		isu_print(PRINT_ERROR, "Scheduler has not been started");
		return ISU_RET_EUSAGE;
	}

	// add task into the correct task list, based off of the priority
	int priority = task->priority;
	if(rv = isu_llist_push(this->task_list[priority], task, ISU_LLIST_TAIL))
	{
		isu_print(PRINT_ERROR, "Failed to add new task to the scheduler");
		return rv;
	}
	
	isu_print(PRINT_DEBUG, "Successfully added new task to scheduler %s", this_name);
	return ISU_RET_SUCCESS;
}

/**
 * @brief     Get the next task to run.
 * @memberof  prr_algorithm_t
 * @see       isu_sched_algorithm_t::get_next_task()
 */
isu_task_t prr_get_next_task(void* _this, int remove)
{
	prr_obj_t *this = _this;
	isu_return_t rv;
	int i;
	int priority = 0;
	isu_print(PRINT_DEBUG, "Getting next task from the scheduler algorithm %s", this_name);
	if(!this)
	{
		isu_print(PRINT_ERROR, "Called with null object");
		//return ISU_RET_EPARAM;
		return NULL;
	}
	if(!this->task_list)
	{
		isu_print(PRINT_ERROR, "Scheduler has not been started");
		//return ISU_RET_EUSAGE;
		return NULL;
	}
	else
	{
		//This function is called if the quantum is up by isu_sched
		// so push add the current task onto the queue if there is still remaining time
		if (this->running_task != NULL && this->running_task->remaining_time != 0)
		{
			prr_add_task(this, this->running_task);
		}

		this->running_task = NULL;

		while(1)
		{
			this->running_task = isu_llist_pop(this->task_list[priority], ISU_LLIST_HEAD);
			if(this->running_task){
				break;
			}
			priority += 1;
			if (priority >= NUM_PRIORITIES){
				this->running_task = NULL; 
				break;
			}
		}
		
		if(this->running_task)
		{
			isu_print(PRINT_DEBUG, "Run task that is next on the priority queue rr");
		}
		else
		{
			isu_print(PRINT_DEBUG, "No task to run, select IDLE task");
		}
	}

	isu_print(PRINT_NOTICE, "Finished slecting task to run in scheduler algorithm %s", this_name);
	return this->running_task;
}

/// @}
/// @}

