/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      PIM Header File                                        */
/* DESCRIPTION:        Port Integration Module                                */
/* FILE NAME:          PIM.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef PIM_H
#define PIM_H

#include "types.h"
#include "PIMAPI.h"
#include "IOAPI.h"
									
void PIM_vStart(uint32* const);
void PIM_vRun(uint32* const);
void PIM_vTerminate(uint32* const);
void PIM_vInitPortBit(PIMAPI_tenPort, IOAPI_tenEHIOType, uint32);
void PIM_vAssertPortBit(PIMAPI_tenPort, uint32, IOAPI_tenTriState);
void PIM_vSetPortMux(PIMAPI_tenPort, IOAPI_tenEHIOType, uint32, uint32);
Bool PIM_boGetPortBitState(PIMAPI_tenPort, uint32);

#endif //PIM_H

