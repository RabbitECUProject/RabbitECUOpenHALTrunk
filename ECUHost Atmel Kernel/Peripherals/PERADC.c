/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <string.h>
#include <stddef.h>
#include "CPUAbstract.h"
#include "IOAPI.h"
#include "ADCAPI.h"
#include "types.h"
#include "ADCHA.h"
#include "PERADC.h"
#include "client.h"
#include "CQUEUE.h"
#include "CRC16.h"
#include "declarations.h"
#include "FEE.h"
#include "IRQ.h"
#include "MSG.h"
#include "regset.h"
#include "SIM.h"
#include "sys.h"

ADCHA_tstADCConversion ADC_astADCConversions[ADCHA_enQueueCount][ADC_nConversionsMax];
CQUEUE_tstQueue ADC_astConversionQueue[ADCHA_enQueueCount];
Bool ADC_boCyclicQueuePending;
ADC_tstADCResult ADC_astResult[ADCHA_enQueueCount][ADC_nConversionsMax];
MSG_tstMBX* ADC_apstMBX[ADCHA_enQueueCount][ADC_nConversionsMax];
sint32 ADC_i32CyclicQueueIDX;

/* Array to determine which queue triggered the last module conversion. Allows
   same AD channel to appear in different queues! */
ADCHA_tenQueue ADC_astADCActiveQueue[ADCHA_enADCModuleCount];

static Bool ADC_boInitiateConversion(ADCHA_tstADCConversion*, ADCHA_tenQueue, uint32);
static void ADC_vRunConversionQueues(void);


void ADC_vStart(uint32* const pu32Stat)
{	
	uint32 u32QueueIDX;
	uint32 u32QueueElementIDX;
	
	for (u32QueueIDX = 0; u32QueueIDX < ADCHA_enQueueCount; u32QueueIDX++)
	{
		for (u32QueueElementIDX = 0; u32QueueElementIDX < ADC_nConversionsMax; u32QueueElementIDX++)
		{
			/* NULL the mailbox pointer array */
			ADC_apstMBX[u32QueueIDX][u32QueueElementIDX] = NULL;
		}
	}
	
	for (u32QueueIDX = 0; u32QueueIDX < 0xffffff; u32QueueIDX++)
	{
		;/* Waste some time *///matthew review
	}	
	
	ADCHA_vStart(pu32Stat);
	
	CQUEUE_xInit(ADC_astConversionQueue, ADC_nConversionsMax, ADC_astADCConversions);
	CQUEUE_xInit(ADC_astConversionQueue + 1, ADC_nConversionsMax, (uint32)ADC_astADCConversions + sizeof(ADCHA_tstADCConversion) * ADC_nConversionsMax * 1);
	CQUEUE_xInit(ADC_astConversionQueue + 2, ADC_nConversionsMax, (uint32)ADC_astADCConversions + sizeof(ADCHA_tstADCConversion) * ADC_nConversionsMax * 2);
	CQUEUE_xInit(ADC_astConversionQueue + 3, ADC_nConversionsMax, (uint32)ADC_astADCConversions + sizeof(ADCHA_tstADCConversion) * ADC_nConversionsMax * 3);
	CQUEUE_xInit(ADC_astConversionQueue + 4, ADC_nConversionsMax, (uint32)ADC_astADCConversions + sizeof(ADCHA_tstADCConversion) * ADC_nConversionsMax * 4);	

	ADC_i32CyclicQueueIDX = -1;
	ADC_boCyclicQueuePending = FALSE;
	OS_xModuleStartOK(*pu32Stat);
}

void ADC_vRun(uint32* const pu32Stat)
{
	uint32 u32CyclicQueueIDX;
	static uint32 u32ADCQueueStuckCount;
	ADCHA_tenQueue enQueue;
	
	if ((ADC_astConversionQueue[ADCHA_enQueueCyclic].u32Tail > ADC_astConversionQueue[ADCHA_enQueueCyclic].u32Head) && (-1 == ADC_i32CyclicQueueIDX))
	{				
		for (u32CyclicQueueIDX = ADC_astConversionQueue[ADCHA_enQueueCyclic].u32Head;
				 u32CyclicQueueIDX <= ADC_astConversionQueue[ADCHA_enQueueCyclic].u32Tail;
				 u32CyclicQueueIDX++)
		{
			if (0 < ADC_astADCConversions[ADCHA_enQueueCyclic][u32CyclicQueueIDX].u32ControlCount)
			{
				ADC_astADCConversions[ADCHA_enQueueCyclic][u32CyclicQueueIDX].u32ControlCount -= 1;	
			}
		}
		
		/* EC - don't want AD conversion complete interrupting here */
		CPU_xEnterCritical();
		
		for (u32CyclicQueueIDX = ADC_astConversionQueue[0].u32Head;
				 u32CyclicQueueIDX <= ADC_astConversionQueue[0].u32Tail;
				 u32CyclicQueueIDX++)
		{		
			if (0 == ADC_astADCConversions[ADCHA_enQueueCyclic][u32CyclicQueueIDX].u32ControlCount)
			{
				ADC_i32CyclicQueueIDX = u32CyclicQueueIDX;
				//ADC_boCyclicQueuePending = ADC_boInitiateConversion(&ADC_astADCConversions[ADCHA_enQueueCyclic][ADC_astConversionQueue[ADCHA_enQueueCyclic].u32Head], ADCHA_enQueueCyclic, ADC_astConversionQueue[0].u32Tail); 				
				break;
			}
		}
	
		CPU_xExitCritical();
	}

	/* Check if the ADC queues are stuck */
	for (enQueue = ADCHA_enQueueCyclic; enQueue < ADCHA_enQueueCount; enQueue++)
	{
		if (ADC_astConversionQueue[enQueue].u32Head != ADC_astConversionQueue[enQueue].u32Tail)
		{
			u32ADCQueueStuckCount++;
		}
	}

	if (5 < u32ADCQueueStuckCount)
	{
		ADCHA_vReset(ADCHA_enADC0);
		u32ADCQueueStuckCount = 0;
	}
}


SYSAPI_tenSVCResult ADC_vInitADCResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enEHIOType, ADCAPI_tstADCCB* pstADCCB)
{
	uint32 u32QueueIDX;
	SYSAPI_tenSVCResult enResult = SYSAPI_enOK;
	ADCHA_tstADCConversion* pstADCConversion;
	
	u32QueueIDX = (ADCAPI_enTrigger1 > pstADCCB->enTrigger) ? 0 : pstADCCB->enTrigger - ADCAPI_en1Hz;		
	
	if (!CQUEUE_xIsFull(ADC_astConversionQueue + u32QueueIDX))
	{
		CPU_xEnterCritical();

		pstADCConversion = &ADC_astADCConversions[u32QueueIDX][0] +
		ADC_astConversionQueue[u32QueueIDX].u32Tail;
		ADCHA_vInitConversion(enIOResource, pstADCConversion, pstADCCB, u32QueueIDX);
		
		switch (pstADCCB->enTrigger)
		{
			case ADCAPI_en1000Hz: pstADCConversion->u32ResetCount = 1; break;
			case ADCAPI_en500Hz: pstADCConversion->u32ResetCount = 2; break;
			case ADCAPI_en250Hz: pstADCConversion->u32ResetCount = 4; break;
			case ADCAPI_en125Hz: pstADCConversion->u32ResetCount = 8; break;
			case ADCAPI_en64Hz: pstADCConversion->u32ResetCount = 31; break;
			case ADCAPI_en32Hz: pstADCConversion->u32ResetCount = 32; break;
			case ADCAPI_en16Hz: pstADCConversion->u32ResetCount = 63; break;
			case ADCAPI_en8Hz: pstADCConversion->u32ResetCount = 125; break;	
			case ADCAPI_en4Hz: pstADCConversion->u32ResetCount = 250; break;
			case ADCAPI_en2Hz: pstADCConversion->u32ResetCount = 500; break;	
			case ADCAPI_en1Hz: pstADCConversion->u32ResetCount = 1000; break;
			default: pstADCConversion->u32ResetCount = 1000; break;
		}
		
		CQUEUE_xAddItem(ADC_astConversionQueue + u32QueueIDX);

		/* Remove to leave it in the queue but exhaust the queue */
		CQUEUE_xRemoveItem(ADC_astConversionQueue + u32QueueIDX);
		
		if (IOAPI_enADD == enEHIOType)
		{
			ADCHA_vInitADCResourcePGA(pstADCConversion);
		}

		ADCHA_vInitChannel(enIOResource);
		CPU_xExitCritical();
	}
	else
	{
		enResult = SYSAPI_enQueueFull;	
	}	
	
	return enResult;
}

void ADC_vTerminate(uint32* const u32Stat)
{

}

void ADC_vInterruptHandler(IOAPI_tenEHIOResource enEHIOResource, void* pvData)
{
	uint32 enADCModule;
	uint32 u32ADCChannel;
	MSG_tstMBX* pstMBX;
	uint32 u32QueueIDX;
	ADCHA_tenQueue enQueue;
	CLIENT_tenErr enErr;
	tpfClientCB pfClientCB;
	uint32 u32ADCResult;
	tstADCModule* pstADC;	
	uint32 u32ADCResultAvailableCount;
	uint32 u32DequeueIDX;

	enADCModule = enEHIOResource - EH_VIO_ADC0;
	
	/* Which queue initiated this conversion? */
	u32QueueIDX = (ADCHA_enQueueCyclic == ADC_astADCActiveQueue[enADCModule]) ? 
				(uint32)ADC_i32CyclicQueueIDX :
				ADC_astConversionQueue[ADC_astADCActiveQueue[enADCModule]].u32Head;	

	pstADC = ADCHA_pstGetADCModule(enADCModule);
    u32ADCResultAvailableCount = ADCHA_u32GetAvailableResultCount(pstADC);
	u32ADCResultAvailableCount = MIN(u32ADCResultAvailableCount, ADC_astConversionQueue[ADC_astADCActiveQueue[enADCModule]].u32Tail);

	for (u32DequeueIDX = u32QueueIDX; u32DequeueIDX < u32QueueIDX + u32ADCResultAvailableCount; u32DequeueIDX++)
	{
		enQueue = ADC_astADCActiveQueue[enADCModule];
	    u32ADCChannel = ADC_astADCConversions[enQueue][u32DequeueIDX].stADCChannel.u32ADChannel;
		enEHIOResource = ADCHA_enGetResourceAndResult(enADCModule, pstADC, u32ADCChannel, &u32ADCResult);
	
		/* Populate the result array - the reference of the element will
		   be posted to the client MBX* queue for callback to user */
		ADC_astResult[enQueue][u32DequeueIDX].u32Result = u32ADCResult;
		ADC_astResult[enQueue][u32DequeueIDX].enEHIOResource = enEHIOResource;

		pstMBX = ADC_apstMBX[enQueue][u32DequeueIDX];
		MSG_vMBXPost(pstMBX, (void*)&ADC_astResult[enQueue][u32DequeueIDX]);
		pfClientCB = (tpfClientCB)ADC_astADCConversions[enQueue][u32DequeueIDX].pfResultCB;
	
		if ((NULL != pfClientCB) && (MSG_boMBXValid(pstMBX)))
		{
			enErr = CLIENT_enEnqueueCB(pstMBX, pfClientCB);
		}
	
		if (ADCHA_enQueueCyclic == enQueue)
		{
			while (-1 != ADC_i32CyclicQueueIDX)
			{
				ADC_i32CyclicQueueIDX = ((sint32)(ADC_astConversionQueue[ADCHA_enQueueCyclic].u32Tail - 1) > ADC_i32CyclicQueueIDX) ? ADC_i32CyclicQueueIDX + 1 : -1;
				if (-1 != ADC_i32CyclicQueueIDX)
				{
					if (0 == ADC_astADCConversions[ADCHA_enQueueCyclic][ADC_i32CyclicQueueIDX].u32ControlCount)
					{
						break;
					}
				}
			}
		}
		else
		{
			CQUEUE_xRemoveItem(ADC_astConversionQueue + enQueue);

			if (ADC_astConversionQueue[enQueue].u32Head == ADC_astConversionQueue[enQueue].u32Tail)
			{
			    /* Release the ADC module now the conversion queue has been processed */
			    ADCHA_vClearModuleBusy(enADCModule);
			}
		}
	}

	ADC_vRunConversionQueues();
	
	/* TODO suppress warning */
	(void)enErr;
}

Bool ADC_vTriggerQueue(ADCAPI_tenTrigger enTrigger)
{
	ADCHA_tenQueue enQueue;
	Bool boResult = FALSE;
	
	if ((ADCAPI_enTrigger1 <= enTrigger) && (ADCAPI_enTriggerCount > enTrigger))
	{
		enQueue = (ADCHA_tenQueue)(enTrigger - ADCAPI_enTrigger1 + 1);	

		if (false == ADCHA_boGetModuleBusy(ADCHA_enADC0))
		{
			ADC_astConversionQueue[enQueue].u32Head = 0;
			ADC_vRunConversionQueues();
			boResult = TRUE;
		}
	}
	
	return boResult;
}

static Bool ADC_boInitiateConversion(ADCHA_tstADCConversion* pstADCConversion, ADCHA_tenQueue enQueue, uint32 u32QueueTail)
{
	Bool boADCInitPending = TRUE;
	uint32 u32QueueIDX;
	MSG_tenMBXErr enMBXErr = MSG_enOK;
	uint32 u32ConversionIDX;

	u32QueueIDX = (ADCHA_enQueueCyclic == enQueue) ? (uint32)ADC_i32CyclicQueueIDX :
	ADC_astConversionQueue[enQueue].u32Head;

	for (u32ConversionIDX = u32QueueIDX; u32ConversionIDX < u32QueueTail; u32ConversionIDX++)
	{
		/* Check if a mailbox is allocated for this conversion */
		if (NULL == ADC_apstMBX[enQueue][u32ConversionIDX])
		{
			enMBXErr = MSG_enMBXNew((MSG_tstMBX**)&ADC_apstMBX[enQueue][u32ConversionIDX],
			MSG_enADCResult);
		}
	}
		
	if (MSG_enOK == enMBXErr)
	{
		/* Remember which queue initiated this conversion */
		ADC_astADCActiveQueue[pstADCConversion->stADCChannel.enADCModule] = enQueue;
		boADCInitPending = ADCHA_boInitiateConversion(pstADCConversion, enQueue, u32QueueTail, FALSE);
		pstADCConversion->u32ControlCount = pstADCConversion->u32ResetCount;
	}

	return boADCInitPending;
}


static void ADC_vRunConversionQueues(void)
{
	ADCHA_tenQueue enQueue;
	Bool boModuleBusy[ADCHA_enADCModuleCount];
	ADCHA_tenADCModule enADCModule;
	Bool boConversionPending;
	ADCHA_tstADCConversion* pstConversion;
	uint32 u32QueueIDX;
	
	for(enADCModule = ADCHA_enADC0; enADCModule < ADCHA_enADCModuleCount; enADCModule++)
	{
		boModuleBusy[enADCModule] = ADCHA_boGetModuleBusy(enADCModule);
	}
	
	for (enQueue = ADCHA_enQueueCyclic; enQueue < ADCHA_enQueueCount; enQueue++)
	{
		switch (enQueue)
		{
			case ADCHA_enQueueCyclic:
			{
				if (-1 < ADC_i32CyclicQueueIDX)
				{
					pstConversion = &ADC_astADCConversions[enQueue][ADC_i32CyclicQueueIDX];
					enADCModule = pstConversion->stADCChannel.enADCModule;
					
					if (FALSE == boModuleBusy[enADCModule])
					{
						boConversionPending = ADC_boInitiateConversion(pstConversion, enQueue, ADC_astConversionQueue[enQueue].u32Tail);
						if (!boConversionPending)
						{
							boModuleBusy[enADCModule] = TRUE;
						}
					}
				}
				break;
			}
			case ADCHA_enQueueTriggered1:
			case ADCHA_enQueueTriggered2:
			case ADCHA_enQueueTriggered3:
			case ADCHA_enQueueTriggered4:
			{
                if (false == CQUEUE_xIsEmpty((CQUEUE_tstQueue*)&ADC_astConversionQueue + enQueue))
				{
					u32QueueIDX = ADC_astConversionQueue[enQueue].u32Head;
					pstConversion = &ADC_astADCConversions[enQueue][u32QueueIDX];
					enADCModule = pstConversion->stADCChannel.enADCModule;
					
					if (FALSE == boModuleBusy[enADCModule])
					{
						boConversionPending = ADC_boInitiateConversion(pstConversion, enQueue, ADC_astConversionQueue[enQueue].u32Tail);
						
						if (!boConversionPending)
						{
							boModuleBusy[enADCModule] = TRUE;
						}
					}
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
}


			
			
