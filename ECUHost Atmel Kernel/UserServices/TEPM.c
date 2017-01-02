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
const TEPM_tstMasters TEPM_rastMasters[] = TEPMHA_nMasterInfo;
CQUEUE_tstQueue TEPM_astProgramKernelQueue[TEPMHA_nEventChannels];
CQUEUE_tstQueue TEPM_astProgramUserQueue[TEPMHA_nEventChannels];
TEPMAPI_tpfEventCB TEPM_atpfEventKernelCB[TEPMHA_nEventChannels];
TEPMAPI_tpfEventCB TEPM_atpfEventUserCB[TEPMHA_nEventChannels];
TEPM_tstTEPMResult TEPM_astEventResult[TEPMHA_nEventChannels];
MSG_tstMBX* TEPM_apstMBX[TEPMHA_nEventChannels];
Bool TEPM_aboTEPMChannelModeInput[TEPMHA_nEventChannels];
Bool TEPM_aboTEPMChannelModeOutput[TEPMHA_nEventChannels];
uint32 TEPM_u32PortClockRequested;
Bool boSynchroniseEnable;

#define TEPM_nTableCount sizeof(TEPM_rastTEPMChannel) / sizeof(TEPM_tstTEPMChannel)

/* Private function declarations
   ----------------------------*/
static void TEPM_vRunEventProgramUserQueue(void*, uint32, uint32);
static void TEPM_vRunEventProgramKernelQueue(void*, uint32, uint32, Bool);	 
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
	}
	
	boSynchroniseEnable = TRUE;
}

void TEPM_vRun(puint32 const pu32Arg)
{
	
	
}

void TEPM_vTerminate(puint32 const pu32Arg)
{
	
	
}

uint32 TEPM_u32InitTEPMChannel(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMChannelCB* pstTEPMChannelCB)
{
	uint32 u32ChannelIDX;
	uint32 u32TableIDX;
	uint32 u32ControlWord = 0;
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

void TEPM_boGetMasterEHIOResourceList(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOResource* penEHIOResource, puint32 pu32MasterCount)
{
	uint32 u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	uint32 u32MasterIDX = 0;
	
	while ((RESM_nMastersMax > *pu32MasterCount) && (TEPMHA_nMastersMax > u32MasterIDX))
	{
		if (NULL != TEPM_rastMasters[u32ChannelIDX].enEHIOResource[u32MasterIDX])
		{
			*penEHIOResource = TEPM_rastMasters[u32ChannelIDX].enEHIOResource[u32MasterIDX];
			penEHIOResource++; 
			(*pu32MasterCount)++; 
			u32MasterIDX++;
		}
	}
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
		//TEPM_vRunEventProgramUserQueue(FTM0, u32ChannelIDX, u32TableIDX);	//matthew problem
	}
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

void TEPM_vInterruptHandler(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	uint32 u32TableIDX;
	uint32 u32Flags;
	TEPMAPI_tpfEventCB pfTEPMEventCB;
    static uint32 u32Sequence;
	uint32 u32StartChannelIDX;
	uint32 u32EndChannelIDX;

	u32Sequence++;

	/* Note here the resource passed in is the timer module resource not the timer channel */
	void* pvModule = TEPMHA_pvGetTimerModuleFromVIO(enEHIOResource);

	u32StartChannelIDX = TEPMHA_u32GetTimerStartChannelInterruptGroup(enEHIOResource);
	u32EndChannelIDX = u32StartChannelIDX + TEPMHA_u32GetTimerChannelsPerInterruptGroup();
	
	/* Loop through the channels contained within this interrupt group */
	for (u32ChannelIDX = u32StartChannelIDX; u32ChannelIDX < u32EndChannelIDX; u32ChannelIDX++)
	{
	    if (true == TEPMHA_boFlagIsSet(pvModule, u32ChannelIDX, &u32Flags, u32Sequence))
		{
		    if (true == TEMPHA_boInterruptEnabled(pvModule, u32ChannelIDX))
			{							
				enEHIOResource = TEPMHA_enGetTimerResourceFromVIOAndIndex(enEHIOResource, u32ChannelIDX);
				u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);													
				
				if (TRUE == TEPM_aboTEPMChannelModeOutput[u32TableIDX])
				{			
					TEPM_vInitiateUserCallBack(enEHIOResource, TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, TEPM_aboTEPMChannelModeInput[u32TableIDX], u32Flags));						
					//TEPM_vRunEventProgramUserQueue(pvModule, u32ChannelIDX, u32TableIDX);	
					TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX, false);
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
			break;
		}
	}
}

IOAPI_tenTriState TEPM_enGetTimerDigitalState(IOAPI_tenEHIOResource enEHIOResource)
{
    return TEPMHA_enGetTimerDigitalState(enEHIOResource);
}

/* Private function definitions
   ---------------------------*/	
static void TEPM_vRunEventProgramUserQueue(void* pvModule, uint32 u32ChannelIDX, uint32 u32TableIDX)
{
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
}


void TEPM_vStartEventProgramKernelQueues(void)
{
	IOAPI_tenEHIOResource enEHIOResource;		
	void* pvModule;
	uint32 u32ChannelIDX;
	uint32 u32SubChannelIDX;
	
	for (enEHIOResource = EH_IO_TMR1; enEHIOResource < EH_LAST_TMR; enEHIOResource++)
	{
		uint32 u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
		
		if (FALSE == TEPM_aboTEPMChannelModeInput[u32TableIDX])
		{
			if ((TRUE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX)) &&
					(TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX)))
			{
				pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
				u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	
				u32SubChannelIDX = TEPMHA_u32GetTimerHardwareSubChannel(u32TableIDX);
			
				/* Queue didn't end normally last cycle */			
				TEPMHA_vForceQueueTerminate(pvModule, u32ChannelIDX, u32SubChannelIDX);				
				CQUEUE_xResetStaticHead(TEPM_astProgramKernelQueue + u32TableIDX);	
			}	
			
			//if ((FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX)) &&
					//(TRUE == CQUEUE_xIsStaticActive(TEPM_astProgramKernelQueue + u32TableIDX)))
			//{		
				///* Queue didn't end normally last cycle */			
				//TEPMHA_vForceQueueTerminate(pvModule, u32ChannelIDX);				
				//CQUEUE_xResetStaticHead(TEPM_astProgramKernelQueue + u32TableIDX);							
			//}		

			if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX) &&
					TRUE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX))
			{
			    /* This queue is populated and head is at zero (static head) */
				pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
				u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	

				TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX, false);
			}
		}
	}
}

void TEPM_u32GetTimerVal(IOAPI_tenEHIOResource enEHIOResource, puint32 pu32Val)
{
	TEPMHA_u32GetTimerVal(enEHIOResource, pu32Val);
}

void TEPM_vSynchroniseEventProgramKernelQueues(void)
{
	IOAPI_tenEHIOResource enEHIOResource;		
	void* pvModule;
	uint32 u32ChannelIDX;
	
	for (enEHIOResource = EH_IO_TMR1; enEHIOResource < EH_LAST_TMR; enEHIOResource++)
	{
		uint32 u32TableIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

		if (FALSE == CQUEUE_xIsEmpty(TEPM_astProgramKernelQueue + u32TableIDX) &&
				FALSE == CQUEUE_xIsAtStaticHead(TEPM_astProgramKernelQueue + u32TableIDX))
		{
			/* If not at static head which means queue is done and reset */
			pvModule = TEPMHA_pvGetModuleFromEnum(TEPM_rastTEPMChannel[u32TableIDX].enModule);		
			u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);	

			TEPM_vRunEventProgramKernelQueue(pvModule, u32ChannelIDX, u32TableIDX, true);
		}
	}
}


static void TEPM_vRunEventProgramKernelQueue(void* pvModule, uint32 u32ChannelIDX, uint32 u32TableIDX, Bool boSynchroniseUpdate)
{
	TEPMAPI_tstTimedKernelEvent* pstTimedEvent = NULL;
	TEPMAPI_ttEventTime tEventTimeScheduled;
	TEPMAPI_ttEventTime tEventTimeRemains;	
	uint32 u32Temp;
	uint32 u32SubChannelIDX;
	volatile Bool boSynchroniseAbort = FALSE;
	
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
						if (FALSE == boSynchroniseEnable)
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
					u32Temp = CEM_u32GlobalCycleTime / 2;
					u32Temp *= *(pstTimedEvent->ptEventTime);
					u32Temp = MIN(0x7fc00000, u32Temp);	
					u32Temp /= 0x8000;					
					tEventTimeScheduled = CEM_tGlobalCycleOrigin + u32Temp;
					tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					
					if (FALSE == boSynchroniseUpdate)
					{
						if ((TEPM_nSoonCounts > tEventTimeRemains) || (-TEPM_nSoonCounts < tEventTimeRemains))
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
					if ((TEPM_nSoonCounts > tEventTimeRemains) || (-TEPM_nSoonCounts < tEventTimeRemains))
					{
						tEventTimeScheduled = TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX) + 0x80;
					}

					break;
				}	
				case TEPMAPI_enHardLinkedTimeStep:
				{		
					if (-TEPM_nSoonCounts > *(pstTimedEvent->ptEventTime))
					{
						tEventTimeScheduled = TEPMHA_tGetScheduledVal(pvModule, u32ChannelIDX, FALSE, 0) + *(pstTimedEvent->ptEventTime);
						tEventTimeRemains = tEventTimeScheduled - TEPMHA_u32GetFreeVal(pvModule, u32ChannelIDX);
					    if ((TEPM_nSoonCounts > tEventTimeRemains) || (-TEPM_nSoonCounts < tEventTimeRemains))
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
				TEPMHA_vCapComAction(pstTimedEvent->enAction, pvModule, u32ChannelIDX, u32SubChannelIDX, tEventTimeScheduled);	
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

static uint32 TEPM_u32GetTimerHardwareSubChannel(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

	return TEPM_rastTEPMChannel[u32ChannelIDX].u32SubChannel;
}





