/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Fuelling                                               */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for fuelling calculations      */
/*                     measurement                                            */
/* FILE NAME:          FUEL.c                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _FUEL_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "FUEL.h"

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
TEPMAPI_tstTimedKernelEvent FUEL_astTimedHoldKernelEvents[2];
SPREADAPI_ttSpreadIDX FUEL_tSpreadAfmTFIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadTAFRxIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadTAFRyIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadCrankingAirflowIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadInjResponseIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadInjShortOpeningIDX;
TABLEAPI_ttTableIDX FUEL_tTableCrankingAirflowIDX;
TABLEAPI_ttTableIDX FUEL_tTableAfmTFIDX;
TABLEAPI_ttTableIDX FUEL_tTableInjResponseIDX;
TABLEAPI_ttTableIDX FUEL_tTableInjShortOpeningIDX;
MAPSAPI_ttMapIDX FUEL_tMapTAFRIDX;
uint16 FUEL_u16InjResponse;
uint16 FUEL_u16CrankingAirflow;
uint32 FUEL_u32SensorStateBank2;
Bool FUEL_boCalculatePending;
Bool FUEL_bo720Injection;
uint32 u32SequenceIDX;


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static void FUEL_vTEPMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime);
static void FUEL_vCyclicCalculate(void);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void FUEL_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;	
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;	
	IOAPI_tenDriveStrength enDriveStrength;
		
	/* Both peak and hold have a switch on and switch off event per cycle */
	TEPMAPI_ttEventCount tEventCount = 2;

	FUEL_u32FuelPumpRunCount = 3000;
		
	/* Set injection time to Xms */
	FUEL_tTimeHoldUs[0] = 1000;
	FUEL_tTimeHoldUs[1] = 1000;
	FUEL_tTimeHoldUs[2] = 1000;
	FUEL_tTimeHoldUs[3] = 1000;
	FUEL_tTimeHoldUs[4] = 1000;
	FUEL_tTimeHoldUs[5] = 1000;
	FUEL_tTimeHoldUs[6] = 1000;
	FUEL_tTimeHoldUs[7] = 1000;
	
	FUEL_tTimeHold[0] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[0]);
	FUEL_tTimeHold[1] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[1]);
	FUEL_tTimeHold[2] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[2]);
	FUEL_tTimeHold[3] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[3]);
	FUEL_tTimeHold[4] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[4]);
	FUEL_tTimeHold[5] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[5]);
	FUEL_tTimeHold[6] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[6]);
	FUEL_tTimeHold[7] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[7]);
	
	/* Set injection phase to 360 degrees */
	FUEL_tStartHoldFraction[0] = (0x10000 * 2) / 720;		
	FUEL_tStartHoldFraction[1] = (0x10000 * 2) / 720;	
	FUEL_tStartHoldFraction[2] = (0x10000 * 2) / 720;	
	FUEL_tStartHoldFraction[3] = (0x10000 * 2) / 720;	
	FUEL_tStartHoldFraction[4] = (0x10000 * 2) / 720;	
	FUEL_tStartHoldFraction[5] = (0x10000 * 5) / 720;	
	FUEL_tStartHoldFraction[6] = (0x10000 * 5) / 720;	
	FUEL_tStartHoldFraction[7] = (0x10000 * 5) / 720;									

	/* Request and initialise Fuel Injector group A */
	if ((0xffff > USERCAL_stRAMCAL.au32InjectionSequence[0]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aFuelIOResource[0]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aFuelIOResource[0];     //FUEL_nInj1Output;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
		/* Initialise the TEPM channel Fuel Injector group A */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;	
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32InjectionSequence[0];
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}			
		
		/* Switch injector on at a fraction of global time */
		FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction[0];
		FUEL_astTimedHoldKernelEvents[0].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		/* Switch injector off at timer ms */
		FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold[0];	
		FUEL_astTimedHoldKernelEvents[1].pfEventCB = FUEL_vTEPMCallBack;
		FUEL_astTimedHoldKernelEvents[1].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);	
	}	
	
	/* Request and initialise Fuel Injector group B */
	if ((0xffff > USERCAL_stRAMCAL.au32InjectionSequence[1]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aFuelIOResource[1]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aFuelIOResource[1];     //FUEL_nInj2Output;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
		/* Initialise the TEPM channel Fuel Injector group B */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;	
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32InjectionSequence[1];		
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}	
	
		/* Switch injector on at a fraction of global time */
		FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction[1];
		FUEL_astTimedHoldKernelEvents[0].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		/* Switch injector off at timer ms */
		FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold[1];
		FUEL_astTimedHoldKernelEvents[1].pfEventCB = FUEL_vTEPMCallBack;	
		FUEL_astTimedHoldKernelEvents[1].enEHIOBitMirrorResource = EH_IO_Invalid;						
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);	
	}
		
	/* Request and initialise Fuel Injector group C */
	if ((0xffff > USERCAL_stRAMCAL.au32InjectionSequence[2]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aFuelIOResource[2]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aFuelIOResource[2];     //FUEL_nInj3Output;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
		/* Initialise the TEPM channel Fuel Injector group C */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;	
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32InjectionSequence[2];
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}		
	
		/* Switch injector on at a fraction of global time */
		FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction[2];
		FUEL_astTimedHoldKernelEvents[0].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		/* Switch injector off at timer ms */
		FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold[2];
		FUEL_astTimedHoldKernelEvents[1].pfEventCB = FUEL_vTEPMCallBack;	
		FUEL_astTimedHoldKernelEvents[1].enEHIOBitMirrorResource = EH_IO_Invalid;					
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);		

		/* Request and initialise Fuel Injector group D */
		enEHIOResource = USERCAL_stRAMCAL.aFuelIOResource[3];     //FUEL_nInj4Output;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);		
	}	
	
	/* Initialise the TEPM channel Fuel Injector group D */
	if ((0xffff > USERCAL_stRAMCAL.au32InjectionSequence[3]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aFuelIOResource[3]))
	{
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;	
			stTEPMChannelCB.boAsyncRequestEnable = TRUE;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32InjectionSequence[3];
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}					
	
		/* Switch injector on at a fraction of global time */
		FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction[3];
		FUEL_astTimedHoldKernelEvents[0].enEHIOBitMirrorResource = EH_IO_Invalid;
	
		/* Switch injector off at timer ms */
		FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
		FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold[3];
		FUEL_astTimedHoldKernelEvents[1].pfEventCB = FUEL_vTEPMCallBack;
		FUEL_astTimedHoldKernelEvents[1].enEHIOBitMirrorResource = EH_IO_Invalid;

		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);		
	}
					
	
	/* Request and initialise required Kernel managed spread for AfmTF */
	FUEL_tSpreadAfmTFIDX = SETUP_tSetupSpread((void*)&FUEL_nXAFMAxisRef, (void*)&USERCAL_stRAMCAL.aUserCURVEAfmTFSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for AfmTF */
	FUEL_tTableAfmTFIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserCURVEAfmTFTable, (void*)&AFM_tAirFlowAFMUg, TYPE_enUInt32, 17, FUEL_tSpreadAfmTFIDX, NULL);	
	
	/* Request and initialise required Kernel managed spread for TAFRx */
	FUEL_tSpreadTAFRxIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMFiltered, (void*)&USERCAL_stRAMCAL.aUserTAFRxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for TAFRy */
	FUEL_tSpreadTAFRyIDX = SETUP_tSetupSpread((void*)&MAP_tKiloPaFiltered, (void*)&USERCAL_stRAMCAL.aUserTAFRySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed map for TAFR */
	FUEL_tMapTAFRIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserMAPTAFR, (void*)&FUEL_u16TAFR, TYPE_enUInt16, 17, 17, FUEL_tSpreadTAFRxIDX, FUEL_tSpreadTAFRyIDX, NULL);	
	
	/* Request and initialise required Kernel managed spread for injector response */
	FUEL_tSpreadInjResponseIDX = SETUP_tSetupSpread((void*)&BVM_tBattVolts, (void*)&USERCAL_stRAMCAL.aUserInjResponseSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for injector response */
	FUEL_tTableInjResponseIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserInjResponseTable, (void*)&FUEL_u16InjResponse, TYPE_enUInt16, 17, FUEL_tSpreadInjResponseIDX, NULL);			

	/* Request and initialise required Kernel managed spread for cranking airflow */
	FUEL_tSpreadCrankingAirflowIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserCrankingAirflowSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for cranking airflow */
	FUEL_tTableCrankingAirflowIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserCrankingAirflowTable, (void*)&FUEL_u16CrankingAirflow, TYPE_enUInt16, 17, FUEL_tSpreadCrankingAirflowIDX, NULL);
	
	/* Request and initialise required Kernel managed spread for injector short opening */
	FUEL_tSpreadInjShortOpeningIDX = SETUP_tSetupSpread((void*)&FUEL_tTimePredictedUsInput, (void*)&USERCAL_stRAMCAL.aUserInjShortOpeningSpread, TYPE_enUInt32, 11, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for injector short opening */
	FUEL_tTableInjShortOpeningIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserInjShortOpeningTable, (void*)&FUEL_tTimePredictedShortOpeningUs, TYPE_enUInt32, 11, FUEL_tSpreadInjShortOpeningIDX, NULL);

	FUEL_bo720Injection = TRUE;

	for (u32SequenceIDX = 0; u32SequenceIDX < FUEL_nFuelSequenceCount; u32SequenceIDX++)
	{
		if ((USERCAL_stRAMCAL.au32InjectionSequence[u32SequenceIDX] & 0xff) == ((USERCAL_stRAMCAL.au32InjectionSequence[u32SequenceIDX] & 0xff00) >> 8))
		{
		    FUEL_bo720Injection = FALSE;
		}
	}

	CLO2_vInit();

	/* Set up fuel pump relay output */
	if (EH_IO_Invalid != USERCAL_stRAMCAL.enFuelPumpRelay)
	{
		if (EH_IO_IIC2_SDA > enEHIOResource)
		{
			enEHIOResource = USERCAL_stRAMCAL.enFuelPumpRelay;
			enEHIOType = IOAPI_enDIOOutput;
			enDriveStrength = IOAPI_enStrong;
		
			SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
		}
	}
}

void FUEL_vRun(puint32 const pu32Arg)
{
	static uint32 u32FuelRunCount = 0;
	uint16 u16WDTVal;
	IOAPI_tenTriState enTriState;
	IOAPI_tenEHIOResource enEHIOResource;
	RELAY_tenBit enBit;

	CLO2_vFilterSensors();
	
	if (0 == u32FuelRunCount % FUEL_nInjRespCalcRate)
	{
		/* Calculate the current spread for injector response */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadInjResponseIDX,
				NULL, NULL);		
		
		/* Lookup the current value for injector response */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableInjResponseIDX,
			NULL, NULL);		
			
		/* Calculate the current spread for TAFRx */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadTAFRxIDX,
		NULL, NULL);		
		
		/* Calculate the current spread for TAFRy */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadTAFRyIDX,
		NULL, NULL);			
		
		/* Lookup the current value for TAFR */
		USER_vSVC(SYSAPI_enCalculateMap, (void*)&FUEL_tMapTAFRIDX,
		NULL, NULL);			
			
	    /* Reset the WDT */
		USER_vSVC(SYSAPI_enResetWatchdog, &u16WDTVal, NULL, NULL);
	}
	
	if ((TRUE == FUEL_boCalculatePending)
		|| (FALSE == FUEL_boFuelPrimed))

	{
		FUEL_vCyclicCalculate();
		FUEL_u32FuelPumpRunCount = 2000;
	}

	if ((TRUE == CTS_boCTSReady)
		&& (FALSE == FUEL_boFuelPrimed))
	{
		if (0 == CAM_u32RPMRaw)
		{
			/* Prime the injectors */
			USER_vSVC(SYSAPI_enTEPMAsyncRequest, NULL, NULL, NULL);
			FUEL_boFuelPrimed = TRUE;
		}
		else
		{
			FUEL_boFuelPrimed = TRUE;
		}
	}
	else if ((2000 == u32FuelRunCount)
	&& (FALSE == FUEL_boFuelPrimed))
	{
		/* Finish prime */
		FUEL_boFuelPrimed = TRUE;
	}

	u32FuelRunCount++;

	if (EH_IO_Invalid != USERCAL_stRAMCAL.enFuelPumpRelay)
	{
		if (0 != FUEL_u32FuelPumpRunCount)
		{
			enEHIOResource = USERCAL_stRAMCAL.enFuelPumpRelay;
			enTriState = IOAPI_enHigh;

			if (EH_IO_IIC2_SDA > enEHIOResource)
			{
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
			else
			{
				enBit = 1 << (enEHIOResource - EH_IO_IIC2_SDA);
				RELAYS_vOutputBit(enBit, IOAPI_enHigh == enTriState);
			}
			
			FUEL_u32FuelPumpRunCount--;
		}
		else
		{
			enEHIOResource = USERCAL_stRAMCAL.enFuelPumpRelay;
			enTriState = IOAPI_enLow;

			if (EH_IO_IIC2_SDA > enEHIOResource)
			{
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
			else
			{
				enBit = 1 << (enEHIOResource - EH_IO_IIC2_SDA);
				RELAYS_vOutputBit(enBit, IOAPI_enHigh == enTriState);
			}
		}
	}
}

void FUEL_vCalculateFuellingValues()
{
	FUEL_boCalculatePending = TRUE;
}

static void FUEL_vCyclicCalculate(void)
{
	static Bool boAFMTransientControl;
	static Bool boFuelAFRCutoff;
	static uint32 u32OverrunCutCount;
	uint32 u32SeqIDX;
	uint8 u8SeqFuelBit;
	uint32 u32FuelCycleTimeUs;
	uint32 u32Temp = 0;
	static Bool boSequentialMode;
	static Bool boHighDuty;
	static uint32 u32SequentialModeCount;
	uint32 u32MaxFuel;
	uint32 u32ReturnlessPressureKpa;

	if ((FALSE == FUEL_boFuelPrimed) && (400 > CAM_u32RPMRaw))
	{
		FUEL_tTimePredictedUs[0] = CTS_u32Primer;
		FUEL_tTimePredictedUs[1] = CTS_u32Primer;
	}
	else
	{
		uint32 u32Temp1;
		uint32 u32Temp2;
		sint32 s32Temp;

		/* Calculate the current spread for AfmTF */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadAfmTFIDX,
				NULL, NULL);		
	
		/* Lookup the current AfmTF value for AfmTF */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableAfmTFIDX,
				NULL, NULL);	
				
		boAFMTransientControl = USERCAL_stRAMCAL.u32AFMTransientControlRPMLimit > CAM_u32RPMRaw ? TRUE : boAFMTransientControl;
		boAFMTransientControl = (USERCAL_stRAMCAL.u32AFMTransientControlRPMLimit + 100) < CAM_u32RPMRaw ? FALSE : boAFMTransientControl;										
				
		if ((USERCAL_stRAMCAL.u16RPMRunThreshold < CAM_u32RPMRaw) || (USERCAL_stRAMCAL.u8CrankingAirflowEnable == FALSE))
		{
			if ((TRUE == USERCAL_stRAMCAL.u8EnableBackupAirflowTransients) &&
			(TRUE == boAFMTransientControl))
			{
				if (AFM_tAirFlowAFMUg > AFM_tAirFlowBackupUg)
				{
					u32Temp1 = (AFM_tAirFlowBackupUg * 9) / 8;
					FUEL_tPredictedAirFlowUg = u32Temp > AFM_tAirFlowAFMUg ? AFM_tAirFlowAFMUg : u32Temp;
				}
				else
				{
					u32Temp1 = (AFM_tAirFlowBackupUg * 7) / 8;
					FUEL_tPredictedAirFlowUg = u32Temp > AFM_tAirFlowAFMUg ? u32Temp : AFM_tAirFlowAFMUg;
				}
			}
			else
			{
				if (USERCAL_stRAMCAL.u32SpeedDensityAFMTPSLim > TPS_tThetaFiltered)
				/* At small throttle angles */
				{
					if (TRUE == USERCAL_stRAMCAL.u8EnableAFMPrimaryInputClosed)
					{
						FUEL_tPredictedAirFlowUg = AFM_tAirFlowAFMUg;
					}
					else if ((FALSE == USERCAL_stRAMCAL.u8EnableAFMPrimaryInputClosed) &&
                             (TRUE == USERCAL_stRAMCAL.u8EnableAFMPrimaryInputOpen))
					{
						u32Temp1 = TPS_tThetaFiltered * (AFM_tAirFlowAFMUg / USERCAL_stRAMCAL.u32SpeedDensityAFMTPSLim);
						u32Temp2 = (USERCAL_stRAMCAL.u32SpeedDensityAFMTPSLim - TPS_tThetaFiltered) * (AFM_tAirFlowVEUg / USERCAL_stRAMCAL.u32SpeedDensityAFMTPSLim);

						FUEL_tPredictedAirFlowUg = u32Temp1 + u32Temp2;
					}
					else
					{
						FUEL_tPredictedAirFlowUg = AFM_tAirFlowVEUg;
					}
				}
				else
				{
					if (TRUE == USERCAL_stRAMCAL.u8EnableAFMPrimaryInputOpen)
					{
						FUEL_tPredictedAirFlowUg = AFM_tAirFlowAFMUg;
					}
					else
					{
						FUEL_tPredictedAirFlowUg = AFM_tAirFlowVEUg;
					}
				}
			}
		}
		else
		{
			/* Calculate the current spread for cranking airflow */
			USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadCrankingAirflowIDX,
			NULL, NULL);
		
			/* Lookup the current value for cranking airflow */
			USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableCrankingAirflowIDX,
			NULL, NULL);
			FUEL_tPredictedAirFlowUg = 1000 * FUEL_u16CrankingAirflow;
		}

		if (TRUE == FUEL_boFuelPrimed)
		{
			CLO2_vRun();
			IAC_vCallBack(NULL);
		}


		/* Add manifold capacitance transitory airflow */
		if (500000 < MAP_s32ManDeltaChargeMassPerSUg)
		{
			FUEL_tPredictedAirFlowUg += (uint32)MAP_s32ManDeltaChargeMassPerSUg;
		}
		else if ((-500000 > (sint32)MAP_s32ManDeltaChargeMassPerSUg) && ((sint32)FUEL_tPredictedAirFlowUg > -MAP_s32ManDeltaChargeMassPerSUg))
		{
			FUEL_tPredictedAirFlowUg += (uint32)MAP_s32ManDeltaChargeMassPerSUg;
		}
		
		FUEL_tPredictedFuelFlowUg = (10u * FUEL_tPredictedAirFlowUg) / FUEL_u16TAFR;
	
		/* Add CTS and ATS correction */
		FUEL_tPredictedFuelFlowUg = ((FUEL_tPredictedFuelFlowUg / 10UL) * (CTS_u32FuelMultiplier + ATS_u32FuelMultiplier - 1000)) / 100UL;	

		/* Add TPS moving enrichment */
		FUEL_tPredictedFuelFlowUg = ((FUEL_tPredictedFuelFlowUg / 10UL) * (TPS_u32MovingTPSEnrichment)) / 100UL;	

		/* Add TPS tip in enrichment */
		FUEL_tPredictedFuelFlowUg = ((FUEL_tPredictedFuelFlowUg / 10UL) * (TPS_u32TipInEnrichment)) / 100UL;

	
		s32Temp = (sint32)FUEL_tPredictedFuelFlowUg + FILM_s32FilmLoadUgDeltaApplied;

		if (0 <= s32Temp)
		{
			FUEL_tPredictedFuelFlowUg = (uint32)s32Temp;
		}
		else
		{
			FUEL_tPredictedFuelFlowUg = 0;
		}
		
		/* Add STT trim */
		FUEL_tPredictedFuelFlowPerInjectionNg[0] = (12 / USERCAL_stRAMCAL.u8InjDivide) * (100 * FUEL_tPredictedFuelFlowUg / CAM_u32RPMRaw);//was filtered!!
		FUEL_tPredictedFuelFlowPerInjectionNg[0] *= CLO2_u32STT[0];
		FUEL_tPredictedFuelFlowPerInjectionNg[1] = (12 / USERCAL_stRAMCAL.u8InjDivide) * (100 * FUEL_tPredictedFuelFlowUg / CAM_u32RPMRaw);//was filtered!!
		FUEL_tPredictedFuelFlowPerInjectionNg[1] *= CLO2_u32STT[1];

		/* Add fuel pressure trim */
		if (USERCAL_stRAMCAL.u8ReturnlessEnable == TRUE)
		{
			u32ReturnlessPressureKpa = 1000 * USERCAL_stRAMCAL.u16ReturnlessPressureKPa;
			u32Temp = MAP_tKiloPaFiltered + u32ReturnlessPressureKpa;
			u32Temp /= (101 + (u32ReturnlessPressureKpa / 1000));
			u32Temp = 1000 > u32Temp ? u32Temp : 999;
			u32Temp = USERMATH_u32GetSquareRoot(u32Temp);

			FUEL_tPredictedFuelFlowPerInjectionNg[0] /= 1000;
			FUEL_tPredictedFuelFlowPerInjectionNg[0] *= u32Temp;
			FUEL_tPredictedFuelFlowPerInjectionNg[1] /= 1000;
			FUEL_tPredictedFuelFlowPerInjectionNg[1] *= u32Temp;
		}


		u32Temp = (uint32)USERCAL_stRAMCAL.u8CylCount * (uint32)USERCAL_stRAMCAL.u16InjFlowRate;
		FUEL_tTimePredictedUs[0] = FUEL_tPredictedFuelFlowPerInjectionNg[0] / u32Temp;
		FUEL_tTimePredictedUs[1] = FUEL_tPredictedFuelFlowPerInjectionNg[1] / u32Temp;

		/* Trim short pulse */		
		if (USERCAL_stRAMCAL.u16MinLinearFuelPulse > FUEL_tTimePredictedUs[0])
		{
			FUEL_tTimePredictedUsInput = FUEL_tTimePredictedUs[0];
		    (void)BOOSTED_boIndexAndCalculateTable(FUEL_tSpreadInjShortOpeningIDX, FUEL_tTableInjShortOpeningIDX);

			/* Pragmatic workaround - predicted event < 1.6 ms */
			if (USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] > FUEL_tTimePredictedUs[0])
			{
				u32Temp = USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] - FUEL_tTimePredictedShortOpeningUs;
				u32Temp *= (BVM_tBattVolts - 4000);
				u32Temp /= 11000;
				FUEL_tTimePredictedShortOpeningUs = USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] - u32Temp;
			}

			FUEL_tTimePredictedUs[0] = FUEL_tTimePredictedShortOpeningUs;
		}	

		if (USERCAL_stRAMCAL.u16MinLinearFuelPulse > FUEL_tTimePredictedUs[1])
		{
			FUEL_tTimePredictedUsInput = FUEL_tTimePredictedUs[1];
			(void)BOOSTED_boIndexAndCalculateTable(FUEL_tSpreadInjShortOpeningIDX, FUEL_tTableInjShortOpeningIDX);

			/* Pragmatic workaround - predicted event < 1.6 ms */
			if (USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] > FUEL_tTimePredictedUs[1])
			{
				u32Temp = USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] - FUEL_tTimePredictedShortOpeningUs;
				u32Temp *= (BVM_tBattVolts - 4000);
				u32Temp /= 11000;
				FUEL_tTimePredictedShortOpeningUs = USERCAL_stRAMCAL.aUserInjShortOpeningTable[10] - u32Temp;
			}

			FUEL_tTimePredictedUs[1] = FUEL_tTimePredictedShortOpeningUs;
		}

		boFuelAFRCutoff = (FUEL_u16TAFR < USERCAL_stRAMCAL.u16AFRReinstate) ? FALSE : boFuelAFRCutoff;
		boFuelAFRCutoff = (FUEL_u16TAFR > USERCAL_stRAMCAL.u16AFRCutoff) ? TRUE : boFuelAFRCutoff;
	}	

	FUEL_tTimePredictedUs[0] = TRUE == boFuelAFRCutoff ? 0 : FUEL_tTimePredictedUs[0];
	FUEL_tTimePredictedUs[1] = TRUE == boFuelAFRCutoff ? 0 : FUEL_tTimePredictedUs[1];


	/* Manage overrun fuel cut */
#ifdef BUILD_BSP_VSS_CAN
	if ((TRUE == MAP_boHighVacuum) &&
		(TRUE == TPS_boThrottleClosed) &&
		(TRUE == IAC_boOverrunCutRPMEnable) &&
		(0 != SENSORS_u16CANVSS) &&
		(0xffff != SENSORS_u16CANVSS))
#else
	if ((TRUE == MAP_boHighVacuum) &&
	    (TRUE == TPS_boThrottleClosed) &&
	    (TRUE == IAC_boOverrunCutRPMEnable))
#endif
	{
		u32OverrunCutCount = 40 > u32OverrunCutCount ? u32OverrunCutCount + 1 : u32OverrunCutCount;

		for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
		{			
			FUEL_aboSeqOverrunCutFlag[u32SeqIDX] =	u32OverrunCutCount > (10 + (u32SeqIDX << 3)) ?	TRUE : FALSE;
		}
	}
	else if (TRUE == IAC_boOverrunCutRPMEnable)
	{		
		u32OverrunCutCount = 3 < u32OverrunCutCount ? u32OverrunCutCount - 4 : 0;

		for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
		{
			FUEL_aboSeqOverrunCutFlag[u32SeqIDX] =	u32OverrunCutCount > (10 + (u32SeqIDX << 3)) ?	TRUE : FALSE;
		}
	}
	else
	{
		u32OverrunCutCount = 0;

		for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
		{
			FUEL_aboSeqOverrunCutFlag[u32SeqIDX] =	FALSE;
		}
	}

	u8SeqFuelBit = 1;

	/* Apply overrun and over-speed fuel cuts */
	for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
	{
		if ((TRUE == FUEL_aboSeqRevCutFlag[u32SeqIDX]) ||
			(TRUE == FUEL_aboSeqOverrunCutFlag[u32SeqIDX]))
		{
			FUEL_tTimeHoldUs[u32SeqIDX] = 600;
			FUEL_tTimeHold[u32SeqIDX] = 400;
		}
		else
		{
			CPU_xEnterCritical();
			if (0 == (u8SeqFuelBit & USERCAL_stRAMCAL.u8SeqFuelAssign))
			{
				FUEL_tTimeHoldUs[u32SeqIDX] = FUEL_tTimePredictedUs[0] + (uint32)FUEL_u16InjResponse;
				FUEL_tTimeHoldUs[u32SeqIDX] = (uint32)((sint32)FUEL_tTimeHoldUs[u32SeqIDX] + USERCAL_stRAMCAL.s16SequenceFuelOffset[u32SeqIDX]);
				FUEL_tTimeHold[u32SeqIDX] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[u32SeqIDX]);
			}
			else
			{
				FUEL_tTimeHoldUs[u32SeqIDX] =  FUEL_tTimePredictedUs[1] + (uint32)FUEL_u16InjResponse;
				FUEL_tTimeHoldUs[u32SeqIDX] = (uint32)((sint32)FUEL_tTimeHoldUs[u32SeqIDX] + USERCAL_stRAMCAL.s16SequenceFuelOffset[u32SeqIDX]);
				FUEL_tTimeHold[u32SeqIDX] = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs[u32SeqIDX]);
			}	
			CPU_xExitCritical();		
		}
			
		u8SeqFuelBit *= 2;
	}
	
	FUEL_boCalculatePending = FALSE;

	u32MaxFuel = Max(FUEL_tTimeHoldUs[0], FUEL_tTimeHoldUs[1]);
	u32MaxFuel = Max(u32MaxFuel, FUEL_tTimeHoldUs[2]);
	u32MaxFuel = Max(u32MaxFuel, FUEL_tTimeHoldUs[3]);
	
	u32FuelCycleTimeUs = 120000000 / (CAM_u32RPMRaw * USERCAL_stRAMCAL.u8InjDivide);
	u32Temp = (0x10000 * u32MaxFuel) / u32FuelCycleTimeUs;


	/* Condition the high duty flag */
	boHighDuty = 40000 < u32Temp ? TRUE : boHighDuty;
	boHighDuty = 30000 > u32Temp ? FALSE : boHighDuty;


	/* Clear sequential counter */
	if (250 > CAM_u32RPMFiltered)
	{
		u32SequentialModeCount = 0;
	}

	/* Check sequential mode */
	if ((350 > CAM_u32RPMFiltered) || (true == boHighDuty) || (FALSE == FUEL_bo720Injection))
	{
		u32SequentialModeCount = 0 < u32SequentialModeCount ? u32SequentialModeCount - 1 : 0;
	}
	else
	{
		u32SequentialModeCount = FUEL_nSeqModeCountLimit > u32SequentialModeCount ? u32SequentialModeCount + 1 : FUEL_nSeqModeCountLimit;
	}


	/* Condition the sequential flag */
	boSequentialMode = FUEL_nSeqModeCountLimit <= u32SequentialModeCount ? TRUE : boSequentialMode;
	boSequentialMode = 0 == u32SequentialModeCount ? FALSE : boSequentialMode;


	if (TRUE == boSequentialMode)
	{
		if (49500 > u32Temp)
		{
			/* Assign the end of event angles */
			FUEL_tStartHoldFraction[0] = 50000 - u32Temp;
			FUEL_tStartHoldFraction[1] = 50000 - u32Temp;
			FUEL_tStartHoldFraction[2] = 50000 - u32Temp;
			FUEL_tStartHoldFraction[3] = 50000 - u32Temp;
		}
		else
		{
			/* Assign the end of event angles */
			FUEL_tStartHoldFraction[0] = 500;
			FUEL_tStartHoldFraction[1] = 500;
			FUEL_tStartHoldFraction[2] = 500;
			FUEL_tStartHoldFraction[3] = 500;
		}
	}
	else
	{
		/* Assign the end of event angles */
		FUEL_tStartHoldFraction[0] = 500;
		FUEL_tStartHoldFraction[1] = 500;
		FUEL_tStartHoldFraction[2] = 500;
		FUEL_tStartHoldFraction[3] = 500;
	}		
}


void FUEL_vTerminate(puint32 const pu32Arg)
{

}


void FUEL_vCallBack(puint32 const pu32Arg)
{

}

static void FUEL_vTEPMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	//FUEL_boFuelPrimed = TRUE;
}

#endif //BUILD_USER
