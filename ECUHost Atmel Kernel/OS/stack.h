/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      STACK Header File                                      */
/* DESCRIPTION:        This code provides struct definition for thread stack  */
/*                                                                            */
/* FILE NAME:          STACK.h                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef STACK_H
#define STACK_H

#include "types.h"

typedef struct
{
	uint32 u32R0;
	uint32 u32R1;
	uint32 u32R2;
	uint32 u32R3;
	uint32 u32R12;
	uint32 u32LR;
	uint32 u32PC;
	uint32 u32xPSR;
} STACK_tstStackFrame;

#endif //STACK_H

