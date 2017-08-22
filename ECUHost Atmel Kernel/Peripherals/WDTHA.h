/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      WDTHA header file                                      */
/* DESCRIPTION:        This code provides watchdog functions abstraction      */
/*                     layer                                                  */
/* FILE NAME:          WDTHA.h                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef WDTHA_H
#define WDTHA_H

#include "types.h"
#include "declarations.h"
#include "wdt.h"

void WDTHA_vStart(uint16, uint16);
Bool WDTHA_boReset(uint16);

#endif //WDTHA_H



