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
#ifndef PACKEDDLL_H
#define PACKEDDLL_H

#define TXRX_MSB_DELIM_SIZE	12u

void PACKEDDLL_vPack(puint8);
void PACKEDDLL_vUnpack(DLL_tstRXDLLData*);

#endif //PACKEDDLL_H

