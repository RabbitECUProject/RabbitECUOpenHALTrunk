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
#include "ADC.h"
#include "ADCAPI.h"
#include "ADCHA.h"
#include "regset.h"

const REGSET_tstReg32Val ADCHA_rastADCReg32Val;// = ADCHA_nReg32Set;
const ADCHA_tstADCChannel ADCHA_rastADCChannel[1];// = ADCHA_nChannelInfo;
const IOAPI_tenEHIOResource ADCHA_raenIOResource[1];// = ADCHA_nIOResourceMap;
uint32 ADCHA_u32PortClockRequested;
uint32 ADCHA_u32Calibrated;

static void ADCHA_vBackupCalibration(tstADCModule* pstADC, ADCHA_tstADCCalStruct* pstADCCalStruct);
static tstADCModule* ADCHA_pstGetADCModule(ADCHA_tenADCModule);

void ADCHA_vStart(uint32* const pu32Stat)
{
#ifdef BUILD_MK60
	ADC_xInitialise(ADC3, ADC3_IRQn);
	ADC_xInitialise(ADC2, ADC2_IRQn);
	ADC_xInitialise(ADC1, ADC1_IRQn);
	ADC_xInitialise(ADC0, ADC0_IRQn);
	REGSET_vInitReg32(&ADCHA_rastADCReg32Val[0]);
#endif
}

bool ADCHA_boBackupCalibrations(void)
{
	bool boRetCode = FALSE;
#ifdef BUILD_MK60	
	ADCHA_tstADCCalStruct* pstADCCalStruct = (ADCHA_tstADCCalStruct*)FEE_ADCREC_ADDRESS;
	
	if (TRUE == FEE_boCheckPartition())
	{
		ADCHA_vBackupCalibration(ADC0, pstADCCalStruct++);
		ADCHA_vBackupCalibration(ADC1, pstADCCalStruct++);
		ADCHA_vBackupCalibration(ADC2, pstADCCalStruct++);
		ADCHA_vBackupCalibration(ADC3, pstADCCalStruct++);
		boRetCode = TRUE;
	}
#endif	
	return boRetCode;
}

void ADCHA_vInitADCResourcePGA(ADCHA_tstADCConversion* pstADCConversion)
{
#ifdef BUILD_MK60
    tstADCModule* pstADC;
    /* Set the differential gain */
    pstADC = ADCHA_pstGetADCModule(pstADCConversion->stADCChannel.enADCModule);	
    pstADC->PGA = (ADC_PGA_PGAG(pstADCCB->enDiffGain) | ADC_PGA_PGAEN_MASK);
#endif //BUILD_MK60
}

void ADCHA_vTerminate(uint32* const u32Stat)
{

}

bool ADCHA_boInitiateConversion(ADCHA_tstADCConversion* pstADCConversion, ADCHA_tenQueue enQueue, uint32 u32QueueIDX)
{
	bool boADCInitPending = TRUE;
	tstADCModule* pstADC;
	uint32 u32Conversion;

#ifdef BUILD_MK60	
	pstADC = ADCHA_pstGetADCModule(pstADCConversion->stADCChannel.enADCModule);
	
	if (ADC_SC1_ADCH_MASK == (ADC_SC1_ADCH_MASK & pstADC->SC1[0]))
	{
		boADCInitPending = FALSE;		

		pstADC->CFG2 &= ~ADC_CFG2_MUXSEL_MASK;
		pstADC->CFG2 |= (pstADCConversion->stADCChannel.enChannelAB << ADC_CFG2_MUXSEL_SHIFT) |
									ADC_CFG2_ADHSC_MASK;
						
		pstADC->SC3 = (ADC_SC3_AVGS_MASK | ADC_SC3_AVGE_MASK | (pstADCConversion->stADCChannel.u32Samples));			
		u32Conversion = ADC_SC1_ADCH_MASK & pstADCConversion->stADCChannel.u32ADChannel;
		u32Conversion |= ADC_SC1_AIEN_MASK;
			
		if (TRUE == pstADCConversion->stADCChannel.boIsDiff)
		{
			u32Conversion |= ADC_SC1_DIFF_MASK;
		}
			
		pstADC->SC1[0] = u32Conversion;		
	}
	
#endif //BUILD_MK60	
	return boADCInitPending;
}

static tstADCModule* ADCHA_pstGetADCModule(ADCHA_tenADCModule enADCModule)
{
	tstADCModule* pstADC = NULL;
	
#ifdef BUILD_MK60
	switch(enADCModule)
	{
		case ADCHA_enADC0: pstADC = ADC0; break;
		case ADCHA_enADC1: pstADC = ADC1; break;
		case ADCHA_enADC2: pstADC = ADC2; break;
		case ADCHA_enADC3: pstADC = ADC3; break;		
		default: pstADC = NULL; break;
	}
#endif //BUILD_MK60
	
	return pstADC;
}

void ADCHA_vCalibrate(tstADCModule* pstADC, uint32 u32ADCIDX, uint32 u32CalFlag)
{
#ifdef BUILD_MK60
	uint16 u16Temp;
	uint16 u16CRC;
	ADCHA_tstADCCalStruct* pstADCCalStruct = (ADCHA_tstADCCalStruct*)FEE_ADCREC_ADDRESS + u32ADCIDX;
			
	u16CRC = CRC16_u16CalcCRC(0xffff, (puint8)pstADCCalStruct, sizeof(ADCHA_tstADCCalStruct) - sizeof(uint32));

#if (ADC_nCalFromRecord)
	if ((uint32)u16CRC != pstADCCalStruct->u32CRC)
#else
	if (true)
#endif			
	{
		pstADC->SC2 &= ~ ADC_SC2_ADTRG_MASK;
		pstADC->SC3 |= ADC_SC3_CAL_MASK;
		
		while (0 != (pstADC->SC2 & ADC_SC2_ADACT_MASK)) {};
		
		if (0 == (pstADC->SC3 & ADC_SC3_CALF_MASK))
		{
			u16Temp = 0;		
			u16Temp += pstADC->CLP0;
			u16Temp += pstADC->CLP1;
			u16Temp += pstADC->CLP2;
			u16Temp += pstADC->CLP3;
			u16Temp += pstADC->CLP4;
			u16Temp += pstADC->CLPS;		
			u16Temp /= 2;
			u16Temp |= 0x8000;
			pstADC->PG = u16Temp;	
			
			u16Temp = 0;		
			u16Temp += pstADC->CLM0;
			u16Temp += pstADC->CLM1;
			u16Temp += pstADC->CLM2;
			u16Temp += pstADC->CLM3;
			u16Temp += pstADC->CLM4;
			u16Temp += pstADC->CLMS;		
			u16Temp /= 2;
			u16Temp |= 0x8000;
			pstADC->MG = u16Temp;			
			
			ADC_u32Calibrated |= u32CalFlag;
		}
	}
	else
	{
		pstADC->OFS = pstADCCalStruct->u32OFS;
		pstADC->PG = pstADCCalStruct->u32PG;
		pstADC->MG = pstADCCalStruct->u32MG;		
	}
#endif //BUILD_MK60
}

static void ADCHA_vBackupCalibration(tstADCModule* pstADC, ADCHA_tstADCCalStruct* pstADCCalStruct)
{	
#ifdef BUILD_MK60
	puint8 pu8ADCCalStruct = (puint8)pstADCCalStruct;
	ADCHA_tstADCCalStruct stADCCalStruct;
	uint16 u16CRC;
	
	stADCCalStruct.u32OFS =	pstADC->OFS;	
	stADCCalStruct.u32PG = pstADC->PG;	
	stADCCalStruct.u32MG =	pstADC->MG;	
	u16CRC = CRC16_u16CalcCRC(0xffff, pu8ADCCalStruct, sizeof(ADCHA_tstADCCalStruct) - sizeof(uint32));
	stADCCalStruct.u32CRC = (uint32)(u16CRC);
	
	FEE_boWriteNVM((puint8)&stADCCalStruct, (puint8)pstADCCalStruct, sizeof(ADCHA_tstADCCalStruct));
#endif //BUILD_MK60
}


IOAPI_tenEHIOResource ADCHA_enGetResourceAndResult(ADCHA_tenADCModule enADCModule, tstADCModule* pstADC, puint32 pu32ADCResult)
{
    IOAPI_tenEHIOResource enEHIOResource;
	uint32 u32ADCChannel;

#ifdef BUILD_MK60
	/* Get the queue, channel and resource */
	u32ADCChannel = ADC_SC1_ADCH_MASK & pstADC->SC1[0];
	enEHIOResource = ADC_raenIOResource[32 * enADCModule + u32ADCChannel];
	
	/* Deactivate the AD module */
	pstADC->SC1[0] |= ADC_SC1_ADCH_MASK;
	*pu32ADCResult = pstADC->R[0];
#endif //BUILD_MK60
	return enEHIOResource;
}

void ADCHA_vInitConversion(IOAPI_tenEHIOResource enIOResource, ADCHA_tstADCConversion* pstADCConversion, ADCAPI_tstADCCB* pstADCCB, uint32 u32QueueIDX)
{
#ifdef BUILD_MK60
	pstADCConversion->stADCChannel.enEHIOResource = enIOResource;
	pstADCConversion->stADCChannel.enADCModule = ADCHA_rastADCChannel[enIOResource].enADCModule;
	pstADCConversion->stADCChannel.u32ADChannel = ADCHA_rastADCChannel[enIOResource].u32ADChannel;
	pstADCConversion->stADCChannel.enChannelAB = ADCHA_rastADCChannel[enIOResource].enChannelAB;
	pstADCConversion->stADCChannel.boIsDiff = ADCHA_rastADCChannel[enIOResource].boIsDiff;
	pstADCConversion->stADCChannel.u32Samples = pstADCCB->enSamplesAv;
	pstADCConversion->pfResultCB = pstADCCB->pfResultCB;
	pstADCConversion->u32ControlCount = 0;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
pstADCConversion->stADCChannel.enEHIOResource = enIOResource;
pstADCConversion->stADCChannel.enADCModule = ADCHA_rastADCChannel[enIOResource].enADCModule;
pstADCConversion->stADCChannel.u32ADChannel = ADCHA_rastADCChannel[enIOResource].u32ADChannel;
pstADCConversion->stADCChannel.boIsDiff = ADCHA_rastADCChannel[enIOResource].boIsDiff;
pstADCConversion->pfResultCB = pstADCCB->pfResultCB;
pstADCConversion->u32ControlCount = 0;
#endif //BUILD_MK60
}

void ADCHA_vRunConversionQueues(void)
{
#ifdef BUILD_MK60
	ADCHA_tenQueue enQueue;	
	bool boModuleBusy[ADCHA_enADCModuleCount];
	ADCHA_tenADCModule enADCModule;
	tstADCModule* pstADC;
	bool boConversionPending;
	ADCHA_tstADCConversion* pstConversion;
	uint32 u32QueueIDX;
	
	for(enADCModule = ADCHA_enADC0; enADCModule < ADCHA_enADCModuleCount; enADCModule++)
	{
		pstADC = ADCHA_pstGetADCModule(enADCModule);
		boModuleBusy[enADCModule] = (ADC_SC1_ADCH_MASK == (ADC_SC1_ADCH_MASK & pstADC->SC1[0])) ? FALSE : TRUE;		
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
						boConversionPending = ADC_boInitiateConversion(pstConversion, enQueue);
						if (!boConversionPending)
						{
							boModuleBusy[enADCModule] = TRUE;
						}
					}
				}				
				break;
			}
			case ADC_enQueueTriggered1:
			case ADC_enQueueTriggered2:
			case ADC_enQueueTriggered3:
			case ADC_enQueueTriggered4:
			{
				if (!CQUEUE_boIsEmpty(ADC_astConversionQueue[enQueue]))
				{
					u32QueueIDX = ADC_astConversionQueue[enQueue].u32Head;
					pstConversion = &ADC_astADCConversions[enQueue][u32QueueIDX];
					enADCModule = pstConversion->stADCChannel.enADCModule;
					
					if (FALSE == boModuleBusy[enADCModule])
					{
						boConversionPending = ADC_boInitiateConversion(pstConversion, enQueue);
						
						if (!boConversionPending)
						{
							//CQUEUE_vRemoveItem(stConversionQueue[enQueue]);
							boModuleBusy[enADCModule] = TRUE;
						}
					}
				}
				break;
			}
		}
	}
#endif //BUILD_MK60
}


			
			
