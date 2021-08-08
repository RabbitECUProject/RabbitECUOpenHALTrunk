/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TEPM_H
#define TEPM_H

#include "IOAPI.h"
#include "TEPMAPI.h"
#include "TEPMHA.h"
#include "SYSAPI.h"

#define TEPM_nSoonCounts  250
#define TEPM_nSoonCountsEDIS  50
#define TEPM_nFarCounts   0xe0000000

#ifdef EXTERN
#undef EXTERN
#endif
#ifdef _TEPM_C
#define EXTERN
#else
#define EXTERN extern
#endif


typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	TEPMAPI_ttEventTime tEventTime;
} TEPM_tstTEPMResult;



typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	TEPMHA_tenModule enModule;
	TEPMHA_tenModuleType enModuleType;
	uint32 u32Channel;
	uint32 u32SubChannel;
	uint32 u32MuxSel;
	IRQn_Type enIRQType;
} TEPM_tstTEPMChannel;

typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource[TEPMHA_nMastersMax];
} TEPM_tstMasters;


void TEPM_vStart(puint32 const pu32Arg);
void TEPM_vRun(puint32 const pu32Arg);
void TEPM_vAsyncRequest(void);
void TEPM_vTerminate(puint32 const pu32Arg);
SYSAPI_tenSVCResult TEPM_vInitTEPMResource(IOAPI_tenEHIOResource, TEPMAPI_tstTEPMResourceCB*);
uint32 TEPM_u32InitTEPMChannel(IOAPI_tenEHIOResource, TEPMAPI_tstTEPMChannelCB*);
void TEPM_boGetMasterEHIOResourceList(IOAPI_tenEHIOResource, IOAPI_tenEHIOResource*, puint32);
void TEPM_vAppendTEPMQueue(IOAPI_tenEHIOResource, TEPMAPI_tstTimedUserEvent*, TEPMAPI_ttEventCount);
void TEPM_vConfigureKernelTEPMInput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedKernelEvent*);
void TEPM_vConfigureKernelTEPMOutput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedKernelEvent*, TEPMAPI_ttEventCount);
void TEPM_vConfigureUserTEPMInput(IOAPI_tenEHIOResource, TEPMAPI_tstTimedUserEvent*);
void TEPM_vInitiateUserCallBack(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime);
void TEPM_vGetTimerVal(IOAPI_tenEHIOResource, puint32);
void TEPM_u32GetFreeVal(IOAPI_tenEHIOResource, puint32);
void TEPM_vInterruptHandler(IOAPI_tenEHIOResource, void*);
void TEPM_vMissingToothInterruptHandler(IOAPI_tenEHIOResource, void*);
uint32 TEPM_u32GetFTMTableIndex(IOAPI_tenEHIOResource);
void TEPM_vStartEventProgramKernelQueues(Bool, uint32);
void TEPM_vRunEventToothProgramKernelQueues(Bool, uint32, uint32);
void TEPM_vSynchroniseEventProgramKernelQueues(void);
IOAPI_tenTriState TEPM_enGetTimerDigitalState(IOAPI_tenEHIOResource);
void TEPM_vEnableSequences(Bool boEnable);
void TEPM_vConfigureMissingToothInterrupt(void);
void TEPM_vSetNextMissingToothInterrupt(IOAPI_tenEHIOResource, TEPMAPI_ttEventTime, uint32);
#endif //TEPM_H

