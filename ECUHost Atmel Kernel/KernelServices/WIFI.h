/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                               							*/
/* PACKAGE TITLE:      XXX Header File                    										*/
/* DESCRIPTION:        XXX   																									*/
/* FILE NAME:          XXX.h                                         					*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef WIFI_H
#define WIFI_H

#include "sys.h"
#include "ENE.h"
#include "RESM.h"

/*<AUTOGEN START>*/
#define WIFI_nReg32Set 																																																										\
{																																																																					\
	{ (volatile uint32*)( PORTA_BASE + offsetof( PORT_Type, PCR[  5 ] ) ), (uint32)PORT_PCR_MUX(4), REGSET_enOr }, 													\
{ NULL, 0, REGSET_enOverwrite }																																																						\
};
/*<AUTOGEN END>*/

#define WIFI_nUARTConfigCB								\
{																					\
	NULL, NULL,															\
	KERNEL_CPURX, KERNEL_CPUTX, (IOAPI_tenEHIOResource)NULL, (IOAPI_tenEHIOResource)NULL,	\
	IOAPI_enPortSerialTransfer,							\
	2 * 2400,																\
	10,																			\
	EH_VIO_UART1,														\
	PROTAPI_enLLUARTSZDelim,										\
	PROTAPI_enNLNone,													\
	PROTAPI_enTLNone,													\
	(IOAPI_tpfTransferCB)WIFI_vReceive,													\
	(IOAPI_tpfTransferCB)NULL																		\
} 

#if BUILD_KERNEL
	#define WIFI_nReportCount				100u
#elif BUILD_KERNEL_APP
	#define WIFI_nReportCount				100u
#else
	#define WIFI_nReportCount				1u
#endif

#define WIFI_nReportLengthMax			64u
#define WIFI_nPromptStringSize		25u
#define WIFI_nSSIDECUHOSTWLAN 		"ECUHOSTWLAN"

#if BUILD_PBL
	#define WIFI_xGetClientHandle()	0xf0000000
#elif BUILD_SBL
	#define WIFI_xGetClientHandle()	0xf0000000
#elif BUILD_KERNEL
	#define WIFI_xGetClientHandle()	OS_stSVCDataStruct.tClientHandle | 0xf0000000
#elif BUILD_KERNEL_APP
	#define WIFI_xGetClientHandle()	OS_stSVCDataStruct.tClientHandle | 0xf0000000
#endif

// "AT+iBDRA\r"
// "AT+iWST0=0\r"
// "AT+iBDRF=7\r"
// "AT+iWLCH=1\r"
// "AT+iWLSI=!ECUHOSTWLAN\r"
// "AT+iDIP=192.168.11.2\r"
// "AT+iBRM=3\r"

#define WIFI_nATPLUSi_CONFIG	\
{															\
"AT+iFD\r",										\
"AT+iBRDA\r",									\
"AT+iBDRF=1\r",								\
"AT+iHIF=1\r",								\
"AT+iSTAP=1\r",								\
"AT+iLTYP=4\r",								\
"AT+iSWEN=1\r",								\
"AT+iDPSZ=10\r",							\
"AT+iWLSI=OPTUS_E3660\r",			\
"AT+iWST0=4\r",								\
"AT+iWPP0=5108701550\r",			\
"AT+iDIP=10.0.0.1\r",					\
"AT+iSNET=255.255.255.0\r",		\
"AT+iEDIP=10.0.0.100\r",			\
"AT+iESNT=255.255.255.0\r",		\
"AT+iCEMC=1\r",								\
"AT+iDOWN\r"									\
}

#define WIFI_nATPLUSi_REPORTS	\
{															\
"AT+iRP0\r",									\
"AT+iRP2\r",									\
"AT+iRP3\r",									\
"AT+iRP4\r",									\
"AT+iRP5\r",									\
"AT+iRP6\r",									\
"AT+iRP10\r",									\
"AT+iRP11\r",									\
"AT+iRP12\r",									\
"AT+iRP13\r",									\
"AT+iRP14\r",									\
"AT+iRP20\r",									\
"AT+iRP21\r"									\
}

#define WIFI_CFG_COUNT	17
#define WIFI_REP_COUNT	13
#define WIFI_nATPLUSi_IOK "I/OK\r"
#define WIFI_nATPLUSi_IERROR "I/ERROR\r"
#define WIFI_nATPLUSi_IONLINE "I/ONLINE\r"

typedef enum
{
	WIFI_enRST,
	WIFI_enCFG,
	WIFI_enREPS,
	WIFI_enCHK,
	WIFI_enCONNECTED,
	WIFI_enERR
} WIFI_tenState;

typedef struct
{
	uint8 au8LocalIP[ 4 ];	
	uint8 au8RemoteIP[ 4 ];
	uint8 au8LocalMAC[ 6 ];	
	uint8 au8RemoteMAC[ 6 ];
	uint8 szLocalIP[ 16 ];	
	uint8 szRemoteIP[ 16 ];
	uint8 szLocalMAC[ 18 ];	
	uint8 szRemoteMAC[ 18 ];
} WIFI_tstWLAN;

void WIFI_vStart(uint32* const);
void WIFI_vRun(uint32* const);
void WIFI_vTerminate(uint32* const);
void WIFI_vReceive(IOAPI_tenEHIOResource, puint8, uint32);
bool WIFI_boConfirmECUHOSTLAN(void);
void WIFI_vReAlignETHHeader(ENE_tstETHUnionFrame**);

#endif // WIFI_H

