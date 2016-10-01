/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SYS Header File                                        */
/* DESCRIPTION:        This code various system for oscillator and bus        */
/*                     frequencies, kernel thread memory usage etc            */
/* FILE NAME:          sys.h                                                  */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SYS_H
#define SYS_H

#include "types.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define KERNEL
//#define DEBUG
#define SYS_TASK_PRIORITISE

#define SYS_FREQ_OSC							(50000000u)
#define SYS_FREQ_OSCER						(50000000u)
#define SYS_FREQ_PLL 							(100000000u)
#define SYS_FREQ_NS_IN_1S					(1000000000u)

#define SYS_CORE_CLK_DIV					(1u)
#define SYS_BUS_CLK_DIV						(2u)
#define SYS_FLEXBUS_CLK_DIV				(2u)
#define SYS_FLASH_CLK_DIV					(4u)

#define SYS_FREQ_CORE							(SYS_FREQ_PLL / SYS_CORE_CLK_DIV)
#define SYS_FREQ_BUS							(SYS_FREQ_PLL / SYS_BUS_CLK_DIV)
#define SYS_FREQ_FLEXBUS					(SYS_FREQ_PLL / SYS_FLEXBUS_CLK_DIV)
#define SYS_FREQ_FLASH						(SYS_FREQ_PLL / SYS_FLASH_CLK_DIV)

#define SYS_PERIOD_OSC_nS					(SYS_FREQ_NS_IN_1S / SYS_FREQ_OSC)
#define	SYS_PERIOD_PLL_nS					(SYS_FREQ_NS_IN_1S / SYS_FREQ_PLL)
#define SYS_PERIOD_RTI_nS					(1000000u)
/*CR1_113*/											
																	
#define SYS_THREAD_STACK_SIZE			(512u)	
#define SYS_THREAD_MAX						(5u)
#define SYS_CLIENT_MODULES_MAX		(10u)
/*CR1_114*/

#endif //SYS_H

