/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CRC16 Header Fil                                       */
/* DESCRIPTION:        This code provides CRC16 function definitions          */
/*                                                                            */
/* FILE NAME:          CRC16.h                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CRC16_H
#define CRC16_H

#include "CRC16HA.h"
#include "types.h"

void CRC16_vStart(uint32* const);
void CRC16_vRun(uint32* const);
void CRC16_vTerminate(uint32* const);
puint16 CRC16_pu16CalcCRC(uint16, puint8, uint16);
uint16 CRC16_u16CalcCRC(uint16, puint8, uint16);

#endif //CRC16_H



