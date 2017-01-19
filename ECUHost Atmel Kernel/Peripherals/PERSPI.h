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
#ifndef SPI_H
#define SPI_H

#include "IOAPI.h"
#include "SYSAPI.h"
#include "types.h"

void SPI_vStart(puint32 const);
void SPI_vRun(puint32 const);
void SPI_vTerminate(puint32 const);
uint32 SPI_u32InitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
void SPI_vInitTransfer(IOAPI_tstTransferCB*);
void SPI_vInterrupt(IOAPI_tenEHIOResource);

#endif //SPI_H

