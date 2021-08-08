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
#include "CEMAPI.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
TEPMAPI_tstTimedKernelEvent EST_astTimedKernelEvents[2];
SPREADAPI_ttSpreadIDX EST_tSpreadTimingxIDX;
SPREADAPI_ttSpreadIDX EST_tSpreadTimingyIDX;
SPREADAPI_ttSpreadIDX EST_tSpreadDwellIDX;
TABLEAPI_ttTableIDX EST_tMapTimingIDX;
TABLEAPI_ttTableIDX EST_tTableDwellIDX;
SPREADAPI_ttSpreadIDX EST_tSpreadCTSTimingTrimIDX;
TABLEAPI_ttTableIDX EST_tCTSTimingTrimIDX;
SPREADAPI_ttSpreadIDX EST_tSpreadATSTimingTrimIDX;
TABLEAPI_ttTableIDX EST_tATSTimingTrimIDX;
	
	
uint16 EST_u16Timing;
uint16 EST_u16Dwell;
sint16 EST_s16CTSTimingTrim;
sint16 EST_s16ATSTimingTrim;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void EST_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;	
	IOAPI_tenDriveStrength enDriveStrength;
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;	
	Bool boCamSyncLate;
	TEPMAPI_tstTimedUserEvent stTimedEvent;
	uint32 u32CamSyncSampleToothCount;
	CEM_tstPatternSetupCB stPatternSetupCB;

	IOAPI_tenEdgePolarity enEdgePolarity = USERCAL_stRAMCAL.u8UserPrimaryEdgeSetup;
	Bool boFirstRising = 0 != USERCAL_stRAMCAL.u8UserFirstEdgeRisingPrimary;
	uint32 u32TriggerType = USERCAL_stRAMCAL.u8TriggerType;
	
	/* Both peak and hold have a switch on and switch off event per cycle */
	TEPMAPI_ttEventCount tEventCount = 2;
	EST_tIgnitionAdvanceMtheta = 0;
		
	/* Set dwell to 10 degrees - 360 degrees */
	EST_tStartFractionA = (0x10000 * 120) / EST_nDegreesPerCycle;
	EST_tStartFractionB = (0x10000 * 120) / EST_nDegreesPerCycle;	
	EST_tStartFractionC = (0x10000 * 120) / EST_nDegreesPerCycle;	
	EST_tStartFractionD = (0x10000 * 120) / EST_nDegreesPerCycle;				
	EST_tDwellUs = 5000u;		
	
	stPatternSetupCB.enEdgePolarity = enEdgePolarity;
	stPatternSetupCB.u32TriggerType = u32TriggerType;
	stPatternSetupCB.boFirstEdgeRising = boFirstRising;

	USER_vSVC(SYSAPI_enSetupCrankTriggerEdgePattern, (void*)&USERCAL_stRAMCAL.aUserPrimaryTriggerTable[0], (void*)&stPatternSetupCB, NULL);
	USER_vSVC(SYSAPI_enSetupSyncPointsPattern, (void*)&USERCAL_stRAMCAL.aUserSyncPointsTable[0], (void*)&USERCAL_stRAMCAL.u32SyncPhaseRepeats, NULL);

	/* Configure the missing tooth interrupt channel */
	USER_vSVC(SYSAPI_enConfigureMissingToothInterrupt, (void*)NULL, (void*)NULL, (void*)NULL);
	/* Request and initialise EST_nESTOutput A ***************************/
	if ((0xffff > USERCAL_stRAMCAL.au32IgnitionSequence[0]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aESTIOResource[0]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aESTIOResource[0];     //EST_nESTOutputA;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	
			(void*)NULL, (void*)NULL);			
	
		/* Initialise the TEPM channel EST_nESTOutput the dwell timer*/
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;	
			stTEPMChannelCB.enPreScalar =  0;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32IgnitionSequence[0];
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}			
		
		/* Switch igniter on at a fraction of global time */
		EST_astTimedKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionA;
		EST_astTimedKernelEvents[0].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[0];
		EST_astTimedKernelEvents[0].boToothScheduled = TRUE;
		
		if (0 < USERCAL_stRAMCAL.boEDISEnable)
		{
			EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedTimeStep;			
		}
		else
		{
			EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		}
		
		EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionA;
		EST_astTimedKernelEvents[0].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[0];
	
		/* Switch igniter off at timer ms */
		if (0 != USERCAL_stRAMCAL.u8ScheduleToothIgnition)
		{
			/* Switch igniter off at timer ms */
			EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
			EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enGlobalLinkedFraction;
			EST_astTimedKernelEvents[1].ptEventTime = &EST_tEndFractionA;
			EST_astTimedKernelEvents[1].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[0];
			EST_astTimedKernelEvents[1].boToothScheduled = TRUE;
		}
		else
		{
		EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
		EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		EST_astTimedKernelEvents[1].ptEventTime = &EST_tDwellUs;	
		EST_astTimedKernelEvents[1].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[0];
		}
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&EST_astTimedKernelEvents[0], (void*)&tEventCount);	
	}

	/* Request and initialise EST_nESTOutput B ***************************/
	if ((0xffff > USERCAL_stRAMCAL.au32IgnitionSequence[1]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aESTIOResource[1]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aESTIOResource[1];     //EST_nESTOutputB;
		enEHIOType = IOAPI_enCaptureCompare;	
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	
			(void*)NULL, (void*)NULL);			
	
		/* Initialise the TEPM channel EST_nESTOutput the dwell timer*/
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
		{	
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;	
			stTEPMChannelCB.enPreScalar = 0;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32IgnitionSequence[1];
	
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}			
		
		/* Switch igniter on at a fraction of global time */
		EST_astTimedKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionB;
		EST_astTimedKernelEvents[0].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[1];
		EST_astTimedKernelEvents[0].boToothScheduled = TRUE;
	
		if (0 < USERCAL_stRAMCAL.boEDISEnable)
		{
			EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedTimeStep;
		}
		else
		{
			EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		}
		
	
		/* Switch igniter off at timer ms */
		if (0 != USERCAL_stRAMCAL.u8ScheduleToothIgnition)
		{
			/* Switch igniter off at timer ms */
			EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
			EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enGlobalLinkedFraction;
			EST_astTimedKernelEvents[1].ptEventTime = &EST_tEndFractionB;
			EST_astTimedKernelEvents[1].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[1];
			EST_astTimedKernelEvents[1].boToothScheduled = TRUE;
		}
		else
		{
		EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
		EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		EST_astTimedKernelEvents[1].ptEventTime = &EST_tDwellUs;	
		EST_astTimedKernelEvents[1].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[1];
		}
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
			(void*)&EST_astTimedKernelEvents[0], (void*)&tEventCount);	
	}

	/* Request and initialise EST_nESTOutput C ***************************/
	if ((0xffff > USERCAL_stRAMCAL.au32IgnitionSequence[2]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aESTIOResource[2]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aESTIOResource[2];     //EST_nESTOutputC;
		enEHIOType = IOAPI_enCaptureCompare;
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	
			(void*)NULL, (void*)NULL);
	
		/* Initialise the TEPM channel EST_nESTOutput the dwell timer*/
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;
			stTEPMChannelCB.enPreScalar = 0;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32IgnitionSequence[2];
		
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}
	
		/* Switch igniter on at a fraction of global time */
		
		EST_astTimedKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionC;
		EST_astTimedKernelEvents[0].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[2];
		EST_astTimedKernelEvents[0].boToothScheduled = TRUE;
		
		if (0 < USERCAL_stRAMCAL.boEDISEnable)
		{
			EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedTimeStep;
		}
		else
		{
			EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		}
		
		EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionC;
		EST_astTimedKernelEvents[0].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[2];
	
		/* Switch igniter off at timer ms */
		if (0 != USERCAL_stRAMCAL.u8ScheduleToothIgnition)
		{
			/* Switch igniter off at timer ms */
			EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
			EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enGlobalLinkedFraction;
			EST_astTimedKernelEvents[1].ptEventTime = &EST_tEndFractionC;
			EST_astTimedKernelEvents[1].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[2];
			EST_astTimedKernelEvents[1].boToothScheduled = TRUE;
		}
		else
		{
		EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
		EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		EST_astTimedKernelEvents[1].ptEventTime = &EST_tDwellUs;
		EST_astTimedKernelEvents[1].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[2];
		}
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource,
		(void*)&EST_astTimedKernelEvents[0], (void*)&tEventCount);
	}


	/* Request and initialise EST_nESTOutput D ***************************/
	if ((0xffff > USERCAL_stRAMCAL.au32IgnitionSequence[3]) && (EH_IO_Invalid > USERCAL_stRAMCAL.aESTIOResource[3]))
	{
		enEHIOResource = USERCAL_stRAMCAL.aESTIOResource[3];     //EST_nESTOutputD;
		enEHIOType = IOAPI_enCaptureCompare;
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	
			(void*)NULL, (void*)NULL);
	
		/* Initialise the TEPM channel EST_nESTOutput the dwell timer*/
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			stTEPMChannelCB.enAction = TEPMAPI_enSetLow;
			stTEPMChannelCB.boInterruptEnable = TRUE;
			stTEPMChannelCB.enPreScalar = 0;
			stTEPMChannelCB.u32Sequence = USERCAL_stRAMCAL.au32IgnitionSequence[3];
		
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}
	
		/* Switch igniter on at a fraction of global time */
		EST_astTimedKernelEvents[0].enAction = TEPMAPI_enSetHigh;
		EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionD;
		EST_astTimedKernelEvents[0].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[3];
		EST_astTimedKernelEvents[0].boToothScheduled = TRUE;
	
		if (0 < USERCAL_stRAMCAL.boEDISEnable)
		{
			EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedTimeStep;
		}
		else
		{
			EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
		}
		
		EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionD;
		EST_astTimedKernelEvents[0].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[3];
	
		/* Switch igniter off at timer ms */
		if (0 != USERCAL_stRAMCAL.u8ScheduleToothIgnition)
		{
		/* Switch igniter off at timer ms */
		EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
			EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enGlobalLinkedFraction;
			EST_astTimedKernelEvents[1].ptEventTime = &EST_tEndFractionD;
			EST_astTimedKernelEvents[1].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[3];
			EST_astTimedKernelEvents[1].boToothScheduled = TRUE;
		}
		else
		{
			EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
		EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
		EST_astTimedKernelEvents[1].ptEventTime = &EST_tDwellUs;
		EST_astTimedKernelEvents[1].enEHIOBitMirrorResource = USERCAL_stRAMCAL.aESTIOMuxResource[3];
		}
	
		USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource,
		(void*)&EST_astTimedKernelEvents[0], (void*)&tEventCount);
	}

	/* Request and initialise required Kernel managed spread for Timingx */
	EST_tSpreadTimingxIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMFiltered, (void*)&USERCAL_stRAMCAL.aUserTimingxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for Timingy */
	EST_tSpreadTimingyIDX = SETUP_tSetupSpread((void*)&MAP_tKiloPaFiltered, (void*)&USERCAL_stRAMCAL.aUserTimingySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Timing*/
	EST_tMapTimingIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserTimingMap, (void*)&EST_u16Timing, TYPE_enUInt16, 17, 17, EST_tSpreadTimingxIDX, EST_tSpreadTimingyIDX, NULL);		

	/* Request and initialise required Kernel managed spread for Dwell */
	EST_tSpreadDwellIDX = SETUP_tSetupSpread((void*)&BVM_tBattVolts, (void*)&USERCAL_stRAMCAL.aUserDwellSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Dwell*/
	EST_tTableDwellIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserDwellTable, (void*)&EST_u16Dwell, TYPE_enUInt16, 17, EST_tSpreadDwellIDX, NULL);

	/* Request and initialise required Kernel managed spread for CTS Timing Trim */
	EST_tSpreadCTSTimingTrimIDX = SETUP_tSetupSpread((void*)&CTS_tTempCFiltered, (void*)&USERCAL_stRAMCAL.aUserCTSTimingCorrectionSpread, TYPE_enInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for CTS Timing Trim*/
	EST_tCTSTimingTrimIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserCTSTimingCorrectionTable, (void*)&EST_s16CTSTimingTrim, TYPE_enInt16, 17, EST_tSpreadCTSTimingTrimIDX, NULL);
	
	/* Request and initialise required Kernel managed spread for ATS Timing Trim */
	EST_tSpreadATSTimingTrimIDX = SETUP_tSetupSpread((void*)&ATS_tTempCFiltered, (void*)&USERCAL_stRAMCAL.aUserDwellSpread, TYPE_enInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for ATS Timing Trim */
	EST_tATSTimingTrimIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserATSTimingCorrectionTable, (void*)&EST_s16ATSTimingTrim, TYPE_enInt16, 17, EST_tSpreadATSTimingTrimIDX, NULL);


	/* Enable the Motor driver enables */
	enEHIOResource = EST_nMotor1EnablePin;
	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);

	enEHIOResource = EST_nMotor2EnablePin;
	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);

	SETUP_vSetDigitalIOHigh(EST_nMotor1EnablePin);
	SETUP_vSetDigitalIOHigh(EST_nMotor2EnablePin);

	/* Setup simple cam sync resource */
	if (EH_IO_Invalid != USERCAL_stRAMCAL.enSimpleCamSyncSource)
	{
		enEHIOResource = USERCAL_stRAMCAL.enSimpleCamSyncSource;
		enEHIOType = IOAPI_enDIOInput;
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);

		/* Setup the simple CAM sync */
		boCamSyncLate = USERCAL_stRAMCAL.boCamSyncHighLate;
		u32CamSyncSampleToothCount = USERCAL_stRAMCAL.u32CamSyncSampleToothCount;

		SETUP_vSetupSimpleCamSync(enEHIOResource, boCamSyncLate, u32CamSyncSampleToothCount);
	}


	/* Request and initialise VVT1 Input  ***************************/
	if (EH_IO_Invalid > USERCAL_stRAMCAL.VVTInputResource)
	{
		enEHIOResource = USERCAL_stRAMCAL.VVTInputResource;
		enEHIOType = IOAPI_enCaptureCompare;
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,
			(void*)NULL, (void*)NULL);

		/* Initialise the VVT Input */
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			stTEPMChannelCB.enAction = TEPMAPI_enCapAny;
			stTEPMChannelCB.boInterruptEnable = TRUE;
			stTEPMChannelCB.u32Sequence = 0x10000000 + USERCAL_stRAMCAL.VVTInputType;

			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
		}

		stTimedEvent.enMethod = TEPMAPI_enLinkVVT1Input;
		stTimedEvent.pfEventCB = NULL;

		USER_vSVC(SYSAPI_enConfigureUserTEPMInput, (void*)&enEHIOResource,
			(void*)&stTimedEvent, (void*)NULL);
	}

	/* Set up EST active output */
	if (EH_IO_Invalid != USERCAL_stRAMCAL.enESTBypass)
	{
		enEHIOResource = USERCAL_stRAMCAL.enESTBypass;
		enEHIOType = IOAPI_enDIOOutput;
		enDriveStrength = IOAPI_enStrong;
		
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}

	/* Matthew testing ADD the rest later!!! */
	//enEHIOResource = USERCAL_stRAMCAL.aESTIOMuxResource[0];
	//SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
}

void EST_vRun(puint32 const pu32Arg)
{
	uint32 u32DwellDegrees;		
	uint32 u32DelayDegrees;	
	uint32 u32DwellUsMax;
	uint32 u32DwellUs;
	uint32 u32Temp;
	sint32 s32ESTTrims[2];
	static uint32 u32CrankDwell;
	static uint32 u32BypassCount;
	uint32 u32Dwell;
	IOAPI_tenTriState enTriState;
	IOAPI_tenEHIOResource enEHIOResource;
	sint16 s16Temp;
	
	/* Calculate the current spread for Timingx */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadTimingxIDX,
			NULL, NULL);		
	
	/* Calculate the current spread for Timingy */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadTimingyIDX,
			NULL, NULL);	

	/* Lookup the current timing value for Timingx */
	USER_vSVC(SYSAPI_enCalculateMap, (void*)&EST_tMapTimingIDX,
		NULL, NULL);	
		
	/* Calculate the current spread for dwell */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadDwellIDX,
	NULL, NULL);

	/* Lookup the current dwell value for dwell */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&EST_tTableDwellIDX,
	NULL, NULL);	
	
	/* Calculate the current spread for CTS trim */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadCTSTimingTrimIDX,
	NULL, NULL);	
	
	/* Lookup the current trim value for CTS trim */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&EST_tCTSTimingTrimIDX,
	NULL, NULL);	
	
	/* Calculate the current spread for ATS trim */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadATSTimingTrimIDX,
	NULL, NULL);	
	
	/* Lookup the current trim value for ATS trim */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&EST_tATSTimingTrimIDX,
	NULL, NULL);	
	
	/* Apply CTS and ATS trims */
	s16Temp = EST_s16CTSTimingTrim + EST_s16ATSTimingTrim;
	s16Temp += EST_u16Timing;
	
	if (0x8000 > s16Temp)
	{
		EST_u16Timing = (uint16)s16Temp;
	}
	else
	{
		EST_u16Timing = 0;
	}
	
	
	CPU_xEnterCritical();

	u32DwellUsMax = (60000000 / CAM_u32RPMRaw) - EST_nDwellOffMinUs;

	if (300 > CAM_u32RPMFiltered)
	{
		u32CrankDwell = USERCAL_stRAMCAL.aUserDwellTable[0];
	}
	else
	{
		u32CrankDwell = 50 < u32CrankDwell ? u32CrankDwell - 50 : 0;
	}

	u32Dwell = MAX(u32CrankDwell, EST_u16Dwell);
		
	if (TRUE == USERCAL_stRAMCAL.boOBDISCADV)
	{	
		if (0 < IAC_s32ISCESTTrim[0])						
		{
			EST_tIgnitionAdvanceMtheta = IAC_s32ISCESTTrim[0] + USERCAL_stRAMCAL.u16TimingMainOffset;
		}
		else
		{
			EST_tIgnitionAdvanceMtheta = 100 * EST_u16Timing + USERCAL_stRAMCAL.u16TimingMainOffset;
		}
	}
	else
	{
		s32ESTTrims[0] = CLO2_s32ISCESTTrim[0] + IAC_s32ISCESTTrim[0];
		s32ESTTrims[1] = CLO2_s32ISCESTTrim[1] + IAC_s32ISCESTTrim[1];

		if (0 <= s32ESTTrims[1])
		{
			EST_tIgnitionAdvanceMtheta = 100 * EST_u16Timing + USERCAL_stRAMCAL.u16TimingMainOffset + (uint16)s32ESTTrims[1];
		}
		else
		{
			EST_tIgnitionAdvanceMtheta = 100 * EST_u16Timing + USERCAL_stRAMCAL.u16TimingMainOffset;

			if (0 < (s32ESTTrims[1] + (sint32)EST_tIgnitionAdvanceMtheta))
			{
				EST_tIgnitionAdvanceMtheta = (uint16)(s32ESTTrims[1] + (sint32)EST_tIgnitionAdvanceMtheta);
			}
			else
			{
				EST_tIgnitionAdvanceMtheta = 0;
			}
		}
	}

	CPU_xExitCritical();

	u32DwellUs = MIN(u32DwellUsMax, (uint32)u32Dwell);
			
	u32Temp = 600000 / CAM_u32RPMRaw;
		
	if (0 == USERCAL_stRAMCAL.u8WastedSparkEnable)
	{
		u32DwellDegrees = (u32DwellUs * EST_nDegreesPerCycle) / u32Temp;
		u32DelayDegrees = 20 * EST_nDegreesPerCycle - u32DwellDegrees / 10 - (EST_tIgnitionAdvanceMtheta / 100);
	}
	else
	{
		u32DwellDegrees = (u32DwellUs * EST_nDegreesPerCycle) / u32Temp;
	u32DelayDegrees = 10 * EST_nDegreesPerCycle - u32DwellDegrees / 10 - (EST_tIgnitionAdvanceMtheta / 100);
	}
	
	if (0 == USERCAL_stRAMCAL.boEDISEnable)
	{
	/* Calculate EST start and end angles */
	switch (USERCAL_stRAMCAL.u8CylCount)
	{
		case 4:
		{
			if (0 == USERCAL_stRAMCAL.u8WastedSparkEnable)
			{
					CPU_xEnterCritical();
					EST_tStartFractionA = (6554 * u32DelayDegrees) / (2 * EST_nDegreesPerCycle);
					EST_tStartFractionB = ((6554 * u32DelayDegrees) / (2 * EST_nDegreesPerCycle));
					EST_tStartFractionC = ((6554 * u32DelayDegrees) / (2 * EST_nDegreesPerCycle));
					EST_tStartFractionD = ((6554 * u32DelayDegrees) / (2 * EST_nDegreesPerCycle));

					EST_tEndFractionA = (2000000 * EST_nDegreesPerCycle - 1000 * EST_tIgnitionAdvanceMtheta) / 10986;
					EST_tEndFractionB = (2000000 * EST_nDegreesPerCycle - 1000 * EST_tIgnitionAdvanceMtheta) / 10986;
					EST_tEndFractionC = (2000000 * EST_nDegreesPerCycle - 1000 * EST_tIgnitionAdvanceMtheta) / 10986;
					EST_tEndFractionD = (2000000 * EST_nDegreesPerCycle - 1000 * EST_tIgnitionAdvanceMtheta) / 10986;
					CPU_xExitCritical();
			}
			else
			{
					CPU_xEnterCritical();
				EST_tStartFractionA = (6554 * u32DelayDegrees) / EST_nDegreesPerCycle;
				EST_tStartFractionB = ((6554 * u32DelayDegrees) / EST_nDegreesPerCycle);
				EST_tStartFractionC = (6554 * u32DelayDegrees) / EST_nDegreesPerCycle;
				EST_tStartFractionD = ((6554 * u32DelayDegrees) / EST_nDegreesPerCycle);

					EST_tEndFractionA = (1000000 * EST_nDegreesPerCycle - 1000 * EST_tIgnitionAdvanceMtheta) / 5493;
					EST_tEndFractionB = (1000000 * EST_nDegreesPerCycle - 1000 * EST_tIgnitionAdvanceMtheta) / 5493;
					EST_tEndFractionC = (1000000 * EST_nDegreesPerCycle - 1000 * EST_tIgnitionAdvanceMtheta) / 5493;
					EST_tEndFractionD = (1000000 * EST_nDegreesPerCycle - 1000 * EST_tIgnitionAdvanceMtheta) / 5493;
					CPU_xExitCritical();
			}

			break;
		}
		case 8:
		{
			CPU_xEnterCritical();
			if (1850 < u32DelayDegrees)
			{
				//EST_tStartFractionA = (6554 * (u32DelayDegrees - 1800)) / EST_nDegreesPerCycle;
				//EST_tStartFractionB = (6554 * (u32DelayDegrees - 1800)) / EST_nDegreesPerCycle;
				//EST_tStartFractionC = (6554 * (u32DelayDegrees - 1800)) / EST_nDegreesPerCycle;
				//EST_tStartFractionD = (6554 * (u32DelayDegrees - 1800)) / EST_nDegreesPerCycle;
				EST_tStartFractionA = (6554 * (u32DelayDegrees)) / EST_nDegreesPerCycle;
				EST_tStartFractionB = (6554 * (u32DelayDegrees)) / EST_nDegreesPerCycle;
				EST_tStartFractionC = (6554 * (u32DelayDegrees)) / EST_nDegreesPerCycle;
				EST_tStartFractionD = (6554 * (u32DelayDegrees)) / EST_nDegreesPerCycle;
			}
			else
			{
				EST_tStartFractionA = (6554 * 50) / EST_nDegreesPerCycle;
				EST_tStartFractionB = (6554 * 50) / EST_nDegreesPerCycle;
				EST_tStartFractionC = (6554 * 50) / EST_nDegreesPerCycle;
				EST_tStartFractionD = (6554 * 50) / EST_nDegreesPerCycle;
			}
			CPU_xExitCritical();

			break;
		}
		default:
		{
			EST_tStartFractionA = (6554 * u32DelayDegrees) / EST_nDegreesPerCycle;
			EST_tStartFractionB = ((6554 * u32DelayDegrees) / EST_nDegreesPerCycle);
			EST_tStartFractionC = ((6554 * u32DelayDegrees) / EST_nDegreesPerCycle);
			EST_tStartFractionD = ((6554 * u32DelayDegrees) / EST_nDegreesPerCycle);
		}
	}

	EST_tDwellUs = EST_xUsToSlowTicks(u32DwellUs);
	}
	else
	{
		if ((60000 > EST_tIgnitionAdvanceMtheta) && (0 <= EST_tIgnitionAdvanceMtheta))
		{
			u32Temp = 1536 - (100 * EST_tIgnitionAdvanceMtheta / 3906);			
		}
		else
		{
			u32Temp = 0;
		}
		
		EST_tStartFractionA = 50;
		EST_tStartFractionB = 50;
		EST_tStartFractionC = 50;
		EST_tStartFractionD = 50;
		EST_tDwellUs = EST_xUsToSlowTicks(u32Temp);
	}



	/* Set the EST active output */
	if (EH_IO_Invalid != USERCAL_stRAMCAL.enESTBypass)
	{
		u32BypassCount = (500U < CAM_u32RPMRaw) && (~0U != u32BypassCount) ? u32BypassCount + 1 : u32BypassCount;
		u32BypassCount = 0U == CAM_u32RPMRaw ? 0U : u32BypassCount;

		if (500 < u32BypassCount)
		{
			enEHIOResource = USERCAL_stRAMCAL.enESTBypass;
			enTriState = IOAPI_enLow;

			if (EH_IO_IIC1_SDA > enEHIOResource)
			{
			USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&enTriState,	(void*)NULL);
		}
		}
		else
		{
			enEHIOResource = USERCAL_stRAMCAL.enESTBypass;

			if (EH_IO_IIC1_SDA > enEHIOResource)
			{

			USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&enTriState,	(void*)NULL);
			}
		}
	}
}

void EST_vTerminate(puint32 const pu32Arg)
{

}


void EST_vCallBack(puint32 const pu32Arg)
{

}

#endif //BUILD_USER
