/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Cyclic Activity Manager                                */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for cyclic activites           */
/*                                                                            */
/* FILE NAME:          CAM.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _CAM_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "FUEL.h"
#include "CAM.h"
#include "CTS.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 CAM_u32RPMTimeout;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void CAM_vStart(puint32 const pu32Arg)
{
	CAM_u32RPMTimeout = 0;
}

void CAM_vRun(puint32 const pu32Arg)
{	
	uint32_t u32SeqIDX;

	if ((CAM_nPeriodRPMTimeout / CAM_nPeriodMs) > CAM_u32RPMTimeout)
	{
		CAM_u32RPMTimeout++;
	}
	else
	{
		CAM_u32RPMRaw = 0;
		CAM_u32RPMRawOld = 0;
	}

	for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
	{
		if (USERCAL_stRAMCAL.au16SeqRPMLimit[u32SeqIDX] < (uint16)CAM_u32RPMRaw)
		{
			FUEL_aboSeqRevCutFlag[u32SeqIDX] = TRUE;
		}
		else if ((USERCAL_stRAMCAL.au16SeqRPMLimit[u32SeqIDX] - USERCAL_stRAMCAL.u16SeqRPMLimitHyst) > (uint16)CAM_u32RPMRaw)
		{
			FUEL_aboSeqRevCutFlag[u32SeqIDX] = FALSE;
		}
	}
}

void CAM_vTerminate(puint32 const pu32Arg)
{

}


void CAM_vCallBack(puint32 const pu32Arg)
{

}

void CAM_vEngineSpeedCB(TEPMAPI_ttEventTime tEventTime)
{
	uint32 u32Temp;
	sint32 s32Temp;
	
	CAM_u32RPMRawOld = CAM_u32RPMRaw;
	CAM_u32RPMRaw = CAM_xTicksToRPM(tEventTime);
	
	s32Temp = CAM_u32RPMRaw - CAM_u32RPMFiltered;
	s32Temp = ABS(s32Temp);

	if (150 > s32Temp)
	{
		if (TRUE == TPS_boThrottleClosed)
		{
			CAM_u32RPMFiltered = (CAM_u32RPMRaw + 7 * CAM_u32RPMFiltered) / 8;
		}
		else
		{
			CAM_u32RPMFiltered = (CAM_u32RPMRaw + CAM_u32RPMFiltered) / 2;
			
		}

		CAM_u32RPMTransitionCounter = 0 < CAM_u32RPMTransitionCounter ? CAM_u32RPMTransitionCounter - 1 : 0;
	}
	else
	{
		CAM_u32RPMFiltered = (CAM_u32RPMRaw + CAM_u32RPMFiltered) / 2;
		CAM_u32RPMTransitionCounter = CAM_nRPMTransitionCount;
	}

	FUEL_vCalculateFuellingValues();
	CAM_u32RPMTimeout = 0;
	
	if (1000000 < CTS_i32StartEnrichmentScaled)
	{
		u32Temp = 1000 * (CTS_i32StartEnrichmentScaled - 1000000);
		u32Temp /= 1050u;
		u32Temp += 1000000;
		CTS_i32StartEnrichmentScaled = u32Temp;		
	}
	
	if (1000000 < CTS_i32PostStartEnrichmentScaled)
	{
		u32Temp = 1000 * (CTS_i32PostStartEnrichmentScaled - 1000000);
		u32Temp /= 1003u;
		u32Temp += 1000000;
		CTS_i32PostStartEnrichmentScaled = u32Temp;		
	}

	if (TPS_u32TipInEnrichment > 1000)
	{
		TPS_u32TipInEnrichment -= 1;
	}
	else
	{
		TPS_u32TipInEnrichment = 1000;
	}
}

#endif //BUILD_USER
