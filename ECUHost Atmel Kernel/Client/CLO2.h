/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      CLO2 Header File                                       */
/* DESCRIPTION:        This code module provides closed loop fuel functions   */
/* FILE NAME:          CLO2.h                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef CLO2_H
#define CLO2_H

#include "IOAPI.h"
#include "types.h"
#include "SETUP.h"
#include "SENSORS.h"
#include "TPS.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _CLO2_C
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define CLO2_nBiasTableData \
{\
38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,\
38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,\
38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,\
38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,\
38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,38,\
38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,\
68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,\
98,100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,\
128,130,132,134,136,138,140,142,144,146,148,150,152,154,156,158,\
158,160,162,164,166,168,170,172,174,176,178,180,182,184,186,188,\
188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,218,\
218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,\
218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,\
218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,\
218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,\
218,218,218,218,218,218,218,218,218,218,218,218,218,218,218,218}



/* GLOBAL TYPE DEFINITIONS ****************************************************/

/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
EXTERN uint32 CLO2_u32STTAvg[2];
EXTERN uint32 CLO2_u32STT[2];
//ASAM mode=readvalue name="STT Bank 1" type=uint32 offset=0 min=0 max=1500 m=1 b=0 units="dl" format=4.0 help="Short Term Trim Bank1"
//ASAM mode=readvalue name="STT Bank 2" type=uint32 offset=4 min=0 max=1500 m=1 b=0 units="dl" format=4.0 help="Short Term Trim Bank2"
EXTERN uint32 CLO2_u32LTT[2];
EXTERN sint32 CLO2_s32ISCESTTrim[2];
EXTERN uint16 CLO2_au16AFRMeasured[2];
//ASAM mode=readvalue name="AFR Measured 1" type=uint16 offset=0 min=5 max=100 m=0.01 b=0 units="dl" format=4.2 help="Measured AFR Bank1"
//ASAM mode=readvalue name="AFR Measured 2" type=uint16 offset=2 min=5 max=100 m=0.01 b=0 units="dl" format=4.2 help="Measured AFR Bank2"
EXTERN uint32 CLO2_u32AutoTuneVE;
//ASAM mode=readvalue name="VE AUTOTUNE DIAG" type=uint32 offset=0 min=5 max=100 m=0.01 b=0 units="dl" format=4.2 help="Autotune VE Diag"


/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void CLO2_vInit(void);
void CLO2_vRun(void);
void CLO2_vFilterSensors(void);


#endif // CLO2_H

