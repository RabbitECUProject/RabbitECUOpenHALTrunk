/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      XXX Header File                    										*/
/* DESCRIPTION:        XXX   																									*/
/* FILE NAME:          XXX.h                                         					*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef COMMONNL_H
#define COMMONNL_H

#include "TYPES.h"

#define COMMONNL_RX_BUFF_LARGE_SIZE		1030u
#define COMMONNL_TX_BUFF_LARGE_SIZE		258u
#define COMMONNL_RX_BUFF_SMALL_SIZE		128u
#define COMMONNL_TX_BUFF_SMALL_SIZE		128u

typedef struct
{
	uint8 u8Data[COMMONNL_RX_BUFF_LARGE_SIZE];
	uint8* pu8RX;
	uint16 u16RXRequestedCount;
	uint16 u16RXReceivedCount;
	uint8 u8SeqNumber;
	uint8 u8RXBlock;
	uint8 biRXErr : 1;
	uint8 biRXOK : 1;
	uint8 biSpare : 6;
} COMMONNL_tstRXLargeBuffer;

typedef struct
{
	uint8 u8Data[COMMONNL_TX_BUFF_LARGE_SIZE];
	uint8* pu8TX;
	uint16 u16TXRequestedCount;
	uint16 u16TXTransmittedCount;
	uint8 u8SeqNumber;
	uint8 u8TXBlock;
	uint8 biTXErr : 1;
	uint8 biTXOK : 1;
	uint8 biSpare : 6;
} COMMONNL_tstTXLargeBuffer;

typedef struct
{
	uint8 u8Data[COMMONNL_RX_BUFF_SMALL_SIZE];
	puint8 pu8RX;
	uint8 u8RXRequestedCount;
	uint8 u8RXReceivedCount;
} COMMONNL_tstRXSmallBuffer;

typedef struct
{
	uint8 u8Data[COMMONNL_TX_BUFF_SMALL_SIZE];
	puint8 pu8TX;
	uint8 u8TXRequestedCount;
	uint8 u8TXReceivedCount;
} COMMONNL_tstTXSmallBuffer;

#endif //COMMONNL_H

