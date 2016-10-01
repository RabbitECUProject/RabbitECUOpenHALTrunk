/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      DSCRIO                                                 */
/* DESCRIPTION:        This code provides support for kernel managed resource */
/*                     discrete I/O	only (e.g. Ethernet controller)           */	
/* FILE NAME:          DSCRIO.c                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "DSCRIO.h"

		
void DSCRIO_vStart(uint32* const pu32Stat)
{
	DSCRIOHA_vStart(pu32Stat);
}

void DSCRIO_vRun(uint32* const pu32Stat)
{
	
}

void DSCRIO_vTerminate(uint32* const pu32Stat)
{

}

void DSCRIO_vSetDiscrete(DSCRIO_tenIOFunction enIOFunction, DSCRIO_tenLogicLevel enLogicLevel)
{
	DSCRIOHA_vSetDiscrete(enIOFunction, enLogicLevel);
}
