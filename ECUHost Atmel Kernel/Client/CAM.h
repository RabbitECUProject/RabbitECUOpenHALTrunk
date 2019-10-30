/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Cyclic Activity Manager Header File                    */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for cyclic activites           */
/*                                                                            */
/* FILE NAME:          MAP.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CAM_H
#define CAM_H

#include <string.h>
#include "FUEL.h"
#include "IOAPI.h"
#include "SENSORS.h"
#include "TEPMAPI.h"
#include "USER.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define CAM_xTicksToRPM(x)      ((60u * SENSORS_nSlowFTMFreq) / ((4u * x)))
#define CAM_nPeriodMs           (10u)
#define CAM_nPeriodRPMTimeout   (600u)
#define CAM_nRPMTransitionCount (10)

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _CAM_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 CAM_u32RPMRawOld;
EXTERN uint32 CAM_u32RPMRaw;		
//ASAM mode=readvalue name="Engine Speed Raw" type=uint32 offset=0 min=0 max=10000 m=1 b=0 units="RPM" format=4.0 help="Engine Speed" 
EXTERN uint32 CAM_u32RPMFiltered;	
//ASAM mode=readvalue name="RPM Transition Counter" type=uint32 offset=0 min=0 max=10000 m=1 b=0 units="RPM" format=4.0 help="RPM Transition Counter"
EXTERN uint32 CAM_u32RPMTransitionCounter;

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void CAM_vStart(uint32 * const pu32Arg);
void CAM_vRun(uint32* const pu32Arg);
void CAM_vTerminate(uint32* const pu32Arg);
void CAM_vCallBack(puint32 const pu32Arg);
void CAM_vEngineSpeedCB(TEPMAPI_ttEventTime tEventTime);

#endif // MAP_H

