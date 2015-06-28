/*
 * @addtogroup scheduler_algorithms Scheduler Algorithms
 * @{
 * @addtogroup sched_srtn Shortest Remaining Time Next
 * @brief     A premptive algorithm that will scheduler the task with the shortest remaining time next
 * @details   This algorithm will always select the task that has the shortest
 *            time left to run. It is premptive but does not need any system 
 *            tick.
 * @{
 * @file      isu_sched_srtn.c
 * @author    Jeramie Vens
 * @date      3/24/15 - Created
 * @brief     Shortest remaining time next algorithm.
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
void* srtn_construct(void);
isu_return_t srtn_destruct(void* this);
isu_return_t srtn_start(void* this, isu_llist_t task_list);
isu_llist_t srtn_stop(void* this);
isu_return_t srtn_add_task(void* this, isu_task_t task);
isu_task_t srtn_get_next_task(void* this, int remove);

/**
 * instantiated algorithm object.
 */
typedef struct SRTN_OBJECT_STRUCT
{
	/// The list of tasks that are in the ready state
	isu_llist_t task_list;
	/// The currently running task
	isu_task_t running_task;
}srtn_obj_t;

/// The name of this scheduling algorithm
static const char const* this_name = "srtn";

/// @class srtn_algorithm_t
/// @brief The static class object which of this scheduling algorithm
/// @extends isu_sched_algorithm_t
typedef struct SRTN_CLASS
{
	/// The base class of the algorithm
	isu_sched_algorithm_t base;
}srtn_algorithm_t;

static srtn_algorithm_t this_static;
/**
 * @brief     Install the srtn algorithm module.
 * @return    The static class object containing the class methods
 * @details   This function is called when the library is installed using the 
 *            isu_module_install() function.
 */
isu_sched_algorithm_t const* install()
{
	this_static.base.construct = srtn_construct;
	this_static.base.destruct = srtn_destruct;
	this_static.base.start = srtn_start;
	this_static.base.stop = srtn_stop;
	this_static.base.add_task = srtn_add_task;
	this_static.base.get_next_task = srtn_get_next_task;
	this_static.base.name = this_name;
	this_static.base.is_premptive = 1;
	this_static.base.quantum = 0;
	return (isu_sched_algorithm_t*)&this_static;
}

/**
 * @brief     Uninstall the srtn algorithm module.
 * @details   This function is called when the module is being uninstalled by
 *            the isu_module_uninstall() function.
 */
void uninstall(void)
{
	isu_print(PRINT_DEBUG, "Uninstalling the %s module", this_name);
	// this module doesn't have anything to clean up so just return without doing anything.	
}

/**
 * @brief     Construct a new srtn algorithm object.
 * @memberof  srtn_algorithm_t
 * @see       isu_sched_algorithm_t::construct()
 */
void* srtn_construct(void)
{
	isu_print(PRINT_DEBUG, "Constructing new %s scheduler algorithm", this_name);
	srtn_obj_t *this = calloc(1, sizeof(srtn_obj_t));
	if(!this)
	{
		isu_print(PRINT_ERROR, "Could not allocate memory for new object");
	}
	return this;
}

/**
 * @brief     Destrcutor for srtn algorithm object
 * @memberof  srtn_algorithm_t
 * @see       isu_sched_algorithm_t::destruct()
 */
isu_return_t srtn_destruct(void* this)
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
 * @memberof  srtn_algorithm_t
 * @see       isu_sched_algorithm_t::start()
 */
isu_return_t srtn_start(void* _this, isu_llist_t task_list)
{
	srtn_obj_t *this = _this;
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
		isu_print(PRINT_DEBUG, "Sorting old task list by remaining time");
		this->task_list = isu_llist_create();
		isu_task_t task;
		// add the existing tasks to the list in the order of remaining time
		while(task = isu_llist_pop(task_list, ISU_LLIST_HEAD))
		{
			isu_task_t i;
			int flag = 0;
			for_each(i, this->task_list)
			{
				if(task->remaining_time < i->remaining_time)
				{
					isu_llist_insert_before(this->task_list, task);
					flag = 1;
					break;
				}
			}
			if(flag == 0)
			{
				isu_llist_push(this->task_list, task, ISU_LLIST_TAIL);
			}
		}
		isu_llist_destroy(task_list);
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
 * @memberof  srtn_algorithm_t
 * @see       isu_sched_algorithm_t::stop()
 */
isu_llist_t srtn_stop(void* _this)
{
	srtn_obj_t *this = _this;
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
 * @memberof  srtn_algorithm_t
 * @see       isu_sched_algorithm_t::add_task()
 */
isu_return_t srtn_add_task(void* _this, isu_task_t task)
{
	srtn_obj_t *this = _this;
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
	
	// iterate through the llist to find where to place the new task
	isu_task_t temp = isu_llist_ittr_start(this->task_list, ISU_LLIST_HEAD);
	if (temp == NULL)
	{
		isu_llist_push(this->task_list, task, ISU_LLIST_HEAD);
		return ISU_RET_SUCCESS;
	}

	while(temp->remaining_time < task->remaining_time)
	{
		temp = isu_llist_ittr_next(this->task_list);
		if (temp == NULL)
		{
			isu_llist_push(this->task_list, task, ISU_LLIST_TAIL);
			return ISU_RET_SUCCESS;
		}
	}
	
	//insert where the itterator is
	if(rv = isu_llist_insert_before(this->task_list, task))
	{
		isu_print(PRINT_ERROR, "Failed to add new task to the scheduler");
		return rv;
	}

	isu_print(PRINT_NOTICE, "Successfully added new task to scheduler %s", this_name);
	return ISU_RET_SUCCESS;
	
}

/**
 * @brief     Get the next task to run.
 * @memberof  srtn_algorithm_t
 * @see       isu_sched_algorithm_t::get_next_task()
 */
isu_task_t srtn_get_next_task(void* _this, int remove)
{
	srtn_obj_t *this = _this;
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
		isu_task_t temp = isu_llist_pop(this->task_list, ISU_LLIST_HEAD);

		if (temp == NULL)
		{
			this->running_task = NULL;
			return this->running_task;
		}
		
		if(temp->remaining_time < this->running_task->remaining_time){
			srtn_add_task(this, this->running_task);
			this->running_task = temp;
			isu_print(PRINT_DEBUG, "Place task back into list");
		}
		else{
			isu_llist_push(this->task_list, temp, ISU_LLIST_HEAD);
		}
	}
	else
	{

		this->running_task = isu_llist_pop(this->task_list, ISU_LLIST_HEAD);
		if(this->running_task)
		{
			isu_print(PRINT_DEBUG, "Run task with shortest remaining time");
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

