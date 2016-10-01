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
#include "uart.h"
#include "regset.h"
#include "sys.h"
#include "IRQ.h"
#include "declarations.h"
#include "os.h"
#include "DLL.h"
#include "SIM.h"

const UART_tstUARTCB UARTHA_rastUARTCB[] = UARTHA_nDataCB;

static tstUARTModule* UARTHA_pstGetUART(IOAPI_tenEHIOResource);
static sint32 UARTHA_u32GetUARTIndex(IOAPI_tenEHIOResource);

void UARTHA_vStart(uint32* const u32Stat)
{
	OS_xModuleStartOK(*u32Stat);
}

void UARTHA_vRun(uint32* const u32Stat)
{

}

void UARTHA_vTerminate(uint32* const u32Stat)
{

}

SYSAPI_tenSVCResult UARTHA_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	REGSET_tstReg8Val astUARTReg8Val[4];
	uint16 u16BaudDiv;
	tstUARTModule* pstUART;
	sint32 i32IDX = UARTHA_u32GetUARTIndex(enEHIOResource);	
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enBadResource;
	
	if ((-1 != i32IDX) && (TRUE == DLL_boInitDLLChannel(enEHIOResource, pstPortConfigCB)))
	{
#ifdef BUILD_MK60
		/* turn on UARTn clock */
		SIM_vSetReg32(UARTHA_rastUARTCB[i32IDX].enSIMReg, UARTHA_rastUARTCB[i32IDX].u32SIMRegMask);		
	
		pstUART = UARTHA_rastUARTCB[i32IDX].UART;
		
		u16BaudDiv = (uint16)(UARTHA_rastUARTCB[i32IDX].u32SourceClockHz 
											/ (8u * pstPortConfigCB->u32BaudRateHz));
		
		astUARTReg8Val[0].reg = (volatile uint8*)((uint8*)UART + offsetof(UART_Type, BDH));
		astUARTReg8Val[0].val = (uint8)(u16BaudDiv / 0x100);
		astUARTReg8Val[0].writeMode =	REGSET_enOverwrite;
		
		astUARTReg8Val[1].reg = (volatile uint8*)((uint8*)UART + offsetof(UART_Type, BDL));
		astUARTReg8Val[1].val = (uint8)(u16BaudDiv % 0x100);
		astUARTReg8Val[1].writeMode =	REGSET_enOverwrite;

		astUARTReg8Val[2].reg = (volatile uint8*)((uint8*)UART + offsetof(UART_Type, C2));
		astUARTReg8Val[2].val = (uint8)(UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK);
		astUARTReg8Val[2].writeMode =	REGSET_enOverwrite;
		
		astUARTReg8Val[3].reg = NULL;
		
		REGSET_vInitReg8(&astUARTReg8Val[0]);		
		
		IRQ_vEnableIRQ(UARTHA_rastUARTCB[i32IDX].IRQn);
		
		enSVCResult = SYSAPI_enOK;
#endif //BUILD_MK60
	}
	
	return enSVCResult;
}

void UARTHA_vSendChar(IOAPI_tenEHIOResource enEHIOResource, uint8 ucData)
{
	tstUARTModule* pstUART  = UARTHA_pstGetUART(enEHIOResource);
	
	if (NULL != pstUART)
	{
#ifdef BUILD_MK60
		pstUART -> D = ucData;	
		pstUART -> C2 |= UART_C2_TIE_MASK;			/* Set UART TIE bit  */	
#endif
	}
}

uint8 UARTHA_u8GetChar(IOAPI_tenEHIOResource enEHIOResource)
{
	tstUARTModule* pstUART  = UARTHA_pstGetUART(enEHIOResource);

	if (NULL != pstUART)
	{
#ifdef BUILD_MK60
		return pstUART -> D;
#endif
	}
	
	return 0;
}

static tstUARTModule* UARTHA_pstGetUART(IOAPI_tenEHIOResource enEHIOResource)
{
	tstUARTModule* pstUART;
	
	switch(enEHIOResource)
	{
#ifdef BUILD_MK60
		case EH_VIO_UART1:
			pstUART = UART0;			
			break;
		
		case EH_VIO_UART2:
			pstUART = UART1;			
			break;

		case EH_VIO_UART3:
			pstUART = UART2;			
			break;

		case EH_VIO_UART4:
			pstUART = UART3;			
			break;

		case EH_VIO_UART5:
			pstUART = UART4;			
			break;
		
		case EH_VIO_UART6:
			pstUART = UART5;			
			break;
		
		default:
			pstUART = NULL;
			break;
#endif //BUILD_MK60
	}
	
	return pstUART;
}

static sint32 UARTHA_u32GetUARTIndex(IOAPI_tenEHIOResource enEHIOResource)
{
    return 0;
}


void UARTHA_vDisableTXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	tstUARTModule* pstUART = UARTHA_pstGetUART(enEHIOResource);
	
#ifdef BUILD_MK60
	pstUART -> C2 &= ~UART_C2_TIE_MASK;			/* Clear UART TIE bit  */
#endif		
}

void UARTHA_vEnableTXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	tstUARTModule* pstUART = UARTHA_pstGetUART(enEHIOResource);
	
#ifdef BUILD_MK60
	pstUART -> C2 &= UART_C2_TIE_MASK;			/* Set UART TIE bit  */	
#endif
}

void UARTHA_vDisableRXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	tstUARTModule* pstUART = UARTHA_pstGetUART(enEHIOResource);
	
#ifdef BUILD_MK60
	pstUART -> C2 &= ~UART_C2_RIE_MASK;			/* Clear UART RIE bit  */	
#endif
}

void UARTHA_vEnableRXInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	tstUARTModule* pstUART = UARTHA_pstGetUART(enEHIOResource);
	
#ifdef BUILD_MK60
	pstUART -> C2 &= UART_C2_RIE_MASK;			/* Set UART RIE bit  */	
#endif
}



			
			
