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


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 IAC_u32TargetStep;
uint32 IAC_u32FeedbackStep;
IAC_tenStepperState IAC_enStepperState;
uint32 IAC_u32StepCounter;
uint32 IAC_u32StepCounterLimit;
uint8 IAC_u8StepArray[4];
uint32 IAC_u32IdleEntryTimer;
uint32 IAC_u32RPMLongAverage;
IAC_tenControlState IAC_enControlState;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void IAC_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength = IOAPI_enWeak;	

	IAC_u32TargetStep = 0;
	IAC_u32FeedbackStep = 0;
	IAC_enStepperState = IAC_enResetHome;
	IAC_u32StepCounter = 0;
	IAC_u32StepCounterLimit = 0;	
	IAC_u32IdleEntryTimer = 0;
	IAC_u32RPMLongAverage = 0;
	IAC_enControlState = IAC_enOpenLoop;
		
	enEHIOResource = EH_IO_ADD1;
	enEHIOType = IOAPI_enDIOOutput;	
	enDriveStrength = IOAPI_enStrong;			
			
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = EH_IO_ADD1;	
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = EH_IO_ADD1;	
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = EH_IO_ADD1;	
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
}

void IAC_vRun(puint32 const pu32Arg)
{
	IOAPI_tenTriState enTriState;	
	IOAPI_tenEHIOResource enEHIOResource;
	uint32 u32Temp;
	sint32 s32Temp;
	uint32 u32TargetRPM;
	uint32 u32IdleEntryRPM;
	static uint32 u32ISCCount;
	
	u32TargetRPM = (uint32)(1300 - CTS_tTempCFiltered / 200);
	u32IdleEntryRPM = u32TargetRPM + 500;
	
	if (500 < CAM_u32RPMRaw)
	{
		(void)USERMATH_u16SinglePoleLowPassFilter16((uint16)CAM_u32RPMRaw, 
			0x10, &IAC_u32RPMLongAverage);	
	}
	else
	{
		IAC_u32RPMLongAverage = 0x100 * CAM_u32RPMRaw;
	}
		
	if ((u32IdleEntryRPM > CAM_u32RPMRaw) && 
			(3000 > TPS_tThetaFiltered) &&
			(0 != CAM_u32RPMRaw))
	{
		s32Temp = (sint32)(IAC_u32RPMLongAverage / 0x100) - (sint32)CAM_u32RPMRaw;
		s32Temp = ABS(s32Temp);
		
		if (50 > s32Temp)
		{
			if (50 <	IAC_u32IdleEntryTimer)
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
			IAC_u32IdleEntryTimer	= 0;
		}
	}
	else
	{
		IAC_u32IdleEntryTimer	= 0;	
		IAC_enControlState = IAC_enOpenLoop;
	}
			
	if (IAC_enResetHome == IAC_enStepperState)
	{
		if (0 == IAC_u32StepCounterLimit)		
		{
			/* If we have just started the stepper reset sequence */
			IAC_u32StepCounter = 0;
			IAC_u32TargetStep = 0;	
			IAC_u32StepCounterLimit = 4 * USERCAL_stRAMCAL.u32UserStepperHomeSteps;
			IAC_u8StepArray[0] = USERCAL_stRAMCAL.aUserStepperCloseTable[0];
			IAC_u8StepArray[1] = USERCAL_stRAMCAL.aUserStepperCloseTable[1];
			IAC_u8StepArray[2] = USERCAL_stRAMCAL.aUserStepperCloseTable[2];
			IAC_u8StepArray[3] = USERCAL_stRAMCAL.aUserStepperCloseTable[3];			
		}
		else if (IAC_u32StepCounterLimit == IAC_u32StepCounter)
		{
			/* If the stepper reset sequence is complete */
			IAC_u32StepCounter = 0;
			IAC_u32TargetStep = (110000 - CTS_tTempCFiltered) / 4000;
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
			if (IAC_enOpenLoop == IAC_enControlState)
			{
				IAC_u32TargetStep = (110000 - CTS_tTempCFiltered) / 4000;	
			}				
			else
			{
				s32Temp = (sint32)u32TargetRPM - (sint32)CAM_u32RPMRaw;
				s32Temp = ABS(s32Temp);
				
				if (80 < s32Temp)
				{				
					if (u32TargetRPM > CAM_u32RPMRaw)
					{
						IAC_u32TargetStep = (64 > IAC_u32TargetStep) ? 
							IAC_u32TargetStep + 1 : IAC_u32TargetStep;									
					}
					else
					{
						u32ISCCount++;
						
						if (0 == u32ISCCount % 16)
						{
							IAC_u32TargetStep = (0 < IAC_u32TargetStep) ? 
								IAC_u32TargetStep - 1 : IAC_u32TargetStep;
						}
					}		
				}	
				else				
				{
					u32ISCCount = 0;
				}
			}
			
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
		enEHIOResource = EH_IO_ADD1;	
		enTriState = (0 == (0x01 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;			
		USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&enTriState,	(void*)NULL);	
		
		enEHIOResource = EH_IO_ADD1;		
		enTriState = (0 == (0x02 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;		
		USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&enTriState,	(void*)NULL);		
		
		enEHIOResource = EH_IO_ADD1;		
		enTriState = (0 == (0x04 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;			
		USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&enTriState,	(void*)NULL);	

		enEHIOResource = EH_IO_ADD1;		
		enTriState = (0 == (0x08 & IAC_u8StepArray[IAC_u32StepCounter & 0x03])) ? IOAPI_enLow : IOAPI_enHigh;
		
		USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
			(void*)&enTriState,	(void*)NULL);		
	
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

void IAC_vTerminate(puint32 const pu32Arg)
{

}


void IAC_vCallBack(puint32 const pu32Arg)
{

}

#endif //BUILD_USER
