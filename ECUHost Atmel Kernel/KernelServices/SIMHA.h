/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SIM Hardware Adaption Header File                      */
/* DESCRIPTION:        This code is provides enum definitions                 */
/*                                                                            */
/* FILE NAME:          SIMHA.h                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SIMHA_H
#define SIMHA_H

#include "regset.h"

#ifdef BUILD_MK60
typedef enum
{
	SIM_SCGC1,
	SIM_SCGC2,
	SIM_SCGC3,
	SIM_SCGC4,
	SIM_SCGC5,
	SIM_SCGC6,
	SIM_SCGC7,
	SIM_SOPT1,
	SIM_SOPT2,
	SIM_SOPT4,
	SIM_SOPT5,
	SIM_SOPT6,
	SIM_SOPT7	
} SIMHA_tenReg;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
typedef enum
{
	SIM_TBC0,
	SIM_TBC1
} SIMHA_tenReg;
#endif //BUILD_SAM3X8E

void SIMHA_vSetRegAddress(SIMHA_tenReg, REGSET_tstReg32Val*);

#endif //SIMHA_H


