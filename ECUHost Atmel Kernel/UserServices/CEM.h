/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CEM_H
#define CEM_H

#include "types.h"
#include "kernelio.h"
#include "IOAPI.h"
#include "TEPM.h"
#include "TEPMAPI.h"
#include "declarations.h"

#define CEM_xEdgesCount (CEM_u8FallingEdgesCount + CEM_u8RisingEdgesCount)
#define CEM_nEdgesMax   80u

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _CEM_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

typedef enum
{
   CEM_enAutocorrelationMode,
   CEM_enOneGroupMissing,
   CEM_enModeCount
} CEM_tenTriggerType;

EXTERN uint32 CEM_u32GlobalCycleFraction;
EXTERN uint32 CEM_u32GlobalCycleTime;
EXTERN TEPMAPI_ttEventTime CEM_tGlobalCycleOrigin;
EXTERN uint32 CEM_u32GlobalCycleOriginCount;
EXTERN TEPMAPI_ttEventTime CEM_tEventTimeLast;
EXTERN uint32 CEM_u32CrankErrorCounts;
//ASAM mode=readvalue name="Crank Error Counts" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="counts" format=5.3 help="Crank Error Counts"

void CEM_vStart(uint32* const);
void CEM_vRun(uint32* const);
void CEM_vTerminate(uint32* const);
Bool CEM_boPopulateCrankEdgeArrays(puint16 const, const Bool, IOAPI_tenEdgePolarity);
Bool CEM_boPopulateSyncPointsArray(puint16 const);
void CEM_vPrimaryEventCB(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
void CEM_vPhaseEventCB(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
void CEM_vFreqEventCB(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
TEPMAPI_ttEventTime CEM_ttGetModulePhase(TEPMHA_tenModule);

#endif //CEM_H

