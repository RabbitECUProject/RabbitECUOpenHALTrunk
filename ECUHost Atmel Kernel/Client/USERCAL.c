/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      USERCAL                                                */
/* DESCRIPTION:        This code module initialises the user calibration      */
/*                     RAM mirror from the EEPROM or ROM image                */
/*                                                                            */
/* FILE NAME:          USERCAL.c                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _USERCAL_C

#include "build.h"

#if BUILD_USER

#include "USERCAL.h"


/* LOCAL CONSTANT DEFINITIONS (STATIC) ****************************************/
const USERCAL_tstCalibration USERCAL_rstROMCAL = USERCAL_nROMCAL;

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
bool USERCAL_boPartitionOK;
	

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void USERCAL_vStart(puint32 const pu32Arg)
{
	puint8 pu8RAMData = (puint8)&USERCAL_stRAMCAL;
	uint16 u16UserCalDataCount = sizeof(USERCAL_stRAMCAL);
	bool boRetVal;
	bool boCheckCRC16 = TRUE;
	bool boNVMToWorking = FALSE;
	
	USER_vSVC(SYSAPI_enCheckPartition, (void*)NULL,
				(void*)NULL,	(void*)NULL);		
	
	USERCAL_boPartitionOK = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult);	
	
	if (TRUE == USERCAL_boPartitionOK)
	{
		USER_vSVC(SYSAPI_enSetupWorkingPage, (void*)&pu8RAMData,
					(void*)&u16UserCalDataCount,	(void*)NULL);			
		
		boRetVal = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult);
		
		if (TRUE == boRetVal)
		{
			boNVMToWorking = TRUE;			
			USER_vSVC(SYSAPI_enNVMWorkingCopy, (void*)&boNVMToWorking,
				(void*)&boCheckCRC16, (void*)NULL);
			
			boNVMToWorking = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult);			
		}
	}

	if (TRUE != boNVMToWorking)
	/* If NVM to working page failed then boot ROM image */
	{
		memcpy((void*)&USERCAL_stRAMCAL, (void*)&USERCAL_rstROMCAL,
						sizeof(USERCAL_tstCalibration));		

#if BUILD_KERNEL_APP
		/* Only when KERNEL_APP testing */
		if (TRUE == USERCAL_boPartitionOK)
		{
			boNVMToWorking = FALSE;
				USER_vSVC(SYSAPI_enNVMWorkingCopy, (void*)&boNVMToWorking,
			(void*)&boCheckCRC16, (void*)NULL);
		}
#endif
	}	
}

void USERCAL_vRun(puint32 const pu32Arg)
{
	
}

void USERCAL_vTerminate(puint32 const pu32Arg)
{
	
}

#endif //BUILD_USER
