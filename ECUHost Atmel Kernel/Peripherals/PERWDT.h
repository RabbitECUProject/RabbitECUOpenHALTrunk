/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      WDT header file                                        */
/* DESCRIPTION:        This code provides wathdog functions                   */
/*                                                                            */
/* FILE NAME:          PERWDT.h                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef PERWDT_H
#define PERWDT_H

#include "types.h"
#include "WDTHA.h"

void WDT_vStart(uint16, uint16);
Bool WDT_boReset(uint16);

#endif //PERWDT_H




