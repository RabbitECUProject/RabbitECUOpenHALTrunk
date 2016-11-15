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
#define _UDSAL_C

#include "UDSAL.h"

#if BUILD_PBL
	#include "ECUHostA_PBL.h"
#endif

unsigned char UDSAL_ucActiveSession;
uint8 UDSAL_u8TransferBlock;
UDSAL_tenDiagSession UDSAL_enDiagSession;
DIAGAPI_tstDataTransferCB UDSAL_stReadDataTransferCB;
DIAGAPI_tstDataTransferCB UDSAL_stWriteDataTransferCB;
uint8 UDSAL_au8ReadTransferData[UDSAL_READ_TRANSFER_SIZE];
uint8 UDSAL_au8WriteTransferData[UDSAL_WRITE_TRANSFER_SIZE];
MSG_tstMBX* UDSAL_pstMBXReadTransfer;
MSG_tstMBX* UDSAL_pstMBXWriteTransfer;
UDSAL_tstDLControlBlock UDSAL_stDLControlBlock;
CQUEUE_tstQueue UDSAL_astDDDICacheQueue[UDSAL_nDDDICacheCount];
uint16 UDSAL_au16DDDICacheSIDs[UDSAL_nDDDICacheCount];
#if BUILD_KERNEL
	uint8 UDSAL_au8DDDICache[UDSAL_nDDDICacheCount][UDSAL_nDDDICacheSize];
#endif
Bool UDSAL_boRDBICaching;


static uint8 UDSAL_u8RESET(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8RDBI(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8RMBA(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8WMBA(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8WDBI(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8DLREQ(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8TFREQ(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8TP(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8SESSION(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8DDDI(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8RC(puint8, uint32, puint8, puint32, uint32);
static uint8 UDSAL_u8TFEXREQ(puint8, uint32, puint8, puint32, uint32);
static void UDSAL_vResponse(puint8, uint8, uint8, puint32, uint32);
static void UDSAL_vConfigureDDDICache(uint16, uint8);
static sint32 UDSAL_i32GetDDDICacheIndex(uint16);
#if (BUILD_SBL)
static Bool UDSAL_boWriteDLBlock(COMMONNL_tstRXLargeBuffer* const, 
															 uint8* const, 
															 uint32);
#endif				
#if (BUILD_SBL) || (BUILD_PBL)
static Bool UDSAL_boUpdateDLBlock(uint32);
#endif

const UDSAL_tstALProcessInfo UDSAL_rastALProcessInfo[UDSAL_SID_COUNT] = UDSAL_nALProcessInfo;

void UDSAL_vProcessBufferCB(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{		
	uint8 u8ServiceIDX;
	uint8 u8Response = UDSAL_RSP_SNSIAS;
#if BUILD_PBL	
	UDSAL_tenContext enContext = UDSAL_enPBLContext;
#endif
#if BUILD_SBL	
	UDSAL_tenContext enContext = UDSAL_enSBLContext;
#endif
#if BUILD_KERNEL	
	UDSAL_tenContext enContext = UDSAL_enAppContext;
#endif
#if BUILD_KERNEL_APP	
	UDSAL_tenContext enContext = UDSAL_enAppContext;
#endif	
	
	for (u8ServiceIDX = 0; u8ServiceIDX < UDSAL_SID_COUNT; u8ServiceIDX++)
	{
		if ((*(pu8RXBuffer + UDSAL_IDX_SID)) == UDSAL_rastALProcessInfo[u8ServiceIDX].u8Service)
		{
			if ((NULL != UDSAL_rastALProcessInfo[u8ServiceIDX].pfProcess) &&
					(enContext >= UDSAL_rastALProcessInfo[u8ServiceIDX].enContext))
			{				
				/* Copy the RX frame to the TX frame and make 
					room for response length word */
				memcpy(pu8TXBuffer, pu8RXBuffer, u32RXDataCount + 2);	
				//u32RXDataCount = 0x100 * *(pu8RXBuffer + UDSAL_IDX_LEN)	+ *(pu8RXBuffer + UDSAL_IDX_LEN + 1)			
				u8Response = (*UDSAL_rastALProcessInfo[u8ServiceIDX].pfProcess)(pu8RXBuffer, u32RXDataCount, pu8TXBuffer, pu32TXByteCount, u32TXCap);
#if BUILD_PBL
				ECUHOSTAPBL_boLatchDiag = true;
#endif				
				break;	
			}	
			else
			{
#if BUILD_PBL
				ECUHOSTAPBL_boLatchDiag = true;
#endif				
				u8Response = UDSAL_RSP_CNC;
				break;
			}	
		}
	}
	
	UDSAL_vResponse(pu8TXBuffer, u8Response, u8ServiceIDX, pu32TXByteCount, u32TXCap);
	
}

static uint8 UDSAL_u8RESET(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response = UDSAL_RSP_NSUPINSESS;
	
	if ((UDSAL_enDiagSessionProgramming == UDSSL_enGetSession()) || 
			(UDSAL_enDiagSessionExtended == UDSSL_enGetSession()))
	{
		u8Response = UDSAL_RSP_OK;
	}

	return u8Response;
}


static uint8 UDSAL_u8RDBI(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response = UDSAL_RSP_BRR;
	uint8 u8DDDIRecsIDX;
	uint8 u8DDDIIDX;
	puint8 pu8DDDIData;
	puint8 pu8RDBIAddress = 0;	
	uint32 u32TransferByteCount = 3;
	uint32 u32BufferBytesFree;
	uint16 u16CID;
	uint32 u32Temp;
	puint8 pu8DDDIResultBuff;	
	sint16 s16DDDICacheIndex;
	
	u32BufferBytesFree = UDSAL_xCalcBytesFree();	
	
	u16CID = *(pu8RXBuffer + UDSAL_IDX_RW_CID - 1) + 0x100 * *(pu8RXBuffer + UDSAL_IDX_RW_CID);
	
	if ((UDSAL_DDDI_CID_START <= u16CID) && (UDSAL_DDDI_CID_END >= u16CID))
	{
		for (u8DDDIIDX = 0; u8DDDIIDX < DIAG_DDDI_COUNT; u8DDDIIDX++)
		{
			if (u16CID == DIAG_astDDDI[u8DDDIIDX].u16DDI)
			{
				pu8DDDIData = (puint8)&DIAG_astDDDI[u8DDDIIDX].u8Data;
				pu8TXBuffer += UDSAL_IDX_RW_DATA;
				s16DDDICacheIndex = UDSAL_i32GetDDDICacheIndex(u16CID);
				
				if ((-1 != s16DDDICacheIndex) && (!CQUEUE_xIsEmpty(UDSAL_astDDDICacheQueue + s16DDDICacheIndex)) && (false == UDSAL_boRDBICaching))
				/* Respond from the data cache */
				{
					u32Temp = (uint32)UDSAL_astDDDICacheQueue[s16DDDICacheIndex].pvData;
					u32Temp += (16 * UDSAL_astDDDICacheQueue[s16DDDICacheIndex].u32Head);
					pu8DDDIResultBuff = (puint8)u32Temp;
					memcpy(pu8TXBuffer, pu8DDDIResultBuff, DIAG_astDDDI[u8DDDIIDX].u8ByteCount);
					CQUEUE_xRemoveItem(UDSAL_astDDDICacheQueue + s16DDDICacheIndex);			
					u32TransferByteCount += DIAG_astDDDI[u8DDDIIDX].u8ByteCount;
					pu8TXBuffer += DIAG_astDDDI[u8DDDIIDX].u8ByteCount;						
				}
				else
				/* Respond per live DDDI */
				{
					for (u8DDDIRecsIDX = 0; u8DDDIRecsIDX < DIAG_astDDDI[u8DDDIIDX].u8RecsCount; u8DDDIRecsIDX++)				
					{
						if (4 == (DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength & 0x0f))
						{
							memcpy(&pu8RDBIAddress, pu8DDDIData, 4);
							pu8DDDIData += 4;
						}
						else
						{
							memcpy(&pu8RDBIAddress, pu8DDDIData, 2);
							pu8DDDIData += 2;
						}					
											
						if ((NULL != pu8RDBIAddress) && (*pu8DDDIData < u32BufferBytesFree))
						{
							memcpy(pu8TXBuffer, pu8RDBIAddress, *pu8DDDIData);	
							u32TransferByteCount += (uint32)*pu8DDDIData;
							u32BufferBytesFree -= (uint32)*pu8DDDIData;
							pu8TXBuffer += (uint8)*pu8DDDIData;						
							pu8DDDIData++;
						}
					}
				}				
				
				/* Calculate the new TX byte count prior to segmentation */
				*pu32TXByteCount += u32TransferByteCount;
				u8Response = UDSAL_RSP_OK;
				break;				
			}				
		}
	}
	else
	{			
		if (TRUE == UDSAL_stReadDataTransferCB.boTransferComplete)
		{
			UDSAL_stReadDataTransferCB.boTransferComplete = FALSE;		
			UDSAL_stReadDataTransferCB.pu8Data = &UDSAL_au8ReadTransferData[0];			
			u16CID = 0x100 * (*(pu8RXBuffer)) + *(pu8RXBuffer + 1);
		}
	}
	
	return u8Response;
}

static uint8 UDSAL_u8RMBA(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response;
	uint8 u8AddressAndLength;
	uint32 u32AddressData = 0;
	uint32 u32TransferByteCount = 2;
	uint32 u32DataMask;
	uint32 u32BufferBytesFree;
	uint32 u32BufferBytesRequired = 2;
	uint8 u8IDX;
	puint8 pu8Source = pu8RXBuffer + 3;
	uint32 u8AddressLength = 0;
	
	u32BufferBytesFree = UDSAL_xCalcBytesFree();	
	
	u8AddressAndLength = *(pu8Source++);
	u32DataMask = 1;
	
	for (u8IDX = 0; u8IDX < (u8AddressAndLength & 0x0f); u8IDX++)
	{
		u32AddressData += *(pu8Source++) * u32DataMask;		
		u32DataMask *= 0x100;		
		u8AddressLength++;
	}
	
	u32DataMask = 1;	
	
	for (u8IDX = 0; u8IDX < (u8AddressAndLength >> 4); u8IDX++)
	{
		u32TransferByteCount += *(pu8Source++) * u32DataMask;		
		u32DataMask *= 0x100;		
	}	
	
	u32TransferByteCount += u8AddressLength;	
	u32BufferBytesRequired += UDSNL_u32GetTransmissionBytesRequired(u32TransferByteCount);
	
	if (u32BufferBytesFree >= u32BufferBytesRequired)
	{				
		memcpy(pu8TXBuffer + 3 + u8AddressLength, (void*)u32AddressData, u32TransferByteCount);
		
		for (u8IDX = 0; u8IDX < u8AddressLength; u8IDX++)
		{
			*(pu8TXBuffer + 3 + u8IDX) = u32AddressData & 0xff;
			u32AddressData = u32AddressData >> 8;
		}		
		
		u8Response = UDSAL_RSP_OK;
		*pu8TXBuffer = u32TransferByteCount / 0x100;
		*(pu8TXBuffer + 1) = u32TransferByteCount % 0x100;
		*pu32TXByteCount += u32TransferByteCount;
	}
	else
	{
		u8Response = UDSAL_RSP_ROOR;		
	}
	
	return u8Response;	
}

static uint8 UDSAL_u8WMBA(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response;
	uint8 u8AddressAndLength;
	uint32 u32AddressData = 0;
	uint32 u32LengthData = 0;
	uint32 u32DataMask;
	uint32 u32BufferBytesFree;
	uint32 u32BufferBytesRequired;
	uint8 u8IDX;
	puint8 pu8Source = pu8RXBuffer + 3;
	
	u8AddressAndLength = *(pu8Source++);
	u32DataMask = 1;
	
	for (u8IDX = 0; u8IDX < (u8AddressAndLength & 0x0f); u8IDX++)
	{
		u32AddressData += *(pu8Source++) * u32DataMask;		
		u32DataMask *= 0x100;		
	}
	
	u32DataMask = 1;	
	
	for (u8IDX = 0; u8IDX < (u8AddressAndLength >> 4); u8IDX++)
	{
		u32LengthData += *(pu8Source++) * u32DataMask;		
		u32DataMask *= 0x100;		
	}	
	
	u32BufferBytesFree = u32TXCap - *pu32TXByteCount;
	u32BufferBytesRequired = 2 + 
													(u8AddressAndLength & 0x0f) +
													(u8AddressAndLength >> 4);
	
	if (u32BufferBytesFree >= u32BufferBytesRequired)
	{		
		memcpy((void*)u32AddressData, pu8Source, u32LengthData);
		u8Response = UDSAL_RSP_OK;
		*pu8TXBuffer = u32BufferBytesRequired / 0x100;
		*(pu8TXBuffer + 1) = u32BufferBytesRequired % 0x100;
		*pu32TXByteCount += u32BufferBytesRequired;
	}
	else
	{
		u8Response = UDSAL_RSP_ROOR;		
	}
	
	return u8Response;	
}

static uint8 UDSAL_u8WDBI(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response = UDSAL_RSP_BRR;
	MSG_tenMBXErr enMBXErr = MSG_enOK;	
	tpfClientCB pfClientCB;	
	CLIENT_tenErr enErr;
	
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	if (TRUE == UDSAL_stWriteDataTransferCB.boTransferComplete)
	{
		UDSAL_stWriteDataTransferCB.boTransferComplete = FALSE;		
		UDSAL_stWriteDataTransferCB.pu8Data = &UDSAL_au8WriteTransferData[0];
		UDSAL_stWriteDataTransferCB.u8DataCount = u32RXDataCount - 3;		
		UDSAL_stWriteDataTransferCB.u16CID = 0x100 * (*(pu8RXBuffer + UDSAL_IDX_RW_CID - 1)) + *(pu8RXBuffer + UDSAL_IDX_RW_CID);
		memcpy((void*)&UDSAL_au8WriteTransferData[0],
					 (void*)(pu8RXBuffer + UDSAL_IDX_RW_DATA - 1),
						u32RXDataCount - 3);		

		/* Check if a mailbox is allocated for diag writes */
		if (NULL == UDSAL_pstMBXWriteTransfer)
		{
			enMBXErr = MSG_enMBXNew((MSG_tstMBX**)&UDSAL_pstMBXWriteTransfer, 
						MSG_enDiagDataWrite);
		}
	
		if (MSG_enOK == enMBXErr)
		{
			MSG_vMBXPost(UDSAL_pstMBXWriteTransfer, (void*)&UDSAL_stWriteDataTransferCB);
			pfClientCB = (tpfClientCB)DIAG_DataWriteCB;
			
			if ((NULL != pfClientCB) && (MSG_boMBXValid(UDSAL_pstMBXWriteTransfer)))
			{
				enErr = CLIENT_enEnqueueCB(UDSAL_pstMBXWriteTransfer, pfClientCB);
				u8Response = (CLIENT_enOK == enErr) ? UDSAL_RSP_OK : UDSAL_RSP_GR;
			}
		}
		else
		{
				u8Response = UDSAL_RSP_GR;
		}
	}
	else
	{
		u8Response = UDSAL_RSP_CNC;		
	}
#endif	
	
	*pu32TXByteCount += 8;

	return u8Response;
}

static uint8 UDSAL_u8DLREQ(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response;	
	uint32 u32Data;
	uint32 u32DataCount;
	COMMONNL_tstRXLargeBuffer* pstSourceBuffer;			
	uint8* pu8TargetAddr;
	Bool boDLReqErr;
	const uint32 ru32AddrMin = BUILD_DL_ADDR_MIN;
	const uint32 ru32AddrMax = BUILD_DL_ADDR_MAX;
	
	u32Data = pu8RXBuffer[5] << 24 
					| pu8RXBuffer[6] << 16 
					| pu8RXBuffer[7] << 8 
					| pu8RXBuffer[8];	
	u32DataCount = pu8RXBuffer[9] << 24 
							 | pu8RXBuffer[10] << 16 
							 | pu8RXBuffer[11] << 8 
							 | pu8RXBuffer[12];	
	
	if ((ru32AddrMin <= u32Data) &&
			(ru32AddrMax >= (u32Data + u32DataCount - 1)))
	{
		pu8TargetAddr = (uint8*)u32Data;	
		pstSourceBuffer = &UDSNL_stRXLargeBuffer;	
		UDSAL_u8TransferBlock = 0u;	

#if	BUILD_KERNEL
		FEE_boWriteControlBlock(pstSourceBuffer, pu8TargetAddr, u32DataCount);	
		boDLReqErr = FEE_boEraseForDownload(pu8TargetAddr, u32DataCount);
#elif	BUILD_KERNEL_APP
		FEE_boWriteControlBlock(pstSourceBuffer, pu8TargetAddr, u32DataCount);	
		boDLReqErr = FEE_boEraseForDownload(pu8TargetAddr, u32DataCount);		
#elif BUILD_SBL
		FEE_boWriteControlBlock(pstSourceBuffer, pu8TargetAddr, u32DataCount);	
		boDLReqErr = FEE_boEraseForDownload(pu8TargetAddr, u32DataCount);		
#elif BUILD_UBL		
		FEE_boWriteControlBlock(pstSourceBuffer, pu8TargetAddr, u32DataCount);	
		boDLReqErr = FEE_boEraseForDownload(pu8TargetAddr, u32DataCount);		
#elif BUILD_PBL		
		boDLReqErr = UDSAL_boWriteDLBlock(pstSourceBuffer, pu8TargetAddr, u32DataCount);	
#endif
		
		u8Response = (false == boDLReqErr) ? UDSAL_RSP_OK : UDSAL_RSP_ROOR;
	}

	*pu32TXByteCount += 8;	
	return u8Response;	
}

static uint8 UDSAL_u8TFREQ(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response = UDSAL_RSP_TDS;
	uint8 u8BlockPrev = pu8RXBuffer[3] - 1;
	
	if (u8BlockPrev != UDSAL_u8TransferBlock)
	{
#if	BUILD_KERNEL
		FEE_boUpdateControlBlock(0);
#elif	BUILD_KERNEL_APP
		FEE_boUpdateControlBlock(0);		
#elif BUILD_SBL
		FEE_boUpdateControlBlock(0);	
#elif BUILD_UBL		
		FEE_boUpdateControlBlock(0);	
#elif BUILD_PBL		
		UDSAL_boUpdateDLBlock(0);	
#endif		
		
		UDSNL_stRXLargeBuffer.biRXErr = true;
		u8Response = UDSAL_RSP_WBSC;
	}
	else
	{
#if	BUILD_KERNEL
		FEE_boUpdateControlBlock(UDSNL_stRXLargeBuffer.u16RXRequestedCount - 2);
#elif	BUILD_KERNEL_APP
		FEE_boUpdateControlBlock(UDSNL_stRXLargeBuffer.u16RXRequestedCount - 2);		
#elif BUILD_SBL
		FEE_boUpdateControlBlock(UDSNL_stRXLargeBuffer.u16RXRequestedCount - 2);
#elif BUILD_UBL		
		FEE_boUpdateControlBlock(UDSNL_stRXLargeBuffer.u16RXRequestedCount - 2);	
#elif BUILD_PBL		
		UDSAL_boUpdateDLBlock(UDSNL_stRXLargeBuffer.u16RXRequestedCount - 2);	
#endif			

		UDSAL_u8TransferBlock++;
		u8Response = UDSAL_RSP_OK;
	}
	
	*pu32TXByteCount += 8;	
	return u8Response;
}

static uint8 UDSAL_u8SESSION(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	
	return 0;
}

static uint8 UDSAL_u8TP(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response;
	
	switch (*(pu8RXBuffer + UDSAL_IDX_SUBSID))
	{
		case 0:
		{
			u8Response = UDSAL_RSP_OK;
			*pu32TXByteCount += 8;
			break;
		}
		case 0x80:
		{
			u8Response = UDSAL_RSP_SUPPRESS;			
			break;			
		}
		default:
		{
			u8Response = UDSAL_RSP_SNSIAS;
			*pu32TXByteCount += 8;			
			break;
		}
	}
	
	return u8Response;
}

static uint8 UDSAL_u8DDDI(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8DDDIIDX;
	uint8 u8Response = UDSAL_RSP_SNS;
	puint8 pu8Data;
	uint16 u16DDDISID;
	uint8 u8ResultByteCount = 0;
	sint16 i16RXDataCount = (uint16)u32RXDataCount - 5;
	uint32 u32Temp;
	
	pu8RXBuffer += 3;	
	
	if (UDSAL_DDDI_SSID_DMBA == *pu8RXBuffer++)
	{
		u8Response = UDSAL_RSP_CNC;
		u16DDDISID = *(pu8RXBuffer++);
		u16DDDISID += 0x100 * *(pu8RXBuffer++);		
		
		for (u8DDDIIDX = 0; u8DDDIIDX < DIAG_DDDI_COUNT; u8DDDIIDX++)
		{			
			if ((u16DDDISID == DIAG_astDDDI[u8DDDIIDX].u16DDI) || (0 == DIAG_astDDDI[u8DDDIIDX].u16DDI))
			{
				DIAG_astDDDI[u8DDDIIDX].u16DDI = u16DDDISID;				
				i16RXDataCount -= 5;
				DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength = *(pu8RXBuffer++);
				DIAG_astDDDI[u8DDDIIDX].u8RecsCount = 0;			
				pu8Data = &DIAG_astDDDI[u8DDDIIDX].u8Data[0];
				*pu32TXByteCount += 5;		
				
				memset((void*)pu8Data, 0, DIAG_DDDI_DATA_COUNT);

				while (0 < i16RXDataCount)
				{			
					if (0 != *(pu8RXBuffer + 4))
					{
						memcpy(pu8Data, pu8RXBuffer, DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength & 0x0f);
						u32Temp = DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength & 0x0f;
						u32Temp += (uint32)pu8RXBuffer;
						u8ResultByteCount	+= *((puint8)u32Temp);					
						pu8Data += (DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength & 0x0f);
						pu8RXBuffer += (DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength & 0x0f);
						memcpy(pu8Data, pu8RXBuffer, DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength >> 4);
						pu8Data += (DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength >> 4);
						pu8RXBuffer += (DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength >> 4);
						i16RXDataCount -= ((DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength & 0x0f) + 
									(DIAG_astDDDI[u8DDDIIDX].u8AddressAndLength >> 4));		
						DIAG_astDDDI[u8DDDIIDX].u8RecsCount++;	

					}	
					else
					{
						/* Kill the loop because we have found trailing zeros */
						i16RXDataCount = 0;
					}
				}
				
				DIAG_astDDDI[u8DDDIIDX].u8ByteCount = u8ResultByteCount;
				
				if (0x9000 == (u16DDDISID & 0xff00))
				{
					UDSAL_vConfigureDDDICache(u16DDDISID, u8ResultByteCount);
				}
				
				u8Response = UDSAL_RSP_OK;
				break;
			}
			else
			{
			}
		}	
	}
	
	return u8Response;
}

static uint8 UDSAL_u8RC(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response = UDSAL_RSP_OK;
	uint16 u16RoutineID;
	Bool boRetCode = FALSE;

	u16RoutineID = 0x100 * *(pu8RXBuffer + 4);
	u16RoutineID += *(pu8RXBuffer + 5);
	
	switch (*(pu8RXBuffer + 3))
	{
		case UDSAL_SSID_RC_STR:
		{
			switch (u16RoutineID)
			{
				case UDSAL_RCUID_RUN_DL:
				{
					
					
				}
				break;
				
				case UDSAL_RCUID_PARTITION:
				{
#if BUILD_SBL
						FEE_boPartition();
#endif
				}
				break;
				
				case UDSAL_RCUID_WORK_TO_NVM:
				{
#if BUILD_SBL
					boRetCode = FEE_boNVMWorkingCopy(FALSE, TRUE);
#elif BUILD_KERNEL			
					boRetCode = FEE_boNVMWorkingCopy(FALSE, TRUE);	
#elif BUILD_KERNEL_APP				
					boRetCode = FEE_boNVMWorkingCopy(FALSE, TRUE);	
#endif		
					if (FALSE == boRetCode)
					{
						u8Response = UDSAL_RSP_CNC;	
					}
					*pu32TXByteCount += 8;
				}
				break;		

				case UDSAL_RCUID_CLEAR_NVM:
				{
#if BUILD_SBL
					boRetCode = FEE_boNVMClear();
#elif BUILD_KERNEL			
					boRetCode = FEE_boNVMClear();	
#elif BUILD_KERNEL_APP				
					boRetCode = FEE_boNVMClear();	
#endif		
					if (FALSE == boRetCode)
					{
						u8Response = UDSAL_RSP_CNC;	
					}
					*pu32TXByteCount += 8;
				}
				break;	
				
				case UDSAL_RCUID_BCKP_ADCCAL:
				{
#if BUILD_SBL					
					boRetCode = ADC_boBackupCalibrations();
#endif					
#if BUILD_KERNEL					
					boRetCode = ADC_boBackupCalibrations();		
#endif
#if BUILD_KERNEL_APP				
					boRetCode = ADC_boBackupCalibrations();		
#endif					
					
					if (FALSE == boRetCode)
					{
						u8Response = UDSAL_RSP_CNC;	
					}
					*pu32TXByteCount += 8;				
				}		
				break;				
				
				default:
				{
					break;
				}
			}
			break;
		}
		default:
		{
				u8Response = UDSAL_RSP_SFNS;			
				break;
		}		
	}
	
	return u8Response;	
}

static void UDSAL_vResponse(puint8 pu8TXBuffer, uint8 u8ResponseCode, uint8 u8ServiceIDX, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8BytesResp;
	
	switch (u8ResponseCode)
	{
		case UDSAL_RSP_OK:
		{
			u8BytesResp = UDSAL_rastALProcessInfo[u8ServiceIDX].u8BytesResp;
			
			switch (UDSAL_rastALProcessInfo[u8ServiceIDX].u8Service)
			{
				case UDSAL_SID_RD:
				{
					*pu8TXBuffer = 0;						
					*(pu8TXBuffer + 1) = u8BytesResp;					
					*(pu8TXBuffer + 3) = 0x20;
					*(pu8TXBuffer + 4) = (COMMONNL_RX_BUFF_LARGE_SIZE - 2) / 0x100;
					*(pu8TXBuffer + 5) = (COMMONNL_RX_BUFF_LARGE_SIZE - 2) % 0x100;
					break;
				}
				default:
				{
					if (0xff != u8BytesResp)
					{
						*(pu8TXBuffer + 1) = u8BytesResp;
					}							
				}
			}
			
			*(pu8TXBuffer + 2) += 0x40;			
			break;
		}
		case UDSAL_RSP_FLOW:
		{
			*(pu8TXBuffer + 0) = 0x30;
			*(pu8TXBuffer + 1) = 0x0;
			*(pu8TXBuffer + 2) = 0x0;
			*(pu8TXBuffer + 3) = 0x0;
			*(pu8TXBuffer + 4) = 0x0;
			*(pu8TXBuffer + 5) = 0x0;
			*(pu8TXBuffer + 6) = 0x0;
			*(pu8TXBuffer + 7) = 0x0;
			*pu32TXByteCount += 8;			
			break;	
		}
		case UDSAL_RSP_SUPPRESS:
		{		
			break;
		}
		default:
		{
			*(pu8TXBuffer + 0) = 3;
			*(pu8TXBuffer + 2) = *(pu8TXBuffer + 1);
			*(pu8TXBuffer + 1) = UDSAL_RSP_SNSIAS;
			*(pu8TXBuffer + 3) = u8ResponseCode;
			*pu32TXByteCount += 8;
			break;
		}			
	}
}

static sint32 UDSAL_i32GetDDDICacheIndex(uint16 u16CacheDDDISID)
{
	uint32 u32DDDICacheIDX = 0;
	sint32 s32DDDICacheIndex = -1;
	
	while (UDSAL_nDDDICacheCount > u32DDDICacheIDX)
	{
		if (u16CacheDDDISID == UDSAL_au16DDDICacheSIDs[u32DDDICacheIDX])
		{
			s32DDDICacheIndex	= u32DDDICacheIDX;
			break;
		}
		u32DDDICacheIDX++;
	}	

	return s32DDDICacheIndex;	
}


static void UDSAL_vConfigureDDDICache(uint16 u16CacheDDDISID, uint8 u8ByteCount)
{
	uint32 u32DDDICacheIDX = 0;
	
	while (UDSAL_nDDDICacheCount > u32DDDICacheIDX)
	{
		if (0 == UDSAL_au16DDDICacheSIDs[u32DDDICacheIDX])
		{		
			UDSAL_astDDDICacheQueue[u32DDDICacheIDX].u32Size = UDSAL_nDDDICacheSize / u8ByteCount;		
#if BUILD_KERNEL
			UDSAL_astDDDICacheQueue[u32DDDICacheIDX].pvData = 
				(void*)&UDSAL_au8DDDICache[u32DDDICacheIDX][0];			
#elif BUILD_KERNEL_APP			
			UDSAL_astDDDICacheQueue[u32DDDICacheIDX].pvData = (void*)0x2000ff00;	
#endif			
			UDSAL_au16DDDICacheSIDs[u32DDDICacheIDX] = u16CacheDDDISID;
			break;
		}
		u32DDDICacheIDX++;		
	}
}

void UDSAL_vStart(uint32* const u32Stat)
{
	uint32 u32DDDICacheIDX;
	
	UDSSL_vStart();
	DIAG_vStart();
	
	UDSAL_stReadDataTransferCB.boTransferComplete = TRUE;
	UDSAL_stWriteDataTransferCB.boTransferComplete = TRUE;	
	UDSAL_boRDBICaching = false;	
	
	for (u32DDDICacheIDX = 0; u32DDDICacheIDX < UDSAL_nDDDICacheCount; u32DDDICacheIDX++)
	{
		memset((void*)&UDSAL_astDDDICacheQueue[u32DDDICacheIDX], 0, sizeof(CQUEUE_tstQueue));
		UDSAL_au16DDDICacheSIDs[u32DDDICacheIDX] = 0;	
	}
	
	OS_xModuleStartOK(*u32Stat);
}

void UDSAL_vRun(uint32* const u32Stat)
{
	uint8 au8UDSPrompt[5];
	puint8 pu8DDDIResultBuff;
	uint32 u32DummyTXCount;
	uint8 u8DDDIIDX;
	uint32 u32Temp;
	
	au8UDSPrompt[0] = 0x00;
	au8UDSPrompt[1] = 0x03;
	au8UDSPrompt[2] = 0x22;
	
	for (u8DDDIIDX = 0; u8DDDIIDX < DIAG_DDDI_COUNT; u8DDDIIDX++)
	{
		if (0x9000 == (DIAG_astDDDI[u8DDDIIDX].u16DDI & 0xff00))
		/* If DDDI active and rate = 1 ms */
		{
			au8UDSPrompt[3] = DIAG_astDDDI[u8DDDIIDX].u16DDI & 0xff;
			au8UDSPrompt[4] = (DIAG_astDDDI[u8DDDIIDX].u16DDI >> 8) & 0xff;		
			
			CPU_xEnterCritical();	

			u32Temp = (uint32)UDSAL_astDDDICacheQueue[0].pvData;
			u32Temp += (16 * UDSAL_astDDDICacheQueue[0].u32Tail);
			u32Temp -= 5;
			pu8DDDIResultBuff = (puint8)u32Temp;
			
			if (!CQUEUE_xIsFull(UDSAL_astDDDICacheQueue))
			{
				CQUEUE_xAddItem(UDSAL_astDDDICacheQueue);
				UDSAL_boRDBICaching = true;
				(void)UDSAL_u8RDBI(&au8UDSPrompt[0], 5u, pu8DDDIResultBuff, &u32DummyTXCount, 20);
				UDSAL_boRDBICaching = false;
			}
			CPU_xExitCritical();
		}
	}
}

void UDSAL_vTerminate(uint32* const u32Stat)
{

}

#if (BUILD_SBL) || (BUILD_PBL)
static Bool UDSAL_boWriteDLBlock(COMMONNL_tstRXLargeBuffer* const pstSourceBuffer, 
															 uint8* const pu8TargetAddress, 
															 uint32 u32DataCount)
{
	Bool boRetVal = FALSE;
	
	if (false == UDSAL_stDLControlBlock.boPending)
	{
		UDSAL_stDLControlBlock.pstSourceBuffer = pstSourceBuffer;
		UDSAL_stDLControlBlock.pu8TargetAddress = pu8TargetAddress;
		UDSAL_stDLControlBlock.u32WriteCount = u32DataCount;
		UDSAL_stDLControlBlock.u32AccumulateCount = 0;		
		UDSAL_stDLControlBlock.boPending = true;	
		UDSAL_stDLControlBlock.boComplete = false;	
		boRetVal = TRUE;		
	}	
	
	return boRetVal;
}
#endif

#if (BUILD_SBL)
static Bool UDSAL_boUpdateDLBlock(uint32 u32BlockWriteCount)
{		
	puint8 pu8SourceData;
	puint8 pu8BufferData;
	
	if (UDSAL_stDLControlBlock.boPending == true)
	{
		if ((UDSAL_DL_BUFF_SIZE >= 
				(u32BlockWriteCount + UDSAL_stDLControlBlock.u32AccumulateCount)) &&
				(0 != u32BlockWriteCount))
		{			
			pu8SourceData = (puint8)&UDSAL_stDLControlBlock.pstSourceBuffer->u8Data[0];
			pu8BufferData = (puint8)((uint32)UDSAL_stDLControlBlock.pu8TargetAddress + UDSAL_stDLControlBlock.u32AccumulateCount);
			
			memcpy((void*)pu8BufferData, (void*)pu8SourceData, u32BlockWriteCount);				
		
			UDSAL_stDLControlBlock.u32AccumulateCount += u32BlockWriteCount;
			
			if (UDSAL_stDLControlBlock.u32WriteCount == UDSAL_stDLControlBlock.u32AccumulateCount)
			{
				UDSAL_stDLControlBlock.boComplete = true;			
				UDSAL_stDLControlBlock.boPending = false;
			}
			else if (UDSAL_DL_BUFF_SIZE < UDSAL_stDLControlBlock.u32AccumulateCount)
			{
				/* Uh oh too many bytes received - ran past the executable section size */
				/* TODO matthew deal with this */
				UDSAL_stDLControlBlock.boPending = false;	
			}
		}
		else
		{
			UDSAL_stDLControlBlock.boPending = false;
		}
	}
	
	return UDSAL_stDLControlBlock.boPending;	
}
#endif

static uint8 UDSAL_u8TFEXREQ(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	uint8 u8Response = UDSAL_RSP_RSE;
	uint32 u32EntryAddress;
	uint16 u16EntryOffset;
#if (BUILD_PBL)	
	puint32 pu32TargetAddress;
#endif
	
	if (true == UDSAL_stDLControlBlock.boComplete)
	{
		u32EntryAddress = (uint32)(UDSAL_stDLControlBlock.pu8TargetAddress) & 0xffff0000;
		u16EntryOffset = 0x100 * pu8RXBuffer[5] + pu8RXBuffer[6];	
		u32EntryAddress += (uint32)u16EntryOffset;
#if (BUILD_PBL)
		pu32TargetAddress = (puint32)((uint32)UDSAL_stDLControlBlock.pu8TargetAddress & 0xffff0000);
		*pu32TargetAddress = 0;		
		PBL_vExecuteDL(u32EntryAddress);
#endif
		u8Response = UDSAL_RSP_OK;
	}	
	
	return u8Response;
}


