/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Manifold Pressure Sensor                               */
/* DESCRIPTION:        This code module initialises the required ADC          */
/*                     resources and functions for manifold pressure          */
/*                     measurement                                            */
/* FILE NAME:          MAP.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _MAP_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "MAP.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
bool MAP_boNewSample;

SPREADAPI_ttSpreadIDX MAP_tSpreadVExIDX;
SPREADAPI_ttSpreadIDX MAP_tSpreadVEyIDX;
TABLEAPI_ttTableIDX MAP_tMapVEIDX;

EXTERN GPM6_ttUg MAP_tManChargeMassOldUg;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : MAP_vADCCallBack
*
* Implementation   : Callback to receive the measured ADC value
*
* Parameter
*    Par1          : enEHIOResource enum of the ADC resource
*    Par2          : u32ADCResult the ADC conversion value
*
* Return Value     : NIL
*******************************************************************************/
static void MAP_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult);


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void MAP_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tenEHIOType enEHIOType;
	ADCAPI_tstADCCB stADCCB;
	
	enEHIOResource = USERCAL_stRAMCAL.u16MAPADResource;
	enEHIOType = IOAPI_enADSE;
	stADCCB.enSamplesAv = ADCAPI_en32Samples;
	stADCCB.pfResultCB = &MAP_vADCCallBack;
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
	
	MAP_tKiloPaFiltered = MAP_nVoltsFilteredInitVal;
	MAP_tKiloPaRaw = 0;
	
	MAP_tManChargeMassUg = 0;
	MAP_s32ManDeltaChargeMassPerSUg = 0;	
	
	/* Request and initialise required Kernel managed spread for VEx */
	MAP_tSpreadVExIDX = SETUP_tSetupSpread((void*)&CAM_u32RPMFiltered, (void*)&USERCAL_stRAMCAL.aUserVExSpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for VEy */
	MAP_tSpreadVEyIDX = SETUP_tSetupSpread((void*)&MAP_tKiloPaFiltered, (void*)&USERCAL_stRAMCAL.aUserVEySpread, TYPE_enUInt32, 17, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed table for VE*/
	MAP_tMapVEIDX = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserMAPVE, (void*)&MAP_u16VE, TYPE_enUInt16, 17, 17, MAP_tSpreadVExIDX, MAP_tSpreadVEyIDX, NULL);		
}

void MAP_vRun(puint32 const pu32Arg)
{
    static GPM6_ttPa MAP_tKiloPaOld = 0;
	static uint32 u32Count;
	sint32 s32DeltaMAP;
	sint32 s32DeltaManifoldMass;
	sint32 s32Temp;
	SPREADAPI_tstSpreadResult* pstSpreadResultVEx;
	SPREADAPI_tstSpreadResult* pstSpreadResultVEy;
	uint16 u16TableIDXx = ~0;
	uint16 u16TableIDXy = ~0;
	static bool boEnableVELearn = false;
	uint8 u8MapFilter = 0x10;

	/* Reduce filtering on RPM transitions */
	if (0 != CAM_u32RPMTransitionCounter)
	{
		u8MapFilter *= 2;
	}

	/* Reduce filtering on TPS transitions */
	if ((0 != TPS_u32TransitionCounter) || (0 != TPS_u32ThrottleMovingCounter))
	{
		u8MapFilter *= 2;
	}

	USERMATH_u16SinglePoleLowPassFilter16(MAP_u32ADCRaw, u8MapFilter, &MAP_u32ADCFiltered);

	s32Temp = MAP_u32ADCFiltered * SENSORS_nADRefVolts;
	s32Temp /= SENSORS_nADScaleMax;
	s32Temp /= SENSORS_nVDivRatio;
		
	MAP_tSensorVolts = s32Temp;		

	/* Calculate manifold pressure */
	s32Temp = (USERCAL_stRAMCAL.s32MapSensorGain * MAP_tSensorVolts) / 1000;
	MAP_tKiloPaFiltered = 0 > (s32Temp + USERCAL_stRAMCAL.s32MapSensorOffset) ? 0 : (uint32)(s32Temp +  USERCAL_stRAMCAL.s32MapSensorOffset);


	/* Calculate the current spread for VEx */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadVExIDX,
	NULL, NULL);
	
	/* Calculate the current spread for VEy */
	USER_vSVC(SYSAPI_enCalculateSpread, (void*)&MAP_tSpreadVEyIDX,
	NULL, NULL);

	/* Lookup the current value for VE */
	USER_vSVC(SYSAPI_enCalculateMap, (void*)&MAP_tMapVEIDX,
	NULL, NULL);


	if ((0 == TPS_u32ThrottleMovingCounter) && (400 < CAM_u32RPMRaw) && (0 == (u32Count % 5)))
	{
		/* Get the current x spread for VE */
           pstSpreadResultVEx = BOOSTED_pstGetSpread(MAP_tSpreadVExIDX);

		/* Get the current x spread for VE */
           pstSpreadResultVEy = BOOSTED_pstGetSpread(MAP_tSpreadVEyIDX);

        u16TableIDXx = (0x4000 > pstSpreadResultVEx->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultVEx->uSpreadData.stSpreadResult.u16SpreadIndex : u16TableIDXx;
        u16TableIDXx = (0xc000 < pstSpreadResultVEx->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultVEx->uSpreadData.stSpreadResult.u16SpreadIndex + 1 : u16TableIDXx;
        u16TableIDXy = (0x4000 > pstSpreadResultVEy->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultVEy->uSpreadData.stSpreadResult.u16SpreadIndex : u16TableIDXy;
        u16TableIDXy = (0xc000 < pstSpreadResultVEy->uSpreadData.stSpreadResult.u16SpreadOffset) ? pstSpreadResultVEy->uSpreadData.stSpreadResult.u16SpreadIndex + 1 : u16TableIDXy;

		/* If the indexes are valid then learn the new VE */
		if ((0xffff != u16TableIDXx) && (0xffff != u16TableIDXy) && (true == boEnableVELearn))
		{
		    if (AFM_u16LearnVE < USERCAL_stRAMCAL.aUserMAPVE[u16TableIDXx][u16TableIDXy])
			{
				USERCAL_stRAMCAL.aUserMAPVE[u16TableIDXx][u16TableIDXy] = 400 < USERCAL_stRAMCAL.aUserMAPVE[u16TableIDXx][u16TableIDXy] ?
					USERCAL_stRAMCAL.aUserMAPVE[u16TableIDXx][u16TableIDXy] - 1 : 400;
			}
		    else if (AFM_u16LearnVE > USERCAL_stRAMCAL.aUserMAPVE[u16TableIDXx][u16TableIDXy])
			{
				USERCAL_stRAMCAL.aUserMAPVE[u16TableIDXx][u16TableIDXy] = 1000 > USERCAL_stRAMCAL.aUserMAPVE[u16TableIDXx][u16TableIDXy] ?
					USERCAL_stRAMCAL.aUserMAPVE[u16TableIDXx][u16TableIDXy] + 1 : 1000;
			}
		}
	}


	if (0 == (u32Count % 5))
	{
		s32DeltaMAP = MAP_tKiloPaFiltered - MAP_tKiloPaOld;
		s32DeltaManifoldMass = (USERCAL_stRAMCAL.u16ManifoldVolumeCC * MAP_nAirDensMgpL) / 1000; 
		s32DeltaManifoldMass *= s32DeltaMAP;
        MAP_s32ManDeltaChargeMassPerSUg = (MAP_nRunFreq * -s32DeltaManifoldMass) / 10000;	
        MAP_tKiloPaOld = MAP_tKiloPaFiltered;
	}

	MAP_boHighVacuum = USERCAL_stRAMCAL.u16HighVacuumEnableKpa > MAP_tKiloPaFiltered ? TRUE : MAP_boHighVacuum;
	MAP_boHighVacuum = USERCAL_stRAMCAL.u16HighVacuumDisableKpa < MAP_tKiloPaFiltered ? FALSE : MAP_boHighVacuum;

	u32Count++;
}

void MAP_vTerminate(puint32 const pu32Arg)
{

}


void MAP_vCallBack(puint32 const pu32Arg)
{

}


static void MAP_vADCCallBack(IOAPI_tenEHIOResource enEHIOResource, uint32 u32ADCResult)
{
	MAP_u32ADCRaw = u32ADCResult;
	MAP_boNewSample = TRUE;
}

#endif //BUILD_USER
