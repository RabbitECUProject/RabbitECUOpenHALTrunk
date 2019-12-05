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
#ifndef DLL_H
#define DLL_H

#include "types.h"
#include "OS.h"
#include "IOAPI.h"

#define RX_BUFF_SIZE 							192u
#define TX_BUFF_SIZE 							32u
#define RX_SEG_FRAME_UNSEG 				0x00u
#define RX_SEG_FRAME_ONE 					0x10u
#define RX_SEG_FRAME_SEQ 					0x20u
#define TX_BUFF_COMPLETE					0xFFu
#define RX_BUFF_FULL							0xFFu
#define DLL_nTXIPBufferSize				1024u

#define	RX_BUFF_SMALL_SIZE	21u

/* Warning do NOT make the working buffer
   sizes LESS than the max frame bytes!! */
#define DLL_nIICTXWorkBuffMaxBytes		16u
#define DLL_nSPITXWorkBuffMaxBytes		16u
#define DLL_nUARTTXWorkBuffMaxBytes		24u
#define DLL_nCANTXWorkBuffMaxBytes		256u
#define DLL_nUSBTXWorkBuffMaxBytes      256u
#define DLL_nENETTXWorkBuffMaxBytes		1024u

#define DLL_nIICTXWorkBuffCount				2u
#define DLL_nSPITXWorkBuffCount				2u
#define DLL_nUARTTXWorkBuffCount			2u
#define DLL_nCANTXWorkBuffCount				2u
#define DLL_nUSBTXWorkBuffCount             1u
#define DLL_nENETTXWorkBuffCount			1u
#define DLL_nClientTXBufferCount	    (DLL_nIICTXWorkBuffCount +		\
																			DLL_nSPITXWorkBuffCount +			\
																			DLL_nUARTTXWorkBuffCount +		\
																			DLL_nCANTXWorkBuffCount +			\
																			DLL_nUSBTXWorkBuffCount +			\
																			DLL_nENETTXWorkBuffCount)

#define DLL_nIICVirtualChannelCount		(EH_LAST_IIC - EH_FIRST_IIC + 1)
#define DLL_nSPIVirtualChannelCount		(EH_LAST_SPI - EH_FIRST_SPI + 1)
#define DLL_nUARTVirtualChannelCount	(EH_LAST_UART - EH_FIRST_UART + 1)
#define DLL_nCANVirtualChannelCount		(EH_LAST_CAN - EH_FIRST_CAN + 1)
#define DLL_nUSBVirtualChannelCount		(EH_LAST_USB - EH_FIRST_USB + 1)
#define DLL_nENETVirtualChannelCount	(EH_ENET_LAST_CH - EH_ENET_FIRST_CH + 1)

#define DLL_xVirtualChannelIndex(x)     (x - EH_FIRST_IIC)

#define DLL_nIICTXQueueByteCount			(DLL_nIICTXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nSPITXQueueByteCount			(DLL_nSPITXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nUARTTXQueueByteCount			(DLL_nUARTTXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nCANTXQueueByteCount			(DLL_nCANTXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nUSBTXQueueByteCount			(DLL_nUSBTXWorkBuffMaxBytes * 2u) + 1u
#define DLL_nENETTXQueueByteCount			(DLL_nENETTXWorkBuffMaxBytes * 2u) + 1u

/* WARNING - Tailored on virtual channel enum! */
#define DLL_nMaxTXFrameBytes	\
{														\
	16,													\
	16,													\
	16,													\
	16,													\
	24,													\
	24,													\
	12,													\
	12,													\
	256,                                                \
	1024,												\
	1024												\
}

#ifdef BUILD_PBL
	#define DLL_xGetClientHandle()	0xf0000000
#endif
#ifdef BUILD_SBL
	#define DLL_xGetClientHandle()	0xf0000000
#endif
#ifdef BUILD_KERNEL
	#define DLL_xGetClientHandle()	OS_stSVCDataStruct.tClientHandle | 0xf0000000
#endif
#ifdef BUILD_KERNEL_APP
	#define DLL_xGetClientHandle()	OS_stSVCDataStruct.tClientHandle | 0xf0000000
#endif

#if RX_BUFF_SIZE > 256u
	#warning "RX buffer size"
#endif

#if TX_BUFF_SIZE > 256u
	#warning "TX buffer size"
#endif

#define DLL_nVirtualChannelCount (EH_ENET_LAST_CH - EH_FIRST_IIC)
#define DLL_nTXBytesBufferSize	25u

typedef void (*pfCallBackDLLToModule)(uint8*);
typedef sint32 DLL_tDLLVirtualChannel;

typedef struct
{
	uint8 u8Data[RX_BUFF_SIZE];
	uint8 u8DataCount;
	uint8 u8RXByteIDX;
	uint8 biFramed : 1;
	uint8 biWithCRC : 1;
	uint8 biRXFull : 1;
	uint8 biSpare : 5;
} DLL_tstRXDLLData;

typedef struct
{
	puint8 pu8TXData;
	uint8 u8ByteCount;
} DLL_tstTXCB;

typedef struct
{
	uint8 u8Data[DLL_nTXBytesBufferSize];
	uint8 u8DataCount;
	uint8 u8TXByteIDX;
	uint8 biFramed : 1;
	uint8 biWithCRC : 1;
	uint8 biTXEmpty : 1;
	uint8 biSpare : 5;	
} DLL_tstTXBytesDLLData;

typedef struct
{
	uint8 u8Data[RX_BUFF_SMALL_SIZE];
	uint8* pu8RX;
	uint8 u8RXRequestedCount;
	uint8 u8RXReceivedCount;
} DLL_tstRXSmallBuffer;

void DLL_vFrameRXCB(IOAPI_tenEHIOResource, puint8);
void DLL_vStart(puint32 const);
void DLL_vRun(puint32 const);
void DLL_vTerminate(puint32 const);
void DLL_vReceiveUARTBytes(IOAPI_tenEHIOResource, DLL_tstRXDLLData*);
Bool DLL_vQueueCANMessage(IOAPI_tenEHIOResource, PROTAPI_tstCANMsg*);	
Bool DLL_boQueueMessage(IOAPI_tenEHIOResource, puint8, uint32);
Bool DLL_boCheckCRC(DLL_tstRXDLLData* pRXFrame);
DLL_tDLLVirtualChannel DLL_tGetVirtualChannel(IOAPI_tenEHIOResource);
Bool DLL_boInitDLLChannel(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
IOAPI_tenPortMode DLL_enGetChannelMode(IOAPI_tenEHIOResource);
DLL_tstRXDLLData* DLL_pstGetRXBuffer(IOAPI_tenEHIOResource);
void* DLL_pvGetBuffered(IOAPI_tenEHIOResource);

#endif //DLL_H

