/**
 * @addtogroup scheduler Scheduler
 * @brief     The scheduler system of the operating system.
 * @details   The scheduler handles installing of scheduler algorithms and
 *            calls the algorithm to determin the next task to run at the correct
 *            times.  
 * @{
 */
/**
 * @file      isu_sched.h
 * @author    Jeramie Vens
 * @date      3/15/15 - Created
 * @date      3/16/15 - documented, changed the prototype of isu_sched_ListAlgorithm()
 *            to return llist object.
 * @date      3/23/15 - Added to the new project repository, added On* funtions
 * @brief     The main include file for the scheduler system.
 * @details   This file should be included by files that need to deal with the
 *            scheduler and is used to call the actual scheduling algorithm.
 */





#ifndef ISU_SCHED_H
#define ISU_SCHED_H

#include "common/isu_types.h"
#include "isu_sched_algorithm.h"

isu_return_t isu_sched_Init();
isu_return_t isu_sched_Destroy();
isu_return_t isu_sched_InstallAlgorithm(isu_sched_algorithm_t *scheduler);
isu_return_t isu_sched_UninstallAlgorithm(char *name);
int isu_sched_SetAlgorithm(char *name);
char* isu_sched_GetAlgorithm();
isu_llist_t isu_sched_ListAlgorithm();
isu_task_t isu_sched_OnTick(int time);
isu_task_t isu_sched_OnTaskEnd(void);
isu_task_t isu_sched_OnTaskArrive(isu_task_t task);

#endif

/// @}
