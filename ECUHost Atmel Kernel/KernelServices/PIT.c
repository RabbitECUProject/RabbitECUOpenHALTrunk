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

#include "IOAPI.h"
#include "regset.h"
#include "sys.h"
#include "declarations.h"
#include "os.h"
#include "PIT.h"
#include "IRQ.h"

extern void PIT3_Handler(void);

const REGSET_tstReg32Val PIT_rastPITReg32Val[] = PIT_nReg32Set;

void PIT_vStart(uint32* const u32Stat)
{
	REGSET_vInitReg32(&PIT_rastPITReg32Val[0]);
	
	//Activate_IRQ(IRQ_enIRQIDX_PIT4_RTI, IRQ_enICRSoftPrio15, PIT4_RTI_Handler);   /* Setup PIT3 interrupt */	
	
	OS_xModuleStartOK(*u32Stat);
}

void PIT_vRun(uint32* const u32Stat)
{
	
}

void PIT_vTerminate(uint32* const u32Stat)
{

}


			
			
