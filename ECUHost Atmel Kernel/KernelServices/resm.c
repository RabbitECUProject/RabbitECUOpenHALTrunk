/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _RESM_C

#include "sys.h"
#include "build.h"
#include <stddef.h>


#include "IOAPI.h"
#include "types.h"
#include "TEPM.h"
#include "RESM.h"

RESM_tstResource RESM_astResource[IO_Total_Count + RES_Count];

void RESM_vStart(uint32* const u32Stat)
{
	uint32 u32IDX;
	
	for (u32IDX = 0; u32IDX < (IO_Total_Count + RES_Count); u32IDX++)
	{
		RESM_astResource[u32IDX].tClientHandle = NULL;
	}	
}

void RESM_vRun(uint32* const u32Stat)
{
	
}

void RESM_vTerminate(uint32* const u32Stat)
{

}

SYSAPI_ttClientHandle RESM_RequestEHIOResource(IOAPI_tenEHIOResource enEHIO, SYSAPI_ttClientHandle tClientHandle)
{		
	if ((NULL == RESM_astResource[enEHIO].tClientHandle)
		||(tClientHandle == RESM_astResource[enEHIO].tClientHandle))
	{
		RESM_astResource[enEHIO].tClientHandle = tClientHandle;
	}
	else
	{
		tClientHandle = NULL;
	}
	
	return tClientHandle;
}

SYSAPI_ttClientHandle RESM_u32GetMasterEHIOResource(IOAPI_tenEHIOResource enEHIOResource)
{
	return RESM_astResource[enEHIOResource].tClientHandle;	
}

uint32 RESM_u32GetMasterEHIOResourceList(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOResource* penEHIOResource)
{
	uint32 u32MasterCount = 0;
	
	if ((EH_FIRST_TMR <= enEHIOResource) &&
	(EH_LAST_TMR >= enEHIOResource))
	{

#if BUILD_KERNEL_OR_KERNEL_APP
	    TEPM_boGetMasterEHIOResourceList(enEHIOResource, penEHIOResource, &u32MasterCount);
#endif			
	}
	
	return u32MasterCount;
}




