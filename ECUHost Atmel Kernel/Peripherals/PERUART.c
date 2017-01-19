/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include "CPUAbstract.h"
#include "peruart.h"

#include "regset.h"
#include "sys.h"
#include "IRQ.h"
#include "declarations.h"
#include "os.h"
#include "DLL.h"
#include "types.h"
#include "SIM.h"


void UART_vStart(uint32* const u32Stat)
{
	OS_xModuleStartOK(*u32Stat);
}

void UART_vRun(uint32* const u32Stat)
{

}

void UART_vTerminate(uint32* const u32Stat)
{

}

SYSAPI_tenSVCResult UART_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{	
	SYSAPI_tenSVCResult enSVCResult;
	
	enSVCResult = UARTHA_enInitBus(enEHIOResource, pstPortConfigCB);
	
	return enSVCResult;
}

void UART_vSendChar(IOAPI_tenEHIOResource enEHIOResource, uint8 ucData)
{
	UARTHA_vSendChar(enEHIOResource, ucData);
}

uint8 UART_u8GetChar(IOAPI_tenEHIOResource enEHIOResource)
{
	return UARTHA_u8GetChar(enEHIOResource);	
}

void UART_vDisableTXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	UARTHA_vDisableTXInterrupt(enEHIOResource);
}

void UART_vEnableTXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	UARTHA_vEnableTXInterrupt(enEHIOResource);
}

void UART_vDisableRXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	UARTHA_vDisableRXInterrupt(enEHIOResource);
}

void UART_vEnableRXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	UART_vEnableRXInterrupt(enEHIOResource);
}


			
			
