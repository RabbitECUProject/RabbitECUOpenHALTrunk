/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CRC16 Header File                                      */
/* DESCRIPTION:        This code provides CRC16 function definitions          */
/*                                                                            */
/* FILE NAME:          CRC16.h                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CRC16HA_H
#define CRC16HA_H

#include "CPUAbstract.h"
#include "SIM.h"

#include "build.h"
#ifdef BUILD_MK60
#include "mk60f12.h"
typedef CRC_Type tstCRCModule;
#endif

void CRC16HA_vStart(uint32* const);
void CRC16HA_vRun(uint32* const);
void CRC16HA_vTerminate(uint32* const);
puint16 CRC16HA_pu16CalcCRC(uint16, puint8, uint16);
uint16 CRC16HA_u16CalcCRC(uint16, puint8, uint16);

#endif //CRC16HA_H



