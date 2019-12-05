/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      TEPM                                                   */
/* DESCRIPTION:        Timed Event Programs module provides services for the  */
/*                     output of timed pulse-trains                           */
/* FILE NAME:          TEPM.c                                                 */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _TEPM_C

#include <string.h>
#include "build.h"
#include "CEM.h"
#include "client.h"
#include "CPUAbstract.h"
#include "CQUEUE.h"
#include "declarations.h"
#include "stddef.h"
#include "types.h"
//#include "USER.h"
#include "IRQ.h"
#include "TEPM.h"
#include "TEPMHA.h"
#include "TEPMAPI.h"
#include "MSG.h"
#include "SIM.h"
#include "REGSET.h"
#include "RESM.h"
#include "IOAPI.h"
#include "SYSAPI.h"

/* Private data declarations
   -------------------------*/
TEPMAPI_tstTimedUserEvent TEPM_aastTimedUserEvents[TEPMHA_nEventChannels][TEPMHA_nUserEventsMax];
TEPMAPI_tstTimedKernelEvent TEPM_aastTimedKernelEvents[TEPMHA_nEventChannels][TEPMHA_nKernelEventsMax];
const TEPM_tstTEPMChannel TEPM_rastTEPMChannel[] = TEPMHA_nChannelInfo;
const IOAPI_tenEHIOResource TEPM_rastTEPMFastChannel[] = TEPMHA_nChannelFastInfo;
const IOAPI_tenEHIOResource TEPM_rastTEPMSlowChannel[] = TEPMHA_nChannelSlowInfo;
CQUEUE_tstQueue TEPM_astProgramKernelQueue[TEPMHA_nEventChannels];
CQUEUE_tstQueue TEPM_astProgramUserQueue[TEPMHA_nEventChannels];
TEPMAPI_tpfEventCB TEPM_atpfEventKernelCB[TEPMHA_nEventChannels];
TEPMAPI_tpfEventCB TEPM_atpfEventUserCB[TEPMHA_nEventChannels];
TEPM_tstTEPMResult TEPM_astEventResult[TEPMHA_nEventChannels];
MSG_tstMBX* TEPM_apstMBX[TEPMHA_nEventChannels];
Bool TEPM_aboTEPMChannelModeInput[TEPMHA_nEventChannels];
Bool TEPM_aboTEPMChannelModeOutput[TEPMHA_nEventChannels];
Bool TEPM_aboTEPMChannelAsyncRequestEnable[TEPMHA_nEventChannels];
uint32 TEPM_au32TEPMChannelSequence[TEPMHA_nEventChannels];
uint32 TEPM_u32PortClockRequested;
Bool TEPM_aboSynchroniseEnable[TEPMHA_nEventChannels];
volatile Bool TEPM_aboQueueOverflow[TEPMHA_nEventChannels];
EXTERN uint32 CAM_u32RPMRaw;	
Bool TEPM_boDisableSequences;

#define TEPM_nTableCount sizeof(TEPM_rastTEPMChannel) / sizeof(TEPM_tstTEPMChannel)

/* Private function declarations
   ----------------------------*/
static void TEPM_vRunEventProgramUserQueue(void);
static void TEPM_vRunEventProgramKernelQueue(void*, uint32, uint32, uint32, Bool);	 
static void* TEPM_pvGetModule(IOAPI_tenEHIOResource);	 
static void* TEPM_pstGetModuleFromEnum(TEPMHA_tenModule);
static uint32 TEPM_u32GetTimerHardwareChannel(IOAPI_tenEHIOResource);

/* Public function definitions
   ---------------------------*/	 
void TEPM_vStart(puint32 const pu32Arg)
{
	uint32 u32QueueIDX;
	
	for (u32QueueIDX = 0; u32QueueIDX < TEPMHA_nEventChannels; u32QueueIDX++)
	{
		CQUEUE_xInit(TEPM_astProgramUserQueue + u32QueueIDX, 
			TEPMHA_nUserEventsMax, (uint32)TEPM_aastTimedUserEvents + u32QueueIDX * TEPMHA_nUserEventsMax * sizeof(TEPMAPI_tstTimedUserEvent));	
		
		CQUEUE_xInit(TEPM_astProgramKernelQueue + u32QueueIDX, 
			TEPMHA_nKernelEventsMax, (uint32)TEPM_aastTimedKernelEvents + u32QueueIDX * TEPMHA_nUserEventsMax * sizeof(TEPMAPI_tstTimedKernelEvent));	
		
		TEPM_atpfEventKernelCB[u32QueueIDX] = NULL;
		TEPM_atpfEventUserCB[u32QueueIDX] = NULL;		
		TEPM_apstMBX[u32QueueIDX] = NULL;
		TEPM_aboTEPMChannelModeInput[u32QueueIDX] = FALSE;
		TEPM_aboTEPMChannelModeOutput[u32QueueIDX] = FALSE;
		TEPM_aboTEPMChannelAsyncRequestEnable[u32QueueIDX] = FALSE;
		TEPM_au32TEPMChannelSequence[u32QueueIDX] = ~0;
		TEPM_boDisableSequences = FALSE;
	}
}

void TEPM_vAsyncRequest(void)
{
	CEM_u32GlobalCycleFraction = 0x0000;
	TEPM_vStartEventProgramKernelQueues(TRUE, 0);
	CEM_u32GlobalCycleFraction += (0x10000 / CEM_u32SyncPoints);
	TEPM_vStartEventProgramKernelQueues(TRUE, 1);

	CEM_u32GlobalCycleFraction = 0x10000;
	TEPM_vStartEventProgramKernelQueues(TRUE, 4);
	CEM_u32GlobalCycleFraction += (0x10000 / CEM_u32SyncPoints);
	TEPM_vStartEventProgramKernelQueues(TRUE, 5);
}

void TEPM_vEnableSequences(Bool boEnable)
{
	TEPM_boDisableSequences = ~boEnable;
}

void TEPM_vRun(puint32 const pu32Arg)
{
	
	
}

void TEPM_vTerminate(puint32 const pu32Arg)
{
	
	
}

uint32 TEPM_u32InitTEPMChannel(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMChannelCB* pstTEPMChannelCB)
{
	uint32 u32TableIDX;
	MSG_tenMBXErr enMBXErr;

	TEPMHA_vInitTEPMChannel(enEHIOResource, pstTEPMChannelCB);
	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

	if ((pstTEPMChannelCB->enAction == TEPMAPI_enCapAny) |
	    (pstTEPMChannelCB->enAction == TEPMAPI_enCapRising) |
	    (pstTEPMChannelCB->enAction == TEPMAPI_enCapFalling))
	{
	    TEPM_aboTEPMChannelModeInput[u32TableIDX] = TRUE;
	    TEPM_aboTEPMChannelModeOutput[u32TableIDX] = FALSE;
	}
	else
	{
	    TEPM_aboTEPMChannelModeOutput[u32TableIDX] = TRUE;	
	    TEPM_aboTEPMChannelModeInput[u32TableIDX] = FALSE;
    }

	TEPM_au32TEPMChannelSequence[u32TableIDX] = pstTEPMChannelCB->u32Sequence & 0xffff;

	if (((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0x0ff) == 0x0ff) ||
	    ((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xff00) == 0xff00))
	{
		TEPM_au32TEPMChannelSequence[u32TableIDX] |= 0x02000000;
	}
	else
	{
		TEPM_au32TEPMChannelSequence[u32TableIDX] |= 0x01000000;
	}


	TEPM_aboTEPMChannelAsyncRequestEnable[u32TableIDX] = pstTEPMChannelCB->boAsyncRequestEnable;

	/* Check if a mailbox is allocated for this table index */
	if (NULL == TEPM_apstMBX[u32TableIDX])
	{
		enMBXErr = MSG_enMBXNew((MSG_tstMBX**)&TEPM_apstMBX[u32TableIDX], 
					MSG_enTEPMEvent);
	}		
	
	/* TODO suppress warning */
	(void)enMBXErr;

	return TEPM_rastTEPMChannel[u32TableIDX].u32MuxSel;
}


SYSAPI_tenSVCResult TEPM_vInitTEPMResource(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMResourceCB* pstTEPMResourceCB)
{
	return TEPMHA_vInitTEPMResource(enEHIOResource, pstTEPMResourceCB);
}


void TEPM_vAppendTEPMQueue(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTimedUserEvent* pstTimedEvents, TEPMAPI_ttEventCount tEventCount)
{
	uint32 u32TableIDX;
	uint32 u32ChannelIDX;
	Bool boQueueEmpty;
	
	CPU_xEnterCritical();
	
	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	boQueueEmpty = CQUEUE_xIsEmpty(TEPM_astProgramUserQueue + u32TableIDX);
	
	while ((0 < tEventCount--)
			&& (FALSE == CQUEUE_xIsFull(TEPM_astProgramUserQueue + u32TableIDX)))
	{
		memcpy((void*)&TEPM_aastTimedUserEvents[u32TableIDX][TEPM_astProgramUserQueue[u32TableIDX].u32Tail],
						 (void*)pstTimedEvents++, sizeof(TEPMAPI_tstTimedUserEvent));
		CQUEUE_xAddItem(TEPM_astProgramUserQueue + u32TableIDX);		
	}	
	
	CPU_xExitCritical();
	
	if (TRUE == boQueueEmpty)
	{
		u32ChannelIDX = TEPM_rastTEPMChannel[u32TableIDX].u32Channel;
		TEPM_vRunEventProgramUserQueue();	//matthew problem
	}
	
	UNUSED(u32ChannelIDX);
}

void TEPM_vConfigureUserTEPMInput(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTimedUserEvent* pstTimedEvent)
{
	uint32 u32TableIDX;
	
	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	
	switch (pstTimedEvent->enMethod)
	{
		case TEPMAPI_enLinkPrimaryProgram:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = &CEM_vPrimaryEventCB;	
			break;			
		}
		case TEPMAPI_enLinkPhaseProgram:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = &CEM_vPhaseEventCB;	
			break;			
		}	
		case TEPMAPI_enLinkFreqInput:
		{
			TEPM_atpfEventKernelCB[u32TableIDX] = & CEM_vFreqEventCB;	
			break;	
		}
		default:
		{
			break;
		}
	}		
		
	TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvent->pfEventCB;	
	
	if (FALSE == CQUEUE_xIsFull(TEPM_astProgramUserQueue + u32TableIDX))
	{
		memcpy((void*)&TEPM_aastTimedUserEvents[u32TableIDX][TEPM_astProgramUserQueue[u32TableIDX].u32Tail],
						 (void*)pstTimedEvent, sizeof(TEPMAPI_tstTimedUserEvent));
		CQUEUE_xAddItem(TEPM_astProgramUserQueue + u32TableIDX);		
	}				
}

void TEPM_vConfigureKernelTEPMOutput(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTimedKernelEvent* pstTimedEvents, TEPMAPI_ttEventCount tEventCount)
{
	uint32 u32TableIDX;
	
	u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
		
	TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvents->pfEventCB;	
	
	/* Reset the queue count it is not possible to append a Kernel output queue */
	CQUEUE_xClearCount(TEPM_astProgramKernelQueue + u32TableIDX);
	
	while ((0 < tEventCount--)
			&& (FALSE == CQUEUE_xIsFull(TEPM_astProgramKernelQueue + u32TableIDX)))
	{
		memcpy((void*)&TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Tail],
						 (void*)pstTimedEvents++, sizeof(TEPMAPI_tstTimedKernelEvent));
		CQUEUE_xAddItem(TEPM_astProgramKernelQueue + u32TableIDX);		
	}			
}

void TEPM_vInitiateUserCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{		
	MSG_tstMBX* pstMBX;
	tpfClientCB pfClientCB;	
	CLIENT_tenErr enErr;	
	uint32 u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);		
	
	/* Store the programmed event resource for the user callback */
	TEPM_astEventResult[u32TableIDX].enEHIOResource = enEHIOResource;	
	
	/* Store the timer result for the user callback */	
	TEPM_astEventResult[u32TableIDX].tEventTime = tEventTime;	
	
	pfClientCB = (tpfClientCB)TEPM_atpfEventUserCB[u32TableIDX];							
	pstMBX = TEPM_apstMBX[u32TableIDX];									
	MSG_vMBXPost(pstMBX, (void*)&TEPM_astEventResult[u32TableIDX]);
							
	if ((NULL != pfClientCB) && (MSG_boMBXValid(pstMBX)))
	{
		enErr = CLIENT_enEnqueueCB(pstMBX, pfClientCB);
	}	
	
	/* TODO suppress warning */
	(void)enErr;
}


void TEPM_u32GetFreeVal(IOAPI_tenEHIOResource enEHIOResource, puint32 pu32Data)
{
	void* pvModule = TEPM_pvGetModule(enEHIOResource);

    TEPMHA_vGetFreeVal(pvModule, pu32Data);
}

void TEPM_vInterruptHandler(IOAPI_tenEHIOResource enEHVIOResource, void* pvData)
{
	uint32 u32ChannelIDX;
	uint32 u32TableIDX;
	uint32 u32Flags;
	TEPMAPI_tpfEventCB pfTEPMEventCB;
    static uint32 u32Sequence[2];
	uint32 u32StartChannelIDX;
	uint32 u32EndChannelIDX;
    IOAPI_tenEHIOResource enEHIOResource;
	uint32 u32Prio = *(puint32)pvData;
	static uint32 u32FalseAlarmCount;

	CPU_xEnterCritical();

	u32Sequence[u32Prio]++;

	/* Note here the resource passed in is the timer module resource not the timer channel */
	void* pvModule = TEPMHA_pvGetTimerModuleFromVIO(enEHVIOResource);

	u32StartChannelIDX = TEPMHA_u32GetTimerStartChannelInterruptGroup(enEHVIOResource);
	u32EndChannelIDX = u32StartChannelIDX + TEPMHA_u32GetTimerChannelsPerInterruptGroup();
	
	/* Loop through the channels contained within this interrupt group */
	for (u32ChannelIDX = u32StartChannelIDX; u32ChannelIDX < u32EndChannelIDX; u32ChannelIDX++)
	{
	    if (true == TEPMHA_boFlagIsSet(pvModule, u32ChannelIDX, &u32Flags, u32Sequence[u32Prio], u32Prio))
		{
		    if (true == TEMPHA_boInterruptEnabled(pvModule, u32ChannelIDX))
			{										
				enEHIOResource = TEPMHA_enGetTimerResourceFromVIOAndIndex(enEHVIOResource, u32ChannelIDX);
				u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);													
				
				if (TRUE == TEPM_aboTEPMChannelModeOutput[u32TableIDX])
				{				
					if (TRUE == TEPM_aboQueueOverflow[u32TableIDX])
					{
						TEPM_aboQueueOverflow[u32TableIDX] = FALSE;
						CQUEUE_xSetHead(TEPM_astProgramKernelQueue + u32TableIDX, 0);
						TEPM_vStartEventProgramKernelQueues(FALSE, TEPM_au32TEPMChannelSequence[u32TableIDX]);	
					}

					else if (FALSE == TEPMHA_boCheckFalseAlarm(pvModule, u32ChannelIDX, TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX)))	
					{
						TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX, 
							TEPM_au32TEPMChannelSequence[u32TableIDX], false);
						TEPM_vInitiateUserCallBack(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));
					}				
					else
					{
						u32FalseAlarmCount++;
					}
				}			
				else if (TRUE == TEPM_aboTEPMChannelModeInput[u32TableIDX])
				{
					pfTEPMEventCB = TEPM_atpfEventKernelCB[u32TableIDX];
						
					if (NULL != pfTEPMEventCB)
					{
						pfTEPMEventCB(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));
					}
				}
            }
			
			TEMPHA_vResetTimerFlag(pvModule, u32ChannelIDX);
			//break;
		}
	}

	CPU_xExitCritical();
}

IOAPI_tenTriState TEPM_enGetTimerDigitalState(IOAPI_tenEHIOResource enEHIOResource)
{
    return TEPMHA_enGetTimerDigitalState(enEHIOResource);
}

/* Private function definitions
   ---------------------------*/	
static void TEPM_vRunEventProgramUserQueue(void)
{
#if 0
	TEPMAPI_tstTimedUserEvent* pstTimedEvent;
	TEPMAPI_ttEventTime tEventTimeScheduled;
	TEPMAPI_ttEventTime tEventTimeRemains;	
	
	if (FALSE == (CQUEUE_xIsEmpty(TEPM_astProgramUserQueue + u32TableIDX)))
	{
		pstTimedEvent = &TEPM_aastTimedUserEvents[u32TableIDX][TEPM_astProgramUserQueue[u32TableIDX].u32Head];		
		
		switch (pstTimedEvent->enMethod)
		{
			case TEPMAPI_enHardAlarm:
			{
				if (0x7ff0 > pstTimedEvent->tEventTime)
				{
					tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + pstTimedEvent->tEventTime;
					tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					if ((0x10 > tEventTimeRemains) || (0x8000 < tEventTimeRemains))
					{
						tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + 0x80;
					}
				}
				else
				{
					tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + pstTimedEvent->tEventTime;			
				}
				break;
			}				
			case TEPMAPI_enHardLinkedTimeStep:
			{		
				if (0x7ff0 > pstTimedEvent->tEventTime)
				{
					tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + pstTimedEvent->tEventTime;
					tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					if ((0x10 > tEventTimeRemains) || (0x8000 < tEventTimeRemains))
					{
						tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + 0x80;
					}
				}
				else
				{
					tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + pstTimedEvent->tEventTime;			
				}
				break;
			}
			default:
			{
				break;
			}
		}		

		//TEPMHA_vCapComAction(pstTimedEvent->enAction, pvModule, u32ChannelIDX, 0, tEventTimeScheduled);				
		TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvent->pfEventCB;
		CQUEUE_xRemoveItem(TEPM_astProgramUserQueue + u32TableIDX);
	}
	else
	{
#ifdef TEPM_REMOVE_RESIDUAL_INTERRUPTS
		/* The queue is empty therefore this was a next window interrupt 
		   so go ahead and disable interrupts */
		TEPMHA_vDisconnectEnable(pvModule, u32ChannelIDX);
#endif		
	}
#endif
}


void TEPM_vStartEventProgramKernelQueues(Bool boAsyncRequest, uint32 u32SequenceIDX)
{
	IOAPI_tenEHIOResource enEHIOResource;		
	void* pvModule;
	uint32 u32ChannelIDX;
	uint32 u32SubChannelIDX;
	const IOAPI_tenEHIOResource raenResourceList[] = TEPMHA_nChannelResourceList;
	uint32 u32TimerChannelIDX;
	uint32 u32TableIDX;
	static uint32 u32SyncRPMLimit = 2000;

	if (TRUE == boAsyncRequest)
	{
		CEM_u32GlobalCycleTime = 0x8000;
	}
	
	for (u32TimerChannelIDX = 0; u32TimerChannelIDX < TEPMHA_nEventChannels; u32TimerChannelIDX++)
	{
		enEHIOResource = raenResourceList[u32TimerChannelIDX];
		u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
		
		if ((TRUE == TEPM_aboTEPMChannelModeOutput[u32TableIDX]) &&
			((u32SequenceIDX == (TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xff)) ||
			 (u32SequenceIDX == ((TEPM_au32TEPMChannelSequence[u32TableIDX] & 0xff00) >> 8))))
		{
			TEPM_aboSynchroniseEnable[u32TimerChannelIDX] = u32SyncRPMLimit < CAM_u32RPMRaw ? FALSE : TRUE;

			if (((TRUE == TEPM_aboTEPMChannelAsyncRequestEnable[u32TableIDX]) && (TRUE == boAsyncRequest)) ||
				(FALSE == boAsyncRequest))
			{
				//if ((TRUE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX)) &&
				//		(TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX)))
				/* EMPTY TEST MAYBE NOT REQUIRED */
				if ((TRUE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX)) &&
                    (TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX)) &&
                    (FALSE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX)))
				{
					pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
					u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	
					u32SubChannelIDX = TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX);
			
					/* Queue didn't end normally last cycle */			
					TEPMHA_vForceQueueTerminate(pvModule, u32ChannelIDX, u32SubChannelIDX);				
					CQUEUE_xResetStaticHead(TEPM_astProgramKernelQueue + u32TableIDX);	
					//TEPM_aboQueueOverflow[u32TableIDX] = TRUE;
				}										

				if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX) &&
						TRUE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX))
				{
					/* This queue is populated and head is at zero (static head) */
					pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
					u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	

					TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX, 
						u32SequenceIDX, false);

					TEPM_aboQueueOverflow[u32TimerChannelIDX] = FALSE;
				}
			}
		}
	}
}

void TEPM_vGetTimerVal(IOAPI_tenEHIOResource enEHIOResource, puint32 pu32Val)
{
	*pu32Val = TEPMHA_u32GetTimerVal(enEHIOResource);
}

void TEPM_vSynchroniseEventProgramKernelQueues(void)
{
	IOAPI_tenEHIOResource enEHIOResource;		
	void* pvModule;
	const IOAPI_tenEHIOResource raenResourceList[] = TEPMHA_nChannelResourceList;
	uint32 u32ChannelIDX;
	uint32 u32TimerChannelIDX;
	uint32 u32TableIDX;

	
	for (u32TimerChannelIDX = 0; u32TimerChannelIDX < TEPMHA_nEventChannels; u32TimerChannelIDX++)
	{
		enEHIOResource = raenResourceList[u32TimerChannelIDX];
		u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

		if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX) &&
			(FALSE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX)) &&
			(TRUE == TEPM_aboTEPMChannelModeOutput[u32TableIDX]))
		{
			/* If not at static head which means queue is done and reset */
			pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
			u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	

			TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX, 
				TEPM_au32TEPMChannelSequence[u32TableIDX], true);
		}
	}
}


static void TEPM_vRunEventProgramKernelQueue(void* pvModule, uint32 u32ChannelIDX, uint32 u32TableIDX, uint32 u32SequenceIDX, Bool boSynchroniseUpdate)
{
	TEPMAPI_tstTimedKernelEvent* pstTimedEvent = NULL;
	TEPMAPI_ttEventTime tEventTimeScheduled = 0;
	TEPMAPI_ttEventTime tEventTimeRemains;	
	uint32 u32Temp;
	uint32 u32ModulePhaseCorrect = 0;
	uint32 u32SubChannelIDX;
	volatile Bool boSynchroniseAbort = FALSE;
	uint32 u32GlobalSequenceFraction;

	/* Mask out the sequence index  alternate origin index and windows span */
	u32SequenceIDX &= 0xff;

	
	if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX))
	{
		if (TRUE == boSynchroniseUpdate)
		{			
			if (1 == TEPM_astProgramKernelQueue[u32TableIDX].u32Head)
			{
				/* If waiting for first event, let's look at it */					
				pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Head - 1];					
				
				switch (pstTimedEvent->enMethod)
				{
					case TEPMAPI_enGlobalLinkedFraction:
					{
						if (FALSE == TEPM_aboSynchroniseEnable[u32TableIDX])
						{				
							/* Not eligible for a synchronise so abort */
							pstTimedEvent = NULL;
						}							
						break;
					}
					default:
					{
						/* Not eligible for a synchronise so abort */
						pstTimedEvent = NULL;
					}
				}
			}
		}
		else
		{		
			pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Head];		
		}
		
		if (NULL != pstTimedEvent)
		{
			u32SubChannelIDX = TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX);

			/* Timed event pointer is NULL for a failed synchronise */
			switch (pstTimedEvent->enMethod)
			{
				case TEPMAPI_enGlobalLinkedFraction:
				{				
				    u32ModulePhaseCorrect = (uint32)CEM_ttGetModulePhase(3 * TEPMHA_enTimerEnumFromModule(pvModule) + u32ChannelIDX / 2);					

					if (1 == (TEPM_au32TEPMChannelSequence[u32TableIDX] >> 24))
					{
						u32GlobalSequenceFraction = 0xffff & (CEM_u32GlobalCycleFraction - (u32SequenceIDX * 0x10000 / CEM_u32SyncPoints));

						/* Divide global cycle time by 8 for scale again by phase repeats this might be half global window */
						u32Temp = CEM_u32GlobalCycleTime / (8 * CEM_u8PhaseRepeats);
					}
					else
					{
						u32GlobalSequenceFraction = 0xffff & ((CEM_u32GlobalCycleFraction >> 1) - (u32SequenceIDX * 0x8000 / CEM_u32SyncPoints));

						/* Divide global cycle time by 8 for scale */
						u32Temp = CEM_u32GlobalCycleTime / 8;//matthew changed for SAM3X8E CAVEAT GRANULARITY MK60!!!!
					}


					if (u32GlobalSequenceFraction < *(pstTimedEvent->ptEventTime))
					{
						if (((0 == u32GlobalSequenceFraction) && (FALSE == boSynchroniseUpdate)) || (TRUE == boSynchroniseUpdate))
						{						
							u32Temp *= (*(pstTimedEvent->ptEventTime) - u32GlobalSequenceFraction);
							u32Temp = MIN(0x7fc00000, u32Temp);	
							u32Temp /= 0x2000;							
										
							tEventTimeScheduled = CEM_tSyncTimeLast + u32ModulePhaseCorrect + u32Temp;
							tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					
							if (FALSE == boSynchroniseUpdate)
							{
								if ((TEPM_nSoonCounts > tEventTimeRemains) || ((UINT32_MAX / 2) < tEventTimeRemains))
								{
									tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + TEPM_nSoonCounts;
								}
							}
							else
							{
								if ((TEPM_nSoonCounts > tEventTimeRemains) || ((UINT32_MAX / 2) < tEventTimeRemains))
								{
									boSynchroniseAbort = TRUE;
								}												
							}
						}
						else
						{
							boSynchroniseAbort = TRUE;
						}
					}
					else
					{
                        boSynchroniseAbort = TRUE;
					}

					break;
				}	
				case TEPMAPI_enGlobalLinkedFractionStep:
				{					
					u32Temp = CEM_u32GlobalCycleTime / 2;
					u32Temp *= *(pstTimedEvent->ptEventTime);
					u32Temp = MIN(0x7fc00000, u32Temp);	
					u32Temp /= 0x8000;					
					tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + u32Temp;
					tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					if ((TEPM_nSoonCounts > tEventTimeRemains) || ((uint32)-TEPM_nSoonCounts < tEventTimeRemains))
					{
						tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + 0x80;
					}

					break;
				}	
				case TEPMAPI_enHardLinkedTimeStep:
				{		
					if ((uint32)-TEPM_nSoonCounts > *(pstTimedEvent->ptEventTime))
					{
						tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + *(pstTimedEvent->ptEventTime);
						tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					    if ((TEPM_nSoonCounts > tEventTimeRemains) || ((uint32)-TEPM_nSoonCounts < tEventTimeRemains))
						{
							tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + 0x80;
						}
					}
					else
					{
						tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + *(pstTimedEvent->ptEventTime);			
					}
					break;
				}
				default:
				{
					break;
				}
			}		

			if (FALSE == boSynchroniseAbort)
			{
				if (FALSE == TEPM_boDisableSequences)
				{
					TEPMHA_vCapComAction(pstTimedEvent->enAction, pvModule, u32ChannelIDX, u32SubChannelIDX, tEventTimeScheduled);	

					if (EH_IO_Invalid != pstTimedEvent->enEHIOBitMirrorResource)
					{
						if (TEPMAPI_enSetLow == pstTimedEvent->enAction)
						{
							IO_vAssertDIOResource(pstTimedEvent->enEHIOBitMirrorResource, IOAPI_enHigh);
						}
					}
				}
				else
				{
					TEPMHA_vCapComAction(TEPMAPI_enSetLow, pvModule, u32ChannelIDX, u32SubChannelIDX, tEventTimeScheduled);	
				}
			}
			
			if (FALSE == boSynchroniseUpdate)
			{
				TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvent->pfEventCB;
				CQUEUE_xRemoveItem(TEPM_astProgramKernelQueue + u32TableIDX);	
			}
		}
	}
	else if (FALSE == boSynchroniseUpdate)
	{
		if (TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX))
		{		
			CQUEUE_xResetStaticHead(TEPM_astProgramKernelQueue + u32TableIDX);	
			pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][0];

			if (EH_IO_Invalid != pstTimedEvent->enEHIOBitMirrorResource)
			{
				IO_vAssertDIOResource(pstTimedEvent->enEHIOBitMirrorResource, IOAPI_enLow);
			}
		}

		/* The queue is empty so go ahead and disable interrupts and connection */
		TEPMHA_vDisconnectEnable(pvModule, u32ChannelIDX);
	}
}

static void* TEPM_pvGetModule(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	void* pvModule;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pvModule = TEPM_pstGetModuleFromEnum(TEPM_rastTEPMChannel[u32ChannelIDX].enModule);
	
	return pvModule;
}

static void* TEPM_pstGetModuleFromEnum(TEPMHA_tenModule enTimerModule)
{
     return TEPMHA_pvGetModuleFromEnum(enTimerModule);
}

static uint32 TEPM_u32GetTimerHardwareChannel(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

	return TEPM_rastTEPMChannel[u32ChannelIDX].u32Channel;
}

uint32 TEPM_u32GetFTMTableIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	return TEPMHA_u32GetFTMTableIndex(enEHIOResource);
}





