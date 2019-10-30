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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

#include "USERCAL.h"


/* LOCAL CONSTANT DEFINITIONS (STATIC) ****************************************/
const USERCAL_tstCalibration USERCAL_rstROMCAL = USERCAL_nROMCAL;


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
bool USERCAL_boPartitionOK;
uint16 USERCAL_u16ConfigCRC16;
//ASAM mode=readvalue name="USERCAL_ConfigCRC" type=uint16 offset=0 min=0 max=65535 m=1 b=0 units="dl" format=5.3 help="Working Calibration CRC"
	

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void USERCAL_vStart(puint32 const pu32Arg)
{
	puint8 pu8RAMData = (puint8)&USERCAL_stRAMCAL;
	uint16 u16UserCalDataCount = sizeof(USERCAL_stRAMCAL);
	bool boRetVal;
	bool boCheckCRC16 = TRUE;
	bool boNVMToWorking = FALSE;

	USERCAL_u16ConfigCRC16 = ~0;
	
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
		//if (TRUE == USERCAL_boPartitionOK)
		//{
		//	boNVMToWorking = FALSE;
		//		USER_vSVC(SYSAPI_enNVMWorkingCopy, (void*)&boNVMToWorking,
		//	(void*)&boCheckCRC16, (void*)NULL);
		//}
#endif
	}	
}

void USERCAL_vRun(puint32 const pu32Arg)
{
	static uint32 u32UserConfigCRCCount = 0;
	static uint16 u16ConfigCRC = ~0;
	uint32 u32Len;
	uint32 u32ConfigByteCount;

	u32ConfigByteCount = au32Offsets[(sizeof(au32Offsets) / sizeof(uint32*)) - 1];

	if (u32UserConfigCRCCount < u32ConfigByteCount)
	{
		if (USERCAL_nCRCCheckChunkSize < (u32ConfigByteCount - u32UserConfigCRCCount))
		{
			u16ConfigCRC = USERMATH_U16GetCRC(u16ConfigCRC, (puint8)((uint32)&USERCAL_stRAMCAL + u32UserConfigCRCCount), USERCAL_nCRCCheckChunkSize);
			u32UserConfigCRCCount += USERCAL_nCRCCheckChunkSize;
		}
		else
		{
			u32Len = u32ConfigByteCount - u32UserConfigCRCCount;
			USERCAL_u16ConfigCRC16 = USERMATH_U16GetCRC(u16ConfigCRC, (puint8)((uint32)&USERCAL_stRAMCAL + u32UserConfigCRCCount), u32Len);					 
			u16ConfigCRC = ~0;
			u32UserConfigCRCCount = 0;
		}
	}
}

void USERCAL_vTerminate(puint32 const pu32Arg)
{
	UNUSED(pu32Arg);
}

#pragma GCC diagnostic pop

#endif //BUILD_USER
