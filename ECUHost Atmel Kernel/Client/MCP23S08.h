/******************************************************************************/
/*                Copyright (c) 2017 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      MCP23S08                                               */
/* DESCRIPTION:        This code module provides functions to invoke the SPI  */
/*                     data transfers to/from the MCP23S08 device             */
/*                                                                            */
/* FILE NAME:          MCP23S08.c                                             */
/* REVISION HISTORY:   11-01-2017 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef MCP23S08_H
#define MCP23S08_H

#include <string.h>
#include "IOAPI.h"
#include "USER.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define MCP23S08_TXRXBufferSize	16u

#define MCP23S08_nCmdWrite 0x46
#define MCP23S08_nCmdRead  0x47

#define MCP23S08_nRegIODIR   0x00
#define MCP23S08_nRegIPOL    0x01
#define MCP23S08_nRegGPINTEN 0x02
#define MCP23S08_nRegDEFVAL  0x03
#define MCP23S08_nRegINTCON  0x04
#define MCP23S08_nRegIOCON   0x05
#define MCP23S08_nRegGPPU    0x06
#define MCP23S08_nRegINTF    0x07
#define MCP23S08_nRegINTCAP  0x08
#define MCP23S08_nRegGPIO    0x09
#define MCP23S08_nRegOLAT    0x0a

#define MCP23S08_nMaskAllOutput 0

typedef void (*MCP23S08_tpfCB)(puint32);
		
		
/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void MCP23S08_vStart(uint32 * const pu32Arg);
void MCP23S08_vRun(uint32* const pu32Arg);
void MCP23S08_vTerminate(uint32* const pu32Arg);
void MCP23S08_vThread(void);
void MCP23S08_vCallBack(puint32 const pu32Arg);
Bool MCP23S08_boSetCallback(MCP23S08_tpfCB pfCB);

/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Return Value     : <Comment on return value >
*******************************************************************************/	
Bool MCP23S08_boTransferData(IOAPI_enTransferType enTransferType, void* pvData, uint32 u32DataCount);

/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Return Value     : <Comment on return value >
*******************************************************************************/	
void MCP23S08_vSetDataDirection(uint8 u8DirMask1);

#endif // MCP23S08_H

