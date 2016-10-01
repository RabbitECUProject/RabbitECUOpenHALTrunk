/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      XXX Header File                    										*/
/* DESCRIPTION:        XXX   																									*/
/* FILE NAME:          XXX.h                                         					*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CEM_H
#define CEM_H

#include "types.h"
#include "IO.h"
#include "IOAPI.h"
#include "TEPM.h"
#include "TEPMAPI.h"
#include "declarations.h"

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _CEM_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN uint32 CEM_u32GlobalCycleFraction;
EXTERN uint32 CEM_u32GlobalCycleTime;
EXTERN TEPMAPI_ttEventTime CEM_tGlobalCycleOrigin;
EXTERN uint32 CEM_u32GlobalCycleOriginCount;

void CEM_vStart(uint32* const);
void CEM_vRun(uint32* const);
void CEM_vTerminate(uint32* const);
bool CEM_boPopulateCrankEdgeArrays(puint16 const pu16EdgeFractionArray , const bool boFirstRising, uint8 u8EdgeCount);
void CEM_vPrimaryEventCB(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
void CEM_vPhaseEventCB(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);

#endif //CEM_H

