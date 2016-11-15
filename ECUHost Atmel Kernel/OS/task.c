/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      TASK                       														*/
/* DESCRIPTION:        This code provides functions for reporting of tasks		*/
/*										 																												*/
/* FILE NAME:          TASK.c                                             		*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "TASKAPI.h"
#include "task.h"

TASK_tsTime TASK_vReportRunTime(struct TASK_tstTask* const pstTask)
{
	return pstTask->tsTaskFinished - pstTask->tsTaskStarted;
}





			
			
