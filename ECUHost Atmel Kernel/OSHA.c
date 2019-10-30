/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      OS                                                     */
/* DESCRIPTION:        This code provides functions for creating OS tasks     */
/*                     and includes the scheduler that creates the task       */
/*                     queues every OS tick, runs the tasks queues then       */
/*                     invokes threads round robin                            */
/*                     the CAN hardware module/s                              */
/* FILE NAME:          OS.c                                                   */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _OSHA_C

#include "OSHA.h"

static void OSHA_vLoadProcessStackASM(struct tq_struct*);

void OSHA_vLoadProcessStack(struct tq_struct* pstDispatcherTask)
{
	OSHA_vLoadProcessStackASM(pstDispatcherTask);
}


//static void __asm OSHA_vLoadProcessStackASM(tq_struct* pstDispatcherTask)
//{
//TASK_PC_OFFSET					EQU	22
//TASK_SP_OFFSET					EQU	18
//MASK_CONTROL_SPSEL_PSP	EQU	2
//
//; load PSP with dispatcher task stack pointer	
		//LDR		R1, [R0, #TASK_SP_OFFSET]
		//MSR		PSP, R1
		//MRS		R0, CONTROL
		//ORR		R0, #MASK_CONTROL_SPSEL_PSP
		//MSR		CONTROL, R0
		//ISB	
		//BX		LR
//}

#ifdef BUILD_MK60
static void OSHA_vLoadProcessStackASM(struct tq_struct* pstDispatcherTask)
{
	__asm("TASK_PC_OFFSET					EQU	22");
	__asm("TASK_SP_OFFSET					EQU	18");
	__asm("MASK_CONTROL_SPSEL_PSP	EQU	2");
	__asm("; load PSP with dispatcher task stack pointer");
	__asm("LDR		R1, [R0, #TASK_SP_OFFSET]");
	__asm("MSR		PSP, R1");
	__asm("MRS		R0, CONTROL");
	__asm("ORR		R0, #MASK_CONTROL_SPSEL_PSP");
	__asm("MSR		CONTROL, R0");
	__asm("ISB");
	__asm("BX		LR");
}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
static void OSHA_vLoadProcessStackASM(struct tq_struct* pstDispatcherTask)
{
	/* load PSP with dispatcher task stack pointer */
	__asm("LDR		R1, [R0, #18]"); //18 is TASK_SP_OFFSET CAVEAT relies on struct packing check offset!!!
	__asm("MSR		PSP, R1");
	__asm("MRS		R0, CONTROL");
	__asm("ORR		R0, #2"); //2 is MASK_CONTROL_SPSEL_PSP
	__asm("MSR		CONTROL, R0");
	__asm("ISB");
	__asm("BX		LR");
}
#endif //BUILD_SAM3X8E



			
			
