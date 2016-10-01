/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                               							*/
/* PACKAGE TITLE:      TQUEUE                      														*/
/* DESCRIPTION:        This code provides functions for running task queues		*/
/*										 																												*/
/* FILE NAME:          TQUEUE.h                                             	*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <string.h>
#include "tqueue.h"
#include "types.h"
#include "task.h"
#include "CPUAbstract.h"


void queue_task( tq_struct* task_pointer, task_queue* task_list )
{
		task_pointer->next = *task_list;
		*task_list = (task_queue)task_pointer;
}

/*
 * Call all tasks in a given list.
 */
void run_cooperative_task_queue( task_queue* list, pfInterTaskFunction pfInterTask, uint32* pu32Stat )
{
	tq_struct* p;
	tq_struct* save_p;
	void (*f)(uint32*);
	
	p = (tq_struct*)*list;
	while ( p != NULL ) 
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

__asm void run_thread_task_queue(task_queue* list, pfInterTaskFunction pfInterThread)
{
TASK_PC_OFFSET	EQU	22
TASK_SP_OFFSET	EQU	18
	
FRAME_R0_OFFSET					EQU	0
FRAME_R1_OFFSET					EQU	4
FRAME_R2_OFFSET					EQU	8
FRAME_R3_OFFSET					EQU	12
FRAME_R12_OFFSET				EQU	16
FRAME_LR_OFFSET					EQU	20
FRAME_PC_OFFSET					EQU	24
FRAME_XPSR_OFFSET				EQU	28
	
TASK_PRIVILEGE_OFFSET		EQU	42
MASK_XPSR_THUMB					EQU	16777216
;
;
;
;	
;	
; get current task in R1
		LDR		R1, [R0]	
	
; change the control register stack select to Process Stack Pointer and
; set the privilege from the task privilege enum
		MOV		R2, #2
		LDR		R3, [R1, #TASK_PRIVILEGE_OFFSET]
		AND		R3, #1
		ORR		R3, R2
		MSR		CONTROL, R3
		ISB
	
; restore the current task stack pointer from task structure stack pointer
		LDR		SP, [R1, #TASK_SP_OFFSET]	
		
; restore the XPSR, setting thumb bit always		
		LDR		R0, [SP, #FRAME_XPSR_OFFSET]
		ORR		R0, #MASK_XPSR_THUMB		
		MSR		XPSR, R0
		
; set the thumb bit of the return vector		
		LDR		R0, [SP, #FRAME_PC_OFFSET]
		ORR		R0, #1
		STR		R0, [SP, #FRAME_XPSR_OFFSET]
		
; restore stack frame	remaining registers then return	
		POP		{R0, R1, R2, R3, R12, LR}
		ADD		SP, #4
		POP		{PC}

; restore current task program counter from task structure program counter
;		LDR		PC, [R1, #TASK_PC_OFFSET]
	
; todo manage thread end or yield
}

			
			
