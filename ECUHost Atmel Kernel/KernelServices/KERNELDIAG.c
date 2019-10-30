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
#define _DIAG_C

#include <string.h>
#include "KERNELDIAG.h"

void DIAG_vStart(void)
{	
	uint32 u32IDX;
	
	DIAG_DataWriteCB = NULL;
	DIAG_ReadWriteCB = NULL;
	DIAG_RoutineCB = NULL;
	
	for (u32IDX = 0; u32IDX < DIAG_DDDI_COUNT; u32IDX++)
	{
		memset((void*)&DIAG_astDDDI[u32IDX], 0, sizeof(DIAG_tstDDDI));
	}
}

void DIAG_vSetCallBack(DIAGAPI_tenCallBackType enCallBackType, void* pfFunction)
{
// #if BUILD_SBL
// 	OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
// 	
// 	switch (enCallBackType)
// 	{
// 		case DIAGAPI_enDataWriteCB:
// 		{
// 			DIAG_DataWriteCB = (DIAGAPI_tDataWriteCB)pfFunction;
// 			break;
// 		}
// 		
// 		case DIAGAPI_enDataReadCB:
// 		{
// 			break;
// 		}
// 		
// 		case DIAGAPI_enRoutineCB:
// 		{
// 			break;
// 		}
// 		
// 		default:
// 		{
// 			OS_stSVCDataStruct.enSVCResult = SYSAPI_enBadCallback;
// 			break;
// 		}
// 	}
// #endif
#if defined(BUILD_KERNEL)
	OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
	
	switch (enCallBackType)
	{
		case DIAGAPI_enDataWriteCB:
		{
			DIAG_DataWriteCB = (DIAGAPI_tDataWriteCB)pfFunction;
			break;
		}
		
		case DIAGAPI_enDataReadCB:
		{
			break;
		}
		
		case DIAGAPI_enRoutineCB:
		{
			break;
		}
		
		default:
		{
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enBadCallback;
			break;
		}
	}
#endif
}










