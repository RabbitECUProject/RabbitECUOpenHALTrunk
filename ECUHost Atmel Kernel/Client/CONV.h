/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Conversion Header File                    							*/
/* DESCRIPTION:        This code module provides data conversion functions  	*/
/*                     functions for the user application   									*/
/* FILE NAME:          CONV.h                                             		*/
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef CONV_H
#define CONV_H

#include <stddef.h>
#include <string.h>
#include "declarations.h"
#include "ioapi.h"/*CR1_94*/
#include "sys.h"
#include "types.h"
#include "units.h"
#include "usercal.h"


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Return Value     : <Comment on return value >
*******************************************************************************/
GPM6_ttVolts CONV_tADCToVolts(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);/*CR1_1*/

/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Return Value     : <Comment on return value >
*******************************************************************************/
GPM6_ttOhms CONV_tVoltsAmpsToOhms(GPM6_ttVolts tVolts, GPM6_ttAmps tAmps);/*CR1_1*/

/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Return Value     : <Comment on return value >
*******************************************************************************/
GPM6_ttAmps CONV_tOhmsVoltsToAmps(GPM6_ttOhms tOhms, GPM6_ttVolts tVolts);/*CR1_1*/

/*******************************************************************************
* Interface        : <Description of interface>
*
* Implementation   : <Description of implementation>
*
* Return Value     : <Comment on return value >
*******************************************************************************/
GPM6_ttOhms CONV_tADCToOhms(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);/*CR1_67*/

#endif // CONV_H

