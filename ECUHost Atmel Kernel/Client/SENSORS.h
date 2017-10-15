/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Sensors Header                                         */
/* DESCRIPTION:        This code manages the sensors AD conversions           */
/*                                                                            */
/*                                                                            */
/* FILE NAME:          SENSORS.h                                              */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SENSORS_H
#define SENSORS_H

#include <string.h>
#include "IOAPI.h"
#include "USER.h"
#include "SYS.h"
#include "TEPMAPI.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define SENSORS_nFastPeriod 				(1u)
#define SENSORS_nSlowPeriod					(8u)
#define SENSORS_nADScaleMax					(4095u)
#define SENSORS_nSensorsRefVolts            (5000u)
#define SENSORS_nADRefVolts                 (3300u)
#define SENSORS_nVDivRatio					(128u)		/* 1/2 voltage divider for 3.3V ECUHost */
#define SENSORS_nFastFTMDivisor             TEPMAPI_enDiv32
#define SENSORS_nFastFTMFreq                (SYS_FREQ_BUS / (1 << SENSORS_nFastFTMDivisor))
#define SENSORS_nSlowFTMDivisor             TEPMAPI_enDiv128
#define SENSORS_nSlowFTMFreq                (SYS_FREQ_BUS / (1 << SENSORS_nSlowFTMDivisor))

#define TPS_nADInput						EH_IO_ADD9
#define AFM_nADInput                        EH_IO_ADD10
#define MAP_nADInput                        EH_IO_ADD10
#define CTS_nADInput                        EH_IO_ADD6
#define ATS_nADInput                        EH_IO_ADD5
#define VRA_nPullupEnablePin                EH_IO_IO28
#define VRB_nPullupEnablePin                EH_IO_IO45
#define VRA_nVREnablePin                    EH_IO_IO47
#define VRB_nVREnablePin                    EH_IO_IO46
#define VR_nHystHighPin                     EH_IO_IO34
#define VR_nHystLowPin                      EH_IO_IO35
#define CRANK_nInput                        EH_IO_TMR1
#define AFM_FREQ_nInput                     EH_IO_ADD8
#define CTS_nSwitchPin                      EH_IO_IO50
#define CLO2_nOxBank2Pin                    EH_IO_IO48

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _SENSORS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN Bool SENSORS_boCTSACBiasHigh;

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void SENSORS_vStart(uint32 * const pu32Arg);
void SENSORS_vRun(uint32* const pu32Arg);
void SENSORS_vTerminate(uint32* const pu32Arg);
void SENSORS_vCallBack(puint32 const pu32Arg);
Bool SENSORS_boSetupADSE(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOType enEHIOType, ADCAPI_tenSamplesAv enSamplesAv, ADCAPI_tpfResultCB pfResultCB, ADCAPI_tenTrigger enTrigger, puint32 pu32Arg);

#endif // CTS_H

