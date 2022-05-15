/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Air Temperature                                        */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for air temperature            */
/*                     measurement                                            */
/* FILE NAME:          ATS.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _ATS_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "ATS.h"
#include "CTS.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
bool ATS_boNewSample;
SPREADAPI_ttSpreadIDX ATS_tSpreadSensorIDX;
SPREADAPI_ttSpreadIDX ATS_tSpreadEnrichmentIDX;
TABLEAPI_ttTableIDX ATS_tTableSensorIDX;
TABLEAPI_ttTableIDX ATS_tTableEnrichmentIDX;

sint32 ATS_i32Enrichment;
uint32 ATS_u32SampleCount;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : ATS_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void ATS_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void ATS_vStart(puint32 const pu32Arg)
{
	ATS_u32ADCRaw = 0;	
	ATS_u32FuelMultiplier = 1000;
	ATS_u32SampleCount = 0;
	ATS_boNewSample = FALSE;
	ATS_tTempCRaw = 20000;
	ATS_tTempCFiltered = 20000;
	
	if (FALSE == USERCAL_stRAMCAL.boATSCANPrimary)	
	{
		/* Request and initialise the ATS ADC input channel */
		SETUP_boSetupADSE(USERCAL_stRAMCAL.u16ATSADResource, IOAPI_enADSE, ADCAPI_en32Samples, &ATS_vADCCallBack, ADCAPI_enTrigger2, pu32Arg);
	}
	
	/* Request and initialise required Kernel managed spread for air sensor */
	ATS_tSpreadSensorIDX = SETUP_tSetupSpread((void*)&ATS_tSensorVolts, (void*)&USERCAL_stRAMCAL.aUserAirSensorSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for air sensor */
	ATS_tTableSensorIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserAirSensorTable, (void*)&ATS_tTempCRaw, TYPE_enInt32, 17, ATS_tSpreadSensorIDX, NULL);		
	
	/* Request and initialise required Kernel managed spread for air temp enrichment */
	ATS_tSpreadEnrichmentIDX = SETUP_tSetupSpread((void*)&ATS_tTempCPort, (void*)&USERCAL_stRAMCAL.aUserAirTempCorrectionSpread, TYPE_enInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for air temp enrichment */
	ATS_tTableEnrichmentIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserAirTempCorrectionTable, (void*)&ATS_i32Enrichment, TYPE_enUInt16, 17, ATS_tSpreadEnrichmentIDX, NULL);		
}

void ATS_vRun(puint32 const pu32Arg)
{
	uint32 u32Temp;
	sint32 s32Temp;
#ifdef BUILD_ATS_PULLUP_SENSE
	volatile static uint32 u32Delta;
	static uint32 u32LowImpModeCount;
	static uint32 u32HighImpModeCount;
	static bool boHotMode;
	static bool boColdMode;
#endif
	static uint32 u32CANATSMsgCount = 0;
	static uint8 u8CANATSOld = 0;
	static uint32 u32RunCount;

	
	if ((TRUE == ATS_boNewSample) || 
		((true == SENSORS_boCANATSNewSample) && (true == USERCAL_stRAMCAL.boATSCANPrimary)))
	{
		ATS_u32SampleCount++;

		if (FALSE == USERCAL_stRAMCAL.boATSCANPrimary)	
		{
			USER_xEnterCritical();/*CR1_16*/
			(void)USERMATH_u16SinglePoleLowPassFilter16((uint16)ATS_u32ADCRaw, ATS_nADFiltVal,
					&ATS_u32ADCFiltered);
			ATS_boNewSample = FALSE;		
			USER_xExitCritical();/*CR1_16*/		

#ifdef BUILD_ATS_PULLUP_SENSE
			if ((FALSE == SENSORS_boATSACBiasHigh) && ((0x100 * ATS_u32ADCRaw) > ATS_u32ADCFiltered))
			{
				u32Temp = 0x100 * ATS_u32ADCRaw - ATS_u32ADCFiltered;
				u32Delta = (u32Temp + 3 * u32Delta)  / 4;
			}
			else if ((TRUE == SENSORS_boATSACBiasHigh) && ((0x100 * ATS_u32ADCRaw) < ATS_u32ADCFiltered))
			{
				u32Temp = ATS_u32ADCFiltered - 0x100 * ATS_u32ADCRaw;
				u32Delta = (u32Temp + 3 * u32Delta)  / 4;
			}			

			if (0 == CAM_u32RPMRaw)
			{
				if (10000 < u32Delta)
				{
					u32HighImpModeCount = 40 > u32HighImpModeCount ? u32HighImpModeCount + 1 : u32HighImpModeCount;
					u32LowImpModeCount = 2 < u32LowImpModeCount ? u32LowImpModeCount - 2 : 0;
				}

				if (7000 > u32Delta)
				{
					u32LowImpModeCount = 40 > u32LowImpModeCount ? u32LowImpModeCount + 1 : u32LowImpModeCount;
					u32HighImpModeCount = 2 < u32HighImpModeCount ? u32HighImpModeCount - 2 : 0;
				}

				boColdMode = 35 < u32HighImpModeCount;
				boHotMode = 35 < u32LowImpModeCount;
			}
			else 
			{
				if (15000 < u32Delta)
				{
					u32HighImpModeCount = 1000 > u32HighImpModeCount ? u32HighImpModeCount + 1 : u32HighImpModeCount;
					u32LowImpModeCount = 2 < u32LowImpModeCount ? u32LowImpModeCount - 2 : 0;
				}

				if (10000 > u32Delta)
				{
					u32LowImpModeCount = 1000 > u32LowImpModeCount ? u32LowImpModeCount + 1 : u32LowImpModeCount;
					u32HighImpModeCount = 2 < u32HighImpModeCount ? u32HighImpModeCount - 2 : 0;
				}

				boColdMode = 500 < u32HighImpModeCount;
				boHotMode = 500 < u32LowImpModeCount;
			}


			if (((FALSE == boHotMode) && (FALSE == boColdMode)) ||
				((TRUE == boHotMode) && (TRUE == boColdMode)))
			{
				return;
			}

#endif //BUILD_BSP_ATS_PULLUP_SENSE	

			u32Temp = ATS_u32ADCFiltered * SENSORS_nADRefVolts;
			u32Temp /= SENSORS_nADScaleMax;
			u32Temp /= SENSORS_nVDivRatio;		
			ATS_tSensorVolts = u32Temp;		
		
			/* Calculate the current spread for air temp sensor */
			USER_vSVC(SYSAPI_enCalculateSpread, (void*)&ATS_tSpreadSensorIDX,
					NULL, NULL);		
		
			/* Lookup the current spread for air temp sensor */
			USER_vSVC(SYSAPI_enCalculateTable, (void*)&ATS_tTableSensorIDX,
				NULL, NULL);
		}
		else
		{
			if (0xff != SENSORS_u8CANATS)
			{
				ATS_tTempCRaw = 1000 * (sint32)(SENSORS_u8CANATS - 40);
				SENSORS_boCANATSNewSample = FALSE;

				u32CANATSMsgCount++;

				if ((3 < u32CANATSMsgCount) && 
				   (u8CANATSOld == SENSORS_u8CANATS) &&
				   (0 != SENSORS_u8CANATS))
				{
					ATS_boATSReady = TRUE;
				}
				else if (10 < u32CANATSMsgCount)
				{
					ATS_boATSReady = TRUE;
				}
			}
			else
			{
				ATS_tTempCRaw = 0;
			}

			u8CANATSOld = SENSORS_u8CANATS;
		}


		if ((0 == CAM_u32RPMRaw) || (TRUE == USERCAL_stRAMCAL.boATSCANPrimary))
		{
			ATS_tTempCFiltered = ATS_tTempCRaw;	
		}
		else
		{
			if (100000 < ATS_tTempCRaw)
			{
				ATS_tTempCFiltered = ATS_tTempCRaw;	
			}
			else
			{
				if (ATS_tTempCRaw < ATS_tTempCFiltered)
				{
					if (1000 < (ATS_tTempCFiltered - ATS_tTempCRaw))
					{
						ATS_tTempCFiltered -= 1000;						
					}
					else
					{
						ATS_tTempCFiltered = ATS_tTempCRaw;
					}
				}
				else if (ATS_tTempCRaw > ATS_tTempCFiltered)
				{
					if (1000 < (ATS_tTempCRaw - ATS_tTempCFiltered))
					{
						ATS_tTempCFiltered += 1000;
					}
					else
					{
						ATS_tTempCFiltered = ATS_tTempCRaw;
					}
				}				
			}
		}
	}

	if (0 == (u32RunCount % 0x20))
	{
		u32Temp = 110000 > MAP_tKiloPaFiltered ? MAP_tKiloPaFiltered : 110000;
		s32Temp = ((110 - ((sint32)u32Temp / 1000u)) * CTS_tTempCFiltered) + (((sint32)u32Temp / 1000u) * ATS_tTempCFiltered);		
		ATS_tTempCPort = s32Temp / 110;


		/* Calculate the current spread for air temp enrichment */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&ATS_tSpreadEnrichmentIDX,
				NULL, NULL);		
		
		/* Lookup the current spread for air temp enrichment */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&ATS_tTableEnrichmentIDX,
			NULL, NULL);										
		
		s32Temp = ATS_i32Enrichment;	
		ATS_u32FuelMultiplier = (uint32)s32Temp;
	}

	u32RunCount++;
}

void ATS_vTerminate(puint32 const pu32Arg)
{

}

void ATS_vCallBack(puint32 const pu32Arg)
{

}

static void __attribute__((used)) ATS_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	ATS_u32ADCRaw = u32ADCResult;
	ATS_boNewSample = TRUE;
}


#endif //BUILD_USER