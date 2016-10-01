/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*   License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher    */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      OS Header File                                         */
/* DESCRIPTION:        This code provides macros that describe the OS task    */
/*                     schedule and OS enums                                  */
/* FILE NAME:          OS.h                                                   */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef OSHA_H
#define OSHA_H

#include "tqueue.h"

void OSHA_vLoadProcessStack(struct tq_struct*);


#endif //OSHA_H
	

