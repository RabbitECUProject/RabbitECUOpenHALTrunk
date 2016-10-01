/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      TIME                  																	*/
/* DESCRIPTION:        This code provides setup and functions for the RTC			*/
/*										 hardware module																				*/	
/* FILE NAME:          TIME.c                                          				*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include "TIME.h"

void TIME_vStart(uint32* const pu32Stat)
{
	OS_xModuleStartOK(*pu32Stat);
}

void TIME_vRun(uint32* const pu32Stat)
{

}

void TIME_vTerminate(uint32* const pu32Stat)
{

}

bool TIME_stGetTime(tstTime* const pstTime)
{
	return true;
}

uint32 TIME_u32GetFATTime(void)
{
	return 0;
}
