/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      MSG                    																*/
/* DESCRIPTION:        This code provides functions for OS messaging					*/
/*										 																												*/
/* FILE NAME:          MSG.c                                             			*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define __MSG_C

#include "MSG.h"

MSG_tstMBX MSG_astMBX[MSG_nMBXCount];
static sint32 MSG_u32GetFreeMBX(void);

void MSG_vStart(uint32* const pu32Stat)
{
	uint32 u32MBXIDX;
	
	for (u32MBXIDX = 0; u32MBXIDX < MSG_nMBXCount; u32MBXIDX++)
	{
		MSG_astMBX[u32MBXIDX].enMSGType = MSG_enNone;
		MSG_astMBX[u32MBXIDX].pstMSG = NULL;
	}
}

void MSG_vRun(uint32* const pu32Stat)
{

}

void MSG_vTerminate(uint32* const pu32Stat)
{
	
}

MSG_tenMBXErr MSG_enMBXNew(MSG_tstMBX** ppstMBX, MSG_tenMSGType enMSGType)
{
	MSG_tenMBXErr enMBXErr = MSG_enBadMSGType;
	sint32 i32MBXIDX = -1;
	
	if ((MSG_enNone < enMSGType) && (MSG_enMSGTypeCount > enMSGType))
	{
		i32MBXIDX = MSG_u32GetFreeMBX();
		enMBXErr = MSG_enMBXQueueFull;
		
		if (-1 != i32MBXIDX)
		{
			MSG_astMBX[i32MBXIDX].enMSGType = enMSGType;
			*ppstMBX = &MSG_astMBX[i32MBXIDX];
			enMBXErr = MSG_enOK;
		}
	}
	return enMBXErr;
}

void MSG_vMBXFree(MSG_tstMBX* const pstMBX)
{
	pstMBX->enMSGType = MSG_enNone;
	pstMBX->pstMSG = NULL;
}

void MSG_vMBXPost(MSG_tstMBX* const pstMBX, void* const pstMSG)
{
	if ((MSG_enNone != pstMBX->enMSGType) && (NULL == pstMBX->pstMSG))
	{
		pstMBX->pstMSG = pstMSG;
	}		
}

MSG_tenMBXErr MSG_vMBXTryPost(MSG_tstMBX* const pstMBX, void* const pstMSG)
{
	MSG_tenMBXErr enMBXErr = MSG_enBadMBX;	
	//todo wait
	if(MSG_enNone != pstMBX->enMSGType)
	{
		enMBXErr = MSG_enMBXFull;
		
		if (NULL == pstMBX->pstMSG)
		{
			pstMBX->pstMSG = pstMSG;
			enMBXErr = MSG_enOK;
		}
	}	
	
	return enMBXErr;
}

MSG_tenMBXErr MSG_enMBXTryFetchWait(MSG_tstMBX* const pstMBX, void* pstMSG)
{
	MSG_tenMBXErr enMBXErr = MSG_enBadMBX;
	//todo wait
	if(MSG_enNone != pstMBX->enMSGType)
	{
		enMBXErr = MSG_enNoMSG;	
		
		if (NULL != pstMBX->pstMSG)
		{
			pstMSG = pstMBX->pstMSG;
			enMBXErr = MSG_enOK;
		}
	}
	
	/* TODO suppress warning */
	(void)pstMSG;
	
	return enMBXErr;	
}

MSG_tenMBXErr MSG_enMBXTryFetch(MSG_tstMBX* const pstMBX, void* pstMSG)
{
	MSG_tenMBXErr enMBXErr = MSG_enBadMBX;
	
	if(MSG_enNone != pstMBX->enMSGType)
	{
		enMBXErr = MSG_enNoMSG;
		
		if (NULL != pstMBX->pstMSG)
		{
			pstMSG = pstMBX->pstMSG;
			enMBXErr = MSG_enOK;
		}	
	}
	
	/* TODO supppress warning */
	(void)pstMSG;
	
	return enMBXErr;
}

bool MSG_boMBXValid(MSG_tstMBX* const pstMBX)
{
	return (MSG_enNone != pstMBX->enMSGType);
}

void MSG_boMBXInvalidate(MSG_tstMBX* const pstMBX)
{
	pstMBX->enMSGType = MSG_enNone;
}

static sint32 MSG_u32GetFreeMBX(void)
{
	uint32 u32MBXIDX;
	sint32 i32RetIDX;
		
	for (u32MBXIDX = 0; u32MBXIDX < MSG_nMBXCount; u32MBXIDX++)
	{
		if (MSG_enNone == MSG_astMBX[u32MBXIDX].enMSGType)
		{
			i32RetIDX = u32MBXIDX;
			break;
		}
	}
	return i32RetIDX;
}

