/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      USBHA Header File                                      */
/* DESCRIPTION:        USB Hardware Adaption                                  */
/* FILE NAME:          USB.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef USBHA_H
#define USBHA_H

#ifdef BUILD_MK60
#include "mk60f12.h"
typedef USB_Type tstUARTModule;
#endif

#ifdef BUILD_SAM3X8E
//#include "sam3x8e.h"
typedef struct Uart tstUARTModule;
#endif

#ifdef BUILD_MK60
#define USBHA_nDataCB                                                          \
{                                                                               \
}
#endif

#ifdef BUILD_SAM3X8E
#define UARTHA_nDataCB                                                          \
{                                                                               \
	{0,	0, 0, 0, 0}	\
}
#endif //BUILD_SAM3X8E

void USBHA_vStart(uint32* const);
void USBHA_vRun(uint32* const);
void USBHA_vTerminate(uint32* const);
SYSAPI_tenSVCResult USBHA_enInitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
void USBHA_vSend(IOAPI_tenEHIOResource, puint8, uint32);
puint8 USBHA_pu8Receive(IOAPI_tenEHIOResource);



#endif //USBHA_H

