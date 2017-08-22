/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Manifold Pressure Sensor                               */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for manifold pressure          */
/*                     measurement                                            */
/* FILE NAME:          MAP.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _MAP_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "MAP.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
bool MAP_boNewSample;
EXTERN GPM6_ttUg MAP_tManChargeMassOldUg;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : MAP_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void MAP_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void MAP_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;
	
	MAP_tKiloPaFiltered = MAP_nVoltsFilteredInitVal;
	MAP_tKiloPaRaw = 0;
	
	MAP_tManChargeMassUg = 0;
	MAP_s32ManDeltaChargeMassPerSUg = 0;		
}

void MAP_vRun(puint32 const pu32Arg)
{
    static GPM6_ttPa MAP_tKiloPaOld = 0;
	static uint32 u32Count;
	sint32 s32DeltaMAP;
	sint32 s32DeltaManifoldMass;

	MAP_tKiloPaRaw = 0;

	MAP_tKiloPaFiltered = USERMATH_u32SinglePoleLowPassFilter32(MAP_tKiloPaRaw, 0x10, &MAP_tKiloPaFiltered);

	if (0 == (u32Count % 5))
	{
		s32DeltaMAP = MAP_tKiloPaFiltered - MAP_tKiloPaOld;
		s32DeltaManifoldMass = (USERCAL_stRAMCAL.u16ManifoldVolumeCC * MAP_nAirDensMgpL) / 1000; 
		s32DeltaManifoldMass *= s32DeltaMAP;
	   MAP_s32ManDeltaChargeMassPerSUg = (MAP_nRunFreq * -s32DeltaManifoldMass) / 10000;	
       MAP_tKiloPaOld = MAP_tKiloPaFiltered;
	}
}

void MAP_vTerminate(puint32 const pu32Arg)
{

}


void MAP_vCallBack(puint32 const pu32Arg)
{

}


static void MAP_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{

}

#endif //BUILD_USER
