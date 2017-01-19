/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Battery Voltage Header File                            */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for battery voltage            */
/*                     measurement	                                          */
/* FILE NAME:          BVM.h                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef BVM_H
#define BVM_H

#include "ADCAPI.h"
#include "CONV.h"
#include "declarations.h"
#include "IOAPI.h"
#include "SYSAPI.h"
#include "UNITS.h"
#include "USER.h"
#include "USERMATH.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define BVM_nVoltsFilteredInitVal	(uint32)(0x100 * 1000 * 12)
/*CR1_87*/

#define BVM_nADInput   EH_IO_ADD11

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _BVM_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN GPM6_ttVolts BVM_tBattVolts;/*CR1_88*/
//ASAM mode=readvalue name="Battery Voltage" type=uint32 offset=0 min=0 max=20 m=0.001 b=0 units="V" format=5.3 help="Battery Voltage" 


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void BVM_vStart(uint32* const pu32Arg);
void BVM_vRun(uint32* const pu32Arg);
void BVM_vTerminate(uint32* const pu32Arg);
void BVM_vCallBack(uint32* const pu32Arg);
void BVM_vThread(void);

#endif // BVM_H

