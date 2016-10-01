/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      TIMER Header File                                      */
/* DESCRIPTION:        This code provides enum definitions for kernel timers  */
/*                                                                            */	
/* FILE NAME:          TIMER.h                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TIMER_H
#define TIMER_H

#include <stddef.h>
#include "sys.h"
#include "types.h"

#define TIMER_nTimerCount	10u

typedef void ( *tpfTimerCallBack )( void );

typedef enum
{
	enReset,
	enPending,
	enExpired
} TIMER_tenState;

typedef struct
{
	TIMER_tenState enState;
	uint32 u32AlarmPoint;
	tpfTimerCallBack pfCallBack;
} TIMER_tstTimer;

void TIMER_vStart( uint32* const );
void TIMER_vRun( uint32* const );
void TIMER_vTerminate( uint32* const );
uint32 TIMER_vConfigureAlarm( uint32 const, tpfTimerCallBack );
uint32 TIMER_vResetAlarm( uint32 const );
uint32 TIMER_vSetAlarm( uint32 const, sint32 const );

#endif // TIMER_H

