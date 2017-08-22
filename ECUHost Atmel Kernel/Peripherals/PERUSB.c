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
#include "perusb.h"

#include "regset.h"
#include "sys.h"
#include "IRQ.h"
#include "declarations.h"
#include "os.h"
#include "DLL.h"
#include "types.h"
#include "SIM.h"


void USB_vStart(uint32* const u32Stat)
{
	OS_xModuleStartOK(*u32Stat);
}

void USB_vRun(uint32* const u32Stat)
{

}

void USB_vTerminate(uint32* const u32Stat)
{

}

SYSAPI_tenSVCResult USB_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{	
	SYSAPI_tenSVCResult enSVCResult;
	
	enSVCResult = USBHA_enInitBus(enEHIOResource, pstPortConfigCB);
	
	return enSVCResult;
}

void USB_vSend(IOAPI_tenEHIOResource enEHIOResource, uint8* pu8Data, uint32 u32TXByteCount)
{
	USBHA_vSend(enEHIOResource, pu8Data, u32TXByteCount);
}

uint8 USB_u8Receive(IOAPI_tenEHIOResource enEHIOResource)
{
	return USBHA_pu8Receive(enEHIOResource);	
}



			
			
