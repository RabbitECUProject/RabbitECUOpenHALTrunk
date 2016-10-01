/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      ECUHOSTEFI                															*/
/* DESCRIPTION:        This code provides the entry point for the kernel			*/
/* FILE NAME:          ECUHOSTEFI.c                                           */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "CPUAbstract.h"
#include "declarations.h"
#include "CPUAbstract.h"
#include "OS.h"
#include "tqueue.h"
#include "task.h"


bool boGlobalTickFlag;
volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
extern uint32 OS_u32SysTickInterval;

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int EFIMain(void)
{
	uint32 u32Stat;
	puint32 pu32PBLWord = (puint32)0x1ffffffc;

	if (0xAA550001 != *pu32PBLWord)	
	{	
		SystemCoreClockUpdate();                     /* Get Core Clock Frequency */
		SysTick_Config(SystemCoreClock / 1000);    /* Generate interrupt each 1 ms */		
	}
	else
	{
		IRQ_vReset();
		CPU_xExitCritical();
	}		
	
	OS_u32SysTickInterval = SystemCoreClock / 1000;	
	OS_vStart(&u32Stat); 
	
  while(1) 
	{
		OS_vRun((uint32*)&u32Stat);
  }  
}

void _init(void)
{


}