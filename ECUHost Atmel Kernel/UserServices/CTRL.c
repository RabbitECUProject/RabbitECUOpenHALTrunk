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
#include <string.h>
#include "CPUAbstract.h"
#include "OS.h"
#include "declarations.h"
#include "CTRLAPI.h"
#include "MATH.h"
#include "CTRL.h"

CTRLAPI_tstPIDCB CTRLAPI_astPIDCB[CTRL_nPIDCTRLCount];
CTRLAPI_tstPIDCB* CTRLAPI_apstPIDCB[CTRL_nPIDCTRLCount + CTRL_nPIDUserCount];
uint32 CTRLAPI_au32Counter[CTRL_nPIDCTRLCount + CTRL_nPIDUserCount];


void CTRL_vStart(puint32 const pu32Stat)
{
	uint32 u32IDX;
	
	for (u32IDX = 0; u32IDX < (CTRL_nPIDCTRLCount + CTRL_nPIDUserCount); u32IDX++)
	{
		CTRLAPI_apstPIDCB[u32IDX] = NULL;	
	}
}

void CTRL_vRun(puint32 const pu32Stat)
{
	CTRLAPI_tstPIDCB* pstPIDCB = NULL;
	uint32 u32PIDIDX;
	uint32 u32CountTerminal;
	
	for (u32PIDIDX = 0; u32PIDIDX < (CTRL_nPIDCTRLCount + CTRL_nPIDUserCount); u32PIDIDX++)
	{	
		pstPIDCB = CTRLAPI_apstPIDCB[u32PIDIDX];
		
		if ((NULL != pstPIDCB) && (TRUE == pstPIDCB->boRun))
		{
			u32CountTerminal = MATH_u32IDXToMask(pstPIDCB->enPIDRate);
			
			if (1024u >= u32CountTerminal)
			/* If terminal count is > 1024 then must be a call iterate controller */
			{
				if (u32CountTerminal <= ++CTRLAPI_au32Counter[u32PIDIDX])
				{
					CTRLAPI_au32Counter[u32PIDIDX] = 0;
					CTRL_vIteratePIDCB(pstPIDCB);
				}
			}
		}
	}
}

void CTRL_vTerminate(puint32 const pu32Stat)
{
	
	
}

CTRLAPI_ttPIDIDX CTRL_tRequestKernelPIDController(CTRLAPI_tstPIDCB* pstPIDCBReq)
{
	CTRLAPI_tstPIDCB* pstPIDCB;
	uint32 u32PIDIDX;
	CTRLAPI_ttPIDIDX tPIDIDX = -1;
	

	/* Request a Kernel managed PID controller */
	for (u32PIDIDX = 0; CTRL_nPIDCTRLCount > u32PIDIDX; u32PIDIDX++)
	{
		if (NULL == CTRLAPI_apstPIDCB[u32PIDIDX])
		{
			pstPIDCB = &CTRLAPI_astPIDCB[u32PIDIDX];
			CTRLAPI_apstPIDCB[u32PIDIDX] = pstPIDCB;
			memcpy((void*)pstPIDCB, (void*)pstPIDCBReq, sizeof(CTRLAPI_tstPIDCB));
			pstPIDCB->i32Error = 0;
			pstPIDCB->i32Integral = 0;
			pstPIDCB->i32IntegralInput = 0;			
			tPIDIDX = u32PIDIDX;
			break;
		}			
	}
	
	return tPIDIDX;	
}



void CTRL_vIteratePIDCB(CTRLAPI_tstPIDCB* pstPIDCB)
{
	sint32 i32ErrorDerivative;
	sint32 i32Result = 0;
	sint32 i32ResultTerm;
	sint32 i32Temp;
	
	pstPIDCB->i32Error = *(pstPIDCB->pi32Target) - *(pstPIDCB->pi32Feedback);	
	
	/* Calculate the integral */
	if (FALSE == pstPIDCB->boResetIntegrator)
	{
		CTRL_xAddSigned32(pstPIDCB->i32Integral, pstPIDCB->i32Error);
	}
	else
	{
		pstPIDCB->i32Integral = 0;
		pstPIDCB->boResetIntegrator = FALSE;
	}
		
	/* Calculate the error derivative */
	i32ErrorDerivative = pstPIDCB->i32Error - pstPIDCB->i32OldError;
	
	/* Calculate the proportional output */
	CTRL_xCalcTermResult(pstPIDCB->i32Error, pstPIDCB->u8PTerm);
		
	/* Accumulate the output */
	CTRL_xAddSigned32(i32Result, i32ResultTerm);
	
	/* Calculate the derivative output */
	CTRL_xCalcTermResult(i32ErrorDerivative, pstPIDCB->u8DTerm);																		
		
	/* Accumulate the output */
	CTRL_xAddSigned32(i32Result, i32ResultTerm);		
	
	/* Calculate the integral output */
	CTRL_xCalcTermResult(pstPIDCB->i32Integral, pstPIDCB->u8ITerm);	
	
	/* Accumulate the output */
	CTRL_xAddSigned32(i32Result, i32ResultTerm);	
	
	
	/* Check for out of range and wind-up */
	if (pstPIDCB->i32OutputMax < i32Result)
	{
		i32Temp = i32Result - pstPIDCB->i32OutputMax;
		i32Result = pstPIDCB->i32OutputMax;
		if (pstPIDCB->boNoWindUp)
		{
			/* Remove the P component */
			i32Temp -= (pstPIDCB->i32Error * pstPIDCB->u8PTerm);
			i32Temp = (0 < i32Temp) ? i32Temp : 0;
			i32Temp = (0 < pstPIDCB->i32Integral) ? (i32Temp / pstPIDCB->u8ITerm) : 0;
			pstPIDCB->i32Integral -= i32Temp;
		}
	}
	else if (pstPIDCB->i32OutputMin > i32Result)
	{
		i32Temp = pstPIDCB->i32OutputMin - i32Result;
		i32Result = pstPIDCB->i32OutputMin;
		if (pstPIDCB->boNoWindUp)
		{
			/* Remove the P component */
			i32Temp += (pstPIDCB->i32Error * pstPIDCB->u8PTerm);
			i32Temp = (0 < i32Temp) ? i32Temp : 0;			
			i32Temp = (0 > pstPIDCB->i32Integral) ? (i32Temp / pstPIDCB->u8ITerm) : 0;
			pstPIDCB->i32Integral += i32Temp;		
		}
	}
	
	
	/* Buffer the output result */
	*(pstPIDCB->pi32Output) = i32Result;
	
	/* Copy old variables */
	pstPIDCB->i32OldError = pstPIDCB->i32Error;
}


void CTRL_vIteratePID(CTRLAPI_ttPIDIDX tPIDIDX)
{
		CTRLAPI_tstPIDCB* pstPIDCB;
	
		pstPIDCB = CTRLAPI_apstPIDCB[tPIDIDX];
	
		CTRL_vIteratePIDCB(pstPIDCB);
}

