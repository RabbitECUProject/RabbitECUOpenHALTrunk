/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Conversion                                             */
/* DESCRIPTION:        This code module provides data conversion functions    */
/*                     functions for the user application                     */
/* FILE NAME:          CONV.c                                                 */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "conv.h"


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
GPM6_ttVolts CONV_tADCToVolts(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	GPM6_ttVolts tVolts;/*CR1_17*/
	
	switch (enEHIOResource)
	{	
	    case EH_IO_ADD11:
		{
		    tVolts = (USERCAL_stRAMCAL.userCalADRefH * u32ADCResult) / 256;
		    tVolts = (100 * tVolts) / (4095 * 22);
			break;
		}
		default:
		{
			tVolts = 0;/*CR1_17*/
		}
	}
	
	return tVolts;	
}

GPM6_ttOhms CONV_tADCToOhms(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	GPM6_ttOhms Ohms;
	
	switch (enEHIOResource)
	{	
		default:
		{
			Ohms = 0;
			break;
		}
	}
	
	return Ohms;
}

GPM6_ttOhms CONV_tVoltsAmpsToOhms(GPM6_ttVolts tVolts, GPM6_ttAmps tAmps)/*CR1_17*/
{
	GPM6_ttOhms tOhms = GPM6_ttOhmsMAXVAL;
	
	if (0 != tAmps)/*CR1_20*/
	{
		tOhms = (tVolts * 1000u) / tAmps;/*CR1_17*/
	}

	return tOhms;
}

GPM6_ttAmps CONV_tOhmsVoltsToAmps(GPM6_ttOhms tOhms, GPM6_ttVolts tVolts)/*CR1_17*/
{
	GPM6_ttAmps tAmps = GPM6_ttAmpsMAXVAL;	
	
	if (0 != tOhms)/*CR1_20*/
	{
		tAmps = (tVolts * 1000u) / tOhms;/*CR1_17*/
	}
	
	return tAmps;
}			
			
#endif //BUILD_USER
