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
	static uint32 u32RunCounter;
	
	if (0 == (u32RunCounter++ % 0x10))
	{	
		USER_xEnterCritical();/*CR1_16*/
		(void)USERMATH_u16SinglePoleLowPassFilter16((uint16)BVM_u32ADCRaw, 0x40,
			&BVM_u32ADCFiltered);
		USER_xExitCritical();/*CR1_16*/
	
		BVM_tBattVolts = CONV_tADCToVolts(EH_IO_ADD11, BVM_u32ADCFiltered);	
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
}

#endif //BUILD_USER




