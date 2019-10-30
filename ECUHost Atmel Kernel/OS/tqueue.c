/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TQUEUE                                                 */
/* DESCRIPTION:        This code provides functions for running task queues   */
/*                                                                            */
/* FILE NAME:          TQUEUE.h                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <string.h>
#include "tqueue.h"
#include "tqueueHA.h"
#include "types.h"
#include "task.h"
#include "CPUAbstract.h"

void queue_task(tq_struct* task_pointer, task_queue* task_list)
{
		task_pointer->next = *task_list;
		*task_list = (task_queue)task_pointer;
}

/*
 * Call all tasks in a given list.
 */
void run_cooperative_task_queue(task_queue* list, pfInterTaskFunction pfInterTask, uint32* pu32Stat)
{
	tq_struct* p;
	tq_struct* save_p;
	void (*f)(uint32*);
	
	p = (struct tq_struct*)*list;
	while (p != NULL) 
	{
		f      = p -> stTask.pfTaskFunction;		// copy the task function pointer
	    p -> stTask.tsTaskStarted = CPUA_OS_u32GetSysTimeStamp();		
		save_p = p;															// save the tq_struct
		p      = (tq_struct*)p -> next;					// point to next node		
		(*f)(pu32Stat);													// run the task function
		save_p -> stTask.tsTaskFinished = CPUA_OS_u32GetSysTimeStamp();		
		*list = (task_queue)p;									// update the list
		save_p -> next = NULL;									// set the old tq_struct->next to NULL
		if (NULL != pfInterTask) pfInterTask();	// run the intertask function
	}
}

void TQUEUE_vRunThreadTaskQueue(task_queue* list, pfInterTaskFunction pfInterThread)
{
	TQUEUEHA_vRunThreadTaskQueue(list, pfInterThread);
}