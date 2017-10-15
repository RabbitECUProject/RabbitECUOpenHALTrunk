/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Setup                                                  */
/* DESCRIPTION:        This code assist with the setup of kernel managed      */
/*                     resources                                              */
/*                                                                            */
/* FILE NAME:          SETUP.c                                                */
/* REVISION HISTORY:   14-04-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _SETUP_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "SETUP.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
Bool SETUP_boSetupADSE(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOType enEHIOType, ADCAPI_tenSamplesAv enSamplesAv, ADCAPI_tpfResultCB pfResultCB, ADCAPI_tenTrigger enTrigger, puint32 pu32Arg)
{
	ADCAPI_tstADCCB stADCCB;

	stADCCB.enSamplesAv = enSamplesAv;
	stADCCB.enDiffGain = ADCAPI_enDiffGain1;
	stADCCB.pfResultCB = pfResultCB;	
	stADCCB.enTrigger = enTrigger;				
		
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{											
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&stADCCB);	

		if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) 
		{
			*pu32Arg |= (uint32)SYSAPI_enResourceRequestFailed;/*CR1_13*/
		}			
	}		
	else
	{	
		*pu32Arg |= (uint32)SYSAPI_enResourceInitFailed;/*CR1_13*/
	}
}

TABLEAPI_ttTableIDX SETUP_tSetupTable(void* pvTableData, void* pvOutputData, TYPE_tenDataType enDataType, sint16 s16TableSize, SPREADAPI_ttSpreadIDX tSpreadIDX, TABLEAPI_pfPIDResultCB pfResultCB)
{
	TABLEAPI_tstTableCB stTableCB;	
	TABLEAPI_ttTableIDX tTableIDX;	
	
	/* Request and initialise required Kernel managed tables */
	stTableCB.pvTableData = pvTableData;
	stTableCB.pvOutputData = pvOutputData;
	stTableCB.enDataType = enDataType;
	stTableCB.i16TableSize = s16TableSize;
	stTableCB.tSpreadIDX = tSpreadIDX;
	stTableCB.pfResultCB = NULL;
	
	USER_vSVC(SYSAPI_enInitialiseTableResource, (void*)&stTableCB, NULL, NULL);	
	tTableIDX = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
				(TABLEAPI_ttTableIDX)pstSVCDataStruct->tClientHandle : -1;		
	
	return tTableIDX;
}

MAPSAPI_ttMapIDX SETUP_tSetupMap(void* pvMapData, void* pvOutputData, TYPE_tenDataType enDataType, sint16 s16MapSizeX, sint16 s16MapSizeY, SPREADAPI_ttSpreadIDX tSpreadIDXX, SPREADAPI_ttSpreadIDX tSpreadIDXY,TABLEAPI_pfPIDResultCB pfResultCB)
{
	MAPSAPI_tstMapCB stMapCB;
	MAPSAPI_ttMapIDX tMapIDX;
	
	/* Request and initialise required Kernel managed tables */
	stMapCB.pvMapData = pvMapData;
	stMapCB.pvOutputData = pvOutputData;
	stMapCB.enDataType = enDataType;
	stMapCB.i16MapSizeX = s16MapSizeX;
	stMapCB.i16MapSizeY = s16MapSizeY;
	stMapCB.tSpreadIDXX = tSpreadIDXX;
	stMapCB.tSpreadIDXY = tSpreadIDXY;
	stMapCB.pfResultCB = NULL;
	
	USER_vSVC(SYSAPI_enInitialiseMapResource, (void*)&stMapCB, NULL, NULL);
	tMapIDX = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
	(MAPSAPI_ttMapIDX)pstSVCDataStruct->tClientHandle : -1;
	
	return tMapIDX;
}
	
SPREADAPI_ttSpreadIDX SETUP_tSetupSpread(void* pvSourceData, void* pvSpreadData, TYPE_tenDataType enDataType, sint16 s16SpreadSize, SPREADAPI_tenSpreadRate enSpreadRate, SPREADAPI_pfPIDResultCB pfResultCB)
{
	SPREADAPI_tstSpreadCB stSpreadCB;	
	SPREADAPI_ttSpreadIDX tSpreadIDX;		
	
	stSpreadCB.pvSourceData = pvSourceData;
	stSpreadCB.pvSpreadData = pvSpreadData;
	stSpreadCB.enDataType = enDataType;
	stSpreadCB.s16SpreadSize = s16SpreadSize;
	stSpreadCB.enSpreadRate = enSpreadRate;
	stSpreadCB.pfResultCB = pfResultCB;
	
	USER_vSVC(SYSAPI_enInitialiseSpreadResource, (void*)&stSpreadCB, NULL, NULL);	
	tSpreadIDX = (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)	?
				(SPREADAPI_ttSpreadIDX)pstSVCDataStruct->tClientHandle : -1;		

	return tSpreadIDX;
}

void SETUP_vSetupDigitalIO(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tenEHIOType enEHIOType, IOAPI_tenDriveStrength enDriveStrength, puint32 pu32Arg)
{	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);	

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{											
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
				(void*)&enEHIOType,	(void*)&enDriveStrength);	

		if (SYSAPI_enOK != pstSVCDataStruct->enSVCResult) 
		{
			*pu32Arg |= (uint32)SYSAPI_enResourceRequestFailed;/*CR1_13*/
		}			
	}		
	else
	{	
		*pu32Arg |= (uint32)SYSAPI_enResourceInitFailed;/*CR1_13*/
	}
}

SETUP_vSetDigitalIOHigh(IOAPI_tenEHIOResource enEHIOResource)
{
    IOAPI_tenTriState enTriState = IOAPI_enHigh;

    USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
	    (void*)&enTriState,	(void*)NULL);
}

SETUP_vSetDigitalIOLow(IOAPI_tenEHIOResource enEHIOResource)
{
	IOAPI_tenTriState enTriState = IOAPI_enLow;

	USER_vSVC(SYSAPI_enAssertDIOResource, (void*)&enEHIOResource,
	(void*)&enTriState,	(void*)NULL);
}

Bool SETUP_vGetDigitalIO(IOAPI_tenEHIOResource enEHIOResource)
{
	Bool boResult;

	USER_vSVC(SYSAPI_enGetDIOResource, (void*)&enEHIOResource,
	(void*)NULL,	(void*)NULL);

	boResult = *(Bool*)pstSVCDataStruct->pvArg1;

	return boResult;
}

#endif //BUILD_USER