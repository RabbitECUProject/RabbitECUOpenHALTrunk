/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      User Header File                                       */
/* DESCRIPTION:        This code module initialises the required user         */
/*                     application task lists and the service call function   */
/* FILE NAME:          USER.h                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef USER_H
#define USER_H

#include <string.h>/*CR1_76*/
#include "BVM.h"
#include "CAM.h"
#include "CTS.h"
#include "DIAG.h"
#include "declarations.h"
#include "EST.h"
#include "FILM.h"
#include "FUEL.h"
#include "LSU4X.h"
#include "LSUH.h"
#include "MAP.h"
#include "IAC.h"
#include "RELAYS.h"
#include "SYSAPI.h"
#include "SENSORS.h"
#include "TPS.h"
#include "types.h"
#include "USERCAL.h"
#include "sys.h"
#include "SYSAPI.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define USER_nUserModuleFunctions																																														\
{																																																														\
	{NULL, NULL, NULL, NULL, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},																											\
	{&MAP_vStart,	&MAP_vRun, &MAP_vTerminate, &MAP_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},											\
	{&LSU4X_vStart,	&LSU4X_vRun, &LSU4X_vTerminate, &LSU4X_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},							\
	{&LSUH_vStart,	&LSUH_vRun, &LSUH_vTerminate, &LSUH_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},								\
	{&TPS_vStart,	&TPS_vRun, &TPS_vTerminate, &TPS_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},											\
	{&BVM_vStart,	&BVM_vRun, &BVM_vTerminate, &BVM_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},											\
	{&CAM_vStart,	&CAM_vRun, &CAM_vTerminate, &CAM_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask10ms},										\
	{&FUEL_vStart,	&FUEL_vRun, &FUEL_vTerminate, &FUEL_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},								\
	{&CTS_vStart,	&CTS_vRun, &CTS_vTerminate, &CTS_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},											\
	{&EST_vStart,	&EST_vRun, &EST_vTerminate, &EST_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},											\
	{&IAC_vStart,	&IAC_vRun, &IAC_vTerminate, &IAC_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask25ms},										\
	{&USERDIAG_vStart, &USERDIAG_vRun, &USERDIAG_vTerminate, &USERDIAG_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},	\
	{&SENSORS_vStart, &SENSORS_vRun, &SENSORS_vTerminate, &SENSORS_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms},			\
	{&FILM_vStart, &FILM_vRun, &FILM_vTerminate, &FILM_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask10ms},								\
	{&RELAYS_vStart, &RELAYS_vRun, &RELAYS_vTerminate, &RELAYS_vCallBack, TASKAPI_enTaskPrLowest, TASKAPI_enTask10ms},								\
	{&USERCAL_vStart,	&USERCAL_vRun, &USERCAL_vTerminate, NULL, TASKAPI_enTaskPrLowest, TASKAPI_enTask1ms}										\
}

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _USER_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define USER_xEnterCritical()/*CR1_16*/
#define USER_xExitCritical()/*CR1_16*/

#if BUILD_ALLOW_CRITICAL
	#undef USER_xEnterCritical()/*CR1_16*/
	#undef USER_xExitCritical()/*CR1_16*/
	#define USER_xEnterCritical() CPU_xEnterCritical()/*CR1_16*/
	#define USER_xExitCritical() CPU_xExitCritical()/*CR1_16*/
#endif

#define USER_nVarsRAMBase 0x20000000u
#define USER_nVarsRAMSize 0x2000u



/* GLOBAL TYPE DEFINITIONS ****************************************************/
/*******************************************************************************
* Description      : Type to enumerate the user application modules
*******************************************************************************/
typedef enum
{
	USER_enUSER,
	USER_enMAP,	
	USER_enLSU4X,
	USER_enLSUH,
	USER_enTPS,	
	USER_enBVM,
	USER_enCAM,
	USER_enFUEL,	
	USER_enCTS,	
	USER_enEST,	
	USER_enIAC,	
	USER_enDIAG,
	USER_enSENSORS,
	USER_enFILM,
	USER_enRELAYS,
	USER_enUSERCAL,
	USER_enUserCount
} USER_tenUserModules;/*CR1_126*/


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN SYSAPI_tstSVCDataStruct* pstSVCDataStruct;

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
Bool USER_vStart(SYSAPI_tstSVCDataStruct*);
void USER_vRun( uint32 * const pu32Arg );/*CR1_1*/
void USER_vTerminate( uint32 * const pu32Arg );/*CR1_1*/
void USER_vSVC(SYSAPI_tenSVCID enSVCID, void* pvArg1, void* pvArg2, void* pvArg3);/*CR1_1*/

#endif // USER_H

