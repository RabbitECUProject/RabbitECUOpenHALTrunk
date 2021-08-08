/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Coolant Temperature Header File                        */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for coolant temperature        */
/*                     measurement                                            */
/* FILE NAME:          CTS.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CTS_H
#define CTS_H

#include <string.h>
#include "IOAPI.h"
#include "SENSORS.h"
#include "USER.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define CTS_nADFiltVal (0x40)
#define CTS_nFanRelayOutput  (RELAY_BIT0)

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _CTS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define CTS_MAXVAL 130000

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 CTS_u32ADCRaw;
EXTERN uint32 CTS_u32ADCFiltered;
//ASAM mode=readvalue name="Coolant Sensor Filtered ADC" type=uint32 offset=0 min=0 max=1048575 m=1 b=0 units="counts" format=5.3 help="CTS Sensor ADC" 
EXTERN GPM6_ttVolts CTS_tSensorVolts;/*CR1_88*/
//ASAM mode=readvalue name="CTS Sensor Voltage" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=3.2 help="CTS Sensor Voltage"
EXTERN GPM6_ttTempC CTS_tTempCRaw;
EXTERN GPM6_ttTempC CTS_tTempCFiltered;
//ASAM mode=readvalue name="Coolant Temperature" type=uint32 offset=0 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.1 help="Coolant Temperature"
EXTERN uint32 CTS_u32Primer;
//ASAM mode=readvalue name="Primer" type=uint32 offset=0 min=0 max=200 m=0.001 b=0 units="us" format=4.1 help="Start Primer"
EXTERN uint32 CTS_u32FuelMultiplier;
EXTERN sint32 CTS_i32StartEnrichmentScaled;
EXTERN sint32 CTS_i32PostStartEnrichmentScaled;
EXTERN Bool CTS_boCTSReady;
EXTERN Bool CTS_boRadFanOn;

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void CTS_vStart(uint32 * const pu32Arg);
void CTS_vRun(uint32* const pu32Arg);
void CTS_vTerminate(uint32* const pu32Arg);
void CTS_vThread(void);
void CTS_vCallBack(puint32 const pu32Arg);

#endif // CTS_H

