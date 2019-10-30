/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Fuelling Header File                                   */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for fuelling calculations      */
/*                     measurement                                            */
/* FILE NAME:          FUEL.c                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef FUEL_H
#define FUEL_H

#include <string.h>
#include "CLO2.h"
#include "IOAPI.h"
#include "SPREADSAPI.h"
#include "TABLESAPI.h"
#include "TEPM.h"
#include "USER.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define FUEL_xUsToSlowTicks(x)          (((SENSORS_nSlowFTMFreq / 100u) * (x))	/ 10000u)		
#define FUEL_nEngineCap                 (1400)
#define FUEL_nInjRespCalcRate           (4)
#define FUEL_nInj4Output                EH_IO_ADD8
#define FUEL_nInj3Output                EH_IO_TMR10
#define FUEL_nInj2Output                EH_IO_TMR11
#define FUEL_nInj1Output                EH_IO_EXTINT
#define FUEL_nShortPulseRangeUs         2000
#define FUEL_nXAFMAxisRef				AFM_tSensorHertz
#define FUEL_nFuelSequenceCount			4
#define FUEL_nSeqModeCountLimit			10

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _FUEL_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN TEPMAPI_ttEventTime 	FUEL_tStartHoldFraction[8];	
EXTERN TEPMAPI_ttEventTime 	FUEL_tTimeHold[8];
EXTERN GPM6_ttPulseUs 			FUEL_tTimeHoldUs[8];	
//ASAM mode=readvalue name="Fuel Injector Hold ms" type=uint32 offset=0 min=0 max=40 m=0.001 b=0 units="ms" format=5.3 help="Fuel Injector Total ms"	
EXTERN GPM6_ttCc						FUEL_tMaxAirFlow;
EXTERN GPM6_ttMcc						FUEL_tPredictedAirFlowMcc;
EXTERN GPM6_ttUg						FUEL_tPredictedAirFlowUg;
EXTERN GPM6_ttG							FUEL_tPredictedFuelFlowUg;
EXTERN GPM6_ttG							FUEL_tPredictedFuelFlowPerInjectionNg[2];
//ASAM mode=readvalue name="Fuel Injector Predicted ms" type=uint32 offset=0 min=0 max=40 m=0.001 b=0 units="ms" format=5.3 help="Fuel Injector Predicted ms"	
EXTERN volatile GPM6_ttPulseUs                  FUEL_tTimePredictedUs[2];	
EXTERN volatile GPM6_ttPulseUs                  FUEL_tTimePredictedUsInput;
EXTERN volatile GPM6_ttPulseUs                 FUEL_tTimePredictedShortOpeningUs;
EXTERN Bool                             FUEL_boFuelPrimed;
EXTERN Bool                             FUEL_u32PrimeCBCount;
EXTERN uint16 FUEL_u16TAFR;
EXTERN Bool FUEL_aboSeqRevCutFlag[8];
EXTERN Bool FUEL_aboSeqOverrunCutFlag[8];
EXTERN uint32 FUEL_u32FuelPumpRunCount;
/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void FUEL_vStart(uint32 * const pu32Arg);
void FUEL_vRun(uint32* const pu32Arg);
void FUEL_vTerminate(uint32* const pu32Arg);
void FUEL_vCallBack(puint32 const pu32Arg);
void FUEL_vCalculateFuellingValues(void);

#endif // FUEL_H
