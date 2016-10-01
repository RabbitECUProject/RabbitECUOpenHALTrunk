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
#ifndef IIC_H
#define IIC_H

#include "IOAPI.h"
#include "SYSAPI.h"
#include "types.h"

void IIC_vStart(puint32 const);
void IIC_vRun(puint32 const);
void IIC_vTerminate(puint32 const);
SYSAPI_tenSVCResult IIC_enInitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
void IIC_vInitTransfer(IOAPI_tstTransferCB*);
void IIC_vInterrupt(IOAPI_tenEHIOResource);

#endif //IIC_H

