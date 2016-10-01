/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      REGSET Header File                                     */
/* DESCRIPTION:        This code is definitions for register setup structs    */
/*                                                                            */
/* FILE NAME:          REGSET.h                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef REGSET_H
#define REGSET_H

#include "OS.h"
#include "types.h"

typedef enum
{	
	REGSET_enOverwrite,
	REGSET_enAnd,
	REGSET_enOr,
	REGSET_enXor
} REGSET_tenRegWriteMode;
	
typedef struct 
{
	volatile uint32* reg;
	uint32 val;
	REGSET_tenRegWriteMode writeMode;
} REGSET_tstReg32Val;

typedef struct 
{
	volatile uint16* reg;
	uint16 val;
	REGSET_tenRegWriteMode writeMode;	
} REGSET_tstReg16Val;

typedef struct 
{
	volatile uint8* reg;
	uint8 val;
	REGSET_tenRegWriteMode writeMode;	
} REGSET_tstReg8Val;


void REGSET_vStart(uint32* const);
void REGSET_vRun(uint32* const);
void REGSET_vTerminate(uint32* const);
void REGSET_vInitReg32(const REGSET_tstReg32Val*);
void REGSET_vInitReg16(const REGSET_tstReg16Val*);
void REGSET_vInitReg8(const REGSET_tstReg8Val*);

#endif //REGSET_H

