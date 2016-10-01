/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      MATH                      															*/
/* DESCRIPTION:        This code provides math support for indexing etc				*/
/*										 																												*/	
/* FILE NAME:          MATH.c                                          				*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "MATH.h"

const uint32 MATH_rau32B[] =
{ 0xAAAAAAAA,
	0xCCCCCCCC,
	0xF0F0F0F0,
	0xFF00FF00,
	0xFFFF0000 };

const uint32 MATH_rauIDXToMask[] =
{
	0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,
	0x00000010,
	0x00000020,
	0x00000040,
	0x00000080,
	0x00000100,
	0x00000200,
	0x00000400,
	0x00000800,
	0x00001000,
	0x00002000,
	0x00004000,
	0x00008000,
	0x00010000,
  0x00020000,
	0x00040000,
	0x00080000,
	0x00100000,
  0x00200000,
	0x00400000,
	0x00800000,
	0x01000000,
  0x02000000,
	0x04000000,
	0x08000000,
	0x10000000,	
  0x20000000,
	0x40000000,
	0x80000000
};
		
uint32 MATH_u32MaskToIDX(uint32 u32Mask)
{
	uint32 u32R = (	u32Mask & MATH_rau32B[0]) != 0;
	
	u32R |= ((u32Mask & MATH_rau32B[4]) != 0) << 4;
	u32R |= ((u32Mask & MATH_rau32B[3]) != 0) << 3;
	u32R |= ((u32Mask & MATH_rau32B[2]) != 0) << 2;
	u32R |= ((u32Mask & MATH_rau32B[1]) != 0) << 1;	
	
	return u32R;
}

uint32 MATH_u32IDXToMask(uint32 u32IDX)
{
	return MATH_rauIDXToMask[u32IDX];
}


			
			
