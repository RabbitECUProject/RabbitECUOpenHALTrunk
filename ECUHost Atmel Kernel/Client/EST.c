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
TABLEAPI_ttTableIDX EST_tMapTimingIDX;
TABLEAPI_ttTableIDX EST_tTableDwellIDX;
uint16 EST_u16Timing;
uint16 EST_u16Dwell;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void EST_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;	
	IOAPI_tenDriveStrength enDriveStrength;
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;	
	TEPMAPI_tstTEPMResourceCB stTEPMResourceCB;

	//uint16 u16TriggerArray[71] = {0, 0x38E, 0x71C, 0xAAA, 0xE38, 0x11C7, 0x1555, 0x18E3, 0x1C71, 0x2000, 0x238E, 0x271C, 0x2AAA, 0x2E38, 0x31C7, 0x3555, 0x38E3, 0x3C71, 0x4000, 0x438E, 0x471C, 0x4AAA, 0x4E38, 0x51C7, 0x5555, 0x58E3, 0x5C71, 0x6000, 0x638E, 0x671C, 0x6AAA, 0x6E38, 0x71C7, 0x7555, 0x8000, 0x838E, 0x871C, 0x8AAA, 0x8E38, 0x91C7, 0x9555, 0x98E3, 0x9C71, 0xA000, 0xA38E, 0xA71C, 0xAAAA, 0xAE38, 0xB1C7, 0xB555, 0xB8E3, 0xBC71, 0xC000, 0xC38E, 0xC71C, 0xCAAA, 0xCE38, 0xD1C7, 0xD555, 0xD8E3, 0xDC71, 0xE000, 0xE38E, 0xE71C, 0xEAAA, 0xEE38, 0xF1C7, 0xF555, 0, 0};
	IOAPI_tenEdgePolarity enEdgePolarity = IOAPI_enEdgeRising;
	bool boFirstRising = FALSE;
	
	/* Both peak and hold have a switch on and switch off event per cycle */
	TEPMAPI_ttEventCount tEventCount = 2;
	EST_tIgnitionAdvanceMtheta = 0;
		
	/* Set dwell to 10 degrees - 360 degrees */
	EST_tStartFractionA = (0x10000 * 300) / EST_nDegreesPerCycle;
	EST_tStartFractionB = (0x10000 * 120) / EST_nDegreesPerCycle;		
	EST_tDwellUs = 5000u;		

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
	
	/* Request and initialise EST_nESTOutput 1 */
	enEHIOResource = EST_nESTOutput1;
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
		
	/* Switch igniter on at a fraction of global time */
	EST_astTimedKernelEvents[0].enAction = TEPMAPI_enSetHigh;
	EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
	EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionA;
	
	/* Switch igniter off at timer ms */
	EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
	EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
	EST_astTimedKernelEvents[1].ptEventTime = &EST_tDwellUs;	
	
	USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
		(void*)&EST_astTimedKernelEvents[0], (void*)&tEventCount);	

	/* Request and initialise EST_nESTOutput 2 */
	enEHIOResource = EST_nESTOutput2;
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
		
	/* Switch igniter on at a fraction of global time */
	EST_astTimedKernelEvents[0].enAction = TEPMAPI_enSetHigh;
	EST_astTimedKernelEvents[0].enMethod = TEPMAPI_enGlobalLinkedFraction;
	EST_astTimedKernelEvents[0].ptEventTime = &EST_tStartFractionB;
	
	/* Switch igniter off at timer ms */
	EST_astTimedKernelEvents[1].enAction = TEPMAPI_enSetLow;
	EST_astTimedKernelEvents[1].enMethod = TEPMAPI_enHardLinkedTimeStep;
	EST_astTimedKernelEvents[1].ptEventTime = &EST_tDwellUs;	
	
	USER_vSVC(SYSAPI_enConfigureKernelTEPMOutput, (void*)&enEHIOResource, 
		(void*)&EST_astTimedKernelEvents[0], (void*)&tEventCount);	
	
	/* Request and initialise required Kernel managed spread for Timingx */
	EST_tSpreadTimingxIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserTimingxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for Timingy */
	EST_tSpreadTimingyIDX = SETUP_tSetupSpread((void*)&TPS_tThetaFiltered, (void*)&USERCAL_stRAMCAL.aUserTimingySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Timingx*/
	EST_tMapTimingIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserTimingMap, (void*)&EST_u16Timing, TYPE_enUInt16, 17, 17, EST_tSpreadTimingxIDX, EST_tSpreadTimingyIDX, NULL);		

	/* Request and initialise required Kernel managed spread for Dwell */
	EST_tSpreadDwellIDX = SETUP_tSetupSpread((void*)&BVM_tBattVolts, (void*)&USERCAL_stRAMCAL.aUserDwellSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for Dwell*/
	EST_tTableDwellIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserDwellTable, (void*)&EST_u16Dwell, TYPE_enUInt16, 17, EST_tSpreadDwellIDX, NULL);

	USER_vSVC(SYSAPI_enSetupCrankTriggerEdgePattern, (void*)&USERCAL_stRAMCAL.aUserPrimaryTriggerTable[0], (void*)&boFirstRising, (void*)&enEdgePolarity);

	USER_vSVC(SYSAPI_enSetupSyncPointsPattern, (void*)&USERCAL_stRAMCAL.aUserSyncPointsTable[0], NULL, NULL);

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
	
	/* Calculate the current spread for Timingy */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadTimingyIDX,
			NULL, NULL);	

	/* Lookup the current timing value for Timingx */
	USER_vSVC(SYSAPI_enCalculateMap, (void*)&EST_tMapTimingIDX,
		NULL, NULL);	
		
	/* Calculate the current spread for dwell */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&EST_tSpreadDwellIDX,
	NULL, NULL);

	/* Lookup the current timingy value for dwell */
	USER_vSVC(SYSAPI_enCalculateTable, (void*)&EST_tTableDwellIDX,
	NULL, NULL);			
	
	EST_tIgnitionAdvanceMtheta = 100 * EST_u16Timing + 24000;

	u32DwellUsMax = (60000000 / CAM_u32RPMRaw) - EST_nDwellOffMinUs;
	u32DwellUs = MIN(u32DwellUsMax, (uint32)EST_u16Dwell);
			
	//if (1300 > CAM_u32RPMRaw)
	//{		
	//    boLowRpmFlag = TRUE;		
	//}
	//else if (1500 < CAM_u32RPMRaw)
	//{
	//    boLowRpmFlag = FALSE;	
	//}

	//if (TRUE == boLowRpmFlag)
	//{
	//	u32DwellUsMin = 10000 - CAM_u32RPMRaw * 5;
	//	u32DwellUs = MAX(u32DwellUsMin, u32DwellUs);
	//}

	u32Temp = 600000 / CAM_u32RPMRaw;
	u32DwellDegrees = (u32DwellUs * EST_nDegreesPerCycle) / u32Temp;	
		
	u32DelayDegrees = 10 * EST_nDegreesPerCycle - u32DwellDegrees / 10 - (EST_tIgnitionAdvanceMtheta / 100);
	
	/* Calculate EST start and end angles */
	EST_tStartFractionA = (6554 * u32DelayDegrees) / EST_nDegreesPerCycle;
	EST_tStartFractionB = ((6554 * u32DelayDegrees) / EST_nDegreesPerCycle) - 0x8000;
	EST_tDwellUs = EST_xUsToSlowTicks(u32DwellUs);
}

void EST_vTerminate(puint32 const pu32Arg)
{

}


void EST_vCallBack(puint32 const pu32Arg)
{

}

#endif //BUILD_USER
