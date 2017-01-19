/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Setup Header                                           */
/* DESCRIPTION:        This code assist with the setup of kernel managed      */
/*                     resources                                              */
/*                                                                            */
/* FILE NAME:          SETUP.c                                                */
/* REVISION HISTORY:   14-04-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SETUP_H
#define SETUP_H

#include <string.h>
#include "IOAPI.h"
#include "USER.h"
#include "SYS.h"
#include "SPREADSAPI.h"
#include "TABLESAPI.h"
#include "TEPMAPI.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
bool SETUP_boSetupADSE(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOType enEHIOType, ADCAPI_tenSamplesAv enSamplesAv, ADCAPI_tpfResultCB pfResultCB, ADCAPI_tenTrigger enTrigger, puint32 pu32Arg);
TABLEAPI_ttTableIDX SETUP_tSetupTable(void* pvTableData, void* pvOutputData, TYPE_tenDataType enDataType, sint16 s16TableSize, SPREADAPI_ttSpreadIDX tSpreadIDX, TABLEAPI_pfPIDResultCB pfResultCB);
SPREADAPI_ttSpreadIDX SETUP_tSetupSpread(void* pvSourceData, void* pvSpreadData, TYPE_tenDataType enDataType, sint16 s16SpreadSize, SPREADAPI_tenSpreadRate enSpreadRate, SPREADAPI_pfPIDResultCB pfResultCB);
void SETUP_vSetupDigitalIO(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOType enEHIOType, IOAPI_tenDriveStrength enDriveStrength, puint32 pu32Arg);
void SETUP_vSetDigitalIOHigh(IOAPI_tenEHIOResource);
void SETUP_vSetDigitalIOLow(IOAPI_tenEHIOResource);

#endif // SETUP_H

