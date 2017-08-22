/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef MAPS_H
#define MAPS_H

#include "MAPSAPI.h"
#include "SYSAPI.h"

#define MAP_nKernelMapCount	5u
#define MAP_nUserMapCount	5u

void MAPS_vStart(puint32 const pu32Arg);
void MAPS_vRun(puint32 const pu32Arg);
void MAPS_vTerminate(puint32 const pu32Arg);
MAPSAPI_ttMapIDX MAP_tRequestKernelMap(MAPSAPI_tstMapCB*);
Bool MAP_vCalculate(MAPSAPI_ttMapIDX);

#endif //MAPS_H

