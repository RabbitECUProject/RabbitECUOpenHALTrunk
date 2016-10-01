/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          DACAPI                                                 */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed DA conversion     */
/*                     initialisation, functions and DA conversion            */
/*                                                                            */
/* FILE NAME:          DACAPI.h                                               */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef DACAPI_H
#define DACAPI_H

#include "types.h"
#include "IOAPI.h"

/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef uint16 DACAPI_ttOutputVoltage;/*CR1_96*/

typedef enum
{
	DACAPI_enWaterMarkReached,
	DACAPI_enPointerTop,
	DACAPI_enPointerBottom
} DACAPI_tenEventType;

typedef enum
{
	DACAPI_enHardwareTrigger = 0,	
	DACAPI_enSoftwareTrigger = 1
} DACAPI_tenTriggerType;

typedef enum
{
	DACAPI_enQueueDepth1 = 0,
	DACAPI_enQueueDepth2 = 1,
	DACAPI_enQueueDepth3 = 2,
	DACAPI_enQueueDepth4 = 3,
	DACAPI_enQueueDepth5 = 4,
	DACAPI_enQueueDepth6 = 5,
	DACAPI_enQueueDepth7 = 6,
	DACAPI_enQueueDepth8 = 7,
	DACAPI_enQueueDepth9 = 8,
	DACAPI_enQueueDepth10 = 9,
	DACAPI_enQueueDepth11 = 10,
	DACAPI_enQueueDepth12 = 11,
	DACAPI_enQueueDepth13 = 12,
	DACAPI_enQueueDepth14 = 13,
	DACAPI_enQueueDepth15 = 14,
	DACAPI_enQueueDepth16 = 15
} DACAPI_tenQueueDepth;

typedef enum
{
	DACAPI_enWatermark1Word = 0,
	DACAPI_enWatermark2Words = 1,
	DACAPI_enWatermark3Words = 2,
	DACAPI_enWatermark4Words = 3
} DACAPI_tenWatermarkWords;

typedef void (*DACAPI_tpfEventCB)(IOAPI_tenEHIOResource, DACAPI_tenEventType);

typedef struct
{
	DACAPI_tenQueueDepth enQueueDepth;
	DACAPI_tenWatermarkWords enWatermarkWords;
	bool boWaterMarkEventEnable;
	bool boPointerTopEventEnable;
	bool boPointerBottomEventEnable;
	DACAPI_tenTriggerType enTriggerType;
	DACAPI_tpfEventCB pfEventCB;
} DACAPI_tstDACCB;
	

#endif //DACAPI_H

