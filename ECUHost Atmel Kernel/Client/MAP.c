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
	MAP_boNewSample = FALSE;
	
	MAP_tManChargeMassUg = 0;
	MAP_s32ManDeltaChargeMassPerSUg = 0;	
	
	enEHIOResource = MAP_nADInput;
	enEHIOType = IOAPI_enADSE;
	stADCCB.enSamplesAv = ADCAPI_en32Samples;
	stADCCB.pfResultCB = &MAP_vADCCallBack;
	stADCCB.enTrigger = ADCAPI_enTrigger4;				
		
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{											
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stADCCB);	

		if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) 
		{
			*pu32Arg |= (uint32)SYSAPI_enResourceRequestFailed;/*CR1_13*/
		}			
	}		
	else
	{	
		*pu32Arg |= (uint32)SYSAPI_enResourceInitFailed;/*CR1_13*/
	}
}

void MAP_vRun(puint32 const pu32Arg)
{
	uint32 u32Temp;
	sint32 s32Temp;
	static uint32 u32SampleCount;
	
	if (TRUE == MAP_boNewSample)
	{
		USER_xEnterCritical();/*CR1_16*/
		(void)USERMATH_u32SinglePoleLowPassFilter((uint16)MAP_u32ADCRaw, MAP_nADFiltVal,
			&MAP_u32ADCFiltered);
		MAP_boNewSample = FALSE;		
		USER_xExitCritical();/*CR1_16*/
		
		u32Temp = MAP_u32ADCFiltered * SENSORS_nADRefVolts;
		u32Temp /= SENSORS_nADScaleMax;
		u32Temp /= SENSORS_nVDivRatio;
		
		MAP_tSensorVolts = u32Temp;
		
		u32Temp = MAP_nSensorGain * MAP_tSensorVolts;
		u32Temp /= 1000;
		u32Temp += MAP_nSensorOffset;
		MAP_tKiloPaRaw = u32Temp;
		
		/* User can add filtering */
		MAP_tKiloPaFiltered = MAP_tKiloPaRaw;
		
		if (0 == (u32SampleCount % 5))
		{
			u32Temp = MAP_nAirDensMgpL * USERCAL_stRAMCAL.u16ManifoldVolumeCC;
			u32Temp /= 1000;
			u32Temp *= MAP_tKiloPaFiltered;
			
			MAP_tManChargeMassUg = u32Temp / 100;	
			
			s32Temp = (sint32)MAP_tManChargeMassUg - (sint32)MAP_tManChargeMassOldUg;
			s32Temp *= 200;
			
			if (0 <= s32Temp)
			{
				MAP_s32ManDeltaChargeMassPerSUg = (10000000 < s32Temp) ? s32Temp : 0;
			}
			else
			{
				MAP_s32ManDeltaChargeMassPerSUg = (-10000000 > s32Temp) ? s32Temp : 0;
			}
				
			MAP_tManChargeMassOldUg = MAP_tManChargeMassUg;
		}	

		u32SampleCount++;
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
	MAP_u32ADCRaw = u32ADCResult;
	MAP_boNewSample = TRUE;
}

#endif //BUILD_USER
