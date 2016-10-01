/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          IICAPI                                                 */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed IIC resources     */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          IICAPI.h                                               */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef IICAPI_H
#define IICAPI_H


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef struct
{
	uint32 u32BaudRateHz;
	uint8 u8SlaveAddress;
} IICAPI_tstIICConfig;	
	

#endif //IICAPI_H

