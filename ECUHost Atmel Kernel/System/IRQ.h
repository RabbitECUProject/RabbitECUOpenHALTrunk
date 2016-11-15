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

#include "build.h"
#include "CANHA.h"
#include "CPUAbstract.h"
#include "declarations.h"
#include "dll.h"
#include "IOAPI.h"
#include "math.h"
#include "SYSAPI.h"
#include "UARTHA.h"

typedef void (*IRQRXCallBack)(IOAPI_tenEHIOResource, void*);
typedef void (*IRQTXCallBack)(IOAPI_tenEHIOResource, void*);

typedef enum
{
    IRQ_enPRIO_0,
    IRQ_enPRIO_1,
    IRQ_enPRIO_2,
    IRQ_enPRIO_3,
    IRQ_enPRIO_4,
    IRQ_enPRIO_5,
    IRQ_enPRIO_6,
    IRQ_enPRIO_7,
    IRQ_enPRIO_8,
    IRQ_enPRIO_9,
    IRQ_enPRIO_10,
    IRQ_enPRIO_11,
    IRQ_enPRIO_12,
    IRQ_enPRIO_13,
    IRQ_enPRIO_14,
    IRQ_enPRIO_15
} IRQ_tenPRIO;

void IRQ_vEnableIRQ(IRQn_Type, IRQ_tenPRIO, IRQRXCallBack, IRQTXCallBack);
void IRQ_vDisableIRQ(IRQn_Type);
void IRQ_vReset(void);

#endif //IRQ_H



