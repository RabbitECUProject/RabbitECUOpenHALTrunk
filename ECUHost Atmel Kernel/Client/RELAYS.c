/******************************************************************************/
/*                Copyright (c) 2017 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      RELAYS                                                 */
/* DESCRIPTION:        This code module initialises the required resources    */
/*                     for driving the user application relays and functions  */
/*                     for controlling the on/off state                       */
/*                                                                            */
/* FILE NAME:          RELAYS.c                                               */
/* REVISION HISTORY:   11-01-2017 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _RELAYS_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "RELAYS.h"

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint8 RELAYS_u8RelayBitState;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void RELAYS_vStart(uint32 * const pu32Arg)
{
	uint32 u32ResIDX;
	IOAPI_tenEHIOType enEHIOType;
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tstPortConfigCB stPortConfigCB;	
	IOAPI_tenDriveStrength enDriveStrength;
	uint32 u32Flags;
	
    /* Setup the RELAYS SPI resources */
	enEHIOResource = EH_VIO_SPI1;	
		
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	NULL, NULL);
	
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		stPortConfigCB.stNetConfig.uNetInfo.stIICNetInfo.u32SlaveAddress = 0;
		stPortConfigCB.enPortMode = IOAPI_enPortSerialTransfer;
		stPortConfigCB.u32BaudRateHz = RELAYS_nSPIFrequency;
		stPortConfigCB.enVIOResource = EH_VIO_SPI1;
		stPortConfigCB.stPinConfig.uPinInfo.stSPIPinInfo.enMOSIPin = EH_IO_SPI1_MOSI;
		stPortConfigCB.stPinConfig.uPinInfo.stSPIPinInfo.enMISOPin = EH_IO_SPI1_MISO;
		stPortConfigCB.stPinConfig.uPinInfo.stSPIPinInfo.enSCKPin = EH_IO_SPI1_SCK;
		stPortConfigCB.enLLProtocol = PROTAPI_enLLNone;
		stPortConfigCB.enTLProtocol = PROTAPI_enTLNone;
		stPortConfigCB.pfRXTransferCB = NULL;
		stPortConfigCB.pfTXTransferCB = NULL;
		stPortConfigCB.stPinConfig.uPinInfo.stSPIPinInfo.boCaptureRising = TRUE;
		stPortConfigCB.stPinConfig.uPinInfo.stSPIPinInfo.boShiftRising = FALSE;
		stPortConfigCB.stPinConfig.uPinInfo.stSPIPinInfo.boIdleHigh = TRUE;

		enEHIOType = IOAPI_enSPIBus;
		
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stPortConfigCB);
	}

	enEHIOResource = RELAYS_nSPI_CS_RESOURCE;
	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;

	/* Set up the CS line */
    SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, &u32Flags);
	SETUP_vSetDigitalIOHigh(RELAYS_nSPI_CS_RESOURCE);

	/* Start the SPI I/O expander IC */
	MCP23S08_vStart(NULL);
			
	/* Set MCP23S08 port to output direction */
    (void)MCP23S08_boSetCallback(&RELAYS_vCallBack);
	SETUP_vSetDigitalIOLow(RELAYS_nSPI_CS_RESOURCE);
	MCP23S08_vSetDataDirection(0);
	SETUP_vSetDigitalIOHigh(RELAYS_nSPI_CS_RESOURCE);
	
	if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult)
	{	
		*pu32Arg |= (uint32)SYSAPI_enResourceInitFailed;
	}	

	RELAYS_u8RelayBitState = 0x53;//matthew debug
}

void RELAYS_vRun(uint32* const pu32Arg)
{		
	SETUP_vSetDigitalIOLow(RELAYS_nSPI_CS_RESOURCE);
	(void)MCP23S08_boTransferData(IOAPI_IO_TX, (void*)&RELAYS_u8RelayBitState, 1);
	RELAYS_u8RelayBitState++;
}

void RELAYS_vTerminate(uint32* const pu32Arg)
{

}

void RELAYS_vCallBack(uint32* const pu32Arg)
{
	SETUP_vSetDigitalIOHigh(RELAYS_nSPI_CS_RESOURCE);
}

void RELAYS_vThread(void)
{
	while(1);	
}

			
#endif //BUILD_USER



