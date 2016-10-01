/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      XXX Header File                    										*/
/* DESCRIPTION:        XXX   																									*/
/* FILE NAME:          XXX.h                                         					*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef PIT_H
#define PIT_H

#include "sys.h"

#define PIT_TLVAL0_val	(SYS_PERIOD_RTI_nS / SYS_PERIOD_OSC_nS)

/*<AUTOGEN START>*/
#define PIT_nReg32Set 																													\
{																																									\
	{ NULL, 0, REGSET_enOverwrite }																									\
};
/*<AUTOGEN END>*/

void PIT_vStart(uint32* const);
void PIT_vRun(uint32* const);
void PIT_vTerminate(uint32* const);

#endif //PIT_H

