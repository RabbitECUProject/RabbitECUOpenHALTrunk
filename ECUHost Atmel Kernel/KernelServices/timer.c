/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      TIMER                                                  */
/* DESCRIPTION:        This code provides functions for kernel timers         */
/*                                                                            */	
/* FILE NAME:          TIMER.c                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "types.h"
#include "declarations.h"
#include "build.h"
#include "OS.h"
#include "TIMER.h"

TIMER_tstTimer TIMER_stTimer[TIMER_nTimerCount];
uint32 u32AlarmCounter;
		
void TIMER_vStart(uint32* const u32Stat)
{
	uint32 u32IDX;
	
	for(u32IDX = 0; u32IDX < TIMER_nTimerCount; u32IDX++)
	{
		TIMER_stTimer[u32IDX].enState = enReset;
		TIMER_stTimer[u32IDX].pfCallBack = NULL;		
	}
	
	OS_xModuleStartOK(*u32Stat);
}

void TIMER_vRun(uint32* const u32Stat)
{
	uint32 u32IDX;
	
	u32AlarmCounter++;
	
#if BUILD_KERNEL_OR_KERNEL_APP
  if (0xffffffff > OS_u32TickCounter)
	{
		OS_u32TickCounter++;
	}
#endif
	
	for (u32IDX = 0; u32IDX < TIMER_nTimerCount; u32IDX++)
	{
		if (enPending == TIMER_stTimer[u32IDX].enState)
		{
			if (u32AlarmCounter == TIMER_stTimer[u32IDX].u32AlarmPoint)
			{
				TIMER_stTimer[u32IDX].enState = enExpired;
				if(NULL != TIMER_stTimer[u32IDX].pfCallBack)
				{
						TIMER_stTimer[u32IDX].pfCallBack();
				}
			}							
		}
	}	
}

void TIMER_vTerminate(uint32* const u32Stat)
{

}

uint32 TIMER_vConfigureAlarm(uint32 const u32DelayMs, tpfTimerCallBack pfCallBack)
{
	uint32 u32IDX;
	
	for(u32IDX = 0; u32IDX < TIMER_nTimerCount; u32IDX++)
	{
		if(enReset == TIMER_stTimer[u32IDX].enState)
		{
			TIMER_stTimer[u32IDX].enState = enPending;
			TIMER_stTimer[u32IDX].pfCallBack = pfCallBack;
			TIMER_stTimer[u32IDX].u32AlarmPoint = u32AlarmCounter +	u32DelayMs;
			break;
		}
	}		
	
	return u32IDX;
}

uint32 TIMER_vResetAlarm(uint32 const u32TimerIndex)
{
	return 0;
}

uint32 TIMER_vSetAlarm(uint32 const u32DelayMs, sint32 const i32TimerIndex)
{
	TIMER_stTimer[i32TimerIndex].u32AlarmPoint = u32AlarmCounter +	u32DelayMs;				
	TIMER_stTimer[i32TimerIndex].enState = enPending;
	
	return 0;
}
