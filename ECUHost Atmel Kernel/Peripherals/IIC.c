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
#include "IIC.h"
#include "IICHA.h"
#include "SYSAPI.h"


void IIC_vStart(puint32 const pu32Stat)
{
	IICHA_vStart(pu32Stat);
}

void IIC_vRun(puint32 const pu32Stat)
{
	
}

void IIC_vTerminate(puint32 const pu32Stat)
{
	
}

SYSAPI_tenSVCResult IIC_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
    return IICHA_enInitBus(enEHIOResource, pstPortConfigCB);		
}

void IIC_vInitTransfer(IOAPI_tstTransferCB* pstTransferCB)
{
    IICHA_vInitTransfer(pstTransferCB);
}

void IIC_vInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	IICHA_vInterrupt(enEHIOResource);
}







			
			
