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

uint16 CEM_au16RisingCrankEdge[5];
uint16 CEM_au16FallingCrankEdge[5];
uint8 CEM_u8RisingEdgesCount;
uint8 CEM_u8FallingEdgesCount;
uint32 CEM_u32CrankEdgeCounter;
Bool CEM_boCrankEdgeFirstRising;
uint32 CEM_u32CrankEdgeTimeoutCount;
TEPMAPI_ttEventTime CEM_tEventTimeOld;

static void CEM_vOriginReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint16 u16OriginGlobalCycleFraction);
static void CEM_vCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction);

void CEM_vStart(uint32* const u32Stat)
{
	CEM_u32GlobalCycleFraction = 0;
	CEM_u32GlobalCycleTime = 0x0000ffff;
	CEM_u32GlobalCycleOriginCount = 0;
	CEM_u32CrankEdgeTimeoutCount = 0;
	CEM_tEventTimeOld = ~0;
	
	memset((void*)&CEM_au16RisingCrankEdge, 0, sizeof(CEM_au16RisingCrankEdge)); 
	memset((void*)&CEM_au16FallingCrankEdge, 0, sizeof(CEM_au16FallingCrankEdge));	
	CEM_u8RisingEdgesCount = 0;
	CEM_u8FallingEdgesCount	= 0;
	CEM_boCrankEdgeFirstRising = FALSE;
	CEM_u32CrankEdgeCounter = 0;
}

void CEM_vRun(uint32* const u32Stat)
{
	if (10 < CEM_u32CrankEdgeTimeoutCount)
	{
		CEM_tEventTimeOld = ~0;		
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

Bool CEM_boPopulateCrankEdgeArrays(puint16 const pu16EdgeFractionArray, const Bool boFirstRising, uint8 u8EdgeCount)
{
	uint32 u32EdgeCount = 0;
	Bool boStat = FALSE;
	
	if ((0 == (u8EdgeCount % 2)) && (1 < u8EdgeCount))
	{	
		CEM_boCrankEdgeFirstRising = boFirstRising;
		
		while (0 < u8EdgeCount)
		{	
			if (TRUE == boFirstRising)
			{
				CEM_au16RisingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
				u32EdgeCount++;
				CEM_au16FallingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);
				u32EdgeCount++;				
			}
			else
			{
				CEM_au16FallingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);		
				u32EdgeCount++;
				CEM_au16RisingCrankEdge[u32EdgeCount / 2] = *(pu16EdgeFractionArray + u32EdgeCount);	
				u32EdgeCount++;
			}
			
			CEM_u8RisingEdgesCount++;
			CEM_u8FallingEdgesCount++;
			u8EdgeCount -= 2;
		}
		
		boStat = TRUE;
	}
	
	return boStat;
}

void CEM_vPrimaryEventCB(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime)
{
	IOAPI_tenTriState enTriState;
	uint16 u16LastGapFraction;
	static volatile uint32 u32DebugArrayHigh[10];
	static volatile uint32 u32DebugArrayLow[10];
	static volatile uint32 u32DebugIDXHigh;
	static volatile uint32 u32DebugIDXLow;

    enTriState = TEPM_enGetTimerDigitalState(enEHIOResource);

	if (IOAPI_enHigh == enTriState)
	{
	    u32DebugIDXHigh = 9 > u32DebugIDXHigh ? u32DebugIDXHigh + 1 : 0;
		u32DebugArrayHigh[u32DebugIDXHigh] = tEventTime;
	}
	else
	{
		u32DebugIDXLow = 9 > u32DebugIDXLow ? u32DebugIDXLow + 1 : 0;
		u32DebugArrayLow[u32DebugIDXLow] = tEventTime;
	}

	CEM_u32CrankEdgeTimeoutCount = 0;
	
	if (IOAPI_enLow == enTriState)
	{		
		if (((CEM_u8FallingEdgesCount + CEM_u8RisingEdgesCount) <= CEM_u32CrankEdgeCounter) 
			&& (FALSE == CEM_boCrankEdgeFirstRising))
		{				
			CEM_u32CrankEdgeCounter = 0;		
			u16LastGapFraction = CEM_au16FallingCrankEdge[CEM_u32CrankEdgeCounter / 2] - 
				CEM_au16RisingCrankEdge[CEM_u8RisingEdgesCount - 1];
			CEM_vCalculateGlobalTime(tEventTime, u16LastGapFraction);
			CEM_vOriginReset(enEHIOResource, tEventTime, CEM_au16FallingCrankEdge[CEM_u32CrankEdgeCounter / 2]);

		}	
		else
		{
			CEM_u32GlobalCycleFraction = CEM_au16FallingCrankEdge[CEM_u32CrankEdgeCounter / 2];
			u16LastGapFraction = CEM_au16FallingCrankEdge[CEM_u32CrankEdgeCounter / 2] - 
				CEM_au16RisingCrankEdge[CEM_u32CrankEdgeCounter / 2];
			CEM_vCalculateGlobalTime(tEventTime, u16LastGapFraction);		
			TEPM_vSynchroniseEventProgramKernelQueues();
		}				
	}
	else if (IOAPI_enHigh == enTriState)
	{		
		if (((CEM_u8FallingEdgesCount + CEM_u8RisingEdgesCount) <= CEM_u32CrankEdgeCounter) 
			&& (TRUE == CEM_boCrankEdgeFirstRising))
		{				
			CEM_u32CrankEdgeCounter = 0;		
			u16LastGapFraction = CEM_au16RisingCrankEdge[CEM_u32CrankEdgeCounter / 2] - 
				CEM_au16FallingCrankEdge[CEM_u8FallingEdgesCount - 1];
			CEM_vCalculateGlobalTime(tEventTime, u16LastGapFraction);			
			CEM_vOriginReset(enEHIOResource, tEventTime, CEM_au16RisingCrankEdge[CEM_u32CrankEdgeCounter / 2]);		
		}	
		else
		{
			CEM_u32GlobalCycleFraction = CEM_au16RisingCrankEdge[CEM_u32CrankEdgeCounter / 2];
			u16LastGapFraction = CEM_au16RisingCrankEdge[CEM_u32CrankEdgeCounter / 2] - 
				CEM_au16FallingCrankEdge[CEM_u32CrankEdgeCounter / 2];
			CEM_vCalculateGlobalTime(tEventTime, u16LastGapFraction);		
			TEPM_vSynchroniseEventProgramKernelQueues();
		}				
	}
		
	CEM_tEventTimeOld = tEventTime;
	CEM_u32CrankEdgeCounter++;
}

static void CEM_vCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction)
{
	TEPMAPI_ttEventTime tTemp;
	
    tTemp = tEventTime - CEM_tEventTimeOld;		

	if (0xffff >= tTemp)
	{
		CEM_u32GlobalCycleTime = (0x10000u * tTemp) / (uint32)u16LastGapFraction;
	}
	else if (0xfffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 4;
		CEM_u32GlobalCycleTime = (0x1000u * tTemp) / (uint32)u16LastGapFraction;
	}
	else if (0xffffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 8;
		CEM_u32GlobalCycleTime = (0x100u * tTemp) / (uint32)u16LastGapFraction;
	}	
	else if (0xfffffff >= tTemp)
	{
		u16LastGapFraction = u16LastGapFraction >> 12;
		CEM_u32GlobalCycleTime = (0x10u * tTemp) / (uint32)u16LastGapFraction;
	}	
	else
	{
	    CEM_u32GlobalCycleTime = ~0u;
	}

	return tTemp;
}

static void CEM_vOriginReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint16 u16OriginGlobalCycleFraction)
{
	uint16 u16Temp;
	
	CEM_tGlobalCycleOrigin = tEventTime;
	u16Temp = CEM_u32GlobalCycleTime / 2;
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
