/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Idle Air Control Header File                           */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for idle air control           */
/*                                                                            */
/* FILE NAME:          IAC.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef IAC_H
#define IAC_H

#include <string.h>
#include "CTS.h"
#include "IOAPI.h"
#include "SETUP.h"
#include "USER.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/
		
/* GLOBAL TYPE DEFINITIONS ****************************************************/

typedef enum
{
	IAC_enResetHome,
	IAC_enNormal
} IAC_tenStepperState;

typedef enum
{
	IAC_enOpenLoop,
	IAC_enClosedLoop
} IAC_tenControlState;
		
/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void IAC_vStart(uint32 * const pu32Arg);
void IAC_vRun(uint32* const pu32Arg);
void IAC_vTerminate(uint32* const pu32Arg);
void IAC_vCallBack(puint32 const pu32Arg);

#endif // MAP_H

