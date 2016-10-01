/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      IRQ Header File                                        */
/* DESCRIPTION:        This code is interrupt common function definitions     */
/*                                                                            */
/* FILE NAME:          IRQ.h                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef IRQ_H
#define IRQ_H

#include "CANHA.h"
#include "CPUAbstract.h"
#include "declarations.h"
#include "dll.h"
#include "IOAPI.h"
#include "math.h"
//#include "MSG.h"
//#include "OS.h"
#include "SYSAPI.h"
#include "UARTHA.h"

typedef void (*IRQRXCallBack)(IOAPI_tenEHIOResource, void*);
typedef void (*IRQTXCallBack)(IOAPI_tenEHIOResource, void*);

void IRQ_vEnableIRQ(IRQn_Type, IRQRXCallBack, IRQTXCallBack);
void IRQ_vDisableIRQ(IRQn_Type);
void IRQ_vReset(void);

#endif //IRQ_H



