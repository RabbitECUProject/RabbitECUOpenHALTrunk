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
#define _ENE_C
#include <string.h>
#include "DLL.h"
#include "ENE.h"
#include "ENEHA.h"

const uint8	ENE_rau8SourceMACAddress[] = ETH_nSourceMACAddress;
const uint8	ENE_rau8DestinationMACAddress[] = ETH_nDestinationMACAddress;
ENE_tstChannelControlBlock ENE_stTXCCB[ENE_nChannelTXCount];
extern uint8_t *TxBuffer;

static sint32 ENE_u32GetENEIndex(IOAPI_tenEHIOResource);
static void ENE_vResetComplete(void);

void ENE_vStart(uint32* const pu32Stat)
{	
	/* reset the ethernet and WIFI phy devices */
#if COMMS_ETH_WIFI	
	DSCRIO_vSetDiscrete(enCOMSRESET, enLow);	
	ENE_boResetComplete = false;
	
	/* set an alarm - hold reset low per PHY device datasheet */
	(void)TIMER_vConfigureAlarm(ENE_nResetDeviceMsWIFI, ENE_vResetComplete);	
#endif	
	
#if COMMS_ETH_WIRED	
	DSCRIO_vSetDiscrete(enCOMSRESET, enLow);	
	ENE_boResetComplete = false;
	
	/* set an alarm - hold reset low per PHY device datasheet */
	(void)TIMER_vConfigureAlarm(ENE_nResetDeviceMsETHERNET, ENE_vResetComplete);	
#endif		
	
	ENEHA_vStart(pu32Stat);
	
	OS_xModuleStartOK(*pu32Stat);
}

void ENE_vRun(uint32* const u32Stat)
{

}

void ENE_vTerminate(uint32* const u32Stat)
{
	
}


SYSAPI_tenSVCResult ENE_enInitChannel(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	SYSAPI_tenSVCResult enSVCResult;
	sint32 i32IDX = ENE_u32GetENEIndex(enEHIOResource);	
	
	if ((-1 != i32IDX) && (TRUE == DLL_boInitDLLChannel(enEHIOResource, pstPortConfigCB)))
	{
		enSVCResult = SYSAPI_enOK;
	}
	else
	{
		enSVCResult = SYSAPI_enBadResource;		
	}
	
	return enSVCResult;
}

void ENE_vReceiveFrame(void)
{
	ENE_tstETHUnionFrame* pstETHUnionFrame;

	pstETHUnionFrame = ENEHA_pstReceiveFrame();	
	
	switch(pstETHUnionFrame -> stETHHeader.u16ETHLengthOrType)
	{
		case 0x0608:
		/* Is ARP request */
			//matthew todo integrate LWIP
			break;
		
		case 0x0008:
		/* Is IP packet */
			//DLL_vIPBufferRXCB(pstETHUnionFrame);
			break;
			
		default:
			break;
	}
}

void ENE_vTransmitFrame(ENE_tstETHUnionFrame* pstETHUnionFrame)
{
	uint16 u16FrameLength = NTOHS(pstETHUnionFrame->uETHData.stIPHeader.u16IPLength) + 14;
	uint16 u16IDX;
	puint8 pu8Buff;// = (puint8)TxBuffer;
	
	pstETHUnionFrame->stETHHeader.u16ETHLengthOrType = HTONS(0x0800);
	
	//memcpy(TxBuffer, pstETHUnionFrame, u16FrameLength);matthew extern txbuffer required
	
	/* Write the destination MAC address to the buffer */
	for(u16IDX = 0; u16IDX < 6; u16IDX++)
	{
		*pu8Buff++ = ENE_rau8DestinationMACAddress[u16IDX];
	}					
	
	/* Write the source MAC address to the buffer */
	for(u16IDX = 0; u16IDX < 6; u16IDX++)
	{
		*pu8Buff++ = ENE_rau8SourceMACAddress[u16IDX];
	}	

	ENEHA_vTransmitFrame(pu8Buff, u16FrameLength);
}

static sint32 ENE_u32GetENEIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	sint32 i32IDX = -1;

	i32IDX= ENEHA_s32GetENEIndex(enEHIOResource);
		
	return i32IDX;
}


static void ENE_vResetComplete(void)
{
	DSCRIO_vSetDiscrete(enCOMSRESET, enHigh);
	ENE_boResetComplete = true;
}
			
