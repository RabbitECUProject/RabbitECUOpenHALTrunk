/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      MSG Header File                                        */
/* DESCRIPTION:        This code provides message struct and enum             */
/*                     definitions                                            */
/* FILE NAME:          MSG.h                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef MSG_H
#define MSG_H

#include "declarations.h"
#include "CPUAbstract.h"
#include "sys.h"	
#include "types.h"

typedef enum
{
	MSG_enNone,
	MSG_enADCResult,
	MSG_enUSERCB,
	MSG_enLWIP,
	MSG_enTEPMEvent,
	MSG_enDiagDataRead,
	MSG_enDiagDataWrite,
	MSG_enDiagFunctionReq,
	MSG_enMSGTypeCount
} MSG_tenMSGType;

typedef enum
{
	MSG_enOK,
	MSG_enMBXQueueFull,
	MSG_enMBXFull,
	MSG_enMBXTimeout,
	MSG_enNoMSG,
	MSG_enBadMBX,
	MSG_enBadMSGType
} MSG_tenMBXErr;

typedef struct
{
	MSG_tenMSGType enMSGType;
	void* pstMSG;	
} MSG_tstMBX;

#define MSG_nMBXCount 128u


/* Public function definitions */
void MSG_vStart(uint32* const);
void MSG_vRun(uint32* const);
void MSG_vTerminate(uint32* const);
MSG_tenMBXErr MSG_enMBXNew(MSG_tstMBX**, MSG_tenMSGType);
void MSG_vMBXFree(MSG_tstMBX* const);
void MSG_vMBXPost(MSG_tstMBX* const, void* const);
MSG_tenMBXErr MSG_vMBXTryPost(MSG_tstMBX* const, void* const);
MSG_tenMBXErr MSG_enMBXTryFetchWait(MSG_tstMBX* const, void*);
MSG_tenMBXErr MSG_enMBXTryFetch(MSG_tstMBX* const, void*);
Bool MSG_boMBXValid(MSG_tstMBX* const);
void MSG_boMBXInvalidate(MSG_tstMBX* const);
	
#endif //MSG_H

