/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      OS                                                     */
/* DESCRIPTION:        This code provides functions for creating OS tasks     */
/*                     and includes the scheduler that creates the task       */
/*                     queues every OS tick, runs the tasks queues then       */
/*                     invokes threads round robin                            */
/*                     the CAN hardware module/s                              */
/* FILE NAME:          OS.c                                                   */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _OS_C

#include <string.h>
#include "OS.h"
#include "OSHA.h"
#include "OSTASKS.h"


/* declare constant task arrays */
const struct TASK_tstTask OS_rastTaskStart[] = OSTASKS_nTaskStart;
const struct TASK_tstTask OS_rastTaskCyclic[] = OSTASKS_nTaskCyclic;
const struct TASK_tstTask OS_rastTaskTerminate[] = OSTASKS_nTaskTerminate;
#define OS_nKERNELTaskStartCount sizeof(OS_rastTaskStart) / sizeof(struct TASK_tstTask)
#define OS_nKERNELTaskCyclicCount sizeof(OS_rastTaskCyclic) / sizeof(struct TASK_tstTask)
#define OS_nKERNELTaskTerminateCount sizeof(OS_rastTaskTerminate) / sizeof(struct TASK_tstTask)
#define OS_nTaskCount (OS_nKERNELTaskStartCount + OS_nKERNELTaskCyclicCount + OS_nKERNELTaskTerminateCount + SYS_THREAD_MAX + 1)	/* + 1 for system idle thread */

task_queue OS_apstTaskQueue[OS_enTaskQueueCount] = {NULL};
tq_struct* OS_pstBackgroundDispatcherTask;

typedef struct 
{
	uint32 u32StartCount;
	uint32 u32CyclicCount;
	uint32 u32TerminateCount;
	uint32 u32TotalCount;
	tq_struct stqTask[OS_nTaskCount];
} OS_tstKERNELTaskNodePool;

typedef struct 
{
	uint32 u32StartCount;
	uint32 u32CyclicCount;
	uint32 u32TerminateCount;
	uint32 u32TotalCount;
	tq_struct stqTask[OS_nClientTasksMax];
} OS_tstCLIENTTaskNodePool;

/* local functions */
void OS_vBackgroundDispatcher(void);
Bool OS_boCreateThread(OS_tenQueueType, TASK_tProgramCounter);
Bool OS_boCreateDispatcherTask(OS_tenQueueType, TASK_tProgramCounter);
void OS_vSystemDummyThread(uint32* const);
static void OS_vRunQueues(void);

/* module data */
OS_tstKERNELTaskNodePool OS_stKERNELTaskNodePool;
OS_tstCLIENTTaskNodePool OS_stCLIENTTaskNodePool;

uint32 OS_u32CLIENTStartTaskCount;
uint32 OS_u32CLIENTCyclicTaskCount;
uint32 OS_u32CLIENTTerminateTaskCount;

OS_tenOSState OS_enOSState;
OS_tenOSKernelMode enOSKernelMode;
Bool OS_bModuleActive[OS_nKERNELTaskStartCount];
uint32 OS_u32StartTaskStatus;
uint32 OS_u32RunTaskStatus;
uint32 OS_u32TerminateTaskStatus;
uint8 OS_u8SchedulerIDX;
uint32 OS_u32SchedulerRunStatus;
uint32 OS_u32SysTickInterval;
uint32 OS_u32SysRunTimeHigh;
uint32 OS_u32SysRunTimeLow;


void OS_vStart(uint32* const u32Stat)
{
	uint32 u32ModuleStartStatus;
	uint8 u8IDX;	
		
	OS_enOSState = OS_enOSStatePreOS;
	enOSKernelMode = enOSKernelMode;
	OS_u32TickCounter = 0;
	CPU_vStart();
	
	OS_u32CLIENTStartTaskCount = 0;
	OS_u32CLIENTCyclicTaskCount = 0;
	OS_u32CLIENTTerminateTaskCount = 0;	
	
	OS_stKERNELTaskNodePool.u32StartCount = OS_nKERNELTaskStartCount;	
	OS_stKERNELTaskNodePool.u32CyclicCount = OS_nKERNELTaskCyclicCount;	
	OS_stKERNELTaskNodePool.u32TerminateCount = OS_nKERNELTaskTerminateCount;		
	OS_stKERNELTaskNodePool.u32TotalCount = 0;	
	
	OS_stCLIENTTaskNodePool.u32StartCount = 0;	
	OS_stCLIENTTaskNodePool.u32CyclicCount = 0;	
	OS_stCLIENTTaskNodePool.u32TerminateCount = 0;		
	OS_stCLIENTTaskNodePool.u32TotalCount = 0;	
	
	OS_stSVCDataStruct.pvData = (void*)&au32SVCResult;	
	
	/* add to static node pool, queue and run the start task list */
	for(u8IDX = 0; u8IDX < OS_nKERNELTaskStartCount; u8IDX++)
	{
		memcpy((void*)&OS_stKERNELTaskNodePool.stqTask[u8IDX].stTask,
						(void*)&OS_rastTaskStart[u8IDX], sizeof(struct TASK_tstTask));
		
		OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount++].stTask.tsTaskQueued = CPUA_OS_u32GetSysTimeStamp();
		queue_task(&OS_stKERNELTaskNodePool.stqTask[u8IDX], &OS_apstTaskQueue[OS_enKERNELQueueStart]);
	}
	
	OS_xPrioritiseQueue(&OS_apstTaskQueue[OS_enKERNELQueueStart]);
	
	/* Run the KERNEL start task queue */
	run_cooperative_task_queue(&OS_apstTaskQueue[OS_enKERNELQueueStart], NULL, &u32ModuleStartStatus);
	
	/* Run the CLIENT start task queue */
	run_cooperative_task_queue(&OS_apstTaskQueue[OS_enCLIENTQueueStart], NULL, &u32ModuleStartStatus);	
	
	
	
	/* add to static node pool the cyclic task list */
	for(u8IDX = OS_nKERNELTaskStartCount; u8IDX < (OS_nKERNELTaskStartCount + OS_nKERNELTaskCyclicCount); u8IDX++)
	{
		memcpy((void*)&OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount++].stTask, 
						(void*)&OS_rastTaskCyclic[u8IDX - OS_nKERNELTaskStartCount], sizeof(struct TASK_tstTask));		
	}	
	
	
	
	/* create the kernel idle thread */
	(void)OS_boCreateThread(OS_enKERNELQueueThread, &OS_vSystemIdleThread);	
		
	/* create a dummy thread */
	(void)OS_boCreateThread(OS_enKERNELQueueThread, &OS_vSystemDummyThread);
	
	/* create the background dispatcher task */
	(void)OS_boCreateDispatcherTask(OS_enKERNELQueueCyclic, &OS_vBackgroundDispatcher);
	
	OS_xModuleStartOK(*u32Stat);
	OS_enOSState = OS_enOSStartRequest;	
}

void OS_vRun(uint32* const u32Stat)
{
	OSHA_vLoadProcessStack(OS_pstBackgroundDispatcherTask);
	OS_vBackgroundDispatcher();
}

void OS_vSchedule(OS_tenOSScheduleCallContext enOSScheduleCallContext)
{
	
	for(OS_u8SchedulerIDX = OS_stKERNELTaskNodePool.u32StartCount; OS_u8SchedulerIDX < (OS_stKERNELTaskNodePool.u32StartCount + OS_stKERNELTaskNodePool.u32CyclicCount); OS_u8SchedulerIDX++)
	{
		if((OS_u32TickCounter % OS_stKERNELTaskNodePool.stqTask[OS_u8SchedulerIDX].stTask.enTaskRateMS) == 0)
		{
			/* only call module run task if module was activated successfully at start */
			//if(OS_bModuleActive[u8IDX] == true)
			//{
				/* queue then run the KERNEL cyclic task list */
				OS_stKERNELTaskNodePool.stqTask[OS_u8SchedulerIDX].stTask.tsTaskQueued = CPUA_OS_u32GetSysTimeStamp();			
				queue_task(&OS_stKERNELTaskNodePool.stqTask[OS_u8SchedulerIDX], &OS_apstTaskQueue[OS_enKERNELQueueCyclic]);	
				//OS_xPrioritiseQueue(&OS_apstTaskQueue[OS_enKERNELQueueCyclic]);			
			//}
		}
	}
	
	for(OS_u8SchedulerIDX = OS_stCLIENTTaskNodePool.u32StartCount; OS_u8SchedulerIDX < (OS_stCLIENTTaskNodePool.u32StartCount + OS_stCLIENTTaskNodePool.u32CyclicCount); OS_u8SchedulerIDX++)
	{
		if((OS_u32TickCounter % OS_stCLIENTTaskNodePool.stqTask[OS_u8SchedulerIDX].stTask.enTaskRateMS) == 0)
		{
			/* only call module run task if module was activated successfully at start */
			//if(OS_bModuleActive[u8IDX] == true)
			//{
				/* queue then run the KERNEL cyclic task list */
				OS_stCLIENTTaskNodePool.stqTask[OS_u8SchedulerIDX].stTask.tsTaskQueued = CPUA_OS_u32GetSysTimeStamp();
				queue_task(&OS_stCLIENTTaskNodePool.stqTask[OS_u8SchedulerIDX], &OS_apstTaskQueue[OS_enCLIENTQueueCyclic]);			
			//}
		}
	}
}

void OS_vBackgroundDispatcher()
{
		/* run the Kernel cyclic task queue */
		run_cooperative_task_queue(&OS_apstTaskQueue[OS_enKERNELQueueCyclic], OS_vInterCoopTasks, &OS_u32SchedulerRunStatus);
		
		/* run the Client cyclic task queue */
		run_cooperative_task_queue(&OS_apstTaskQueue[OS_enCLIENTQueueCyclic], OS_vInterCoopTasks, &OS_u32SchedulerRunStatus);
	
		/* invoke the Kernel next thread to run round robin */
		TQUEUEHA_vRunThreadTaskQueue(&OS_apstTaskQueue[OS_enKERNELQueueThread], OS_vInterCoopTasks);
}

void OS_vTerminate(uint32* const u32Stat)
{

}

void OS_vInterCoopTasks(void)
{
	OS_vRunQueues();
}

static void OS_vRunQueues(void)
{
	CLIENT_vRunUserCBQueue();
}

void OS_vSetCyclicTaskList(struct TASK_tstTask* pstTask)
{
	uint16 u16TaskID = 1000u;
	while((pstTask->pfTaskFunction != NULL) && 
				  (pstTask->tTaskID == u16TaskID) &&
					(u16TaskID++ < (1000u + OS_nClientTasksMax)))
	{
		memcpy((void*)&OS_stCLIENTTaskNodePool.stqTask[OS_stCLIENTTaskNodePool.u32TotalCount++].stTask, 
						(void*)pstTask++, sizeof(struct TASK_tstTask));
	}	
}

void OS_vPrioritiseQueue(task_queue* list)
{
	tq_struct* pstNode1;
	tq_struct* pstNode2;
	tq_struct* pstNode3;
	uint16 u16I;
	uint16 u16J;
	uint16 u16NodeCount = 1;
	
	
	pstNode1 = (tq_struct*)*list;
	
	while(pstNode1 -> next != NULL)
	{
		u16NodeCount++;
		pstNode1 = (tq_struct*)pstNode1 -> next;
	}
	
	for(u16I = 0; u16I < u16NodeCount - 1; u16I++)
	{
		pstNode1 = (tq_struct*)list;
		pstNode2 = (tq_struct*)(pstNode1 -> next);
		pstNode3 = (tq_struct*)(pstNode2 -> next);
		
		for(u16J = 0; u16J < u16NodeCount - u16I - 1; u16J++)
		{
			if(pstNode2 -> stTask.enTaskPriority  > pstNode3 -> stTask.enTaskPriority)
			{
				pstNode2 -> next = (struct tq_struct*)(pstNode3 -> next);
				pstNode3 -> next = (struct tq_struct*)pstNode2;

				if(u16J == 0)
				{
					*list = (struct tq_struct*)pstNode3;
				}
				else	
				{
					pstNode1 -> next = (struct tq_struct*)pstNode3;
				}
				
				pstNode1 = pstNode3;
				pstNode3 = (tq_struct*)(pstNode2 -> next);				
			}
			else
			{
				pstNode1 = pstNode2;
				pstNode2 = pstNode3;
				pstNode3 = (tq_struct*)(pstNode3 -> next);
			}
		}
	}
}

OS_tenOSState OS_enGetState(void)
{
	return OS_enOSState;
}

void OS_vSetState(OS_tenOSState enOSState)
{
	OS_enOSState = enOSState;
}

task_queue* OS_stGetOSQueue(OS_tenQueueType enQueueType)
{
	return &OS_apstTaskQueue[enQueueType];
}

OS_tenOSKernelMode OS_enGetOSKernelMode(void)
{
	return enOSKernelMode;
}

void OS_vOverrunTaskReport(OS_tenQueueType enQueueType)
{
	
}

void OS_vSuspendThread(OS_tenQueueType enQueueType)
{
	
}

Bool OS_boCreateThread(OS_tenQueueType enQueueType, TASK_tProgramCounter pEntryPoint)
{
	/* add the thread to system node pool */
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.tTaskID = 1000;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.enTaskType = enTaskThread;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.enTaskState = enTaskReady;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.pfTaskFunction = (TASK_pfTaskFunction)pEntryPoint;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.tTaskProgramCounter = pEntryPoint;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.tTaskStackPointer = 
			THREAD_pu32GetThreadStack(&OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount]);	
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.enPrivilege = 
		(OS_enKERNELQueueThread == enQueueType) ? enTaskPrivileged : enTaskUnprivileged;
	
	queue_task(&OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount], &OS_apstTaskQueue[enQueueType]);	
	OS_stKERNELTaskNodePool.u32TotalCount++;	

	return true;	
}

Bool OS_boCreateDispatcherTask(OS_tenQueueType enQueueType, TASK_tProgramCounter pEntryPoint)
{
	/* add the task to system node pool */
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.tTaskID = 1000;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.enTaskType = enTaskFunction;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.enTaskState = enTaskReady;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.pfTaskFunction = (void (*)(uint32 *const))pEntryPoint;
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.tTaskStackPointer = 
			THREAD_pu32GetThreadStack(&OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount]);	
	OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount].stTask.enPrivilege = enTaskPrivileged;
	OS_pstBackgroundDispatcherTask = &OS_stKERNELTaskNodePool.stqTask[OS_stKERNELTaskNodePool.u32TotalCount];	
	OS_stKERNELTaskNodePool.u32TotalCount++;	

	return true;	
}

SYSAPI_tenSVCResult OS_enAddTaskToQueue(OS_tenQueueType enQueueType, struct TASK_tstTask* pstTask)
{
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enOK;
	
	if (OS_nClientTasksMax > OS_stCLIENTTaskNodePool.u32TotalCount)
	{
		memcpy((void*)&OS_stCLIENTTaskNodePool.stqTask[OS_stCLIENTTaskNodePool.u32TotalCount].stTask,
						(void*)pstTask, sizeof(struct TASK_tstTask));
		
		OS_stCLIENTTaskNodePool.stqTask[OS_stCLIENTTaskNodePool.u32TotalCount].stTask.tsTaskQueued = CPUA_OS_u32GetSysTimeStamp();
		
		switch (enQueueType)
		{
			case OS_enCLIENTQueueStart:
			{				
				queue_task(&OS_stCLIENTTaskNodePool.stqTask[OS_stCLIENTTaskNodePool.u32TotalCount++], &OS_apstTaskQueue[OS_enCLIENTQueueStart]);		
				OS_stCLIENTTaskNodePool.u32StartCount++;
				break;
			}
			case OS_enCLIENTQueueCyclic:
			{				
				queue_task(&OS_stCLIENTTaskNodePool.stqTask[OS_stCLIENTTaskNodePool.u32TotalCount++], &OS_apstTaskQueue[OS_enCLIENTQueueCyclic]);		
				OS_stCLIENTTaskNodePool.u32CyclicCount++;
				break;
			}
			case OS_enCLIENTQueueTerminate:
			{				
				queue_task(&OS_stCLIENTTaskNodePool.stqTask[OS_stCLIENTTaskNodePool.u32TotalCount++], &OS_apstTaskQueue[OS_enCLIENTQueueTerminate]);		
				OS_stCLIENTTaskNodePool.u32TerminateCount++;
				break;
			}						
		}
	}
	else
	{
		enSVCResult = SYSAPI_enQueueFull;
	}

	return enSVCResult;
}


void OS_vSystemIdleThread(uint32* const u32Stat)
{
	uint32 u32A;
	uint32 u32B;
	
	while(1)
	{
		u32A++;
		u32B++;
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		//DSCRIO_vSetDiscrete(enSYSTELLTALE, enToggle);	
	}
}

void OS_vSystemDummyThread(uint32* const u32Stat)
{
	uint32 u32A;
	uint32 u32B;
	
	while(1)
	{
		u32A++;
		u32B++;
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");		
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");	
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");		
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");	
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");		
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");	
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");		
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");	
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");		
		__asm("nop");	
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");	
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");		
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");			
		//DSCRIO_vSetDiscrete(enSYSTELLTALE, enToggle);		
	}
}





			
			
