/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "CPUAbstract.h"
#include "regset.h"
#include "sys.h"
#include "declarations.h"
#include "os.h"
#include "IOAPI.h"
#include "DACAPI.h"
#include "DAC.h"
#include "types.h"
#include "SIM.h"

DACAPI_ttOutputVoltage DACHA_atQueue[DACHA_nDACCount][DAC_nQueueSize];
uint32 DACHA_au32QueueIDX[DACHA_nDACCount];
uint32 DACHA_u32PortClockRequested;

void DACHA_vStart(puint32 const pu32Stat)
{

}

void DACHA_vRun(puint32 const pu32Stat)
{
#ifdef BUILD_MK60	
	tstDACModule* pstDAC = NULL;	
	uint32 u32QueueIDX;
	uint32 u32DACIDX;
	uint32 u32ClockReq = 1;
		
	for (u32DACIDX = 0; u32DACIDX < DACHA_nDACCount; u32DACIDX++)
	{
		if (u32ClockReq == (u32ClockReq & DACHA_u32PortClockRequested))
		{

			pstDAC = (0 == u32DACIDX) ? DAC0 : DAC1;	
				
			if ((pstDAC->C2 & DAC_C2_DACBFUP_MASK) < ++DACHA_au32QueueIDX[u32DACIDX])
			{
				for (u32QueueIDX = 0; u32QueueIDX <= (pstDAC->C2 & DAC_C2_DACBFUP_MASK); u32QueueIDX++)
				{		
					pstDAC->DAT[u32QueueIDX].DATH = (DACHA_atQueue[u32DACIDX][u32QueueIDX] & 0x0f00) >> 8;
					pstDAC->DAT[u32QueueIDX].DATL = DACHA_atQueue[u32DACIDX][u32QueueIDX] & 0xff;
				}
				DACHA_au32QueueIDX[u32DACIDX] = 0;
			}
			
			/* Set the read pointer */
			pstDAC->C2 = (pstDAC->C2 & DAC_C2_DACBFUP_MASK) | DAC_C2_DACBFRP(DACHA_au32QueueIDX[u32DACIDX]);

		}
		u32ClockReq *= 2;
	}
#endif //BUILD_MK60	
}

void DACHA_vTerminate(puint32 const pu32Stat)
{
	
}


SYSAPI_tenSVCResult DACHA_vInitDACResource(IOAPI_tenEHIOResource enEHIOResource, DACAPI_tstDACCB* pstDACCB)
{
	tstDACModule* pstDAC = NULL;	
	SYSAPI_tenSVCResult enResult = SYSAPI_enBadResource;
	
	switch (enEHIOResource)
	{
#ifdef BUILD_MK60
		case EH_O_DAC1:
		{				
			pstDAC = DAC0;
			SIM_vSetReg32(SIM_SCGC2, SIM_SCGC2_DAC0_MASK);
			DACHA_u32PortClockRequested |= 1;			
			break;
		}
		case EH_O_DAC2:
		{
			pstDAC = DAC1;
			SIM_vSetReg32(SIM_SCGC2, SIM_SCGC2_DAC1_MASK);
			DACHA_u32PortClockRequested |= 2;					
			break;
		}
#endif //BUILD_MK60
		default:
			pstDAC = NULL;
			break;
	}
	
	if (NULL != pstDAC)
	{
		enResult = SYSAPI_enOK;
		
#ifdef BUILD_MK60
		pstDAC->C0 |= (DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK);									
		if (DACAPI_enSoftwareTrigger == pstDACCB->enTriggerType) pstDAC->C0 |= DAC_C0_DACTRGSEL_MASK;
		if (TRUE == pstDACCB->boWaterMarkEventEnable) pstDAC->C0 |= DAC_C0_DACBWIEN_MASK;
		if (TRUE == pstDACCB->boPointerTopEventEnable) pstDAC->C0 |= DAC_C0_DACBTIEN_MASK;
		if (TRUE == pstDACCB->boPointerBottomEventEnable) pstDAC->C0 |= DAC_C0_DACBBIEN_MASK;		
		pstDAC->C1 = DAC_C1_DACBFWM(pstDACCB->enWatermarkWords) | DAC_C1_DACBFEN_MASK;								
		pstDAC->C2 = DAC_C2_DACBFUP(pstDACCB->enQueueDepth);
#endif //BUILD_MK60
	}	
	
	return enResult;
}
			
void DACHA_vWriteDACQueue(IOAPI_tenEHIOResource enEHIOResource, DACAPI_ttOutputVoltage* pOutputVoltage)
{		
#ifdef BUILD_MK60
	uint32 u32QueueIDX;
	tstDACModule* pstDAC;
	uint32 u32Queue;
	pstDAC = (EH_O_DAC1 == enEHIOResource) ? DAC0 : DAC1;
	u32Queue = (EH_O_DAC1 == enEHIOResource) ? 0 : 1;
	
	for (u32QueueIDX = 0; u32QueueIDX <= (pstDAC->C2 & DAC_C2_DACBFUP_MASK); u32QueueIDX++)
	{
			DACHA_atQueue[u32Queue][u32QueueIDX] = *pOutputVoltage++;
	}	
#endif //BUILD_MK60
}
