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
#include "ADC.h"
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
bool ADC_boCyclicQueuePending;
ADC_tstADCResult ADC_astResult[ADCHA_enQueueCount][ADC_nConversionsMax];
MSG_tstMBX* ADC_apstMBX[ADCHA_enQueueCount][ADC_nConversionsMax];
sint32 ADC_i32CyclicQueueIDX;

/* Array to determine which queue triggered the last module conversion. Allows
   same AD channel to appear in different queues! */
ADCHA_tenQueue ADC_astADCActiveQueue[ADCHA_enADCModuleCount];

static bool ADC_boInitiateConversion(ADCHA_tstADCConversion*, ADCHA_tenQueue);
static void ADC_vCalibrate(tstADCModule*, uint32, uint32);
static void ADC_vInitInterrupts(IRQn_Type);
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
		/* Waste some time *///matthew review
	}	
	
	ADCHA_vStart(pu32Stat);
	
	CQUEUE_xInit(ADC_astConversionQueue, ADC_nConversionsMax, ADC_astADCConversions);
	CQUEUE_xInit(ADC_astConversionQueue + 1, ADC_nConversionsMax, ADC_astADCConversions + ADC_nConversionsMax);
	CQUEUE_xInit(ADC_astConversionQueue + 2, ADC_nConversionsMax, ADC_astADCConversions + 2 * ADC_nConversionsMax);
	CQUEUE_xInit(ADC_astConversionQueue + 3, ADC_nConversionsMax, ADC_astADCConversions + 3 * ADC_nConversionsMax);
	CQUEUE_xInit(ADC_astConversionQueue + 4, ADC_nConversionsMax, ADC_astADCConversions + 4 * ADC_nConversionsMax);	

	ADC_i32CyclicQueueIDX = -1;
	ADC_boCyclicQueuePending = FALSE;
	OS_xModuleStartOK(*pu32Stat);
}

void ADC_vRun(uint32* const pu32Stat)
{
	uint32 u32CyclicQueueIDX;
	
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
				ADC_boCyclicQueuePending = ADC_boInitiateConversion(&ADC_astADCConversions[ADCHA_enQueueCyclic][ADC_astConversionQueue[ADCHA_enQueueCyclic].u32Head], ADCHA_enQueueCyclic); 				
				break;
			}
		}
	
		CPU_xExitCritical();
	}
}

bool ADC_boBackupCalibrations(void)
{
	return ADCHA_boBackupCalibrations();
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
		
		if (IOAPI_enADD == enEHIOType)
		{
			ADCHA_vInitADCResourcePGA(pstADCConversion);
		}
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
#ifdef BUILD_MK60
	tstADCModule* pstADC;	
#endif //BUILD_MK60
	
	/* Which queue initiated this conversion? */
	u32QueueIDX = (ADCHA_enQueueCyclic == ADC_astADCActiveQueue[enADCModule]) ? 
				(uint32)ADC_i32CyclicQueueIDX :
				ADC_astConversionQueue[ADC_astADCActiveQueue[enADCModule]].u32Head;	

#ifdef BUILD_MK60
	pstADC = ADCHA_pstGetADCModule(enADCModule);
	enEHIOResource = ADCHA_enGetResourceAndResult(enADCModule, pstADC, &u32ADCResult);
	enQueue = ADC_astADCActiveQueue[enADCModule];
#endif //BUILD_MK60
	
	/* Populate the result array - the reference of the element will
	   be posted to the client MBX* queue for callback to user */
	ADC_astResult[enQueue][u32QueueIDX].u32Result = u32ADCResult;
	ADC_astResult[enQueue][u32QueueIDX].enEHIOResource = enEHIOResource;

	pstMBX = ADC_apstMBX[enQueue][u32QueueIDX];
	MSG_vMBXPost(pstMBX, (void*)&ADC_astResult[enQueue][u32QueueIDX]);
	pfClientCB = (tpfClientCB)ADC_astADCConversions[enQueue][u32QueueIDX].pfResultCB;
	
	if ((NULL != pfClientCB) && (MSG_boMBXValid(pstMBX)))
	{
		enErr = CLIENT_enEnqueueCB(pstMBX, pfClientCB);
	}
	
	if (ADCHA_enQueueCyclic == enQueue)
	{
		while (-1 != ADC_i32CyclicQueueIDX)
		{
			ADC_i32CyclicQueueIDX = ((ADC_astConversionQueue[ADCHA_enQueueCyclic].u32Tail - 1) > ADC_i32CyclicQueueIDX) ? ADC_i32CyclicQueueIDX + 1 : -1;
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
	}
	
	ADCHA_vRunConversionQueues();
	
	/* TODO suppress warning */
	(void)enErr;
}

bool ADC_vTriggerQueue(ADCAPI_tenTrigger enTrigger)
{
	ADCHA_tenQueue enQueue;
	bool boResult = FALSE;
	
	if ((ADCAPI_enTrigger1 <= enTrigger) && (ADCAPI_enTriggerCount > enTrigger))
	{
		enQueue = (ADCHA_tenQueue)(enTrigger - ADCAPI_enTrigger1 + 1);	
		ADC_astConversionQueue[enQueue].u32Head = 0;
		ADC_vRunConversionQueues();
		boResult = TRUE;
	}
	
	return boResult;
}

static bool ADC_boInitiateConversion(ADCHA_tstADCConversion* pstADCConversion, ADCHA_tenQueue enQueue)
{
	bool boADCInitPending = TRUE;
	uint32 u32QueueIDX;
	MSG_tenMBXErr enMBXErr = MSG_enOK;

	u32QueueIDX = (ADCHA_enQueueCyclic == enQueue) ? ADC_i32CyclicQueueIDX :
	ADC_astConversionQueue[enQueue].u32Head;

	/* Check if a mailbox is allocated for this conversion */
	if (NULL == ADC_apstMBX[enQueue][u32QueueIDX])
	{
		enMBXErr = MSG_enMBXNew((MSG_tstMBX**)&ADC_apstMBX[enQueue][u32QueueIDX],
		MSG_enADCResult);
	}
		
	if (MSG_enOK == enMBXErr)
	{
		boADCInitPending = ADCHA_boInitiateConversion(pstADCConversion, enQueue, u32QueueIDX);

		/* Remember which queue initiated this conversion */
		ADC_astADCActiveQueue[pstADCConversion->stADCChannel.enADCModule] = enQueue;
		pstADCConversion->u32ControlCount = pstADCConversion->u32ResetCount;
	}

	return boADCInitPending;
}


static void ADC_vCalibrate(tstADCModule* pstADC, uint32 u32ADCIDX, uint32 u32CalFlag)
{
    ADCHA_vCalibrate(pstADC, u32ADCIDX, u32CalFlag);
}


static void ADC_vInitInterrupts(IRQn_Type enIrq)
{
	IRQ_vEnableIRQ(enIrq, NULL, NULL);
}

static void ADC_vRunConversionQueues(void)
{
	ADCHA_vRunConversionQueues();
}


			
			
