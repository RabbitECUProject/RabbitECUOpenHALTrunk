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
#include "CPUAbstract.h"
#include "kernelio.h"

const IO_tstEHPadResource IO_rastEHPadResource[IO_Total_Discrete_Count + KERNEL_DISCRETE_HIDDEN] = IO_nPadResourceMap;
IOAPI_tenEHIOResource IO_aenEHIOMasterList[RESM_nMastersMax];

static void IO_vSetIOMux(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, uint32);
static void IO_vClearMasterList(void);

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
#if BUILD_PBL
#elif BUILD_SBL
#elif BUILD_KERNEL	
		ADC_vInitADCResource(enIOResource, enEHIOType, pstADCCB);	
#elif BUILD_KERNEL_APP
		ADC_vInitADCResource(enIOResource, enEHIOType, pstADCCB);			
#endif		
	}
}

void IO_vInitDACResource(IOAPI_tenEHIOResource enIOResource, IOAPI_tenEHIOType enEHIOType, DACAPI_tstDACCB* pstDACCB)
{
	if ((0u != IO_rastEHPadResource[enIOResource].boIsDAC) && (IOAPI_enDAC == enEHIOType))

	/* Init DIO only if port bit non zero signifies DIO supported */
	{	
#if BUILD_PBL
#elif BUILD_SBL		
#elif BUILD_KERNEL	
		DAC_vInitDACResource(enIOResource, pstDACCB);	
#elif BUILD_KERNEL_APP	
		DAC_vInitDACResource(enIOResource, pstDACCB);			
#endif		
	}
}


SYSAPI_tenSVCResult IO_enInitCommsResource(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	SYSAPI_tenSVCResult enSVCResult;
	uint32 u32MuxSel = 0;
	
	if ((EH_FIRST_IIC <= enEHIOResource) &&
	(EH_LAST_IIC >= enEHIOResource))
	{
#if BUILD_PBL
#elif BUILD_SBL
#elif BUILD_KERNEL
		enSVCResult = IIC_enInitBus(enEHIOResource, pstPortConfigCB);
#elif BUILD_KERNEL_APP
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
		enSVCResult = CAN_enInitBus(enEHIOResource, pstPortConfigCB);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stCANPinInfo.enRXPin, IOAPI_enCANBus, 2u);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stCANPinInfo.enTXPin, IOAPI_enCANBus, 2u);		
	}	

	else if ((EH_FIRST_SPI <= enEHIOResource) &&
	(EH_LAST_SPI >= enEHIOResource))
	{
		u32MuxSel = SPI_u32InitBus(enEHIOResource, pstPortConfigCB);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enMOSIPin, IOAPI_enSPIBus, u32MuxSel);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enMISOPin, IOAPI_enSPIBus, u32MuxSel);
		IO_vSetIOMux(pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.enMISOPin, IOAPI_enSPIBus, u32MuxSel);
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
	uint32 u32Masters;
	uint32 u32MasterIDX = 0;
	Bool boMasterOK = FALSE;
	SYSAPI_ttClientHandle tMasterClientHandle;
	
	IO_vClearMasterList();
	u32Masters = RESM_u32GetMasterEHIOResourceList(enEHIOResource, (IOAPI_tenEHIOResource*)&IO_aenEHIOMasterList);
	
#if BUILD_PBL
#elif BUILD_SBL
#elif BUILD_KERNEL
	while (u32Masters > u32MasterIDX)
	{
		tMasterClientHandle = RESM_u32GetMasterEHIOResource(IO_aenEHIOMasterList[u32MasterIDX]);
		if (OS_stSVCDataStruct.tClientHandle != tMasterClientHandle)
		{
			if (0 == tMasterClientHandle)
			{
				enSVCResult = SYSAPI_enMasterResourceUninitialised;
			}
			else
			{
				enSVCResult = SYSAPI_enMasterResourceMismatch;				
			}
			boMasterOK = FALSE;
			OS_stSVCDataStruct.pvData = (void*)&IO_aenEHIOMasterList;
		}
		u32MasterIDX++;
	}
		
	u32MuxSel = TEPM_u32InitTEPMChannel(enEHIOResource, pstTEPMChannelCB);
#elif BUILD_KERNEL_APP
	while (u32Masters > u32MasterIDX)
	{
		tMasterClientHandle = RESM_u32GetMasterEHIOResource(IO_aenEHIOMasterList[u32MasterIDX]);
		if (OS_stSVCDataStruct.tClientHandle != tMasterClientHandle)
		{
			if (0 == tMasterClientHandle)
			{
				enSVCResult = SYSAPI_enMasterResourceUninitialised;
			}
			else
			{
				enSVCResult = SYSAPI_enMasterResourceMismatch;				
			}
			boMasterOK = FALSE;
			OS_stSVCDataStruct.pvData = (void*)&IO_aenEHIOMasterList;
		}
		u32MasterIDX++;
	}
		
	u32MuxSel = TEPM_u32InitTEPMChannel(enEHIOResource, pstTEPMChannelCB);	
#endif	
	IO_vSetIOMux(enEHIOResource, IOAPI_enTEPM, u32MuxSel);
	
	/* TODO suppress warning */
	(void)boMasterOK;
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

static void IO_vClearMasterList(void)
{
	uint32 u32MasterIDX;
	
	for (u32MasterIDX = 0; u32MasterIDX < RESM_nMastersMax; u32MasterIDX++)
	{	
		IO_aenEHIOMasterList[u32MasterIDX] = EH_IO_Invalid;
	}
}
