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
#include "MATH.h"
#include "PERADC.h"
#include "ADCAPI.h"
#include "ADCHA.h"
#include "regset.h"

const REGSET_tstReg32Val ADCHA_rastADCReg32Val = ADCHA_nReg32Set;
const ADCHA_tstADCChannel ADCHA_rastADCChannel[] = ADCHA_nChannelInfo;
const IOAPI_tenEHIOResource ADCHA_raenIOResource[] = ADCHA_nIOResourceMap;
uint32 ADCHA_u32PortClockRequested;
uint32 ADCHA_u32Calibrated;
Bool ADCHA_aboModuleBusy[ADCHA_enChannelCount];

static void ADCHA_vBackupCalibration(tstADCModule* pstADC, ADCHA_tstADCCalStruct* pstADCCalStruct);

void ADCHA_vStart(uint32* const pu32Stat)
{
    uint32 u32IDX;

	for (u32IDX = 0; u32IDX < ADCHA_enChannelCount; u32IDX++)
	{
        ADCHA_aboModuleBusy[u32IDX] = false;
	}

#ifdef BUILD_MK60
	ADC_xInitialise(ADC3, ADC3_IRQn);
	ADC_xInitialise(ADC2, ADC2_IRQn);
	ADC_xInitialise(ADC1, ADC1_IRQn);
	ADC_xInitialise(ADC0, ADC0_IRQn);
	REGSET_vInitReg32(&ADCHA_rastADCReg32Val[0]);
#endif

#ifdef BUILD_SAM3X8E
    tstADCModule* pstADCModule = ADC;
	(void)SIM_boEnablePeripheralClock(ADC_IRQn);
	ADCHA_u32PortClockRequested = 1;
    adc_init(pstADCModule, SYS_FREQ_BUS, ADCHA_nADCCLKFREQ, ADCHA_nADCTRSTTIME);
	IRQ_vEnableIRQ(ADC_IRQn, IRQ_enPRIO_8, ADC_vInterruptHandler, NULL);
#endif
}

Bool ADCHA_boBackupCalibrations(void)
{
	Bool boRetCode = FALSE;
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

void ADCHA_vInitChannel(IOAPI_tenEHIOResource enIOResource)
{
#ifdef BUILD_SAM3X8E
    tstADCModule* pstADC = ADCHA_pstGetADCModule(ADCHA_enADC0);
	uint32 u32Channel = ADCHA_rastADCChannel[enIOResource].u32ADChannel;
    adc_enable_channel(pstADC, u32Channel);
	//adc_enable_interrupt(pstADC, 1 << u32Channel);
#endif //BUILD_SAM3X8E
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

Bool ADCHA_boInitiateConversion(ADCHA_tstADCConversion* pstADCConversion, ADCHA_tenQueue enQueue, uint32 u32QueueTail, Bool boPreserveSeq)
{
	Bool boADCInitPending = TRUE;
	tstADCModule* pstADC;
	ADCHA_tstADCConversion* pstADCConversionQueue;

#ifdef BUILD_MK60	
	uint32 u32Conversion;
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

#ifdef BUILD_SAM3X8E
    uint32 u32ConversionIDX;
	enum adc_channel_num_t aenChannels[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint32 u32ChannelMask = 0;
    uint32 u32Temp = 0;
	uint32 u32TerminateInterrupt = 0;

	boADCInitPending = FALSE;	
	ADCHA_aboModuleBusy[0] = true;

	pstADC = ADC;
	//adc_disable_all_channel(pstADC);
	adc_disable_interrupt(pstADC, 0xffff);

	///* Clear flags */
	//for (u32ConversionIDX = 0; u32ConversionIDX < 16; u32ConversionIDX++)
	//{
		//u32ChannelMask = MATH_u32IDXToMask(u32ConversionIDX);
//
		//if (u32ChannelMask == (pstADC->ADC_ISR & u32ChannelMask))
		//{
			///* Dummy read the data register to clear the flag */
			//u32ChannelMask = pstADC->ADC_CDR[u32ConversionIDX];
		//}
	//}

	/* Enable the required channels except the last */
	//pstADCConversionQueue = pstADCConversion;
	//for (u32ConversionIDX = 0; u32ConversionIDX < u32QueueTail; u32ConversionIDX++)
	//{
		//adc_enable_channel(pstADC, (enum adc_channel_num_t)pstADCConversionQueue->stADCChannel.u32ADChannel);
		//pstADCConversionQueue++;
	//}

    /* Configure the sequence */
	u32ChannelMask = 0;
	pstADCConversionQueue = pstADCConversion;
	for (u32ConversionIDX = 0; u32ConversionIDX < u32QueueTail; u32ConversionIDX++)
	{
		u32ChannelMask |= MATH_u32IDXToMask(pstADCConversionQueue->stADCChannel.u32ADChannel);
		pstADCConversionQueue++;
	}

	for (u32ConversionIDX = 0; u32ConversionIDX < 16; u32ConversionIDX++)
	{
	    u32Temp = MATH_u32IDXToMask(u32ConversionIDX);
		u32Temp &= u32ChannelMask;

	    if (0 != u32Temp)
		{
		    aenChannels[u32ConversionIDX] = (enum adc_channel_num_t)u32ConversionIDX;
			u32TerminateInterrupt = u32Temp;
		}
	}

	adc_enable_interrupt(pstADC, u32TerminateInterrupt);

    //if (TRUE == boPreserveSeq)
	{
	    adc_start_sequencer(pstADC);
	    adc_configure_sequence(pstADC, aenChannels, 16);
	}

	adc_start(pstADC);

#endif //BUILD_SAM3X8E

	return boADCInitPending;
}

tstADCModule* ADCHA_pstGetADCModule(ADCHA_tenADCModule enADCModule)
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

#ifdef BUILD_SAM3X8E
switch(enADCModule)
{
	case ADCHA_enADC0: pstADC = ADC; break;
	default: pstADC = NULL; break;
}
#endif //BUILD_SAM3X8E
	
	return pstADC;
}

uint32 ADCHA_u32GetAvailableResultCount(tstADCModule* pstADC)
{
    uint32 u32RetVal = 1;

#ifdef BUILD_SAM3X8E
    u32RetVal = 0;
	uint32 u32ChannelMask = 1;

	while (0x8000 > u32ChannelMask)
	{
		if (u32ChannelMask == (u32ChannelMask & pstADC->ADC_CHSR)) u32RetVal++;
		u32ChannelMask *= 2;
	}
#endif //BUILD_SAM3X8E

    return u32RetVal;
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


IOAPI_tenEHIOResource ADCHA_enGetResourceAndResult(ADCHA_tenADCModule enADCModule, tstADCModule* pstADC, uint32 u32ADCChannel, puint32 pu32ADCResult)
{
    IOAPI_tenEHIOResource enEHIOResource;

#ifdef BUILD_MK60
	/* Get the queue, channel and resource */
	//u32ADCChannel = ADC_SC1_ADCH_MASK & pstADC->SC1[0];//matthew check here against channel
	enEHIOResource = ADC_raenIOResource[32 * enADCModule + u32ADCChannel];
	
	/* Deactivate the AD module */
	pstADC->SC1[0] |= ADC_SC1_ADCH_MASK;
	*pu32ADCResult = pstADC->R[0];
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
	/* Get the queue, channel and resource */
	enEHIOResource = ADCHA_raenIOResource[u32ADCChannel];
	*pu32ADCResult = pstADC->ADC_CDR[u32ADCChannel];
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
#endif //BUILD_SAM3X8E
}

Bool ADCHA_boGetModuleBusy(ADCHA_tenADCModule enADCModule)
{
    Bool boModuleBusy;
	tstADCModule* pstADC = ADCHA_pstGetADCModule(enADCModule);

#ifdef BUILD_MK60
    pstADC = ADCHA_pstGetADCModule(enADCModule);
	boModuleBusy = (ADC_SC1_ADCH_MASK == (ADC_SC1_ADCH_MASK & pstADC->SC1[0])) ? FALSE : TRUE;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
	pstADC = ADCHA_pstGetADCModule(enADCModule);
	boModuleBusy = ADCHA_aboModuleBusy[0];
#endif //BUILD_SAM3X8E

    return boModuleBusy;
}		

void ADCHA_vClearModuleBusy(ADCHA_tenADCModule enADCModule)
{
    ADCHA_aboModuleBusy[enADCModule] = false;
}	
			
