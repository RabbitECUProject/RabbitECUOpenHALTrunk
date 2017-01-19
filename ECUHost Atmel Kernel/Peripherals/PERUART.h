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
#ifndef UART_H
#define UART_H

#include <stddef.h>
#include "CPUAbstract.h"
#include "DLL.h"
#include "IOAPI.h"
#include "SIMHA.h"
#include "sys.h"
#include "types.h"
#include "UARTHA.h"


typedef struct
{
	uint32 u32SourceClockHz;
	SIMHA_tenReg enSIMReg;
	uint32 u32SIMRegMask;
	tstUARTModule* pstUART;
	IRQn_Type IRQn;
} UART_tstUARTCB;

void UART_vStart(uint32* const);
void UART_vRun(uint32* const);
void UART_vTerminate(uint32* const);
SYSAPI_tenSVCResult UART_enInitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
void UART_vSendChar(IOAPI_tenEHIOResource, uint8);
uint8 UART_u8GetChar(IOAPI_tenEHIOResource);
void UART_vDisableTXInterrupt(IOAPI_tenEHIOResource);
void UART_vEnableTXInterrupt(IOAPI_tenEHIOResource);
void UART_vDisableRXInterrupt(IOAPI_tenEHIOResource);
void UART_vEnableRXInterrupt(IOAPI_tenEHIOResource);

#endif //UART_H

