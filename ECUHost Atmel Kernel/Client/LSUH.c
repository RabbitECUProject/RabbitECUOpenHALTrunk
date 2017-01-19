/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      LSUH                                                   */
/* DESCRIPTION:        This code module initialises the required ADC, DAC     */
/*                     CTRL, spread, table, DIO and TEPM resources for        */
/*                     managing the heater control for the LSU4.X sensor/s    */
/*                                                                            */
/* FILE NAME:          LSUH.c                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _LSUH_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "LSUH.h"

/* LOCAL MACRO DEFINITIONS ****************************************************/
#define LSUH_nADConfigCount (sizeof(LSUH_rastADConfig) / sizeof(LSUH_tstADConfig))

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
TEPMAPI_tstTimedUserEvent LSUH_aastTimedUserEvents[LSU_DEVICE_COUNT][TEPMAPI_nUserEventsMax];
uint32 LSUH_au32TEPMTimeout[LSU_DEVICE_COUNT];
uint32 LSUH_au32HeaterDuty[LSU_DEVICE_COUNT];
//ASAM mode=readvalue name="Heater Duty 1" type=uint32 offset=0 min=0 max=100 m=0.001 b=0 units="%" format=6.3 help="Heater Left Duty Cycle" 
//ASAM mode=readvalue name="Heater Duty 2" type=uint32 offset=4 min=0 max=100 m=0.001 b=0 units="%" format=6.3 help="Heater Right Duty Cycle" 
uint32 LSUH_u32HeaterDutyMax;
uint32 LSUH_au32HeaterLowADCOnRaw[LSU_DEVICE_COUNT];
uint32 LSUH_au32HeaterLowADCOffRaw[LSU_DEVICE_COUNT];
uint32 LSUH_au32HeaterLowADCDeltaFiltered[LSU_DEVICE_COUNT];
GPM6_ttVolts LSUH_atHeaterSenseVolts[LSU_DEVICE_COUNT];/*CR1_54*/
GPM6_ttAmps LSUH_atHeaterAmps[LSU_DEVICE_COUNT];/*CR1_54*/
GPM6_ttMilliVolts LSUH_tHeaterEffMax;/*CR1_54*/
GPM6_ttMilliWatts LSUH_atHeaterPower[LSU_DEVICE_COUNT];
bool LSUH_aboADCQueuePending[LSU_DEVICE_COUNT];
CTRLAPI_ttPIDIDX LSUH_atPIDLSUH1IDX[LSU_DEVICE_COUNT];
sint32 LSUH_ai32PIDTarget[LSU_DEVICE_COUNT];
sint32 LSUH_ai32PIDFeedback[LSU_DEVICE_COUNT];
sint32 LSUH_ai32PIDOutput[LSU_DEVICE_COUNT];
uint32 LSUH_au32ACSampleCounter[LSU_DEVICE_COUNT];
bool LSUH_aboNernstRNewValue[LSU_DEVICE_COUNT];


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : LSUH_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void LSUH_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);/*CR1_53*/

/*******************************************************************************
* Interface        : LSUH_vInitiateTEPMQueue
*
* Implementation   : Function to initiate a new TEPM queue timed output
*
* Parameter
*    Par1          : enEHIOResource enum of the timer resource
*    Par2          : tEventTime the timer value at the last timer event of interest
*
* Return Value     : NIL
*******************************************************************************/
static void LSUH_vInitiateTEPMQueue(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime);


/* LOCAL CONSTANT DEFINITIONS (STATIC) ****************************************/
const LSUH_tstADConfig LSUH_rastADConfig[] = LSUH_nADConfig;


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void LSUH_vStart(uint32 * const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;
	CTRLAPI_tstPIDCB stPIDCB;
	CTRLAPI_tenCTRLType enCTRLType;	
	TEPMAPI_tstTEPMResourceCB stTEPMResourceCB;
	TEPMAPI_tstTEPMChannelCB stTEPMChannelCB;
	IOAPI_tenDriveStrength enDriveStrength = IOAPI_enWeak;
	uint32 u32ADConfigIDX;
	bool boInitFailed = FALSE;
	
	LSUH_tHeaterEffMax = LSUH_nHeaterEffInit;
	LSUH_au32ACSampleCounter[0] = 0;
	LSUH_au32ACSampleCounter[1] = 0;	
	
	/* Request and initialise FTM for EH_IO_TMR15 and EH_IO_TMR16 */
	enEHIOResource = EH_VIO_TC0;	
	enEHIOType = IOAPI_enTEPM;	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);
	
	/* ONLY CONFIGURE THE FTM MODULE ONCE PER PROJECT! */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		stTEPMResourceCB.enEHIOResource = EH_VIO_TC0;
		stTEPMResourceCB.enPreScalar = TEPMAPI_enDiv128;
		stTEPMResourceCB.enCountType = TEPMAPI_enCountUp;		
					
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stTEPMResourceCB);	
	}
	//
	///* Request and initialise EH_IO_TMR15 */
	//enEHIOResource = EH_IO_TMR1;
	//enEHIOType = IOAPI_enCaptureCompare;	
	//USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
		//
	///* Initialise the TEPM channel EH_IO_TMR15 */
	//if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	//{	
		//stTEPMChannelCB.enAction = TEPMAPI_enToggle;
		//stTEPMChannelCB.boInterruptEnable = TRUE;	
	//
		//USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			//(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	//}		
	
	/* Request and initialise EH_IO_TMR16 */
	//enEHIOResource = EH_IO_TMR1;
	//enEHIOType = IOAPI_enCaptureCompare;	
	//USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
	//
	///* Initialise the TEPM channel EH_IO_TMR16 */
	//if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	//{	
		//stTEPMChannelCB.enAction = TEPMAPI_enToggle;
		//stTEPMChannelCB.boInterruptEnable = TRUE;	
	//
		//USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
			//(void*)&enEHIOType,	(void*)&stTEPMChannelCB);
	//}		
	
	/* Request and initialise AD converter resources */
	for (u32ADConfigIDX = 0; u32ADConfigIDX < LSUH_nADConfigCount; u32ADConfigIDX++)
	{
		enEHIOResource = LSUH_rastADConfig[u32ADConfigIDX].enEHIOResource;
		enEHIOType = LSUH_rastADConfig[u32ADConfigIDX].enEHIOType;
		stADCCB.enSamplesAv = LSUH_rastADConfig[u32ADConfigIDX].enSamplesAv;
		stADCCB.pfResultCB = LSUH_rastADConfig[u32ADConfigIDX].pfResultCB;
		stADCCB.enTrigger = LSUH_rastADConfig[u32ADConfigIDX].enTrigger;				
			
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{											
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stADCCB);	

			if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) boInitFailed = TRUE;			
		}		
		else
		{	
			boInitFailed = TRUE;
			break;
		}
	}
	
	/* Initialise pump current enable control lines */
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		enEHIOResource = EH_IO_ADD5;
		enEHIOType = IOAPI_enDIOOutput;	
		enDriveStrength = IOAPI_enStrong;			
			
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
	
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{											
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&enDriveStrength);	

			if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) boInitFailed = TRUE;			
		}		
		else
		{	
			boInitFailed = TRUE;
		}
	}		
	
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	
	{
		enEHIOResource = EH_IO_ADD6;
		enEHIOType = IOAPI_enDIOOutput;	
		enDriveStrength = IOAPI_enStrong;			
			
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	
	
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{											
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&enDriveStrength);	

			if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) boInitFailed = TRUE;			
		}		
		else
		{	
			boInitFailed = TRUE;
		}
	}	

	/* Request and initialise the LSUH1 PID controller */
	stPIDCB.enPIDRate = CTRLAPI_enPIDCall;
	stPIDCB.pi32Target = &LSUH_ai32PIDTarget[0];
	stPIDCB.pi32Feedback = &LSUH_ai32PIDFeedback[0];	
	stPIDCB.pi32Output = &LSUH_ai32PIDOutput[0];	
	stPIDCB.u8PTerm = 0xFF;
	stPIDCB.u8ITerm = 0x03;
	stPIDCB.u8DTerm = 0x08;	
	stPIDCB.boNoWindUp = TRUE;	
	stPIDCB.boReset = FALSE;	
	stPIDCB.boResetIntegrator = FALSE;		
	stPIDCB.boRun = TRUE;
	stPIDCB.i32OutputMin = -2500000;	
	stPIDCB.i32OutputMax = 2500000;		
	enCTRLType = CTRLAPI_enPID;
	
	USER_vSVC(SYSAPI_enInitialiseCTRLResource, (void*)&enCTRLType, (void*)&stPIDCB, NULL);	
	
	LSUH_atPIDLSUH1IDX[0] = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
				(CTRLAPI_ttPIDIDX)pstSVCDataStruct->tClientHandle : -1;		
	
	*pu32Arg = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	? 
			*pu32Arg | SYSAPI_CALL_MASK_FAIL_RES_INIT : 
			*pu32Arg & ~SYSAPI_CALL_MASK_FAIL_RES_INIT;
			
	/* Request and initialise the LSUH2 PID controller */
	stPIDCB.enPIDRate = CTRLAPI_enPIDCall;
	stPIDCB.pi32Target = &LSUH_ai32PIDTarget[1];
	stPIDCB.pi32Feedback = &LSUH_ai32PIDFeedback[1];	
	stPIDCB.pi32Output = &LSUH_ai32PIDOutput[1];	
	USER_vSVC(SYSAPI_enInitialiseCTRLResource, (void*)&enCTRLType, (void*)&stPIDCB, NULL);	
	
	LSUH_atPIDLSUH1IDX[1] = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
				(CTRLAPI_ttPIDIDX)pstSVCDataStruct->tClientHandle : -1;		
	
	if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult)
	{
		*pu32Arg |= SYSAPI_CALL_MASK_FAIL_RES_INIT;	
	}		
			
	/* Initialise variables */
	LSUH_au32TEPMTimeout[0] = ~0;
	LSUH_au32TEPMTimeout[1] = ~0;	
	
	LSUH_au32HeaterDuty[0] = 80 * LSU_DUTY_RES;
	LSUH_au32HeaterDuty[1] = 80 * LSU_DUTY_RES;	
	
	LSUH_ai32PIDTarget[0] = 80000u;
	LSUH_ai32PIDTarget[1] = 80000u;	
	
	LSUH_au32ACADCDeltaFiltered[0] = 0;
	LSUH_au32ACADCDeltaFiltered[1] = 0;
	
	LSUH_au32HeaterLowADCDeltaFiltered[0] = 0;
	LSUH_au32HeaterLowADCDeltaFiltered[1] = 0;	
	
	LSUH_aboSensorReady[0] = FALSE;	
	LSUH_aboSensorReady[1] = TRUE;	

	LSU4X_atNernstSenseOhms[0] = 1000000u;
	LSU4X_atNernstSenseOhms[1] = 1000000u;	
}

void LSUH_vRun(uint32* const pu32Arg )
{
	uint32 u32Temp;
	uint32 u32MaxDuty;
	CTRLAPI_tenCTRLType enCTRLType;	
	ADCAPI_tenTrigger enTrigger = ADCAPI_enTrigger3;	
	uint32 u32LSUDeviceIDX;	
	IOAPI_tenEHIOResource enEHIOResourceTimer;	
	IOAPI_tenEHIOResource enEHIOResourceADC;
	static uint32 u32RunCount = 0;/*CR1_56*/;	
	
	if (0 == (u32RunCount++ % (uint32)LSUH_nCallsIn100Ms))/*CR1_57*/
	{
		LSUH_tHeaterEffMax = (LSUH_nHeffVLimit > LSUH_tHeaterEffMax) ?
						LSUH_tHeaterEffMax + (LSUH_nHeffVRampRate / LSUH_nRateHZ) : LSUH_tHeaterEffMax;	/*CR1_57*/
	}
	
	u32LSUDeviceIDX = u32RunCount % 2;

	enEHIOResourceTimer = (0 == u32LSUDeviceIDX ) ? EH_IO_TMR1 : EH_IO_TMR1;
	enEHIOResourceADC = (0 == u32LSUDeviceIDX ) ? EH_IO_ADD8 : EH_IO_ADD9;				
	
	/* Catch a lost TEPM for heater */
	if (10000 < LSUH_au32TEPMTimeout[u32LSUDeviceIDX]++)
	{
		LSUH_vInitiateTEPMQueue(enEHIOResourceTimer, 0u);
	}			
		
	/* ADC pending flags are expected to be syncronised... */
	if ((TRUE == LSUH_aboADCQueuePending[0] ||
			(TRUE == LSUH_aboADCQueuePending[1])))
	{
		USER_vSVC(SYSAPI_enTriggerADQueue, (void*)&enTrigger, NULL, NULL);
		LSUH_aboADCQueuePending[0] = FALSE;
		LSUH_aboADCQueuePending[1] = FALSE;			
	}	
	
	/* Iterate the heater PID controller if new R value */
	if (TRUE == LSUH_aboNernstRNewValue[u32LSUDeviceIDX])
	{
		enCTRLType = CTRLAPI_enPID;					
		
		USER_vSVC(SYSAPI_enIterateCTRLResource, (void*)&enCTRLType, (void*)&LSUH_atPIDLSUH1IDX[u32LSUDeviceIDX], NULL);	
		LSUH_aboNernstRNewValue[u32LSUDeviceIDX] = FALSE;
	}
	
	LSUH_atHeaterSenseVolts[u32LSUDeviceIDX] = CONV_tADCToVolts(
							enEHIOResourceADC, LSUH_au32HeaterLowADCDeltaFiltered[u32LSUDeviceIDX]);
	
	LSUH_atHeaterAmps[u32LSUDeviceIDX] = CONV_tOhmsVoltsToAmps(LSUH_nHeaterSenseOhms, LSUH_atHeaterSenseVolts[u32LSUDeviceIDX]);
	LSUH_atHeaterOhms[u32LSUDeviceIDX] = CONV_tVoltsAmpsToOhms(BVM_tBattVolts - LSUH_atHeaterSenseVolts[u32LSUDeviceIDX] - 800u, 
																																			LSUH_atHeaterAmps[u32LSUDeviceIDX]);
	
	LSUH_ai32PIDFeedback[u32LSUDeviceIDX] = LSU4X_atNernstSenseOhms[u32LSUDeviceIDX];
	LSUH_atHeaterPower[u32LSUDeviceIDX] = (10000000 - (3 * LSUH_ai32PIDOutput[u32LSUDeviceIDX])) / 1000;
	
	/* Calculate the maximum duty cycle sustained by the current sense resistors */
	u32MaxDuty = (1000000u * (uint32)LSUH_nMaxDutyDenom) / LSUH_atHeaterAmps[u32LSUDeviceIDX];
	u32MaxDuty /= LSUH_atHeaterAmps[u32LSUDeviceIDX];
	
	/* Calculated the duty cycle to deliver requested power (n = Preq / VI))*/
	u32Temp = LSUH_atHeaterPower[u32LSUDeviceIDX] * 1000u;	
	u32Temp /= (BVM_tBattVolts - LSUH_atHeaterSenseVolts[u32LSUDeviceIDX] - 800u);
	u32Temp *= 1000;
	u32Temp /= LSUH_atHeaterAmps[u32LSUDeviceIDX];
	u32Temp = (u32MaxDuty > u32Temp) ? u32Temp : u32MaxDuty;		
	u32Temp = (LSUH_nMaxDuty > u32Temp) ? u32Temp : LSUH_nMaxDuty;
	u32Temp = (LSUH_nMinDuty < u32Temp) ? u32Temp : LSUH_nMinDuty;	
	LSUH_au32HeaterDuty[u32LSUDeviceIDX] = u32Temp;
	
	/* Max duty cycle until sensor ready */
	LSUH_au32HeaterDuty[u32LSUDeviceIDX] = (FALSE == LSUH_aboSensorReady[u32LSUDeviceIDX]) ? LSUH_nMaxDuty : LSUH_au32HeaterDuty[u32LSUDeviceIDX];
	
	/* Limit to max heater power during startup */
	u32Temp = LSUH_tHeaterEffMax * LSUH_tHeaterEffMax;
	u32Temp /= (BVM_tBattVolts - LSUH_atHeaterSenseVolts[u32LSUDeviceIDX] - 800u);
	u32Temp *= 1000u;
	u32Temp /= (BVM_tBattVolts - LSUH_atHeaterSenseVolts[u32LSUDeviceIDX] - 800u);
	
	LSUH_u32HeaterDutyMax = u32Temp;
	LSUH_au32HeaterDuty[u32LSUDeviceIDX] = MIN(LSUH_au32HeaterDuty[u32LSUDeviceIDX], LSUH_u32HeaterDutyMax);
}

void LSUH_vTerminate(uint32* const pu32Arg )
{

}

void LSUH_vCallBack(uint32* const pu32Arg )
{

}

static void LSUH_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	uint16 u16ADCDelta;
	uint32 u32Temp;/*CR1_65*/
	uint32 u32LSUDeviceIDX;
	
	switch (enEHIOResource)
	{
		case LSUH_nHMFInput:
		{
			u32LSUDeviceIDX = 0;
			break;
		}
		default:
		{
			u32LSUDeviceIDX = 0xffffffff;		
			break;			
		}
	}		
	
	/* Trap invalid timer error */
	if (LSU_DEVICE_COUNT <= u32LSUDeviceIDX) return;
	
	if (TRUE == LSUH_aboHeaterIsOn[u32LSUDeviceIDX])
	{
		LSUH_au32HeaterLowADCOnRaw[u32LSUDeviceIDX] = u32ADCResult;	
		u32Temp = (LSUH_au32HeaterLowADCOnRaw[u32LSUDeviceIDX] > LSUH_au32HeaterLowADCOffRaw[u32LSUDeviceIDX]) ?
		LSUH_au32HeaterLowADCOnRaw[u32LSUDeviceIDX] - LSUH_au32HeaterLowADCOffRaw[u32LSUDeviceIDX] : 0;
		
		u32Temp = MIN(u32Temp, 0xffff);/*CR1_65*/
		u16ADCDelta = (uint16)u32Temp;/*CR1_65*/
		
		(void)USERMATH_u32SinglePoleLowPassFilter(u16ADCDelta, LSUH_nHeatCurrFilt,
					&LSUH_au32HeaterLowADCDeltaFiltered[u32LSUDeviceIDX]);
	}
	else
	{
		//matthew hack
		//LSUH_u32HeaterLowADCOffRaw[u32LSUDeviceIDX] = u32ADCResult;	
		LSUH_au32HeaterLowADCOffRaw[u32LSUDeviceIDX] = 4;			
	}		
}

void LSUH_vTEPMCallBackLow(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	uint32 u32SampleCount;
	uint32 u32SampleDiscard;
	puint32 pu32SampleArray;
	uint32 u32LSUDeviceIDX;
	uint32 u32SampleIDX;
	bool boSamplesOK = TRUE;
	IOAPI_tenEHIOResource enEHIOResourceAD;	
	
	switch (enEHIOResource)
	{
		case EH_IO_TMR2:
		{
			u32LSUDeviceIDX = 0;
			enEHIOResourceAD = EH_IO_ADD5;
			break;
		}
		case EH_IO_TMR1:
		{
			u32LSUDeviceIDX = 1;
			enEHIOResourceAD = EH_IO_ADD7;			
			break;	
		}
		default:
		{
			u32LSUDeviceIDX = 0xffffffff;		
			break;			
		}
	}		
	
	/* Trap invalid timer error */
	if (LSU_DEVICE_COUNT <= u32LSUDeviceIDX) return;
		
	pu32SampleArray = (puint32)(&LSU4X_aau32ACSamples[u32LSUDeviceIDX][0]);
	
	/* Limit the amount of samples - use the last 8 samples */
	if (8 < LSU4X_au32ACSampleCount[u32LSUDeviceIDX])
	{
		pu32SampleArray += (LSU4X_au32ACSampleCount[u32LSUDeviceIDX] - 8);
		LSU4X_au32ACSampleCount[u32LSUDeviceIDX] = 8;		
	}		
		
	u32SampleCount = LSU4X_au32ACSampleCount[u32LSUDeviceIDX];
	u32SampleCount = MAX(u32SampleCount, 2u);
	u32SampleDiscard = (u32SampleCount - 1) >> 1;
	
	LSUH_aboADCQueuePending[u32LSUDeviceIDX] = TRUE;
	LSUH_aboHeaterIsOn[u32LSUDeviceIDX] = FALSE;
	LSU4X_au8ACCallBackWait[u32LSUDeviceIDX] = 0;	
	LSUH_au32ACADCDelta[u32LSUDeviceIDX] = USERMATH_u32DiscardAndAverage32(pu32SampleArray, u32SampleCount, u32SampleDiscard);
	
	if ((LSUH_nHeaterOhmsSamples - 1) > LSUH_au32ACSampleCounter[u32LSUDeviceIDX])
	{
		LSUH_aau32ADADCSamples[u32LSUDeviceIDX][LSUH_au32ACSampleCounter[u32LSUDeviceIDX]] = LSUH_au32ACADCDelta[u32LSUDeviceIDX];
		LSUH_au32ACSampleCounter[u32LSUDeviceIDX]++;		
	}
	else
	{		
		u32SampleCount = LSUH_nHeaterOhmsSamples;
		u32SampleDiscard = u32SampleCount >> 1;		
		LSUH_aau32ADADCSamples[u32LSUDeviceIDX][LSUH_au32ACSampleCounter[u32LSUDeviceIDX]] = LSUH_au32ACADCDelta[u32LSUDeviceIDX];		
		LSUH_au32ACADCDeltaFiltered[u32LSUDeviceIDX] = USERMATH_u32DiscardAndAverage32(&LSUH_aau32ADADCSamples[u32LSUDeviceIDX][0], u32SampleCount, u32SampleDiscard);
		
		for (u32SampleIDX = 0; u32SampleIDX < LSUH_nHeaterOhmsSamples; u32SampleIDX++)
		{
			if ((LSUH_nMinACSample > LSUH_aau32ADADCSamples[u32LSUDeviceIDX][u32SampleIDX]) ||
					(LSUH_nMaxACSample < LSUH_aau32ADADCSamples[u32LSUDeviceIDX][u32SampleIDX]))
			{
				boSamplesOK	= FALSE;
			}
		}
		
		if (TRUE == boSamplesOK)
		{
			LSU4X_atNernstSenseOhms[u32LSUDeviceIDX] = CONV_tADCToOhms(enEHIOResourceAD, LSUH_au32ACADCDeltaFiltered[u32LSUDeviceIDX]);	
			LSUH_aboNernstRNewValue[u32LSUDeviceIDX] = true;
		}
		
		LSUH_au32ACSampleCounter[u32LSUDeviceIDX] = 0;		
	}
}

void LSUH_vTEPMCallBackHigh(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	uint32 u32LSUDeviceIDX;
	
	switch (enEHIOResource)
	{
		case EH_IO_TMR3:
		{
			u32LSUDeviceIDX = 0;
			break;
		}
		case EH_IO_TMR4:
		{
			u32LSUDeviceIDX = 1;
			break;	
		}
		default:
		{
			u32LSUDeviceIDX = 0xffffffff;		
			break;			
		}
	}		
	
	/* Trap invalid timer error */
	if (LSU_DEVICE_COUNT <= u32LSUDeviceIDX) return;	
	
	LSUH_aboADCQueuePending[u32LSUDeviceIDX] = TRUE;	
	LSUH_aboHeaterIsOn[u32LSUDeviceIDX] = TRUE;	
	LSUH_vInitiateTEPMQueue(enEHIOResource, tEventTime);	
}

static void LSUH_vInitiateTEPMQueue(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	TEPMAPI_ttEventCount tEventCount;
	uint16 u16LongPhase;
	uint16 u16ShortPhase;
	sint16 s16PLLTrim;
	uint32 u32LSUDeviceIDX;
	
	switch (enEHIOResource)
	{
		case EH_IO_TMR5:
		{
			u32LSUDeviceIDX = 0;
			break;
		}
		case EH_IO_TMR6:
		{
			u32LSUDeviceIDX = 1;
			break;	
		}
		default:
		{
			u32LSUDeviceIDX = 0xffffffff;		
			break;			
		}
	}		
	
	/* Trap invalid timer error */
	if (LSU_DEVICE_COUNT <= u32LSUDeviceIDX) return;	
	
	/* Calculate the phase error between AC waveform and heater duty start */
	u16LongPhase = (uint16)tEventTime - LSU4X_u16PWMLastStart;
	u16ShortPhase = u16LongPhase % (LSU4X_u16PWMPeriod / 10);
	
	/* Calculate the required PLL trim */
	s16PLLTrim = (LSU4X_u16PWMPeriod / 20) < u16ShortPhase ?  
				(sint16)(u16ShortPhase - (LSU4X_u16PWMPeriod / 10)) :
				(sint16)u16ShortPhase;
	
	s16PLLTrim /= 4;	
	tEventCount = 2;
	
	LSUH_aastTimedUserEvents[u32LSUDeviceIDX][0].enAction = TEPMAPI_enSetLow;
	LSUH_aastTimedUserEvents[u32LSUDeviceIDX][0].enMethod = TEPMAPI_enHardLinkedTimeStep;
	LSUH_aastTimedUserEvents[u32LSUDeviceIDX][0].tEventTime = 
		((uint16)(LSU4X_u16PWMPeriod - s16PLLTrim) * LSUH_au32HeaterDuty[u32LSUDeviceIDX]) / (100ul * LSU_DUTY_RES);
	LSUH_aastTimedUserEvents[u32LSUDeviceIDX][0].pfEventCB = &LSUH_vTEPMCallBackLow;		

	LSUH_aastTimedUserEvents[u32LSUDeviceIDX][1].enAction = TEPMAPI_enSetHigh;
	LSUH_aastTimedUserEvents[u32LSUDeviceIDX][1].enMethod = TEPMAPI_enHardLinkedTimeStep;
	LSUH_aastTimedUserEvents[u32LSUDeviceIDX][1].tEventTime = 
		((uint16)(LSU4X_u16PWMPeriod - s16PLLTrim) * (100ul * LSU_DUTY_RES - LSUH_au32HeaterDuty[u32LSUDeviceIDX]) / (100ul * LSU_DUTY_RES));
	LSUH_aastTimedUserEvents[u32LSUDeviceIDX][1].pfEventCB = &LSUH_vTEPMCallBackHigh;
	
	USER_vSVC(SYSAPI_enAppendTEPMQueue, (void*)&enEHIOResource, 
		(void*)&LSUH_aastTimedUserEvents[u32LSUDeviceIDX][0], (void*)&tEventCount);
	
	LSUH_au32TEPMTimeout[u32LSUDeviceIDX] = 0;
}

#endif //BUILD_USER




