/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      User Math Header File                                  */
/* DESCRIPTION:        This code module provides mathematical data filtering  */
/*                     functions for the user application                     */
/* FILE NAME:          USERMATH.h                                             */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef USERMATH_H
#define USERMATH_H

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "declarations.h"
#include "sys.h"
#include "types.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define MIN(a, b)		((a) > (b)) ? (b) : (a)
#define MAX(a, b)		((a) < (b)) ? (b) : (a)
#define ABS(a)   		(((a) > 0) 	? (a) : (-a))


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
uint16 USERMATH_u32SinglePoleLowPassFilter(uint16 u16NewVal, uint8 u8FilterWeight, puint32 pu32FilterVal);/*CR1_1*/
uint32 USERMATH_u32DiscardAndAverage32(puint32 pu32Samples, uint32 u32SampleCount, uint32 u32DiscardCount);/*CR1_1*/;
sint32 USERMATH_s32DiscardAndAverage32(psint32 ps32Samples, uint32 u32SampleCount, uint32 u32DiscardCount);/*CR1_1*/

#endif // UNITS_H

