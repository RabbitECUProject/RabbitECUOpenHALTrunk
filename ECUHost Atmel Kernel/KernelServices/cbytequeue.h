/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      XXX Header File                    										*/
/* DESCRIPTION:        XXX   																									*/
/* FILE NAME:          XXX.h                                         					*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "sys.h"
#include "types.h"


#ifndef CBYTEQUEUE_H
#define CBYTEQUEUE_H

typedef struct
{	
	uint32 u32Head;
	uint32 u32Tail;
	uint32 u32Size;
	puint8 pu8Data;
} CBYTEQUEUE_tstQueue;


#endif //CBYTEQUEUE_H



