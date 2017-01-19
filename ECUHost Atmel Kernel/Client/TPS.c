/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Throttle Position Sensor                               */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for throttle position          */
/*                     measurement                                            */
/* FILE NAME:          TPS.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _TPS_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "TPS.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
bool TPS_boNewSample;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : TPS_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void TPS_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void TPS_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;
	
	TPS_u32ADCRaw = 0;
	TPS_tThetaRaw = 0;
	TPS_tThetaFiltered = 0;
	TPS_boNewSample = FALSE;
	
	enEHIOResource = TPS_nADInput;
	enEHIOType = IOAPI_enADSE;
	stADCCB.enSamplesAv = ADCAPI_en32Samples;
	stADCCB.pfResultCB = &TPS_vADCCallBack;
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

void TPS_vRun(puint32 const pu32Arg)
{
	uint32 u32Temp;
	uint32 u32TempLimit;
	
	if (TRUE == TPS_boNewSample)
	{
		/* Adding critical sections per code review but not actually needed yet */
		USER_xEnterCritical();/*CR1_16*/
		(void)USERMATH_u32SinglePoleLowPassFilter((uint16)TPS_u32ADCRaw, TPS_nADFiltVal,
			&TPS_u32ADCFiltered);
		TPS_boNewSample = FALSE;		
		USER_xExitCritical();/*CR1_16*/
		
		u32Temp = TPS_u32ADCFiltered * SENSORS_nADRefVolts;
		u32Temp /= SENSORS_nADScaleMax;
		u32Temp /= SENSORS_nVDivRatio;
		
		TPS_tSensorVolts = u32Temp;		
		
		u32Temp = MAX(USERCAL_stRAMCAL.userCalTPSCalMin, TPS_tSensorVolts);
		u32Temp = MIN(USERCAL_stRAMCAL.userCalTPSCalMax, u32Temp);				
		u32Temp -= USERCAL_stRAMCAL.userCalTPSCalMin;
		
		u32Temp *= TPS_nAngleRange;
		u32Temp /= (USERCAL_stRAMCAL.userCalTPSCalMax - 
								USERCAL_stRAMCAL.userCalTPSCalMin);
		
		TPS_tThetaRaw = u32Temp;
		
		/* User can add filtering */
		TPS_tThetaFiltered = TPS_tThetaRaw;		
	}
}

void TPS_vTerminate(puint32 const pu32Arg)
{

}

void TPS_vCallBack(puint32 const pu32Arg)
{

}

static void TPS_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	TPS_u32ADCRaw = u32ADCResult;
	TPS_boNewSample = TRUE;
}

#endif //BUILD_USER
