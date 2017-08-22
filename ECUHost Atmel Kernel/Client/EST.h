/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Electronic Spark Timing Header File                    */
/* DESCRIPTION:        This code module initialises the required              */
/*                     resources and functions for electronic spark timing    */
/*                                                                            */
/* FILE NAME:          EST.h                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef EST_H
#define EST_H

#include <string.h>
#include "IOAPI.h"
#include "TEPMAPI.h"
#include "USER.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _EST_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define EST_nMotor1EnablePin          EH_IO_IO23
#define EST_nMotor2EnablePin          EH_IO_IO25
#define EST_nESTOutput1               EH_IO_TMR3
#define EST_nESTOutput2               EH_IO_TMR4
#define EST_nSlowFTMDivisor           TEPMAPI_enDiv128
#define EST_nDwellOffMinUs            1500u
#define EST_nDegreesPerCycle          360u
#define EST_xUsToSlowTicks(x)          (((SENSORS_nSlowFTMFreq / 100u) * (x))	/ 10000u)

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/

EXTERN TEPMAPI_ttEventTime EST_tStartFractionB;
EXTERN TEPMAPI_ttEventTime EST_tStartFractionA;	
//ASAM mode=readvalue name="EST Start Fraction" type=uint16 offset=0 min=0 max=1 m=0.00001526 b=0 units="dl" format=6.0 help="EST Dwell Start Fraction"  
EXTERN TEPMAPI_ttEventTime EST_tDwellUs;	
//ASAM mode=readvalue name="EST Dwell Fraction" type=uint16 offset=0 min=0 max=1 m=0.00001526 b=0 units="dl" format=6.0 help="EST Dwell Fraction" 
EXTERN GPM6_ttMTheta EST_tIgnitionAdvanceMtheta;	
//ASAM mode=readvalue name="EST Ignition Advance" type=uint32 offset=0 min=0 max=180 m=0.001 b=0 units="degrees" format=2.1 help="EST Ignition Advance" 


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void EST_vStart(uint32 * const pu32Arg);
void EST_vRun(uint32* const pu32Arg);
void EST_vTerminate(uint32* const pu32Arg);
void EST_vCallBack(puint32 const pu32Arg);


#endif // EST_H

