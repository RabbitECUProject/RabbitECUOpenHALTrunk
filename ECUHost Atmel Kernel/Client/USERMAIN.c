/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      USERMAIN                                               */
/* DESCRIPTION:        This code module provides the user application entry   */
/*                     point                                                  */
/*                                                                            */
/* FILE NAME:          USERMAIN.c                                             */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "build.h"
#include "user.h"

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint32 USER_u32Stat;

/* GLOBAL FUNCTION DEFINITIONS ************************************************/

#if (BUILD_USER == 1)
int usermain(void)
{
	(void)USER_vStart(&OS_stSVCDataStruct);
	
	return (int)USER_u32Stat;/*CR1_6*/
}
#endif //BUILD_USER

