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
#include "DAC.h"

DACAPI_tpfEventCB DAC_apfEvent[DACHA_nDACCount];


void DAC_vStart(puint32 const pu32Stat)
{
	uint32 u32DACIDX;
	
	for (u32DACIDX = 0; u32DACIDX < DACHA_nDACCount; u32DACIDX++)
	{
	    DAC_apfEvent[u32DACIDX] = NULL;
	}
}

void DAC_vRun(puint32 const pu32Stat)
{
    DACHA_vRun(pu32Stat);
}

void DAC_vTerminate(puint32 const pu32Stat)
{
	
}

SYSAPI_tenSVCResult DAC_vInitDACResource(IOAPI_tenEHIOResource enEHIOResource, DACAPI_tstDACCB* pstDACCB)
{
    return DACHA_vInitDACResource(enEHIOResource, pstDACCB);
}
			
void DAC_vWriteDACQueue(IOAPI_tenEHIOResource enEHIOResource, DACAPI_ttOutputVoltage* pOutputVoltage)
{
	DACHA_vWriteDACQueue(enEHIOResource, pOutputVoltage);
}
