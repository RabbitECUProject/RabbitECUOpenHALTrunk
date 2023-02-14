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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

/* Private data declarations
   -------------------------*/
const TEPM_tstTEPMChannel TEPMHA_rastTEPMChannel[] = TEPMHA_nChannelInfo;
TEPMAPI_ttEventTime* TEPMHA_ptMasterClock;
uint32 TEPMHA_u32MissingRepeats;
EXTERN CEM_au32TimerOffsets[];

#define TEPM_nTableCount sizeof(TEPMHA_rastTEPMChannel) / sizeof(TEPM_tstTEPMChannel)

/* Private function declarations
   ----------------------------*/
static void* TEPMHA_pvGetModule(IOAPI_tenEHIOResource);	
static void TEPMHA_vSyncModules(void);
static Bool TEPMHA_boModuleIsTimer(void*);
static Bool TEPMHA_boModuleIsPWM(void*);
static TEPMHA_tenModule TEPMHA_enGetEnumFromVIO(IOAPI_tenEHIOResource);

/* Public function definitions
   ---------------------------*/	 

void TEPMHA_vInitTEPMChannel(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMChannelCB* pstTEPMChannelCB)
{
	tstTimerModule* pstTimerModule;
	uint32 u32ChannelIDX;
	uint32 u32ControlWord = 0;
	pwm_channel_t stPWMChannelCB;
	
#ifdef BUILD_MK60
	vpuint32 vpuFTMReg;
	pstTimerModule = TEPMHA_pvGetModule(enEHIOResource);
	u32ChannelIDX = TEPM_u32GetTimerHardwareChannel(enEHIOResource);
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
	tstPWMModule* pstPWMModule;
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);

	if (TEPMHA_rastTEPMChannel[u32ChannelIDX].enModuleType == TEPMHA_enCapCom)
	{
	    /* If this resource is associated with a timer module */
		pstTimerModule = (tstTimerModule*)TEPMHA_pvGetModule(enEHIOResource);

		switch (pstTEPMChannelCB->enAction)
		{
			case TEPMAPI_enSetHigh:
			{
				u32ControlWord = pstTimerModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2].TC_CMR;

				if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
				{
					u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
					u32ControlWord |= TC_CMR_ACPA_SET;
					u32ControlWord |= TC_CMR_WAVE;
					tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
					tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
					IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
				}
				else
				{
					u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
					u32ControlWord |= TC_CMR_BCPB_SET;
					u32ControlWord |= TC_CMR_WAVE;
					tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
					tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
					IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
				}
				break;
			}
			case TEPMAPI_enSetLow:
			{
				u32ControlWord = pstTimerModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2].TC_CMR;

				if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
				{
					u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
					u32ControlWord |= TC_CMR_WAVE; /* Waveform mode is enabled */
					u32ControlWord |= TC_CMR_ACPA_CLEAR;
					tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
					IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
					tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
				}
				else
				{
					u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
					u32ControlWord |= TC_CMR_WAVE; /* Waveform mode is enabled */
					u32ControlWord |= TC_CMR_BCPB_CLEAR;
					u32ControlWord |= TC_CMR_EEVT_XC0;
					tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
					//tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, TC_SR_CPBS);
					IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
					tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
				}
				break;
			}
			case TEPMAPI_enToggle: u32ControlWord = 0; break;
			case TEPMAPI_enCapRising:
			{
			    u32ControlWord = TC_CMR_LDRA_RISING | TC_CMR_LDRB_RISING | TC_CMR_TCCLKS_TIMER_CLOCK4;
				tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
				tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
				tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, TC_SR_LDRAS | TC_SR_LDRBS);
				IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_3, TEPM_vInterruptHandler, NULL);
				TEPMHA_ptMasterClock = (TEPMAPI_ttEventTime*)&(pstTimerModule->TC_CHANNEL[u32ChannelIDX].TC_CV);
				break;
			}
			case TEPMAPI_enCapFalling: u32ControlWord = 0; break;
			case TEPMAPI_enCapAny://matthew problem here
			{
				//u32ControlWord = TC_CMR_LDRA_RISING | TC_CMR_LDRB_FALLING | TC_CMR_TCCLKS_TIMER_CLOCK4;
			    u32ControlWord = TC_CMR_LDRA_EDGE | TC_CMR_LDRB_EDGE | TC_CMR_TCCLKS_TIMER_CLOCK4;
				tc_init(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, u32ControlWord);
				tc_start(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2);
				tc_enable_interrupt(pstTimerModule, TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2, TC_SR_LDRAS | TC_SR_LDRBS);
				IRQ_vEnableIRQ(TEPMHA_rastTEPMChannel[u32ChannelIDX].enIRQType, IRQ_enPRIO_3, TEPM_vInterruptHandler, NULL);
				TEPMHA_ptMasterClock = (TEPMAPI_ttEventTime*)&(pstTimerModule->TC_CHANNEL[u32ChannelIDX].TC_CV);
				break;
			}
			default:
			{
				break;
			}
			TEPMHA_vSyncModules();
		}
	}
	else if (TEPMHA_rastTEPMChannel[u32ChannelIDX].enModuleType == TEPMHA_enPWM)
	{
	    /* If this resource is associated with a PWM module */
	    pstPWMModule = (tstPWMModule*)TEPMHA_pvGetModule(enEHIOResource);

		memset(&stPWMChannelCB, 0, sizeof(pwm_channel_t));

		stPWMChannelCB.channel = TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel;
		stPWMChannelCB.polarity = PWM_LOW;
		//stPWMChannelCB.alignment = PWM_ALIGN_CENTER;
		stPWMChannelCB.ul_prescaler = 0x0c;//pstTEPMChannelCB->enPreScalar;

        pwm_channel_init(pstPWMModule, &stPWMChannelCB);         
	}
#endif //BUILD_SAM3X8E
}


SYSAPI_tenSVCResult TEPMHA_vInitTEPMResource(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMResourceCB* pstTEPMResourceCB)
{
	tstTimerModule* pstTimerModule = NULL;
	pwm_clock_t stPWMClockConfig;
	
#ifdef BUILD_MK60
	REGSET_tstReg32Val astTEPMReg32Val[2];
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

#ifdef BUILD_SAM3X8E
	tstPWMModule* pstPWMModule = NULL;


	switch (enEHIOResource)
	{
		case EH_VIO_TC0:
		{
		    pstTimerModule = TC0;
			break;
		}
		case EH_VIO_TC1:
		{
			pstTimerModule = TC1;
			break;
		}
		case EH_VIO_TC2:
		{
			pstTimerModule = TC2;
			break;
		}
		case EH_VIO_PWM:
		{
			pstPWMModule = PWM;
			break;
		}
		default:
		{
		     break;
		}
	}

	if (NULL != pstTimerModule)
	{
	    TEPMHA_xRequestTimerClock(pstTimerModule);
	}
	else if (NULL != pstPWMModule)
	{
		TEPMHA_xRequestPWMClock(pstPWMModule);

		stPWMClockConfig.ul_mck = SYS_FREQ_BUS;
		stPWMClockConfig.ul_clka = SYS_FREQ_BUS / pstTEPMResourceCB->enPreScalar;
		stPWMClockConfig.ul_clkb = SYS_FREQ_BUS / pstTEPMResourceCB->enPreScalar;
        pwm_init(pstPWMModule, &stPWMClockConfig);
		IRQ_vEnableIRQ(PWM_IRQn, IRQ_enPRIO_15, TEPM_vInterruptHandler, NULL);
	}
#endif //BUILD_MK60

	return SYSAPI_enOK;
}


void TEPMHA_vForceQueueTerminate(void* pvModule, uint32 u32ChannelIDX, uint32 u32SubChannelIDX)
{
	volatile TEPMAPI_ttEventTime tEventTimeRemains;	
    tstTimerModule* pstTimerModule;
		
	CPU_xEnterCritical();

#ifdef BUILD_MK60
	uint32 u32Temp;
	volatile TEPMAPI_ttEventTime tEventTimeScheduled;
    pstTimerModule = (tstTimerModule*)pvModule;
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

#ifdef BUILD_SAM3X8E
	volatile uint32 u32Stat;
    pstTimerModule = (tstTimerModule*)pvModule;
	sint32 s32ExitLoops = 100;

	if (0 == u32SubChannelIDX)
	{
		TEPMHA_vCapComAction(TEPMAPI_enSetLow, pvModule, u32ChannelIDX, u32SubChannelIDX, tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 10);
		
		tc_disable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPAS);

		u32Stat = 0;

	    tEventTimeRemains = tc_read_ra(pstTimerModule, u32ChannelIDX / 2) -
		                    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		while (0 != (0x80000000 &  tEventTimeRemains))
		{
			tEventTimeRemains = tc_read_ra(pstTimerModule, u32ChannelIDX / 2) -
								tc_read_cv(pstTimerModule, u32ChannelIDX / 2);
		}

	    while ((0 == (u32Stat & TC_SR_CPAS)) && (0 < s32ExitLoops)) 
	    {
		    u32Stat = tc_get_status(pstTimerModule, u32ChannelIDX / 2);
			s32ExitLoops--;
	    }
	}
	else
	{
		TEPMHA_vCapComAction(TEPMAPI_enSetLow, pvModule, u32ChannelIDX, u32SubChannelIDX, tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 10);		
		
		tc_disable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPBS);
		
		u32Stat = 0;
	    tEventTimeRemains = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
		                    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);
		
		while (0 == (0x80000000 &  tEventTimeRemains))
		{
			tEventTimeRemains = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
								tc_read_cv(pstTimerModule, u32ChannelIDX / 2);
		}

	    while ((0 == (u32Stat & TC_SR_CPBS)) && (0 < s32ExitLoops)) 
	    {
		    u32Stat = tc_get_status(pstTimerModule, u32ChannelIDX / 2);
			s32ExitLoops--;
	    }


		/*
	    u32ControlWord = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;
		u32ControlWord &= ~TC_CMR_BCPB_TOGGLE;
		u32ControlWord |= TC_CMR_BCPB_CLEAR;
		pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;

	    u32Stat = 0;
	    tEventTimeRemains = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
		                    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		* If already past then force again *
		if ((UINT32_MAX / 2) < tEventTimeRemains)
	    {	
		    tEventTimeScheduled = tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 120;
			tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
	    }

	    * If is not too close then reschedule very close *
		if (((UINT32_MAX / 2) > tEventTimeRemains) && 
		     (tEventTimeRemains > 50))
	    {	
		    tEventTimeScheduled = tc_read_cv(pstTimerModule, u32ChannelIDX / 2) + 120;
			tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
	    }

		while (0 == (u32Stat & TC_SR_CPBS))
		{
			u32Stat = tc_get_status(pstTimerModule, u32ChannelIDX / 2);

			tEventTimeRemains = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
			tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

			if ((UINT32_MAX / 2) < tEventTimeRemains) break;
		}*/
	}
#endif //BUILD_SAM3X8E
	
	CPU_xExitCritical();	
}

Bool TEPMHA_boCheckFalseAlarm(void* pvModule, uint32 u32ChannelIDX, uint32 u32SubChannelIDX)
{
    Bool boFalseAlarm = FALSE;
	uint32 u32Delay;
    tstTimerModule* pstTimerModule = (tstTimerModule*)pvModule;

	if (0 == u32SubChannelIDX)
	{
		u32Delay = tc_read_ra(pstTimerModule, u32ChannelIDX / 2) -
	    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		if ((UINT32_MAX / 2) > u32Delay)
		{
			boFalseAlarm = TRUE;
		}
	}
	else
	{
		u32Delay = tc_read_rb(pstTimerModule, u32ChannelIDX / 2) -
	    tc_read_cv(pstTimerModule, u32ChannelIDX / 2);

		if ((UINT32_MAX / 2) > u32Delay)
		{
			boFalseAlarm = TRUE;
		}
	}

	return boFalseAlarm;
}


void TEPMHA_vCapComAction(TEPMAPI_tenAction enAction, void* pvModule, uint32 u32ChannelIDX, uint32 u32SubChannelIDX, TEPMAPI_ttEventTime tEventTimeScheduled)
{
	uint32 u32ControlWord;
	tstTimerModule* pstTimerModule;
	tstPWMModule* pstPWMModule;

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
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
            if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	        {
			    pstTimerModule = (tstTimerModule*)pvModule;

				u32ControlWord = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;

				if (0 == u32SubChannelIDX)
				{
					tc_write_ra(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
					u32ControlWord &= ~TC_CMR_ACPA_CLEAR;
					u32ControlWord |= TC_CMR_ACPA_SET;
					pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;
					tc_enable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPAS);
					//tc_write_ra(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
				}
				else if (1 == u32SubChannelIDX)
				{
					tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
					u32ControlWord &= ~TC_CMR_BCPB_CLEAR;
					u32ControlWord |= TC_CMR_BCPB_SET;
					pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;
					tc_enable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPBS);
					//tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
				}  
			}          
			else if (TRUE == TEPMHA_boModuleIsPWM(pvModule))
			{
				pwm_channel_t stPWMChannel;

			    pstPWMModule = (tstPWMModule*)pvModule;

				memset(&stPWMChannel, 0, sizeof(pwm_channel_t));
				stPWMChannel.channel = u32ChannelIDX;

			    pwm_channel_enable(pstPWMModule,  u32ChannelIDX);

				while (pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CCNT > 5)
				{
				    pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CDTY = 1;
				    pstPWMModule->PWM_CH_NUM[u32ChannelIDX].PWM_CPRD = 10;
				}


				pwm_channel_update_period(pstPWMModule, &stPWMChannel, 6000u);
				pwm_channel_update_duty(pstPWMModule, &stPWMChannel, 4000u);
			}
#endif //BUILD_SAM3X8E
            break;
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
#endif //BUILD_MK60		

#ifdef BUILD_SAM3X8E
            if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	        {
			    pstTimerModule = (tstTimerModule*)pvModule;

				u32ControlWord = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;

				if (0 == u32SubChannelIDX)
				{
					u32ControlWord &= ~TC_CMR_ACPA_SET;
					u32ControlWord |= TC_CMR_ACPA_CLEAR;
					pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;
					tc_enable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPAS);
					tc_write_ra(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
				}
				else if (1 == u32SubChannelIDX)
				{
					u32ControlWord &= ~TC_CMR_BCPB_SET;
					u32ControlWord |= TC_CMR_BCPB_CLEAR;
					pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR = u32ControlWord;
					tc_enable_interrupt(pstTimerModule, u32ChannelIDX / 2, TC_SR_CPBS);
					tc_write_rb(pstTimerModule, u32ChannelIDX / 2, tEventTimeScheduled);
				} 
			}      
			else if (TRUE == TEPMHA_boModuleIsPWM(pvModule))
			{

			}  
#endif //BUILD_SAM3X8E	
            break;	
		}			
		case TEPMAPI_enToggle:	
		{		
#ifdef BUILD_MK60			
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_CHIE_MASK);
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC &= ~FTM_CnSC_ELSB_MASK;	
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;
#endif //BUILD_MK60	
            break;			
		}
		case TEPMAPI_enEndProgram:			
		{		
#ifdef BUILD_MK60			
			pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = pstTimerModule->CONTROLS[u32ChannelIDX].CnSC;
			pstTimerModule->CONTROLS[u32ChannelIDX].CnV = tEventTimeScheduled;				
#endif //BUILD_MK60
            break;
		}
		default:
		{
			break;
		}
	}				
}


static void* TEPMHA_pvGetModule(IOAPI_tenEHIOResource enEHIOResource)
{
	uint32 u32ChannelIDX;
	void* pvModule;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pvModule = TEPMHA_pvGetModuleFromEnum(TEPMHA_rastTEPMChannel[u32ChannelIDX].enModule);
	
	return pvModule;
}


void* TEPMHA_pvGetModuleFromEnum(TEPMHA_tenModule enTimerModule)
{
	void* pvModule;
	
#ifdef BUILD_MK60
	switch (enTimerModule)
	{
		case FTM_enFTM0: pvModule = (tstTimerModule*)FTM0; break;
		case FTM_enFTM1: pvModule = (tstTimerModule*)FTM1; break;
		case FTM_enFTM2: pvModule = (tstTimerModule*)FTM2; break;
		case FTM_enFTM3: pvModule = (tstTimerModule*)FTM3; break;
		default: pvModule = NULL; break;
	}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
	switch (enTimerModule)
	{
		case TEPMHA_enTC0: pvModule = (void*)TC0; break;
		case TEPMHA_enTC1: pvModule = (void*)TC1; break;
		case TEPMHA_enTC2: pvModule = (void*)TC2; break;
		case TEPMHA_enPWM0: pvModule = (void*)PWM; break;
		default: pvModule = NULL; break;
	}
#endif //BUILD_SAMX3X8E
	
	return pvModule;
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
	else if (EH_IO_ADD5 == enEHIOResource)
	{
	    u32ChannelIDX = 12;
	}
	else if (EH_IO_ADD7 == enEHIOResource)
	{
		u32ChannelIDX = 13;
	}
	else if (EH_IO_ADD8 == enEHIOResource)
	{
		u32ChannelIDX = 14;
	}
	else if (EH_IO_EXTINT == enEHIOResource)
	{
		u32ChannelIDX = 15;
	}
#endif //BUILD_SAM3X8E
	
	return u32ChannelIDX;
}



Bool TEPMHA_boFlagIsSet(void* pvModule, uint32 u32ChannelIDX, puint32 pu32Flags, uint32 u32Sequence, uint32 u32Prio)
{
	Bool boFlagIsSet = false;
	tstTimerModule* pstTimerModule;

#ifdef BUILD_MK60
    pstTimerModule = (tstTimerModule*)pvModule;
	boFlagIsSet = (FTM_CnSC_CHF_MASK == (FTM_CnSC_CHF_MASK & pstTimerModule->CONTROLS[u32ChannelIDX].CnSC));
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
    static uint32 u32Seq[2];
    static uint32 u32FlagsCache[9];   /* Once the flags are read they are cleared so cache them */
	uint32 u32CacheIndex = 0;
	uint32 u32CMR;
	Bool boWaveMode;

	if ((TC0 == (tstTimerModule*)pvModule) ||
	    (TC1 == (tstTimerModule*)pvModule) ||
		(TC2 == (tstTimerModule*)pvModule))
	{
	    pstTimerModule = (tstTimerModule*)pvModule;

	    switch ((uint32)pstTimerModule)
		{
			case (uint32)TC0: u32CacheIndex = (u32ChannelIDX / 2); break;
			case (uint32)TC1: u32CacheIndex = 3 + (u32ChannelIDX / 2); break;
			case (uint32)TC2: u32CacheIndex = 6 + (u32ChannelIDX / 2); break;
		}

		if (u32Seq[u32Prio] != u32Sequence)
		{
			u32FlagsCache[u32CacheIndex] = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_SR;
			u32Seq[u32Prio] = u32Sequence;
		}

		u32CMR = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_CMR;

		if (0 == u32FlagsCache[u32CacheIndex])
		{
			u32FlagsCache[u32CacheIndex] = pstTimerModule->TC_CHANNEL[u32ChannelIDX / 2].TC_SR;
		}

		*pu32Flags = u32FlagsCache[u32CacheIndex];
		boWaveMode = TC_CMR_WAVE == (u32CMR & TC_CMR_WAVE) ? TRUE : FALSE;

		if (0 == (u32ChannelIDX & 0x1))
		{
			/* Is channel A? */
			if (FALSE == boWaveMode)
			{
				boFlagIsSet = 0 != (*pu32Flags & (TC_SR_LDRAS | TC_SR_LDRBS)) ?
				TRUE : FALSE;
			}
			else
			{
				boFlagIsSet = 0 != (*pu32Flags & TC_IMR_CPAS) ? TRUE : FALSE;
			}
		}
		else
		{
			/* Is channel B? */
			if (FALSE == boWaveMode)
			{
				/* There is no input capture on the physical B channel */
				boFlagIsSet = FALSE;
			}
			else
			{
				boFlagIsSet = 0 != (*pu32Flags & TC_IMR_CPBS) ? TRUE : FALSE;
			}
		}
	}
	else if (PWM == (tstPWMModule*)pvModule)
	{
	    boFlagIsSet = TRUE;
	}	
#endif //BUILD_SAM3X8E

    return boFlagIsSet;
}

Bool TEMPHA_boInterruptEnabled(void* pvModule, uint32 u32ChannelIDX)
{
    Bool boEnableSet = false;

#ifdef BUILD_MK60
	boEnableSet = (FTM_CnSC_CHIE_MASK == (FTM_CnSC_CHIE_MASK & (*tstTimerModule)pvModule->CONTROLS[u32ChannelIDX].CnSC));
#endif

#ifdef BUILD_SAM3X8E    
    if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	{
	    tstTimerModule* pstModule = (tstTimerModule*)pvModule;
		if (0 == (u32ChannelIDX & 0x01))
		{
			boEnableSet = 0 != (pstModule->TC_CHANNEL[u32ChannelIDX / 2].TC_IMR & (TC_IMR_LDRAS | TC_IMR_CPAS)) ?
			TRUE : FALSE;
		}
		else
		{
			boEnableSet = 0 != (pstModule->TC_CHANNEL[u32ChannelIDX / 2].TC_IMR & (TC_IMR_LDRBS | TC_IMR_CPBS)) ?
			TRUE : FALSE;
		}	
	}
	else if (TRUE == TEPMHA_boModuleIsPWM(pvModule))
	{

	}
#endif //BUILD_SAM3X8E

    return boEnableSet;
}

TEPMAPI_ttEventTime TEPMHA_tGetScheduledVal(void* pvModule, uint32 u32ChannelIDX, Bool boInputMode, uint32 u32Flags)
{
    TEPMAPI_ttEventTime tEventTime = 0;

#ifdef BUILD_MK60
    tEventTime = pstTimerModule->CONTROLS[u32ChannelIDX].CnV;
#endif

#ifdef BUILD_SAM3X8E
    if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	{
		if (TRUE == boInputMode)
		{
			if (0 != (u32Flags & TC_SR_LDRAS))
			{
				tEventTime = tc_read_ra((tstTimerModule*)pvModule, u32ChannelIDX / 2);
			}
			if (0 != (u32Flags & TC_SR_LDRBS))
			{
				tEventTime = tc_read_rb((tstTimerModule*)pvModule, u32ChannelIDX / 2);
			}
		}
		else
		{
			if (0 == (u32ChannelIDX & 0x01))
			{
				tEventTime = tc_read_ra((tstTimerModule*)pvModule, u32ChannelIDX / 2);
			}
			else
			{
				tEventTime = tc_read_rb((tstTimerModule*)pvModule, u32ChannelIDX / 2);
			}
		}	
	}
    else if (TRUE == TEPMHA_boModuleIsPWM(pvModule))
    {

	}
#endif //BUILD_SAM3X8E


    return tEventTime;
}

IOAPI_tenTriState TEPMHA_enGetTimerDigitalState(IOAPI_tenEHIOResource enEHIOResource)
{
    IOAPI_tenTriState enTriState = IOAPI_enError;
	uint32 u32ChannelIDX;
	void* pvModule;
	
	u32ChannelIDX = TEPMHA_u32GetFTMTableIndex(enEHIOResource);
	pvModule = TEPMHA_pvGetModuleFromEnum(TEPMHA_rastTEPMChannel[u32ChannelIDX].enModule);

#ifdef BUILD_MK60
	enTriState = IO_enGetDIOResourceState(enEHIOResource);
#endif

#ifdef BUILD_SAM3X8E
    if (((EH_IO_TMR1 <= enEHIOResource) && (EH_IO_TMR12 >= enEHIOResource)) ||
	(EH_IO_ADD5 == enEHIOResource) ||
	(EH_IO_EXTINT == enEHIOResource) ||
	(EH_IO_ADD6 == enEHIOResource) ||
	(EH_IO_ADD7 == enEHIOResource))
	{
	    if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
		{
		    tstTimerModule* pstModule = (tstTimerModule*)pvModule;
			if (0 == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel)
			{
				enTriState = 0 != (pstModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2].TC_SR & TC_SR_MTIOA) ?
					IOAPI_enHigh : IOAPI_enLow;
			}
			else
			{
				enTriState = 0 != (pstModule->TC_CHANNEL[TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel / 2].TC_SR & TC_SR_MTIOB) ?
				    IOAPI_enHigh : IOAPI_enLow;
			}
		}
	}
#endif

    return enTriState;
}

void TEMPHA_vResetTimerFlag(void* pvModule, uint32 u32ChannelIDX)
{
#ifdef BUILD_MK60
    (tstTimerModule*)pvModule->CONTROLS[u32ChannelIDX].CnSC &= ~FTM_CnSC_CHF_MASK;
#endif
}


uint32 TEPMHA_u32GetFreeVal(void* pvModule, uint32 u32ChannelIDX)
{
    uint32 u32FreeVal = ~0;

#ifdef BUILD_MK60
	u32FreeVal = pstTimerModule->CNT;
#endif
#ifdef BUILD_SAM3X8E
    if (TRUE == TEPMHA_boModuleIsTimer(pvModule))
	{
	    u32FreeVal = ((tstTimerModule*)pvModule)->TC_CHANNEL[u32ChannelIDX / 2].TC_CV;
	}
#endif

	return u32FreeVal;
}

void TEPMHA_vDisconnectEnable(void* pvModule, uint32 u32ChannelIDX)
{
#ifdef BUILD_MK60
	uint32 u32Temp;
	u32Temp = (tstTimerModule*)pvModule->CONTROLS[u32ChannelIDX].CnSC;
	u32Temp &= ~FTM_CnSC_CHIE_MASK;
	pstTimerModule->CONTROLS[u32ChannelIDX].CnSC = u32Temp;
#endif //BUILD_MK60
}

TEPMAPI_ttEventTime TEPMHA_u32GetTimerVal(IOAPI_tenEHIOResource enEHIOResource)
{
	tstTimerModule* pstTimerModule;
	TEPMAPI_ttEventTime tEventTime = 0;

#ifdef BUILD_SAM3X8E
	pstTimerModule = TEPMHA_pvGetTimerModuleFromVIO(enEHIOResource);
	tEventTime = pstTimerModule->TC_CHANNEL[0].TC_CV;
#endif //BUILD_SAM3X8E

	return tEventTime;
}

void* TEPMHA_pvGetTimerModuleFromVIO(IOAPI_tenEHIOResource enEHIOResource)
{
	void* pvModule;
	
	switch(enEHIOResource)
	{
#ifdef BUILD_MK60

#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
        case EH_VIO_TC0: pvModule = (tstTimerModule*)TC0; break;	
        case EH_VIO_TC1: pvModule = (tstTimerModule*)TC1; break;
        case EH_VIO_TC2: pvModule = (tstTimerModule*)TC2; break;
		case EH_VIO_TCC0: pvModule = (tstTimerModule*)TC0; break;
		case EH_VIO_TCC1: pvModule = (tstTimerModule*)TC0; break;
		case EH_VIO_TCC2: pvModule = (tstTimerModule*)TC0; break;
		case EH_VIO_TCC3: pvModule = (tstTimerModule*)TC1; break;
		case EH_VIO_TCC4: pvModule = (tstTimerModule*)TC1; break;
		case EH_VIO_TCC5: pvModule = (tstTimerModule*)TC1; break;
		case EH_VIO_TCC6: pvModule = (tstTimerModule*)TC2; break;
		case EH_VIO_TCC7: pvModule = (tstTimerModule*)TC2; break;
		case EH_VIO_TCC8: pvModule = (tstTimerModule*)TC2; break;
		case EH_VIO_PWM: pvModule = (tstPWMModule*)PWM; break;
#endif
		default: pvModule = NULL; break;
	}
	
	return pvModule;
}

uint32 TEPMHA_u32GetTimerChannelsPerInterruptGroup(void)
{
    uint32 u32ChannelCount = 0;
#ifdef BUILD_MK60
    u32ChannelCount = 8;
#endif //BUILD_SAM3X8E

#ifdef BUILD_SAM3X8E
    u32ChannelCount = 2;
#endif //BUILD_SAM3X8E
	
	return u32ChannelCount;
}

IOAPI_tenEHIOResource TEPMHA_enGetTimerResourceFromVIOAndIndex(IOAPI_tenEHIOResource enEHIOResource, uint32 u32Channel)
{
    uint32 u32ChannelIDX;
	IOAPI_tenEHIOResource enChannelEHIOResource = EH_IO_Invalid;

#ifdef BUILD_SAM3X8E
    TEPMHA_tenModule enModule = TEPMHA_enGetEnumFromVIO(enEHIOResource);

	for (u32ChannelIDX = 0; u32ChannelIDX < TEPMHA_nEventChannels; u32ChannelIDX++)
	{   
	    if ((enModule == TEPMHA_rastTEPMChannel[u32ChannelIDX].enModule) &&
		    (u32Channel == TEPMHA_rastTEPMChannel[u32ChannelIDX].u32Channel + TEPMHA_rastTEPMChannel[u32ChannelIDX].u32SubChannel) &&
            (TEPMHA_enCapCom == TEPMHA_rastTEPMChannel[u32ChannelIDX].enModuleType))
		{
		    enChannelEHIOResource = TEPMHA_rastTEPMChannel[u32ChannelIDX].enEHIOResource;
		    break;
		}
	}
#endif //BUILD_SAM3X8E
	return enChannelEHIOResource;
}

uint32 TEPMHA_u32GetTimerStartChannelInterruptGroup(IOAPI_tenEHIOResource enEHIOResource)
{
    uint32 u32StartChannel;

#ifdef BUILD_SAM3X8E
    switch (enEHIOResource)
	{
	    case EH_VIO_TCC0: u32StartChannel = 0; break;
	    case EH_VIO_TCC1: u32StartChannel = 2; break;
	    case EH_VIO_TCC2: u32StartChannel = 4; break;
	    case EH_VIO_TCC3: u32StartChannel = 0; break;
	    case EH_VIO_TCC4: u32StartChannel = 2; break;
	    case EH_VIO_TCC5: u32StartChannel = 4; break;
	    case EH_VIO_TCC6: u32StartChannel = 0; break;
	    case EH_VIO_TCC7: u32StartChannel = 2; break;
	    case EH_VIO_TCC8: u32StartChannel = 4; break;
		default : u32StartChannel = 0; break;
	}
#endif //BUILD_SAM3X8E

    return u32StartChannel;
}

uint32 TEPMHA_u32GetTimerHardwareSubChannel(uint32 u32TableIndex)
{
    uint32 u32ChannelSubIDX = 0;

#ifdef BUILD_SAM3X8E
	u32ChannelSubIDX = TEPMHA_rastTEPMChannel[u32TableIndex].u32SubChannel;
#endif //BUILD_SAM3X8E

    return u32ChannelSubIDX;
}

TEPMHA_tenModule TEPMHA_enTimerEnumFromModule(tstTimerModule* pstTimerModule)
{
#ifdef BUILD_SAM3X8E
	TEPMHA_tenModule enTimerModule = TEPMHA_enTC0;

    if (TC0 == pstTimerModule)
	{
		enTimerModule = TEPMHA_enTC0;
	}
    else if (TC1 == pstTimerModule)
    {
		enTimerModule = TEPMHA_enTC1;
	}
    else if (TC2 == pstTimerModule)
    {
		enTimerModule = TEPMHA_enTC2;
	}
#endif //BUILD_SAM3X8E

	return enTimerModule;
}


static void TEPMHA_vSyncModules(void)
{
#ifdef BUILD_SAM3X8E
	tc_sync_trigger((tstTimerModule*)TC0);
	tc_sync_trigger((tstTimerModule*)TC1);
	tc_sync_trigger((tstTimerModule*)TC2);
#endif //BUILD_SAM3X8E
}

static Bool TEPMHA_boModuleIsTimer(void* pvModule)
{
    Bool boRetVal = FALSE;

    #ifdef BUILD_SAM3X8E
    if ((TC0 == (tstTimerModule*)pvModule) ||
        (TC1 == (tstTimerModule*)pvModule) ||
        (TC2 == (tstTimerModule*)pvModule))
    {
	    boRetVal = TRUE;
    }
    #endif //BUILD_SAM3X8E

    return boRetVal;
}

static TEPMHA_tenModule TEPMHA_enGetEnumFromVIO(IOAPI_tenEHIOResource enEHIOResource)
{
    TEPMHA_tenModule enModule;
#ifdef BUILD_SAM3X8E
    switch (enEHIOResource)
	{
	    case EH_VIO_TC0:
		case EH_VIO_TCC0:
		case EH_VIO_TCC1:
		case EH_VIO_TCC2:
		{
		    enModule = TEPMHA_enTC0;
			break;
		}
	    case EH_VIO_TC1:
	    case EH_VIO_TCC3:
	    case EH_VIO_TCC4:
	    case EH_VIO_TCC5:
	    {
		    enModule = TEPMHA_enTC1;
		    break;
	    }
	    case EH_VIO_TC2:
	    case EH_VIO_TCC6:
	    case EH_VIO_TCC7:
	    case EH_VIO_TCC8:
	    {
		    enModule = TEPMHA_enTC2;
		    break;
	    }
		case EH_VIO_PWM:
		{

		    enModule = TEPMHA_enPWM0;
			break;
		}
		default:
		{
		    enModule = TEPMHA_enModuleInvalid;
			break;		 
		}
	}
#endif //BUILD_SAM3X8E

    return enModule;
}

static Bool TEPMHA_boModuleIsPWM(void* pvModule)
{
    Bool boRetVal = FALSE;

#ifdef BUILD_SAM3X8E
    if (PWM == (tstPWMModule*)pvModule)
    {
	    boRetVal = TRUE;
    }
#endif //BUILD_SAM3X8E

    return boRetVal;
}

void TEPMHA_vConfigureMissingToothInterrupt(void)
{
	uint32 u32ControlWord = 0;
		
#if defined(BUILD_MK60) || defined(BUILD_MK64)
	vpuint32 vpuFTMReg;
	tstTimerModule* pstTimerModule = FTM1;
	vpuFTMReg = (vpuint32)((uint32)pstTimerModule + (uint32)offsetof(tstTimerModule, CONTROLS[0]));

	u32ControlWord = (FTM_CnSC_MSA_MASK | FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK);
	u32ControlWord |= FTM_CnSC_CHIE_MASK;
	*vpuFTMReg = u32ControlWord;

	IRQ_vEnableIRQ(FTM1_IRQn, IRQ_enPRIO_15, TEPM_vMissingToothInterruptHandler, NULL);
#endif //BUILD_MK6X

#ifdef BUILD_SAM3X8E
	tstTimerModule* pstTimerModule = TC1;

	u32ControlWord = pstTimerModule->TC_CHANNEL[0].TC_CMR;
	u32ControlWord |= TC_CMR_TCCLKS_TIMER_CLOCK4;
	u32ControlWord |= TC_CMR_ACPA_SET;
	u32ControlWord |= TC_CMR_WAVE;
	
	tc_init(pstTimerModule, 0, u32ControlWord);
	tc_start(pstTimerModule, 0);
	
	IRQ_vEnableIRQ(TC3_IRQn, IRQ_enPRIO_15, TEPM_vMissingToothInterruptHandler, NULL);
#endif
}

uint32 TEPMHA_u32SetNextMissingToothInterrupt(TEPMAPI_ttEventTime tReference, TEPMAPI_ttEventTime tLastGap, uint32 u32Repeats)
{
	static uint32 u32Gap = 0;
	uint32 u32FlagsCache;
	
#if defined (BUILD_SAM3X8E)
	tstTimerModule* pstTimerModule = TC1;
	uint32 u32TimerVal;

	if (0 != u32Repeats)
	{
		u32TimerVal = tReference;
		u32TimerVal += CEM_au32TimerOffsets[3];
		u32Gap = tLastGap;
		TEPMHA_u32MissingRepeats = u32Repeats;
	}
	else
	{
		u32TimerVal = pstTimerModule->TC_CHANNEL[0].TC_RA;
	}

	if (0 != TEPMHA_u32MissingRepeats)
	{
		u32TimerVal += u32Gap;
		
		/* Read status register to clear flags */
		u32FlagsCache = pstTimerModule->TC_CHANNEL[0].TC_SR;
		
		tc_write_ra(pstTimerModule, 0, u32TimerVal);
		tc_enable_interrupt(pstTimerModule, 0, TC_SR_CPAS);
		TEPMHA_u32MissingRepeats--;
	}
	else
	{
		tc_disable_interrupt(pstTimerModule, 0, TC_SR_CPAS);
	}
#endif //BUILD_SAM3X8E

#if defined(BUILD_MK60) || defined(BUILD_MK64)
	uint32 u32ControlWord;
#endif //BUILD_MK6X

	return u32Gap;
}

#pragma GCC diagnostic pop