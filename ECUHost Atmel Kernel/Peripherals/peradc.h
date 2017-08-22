/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      ADC Header File                                        */
/* DESCRIPTION:        ADC                                                    */
/* FILE NAME:          ADC.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef PERADC_H
#define PERADC_H

#include "ADCAPI.h"
#include "IOAPI.h"
#include "SYSAPI.h"
#include "types.h"

#define ADC_nCalFromRecord	1

typedef enum
{
	ADC_enqueueChannelErr,
	ADC_enqueueQueueFullErr
} ADC_tenEnqueueErr;


typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	uint32 u32Result;
} ADC_tstADCResult;


#define ADC_nConversionsMax     10u
#define ADC_nChannels           IO_AD_Count	

void ADC_vStart(uint32* const);
void ADC_vRun(uint32* const);
void ADC_vTerminate(uint32* const);
void ADC_vInterruptHandler(IOAPI_tenEHIOResource, void*);
SYSAPI_tenSVCResult ADC_vInitADCResource(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, ADCAPI_tstADCCB*);
Bool ADC_vTriggerQueue(ADCAPI_tenTrigger);
Bool ADC_boBackupCalibrations(void);
void ADC_vRunConversionQueueHAs(void);

#endif //PERADC_H

