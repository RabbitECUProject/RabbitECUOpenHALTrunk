/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX																										*/
/* FILE NAME:          XXX.c                                          				*/
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
bool CEM_boCrankEdgeFirstRising;
uint32 CEM_u32CrankEdgeTimeoutCount;
uint32 CEM_u32EventTimeOld;

static void CEM_vOriginReset(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_ttEventTime tEventTime, uint16 u16OriginGlobalCycleFraction);
static void CEM_vCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction);

void CEM_vStart(uint32* const u32Stat)
{
	CEM_u32GlobalCycleFraction = 0;
	CEM_u32GlobalCycleTime = 0x0000ffff;
	CEM_u32GlobalCycleOriginCount = 0;
	CEM_u32CrankEdgeTimeoutCount = 0;
	CEM_u32EventTimeOld = 0xffffffff;
	
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
		CEM_u32EventTimeOld = 0xffffffff;		
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

bool CEM_boPopulateCrankEdgeArrays(puint16 const pu16EdgeFractionArray, const bool boFirstRising, uint8 u8EdgeCount)
{
	uint32 u32EdgeCount = 0;
	bool boStat = FALSE;
	
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
	
	enTriState = IO_enGetDIOResourceState(enEHIOResource);
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
		}				
	}
	else if (IOAPI_enHigh == enTriState)
	{		
		if (((CEM_u8FallingEdgesCount + CEM_u8RisingEdgesCount) <= CEM_u32CrankEdgeCounter) 
			&& (TRUE == CEM_boCrankEdgeFirstRising))
		{				
			CEM_u32CrankEdgeCounter = 0;		
			u16LastGapFraction = CEM_au16FallingCrankEdge[CEM_u32CrankEdgeCounter / 2] - 
				CEM_au16RisingCrankEdge[CEM_u8RisingEdgesCount - 1];
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
		
	CEM_u32EventTimeOld = 0xffff & tEventTime;
	CEM_u32CrankEdgeCounter++;
}

static void CEM_vCalculateGlobalTime(TEPMAPI_ttEventTime tEventTime, uint16 u16LastGapFraction)
{
	uint16 u16Temp;
	
	if (0xffffffff != CEM_u32EventTimeOld)
	{
		u16Temp = tEventTime - (uint16)CEM_u32EventTimeOld;		
		
		CEM_u32GlobalCycleTime = (0x10000 * (uint32)u16Temp) / (uint32)u16LastGapFraction;
	}
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
