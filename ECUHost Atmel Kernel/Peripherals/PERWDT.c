/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      WDT                                                    */
/* DESCRIPTION:        This code provides watchdog functions                  */
/*                                                                            */
/* FILE NAME:          PERWDT.c                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "PERWDT.h"

void WDT_vStart(uint16 u16ResetVal, uint16 u16WindowVal)
{
	WDTHA_vStart(u16ResetVal, u16WindowVal);
}

Bool WDT_boReset(uint16 u16InputVal)
{
	return WDTHA_boReset(u16InputVal);
}

