/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          TepmAPI                                                */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed timed events      */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          TepmAPI.h                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TEPMAPI_H
#define TEPMAPI_H

#ifdef KERNEL_WARNINGS
	#warning define must match kernel value!!!!
#endif
#define TEPMAPI_nKernelEventsMax 4
#define TEPMAPI_nUserEventsMax 4
/*CR1_121*/


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef uint32 TEPMAPI_ttEventTime;
typedef void (*TEPMAPI_tpfEventCB)(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
typedef uint32 TEPMAPI_ttEventCount;

typedef enum
{
	TEPMAPI_enActionReset,
	TEPMAPI_enSetHigh,
	TEPMAPI_enSetLow,
	TEPMAPI_enToggle,
	TEPMAPI_enCapRising,
	TEPMAPI_enCapFalling,
	TEPMAPI_enCapAny,
	TEPMAPI_enNoAction,
	TEPMAPI_enEndProgram,
	TEPMAPI_enResetProgram
} TEPMAPI_tenAction;/*CR1_122*/

typedef enum
{
	TEPMAPI_enMethodReset,
	TEPMAPI_enGlobalLinkedTimeStep,
	TEPMAPI_enGlobalLinkedFractionStep,	
	TEPMAPI_enGlobalLinkedFraction,
	TEPMAPI_enHardLinkedTimeStep,
	TEPMAPI_enHardLinkedFractionStep,	
	TEPMAPI_enGlobalAlarm,
	TEPMAPI_enHardAlarm,
	TEPMAPI_enMaxDelta,
	TEPMAPI_enMinDelta,
	TEPMAPI_enLinkPrimaryProgram,
	TEPMAPI_enLinkVVT1Input,
	TEPMAPI_enLinkVVT2Input,
	TEPMAPI_enLinkVVT3Input,
	TEPMAPI_enLinkVVT4Input,
	TEPMAPI_enLinkFreqInput,
	TEPMAPI_enLinkDutyInput
} TEPMAPI_tenMethod;/*CR1_122*/	

typedef enum
{
	TEPMAPI_enDiv1 = 0,
	TEPMAPI_enDiv2 = 1,
	TEPMAPI_enDiv4 = 2,
	TEPMAPI_enDiv8 = 3,
	TEPMAPI_enDiv16 = 4,
	TEPMAPI_enDiv32 = 5,
	TEPMAPI_enDiv64 = 6,	
	TEPMAPI_enDiv128 = 7
} TEPMAPI_tenPreScalar;/*CR1_122*/

typedef enum
{
	TEPMAPI_enCountUp = 0,
	TEPMAPI_enCountUpDown = 1
} TEPMAPI_tenCountType;/*CR1_122*/
	
typedef struct  
{
	TEPMAPI_tenAction enAction;
	TEPMAPI_tenMethod enMethod;
	TEPMAPI_ttEventTime tEventTime;
	TEPMAPI_tpfEventCB pfEventCB;	
} TEPMAPI_tstTimedUserEvent;/*CR1_122*/

typedef struct  
{
	TEPMAPI_tenAction enAction;
	TEPMAPI_tenMethod enMethod;
	TEPMAPI_ttEventTime* ptEventTime;
	TEPMAPI_ttEventTime tFractionalEventTime;
	TEPMAPI_tpfEventCB pfEventCB;	
	IOAPI_tenEHIOResource enEHIOBitMirrorResource;
	Bool boToothScheduled;
} TEPMAPI_tstTimedKernelEvent;/*CR1_122*/

typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	TEPMAPI_tenPreScalar enPreScalar;	
	TEPMAPI_tenCountType enCountType;
} TEPMAPI_tstTEPMResourceCB;/*CR1_122*/

typedef struct
{
	TEPMAPI_tenAction enAction;
	TEPMAPI_tenPreScalar enPreScalar;	
	Bool boInterruptEnable;
	Bool boAsyncRequestEnable;
	uint32 u32Sequence;
	IOAPI_tenEHIOResource enLinkedResource;
} TEPMAPI_tstTEPMChannelCB;

typedef struct
{
	uint32 u32HighTime;
	uint32 u32LowTime;
	uint32 u32RisingTime;
	uint32 u32FallingTime;
	uint32 u32Delay;
} TEPMAPI_tstTimerMeasurements;

typedef struct 
{
	IOAPI_tenEHIOResource enEHIOResource;
	Bool boCamSyncHighLate;
	uint32 u32CamSyncSampleToothCount;
} TEPMAPI_tstSimpleCamSync;


#endif //TEPMAPI_H

