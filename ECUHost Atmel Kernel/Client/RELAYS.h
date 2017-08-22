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


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void RELAYS_vStart(uint32 * const pu32Arg );
void RELAYS_vRun(uint32* const pu32Arg );
void RELAYS_vTerminate(uint32* const pu32Arg );
void RELAYS_vCallBack(uint32* const pu32Arg );
void RELAYS_vThread(void);

#endif // RELAYS_H

