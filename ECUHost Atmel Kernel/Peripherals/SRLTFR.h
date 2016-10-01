/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      SRLTFR                                                 */
/* DESCRIPTION:        Serial Transfer header file                            */
/* FILE NAME:          SRLTFR.h                                               */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SRLTFR_H
#define SRLTFR_H

#include <stddef.h>
#include "sys.h"
#include "types.h"
#include "CQUEUE.h"

#define SRLTFR_nTransfersMax 5u

#ifdef _SRLTFR_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN IOAPI_tstTransferCB astTransferInfo[SRLTFR_nTransfersMax];
EXTERN CQUEUE_tstQueue stTransferQueue;

void SRLTFR_vStart(uint32* const);
void SRLTFR_vRun(uint32* const);
void SRLTFR_vTerminate(uint32* const);
SYSAPI_tenSVCResult SRLTFR_enEnqueue(IOAPI_tstTransferCB*);
void SRLTFR_vNotifyCB(IOAPI_tenEHIOResource);


#endif //CAN_H

