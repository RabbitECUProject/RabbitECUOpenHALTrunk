/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      Declarations Header File                               */
/* DESCRIPTION:        This code provides various required declarations       */
/*                                                                            */
/* FILE NAME:          declarations.h                                         */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef DECLARATIONS_H
#define DECLARATIONS_H


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#ifndef NULL
#define NULL ((void*)0)
#endif	
/*CR1_98*/

#ifndef true
#define true 1
#endif
/*CR1_99*/

#ifndef false
#define false 0
#endif
/*CR1_99*/


#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

#define nASCII_CR 0x0d
#define nASCII_LF 0x0a


#define MIN(a, b)		((a) > (b)) ? (b) : (a)
#define MAX(a, b)		((a) < (b)) ? (b) : (a)
#define ABS(a)   		(((a) > 0) 	? (a) : (-a))


#endif //DECLARATIONS_H

