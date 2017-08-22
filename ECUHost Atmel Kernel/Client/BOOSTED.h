/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Setup Header                                           */
/* DESCRIPTION:        This code assist with the setup of kernel managed      */
/*                     resources                                              */
/*                                                                            */
/* FILE NAME:          BOOSTED.c                                              */
/* REVISION HISTORY:   14-04-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef BOOSTED_H
#define BOOSTED_H

#include <string.h>
#include "IOAPI.h"
#include "MAPSAPI.h"
#include "USER.h"
#include "SYS.h"
#include "SPREADSAPI.h"
#include "TABLESAPI.h"
#include "TEPMAPI.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
Bool BOOSTED_boIndexAndCalculateMap(SPREADAPI_ttSpreadIDX, SPREADAPI_ttSpreadIDX, MAPSAPI_ttMapIDX);
SPREADAPI_tstSpreadResult* BOOSTED_pstGetSpread(SPREADAPI_ttSpreadIDX);

#endif // BOOSTED_H

