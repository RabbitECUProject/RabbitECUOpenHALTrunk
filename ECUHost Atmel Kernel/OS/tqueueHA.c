/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TQUEUE Hardware Adaption                               */
/* DESCRIPTION:        This code provides functions for running task queues   */
/*                                                                            */
/* FILE NAME:          TQUEUEHA.h                                             */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <string.h>
#include "CPUAbstract.h"
#include "tqueue.h"
#include "types.h"
#include "task.h"


static void run_thread_task_queue(task_queue* list, pfInterTaskFunction pfInterThread);

void TQUEUEHA_vRunThreadTaskQueue(task_queue* list, pfInterTaskFunction pfInterThread)
{
	run_thread_task_queue(list, pfInterThread);
}

static void run_thread_task_queue(task_queue* list, pfInterTaskFunction pfInterThread)
{
	__asm(".EQU TASK_PC_OFFSET, 22");
	__asm(".EQU TASK_SP_OFFSET, 18");
	__asm(".EQU FRAME_R0_OFFSET, 0");
	__asm(".EQU FRAME_R1_OFFSET, 4");
	__asm(".EQU FRAME_R2_OFFSET, 8");
	__asm(".EQU FRAME_R3_OFFSET, 12");
	__asm(".EQU FRAME_R12_OFFSET, 16");
	__asm(".EQU FRAME_LR_OFFSET, 20");
	__asm(".EQU FRAME_PC_OFFSET, 24");
	__asm(".EQU FRAME_XPSR_OFFSET, 28");
	__asm(".EQU TASK_PRIVILEGE_OFFSET, 42");
	__asm(".EQU MASK_XPSR_THUMB, 16777216");

/* get current task in R1 */
	__asm("LDR		R1, [R0]");	
	
/* change the control register stack select to Process Stack Pointer and */
/* set the privilege from the task privilege enum */
	__asm("MOV		R2, #2");
	__asm("LDR		R3, [R1, #TASK_PRIVILEGE_OFFSET]");
	__asm("AND		R3, #1");
	__asm("ORR		R3, R2");
	__asm("MSR		CONTROL, R3");
	__asm("ISB");
	
/* restore the current task stack pointer from task structure stack pointer */
	__asm("LDR		SP, [R1, #TASK_SP_OFFSET]");	
		
/* restore the XPSR, setting thumb bit always */		
	__asm("LDR		R0, [SP, #FRAME_XPSR_OFFSET]");
	__asm("ORR		R0, #MASK_XPSR_THUMB");		
/* __asm("MSR		XPSR, R0"); matthew*/
		
/* set the thumb bit of the return vector */		
	__asm("LDR		R0, [SP, #FRAME_PC_OFFSET]");
	__asm("ORR		R0, #1");
	__asm("STR		R0, [SP, #FRAME_XPSR_OFFSET]");
		
/* restore stack frame	remaining registers then return	*/
	__asm("POP		{R0, R1, R2, R3, R12, LR}");
	__asm("ADD		SP, #4");
	__asm("POP		{PC}");

/* restore current task program counter from task structure program counter */
/*		LDR		PC, [R1, #TASK_PC_OFFSET] */
}

			
			
