/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CAN Header File                                        */
/* DESCRIPTION:        This code provides functions for interacting with      */
/*                     the CAN hardware module/s                              */
/* FILE NAME:          CAN.h                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CAN_H
#define CAN_H

#include <stddef.h>
#include "CANHA.h"
#include "CPUAbstract.h"
#include "declarations.h"
#include "DLL.h"
#include "IOAPI.h"
#include "IRQ.h"
#include "MATH.h"
#include "CPUAbstract.h"
#include "os.h"
#include "protapi.h"
#include "regset.h"
#include "SIM.h"
#include "types.h"


void CAN_vStart(uint32* const);
void CAN_vRun(uint32* const);
void CAN_vTerminate(uint32* const);
void CAN_vInitTransfer(IOAPI_tstTransferCB*);
SYSAPI_tenSVCResult CAN_enInitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);

#endif //CAN_H

