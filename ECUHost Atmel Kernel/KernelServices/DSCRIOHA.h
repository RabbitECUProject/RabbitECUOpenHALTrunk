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
#ifndef DSCRIOHA_H
#define DSCRIOHA_H

#if defined(BUILD_MK60)
#include "mk60f12.h"
typedef GPIO_Type tstGPIOModule;
#elif defined(BUILD_SAM3X8E)
#include "sam3x8e.h"
typedef Pio tstGPIOModule;
#endif

#ifdef BUILD_MK60
#define DSCRIOHA_nReg32Set                                                                                                  \
{                                                                                                                           \
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[6])), (uint32)PORT_PCR_MUX(1), REGSET_enOverwrite },          \
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[7])), (uint32)PORT_PCR_MUX(1), REGSET_enOverwrite },          \
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[8])), (uint32)PORT_PCR_MUX(1), REGSET_enOverwrite },          \
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[26])), (uint32)PORT_PCR_MUX(1), REGSET_enOverwrite },         \
	{ NULL, 0, REGSET_enOverwrite }																																															\
};
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define DSCRIOHA_nReg32Set             \
{                                      \
	{ NULL, 0, REGSET_enOverwrite }    \
};
#endif //BUILD_MK60

#ifdef BUILD_MK60
#define DSCRIOHA_nIOFunctionConfig   \
{                                    \
	{ PTE, 0x00000040 },             \
	{ PTE, 0x00000100 },             \
	{ PTE, 0x00000040 },             \
	{ PTE, 0x04000000 }              \
};
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define DSCRIOHA_nIOFunctionConfig   \
{                                    \
	{ NULL, 0x00000000 }             \
};
#endif //BUILD_SAM3X8E

void DSCRIOHA_vStart(uint32* const);
void DSCRIOHA_vRun(uint32* const);
void DSCRIOHA_vTerminate(uint32* const);

#endif //DSCRIOHA_H

