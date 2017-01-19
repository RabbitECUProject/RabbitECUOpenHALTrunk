/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                                                                                                           */
/* PACKAGE TITLE:      Fuel Film Model                    										*/
/* DESCRIPTION:        This code module calculates the fuel film load 				*/
/*										 for manifold wetting compensation											*/
/*																																						*/
/* FILE NAME:          FILM.c                                             		*/
/* REVISION HISTORY:   28-04-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _FILM_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "MAP.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
GPM6_ttNg FILM_tFilmLoadUg;
GPM6_ttNg FILM_tFilmLoadUgOld;
sint32 FILM_s32FilmLoadUgDelta;
uint32 FILM_u32FilmCallCount;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void FILM_vStart(puint32 const pu32Arg)
{
	FILM_tFilmLoadUg = 0;
	FILM_tFilmLoadUgOld = 0;	
	FILM_u32FilmCallCount = 0;
	FILM_s32FilmLoadUgDeltaApplied = 0;
}

void FILM_vRun(puint32 const pu32Arg)
{
	sint32 s32Temp;	
	uint32 u32Temp;
	
	FILM_u32FilmCallCount++;
	
	if (0 < CAM_u32RPMRaw)
	{
		/* Accumulate film with TPS based wall impact coefficient */
		u32Temp = (TPS_tThetaFiltered + 5000) / 950u;
		u32Temp *= FUEL_tPredictedFuelFlowUg;
		u32Temp /= 10000u;
		u32Temp += FILM_tFilmLoadUg;
	}
	else
	{
		u32Temp = FILM_tFilmLoadUg;		
	}
	
	
	/* Temporary film evaporation code */
	s32Temp = CTS_tTempCFiltered + (110000 - MAP_tKiloPaFiltered);
	s32Temp /= 5000;
	s32Temp = 1000 - s32Temp;
	u32Temp /= 100;
	u32Temp *= ((uint32)s32Temp);
	u32Temp /= 10;
		
	FILM_tFilmLoadUg = MIN(u32Temp, 15000000);	

	if (0 == (FILM_u32FilmCallCount % 0x2))	
	{
		FILM_s32FilmLoadUgDelta = (sint32)FILM_tFilmLoadUg - (sint32)FILM_tFilmLoadUgOld;
		FILM_tFilmLoadUgOld = FILM_tFilmLoadUg;		
	}
	
	/* Clip the accumulate / evaporate derivative */
	if (0 <= FILM_s32FilmLoadUgDelta)
	{
		s32Temp = (500 > FILM_s32FilmLoadUgDelta) ? 
			0 : 20 * FILM_s32FilmLoadUgDelta;
		FILM_s32FilmLoadUgDeltaApplied = (80000 < s32Temp) ? 
			80000 : s32Temp;		
	}
	else
	{
		s32Temp = (-500 < FILM_s32FilmLoadUgDelta) ? 
			0 : 20 * FILM_s32FilmLoadUgDelta;	
		FILM_s32FilmLoadUgDeltaApplied = (-80000 > s32Temp) ? 
			-80000 : s32Temp;			
	}
}

void FILM_vTerminate(puint32 const pu32Arg)
{

}


void FILM_vCallBack(puint32 const pu32Arg)
{

}


#endif //BUILD_USER