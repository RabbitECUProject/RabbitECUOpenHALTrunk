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
const TEPM_tstTEPMChannel TEPMHA_rastTEPMChannel[] = TEPMHA_nChannelInfo;

#define TEPM_nTableCount sizeof(TEPMHA_rastTEPMChannel) / sizeof(TEPM_tstTEPMChannel)

/* Private function declarations
   ----------------------------*/
static void TEPMHA_vRunEventProgramUserQueue(tstTimerModule*, uint32, uint32);
static void TEPMHA_vRunEventProgramKernelQueue(tstTimerModule*, uint32, uint32, Bool);
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
	uint32 u32ChannelSubIDX;
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

#ifdef BUILD_SAM3X8E
	pstTimerModule = TEPMHA_pstGetTimerModule(enEHIOResource);
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	SIM_boEnablePeripheralClock(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType);

	switch (pstTEPMChannelCB->enAction)
	{
		case TEPMAPI_enSetHigh:
		{
		    u32ControlWord = pstTimerModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel].TC_CMR;

			if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
			{
			    u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
				u32ControlWord |= TC_CMR_ACPA_SET;
				u32ControlWord |= TC_CMR_WAVE;
			    tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, u32ControlWord);
			    tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel);
			    tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, TC_SR_CPAS);
			}
			else
			{
				u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
				u32ControlWord |= TC_CMR_BCPB_SET;
				u32ControlWord |= TC_CMR_WAVE;
				tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, u32ControlWord);
				tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel);
				tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, TC_SR_CPBS);
			}
            break;
        }
		case TEPMAPI_enSetLow:
		{
			u32ControlWord = pstTimerModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel].TC_CMR;

			if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
			{
                u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
                u32ControlWord |= TC_CMR_WAVE; /* Waveform mode is enabled */
                u32ControlWord |= TC_CMR_ACPA_CLEAR;
				//u32ControlWord |= TC_CMR_ACPC_CLEAR;
				tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, u32ControlWord);
				tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, TC_SR_CPAS);
				tc_write_ra(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, 0xffff);
				tc_write_rc(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, 0xffff);
				tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel);
			}
			else
			{
                u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
                u32ControlWord |= TC_CMR_WAVE; /* Waveform mode is enabled */
                u32ControlWord |= TC_CMR_BCPB_CLEAR;
                //u32ControlWord |= TC_CMR_BCPC_CLEAR;
				tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, u32ControlWord);
				tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, TC_SR_CPBS);
				tc_write_rb(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, 0xffff);
				tc_write_rc(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, 0xffff);
				tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel);
			}
			break;
		}
		case TEPMAPI_enToggle: u32ControlWord = 0; break;
		case TEPMAPI_enCapRising: u32ControlWord = 0; break;
		case TEPMAPI_enCapFalling: u32ControlWord = 0; break;
		case TEPMAPI_enCapAny:
		{
			u32ControlWord = TC_CMR_LDRA_EDGE | TC_CMR_LDRB_EDGE | TC_CMR_TCCLKS_TIMER_CLOCK4;
			tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, u32ControlWord);
			tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel);
			tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel, TC_SR_LDRAS | TC_SR_LDRBS);
            break;
		}
		default:
		{
		    break;
		}
	}
#endif //BUILD_SAM3X8E
}


SYSAPI_tenSVCResult TEPMHA_vInitTEPMResource(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMResourceCB* pstTEPMResourceCB)
{
	tstTimerModule* pstTimerModule;
	REGSET_tstReg32Val astTEPMReg32Val[2];
	
#ifdef BUILD_MK60
	switch (enEHIOResource)
	{
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
		default:
		{
			pstTimerModule = NULL;
			break;
		}	
	    TEPMHA_xRequestPortClock(pstTimerModule);				
	}	

	astTEPMReg32Val[0].reg = (vpuint32)(pstTimerModule + offsetof(tstTimerModule, SC));
	astTEPMReg32Val[0].val = (uint32)(FTM_SC_PS(pstTEPMResourceCB->enPreScalar) | FTM_SC_CLKS(1) |
			 (pstTEPMResourceCB->enCountType << FTM_SC_CPWMS_SHIFT));														
	astTEPMReg32Val[0].writeMode = REGSET_enOverwrite;	
	astTEPMReg32Val[1].reg = NULL;			
	REGSET_vInitReg32(&astTEPMReg32Val[0]);		
#endif //BUILD_MK60

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

//static void TEPMHA_vRunEventProgramKernelQueue(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX, uint32 u32TableIDX, Bool boSynchroniseUpdate)
//{
	//TEPMAPI_tstTimedKernelEvent* pstTimedEvent = NULL;
	//TEPMAPI_ttEventTime tEventTimeScheduled;
	//TEPMAPI_ttEventTime tEventTimeRemains;	
	//uint32 u32Temp;
	//Bool boSynchroniseAbort = FALSE;
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

#ifdef BUILD_SAM3X8E
	switch (enTimerModule)
	{
		case TEPMHA_enTC0: pstTimerModule = TC0; break;
		case TEPMHA_enTC1: pstTimerModule = TC1; break;
		case TEPMHA_enTC2: pstTimerModule = TC2; break;
		default: pstTimerModule = TC0; break;
	}
#endif //BUILD_SAMX3X8E
	
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

#ifdef BUILD_SAM3X8E
if ((EH_IO_TMR12 >= enEHIOResource) && (EH_IO_TMR1 <= enEHIOResource))
{
	u32ChannelIDX = enEHIOResource - EH_IO_TMR1;
}
#endif //BUILD_SAM3X8E
	
	return u32ChannelIDX;
}




Bool TEPMHA_boFlagIsSet(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX, Bool boInputMode, puint32 pu32Flags, uint32 u32Sequence)
{
	Bool boFlagIsSet = false;

#ifdef BUILD_MK60
	boFlagIsSet = (FTM_CnSC_CHF_MASK == (FTM_CnSC_CHF_MASK & pstTimerModule->CONTROLS[u32ChannelIDX].CnSC));
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
    static uint32 u32Seq;
    static uint32 u32FlagsCache[9];   /* Once the flags are read they are cleared so cache them */
	uint32 u32CacheIndex = 0;

	if (u32Seq != u32Sequence)
	{
	    memset(&u32FlagsCache, 0, sizeof(u32FlagsCache));
		u32Seq = u32Sequence;
	}

	switch ((uint32)pstTimerModule)
	{
        case (uint32)TC0: u32CacheIndex = (u32ChannelIDX / 2); break;
        case (uint32)TC1: u32CacheIndex = 3 + (u32ChannelIDX / 2); break;
        case (uint32)TC2: u32CacheIndex = 6 + (u32ChannelIDX / 2); break;
	}

    if (TRUE == boInputMode)
	{
	    if (0 == u32FlagsCache[u32CacheIndex])
		{
	        u32FlagsCache[u32CacheIndex] = pstTimerModule->TC_CHANNEL[u32ChannelIDX].TC_SR;
		}
		*pu32Flags = u32FlagsCache[u32CacheIndex];

		if (0 == (u32ChannelIDX & 0x1))
		/* Is channel A? */
		{
            boFlagIsSet = 0 != (*pu32Flags & TC_SR_LDRAS) ?
		    TRUE : FALSE;
		}
		else
		/* Is channel B? */
		{
			boFlagIsSet = 0 != (*pu32Flags & TC_SR_LDRBS) ?
			TRUE : FALSE;
		}
	}
    else
    {
	    if (0 == u32FlagsCache[u32CacheIndex])
	    {
		    u32FlagsCache[u32CacheIndex] = pstTimerModule->TC_CHANNEL[u32ChannelIDX].TC_SR;
	    }
	    *pu32Flags = u32FlagsCache[u32CacheIndex];

		if (0 == (u32ChannelIDX & 0x1))
		/* Is channel A? */
		{
	        boFlagIsSet = 0 != (*pu32Flags & TC_IMR_CPAS) ? TRUE : FALSE;
		}
		else
		/* Is channel B? */
		{
			boFlagIsSet = 0 != (*pu32Flags & TC_IMR_CPBS) ? TRUE : FALSE;
		}

    }
#endif //BUILD_SAM3X8E

    return boFlagIsSet;
}

Bool TEMPHA_boInterruptEnabled(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX, Bool boInputMode)
{
    Bool boEnableSet = false;

#ifdef BUILD_MK60
	boEnableSet = (FTM_CnSC_CHIE_MASK == (FTM_CnSC_CHIE_MASK & pstTimerModule->CONTROLS[u32ChannelIDX].CnSC));
#endif

#ifdef BUILD_SAM3X8E
    if (TRUE == boInputMode)
	{
        boEnableSet = 0 != (pstTimerModule->TC_CHANNEL[u32ChannelIDX].TC_IMR & (TC_IMR_LDRAS | TC_IMR_LDRBS)) ?
		    TRUE : FALSE;
    }
#endif //BUILD_SAM3X8E

    return boEnableSet;
}

TEPMAPI_ttEventTime TEPMHA_tGetScheduledVal(tstTimerModule* pstTimerModule, uint32 u32ChannelIDX, Bool boInputMode, uint32 u32Flags)
{
    TEPMAPI_ttEventTime tEventTime = 0;
	uint32 u32Temp;

#ifdef BUILD_MK60
    tEventTime = pstTimerModule->CONTROLS[u32ChannelIDX].CnV;
#endif

#ifdef BUILD_SAM3X8E
    if (TRUE == boInputMode)
	{
        if (0 != (u32Flags & TC_SR_LDRAS))
		{
		    u32Temp = tc_read_ra(pstTimerModule, u32ChannelIDX);
		    tEventTime = (uint16)(u32Temp >> 1);
	    }
        if (0 != (u32Flags & TC_SR_LDRBS))
		{
		    u32Temp = tc_read_rb(pstTimerModule, u32ChannelIDX);
		    tEventTime = (uint16)(u32Temp >> 1);
	    }
    }
#endif //BUILD_SAM3X8E

    return tEventTime;
}

IOAPI_tenTriState TEPMHA_enGetTimerDigitalState(IOAPI_tenEHIOResource enEHIOResource)
{
    IOAPI_tenTriState enTriState = IOAPI_enError;
	uint32 u32ChannelIDX;
	tstTimerModule* pstTimerModule;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pstTimerModule = TEPMHA_pstConvertTimerModule(TEPMHA_rastTEPMChannel[u32ChannelIDX].enTimerModule);

#ifdef BUILD_MK60
	enTriState = IO_enGetDIOResourceState(enEHIOResource);
#endif

#ifdef BUILD_SAM3X8E
    if ((EH_IO_TMR1 <= enEHIOResource) && (EH_IO_TMR12 >= enEHIOResource))
	{
	    if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
		{
		    enTriState = 0 != (pstTimerModule->TC_CHANNEL->TC_SR & TC_SR_MTIOA) ?
			    IOAPI_enHigh : IOAPI_enLow;
		}
		else
		{
		    enTriState = 0 != (pstTimerModule->TC_CHANNEL->TC_SR & TC_SR_MTIOB) ?
		    IOAPI_enHigh : IOAPI_enLow;
		}
	}
#endif

    return enTriState;
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
    //*pu32Val = pstTimerModule->TC_CHANNEL[0].TC_CV;
#endif
}

uint32 TEPMHA_u32GetFreeVal(tstTimerModule* pstTimerModule)
{
    uint32 u32FreeVal;

	#ifdef BUILD_MK60
	u32FreeVal = pstTimerModule->CNT;
	#endif
	#ifdef BUILD_SAM3X8E
	//u32FreeVal = pstTimerModule->TC_CHANNEL[0].TC_CV;//matthew
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

static void TEPMHA_vInitInterrupts(IRQn_Type enIrq)
{
	IRQ_vEnableIRQ(enIrq, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
}

static uint32 TEPMHA_pstGetFTMChannel(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	return TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel;
}