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
#ifndef MACROS_H
#define MACROS_H

#include "types.h"

//TODO porting does not belong here
#define PORT_HOSTNETWORK_ENDIANSWITCH

#define U32TOBYTE3(x)		(uint8)(((uint32)(x) >> 24) & 0xff)
#define U32TOBYTE2(x) 	(uint8)(((uint32)(x) >> 16) & 0xff)
#define U32TOBYTE1(x) 	(uint8)(((uint32)(x) >>  8) & 0xff)
#define U32TOBYTE0(x) 	(uint8)(((uint32)(x) >>  0) & 0xff)

#define U16TOBYTE1(x) 	(uint8)(((uint16)(x) >>  8) & 0xff)
#define U16TOBYTE0(x) 	(uint8)(((uint16)(x) >>  0) & 0xff)

#ifdef PORT_HOSTNETWORK_ENDIANSWITCH
	#define HTONS(x) 			((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))
	#define NTOHS(x) 			HTONS(x)
	#define HTONL(x) 			((((x) & 0xff) << 24) | \
													(((x) & 0xff00) << 8) | \
													(((x) & 0xff0000UL) >> 8) | \
													(((x) & 0xff000000UL) >> 24))
	#define NTOHL(x) 			HTONL(x)
#else
	#define HTONS(x) 			(x)
	#define NTOHS(x) 			(x)
	#define HTONL(x) 			(x)
	#define NTOHL(x) 			(x)
#endif //PORT_HOSTNETWORK_ENDIANSWITCH

#endif //MACROS_H

