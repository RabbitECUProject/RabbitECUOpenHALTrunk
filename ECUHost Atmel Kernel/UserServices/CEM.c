/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _CEM_C

#include "CEM.h"
#include "declarations.h"
#include "EST.h"//matthew
#include "SETUP.h"//matthew

uint16 CEM_au16RisingCrankEdge[80];
uint16 CEM_au16FallingCrankEdge[80];
uint16 CEM_au16AllEdge[160];
Bool CEM_aboSyncEdge[160];
uint16 CEM_au16SyncPoints[36];
uint8 CEM_u8RisingEdgesCount;
uint8 CEM_u8FallingEdgesCount;
uint32 CEM_u32CrankEdgeCounter;
uint32 CEM_u32ToothEdgeCounter;
uint32 CEM_u32CycleToothEdgeCounter;
Bool CEM_boCrankEdgeFirstRising;
uint32 CEM_u32CrankEdgeTimeoutCount;
uint8 CEM_au8AutocorrPeak[4];
uint8 CEM_au8ConfirmedAutocorrPeak[4];
IOAPI_tenEdgePolarity CEM_enEdgePolarity;
TEPMAPI_tstTimerMeasurements CEM_stTimerMeasurements[TEPMHA_nEventChannels];
Bool CEM_boEdgesReady;
uint32 CEM_au32TimerOffsets[9];
CEM_tenTriggerType CEM_enTriggerType;
uint8 CEM_u8SimpleMissingSync;
uint8 CEM_u8MissingToothCount;
uint8 CEM_u8MissingToothCountMax;
uint32 CEM_u32SmallestToothTime;
TEPMAPI_tstSimpleCamSync stSimpleCamSync;
extern IOAPI_tenEHIOResource TEPM_stTEPMVVTInput;
TEPMAPI_ttEventTime CEM_tLastVVTTimer;
TEPMAPI_ttEventTime CEM_tLastVVTHighTime;
TEPMAPI_ttEventTime CEM_tLastVVTLowTime;
uint32 CEM_u32EdgeCount = 0;

static void CEM_vSequenceReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint32 u32OriginGlobalCycleFraction, Bool boLatePhase);
static TEPMAPI_ttEventTime CEM_tCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction, Bool boGlobalTimeEnable);
static void CEM_vProcessAllEdges(void);
static void CEM_vPhaseError(uint32);
static Bool CEM_boGetLatePhaseSimpleCamSync(void);
static uint32 CEM_u32GetMissingThreshold(uint32);
static void CEM_vRunToothSchedulingOnTooth(void);

void CEM_vStart(uint32* const u32Stat)
{
	CEM_u32GlobalCycleFraction = 0;
	CEM_u32GlobalCycleTime = 0x0000ffff;
	CEM_u32GlobalCycleOriginCount = 0;
	CEM_u32CrankEdgeTimeoutCount = 0;
	CEM_tEventTimeLast = ~0;
	CEM_u32CrankEdgeCounter = 0xff;
	CEM_boEdgesReady = FALSE;
	CTS_boCTSReady = FALSE;
	CEM_u8SimpleMissingSync = 0;
	CEM_u32CrankErrorCounts = 0;
	//CEM_u8PhaseRepeats = 1;//matthew must be dynamic!!!
	
	memset((void*)&CEM_au16RisingCrankEdge, 0, sizeof(CEM_au16RisingCrankEdge)); 
	memset((void*)&CEM_au16FallingCrankEdge, 0, sizeof(CEM_au16FallingCrankEdge));	
	CEM_u8RisingEdgesCount = 0;
	CEM_u8FallingEdgesCount	= 0;
	CEM_u32SyncPoints = 0;
	CEM_boCrankEdgeFirstRising = FALSE;
	CEM_au8AutocorrPeak[0] = 128;
	CEM_au8AutocorrPeak[1] = 128;
	CEM_au8AutocorrPeak[2] = 128;
	CEM_au8AutocorrPeak[3] = 128;
	CEM_au8ConfirmedAutocorrPeak[0] = 128;
	CEM_au8ConfirmedAutocorrPeak[1] = 128;
	CEM_au8ConfirmedAutocorrPeak[2] = 128;
	CEM_au8ConfirmedAutocorrPeak[3] = 128;

	stSimpleCamSync.enEHIOResource = EH_IO_Invalid;
}

void CEM_vRun(uint32* const u32Stat)
{
	if (10 < CEM_u32CrankEdgeTimeoutCount)
	{
		CEM_tEventTimeLast = ~0;		
		CEM_u32GlobalCycleOriginCount = 0;	
		CEM_u32EdgeCount = 0;
	}
	else
	{
		CEM_u32CrankEdgeTimeoutCount++;
	}
}

void CEM_vTerminate(uint32* const u32Stat)
{

}

Bool CEM_boPopulateSyncPointsArray(puint16 const pu16EdgeFractionArray)
{
    Bool boRetVal = true;

    CEM_u32SyncPoints = 0;

	while (((0 != *(pu16EdgeFractionArray + CEM_u32SyncPoints)) && (36 > CEM_u32SyncPoints)) | (0 == CEM_u32SyncPoints))
	{
		CEM_au16SyncPoints[CEM_u32SyncPoints] = *(pu16EdgeFractionArray + CEM_u32SyncPoints);
		CEM_u32SyncPoints++;
	}

	CEM_u32SequenceFraction = 0x2000ul / CEM_u32SyncPoints;

	return boRetVal;
}

Bool CEM_boPopulateCrankEdgeArrays(puint16 const pu16EdgeFractionArray, const Bool boFirstRising, IOAPI_tenEdgePolarity enEdgePolarity, uint32_t u32TriggerType)
{
	uint32 u32EdgeCount = 0;
	Bool boStat = FALSE;
	uint8 u8EdgeCount = 0;
	CEM_u8RisingEdgesCount = 0;
	CEM_u8FallingEdgesCount = 0;
	
	CEM_boCrankEdgeFirstRising = boFirstRising;
	CEM_enEdgePolarity = enEdgePolarity;

	while (((0 != *(pu16EdgeFractionArray + u32EdgeCount)) && (80 > u8EdgeCount)) | (0 == u32EdgeCount))
	{
		if (IOAPI_enEdgeBoth == enEdgePolarity)
		{
	        if (TRUE == boFirstRising)
		    {
		        CEM_au16RisingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
			}
			else
		    {
			    CEM_au16FallingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
			}
			CEM_au16AllEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
			u32EdgeCount++;

	        if (TRUE == boFirstRising)
		    {
			    CEM_au16FallingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
			}
			else
		    {
			    CEM_au16RisingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
			}
			CEM_au16AllEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
			u32EdgeCount++;	
			CEM_u8RisingEdgesCount++;
			CEM_u8FallingEdgesCount++;
		}
		if (IOAPI_enEdgeFalling == enEdgePolarity)
		{
		    CEM_au16FallingCrankEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
		    CEM_au16AllEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
		    u32EdgeCount++;	
		    CEM_u8FallingEdgesCount++;
		}
		if (IOAPI_enEdgeRising == enEdgePolarity)
		{
		    CEM_au16RisingCrankEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
		    CEM_au16AllEdge[u32EdgeCount] = *(pu16EdgeFractionArray + u32EdgeCount);
		    u32EdgeCount++;	
		    CEM_u8RisingEdgesCount++;
		}
	}

	CEM_enTriggerType = (0 != u32TriggerType) ? CEM_enTypeSuzukiM15A + u32TriggerType - 1 : 0;
		
	CEM_vProcessAllEdges();
	boStat = TRUE;
	CEM_boEdgesReady = TRUE;
		
	return boStat;
}

static void CEM_vProcessAllEdges(void)
{
    uint32 u32OldData;
	uint32 u32DeltaCount = 0;
    sint32 s32Temp;

    for (uint32 u32ArrayIDX = 0; u32ArrayIDX < ((uint32)CEM_xEdgesCount - 1); u32ArrayIDX++)
	{
        u32OldData = CEM_au16AllEdge[u32ArrayIDX];
	    CEM_au16AllEdge[u32ArrayIDX] = CEM_au16AllEdge[u32ArrayIDX + 1] - u32OldData;
	}

	CEM_au16AllEdge[CEM_xEdgesCount - 1] = ~CEM_au16AllEdge[CEM_xEdgesCount - 1] + 1;

	CEM_aboSyncEdge[0] = TRUE;

    for (uint32 u32ArrayIDX = 1; u32ArrayIDX < CEM_xEdgesCount; u32ArrayIDX++)
	{
	    s32Temp = CEM_au16AllEdge[u32ArrayIDX] - CEM_au16AllEdge[u32ArrayIDX - 1];

        if (1 < s32Temp)
        {
            u32DeltaCount++;
			CEM_u8SimpleMissingSync = (u32ArrayIDX + 1) % CEM_xEdgesCount;

			/* Calculate how many missing teeth there are (+10) for safety */
			CEM_u8MissingToothCountMax = (CEM_au16AllEdge[u32ArrayIDX] + 10) / CEM_au16AllEdge[u32ArrayIDX - 1];
			CEM_u8MissingToothCountMax--;
        }

		CEM_aboSyncEdge[u32ArrayIDX] = TRUE;
	}

	if (CEM_enTriggerType < CEM_enTypeSuzukiM15A)
	{
		CEM_enTriggerType = (1 == u32DeltaCount) && (8 < CEM_xEdgesCount) ? CEM_enOneGroupMissing : CEM_enAutocorrelationMode;
	}

	if (CEM_enTriggerType == CEM_enOneGroupMissing)
	{
		CEM_aboSyncEdge[CEM_u8SimpleMissingSync] = FALSE;
		CEM_aboSyncEdge[CEM_u8SimpleMissingSync - 1] = FALSE;
	}
}

void CEM_vSetSyncPhaseRepeats(uint32 u32SyncPhaseRepeats)
{
    CEM_u8PhaseRepeats = (uint8)u32SyncPhaseRepeats;
}

TEPMAPI_ttEventTime CEM_ttGetModulePhase(uint32 u32ChannelIDX)
{
    TEPMAPI_ttEventTime tPhaseOffset = 0;

	tPhaseOffset = CEM_au32TimerOffsets[u32ChannelIDX];

    return tPhaseOffset;
}

void CEM_vPrimaryEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	uint16 u16LastGapFraction;
	TEPMAPI_ttEventTime tLastGapTime;
	IOAPI_tenTriState enTriState;
	static uint32 au32AutoCorr[CEM_nEdgesMax];
	static uint32 au32InputArray[CEM_nEdgesMax];
	static Bool aboConfirmedFlags[4] = {0, 0, 0, 0};
	static uint8 au8ErrorCount[4];
	uint8 u8PhasePrev;
	uint32 u32Temp;
	Bool boPhaseLocked = FALSE;
	static Bool boLatePhase;
	Bool boAutoCorrelationPolarityCheck = FALSE;
	static uint8 u8SameGapCount;
	
	//#define DEBUG_SYNC

	#ifdef DEBUG_SYNC
	volatile static uint32 temp1[16];
	volatile static uint32 temp2[16];
	volatile static uint32 tempcount;
	#endif

	if (FALSE == CEM_boEdgesReady) return;

	u8PhasePrev = CEM_u32CrankEdgeCounter;

	/* Sanity check the edges count */
	if (CEM_xEdgesCount > CEM_nEdgesMax) return;

	CEM_u32EdgeCount++;

	/* Sanity check edge polarity */
	if (IOAPI_enEdgeFalling == CEM_enEdgePolarity)
	{
		enTriState = TEPM_enGetTimerDigitalState(enEHIOResource);

		/* Invert the polarity because the circuit inverts */
		if (IOAPI_enLow == enTriState) return;
	}
	else if (IOAPI_enEdgeRising == CEM_enEdgePolarity)
	{
		enTriState = TEPM_enGetTimerDigitalState(enEHIOResource);

		/* Invert the polarity because the circuit inverts */
		if (IOAPI_enHigh == enTriState) return;
	}
	else
	{
		enTriState = TEPM_enGetTimerDigitalState(enEHIOResource);
		boAutoCorrelationPolarityCheck = TRUE;
	}

	CEM_au32TimerOffsets[8] = ((tstTimerModule*)TC2)->TC_CHANNEL[2].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
	CEM_au32TimerOffsets[7] = ((tstTimerModule*)TC2)->TC_CHANNEL[1].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
	CEM_au32TimerOffsets[6] = ((tstTimerModule*)TC2)->TC_CHANNEL[0].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
	CEM_au32TimerOffsets[5] = ((tstTimerModule*)TC1)->TC_CHANNEL[2].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
	CEM_au32TimerOffsets[4] = ((tstTimerModule*)TC1)->TC_CHANNEL[1].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
	CEM_au32TimerOffsets[3] = ((tstTimerModule*)TC1)->TC_CHANNEL[0].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
	CEM_au32TimerOffsets[2] = ((tstTimerModule*)TC0)->TC_CHANNEL[2].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
	CEM_au32TimerOffsets[1] = ((tstTimerModule*)TC0)->TC_CHANNEL[1].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
	CEM_au32TimerOffsets[0] = 0;


	/* Increment the confirmed channels */
	uint32 u32PhaseMin = 0;
	uint32 u32PhaseMax = (CEM_xEdgesCount / CEM_u8PhaseRepeats) - 1;
	

	if (CEM_enAutocorrelationMode == CEM_enTriggerType)
	{
		for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
		{
			if (TRUE == aboConfirmedFlags[u8ArrayIDX])
			{
				CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX]++;
				
				if (u32PhaseMax < CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX])
				{
					CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] -= (CEM_xEdgesCount / CEM_u8PhaseRepeats);
				}
			}
			else
			{
				CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] = 128;
			}
			
			u32PhaseMin += (CEM_xEdgesCount / CEM_u8PhaseRepeats);
			u32PhaseMax += (CEM_xEdgesCount / CEM_u8PhaseRepeats);
			u32PhaseMin = u32PhaseMin % CEM_xEdgesCount;
			u32PhaseMax = u32PhaseMax % CEM_xEdgesCount;
		}
	}


	if	(CEM_enAutocorrelationMode == CEM_enTriggerType)
	{
		/* Increment the edge counter if possible */
		for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
		{
			if (TRUE == aboConfirmedFlags[u8ArrayIDX])
			{
				CEM_u32CrankEdgeCounter = (CEM_u32CrankEdgeCounter + 1) % CEM_xEdgesCount;
				break;
			}
		}
	}

	u16LastGapFraction = CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount] / CEM_u8PhaseRepeats;
	tLastGapTime = CEM_tCalculateGlobalTime(tEventTime, u16LastGapFraction, CEM_aboSyncEdge[CEM_u32CrankEdgeCounter]);

	CEM_tEventTimeLast = tEventTime;
	CEM_tToothEventTimeLast = tEventTime;
	CEM_u32CrankEdgeTimeoutCount = 0;

	/* Shift the input array */
	puint32 pu32 = au32InputArray;
	pu32 += (CEM_nEdgesMax - CEM_xEdgesCount);
	memcpy(pu32, pu32 + 1, sizeof(uint32) * CEM_xEdgesCount);
	
	/* Buffer the new gap time */
	au32InputArray[CEM_nEdgesMax - 1] = tLastGapTime;
	
	/* If enough edges have arrived to proceed */
	if (CEM_xEdgesCount <= CEM_u32EdgeCount)
	{
		switch (CEM_enTriggerType)
		{
			case CEM_enAutocorrelationMode:
			{
				CEM_au8AutocorrPeak[0] = 0x80;
				CEM_au8AutocorrPeak[1] = 0x80;
				CEM_au8AutocorrPeak[2] = 0x80;
				CEM_au8AutocorrPeak[3] = 0x80;
				
				/* Calculate the autocorrelation array */
				memset((void*)&au32AutoCorr, 0, CEM_nEdgesMax * sizeof(uint32));
				uint32 u32ControlArrayIDX = 0;
				uint32 u32PhaseSkip = 1;
				Bool boAutoCorrelate = TRUE;
				
				if (TRUE == boAutoCorrelationPolarityCheck)
				{
					u32PhaseSkip = 2;
					
					if (enTriState == IOAPI_enHigh)
					{
						if (TRUE != CEM_boCrankEdgeFirstRising)
						{
							boAutoCorrelate = FALSE;
						}
					}
					else
					{
						if (FALSE != CEM_boCrankEdgeFirstRising)
						{
							boAutoCorrelate = FALSE;
						}
					}
				}
				
				if (TRUE == boAutoCorrelate)
				{
					for (uint32 u32AutocorrPhase = 0; u32AutocorrPhase < CEM_xEdgesCount; u32AutocorrPhase += u32PhaseSkip)
					{
						uint32 u32InputArrayIDX = CEM_nEdgesMax - CEM_xEdgesCount;
						
						for (uint32 u32ShiftCount = 0; u32ShiftCount < CEM_xEdgesCount; u32ShiftCount++)
						{
							u32Temp = (au32InputArray[u32InputArrayIDX] * (uint32)CEM_au16AllEdge[u32ControlArrayIDX]);
							u32Temp /= 0x100u;
							au32AutoCorr[u32AutocorrPhase] += u32Temp;
							u32ControlArrayIDX = (u32ControlArrayIDX + 1) % CEM_xEdgesCount;
							u32InputArrayIDX++;
						}
						u32ControlArrayIDX++;
					}
					
					/* Find the autocorrelation peak */
					uint32 u32AutoPeak = 0;
					
					for (uint32 u32ArrayIDX = 0; u32ArrayIDX < CEM_xEdgesCount; u32ArrayIDX++)
					{
						if (u32AutoPeak < au32AutoCorr[u32ArrayIDX])
						{
							u32AutoPeak = au32AutoCorr[u32ArrayIDX];
						}
					}
					
					/* Set the auto-peaks indices array */
					for (uint8 u8ArrayIDX = 0; u8ArrayIDX < CEM_xEdgesCount; u8ArrayIDX++)
					{
						if (u32AutoPeak == au32AutoCorr[u8ArrayIDX])
						{
							if (128u == CEM_au8AutocorrPeak[0])
							{
								CEM_au8AutocorrPeak[0] = u8ArrayIDX;
							}
							else if (128u == CEM_au8AutocorrPeak[1])
							{
								CEM_au8AutocorrPeak[1] = u8ArrayIDX;
							}
							else if (128u == CEM_au8AutocorrPeak[2])
							{
								CEM_au8AutocorrPeak[2] = u8ArrayIDX;
							}
							else if (128u == CEM_au8AutocorrPeak[3])
							{
								CEM_au8AutocorrPeak[3] = u8ArrayIDX;
							}
						}
					}
					
					u32PhaseMin = 0;
					u32PhaseMax = (CEM_xEdgesCount / CEM_u8PhaseRepeats) - 1;
					
					/* Buffer the confirmed array if is not initialized */
					for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
					{
						if (128u > CEM_au8AutocorrPeak[u8ArrayIDX])
						{
							if (127 < CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX])
							{
								if ((u32PhaseMin <= CEM_au8AutocorrPeak[u8ArrayIDX]) &&
								(u32PhaseMax >= CEM_au8AutocorrPeak[u8ArrayIDX]))
								{
									CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] = CEM_au8AutocorrPeak[u8ArrayIDX];
									
									if (0xff == CEM_u32CrankEdgeCounter)
									{
										CEM_u32CrankEdgeCounter = CEM_au8AutocorrPeak[u8ArrayIDX];
									}
								}
							}
						}
						
						u32PhaseMin += (CEM_xEdgesCount / CEM_u8PhaseRepeats);
						u32PhaseMax += (CEM_xEdgesCount / CEM_u8PhaseRepeats);
						u32PhaseMin = u32PhaseMin % CEM_xEdgesCount;
						u32PhaseMax = u32PhaseMax % CEM_xEdgesCount;
					}
					
					/* Clear the confirmed flags */
					for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
					{
						aboConfirmedFlags[u8ArrayIDX] = FALSE;
					}
					
					/* Check if the confirmed array is still confirmed */
					for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
					{
						for (uint8 u8ConfirmedArrayIDX = 0; u8ConfirmedArrayIDX < 4; u8ConfirmedArrayIDX++)
						{
							if (CEM_au8ConfirmedAutocorrPeak[u8ConfirmedArrayIDX] == CEM_au8AutocorrPeak[u8ArrayIDX])
							{
								aboConfirmedFlags[u8ConfirmedArrayIDX] = TRUE;
								au8ErrorCount[u8ConfirmedArrayIDX] = 0;
							}
						}
					}
					
					/* Increment error counter of unconfirmed */
					for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
					{
						if (FALSE == aboConfirmedFlags[u8ArrayIDX])
						{
							au8ErrorCount[u8ArrayIDX] = (au8ErrorCount[u8ArrayIDX] + 1) % 0x80;
						}
					}
					
					/* Check for phase lock on any index */
					for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
					{
						if (CEM_u32CrankEdgeCounter == CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX]) {boPhaseLocked = TRUE;}
					}
					
					/* Buffer and edge count if we are phase locked */
					if (FALSE == boPhaseLocked)
					{
						for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
						{
							if (TRUE == aboConfirmedFlags[u8ArrayIDX])
							{
								CEM_u32CrankEdgeCounter = CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX];
							}
						}
					}
					

					CEM_u32GlobalCycleFraction += CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount];
					CEM_u32GlobalCycleFraction &= 0x1ffff;
					
					/* Are we at a calculation edge? */
					if (0 == CEM_u32CrankEdgeCounter)
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}

					if (1 == CEM_u8PhaseRepeats)
					{
						if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[1])
						{
							CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
							TEPM_vSynchroniseEventProgramKernelQueues();
						}
						if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[2])
						{
							CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
							TEPM_vSynchroniseEventProgramKernelQueues();
						}
						if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[3])
						{
							CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
							TEPM_vSynchroniseEventProgramKernelQueues();
						}
					}

				}
					break;				
			}
			case CEM_enTypeSuzukiM15A:
			{
				CEM_u32GlobalCycleFraction += CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount];

				if (au32InputArray[CEM_nEdgesMax - 1] > (au32InputArray[CEM_nEdgesMax - 2] * 2))
				{
					/* Crank sensor gap */
					if (((CEM_u32CrankEdgeCounter != 21) && (CEM_u32CrankEdgeCounter != 7)) &&
					(100 < CEM_u32EdgeCount))
					{
						CEM_vPhaseError(CEM_u32CrankEdgeCounter);
					}

					CEM_u32CrankEdgeCounter++;

					u8SameGapCount = 0;

					CEM_u32CrankEdgeCounter = 30 <= CEM_u32CrankEdgeCounter ? 1 : CEM_u32CrankEdgeCounter;
				}
				else if (au32InputArray[CEM_nEdgesMax - 1] < (au32InputArray[CEM_nEdgesMax - 2] / 2))
				/* Crank sensor end gap */
				{
					if (((CEM_u32CrankEdgeCounter != 22) && (CEM_u32CrankEdgeCounter != 9)) &&
					(100 < CEM_u32EdgeCount))
					{
						CEM_vPhaseError(CEM_u32CrankEdgeCounter);
					}

					if (0 < u8SameGapCount)
					{
						CEM_u32CrankEdgeCounter = 10;
					}
					else
					{
						CEM_u32CrankEdgeCounter = 23;
						CEM_u32GlobalCycleFraction = CEM_au16AllEdge[0] + CEM_au16AllEdge[1];
					}

					u8SameGapCount = 0;
				}
				else
				{
					/* Crank sensor same gap */
					u8SameGapCount++;
					CEM_u32CrankEdgeCounter++;
				}


				if (31 == CEM_u32CrankEdgeCounter)
				{
					CEM_u32GlobalCycleFraction &= 0x10000;
					CEM_vSequenceReset(enEHIOResource, tEventTime, 0, boLatePhase);
					TEPM_vSynchroniseEventProgramKernelQueues();
					CEM_u32CrankEdgeCounter = 1;
				}

				if (1 == CEM_u8PhaseRepeats)
				{
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x4000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x8000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0xc000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}
				else if (2 == CEM_u8PhaseRepeats)
				{
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}

				break;
			}
			case CEM_enToyota2NFE:
			{
				CEM_u32GlobalCycleFraction += CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount];

				if (au32InputArray[CEM_nEdgesMax - 1] > au32InputArray[CEM_nEdgesMax - 2] * 2)
				{
					if ((CEM_u32CrankEdgeCounter != CEM_u8SimpleMissingSync) &&
					(100 < CEM_u32EdgeCount))
					{
						CEM_vPhaseError(CEM_u32CrankEdgeCounter);
					}

					CEM_u32CrankEdgeCounter = CEM_u8SimpleMissingSync;
					CEM_u32ToothEdgeCounter = (CEM_u8SimpleMissingSync + CEM_u8MissingToothCountMax) %
					CEM_xAllEdgesCount;
					
					if (1 == CEM_u8PhaseRepeats)
					{
						CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
					}
					else
					{
						if ((CEM_u32CycleToothEdgeCounter != CEM_u32ToothEdgeCounter) &&
						(CEM_u32CycleToothEdgeCounter != (CEM_u32ToothEdgeCounter + CEM_xAllEdgesCount)))
						{
							CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
						}
					}
				}
				else
				{
					CEM_u32SmallestToothTime = au32InputArray[CEM_nEdgesMax - 1];
				}
				
				
				// Is tooth prior to gap/gaps
				if (CEM_u8SimpleMissingSync ==
				((CEM_u32CrankEdgeCounter + 1) % (CEM_xEdgesCount + CEM_u8MissingToothCountMax)))
				{
					TEPM_vSetNextMissingToothInterrupt(enEHIOResource, CEM_u32SmallestToothTime, CEM_u8MissingToothCountMax);
				}

				if (10 == CEM_u32CrankEdgeCounter)
				{
					#ifdef DEBUG_SYNC
					tempcount = (tempcount + 1) % 16;
					temp1[tempcount] = 	(CEM_tLastVVTHighTime << 16) + CEM_tLastVVTLowTime;
					temp2[tempcount] = 	CEM_u32GlobalCycleFraction;
					#endif

					if (CEM_tLastVVTHighTime < CEM_tLastVVTLowTime)
					{
						if (0x10000 <= CEM_u32GlobalCycleFraction)
						{
							CEM_vPhaseError(CEM_nEarlySyncError);
						}
						
						boLatePhase = false;
					}
					else
					{
						if (0x10000 > CEM_u32GlobalCycleFraction)
						{
							CEM_vPhaseError(CEM_nLateSyncError);
						}
						
						boLatePhase = true;
					}
				}

				if (0 == CEM_u32CrankEdgeCounter)
				{
					CEM_vSequenceReset(enEHIOResource, tEventTime, 0, boLatePhase);
					TEPM_vSynchroniseEventProgramKernelQueues();
				}

				if (1 == CEM_u8PhaseRepeats)
				{
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x4000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x8000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0xc000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}
				else if (2 == CEM_u8PhaseRepeats)
				{
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}

				CEM_u32CrankEdgeCounter = (CEM_u32CrankEdgeCounter + 1) % CEM_xEdgesCount;
				CEM_vRunToothSchedulingOnTooth();
				
				break;
			}
			case CEM_enOneGroupMissing:
			{
				CEM_u32GlobalCycleFraction += CEM_au16AllEdge[(CEM_u32CrankEdgeCounter - 1) % CEM_xEdgesCount];

				u32Temp = CEM_u32GetMissingThreshold(au32InputArray[CEM_nEdgesMax - 2]);

				if (au32InputArray[CEM_nEdgesMax - 1] > u32Temp)
				{
					if ((CEM_u32CrankEdgeCounter != CEM_u8SimpleMissingSync) &&
					(100 < CEM_u32EdgeCount))
					{
						CEM_vPhaseError(CEM_u32CrankEdgeCounter);
					}
		
		
					CEM_u32CrankEdgeCounter = CEM_u8SimpleMissingSync;
					CEM_u32ToothEdgeCounter = (CEM_u8SimpleMissingSync + CEM_u8MissingToothCountMax) %
					CEM_xAllEdgesCount;
		
					if (1 == CEM_u8PhaseRepeats)
					{
						CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
					}
					else
					{
						if ((CEM_u32CycleToothEdgeCounter != CEM_u32ToothEdgeCounter) &&
						(CEM_u32CycleToothEdgeCounter != (CEM_u32ToothEdgeCounter + CEM_xAllEdgesCount)))
						{
							CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
						}
					}
				}
				else
				{
					CEM_u32SmallestToothTime = au32InputArray[CEM_nEdgesMax - 1];
				}
	
				// Is tooth prior to gap/gaps
				if (CEM_u8SimpleMissingSync ==
				((CEM_u32CrankEdgeCounter + 1) % (CEM_xEdgesCount + CEM_u8MissingToothCountMax)))
				{
					TEPM_vSetNextMissingToothInterrupt(enEHIOResource, CEM_u32SmallestToothTime, CEM_u8MissingToothCountMax);
				}

				if (CEM_u32CrankEdgeCounter == stSimpleCamSync.u32CamSyncSampleToothCount)
				{
					boLatePhase = CEM_boGetLatePhaseSimpleCamSync();
				}

				if (0 == CEM_u32CrankEdgeCounter)
				{
					CEM_vSequenceReset(enEHIOResource, tEventTime, 0, boLatePhase);
					TEPM_vSynchroniseEventProgramKernelQueues();
				}
	
				if (1 == CEM_u8PhaseRepeats)
				{
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x4000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0x8000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, 0xc000, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}
				else if (2 == CEM_u8PhaseRepeats)
				{
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[1])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[2])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
					if ((0xffff & CEM_u32GlobalCycleFraction) == CEM_au16SyncPoints[3])
					{
						CEM_vSequenceReset(enEHIOResource, tEventTime, CEM_u32GlobalCycleFraction, boLatePhase);
						TEPM_vSynchroniseEventProgramKernelQueues();
					}
				}

				CEM_u32CrankEdgeCounter = (CEM_u32CrankEdgeCounter + 1) % CEM_xEdgesCount;
	
				if (1 == CEM_u8PhaseRepeats)
				{
					TEPM_vRunEventToothProgramKernelQueues(FALSE, CEM_u32CycleToothEdgeCounter, CEM_u32SmallestToothTime);
					CEM_u32ToothEdgeCounter = (CEM_u32ToothEdgeCounter + 1) % CEM_xAllEdgesCount;
					CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
				}
				else
				{
					TEPM_vRunEventToothProgramKernelQueues(FALSE, CEM_u32CycleToothEdgeCounter, CEM_u32SmallestToothTime);
					CEM_u32CycleToothEdgeCounter = (CEM_u32CycleToothEdgeCounter + 1) % (2 * CEM_xAllEdgesCount);
					CEM_u32ToothEdgeCounter = (CEM_u32ToothEdgeCounter + 1) % CEM_xAllEdgesCount;
				}
				break;
			}
			
			default:
			{
				break;
			}
		}
		
		if ((1u != ((CEM_u32CrankEdgeCounter - u8PhasePrev) % CEM_xEdgesCount)) && (TRUE == boPhaseLocked))
		{
			CEM_vPhaseError(0);
		}
	}
}

static uint32 CEM_u32GetMissingThreshold(uint32 u32GapPrev)
{
	uint32 u32MissingThreshold;

	if (1 == CEM_u8MissingToothCountMax)
	{
		u32MissingThreshold = (3 * u32GapPrev) / 2;
	}
	else if (2 == CEM_u8MissingToothCountMax)
	{
		u32MissingThreshold = 2 * u32GapPrev;
	}

	return u32MissingThreshold;
}

static Bool CEM_boGetLatePhaseSimpleCamSync(void)
{
	Bool boLatePhase;
	IOAPI_tenTriState enTriState;

	enTriState = IO_enGetDIOResourceState(stSimpleCamSync.enEHIOResource);

	if (true == stSimpleCamSync.boCamSyncHighLate)
	{
		boLatePhase = (IOAPI_enHigh == enTriState);
	}
	else
	{
		boLatePhase = (IOAPI_enLow == enTriState);
	}
	
	return boLatePhase;
}

static void CEM_vPhaseError(uint32 code_in)
{
	volatile static int code;
	volatile static int early_phase_err;
	volatile static int late_phase_err;
	code = code_in; 
	
	if (0 <= code_in)
	{
		CEM_u32CrankErrorCounts++;		
	}
	else if (CEM_nLateSyncError == code_in)
	{
		late_phase_err++;
	}
	else
	{
		early_phase_err++;
	}
	
	UNUSED(code);
}


static TEPMAPI_ttEventTime CEM_tCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction, Bool boGlobalTimeEnable)
{
	TEPMAPI_ttEventTime tTemp;
	TEPMAPI_ttEventTime tGlobalTimeNew = 0;
	uint32 u32Temp;
	
#ifdef DEBUG_GLOB_TIME
	volatile static uint32 test1[16];
	volatile static uint32 test2[16];
	volatile static uint32 testcount;
	
	testcount = (testcount + 1) % 16;
#endif
	
    tTemp = tEventTime - CEM_tEventTimeLast;
	
#ifdef DEBUG_GLOB_TIME
	test1[testcount] = tTemp;
	test2[testcount] = u16LastGapFraction;
#endif		

	if (0xffff >= tTemp)
	{
		tGlobalTimeNew = (0x10000u * tTemp) / (uint32)u16LastGapFraction;
	}
	else if (0xfffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 4;
		tGlobalTimeNew = (0x1000u * tTemp) / (uint32)u16LastGapFraction;
	}
	else if (0xffffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 8;
		tGlobalTimeNew = (0x100u * tTemp) / (uint32)u16LastGapFraction;
	}	
	else if (0xfffffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 12;
		tGlobalTimeNew = (0x10u * tTemp) / (uint32)u16LastGapFraction;
	}	
	else
	{
	    CEM_u32GlobalCycleTime = ~0u;
	}

	if (TRUE == boGlobalTimeEnable)
	{
		if (TRUE == TPS_boThrottleClosed)
		{
			u32Temp = USERCAL_stRAMCAL.u16ESTFilterClosed * CEM_u32GlobalCycleTime;
			u32Temp += ((0x100 - USERCAL_stRAMCAL.u16ESTFilterClosed) * tGlobalTimeNew);
			u32Temp /= 0x100;
		}
		else
		{
			u32Temp = USERCAL_stRAMCAL.u16ESTFilterOpen * CEM_u32GlobalCycleTime;
			u32Temp += ((0x100 - USERCAL_stRAMCAL.u16ESTFilterOpen) * tGlobalTimeNew);
			u32Temp /= 0x100;
		}

		CEM_u32GlobalCycleTime = u32Temp;

		if (tGlobalTimeNew < CEM_u32GlobalCycleTime)
		{
			CEM_u32GlobalCycleTime--;
		}
		else if (tGlobalTimeNew > CEM_u32GlobalCycleTime)
		{
			CEM_u32GlobalCycleTime++;
		}
	}

	return tTemp;
}


static void CEM_vSequenceReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint32 u32OriginGlobalCycleFraction, Bool boLatePhase)
{
	uint32 u32Temp;
	static uint32 u32SequenceIDX;
	static IOAPI_tenTriState enTriState;
	static uint16 u16LastResetFraction;

	CEM_tSyncTimeLast = tEventTime;

	u32SequenceIDX++;	
	u32Temp = CEM_u32GlobalCycleTime / (4 * CEM_u8PhaseRepeats);
	u32Temp = 0x10000 > u32Temp ? u32Temp : 0xffff;

	if (1 == CEM_u8PhaseRepeats)
	{
		if (0 == u32OriginGlobalCycleFraction)
		{
			u32SequenceIDX = 0;

			CEM_u32GlobalCycleFraction = u32OriginGlobalCycleFraction;
			CEM_u32GlobalCycleOriginCount++;
		}
	}
	else
	{
		if (0 == u32OriginGlobalCycleFraction)
		{
			u16LastResetFraction = CEM_u32GlobalCycleFraction;
			 
			if ((0x10001 < CEM_u32GlobalCycleFraction) && (false == boLatePhase)) 
			{
				CEM_u32CamErrorCounts++;
				CEM_u32CamRunningErrorCounts = 5 > CEM_u32CamRunningErrorCounts ? CEM_u32CamRunningErrorCounts + 1 : 5;
			}
			
			
			if (5 == CEM_u32CamRunningErrorCounts)	
			/* Many cam errors */
			{
				if (0 == (CEM_u32GlobalCycleOriginCount % 8))
				/* Try swap phase */
				{
					if (0x10000 <= CEM_u32GlobalCycleFraction)
					{
						u32SequenceIDX = 4;
						CEM_u32GlobalCycleFraction = 0x10000 + u32OriginGlobalCycleFraction;
					}
					else
					{
						u32SequenceIDX = 0;
						CEM_u32GlobalCycleFraction = u32OriginGlobalCycleFraction;
					}

					CEM_u32CamRunningErrorCounts = 0;
				}
				else
				/* Keep phase and reset */
				{

					if (0x10000 <= CEM_u32GlobalCycleFraction)
					{
						u32SequenceIDX = 0;
						CEM_u32GlobalCycleFraction = u32OriginGlobalCycleFraction;
					}
					else
					{
						u32SequenceIDX = 4;
						CEM_u32GlobalCycleFraction = 0x10000 + u32OriginGlobalCycleFraction;
					}
				}
			}
			else if ((0x10000 <= CEM_u32GlobalCycleFraction) && (true == boLatePhase))
			{
				u32SequenceIDX = 0;
				CEM_u32GlobalCycleFraction = u32OriginGlobalCycleFraction;
			}
			else
			{
				CEM_u32GlobalCycleFraction = 0x10000 + u32OriginGlobalCycleFraction;
			}

			CEM_u32GlobalCycleOriginCount++;

			if (0 == (CEM_u32GlobalCycleOriginCount % 8))
			{
				CEM_u32CamRunningErrorCounts = 0 < CEM_u32CamRunningErrorCounts ? CEM_u32CamRunningErrorCounts - 1 : 0;
			}

			PIM_vAssertPortBit(PIMAPI_enPHYS_PORT_A, 1 << 20, enTriState);	
			enTriState = IOAPI_enLow == enTriState ? IOAPI_enHigh : IOAPI_enLow;
		}
	}

	
	if (1 < CEM_u32GlobalCycleOriginCount)//matthew
	{
		TEPM_vStartEventProgramKernelQueues(FALSE, u32SequenceIDX);	
	}

	//if (0 == (0xffff & CEM_u32GlobalCycleFraction))
	if ((0 == (0xffff & CEM_u32GlobalCycleFraction)) || (0x8000 == (0xffff & CEM_u32GlobalCycleFraction)))
	{
		TEPM_vInitiateUserCallBack(enEHIOResource, (uint16)u32Temp);
	}
	
	UNUSED(u16LastResetFraction);
}

void CEM_vPhaseEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	IOAPI_tenTriState enTriState;
	volatile static samples[16];
	volatile static samplecounter;
	
	samplecounter = (samplecounter + 1) % 16;

	if (enEHIOResource == TEPM_stTEPMVVTInput)
	{
		enTriState = IO_enGetDIOResourceState(enEHIOResource);

		/* Note logic is inverted */
		if (IOAPI_enLow == enTriState)
		{
			CEM_tLastVVTLowTime = tEventTime - CEM_tLastVVTTimer;
		}
		else
		{
			CEM_tLastVVTHighTime = tEventTime - CEM_tLastVVTTimer;
		}

		samples[samplecounter] = tEventTime - CEM_tLastVVTTimer;
		CEM_tLastVVTTimer = tEventTime;
	}
}


void CEM_vSetupSimpleCamSync(IOAPI_tenEHIOResource enEHIOResource, Bool boCamSyncHighLate, uint32 u32CamSyncSampletoothCount)
{
	stSimpleCamSync.enEHIOResource = enEHIOResource;
	stSimpleCamSync.boCamSyncHighLate = boCamSyncHighLate;
	stSimpleCamSync.u32CamSyncSampleToothCount = u32CamSyncSampletoothCount;
}

void CEM_vFreqEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	static uint32 u32OldOSTick;
	volatile static uint32 samples[100];
	static uint32 counter;
	static uint32 u32TimeOld;
	static uint32 u32Temp;
	static uint32 u32ErrCount;

	samples[counter] = tEventTime - u32TimeOld;

	u32Temp -= 5;
	u32Temp *= 2;

	if ((u32Temp < samples[counter]) &&
		(2 > u32ErrCount))
	{
		samples[counter] /= 2;
		u32ErrCount++;
	}
	else
	{
		u32ErrCount = 0;
	}

	u32Temp = samples[counter];

	if (OS_u32TickCounter > (u32OldOSTick + 1))
	{
		TEPM_vInitiateUserCallBack(enEHIOResource, samples[counter]);
		u32OldOSTick = OS_u32TickCounter;
	}

	counter = (counter + 1) % 100;
	u32TimeOld = tEventTime;
}

uint32 CEM_u32GetAllEdgesCount(void)
{
	return CEM_xAllEdgesCount;
}

static void CEM_vRunToothSchedulingOnTooth(void)
{
	if (1 == CEM_u8PhaseRepeats)
	{
		TEPM_vRunEventToothProgramKernelQueues(FALSE, CEM_u32CycleToothEdgeCounter, CEM_u32SmallestToothTime);
		CEM_u32ToothEdgeCounter = (CEM_u32ToothEdgeCounter + 1) % CEM_xAllEdgesCount;
		CEM_u32CycleToothEdgeCounter = CEM_u32ToothEdgeCounter;
	}
	else
	{
		TEPM_vRunEventToothProgramKernelQueues(FALSE, CEM_u32CycleToothEdgeCounter, CEM_u32SmallestToothTime);
		CEM_u32CycleToothEdgeCounter = (CEM_u32CycleToothEdgeCounter + 1) % (2 * CEM_xAllEdgesCount);
		CEM_u32ToothEdgeCounter = (CEM_u32ToothEdgeCounter + 1) % CEM_xAllEdgesCount;
	}
}