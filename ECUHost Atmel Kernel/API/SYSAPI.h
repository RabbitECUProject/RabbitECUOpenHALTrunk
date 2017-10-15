/******************************************************************************/
/*                Copyright (c) 2016 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* API TITLE:          SYSAPI                                                 */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required for System calls                    */
/*                                                                            */
/* FILE NAME:          SYSAPI.h                                               */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SYSAPI_H
#define SYSAPI_H

#include "types.h"
#include "TASKAPI.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define SYSAPI_CALL_MASK_FAIL_RES_INIT 	1u


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef enum
{
	SYSAPI_enSetSVCDataPointer = 0x1000,
	/* USER_vSVC(SYSAPI_enSetSVCDataPointer, SYSAPI_stSVCDataStruct*, NULL, NULL) */
	
	SYSAPI_enSetClientStartTask = 0x1010,
	/* USER_vSVC(SYSAPI_enSetClientStartTask, SYSAPI_tpfUserTaskFunction, TASKAPI_tenPriority*, TASKAPI_tenRateMs*) */
	
	SYSAPI_enSetClientCyclicTask = 0x1011,
	/* USER_vSVC(SYSAPI_enSetClientStartTask, SYSAPI_tpfUserTaskFunction, TASKAPI_tenPriority*, TASKAPI_tenRateMs*) */
	
	SYSAPI_enSetClientTerminateTask = 0x1012,
	/* USER_vSVC(SYSAPI_enSetClientStartTask, SYSAPI_tpfUserTaskFunction, TASKAPI_tenPriority*, TASKAPI_tenRateMs*) */
	
	SYSAPI_enPopulateADArray = 0x1020,
	/* USER_vSVC(SYSAPI_enPopulateADArray, NULL, NULL, NULL) TBC*/
	
	SYSAPI_enRequestIOResource = 0x1030,
	/* USER_vSVC(SYSAPI_enRequestIOResource, IOAPI_tenEHIOResource*, NULL, NULL) */
	
	SYSAPI_enInitialiseIOResource = 0x1031,
	/* USER_vSVC(SYSAPI_enInitialiseIOResource, IOAPI_tenEHIOResource*, IOAPI_tenEHIOType*, IOAPI_tenAsApplicable*) */
	
	SYSAPI_enInitialiseCTRLResource = 0x1032,	
	/* USER_vSVC(SYSAPI_enInitialiseCTRLResource,	CTRLAPI_tenCTRLType, CTRLAPI_tstPIDCB*, NULL) */
	
	SYSAPI_enIterateCTRLResource = 0x1033,		
	/* USER_vSVC(SYSAPI_enIterateCTRLResource, CTRLAPI_tenCTRLType*, CTRLAPI_ttPIDIDX*, NULL) */	
	
	SYSAPI_enInitialiseSpreadResource = 0x1034,
	/* USER_vSVC(SYSAPI_enInitialiseSpreadResource, SPREADAPI_tstSpreadCB*, NULL, NULL) */	
	
	SYSAPI_enCalculateSpread = 0x1035,
	/* USER_vSVC(SYSAPI_enCalculateSpread, SPREADAPI_ttSpreadIDX*, NULL, NULL) */	
	
	SYSAPI_enInitialiseTableResource = 0x1036,
	/* USER_vSVC(SYSAPI_enInitialiseTableResource, TABLEAPI_tstTableCB*, NULL, NULL) */	
	
	SYSAPI_enCalculateTable = 0x1037,	
	/* USER_vSVC(SYSAPI_enCalculateTable, TABLEAPI_ttTableIDX*, NULL, NULL) */	
	
	SYSAPI_enAssertDIOResource = 0x1050,
	/* USER_vSVC(SYSAPI_enAssertDIOResource, IOAPI_tenEHIOResource*, IOAPI_tenTriState*, NULL) */	
	
	SYSAPI_enTriggerADQueue = 0x1051,
	/* USER_vSVC(SYSAPI_enTriggerADQueue,	ADCAPI_tenTrigger*, NULL, NULL) */
	
	SYSAPI_enRequestIOBusTransfer = 0x1052,
	/* USER_vSVC(SYSAPI_enRequestIOBusTransfer,	IOAPI_tenEHIOResource*, IOAPI_tstTransferCB*, NULL) */
	
	SYSAPI_enAppendTEPMQueue = 0x1053,
	/* USER_vSVC(SYSAPI_enAppendTEPMQueue, IOAPI_tenEHIOResource*, TEPMAPI_tstTimedEvent*, TEPMAPI_ttEventCount*) */
	
	SYSAPI_enWriteDACQueue = 0x1054, 
	/* USER_vSVC(SYSAPI_enWriteDACQueue, IOAPI_tenEHIOResource*, DACAPI_ttOutputVoltage*, NULL) */	
	
	SYSAPI_enSetDiagCallback = 0x1055,
	/* USER_vSVC(SYSAPI_enSetDiagCallback, DIAGAPI_tenCallBackType*, DIAGAPI_tDataWriteCB*, NULL) */
	
	SYSAPI_enGetTimerValue = 0x1060,
	/* USER_vSVC(SYSAPI_enGetTimerValue, IOAPI_tenEHIOResource*, puint32*, NULL) */
	
	SYSAPI_enGetCRC16 = 0x1061,
	/* USER_vSVC(SYSAPI_enGetCRC16,	NULL, NULL, NULL) TBC */
	
	SYSAPI_enCheckPartition = 0x1062,
	/* USER_vSVC(SYSAPI_enCheckPartition,	NULL, NULL, NULL) */
	
	SYSAPI_enSetupWorkingPage = 0x1063,
	/* USER_vSVC(SYSAPI_enSetupWorkingPage, puint8*, uint16*, NULL) */
	
	SYSAPI_enNVMWorkingCopy = 0x1064,
	/* USER_vSVC(SYSAPI_enNVMWorkingCopy, bool*, uint16*, NULL) */	
	
	SYSAPI_enQueueCANMessage = 0x1065,

	SYSAPI_enConfigureUserTEPMInput = 0x1066,

	SYSAPI_enConfigureKernelTEPMOutput = 0x1067,

	SYSAPI_enSetupCrankTriggerEdgePattern = 0x1068,

    SYSAPI_enResetWatchdog = 0x1069,

	SYSAPI_enSetupSyncPointsPattern = 0x1070,
	/* USER_vSVC(enResetWatchdog, uint16*, NULL, NULL) */	

	SYSAPI_enInitialiseMapResource = 0x1071,
	/* USER_vSVC(SYSAPI_enInitialiseMapResource, MAPSAPI_tstMapCB*, NULL, NULL) */

	SYSAPI_enCalculateMap = 0x1072,
	/* USER_vSVC(SYSAPI_enCalculateMap, MAPSAPI_ttMapIDX*, NULL, NULL) */

	SYSAPI_enGetSpreadResult = 0x1073,
	/* USER_vSVC(SYSAPI_enGetSpreadResult, TABLEAPI_ttTableIDX*, NULL, NULL) */	

	SYSAPI_enGetDIOResource = 0x1074
	/* USER_vSVC(SYSAPI_enGetDIOResource, IOAPI_tenEHIOResource*, NULL, NULL) */
} SYSAPI_tenSVCID;

typedef enum
{
	SYSAPI_enOK,
	SYSAPI_enFail,
	SYSAPI_enQueueFull,	
	SYSAPI_enBadArgument,
	SYSAPI_enBadResource,
	SYSAPI_enResourceAlreadyAllocated,
	SYSAPI_enMasterResourceUninitialised,
	SYSAPI_enMasterResourceMismatch,
	SYSAPI_enBaudRateUnavailable,
	SYSAPI_enResourceUnavailable,
	SYSAPI_enBadCallback,
	SYSAPI_enBadCRC16,
	SYSAPI_enBadPartition,
	SYSAPI_enBadNVMWorkingCopy	
} SYSAPI_tenSVCResult;

typedef enum
{
	SYSAPI_enStartOK,
	SYSAPI_enResourceRequestFailed,
	SYSAPI_enResourceInitFailed
} SYSAPI_tenStartResult;

typedef uint32 SYSAPI_ttClientHandle;

typedef struct
{
	SYSAPI_tenSVCResult enSVCResult;
	SYSAPI_ttClientHandle tClientHandle;/*CR1_117*/
	SYSAPI_tenSVCID enSVCID;
	void* pvArg1;
	void* pvArg2;
	void* pvArg3;
	void* pvData;
} SYSAPI_tstSVCDataStruct;

/* typedef of user start, run and terminate function pointers */
typedef void (*SYSAPI_tpfUserTaskFunction)(uint32 * const pu32Arg);/*CR1_116*/

typedef struct
{
	SYSAPI_tpfUserTaskFunction pfStart;
	SYSAPI_tpfUserTaskFunction pfRun;
	SYSAPI_tpfUserTaskFunction pfTerminate;
	SYSAPI_tpfUserTaskFunction pfCallBack;
	TASKAPI_tenPriority enPriority;
	TASKAPI_tenRateMs		enRateMs;
} SYSAPI_tstUserModuleFunctions;


#endif // SYSAPI_H

