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
Bool TPS_boNewSample;
const uint32 TPS_au32AreaVolRatio[] = TPS_nAreaVolRatioData;
const uint32 TPS_au32AngleSpread[] = TPS_nAngleSpreadData;
SPREADAPI_ttSpreadIDX TPS_tSpreadAreaVolRatioIDX;
TABLEAPI_ttTableIDX TPS_tAreaVolRatioTableIDX;
uint32 TPS_u32AreaVolRatio;

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
	TPS_u32ManifoldVolumeTau = 0;
	TPS_u32AreaVolRatio = 0;
	TPS_boNewSample = FALSE;
	TPS_u32ThrottleMovingCounter = 0;
	
	enEHIOResource = TPS_nADInput;
	enEHIOType = IOAPI_enADSE;
	stADCCB.enSamplesAv = ADCAPI_en32Samples;
	stADCCB.pfResultCB = &TPS_vADCCallBack;
	stADCCB.enTrigger = ADCAPI_enTrigger4;				
		
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	/* Request and initialise required Kernel managed spread for area volume ratio */
	TPS_tSpreadAreaVolRatioIDX = SETUP_tSetupSpread((void*)&TPS_tThetaRaw, (void*)&TPS_au32AngleSpread,  TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for area volume ratio */
	TPS_tAreaVolRatioTableIDX = SETUP_tSetupTable((void*)&TPS_au32AreaVolRatio, (void*)&TPS_u32AreaVolRatio, TYPE_enUInt32, 17, TPS_tSpreadAreaVolRatioIDX, NULL);


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
	static GPM6_ttTheta TPS_tThetaRawOld;
	static uint32 u32Count;

	if (TRUE == TPS_boNewSample)
	{
		/* Adding critical sections per code review but not actually needed yet */
		USER_xEnterCritical();/*CR1_16*/
		(void)USERMATH_u16SinglePoleLowPassFilter16((uint16)TPS_u32ADCRaw, TPS_nADFiltVal,
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
		TPS_tThetaFiltered = USERMATH_u32SinglePoleLowPassFilter32(TPS_tThetaRaw, 0x10, &TPS_tThetaFiltered);

	    /* Calculate the current spread for area volume ratio */
	    USER_vSVC(SYSAPI_enCalculateSpread, (void*)&TPS_tSpreadAreaVolRatioIDX,
	        NULL, NULL);
	
	    /* Lookup the current table value for area volume ratio */
	    USER_vSVC(SYSAPI_enCalculateTable, (void*)&TPS_tAreaVolRatioTableIDX,
	        NULL, NULL);

		u32Temp = USERCAL_stRAMCAL.u8ManifoldTimeConstantTrim * TPS_u32AreaVolRatio;
		//u32Temp = 255 * TPS_u32AreaVolRatio;
		TPS_u32ManifoldVolumeTau = u32Temp / 0x1000;

		/* Calculate throttle derivative */
		if (0 == (u32Count++ % TPS_nDerivCalcRate))
		{
		    if (0 <= (TPS_tThetaRaw - TPS_tThetaRawOld))
			{
		        u32Temp = (TPS_nRunRate / TPS_nDerivCalcRate) * (TPS_tThetaRaw - TPS_tThetaRawOld) + 0x80000000;
			}
		    else
			{
		        u32Temp = 0x80000000 - (TPS_nRunRate / TPS_nDerivCalcRate) * (TPS_tThetaRawOld - TPS_tThetaRaw);
			}

		    TPS_tThetaDerivative = USERMATH_u32SinglePoleLowPassFilter32(u32Temp, 0x40, &TPS_tThetaDerivative);
			TPS_tThetaRawOld = TPS_tThetaRaw;

			if ((0x80040000 < TPS_tThetaDerivative) || (0x7ffc0000 > TPS_tThetaDerivative))
			{
			   TPS_u32ThrottleMovingCounter = (TPS_nRunRate * TPS_nThrottleMovingCounterDurationMs) / (1000 * TPS_nDerivCalcRate);
			}
			else
			{
			    TPS_u32ThrottleMovingCounter = 0 < TPS_u32ThrottleMovingCounter ? TPS_u32ThrottleMovingCounter - 1 : 0;
			}
		}
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
