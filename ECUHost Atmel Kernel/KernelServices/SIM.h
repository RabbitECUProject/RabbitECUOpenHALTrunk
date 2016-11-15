/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SIM Header File                                        */
/* DESCRIPTION:        This code is provides enum definitions                 */
/*                                                                            */
/* FILE NAME:          SIM.h                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SIM_H
#define SIM_H

#include "SIMHA.h"
#include "regset.h"
#include "sys.h"
#include "types.h"


void SIM_vSetReg32(SIMHA_tenReg, uint32);
void SIM_vClrReg32(SIMHA_tenReg, uint32);
Bool SIM_boEnablePeripheralClock(IRQn_Type);

#endif //SIM_H


