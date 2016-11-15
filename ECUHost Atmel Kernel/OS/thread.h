/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      THREAD Header File                                     */
/* DESCRIPTION:        This code provides thread stack function declarations  */
/*                                                                            */
/* FILE NAME:          THREAD.h                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef THREAD_H
#define THREAD_H

#include "sys.h"
#include "types.h"
#include "task.h"
#include "TASKAPI.h"
#include "tqueue.h"

uint32* THREAD_pu32GetThreadStack(struct tq_struct*);
	
#endif //THREAD_H
	

