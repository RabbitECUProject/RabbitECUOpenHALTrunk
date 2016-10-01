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
#ifndef DIAG_H
#define DIAG_H

#include "declarations.h"
#include "build.h"
#include "OS.h"
#include "DIAGAPI.h"

#define DIAG_DDDI_COUNT				5
#define DIAG_DDDI_DATA_COUNT	200

typedef __packed struct
{
	uint16 u16DDI;
	uint8 u8AddressAndLength;
	uint8 u8RecsCount;
	uint8 u8ByteCount;
	uint8 u8Data[DIAG_DDDI_DATA_COUNT];
} DIAG_tstDDDI;


#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _DIAG_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

EXTERN DIAG_tstDDDI DIAG_astDDDI[DIAG_DDDI_COUNT];
EXTERN DIAGAPI_tDataWriteCB DIAG_DataWriteCB;
EXTERN DIAGAPI_tReadWriteCB DIAG_ReadWriteCB;
EXTERN DIAGAPI_tRoutineCB DIAG_RoutineCB;

void DIAG_vStart(void);
void DIAG_vSetCallBack(DIAGAPI_tenCallBackType, void*);

#endif //DIAG_H

