/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
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
#include "MAP.h"


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
SPREADAPI_ttSpreadIDX AFM_tSpreadBackupAirflowxIDX;
SPREADAPI_ttSpreadIDX AFM_tSpreadBackupAirflowyIDX;
MAPSAPI_ttMapIDX AFM_tMapBackupAirflowIDX;

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void AFM_vStart(puint32 const pu32Arg)
{
	AFM_boNewSample = FALSE;	
	
#ifdef BUILD_BSP_AFM_ANALOG
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;	
	IOAPI_tenEHIOResource enEHIOResource;	
	enEHIOResource = USERCAL_stRAMCAL.u16AFMADResource;
	enEHIOType = IOAPI_enADSE;
	stADCCB.enSamplesAv = ADCAPI_en32Samples;
	stADCCB.pfResultCB = &AFM_vADCCallBack;
	stADCCB.enTrigger = ADCAPI_enTrigger4;
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
#endif
	
	AFM_tSensorHertz = 0;				

	/* Request and initialise required Kernel managed spread for backup airflow x */
	AFM_tSpreadBackupAirflowxIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMRaw, (void*)&USERCAL_stRAMCAL.aUserBackupAirflowxSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for backup airflow y */
	AFM_tSpreadBackupAirflowyIDX = SETUP_tSetupSpread((void*)&TPS_tThetaFiltered, (void*)&USERCAL_stRAMCAL.aUserBackupAirflowySpread, TYPE_enUInt32, 11, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed map for backup airflow */
	AFM_tMapBackupAirflowIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserBackupAirflowMap, (void*)&AFM_tAirFlowBackupRawUg, TYPE_enUInt32, 11, 11, AFM_tSpreadBackupAirflowxIDX, AFM_tSpreadBackupAirflowyIDX, NULL);	
}

void AFM_vRun(puint32 const pu32Arg)
{
	uint32 u32Temp;
	uint8 u8ManifoldTimeConstantFilter;
	SPREADAPI_tstSpreadResult* pstSpreadResultAirflowBackupX;
	SPREADAPI_tstSpreadResult* pstSpreadResultAirflowBackupY;
	uint16 u16TableIDXx = ~0;
	uint16 u16TableIDXy = ~0;
	
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

            u16TableIDXx = (0x1000u > pstSpreadResultAirflowBackupX->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultAirflowBackupX->uSpreadData.stSpreadResult.u16SpreadIndex : u16TableIDXx;
            u16TableIDXx = (0xf000u < pstSpreadResultAirflowBackupX->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultAirflowBackupX->uSpreadData.stSpreadResult.u16SpreadIndex + 1 : u16TableIDXx;
            u16TableIDXy = (0x1000u > pstSpreadResultAirflowBackupY->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultAirflowBackupY->uSpreadData.stSpreadResult.u16SpreadIndex : u16TableIDXy;
            u16TableIDXy = (0xf000u < pstSpreadResultAirflowBackupY->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultAirflowBackupY->uSpreadData.stSpreadResult.u16SpreadIndex + 1 : u16TableIDXy;

			/* If the indexes are valid then learn the new airflow */
			if ((0xffff != u16TableIDXx) && (0xffff != u16TableIDXy))
			{
			    u32Temp = AFM_tAirFlowAFMUg / 4 + (3 * USERCAL_stRAMCAL.aUserBackupAirflowMap[u16TableIDXx][u16TableIDXy]) / 4;
			    USERCAL_stRAMCAL.aUserBackupAirflowMap[u16TableIDXx][u16TableIDXy] = u32Temp;
			}
		}
	}

#ifdef BUILD_BSP_AFM_FREQ
	/* Calculate predicted VE method airflow */
	u32Temp = MAP_u16VE * USERCAL_stRAMCAL.u16CylinderCC * USERCAL_stRAMCAL.u8CylCount / 120;
	u32Temp *= CAM_u32RPMRaw;
	u32Temp /= 10000;
	u32Temp *= MAP_tKiloPaFiltered;
	u32Temp /= 10000;

	/* Correct for air temperature */
	u32Temp = (u32Temp * 2980 ) / ((ATS_tTempCPort / 100) + 2730);

	AFM_tAirFlowVEUg = 1225 * u32Temp;

	/* Calculate current running VE */
	u32Temp = USERCAL_stRAMCAL.u16CylinderCC * USERCAL_stRAMCAL.u8CylCount / 120;
	u32Temp *= CAM_u32RPMFiltered;
	u32Temp /= 10;
	u32Temp *= MAP_tKiloPaFiltered;
	u32Temp /= 10000;
	u32Temp *= 1225;
	u32Temp /= 1000;
	AFM_u16LearnVE = AFM_tAirFlowAFMUg / u32Temp;
#endif
}

void AFM_vTerminate(puint32 const pu32Arg)
{

}


void AFM_vCallBack(puint32 const pu32Arg)
{

}


static void __attribute__((used)) AFM_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	AFM_u32ADCRaw = u32ADCResult;
	AFM_boNewSample = TRUE;
}



#endif //BUILD_USER
