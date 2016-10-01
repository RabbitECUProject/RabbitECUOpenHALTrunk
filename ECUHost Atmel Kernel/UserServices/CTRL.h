/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CTRL_H
#define CTRL_H

#include "sys.h"	
#include "CTRLAPI.h"

#define CTRL_nPIDCTRLCount	10u
#define CTRL_nPIDUserCount	10u

#define CTRL_xAddSigned32(x, y)			\
if ((0 < y) && (0 < x))							\
{																		\
	if ((INT32_MAX - x) > y)					\
	{																	\
		x += y;													\
	}																	\
	else															\
	{																	\
		x = INT32_MAX;									\
	}																	\
}																		\
else if ((0 > y) && (0 > x))				\
{																		\
	if ((x - INT32_MIN) > (~y + 1))		\
	{																	\
		x += y;													\
	}																	\
	else															\
	{																	\
		x = INT32_MIN;									\
	}																	\
}																		\
else																\
{																		\
	x += y;														\
}																		\

#define CTRL_xCalcTermResult(x, y)					\
i32Temp =																		\
(0 < y) ? (0x7FFFFFFF / (y)) : 0x7FFFFFFF;	\
if (0 <= x)																	\
{																						\
	i32ResultTerm = (i32Temp < x) ?						\
						INT32_MAX : x * y;							\
}																						\
else																				\
{																						\
	i32ResultTerm = (i32Temp < (~x + 1)) ?		\
						INT32_MIN : x * y;							\
}																						\
	
void CTRL_vStart(puint32 const);
void CTRL_vRun(puint32 const);
void CTRL_vTerminate(puint32 const);
CTRLAPI_ttPIDIDX CTRL_tRequestKernelPIDController(CTRLAPI_tstPIDCB* pstPIDCBReq);
void CTRL_vIteratePIDCB(CTRLAPI_tstPIDCB*);
void CTRL_vIteratePID(CTRLAPI_ttPIDIDX);

#endif //CTRL_H

