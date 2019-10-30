/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Setup                                                  */
/* DESCRIPTION:        This code assist with the setup of kernel managed      */
/*                     resources                                              */
/*                                                                            */
/* FILE NAME:          BOOSTED.c                                              */
/* REVISION HISTORY:   14-04-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _BOOSTED_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "BOOSTED.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
Bool BOOSTED_boIndexAndCalculateMap(SPREADAPI_ttSpreadIDX tSpreadxIDX, SPREADAPI_ttSpreadIDX tSpreadyIDX, MAPSAPI_ttMapIDX tMapIDX)
{
    Bool boRetVal = false;

	if ((-1 != tSpreadxIDX) && (-1 != tSpreadyIDX) && (-1 != tMapIDX))
	{
		/* Calculate the current spread for Map IDXx */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&tSpreadxIDX,
		NULL, NULL);
	    
		/* Calculate the current spread for Map IDXy */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&tSpreadyIDX,
		NULL, NULL);

		/* Lookup the current value for Map */
		USER_vSVC(SYSAPI_enCalculateMap, (void*)&tMapIDX,
		NULL, NULL);

		boRetVal = true;
	}

	return boRetVal;
}

Bool BOOSTED_boIndexAndCalculateTable(SPREADAPI_ttSpreadIDX tSpreadIDX, TABLEAPI_ttTableIDX tTableIDX)
{
	Bool boRetVal = false;

	if (-1 != tSpreadIDX)
	{
		/* Calculate the current spread for Table IDX */
		USER_vSVC(SYSAPI_enCalculateSpread, (void*)&tSpreadIDX,
		NULL, NULL);		

		/* Lookup the current value for Table */
		USER_vSVC(SYSAPI_enCalculateTable, (void*)&tTableIDX,
		NULL, NULL);

		boRetVal = true;
	}

	return boRetVal;
}

SPREADAPI_tstSpreadResult* BOOSTED_pstGetSpread(SPREADAPI_ttSpreadIDX tSpreadxIDX)
{
    SPREADAPI_ttSpreadIDX stSpreadxIDX = tSpreadxIDX;

    USER_vSVC(SYSAPI_enGetSpreadResult, (void*)&stSpreadxIDX,
				NULL, NULL);

    return OS_stSVCDataStruct.pvArg1;
}

void BOOSTED_vEnableWatchdog(uint16 u16ReloadCount, uint16 u16WindowCount)
{
    USER_vSVC(SYSAPI_enSetupWatchdog, (void*)&u16ReloadCount,
    (void*)&u16WindowCount, NULL);
}

#endif //BUILD_USER