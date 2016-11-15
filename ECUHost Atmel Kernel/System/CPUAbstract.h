/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CPUAbstract Header File                                */
/* DESCRIPTION:        This code provides high level CPU abstraction macros   */
/*                                                                            */
/* FILE NAME:          CPUAbstract.h                                          */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "sys.h"
#include "types.h"

#ifndef HARDABSTRACT_H
#define HARDABSTRACT_H

#ifdef BUILD_MK60
	#include "mk60f12.h"
#endif
#ifdef BUILD_SAM3X8E
	#include "sam3xa.h"
#endif

#define CPUA_OS_u32GetSysTimeStamp()			0xFFFFFF; //TODO

#define	CPU_xEnterCritical()		__disable_irq()
#define CPU_xExitCritical()		__enable_irq()

void CPU_vStart(void);
void CPU_vEnterCritical(void);
void CPU_vExitCritical(void);

#endif //HARDABSTRACT_H

