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
#include "PERSPI.h"
#include "SPIHA.h"
#include "SYSAPI.h"


void SPI_vStart(puint32 const pu32Stat)
{
	SPIHA_vStart(pu32Stat);
}

void SPI_vRun(puint32 const pu32Stat)
{
	
}

void SPI_vTerminate(puint32 const pu32Stat)
{
	
}

uint32 SPI_u32InitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
    return SPIHA_u32InitBus(enEHIOResource, pstPortConfigCB);		
}

void SPI_vInitTransfer(IOAPI_tstTransferCB* pstTransferCB)
{
    SPIHA_vInitTransfer(pstTransferCB);
}

void SPI_vInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	SPIHA_vInterruptHandler(enEHIOResource);
}







			
			
