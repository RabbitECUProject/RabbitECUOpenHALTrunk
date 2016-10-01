/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      DSCRIO Hardware Adaption                               */
/* DESCRIPTION:        This code provides support for kernel managed resource */
/*                     discrete I/O	only (e.g. Ethernet controller)           */	
/* FILE NAME:          DSCRIOHA.c                                             */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "declarations.h"
#include "dscrio.h"
#include "CPUAbstract.h"

const REGSET_tstReg32Val DSCRIOHA_rastDSCRIOReg32Val[] = DSCRIOHA_nReg32Set;
const DSCRIO_tstIOFunctionPortConfig DSCRIO_stIOFunctionPortConfig[enIOFunctionCount]
		= DSCRIOHA_nIOFunctionConfig;

void DSCRIOHA_vStart(uint32* const u32Stat)
{
	REGSET_vInitReg32(&DSCRIOHA_rastDSCRIOReg32Val[0]);
	OS_xModuleStartOK(*u32Stat);
}

void DSCRIOHA_vRun(uint32* const u32Stat)
{
	
}

void DSCRIOHA_vTerminate(uint32* const u32Stat)
{

}
		
void DSCRIOHA_vSetDiscrete(DSCRIO_tenIOFunction enIOFunction, DSCRIO_tenLogicLevel enLogicLevel)
{
	switch (enLogicLevel)
	{
		case enLow:
#ifdef BUILD_MK60
		DSCRIO_stIOFunctionPortConfig[enIOFunction].pstGPIO->PCOR =
		DSCRIO_stIOFunctionPortConfig[enIOFunction].u32Bit;
#endif
		break;

		case enHigh:
#ifdef BUILD_MK60
		DSCRIO_stIOFunctionPortConfig[enIOFunction].pstGPIO->PSOR =
		DSCRIO_stIOFunctionPortConfig[enIOFunction].u32Bit;
#endif
		break;
		
		case enToggle:
#ifdef BUILD_MK60
		DSCRIO_stIOFunctionPortConfig[enIOFunction].pstGPIO->PTOR =
		DSCRIO_stIOFunctionPortConfig[enIOFunction].u32Bit;
#endif
		break;
		
		case enHighZ:
#ifdef BUILD_MK60
		DSCRIO_stIOFunctionPortConfig[enIOFunction].pstGPIO->PDDR &=
		~DSCRIO_stIOFunctionPortConfig[enIOFunction].u32Bit;
#endif
		break;
	}
	
	if(enHighZ != enLogicLevel)
	{
#ifdef BUILD_MK60
		DSCRIO_stIOFunctionPortConfig[enIOFunction].pstGPIO->PDDR |=
		DSCRIO_stIOFunctionPortConfig[enIOFunction].u32Bit;
#endif
	}
}
