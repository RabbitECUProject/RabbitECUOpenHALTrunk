/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Sensors                                                */
/* DESCRIPTION:        This code manages the sensors AD conversions           */
/*                                                                            */
/*                                                                            */
/* FILE NAME:          SENSORS.c                                              */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _SENSORS_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "SENSORS.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 SENSORS_u32RunCounter;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static void SENSORS_vCEMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void SENSORS_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength;
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;
	TEPMAPI_tstTEPMResourceCB stTEPMResourceCB;	
	TEPMAPI_tstTimedUserEvent stTimedEvent;
	SENSORS_u32RunCounter = 0;

	/* Enable the Hall-Effect sensor type */
	enEHIOResource = VRA_nPullupEnablePin;
	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRB_nPullupEnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRA_nVREnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRB_nVREnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VR_nHystHighPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VR_nHystLowPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = CTS_nSwitchPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);


	if (TRUE == USERCAL_stRAMCAL.u8UserPrimaryVREnable)
	{
		SETUP_vSetDigitalIOHigh(VRA_nVREnablePin);
		SETUP_vSetDigitalIOLow(VRA_nPullupEnablePin);
	}
	else
	{
		SETUP_vSetDigitalIOLow(VRA_nVREnablePin);
		SETUP_vSetDigitalIOHigh(VRA_nPullupEnablePin);
	}

	if (TRUE == USERCAL_stRAMCAL.u8UserSecondaryVREnable)
	{
		SETUP_vSetDigitalIOLow(VRB_nPullupEnablePin);
		SETUP_vSetDigitalIOHigh(VRB_nVREnablePin);
	}
	else
	{
		SETUP_vSetDigitalIOHigh(VRB_nPullupEnablePin);
		SETUP_vSetDigitalIOLow(VRB_nVREnablePin);
	}

	SETUP_vSetDigitalIOHigh(VR_nHystHighPin);
	SETUP_vSetDigitalIOLow(VR_nHystLowPin);

	
	/* Request and initialise FTM for CRANK_nInput */
	enEHIOResource = EH_VIO_TC0;	
	enEHIOType = IOAPI_enTEPM;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* ONLY CONFIGURE THE FTM MODULE ONCE PER PROJECT! */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		stTEPMResourceCB.enEHIOResource = EH_VIO_TC0;
		stTEPMResourceCB.enPreScalar = SENSORS_nFastFTMDivisor;
		stTEPMResourceCB.enCountType = TEPMAPI_enCountUp;		
					
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMResourceCB);	
	}	
		
	/* Request and initialise CRANK_nInput */
	enEHIOResource = CRANK_nInput;
	enEHIOType = IOAPI_enCaptureCompare;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
	
	/* Initialise the TEPM channel CRANK_nInput */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{	
		stTEPMChannelCB.enAction = TEPMAPI_enCapAny;
		stTEPMChannelCB.boInterruptEnable = TRUE;	
	
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}	

	stTimedEvent.enMethod = TEPMAPI_enLinkPrimaryProgram;
	stTimedEvent.pfEventCB = &SENSORS_vCEMCallBack;
	
	USER_vSVC(SYSAPI_enConfigureUserTEPMInput, (void*)&enEHIOResource, 
		(void*)&stTimedEvent, (void*)NULL);	


	/* Request and initialise AFM_FREQ_nInput */
	enEHIOResource = AFM_FREQ_nInput;
	enEHIOType = IOAPI_enCaptureCompare;
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* Initialise the TEPM channel AFM_FREQ_nInput */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		stTEPMChannelCB.enAction = TEPMAPI_enCapAny;
		stTEPMChannelCB.boInterruptEnable = TRUE;
		
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	}

	stTimedEvent.enMethod = TEPMAPI_enLinkFreqInput;
	stTimedEvent.pfEventCB = &SENSORS_vCEMCallBack;
	
	USER_vSVC(SYSAPI_enConfigureUserTEPMInput, (void*)&enEHIOResource,
	(void*)&stTimedEvent, (void*)NULL);

	/* Enable the Hall-Effect sensor type */
	enEHIOResource = VRA_nPullupEnablePin;
	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRB_nPullupEnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRA_nVREnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VRB_nVREnablePin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VR_nHystHighPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = VR_nHystLowPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);


	if (TRUE == USERCAL_stRAMCAL.u8UserPrimaryVREnable)
	{
		SETUP_vSetDigitalIOHigh(VRA_nVREnablePin);
		SETUP_vSetDigitalIOLow(VRA_nPullupEnablePin);
	}
	else
	{
		SETUP_vSetDigitalIOLow(VRA_nVREnablePin);
		SETUP_vSetDigitalIOHigh(VRA_nPullupEnablePin);
	}

	if (TRUE == USERCAL_stRAMCAL.u8UserSecondaryVREnable)
	{
		SETUP_vSetDigitalIOLow(VRB_nPullupEnablePin);
		SETUP_vSetDigitalIOHigh(VRB_nVREnablePin);
	}
	else
	{
		SETUP_vSetDigitalIOHigh(VRB_nPullupEnablePin);
		SETUP_vSetDigitalIOLow(VRB_nVREnablePin);
	}

	SETUP_vSetDigitalIOHigh(VR_nHystHighPin);
	SETUP_vSetDigitalIOLow(VR_nHystLowPin);
}

void SENSORS_vRun(puint32 const pu32Arg)
{
	ADCAPI_tenTrigger enTrigger;
	volatile static uint32 a = 4;
	volatile static uint32 b = 5;
	static uint32 u32Count;

		
	if (0 == (SENSORS_u32RunCounter % SENSORS_nFastPeriod))
	{
		enTrigger = ADCAPI_enTrigger4;	
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, (void*)NULL, (void*)NULL);/*CR1_12*/
	}
	
	if (0 == (SENSORS_u32RunCounter % SENSORS_nSlowPeriod))
	{
		if (0 == (u32Count++ % 2))
		{
			SENSORS_boCTSACBiasHigh = TRUE;
			SETUP_vSetDigitalIOHigh(CTS_nSwitchPin);
		}
		else
		{
			SENSORS_boCTSACBiasHigh = FALSE;
			SETUP_vSetDigitalIOLow(CTS_nSwitchPin);
		}
	}	

	if ((SENSORS_nSlowPeriod / 2) == (SENSORS_u32RunCounter % SENSORS_nSlowPeriod))
	{
		enTrigger = ADCAPI_enTrigger2;	
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, (void*)NULL, (void*)NULL);/*CR1_12*/
	}
	
	if (0 == SENSORS_u32RunCounter % 0x05)
	{
	    SETUP_vSetDigitalIOHigh(VR_nHystHighPin);
	    SETUP_vSetDigitalIOHigh(VR_nHystLowPin);
	}
	else if (a == SENSORS_u32RunCounter % 0x05)
	{
	    SETUP_vSetDigitalIOLow(VR_nHystLowPin);
	}
	else if (b == SENSORS_u32RunCounter % 0x05)
	{
	    SETUP_vSetDigitalIOLow(VR_nHystHighPin);
	}

	
	SENSORS_u32RunCounter++;
}

void SENSORS_vTerminate(puint32 const pu32Arg)
{

}


void SENSORS_vCallBack(puint32 const pu32Arg)
{

}

static void SENSORS_vCEMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	switch (enEHIOResource)
	{
		case CRANK_nInput:
		{
		    CAM_vEngineSpeedCB(tEventTime);		
			break;	
		}
		case AFM_FREQ_nInput:
		{
			AFM_tSensorHertz = SENSORS_nSlowFTMFreq / tEventTime;
			break;
		}
		default:
		{
			break;
		}
	}	
}


#endif //BUILD_USER
