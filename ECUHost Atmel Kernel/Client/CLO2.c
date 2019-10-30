/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      CLO2                                                   */
/* DESCRIPTION:        This code module provides closed loop fuel functions   */
/* FILE NAME:          CLO2.c                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _CLO2_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "CLO2.h"
#include "FUEL.h"

/* LOCAL MACRO DEFINITIONS ****************************************************/

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 CLO2_u32SensorStateBank[2];
Bool CLO2_boSensorState[2];
uint32 CLO2_u32ReadyCount[2];

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/

/* LOCAL CONSTANT DEFINITIONS (STATIC) ****************************************/

/* GLOBAL FUNCTION DEFINITIONS ************************************************/

void CLO2_vInit(void)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength;
	uint32 u32Arg;

    CLO2_u32STT[0] = 1000;
    CLO2_u32STT[1] = 1000;

	CLO2_s32ISCESTTrim[0] = 0;
	CLO2_s32ISCESTTrim[1] = 0;

	CLO2_au16AFRMeasured[0] = ~0;
	CLO2_au16AFRMeasured[1] = ~0;

	/* Enable the Oxygen sensor bank 1 discrete input sensor type */
	enEHIOResource = CLO2_nOxBank1Pin;
	enEHIOType = IOAPI_enDIOInput;
	enDriveStrength = IOAPI_enStrong;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, &u32Arg);

	/* Enable the Oxygen sensor bank 2 discrete input sensor type */
	enEHIOResource = CLO2_nOxBank2Pin;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, &u32Arg);
}

void CLO2_vFilterSensors(void)
{
	CLO2_boSensorState[0] = SETUP_vGetDigitalIO(CLO2_nOxBank1Pin);

	if (TRUE == CLO2_boSensorState[0])
	{
		CLO2_u32SensorStateBank[0] = (7 * CLO2_u32SensorStateBank[0]) / 8;
	}
	else
	{
		CLO2_u32SensorStateBank[0] = (7 * CLO2_u32SensorStateBank[0] + 1000) / 8;
	}

	CLO2_boSensorState[1] = SETUP_vGetDigitalIO(CLO2_nOxBank2Pin);

	if (TRUE == CLO2_boSensorState[1])
	{
		CLO2_u32SensorStateBank[1] = (7 * CLO2_u32SensorStateBank[1]) / 8;
	}
	else
	{
		CLO2_u32SensorStateBank[1] = (7 * CLO2_u32SensorStateBank[1] + 1000) / 8;
	}
}

void CLO2_vRun(void)
{
	static Bool aboSTTRichening[2];
	static Bool aboSimRichening[2];
	volatile static uint8 au8STTLTT[2];
	volatile static uint8 au8Count[2];
	volatile static uint8 au8CountRichLimit[2];
	volatile static uint8 au8CountLeanLimit[2];
	static Bool aboOutReady[2];
	static Bool aboOutConfigured[2];
	IOAPI_tenEHIOResource enEHIOResource = EH_IO_Invalid;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenDriveStrength enDriveStrength;
	uint32 u32Arg;
	uint32 u32Temp;
	uint32 u32SeqIDX;
	Bool boFuelCutActive = FALSE;
	const uint8_t au8BiasTable[256] = CLO2_nBiasTableData;
	uint32 u32CLStep = TRUE ==  TPS_boThrottleClosed ? USERCAL_stRAMCAL.u32CLStepClosed : USERCAL_stRAMCAL.u32CLStepOpen;
	uint32 u32CLInc = TRUE == TPS_boThrottleClosed ? USERCAL_stRAMCAL.u32CLIncClosed : USERCAL_stRAMCAL.u32CLIncOpen;

	if ((0 != USERCAL_stRAMCAL.boUEGOCANPrimary) && (TRUE == SENSORS_aboCANUEGONewSample[0]))
	{
		CLO2_au16AFRMeasured[0] = SENSORS_au16CANAFR[0];

		/* Buffer the auto-tune diag variable */
		CLO2_u32AutoTuneVE = CLO2_au16AFRMeasured[0] + (FUEL_u16TAFR << 16);
		SENSORS_aboCANUEGONewSample[0] = FALSE;
	}

	if ((FALSE == USERCAL_stRAMCAL.u8CLO2Leftenable) && (FALSE == USERCAL_stRAMCAL.u8CLO2Leftenable))
	{
		CLO2_u32STT[0] = 1000;
		CLO2_u32STT[1] = 1000;
		return;
	}

	CLO2_u32ReadyCount[0] = (100000 > CLO2_u32ReadyCount[0]) && (0 < CAM_u32RPMRaw) ? 
			CLO2_u32ReadyCount[0] + 1 : CLO2_u32ReadyCount[0];
	CLO2_u32ReadyCount[1] = (100000 > CLO2_u32ReadyCount[1]) && (0 < CAM_u32RPMRaw) ? 
			CLO2_u32ReadyCount[1] + 1 : CLO2_u32ReadyCount[1];

	for (u32SeqIDX = 0; u32SeqIDX < 8; u32SeqIDX++)
	{
		boFuelCutActive |= FUEL_aboSeqOverrunCutFlag[u32SeqIDX];
	}

	if (200 < CLO2_u32ReadyCount[0])
	{
		if (((FUEL_u16TAFR < 1475) && (FUEL_u16TAFR > 1465)) &&
			(FALSE == boFuelCutActive))
		{
			if (800 < CLO2_u32SensorStateBank[0])
			{
				if ((TRUE == aboSTTRichening[0]) && (USERCAL_stRAMCAL.u32STTNegLimit < CLO2_u32STT[0]))  CLO2_u32STT[0] -= u32CLStep;

				CLO2_u32STT[0] = USERCAL_stRAMCAL.u32STTNegLimit < CLO2_u32STT[0] ? CLO2_u32STT[0] - u32CLInc : CLO2_u32STT[0];
				aboSTTRichening[0] = FALSE;
			}
			else if (200 > CLO2_u32SensorStateBank[0])
			{
				if ((FALSE == aboSTTRichening[0]) && (USERCAL_stRAMCAL.u32STTPosLimit > CLO2_u32STT[0])) CLO2_u32STT[0] += u32CLStep;

				CLO2_u32STT[0] = USERCAL_stRAMCAL.u32STTPosLimit > CLO2_u32STT[0] ? CLO2_u32STT[0] + u32CLInc : CLO2_u32STT[0];
				aboSTTRichening[0] = TRUE;
			}
		}
		else
		{
			CLO2_u32STT[0] = 1000;
		}

		aboOutReady[0] = TRUE;

		/* If no CAN data yet then just set to neutral */
		SENSORS_u8CANSTT[0] = 0xff != SENSORS_u8CANSTT[0] ? SENSORS_u8CANSTT[0] : 0x80;
		SENSORS_u8CANLTT[0] = 0xff != SENSORS_u8CANLTT[0] ? SENSORS_u8CANLTT[0] : 0x80;
	}


	if (200 < CLO2_u32ReadyCount[1])
	{
		if (((FUEL_u16TAFR < 1475) && (FUEL_u16TAFR > 1465)) &&
			(FALSE == boFuelCutActive))
		{
			if (800 < CLO2_u32SensorStateBank[1])
			{
				if ((TRUE == aboSTTRichening[1]) && (USERCAL_stRAMCAL.u32STTNegLimit < CLO2_u32STT[1]))  CLO2_u32STT[1] -= u32CLStep;

				CLO2_u32STT[1] = USERCAL_stRAMCAL.u32STTNegLimit < CLO2_u32STT[1] ? CLO2_u32STT[1] - u32CLInc : CLO2_u32STT[1];
				aboSTTRichening[1] = FALSE;
			}
			else if (200 > CLO2_u32SensorStateBank[1])
			{
				if ((FALSE == aboSTTRichening[1]) && (USERCAL_stRAMCAL.u32STTPosLimit > CLO2_u32STT[1])) CLO2_u32STT[1] += u32CLStep;

				CLO2_u32STT[1] = USERCAL_stRAMCAL.u32STTPosLimit > CLO2_u32STT[1] ? CLO2_u32STT[1] + u32CLInc : CLO2_u32STT[1];
				aboSTTRichening[1] = TRUE;
			}
		}
		else
		{
			CLO2_u32STT[1] = 1000;
		}

		aboOutReady[1] = TRUE;

		/* If no CAN data yet then just set to neutral */
		SENSORS_u8CANSTT[1] = 0xff != SENSORS_u8CANSTT[1] ? SENSORS_u8CANSTT[1] : 0x80;
		SENSORS_u8CANLTT[1] = 0xff != SENSORS_u8CANLTT[1] ? SENSORS_u8CANLTT[1] : 0x80;
	}


#ifdef BUILD_BSP_STT_CAN
	if ((0xff != SENSORS_u8CANSTT[0]) && (0xff != SENSORS_u8CANLTT[0]))
	{
		au8STTLTT[0] = 2 * SENSORS_u8CANSTT[0] + SENSORS_u8CANLTT[0];

		au8CountRichLimit[0] = (au8BiasTable[au8STTLTT[0]]) / 15;
		au8CountLeanLimit[0] = (256 - au8BiasTable[au8STTLTT[0]]) / 15;

		if (TRUE == aboSimRichening[0])
		{
			if (++au8Count[0] > au8CountRichLimit[0])
			{
				if ((TRUE == aboOutConfigured[0]) &&
				(FUEL_u16TAFR > 1465))
				{
					SETUP_vSetDigitalIOLow(CLO2_nSwitchBank1Pin);
				}
				au8Count[0] = 0;
				aboSimRichening[0] = FALSE;
			}
		}
		else
		{
			if (++au8Count[0] > au8CountLeanLimit[0])
			{
				if (TRUE == aboOutConfigured[0])
				{
					SETUP_vSetDigitalIOHigh(CLO2_nSwitchBank1Pin);
				}

				au8Count[0] = 0;
				aboSimRichening[0] = TRUE;
			}
		}
	}

	if ((0xff != SENSORS_u8CANSTT[1]) && (0xff != SENSORS_u8CANLTT[1]))
	{
		au8STTLTT[1] = 2 * SENSORS_u8CANSTT[1] + SENSORS_u8CANLTT[1];

		au8CountRichLimit[1] = (au8BiasTable[au8STTLTT[1]]) / 15;
		au8CountLeanLimit[1] = (256 - au8BiasTable[au8STTLTT[1]]) / 15;

		if (TRUE == aboSimRichening[1])
		{
			if (++au8Count[1] > au8CountRichLimit[1])
			{
				if ((TRUE == aboOutConfigured[1]) &&
					(FUEL_u16TAFR > 1465))
				{
					SETUP_vSetDigitalIOLow(CLO2_nSwitchBank2Pin);
				}
				au8Count[1] = 0;
				aboSimRichening[1] = FALSE;
			}
		}
		else
		{
			if (++au8Count[1] > au8CountLeanLimit[1])
			{
				if (TRUE == aboOutConfigured[1])
				{
					SETUP_vSetDigitalIOHigh(CLO2_nSwitchBank2Pin);
				}

				au8Count[1] = 0;
				aboSimRichening[1] = TRUE;
			}
		}
	}

	CLO2_u32STTAvg[0] = ((3 * CLO2_u32STTAvg[0]) / 4) + CLO2_u32STT[0] / 4;
	CLO2_u32STTAvg[1] = ((3 * CLO2_u32STTAvg[1]) / 4) + CLO2_u32STT[1] / 4;

	CLO2_u32STTAvg[0] =  CLO2_u32STT[0] > CLO2_u32STTAvg[0] ? CLO2_u32STTAvg[0] + 1 : CLO2_u32STTAvg[0];
	CLO2_u32STTAvg[1] =  CLO2_u32STT[1] > CLO2_u32STTAvg[1] ? CLO2_u32STTAvg[1] + 1 : CLO2_u32STTAvg[1];
	

	if ((FALSE == aboOutConfigured[0]) && (TRUE == aboOutReady[0]))
	{
		enEHIOResource = CLO2_nSwitchBank1Pin;
		enEHIOType = IOAPI_enDIOOutput;
		enDriveStrength = IOAPI_enStrong;
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, &u32Arg);
		aboOutConfigured[0] = TRUE;
	}

	if ((FALSE == aboOutConfigured[1]) && (TRUE == aboOutReady[1]))
	{
		enEHIOResource = CLO2_nSwitchBank2Pin;
		enEHIOType = IOAPI_enDIOOutput;
		enDriveStrength = IOAPI_enStrong;
		SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, &u32Arg);
		aboOutConfigured[1] = TRUE;
	}
#endif

	if (TRUE == TPS_boThrottleClosed)
	{
		if (1000 < CLO2_u32STT[0])
		{
			CLO2_s32ISCESTTrim[0] = 
				~(((CLO2_u32STT[0] - 1000) * USERCAL_stRAMCAL.u32CLO2ESTTrimPos) / (1000 - USERCAL_stRAMCAL.u32STTPosLimit));
		}
		else
		{
			CLO2_s32ISCESTTrim[0] = 
				(((1000 - CLO2_u32STT[0]) * USERCAL_stRAMCAL.u32CLO2ESTTrimNeg) / (1000 - USERCAL_stRAMCAL.u32STTNegLimit));
		}
	}
	else
	{
		CLO2_s32ISCESTTrim[0] = 0;
	}

	if ((TRUE == TPS_boThrottleClosed) && (IAC_enClosedLoop == IAC_enControlState))
	{
		if (CLO2_u32STTAvg[1] < CLO2_u32STT[1])
		{
			u32Temp = (((CLO2_u32STT[1] - CLO2_u32STTAvg[1]) * USERCAL_stRAMCAL.u32CLO2ESTTrimPos) / 5);
			u32Temp = USERCAL_stRAMCAL.u32CLO2ESTTrimPos < u32Temp ? USERCAL_stRAMCAL.u32CLO2ESTTrimPos : u32Temp;
			CLO2_s32ISCESTTrim[1] = ~u32Temp;
		}
		else
		{
			u32Temp = (((CLO2_u32STTAvg[1] - CLO2_u32STT[1]) * USERCAL_stRAMCAL.u32CLO2ESTTrimNeg) / 5);
			u32Temp = USERCAL_stRAMCAL.u32CLO2ESTTrimNeg < u32Temp ? USERCAL_stRAMCAL.u32CLO2ESTTrimNeg : u32Temp;
			CLO2_s32ISCESTTrim[1] = u32Temp;
		}
	}
	else
	{
		CLO2_s32ISCESTTrim[1] = 0;
	}
}

#endif //BUILD_USER



