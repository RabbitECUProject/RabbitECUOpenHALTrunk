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
#ifndef UDSSL_H
#define UDSSL_H

#include "COMMONNL.h"
#include "DECLARATIONS.h"
#include "DIAGAPI.h"
#include "DLL.h"
#include "TIMER.h"
#include "TYPES.h"
#include "OS.h"
#include "UDSAL.h"
#include "UDSNL.h"

//buffer index breakdown
#define UDSSL_IDX_LEN			0x00
#define UDSSL_IDX_SID			0x01
#define UDSSL_IDX_SUBSID	0x02

//UDS session definitions
#define UDSSL_SESS_DFLT		0x81
#define UDSSL_SESS_PGM		0x85
#define UDSSL_SESS_EXT		0x87

#define UDSSL_nSessionTimeout	5000u
	

typedef struct
{
	uint32 u32TimerIDX;
	DIAGAPI_tenSession enSession;
} UDSSL_tstSessionCB;

void UDSSL_vStart(void);
void UDSSL_vProcessBufferCB(puint8, uint32, puint8, puint32, uint32);
DIAGAPI_tenSession UDSSL_enGetSession(void);	
void UDSSL_vResetSessionTimer(void);
void UDSSL_vSessionTimeout(void);

#endif //UDSSL_H

