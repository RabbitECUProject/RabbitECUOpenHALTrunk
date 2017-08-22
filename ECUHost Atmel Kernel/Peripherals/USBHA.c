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
#include "types.h"
#include "CPUAbstract.h"
#include "perusb.h"
#include "regset.h"
#include "sys.h"
#include "IRQ.h"
#include "declarations.h"
#include "os.h"
#include "DLL.h"
#include "SIM.h"
#include "uotghs_device.h"
#include "udc.h"
#include "udi_cdc.h"


Bool my_flag_authorise_cdc_transfer = false;
uint8 USBHA_u8Port;


void USBHA_vStart(uint32* const u32Stat)
{
	OS_xModuleStartOK(*u32Stat);
}

void USBHA_vRun(uint32* const u32Stat)
{

}

void USBHA_vTerminate(uint32* const u32Stat)
{

}

SYSAPI_tenSVCResult USBHA_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	SYSAPI_tenSVCResult enSVCResult;
	Bool boRetVal;

#ifdef BUILD_MK60
	tstUSBModule* pstUSB;
	
#endif //BUILD_MK60	

#ifdef BUILD_SAM3X8E
    uint8_t bmAttributes;
    udc_start();	

	bmAttributes = USB_EP_TYPE_CONTROL;
	udd_ep_alloc(0, bmAttributes, 512u);

	boRetVal = DLL_boInitDLLChannel(enEHIOResource, pstPortConfigCB);
#endif //BUILD_SAM3X8E

	enSVCResult = TRUE == boRetVal ? SYSAPI_enOK : SYSAPI_enBadResource;
	return enSVCResult;
}

bool my_callback_cdc_enable(void)
{
	my_flag_authorise_cdc_transfer = true;
	return true;
}
void my_callback_cdc_disable(void)
{
	my_flag_authorise_cdc_transfer = false;
}

void my_callback_rx_notify(uint8_t port)
{
    DLL_tstRXDLLData* pstRXDLLData = DLL_pstGetRXBuffer(EH_VIO_USB);
	puint8 pu8RXData = pstRXDLLData->u8Data;

	USBHA_u8Port = port;

    uint32 u32BytesRXCount = udi_cdc_get_nb_received_data();
	pstRXDLLData->u8DataCount = u32BytesRXCount;

    udi_cdc_read_buf((void*)pu8RXData, u32BytesRXCount);
	DLL_vFrameRXCB(EH_VIO_USB, pu8RXData);
}

void my_callback_tx_empty_notify(uint8_t port)
{

}
void my_callback_config(uint8_t port, usb_cdc_line_coding_t * cfg)
{

}

void my_callback_cdc_set_dtr(uint8_t port, bool b_enable)
{

}

void my_callback_cdc_set_rts(uint8_t port, bool b_enable)
{

}


void USBHA_vSend(IOAPI_tenEHIOResource enEHIOResource, puint8 pu8Data, uint32 u32TXByteCount)
{
	if (my_flag_authorise_cdc_transfer) 
	{
	    udi_cdc_write_buf((const void*)pu8Data, u32TXByteCount);
	}
}

puint8 USBHA_pu8Receive(IOAPI_tenEHIOResource enEHIOResource)
{

}








			
			
