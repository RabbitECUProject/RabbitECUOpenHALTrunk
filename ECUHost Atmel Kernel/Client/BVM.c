/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Battery Voltage                                        */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for battery voltage            */
/*                     measurement                                            */
/* FILE NAME:          BVM.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#define _BVM_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "BVM.h"

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 BVM_u32ADCRaw;
uint32 BVM_u32ADCFiltered;
uint32 BVM_u32ADCPreFiltered;
Bool BVM_boNewSample;


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : BVM_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void BVM_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);/*CR1_11*//*CR1_10*/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void BVM_vStart(uint32 * const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;
	
	BVM_u32ADCFiltered = BVM_nVoltsFilteredInitVal;/*CR1_9*/
	BVM_u32ADCRaw = 0;/*CR1_9*//*CR1_15*/
	
	enEHIOResource = BVM_nADInput;
	enEHIOType = IOAPI_enADSE;
	stADCCB.enSamplesAv = ADCAPI_en32Samples;
	stADCCB.pfResultCB = &BVM_vADCCallBack;
	stADCCB.enTrigger = ADCAPI_enTrigger2;				
		
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);/*CR1_12*/

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

void BVM_vRun(uint32* const pu32Arg)
{
	static sint32 s32SecondDerivativeLimitNeg = 0;
	static sint32 s32SecondDerivativeLimitPos = 0;
	
	if (TRUE == BVM_boNewSample)
	{	
		/* User can add filtering */
		BVM_u32ADCPreFiltered = USERMATH_u32SinglePoleLowPassFilter32(BVM_u32ADCRaw, 0x80, &BVM_u32ADCPreFiltered);

		if (BVM_u32ADCPreFiltered > BVM_u32ADCFiltered)
		{
		/* New sample higher than pre-filter */
			if (s32SecondDerivativeLimitPos < (sint32)(BVM_u32ADCPreFiltered - BVM_u32ADCFiltered))
			{
				s32SecondDerivativeLimitPos += 2;
				BVM_u32ADCFiltered += s32SecondDerivativeLimitPos;
			}
			else
			{
				BVM_u32ADCFiltered = BVM_u32ADCPreFiltered;
			}

			s32SecondDerivativeLimitNeg = 0 > s32SecondDerivativeLimitNeg ? s32SecondDerivativeLimitNeg + 2 : 0;
		}
		else if (BVM_u32ADCPreFiltered < BVM_u32ADCFiltered)
		/* New sample lower than pre-filter */
		{
			if (s32SecondDerivativeLimitNeg > (sint32)(BVM_u32ADCPreFiltered - BVM_u32ADCFiltered))
			{
				s32SecondDerivativeLimitNeg -= 2;
				BVM_u32ADCFiltered += s32SecondDerivativeLimitNeg;
			}
			else
			{
				BVM_u32ADCPreFiltered = BVM_u32ADCPreFiltered;
			}

			s32SecondDerivativeLimitPos = 0 < s32SecondDerivativeLimitPos ? s32SecondDerivativeLimitPos - 2 : 0;
		}
		else
		{
			s32SecondDerivativeLimitPos = 0 < s32SecondDerivativeLimitPos ? s32SecondDerivativeLimitPos - 2 : 0;
			s32SecondDerivativeLimitNeg = 0 > s32SecondDerivativeLimitNeg ? s32SecondDerivativeLimitNeg + 2 : 0;
		}

		BVM_u32ADCFiltered = (uint32)0x7fffffff < BVM_u32ADCFiltered ? 0 : BVM_u32ADCFiltered;
	
		BVM_tBattVolts = CONV_tADCToVolts(EH_IO_ADD11, BVM_u32ADCFiltered);	
		BVM_boNewSample = FALSE;
	}
}

void BVM_vTerminate(uint32* const pu32Arg)
{

}

void BVM_vCallBack(uint32* const pu32Arg)
{

}

static void BVM_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	BVM_u32ADCRaw = u32ADCResult;
	BVM_boNewSample = TRUE;
}

#endif //BUILD_USER




