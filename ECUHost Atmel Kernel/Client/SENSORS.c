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
#include "USERMATH.h"
#include "IAC.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 SENSORS_u32RunCounter;
uint8 SENSORS_au8CANBufferMap[] = SENSORS_nCANBufferMap;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static void SENSORS_vCEMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime);
static void SENSORS_vGetCANSensorData(void);
static uint32 SENSORS_u32GetCANData(puint8 pu8CANBuffer, uint32 u32Offset, uint32 u32ByteCount);


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

	/* Matthew cAM debug */
	enEHIOResource = CAM_nDebug;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);

#ifdef BUILD_CTS_PULLUP_SENSE
	enEHIOResource = CTS_nSwitchPin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
#endif

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
	
	/* ONLY CONFIGURE THE TC0 MODULE ONCE PER PROJECT! */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		stTEPMResourceCB.enEHIOResource = EH_VIO_TC0;
		stTEPMResourceCB.enPreScalar = SENSORS_nFastFTMDivisor;
		stTEPMResourceCB.enCountType = TEPMAPI_enCountUp;		
					
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMResourceCB);	
	}	
		
	/* Request and initialise TC1 for missing tooth interrupt */
	enEHIOResource = EH_VIO_TC1;
	enEHIOType = IOAPI_enTEPM;
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* ONLY CONFIGURE THE TC1 MODULE ONCE PER PROJECT! */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		stTEPMResourceCB.enEHIOResource = EH_VIO_TC1;
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

	/* Request and initialise TC2 for AFM frequency or cam sensor input */
	enEHIOResource = EH_VIO_TC2;
	enEHIOType = IOAPI_enTEPM;
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* ONLY CONFIGURE THE TC2 MODULE ONCE PER PROJECT! */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		stTEPMChannelCB.enAction = TEPMAPI_enCapAny;
		stTEPMResourceCB.enEHIOResource = EH_VIO_TC2;
		stTEPMResourceCB.enPreScalar = 1 << EST_nSlowFTMDivisor;
		stTEPMResourceCB.enCountType = TEPMAPI_enCountUp;
		
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMResourceCB);
	}

	/* Request and initialise AFM_FREQ_nInput */
	enEHIOResource = AFM_FREQ_nInput;
	enEHIOType = IOAPI_enCaptureCompare;
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* Initialise the TEPM channel AFM_FREQ_nInput */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		stTEPMChannelCB.enAction = TEPMAPI_enCapRising;
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

	SENSORS_u8CANCTS = 0xff;
	SENSORS_u8CANATS = 0xff;
	SENSORS_u8CANSTT[0] = 0xff;
	SENSORS_u8CANSTT[1] = 0xff;
	SENSORS_u8CANLTT[0] = 0xff;
	SENSORS_u8CANLTT[1] = 0xff;
}

void SENSORS_vRun(puint32 const pu32Arg)
{
	ADCAPI_tenTrigger enTrigger;
	PROTAPI_tstCANMsg stCANMsg;
	IOAPI_tenEHIOResource enEHIOResource;
	uint8 u8TriggerHystLow = 2 - USERCAL_stRAMCAL.u8TriggerPullStrength;
	uint8 u8TriggerHystHigh = 4 + USERCAL_stRAMCAL.u8TriggerPullStrength;
		
	if (0 == (SENSORS_u32RunCounter % SENSORS_nFastPeriod))
	{
		enTrigger = ADCAPI_enTrigger4;	
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, (void*)NULL, (void*)NULL);/*CR1_12*/
	}	

#ifdef BUILD_CTS_PULLUP_SENSE
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
#endif

	if (1 == (SENSORS_u32RunCounter % SENSORS_nSlowPeriod))
	{
		enTrigger = ADCAPI_enTrigger2;	
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, (void*)NULL, (void*)NULL);/*CR1_12*/
	}
	
	if (0 == SENSORS_u32RunCounter % 0x05)
	{
		if (0 < u8TriggerHystLow)
		{
			SETUP_vSetDigitalIOHigh(VR_nHystLowPin);
		}
		else
		{
			SETUP_vSetDigitalIOLow(VR_nHystLowPin);
		}

		if (0 < u8TriggerHystHigh)
		{
			SETUP_vSetDigitalIOHigh(VR_nHystHighPin);
		}
		else
		{
			SETUP_vSetDigitalIOLow(VR_nHystHighPin);
		}
	}
	else if (u8TriggerHystLow == SENSORS_u32RunCounter % 0x05)
	{
	    SETUP_vSetDigitalIOLow(VR_nHystLowPin);
	}
	if (u8TriggerHystHigh == SENSORS_u32RunCounter % 0x05)
	{
	    SETUP_vSetDigitalIOLow(VR_nHystHighPin);
	}

	if (0 == SENSORS_u32RunCounter % 0x08)
	{
		SENSORS_vGetCANSensorData();
	}

	if (0xc0 == SENSORS_u32RunCounter % 0x400)
	{
		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 0x7e0;
		stCANMsg.u32DWL = 0x00000000;
		stCANMsg.u32DWH = 0x02010600;
		enEHIOResource = EH_VIO_CAN1;

		//USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/
		UNUSED(stCANMsg);
		UNUSED(enEHIOResource);
	}

	if (0x1c0 == SENSORS_u32RunCounter % 0x400)
	{
		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 0x7e0;
		stCANMsg.u32DWL = 0x00000000;
		stCANMsg.u32DWH = 0x02010700;
		enEHIOResource = EH_VIO_CAN1;

		//USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/
	}

	if (0x2c0 == SENSORS_u32RunCounter % 0x400)
	{
		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 0x7e0;
		stCANMsg.u32DWL = 0x00000000;
		stCANMsg.u32DWH = 0x02010800;
		enEHIOResource = EH_VIO_CAN1;

		//USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/
	}

	if (0x3c0 == SENSORS_u32RunCounter % 0x400)
	{
		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 0x7e0;
		stCANMsg.u32DWL = 0x00000000;
		stCANMsg.u32DWH = 0x02010900;
		enEHIOResource = EH_VIO_CAN1;

		//USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/
	}

	if ((0 == SENSORS_u32RunCounter % 0x40) && (IAC_enClosedLoop == IAC_enControlState))
	{
		stCANMsg.u8DLC = 8;
		stCANMsg.u32ID = 0x7e0;
		stCANMsg.u32DWL = 0x00000000;
		stCANMsg.u32DWH = 0x02010e00;
		enEHIOResource = EH_VIO_CAN1;

		//USER_vSVC(SYSAPI_enQueueCANMessage, (void*)&enEHIOResource, (void*)&stCANMsg, (void*)NULL);/*CR1_12*/
	}

	
	SENSORS_u32RunCounter++;
}

void SENSORS_vInvalidateCAN16Data(uint32 u32MSGIDX, uint32 u32DataOffset)
{
	IOAPI_tenEHIOResource enEHIOResource;
	puint8 pu8CANDataBuffer;
	enEHIOResource = EH_VIO_CAN1;

	USER_vSVC(SYSAPI_enGetRawCommsBuffer, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		pu8CANDataBuffer = (puint8)pstSVCDataStruct->pvArg1;
		pu8CANDataBuffer += (8 * u32MSGIDX);
		pu8CANDataBuffer += u32DataOffset;
		memset(pu8CANDataBuffer, 0xff, 2);
	}
}

static void SENSORS_vGetCANSensorData()
{
	IOAPI_tenEHIOResource enEHIOResource;
	uint8* pu8CANDataBuffer = {0};
	enEHIOResource = EH_VIO_CAN1;

	USER_vSVC(SYSAPI_enGetRawCommsBuffer, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		pu8CANDataBuffer = (puint8)pstSVCDataStruct->pvArg1;
	}

	if (NULL != pu8CANDataBuffer)
	{
		SENSORS_u8CANCTS = pu8CANDataBuffer[1];
		SENSORS_u8CANATS = pu8CANDataBuffer[0];
		SENSORS_u16CANTPS = pu8CANDataBuffer[11] * 0x100 + pu8CANDataBuffer[10];
		SENSORS_u8CANPPS = pu8CANDataBuffer[12];
		SENSORS_au16CANAFR[0] = pu8CANDataBuffer[20] + 0x100 * pu8CANDataBuffer[21];
		SENSORS_u16CANVSS = (uint16)SENSORS_u32GetCANData(pu8CANDataBuffer, USERCAL_stRAMCAL.au8SensorCANDataOffset[4], USERCAL_stRAMCAL.au8SensorCANDataByteCount[4]);

		SENSORS_boCANCTSNewSample = 0xff != SENSORS_u8CANCTS ? TRUE : FALSE;
		SENSORS_boCANATSNewSample = 0xff != SENSORS_u8CANATS ? TRUE : FALSE;
		SENSORS_boCANTPSNewSample = 0xffff != SENSORS_u16CANTPS ? TRUE : FALSE;
		SENSORS_boCANPPSNewSample = 0xff != SENSORS_u8CANPPS ? TRUE : FALSE;
		SENSORS_boCANVSSNewSample = 0xffff != SENSORS_u16CANVSS ? TRUE : FALSE;
		SENSORS_aboCANUEGONewSample[0] = 0xffff != SENSORS_au16CANAFR[0] ? TRUE : FALSE;

		if ((3 == pu8CANDataBuffer[27]) && (65 == pu8CANDataBuffer[26]))
		{
			switch (pu8CANDataBuffer[25])
			{
				case SENSORS_nOBDSTT1PID:
				{
					SENSORS_u8CANSTT[0] = pu8CANDataBuffer[24];
					SENSORS_u8CANSTT[0] = 100 < SENSORS_u8CANSTT[0] ? SENSORS_u8CANSTT[0] : 100;
					SENSORS_u8CANSTT[0] = 156 > SENSORS_u8CANSTT[0] ? SENSORS_u8CANSTT[0] : 156;
					break;
				}
				case SENSORS_nOBDLTT1PID:
				{
					SENSORS_u8CANLTT[0] = pu8CANDataBuffer[24];
					SENSORS_u8CANLTT[0] = 100 < SENSORS_u8CANLTT[0] ? SENSORS_u8CANLTT[0] : 100;
					SENSORS_u8CANLTT[0] = 156 > SENSORS_u8CANLTT[0] ? SENSORS_u8CANLTT[0] : 156;
					break;
				}
				case SENSORS_nOBDSTT2PID:
				{
					SENSORS_u8CANSTT[1] = pu8CANDataBuffer[24];
					SENSORS_u8CANSTT[1] = 100 < SENSORS_u8CANSTT[1] ? SENSORS_u8CANSTT[1] : 100;
					SENSORS_u8CANSTT[1] = 156 > SENSORS_u8CANSTT[1] ? SENSORS_u8CANSTT[1] : 156;
					break;
				}
				case SENSORS_nOBDLTT2PID:
				{
					SENSORS_u8CANLTT[1] = pu8CANDataBuffer[24];
					SENSORS_u8CANLTT[1] = 100 < SENSORS_u8CANLTT[1] ? SENSORS_u8CANLTT[1] : 100;
					SENSORS_u8CANLTT[1] = 156 > SENSORS_u8CANLTT[1] ? SENSORS_u8CANLTT[1] : 156;
					break;
				}
				case SENSORS_nOBDESTADVPID:
				{
					SENSORS_u8OBDAdv = pu8CANDataBuffer[24];
					SENSORS_u8OBDAdv = 128 < SENSORS_u8OBDAdv ? SENSORS_u8OBDAdv : 128;
					SENSORS_u8OBDAdv = 228 > SENSORS_u8OBDAdv ? SENSORS_u8OBDAdv : 228;
					SENSORS_boOBDAdvNewSample = TRUE;
					break;
				}
				default:
				{
					break;
				}
			}

			pu8CANDataBuffer[26] = 0xff;
			pu8CANDataBuffer[27] = 0xff;
		}
	}
}

static uint32 SENSORS_u32GetCANData(puint8 pu8CANBuffer, uint32 u32Offset, uint32 u32ByteCount)
{
    uint32 u32Data = 0;
	puint8 pu8Data;

	while (0 < u32ByteCount)
	{
		pu8Data = pu8CANBuffer + SENSORS_au8CANBufferMap[u32Offset];
		u32Data += ((*pu8Data) << (8 * (u32ByteCount - 1)));
		u32Offset++;
		u32ByteCount--;
	}

	return u32Data;
}


void SENSORS_vTerminate(puint32 const pu32Arg)
{

}


void SENSORS_vCallBack(puint32 const pu32Arg)
{

}

static void SENSORS_vCEMCallBack(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	uint32 u32Hertz;
	static uint32 au32Hertz[SENSORS_nAFMHertzSamples];
	static uint32 u32HertzSampleIDX;


	switch (enEHIOResource)
	{
		case CRANK_nInput:
		{
		    CAM_vEngineSpeedCB(tEventTime);		
			break;	
		}
		case AFM_FREQ_nInput:
		{
			au32Hertz[u32HertzSampleIDX] = (uint32)SENSORS_nSlowFTMFreq / tEventTime;
			u32Hertz = USERMATH_u32DiscardAndAverage32(au32Hertz, SENSORS_nAFMHertzSamples, 1);

			if (AFM_tSensorHertz <= u32Hertz)
			{
				AFM_tSensorHertz = u32Hertz;
			}
			else
			{
				AFM_tSensorHertz = 100 < (AFM_tSensorHertz - u32Hertz) ? AFM_tSensorHertz - 100 : u32Hertz;
			}

			u32HertzSampleIDX = (u32HertzSampleIDX + 1) % SENSORS_nAFMHertzSamples;

			break;
		}
		default:
		{
			break;
		}
	}	
}


#endif //BUILD_USER
