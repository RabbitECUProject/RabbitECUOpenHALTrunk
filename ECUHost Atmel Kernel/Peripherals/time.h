/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TIME Header File                                       */
/* DESCRIPTION:        This code provides struct definitions for the RTC      */
/*                     hardware module                                        */	
/* FILE NAME:          TIME.h                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TIME_H
#define TIME_H

#include "declarations.h"
#include "CPUAbstract.h"
#include "os.h"
#include "sys.h"
#include "time.h"


typedef struct
{
	uint32 biYear:7;
	uint32 biMonth:4;
	uint32 biDay:5;
	uint32 biHours:5;
	uint32 biMinutes:6;
	uint32 biSeconds:5;
} tstTime;


void TIME_vStart(uint32* const);
void TIME_vRun(uint32* const);
void TIME_vTerminate(uint32* const);
Bool TIME_stGetTime(tstTime* const);
uint32 TIME_u32GetFATTime(void);

#endif //TIME_H

