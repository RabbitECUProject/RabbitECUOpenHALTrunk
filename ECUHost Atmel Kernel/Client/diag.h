/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Diagnostics Header File                                */
/* DESCRIPTION:        This code module initialises the required comms        */
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

typedef struct 
{
	SPREADAPI_ttSpreadIDX tSpreadIDX;
	uint8* pData;
} USERDIAG_tstSpreadIDXAddressPair;

typedef enum
{
	MODE1_Supported1,
	MODE1_StatSinceDTCClear,
	MODE1_FreezeDTC,
	MODE1_FuelSysStatus,
	MODE1_CalcEngineLoad,
	MODE1_ECT,
	MODE1_STT_Bank1,
	MODE1_LTT_Bank1,
	MODE1_STT_Bank2,
	MODE1_LTT_Bank2,
	MODE1_FuelPressure,
	MODE1_MAP,
	MODE1_RPM,
	MODE1_VSS,
	MODE1_IgnAdvance,
	MODE1_IAT,
	MODE1_MAFMassPerSec,
	MODE1_TPS,
	MODE1_CmdSecAirStatus,
	MODE1_O2SensorsPresent,
	MODE1_O2Sensor1StatSTFT,
	MODE1_O2Sensor2StatSTFT,
	MODE1_O2Sensor3StatSTFT,
	MODE1_O2Sensor4StatSTFT,
	MODE1_O2Sensor5StatSTFT,
	MODE1_O2Sensor6StatSTFT,
	MODE1_O2Sensor7StatSTFT,
	MODE1_O2Sensor8StatSTFT,
	MODE1_OBDStandards,
	MODE1_O2SensorsPreset4Bank,
	MODE1_AuxInputStat,
	MODE1_RunTimeStart,
	\
	MODE1_Mode1Supported2,
	MODE1_DistMIL,
	MODE1_RailPressure,
	MODE1_GaugePressure,
	MODE1_O2Sensor1AFEquivalence,
	MODE1_O2Sensor2AFEquivalence,
	MODE1_O2Sensor3AFEquivalence,
	MODE1_O2Sensor4AFEquivalence,
	MODE1_O2Sensor5AFEquivalence,
	MODE1_O2Sensor6AFEquivalence,
	MODE1_O2Sensor7AFEquivalence,
	MODE1_O2Sensor8AFEquivalence,
	MODE1_CommandEGR,
	MODE1_EGRError,
	MODE1_CommandEvap,
	MODE1_FuelLevelInput,
	MODE1_OKWarmUps,
	MODE1_DistSinceOK,
	MODE1_EvapPressure,
	MODE1_AbsolutePressure,
	MODE1_O2Sensor1AFEquivalenceB,
	MODE1_O2Sensor2AFEquivalenceB,
	MODE1_O2Sensor3AFEquivalenceB,
	MODE1_O2Sensor4AFEquivalenceB,
	MODE1_O2Sensor5AFEquivalenceB,
	MODE1_O2Sensor6AFEquivalenceB,
	MODE1_O2Sensor7AFEquivalenceB,
	MODE1_O2Sensor8AFEquivalenceB,
	MODE1_Catalyst1Temp_Bank1,
	MODE1_Catalyst2Temp_Bank1,
	MODE1_Catalyst1Temp_Bank2,
	MODE1_Catalyst2Temp_Bank2,
	\
	MODE1_Mode1Supported3,
	MODE1_1,
	MODE1_2,
	MODE1_3,
	MODE1_4,
	MODE1_5,
	MODE1_6,
	MODE1_7,
	MODE1_8,
	MODE1_9,
	MODE1_10,
	MODE1_11,
	MODE1_12,
	MODE1_13,
	MODE1_14,
	MODE1_15,
	MODE1_16,
	MODE1_17,
	MODE1_18,
	MODE1_19,
	MODE1_20,
	MODE1_21,
	MODE1_22,
	MODE1_23,
	MODE1_24,
	MODE1_25,
	MODE1_26,
	MODE1_27,
	MODE1_28,
	MODE1_29,
	MODE1_30,
	MODE1_31,
	MODE1_Mode1Supported4,
	MODE1_PIDS_COUNT
} USERDIAG_tenMode1PIDS;



/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define USERDIAG_nUARTFrequency 9600u
#define USERDIAG_nCAN1Frequency 500000u
#define USERDIAG_nRPCUDSREQSourcePort			0x9c04
#define USERDIAG_nRPCUDSREQDestPort				0x9c14
#define USERDIAG_nRPCUDSRESPSourcePort		0x9c05
#define USERDIAG_nRPCUDSRESPDestPort			0x9c15
//#define USERDIAG_nEnableUART
#define USERDIAG_nEnableCAN
//#define USERDIAG_nEnableENET
#define USERDIAG_nEnableUSB

#define USER_DIAG_nCIDInfo																																																	\
{																																																														\
	{ 0x8000,	(puint8)&USERCAL_stRAMCAL.userCalADRefH, 								sizeof(GPM6_ttVolts), 	4,	DIAGAPI_enSecLevelInactive},	\
	{ 0x8001,	(puint8)&USERCAL_stRAMCAL.userCalVRef445, 							sizeof(GPM6_ttVolts), 	4,	DIAGAPI_enSecLevelInactive},	\
	{ 0x8002,	(puint8)&USERCAL_stRAMCAL.userCalVNernstTarget, 				sizeof(GPM6_ttVolts), 	4,	DIAGAPI_enSecLevelInactive},	\
	{ 0x8003,	(puint8)&USERCAL_stRAMCAL.userCalRNernstReady, 					sizeof(GPM6_ttVolts), 	4,	DIAGAPI_enSecLevelInactive},	\
	{ 0x8004,	(puint8)&USERCAL_stRAMCAL.au32UserCalPumpCurrentPFactor, 2 * sizeof(uint32), 	4,	DIAGAPI_enSecLevelInactive}		\
}

#define USERDIAG_nMode1PIDOffsets	\
{\
	0,\
	4,\
	8,\
	10,\
	12,\
	13,\
	14,\
	15,\
	16,\
	17,\
	18,\
	19,\
	20,\
	22,\
	23,\
	24,\
	25,\
	27,\
	28,\
	29,\
	30,\
	32,\
	34,\
	36,\
	38,\
	40,\
	42,\
	44,\
	46,\
	47,\
	48,\
	49,\
	51,\
	55,\
	57,\
	59,\
	61,\
	65,\
	69,\
	73,\
	77,\
	81,\
	85,\
	89,\
	93,\
	94,\
	95,\
	96,\
	97,\
	98,\
	100,\
	102,\
	103,\
	107,\
	111,\
	115,\
	119,\
	123,\
	127,\
	131,\
	135,\
	137,\
	139,\
	141,\
	143,\
	147,\
	151,\
	153,\
	155,\
	157,\
	158,\
	159,\
	160,\
	161,\
	162,\
	163,\
	164,\
	165,\
	167,\
	169,\
	173,\
	177,\
	178,\
	179,\
	181,\
	183,\
	185,\
	187,\
	189,\
	191,\
	193,\
	194,\
	195,\
	196,\
	198,\
	200,\
	201\
}

#define USERDIAG_nMode1BufferSize 256

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _USERDIAG_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define DIAG_nSPREADSMAX 5
#define DIAG_nSPREADSRECORDSMAX 50

EXTERN SPREADAPI_tstSpreadResult DIAG_astSpreadResult[DIAG_nSPREADSMAX];
//ASAM mode=readvalue name="_DIAG Spread Result Table 0" type=uint32 offset=0 min=0 max=1 m=1 b=0 units="dl" format=6.0 help="Internal Spread Result Table"  
//ASAM mode=readvalue name="_DIAG Spread Result Table 1" type=uint32 offset=4 min=0 max=1 m=1 b=0 units="dl" format=6.0 help="Internal Spread Result Table"  
//ASAM mode=readvalue name="_DIAG Spread Result Table 2" type=uint32 offset=8 min=0 max=1 m=1 b=0 units="dl" format=6.0 help="Internal Spread Result Table"  
//ASAM mode=readvalue name="_DIAG Spread Result Table 3" type=uint32 offset=12 min=0 max=1 m=1 b=0 units="dl" format=6.0 help="Internal Spread Result Table" 

EXTERN uint8* DIAG_apu8SpreadTableAddresses[DIAG_nSPREADSMAX];
//ASAM mode=writevalue name="_DIAG Spread Address Table 0" type=uint32 offset=0 min=0 max=1 m=1 b=0 units="dl" format=6.0 help="Internal Spread Address Table"  
//ASAM mode=writevalue name="_DIAG Spread Address Table 1" type=uint32 offset=4 min=0 max=1 m=1 b=0 units="dl" format=6.0 help="Internal Spread Address Table"  
//ASAM mode=writevalue name="_DIAG Spread Address Table 2" type=uint32 offset=8 min=0 max=1 m=1 b=0 units="dl" format=6.0 help="Internal Spread Address Table"  
//ASAM mode=writevalue name="_DIAG Spread Address Table 3" type=uint32 offset=12 min=0 max=1 m=1 b=0 units="dl" format=6.0 help="Internal Spread Address Table"  
EXTERN USERDIAG_tstSpreadIDXAddressPair DIAG_astIDXAddressPairs[DIAG_nSPREADSRECORDSMAX];
 


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void USERDIAG_vStart(uint32* const pu32Arg);
void USERDIAG_vRun(uint32* const pu32Arg);
void USERDIAG_vTerminate(uint32* const pu32Arg);
void USERDIAG_vCallBack(uint32* const pu32Arg);
void USERDIAG_vAddIDXAddressPair(USERDIAG_tstSpreadIDXAddressPair* pstSpreadIDXAddressPair);

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

