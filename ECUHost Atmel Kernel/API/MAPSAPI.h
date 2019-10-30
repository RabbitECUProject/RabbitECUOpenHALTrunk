/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          MapsAPI                                                */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed maps              */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          MapsAPI.h                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef MAPSAPI_H
#define MAPSAPI_H

#include "types.h"
#include "SPREADSAPI.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef sint16 MAPSAPI_ttMapIDX;/*CR1_118*/
typedef void (*MAPSAPI_pfPIDResultCB)(sint32);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

typedef __packed struct
{
	void* pvMapData;
	void* pvOutputData;
	TYPE_tenDataType enDataType;
	sint16 i16MapSizeX;
	sint16 i16MapSizeY;
	SPREADAPI_ttSpreadIDX tSpreadIDXX;
	SPREADAPI_ttSpreadIDX tSpreadIDXY;
	MAPSAPI_pfPIDResultCB pfResultCB;
} MAPSAPI_tstMapCB;

#pragma GCC diagnostic pop

#endif //MAPSAPI_H

