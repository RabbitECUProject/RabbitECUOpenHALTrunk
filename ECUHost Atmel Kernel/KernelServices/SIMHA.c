/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      SIM Hardware Adaption                                  */
/* DESCRIPTION:        This code is provides functions for clock register     */
/*                     setting in future will support low power clocking      */
/*                     for power modes and shutdown etc                       */
/* FILE NAME:          SIMHA.c                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define SIMHA_C

#include "CPUAbstract.h"
#include "SIM.h"


void SIMHA_vSetRegAddress(SIMHA_tenReg enReg, REGSET_tstReg32Val* pstReg32Val)
{
#ifdef BUILD_MK60
	switch(enReg)
	{
		case SIM_SCGC1:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SCGC1));
			break;
		
		case SIM_SCGC2:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SCGC2));
			break;
		
		case SIM_SCGC3:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SCGC3));
			break;
				
		case SIM_SCGC4:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SCGC4));
			break;
		
		case SIM_SCGC5:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SCGC5));
			break;

		case SIM_SCGC6:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SCGC6));
			break;
		
		case SIM_SCGC7:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SCGC7));
			break;
		
		case SIM_SOPT1:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SOPT1));
			break;
		
		case SIM_SOPT2:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SOPT2));
			break;
				
		case SIM_SOPT4:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SOPT4));
			break;
		
		case SIM_SOPT5:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SOPT5));
			break;			
		
		case SIM_SOPT6:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SOPT6));
			break;
		
		case SIM_SOPT7:
			pstReg32Val->reg = (volatile uint32*)(SIM_BASE + offsetof(SIM_Type, SOPT7));
			break;	
		
		default:
			break;
	}
#endif //BUILD_MK60
}

Bool SIMHA_boEnablePeripheralClock(IRQn_Type IRQn)
{
#ifdef BUILD_SAM3X8E
    uint32 u32RetVal;
	Bool boRetVal;

	u32RetVal = pmc_enable_periph_clk(IRQn);
	boRetVal = (0 == u32RetVal)  ? TRUE : FALSE;

	return boRetVal;
#endif //BUILD_SAM3X8E

    return FALSE;
}

		
			




