/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Diagnostics Header File                     						*/
/* DESCRIPTION:        This code module initialises the required comms     		*/
/*                     resources for and provides functions for diagnostics   */
/* FILE NAME:          DIAG.h                                                 */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef USERDIAG_H
#define USERDIAG_H

#include <string.h>
#include "ADCAPI.h"
#include "BVM.h"
#include "declarations.h"
#include "DIAGAPI.h"
#include "IOAPI.h"
#include "SYSAPI.h"
#include "UNITS.h"
#include "USER.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define USERDIAG_nUARTFrequency 9600u
#define USERDIAG_nCAN1Frequency 500000u
#define USERDIAG_nRPCUDSREQSourcePort			0x9c04
#define USERDIAG_nRPCUDSREQDestPort				0x9c14
#define USERDIAG_nRPCUDSRESPSourcePort		0x9c05
#define USERDIAG_nRPCUDSRESPDestPort			0x9c15
#define USERDIAG_nEnableUART							0
#define USERDIAG_nEnableCAN								0
#define USERDIAG_nEnableENET							1

#define USER_DIAG_nCIDInfo																																																	\
{																																																														\
	{ 0x8000,	(puint8)&USERCAL_stRAMCAL.userCalADRefH, 								sizeof(GPM6_ttVolts), 	4,	DIAGAPI_enSecLevelInactive},	\
	{ 0x8001,	(puint8)&USERCAL_stRAMCAL.userCalVRef445, 							sizeof(GPM6_ttVolts), 	4,	DIAGAPI_enSecLevelInactive},	\
	{ 0x8002,	(puint8)&USERCAL_stRAMCAL.userCalVNernstTarget, 				sizeof(GPM6_ttVolts), 	4,	DIAGAPI_enSecLevelInactive},	\
	{ 0x8003,	(puint8)&USERCAL_stRAMCAL.userCalRNernstReady, 					sizeof(GPM6_ttVolts), 	4,	DIAGAPI_enSecLevelInactive},	\
	{ 0x8004,	(puint8)&USERCAL_stRAMCAL.au32UserCalPumpCurrentPFactor, 2 * sizeof(uint32), 	4,	DIAGAPI_enSecLevelInactive}		\
}

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _USERDIAG_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

/* GLOBAL TYPE DEFINITIONS ****************************************************/
/*******************************************************************************
* Description      : Type to hold a CID configuration
*******************************************************************************/
typedef struct
{
	uint16 u16CID;
	puint8 pu8Data;
	uint8 u8DataCount;
	uint8 u8DataSize;
	DIAGAPI_tenDiagSecurityLevel enSecLevel;
} USERDIAG_tstIdentifierIB;


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void USERDIAG_vStart(uint32* const pu32Arg);
void USERDIAG_vRun(uint32* const pu32Arg);
void USERDIAG_vTerminate(uint32* const pu32Arg);
void USERDIAG_vCallBack(uint32* const pu32Arg);

/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Parameter
*    Par1          : <Comment on u8Param1 (line 1)>
*    Par2          : <Comment on pu16Param2 (line 1)>
*                    <Comment on pu16Param2 (line 2)>
*
* Return Value     : <Comment on return value >
*******************************************************************************/
DIAGAPI_tenCBResult USERDIAG_boReadDataCallBack(uint16 u16CID, puint8 pu8Data, uint8 u8DataCount);/*CR1_1*/;

/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Parameter
*    Par1          : <Comment on u8Param1 (line 1)>
*    Par2          : <Comment on pu16Param2 (line 1)>
*                    <Comment on pu16Param2 (line 2)>
*
* Return Value     : <Comment on return value >
*******************************************************************************/
DIAGAPI_tenCBResult USERDIAG_boWriteDataCallBack(uint16 u16CID, puint8 pu8Data, uint8 u8DataCount);/*CR1_1*/

/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Parameter
*    Par1          : <Comment on u8Param1 (line 1)>
*    Par2          : <Comment on pu16Param2 (line 1)>
*                    <Comment on pu16Param2 (line 2)>
*
* Return Value     : <Comment on return value >
*******************************************************************************/
DIAGAPI_tenCBResult USERDIAG_boRoutineControl(DIAGAPI_tenRoutineAction enRoutineAction, puint8 pu8Data, uint8 u8DataCount);/*CR1_1*/;

#endif // USERDIAG_H

