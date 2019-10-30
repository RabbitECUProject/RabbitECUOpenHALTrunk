/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      ECUHOSTEFI                                             */
/* DESCRIPTION:        This code provides the entry point for the kernel      */
/* FILE NAME:          ECUHOSTEFI.c                                           */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "build.h"
#include "CPUAbstract.h"
#include "declarations.h"
#include "CPUAbstract.h"
#include "OS.h"
#include "IRQ.h"
#include "sys.h"
#include "tqueue.h"
#include "task.h"
#include "types.h"


Bool boGlobalTickFlag;
extern void SystemCoreClockUpdate(void);
volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
extern uint32 OS_u32SysTickInterval;
int EFIMain(void);

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int EFIMain(void)
{
	uint32 u32Stat;
	puint32 pu32PBLWord = (puint32)PBL_WORD_ADDR;

	if (0xAA550001 != *pu32PBLWord)	
	{	
		SystemCoreClockUpdate();                     /* Get Core Clock Frequency */
		SysTick_Config(SYS_FREQ_BUS / 1000);    /* Generate interrupt each 1 ms */		
	}
	else
	{
		IRQ_vReset();
		CPU_xExitCritical();
	}		
	
	OS_u32SysTickInterval = SYS_FREQ_BUS / 1000;	
	OS_vStart(&u32Stat); 
	
  while(1) 
	{
		OS_vRun((uint32*)&u32Stat);
  }  
}
