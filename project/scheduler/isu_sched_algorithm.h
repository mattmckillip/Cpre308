/**
 * @addtogroup scheduler
 * @{
 */
/**
 * @file      isu_sched_algorithm.h
 * @author    Jeramie Vens
 * @date      3/14/15 - Created
 * @date      3/15/15 - Restructured and renamed functions
 * @date      3/16/15 - Added documentation
 * @date      3/23/15 - Moved to new project repository
 * @date      3/24/15 - Removed pointers from tasks
 * @brief     Main include file and definition needed for new scheduler modules.
 * @details   To create a new schedueler algorithm include this header file and
 *            Support each of the function pointers in the isu_sched_algorithm_t
 *            object.  The module should also implement an install function that
 *            takes no parameters and returns a pointer to a new 
 *            isu_sched_algorithm_t object.
 */

#ifndef ISU_SCHED_ALGORITHM_H
#define ISU_SCHED_ALGORITHM_H

#include "llist/isu_llist.h"
#include "common/isu_types.h"
#include "task/isu_task.h"

/**
 * @class     isu_sched_algorithm_t
 * @brief     An abstract class that new scheduler algorithms can implement
 */
typedef struct ISU_SCHED_ALGORITH_CLASS
{
	/**
	 * @brief      Construct a new algorithm object and return it.
	 * @return     The new algorithm object
	 * @details    The function should create a new algorithm object and return
	 *             that object.  This object is the instiance of the object and
	 *             has state data in it.  It will be passed into all other
	 *             methods of this class on future calls.
	 */
	void* (*construct)(void);
	/**
	 * @brief      Destruct the algorithm object and delete all memory used.
	 * @param      this
	 *                  This algorithm object.
	 * @return     ISU_RET_SUCCESS:
	 *                  The object was successfully deleted.
	 * @return     Error if the object could not be deleted.
	 * @details    This method is the destructor of the class.  It is called
	 *             when the object returned from the constructor is no longer
	 *             needed.  After an object is destructed it will never be used
	 *             again.
	 */
	isu_return_t (*destruct)(void* this);
	/**
	 * @brief      Start the scheduling algorithm with the given intial list of tasks.
	 * @param      this
	 *                  this algorithm object.
	 * @param      task_list
	 *                  The intial task list to start the scheduler with.  All
	 *                  the tasks in this list should be considered in the READY
	 *                  state.
	 * @return     ISU_RET_SUCCESS:
	 *                  The scheduler was successfully started.
	 * @return     Error if the scheduler could not be started.
	 */
	isu_return_t (*start)(void* this, isu_llist_t task_list);
	/**
	 * @brief      Stop the scheduler from running and return a list of ready tasks.
	 * @param      this
	 *                  This algorithm object.
	 * @return     A list of ready tasks if any exist or NULL if there are no 
	 *             ready tasks.
	 * @details    This method will stop the started scheduling algorithm and 
	 *             return a list of ready tasks which should include the 
	 *             currently running task at the HEAD of the list.
	 * @todo       Make sure to add the running task to the list of ready tasks
	 */
	isu_llist_t (*stop)(void* this);	// stop the scheduler and return a list of all currently ready tasks
	/**
	 * @brief      Add a task to the list of ready tasks in the scheduler.
	 * @param      this
	 *                  This algorithm object.
	 * @param      task
	 *                  The task to add to the ready list.
	 * @return     ISU_RET_SUCCESS:
	 *                  The task was successfully added to the ready list.
	 * @return     Error if the task could not be added to the list.
	 * @details    This method will be called anytime a new task is ready to 
	 *             run.  It should only add the task to the list and return
	 *             exit status.  If the algorithm is set to be premptive with
	 *             the is_premptive flag then after add_task() is called the
	 *             get_next_task() will be called next.
	 */
	isu_return_t (*add_task)(void* this, isu_task_t task);
	/**
	 * @brief      Get the next task that should be run on the processor.
	 * @param      this
	 *                  This algorithm object.
	 * @param      remove
	 *                  If true the task is no longer ready and should be
	 *                  removed from the ready task list.  If false the task
	 *                  should be considered still runnable and be moved into
	 *                  the ready list.
	 * @return     The next task to run or NULL if no task if the IDLE task 
	 *             should be run next.
	 */
	isu_task_t (*get_next_task)(void* this, int remove);
	/**
	 * The name of this scheduling algorithm.
	 */
	char const* name;
	/**
	 * If this algorithm is premptive or not.  If set to true the algorithm
	 * will be called each time a new task arrives after calling the
	 * add_task() method.
	 */
	int is_premptive;
	/**
	 * If this algorithm should be called peridically set this to a number
	 * greater than 0.  If this is set the get_next_task() method will be called
	 * every isu_sched_algorithm_t::quantum milliseconds.
	 */
	int quantum;
	/**
	 * If set to true then reset the quantum counter after each call to 
	 * get_next_task().  If set to false then get_next_task() will only
	 * call on intervals of isu_sched_algorithm_t::quanum and will force the 
	 * processor to idle if a task finishes before it's quanum is up.
	 */
	int reset_quantum;
}isu_sched_algorithm_t;


#endif

/// @}

