/******************************************************************************/
/*    Copyright (c) 2019 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Generic Data Header File                               */
/* DESCRIPTION:        This code module contains generic data transforms      */
/*                                                                            */
/* FILE NAME:          gendata.c                                              */
/* REVISION HISTORY:   28-06-2019 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef GENDATA_H
#define GENDATA_H

#include "MAPSAPI.h"
#include "TABLESAPI.h"

/* GLOBAL MACRO DEFINITIONS ***************************************************/

#ifdef EXTERN
	#undef EXTERN
#endif	
#ifdef _GENDATA_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define GENDATA_nPWM2DTableCount 8
#define GENDATA_nPWM3DTableCount 4
#define GENDATA_nPWM2DTableDimX 17
#define GENDATA_nPWM3DTableDimX 17
#define GENDATA_nPWM3DTableDimY 17
#define GENDATA_nVarsCount 50

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint16 GENDATA_u16VAR1;
//ASAM mode=readvalue name="Generic Data 1|LINK|Engine Speed Raw" type=uint16 offset=0 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic RPM"
EXTERN uint16 GENDATA_u16VAR2;
//ASAM mode=readvalue name="Generic Data 2|LINK|Throttle Angle" type=uint16 offset=0 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic TPS"
EXTERN uint16 GENDATA_u16VAR3;
//ASAM mode=readvalue name="Generic Data 3|LINK|MAP kPa" type=uint16 offset=0 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic MAP"
EXTERN uint16 GENDATA_u16VAR4;
//ASAM mode=readvalue name="Generic Data 4|LINK|Coolant Temperature" type=uint16 offset=0 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic CTS"
EXTERN uint16 GENDATA_u16VAR5;
//ASAM mode=readvalue name="Generic Data 5|LINK|Air Temperature" type=uint16 offset=0 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic CTS"
		
EXTERN uint16 GENDATA_u16SOURCE[16];
//ASAM mode=readvalue name="Generic Source IDX1" type=uint16 offset=0 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX1"
//ASAM mode=readvalue name="Generic Source IDX2" type=uint16 offset=2 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX2"
//ASAM mode=readvalue name="Generic Source IDX3" type=uint16 offset=4 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX3"
//ASAM mode=readvalue name="Generic Source IDX4" type=uint16 offset=6 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX4"
//ASAM mode=readvalue name="Generic Source IDX5" type=uint16 offset=8 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX5"
//ASAM mode=readvalue name="Generic Source IDX6" type=uint16 offset=10 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX6"
//ASAM mode=readvalue name="Generic Source IDX7" type=uint16 offset=12 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX7"
//ASAM mode=readvalue name="Generic Source IDX8" type=uint16 offset=14 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX8"
//ASAM mode=readvalue name="Generic Source IDX9" type=uint16 offset=16 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX9"
//ASAM mode=readvalue name="Generic Source IDX10" type=uint16 offset=18 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX10"
//ASAM mode=readvalue name="Generic Source IDX11" type=uint16 offset=20 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX11"
//ASAM mode=readvalue name="Generic Source IDX12" type=uint16 offset=22 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX12"
//ASAM mode=readvalue name="Generic Source IDX13" type=uint16 offset=24 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX13"
//ASAM mode=readvalue name="Generic Source IDX14" type=uint16 offset=26 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX14"
//ASAM mode=readvalue name="Generic Source IDX15" type=uint16 offset=28 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX15"
//ASAM mode=readvalue name="Generic Source IDX16" type=uint16 offset=30 min=0 max=32767 m=1 b=-32768 units="dl" format=5.3 help="Generic Source IDX16"
EXTERN uint16 GENDATA_u16PWMOutput[12];
		
		
		
/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void GENDATA_vStart(uint32 * const pu32Arg);
void GENDATA_vRun(uint32* const pu32Arg);
void GENDATA_vTerminate(uint32* const pu32Arg);


#endif // GENDATA_H
