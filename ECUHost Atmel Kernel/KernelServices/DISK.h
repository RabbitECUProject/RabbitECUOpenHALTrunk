/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      DISK Header File                 											*/
/* DESCRIPTION:        This code provides enum definitions for disk support		*/
/*										 																												*/	
/* FILE NAME:          DISK.h                                          				*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef DISK_H
#define DISK_H

#include <stddef.h>
#include "sys.h"
#include "types.h"

#define DISK_nDISKS	1

typedef enum
{
	enDiskSDCard,
	enDiskFlash,
	enDiskEEPROM
} DISK_tenDiskIndex;

void DISK_vStart(uint32* const);
void DISK_vRun(uint32* const);
void DISK_vTerminate(uint32* const);

#endif //DSCRIO_H

