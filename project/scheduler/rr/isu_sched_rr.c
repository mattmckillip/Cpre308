/*
 * @addtogroup scheduler_algorithms Scheduler Algorithms
 * @{
 * @addtogroup sched_rr Round Robbin
 * @brief     Round robbin scheduling algorithm
 * @details   This algorithm will give each task a set amount of time to run
 *            and then cycle to the next task.  It is non-premptive, but does
 *            use they system tick.
 * @{
 * @file      isu_sched_rr.c
 * @author    Jeramie Vens
 * @date      3/24/15 - Created
 * @brief     Round Robbin scheduling algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include "scheduler/isu_sched.h"
#include "scheduler/isu_sched_algorithm.h"
#include "task/isu_task.h"
#include "llist/isu_llist.h"
#include "common/isu_types.h"
#include "common/isu_error.h"

/// @def QUANTUM
/// The amount of time to give each process to run
#define QUANTUM 5

//-- Prototypes --//
void* rr_construct(void);
isu_return_t rr_destruct(void* this);
isu_return_t rr_start(void* this, isu_llist_t task_list);
isu_llist_t rr_stop(void* this);
isu_return_t rr_add_task(void* this, isu_task_t task);
isu_task_t rr_get_next_task(void* this, int remove);

/**
 * instantiated algorithm object.
 */
typedef struct RR_OBJECT_STRUCT
{
	/// The list of tasks that are in the ready state
	isu_llist_t task_list;
	/// The currently running task
	isu_task_t running_task;
	/// Flag if the itterator has been started on the task list
	int ittr_flag;
}rr_obj_t;

/// The name of this scheduling algorithm
static const char const* this_name = "rr";

/// @class rr_algorithm_t
/// @brief The static class object which of this scheduling algorithm
/// @extends isu_sched_algorithm_t
typedef struct FCFS_CLASS
{
	/// The base class of the algorithm
	isu_sched_algorithm_t base;
}rr_algorithm_t;

static rr_algorithm_t this_static;
/**
 * @brief     Install the rr algorithm module.
 * @return    The static class object containing the class methods
 * @details   This function is called when the library is installed using the 
 *            isu_module_install() function.
 */
isu_sched_algorithm_t const* install()
{
	this_static.base.construct = rr_construct;
	this_static.base.destruct = rr_destruct;
	this_static.base.start = rr_start;
	this_static.base.stop = rr_stop;
	this_static.base.add_task = rr_add_task;
	this_static.base.get_next_task = rr_get_next_task;
	this_static.base.name = this_name;
	this_static.base.is_premptive = 0;
	this_static.base.quantum = QUANTUM;
	this_static.base.reset_quantum = 1;
	return (isu_sched_algorithm_t*)&this_static;
}

/**
 * @brief     Uninstall the rr algorithm module.
 * @details   This function is called when the module is being uninstalled by
 *            the isu_module_uninstall() function.
 */
void uninstall(void)
{
	isu_print(PRINT_DEBUG, "Uninstalling the %s module", this_name);
	// this module doesn't have anything to clean up so just return without doing anything.	
}

/**
 * @brief     Construct a new rr algorithm object.
 * @memberof  rr_algorithm_t
 * @see       isu_sched_algorithm_t::construct()
 */
void* rr_construct(void)
{
	isu_print(PRINT_DEBUG, "Constructing new %s scheduler algorithm", this_name);
	rr_obj_t *this = calloc(1, sizeof(rr_obj_t));
	if(!this)
	{
		isu_print(PRINT_ERROR, "Could not allocate memory for new object");
	}
	return this;
}

/**
 * @brief     Destrcutor for rr algorithm object
 * @memberof  rr_algorithm_t
 * @see       isu_sched_algorithm_t::destruct()
 */
isu_return_t rr_destruct(void* this)
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
 * @memberof  rr_algorithm_t
 * @see       isu_sched_algorithm_t::start()
 */
isu_return_t rr_start(void* _this, isu_llist_t task_list)
{
	rr_obj_t *this = _this;
	isu_print(PRINT_DEBUG, "Starting scheduler algorithm %s", this_name);
	if(!this)
	{
		isu_print(PRINT_ERROR, "Called with null object");
		return ISU_RET_EPARAM;
	}
	if(this->task_list)
	{
		isu_print(PRINT_ERROR, "Algorithm already started");
		return ISU_RET_EUSAGE;
	}
	if(task_list)
	{
		isu_print(PRINT_DEBUG, "Using old task list");
		this->task_list = task_list;
	}
	else
	{
		isu_print(PRINT_DEBUG, "Creating new task list");
		this->task_list = isu_llist_create();
	}
	this->ittr_flag = 0;
	isu_print(PRINT_DEBUG, "Finished starting scheduler algorithm %s", this_name);
	return ISU_RET_SUCCESS;
}

/**
 * @brief     Stop the algorithm
 * @memberof  rr_algorithm_t
 * @see       isu_sched_algorithm_t::stop()
 */
isu_llist_t rr_stop(void* _this)
{
	rr_obj_t *this = _this;
	isu_llist_t list = NULL;
	isu_print(PRINT_DEBUG, "Stopping scheduler algorithm %s", this_name);
	if(!this)
	{
		isu_print(PRINT_ERROR, "Called with null object");
		//return ISU_RET_EPARAM;
		return NULL;
	}
	if(isu_llist_count(this->task_list) > 0)
	{
		isu_print(PRINT_DEBUG, "Saving task list");
		list = this->task_list;
		this->task_list = NULL;
	}
	else
	{
		isu_llist_destroy(this->task_list);
		this->task_list = NULL;
	}
	isu_print(PRINT_DEBUG, "Successfully stopped scheduler %s", this_name);
	return list;
}

/**
 * @brief     Add a new task to the ready list
 * @memberof  rr_algorithm_t
 * @see       isu_sched_algorithm_t::add_task()
 */
isu_return_t rr_add_task(void* _this, isu_task_t task)
{
	rr_obj_t *this = _this;
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
	
	if(rv = isu_llist_push(this->task_list, task, ISU_LLIST_TAIL))
	{
		isu_print(PRINT_ERROR, "Failed to add new task to the scheduler");
		return rv;
	}

	isu_print(PRINT_DEBUG, "Successfully added new task to scheduler %s", this_name);
	return ISU_RET_SUCCESS;
}

/**
 * @brief     Get the next task to run.
 * @memberof  rr_algorithm_t
 * @see       isu_sched_algorithm_t::get_next_task()
 */
isu_task_t rr_get_next_task(void* _this, int remove)
{
	rr_obj_t *this = _this;
	isu_return_t rv;
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
			rr_add_task(this, this->running_task);
		}

		this->running_task = NULL;


		this->running_task = isu_llist_pop(this->task_list, ISU_LLIST_HEAD);
		
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

