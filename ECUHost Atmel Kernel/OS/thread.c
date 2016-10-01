/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      THREAD                                                 */
/* DESCRIPTION:        This code provides thread stack functions              */
/*                                                                            */
/* FILE NAME:          THREAD.c                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "thread.h"
#include "declarations.h"
#include "stack.h"

typedef struct
{
	uint32 u32ThreadStack[SYS_THREAD_STACK_SIZE];
} THREAD_tastThreadStack;

THREAD_tastThreadStack THREAD_astThreadStack[SYS_THREAD_MAX];
tq_struct* THREAD_pstStackOwner[SYS_THREAD_MAX] = { NULL };

uint32* THREAD_pu32GetThreadStack(tq_struct* pstStackOwnerTask)
{
	uint32 u32StackIDX;
	uint32* pu32StackPointer = NULL;
	STACK_tstStackFrame* pstStackFrame;
	
	for(u32StackIDX = 0; u32StackIDX < SYS_THREAD_MAX; u32StackIDX++)
	{
		if(NULL == THREAD_pstStackOwner[u32StackIDX])
		{
			THREAD_pstStackOwner[u32StackIDX] = pstStackOwnerTask;
			pu32StackPointer = (uint32*)((uint32)&THREAD_astThreadStack[u32StackIDX] +
								(uint32)SYS_THREAD_STACK_SIZE * sizeof(uint32));				
			break;
		}		
	}
	
	/* set the thread entry point at the stack frame program counter
	   ready for the initial thread context restore */
	pu32StackPointer -= (sizeof(STACK_tstStackFrame) / sizeof(uint32));
	pstStackFrame = (STACK_tstStackFrame*)pu32StackPointer;
	pstStackFrame->u32PC = (uint32)pstStackOwnerTask->stTask.tTaskProgramCounter;
	
	return pu32StackPointer;
}






			
			
