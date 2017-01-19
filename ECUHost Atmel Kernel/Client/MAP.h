/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Manifold Pressure Sensor Header File										*/
/* DESCRIPTION:        This code module initialises the required ADC  				*/
/*										 resources and functions for manifold pressure					*/
/*										 measurement																						*/
/* FILE NAME:          MAP.c                                             			*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef MAP_H
#define MAP_H

#include <string.h>
#include "IOAPI.h"
#include "USER.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define MAP_nVoltsFilteredInitVal	(uint32)(101300u)
#define MAP_nADFiltVal						(0x10)
#define MAP_nSensorGain						(19820u)
#define MAP_nSensorOffset					(6164u)
#define MAP_nAirDensMgpL					(1190u)
	
#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _MAP_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 MAP_u32ADCRaw;
//ASAM mode=readvalue name="MAP Sensor ADC" type=uint32 offset=0 min=0 max=4095 m=1 b=0 units="dl" format=5.3 help="MAP Sensor ADC Counts"
EXTERN uint32 MAP_u32ADCFiltered;
EXTERN GPM6_ttVolts MAP_tSensorVolts;/*CR1_88*/
//ASAM mode=readvalue name="MAP Sensor Voltage" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=5.3 help="MAP Sensor Voltage"
EXTERN GPM6_ttPa MAP_tKiloPaRaw;
EXTERN GPM6_ttPa MAP_tKiloPaFiltered;
//ASAM mode=readvalue name="MAP kPa" type=uint32 offset=0 min=0 max=120 m=0.001 b=0 units="kPa" format=5.3 help="Manifold Pressure"
EXTERN GPM6_ttUg MAP_tManChargeMassUg;
EXTERN sint32 MAP_s32ManDeltaChargeMassPerSUg;
		
/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void MAP_vStart(uint32 * const pu32Arg);
void MAP_vRun(uint32* const pu32Arg);
void MAP_vTerminate(uint32* const pu32Arg);
void MAP_vCallBack(puint32 const pu32Arg);

#endif // MAP_H
