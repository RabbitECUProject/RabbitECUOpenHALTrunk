/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "CPUAbstract.h"
#include "types.h"

Bool CPU_boCriticalNestCounter;

void CPU_vStart(void)
{
	CPU_boCriticalNestCounter = 0;
}

void CPU_vEnterCritical(void)
{
	CPU_xEnterCritical();
	//CPU_boCriticalNestCounter++;	
}

void CPU_vExitCritical(void)
{
	//if (1 == CPU_boCriticalNestCounter)
	//{
		CPU_xExitCritical();
	//	CPU_boCriticalNestCounter = 0;
	//}
	//else
	//{
	//	CPU_boCriticalNestCounter--;			
	//}
}


