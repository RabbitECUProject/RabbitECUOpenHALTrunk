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
#ifndef DACHA_H
#define DACHA_H

#include "CPUAbstract.h"
#include "regset.h"
#include "sys.h"
#include "declarations.h"
#include "os.h"
#include "IOAPI.h"
#include "DACAPI.h"
#include "SYSAPI.h"
#include "types.h"
#include "SIM.h"

#define DACHA_nDACCount 		2u

#ifdef BUILD_MK60
#include "mk60f12.h"
typedef DAC_Type tstDACModule;
#endif
#ifdef BUILD_SAM3X8E
#include "sam3x8e.h"
typedef Dacc tstDACModule;
#endif

void DACHA_vStart(puint32 const);
void DACHA_vRun(puint32 const);
void DACHA_vTerminate(puint32 const);
SYSAPI_tenSVCResult DACHA_vInitDACResource(IOAPI_tenEHIOResource, DACAPI_tstDACCB*);
void DACHA_vWriteDACQueue(IOAPI_tenEHIOResource, DACAPI_ttOutputVoltage*);	
#endif //DACHA_H

