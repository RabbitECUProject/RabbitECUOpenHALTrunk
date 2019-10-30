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
#include "SENSORS.h"
#include "usercal.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
Bool TPS_boNewSample;
const uint32 TPS_au32AreaVolRatio[] = TPS_nAreaVolRatioData;
const uint32 TPS_au32AngleSpread[] = TPS_nAngleSpreadData;
SPREADAPI_ttSpreadIDX TPS_tSpreadAreaVolRatioIDX;
TABLEAPI_ttTableIDX TPS_tAreaVolRatioTableIDX;
SPREADAPI_ttSpreadIDX TPS_tSpreadTPSMovingIDX;
TABLEAPI_ttTableIDX TPS_tTPSMovingTableIDX;
SPREADAPI_ttSpreadIDX TPS_tSpreadTipInIDX;
TABLEAPI_ttTableIDX TPS_tTipInTableIDX;
uint32 TPS_u32AreaVolRatio;
uint32 TPS_u32TipInEnrichmentLookup;

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

	TPS_u32TipInEnrichment = 1000;
	
	enEHIOResource = USERCAL_stRAMCAL.u16TPSADResource;
	enEHIOType = IOAPI_enADSE;
	stADCCB.enSamplesAv = ADCAPI_en32Samples;
	stADCCB.pfResultCB = &TPS_vADCCallBack;
	stADCCB.enTrigger = ADCAPI_enTrigger4;				
		
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	/* Request and initialise required Kernel managed spread for area volume ratio */
	TPS_tSpreadAreaVolRatioIDX = SETUP_tSetupSpread((void*)&TPS_tThetaRaw, (void*)&TPS_au32AngleSpread,  TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for area volume ratio */
	TPS_tAreaVolRatioTableIDX = SETUP_tSetupTable((void*)&TPS_au32AreaVolRatio, (void*)&TPS_u32AreaVolRatio, TYPE_enUInt32, 17, TPS_tSpreadAreaVolRatioIDX, NULL);

	/* Request and initialise required Kernel managed spread for moving throttle enrichment  */
	TPS_tSpreadTPSMovingIDX = SETUP_tSetupSpread((void*)&TPS_tThetaDerivativeNormalised, (void*)&USERCAL_stRAMCAL.aUserTPSMovementCorrectionSpread,  TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for moving throttle enrichment */
	TPS_tTPSMovingTableIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserTPSMovementCorrectionTable, (void*)&TPS_u32MovingTPSEnrichment, TYPE_enUInt16, 17, TPS_tSpreadTPSMovingIDX, NULL);

	/* Request and initialise required Kernel managed spread for tip in throttle enrichment  */
	TPS_tSpreadTipInIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMFiltered, (void*)&USERCAL_stRAMCAL.aUserTipInCorrectionSpread,  TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for tip in throttle enrichment */
	TPS_tTipInTableIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserTipInCorrectionTable, (void*)&TPS_u32TipInEnrichmentLookup, TYPE_enUInt16, 17, TPS_tSpreadTipInIDX, NULL);

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
	sint16 s16Temp;
	sint32 s32Temp;
	static GPM6_ttTheta TPS_tThetaRawOld;
	static uint32 u32Count;
	static sint32 s32SecondDerivativeLimitNeg = 0;
	static sint32 s32SecondDerivativeLimitPos = 0;
	static uint16 s16OldCANTPS;

	if (TRUE == USERCAL_stRAMCAL.boTPSCANPrimary)
	{
		if (TRUE == SENSORS_boCANTPSNewSample)
		{
			if (s16OldCANTPS == SENSORS_u16CANTPS)
			{
				TPS_u16CANTPSDeltaNegCount = 1 > TPS_u16CANTPSDeltaNegCount ? 0 : TPS_u16CANTPSDeltaNegCount - 1;
				TPS_u16CANTPSDeltaPosCount = 1 > TPS_u16CANTPSDeltaPosCount ? 0 : TPS_u16CANTPSDeltaPosCount - 1;
			}
			else
			{
				s16Temp = SENSORS_u16CANTPS - s16OldCANTPS;

				if (0 > s16Temp)
				{
					s16Temp = ABS(s16Temp);
					s16Temp = 5 > s16Temp ? 0 : s16Temp - 5;
					s16Temp = 30 > s16Temp ? s16Temp : 30;
					s16Temp *= 10;
					TPS_u16CANTPSDeltaNegCount += s16Temp;
					TPS_u16CANTPSDeltaNegCount = Min(TPS_u16CANTPSDeltaNegCount, 200);
				}
				else
				{
					s16Temp = 5 > s16Temp ? 0 : s16Temp - 5;
					s16Temp = 30 > s16Temp ? s16Temp : 30;
					s16Temp *= 10;
					TPS_u16CANTPSDeltaPosCount += s16Temp;
					TPS_u16CANTPSDeltaPosCount = Min(TPS_u16CANTPSDeltaPosCount, 200);
				}
			}

			/* Calculate CAN based throttle angle */
			u32Temp = SENSORS_u16CANTPS;
			u32Temp = MAX(1750, u32Temp);
			u32Temp = MIN(2500, u32Temp);
			u32Temp -= 1750;
			
			u32Temp *= TPS_nAngleRange;
			u32Temp /= 750;

			TPS_tThetaRaw = u32Temp;
			
			/* User can add filtering */
			TPS_tThetaPreFiltered = USERMATH_u32SinglePoleLowPassFilter32(TPS_tThetaRaw, 0x80, &TPS_tThetaPreFiltered);

			TPS_tThetaFiltered = TPS_tThetaPreFiltered;

			s16OldCANTPS = SENSORS_u16CANTPS;
			SENSORS_boCANTPSNewSample = FALSE;
			SENSORS_vInvalidateCAN16Data(1, 2);
		}
	}
	else
	{
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
			TPS_tThetaPreFiltered = USERMATH_u32SinglePoleLowPassFilter32(TPS_tThetaRaw, 0x80, &TPS_tThetaPreFiltered);

			if (TPS_tThetaPreFiltered > TPS_tThetaFiltered)
			{
			/* New sample higher than pre-filter */
				if (s32SecondDerivativeLimitPos < (sint32)(TPS_tThetaPreFiltered - TPS_tThetaFiltered))
				{
					s32SecondDerivativeLimitPos = (sint32)TPS_nFirstDLimitPos > s32SecondDerivativeLimitPos ? s32SecondDerivativeLimitPos + TPS_nSecondDLimit : TPS_nFirstDLimitPos;

					if (s32SecondDerivativeLimitPos < (sint32)(TPS_tThetaPreFiltered - TPS_tThetaFiltered))
					{
						TPS_tThetaFiltered += s32SecondDerivativeLimitPos;
					}
					else
					{
						TPS_tThetaFiltered = TPS_tThetaPreFiltered;
					}
				}
				else
				{
					TPS_tThetaFiltered = TPS_tThetaPreFiltered;
				}

				s32SecondDerivativeLimitNeg = 0 > s32SecondDerivativeLimitNeg ? s32SecondDerivativeLimitNeg + TPS_nSecondDLimit : 0;
				s32SecondDerivativeLimitNeg = 0 > s32SecondDerivativeLimitNeg ? s32SecondDerivativeLimitNeg + TPS_nSecondDLimit : 0;
			}
			else if (TPS_tThetaPreFiltered < TPS_tThetaFiltered)
			/* New sample lower than pre-filter */
			{
				if (s32SecondDerivativeLimitNeg > (sint32)(TPS_tThetaPreFiltered - TPS_tThetaFiltered))
				{
					s32SecondDerivativeLimitNeg = (sint32)TPS_nFirstDLimitNeg < s32SecondDerivativeLimitNeg ? s32SecondDerivativeLimitNeg - (sint32)TPS_nSecondDLimit : (sint32)TPS_nFirstDLimitNeg;
					TPS_tThetaFiltered += s32SecondDerivativeLimitNeg;
				}
				else
				{
					TPS_tThetaFiltered = TPS_tThetaPreFiltered;
				}

				s32SecondDerivativeLimitPos = 0 < s32SecondDerivativeLimitPos ? s32SecondDerivativeLimitPos - TPS_nSecondDLimit : 0;
				s32SecondDerivativeLimitPos = 0 < s32SecondDerivativeLimitPos ? s32SecondDerivativeLimitPos - TPS_nSecondDLimit : 0;
			}
			else
			{
				s32SecondDerivativeLimitPos = 0 < s32SecondDerivativeLimitPos ? s32SecondDerivativeLimitPos - TPS_nSecondDLimit : 0;
				s32SecondDerivativeLimitNeg = 0 > s32SecondDerivativeLimitNeg ? s32SecondDerivativeLimitNeg + TPS_nSecondDLimit : 0;
				s32SecondDerivativeLimitPos = 0 < s32SecondDerivativeLimitPos ? s32SecondDerivativeLimitPos - TPS_nSecondDLimit : 0;
				s32SecondDerivativeLimitNeg = 0 > s32SecondDerivativeLimitNeg ? s32SecondDerivativeLimitNeg + TPS_nSecondDLimit : 0;
			}

			TPS_tThetaFiltered = (uint32)0x7fffffff < TPS_tThetaFiltered ? 0 : TPS_tThetaFiltered;


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
				s32Temp = (sint32)TPS_tThetaFiltered - (sint32)TPS_tThetaRawOld;
				TPS_tThetaDerivative = s32Temp * (sint32)TPS_nRunRate;
				TPS_tThetaDerivative /= (sint32)TPS_nDerivCalcRate;

				if ((150000 < TPS_tThetaDerivative) || (-150000 > TPS_tThetaDerivative))
				{
					TPS_u32ThrottleMovingCounter = (TPS_nRunRate * TPS_nThrottleMovingCounterDurationMs) / (1000 * TPS_nDerivCalcRate);
				}
				else
				{
					TPS_u32ThrottleMovingCounter = 0 < TPS_u32ThrottleMovingCounter ? TPS_u32ThrottleMovingCounter - 1 : 0;
				}

				TPS_tThetaRawOld = TPS_tThetaFiltered;
				TPS_tThetaDerivativeNormalised = 0x80000000 + TPS_tThetaDerivative;

				/* Calculate the current spread for TPS movement enrichment */
				USER_vSVC(SYSAPI_enCalculateSpread, (void*)&TPS_tSpreadTPSMovingIDX,
				NULL, NULL);
			
				/* Lookup the current spread for TPS movement enrichment */
				USER_vSVC(SYSAPI_enCalculateTable, (void*)&TPS_tTPSMovingTableIDX,
				NULL, NULL);
			}

			if ((500 < s32SecondDerivativeLimitPos) || (-5000 > s32SecondDerivativeLimitNeg))
			{
				TPS_u32ThrottleMovingCounter = (TPS_nRunRate * TPS_nThrottleMovingCounterDurationMs) / (1000 * TPS_nDerivCalcRate);
			}
			else
			{
				TPS_u32ThrottleMovingCounter = 0 < TPS_u32ThrottleMovingCounter ? TPS_u32ThrottleMovingCounter - 1 : 0;
			}
		}
	}


    if (TRUE == USERCAL_stRAMCAL.boPPSCANPrimary)
	{
		if (0xff != SENSORS_u8CANPPS)
		{
			TPS_boThrottleClosed = (0 == SENSORS_u8CANPPS) ? TRUE : TPS_boThrottleClosed;
			TPS_boThrottleClosed = (1 < SENSORS_u8CANPPS) ? FALSE : TPS_boThrottleClosed;
		}
		else
		{
			TPS_boThrottleClosed = USERCAL_stRAMCAL.u32TPSClosedLower > TPS_tThetaFiltered ? TRUE : TPS_boThrottleClosed;
			TPS_boThrottleClosed = USERCAL_stRAMCAL.u32TPSClosedUpper < TPS_tThetaFiltered ? FALSE : TPS_boThrottleClosed;
		}
	}
	else
	{
		if (USERCAL_stRAMCAL.u32TPSClosedLower > TPS_tThetaFiltered)
		{
			TPS_boThrottleClosed = TRUE;
			TPS_u32TipInEnrichment = 1000;
		}
		else if (USERCAL_stRAMCAL.u32TPSClosedUpper < TPS_tThetaFiltered)
		{
			if (TPS_boThrottleClosed == TRUE)
			{
				TPS_boThrottleClosed = FALSE;

				TPS_u32TransitionCounter = TPS_nTransitionCounterTipIn;

				/* Calculate the current spread for tip in enrichment */
				USER_vSVC(SYSAPI_enCalculateSpread, (void*)&TPS_tSpreadTipInIDX,
				NULL, NULL);
				
				/* Lookup the current value for tip in enrichment */
				USER_vSVC(SYSAPI_enCalculateTable, (void*)&TPS_tTipInTableIDX,
				NULL, NULL);

				if (TPS_u32TipInEnrichmentLookup > TPS_u32TipInEnrichment)
				{
					TPS_u32TipInEnrichment = TPS_u32TipInEnrichmentLookup;
				}
			}
		}
	}

	TPS_u32TransitionCounter = 0 < TPS_u32TransitionCounter ? TPS_u32TransitionCounter - 1 : 0;
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
