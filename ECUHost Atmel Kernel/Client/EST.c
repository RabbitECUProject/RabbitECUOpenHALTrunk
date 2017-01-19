/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Electronic Spark Timing Header File                    */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for electronic spark timing    */
/*                                                                            */
/* FILE NAME:          EST.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _EST_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "EST.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
TEPMAPI_tstTimedKernelEvent EST_astTimedKernelEvents[2];
SPREADAPI_ttSpreadIDX EST_tSpreadTimingxIDX;
SPREADAPI_ttSpreadIDX EST_tSpreadTimingyIDX;
SPREADAPI_ttSpreadIDX EST_tSpreadDwellIDX;
TABLEAPI_ttTableIDX EST_tTableTimingxIDX;
TABLEAPI_ttTableIDX EST_tTableTimingyIDX;
TABLEAPI_ttTableIDX EST_tTableDwellIDX;
uint16 EST_u16TimingX;
uint16 EST_u16TimingY;
uint16 EST_u16Dwell;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void EST_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;	
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;	
	TEPMAPI_tstTEPMResourceCB stTEPMResourceCB;

	uint16 u16TriggerArray[2] = {0, 0xaaaa};
	//uint16 u16TriggerArray[2] = {0, 0x8000};
	uint8 u8EdgeCount = 2;
	bool boFirstRising = TRUE;
	
	/* Both peak and hold have a switch on and switch off event per cycle */
	TEPMAPI_ttEventCount tEventCount = 2;
	EST_tIgnitionAdvanceMtheta = 0;
		
	/* Set dwell to 10 degrees - 170 degrees */
	EST_tStartFraction = (0x10000 * 120) / 180;	
	EST_tDwellFraction = (0x10000 * 50) / 180;		

	/* Request and initialise PWM for EST_nESTOutput*/
	enEHIOResource = EH_VIO_TC2;
	enEHIOType = IOAPI_enTEPM;
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* ONLY CONFIGURE THE TC2 MODULE ONCE PER PROJECT! */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		stTEPMResourceCB.enEHIOResource = EH_VIO_TC2;
		stTEPMResourceCB.enPreScalar = 1 << EST_nSlowFTMDivisor;
		stTEPMResourceCB.enCountType = TEPMAPI_enCountUp;
		
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMResourceCB);
	}
	
	/* Request and initialise EST_nESTOutput */
	enEHIOResource = EST_nESTOutput;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);			
	
	/* Initialise the TEPM channel EST_nESTOutput the dwell timer*/
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
		stTEPMChannelCB.boInterruptEnable = TRUE;	
		stTEPMChannelCB.enPreScalar =  0;
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}			
		
	/* Switch injector on at a fraction of global time */
	EST_astTimedKernelEvents[0].enAction = TEPMAPI_enSetHigh;
	EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
	EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFraction;
	
	/* Switch injector off at timer ms */
	EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
	EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enGlobalLinkedFractionStep;
	EST_astTimedKernelEvents[1].ptEventTime = &EST_tDwellFraction;	
	
	USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
		(void*)&EST_astTimedKernelEvents[0], (void*)&tEventCount);	
	
	/* Request and initialise required Kernel managed spread for Timingx */
	EST_tSpreadTimingxIDX = SETUP_tSetupSpread((void*)&MAP_tKiloPaFiltered, (void*)&USERCAL_stRAMCAL.aUserTimingxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Timingx*/
	EST_tTableTimingxIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserTimingxTable, (void*)&EST_u16TimingX, TYPE_enUInt16, 17, EST_tSpreadTimingxIDX, NULL);	
	
	/* Request and initialise required Kernel managed spread for Timingy */
	EST_tSpreadTimingyIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserTimingySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Timingy*/
	EST_tTableTimingyIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserTimingyTable, (void*)&EST_u16TimingY, TYPE_enUInt16, 17, EST_tSpreadTimingyIDX, NULL);		

	/* Request and initialise required Kernel managed spread for Dwell */
	EST_tSpreadDwellIDX = SETUP_tSetupSpread((void*)&BVM_tBattVolts, (void*)&USERCAL_stRAMCAL.aUserDwellSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Dwell*/
	EST_tTableDwellIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserDwellTable, (void*)&EST_u16Dwell, TYPE_enUInt16, 17, EST_tSpreadDwellIDX, NULL);

	USER_vSVC(SYSAPI_enSetupCrankTriggerEdgePattern, (void*)&u16TriggerArray, (void*)&boFirstRising, (void*)&u8EdgeCount);
}

void EST_vRun(puint32 const pu32Arg)
{
	uint32 u32DwellDegrees;		
	uint32 u32DelayDegrees;	
	uint32 u32DwellUsMax;
	uint32 u32DwellUsMin;
	uint32 u32DwellUs;
	uint32 u32Temp;
	uint16 u16Temp;
	static Bool boLowRpmFlag;
	
	/* Calculate the current spread for Timingx */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadTimingxIDX,
			NULL, NULL);		
	
	/* Lookup the current timing value for Timingx */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&EST_tTableTimingxIDX,
		NULL, NULL);	

	/* Calculate the current spread for Timingy */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadTimingyIDX,
			NULL, NULL);			

	/* Lookup the current timingy value for Timingy */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&EST_tTableTimingyIDX,
		NULL, NULL);
		
	/* Calculate the current spread for dwell */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadDwellIDX,
	NULL, NULL);

	/* Lookup the current timingy value for dwell */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&EST_tTableDwellIDX,
	NULL, NULL);			
	
	EST_tIgnitionAdvanceMtheta = 100 * (EST_u16TimingX + EST_u16TimingY);

	u32DwellUsMax = (30000000 / CAM_u32RPMRaw) - EST_nDwellOffMinUs;
	u32DwellUs = MIN(u32DwellUsMax, (uint32)EST_u16Dwell);
			
	if (1300 > CAM_u32RPMRaw)
	{		
	    boLowRpmFlag = TRUE;		
	}
	else if (1500 < CAM_u32RPMRaw)
	{
	    boLowRpmFlag = FALSE;	
	}

	if (TRUE == boLowRpmFlag)
	{
		u32DwellUsMin = 10000 - CAM_u32RPMRaw * 5;
		u32DwellUs = MAX(u32DwellUsMin, u32DwellUs);
	}

	u32Temp = 30000000 / CAM_u32RPMRaw;
	u32DwellDegrees = (u32DwellUs * 180) / u32Temp;	
		
	u32DelayDegrees = 180 - u32DwellDegrees - (EST_tIgnitionAdvanceMtheta / 1000);
	
	/* Calculate EST start and end angles */
	EST_tStartFraction = (0x10000 * u32DelayDegrees) / 180;	
	EST_tDwellFraction = (0x10000 * u32DwellDegrees) / 180;	
}

void EST_vTerminate(puint32 const pu32Arg)
{

}


void EST_vCallBack(puint32 const pu32Arg)
{

}

#endif //BUILD_USER
