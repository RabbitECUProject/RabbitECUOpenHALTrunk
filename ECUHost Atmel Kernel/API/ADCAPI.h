/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          ADCAPI                                                 */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed AD conversion     */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          ADCAPI.h                                               */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef ADCAPI_H
#define ADCAPI_H

#include "types.h"
#include "IOAPI.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef void (*ADCAPI_tpfResultCB)(IOAPI_tenEHIOResource, uint32);/*CR1_83*/

typedef enum
{
	ADCAPI_en1Sample,
	ADCAPI_en4Samples,
	ADCAPI_en8Samples,
	ADCAPI_en16Samples,
	ADCAPI_en32Samples	
} ADCAPI_tenSamplesAv;

typedef enum
{
	ADCAPI_enDiffGain1,
	ADCAPI_enDiffGain2,
	ADCAPI_enDiffGain4,
	ADCAPI_enDiffGain8,
	ADCAPI_enDiffGain16,	
	ADCAPI_enDiffGain32,
	ADCAPI_enDiffGain64	
} ADCAPI_tenDiffGain;

typedef enum
{
	ADCAPI_en1000Hz = 0,
	ADCAPI_en500Hz = 1,
	ADCAPI_en250Hz = 2,
	ADCAPI_en125Hz = 3,
	ADCAPI_en64Hz = 4,
	ADCAPI_en32Hz = 5,
	ADCAPI_en16Hz = 6,
	ADCAPI_en8Hz = 7,	
	ADCAPI_en4Hz = 8,
	ADCAPI_en2Hz = 9,	
	ADCAPI_en1Hz = 10,	
	ADCAPI_enTrigger1 = 11,
	ADCAPI_enTrigger2 = 12,
	ADCAPI_enTrigger3 = 13,
	ADCAPI_enTrigger4 = 14,
	ADCAPI_enTriggerCount
} ADCAPI_tenTrigger;	
	
typedef struct
{
	ADCAPI_tenSamplesAv enSamplesAv;
	ADCAPI_tenDiffGain enDiffGain;
	ADCAPI_tpfResultCB pfResultCB;	
	ADCAPI_tenTrigger enTrigger;
} ADCAPI_tstADCCB;
	

#endif //ADCAPI_H

