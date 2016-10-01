/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      DSCRIO Header File                                     */
/* DESCRIPTION:        This code provides macros for discrete I/O setup       */
/*                     and enum definitions                                   */	
/* FILE NAME:          DSCRIO.h                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef DSCRIO_H
#define DSCRIO_H

#include <stddef.h>
#include "CPUAbstract.h"
#include "declarations.h"
#include "DSCRIOHA.h"
#include "OS.h"
#include "regset.h"
#include "types.h"


typedef struct
{
	tstGPIOModule* pstGPIO;
	uint32 u32Bit;
} DSCRIO_tstIOFunctionPortConfig;


typedef enum
{
	enCOMSRESET,
	enSYSTELLTALE,
	enFIFOPRS,
	enWIFIMS,
	enIOFunctionCount
} DSCRIO_tenIOFunction;

typedef enum
{
	enLow,
	enHigh,
	enToggle,
	enHighZ
} DSCRIO_tenLogicLevel;

void DSCRIO_vStart(uint32* const);
void DSCRIO_vRun(uint32* const);
void DSCRIO_vTerminate(uint32* const);
void DSCRIO_vSetDiscrete(DSCRIO_tenIOFunction, DSCRIO_tenLogicLevel);
void DSCRIOHA_vSetDiscrete(DSCRIO_tenIOFunction, DSCRIO_tenLogicLevel);

#endif //DSCRIO_H

