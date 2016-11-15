/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          ResourceAPI                                            */
/* DESCRIPTION:        This API provides type definitions for off micro       */
/*                     resources                                              */
/*                                                                            */
/*                                                                            */
/* FILE NAME:          SpreadsAPI.h                                           */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "sys.h"


#ifndef RESAPI_H
#define RESAPI_H

#include "types.h"

typedef enum
{
	SDHC_MEM,
	SPI_MEM,
	RES_Count
} RES_tenResource;

#endif //RESM_H



