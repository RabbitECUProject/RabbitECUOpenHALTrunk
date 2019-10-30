/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Idle Air Control                                       */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for idle air control           */
/*                                                                            */
/* FILE NAME:          IAC.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _IAC_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "IAC.h"
#include "usercal.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
#ifdef BUILD_BSP_IAC_STEPPER
uint32 IAC_u32TargetStep;
uint32 IAC_u32TargetStepNormalised;
uint32 IAC_u32FeedbackStep;
//ASAM mode=readvalue name="IAC Step" type=uint32 offset=0 min=0 max=255 m=1 b=0 units="dl" format=5.3 help="IAC Current Stepper Position"
uint32 IAC_u32StepsLearnedMin;
//ASAM mode=readvalue name="IAC Step Learned Min" type=uint32 offset=0 min=0 max=255 m=1 b=0 units="dl" format=5.3 help="IAC Learned Min Position"
IAC_tenStepperState IAC_enStepperState;
uint32 IAC_u32StepCounter;
uint32 IAC_u32StepCounterLimit;
uint8 IAC_u8StepArray[4];
#endif

uint32 IAC_u32IdleEntryTimer;
uint32 IAC_u32RPMLongAverage;
SPREADAPI_ttSpreadIDX IAC_tSpreadISCTargetIDX;
TABLEAPI_ttTableIDX IAC_tTableISCTargetIDX;
uint16 IAC_u16ISCTarget;
//ASAM mode=readvalue name="IAC Target RPM" type=uint16 offset=0 min=0 max=4095 m=1 b=0 units="dl" format=5.3 help="IAC Current Target RPM"
uint16 IAC_u16ISCTargetRamp;
Bool IAC_boRun;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
static uint32 IAC_u32GetOpenStepsTempNormalised(uint32 u32Temp);
static Bool IAC_boUnderStepMins(void);


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void IAC_vStart(puint32 const pu32Arg)
{
#ifdef BUILD_BSP_IAC_STEPPER
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength = IOAPI_enWeak;	

	IAC_u32TargetStep = 0;
	IAC_u32FeedbackStep = 0;
	IAC_enStepperState = IAC_enResetHome;
	IAC_u32StepCounter = 0;
	IAC_u32StepCounterLimit = 0;	

	enEHIOType = IOAPI_enDIOOutput;	
	enDriveStrength = IOAPI_enStrong;
			
	if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[0])	
	{
		enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[0];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}	
	
	if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[1])
	{
		enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[1];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}
	
	if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[2])
	{
		enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[2];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}
	
	if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[3])
	{
		enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[3];
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	}				


#endif

	IAC_u32IdleEntryTimer = 0;
	IAC_u32RPMLongAverage = 0;
	IAC_enControlState = IAC_enOpenLoop;

	/* Request and initialise required Kernel managed spread for ISC target spread*/
	IAC_tSpreadISCTargetIDX = SETUP_tSetupSpread((void*)&CTS_tTempCFiltered, (void*)&USERCAL_stRAMCAL.aUserISCSpeedTargetSpread , TYPE_enInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for ISC target */
	IAC_tTableISCTargetIDX = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserISCSpeedTargetTable, (void*)&IAC_u16ISCTarget, TYPE_enUInt16, 17, IAC_tSpreadISCTargetIDX, NULL);
}

void IAC_vRun(puint32 const pu32Arg)
{
	IOAPI_tenTriState enTriState;	
	IOAPI_tenEHIOResource enEHIOResource;
	uint32 u32Temp;
	sint32 s32Temp;
	static sint32 s32RPMErrOld;
	sint32 s32ErrDelta;
	sint32 s32ErrDeltaAbs;
	uint32 u32IdleEntryRPM;
	static uint32 u32ISCCount;
	static uint16 u16TargetRamp = 100;
	uint32 u32OpenLoopTargetStepNormalised;
	static uint32 u32OBDAdvFiltered = 500 * (0x80 + 40);
	uint32 u32TempClipped;
	static uint32 u32StepsLearnedCount;


	if (TRUE == IAC_boRun)
	{
		/* Calculate the current spread for ISC target */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&IAC_tSpreadISCTargetIDX,
		NULL, NULL);

		/* Lookup the current value for ISC target */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&IAC_tTableISCTargetIDX,
		NULL, NULL);

		IAC_boOverrunCutRPMEnable = (IAC_u16ISCTarget + USERCAL_stRAMCAL.u16OverrunCutEnableRPM) > CAM_u32RPMRaw ? FALSE : IAC_boOverrunCutRPMEnable; 
		IAC_boOverrunCutRPMEnable = (IAC_u16ISCTarget + USERCAL_stRAMCAL.u16OverrunCutDisableRPM) < CAM_u32RPMRaw ? TRUE : IAC_boOverrunCutRPMEnable; 
	
		u32IdleEntryRPM = IAC_u16ISCTarget + USERCAL_stRAMCAL.u16IdleEntryOffset;
		IAC_u16ISCTargetRamp = IAC_u16ISCTarget + u16TargetRamp;
	
		if (USERCAL_stRAMCAL.u16RPMRunThreshold < CAM_u32RPMRaw)
		{
			(void)USERMATH_u16SinglePoleLowPassFilter16((uint16)CAM_u32RPMRaw, 
				0x10, &IAC_u32RPMLongAverage);	
		}
		else
		{
			IAC_u32RPMLongAverage = 0x100 * CAM_u32RPMRaw;
		}
		
		if ((u32IdleEntryRPM > CAM_u32RPMRaw) && 
				(TRUE == TPS_boThrottleClosed) &&
				(0 != CAM_u32RPMRaw))
		{
			s32Temp = (sint32)(IAC_u32RPMLongAverage / 0x100) - (sint32)CAM_u32RPMRaw;
			s32Temp = ABS(s32Temp);
		
			if (300 > s32Temp)
			{
				if (10 < IAC_u32IdleEntryTimer)
				{				
					IAC_enControlState = IAC_enClosedLoop;
				}
				else
				{
					IAC_u32IdleEntryTimer++;	
				}
			}
			else
			{
				IAC_u32IdleEntryTimer = 0;
			}
		}
		else
		{
			IAC_u32IdleEntryTimer	= 0;	
			IAC_enControlState = IAC_enOpenLoop;
		}


		if (IAC_enClosedLoop == IAC_enControlState)
		{
			if (TRUE == USERCAL_stRAMCAL.boOBDISCADV)
			{
				if (TRUE == SENSORS_boOBDAdvNewSample)
				{
					s32Temp = 500 * SENSORS_u8OBDAdv;
					u32OBDAdvFiltered = ((7 * u32OBDAdvFiltered) / 8) + s32Temp / 8;

					if ((128 * 500) < s32Temp)
					{
						IAC_s32ISCESTTrim[0] = s32Temp - 128 * 500;
						IAC_s32ISCESTTrim[1] = s32Temp - 128 * 500;
					}
					else
					{
						IAC_s32ISCESTTrim[0] = 0;
						IAC_s32ISCESTTrim[1] = 0;
					}

					SENSORS_boOBDAdvNewSample = FALSE;
				}
				else
				{
					if (((sint32)u32OBDAdvFiltered - 128 * 500 + 2000) < IAC_s32ISCESTTrim[0])
					{
						IAC_s32ISCESTTrim[0] -= 2000;
						IAC_s32ISCESTTrim[1] -= 2000;
					}
					if (((sint32)u32OBDAdvFiltered - 128 * 500 - 2000) > IAC_s32ISCESTTrim[0])
					{
						IAC_s32ISCESTTrim[0] += 2000;
						IAC_s32ISCESTTrim[1] += 2000;
					}
				}
			}
			else
			{
				if ((IAC_u16ISCTarget - 80) > (uint16)CAM_u32RPMRaw)
				{
					u16TargetRamp = 100;
				}

				/* Hold the rpm error in s32Temp */
				s32Temp = (sint32)IAC_u16ISCTargetRamp - (sint32)CAM_u32RPMRaw;

				s32ErrDelta = s32Temp - s32RPMErrOld;
				s32ErrDeltaAbs = ABS(s32ErrDelta);

				if (0 <= s32Temp)
				{
					s32Temp = 40 < s32Temp ? s32Temp - 40 : 0;
				}
				else
				{
					s32Temp = -40 > s32Temp ? s32Temp + 40 : 0;
				}

				if (20 > s32ErrDeltaAbs)
				{
					s32ErrDelta = 0;
				}
				else
				{
					if (0 > s32ErrDelta)
					{
						s32ErrDelta += 20;
						s32ErrDelta *= 2;
					}
					else
					{
						s32ErrDelta -= 20;
						s32ErrDelta *= 2;
					}
				}

				s32Temp /= 4;
				s32Temp += s32ErrDelta;

				if ((0 == TPS_u16CANTPSDeltaNegCount) && (0 == TPS_u16CANTPSDeltaPosCount))
				{
					if (0 <= s32Temp)
					{
						CPU_xEnterCritical();
						IAC_s32ISCESTTrim[0] = (sint32)USERCAL_stRAMCAL.u32ISCESTTrimPos > (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) ? (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) : (sint32)USERCAL_stRAMCAL.u32ISCESTTrimPos;
						IAC_s32ISCESTTrim[1] = (sint32)USERCAL_stRAMCAL.u32ISCESTTrimPos > (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) ? (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) : (sint32)USERCAL_stRAMCAL.u32ISCESTTrimPos;
						CPU_xExitCritical();
					}
					else
					{
						CPU_xEnterCritical();
						IAC_s32ISCESTTrim[0] = ~(sint32)USERCAL_stRAMCAL.u32ISCESTTrimNeg < (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) ? (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) : (sint32)~USERCAL_stRAMCAL.u32ISCESTTrimNeg;
						IAC_s32ISCESTTrim[1] = ~(sint32)USERCAL_stRAMCAL.u32ISCESTTrimNeg < (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) ? (USERCAL_stRAMCAL.u16ESTCLGain * s32Temp) : (sint32)~USERCAL_stRAMCAL.u32ISCESTTrimNeg;
						CPU_xExitCritical();
					}	
				}
				else
				{
					if (0 != TPS_u16CANTPSDeltaNegCount)
					{
						IAC_s32ISCESTTrim[0] = 50 * TPS_u16CANTPSDeltaNegCount;
						IAC_s32ISCESTTrim[1] = 50 * TPS_u16CANTPSDeltaNegCount;
					}
					else
					{
						IAC_s32ISCESTTrim[0] = 0;
						IAC_s32ISCESTTrim[1] = 0;
					}
				}				
				s32RPMErrOld = (sint32)IAC_u16ISCTargetRamp - (sint32)CAM_u32RPMRaw;
			}

			u16TargetRamp = 0 < u16TargetRamp ? u16TargetRamp - 1 : 0;
		}
		else
		{
			IAC_s32ISCESTTrim[0] = 0;
			IAC_s32ISCESTTrim[1] = 0;

			u16TargetRamp = 100;
		}

		IAC_boRun = FALSE;
	}

	if (TRUE == USERCAL_stRAMCAL.u8StepperIACEnable)
	{
		if (IAC_enResetHome == IAC_enStepperState)
		{
			if (0 == IAC_u32StepCounterLimit)
			{
				/* If we have just started the stepper reset sequence */
				IAC_u32StepCounter = 0;
				IAC_u32TargetStepNormalised = 0;
				IAC_u32TargetStep = 0;
				IAC_u32StepCounterLimit = 4 * USERCAL_stRAMCAL.u32UserStepperHomeSteps;
				IAC_u32StepsLearnedMin = ~0;
				u32StepsLearnedCount = 0;
				IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperCloseTable[0];
				IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperCloseTable[1];
				IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperCloseTable[2];
				IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperCloseTable[3];
			}
			else if (IAC_u32StepCounterLimit == IAC_u32StepCounter)
			{
				/* If the stepper reset sequence is complete */
				IAC_u32StepCounter = 0;


				if (0 <= CTS_tTempCFiltered)
				{
					u32Temp = 100000 > CTS_tTempCFiltered ? CTS_tTempCFiltered : 100000;
				}
				else
				{
					u32Temp = 0;
				}

				IAC_u32TargetStepNormalised = IAC_u32GetOpenStepsTempNormalised(u32Temp);
				IAC_u32TargetStep = (USERCAL_stRAMCAL.u16IACStepsOpenCold * IAC_u32TargetStepNormalised) / 256;

				IAC_u32FeedbackStep = 0;
				IAC_u32StepCounterLimit = 4 * IAC_u32TargetStep;
				IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperOpenTable[0];
				IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperOpenTable[1];
				IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperOpenTable[2];
				IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperOpenTable[3];
				IAC_enStepperState = IAC_enNormal;
			}
		}
		else if (IAC_enNormal == IAC_enStepperState)
		{
			if (IAC_u32StepCounterLimit == IAC_u32StepCounter)
			{
				if (0 <= CTS_tTempCFiltered)
				{
					u32TempClipped = 100000 > CTS_tTempCFiltered ? CTS_tTempCFiltered : 100000;
				}
				else
				{
					u32TempClipped = 0;
				} 

				u32OpenLoopTargetStepNormalised = IAC_u32GetOpenStepsTempNormalised(u32TempClipped);

				if (IAC_enOpenLoop == IAC_enControlState)
				{
					IAC_u32TargetStepNormalised = u32OpenLoopTargetStepNormalised;
					u32StepsLearnedCount = 0;
				}
				else
				{
					/* Closed loop */
					s32Temp = (sint32)IAC_u16ISCTarget - (sint32)CAM_u32RPMRaw;
					s32Temp = ABS(s32Temp);
			
					if (60 < s32Temp)
					{
						u32ISCCount++;
						u32StepsLearnedCount = 0;
						
						if (IAC_u16ISCTarget > CAM_u32RPMRaw)
						{
							if (0 == (u32ISCCount % 4))
							{
								IAC_u32TargetStepNormalised = u32OpenLoopTargetStepNormalised > IAC_u32TargetStepNormalised ?
								IAC_u32TargetStepNormalised + 1 : IAC_u32TargetStepNormalised;
							}
						}
						else
						{		
							if (USERCAL_stRAMCAL.u16IACStepsOpenHot > ((IAC_u32TargetStepNormalised * USERCAL_stRAMCAL.u16IACStepsOpenHot) / 256))
							{
								if (250 < s32Temp)
								{
									if (false == IAC_boUnderStepMins())
									{
										if (0 == (u32ISCCount % 8))
										{
											IAC_u32TargetStepNormalised = (0 < IAC_u32TargetStepNormalised) ?
											IAC_u32TargetStepNormalised - 1 : IAC_u32TargetStepNormalised;
										}
									}
									else
									{
										/* Proceed very slowly to learn when under min learn */
										if (0 == (u32ISCCount % 16))
										{
											IAC_u32TargetStepNormalised = (0 < IAC_u32TargetStepNormalised) ?
											IAC_u32TargetStepNormalised - 1 : IAC_u32TargetStepNormalised;
										}
									}
								}
								else
								{
									if (false == IAC_boUnderStepMins())
									{
										if (0 == (u32ISCCount % 16))
										{
											IAC_u32TargetStepNormalised = (0 < IAC_u32TargetStepNormalised) ?
											IAC_u32TargetStepNormalised - 1 : IAC_u32TargetStepNormalised;
										}
									}
									else
									{
										/* Proceed very slowly to learn when under min learn */
										if (0 == (u32ISCCount % 64))
										{
											IAC_u32TargetStepNormalised = (0 < IAC_u32TargetStepNormalised) ?
											IAC_u32TargetStepNormalised - 1 : IAC_u32TargetStepNormalised;
										}
									}
								}
							}
							else
							{
								if (0 == (u32ISCCount % 16))
								{
									IAC_u32TargetStep = (0 < IAC_u32TargetStep) ?
									IAC_u32TargetStep - 1 : IAC_u32TargetStep;
								}
							}		
						}
					}
					else
					{
						u32ISCCount = 0;
						u32StepsLearnedCount++;

						if ((IAC_LEARN_COUNTS - 1) == (u32StepsLearnedCount % IAC_LEARN_COUNTS))
						{
							if (~0U == IAC_u32StepsLearnedMin)
							{
								IAC_u32StepsLearnedMin = IAC_u32FeedbackStep;
							}
							else
							{
								if (IAC_u32FeedbackStep < IAC_u32StepsLearnedMin)
								{
									if ((IAC_u32StepsLearnedMin - IAC_u32FeedbackStep) > (USERCAL_stRAMCAL.u16IACStepsOpenHot / 2))
									{
										IAC_u32StepsLearnedMin -= (USERCAL_stRAMCAL.u16IACStepsOpenHot / 2);
									}
									else
									{
										IAC_u32StepsLearnedMin = IAC_u32StepsLearnedMin - 1;
									}
								}
								else if (IAC_u32FeedbackStep > IAC_u32StepsLearnedMin)
								{
									IAC_u32StepsLearnedMin = IAC_u32FeedbackStep;
								}
							}
						}
					}
				}
		
				IAC_u32TargetStep = IAC_u32TargetStepNormalised * USERCAL_stRAMCAL.u16IACStepsOpenCold / 0x100;
				s32Temp = (sint32)IAC_u32TargetStep - (sint32)IAC_u32FeedbackStep;
		
				if (1 < (ABS(s32Temp)))
				{
					if (IAC_u32TargetStep < IAC_u32FeedbackStep)
					{
						IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperCloseTable[0];
						IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperCloseTable[1];
						IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperCloseTable[2];
						IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperCloseTable[3];
						IAC_u32StepCounterLimit = 4;
						IAC_u32StepCounter	= 0;
					}
					else
					{
						IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperOpenTable[0];
						IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperOpenTable[1];
						IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperOpenTable[2];
						IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperOpenTable[3];
						IAC_u32StepCounterLimit = 4;
						IAC_u32StepCounter	= 0;
					}
				}
			}
		}

		if (IAC_u32StepCounterLimit > IAC_u32StepCounter)
		{
			if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[0])	
			{
				enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[0];
				enTriState = (0 == (0x01 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
	
			if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[1])	
			{
				enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[1];
				enTriState = (0 == (0x02 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}
	
			if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[2])	
			{
				enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[2];
				enTriState = (0 == (0x04 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}

			if (EH_IO_Invalid > USERCAL_stRAMCAL.aIACIOResource[3])	
			{
				enEHIOResource = USERCAL_stRAMCAL.aIACIOResource[3];
				enTriState = (0 == (0x08 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;	
				USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
				(void*)&enTriState,	(void*)NULL);
			}

	
			IAC_u32StepCounter++;
	
			if (0 == (0x03 & IAC_u32StepCounter))
			{
				if (IAC_u32FeedbackStep > IAC_u32TargetStep)
				{
					IAC_u32FeedbackStep--;
				}
				else if (IAC_u32FeedbackStep < IAC_u32TargetStep)
				{
					IAC_u32FeedbackStep++;
				}
			}
		}
	}
}

static uint32 IAC_u32GetOpenStepsTempNormalised(uint32 u32Temp)
{
	static volatile uint32 u32OpenStepsNormalised;
	static volatile uint32 u32ColdNormalisedSteps = 255;
	static volatile uint32 u32HotNormalisedSteps;

	//u32HotNormalisedSteps = ((uint32)USERCAL_stRAMCAL.u16IACStepsOpenCold * 0x100ul) / (uint32)USERCAL_stRAMCAL.u16IACStepsOpenHot;
	u32HotNormalisedSteps = 50;
	u32OpenStepsNormalised = (((100000ul - u32Temp) * u32ColdNormalisedSteps) + (u32Temp * u32HotNormalisedSteps)) / 100000ul;

	return u32OpenStepsNormalised;
}

static Bool IAC_boUnderStepMins(void)
{
	Bool boUnderMins = IAC_u32StepsLearnedMin >= IAC_u32TargetStep;

	return boUnderMins;
}


void IAC_vTerminate(puint32 const pu32Arg)
{

}


void IAC_vCallBack(puint32 const pu32Arg)
{
	IAC_boRun = TRUE;
}

#endif //BUILD_USER
