/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef PERUSB_H
#define PERUSB_H

#include <stddef.h>
#include "CPUAbstract.h"
#include "DLL.h"
#include "IOAPI.h"
#include "SIMHA.h"
#include "sys.h"
#include "types.h"
#include "USBHA.h"


typedef struct
{
	uint32 u32SourceClockHz;
	SIMHA_tenReg enSIMReg;
	uint32 u32SIMRegMask;
	IRQn_Type IRQn;
} USB_tstUSBCB;

void USB_vStart(uint32* const);
void USB_vRun(uint32* const);
void USB_vTerminate(uint32* const);
SYSAPI_tenSVCResult USB_enInitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
void USB_vSend(IOAPI_tenEHIOResource, puint8, uint32);
puint8 USB_pu8Receive(IOAPI_tenEHIOResource);


#endif //PERUSB_H

