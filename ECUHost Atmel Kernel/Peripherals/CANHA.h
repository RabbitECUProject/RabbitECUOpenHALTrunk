/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CANHA Header File                                      */
/* DESCRIPTION:        CAN Hardware Adaption                                  */
/* FILE NAME:          CANHA.h                                                */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CANHA_H
#define CANHA_H

#include "types.h"
#include "IOAPI.h"
#include "SYSAPI.h"

#define CANHA_nSetTimingData            \
{                                       \
	{ 33333,    59,   10,  7,  2 },     \
	{ 50000,    58,   7,   7,  2 },     \
	{ 62500,    49,   7,   6,  2 },		\
	{ 83333,    49,   5,   4,  2 },		\
	{ 100000,   49,   4,   3,  2 },		\
	{ 125000,   24,   7,   6,  2 },		\
	{ 250000,   9,    10,  7,  2 },		\
	{ 500000,   9,    4,   2,  1 },		\
	{ 1000000,  4,    6,   1,  1 }		\
}


#define CAN_nCANModules        2
#define CAN_nInRingBuffSize    16u
#define CAN_nCANMailboxCount   16u
#define CAN_nCANRXMailboxCount 14u


#define CAN_nCANMBXInactive1   0x00
#define CAN_nCANMBXBusy        0x01
#define CAN_nCANMBXFull        0x02
#define CAN_nCANMBXEmpty       0x04
#define CAN_nCANMBXOverrun     0x06
#define CAN_nCANMBXInactive2   0x08
#define CAN_nCANMBXAbort       0x09
#define CAN_nCANMBXRAnswer     0x0A
#define CAN_nCANMBXData        0x0C
#define CAN_nCANMBXTAnswer     0x0E

#define CANHA_nCANIDSTDSHIFT   18

typedef struct
{
	uint32 u32BitRate;
	uint8 u8PRESDIV;
	uint8 u8TSEG1;
	uint8 u8TSEG2;
	uint8 u8SJW;
} CANHA_tstTimingSettings;

typedef struct
{
	uint32 biTimestamp : 16;
	uint32 biDLC : 4;
	uint32 biRTR : 1;
	uint32 biIDE : 1;
	uint32 biSRR : 1;
	uint32 biCAN : 1;
	uint32 biTimestampHighNibble : 4;
	uint32 biChannel : 4;
} CANHA_tstCANCS;

#ifdef BUILD_MK60
typedef struct  
{
	uint32 u32CS;
	uint32 u32ID;
	uint32 u32DWH;
	uint32 u32DWL;
} CANHA_tstCANMB;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
typedef struct
{
    uint32 u32MR;
	uint32 u32AM;
	uint32 u32ID;
	uint32 u32FID;
	uint32 u32CS;
	uint32 u32DWL;
	uint32 u32DWH;
	uint32 u32CR;
} CANHA_tstCANMB;
#endif //BUILD_SAM3X8E


#ifdef BUILD_MK60
#include "mk60f12.h"
typedef CAN_Type tstCANModule;
#endif
#ifdef BUILD_SAM3X8E
#include "sam3xa.h"
typedef Can tstCANModule;
#define CAN_MB_TYPE_NDIS 0
#define CAN_MB_TYPE_RX 1
#define CAN_MB_TYPE_TX 3
#endif

void CANHA_vStart(uint32* const);
void CANHA_vRun(uint32* const);
void CANHA_vTerminate(uint32* const);
void CANHA_vInitTransfer(IOAPI_tstTransferCB*);
uint32 CANHA_u32InitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
Bool CANHA_boReadMB(tstCANModule*, CANHA_tstCANMB*);
void CANHA_vInterrupt(IOAPI_tenEHIOResource);

#endif //CANHA_H

