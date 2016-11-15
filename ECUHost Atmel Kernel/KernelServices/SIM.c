/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SIM                                                    */
/* DESCRIPTION:        This code is provides functions for clock register     */
/*                     setting in future will support low power clocking      */
/*                     for power modes and shutdown etc                       */	
/* FILE NAME:          SIM.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "CPUAbstract.h"
#include "OS.h"
#include "SIM.h"
#include "regset.h"

static void SIM_vSetRegAddress(SIMHA_tenReg, REGSET_tstReg32Val*);
REGSET_tstReg32Val SIM_astSIMReg32Val[2];

void SIM_vSetReg32(SIMHA_tenReg enReg, uint32 u32RegMask)
{
	SIM_vSetRegAddress(enReg, &SIM_astSIMReg32Val[0]);
	SIM_astSIMReg32Val[0].val = u32RegMask;
	SIM_astSIMReg32Val[0].writeMode = REGSET_enOr;
	SIM_astSIMReg32Val[1].reg = NULL;
	REGSET_vInitReg32(&SIM_astSIMReg32Val[0]);
}

void SIM_vClrReg32(SIMHA_tenReg enReg, uint32 u32RegMask)
{
	SIM_vSetRegAddress(enReg, &SIM_astSIMReg32Val[0]);
	SIM_astSIMReg32Val[0].val = ~u32RegMask;
	SIM_astSIMReg32Val[0].writeMode = REGSET_enAnd;
	SIM_astSIMReg32Val[1].reg = NULL;
	REGSET_vInitReg32(&SIM_astSIMReg32Val[0]);
}

Bool SIM_boEnablePeripheralClock(IRQn_Type IRQn)
{
    return SIMHA_boEnablePeripheralClock(IRQn);
}

static void SIM_vSetRegAddress(SIMHA_tenReg enReg, REGSET_tstReg32Val* pstReg32Val)
{
    SIMHA_vSetRegAddress(enReg, pstReg32Val);
}
		
			




