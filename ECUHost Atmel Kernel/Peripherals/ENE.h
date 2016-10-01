/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef ENE_H
#define ENE_H

#include "declarations.h"
#include "DSCRIO.h"
#include "IOAPI.h"
#include "macros.h"
#include "regset.h"
#include "SIM.h"
#include "SYSAPI.h"
#include "types.h"
#include "TIMER.h"


#define ENE_nResetDeviceMsWIFI 100u
#define ENE_nResetDeviceMsETHERNET 10u
#define ETH_nSourceMACAddress			{ 0x00, 0x0a, 0xe6, 0xbc, 0xee, 0xd8 }
#define ETH_nDestinationMACAddress		{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }

#if (COMMS_ETH_WIRED)
	#define ETH_nSourceIPAddress				0xC0A80A00
	#define ETH_nDestinationIPAddress		0xC0A80A64
#endif

#if (COMMS_ETH_WIFI)
	#define ETH_nSourceIPAddress				0x0a000000
	#define ETH_nDestinationIPAddress		0x0a000064
#endif

#define ETH_nMaxLength                      1518u
#define ETH_nETHHeaderLength                14u
#define ETH_nIPHeaderLength                 20u
#define	ETH_nUDPHeaderLength                8u
#define ETH_xIPPacketLength(x)              ((x)	- ETH_nETHHeaderLength)
#define ETH_xUDPPacketLength(x)             ((x)	-  (ETH_nETHHeaderLength + ETH_nIPHeaderLength))
#define ENE_nChannelTXCount                 5u
#define ENE_xIntActivated(x)	((ENET -> EIR & (x)) == (x))
#define ENE_xIntClearFlag(x)	(ENET -> EIR |= (x))

//TODO this typedef does not belong here!

typedef enum
{
	enChannelNoProtocol,
	enChannelUDP,
	enChannelTCP,
	enChannelARP,
	enChannelProtocolCount
} ENE_tenChannelIPProtocol;

typedef enum
{
	enChannelNotPolled,
	enChannelPolledOK,
	enChannelPolledResend
} ENE_tenChannelPoll;

typedef enum
{
	enChannelMDACBusMultiplex,
	enChannelMDACUDSResponse,
	enChannelMDACApplicationData,
	enChannelUSERStream,
	enChannelOff
} ENE_tenChannelDesignator;

typedef enum
{
	enChannelInactive,
	enChannelActive,
	enChannelErr
} ENE_tenChannelStatus;

typedef __packed struct
{
	uint8 	au8ETHMACDestination[6];
	uint8 	au8ETHMACSource[6];
	uint16	u16ETHLengthOrType;
} ENE_tstETHHeader;

typedef __packed struct
{
	uint8 	u8IPVersionIHL;
	uint8 	u8DSCPECN;
	uint16	u16IPLength;
	uint16  u16ID;
	uint16	u16FlagsFragment;
	uint8		u8TimeToLive;
	uint8 	u8Protocol;
	uint16	u16IPHeaderChecksum;
	uint32	u32SourceIPAddress;
	uint32	u32DestinationIPAddress;
} ENE_tstIPHeader;

typedef __packed struct
{
	uint32 	u32Dummy;
} ENE_tstTCPHeader;

typedef __packed struct
{
	uint32 	u32Dummy;
} ENE_tstICMPHeader;

typedef __packed struct
{
	uint16	u16SourcePort;
	uint16  u16DestinationPort;
	uint16	u16UDPLength;
	uint16	u16UDPHeaderChecksum;
} ENE_tstUDPHeader;

typedef __packed struct
{
	ENE_tstETHHeader	stETHHeader;
	// IP header union
	__packed union
	{
		ENE_tstIPHeader stIPHeader;
		uint8 u8ETHData[sizeof(ENE_tstIPHeader)];
	} uETHData;
	// UDP header union
	__packed union
	{
		ENE_tstUDPHeader stUDPHeader;
		uint8 u8ETHData[sizeof(ENE_tstUDPHeader)];
	}	uIPData;
	__packed uint8 au8UDPData[1];
} ENE_tstETHUnionFrame;

typedef struct
{
	ENE_tstETHHeader stETHHeader;
	uint16 u16ChannelETHLength;	
	ENE_tenChannelIPProtocol enChannelIPProtocol;	
	uint8	u8ChannelStatus;
	ENE_tenChannelPoll enChannelPoll;
	uint16 u16ChannelUserRateMs;
	uint16 u16ChannelPollTimeout;
	union
	{
		ENE_tstIPHeader stIPHeader;
		ENE_tstICMPHeader stICMPHeader;	
	} uILPHeader;	
	union
	{
		ENE_tstUDPHeader stUDPHeader;
		ENE_tstTCPHeader stTCPHeader;
	} uTPLHeader;
	
} ENE_tstChannelControlBlock;

#undef EXTERN
#ifdef _ENE_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN bool ENE_boResetComplete;

#define ENE_nPHYAddress 0u
#define ENE_nNumRXBDs NUM_RXBDS

void ENE_vStart(uint32* const);
void ENE_vRun(uint32* const);
void ENE_vTerminate(uint32* const);
void ENE_vReceiveFrame(void);
void ENE_vTransmitFrame(ENE_tstETHUnionFrame*);
SYSAPI_tenSVCResult ENE_enInitChannel(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);

#endif //ENE_H

