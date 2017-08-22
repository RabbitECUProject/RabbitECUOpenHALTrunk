/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Fuel Film Model Header File                   					*/
/* DESCRIPTION:        This code module calculates the fuel film load 				*/
/*										 for manifold wetting compensation											*/
/*																																						*/
/* FILE NAME:          FILM.h                                             		*/
/* REVISION HISTORY:   28-04-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef FILM_H
#define FILM_H

#include <string.h>
#include "IOAPI.h"
#include "USER.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/

	
#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _FILM_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN sint32 FILM_s32FilmLoadUgDeltaApplied;		
		
/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void FILM_vStart(uint32 * const pu32Arg);
void FILM_vRun(uint32* const pu32Arg);
void FILM_vTerminate(uint32* const pu32Arg);
void FILM_vCallBack(puint32 const pu32Arg);

#endif // MAP_H
