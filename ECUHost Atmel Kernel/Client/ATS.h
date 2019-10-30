/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Coolant Temperature Header File                        */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for coolant temperature        */
/*                     measurement                                            */
/* FILE NAME:          ATS.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef ATS_H
#define ATS_H

#include <string.h>
#include "IOAPI.h"
#include "SENSORS.h"
#include "USER.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define ATS_nADFiltVal (0x10)

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _ATS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 ATS_u32ADCRaw;
EXTERN uint32 ATS_u32ADCFiltered;
//ASAM mode=readvalue name="Air Sensor Filtered ADC" type=uint32 offset=0 min=0 max=1048575 m=1 b=0 units="counts" format=5.3 help="ATS Sensor ADC" 
EXTERN GPM6_ttVolts ATS_tSensorVolts;/*CR1_88*/
//ASAM mode=readvalue name="ATS Sensor Voltage" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=3.2 help="ATS Sensor Voltage"
EXTERN GPM6_ttTempC ATS_tTempCRaw;
EXTERN GPM6_ttTempC ATS_tTempCPort;
EXTERN GPM6_ttTempC ATS_tTempCFiltered;
//ASAM mode=readvalue name="Air Temperature" type=uint32 offset=0 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.1 help="Air Temperature"
EXTERN uint32 ATS_u32FuelMultiplier;
EXTERN Bool ATS_boATSReady;

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void ATS_vStart(uint32 * const pu32Arg);
void ATS_vRun(uint32* const pu32Arg);
void ATS_vTerminate(uint32* const pu32Arg);
void ATS_vThread(void);
void ATS_vCallBack(puint32 const pu32Arg);

#endif // ATS_H

