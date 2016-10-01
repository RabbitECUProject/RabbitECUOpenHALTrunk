/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SDHC Header File                                       */
/* DESCRIPTION:        This code provides macros for setup and control of the */
/*                     SDHC hardware module                                   */	
/* FILE NAME:          SDHC.h                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SDHC_H
#define SDHC_H

#include "types.h"


void SDHC_vStart(uint32* const);
void SDHC_vRun(uint32* const);
void SDHC_vTerminate(uint32* const);
void SDHC_vHWInit(void);
void SDHC_vHWDeInit(void);
uint8 SDHC_u8DiskRead(uint8* const, const uint32, const uint16);
uint8 SDHC_u8DiskWrite(uint8* const, const uint32, const uint16);


#endif //SDHC_H

