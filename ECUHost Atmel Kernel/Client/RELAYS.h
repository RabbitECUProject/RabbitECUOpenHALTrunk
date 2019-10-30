/******************************************************************************/
/*                Copyright (c) 2017 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      RELAYS header file                                     */
/* DESCRIPTION:        This code module initialises the required resources    */
/*                     for driving the user application relays and functions  */
/*                     for controlling the on/off state                       */
/*                                                                            */
/* FILE NAME:          RELAYS.c                                               */
/* REVISION HISTORY:   11-01-2017 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef RELAYS_H
#define RELAYS_H


#include "declarations.h"
#include "MCP23S08.h"
#include "IOAPI.h"
#include "SYSAPI.h"
#include "USER.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define RELAYS_nSPIFrequency 2000000u
#define RELAYS_nSPI_CS_RESOURCE EH_IO_IO22
#define RELAYS_TELLTALE_1 EH_IO_IO41
#define RELAYS_TELLTALE_2 EH_IO_IO43

#undef EXTERN
#ifdef _RELAYS_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* GLOBAL TYPE DEFINITIONS ****************************************************/
/*******************************************************************************
* Description      : Type to hold an RELAYS character configuration
*******************************************************************************/
typedef enum
{
	RELAY_BIT0 = 1,
	RELAY_BIT1 = 2,
	RELAY_BIT2 = 4,
	RELAY_BIT3 = 8,
	RELAY_BIT4 = 16,
	RELAY_BIT5 = 32,
	RELAY_BIT6 = 64,
	RELAY_BIT7 = 128,
} RELAY_tenBit;

typedef enum
{
	NO_OUT,
	AND_NEXT,
	OR_NEXT,
	NOT_NEXT,
	XOR_NEXT,
	OUT1_ON,
	OUT1_OFF,
	OUT2_ON,
	OUT2_OFF,
	OUT3_ON,
	OUT3_OFF,
	OUT4_ON,
	OUT4_OFF,
	OUT5_ON,
	OUT5_OFF,
	OUT6_ON,
	OUT6_OFF,
	OUT7_ON,
	OUT7_OFF,
	OUT8_ON,
	OUT8_OFF,
	OUT9_ON,
	OUT9_OFF,
	OUT10_ON,
	OUT10_OFF
} RELAYS_tenChainOutput;

typedef enum
{
	 GT,
	 LT,
	 EE,
	 NE
} RELAYS_tenCompare;

typedef enum
{
	REL_INVALID,
	REL_HIGH,
	REL_LOW,
	REL_TOGGLE,
} RELAYS_tenAction;

#define RELAY_MAP \
{\
	RELAY_BIT5,\
	RELAY_BIT4,\
	RELAY_BIT6,\
	RELAY_BIT7,\
	RELAY_BIT2,\
	RELAY_BIT3,\
	RELAY_BIT1,\
	RELAY_BIT0\
}

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void RELAYS_vStart(uint32 * const pu32Arg );
void RELAYS_vRun(uint32* const pu32Arg );
void RELAYS_vTerminate(uint32* const pu32Arg );
void RELAYS_vCallBack(uint32* const pu32Arg );
void RELAYS_vOutputBit(RELAY_tenBit enBit, Bool boBitOn);
void RELAYS_vThread(void);

#endif // RELAYS_H

