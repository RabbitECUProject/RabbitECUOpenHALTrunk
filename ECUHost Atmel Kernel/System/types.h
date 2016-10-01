/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      Types Header File                                      */
/* DESCRIPTION:        This code provides various required types              */
/*                     declarations                                           */
/* FILE NAME:          types.h                                                */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h> //avoid redefinition of standard types error!

#ifdef BUILD_PACKED
#define BUILDPACKED __packed
#endif
#ifndef BUILD_PACKED
#define BUILDPACKED
#endif

/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef unsigned int* puint32;
typedef unsigned short* puint16;
typedef unsigned char* puint8;

typedef int* psint32;
typedef short* psint16;
typedef char* psint8;

typedef volatile unsigned int* vpuint32;
typedef volatile unsigned short* vpuint16;
typedef volatile unsigned char* vpuint8;

typedef unsigned char bool;
typedef signed char sint8;
typedef short sint16;
typedef int sint32;
#ifndef _SYS__STDINT_H
typedef unsigned int uint32_t;
#endif
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef unsigned int* puint32_t;
typedef unsigned short* puint16_t;
typedef unsigned char* puint8_t;
typedef short int16_t;
#ifndef _SYS__STDINT_H
typedef int int32_t;
#endif

typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned int UINT;
typedef unsigned short WORD;

typedef enum
{
	TYPE_enUInt8,
	TYPE_enUInt16,
	TYPE_enUInt32,
	TYPE_enInt8,
	TYPE_enInt16,
	TYPE_enInt32
} TYPE_tenDataType;
	

#endif //TYPES_H

