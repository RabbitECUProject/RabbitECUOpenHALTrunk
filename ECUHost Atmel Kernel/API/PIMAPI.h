/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          PIMAPI                                                 */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed port integration  */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          PIMAPI.h                                               */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef PIMAPI_H
#define PIMAPI_H


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define	PIMAPI_PORT_BIT_0  0x00000001
#define	PIMAPI_PORT_BIT_1  0x00000002
#define	PIMAPI_PORT_BIT_2  0x00000004
#define	PIMAPI_PORT_BIT_3  0x00000008
#define	PIMAPI_PORT_BIT_4  0x00000010
#define	PIMAPI_PORT_BIT_5  0x00000020
#define	PIMAPI_PORT_BIT_6  0x00000040
#define	PIMAPI_PORT_BIT_7  0x00000080
#define	PIMAPI_PORT_BIT_8  0x00000100
#define	PIMAPI_PORT_BIT_9  0x00000200
#define	PIMAPI_PORT_BIT_10 0x00000400
#define	PIMAPI_PORT_BIT_11 0x00000800
#define	PIMAPI_PORT_BIT_12 0x00001000
#define	PIMAPI_PORT_BIT_13 0x00002000
#define	PIMAPI_PORT_BIT_14 0x00004000
#define	PIMAPI_PORT_BIT_15 0x00008000
#define	PIMAPI_PORT_BIT_16 0x00010000
#define	PIMAPI_PORT_BIT_17 0x00020000
#define	PIMAPI_PORT_BIT_18 0x00040000
#define	PIMAPI_PORT_BIT_19 0x00080000
#define	PIMAPI_PORT_BIT_20 0x00100000
#define	PIMAPI_PORT_BIT_21 0x00200000
#define	PIMAPI_PORT_BIT_22 0x00400000
#define	PIMAPI_PORT_BIT_23 0x00800000
#define	PIMAPI_PORT_BIT_24 0x01000000
#define	PIMAPI_PORT_BIT_25 0x02000000
#define	PIMAPI_PORT_BIT_26 0x04000000
#define	PIMAPI_PORT_BIT_27 0x08000000
#define	PIMAPI_PORT_BIT_28 0x10000000
#define	PIMAPI_PORT_BIT_29 0x20000000
#define	PIMAPI_PORT_BIT_30 0x40000000
#define	PIMAPI_PORT_BIT_31 0x80000000


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef enum
{
	PIMAPI_enPHYS_PORT_A = 0,
	PIMAPI_enPHYS_PORT_B = 1,
	PIMAPI_enPHYS_PORT_C = 2,
	PIMAPI_enPHYS_PORT_D = 3,
	PIMAPI_enPHYS_PORT_E = 4,
	PIMAPI_enVIRT_PORT_F,
	PIMAPI_enPORT_Count,
	PIMAPI_enPortInvalid
} PIMAPI_tenPort;


#endif //PIMAPI_H

