/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          TablesAPI                                              */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed tables            */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          TablesAPI.h                                            */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TABLESAPI_H
#define TABLESAPI_H

#include "types.h"
#include "SPREADSAPI.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef sint16 TABLEAPI_ttTableIDX;/*CR1_118*/
typedef void (*TABLEAPI_pfPIDResultCB)(sint32);

typedef struct
{
	void* pvTableData;
	void* pvOutputData;
	TYPE_tenDataType enDataType;
	sint16 i16TableSize;
	SPREADAPI_ttSpreadIDX tSpreadIDX;/*CR1_119*/
	TABLEAPI_pfPIDResultCB pfResultCB;
} TABLEAPI_tstTableCB;

#endif //TABLESAPI_H

