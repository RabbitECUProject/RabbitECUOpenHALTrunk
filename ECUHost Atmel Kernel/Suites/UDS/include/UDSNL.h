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
#ifndef UDSNL_H
#define UDSNL_H

#include "COMMONNL.h"
#include "UDSAL.h"

#define UDSNL_SMALL_BUFF_SIZE 				8u
#define UDSNL_RX_BUFF_LARGE_SIZE			1030u
#define UDSNL_TX_BUFF_LARGE_SIZE			258u
#define UDSNL_nActiveResourceTimeout	5000u


typedef enum
{
	UDSNL_enSegRXModeSmall,
	UDSNL_enSegRXModeLarge,
	UDSNL_enSegRXModeInvalid,
	USDNL_enSegRXErr
} UDSNL_tenSegRXMode;

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _UDSNL_C
	#define EXTERN
#else
	#define EXTERN extern
#endif
	
EXTERN COMMONNL_tstRXLargeBuffer UDSNL_stRXLargeBuffer;	
EXTERN COMMONNL_tstTXLargeBuffer UDSNL_stTXLargeBuffer;
EXTERN COMMONNL_tstRXSmallBuffer UDSNL_stRXSmallBuffer;
EXTERN COMMONNL_tstTXSmallBuffer UDSNL_stTXSmallBuffer;

void UDSNL_vStart(void);
void UDSNL_vReceiveFrame(IOAPI_tenEHIOResource, puint8, puint8, puint32, uint32);
void UDSNL_vActiveResourceTimeout(void);
uint32 UDSNL_u32GetTransmissionBytesRequired(uint32);

#endif //UDSNL_H
