/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TQUEUE Header File                                     */
/* DESCRIPTION:        This code provides definitions of task queue structs   */
/*                                                                            */
/* FILE NAME:          TQUEUEHA.h                                             */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef _TQUEUEHA_H
#define _TQUEUEHA_H

#include "tqueue.h"

void TQUEUEHA_vRunThreadTaskQueue(task_queue* list, pfInterTaskFunction pfInterThread);

#endif /* _TQUEUEHA_H */
