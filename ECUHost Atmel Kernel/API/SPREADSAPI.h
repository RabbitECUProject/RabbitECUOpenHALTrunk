/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          SpreadsAPI                                             */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed spreads           */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          SpreadsAPI.h                                           */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SPREADSAPI_H
#define SPREADSAPI_H

#include "types.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef sint16 SPREADAPI_ttSpreadIDX;/*CR1_111*/

typedef BUILDPACKEDKEIL struct
{
	uint16 u16SpreadIndex;
	uint16 u16SpreadOffset;
} SPREADAPI_tstSpreadResult;

typedef void (*SPREADAPI_pfPIDResultCB)(SPREADAPI_tstSpreadResult);

typedef enum
{
	SPREADAPI_enSpread1ms = 0,
	SPREADAPI_enSpread2ms = 1,
	SPREADAPI_enSpread4ms = 2,
	SPREADAPI_enSpread8ms = 3,
	SPREADAPI_enSpread16ms = 4,
	SPREADAPI_enSpread32ms = 5,
	SPREADAPI_enSpread64ms = 6,
	SPREADAPI_enSpread128ms = 7,
	SPREADAPI_enSpread256ms = 8,
	SPREADAPI_enSpread512ms = 9,	
	SPREADAPI_enSpread1024ms = 10,
	SPREADAPI_enSpreadCall = 11,	
} SPREADAPI_tenSpreadRate;

typedef BUILDPACKEDKEIL struct
{
	void* pvSourceData;
	void* pvSpreadData;
	TYPE_tenDataType enDataType;
	sint16 s16SpreadSize;
	SPREADAPI_tenSpreadRate enSpreadRate;
	SPREADAPI_pfPIDResultCB pfResultCB;
} SPREADAPI_tstSpreadCB;

#endif //SPREADSAPI_H

