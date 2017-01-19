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
TEPMAPI_tstTimedKernelEvent FUEL_astTimedPeakKernelEvents[2];
TEPMAPI_tstTimedKernelEvent FUEL_astTimedHoldKernelEvents[2];
SPREADAPI_ttSpreadIDX FUEL_tSpreadMAPVExIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadMAPVEyIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadInjResponseIDX;
SPREADAPI_ttSpreadIDX FUEL_tSpreadInjPeakTimeIDX;
TABLEAPI_ttTableIDX FUEL_tTableMAPVExIDX;
TABLEAPI_ttTableIDX FUEL_tTableMAPVEyIDX;
TABLEAPI_ttTableIDX FUEL_tTableInjResponseIDX;
TABLEAPI_ttTableIDX FUEL_tTableInjPeakTimeIDX;
uint16 FUEL_u16VEX;
uint16 FUEL_u16VEY;
uint16 FUEL_u16InjResponse;
uint16 FUEL_u16InjPeakTime;
bool FUEL_boCalculatePending;

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
	FUEL_tTimePeakUs = 3000;
	
	FUEL_tTimeHold = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs);
	FUEL_tTimePeak = FUEL_xUsToSlowTicks(FUEL_tTimePeakUs);
	
	/* Set injection phase to 10 degrees */
	FUEL_tStartPeakFraction = (0x10000 * 10) / 180;	
	FUEL_tStartHoldFraction = (0x10000 * 10) / 180;			

	/* Request and initialise FUEL_nHoldOutput */
	enEHIOResource = FUEL_nHoldOutput;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
	/* Initialise the TEPM channel FUEL_nHoldOutput the hold timer*/
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
	
	
	/* Request and initialise FUEL_nPeakOutput the peak timer*/
	enEHIOResource = FUEL_nPeakOutput;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
	/* Initialise the TEPM channel FUEL_nPeakOutput */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
		stTEPMChannelCB.boInterruptEnable = TRUE;	
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}			
		
	/* Switch injector on at a fraction of global time */
	FUEL_astTimedPeakKernelEvents[0].enAction = TEPMAPI_enSetHigh;
	FUEL_astTimedPeakKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
	FUEL_astTimedPeakKernelEvents[0].ptEventTime = &FUEL_tStartPeakFraction;
	
	/* Switch injector off at timer ms */
	FUEL_astTimedPeakKernelEvents[1].enAction = TEPMAPI_enSetLow;
	FUEL_astTimedPeakKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
	FUEL_astTimedPeakKernelEvents[1].ptEventTime = &FUEL_tTimePeak;	
	
	USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
		(void*)&FUEL_astTimedPeakKernelEvents[0], (void*)&tEventCount);		
	
	/* Request and initialise required Kernel managed spread for VEx */
	FUEL_tSpreadMAPVExIDX = SETUP_tSetupSpread((void*)&MAP_tKiloPaFiltered, (void*)&USERCAL_stRAMCAL.aUserMAPVExSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for VEx */
	FUEL_tTableMAPVExIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserMAPVExTable, (void*)&FUEL_u16VEX, TYPE_enUInt16, 17, FUEL_tSpreadMAPVExIDX, NULL);	
	
	/* Request and initialise required Kernel managed spread for VEy */
	FUEL_tSpreadMAPVEyIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserMAPVEySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for VEy */
	FUEL_tTableMAPVEyIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserMAPVEyTable, (void*)&FUEL_u16VEY, TYPE_enUInt16, 17, FUEL_tSpreadMAPVEyIDX, NULL);	
	
	/* Request and initialise required Kernel managed spread for injector response */
	FUEL_tSpreadInjResponseIDX = SETUP_tSetupSpread((void*)&BVM_tBattVolts, (void*)&USERCAL_stRAMCAL.aUserInjResponseSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for injector response */
	FUEL_tTableInjResponseIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserInjResponseTable, (void*)&FUEL_u16InjResponse, TYPE_enUInt16, 17, FUEL_tSpreadInjResponseIDX, NULL);		
	
	/* Request and initialise required Kernel managed spread for injector peak time */
	FUEL_tSpreadInjPeakTimeIDX = SETUP_tSetupSpread((void*)&BVM_tBattVolts, (void*)&USERCAL_stRAMCAL.aUserInjPeakTimeSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for injector peak time  */
	FUEL_tTableInjPeakTimeIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserInjPeakTimeTable, (void*)&FUEL_u16InjPeakTime, TYPE_enUInt16, 17, FUEL_tSpreadInjPeakTimeIDX, NULL);		
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
		
		/* Calculate the current spread for injector peak time */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadInjPeakTimeIDX,
				NULL, NULL);		
		
		/* Lookup the current value for injector peak time */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableInjPeakTimeIDX,
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
	uint32 u32AFR;
	uint32 u32Hz = CAM_u32RPMRaw * 2;	
	
	/* Calculate the current spread for VEX */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadMAPVExIDX,
			NULL, NULL);		
	
	/* Lookup the current VEx value for VEX */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableMAPVExIDX,
		NULL, NULL);	

	/* Calculate the current spread for VEY */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&FUEL_tSpreadMAPVEyIDX,
			NULL, NULL);			

	/* Lookup the current VEy value for VEY */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&FUEL_tTableMAPVEyIDX,
		NULL, NULL);	
	
	if (60000 > TPS_tThetaFiltered)
	{
		u32AFR = 1470;
	}
	else
	{
		u32AFR = 1400 - ((TPS_tThetaFiltered - 60000) / 150);		
	}
	
	FUEL_tMaxAirFlow = (FUEL_nEngineCap * CAM_u32RPMRaw) / 120u;
	u32Temp = ((MAP_tKiloPaFiltered / 10) * FUEL_tMaxAirFlow) / 10000u;
	u32Temp = FUEL_u16VEX * u32Temp;
	FUEL_tPredictedAirFlowMcc = FUEL_u16VEY * (u32Temp / 1000u);
	FUEL_tPredictedAirFlowUg = (FUEL_tPredictedAirFlowMcc * 119) / 100;
	
	if (0 < MAP_s32ManDeltaChargeMassPerSUg)
	/* Add manifold capacitance transitory airflow */
	{
		FUEL_tPredictedAirFlowUg += (uint32)MAP_s32ManDeltaChargeMassPerSUg;
	}	
	
	FUEL_tPredictedFuelFlowUg = (100 * FUEL_tPredictedAirFlowUg) / u32AFR;
	
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
		
	FUEL_tPredictedFuelFlowPerInjectionNg = 100 * ((10 * FUEL_tPredictedFuelFlowUg) / u32Hz);
	FUEL_tTimePredictedUs = 100 * FUEL_tPredictedFuelFlowPerInjectionNg / 6200;
	
	FUEL_tTimeHoldUs = FUEL_tTimePredictedUs + (uint32)FUEL_u16InjResponse;

	FUEL_tTimePredictedUs = MAX(FUEL_tTimePredictedUs, 1100u);	
	FUEL_tTimePeakUs = (uint32)FUEL_u16InjPeakTime;	
	
	FUEL_tTimeHold = FUEL_xUsToSlowTicks(FUEL_tTimeHoldUs);
	FUEL_tTimePeak = FUEL_xUsToSlowTicks(FUEL_tTimePeakUs);	

	FUEL_tTimeHold = FUEL_tTimeHold > FUEL_tTimePeak ? FUEL_tTimeHold : FUEL_tTimePeak;
	
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
