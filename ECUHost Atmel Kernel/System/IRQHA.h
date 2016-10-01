/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      IRQ Header File                     										*/
/* DESCRIPTION:        This code is interrupt common function definitions			*/
/*										 																												*/
/* FILE NAME:          IRQ.h                                          				*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "sys.h"
#ifdef KERNEL

#ifndef IRQ_H
#define IRQ_H

#include "ADC.h"
#include "build.h"
#include "CANHA.h"
#include "CLIENT.h"
#include "declarations.h"
#include "dll.h"
#include "ENE.h"
#include "IOAPI.h"
#include "IIC.h"
#include "math.h"
#include "MSG.h"
#include "OS.h"
#include "SYSAPI.h"
#include "TEPM.h"
#include "timer.h"
#include "tqueue.h"
#include "UART.h"
//#include "UDSAL.h"


void IRQ_vEnableIRQ(IRQn_Type);
void IRQ_vDisableIRQ(IRQn_Type);
void IRQ_vCommonUART(tstUARTModule*, IOAPI_tenEHIOResource);
void IRQ_vCommonCAN(CAN_Type*, IOAPI_tenEHIOResource);

#endif // IRQ_H

#endif // KERNEL

