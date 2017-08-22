/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Air Flow Meter                                         */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for air flow measurement       */
/*                                                                            */
/* FILE NAME:          AFM.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _AFM_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "AFM.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
bool AFM_boNewSample;
EXTERN GPM6_ttUg AFM_tManChargeMassOldUg;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : AFM_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void AFM_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);
SPREADAPI_ttSpreadIDX AFM_tSpreadAfmTFIDX;
SPREADAPI_ttSpreadIDX AFM_tSpreadBackupAirflowxIDX;
SPREADAPI_ttSpreadIDX AFM_tSpreadBackupAirflowyIDX;
MAPSAPI_ttMapIDX AFM_tMapBackupAirflowIDX;
TABLEAPI_ttTableIDX AFM_tTableAfmTFIDX;

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void AFM_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;
	
	AFM_boNewSample = FALSE;		
	enEHIOResource = AFM_nADInput;
	enEHIOType = IOAPI_enADSE;
	stADCCB.enSamplesAv = ADCAPI_en32Samples;
	stADCCB.pfResultCB = &AFM_vADCCallBack;
	stADCCB.enTrigger = ADCAPI_enTrigger4;				
		
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	(void*)NULL, (void*)NULL);

	/* Request and initialise required Kernel managed spread for AfmTF */
	AFM_tSpreadAfmTFIDX = SETUP_tSetupSpread((void*)&AFM_tSensorVolts, (void*)&USERCAL_stRAMCAL.aUserCURVEAfmTFSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for backup airflow x */
	AFM_tSpreadBackupAirflowxIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserBackupAirflowxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for backup airflow y */
	AFM_tSpreadBackupAirflowyIDX = SETUP_tSetupSpread((void*)&TPS_tThetaFiltered, (void*)&USERCAL_stRAMCAL.aUserBackupAirflowySpread, TYPE_enUInt32, 11, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed map for backup airflow */
	AFM_tMapBackupAirflowIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserBackupAirflowMap, (void*)&AFM_tAirFlowBackupRawUg, TYPE_enUInt32, 11, 11, AFM_tSpreadBackupAirflowxIDX, AFM_tSpreadBackupAirflowyIDX, NULL);	

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

void AFM_vRun(puint32 const pu32Arg)
{
	uint32 u32Temp;
	sint32 s32Temp;
	uint8 u8ManifoldTimeConstantFilter;
	static uint32 u32SampleCount;
	SPREADAPI_tstSpreadResult* pstSpreadResultAirflowBackupX;
	SPREADAPI_tstSpreadResult* pstSpreadResultAirflowBackupY;
	uint32 u32TableIDXx = ~0;
	uint32 u32TableIDXy = ~0;
	
	if (TRUE == AFM_boNewSample)
	{
		USER_xEnterCritical();/*CR1_16*/
		(void)USERMATH_u16SinglePoleLowPassFilter16((uint16)AFM_u32ADCRaw, AFM_nADFiltVal,
			&AFM_u32ADCFiltered);
		AFM_boNewSample = FALSE;		
		USER_xExitCritical();/*CR1_16*/
		
		u32Temp = AFM_u32ADCFiltered * SENSORS_nADRefVolts;
		u32Temp /= SENSORS_nADScaleMax;
		u32Temp /= SENSORS_nVDivRatio;
		
		AFM_tSensorVolts = u32Temp;

		if (0 == CAM_u32RPMRaw)
		{
			/* Calculate the current spread for AfmTF */
			USER_vSVC(SYSAPI_enCalculateSpread, (void*)&AFM_tSpreadAfmTFIDX,
				NULL, NULL);		
	
			/* Lookup the current AfmTF value for AfmTF */
			USER_vSVC(SYSAPI_enCalculateTable, (void*)&AFM_tTableAfmTFIDX,
			    NULL, NULL);
		}

        (void)BOOSTED_boIndexAndCalculateMap(AFM_tSpreadBackupAirflowxIDX, AFM_tSpreadBackupAirflowyIDX, AFM_tMapBackupAirflowIDX);

		u8ManifoldTimeConstantFilter = USERMATH_u8GetFilterFromTimeConstant(0x01, TPS_u32ManifoldVolumeTau );
		u8ManifoldTimeConstantFilter = ~u8ManifoldTimeConstantFilter;
		u8ManifoldTimeConstantFilter++;

		AFM_tAirFlowBackupUg = USERMATH_u32SinglePoleLowPassFilter32(AFM_tAirFlowBackupRawUg, u8ManifoldTimeConstantFilter, &AFM_tAirFlowBackupUg);

		if (0 == TPS_u32ThrottleMovingCounter)
		{
			/* Get the current x spread for backup airflow */
            pstSpreadResultAirflowBackupX = BOOSTED_pstGetSpread(AFM_tSpreadBackupAirflowxIDX);

			/* Get the current x spread for backup airflow */
            pstSpreadResultAirflowBackupY = BOOSTED_pstGetSpread(AFM_tSpreadBackupAirflowyIDX);

            u32TableIDXx = (0x1000 > pstSpreadResultAirflowBackupX->u16SpreadOffset) ? pstSpreadResultAirflowBackupX->u16SpreadIndex : u32TableIDXx;
            u32TableIDXx = (0xf000 < pstSpreadResultAirflowBackupX->u16SpreadOffset) ? pstSpreadResultAirflowBackupX->u16SpreadIndex + 1 : u32TableIDXx;
            u32TableIDXy = (0x1000 > pstSpreadResultAirflowBackupY->u16SpreadOffset) ? pstSpreadResultAirflowBackupY->u16SpreadIndex : u32TableIDXy;
            u32TableIDXy = (0xf000 < pstSpreadResultAirflowBackupY->u16SpreadOffset) ? pstSpreadResultAirflowBackupY->u16SpreadIndex + 1 : u32TableIDXy;

			/* If the indexes are valid then learn the new airflow */
			if ((~0 != u32TableIDXx) && (~0 != u32TableIDXy))
			{
			    u32Temp = AFM_tAirFlowUg / 4 + (3 * USERCAL_stRAMCAL.aUserBackupAirflowMap[u32TableIDXx][u32TableIDXy]) / 4;
			    USERCAL_stRAMCAL.aUserBackupAirflowMap[u32TableIDXx][u32TableIDXy] = u32Temp;
			}
		}
	}
}

void AFM_vTerminate(puint32 const pu32Arg)
{

}


void AFM_vCallBack(puint32 const pu32Arg)
{

}


static void AFM_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	AFM_u32ADCRaw = u32ADCResult;
	AFM_boNewSample = TRUE;
}

#endif //BUILD_USER
