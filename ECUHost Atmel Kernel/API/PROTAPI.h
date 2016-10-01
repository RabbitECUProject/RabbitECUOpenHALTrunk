/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          ProtAPI                                                */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed protocol          */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          ProtAPI.h                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef PROTAPI_H
#define PROTAPI_H

#include "sys.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef enum
{
	PROTAPI_enTLKWP,
	PROTAPI_enTLUDS,
	PROTAPI_enTLUDS_UDP,
	PROTAPI_enTLJ1939,
	PROTAPI_enTLUDP,
	PROTAPI_enTLNone
} PROTAPI_tenTLProtocol;
/*CR1_109*/

typedef enum
{
	PROTAPI_enISO15765,
	PROTAPI_enISO15765_IP,
	PROTAPI_enNLNone
} PROTAPI_tenNLProtocol;
/*CR1_109*/

typedef enum
{
	PROTAPI_enLLUARTPacked,
	PROTAPI_enLLUARTSZDelim,
	PROTAPI_enLLCAN11,
	PROTAPI_enLLCAN29,
	PROTAPI_enLL802_3,
	PROTAPI_enLLNone
} PROTAPI_tenLLProtocol;
/*CR1_109*/

typedef struct
{
	uint8 u8DLC;
	uint32 u32ID;			
	uint32 u32DWH;
	uint32 u32DWL;     	
} PROTAPI_tstCANMsg;

#endif //PROTAPI_H

