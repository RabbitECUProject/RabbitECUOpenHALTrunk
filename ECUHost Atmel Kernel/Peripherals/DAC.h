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
#ifndef DAC_H
#define DAC_H

#include "CPUAbstract.h"
#include "DACAPI.h"
#include "DACHA.h"
#include "declarations.h"
#include "IOAPI.h"
#include "os.h"
#include "regset.h"
#include "SIM.h"
#include "SYSAPI.h"
#include "sys.h"
#include "types.h"

#define DAC_nQueueSize	16u

void DAC_vStart(puint32 const);
void DAC_vRun(puint32 const);
void DAC_vTerminate(puint32 const);
SYSAPI_tenSVCResult DAC_vInitDACResource(IOAPI_tenEHIOResource, DACAPI_tstDACCB*);
void DAC_vWriteDACQueue(IOAPI_tenEHIOResource, DACAPI_ttOutputVoltage*);	
#endif //DAC_H

