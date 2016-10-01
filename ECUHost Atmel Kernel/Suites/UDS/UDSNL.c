/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX																										*/
/* FILE NAME:          XXX.c                                          				*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _UDSNL_C
#include "types.h"
#include "declarations.h"
#include "IOAPI.h"
#include "DIAGAPI.h"
#include "OS.h"
#include "DLL.h"
#include "COMMONNL.h"
#include "UDSNL.h"
#include "UDSSL.h"
#include "UDSAL.h"


UDSNL_tenSegRXMode UDSNL_enSegRXMode = UDSNL_enSegRXModeInvalid;
IOAPI_tenEHIOResource UDSNL_enActiveEHIOResource;
sint32 UDSNL_i32ResourceTimerIDX;

static bool UDSNL_boCheckActiveResource(IOAPI_tenEHIOResource);
static void UDSNL_vSegmentTXBuffer(puint8, uint32, puint32, uint32);

void
UDSNL_vStart(void)
{
	UDSNL_enActiveEHIOResource = EH_IO_Invalid;
	UDSNL_i32ResourceTimerIDX = -1;
}

void 
UDSNL_vReceiveFrame(IOAPI_tenEHIOResource enEHIOResource, puint8 pu8RXData, puint8 pu8TXData, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint32 u32EntryTXByteCount = *pu32TXByteCount;
	
	if (false == UDSNL_boCheckActiveResource(enEHIOResource))
	{
		return;
	}	
	
	switch(*pu8RXData & 0xf0)
	{
		{
			case RX_SEG_FRAME_UNSEG:
			{
				/* High byte of count word is 0 for an unsegmented message */
				UDSNL_stRXSmallBuffer.u8Data[0] = 0;
				memcpy(&UDSNL_stRXSmallBuffer.u8Data[1], pu8RXData, 8);
				UDSNL_stRXSmallBuffer.u8RXReceivedCount = 1 + (UDSNL_stRXSmallBuffer.u8Data[1] & 0x07);
				UDSSL_vProcessBufferCB(&UDSNL_stRXSmallBuffer.u8Data[0], 
					UDSNL_stRXSmallBuffer.u8RXReceivedCount, pu8TXData, pu32TXByteCount, u32TXCap);				

				// 				UDSNL_stRXSmallBuffer.u8RXReceivedCount = 8;
// 				UDSSL_vProcessBufferCB(&UDSNL_stRXSmallBuffer.u8Data[0], 8, pu8TXData, pu32TXByteCount, u32TXCap);
				
				if (0 != (*pu32TXByteCount - u32EntryTXByteCount))
				{
					UDSNL_vSegmentTXBuffer(pu8TXData, *pu32TXByteCount - u32EntryTXByteCount, pu32TXByteCount, u32TXCap);
				}
				break;
			}
			case RX_SEG_FRAME_ONE:
			{
				switch(*(pu8RXData+2))
				{
					case UDSAL_SID_RD:
						UDSNL_stRXSmallBuffer.pu8RX = (uint8*)&UDSNL_stRXSmallBuffer.u8Data[0];	
						UDSNL_stRXLargeBuffer.u8SeqNumber = 0x20;					
						UDSNL_stRXSmallBuffer.u8RXRequestedCount = 0x100 * (*pu8RXData & 0x0f)
																												+ *(pu8RXData + 1);									
						memcpy(UDSNL_stRXSmallBuffer.pu8RX, pu8RXData, 8);
						UDSNL_stRXSmallBuffer.pu8RX += 8;
						UDSNL_stRXSmallBuffer.u8RXReceivedCount = 6;	
						UDSNL_enSegRXMode = UDSNL_enSegRXModeSmall;
						break;
					
					case UDSAL_SID_TD:
						UDSNL_stRXLargeBuffer.pu8RX = (uint8*)&UDSNL_stRXLargeBuffer.u8Data[0];	
						UDSNL_stRXLargeBuffer.u8SeqNumber = 0x20;
						UDSNL_stRXLargeBuffer.u16RXRequestedCount = 0x100 * (*pu8RXData & 0x0f)
																												+ *(pu8RXData + 1);					
						UDSNL_stRXLargeBuffer.u8RXBlock = *(pu8RXData + 3);
						UDSNL_stRXLargeBuffer.biRXErr = false;
						UDSNL_stRXLargeBuffer.biRXOK = false;				
						memcpy(UDSNL_stRXLargeBuffer.pu8RX, pu8RXData + 4, 4);
						UDSNL_stRXLargeBuffer.pu8RX += 4;
						UDSNL_stRXLargeBuffer.u16RXReceivedCount = 6;	
						UDSNL_enSegRXMode = UDSNL_enSegRXModeLarge;
					
						//memcpy(&DIAG_u8DiagBuff, &pRXBytes->u8Data[1], RX_BUFF_SIZE);
						//UDSSL_vProcessBufferCB(&UDSNL_stRXLargeBuffer.u8Data[0], UDSNL_stRXLargeBuffer.u8RXReceivedCount);			
						break;
					
					default:
						UDSNL_stRXSmallBuffer.pu8RX = (uint8*)&UDSNL_stRXSmallBuffer.u8Data[0];	
						UDSNL_stRXSmallBuffer.u8RXRequestedCount = 0x100 * (*pu8RXData & 0x0f)
																												+ *(pu8RXData + 1);									
						memcpy(UDSNL_stRXSmallBuffer.pu8RX, pu8RXData, 8);
					
						/* Mask the byte count high byte */
						*(UDSNL_stRXSmallBuffer.pu8RX) &= 0x0f;
						UDSNL_stRXSmallBuffer.pu8RX += 8;
						UDSNL_stRXSmallBuffer.u8RXReceivedCount = 6;	
						UDSNL_enSegRXMode = UDSNL_enSegRXModeSmall;																
						break;
				}
				break;
			}
			case RX_SEG_FRAME_SEQ:
			{
				switch(UDSNL_enSegRXMode)
				{
					case UDSNL_enSegRXModeSmall:
						if (UDSNL_stRXSmallBuffer.u8RXRequestedCount > UDSNL_stRXSmallBuffer.u8RXReceivedCount)
						{
							if ((COMMONNL_RX_BUFF_SMALL_SIZE - 7) >= UDSNL_stRXSmallBuffer.u8RXReceivedCount)
							{
								memcpy(UDSNL_stRXSmallBuffer.pu8RX, &pu8RXData[1], 7);
								UDSNL_stRXSmallBuffer.pu8RX += 7;
								UDSNL_stRXSmallBuffer.u8RXReceivedCount += 7;	
								if(UDSNL_stRXSmallBuffer.u8RXReceivedCount >= UDSNL_stRXSmallBuffer.u8RXRequestedCount)
								{
									//if(UDSNL_stRXSmallBuffer.u8RXRequestedCount <= DIAG_COMMON_BUFF_SIZE)
									{
										UDSSL_vProcessBufferCB(&UDSNL_stRXSmallBuffer.u8Data[0], UDSNL_stRXSmallBuffer.u8RXReceivedCount, pu8TXData, pu32TXByteCount, u32TXCap);
										UDSNL_vSegmentTXBuffer(pu8TXData, *pu32TXByteCount - u32EntryTXByteCount, pu32TXByteCount, u32TXCap);
										UDSNL_enSegRXMode = UDSNL_enSegRXModeInvalid;
									}
								}
							}
							else
							{
								UDSNL_enSegRXMode = USDNL_enSegRXErr;
								//UDSSL_vProcessErrorCB(&UDSNL_stRXSmallBuffer.u8Data[0], UDSNL_stRXSmallBuffer.u8RXReceivedCount, pu8TXData, pu32TXByteCount, u32TXCap);																
							}							
						}
						else
						{
							UDSNL_enSegRXMode = USDNL_enSegRXErr;
							//UDSSL_vProcessErrorCB(&UDSNL_stRXSmallBuffer.u8Data[0], UDSNL_stRXSmallBuffer.u8RXReceivedCount, pu8TXData, pu32TXByteCount, u32TXCap);	
						}					
						break;
					
					case UDSNL_enSegRXModeLarge:
						if((((UDSNL_stRXLargeBuffer.u8SeqNumber + 1) & 0x0f)  == 
								(pu8RXData[0] & 0x0f)) &&
									(UDSNL_stRXLargeBuffer.u16RXRequestedCount > UDSNL_stRXLargeBuffer.u16RXReceivedCount))
						{
							memcpy(UDSNL_stRXLargeBuffer.pu8RX, &pu8RXData[1], 7);
							UDSNL_stRXLargeBuffer.pu8RX += 7;
							UDSNL_stRXLargeBuffer.u16RXReceivedCount += 7;	
							UDSNL_stRXLargeBuffer.u8SeqNumber = pu8RXData[0];
							if(UDSNL_stRXLargeBuffer.u16RXReceivedCount >= UDSNL_stRXLargeBuffer.u16RXRequestedCount)
							{
								UDSNL_stRXLargeBuffer.biRXOK = true;
								UDSNL_enSegRXMode = UDSNL_enSegRXModeInvalid;
								
								/* Re-create the request to initiate a response */
								UDSNL_stRXSmallBuffer.u8Data[0] = 0;
								UDSNL_stRXSmallBuffer.u8Data[1] = 4;
								UDSNL_stRXSmallBuffer.u8Data[2] = UDSAL_SID_TD;								
								UDSNL_stRXSmallBuffer.u8Data[3] =	UDSNL_stRXLargeBuffer.u8RXBlock;
								
								UDSSL_vProcessBufferCB(&UDSNL_stRXSmallBuffer.u8Data[0], 
									UDSNL_stRXSmallBuffer.u8RXReceivedCount, pu8TXData, pu32TXByteCount, u32TXCap);				
								UDSNL_vSegmentTXBuffer(pu8TXData, *pu32TXByteCount - u32EntryTXByteCount, pu32TXByteCount, u32TXCap);						
								break;
							}
						}
						else
						{
							UDSNL_stRXLargeBuffer.biRXErr = true;
							UDSNL_stRXLargeBuffer.biRXOK = false;
						}
						break;
						
					default:
						break;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

static bool
UDSNL_boCheckActiveResource(IOAPI_tenEHIOResource enEHIOResource)
{
	bool boResourceOK = false;
	
	if ((EH_IO_Invalid == UDSNL_enActiveEHIOResource)
	 || (UDSNL_enActiveEHIOResource == enEHIOResource))
	{
		UDSNL_enActiveEHIOResource = enEHIOResource;
		boResourceOK = true;
		if (-1 == UDSNL_i32ResourceTimerIDX)
		{			
			UDSNL_i32ResourceTimerIDX = TIMER_vConfigureAlarm(UDSNL_nActiveResourceTimeout, UDSNL_vActiveResourceTimeout);
		}
		else
		{
			TIMER_vSetAlarm(UDSNL_nActiveResourceTimeout, UDSNL_i32ResourceTimerIDX);
		}
	}
	
	return boResourceOK;
}

void UDSNL_vActiveResourceTimeout(void)
{
	UDSNL_enActiveEHIOResource = EH_IO_Invalid;
}

static void UDSNL_vSegmentTXBuffer(puint8 pu8TXData, uint32 u32TransferByteCount, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint32 u32TXBytesRequired;
	uint16 u16FrameIDX;
	uint8 u8ByteIDX;
	uint8 u8Seq;
	puint8 pu8Dest;
	puint8 pu8Source;
	uint8 u8Data[7];
	
	u32TXBytesRequired =  UDSNL_u32GetTransmissionBytesRequired(u32TransferByteCount);
	
	if (0x8 >= u32TransferByteCount)//matthew210915
	{
		for (u8ByteIDX = 0; u8ByteIDX < 8; u8ByteIDX++)
		{
			*(pu8TXData + u8ByteIDX) = *(pu8TXData + u8ByteIDX + 1);
		}
		if (8 > *pu8TXData)
		{
			for (u8ByteIDX = *pu8TXData + 1; u8ByteIDX < 8; u8ByteIDX++)
			{
				*(pu8TXData + u8ByteIDX) = 0;
			}
		}
	}
	else
	{
		/* Segment the TX data buffer */	
		u16FrameIDX = u32TransferByteCount / 7;
		pu8Dest = pu8TXData + 9 + 8 * (u16FrameIDX - 1);
		pu8Source = pu8TXData + 8 + 7 * (u16FrameIDX - 1);
		
		/* Copy data */
		for (; u16FrameIDX >= 1; u16FrameIDX--)
		{
			memcpy(&u8Data, pu8Source, 7);
			memcpy(pu8Dest, &u8Data, 7);			
			pu8Dest -= 8;
			pu8Source -= 7;
		}
		memcpy(pu8Dest + 1, pu8Source + 1, 6);
		
		/* Write sequence numbers */
		u16FrameIDX = u32TransferByteCount / 7;		
		u8Seq = (u16FrameIDX - 1) & 0x0f;
		pu8Dest = pu8TXData + 8 + 8 * (u16FrameIDX - 1);
		for (; u16FrameIDX >= 1; u16FrameIDX--)
		{	
			*pu8Dest = 0x20 + u8Seq--;
			u8Seq &= 0x0f;
			pu8Dest -= 8;
		}			
		
		*pu8TXData |= 0x10;
		*(pu8TXData + 1) = (uint8)(u32TransferByteCount) & 0xff;
		*pu8TXData = 0x10 + (((u32TransferByteCount) & 0xf00) >> 8);	
		
		/* Add in the additional bytes required for segmentation */
		*pu32TXByteCount += (u32TXBytesRequired - u32TransferByteCount);
	}
}

uint32 UDSNL_u32GetTransmissionBytesRequired(uint32 u32TransferByteCount)
{
	uint32 u32TransmissionBytesRequired;
	
	if (8 > u32TransferByteCount)
	{
		u32TransmissionBytesRequired = 8;
	}
	else
	{
		u32TransmissionBytesRequired =  
				8 + (8 * (((u32TransferByteCount - 7) / 7) + 1));
	}
	
	return u32TransmissionBytesRequired;	
}
