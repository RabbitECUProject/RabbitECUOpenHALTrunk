/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Throttle Position Sensor Header File                   */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for throttle position          */
/*                     measurement                                            */
/* FILE NAME:          TPS.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TPS_H
#define TPS_H

#include <string.h>
#include "IOAPI.h"
#include "USER.h"
#include "USERCAL.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define TPS_nADFiltVal	(0x80)
#define TPS_nAngleRange	(90000u)

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _MAP_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 TPS_u32ADCRaw;
//ASAM mode=readvalue name="TPS Sensor ADC" type=uint32 offset=0 min=0 max=4095 m=1 b=0 units="dl" format=5.3 help="TPS Sensor ADC Counts"
EXTERN uint32 TPS_u32ADCFiltered;
EXTERN GPM6_ttVolts TPS_tSensorVolts;/*CR1_88*/
//ASAM mode=readvalue name="TPS Sensor Voltage" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=5.3 help="TPS Sensor Voltage"
EXTERN GPM6_ttTheta TPS_tThetaRaw;
EXTERN GPM6_ttTheta TPS_tThetaFiltered;
//ASAM mode=readvalue name="Throttle Angle" type=uint32 offset=0 min=0 max=90 m=0.001 b=0 units="degrees" format=5.3 help="Throttle Angle"

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void TPS_vStart(uint32 * const pu32Arg);
void TPS_vRun(uint32* const pu32Arg);
void TPS_vTerminate(uint32* const pu32Arg);
void TPS_vCallBack(puint32 const pu32Arg);

#endif // TPS_H
