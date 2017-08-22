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
#include "build.h"
#include "CANHA.h"
#include "CBYTEQUEUE.h"
#include "declarations.h"
#include "DLL.h"
#include "ENE.h"
#include "IOAPI.h"
#include "macros.h"
#include "OS.h"
#include "PACKEDDLL.h"
#include "PROTAPI.h"
#include "RESM.h"
#include "peruart.h"
#include "UDSNL.h"


const uint32 ETH_ru32SourceIPAddress = ETH_nSourceIPAddress;
const uint32 ETH_ru32DestinationIPAddress = ETH_nDestinationIPAddress;		
const uint16 DLL_rau16TXFrameMaxBytes[] = DLL_nMaxTXFrameBytes;		

IOAPI_tstPortConfigCB DLL_astPortConfigCB[DLL_nVirtualChannelCount]; 
DLL_tstRXDLLData DLL_stRXDLLData[DLL_nVirtualChannelCount];
uint16 DLL_au16PacketSeq[DLL_nVirtualChannelCount];

/* Circular TX queues */
uint8	DLL_au8ByteIICByteQueue[DLL_nIICVirtualChannelCount][DLL_nIICTXQueueByteCount];
uint8	DLL_au8ByteSPIByteQueue[DLL_nSPIVirtualChannelCount][DLL_nSPITXQueueByteCount];
uint8	DLL_au8ByteUARTByteQueue[DLL_nUARTVirtualChannelCount][DLL_nUARTTXQueueByteCount];
uint8	DLL_au8ByteCANByteQueue[DLL_nCANVirtualChannelCount][DLL_nCANTXQueueByteCount];
uint8	DLL_au8ByteUSBByteQueue[DLL_nUSBVirtualChannelCount][DLL_nUSBTXQueueByteCount];
uint8	DLL_au8ByteENETByteQueue[DLL_nENETVirtualChannelCount][DLL_nENETTXQueueByteCount];

/* Temp TX buff to build response and to dequeue byte-queue to for TX */
uint8 DLL_au8TXIICBuffBuild[DLL_nIICTXWorkBuffCount][DLL_nIICTXWorkBuffMaxBytes];
uint8 DLL_au8TXSPIBuffBuild[DLL_nSPITXWorkBuffCount][DLL_nSPITXWorkBuffMaxBytes];
uint8 DLL_au8TXUARTBuffBuild[DLL_nUARTTXWorkBuffCount][DLL_nUARTTXWorkBuffMaxBytes];
uint8 DLL_au8TXCANBuffBuild[DLL_nCANTXWorkBuffCount][DLL_nCANTXWorkBuffMaxBytes];
uint8 DLL_au8TXUSBBuffBuild[DLL_nUSBTXWorkBuffCount][DLL_nUSBTXWorkBuffMaxBytes];
uint8 DLL_au8TXENETBuffBuild[DLL_nENETTXWorkBuffCount][DLL_nENETTXWorkBuffMaxBytes];

/* Bytequeue struct array */
CBYTEQUEUE_tstQueue DLL_astTXDLLByteQueue[DLL_nIICVirtualChannelCount +
																					DLL_nSPIVirtualChannelCount +
																					DLL_nUARTVirtualChannelCount +
																					DLL_nCANVirtualChannelCount +
																					DLL_nUSBVirtualChannelCount +
																					DLL_nENETVirtualChannelCount];

/* Build buffer client array */
IOAPI_tenEHIOResource DLL_aenDLLTXBufferClient[	DLL_nIICTXWorkBuffCount +
																								DLL_nSPITXWorkBuffCount +
																								DLL_nUARTTXWorkBuffCount +
																								DLL_nCANTXWorkBuffCount +
																								DLL_nUSBTXWorkBuffCount +
																								DLL_nENETTXWorkBuffCount];
																					
/* Transmit control structure array */
DLL_tstTXCB DLL_astTXCB[									DLL_nIICTXWorkBuffCount +
																					DLL_nSPITXWorkBuffCount +
																					DLL_nUARTTXWorkBuffCount +
																					DLL_nCANTXWorkBuffCount +
																					DLL_nUSBTXWorkBuffCount +
																					DLL_nENETTXWorkBuffCount];
																					

static void DLL_vInsertCANIDAndDLC(puint8, puint32, uint32);
static IOAPI_tenEHIOResource DLL_tGetEHIOResource(DLL_tDLLVirtualChannel);
static Bool DLL_boSendFrame(IOAPI_tenEHIOResource, puint8, uint32);

static void DLLBYTEQUEUE_vInit(CBYTEQUEUE_tstQueue*, uint32, void*);
static uint32 DLLBYTEQUEUE_u32GetVacantCount(CBYTEQUEUE_tstQueue*);
static uint32 DLLBYTEQUEUE_u32GetQueuedCount(CBYTEQUEUE_tstQueue*);
static Bool DLLBYTEQUEUE_vQueueBytes(CBYTEQUEUE_tstQueue*, puint8, uint32);
static void DLLBYTEQUEUE_vDequeueBytes(CBYTEQUEUE_tstQueue*, puint8, puint32);
static puint8 DLL_pu8GetTXClientBuffer(IOAPI_tenEHIOResource, puint32 pu32TXBufferCap);	
static void DLL_vReleaseTXClientBuffer(puint8);

DLL_tstRXDLLData* DLL_pstGetRXBuffer(IOAPI_tenEHIOResource enEHIOResource)
{
    return &DLL_stRXDLLData[enEHIOResource - EH_VIO_IIC1];
}

//void DLL_vIPBufferRXCB(ENE_tstETHUnionFrame* pstETHUnionFrame)
//{	
	//uint8 u8ENEChannelIDX;
	//uint16 u16Temp;
	//puint8 pu8FrameData;
	//
	//IOAPI_tenEHIOResource	enEHIOResource = EH_IO_Invalid;
	//DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(EH_VIO_ENET_CH1);		
	//
	//for (u8ENEChannelIDX = 0; u8ENEChannelIDX < (EH_ENET_LAST_CH - EH_ENET_FIRST_CH + 1); u8ENEChannelIDX++)
	//{
		//if ((EH_ENET_FIRST_CH <= 
				//DLL_astPortConfigCB[DLLVirtualChannelIDX + u8ENEChannelIDX].enVIOResource)
			//&&(EH_ENET_LAST_CH >= 
				//DLL_astPortConfigCB[DLLVirtualChannelIDX + u8ENEChannelIDX].enVIOResource))
		//{
//#if (COMMS_ETH_WIFI)
			//if(NTOHS(DLL_astPortConfigCB[DLLVirtualChannelIDX + u8ENEChannelIDX].stNetConfig.uNetInfo.stLANNetInfo.u16RPCREQSourcePort) == 
						//(pstETHUnionFrame -> uIPData.stUDPHeader.u16SourcePort))				
//#endif			
			//{
				//if(NTOHS(DLL_astPortConfigCB[DLLVirtualChannelIDX + u8ENEChannelIDX].stNetConfig.uNetInfo.stLANNetInfo.u16RPCREQDestPort) == 
							//(pstETHUnionFrame -> uIPData.stUDPHeader.u16DestinationPort))	
				//{
					//enEHIOResource = (IOAPI_tenEHIOResource)(EH_VIO_ENET_CH1 + u8ENEChannelIDX);		
					//break;
				//}	
			//}
		//}
	//}
	//
	//if (EH_IO_Invalid != enEHIOResource)
	//{
		//DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);
		//DLL_au16PacketSeq[DLLVirtualChannelIDX] = NTOHS(pstETHUnionFrame->uETHData.stIPHeader.u16ID);
		//u16Temp = NTOHS(pstETHUnionFrame->uIPData.stUDPHeader.u16UDPLength);
		//u16Temp = MIN(u16Temp, (uint16)RX_BUFF_SIZE);	//matthew	
		//pu8FrameData = &pstETHUnionFrame->au8UDPData[0];
		//
		//DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount = u16Temp;			
		//memcpy((void*)&DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data,
									 //(void*)pu8FrameData, u16Temp);	
//
		//DLL_vFrameRXCB(enEHIOResource, (puint8)&DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[0]);				
	//}
//}

void DLL_vIPBufferTX(IOAPI_tenEHIOResource enEHIOResource, puint8 pu8TXData, uint32 u32TXByteCount)
{
	ENE_tstETHUnionFrame* pstETHUnionFrame = (ENE_tstETHUnionFrame*)pu8TXData;
	DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);
	
	/* Move the transmit data to the UDP payload offset */
	if (DLL_nENETTXWorkBuffMaxBytes >= (u32TXByteCount + offsetof(ENE_tstETHUnionFrame, au8UDPData)))
	{		
		memmove((void*)pstETHUnionFrame->au8UDPData, (void*)pstETHUnionFrame, u32TXByteCount);	
	}	

	/* Check if we have already descended through the UDS_UDP layer
     because in that case we need to add the TL header not
	   added by a TCP/IP stack */	
	if (PROTAPI_enTLUDS_UDP == DLL_astPortConfigCB[DLLVirtualChannelIDX].enTLProtocol)
	{
		pstETHUnionFrame->uIPData.stUDPHeader.u16UDPHeaderChecksum = 0;
		pstETHUnionFrame->uIPData.stUDPHeader.u16UDPLength = HTONS((uint16)u32TXByteCount + ETH_nUDPHeaderLength);
		/* RPC response ports are +1 of the RPC request ports */
		pstETHUnionFrame->uIPData.stUDPHeader.u16SourcePort	= HTONS(DLL_astPortConfigCB[DLLVirtualChannelIDX].stNetConfig.uNetInfo.stLANNetInfo.u16RPCREQSourcePort + 1);
		pstETHUnionFrame->uIPData.stUDPHeader.u16DestinationPort	= HTONS(DLL_astPortConfigCB[DLLVirtualChannelIDX].stNetConfig.uNetInfo.stLANNetInfo.u16RPCREQDestPort + 1);		
	}
	
	/* Check if we have already descended through the 15765_IP layer
     because in that case we need to add the NL header not
	   added by a TCP/IP stack */		
	if (PROTAPI_enISO15765_IP == DLL_astPortConfigCB[DLLVirtualChannelIDX].enNLProtocol)	
	{
			/* Host To Network IP ***********************************/	
		
			/* Write the VersionIHL to the buffer */
			pstETHUnionFrame -> uETHData.stIPHeader.u8IPVersionIHL = 0x45;

			/* Write the DSCPECN to the buffer */
			pstETHUnionFrame -> uETHData.stIPHeader.u8DSCPECN = 0;
		
			/* Write the ID to buffer */					
			pstETHUnionFrame -> uETHData.stIPHeader.u16ID = HTONS(DLL_au16PacketSeq[DLLVirtualChannelIDX]);	
		
			/* Write the flags to the buffer */
			pstETHUnionFrame -> uETHData.stIPHeader.u16FlagsFragment = 0;
	
			/* Write the time to live to the buffer */
			pstETHUnionFrame -> uETHData.stIPHeader.u8TimeToLive = 0x80;		

			/* Write the protocol to the buffer */
			pstETHUnionFrame -> uETHData.stIPHeader.u8Protocol = 0x11;	
			
			/* Write the IP length to the buffer */
			pstETHUnionFrame -> uETHData.stIPHeader.u16IPLength = 
				HTONS(ETH_nIPHeaderLength + ETH_nUDPHeaderLength + (uint16)u32TXByteCount);
		
			/* Write the IP source address to the buffer */
			pstETHUnionFrame -> uETHData.stIPHeader.u32SourceIPAddress = 
					HTONL(ETH_ru32SourceIPAddress);

			/* Write the IP destination address to the buffer */
			pstETHUnionFrame -> uETHData.stIPHeader.u32DestinationIPAddress = 
					HTONL(ETH_ru32DestinationIPAddress);		
				
			/* Clear the checksum field ready for the hardware accelerator */
			pstETHUnionFrame -> uETHData.stIPHeader.u16IPHeaderChecksum = 0;
	}
	
	ENE_vTransmitFrame(pstETHUnionFrame);
}

/* pu8RXData receive frame could be various frame format, hardware mailbox etc */
void DLL_vFrameRXCB(IOAPI_tenEHIOResource enEHIOResource, puint8 pu8RXData)
{
	puint8 pu8TXData;
	puint8 pu8TXBuffer = NULL;
	uint32 u32TXByteCount = 0;
	uint32 u32TXByteCountOld;
	uint32 u32TXBufferCap;
	uint32 u32Temp;
	CANHA_tstCANMB* pstCANMB;
	Bool boQueuedOK;
	
	DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);	

	/* Get a response buffer */		
	pu8TXBuffer = DLL_pu8GetTXClientBuffer(enEHIOResource, &u32TXBufferCap);
	
	if (NULL != pu8TXBuffer)
	{
		pu8TXData = pu8TXBuffer;		
	}
	else
	{
		/* No TX response buffer is available */
		DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount = 0;
		return;
	}
		
	
	switch (DLL_astPortConfigCB[DLLVirtualChannelIDX].enLLProtocol)
	{			
		case PROTAPI_enLLUARTSZDelim:
		{
			while((nASCII_CR == *pu8RXData) || (nASCII_LF == *pu8RXData))
			{
				pu8RXData++;
			}
			break;
		}
		default:
		{
			break;
		}
	}
		
	switch (DLL_astPortConfigCB[DLLVirtualChannelIDX].enNLProtocol)
	{
		case PROTAPI_enNLNone:
		{
			/* Invoke callback to USER NL RX handler */
			if ((NULL != DLL_astPortConfigCB[DLLVirtualChannelIDX].pfRXTransferCB)
			 && (NULL != pu8RXData))
			{
				DLL_astPortConfigCB[DLLVirtualChannelIDX].pfRXTransferCB(enEHIOResource, 
								pu8RXData, 
								(uint32)(DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount),
								IOAPI_IO_RX);	
			}	
			break;
		}	
		case PROTAPI_enISO15765:
		{
			switch (DLL_astPortConfigCB[DLLVirtualChannelIDX].enTLProtocol)
			{
				case PROTAPI_enTLUDS:
				{				
					switch (DLL_astPortConfigCB[DLLVirtualChannelIDX].enLLProtocol)
					{
						case PROTAPI_enLLCAN11:
						{	
							/* Get message ID */
							pstCANMB = (CANHA_tstCANMB*)pu8RXData;
							
							//u32Temp = (pstCANMB -> u32ID) >> CANHA_nCANIDSTDSHIFT;
							u32Temp = pstCANMB -> u32ID;
							
							/* Is the message ID for diagnostics? */
							if ((DLL_astPortConfigCB[DLLVirtualChannelIDX].stNetConfig.uNetInfo.stCANNetInfo.u32GlobalCANDiagAddress
								== u32Temp) ||
									(DLL_astPortConfigCB[DLLVirtualChannelIDX].stNetConfig.uNetInfo.stCANNetInfo.u32CANDiagAddress
								== u32Temp))
							{							
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[0] = (uint8)(pstCANMB -> u32DWH >> 24);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[1] = (uint8)(pstCANMB -> u32DWH >> 16);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[2] = (uint8)(pstCANMB -> u32DWH >> 8);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[3] = (uint8)(pstCANMB -> u32DWH >> 0);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[4] = (uint8)(pstCANMB -> u32DWL >> 24);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[5] = (uint8)(pstCANMB -> u32DWL >> 16);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[6] = (uint8)(pstCANMB -> u32DWL >> 8);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[7] = (uint8)(pstCANMB -> u32DWL >> 0);	
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount = (uint8)((pstCANMB -> u32CS & 0x000f0000) >> 16);
								
								pu8RXData = &DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[0];
								UDSNL_vReceiveFrame(enEHIOResource, pu8RXData, pu8TXData, &u32TXByteCount, u32TXBufferCap);																
								
								if (0 < u32TXByteCount)
								{
									/* Insert the CANIDs and DLC */
									DLL_vInsertCANIDAndDLC(pu8TXBuffer, &u32TXByteCount,
										DLL_astPortConfigCB[DLLVirtualChannelIDX].stNetConfig.uNetInfo.stCANNetInfo.u32CANDiagAddress + 8);
								}								
							}
						}

						case PROTAPI_enUSB2:
						{		
							while (0 < DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount)
							{		
								u32TXByteCountOld = u32TXByteCount;	
								UDSNL_vReceiveFrame(enEHIOResource, pu8RXData, pu8TXData, &u32TXByteCount, u32TXBufferCap);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount = 7 < DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount ? 
									DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount - 8 : 0;
								pu8RXData = 0 < DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount ? pu8RXData + 8 : NULL;
								pu8TXData += (u32TXByteCount - u32TXByteCountOld);
								DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount = 0 == *pu8RXData ?
									0 : DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount;
							}
					
							break;
						}
						default:
						{
							break;
						}
					}						
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}	
		case PROTAPI_enISO15765_IP:
		{	
			switch (DLL_astPortConfigCB[DLLVirtualChannelIDX].enTLProtocol)
			{
				case PROTAPI_enTLUDS_UDP:
				{		
					while (0 < DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount)
					{		
						u32TXByteCountOld = u32TXByteCount;	
						UDSNL_vReceiveFrame(enEHIOResource, pu8RXData, pu8TXData, &u32TXByteCount, u32TXBufferCap);
						DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount = 7 < DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount ? 
							DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount - 8 : 0;
						pu8RXData = 0 < DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount ? pu8RXData + 8 : NULL;
						pu8TXData += (u32TXByteCount - u32TXByteCountOld);
						DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount = 0 == *pu8RXData ?
							0 : DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount;
					}
					
					break;
				}
				default:
				{
					break;
				}
			}						
		}
		default:
		{
			break;
		}
	}
	
	if (0 == u32TXByteCount)
	{
		/* Release the buffer if no response generated */
		DLL_vReleaseTXClientBuffer(pu8TXData);
	}
	else
	{
		/* Enqueue the response */
		boQueuedOK = DLLBYTEQUEUE_vQueueBytes(&DLL_astTXDLLByteQueue[DLLVirtualChannelIDX], pu8TXBuffer, u32TXByteCount);
		
		/* Release the buffer */
		DLL_vReleaseTXClientBuffer(pu8TXBuffer);			
	}
	
	/* TODO warning suppressed */
	(void)boQueuedOK;
}

void DLL_vStart(uint32* const u32Stat)
{
	uint32 u32QueueIDX = 0;
	uint32 u32IDX;
	
	/* Initialise IIC byte queue structs */	
	for (u32IDX = 0; u32IDX < DLL_nIICVirtualChannelCount; u32IDX++)
	{
		DLLBYTEQUEUE_vInit(&DLL_astTXDLLByteQueue[u32QueueIDX++], 
				DLL_nIICTXQueueByteCount, (puint8)&DLL_au8ByteIICByteQueue[u32IDX][0]);
	}
	
	/* Initialise SPI byte queue structs */	
	for (u32IDX = 0; u32IDX < DLL_nSPIVirtualChannelCount; u32IDX++)
	{
		DLLBYTEQUEUE_vInit(&DLL_astTXDLLByteQueue[u32QueueIDX++], 
				DLL_nSPITXQueueByteCount, (puint8)&DLL_au8ByteSPIByteQueue[u32IDX][0]);
	}

	/* Initialise UART byte queue structs */
	for (u32IDX = 0; u32IDX < DLL_nUARTVirtualChannelCount; u32IDX++)
	{
		DLLBYTEQUEUE_vInit(&DLL_astTXDLLByteQueue[u32QueueIDX++], 
				DLL_nUARTTXQueueByteCount, (puint8)&DLL_au8ByteUARTByteQueue[u32IDX][0]);
	}

	/* Initialise CAN byte queue structs */
	for (u32IDX = 0; u32IDX < DLL_nCANVirtualChannelCount; u32IDX++)
	{
		DLLBYTEQUEUE_vInit(&DLL_astTXDLLByteQueue[u32QueueIDX++], 
				DLL_nCANTXQueueByteCount, (puint8)&DLL_au8ByteCANByteQueue[u32IDX][0]);
	}

	/* Initialise USB byte queue structs */
	for (u32IDX = 0; u32IDX < DLL_nUSBVirtualChannelCount; u32IDX++)
	{
		DLLBYTEQUEUE_vInit(&DLL_astTXDLLByteQueue[u32QueueIDX++],
		DLL_nUSBTXQueueByteCount, (puint8)&DLL_au8ByteUSBByteQueue[u32IDX][0]);
	}

	/* Initialise ENET byte queue structs */
	for (u32IDX = 0; u32IDX < DLL_nENETVirtualChannelCount; u32IDX++)
	{
		DLLBYTEQUEUE_vInit(&DLL_astTXDLLByteQueue[u32QueueIDX++], 
				DLL_nENETTXQueueByteCount, (puint8)&DLL_au8ByteENETByteQueue[u32IDX][0]);
	}	
	
	/* Initialise the buffer client array */
	for (u32IDX = 0; u32IDX < DLL_nClientTXBufferCount; u32IDX++)	
	{
		DLL_aenDLLTXBufferClient[u32IDX] = EH_IO_Invalid;
	}
	
	/* Initialise the TX control blocks */
	for (u32IDX = 0; u32IDX < (DLL_nIICTXWorkBuffCount +
															DLL_nSPITXWorkBuffCount +
															DLL_nUARTTXWorkBuffCount +
															DLL_nCANTXWorkBuffCount +
															DLL_nUSBTXWorkBuffCount +
															DLL_nENETTXWorkBuffCount); u32IDX++)
	{	
		DLL_astTXCB[u32IDX].pu8TXData = NULL;
	}
	
	OS_xModuleStartOK(*u32Stat);
}

void DLL_vRun(uint32* const u32Stat)
{
	DLL_tDLLVirtualChannel DLLVirtualChannelIDX;
	IOAPI_tenEHIOResource enEHIOResource;
	uint32 u32TXBytesMaxDequeued;
	uint32 u32TXBufferCap;
	puint8 pu8TXData;
	Bool boReleaseImmediate;
	
	for (DLLVirtualChannelIDX = 0; DLLVirtualChannelIDX < DLL_nVirtualChannelCount; DLLVirtualChannelIDX++)
	{
		if (0 < DLLBYTEQUEUE_u32GetQueuedCount(&DLL_astTXDLLByteQueue[DLLVirtualChannelIDX]))
		{
			enEHIOResource = DLL_tGetEHIOResource(DLLVirtualChannelIDX);
					
			if (DLL_astPortConfigCB[DLLVirtualChannelIDX].enVIOResource == enEHIOResource)
			{
				u32TXBytesMaxDequeued = DLL_rau16TXFrameMaxBytes[DLLVirtualChannelIDX];
				
				/* Get a TX buffer */		
				pu8TXData = DLL_pu8GetTXClientBuffer(enEHIOResource, &u32TXBufferCap);
				
				if (NULL != pu8TXData)
				{				
					/* Uh oh really need to check if channel is available here first */
					DLLBYTEQUEUE_vDequeueBytes(&DLL_astTXDLLByteQueue[DLLVirtualChannelIDX], 
						pu8TXData, &u32TXBytesMaxDequeued);
					
					boReleaseImmediate = DLL_boSendFrame(enEHIOResource, pu8TXData, u32TXBytesMaxDequeued);	
					
					if (true == boReleaseImmediate)
					/* Some TX channel such as ENET rebuffer so release is OK, others
					   such as UART transit more bytes by CB, so no immediate release */
					{						
						DLL_vReleaseTXClientBuffer(pu8TXData);
					}
				}
			}			
		}
	}
}

void DLL_vTerminate(uint32* const u32Stat)
{

}

Bool DLL_boInitDLLChannel(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstCommsConfig)
{
	Bool boRetVal = false;
	SYSAPI_ttClientHandle tClientHandleReq;	
	SYSAPI_ttClientHandle tClientHandle;
	DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);	

	if (-1 != DLLVirtualChannelIDX)
	{
	    if ((EH_FIRST_UART <= pstCommsConfig->enVIOResource) &&
		(EH_LAST_UART >= pstCommsConfig->enVIOResource))
		{
			tClientHandleReq = DLL_xGetClientHandle();
			tClientHandle  = (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stUARTPinInfo.enRXPin, tClientHandleReq);
			tClientHandle &= (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stUARTPinInfo.enTXPin, tClientHandleReq);
				
			if (EH_IO_Invalid != pstCommsConfig->stPinConfig.uPinInfo.stUARTPinInfo.enRTSPin)
			{
				tClientHandle &= (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stUARTPinInfo.enRTSPin, tClientHandleReq);
			}
				
			if (EH_IO_Invalid != pstCommsConfig->stPinConfig.uPinInfo.stUARTPinInfo.enCTSPin)
			{				
				tClientHandle &= (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stUARTPinInfo.enCTSPin, tClientHandleReq);
			}
		}

	    else if ((EH_FIRST_IIC <= pstCommsConfig->enVIOResource) &&
	    (EH_LAST_IIC >= pstCommsConfig->enVIOResource))
		{
			tClientHandleReq = DLL_xGetClientHandle();				
			tClientHandle  = (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stIICPinInfo.enSDAPin, tClientHandleReq);
			tClientHandle &= (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stIICPinInfo.enSCLPin, tClientHandleReq);
		}

	    else if ((EH_FIRST_SPI <= pstCommsConfig->enVIOResource) &&
	    (EH_LAST_SPI >= pstCommsConfig->enVIOResource))
		{
			tClientHandleReq = DLL_xGetClientHandle();				
			tClientHandle  = (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stSPIPinInfo.enMISOPin, tClientHandleReq);
			tClientHandle &= (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stSPIPinInfo.enMOSIPin, tClientHandleReq);
		    tClientHandle &= (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stSPIPinInfo.enSCKPin, tClientHandleReq);
		}

	    else if ((EH_ENET_FIRST_CH <= pstCommsConfig->enVIOResource) &&
	    (EH_ENET_LAST_CH >= pstCommsConfig->enVIOResource))
		{
			/* No additional pin resources are required for ENET - Kernel managed resource*/
			tClientHandleReq = DLL_xGetClientHandle();
			tClientHandle = tClientHandleReq;
		}	

	    else if ((EH_FIRST_CAN <= pstCommsConfig->enVIOResource) &&
	    (EH_LAST_CAN >= pstCommsConfig->enVIOResource))
		{
			tClientHandleReq = DLL_xGetClientHandle();				
			tClientHandle  = (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stCANPinInfo.enTXPin, tClientHandleReq);
			tClientHandle &= (SYSAPI_ttClientHandle)RESM_RequestEHIOResource(pstCommsConfig->stPinConfig.uPinInfo.stCANPinInfo.enRXPin, tClientHandleReq);
		}	

	    else if ((EH_FIRST_USB <= pstCommsConfig->enVIOResource) &&
	    (EH_LAST_USB >= pstCommsConfig->enVIOResource))
		{
			tClientHandleReq = DLL_xGetClientHandle();
			tClientHandle = tClientHandleReq;				
		}
					
		else
		{
			tClientHandleReq = 0xffffffff;
			tClientHandle = 0;
		}
			
			
	    if (tClientHandleReq == tClientHandle)
		/* Proceed if required resources were obtained */
		{
			memcpy(&DLL_astPortConfigCB[DLLVirtualChannelIDX], pstCommsConfig, sizeof(IOAPI_tstPortConfigCB));			
			boRetVal = true;
		}			
	}
		
	return boRetVal;
}

void DLL_vReceiveUARTBytes(IOAPI_tenEHIOResource enEHIOResource, DLL_tstRXDLLData* pRXFrame)
{
	DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);
	
	DLL_stRXDLLData[DLLVirtualChannelIDX].biRXFull = false;
	
	while(pRXFrame->u8DataCount-- > 0)
	{
		switch(DLL_astPortConfigCB[DLLVirtualChannelIDX].enLLProtocol)
		{		
			case PROTAPI_enLLUARTPacked:		
				if((0x80 & pRXFrame->u8Data[0]) == 0x80)
				{
					if((0xC0 & pRXFrame->u8Data[0]) == 0xC0)
					{
						DLL_stRXDLLData[DLLVirtualChannelIDX].u8RXByteIDX = 0;	
					}
					else
					{
						if(DLL_stRXDLLData[DLLVirtualChannelIDX].u8RXByteIDX == (TXRX_MSB_DELIM_SIZE - 1))
							DLL_stRXDLLData[DLLVirtualChannelIDX].biRXFull = true;
					}
				}	
				DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[
					DLL_stRXDLLData[DLLVirtualChannelIDX].u8RXByteIDX++] =
					pRXFrame->u8Data[pRXFrame->u8RXByteIDX++];				
				break;
				
			case PROTAPI_enLLUARTSZDelim:	
				if((NULL != pRXFrame -> u8Data[0])
					&& (nASCII_LF != pRXFrame -> u8Data[0]))
				{
					DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount++;
					DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[
						DLL_stRXDLLData[DLLVirtualChannelIDX].u8RXByteIDX++] =
						pRXFrame->u8Data[pRXFrame->u8RXByteIDX];
					
					if(nASCII_CR == pRXFrame -> u8Data[0])
					{
						DLL_stRXDLLData[DLLVirtualChannelIDX].biRXFull = true;
					}							
				}
				break;
				
			default:
				break;
		}
	}	
	
	if(DLL_stRXDLLData[DLLVirtualChannelIDX].biRXFull == true) 
	{
		if(PROTAPI_enLLUARTPacked == DLL_astPortConfigCB[DLLVirtualChannelIDX].enLLProtocol)
		{
			PACKEDDLL_vUnpack(&DLL_stRXDLLData[DLLVirtualChannelIDX]);	
		}
		
		if(PROTAPI_enLLUARTSZDelim == DLL_astPortConfigCB[DLLVirtualChannelIDX].enLLProtocol)
		{
			DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[DLL_stRXDLLData[DLLVirtualChannelIDX].u8RXByteIDX] = 0;	
			DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount++;
		}
		
		DLL_stRXDLLData[DLLVirtualChannelIDX].u8RXByteIDX = 0;
		DLL_stRXDLLData[DLLVirtualChannelIDX].u8DataCount = 8;
		pRXFrame->biRXFull = false;

		DLL_vFrameRXCB(enEHIOResource, &DLL_stRXDLLData[DLLVirtualChannelIDX].u8Data[0]);		
	}
}

void DLL_vTransmitUARTBytesCB(IOAPI_tenEHIOResource enEHIOResource)
{
 	DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);	
	
	if (NULL != DLL_astTXCB[DLLVirtualChannelIDX].pu8TXData)
	{
		if (0 < DLL_astTXCB[DLLVirtualChannelIDX].u8ByteCount)
		{	
			DLL_astTXCB[DLLVirtualChannelIDX].pu8TXData++;	
			DLL_astTXCB[DLLVirtualChannelIDX].u8ByteCount--;			
			UART_vSendChar(enEHIOResource,
				*DLL_astTXCB[DLLVirtualChannelIDX].pu8TXData);
		}
		else
		{
			UART_vDisableTXInterrupt(enEHIOResource);
			DLL_vReleaseTXClientBuffer(DLL_astTXCB[DLLVirtualChannelIDX].pu8TXData);	
			DLL_astTXCB[DLLVirtualChannelIDX].pu8TXData	= NULL;
		}
	}
}


static Bool DLL_boSendFrame(IOAPI_tenEHIOResource enEHIOResource, puint8 pu8TXData, uint32 u32TXByteCount)
{
	DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);
	IOAPI_tstTransferCB stTransferCB;	
	PROTAPI_tstCANMsg stCANMsg;
	Bool boSyncSent = false;
	
	if ((EH_FIRST_UART <= enEHIOResource) &&
	(EH_LAST_UART >= enEHIOResource))
	{
		switch (DLL_astPortConfigCB[DLLVirtualChannelIDX].enLLProtocol)
		{	
			case PROTAPI_enLLUARTPacked:
			{
				if (9 > u32TXByteCount)
				{
					//memcpy((DLL_tstTXBytesDLLData*)&DLL_astTXBytesDLLData[DLLVirtualChannelIDX].u8Data[0], pu8TXData, u32TXByteCount);
					//DLL_astTXBytesDLLData[DLLVirtualChannelIDX].u8DataCount = (uint8)u32TXByteCount;	
					//PACKEDDLL_vPack(&DLL_astTXBytesDLLData[DLLVirtualChannelIDX].u8Data[0]);
					//DLL_astTXBytesDLLData[DLLVirtualChannelIDX].u8DataCount = 12;
					//UART_vSendChar(enEHIOResource,
						//DLL_astTXBytesDLLData[DLLVirtualChannelIDX].u8Data[0]);
					//DLL_astTXBytesDLLData[DLLVirtualChannelIDX].u8TXByteIDX = 1;		
				}
				break;				
			}
			case PROTAPI_enLLUARTSZDelim:
			{
				u32TXByteCount = DLL_nTXBytesBufferSize >= u32TXByteCount ? u32TXByteCount : DLL_nTXBytesBufferSize;
					
				if (NULL == DLL_astTXCB[DLLVirtualChannelIDX].pu8TXData)
				{
					DLL_astTXCB[DLLVirtualChannelIDX].pu8TXData	= pu8TXData;
					DLL_astTXCB[DLLVirtualChannelIDX].u8ByteCount = u32TXByteCount;						
					UART_vSendChar(enEHIOResource, *(DLL_astTXCB[DLLVirtualChannelIDX].pu8TXData));	
				}
						
			}
			default:
			{
				break;
			}
		}		
	}

	else if ((EH_ENET_FIRST_CH <= enEHIOResource) &&
	(EH_ENET_LAST_CH >= enEHIOResource))
	{
		switch (DLL_astPortConfigCB[DLLVirtualChannelIDX].enLLProtocol)
		{				
			case PROTAPI_enLL802_3:
			{			
				DLL_vIPBufferTX(enEHIOResource, pu8TXData, u32TXByteCount);
				boSyncSent = true;
				break;
			}
			default:
			{
				break;
			}
		}
	}	

	else if ((EH_FIRST_CAN <= enEHIOResource) &&
	(EH_LAST_CAN >= enEHIOResource))
	{
		stCANMsg.u32ID = ((*(pu8TXData) & 0x1f) << 24)
										+ (*(pu8TXData + 1) << 16)
										+ (*(pu8TXData + 2) << 8)
										+ (*(pu8TXData + 3) << 0);
		stCANMsg.u32DWH = (*(pu8TXData + 4) << 24)
										+ (*(pu8TXData + 5) << 16)
										+ (*(pu8TXData + 6) << 8)
										+ (*(pu8TXData + 7) << 0);			
		stCANMsg.u32DWL = (*(pu8TXData + 8) << 24)
										+ (*(pu8TXData + 9) << 16)
										+ (*(pu8TXData + 10) << 8)
										+ (*(pu8TXData + 11) << 0);	
		stCANMsg.u8DLC = u32TXByteCount - 4;//matthew pipeline here?
			
		stTransferCB.enEHIOResource = enEHIOResource;
		stTransferCB.enTransferType = IOAPI_IO_TX;
		stTransferCB.pfCB = NULL;
		stTransferCB.pvData = (void*)&stCANMsg;
		stTransferCB.u32ByteCount = u32TXByteCount;
			
		CAN_vInitTransfer(&stTransferCB);		
		boSyncSent = true;
	}

	else if ((EH_FIRST_USB <= enEHIOResource) &&
	(EH_LAST_USB >= enEHIOResource))
	{
		switch (DLL_astPortConfigCB[DLLVirtualChannelIDX].enLLProtocol)
		{				
			case PROTAPI_enUSB2:
			{			
				USB_vSend(enEHIOResource, pu8TXData, u32TXByteCount);
				boSyncSent = true;
				break;
			}
			default:
			{
				break;
			}
		}
	}	
		
	else
	{

	}

	
	return boSyncSent;
}

Bool DLL_vQueueCANMessage(IOAPI_tenEHIOResource enEHIOResource, PROTAPI_tstCANMsg* pstCANMsg)
{
	Bool boRetVal;
	uint8 au8TXData[12];
	uint32 u32TXByteCount = pstCANMsg->u8DLC + 4;
	
	au8TXData[0] = (pstCANMsg->u8DLC - 1) << 5;
	au8TXData[0] += ((pstCANMsg->u32ID & 0x1f000000) >> 24);
	au8TXData[1] = (pstCANMsg->u32ID >> 16) & 0xff;
	au8TXData[2] = (pstCANMsg->u32ID >> 8) & 0xff;
	au8TXData[3] = pstCANMsg->u32ID & 0xff;
	au8TXData[4] = (pstCANMsg->u32DWH >> 24) & 0xff;
	au8TXData[5] = (pstCANMsg->u32DWH >> 16) & 0xff;	
	au8TXData[6] = (pstCANMsg->u32DWH >> 8) & 0xff;	
	au8TXData[7] = pstCANMsg->u32DWH & 0xff;	
	au8TXData[8] = (pstCANMsg->u32DWL >> 24) & 0xff;
	au8TXData[9] = (pstCANMsg->u32DWL >> 16) & 0xff;	
	au8TXData[10] = (pstCANMsg->u32DWL >> 8) & 0xff;	
	au8TXData[11] = pstCANMsg->u32DWL & 0xff;	

	boRetVal = DLL_boQueueMessage(enEHIOResource, &au8TXData[0], u32TXByteCount);
	
	return boRetVal;
}

Bool DLL_boQueueMessage(IOAPI_tenEHIOResource enEHIOResource, puint8 pu8TXBuffer, uint32 u32TXByteCount)
{
	DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);
	Bool boQueueOK = false;
	
	boQueueOK = DLLBYTEQUEUE_vQueueBytes(&DLL_astTXDLLByteQueue[DLLVirtualChannelIDX], pu8TXBuffer, u32TXByteCount);	
	
	return boQueueOK;
}

DLL_tDLLVirtualChannel DLL_tGetVirtualChannel(IOAPI_tenEHIOResource enEHIOResource)
{
	DLL_tDLLVirtualChannel DLLVirtualChannel = -1;
	
	if ((EH_FIRST_VIO_COMMS <= enEHIOResource) && (EH_LAST_VIO_COMMS >= enEHIOResource))
	{
		DLLVirtualChannel = enEHIOResource - EH_VIO_IIC1;
	}
		
	return DLLVirtualChannel;
}

static IOAPI_tenEHIOResource DLL_tGetEHIOResource(DLL_tDLLVirtualChannel DLLVirtualChannel)
{
	return (IOAPI_tenEHIOResource)(EH_VIO_IIC1 + DLLVirtualChannel);
}

IOAPI_tenPortMode DLL_enGetChannelMode(IOAPI_tenEHIOResource enEHIOResource)
{
	IOAPI_tenPortMode enMode = IOAPI_enPortNone;
	DLL_tDLLVirtualChannel DLLVirtualChannelIDX = DLL_tGetVirtualChannel(enEHIOResource);

	if (-1 != DLLVirtualChannelIDX)
	{
		enMode = DLL_astPortConfigCB[DLLVirtualChannelIDX].enPortMode;
	}
	
	return enMode;
}


static void DLLBYTEQUEUE_vInit(CBYTEQUEUE_tstQueue* pstQueue, uint32 u32Size, void* pu8Data)
{
	pstQueue->u32Head = 0;
	pstQueue->u32Tail = 0;
	pstQueue->u32Size = u32Size;
	pstQueue->pu8Data = pu8Data;
}

static uint32 DLLBYTEQUEUE_u32GetVacantCount(CBYTEQUEUE_tstQueue* pstQueue)
{
	uint32 u32VacantCount;
	
	if (pstQueue->u32Head <= pstQueue->u32Tail)
	{
		u32VacantCount = pstQueue->u32Size - (pstQueue->u32Tail - pstQueue->u32Head) - 1;		
	}
	else
	{
		u32VacantCount = pstQueue->u32Head - pstQueue->u32Tail - 1;
	}
	
	return u32VacantCount;
}

static uint32 DLLBYTEQUEUE_u32GetQueuedCount(CBYTEQUEUE_tstQueue* pstQueue)
{
	return (pstQueue->u32Size - DLLBYTEQUEUE_u32GetVacantCount(pstQueue) - 1);
}

static Bool DLLBYTEQUEUE_vQueueBytes(CBYTEQUEUE_tstQueue* pstQueue, puint8 pu8TXData, uint32 u32TXByteCount)
{
	Bool boRetVal = FALSE;
	uint32 u32Temp;
		
	
	if (u32TXByteCount <= DLLBYTEQUEUE_u32GetVacantCount(pstQueue))
	{
		if (pstQueue->u32Head <= pstQueue->u32Tail)
		{
			u32Temp = pstQueue->u32Size - pstQueue->u32Tail;
			memcpy(pstQueue->pu8Data + pstQueue->u32Tail, pu8TXData, MIN(u32Temp, u32TXByteCount));
			
			if (u32Temp < u32TXByteCount)
			{
				memcpy(pstQueue->pu8Data, pu8TXData + u32Temp, u32TXByteCount - u32Temp);
			}
		}
		else
		{
			memcpy(pstQueue->pu8Data + pstQueue->u32Tail, pu8TXData, u32TXByteCount);
		}
		
		boRetVal = TRUE;	
		pstQueue->u32Tail = (pstQueue->u32Tail + u32TXByteCount) % pstQueue->u32Size;
	}
		
	return boRetVal;
}

static void DLLBYTEQUEUE_vDequeueBytes(CBYTEQUEUE_tstQueue* pstQueue, puint8 pu8Data, puint32 pu32ByteCount)
{
	uint32 u32Temp;
	uint32 u32BytesQueued = DLLBYTEQUEUE_u32GetQueuedCount(pstQueue);
	
	*pu32ByteCount = MIN(*pu32ByteCount , u32BytesQueued);

	if (pstQueue->u32Head < pstQueue->u32Tail)
	{
		memcpy(pu8Data, pstQueue->pu8Data + pstQueue->u32Head, *pu32ByteCount);
	}
	else
	{
		u32Temp = pstQueue->u32Size - pstQueue->u32Head;
		u32Temp = MIN(*pu32ByteCount, u32Temp);
		memcpy(pu8Data, pstQueue->pu8Data + pstQueue->u32Head, u32Temp);
		
		if (*pu32ByteCount > u32Temp)
		{
			memcpy(pu8Data + u32Temp, pstQueue->pu8Data, *pu32ByteCount - u32Temp);			
		}
	}
	
	pstQueue->u32Head = (pstQueue->u32Head + *pu32ByteCount) % pstQueue->u32Size;
}

static puint8 DLL_pu8GetTXClientBuffer(IOAPI_tenEHIOResource enEHIOResource, puint32 pu32TXBufferCap)
{
	uint8 u8BufferIDXStart;
	uint8 u8BufferIDXEnd;
	uint8 u8BufferIDX;
	puint8 pu8Buffer = NULL;
	uint32 u32BufferBytes;
	Bool boVacantBuff = false;
	
	if ((EH_FIRST_IIC <= enEHIOResource) &&
	(EH_LAST_IIC >= enEHIOResource))
	{
		u8BufferIDXStart = 0;
		u8BufferIDXEnd = DLL_nIICTXWorkBuffCount;
		pu8Buffer = &DLL_au8TXIICBuffBuild[0][0];
		u32BufferBytes = DLL_nIICTXWorkBuffMaxBytes;
	}
		
	else if ((EH_FIRST_SPI <= enEHIOResource) &&
	(EH_LAST_SPI >= enEHIOResource))
	{
		u8BufferIDXStart = DLL_nIICTXWorkBuffCount;
		u8BufferIDXEnd = DLL_nIICTXWorkBuffCount + 
											DLL_nSPITXWorkBuffCount;
		pu8Buffer = &DLL_au8TXSPIBuffBuild[0][0];
		u32BufferBytes = DLL_nSPITXWorkBuffMaxBytes;			
	}		
		
	else if ((EH_FIRST_UART <= enEHIOResource) &&
	(EH_LAST_UART >= enEHIOResource))
	{
		u8BufferIDXStart = DLL_nIICTXWorkBuffCount + 
												DLL_nSPITXWorkBuffCount;
		u8BufferIDXEnd = DLL_nIICTXWorkBuffCount + 
											DLL_nSPITXWorkBuffCount + 
											DLL_nUARTTXWorkBuffCount;
		pu8Buffer = &DLL_au8TXUARTBuffBuild[0][0];
		u32BufferBytes = DLL_nUARTTXWorkBuffMaxBytes;			
	}				
		
	else if ((EH_FIRST_CAN <= enEHIOResource) &&
	(EH_LAST_CAN >= enEHIOResource))
	{
		u8BufferIDXStart = DLL_nIICTXWorkBuffCount +
		DLL_nSPITXWorkBuffCount +
		DLL_nUARTTXWorkBuffCount;
		u8BufferIDXEnd = DLL_nIICTXWorkBuffCount +
		DLL_nSPITXWorkBuffCount +
		DLL_nUARTTXWorkBuffCount +
		DLL_nCANTXWorkBuffCount;
		pu8Buffer = &DLL_au8TXCANBuffBuild[0][0];
		u32BufferBytes = DLL_nCANTXWorkBuffMaxBytes;
	}

	else if ((EH_FIRST_USB <= enEHIOResource) &&
	(EH_LAST_USB >= enEHIOResource))
	{
		u8BufferIDXStart = DLL_nIICTXWorkBuffCount +
		DLL_nSPITXWorkBuffCount +
		DLL_nUARTTXWorkBuffCount +
		DLL_nCANTXWorkBuffCount;
		u8BufferIDXEnd = DLL_nIICTXWorkBuffCount +
		DLL_nSPITXWorkBuffCount +
		DLL_nUARTTXWorkBuffCount +
		DLL_nCANTXWorkBuffCount +
		DLL_nUSBTXWorkBuffCount;
		pu8Buffer = &DLL_au8TXUSBBuffBuild[0][0];
		u32BufferBytes = DLL_nUSBTXWorkBuffMaxBytes;
	}

	else if ((EH_ENET_FIRST_CH <= enEHIOResource) &&
	(EH_ENET_LAST_CH >= enEHIOResource))
	{
		u8BufferIDXStart = DLL_nIICTXWorkBuffCount +
		DLL_nSPITXWorkBuffCount +
		DLL_nUARTTXWorkBuffCount +
		DLL_nCANTXWorkBuffCount +
		DLL_nUSBTXWorkBuffCount;
		u8BufferIDXEnd = DLL_nIICTXWorkBuffCount +
		DLL_nSPITXWorkBuffCount +
		DLL_nUARTTXWorkBuffCount +
		DLL_nCANTXWorkBuffCount +
		DLL_nUSBTXWorkBuffCount +
		DLL_nENETTXWorkBuffCount;
		pu8Buffer = &DLL_au8TXENETBuffBuild[0][0];
		u32BufferBytes = DLL_nENETTXWorkBuffMaxBytes;
	}
		
	else
	{
		u8BufferIDXStart = 0;
		u8BufferIDXStart = 0;
	}
			
	for (u8BufferIDX = u8BufferIDXStart; u8BufferIDX < u8BufferIDXEnd; u8BufferIDX++)
	{
		if(EH_IO_Invalid == DLL_aenDLLTXBufferClient[u8BufferIDX])
		{
			DLL_aenDLLTXBufferClient[u8BufferIDX] = enEHIOResource;
			boVacantBuff = true;
			break;
		}
		pu8Buffer += u32BufferBytes;
	}
	
	pu8Buffer = (true == boVacantBuff) ? pu8Buffer : NULL;
	*pu32TXBufferCap = u32BufferBytes;
	
	return pu8Buffer;
}

static void DLL_vInsertCANIDAndDLC(puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32CANID)
{
	uint32 u32TXByteCount = *pu32TXByteCount;
	uint32 u32TXByteCountNew = 0;
	uint32 u32FrameCount = u32TXByteCount / 8;
	uint32 u32FrameBytes;
	puint8 pu8Dest;
	puint8 pu8Source;
		
	if (0 != (u32TXByteCount % 8)) 
	{	
		u32FrameCount++;
	}
		
	while (0 < u32FrameCount--)
	{
		pu8Dest = pu8TXBuffer + (12 * u32FrameCount) + 4;
		pu8Source = pu8TXBuffer + (8 * u32FrameCount);
		
		if (0 != (u32TXByteCount % 8))
		{
			u32FrameBytes = u32TXByteCount % 8;
			u32TXByteCount -= (u32TXByteCount % 8);
		}
		else
		{
			u32FrameBytes = 8;
			u32TXByteCount -= 8;
		}
		
		u32TXByteCountNew += u32FrameBytes;
		u32TXByteCountNew += 4;

		while (0 < u32FrameBytes)
		{
			u32FrameBytes--;
			*(pu8Dest + u32FrameBytes) = *(pu8Source + u32FrameBytes);			
		}
			
		pu8Dest -= 4;
		*pu8Dest++ = (((u32FrameBytes - 1) & 0xff) << 5) + ((u32CANID & 0x1f000000) >> 24);
		*pu8Dest++ = (u32CANID & 0xff0000) >> 16;		
		*pu8Dest++ = (u32CANID & 0xff00) >> 8;		
		*pu8Dest = (u32CANID & 0xff);		
		

	}
	
	*pu32TXByteCount = u32TXByteCountNew;
}
	
static void DLL_vReleaseTXClientBuffer(puint8 pu8TXBuffer)
{
	uint8 u8BufferIDXStart;
	uint8 u8BufferIDXEnd;
	uint8 u8BufferIDX;
	puint8 pu8Buffer;
	uint32 u32BufferBytes;
	Bool boBufferReleased = false;

	/* Try release a IIC client working buffer */
	u8BufferIDXStart = 0;
	u8BufferIDXEnd = DLL_nIICTXWorkBuffCount;
	pu8Buffer = &DLL_au8TXIICBuffBuild[0][0];
	u32BufferBytes = DLL_nIICTXWorkBuffMaxBytes;
	
	for (u8BufferIDX = u8BufferIDXStart; u8BufferIDX < u8BufferIDXEnd; u8BufferIDX++)	
	{		
		if ((pu8Buffer <= pu8TXBuffer) && ((pu8Buffer + u32BufferBytes) > pu8TXBuffer))	
		{																																								
			DLL_aenDLLTXBufferClient[u8BufferIDX] = EH_IO_Invalid;												
			boBufferReleased = true;																												
		}																																								
		pu8Buffer += u32BufferBytes;	
	}
	if (true == boBufferReleased) return;

	/* Try release a SPI client working buffer */	
	u8BufferIDXStart = DLL_nIICTXWorkBuffCount;
	u8BufferIDXEnd = DLL_nIICTXWorkBuffCount + 
									 DLL_nSPITXWorkBuffCount;
	pu8Buffer = &DLL_au8TXSPIBuffBuild[0][0];
	u32BufferBytes = DLL_nSPITXWorkBuffMaxBytes;	
	
	for (u8BufferIDX = u8BufferIDXStart; u8BufferIDX < u8BufferIDXEnd; u8BufferIDX++)	
	{		
		if ((pu8Buffer <= pu8TXBuffer) && ((pu8Buffer + u32BufferBytes) > pu8TXBuffer))	
		{																																								
			DLL_aenDLLTXBufferClient[u8BufferIDX] = EH_IO_Invalid;												
			boBufferReleased = true;																												
		}																																								
		pu8Buffer += u32BufferBytes;	
	}
	if (true == boBufferReleased) return;
	
	/* Try release a UART client working buffer */		
	u8BufferIDXStart = DLL_nIICTXWorkBuffCount + 
										 DLL_nSPITXWorkBuffCount;
	u8BufferIDXEnd = DLL_nIICTXWorkBuffCount + 
									 DLL_nSPITXWorkBuffCount + 
									 DLL_nUARTTXWorkBuffCount;
	pu8Buffer = &DLL_au8TXUARTBuffBuild[0][0];
	u32BufferBytes = DLL_nUARTTXWorkBuffMaxBytes;		
	
	for (u8BufferIDX = u8BufferIDXStart; u8BufferIDX < u8BufferIDXEnd; u8BufferIDX++)	
	{		
		if ((pu8Buffer <= pu8TXBuffer) && ((pu8Buffer + u32BufferBytes) > pu8TXBuffer))	
		{																																								
			DLL_aenDLLTXBufferClient[u8BufferIDX] = EH_IO_Invalid;												
			boBufferReleased = true;																												
		}																																								
		pu8Buffer += u32BufferBytes;	
	}
	if (true == boBufferReleased) return;
	
	/* Try release a CAN client working buffer */		
	u8BufferIDXStart = DLL_nIICTXWorkBuffCount + 
										 DLL_nSPITXWorkBuffCount +
										 DLL_nUARTTXWorkBuffCount;
	u8BufferIDXEnd = DLL_nIICTXWorkBuffCount + 
									 DLL_nSPITXWorkBuffCount + 
									 DLL_nUARTTXWorkBuffCount +
									 DLL_nCANTXWorkBuffCount;
	pu8Buffer = &DLL_au8TXCANBuffBuild[0][0];
	
	for (u8BufferIDX = u8BufferIDXStart; u8BufferIDX < u8BufferIDXEnd; u8BufferIDX++)	
	{		
		if ((pu8Buffer <= pu8TXBuffer) && ((pu8Buffer + u32BufferBytes) > pu8TXBuffer))	
		{																																								
			DLL_aenDLLTXBufferClient[u8BufferIDX] = EH_IO_Invalid;												
			boBufferReleased = true;																												
		}																																								
		pu8Buffer += u32BufferBytes;	
	}
	if (true == boBufferReleased) return;

	/* Try release a USB client working buffer */
	u8BufferIDXStart = DLL_nIICTXWorkBuffCount +
	DLL_nSPITXWorkBuffCount +
	DLL_nUARTTXWorkBuffCount +
	DLL_nCANTXWorkBuffCount;
	u8BufferIDXEnd = DLL_nIICTXWorkBuffCount +
	DLL_nSPITXWorkBuffCount +
	DLL_nUARTTXWorkBuffCount +
	DLL_nCANTXWorkBuffCount +
	DLL_nUSBTXWorkBuffCount;
	pu8Buffer = &DLL_au8TXUSBBuffBuild[0][0];
	
	for (u8BufferIDX = u8BufferIDXStart; u8BufferIDX < u8BufferIDXEnd; u8BufferIDX++)
	{
		if ((pu8Buffer <= pu8TXBuffer) && ((pu8Buffer + u32BufferBytes) > pu8TXBuffer))
		{
			DLL_aenDLLTXBufferClient[u8BufferIDX] = EH_IO_Invalid;
			boBufferReleased = true;
		}
		pu8Buffer += u32BufferBytes;
	}
	if (true == boBufferReleased) return;

	/* Try release a ENET client working buffer */	
	u8BufferIDXStart = DLL_nIICTXWorkBuffCount + 
										 DLL_nSPITXWorkBuffCount +
										 DLL_nUARTTXWorkBuffCount +
										 DLL_nCANTXWorkBuffCount +
										 DLL_nUSBTXWorkBuffCount;
	u8BufferIDXEnd = DLL_nIICTXWorkBuffCount + 
									 DLL_nSPITXWorkBuffCount + 
									 DLL_nUARTTXWorkBuffCount +
									 DLL_nCANTXWorkBuffCount +
									 DLL_nUSBTXWorkBuffCount +
									 DLL_nENETTXWorkBuffCount;
	pu8Buffer = &DLL_au8TXENETBuffBuild[0][0];
	u32BufferBytes = DLL_nENETTXWorkBuffMaxBytes;		
	
	for (u8BufferIDX = u8BufferIDXStart; u8BufferIDX < u8BufferIDXEnd; u8BufferIDX++)	
	{		
		if ((pu8Buffer <= pu8TXBuffer) && ((pu8Buffer + u32BufferBytes) > pu8TXBuffer))	
		{																																								
			DLL_aenDLLTXBufferClient[u8BufferIDX] = EH_IO_Invalid;												
			boBufferReleased = true;																												
		}																																								
		pu8Buffer += u32BufferBytes;	
	}
}

