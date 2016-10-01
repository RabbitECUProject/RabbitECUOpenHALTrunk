/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CLIENT Header File                                     */
/* DESCRIPTION:        This code provides struct and enum definitions for     */
/*                     client queues and callbacks                            */
/* FILE NAME:          CLIENT.h                                               */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CLIENT_H
#define CLIENT_H

//#include "ADC.h"
//#include "ADCAPI.h"
#include "build.h"
#include "CPUAbstract.h"
#include "CQUEUE.h"
#include "declarations.h"
#include "IOAPI.h"
#include "MSG.h"
#include "OS.h"
#include "sys.h"
#include "SYSAPI.h"
#include "task.h"
#include "TASKAPI.h"
#include "TEPM.h"
#include "TEPMAPI.h"
#include "types.h"
//#include "USER.h"


#define CLIENT_nUserCBMax 					20u
#define CLIENT_nUserAppImageBase		0x00050000u
/*CR1_90*/

typedef void (*tpfClientCB)(void*);
typedef int (*tpfUserStart)(void);

typedef enum
{
	TaskQueue_Start,
	TaskQueue_Run,
	TaskQueue_Terminate,
} CLIENT_tenTaskQueue;

typedef enum
{
	CLIENT_enOK,
	CLIENT_enFailQueueFull,
	CLIENT_enErrUNK
} CLIENT_tenErr;

typedef struct
{
	tpfClientCB pfCB;
	MSG_tstMBX* pstMBX;
} CLIENT_tstCBInfo;

																																				   																		
void CLIENT_vStart(uint32* const);
CLIENT_tenErr CLIENT_enEnqueueCB(MSG_tstMBX*, tpfClientCB);
SYSAPI_tenSVCResult CLIENT_enAddTask(OS_tenQueueType, SYSAPI_tpfUserTaskFunction, TASKAPI_tenPriority, TASKAPI_tenRateMs);
void CLIENT_vRunUserCBQueue(void);

#endif //CLIENT_H

