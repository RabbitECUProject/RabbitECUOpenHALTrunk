/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      UARTHA Header File                                     */
/* DESCRIPTION:        UART Hardware Adaption                                 */
/* FILE NAME:          UART.h                                                 */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef UARTHA_H
#define UARTHA_H

#ifdef BUILD_MK60
#include "mk60f12.h"
typedef UART_Type tstUARTModule;
#endif

#ifdef BUILD_SAM3X8E
#include "sam3x8e.h"
typedef Uart tstUARTModule;
#endif

#ifdef BUILD_MK60
#define UARTHA_nDataCB                                                          \
{                                                                               \
	{SYS_FREQ_CORE,	SIM_SCGC4,	SIM_SCGC4_UART0_MASK, UART0, UART0_RX_TX_IRQn},	\
	{SYS_FREQ_BUS,	SIM_SCGC4,	SIM_SCGC4_UART1_MASK, UART1, UART1_RX_TX_IRQn},	\
	{SYS_FREQ_BUS,	SIM_SCGC4,	SIM_SCGC4_UART2_MASK, UART2, UART2_RX_TX_IRQn},	\
	{SYS_FREQ_BUS,	SIM_SCGC4,	SIM_SCGC4_UART3_MASK, UART3, UART3_RX_TX_IRQn},	\
	{SYS_FREQ_BUS,	SIM_SCGC1,	SIM_SCGC1_UART4_MASK, UART4, UART4_RX_TX_IRQn},	\
	{SYS_FREQ_BUS,	SIM_SCGC1,	SIM_SCGC1_UART5_MASK, UART5, UART5_RX_TX_IRQn}	\
}
#endif

#ifdef BUILD_SAM3X8E
#define UARTHA_nDataCB                                                          \
{                                                                               \
	{0,	0, 0, 0, 0}	\
}
#endif //BUILD_SAM3X8E

void UARTHA_vStart(uint32* const);
void UARTHA_vRun(uint32* const);
void UARTHA_vTerminate(uint32* const);
SYSAPI_tenSVCResult UARTHA_enInitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
void UARTHA_vSendChar(IOAPI_tenEHIOResource, uint8);
uint8 UARTHA_u8GetChar(IOAPI_tenEHIOResource);
void UARTHA_vDisableTXInterrupt(IOAPI_tenEHIOResource);
void UARTHA_vEnableTXInterrupt(IOAPI_tenEHIOResource);
void UARTHA_vDisableRXInterrupt(IOAPI_tenEHIOResource);
void UARTHA_vEnableRXInterrupt(IOAPI_tenEHIOResource);


#endif //UARTHA_H

