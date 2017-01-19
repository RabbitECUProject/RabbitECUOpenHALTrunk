/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      LSU4.X Header File                                     */
/* DESCRIPTION:        This code module initialises the required ADC, DAC     */
/*                     CTRL, spread, table, DIO and TEPM resources for        */
/*                     managing the nernst cell voltage and impedence         */
/*                     measurement for the LSU4.X sensor/s                    */
/* FILE NAME:          LSU4X.h                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef LSU4X_H
#define LSU4X_H

#include "ADCAPI.h"
#include "DACAPI.h"
#include "CTRLAPI.h"
#include "CONV.h"
#include "declarations.h"
#include "IOAPI.h"
#include "LSUH.h"
//#include "mk60f12.h"
#include "SPREADSAPI.h"
#include "TABLESAPI.h"
#include "TEPMAPI.h"
#include "UNITS.h"
#include "USER.h"
#include "USERMATH.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define LSU4X_nDACQueueSize             1u
#define LSU4X_nDACRange                 3200u 
#define LSU4X_nADCRange                 4095u
#define LSU4X_nPWMCycCount              40u
#define LSU4X_nStartACSample            5u
#define LSU4X_nEndACSample              18u
#define LSU4X_nPumpCurrSamples          10u
#define LSU4X_nACSamplesCount           20u
#define LSU4X_nDCSamplesCount           20u
#define LSU4X_nWarmMsMin                4000u
#define LSU4X_nWarmMsMax                10000u
#define LSU4X_nWarmPrePumpMs            2000u
#define LSU4X_nIOSWLResourcePin			EH_IO_IO36
#define LSU4X_nIOSWHResourcePin			EH_IO_IO37
#define LSU4X_nIOIENResourcePinLeft     EH_IO_IO39
#define LSU4X_nIOIENResourcePinRight    EH_IO_Invalid
#define LSU4X_nVSDCInput                EH_IO_IO52
#define LSU4X_nDCDInput                 EH_IO_ADD1
#define LSU4X_nACDInput                 EH_IO_ADD3
/*CR1_105*/

#define LSU4X_nADConfig																																				\
{																																															\
	{LSU4X_nVSDCInput,  IOAPI_enADSE,  ADCAPI_en4Samples, ADCAPI_enDiffGain1, &LSU4X_vADCCallBack, ADCAPI_enTrigger1},	\
	{LSU4X_nACDInput,  IOAPI_enADD,  ADCAPI_en8Samples, ADCAPI_enDiffGain32, &LSU4X_vADCCallBack, ADCAPI_enTrigger1},	\
	{LSU4X_nDCDInput,  IOAPI_enADD,  ADCAPI_en4Samples, ADCAPI_enDiffGain8, &LSU4X_vADCCallBack, ADCAPI_enTrigger1},	\
}

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _LSU4X_C
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
} LSU4X_tstADConfig;


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 LSU4X_au32ACSampleCount[LSU_DEVICE_COUNT];
EXTERN uint32 LSU4X_au32DCSampleCount[LSU_DEVICE_COUNT];
EXTERN uint16 LSU4X_u16PWMPeriod;
//ASAM mode=readvalue name="PWMPeriod" type=uint8 offset=0 min=0 max=5 m=0.001 b=1 units="dl" format=6.0 help="PWM Period"    
EXTERN uint16 LSU4X_u16PWMLastStart;
//ASAM mode=readvalue name="PWMLastStart" type=uint8 offset=0 min=0 max=5 m=0.001 b=1 units="dl" format=6.0 help="PWM Period Last Origin"    
EXTERN GPM6_ttOhms LSU4X_atNernstSenseOhms[LSU_DEVICE_COUNT];
//ASAM mode=readvalue name="Nernst Rout 1" type=uint32 offset=0 min=0 max=1000 m=0.001 b=0 units="Ohm" format=3.0 help="Nernst 1 output impedence" 
//ASAM mode=readvalue name="Nernst Rout 2" type=uint32 offset=4 min=0 max=1000 m=0.001 b=0 units="Ohm" format=3.0 help="Nernst 2 output impedence"  
EXTERN GPM6_ttLambda LSU4X_atLambdaRaw[LSU_DEVICE_COUNT];
//ASAM mode=readvalue name="Lambda 1" type=uint16 offset=0 min=0 max=5 m=0.001 b=0 units="dl" format=4.3 help="Lambda 1 Measurement (dimensionless)" 
//ASAM mode=readvalue name="Lambda 2" type=uint16 offset=2 min=0 max=5 m=0.001 b=0 units="dl" format=4.3 help="Lambda 2 Measurement (dimensionless)"
EXTERN uint32 LSU4X_aau32ACSamples[LSU_DEVICE_COUNT][LSU4X_nACSamplesCount];
//ASAM mode=readvalue name="Samples1" type=uint32 offset=0 min=0 max=5 m=1 b=0 units="n" format=6.0 help="sample 1"  
//ASAM mode=readvalue name="Samples2" type=uint32 offset=4 min=0 max=5 m=1 b=0 units="n" format=6.0 help="sample 2" 
//ASAM mode=readvalue name="Samples3" type=uint32 offset=8 min=0 max=5 m=1 b=0 units="n" format=6.0 help="sample 3" 
//ASAM mode=readvalue name="Samples4" type=uint32 offset=12 min=0 max=5 m=1 b=0 units="n" format=6.0 help="sample 4"
//ASAM mode=readvalue name="Samples5" type=uint32 offset=16 min=0 max=5 m=1 b=0 units="n" format=6.0 help="sample 5"
//ASAM mode=readvalue name="Samples6" type=uint32 offset=20 min=0 max=5 m=1 b=0 units="n" format=6.0 help="sample 6" 
//ASAM mode=readvalue name="Samples7" type=uint32 offset=24 min=0 max=5 m=1 b=0 units="n" format=6.0 help="sample 7" 
//ASAM mode=readvalue name="Samples8" type=uint32 offset=28 min=0 max=5 m=1 b=0 units="n" format=6.0 help="sample 8" 
EXTERN sint32 LSU4X_aas32DCSamples[LSU_DEVICE_COUNT][LSU4X_nDCSamplesCount];
EXTERN uint8 LSU4X_au8ACCallBackWait[LSU_DEVICE_COUNT];


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void LSU4X_vStart(uint32 * const pu32Arg);
void LSU4X_vRun(uint32* const pu32Arg);
void LSU4X_vTerminate(uint32* const pu32Arg);
void LSU4X_vCallBack(uint32* const pu32Arg);
void LSU4X_vThread(void);

#endif // LSU4X_H

