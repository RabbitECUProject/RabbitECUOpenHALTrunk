/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*   License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher    */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */
/* PACKAGE TITLE:      OS Header File                                         */
/* DESCRIPTION:        This code provides macros that describe the OS task    */
/*                     schedule and OS enums                                  */
/* FILE NAME:          OS.h                                                   */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#ifndef OSTASKS_H
#define OSTASKS_H

extern void UDSAL_vStart(puint32);
extern void UDSAL_vRun(puint32);
extern void UDSAL_vTerminate(puint32);

#include "PERADC.h"
#include "PERCAN.h"
#include "CEM.h"
#include "CRC16.h"
#include "CLIENT.h"
#include "CTRL.h"
#include "DAC.h"
#include "DLL.h"
#include "FEE.h"
#include "maps.h"
#include "PIM.h"
#include "PIT.h"
#include "RESM.h"
#include "spreads.h"
#include "SRLTFR.h"
#include "tables.h"
#include "TASKAPI.h"
#include "TEPM.h"
#include "time.h"
#include "TIMER.h"
#include "PERUART.h"
#include "task.h"

#define OSTASKS_nTaskStart 																																																																										\
/* enTaskID, 					enType, 		enPriority,     enState,      enRateMS,     		pfFunction,	 tSP,  tPC, tsCreated,  tsStarted, tsFinished, tsAccumulated, tsPrivilege */\
{																																																																																					\
	{ 1u     , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &CLIENT_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 2u     , enTaskFunction	, (TASKAPI_tenPriority)5u            , enTaskReady, TASKAPI_enTaskCycNot	, &RESM_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 3u     , enTaskFunction	, (TASKAPI_tenPriority)5u            , enTaskReady, TASKAPI_enTaskCycNot	, &SPREADS_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 4u     , enTaskFunction	, (TASKAPI_tenPriority)5u            , enTaskReady, TASKAPI_enTaskCycNot	, &TABLES_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 5u     , enTaskFunction	, (TASKAPI_tenPriority)5u            , enTaskReady, TASKAPI_enTaskCycNot	, &MAPS_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 6u     , enTaskFunction	, (TASKAPI_tenPriority)5u            , enTaskReady, TASKAPI_enTaskCycNot	, &UART_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 7u     , enTaskFunction	, (TASKAPI_tenPriority)15u           , enTaskReady, TASKAPI_enTaskCycNot	, &UDSAL_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 8u     , enTaskFunction	, (TASKAPI_tenPriority)3u            , enTaskReady, TASKAPI_enTaskCycNot	, &PIM_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 9u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll           , enTaskReady, TASKAPI_enTaskCycNot	, &FEE_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 10u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll           , enTaskReady, TASKAPI_enTaskCycNot	, &CRC16_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 11u     , enTaskFunction	, (TASKAPI_tenPriority)200u,           enTaskReady, TASKAPI_enTaskCycNot	, &ADC_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 12u     , enTaskFunction	, TASKAPI_enTaskPrMaxAll,              enTaskReady, TASKAPI_enTaskCycNot	, &DAC_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 13u     , enTaskFunction	, (TASKAPI_tenPriority)100u          , enTaskReady, TASKAPI_enTaskCycNot	, &PIT_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 14u     , enTaskFunction	, (TASKAPI_tenPriority)66u           , enTaskReady, TASKAPI_enTaskCycNot	, &DLL_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 15u    , enTaskFunction	, (TASKAPI_tenPriority)77u           , enTaskReady, TASKAPI_enTaskCycNot	, &TIMER_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 16u    , enTaskFunction	, (TASKAPI_tenPriority)78u           , enTaskReady, TASKAPI_enTaskCycNot	, &CAN_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 17u    , enTaskFunction	, (TASKAPI_tenPriority)78u           , enTaskReady, TASKAPI_enTaskCycNot	, &CTRL_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 18u    , enTaskFunction	, (TASKAPI_tenPriority)78u           , enTaskReady, TASKAPI_enTaskCycNot	, &TEPM_vStart			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 19u    , enTaskFunction	, (TASKAPI_tenPriority)78u           , enTaskReady, TASKAPI_enTaskCycNot	, &CEM_vStart				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 20u    , enTaskFunction	, (TASKAPI_tenPriority)78u           , enTaskReady, TASKAPI_enTaskCycNot	, &SRLTFR_vStart		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	} \
}

#define OSTASKS_nTaskCyclic 																																																																										\
/* enTaskID, 					enType, 		enPriority,     enState,      enRateMS, 				pfFunction,	 tSP,  tPC, tsCreated,  tsStarted, tsFinished, tsAccumulated, tsPrivilege */\
{																																																																																					\
	{ 21u    , enTaskCyclic		, TASKAPI_enTaskPrMaxCyc, enTaskReady, TASKAPI_enTask100ms					, &UART_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 22u    , enTaskCyclic		, (TASKAPI_tenPriority)101u          , enTaskReady, TASKAPI_enTask10ms						, &UDSAL_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 23u    , enTaskCyclic		, (TASKAPI_tenPriority)180u          , enTaskReady, TASKAPI_enTask20ms					, &PIM_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 24u    , enTaskCyclic		, (TASKAPI_tenPriority)115u          , enTaskReady, TASKAPI_enTask20ms					, &ADC_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 25u    , enTaskCyclic		, (TASKAPI_tenPriority)115u          , enTaskReady, TASKAPI_enTask20ms					, &DAC_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 26u    , enTaskCyclic		, (TASKAPI_tenPriority)110u          , enTaskReady, TASKAPI_enTask20ms					, &PIT_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 27u    , enTaskCyclic		, (TASKAPI_tenPriority)200u          , enTaskReady, TASKAPI_enTask1ms						, &DLL_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 28u    , enTaskCyclic		, (TASKAPI_tenPriority)166u          , enTaskReady, TASKAPI_enTask1ms						, &FEE_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 29u    , enTaskCyclic		, (TASKAPI_tenPriority)19u           , enTaskReady, TASKAPI_enTask1ms						, &CAN_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 30u    , enTaskCyclic		, (TASKAPI_tenPriority)19u           , enTaskReady, TASKAPI_enTask1ms						, &CTRL_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 31u    , enTaskCyclic		, (TASKAPI_tenPriority)19u           , enTaskReady, TASKAPI_enTask1ms						, &CRC16_vRun				, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 32u    , enTaskCyclic		, (TASKAPI_tenPriority)19u           , enTaskReady, TASKAPI_enTask20ms					, &CEM_vRun					, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 33u    , enTaskCyclic		, (TASKAPI_tenPriority)19u           , enTaskReady, TASKAPI_enTask1ms						, &SRLTFR_vRun			, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	} \
}

#define OSTASKS_nTaskTerminate																																																																									\
/* enTaskID, 					enType, 		enPriority,     enState,      enRateMS,     		pfFunction,	 tSP,  tPC, tsCreated,  tsStarted, tsFinished, tsAccumulated, tsPrivilege */\
{																																																																																					\
	{ 34u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &UART_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 35u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &UDSAL_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 36u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &PIM_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 37u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &ADC_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 38u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &DAC_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 39u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &PIT_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 40u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &DLL_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 41u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &FEE_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 42u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &TIMER_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 43u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &CAN_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 44u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &CRC16_vTerminate	, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 45u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &CEM_vTerminate		, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	},\
	{ 46u    , enTaskFunction	, TASKAPI_enTaskPrMaxAll, enTaskReady, TASKAPI_enTaskCycNot	, &SRLTFR_vTerminate, NULL,	NULL, 0u   		, 0u     		 , 0u 			 ,	 0u		, enTaskPrivileged	} \
}

#endif //OSTASKS_H
	

