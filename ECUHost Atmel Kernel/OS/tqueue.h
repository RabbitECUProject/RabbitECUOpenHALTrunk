/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TQUEUE Header File                                     */
/* DESCRIPTION:        This code provides definitions of task queue structs   */
/*                                                                            */
/* FILE NAME:          TQUEUE.h                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef _TQUEUE_H
#define _TQUEUE_H

#include "types.h"
#include "task.h"
#include "TASKAPI.h"

/* modified linux task queue header */
typedef void (*pfInterTaskFunction)(void);

typedef BUILDPACKEDKEIL struct tq_struct  
{
	struct tq_struct* next;			/* linked list of queued tasks */
	struct tq_struct* prev;			/* doubly linked list for removal and insertion */
	struct TASK_tstTask stTask;			/* the task */
} BUILDPACKEDATMEL tq_struct;

typedef struct tq_struct* task_queue;


void queue_task(struct tq_struct* task_pointer, task_queue* task_list);
void run_cooperative_task_queue(task_queue* list, pfInterTaskFunction, uint32* pu32Stat);
void TQUEUE_vRunThreadTaskQueue(task_queue* list, pfInterTaskFunction);

#endif /* _TQUEUE_H */
