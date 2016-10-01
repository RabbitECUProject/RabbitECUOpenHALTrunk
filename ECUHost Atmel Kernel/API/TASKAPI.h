/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          TaskAPI                                                */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed application tasks */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          TaskAPI.h                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TASKAPI_H
#define TASKAPI_H

#include "types.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef enum
{	
	TASKAPI_enTaskPrMaxAll = 0u,
	TASKAPI_enTaskPrMaxCyc = 128u,
	TASKAPI_enTaskPrLowest = 255u,
}	TASKAPI_tenPriority;/*CR1_120*/

typedef enum
{
	TASKAPI_enTaskCycBad = 0u,
	TASKAPI_enTask1ms = 1u,
	TASKAPI_enTask2ms = 2u,
	TASKAPI_enTask5ms = 5u,
	TASKAPI_enTask10ms = 10u,
	TASKAPI_enTask20ms = 20u,	
	TASKAPI_enTask25ms = 25u,
	TASKAPI_enTask50ms = 50u,	
	TASKAPI_enTask100ms = 100u,
	TASKAPI_enTask200ms = 200u,
	TASKAPI_enTask250ms = 250u,	
	TASKAPI_enTaskCycNot = 255u
} TASKAPI_tenRateMs;/*CR1_120*/

	
#endif //TASKAPI_H
	

