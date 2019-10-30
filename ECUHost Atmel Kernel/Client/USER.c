/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      User                                                   */
/* DESCRIPTION:        This code module initialises the required user         */
/*                     application task lists and the service call function   */
/* FILE NAME:          USER.c                                                 */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#define _USER_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if (BUILD_USER == 1)

#include "user.h"

/* LOCAL MACRO DEFINITIONS ****************************************************/
#define USER_nUserModuleCount sizeof(USER_rastUserModuleFunctions ) / sizeof(SYSAPI_tstUserModuleFunctions )/*CR1_75*/

/* LOCAL CONSTANT DEFINITIONS (STATIC) ****************************************/
const SYSAPI_tstUserModuleFunctions USER_rastUserModuleFunctions[USER_enUserCount] = USER_nUserModuleFunctions;
//static void USER_vSVCFunction(SYSAPI_tenSVCID, void*, void*, void*);

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
Bool USER_vStart(SYSAPI_tstSVCDataStruct* pstSVCDataStructArg)
{
	SYSAPI_tpfUserTaskFunction pfUserTask;
	TASKAPI_tenPriority enPriority;
	TASKAPI_tenRateMs enRateMs;
	uint32 u32ModuleIDX;
	bool boRetVal = false;

#if defined(BUILD_USER_APP	)
	memset((void*)USER_nVarsRAMBase, 0, USER_nVarsRAMSize);/*CR1_77*/
#endif
	
	pstSVCDataStruct = pstSVCDataStructArg;	
	
	/* Add all of the client start tasks */
	for (u32ModuleIDX = 0; u32ModuleIDX < USER_nUserModuleCount; u32ModuleIDX++)
	{
		pfUserTask = USER_rastUserModuleFunctions[u32ModuleIDX].pfStart;
		if ((NULL!= pfUserTask) && (SYSAPI_enOK == pstSVCDataStruct->enSVCResult))
		{		
			enPriority = USER_rastUserModuleFunctions[u32ModuleIDX].enPriority;
			enRateMs = USER_rastUserModuleFunctions[u32ModuleIDX].enRateMs;
			USER_vSVC(SYSAPI_enSetClientStartTask, (void*)&pfUserTask, (void*)&enPriority, (void*)&enRateMs);
		}
	}
	
	/* Add all of the client cyclic tasks */
	for (u32ModuleIDX = 0; u32ModuleIDX < USER_nUserModuleCount; u32ModuleIDX++)
	{
		pfUserTask = USER_rastUserModuleFunctions[u32ModuleIDX].pfRun;
		if ((NULL!= pfUserTask) && (SYSAPI_enOK == pstSVCDataStruct->enSVCResult))
		{		
			enPriority = USER_rastUserModuleFunctions[u32ModuleIDX].enPriority;
			enRateMs = USER_rastUserModuleFunctions[u32ModuleIDX].enRateMs;
			USER_vSVC(SYSAPI_enSetClientCyclicTask, (void*)&pfUserTask, (void*)&enPriority, (void*)&enRateMs);
		}
	}	
	
	/* Add all of the terminate cyclic tasks */
	for (u32ModuleIDX = 0; u32ModuleIDX < USER_nUserModuleCount; u32ModuleIDX++)
	{
		pfUserTask = USER_rastUserModuleFunctions[u32ModuleIDX].pfRun;
		if ((NULL!= pfUserTask) && (SYSAPI_enOK == pstSVCDataStruct->enSVCResult))
		{
			enPriority = USER_rastUserModuleFunctions[u32ModuleIDX].enPriority;
			enRateMs = USER_rastUserModuleFunctions[u32ModuleIDX].enRateMs;
			USER_vSVC(SYSAPI_enSetClientTerminateTask, (void*)&pfUserTask, (void*)&enPriority, (void*)&enRateMs);
		}
	}	

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		boRetVal = true;	
	}

	return boRetVal;
}

void USER_vRun(uint32 * const pu32Arg )
{
	
}

void USER_vTerminate(uint32* const pu32Arg )
{

}


//void __attribute__((optimize("-O0"))) USER_vSVC(SYSAPI_tenSVCID enSVCID, void* pvArg1, void* pvArg2, void* pvArg3)
//{     
    ///* This line prevents non-stacking of parameters due to optimisation */   
	//if (0 == (enSVCID | *(uint32*)pvArg1 | *(uint32*)pvArg2 | *(uint32*)pvArg3)) return;
//
	//__asm("SVC 0x00");
//}

void USER_vSVC(SYSAPI_tenSVCID enSVCID, void* pvArg1, void* pvArg2, void* pvArg3)
{
	pstSVCDataStruct->enSVCID = enSVCID;
	pstSVCDataStruct->pvArg1 = pvArg1;
	pstSVCDataStruct->pvArg2 = pvArg2;
	pstSVCDataStruct->pvArg3 = pvArg3;
	__asm("SVC 0x00");
}

#endif //BUILD_USER
