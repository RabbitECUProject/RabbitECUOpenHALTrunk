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
Bool RELAYS_boInit;
const RELAY_tenBit RELAY_raenMAP[] = RELAY_MAP;

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
	uint32 u32VarAddress;
	uint32 u32Temp;
	
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
		stPortConfigCB.stPinConfig.uPinInfo.stSPIPinInfo.boClockIdleHigh = TRUE;

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

	enEHIOResource = RELAYS_TELLTALE_1;
	enEHIOType = IOAPI_enDIOOutput;
	enDriveStrength = IOAPI_enStrong;
	
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
	enEHIOResource = RELAYS_TELLTALE_2;
	SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);


	for (u32ResIDX = 0; u32ResIDX < 8; u32ResIDX++)
	{
		u32VarAddress = USERCAL_stRAMCAL.aLogicBlockVar[u32ResIDX] & 0xfffff;

		if (0 != u32VarAddress)
		{
			switch (USERCAL_stRAMCAL.aLogicBlockChainOutput[u32ResIDX])
			{
				case NO_OUT: break;
				case AND_NEXT:break;
				case OR_NEXT:break;
				case NOT_NEXT: break;
				case XOR_NEXT: break;
				default:
				{
					u32Temp = USERCAL_stRAMCAL.aLogicBlockChainOutput[u32ResIDX] - (uint32)OUT1_ON;
					enEHIOResource = (IOAPI_tenEHIOResource)(u32Temp / 2);
					enEHIOType = IOAPI_enDIOOutput;

					SETUP_vSetupDigitalIO(enEHIOResource, enEHIOType, enDriveStrength, pu32Arg);
				}			
			}
		}
	}

	
	if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult)
	{	
		*pu32Arg |= (uint32)SYSAPI_enResourceInitFailed;
	}	

	RELAYS_u8RelayBitState = 0;
	RELAYS_boInit = false;
}

void RELAYS_vRun(uint32* const pu32Arg)
{	
	static uint32 u32Count;
	static uint32 u32LogicIDX;
	static Bool boResultPrev;
	uint32 u32VarAddress;
	bool boIsSigned;
	uint32 u32ByteCount;
	RELAYS_tenCompare enCompare;
	bool boResult = false;
	static RELAYS_tenChainOutput enChainOutputPrev;
	uint32 u32Temp;
	IOAPI_tenTriState enTriState;
	IOAPI_tenEHIOResource enEHIOResource;
	RELAY_tenBit enBit;

	if (false == RELAYS_boInit) return;

	if (511 < u32Count++)
	{
		//SETUP_vSetDigitalIOLow(RELAYS_nSPI_CS_RESOURCE);
		MCP23S08_vSetDataDirection(0);
		u32Count = 0;
	}
	else
	{
		//SETUP_vSetDigitalIOLow(RELAYS_nSPI_CS_RESOURCE);
		(void)MCP23S08_boTransferData(IOAPI_IO_TX, (void*)&RELAYS_u8RelayBitState, 1);
	}

	u32VarAddress = USERCAL_stRAMCAL.aLogicBlockVar[u32LogicIDX] & 0xfffff;

	if (0 != u32VarAddress)
	{
		u32VarAddress += 0x20000000;

		boIsSigned = (USERCAL_stRAMCAL.aLogicBlockVar[u32LogicIDX] & 0x400000) != 0 ? true : false;
		u32ByteCount = 1 + ((USERCAL_stRAMCAL.aLogicBlockVar[u32LogicIDX] & 0x300000) >> 20);
		enCompare =  (USERCAL_stRAMCAL.aLogicBlockVar[u32LogicIDX] & 0x3800000) >> 23;

		if (false == boIsSigned)
		{
			switch (enCompare)
			{
				case GT:
				{
					switch (u32ByteCount)
					{
						case 4:	boResult = *(uint32*)u32VarAddress > USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 2:	boResult = *(uint16*)u32VarAddress > USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 1:	boResult = *(uint8*)u32VarAddress > USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						default: boResult = false; break;
					}
					break;
				}
				case LT:
				{
					switch (u32ByteCount)
					{
						case 4:	boResult = *(uint32*)u32VarAddress < USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 2:	boResult = *(uint16*)u32VarAddress < USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 1:	boResult = *(uint8*)u32VarAddress < USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						default: boResult = false; break;												
					}
					break;
				}
				case EE:
				{
					switch (u32ByteCount)
					{
						case 4:	boResult = *(uint32*)u32VarAddress == USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 2:	boResult = *(uint16*)u32VarAddress == USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 1:	boResult = *(uint8*)u32VarAddress == USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						default: boResult = false; break;
					}
					break;
				}
				case NE:
				{
					switch (u32ByteCount)
					{
						case 4:	boResult = *(uint32*)u32VarAddress != USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 2:	boResult = *(uint16*)u32VarAddress != USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 1:	boResult = *(uint8*)u32VarAddress != USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						default: boResult = false; break;	
					}
					break;
				}
			}
		}
		else
		{
			switch (enCompare)
			{
				case GT:
				{
					switch (u32ByteCount)
					{
						case 4:	boResult = *(sint32*)u32VarAddress > (sint32)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 2:	boResult = *(sint16*)u32VarAddress > (sint16)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 1:	boResult = *(sint8*)u32VarAddress > (sint8)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						default: boResult = false; break;
					}
					break;
				}
				case LT:
				{
					switch (u32ByteCount)
					{
						case 4:	boResult = *(sint32*)u32VarAddress < (sint32)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 2:	boResult = *(sint16*)u32VarAddress < (sint16)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 1:	boResult = *(sint8*)u32VarAddress < (sint8)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						default: boResult = false; break;
					}
					break;
				}
				case EE:
				{
					switch (u32ByteCount)
					{
						case 4:	boResult = *(sint32*)u32VarAddress == (sint32)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 2:	boResult = *(sint16*)u32VarAddress == (sint16)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 1:	boResult = *(sint8*)u32VarAddress == (sint8)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						default: boResult = false; break;
					}
					break;
				}
				case NE:
				{
					switch (u32ByteCount)
					{
						case 4:	boResult = *(sint32*)u32VarAddress != (sint32)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 2:	boResult = *(sint16*)u32VarAddress != (sint16)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						case 1:	boResult = *(sint8*)u32VarAddress != (sint8)USERCAL_stRAMCAL.aLogicBlockOperand[u32LogicIDX] ? TRUE : FALSE; break;
						default: boResult = false; break;
					}
					break;
				}
			}
		}


		switch (enChainOutputPrev)
		{
			case NO_OUT: break;
			case AND_NEXT: boResult &= boResultPrev; break;
			case OR_NEXT: boResult |= boResultPrev; break;
			case NOT_NEXT: 
			{
				boResult = boResultPrev & !boResult; 
				break;
			}
			case XOR_NEXT: boResult ^= boResultPrev; break;
			default: break;
		}


		switch (USERCAL_stRAMCAL.aLogicBlockChainOutput[u32LogicIDX])
		{
			case NO_OUT: break;
			case AND_NEXT: boResultPrev = boResult; break;
			case OR_NEXT: boResultPrev = boResult; break;
			case NOT_NEXT: boResultPrev = boResult; break;
			case XOR_NEXT: boResultPrev = boResult; break;

			default:
			{
				if (true == boResult)
				{
					u32Temp = USERCAL_stRAMCAL.aLogicBlockChainOutput[u32LogicIDX] - (uint32)OUT1_ON;
					enTriState = u32Temp & 0x01 ? IOAPI_enLow : IOAPI_enHigh;
					enEHIOResource = (IOAPI_tenEHIOResource)(u32Temp / 2);

					if (EH_IO_IIC2_SDA > enEHIOResource)
					{
						USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
						(void*)&enTriState,	(void*)NULL);
					}
					else
					{
						enBit = RELAY_raenMAP[enEHIOResource - EH_IO_IIC2_SDA];
						boResult = IOAPI_enHigh == enTriState;
						RELAYS_vOutputBit(enBit, boResult);
					}

					break;
				}

				boResultPrev = false;
			}
		}

		enChainOutputPrev = USERCAL_stRAMCAL.aLogicBlockChainOutput[u32LogicIDX];
	}

	u32LogicIDX = (u32LogicIDX + 1) & 0x7;
}

void RELAYS_vTerminate(uint32* const pu32Arg)
{

}


void RELAYS_vOutputBit(RELAY_tenBit enBit, Bool boBitOn)
{
	if (FALSE == boBitOn)
	{
		RELAYS_u8RelayBitState &= ~enBit;
	}
	else
	{
		RELAYS_u8RelayBitState |= enBit;
	}
}


void RELAYS_vCallBack(uint32* const pu32Arg)
{
	SETUP_vSetDigitalIOHigh(RELAYS_nSPI_CS_RESOURCE);
	RELAYS_boInit = true;
}

void RELAYS_vThread(void)
{
	while(1);	
}

			
#endif //BUILD_USER



