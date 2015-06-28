/*
 * @addtogroup scheduler_algorithms Scheduler Algorithms
 * @{
 * @addtogroup sched_fcfs First Come First Serve
 * @brief     A simple first come first serve scheduling algorithm.
 * @details   This algorithm will always select the task that arrived earliest
 *            out of all ready tasks to run next.  It is non-premptive and does
 *            not need any system tick.
 * @{
 * @file      isu_sched_fcfs.c
 * @author    Jeramie Vens
 * @date      3/16/15 - Created
 * @date      3/17/15 - Updated class inheritance and added documentation.
 * @date      3/23/15 - Moved into the new project and changed tasks to no 
 *            longer be pointers
 * @brief     Simple first come first serve scheduling algorithm.
 */

#include <stdio.h>
#include <stdlib.h>
#include "scheduler/isu_sched.h"
#include "scheduler/isu_sched_algorithm.h"
#include "task/isu_task.h"
#include "llist/isu_llist.h"
#include "common/isu_types.h"
#include "common/isu_error.h"

//-- Prototypes --//
void* fcfs_construct(void);
isu_return_t fcfs_destruct(void* this);
isu_return_t fcfs_start(void* this, isu_llist_t task_list);
isu_llist_t fcfs_stop(void* this);
isu_return_t fcfs_add_task(void* this, isu_task_t task);
isu_task_t fcfs_get_next_task(void* this, int remove);

/**
 * instantiated algorithm object.
 */
typedef struct FCFS_OBJECT_STRUCT
{
	/// The list of tasks that are in the ready state
	isu_llist_t task_list;
	/// The currently running task
	isu_task_t running_task;
}fcfs_obj_t;

/// The name of this scheduling algorithm
static const char const* this_name = "fcfs";

/// @class fcfs_algorithm_t
/// @brief The static class object which of this scheduling algorithm
/// @extends isu_sched_algorithm_t
typedef struct FCFS_CLASS
{
	/// The base class of the algorithm
	isu_sched_algorithm_t base;
}fcfs_algorithm_t;

static fcfs_algorithm_t this_static;
/**
 * @brief     Install the fcfs algorithm module.
 * @return    The static class object containing the class methods
 * @details   This function is called when the library is installed using the 
 *            isu_module_install() function.
 */
isu_sched_algorithm_t const* install()
{
	this_static.base.construct = fcfs_construct;
	this_static.base.destruct = fcfs_destruct;
	this_static.base.start = fcfs_start;
	this_static.base.stop = fcfs_stop;
	this_static.base.add_task = fcfs_add_task;
	this_static.base.get_next_task = fcfs_get_next_task;
	this_static.base.name = this_name;
	this_static.base.is_premptive = 0;
	this_static.base.quantum = 0;
	return (isu_sched_algorithm_t*)&this_static;
}

/**
 * @brief     Uninstall the fcfs algorithm module.
 * @details   This function is called when the module is being uninstalled by
 *            the isu_module_uninstall() function.
 */
void uninstall(void)
{
	isu_print(PRINT_DEBUG, "Uninstalling the %s module", this_name);
	// this module doesn't have anything to clean up so just return without doing anything.	
}

/**
 * @brief     Construct a new fcfs algorithm object.
 * @memberof  fcfs_algorithm_t
 * @see       isu_sched_algorithm_t::construct()
 */
void* fcfs_construct(void)
{
	isu_print(PRINT_DEBUG, "Constructing new %s scheduler algorithm", this_name);
	fcfs_obj_t *this = calloc(1, sizeof(fcfs_obj_t));
	if(!this)
	{
		isu_print(PRINT_ERROR, "Could not allocate memory for new object");
	}
	return this;
}

/**
 * @brief     Destrcutor for fcfs algorithm object
 * @memberof  fcfs_algorithm_t
 * @see       isu_sched_algorithm_t::destruct()
 */
isu_return_t fcfs_destruct(void* this)
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
 * @memberof  fcfs_algorithm_t
 * @see       isu_sched_algorithm_t::start()
 */
isu_return_t fcfs_start(void* _this, isu_llist_t task_list)
{
	fcfs_obj_t *this = _this;
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
	isu_print(PRINT_DEBUG, "Finished starting scheduler algorithm %s", this_name);
	return ISU_RET_SUCCESS;
}

/**
 * @brief     Stop the algorithm
 * @memberof  fcfs_algorithm_t
 * @see       isu_sched_algorithm_t::stop()
 */
isu_llist_t fcfs_stop(void* _this)
{
	fcfs_obj_t *this = _this;
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
 * @memberof  fcfs_algorithm_t
 * @see       isu_sched_algorithm_t::add_task()
 */
isu_return_t fcfs_add_task(void* _this, isu_task_t task)
{
	fcfs_obj_t *this = _this;
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
 * @memberof  fcfs_algorithm_t
 * @see       isu_sched_algorithm_t::get_next_task()
 */
isu_task_t fcfs_get_next_task(void* _this, int remove)
{
	fcfs_obj_t *this = _this;
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
	if(!remove && this->running_task)
	{
		// if we are not removing the task from the list then it should run again
		isu_print(PRINT_DEBUG, "Run same task again");
	}
	else
	{
		this->running_task = isu_llist_pop(this->task_list, ISU_LLIST_HEAD);
		if(this->running_task)
		{
			isu_print(PRINT_DEBUG, "Run task which arrived earliest");
		}
		else
		{
			isu_print(PRINT_DEBUG, "No task to run, select IDLE task");
		}
	}
	isu_print(PRINT_DEBUG, "Finished slecting task to run in scheduler algorithm %s", this_name);
	return this->running_task;
}

/// @}
/// @}

