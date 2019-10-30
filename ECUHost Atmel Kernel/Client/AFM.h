/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Air Flow Meter Sensor Header File                      */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for air flow measurement       */
/*                                                                            */
/* FILE NAME:          AFM.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef AFM_H
#define AFM_H

#include <string.h>
#include "boosted.h"
#include "IOAPI.h"
#include "USER.h"
#include "units.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define AFM_nVoltsFilteredInitVal	(uint32)(0u)
#define AFM_nADFiltVal						(0x10)

	
#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _AFM_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 AFM_u32ADCRaw;
//ASAM mode=readvalue name="AFM Sensor ADC" type=uint32 offset=0 min=0 max=4095 m=1 b=0 units="dl" format=5.3 help="AFM Sensor ADC Counts"
EXTERN uint32 AFM_u32ADCFiltered;
EXTERN GPM6_ttHertz AFM_tSensorHertz;/*CR1_88*/
//ASAM mode=readvalue name="AFM Sensor Frequency" type=uint32 offset=0 min=0 max=65535 m=1 b=0 units="Hz" format=5.3 help="AFM Sensor Flow"
EXTERN GPM6_ttVolts AFM_tSensorVolts;/*CR1_88*/
//ASAM mode=readvalue name="AFM Sensor Voltage" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=5.3 help="AFM Sensor Flow"
EXTERN GPM6_ttUg AFM_tAirFlowAFMUg;
//ASAM mode=readvalue name="AFM Sensor Mass Flow" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=5.3 help="AFM Sensor Flow"
EXTERN GPM6_ttUg AFM_tAirFlowBackupUg;
//ASAM mode=readvalue name="AFM Sensor Mass Filtered Backup" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=5.3 help="AFM Sensor Flow Filtered"
EXTERN GPM6_ttUg AFM_tAirFlowBackupRawUg;
EXTERN GPM6_ttUg AFM_tAirFlowVEUg;
EXTERN uint16 AFM_u16LearnVE;
//ASAM mode=readvalue name="AFM VE Learn" type=uint16 offset=0 min=0 max=2 m=0.001 b=0 units="dl" format=5.3 help="AFM VE Learn"
		
/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void AFM_vStart(uint32 * const pu32Arg);
void AFM_vRun(uint32* const pu32Arg);
void AFM_vTerminate(uint32* const pu32Arg);
void AFM_vCallBack(puint32 const pu32Arg);

#endif // AFM_H
