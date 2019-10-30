/******************************************************************************/
/*    Copyright (c) 2018 MD Automotive Controls. Original Work.               */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   20-06-2018 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
/* Licensing Information
Licensor: MDAC
Licensed to: PRD
Licensed software: ECUHost Kernel for Rabbit ECU
License number: XXX
*/


#include "CPUAbstract.h"
#include "kernelio.h"

const IO_tstEHPadResource IO_rastEHPadResource[IO_Total_Discrete_Count + KERNEL_DISCRETE_HIDDEN + 1] = IO_nPadResourceMap;
IOAPI_tenEHIOResource IO_aenEHIOMasterList[RESM_nMastersMax];

static void IO_vSetIOMux(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, uint32);

void IO_vInitDIOResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enIOType, IOAPI_tenDriveStrength enDriveStrength)
{
	if (0u != IO_rastEHPadResource[enIOResource].u32PortBit)
	/* Init DIO only if port bit non zero signifies DIO supported */
	{	
		PIM_vInitPortBit(IO_rastEHPadResource[enIOResource].enPort, enIOType, IO_rastEHPadResource[enIOResource].u32PortBit);	
	}
}

void IO_vInitADCResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enEHIOType, ADCAPI_tstADCCB* pstADCCB)
{
	if (((0u != IO_rastEHPadResource[enIOResource].boIsADSE) && (IOAPI_enADSE == enEHIOType))
		|| ((0u != IO_rastEHPadResource[enIOResource].boIsADD) && (IOAPI_enADD == enEHIOType)))
	/* Init DIO only if port bit non zero signifies DIO supported */
	{	
#if defined(BUILD_PBL) || defined(BUILD_SBL) || defined(BUILD_KERNEL)	
		ADC_vInitADCResource(enIOResource, enEHIOType, pstADCCB);	
#elif defined(BUILD_KERNEL_APP)
		ADC_vInitADCResource(enIOResource, enEHIOType, pstADCCB);			
#endif		
	}
}

void IO_vInitDACResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enEHIOType, DACAPI_tstDACCB* pstDACCB)
{
	if ((0u != IO_rastEHPadResource[enIOResource].boIsDAC) && (IOAPI_enDAC == enEHIOType))

	/* Init DIO only if port bit non zero signifies DIO supported */
	{	
#if defined(BUILD_PBL) || defined(BUILD_SBL) || defined(BUILD_KERNEL)	
		DAC_vInitDACResource(enIOResource, pstDACCB);	
#elif defined(BUILD_KERNEL_APP	)
		DAC_vInitDACResource(enIOResource, pstDACCB);			
#endif		
	}
}


SYSAPI_tenSVCResult IO_enInitCommsResource(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enFail;
	uint32 u32MuxSel = 0;
	
	if ((EH_FIRST_IIC <= enEHIOResource) &&
	(EH_LAST_IIC >= enEHIOResource))
	{
#if defined(BUILD_PBL) || defined(BUILD_SBL) || defined(BUILD_KERNEL)	
		enSVCResult = IIC_enInitBus(enEHIOResource, pstPortConfigCB);
#elif defined(BUILD_KERNEL_APP)
		enSVCResult = IIC_enInitBus(enEHIOResource, pstPortConfigCB);		
#endif		
	}
	
	else if ((EH_FIRST_UART <= enEHIOResource) &&
	(EH_LAST_UART >= enEHIOResource))
	{
		enSVCResult = UART_enInitBus(enEHIOResource, pstPortConfigCB);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stUARTPinInfo.enRXPin, IOAPI_enUARTBus, 3u);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stUARTPinInfo.enTXPin, IOAPI_enUARTBus, 3u);		
	}	
	
	else if ((EH_FIRST_CAN <= enEHIOResource) &&
	(EH_LAST_CAN >= enEHIOResource))
	{
		u32MuxSel = CAN_u32InitBus(enEHIOResource, pstPortConfigCB);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stCANPinInfo.enRXPin, IOAPI_enCANBus, u32MuxSel);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stCANPinInfo.enTXPin, IOAPI_enCANBus, u32MuxSel);	
		enSVCResult = SYSAPI_enOK;	
	}	

	else if ((EH_FIRST_SPI <= enEHIOResource) &&
	(EH_LAST_SPI >= enEHIOResource))
	{
		u32MuxSel = SPI_u32InitBus(enEHIOResource, pstPortConfigCB);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enMOSIPin, IOAPI_enSPIBus, u32MuxSel);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enMISOPin, IOAPI_enSPIBus, u32MuxSel);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enSCKPin, IOAPI_enSPIBus, u32MuxSel);
		enSVCResult = SYSAPI_enOK;	
	}

	else if ((EH_VIO_USB <= enEHIOResource) &&
	(EH_VIO_USB >= enEHIOResource))
	{
		enSVCResult = USB_enInitBus(enEHIOResource, pstPortConfigCB);
	}
				
	if ((EH_ENET_FIRST_CH <= enEHIOResource) &&
	(EH_ENET_LAST_CH >= enEHIOResource))
	{
		//enSVCResult = ENE_enInitChannel(enEHIOResource, pstPortConfigCB);	
	}
	
	return enSVCResult;
}

SYSAPI_tenSVCResult IO_enInitTEPMChannel(IOAPI_tenEHIOResource enEHIOResource, TEPMAPI_tstTEPMChannelCB* pstTEPMChannelCB)
{
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enOK;
	uint32 u32MuxSel;
	u32MuxSel = TEPM_u32InitTEPMChannel(enEHIOResource, pstTEPMChannelCB);	
	IO_vSetIOMux(enEHIOResource, IOAPI_enTEPM, u32MuxSel);
	
	return enSVCResult;
}

void IO_vAssertDIOResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenTriState enTriState)
{
	if (0u != IO_rastEHPadResource[enIOResource].u32PortBit)
	/* Assert DIO only if port bit non zero signifies DIO supported */
	{	
		PIM_vAssertPortBit(IO_rastEHPadResource[enIOResource].enPort, IO_rastEHPadResource[enIOResource].u32PortBit, enTriState);	
	}		
}

Bool IO_boGetDIOResource(IOAPI_tenEHIOResource enIOResource)
{
	Bool boPinState = false;

	if (0u != IO_rastEHPadResource[enIOResource].u32PortBit)
	/* Assert DIO only if port bit non zero signifies DIO supported */
	{
		boPinState = PIM_boGetPortBitState(IO_rastEHPadResource[enIOResource].enPort, IO_rastEHPadResource[enIOResource].u32PortBit);
	}

	return boPinState;
}

IOAPI_tenTriState IO_enGetDIOResourceState(IOAPI_tenEHIOResource enIOResource)
{
	IOAPI_tenTriState enTriState = IOAPI_enError;
	Bool boBitStateHigh;
	
	if (0u != IO_rastEHPadResource[enIOResource].u32PortBit)
	/* Assert DIO only if port bit non zero signifies DIO supported */
	{	
		boBitStateHigh = PIM_boGetPortBitState(IO_rastEHPadResource[enIOResource].enPort, IO_rastEHPadResource[enIOResource].u32PortBit);	
		enTriState = TRUE == boBitStateHigh ? IOAPI_enHigh : IOAPI_enLow;
	}		
	
	return enTriState;
}

static void IO_vSetIOMux(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enIOType, uint32 u32MuxSel)
{
	uint32 u32PortBit = IO_rastEHPadResource[enIOResource].u32PortBit;
	PIMAPI_tenPort enPort = IO_rastEHPadResource[enIOResource].enPort;

	PIM_vSetPortMux(enPort, enIOType, u32PortBit, u32MuxSel);
}

