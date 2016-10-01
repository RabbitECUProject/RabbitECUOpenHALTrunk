/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SDHC                                                   */
/* DESCRIPTION:        This code provides setup and functions for the SDHC    */
/*                     hardware module                                        */	
/* FILE NAME:          SDHC.c                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "types.h"
#include "CPUAbstract.h"
#include "regset.h"
#include "declarations.h"
//#include "OS.h"
#include "disk.h"
#include "sdhc.h"
#include "timer.h"
#include "sim.h"
#include "SDHCHA.h"


void SDHC_vStart(uint32* const pu32Stat)
{
    SDHCHA_vStart(pu32Stat)	;
	OS_xModuleStartOK(*pu32Stat);
	//SDHC__vCardInitialise();
}

void SDHC_vRun(uint32* const pu32Stat)
{
    SDHCHA_vRun(pu32Stat);	
}

void SDHC_vTerminate(uint32* const pu32Stat)
{
    SDHC_vTerminate(pu32Stat);
}

void SDHC_vHWInit(void)
{
    SDHCHA_vHWInit();
}

void SDHC_vHWDeInit(void)
{
    SDHC_vHWDeInit();
}

uint8 SDHC_u8DiskRead(uint8* const pu8Buf, const uint32 u32Sector, const uint16 u16SectorCount)
{
     SDHC_u8DiskRead(pu8Buf, u32Sector, u16SectorCount);

	return 0;
}

uint8 SDHC_u8DiskWrite(uint8* const pu8Buf, const uint32 u32Sector, const uint16 u16SectorCount)
{
	return 0;
}



