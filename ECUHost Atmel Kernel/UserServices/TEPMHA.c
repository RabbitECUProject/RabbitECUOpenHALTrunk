/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      TEPM Hardware Abstraction                              */
/* DESCRIPTION:        Timed Event Programs module provides services for the  */
/*                     output of timed pulse-trains                           */
/* FILE NAME:          TEPMHA.c                                               */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "CPUAbstract.h"
#include <string.h>
#include "build.h"
#include "CEM.h"
#include "client.h"
#include "CPUAbstract.h"
#include "CQUEUE.h"
#include "declarations.h"
#include "stddef.h"
#include "types.h"
#include "IRQ.h"
#include "TEPM.h"
#include "TEPMAPI.h"
#include "MSG.h"
#include "SIM.h"
#include "REGSET.h"
#include "RESM.h"
#include "IOAPI.h"
#include "SYSAPI.h"

/* Private data declarations
   -------------------------*/
const TEPM_tstTEPMChannel TEPMHA_rastTEPMChannel[] = TEPMHA_nChannelInfo;

#define TEPM_nTableCount sizeof(TEPMHA_rastTEPMChannel) / sizeof(TEPM_tstTEPMChannel)

/* Private function declarations
   ----------------------------*/
static void TEPMHA_vRunEventProgramUserQueue(tstTimerModule*, uint32, uint32);
static void TEPMHA_vRunEventProgramKernelQueue(tstTimerModule*, uint32, uint32, bool);
static void TEPMHA_vInitInterrupts(IRQn_Type);	 
static tstTimerModule* TEPMHA_pstGetTimerModule(IOAPI_tenEHIOResource);	 
static uint32 TEPMHA_pstGetFTMChannel(IOAPI_tenEHIOResource);


/* Public function definitions
   ---------------------------*/	 

void TEPMHA_vInitTEPMChannel(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMChannelCB* pstTEPMChannelCB)
{
	tstTimerModule* pstTimerModule;
	vpuint32 vpuFTMReg;
	uint32 u32ChannelIDX;
	uint32 u32ControlWord = 0;
	
#ifdef BUILD_MK60
	pstTimerModule = TEPMHA_pstGetTimerModule(enEHIOResource);
	u32ChannelIDX = TEPM_pstGetFTMChannel(enEHIOResource);
	vpuFTMReg = (vpuint32)((uint32)pstTimerModule + (uint32)offsetof(tstTimerModule, CONTROLS[u32ChannelIDX]));
		
	switch (pstTEPMChannelCB->enAction)
	{
		case TEPMAPI_enSetHigh:
		u32ControlWord |=
		(FTM_CnSC_MSA_MASK | FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK); break;
		case TEPMAPI_enSetLow:
		u32ControlWord |=
		(FTM_CnSC_MSA_MASK | FTM_CnSC_ELSB_MASK); break;
		case TEPMAPI_enToggle: u32ControlWord |=
		(FTM_CnSC_MSA_MASK | FTM_CnSC_ELSA_MASK); break;
		case TEPMAPI_enCapRising: u32ControlWord |=
		(FTM_CnSC_ELSA_MASK); boIsInput = TRUE; break;
		case TEPMAPI_enCapFalling: u32ControlWord |=
		(FTM_CnSC_ELSB_MASK); boIsInput = TRUE; break;
		case TEPMAPI_enCapAny:	u32ControlWord |=
		(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK); boIsInput = TRUE; break;
		default: break;
	}
		
	u32ControlWord = (TRUE == pstTEPMChannelCB->boInterruptEnable) ?
	u32ControlWord | FTM_CnSC_CHIE_MASK : u32ControlWord;

	*vpuFTMReg = u32ControlWord;
#endif //BUILD_MK60
}


SYSAPI_tenSVCResult TEPMHA_vInitTEPMResource(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMResourceCB* pstTEPMResourceCB)
{
	tstTimerModule* pstTimerModule;
	REGSET_tstReg32Val astTEPMReg32Val[2];
	
	switch (enEHIOResource)
	{
#ifdef BUILD_MK60
		case EH_VIO_FTM0:
		{
			pstTimerModule = FTM0;
			break;
		}
		case EH_VIO_FTM1:
		{
			pstTimerModule = FTM1;
			break;
		}
		case EH_VIO_FTM2:
		{
			pstTimerModule = FTM2;
			break;
		}
		case EH_VIO_FTM3:
		{
			pstTimerModule = FTM3;
			break;
		}
#endif //BUILD_MK60
		default:
		{
			pstTimerModule = NULL;
			break;
		}			
	}
	
	TEPMHA_xRequestPortClock(pstTimerModule);
	
#ifdef BUILD_MK60
	astTEPMReg32Val[0].reg = (vpuint32)(pstTimerModule + offsetof(tstTimerModule, SC));
	astTEPMReg32Val[0].val = (uint32)(FTM_SC_PS(pstTEPMResourceCB->enPreScalar) | FTM_SC_CLKS(1) |
			 (pstTEPMResourceCB->enCountType << FTM_SC_CPWMS_SHIFT));														
	astTEPMReg32Val[0].writeMode = REGSET_enOverwrite;	
	astTEPMReg32Val[1].reg = NULL;			
	REGSET_vInitReg32(&astTEPMReg32Val[0]);		
#endif //BUILD_MK60

	TEPMHA_xInitInterrupts(pstTimerModule);
	
	return SYSAPI_enOK;
}



void TEPMHA_vForceQueueTerminate(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX)
{
	TEPMAPI_ttEventTime tEventTimeScheduled;
	TEPMAPI_ttEventTime tEventTimeRemains;	
	uint32 u32Temp;
		
	CPU_xEnterCritical();

#ifdef BUILD_MK60
	u32Temp = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC & ~FTM_CnSC_CHIE_MASK;	
	u32Temp &= ~FTM_CnSC_CHF_MASK;	
	
	tEventTimeRemains = (0xffff & pstTimerModule->CONTROLS[u32ChannelIDX].CnV) - 
											(0xffff & pstTimerModule->CNT);
	
	if (0 < tEventTimeRemains)
	{	
		tEventTimeScheduled = pstTimerModule->CNT + 2;
		pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;			
		
		/* Force it now! */
		while (pstTimerModule->CONTROLS[u32ChannelIDX].CnV != tEventTimeScheduled)
		{
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;					
		}	

		/* Wait for it! Dangerous!!!*/
		while (pstTimerModule->CONTROLS[u32ChannelIDX].CnV != pstTimerModule->CNT){}
	}
	else
	{
		pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;					
	}
#endif //BUILD_MK60
	
	CPU_xExitCritical();	
}

//static void TEPMHA_vRunEventProgramKernelQueue(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX, uint32 u32TableIDX, bool boSynchroniseUpdate)
//{
	//TEPMAPI_tstTimedKernelEvent* pstTimedEvent = NULL;
	//TEPMAPI_ttEventTime tEventTimeScheduled;
	//TEPMAPI_ttEventTime tEventTimeRemains;	
	//uint32 u32Temp;
	//bool boSynchroniseAbort = FALSE;
	//
	//if (FALSE == CQUEUE_boIsEmpty(TEPM_astProgramKernelQueue[u32TableIDX]))
	//{
		//if (TRUE == boSynchroniseUpdate)
		//{			
			//if (1 == TEPM_astProgramKernelQueue[u32TableIDX].u32Head)
			//{
				///* If waiting for first event, let's look at it */					
				//pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Head - 1];					
				//
				//switch (pstTimedEvent->enMethod)
				//{
					//case TEPMAPI_enGlobalLinkedFraction:
					//{
						//if (FALSE == boSynchroniseEnable)
						//{				
							///* Not eligible for a synchronise so abort */
							//pstTimedEvent = NULL;
						//}							
						//break;
					//}
					//default:
					//{
						///* Not eligible for a synchronise so abort */
						//pstTimedEvent = NULL;
					//}
				//}
			//}
		//}
		//else
		//{		
			//pstTimedEvent = &TEPM_aastTimedKernelEvents[u32TableIDX][TEPM_astProgramKernelQueue[u32TableIDX].u32Head];		
		//}
		//
		//if (NULL != pstTimedEvent)
		//{
			///* Timed event pointer is NULL for a failed synchronise */
			//switch (pstTimedEvent->enMethod)
			//{
				//case TEPMAPI_enGlobalLinkedFraction:
				//{					
					//u32Temp = CEM_u32GlobalCycleTime / 2;
					//u32Temp *= *(pstTimedEvent->ptEventTime);
					//u32Temp = MIN(0x7fc00000, u32Temp);	
					//u32Temp /= 0x8000;					
					//tEventTimeScheduled = CEM_tGlobalCycleOrigin + u32Temp;
					//tEventTimeRemains = tEventTimeScheduled - pstTimerModule->CNT;
					//
					//if (FALSE == boSynchroniseUpdate)
					//{
						//if ((0x10 > tEventTimeRemains) || (0xfff0 < tEventTimeRemains))
						//{
							//tEventTimeScheduled = pstTimerModule->CNT + 0x80;
						//}
					//}
					//else
					//{
						//if ((0x10 > tEventTimeRemains) || (0xe000 < tEventTimeRemains))
						//{
							//boSynchroniseAbort = TRUE;
						//}												
					//}
//
					//break;
				//}	
				//case TEPMAPI_enGlobalLinkedFractionStep:
				//{					
					//u32Temp = CEM_u32GlobalCycleTime / 2;
					//u32Temp *= *(pstTimedEvent->ptEventTime);
					//u32Temp = MIN(0x7fc00000, u32Temp);	
					//u32Temp /= 0x8000;					
					//tEventTimeScheduled = pstTimerModule->CONTROLS[u32ChannelIDX].CnV + u32Temp;
					//tEventTimeRemains = tEventTimeScheduled - pstTimerModule->CNT;
					//if ((0x10 > tEventTimeRemains) || (0xfff0 < tEventTimeRemains))
					//{
						//tEventTimeScheduled = pstTimerModule->CNT + 0x80;
					//}
//
					//break;
				//}	
				//case TEPMAPI_enHardLinkedTimeStep:
				//{		
					//if (0x7ff0 > *(pstTimedEvent->ptEventTime))
					//{
						//tEventTimeScheduled = pstTimerModule->CONTROLS[u32ChannelIDX].CnV + *(pstTimedEvent->ptEventTime);
						//tEventTimeRemains = tEventTimeScheduled - pstTimerModule->CNT;
						//if ((0x10 > tEventTimeRemains) || (0xfff0 < tEventTimeRemains))
						//{
							//tEventTimeScheduled = pstTimerModule->CNT + 0x80;
						//}
					//}
					//else
					//{
						//tEventTimeScheduled = pstTimerModule->CONTROLS[u32ChannelIDX].CnV + *(pstTimedEvent->ptEventTime);			
					//}
					//break;
				//}
				//default:
				//{
					//break;
				//}
			//}		
//
			//if (FALSE == boSynchroniseAbort)
			//{
				//TEPM_vCapComAction(pstTimedEvent->enAction, pstTimerModule, u32ChannelIDX, tEventTimeScheduled);	
			//}
			//
			//if (FALSE == boSynchroniseUpdate)
			//{
				//TEPM_atpfEventUserCB[u32TableIDX] = pstTimedEvent->pfEventCB;
				//CQUEUE_vRemoveItem(TEPM_astProgramKernelQueue[u32TableIDX]);	
			//}
		//}
	//}
	//else if (FALSE == boSynchroniseUpdate)
	//{
		//if (TRUE == CQUEUE_boIsStaticActive(TEPM_astProgramKernelQueue[u32TableIDX]))
		//{		
			//CQUEUE_vResetStaticHead(TEPM_astProgramKernelQueue[u32TableIDX]);	
		//}
//
		///* The queue is empty so go ahead and disable interrupts and connection */
		//u32Temp = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC;
		//u32Temp &= ~FTM_CnSC_CHIE_MASK;
		//pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;
	//}
//}

void TEPMHA_vCapComAction(TEPMAPI_tenAction enAction, tstTimerModule* pstTimerModule, uint32 u32ChannelIDX, TEPMAPI_ttEventTime tEventTimeScheduled)
{
	uint32 u32Temp;
	
	switch (enAction)
	{		
		case TEPMAPI_enSetHigh:
		{			
#ifdef BUILD_MK60
			u32Temp =  pstTimerModule->CONTROLS[u32ChannelIDX].CnSC | FTM_CnSC_ELSA_MASK | FTM_CnSC_ELSB_MASK | FTM_CnSC_CHIE_MASK;
			u32Temp &= ~FTM_CnSC_CHF_MASK;			
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;						
			
			while (pstTimerModule->CONTROLS[u32ChannelIDX].CnV != tEventTimeScheduled)
			{
				pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;					
			}			
			break;
#endif //BUILD_MK60
		}
		case TEPMAPI_enSetLow:
		{	
#ifdef BUILD_MK60				
			u32Temp = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC | FTM_CnSC_CHIE_MASK;						
			u32Temp &= ~FTM_CnSC_CHF_MASK;
			u32Temp &= ~(FTM_CnSC_ELSA_MASK);		
			
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;						
			
			while (pstTimerModule->CONTROLS[u32ChannelIDX].CnV != tEventTimeScheduled)
			{
				pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;					
			}			
			//pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;			
			
			break;	
#endif //BUILD_MK60				
		}			
		case TEPMAPI_enToggle:	
		{		
#ifdef BUILD_MK60			
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_CHIE_MASK);
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC &= ~FTM_CnSC_ELSB_MASK;	
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;
			break;		
#endif //BUILD_MK60				
		}
		case TEPMAPI_enEndProgram:			
		{		
#ifdef BUILD_MK60			
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;				
			break;
#endif //BUILD_MK60
		}
		default:
		{
			break;
		}
	}				
}


static tstTimerModule* TEPMHA_pstGetTimerModule(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	tstTimerModule* pstTimerModule;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pstTimerModule = TEPMHA_pstConvertTimerModule(TEPMHA_rastTEPMChannel[u32ChannelIDX].enTimerModule);
	
	return pstTimerModule;
}

tstTimerModule* TEPMHA_pstConvertTimerModule(TEPMHA_tenTimerModule enTimerModule)
{
	tstTimerModule* pstTimerModule;
	
#ifdef BUILD_MK60
	switch (enTimerModule)
	{
		case FTM_enFTM0: pstTimerModule = FTM0; break;
		case FTM_enFTM1: pstTimerModule = FTM1; break;
		case FTM_enFTM2: pstTimerModule = FTM2; break;
		case FTM_enFTM3: pstTimerModule = FTM3; break;
		default: pstTimerModule = FTM0; break;
	}
#endif //BUILD_MK60
	
	return pstTimerModule;
}


uint32 TEPMHA_u32GetFTMTableIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX = 0;
	
#ifdef BUILD_MK60
	if ((EH_IO_TMR16 >= enEHIOResource) && (EH_IO_TMR1 <= enEHIOResource))
	{
		u32ChannelIDX = enEHIOResource - EH_IO_TMR1;
	}
	else
	{
		switch (enEHIOResource)
		{
			case EH_IO_ADSE4: u32ChannelIDX = 16; break;
			case EH_IO_GPSE9: u32ChannelIDX = 16; break;
			case EH_IO_GPSE8: u32ChannelIDX = 16; break;
			case EH_IO_ADSE5: u32ChannelIDX = 16; break;
			case EH_IO_GPSE7: u32ChannelIDX = 16; break;
			case EH_IO_CAN1T: u32ChannelIDX = 16; break;
			case EH_IO_CAN1R: u32ChannelIDX = 16; break;
		}
	}
#endif //BUILD_MK60
	
	return u32ChannelIDX;
}


bool TEPMHA_boFlagIsSet(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX)
{
	bool boFlagIsSet = false;

#ifdef BUILD_MK60
	boFlagIsSet = (FTM_CnSC_CHF_MASK == (FTM_CnSC_CHF_MASK & pstTimerModule->CONTROLS[u32ChannelIDX].CnSC));
#endif //BUILD_MK60
    return boFlagIsSet;
}

bool TEMPHA_boInterruptEnabled(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX)
{
    bool boEnableSet = false;

#ifdef BUILD_MK60
	boEnableSet = (FTM_CnSC_CHIE_MASK == (FTM_CnSC_CHIE_MASK & pstTimerModule->CONTROLS[u32ChannelIDX].CnSC));
#endif
    return boEnableSet;
}

uint32 TEPMHA_u32GetScheduledVal(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX)
{
    uint32 u32ScheduledVal;

#ifdef BUILD_MK60
    u32ScheduledVal = pstTimerModule->CONTROLS[u32ChannelIDX].CnV;
#endif

    return u32ScheduledVal;
}

void TEMPHA_vResetTimerFlag(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX)
{
#ifdef BUILD_MK60
    pstTimerModule->CONTROLS[u32ChannelIDX].CnSC &= ~FTM_CnSC_CHF_MASK;
#endif
}

void TEPMHA_vGetFreeVal(tstTimerModule* pstTimerModule, puint32 pu32Val)
{
#ifdef BUILD_MK60
	*pu32Val = pstTimerModule->CNT;
#endif
#ifdef BUILD_SAM3X8E
    *pu32Val = pstTimerModule->TC_CHANNEL[0].TC_CV;
#endif
}

uint32 TEPMHA_u32GetFreeVal(tstTimerModule* pstTimerModule)
{
    uint32 u32FreeVal;

	#ifdef BUILD_MK60
	u32FreeVal = pstTimerModule->CNT;
	#endif
	#ifdef BUILD_SAM3X8E
	u32FreeVal = pstTimerModule->TC_CHANNEL[0].TC_CV;//matthew
	#endif

	return u32FreeVal;
}

void TEPMHA_vDisconnectEnable(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX)
{
	uint32 u32Temp;

#ifdef BUILD_MK60
	u32Temp = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC;
	u32Temp &= ~FTM_CnSC_CHIE_MASK;
	pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;
#endif //BUILD_MK60
}

void TEPMHA_u32GetTimerVal(IOAPI_tenEHIOResource enEHIOResource, puint32 pu32Val)
{
//matthew todo

}

tstTimerModule* TEPMHA_pstGetTimerModuleFromEnum(TEPMHA_tenTimerModule enTimerModule)
{
	tstTimerModule* pstTimerModule;
	
	switch(enTimerModule)
	{
#ifdef BUILD_MK60
		case FTM_enFTM0: pstTimerModule = FTM0; break;
		case FTM_enFTM1: pstTimerModule = FTM1; break;
		case FTM_enFTM2: pstTimerModule = FTM2; break;
		case FTM_enFTM3: pstTimerModule = FTM3; break;
#endif //BUILD_MK60
		default: pstTimerModule = NULL; break;
	}
	
	return pstTimerModule;
}