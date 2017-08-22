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
TABLEAPI_ttTableIDX FUEL_tTableCrankingAirflowIDX;
TABLEAPI_ttTableIDX FUEL_tTableAfmTFIDX;
TABLEAPI_ttTableIDX FUEL_tTableInjResponseIDX;
MAPSAPI_ttMapIDX FUEL_tMapTAFRIDX;
uint16 FUEL_u16TAFR;
uint16 FUEL_u16InjResponse;
uint16 FUEL_u16CrankingAirflow;
Bool FUEL_boCalculatePending;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static void FUEL_vTEPMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime);
static void FUEL_vLookup(void);
static void FUEL_vCyclicCalculate(void);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void FUEL_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;	
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;	
	TEPMAPI_tstTEPMResourceCB stTEPMResourceCB;
		
	/* Both peak and hold have a switch on and switch off event per cycle */
	TEPMAPI_ttEventCount tEventCount = 2;
		
	/* Set injection time to Xms */
	FUEL_tTimeHoldUs = 1000;
	
	FUEL_tTimeHold = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs);
	
	/* Set injection phase to 6 degrees */
	FUEL_tStartHoldFraction = (0x10000 * 6) / 360;			

	/* Request and initialise Fuel Injector 1 */
	enEHIOResource = FUEL_nInj1Output;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
	/* Initialise the TEPM channel Fuel Injector 1 */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
		stTEPMChannelCB.boInterruptEnable = TRUE;	
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}			
		
	/* Switch injector on at a fraction of global time */
	FUEL_astTimedHoldKernelEvents[0].enAction = TEPMAPI_enSetHigh;
	FUEL_astTimedHoldKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
	FUEL_astTimedHoldKernelEvents[0].ptEventTime = &FUEL_tStartHoldFraction;
	
	/* Switch injector off at timer ms */
	FUEL_astTimedHoldKernelEvents[1].enAction = TEPMAPI_enSetLow;
	FUEL_astTimedHoldKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
	FUEL_astTimedHoldKernelEvents[1].ptEventTime = &FUEL_tTimeHold;	
	
	USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
		(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);		
	
	/* Request and initialise Fuel Injector 2 */
	enEHIOResource = FUEL_nInj2Output;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
	/* Initialise the TEPM channel Fuel Injector 2 */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
		stTEPMChannelCB.boInterruptEnable = TRUE;	
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}					
	
	USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
		(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);	
		
	/* Request and initialise Fuel Injector 3 */
	enEHIOResource = FUEL_nInj3Output;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
	/* Initialise the TEPM channel Fuel Injector 3 */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
		stTEPMChannelCB.boInterruptEnable = TRUE;	
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}					
	
	USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
		(void*)&FUEL_astTimedHoldKernelEvents[0], (void*)&tEventCount);				
	
	/* Request and initialise required Kernel managed spread for AfmTF */
	FUEL_tSpreadAfmTFIDX = SETUP_tSetupSpread((void*)&AFM_tSensorVolts, (void*)&USERCAL_stRAMCAL.aUserCURVEAfmTFSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for AfmTF */
	FUEL_tTableAfmTFIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserCURVEAfmTFTable, (void*)&AFM_tAirFlowUg, TYPE_enUInt32, 17, FUEL_tSpreadAfmTFIDX, NULL);	
	
	/* Request and initialise required Kernel managed spread for TAFRx */
	FUEL_tSpreadTAFRxIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserTAFRxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for TAFRy */
	FUEL_tSpreadTAFRyIDX = SETUP_tSetupSpread((void*)&TPS_tThetaFiltered, (void*)&USERCAL_stRAMCAL.aUserTAFRySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

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
	

}

void FUEL_vRun(puint32 const pu32Arg)
{
	static uint32 u32Count;
	uint16 u16WDTVal;
	
	if (0 == u32Count % FUEL_nInjRespCalcRate)
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
	
	if (TRUE == FUEL_boCalculatePending)
	{
		FUEL_vCyclicCalculate();
	}
}

void FUEL_vCalculateFuellingValues()
{
	FUEL_boCalculatePending = TRUE;
}

static void FUEL_vCyclicCalculate(void)
{
	uint32 u32Temp;	
	sint32 s32Temp;
	
	/* Calculate the current spread for AfmTF */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadAfmTFIDX,
			NULL, NULL);		
	
	/* Lookup the current AfmTF value for AfmTF */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableAfmTFIDX,
		NULL, NULL);					
				
	if (400 < CAM_u32RPMRaw)
	{
		if (TRUE == USERCAL_stRAMCAL.u8EnableBackupAirflowTransients)
		{
			if (AFM_tAirFlowUg > AFM_tAirFlowBackupUg)
			{
				u32Temp = (AFM_tAirFlowBackupUg * 17) / 16;
				FUEL_tPredictedAirFlowUg = u32Temp > AFM_tAirFlowUg ? AFM_tAirFlowUg : u32Temp;
			}
			else
			{
				u32Temp = (AFM_tAirFlowBackupUg * 15) / 16;
				FUEL_tPredictedAirFlowUg = u32Temp > AFM_tAirFlowUg ? u32Temp : AFM_tAirFlowUg;
			}
		}
		else
		{
			FUEL_tPredictedAirFlowUg = AFM_tAirFlowUg;
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
	

	/* Add manifold capacitance transitory airflow */
	if (500000 < MAP_s32ManDeltaChargeMassPerSUg)
	{
		FUEL_tPredictedAirFlowUg += (uint32)MAP_s32ManDeltaChargeMassPerSUg;
	}
	else if ((-500000 > MAP_s32ManDeltaChargeMassPerSUg) && (FUEL_tPredictedAirFlowUg > -MAP_s32ManDeltaChargeMassPerSUg))
	{
		FUEL_tPredictedAirFlowUg += (uint32)MAP_s32ManDeltaChargeMassPerSUg;
	}
		
	FUEL_tPredictedFuelFlowUg = (10u * FUEL_tPredictedAirFlowUg) / FUEL_u16TAFR;
	
	/* Add CTS correction */
	FUEL_tPredictedFuelFlowUg = ((FUEL_tPredictedFuelFlowUg / 10) * CTS_u32FuelMultiplier) / 100;	
	
	s32Temp = (sint32)FUEL_tPredictedFuelFlowUg + FILM_s32FilmLoadUgDeltaApplied;

	if (0 <= s32Temp)
	{
	    FUEL_tPredictedFuelFlowUg = (uint32)s32Temp;
	}
	else
	{
	    FUEL_tPredictedFuelFlowUg = 0;
	}
		
	FUEL_tPredictedFuelFlowPerInjectionNg = 6000 * (100 * FUEL_tPredictedFuelFlowUg / CAM_u32RPMRaw);
	u32Temp = (uint32)USERCAL_stRAMCAL.u8CylCount * (uint32)USERCAL_stRAMCAL.u16InjFlowRate;
	FUEL_tTimePredictedUs = FUEL_tPredictedFuelFlowPerInjectionNg / u32Temp;
	
	FUEL_tTimeHoldUs = FUEL_tTimePredictedUs + (uint32)FUEL_u16InjResponse;
	FUEL_tTimeHold = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs);
	
	FUEL_boCalculatePending = FALSE;	
}

void FUEL_vTerminate(puint32 const pu32Arg)
{

}


void FUEL_vCallBack(puint32 const pu32Arg)
{

}

static void FUEL_vTEPMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	
	
	
}

#endif //BUILD_USER
