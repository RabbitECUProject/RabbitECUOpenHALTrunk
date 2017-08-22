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
	if ((CAM_nPeriodRPMTimeout / CAM_nPeriodMs) > CAM_u32RPMTimeout)
	{
		CAM_u32RPMTimeout++;
	}
	else
	{
		CAM_u32RPMRaw = 0;
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
	
	CAM_u32RPMRaw = CAM_xTicksToRPM(tEventTime);	
	FUEL_vCalculateFuellingValues();
	CAM_u32RPMTimeout = 0;
	
	if (1000000 < CTS_i32StartEnrichmentScaled)
	{
		u32Temp = 1000 * (CTS_i32StartEnrichmentScaled - 1000000);
		u32Temp /= 1020u;
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
}

#endif //BUILD_USER
