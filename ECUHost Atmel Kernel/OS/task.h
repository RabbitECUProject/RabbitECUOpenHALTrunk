/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TASK Header File                                       */
/* DESCRIPTION:        This code provides definitions of task structs and     */
/*                     enums                                                  */
/* FILE NAME:          TASK.h                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TASK_H
#define TASK_H

#include "types.h"
#include "TASKAPI.h"

typedef enum
{
	enTaskNew,
	enTaskRunning,
	enTaskReady,
	enTaskFinished,
	enTaskBlocked
} TASK_tenState;

typedef enum
{ 
	enTaskFunction,
	enTaskCyclic,
	enTaskThread
} TASK_tenType;

typedef enum
{
	enTaskPrivileged = 0,
	enTaskUnprivileged = 1
} TASK_tenPrivilege;

typedef uint16 TASK_tID;
typedef uint32 TASK_tsTime;
typedef void* TASK_tProgramCounter;
typedef void* TASK_tStackPointer;
typedef	void (*TASK_pfTaskFunction)(uint32* const);


typedef BUILDPACKED struct TASK_tstTask
{
	TASK_tID tTaskID;                           //0
	TASK_tenType enTaskType;                    //2
	TASKAPI_tenPriority enTaskPriority;         //3
	TASK_tenState enTaskState;                  //4
	TASKAPI_tenRateMs enTaskRateMS;             //5
	TASK_pfTaskFunction pfTaskFunction;         //6
	TASK_tStackPointer tTaskStackPointer;       //10
	TASK_tProgramCounter tTaskProgramCounter;   //14
	TASK_tsTime tsTaskQueued;                   //18
	TASK_tsTime tsTaskStarted;                  //22
	TASK_tsTime tsTaskFinished;                 //26
	TASK_tsTime tsTaskAccumulated;              //30
	TASK_tenPrivilege enPrivilege;              //34
} TASK_tstTask;

TASK_tsTime TASK_vReportRunTime(struct TASK_tstTask* const);
	
#endif //TASK_H
	

