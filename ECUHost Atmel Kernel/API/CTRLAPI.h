/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          CtrlAPI                                                */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed controllers       */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          CtrlAPI.h                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CTRLAPI_H
#define CTRLAPI_H

#include "types.h"	

/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef void (*CTRLAPI_pfPIDResultCB)(uint32);
typedef sint16 CTRLAPI_ttPIDIDX;/*CR1_95*/

typedef enum
{
	CTRLAPI_enPID,
	CTRLAPI_enFIC
} CTRLAPI_tenCTRLType;

typedef enum
{
	CTRLAPI_enPID1ms = 0,
	CTRLAPI_enPID2ms = 1,
	CTRLAPI_enPID4ms = 2,
	CTRLAPI_enPID8ms = 3,
	CTRLAPI_enPID16ms = 4,
	CTRLAPI_enPID32ms = 5,
	CTRLAPI_enPID64ms = 6,
	CTRLAPI_enPID128ms = 7,
	CTRLAPI_enPID256ms = 8,
	CTRLAPI_enPID512ms = 9,	
	CTRLAPI_enPID1024ms = 10,
	CTRLAPI_enPIDCall = 11,	
} CTRLAPI_tenPIDRate;


typedef __packed struct
{
	sint32* pi32Target;
	sint32* pi32Feedback;
	sint32 i32Error;
	sint32 i32OldError;
	sint32 i32Integral;
	sint32 i32IntegralInput;
	uint8 u8PTerm;
	uint8 u8ITerm;
	uint8 u8DTerm;
	sint32* pi32Output;
	sint32 i32OutputMin;
	sint32 i32OutputMax;
	bool boNoWindUp;
	bool boResetIntegrator;
	bool boRun;
	bool boReset;
	CTRLAPI_tenPIDRate enPIDRate;
	CTRLAPI_pfPIDResultCB pfResultCB;
} CTRLAPI_tstPIDCB;
	

#endif //CTRLAPI_H

