/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SRLTFR                                                 */
/* DESCRIPTION:        Serial Transfer file                                   */
/* FILE NAME:          SRLTFR.c                                               */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _SRLTFR_C
 
#include <string.h>
#include "IIC.h"
#include "IOAPI.h"
#include "PERSPI.h"
#include "SYSAPI.h"
#include "types.h"
#include "CQUEUE.h"
#include "SRLTFR.h"
#include "declarations.h"

IOAPI_tenEHIOResource SRLTFR_enActiveEHIOResource;

void SRLTFR_vStart(puint32 const pu32Arg)
{
	CQUEUE_xInit(&stTransferQueue, SRLTFR_nTransfersMax, astTransferInfo);	
	SRLTFR_enActiveEHIOResource = EH_IO_Invalid;
}

void SRLTFR_vRun(puint32 const pu32Arg)
{
	if (!CQUEUE_xIsEmpty(&stTransferQueue))
	{
		switch (astTransferInfo[stTransferQueue.u32Head].enEHIOResource)
		{
			case EH_VIO_IIC1:
			case EH_VIO_IIC2:	
			{
				IIC_vInitTransfer(&astTransferInfo[stTransferQueue.u32Head]);
				SRLTFR_enActiveEHIOResource = astTransferInfo[stTransferQueue.u32Head].enEHIOResource;
			}
			case EH_VIO_SPI1:
			case EH_VIO_SPI2:
			{
				SPI_vInitTransfer(&astTransferInfo[stTransferQueue.u32Head]);
				SRLTFR_enActiveEHIOResource = astTransferInfo[stTransferQueue.u32Head].enEHIOResource;
			}
			default:
			{

			}	
		}
	}
}

void SRLTFR_vTerminate(puint32 const pu32Arg)
{
	
}

SYSAPI_tenSVCResult SRLTFR_enEnqueue(IOAPI_tstTransferCB* pstTransferCB)
{
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enQueueFull;
	uint32 u32Flag;
	

	if (!CQUEUE_xIsFull(&stTransferQueue))
	{
		memcpy((void*)&astTransferInfo[stTransferQueue.u32Head],
						 (void*)pstTransferCB,
						  sizeof(IOAPI_tstTransferCB));	

		enSVCResult = SYSAPI_enOK;
		CQUEUE_xAddItem(&stTransferQueue);

		if (TRUE == pstTransferCB->boBlockingMode)
		{
			SRLTFR_vRun(&u32Flag);
			enSVCResult = SYSAPI_enOK;

			/* Clear the queue because it was all sent in blocking mode */
			CQUEUE_xRemoveItem(&stTransferQueue);
		}
	}
	
	
	return enSVCResult;
}

void SRLTFR_vNotifyCB(IOAPI_tenEHIOResource enEHIOResource)
{
	IOAPI_enTransferType enTransferType;
	IOAPI_tpfTransferCB pfCB;
	
	if (SRLTFR_enActiveEHIOResource == enEHIOResource)
	{
		enTransferType = astTransferInfo[stTransferQueue.u32Head].enTransferType;
		pfCB = astTransferInfo[stTransferQueue.u32Head].pfCB;
		
		if ((NULL != pfCB) && (IOAPI_IO_TX == enTransferType))
		{
			(*pfCB)(enEHIOResource, 
						astTransferInfo[stTransferQueue.u32Head].pvData, 
						astTransferInfo[stTransferQueue.u32Head].u32ByteCount,
						astTransferInfo[stTransferQueue.u32Head].enTransferType);
		}
		CQUEUE_xRemoveItem(&stTransferQueue);
	}
}
			
			
