/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Units Header File                                      */
/* DESCRIPTION:        This code module SI typedefs for variables with        */
/*                     various resolutions                                    */
/*                                                                            */
/* FILE NAME:          units.h                                                */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef UNITS_H
#define UNITS_H

#include <stddef.h>
#include <limits.h>
#include "sys.h"
#include "types.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
typedef uint16 GPM6_ttLambda;
typedef uint32 GPM6_ttVolts;
typedef uint32 GPM6_ttMilliVolts;
typedef uint32 GPM6_ttMicroVolts;
typedef uint32 GPM6_ttOhms;
typedef uint32 GPM6_ttAmps;
typedef sint32 GPM6_ttMilliAmps;
typedef sint32 GPM6_ttMicroAmps;
typedef uint32 GPM6_ttWatts;
typedef uint32 GPM6_ttMilliWatts;
typedef uint32 GPM6_ttMicroWatts;
typedef uint32 GPM6_ttKiloPa;
typedef uint32 GPM6_ttPa;
typedef uint32 GPM6_ttTheta;
typedef uint32 GPM6_ttMTheta;
typedef sint32 GPM6_ttTempC;
typedef uint32 GPM6_ttPulseUs;
typedef uint32 GPM6_ttG;
typedef uint32 GPM6_ttNg;
typedef uint32 GPM6_ttUg;
typedef uint32 GPM6_ttMg;
typedef uint32 GPM6_ttCc;
typedef uint32 GPM6_ttMcc;
typedef uint32 GPM6_ttUcc;
/*CR1_124*/

#define GPM6_ttLambdaMAXVAL 		USHRT_MAX
#define GPM6_ttVoltsMAXVAL 			UINT_MAX
#define GPM6_ttMilliVoltsMAXVAL UINT_MAX
#define GPM6_ttMicroVoltsMAXVAL UINT_MAX
#define GPM6_ttOhmsMAXVAL 			UINT_MAX
#define GPM6_ttAmpsMAXVAL 			UINT_MAX
#define GPM6_ttMilliAmpsMAXVAL 	INT_MAX
#define GPM6_ttMicroAmpsMAXVAL 	INT_MAX
#define GPM6_ttWattsMAXVAL 			UINT_MAX
#define GPM6_ttMilliWattsMAXVAL UINT_MAX
#define GPM6_ttMicroWattsMAXVAL UINT_MAX
#define GPM6_ttPaMAXVAL					UINT_MAX
#define GPM6_ttThetaMAXVAL			UINT_MAX
#define GPM6_ttTempCMAXVAL			SINT_MAX

#endif // UNITS_H

