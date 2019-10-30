/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          DiagAPI                                                */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed diagnostics       */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          DiagAPI.h                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef DIAGAPI_H
#define DIAGAPI_H

#include "types.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef enum
{
	DIAGAPI_enNoSession,
	DIAGAPI_enDefaultSession,
	DIAGAPI_enExtendedSession,
	DIAGAPI_enProgrammingSession
} DIAGAPI_tenSession;

typedef enum
{
	DIAGAPI_enSecLevelInactive,
	DIAGAPI_enSecLevelOEM,
	DIAGAPI_enSecLevelMDAC
} DIAGAPI_tenDiagSecurityLevel;

typedef enum
{
	DIAGAPI_enStartRoutine,
	DIAGAPI_enStopRoutine,
	DIAGAPI_enResultsRoutine
} DIAGAPI_tenRoutineAction;

typedef struct
{
	Bool boTransferComplete;
	uint16 u16CID;
	puint8 pu8Data;
	uint8 u8DataCount;
} DIAGAPI_tstDataTransferCB;

typedef enum
{
	DIAGAPI_enDataWriteCB,
	DIAGAPI_enDataReadCB,
	DIAGAPI_enRoutineCB
} DIAGAPI_tenCallBackType;

typedef enum
{
	DIAGAPI_enCBOK,
	DIAGAPI_enRWSecurityError,
	DIAGAPI_enRWParamNotFound,
	DIAGAPI_enRWGenFail
} DIAGAPI_tenCBResult;


typedef DIAGAPI_tenCBResult (*DIAGAPI_tDataWriteCB)(uint16, puint8, uint8);
typedef DIAGAPI_tenCBResult (*DIAGAPI_tReadWriteCB)(uint16, puint8, uint8);
typedef DIAGAPI_tenCBResult (*DIAGAPI_tRoutineCB)(DIAGAPI_tenRoutineAction, puint8, uint8);

#endif //DIAGAPI_H

