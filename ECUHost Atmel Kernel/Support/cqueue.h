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
#ifndef CQUEUE_H
#define CQUEUE_H

#include "sys.h"
#include "types.h"
#include "CPUAbstract.h"

typedef struct
{	
	uint32 u32Head;
	uint32 u32Tail;
	uint32 u32Size;
	void* pvData;
} CQUEUE_tstQueue;

#define CQUEUE_vInit(x,y,z)	\
x##.u32Head = 0;						\
x##.u32Tail = 0;						\
x##.u32Size = y;						\
x##.pvData = (void*)&##z;

#define CQUEUE_xInit(x,y,z)	\
(x)->u32Head = 0;						\
(x)->u32Tail = 0;						\
(x)->u32Size = (y);						\
(x)->pvData = (void*)(z);

#define	CQUEUE_boIsEmpty(x)	(x##.u32Head == x##.u32Tail) ? true : false

#define	CQUEUE_xIsEmpty(x)	((x)->u32Head == (x)->u32Tail ? true : false)

#define	CQUEUE_boIsStaticActive(x)	(x##.u32Tail != 0) ? true : false

#define	CQUEUE_xIsStaticActive(x)	((x)->u32Tail != 0 ? true : false)

#define	CQUEUE_boIsAtStaticHead(x)	(x##.u32Head == 0) ? true : false

#define	CQUEUE_xIsAtStaticHead(x)	((x)->u32Head == 0 ? true : false)

#define	CQUEUE_vResetStaticHead(x)	(x##.u32Head = 0)

#define	CQUEUE_xResetStaticHead(x)	(x)->u32Head = 0

#define CQUEUE_boIsFull(x)	(x##.u32Head == ((x##.u32Tail + 1) % x##.u32Size)) ? true : false

#define CQUEUE_xIsFull(x)	((x)->u32Head == ((x)->u32Tail + 1) % (x)->u32Size ? true : false)

#define CQUEUE_vAddItem(x)                             \
	CPU_xEnterCritical();                              \
	x##.u32Tail = (x##.u32Tail + 1) % x##.u32Size;     \
	CPU_xExitCritical()

#define CQUEUE_xAddItem(x)                           \
CPU_xEnterCritical();                                \
(x)->u32Tail = ((x)->u32Tail + 1) % (x)->u32Size;    \
CPU_xExitCritical()
	
#define CQUEUE_vRemoveItem(x)                           \
	CPU_xEnterCritical();                               \
	if (!CQUEUE_boIsEmpty(x))                           \
	{                                                   \
		x##.u32Head = (x##.u32Head + 1) % x##.u32Size;	\
	}                                                   \
	CPU_xExitCritical()

#define CQUEUE_xRemoveItem(x)                         \
CPU_xEnterCritical();                                 \
if (false == (CQUEUE_xIsEmpty(x)))                    \
{                                                     \
	(x)->u32Head = ((x)->u32Head + 1) % (x)->u32Size; \
}                                                     \
CPU_xExitCritical()

#define CQUEUE_xSetHead(x, y)                         \
CPU_xEnterCritical();                                 \
(x)->u32Head = y;                                 \
CPU_xExitCritical()

#define CQUEUE_vClearCount(x)	\
x##.u32Head = 0;							\
x##.u32Tail = 0

#define CQUEUE_xClearCount(x)	\
(x)->u32Head = 0;							\
(x)->u32Tail = 0
	
#endif //CQUEUE_H



