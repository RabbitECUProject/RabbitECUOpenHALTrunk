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
#ifndef OS_H
#define OS_H

#include <string.h>
#include "declarations.h"
#include "compiler.h"

#ifdef OS_C
#include "OSHA.h"
#include "OSTASKS.h"
#include "sys.h"
#include "TASKAPI.h"
#endif

#include "SYSAPI.h"
#include "task.h"
#include "tqueue.h"
#include "thread.h"
#include "tqueueHA.h"

typedef enum
{
	OS_enOSStateReset,
	OS_enOSStatePreBootloader,
	OS_enOSStatePreOS,
	OS_enOSStartRequest,
	OS_enOSStateOSStarted,
	OS_enOSStateAppStarted,
	OS_enOSStateCount
} OS_tenOSState;

typedef enum
{
	OS_enOSModeKernel,
	OS_enOSModeClient
} OS_tenOSKernelMode;

typedef enum
{
	OS_enOSSchedulerTick,
	OS_enOSSchedulerSWI,	
	OS_enOSSchedulerBHRequest
} OS_tenOSScheduleCallContext;

typedef enum
{
	OS_enKERNELQueueStart,
	OS_enKERNELQueueCyclic,
	OS_enKERNELQueueThread,
	OS_enKERNELQueueTerminate,
	OS_enCLIENTQueueStart,
	OS_enCLIENTQueueCyclic,
	OS_enCLIENTQueueThread,
	OS_enCLIENTQueueTerminate,
	OS_enTaskQueueCount
} OS_tenQueueType;

	
#define OS_xModuleStartOK(x) 			((x) = ((x) << 1) | 1)
#define OS_xModuleStartFAIL(x)		((x) = (x) << 1)
#define OS_xModuleRunOK(x) 				((x) = ((x) << 1) | 1)
#define OS_xModuleRunEnd(x) 			((x) = ((x) << 1))
#define OS_xModuleRunFAIL(x)			((x) = ((x) = 0)
#define OS_xAddTaskToRunQueue(x) 	(		
#define OS_nTickCounterTerminal 		4000000000u
#define OS_MODULE_START(x)      	x (&OS_ucModuleStartStatus);
#define OS_nKernelHandle						0xF0001000
	
/* macro to add any task to the static task queue */
#define OS_xAddTaskToList(x, y)		memcpy((OS_tstSYSTaskNodePool*)x->stTask[(OS_tstSYSTaskNodePool*)x->u16Count++], \
																		(TASK_tstTask*)y, sizeof(TASK_tstTask))

#ifdef SYS_TASK_PRIORITISE		
	#define OS_xPrioritiseQueue(x)	OS_vPrioritiseQueue(x);
#else
	#define OS_xPrioritiseQueue(x)	OS_vPrioritiseQueue(NULL);
#endif

#define OS_nClientTasksMax 35u

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _OS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN SYSAPI_tstSVCDataStruct OS_stSVCDataStruct;
EXTERN uint32 au32SVCResult[10];
EXTERN uint32 OS_u32TickCounter;

void OS_vStart(uint32* const);
void OS_vRun(uint32* const);
void OS_vSchedule(OS_tenOSScheduleCallContext);
void OS_vTerminate(uint32* const);
void OS_vSetCyclicTaskList(struct TASK_tstTask*);
void OS_vPrioritiseQueue(task_queue*);
void OS_vBackgroundDispatcher(void);
OS_tenOSState OS_enGetState(void);
void OS_vSetState(OS_tenOSState);
task_queue* OS_stGetOSQueue(OS_tenQueueType);
OS_tenOSKernelMode OS_enGetOSKernelMode(void);
void OS_vOverrunTaskReport(OS_tenQueueType);
void OS_vSuspendThread(OS_tenQueueType);
void OS_vSystemIdleThread(uint32* const);
void OS_vInterCoopTasks(void);
SYSAPI_tenSVCResult OS_enAddTaskToQueue(OS_tenQueueType, struct TASK_tstTask*);

#endif //OS_H
	

