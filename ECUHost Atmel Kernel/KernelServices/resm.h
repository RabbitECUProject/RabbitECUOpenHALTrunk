/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      XXX Header File                    										*/
/* DESCRIPTION:        XXX   																									*/
/* FILE NAME:          XXX.h                                         					*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "sys.h"
#include "RESAPI.h"
#include "IOAPI.h"
#include "SYSAPI.h"



#ifndef RESM_H
#define RESM_H

#include "types.h"

typedef struct
{
	SYSAPI_ttClientHandle tClientHandle;
} RESM_tstResource;

#define RESM_nMastersMax	10u

#undef EXTERN
#ifdef _RESM_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN SYSAPI_ttClientHandle SYSAPI_atMasterHandle[RESM_nMastersMax];

void RESM_vStart(uint32* const u32Stat);
void RESM_vRun(uint32* const u32Stat);
void RESM_vTerminate(uint32* const u32Stat);
SYSAPI_ttClientHandle RESM_RequestEHIOResource(IOAPI_tenEHIOResource, SYSAPI_ttClientHandle);
uint32 RESM_u32GetMasterEHIOResourceList(IOAPI_tenEHIOResource, IOAPI_tenEHIOResource*);
SYSAPI_ttClientHandle RESM_u32GetMasterEHIOResource(IOAPI_tenEHIOResource);

#endif //RESM_H



