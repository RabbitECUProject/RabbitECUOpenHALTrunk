/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      LSUH Header File                                       */
/* DESCRIPTION:        This code module initialises the required ADC, DAC     */
/*                     CTRL, spread, table, DIO and TEPM resources for        */
/*                     managing the heater control for the LSU4.X sensor/s    */
/*                                                                            */
/* FILE NAME:          LSUH.h                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef LSUH_H
#define LSUH_H

#include "ADCAPI.h"
#include "CONV.h"
#include "CTRLAPI.h"
#include "declarations.h"
#include "IOAPI.h"
#include "LSU4X.h"
#include "TEPMAPI.h"
#include "UNITS.h"
#include "USER.h"
#include "USERMATH.h"

#define LSUH_nHMFInput              EH_IO_IIC1_SCL

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define LSUH_nADConfig																																												\
{																																																							\
	{EH_IO_ADD8, IOAPI_enADSE, ADCAPI_en4Samples, ADCAPI_enDiffGain1, &LSUH_vADCCallBack, ADCAPI_enTrigger3}	\
}

#define LSU_DUTY_RES				10u
#define LSUH_nRateHZ				1000u
#define LSUH_nHeaterEffInit			7000u				/* 7 volts effective heater at startup */
#define LSUH_nHeaterSenseOhms		180u
#define LSUH_nHeaterOhmsSamples	    3u
#define LSUH_nMinACSample			50000u
#define LSUH_nMaxACSample			1600000u
#define LSUH_nCurrSenseOhms			0.18
#define	LSUH_nCurrSensePower		0.5
#define LSUH_nMaxDutyDenom			((1000 * LSUH_nCurrSensePower) / LSUH_nCurrSenseOhms)
#define LSUH_nMinDuty				200u
#define LSUH_nMaxDuty				980u
#define LSUH_nCallsIn100Ms			(0.1 * LSUH_nRateHZ)
#define	LSUH_nHeffVLimit			13000u
#define LSUH_nHeffVRampRate			40000u
#define LSUH_nTEPMTimeOut			(10 * LSUH_nRateHZ)
#define LSUH_nHeatCurrFilt			16u

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _LSUH_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* GLOBAL TYPE DEFINITIONS ****************************************************/
/*******************************************************************************
* Description      : Type to hold an AD resource configuration
*******************************************************************************/
typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tenSamplesAv enSamplesAv;
	ADCAPI_tenDiffGain enDiffGain;	
	ADCAPI_tpfResultCB pfResultCB;	
	ADCAPI_tenTrigger enTrigger;
} LSUH_tstADConfig;


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN bool LSUH_aboHeaterIsOn[LSU_DEVICE_COUNT];
EXTERN bool LSUH_aboSensorReady[LSU_DEVICE_COUNT];
EXTERN uint32 LSUH_au32ACADCDeltaFiltered[LSU_DEVICE_COUNT];
EXTERN uint32 LSUH_aau32ADADCSamples[LSU_DEVICE_COUNT][LSUH_nHeaterOhmsSamples];
EXTERN uint32 LSUH_au32ACADCDelta[LSU_DEVICE_COUNT];
EXTERN GPM6_ttOhms LSUH_atHeaterOhms[LSU_DEVICE_COUNT];
//ASAM mode=readvalue name="Heater Ohms 1" type=uint32 offset=0 min=0 max=10 m=0.001 b=0 units="Ohms" format=5.3 help="Heater 1 Ohms"
//ASAM mode=readvalue name="Heater Ohms 2" type=uint32 offset=4 min=0 max=10 m=0.001 b=0 units="Ohms" format=5.3 help="Heater 2 Ohms"


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void LSUH_vStart(uint32 * const pu32Arg);
void LSUH_vRun(uint32* const pu32Arg);
void LSUH_vTerminate(uint32* const pu32Arg);
void LSUH_vCallBack(uint32* const pu32Arg);

#endif // LSUH_H

