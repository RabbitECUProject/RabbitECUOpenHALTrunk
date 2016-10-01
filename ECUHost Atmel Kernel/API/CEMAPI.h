/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          CEMAPI                                                 */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel Cyclic Event Manager      */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          CEMAPI.h                                               */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CEMAPI_H
#define CEMAPI_H


/* GLOBAL MACRO DEFINITIONS ***************************************************/


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef uint16 CEMAPI_ttEventTime;
typedef void (*CEMAPI_tpfEventCB)(IOAPI_tenEHIOResource, CEMAPI_ttEventTime);

typedef enum
{
	CEMAPI_enHardwareRising,
	CEMAPI_enHardwareFalling,
	CEMAPI_enHardwareAny	
} CEMAPI_tenEventTrigger;

typedef struct
{
	uint32 u32CycleDuration;
	puint32 pu32EventProgram;
	puint32 pu32CallbackProgram;
	IOAPI_tenEHIOResource enIOHardwareTrigger;
} IOAPI_tenCEMCycleCB;
	
	
	
	
	
	
	




#endif //CEMAPI_H

