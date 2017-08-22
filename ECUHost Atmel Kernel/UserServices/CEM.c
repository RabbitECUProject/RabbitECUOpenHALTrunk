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
uint16 CEM_au16SyncPoints[36];
uint8 CEM_u8RisingEdgesCount;
uint8 CEM_u8FallingEdgesCount;
uint32 CEM_u8CrankEdgeCounter;
Bool CEM_boCrankEdgeFirstRising;
uint32 CEM_u32CrankEdgeTimeoutCount;
uint8 CEM_au8AutocorrPeak[4];
uint8 CEM_au8ConfirmedAutocorrPeak[4];
IOAPI_tenEdgePolarity CEM_enEdgePolarity;
Bool CEM_boEdgesReady;
uint32 CEM_au32TimerOffsets[3];
CEM_tenTriggerType CEM_enTriggerType;
uint8 CEM_u8SimpleMissingSync;
uint8 CEM_u32SyncPoints;

static void CEM_vOriginReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint16 u16OriginGlobalCycleFraction);
static TEPMAPI_ttEventTime CEM_tCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction);
static void CEM_vProcessAllEdges(void);
static void CEM_vPhaseError(void);

void CEM_vStart(uint32* const u32Stat)
{
	CEM_u32GlobalCycleFraction = 0;
	CEM_u32GlobalCycleTime = 0x0000ffff;
	CEM_u32GlobalCycleOriginCount = 0;
	CEM_u32CrankEdgeTimeoutCount = 0;
	CEM_tEventTimeLast = ~0;
	CEM_u8CrankEdgeCounter = 0xff;
	CEM_boEdgesReady = FALSE;
	CEM_u8SimpleMissingSync = 0;
	
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
}

void CEM_vRun(uint32* const u32Stat)
{
	if (10 < CEM_u32CrankEdgeTimeoutCount)
	{
		CEM_tEventTimeLast = ~0;		
		CEM_u32GlobalCycleOriginCount	= 0;	
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

	return boRetVal;
}


Bool CEM_boPopulateCrankEdgeArrays(puint16 const pu16EdgeFractionArray, const Bool boFirstRising, IOAPI_tenEdgePolarity enEdgePolarity)
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

    for (uint32 u32ArrayIDX = 0; u32ArrayIDX < (CEM_xEdgesCount - 1); u32ArrayIDX++)
	{
        u32OldData = CEM_au16AllEdge[u32ArrayIDX];
	    CEM_au16AllEdge[u32ArrayIDX] = CEM_au16AllEdge[u32ArrayIDX + 1] - u32OldData;
	}

	CEM_au16AllEdge[CEM_xEdgesCount - 1] = ~CEM_au16AllEdge[CEM_xEdgesCount - 1] + 1;

    for (uint32 u32ArrayIDX = 1; u32ArrayIDX < CEM_xEdgesCount; u32ArrayIDX++)
	{
	    s32Temp = CEM_au16AllEdge[u32ArrayIDX] - CEM_au16AllEdge[u32ArrayIDX - 1];

        if (1 < s32Temp)
        {
            u32DeltaCount++;
			CEM_u8SimpleMissingSync = (u32ArrayIDX + 1) % CEM_xEdgesCount;
        }
	}

	CEM_enTriggerType = 1 == u32DeltaCount ? CEM_enOneGroupMissing : CEM_enAutocorrelationMode;

}


TEPMAPI_ttEventTime CEM_ttGetModulePhase(TEPMHA_tenModule enModule)
{
    TEPMAPI_ttEventTime tPhaseOffset = 0;

	if (TEPMHA_enModuleCount > enModule)
	{
	    tPhaseOffset = CEM_au32TimerOffsets[enModule];
	}

    return  tPhaseOffset;
}

void CEM_vPrimaryEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	uint16 u16LastGapFraction;
	TEPMAPI_ttEventTime tLastGapTime;
	IOAPI_tenTriState enTriState;
	static uint32 au32AutoCorr[CEM_nEdgesMax];
	static uint32 au32InputArray[CEM_nEdgesMax];
	static uint8 u8Phase = 0x80;
	static Bool aboConfirmedFlags[4] = {0, 0, 0, 0};
	static uint8 au8ErrorCount[4];
	uint8 u8PhasePrev;
	uint32 u32Temp;
	static uint32 u32EdgeCount = 0;
	Bool boPhaseLocked = FALSE;
	static uint8 u8PhaseRepeats = 1;

	if (FALSE == CEM_boEdgesReady) return;

	/* Sanity check the edges count */
	if (CEM_xEdgesCount > CEM_nEdgesMax) return;

    CEM_au32TimerOffsets[2] = ((tstTimerModule*)TC2)->TC_CHANNEL[0].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[1] = ((tstTimerModule*)TC1)->TC_CHANNEL[0].TC_CV - ((tstTimerModule*)TC0)->TC_CHANNEL[0].TC_CV;
    CEM_au32TimerOffsets[0] = 0;

	u32EdgeCount++;

	/* Sanity check edge polarity */
	if (IOAPI_enEdgeFalling == CEM_enEdgePolarity)
	{
		enTriState = TEPM_enGetTimerDigitalState(enEHIOResource);
	    if (IOAPI_enLow != enTriState) return;
	}
	else if (IOAPI_enEdgeRising == CEM_enEdgePolarity)
	{
		enTriState = TEPM_enGetTimerDigitalState(enEHIOResource);
	    if (IOAPI_enHigh != enTriState) return;
	}

	/* Increment the confirmed channels */
	uint32 u32PhaseMin = 0;
	uint32 u32PhaseMax = (CEM_xEdgesCount / u8PhaseRepeats) - 1;
	
	for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
	{
	    if (TRUE == aboConfirmedFlags[u8ArrayIDX])
	    {
	        CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX]++;
	
	        if (u32PhaseMax < CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX])
	        {
	            CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] -= (CEM_xEdgesCount / u8PhaseRepeats);
	        }
	    }
	    else
	    {
	        CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] = 128;
	    }
	
	    u32PhaseMin += (CEM_xEdgesCount / u8PhaseRepeats);
	    u32PhaseMax += (CEM_xEdgesCount / u8PhaseRepeats);
		u32PhaseMin = u32PhaseMin % CEM_xEdgesCount;
		u32PhaseMax = u32PhaseMax % CEM_xEdgesCount;
	}

    u16LastGapFraction = u8PhaseRepeats * CEM_au16AllEdge[CEM_u8CrankEdgeCounter];
	tLastGapTime = CEM_tCalculateGlobalTime(tEventTime, u16LastGapFraction);	

	/* Increment the edge counter if possible */
	for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
	{
	    if (TRUE == aboConfirmedFlags[u8ArrayIDX])
	    {
	        CEM_u8CrankEdgeCounter = (CEM_u8CrankEdgeCounter + 1) % CEM_xEdgesCount;
	        break;
	    }
	}

	CEM_tEventTimeLast = tEventTime;
	CEM_u32CrankEdgeTimeoutCount = 0;

	/* Shift the input array */
	for (uint32 u32ArrayIDX = (CEM_nEdgesMax - CEM_xEdgesCount); u32ArrayIDX < (CEM_nEdgesMax - 1); u32ArrayIDX++)
	{
	    au32InputArray[u32ArrayIDX] = au32InputArray[u32ArrayIDX + 1];
	}
	
	/* Buffer the new gap time */
	au32InputArray[CEM_nEdgesMax - 1] = tLastGapTime;
	
	/* If enough edges have arrived to proceed */
	if ((CEM_nEdgesMax / u8PhaseRepeats) <= u32EdgeCount)
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
	
				for (uint32 u32AutocorrPhase = 0; u32AutocorrPhase < CEM_xEdgesCount; u32AutocorrPhase++)
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
	
				uint32 u32PhaseMin = 0;
				uint32 u32PhaseMax = (CEM_xEdgesCount / u8PhaseRepeats) - 1;
	
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
	
								if (0xff == CEM_u8CrankEdgeCounter)
								{
									CEM_u8CrankEdgeCounter = CEM_au8AutocorrPeak[u8ArrayIDX];
								}
							}
						}
					}
	
					u32PhaseMin += (CEM_xEdgesCount / u8PhaseRepeats);
					u32PhaseMax += (CEM_xEdgesCount / u8PhaseRepeats);
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
					if (CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX] == CEM_au8AutocorrPeak[u8ArrayIDX])
					{
						aboConfirmedFlags[u8ArrayIDX] = TRUE;
						au8ErrorCount[u8ArrayIDX] = 0;
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
	
				/* Check for phase lock */
				for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
				{
					if (CEM_u8CrankEdgeCounter == CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX]) {boPhaseLocked = TRUE;}
				}
	
				/* Buffer and edge count if we are phase locked */
				if (FALSE == boPhaseLocked)
				{
					for (uint8 u8ArrayIDX = 0; u8ArrayIDX < 4; u8ArrayIDX++)
					{
						if (TRUE == aboConfirmedFlags[u8ArrayIDX])
						{
							CEM_u8CrankEdgeCounter = CEM_au8ConfirmedAutocorrPeak[u8ArrayIDX];
						}
					}
				}

				CEM_u32GlobalCycleFraction += u8PhaseRepeats * CEM_au16AllEdge[(CEM_u8CrankEdgeCounter - 1) % CEM_xEdgesCount];
	
				/* Are we at a calculation edge? */
				//if (30 == CEM_u8CrankEdgeCounter & 0)
				{
					//TEPM_vSynchroniseEventProgramKernelQueues();
				}
				if ((0 == CEM_u8CrankEdgeCounter) || (34 == CEM_u8CrankEdgeCounter))
				{
					CEM_vOriginReset(enEHIOResource, tEventTime, 0);
				}
				break;
			}
			case CEM_enOneGroupMissing:
			{
				CEM_u32GlobalCycleFraction += u8PhaseRepeats * CEM_au16AllEdge[(CEM_u8CrankEdgeCounter - 1) % CEM_xEdgesCount];

		        if (au32InputArray[CEM_nEdgesMax - 1] > au32InputArray[CEM_nEdgesMax - 2] * 2)
				{
			        CEM_u8CrankEdgeCounter = CEM_u8SimpleMissingSync;
				}

				if (0 == CEM_u8CrankEdgeCounter)
				{
				    CEM_vOriginReset(enEHIOResource, tEventTime, 0);
				}

				for (uint32 u32PointIDX = 1; u32PointIDX < CEM_u32SyncPoints; u32PointIDX++)
				{
				    if (CEM_u32GlobalCycleFraction == CEM_au16SyncPoints[u32PointIDX])
					{
					    TEPM_vSynchroniseEventProgramKernelQueues();
						break;
					}
				}

				CEM_u8CrankEdgeCounter = (CEM_u8CrankEdgeCounter + 1) % CEM_xEdgesCount;

				break;
			}
		}
	

		if ((1u != ((CEM_u8CrankEdgeCounter - u8PhasePrev) % CEM_xEdgesCount)) && (TRUE == boPhaseLocked))
		{
			CEM_vPhaseError();
		}
	}
}

static void CEM_vPhaseError(void)
{
    static uint32 u32PhaseErrorCount = 0;

	u32PhaseErrorCount++;
}

static TEPMAPI_ttEventTime CEM_tCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction)
{
	TEPMAPI_ttEventTime tTemp;
	TEPMAPI_ttEventTime tGlobalTimeNew;
	
    tTemp = tEventTime - CEM_tEventTimeLast;		

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

	if (tGlobalTimeNew < CEM_u32GlobalCycleTime)
	{
        CEM_u32GlobalCycleTime = tGlobalTimeNew;
	}
	else
	{
        CEM_u32GlobalCycleTime = (CEM_u32GlobalCycleTime / 2) + (tGlobalTimeNew / 2);
	}

	return tTemp;
}

static void CEM_vOriginReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint16 u16OriginGlobalCycleFraction)
{
	uint16 u16Temp;
	
	CEM_tGlobalCycleOrigin = tEventTime;
	u16Temp = CEM_u32GlobalCycleTime / 8;
	u16Temp = 0x10000 > u16Temp ? u16Temp : 0xffff;
	TEPM_vInitiateUserCallBack(enEHIOResource, u16Temp);
	CEM_u32GlobalCycleFraction = u16OriginGlobalCycleFraction;
	CEM_u32GlobalCycleOriginCount++;
	
	if (1 < CEM_u32GlobalCycleOriginCount++)
	{
		TEPM_vStartEventProgramKernelQueues();	
	}
}

void CEM_vPhaseEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	
	
}
