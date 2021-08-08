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
#ifndef TABLES_H
#define TABLES_H

#include "TABLESAPI.h"
#include "SYSAPI.h"

#define TABLE_nKernelTableCount	30u
#define TABLE_nUserTableCount	30u

void TABLES_vStart(puint32 const pu32Arg);
void TABLES_vRun(puint32 const pu32Arg);
void TABLES_vTerminate(puint32 const pu32Arg);
TABLEAPI_ttTableIDX TABLE_tRequestKernelTable(TABLEAPI_tstTableCB*);
Bool TABLE_vCalculate(TABLEAPI_ttTableIDX);

#endif //TABLES_H

