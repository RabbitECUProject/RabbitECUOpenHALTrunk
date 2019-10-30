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
#define SENSORS_nFastPeriod 				(2u)
#define SENSORS_nSlowPeriod					(8u)
#define SENSORS_nADScaleMax					(4095u)
#define SENSORS_nSensorsRefVolts            (5000u)
#define SENSORS_nADRefVolts                 (3300u)
#define SENSORS_nVDivRatio					(128u)		/* 1/2 voltage divider for 3.3V ECUHost */
#define SENSORS_nFastFTMDivisor             TEPMAPI_enDiv32
#define SENSORS_nFastFTMFreq                (SYS_FREQ_BUS / (1 << SENSORS_nFastFTMDivisor))
#define SENSORS_nSlowFTMDivisor             TEPMAPI_enDiv128
#define SENSORS_nSlowFTMFreq                (SYS_FREQ_BUS / (1 << SENSORS_nSlowFTMDivisor))
#define SENSORS_nAFMHertzSamples			(3)


#define VRA_nPullupEnablePin                EH_IO_IO28
#define VRB_nPullupEnablePin                EH_IO_IO45
#define VRA_nVREnablePin                    EH_IO_IO47
#define VRB_nVREnablePin                    EH_IO_IO46
#define VR_nHystHighPin                     EH_IO_IO34
#define VR_nHystLowPin                      EH_IO_IO35
#define CRANK_nInput                        EH_IO_TMR1

#ifdef BUILD_BSP_AFM_FREQ
#define AFM_FREQ_nInput                     EH_IO_TMR2
#endif

#define CTS_nSwitchPin                      EH_IO_IO50
#define CLO2_nSwitchBank1Pin				EH_IO_IO50
#define CLO2_nSwitchBank2Pin				EH_IO_IO49
#define CLO2_nOxBank2Pin                    EH_IO_IO48
#define CLO2_nOxBank1Pin                    EH_IO_IO44
#define CAM_nDebug							EH_IO_IO43

#define SENSORS_nOBDSTT1PID	                6
#define SENSORS_nOBDLTT1PID	                7
#define SENSORS_nOBDSTT2PID	                8
#define SENSORS_nOBDLTT2PID	                9
#define SENSORS_nOBDESTADVPID	            14

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _SENSORS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define SENSORS_nCANBufferMap  \
{3,2,1,0,7,6,5,4,8+3,8+2,8+1,8+0,8+7,8+6,8+5,8+4,16+3,16+2,16+1,16+0,16+7,16+6,16+5,16+4,24+3,24+2,24+1,24+0,24+7,24+6,24+5,24+4}


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN Bool SENSORS_boCTSACBiasHigh;
EXTERN uint8 SENSORS_u8CANCTS;
//ASAM mode=readvalue name="CAN CTS" type=uint8 offset=0 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="CTS CAN"
EXTERN uint8 SENSORS_u8CANATS;
//ASAM mode=readvalue name="CAN ATS" type=uint8 offset=0 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="ATS CAN"
EXTERN uint16 SENSORS_u16CANTPS;
//ASAM mode=readvalue name="CAN TPS" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=5.0 help="TPS CAN"
EXTERN uint8 SENSORS_u8CANPPS;
//ASAM mode=readvalue name="CAN PPS" type=uint8 offset=0 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PPS CAN"
EXTERN uint16 SENSORS_u16CANVSS;
//ASAM mode=readvalue name="CAN VSS" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="VSS CAN"
EXTERN uint16 SENSORS_au16CANAFR[2];
//ASAM mode=readvalue name="CAN AFR" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="VSS AFR"
EXTERN uint8 SENSORS_u8CANSTT[2];
EXTERN uint8 SENSORS_u8CANLTT[2];
EXTERN uint8 SENSORS_u8OBDAdv;
EXTERN Bool SENSORS_boCANCTSNewSample;
EXTERN Bool SENSORS_boCANATSNewSample;
EXTERN Bool SENSORS_boCANTPSNewSample;
EXTERN Bool SENSORS_boCANPPSNewSample;
EXTERN Bool SENSORS_boCANVSSNewSample;
EXTERN Bool SENSORS_aboCANUEGONewSample[2];
EXTERN Bool SENSORS_boOBDAdvNewSample;

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void SENSORS_vStart(uint32 * const pu32Arg);
void SENSORS_vRun(uint32* const pu32Arg);
void SENSORS_vTerminate(uint32* const pu32Arg);
void SENSORS_vCallBack(puint32 const pu32Arg);
Bool SENSORS_boSetupADSE(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOType enEHIOType, ADCAPI_tenSamplesAv enSamplesAv, ADCAPI_tpfResultCB pfResultCB, ADCAPI_tenTrigger enTrigger, puint32 pu32Arg);
void SENSORS_vInvalidateCAN16Data(uint32 u32MSGIDX, uint32 u32DataOffset);

#endif // CTS_H

