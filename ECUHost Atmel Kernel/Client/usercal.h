/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      USERCAL Header File                                    */
/* DESCRIPTION:        This code module initialises the user calibration      */
/*                     RAM mirror from the EEPROM or ROM image                */
/*                                                                            */
/* FILE NAME:          USERCAL.h                                              */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef USERCAL_H
#define USERCAL_H

#include <stddef.h>
#include <string.h>
#include "declarations.h"
#include "sys.h"
#include "SYSAPI.h"
#include "types.h"
#include "UNITS.h"
#include "user.h"


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#define USERCAL_nFlexRAMBASE	0x14000000
#define USERCAL_nCRCCheckChunkSize 64

#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wno-missing-braces"
#pragma GCC diagnostic ignored "-Wmissing-braces"

//#define TESTCAL
//#define C18CAL
#define TWONR_FE_CAL
//#define KTM_SC_CAL
//#define L98CAL
//#define M15A_CAL

#ifdef TESTCAL
#include "TEST_CAL.h"	
#endif

#ifdef C18CAL
#include "C18_CAL.h"	
#endif

#ifdef TWONR_FE_CAL
#include "2NR_FE_CAL.h"
#endif

#ifdef L98CAL
#include "L98_CAL.h"
#endif

#ifdef KTM_SC_CAL
#include "KTM_CAL_SCA.h"
#endif

#ifdef M15A_CAL
#include "Suzuki_M15A.h"
#endif

#pragma GCC diagnostic pop

#ifdef EXTERN
	#undef EXTERN
#endif
#ifdef _USERCAL_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


/* GLOBAL TYPE DEFINITIONS ****************************************************/
/*******************************************************************************
* Description      : Type to the user calibration
*******************************************************************************/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

typedef __packed struct
/* The usercal struct MUST be single and packed with CRC16 last two bytes
	 for the Kernel CRC16 NVM store and restore to function */
	 /* CAVEAT!!! Make first element uint32 for %4 alignment!!! */
{
	/* 0 */
	GPM6_ttVolts userCalADRefH;
	GPM6_ttVolts userCalVRef445;	
	GPM6_ttVolts userCalVNernstTarget;
	GPM6_ttOhms	userCalRNernstReady;
	GPM6_ttOhms	userCalRHeaterThreshold;	
	uint32 au32UserCalPumpCurrentPFactor[1];	
	uint32 au32UserCalPumpCurrentMFactor[1];		
	sint32 ai32UserCalPumpCurrentOffset[1];		
	GPM6_ttMicroAmps aUserCalPumpSpread[17];	
	GPM6_ttLambda aUserCalPumpLambda[17];
	/* 1 */	
	uint8 au8UserCalPumpSensorTrim[1];
	uint32 aUserStoichFuelCal;
	uint8 au8UserCalPumpDeadband[1];	
	uint16 au16UserCalFreeAirPoints[19];
	GPM6_ttAmps aUserHeaterAlarmCurrent[1];	
	uint8 u8WarmUpDelay;
	uint8 u8DisplayMode;
	uint8 u8BackLightEnable;
	GPM6_ttVolts userCalTPSCalMin;
	GPM6_ttVolts userCalTPSCalMax;	
	/* 2 */	
	GPM6_ttVolts aUserCURVEAfmTFSpread[17];	
	GPM6_ttUg aUserCURVEAfmTFTable[17];	
	GPM6_ttVolts aUserInjResponseSpread[17];	
	uint16 aUserInjResponseTable[17];		
	uint32 aUserCoolantSensorSpread[17];	
	GPM6_ttTempC aUserCoolantSensorTable[17];			
	GPM6_ttTempC aUserCoolantEnrichmentSpread[17];	
	sint32 aUserCoolantEnrichmentTable[17];	
	uint32 aUserTAFRxSpread[17];	
	GPM6_ttMTheta aUserTAFRySpread[17];	
	/* 3 */	
	uint16 aUserMAPTAFR[17][17];
	uint32 aUserTimingxSpread[17];	
	GPM6_ttMTheta aUserTimingySpread[17];	
	uint16 aUserTimingMap[17][17];	
	uint32 aUserVExSpread[17];	
	GPM6_ttKiloPa aUserVEySpread[17];	
	uint16 aUserMAPVE[17][17];
	GPM6_ttTempC aUserCoolantStartEnrichmentSpread[17];	
	sint32 aUserCoolantStartEnrichmentTable[17];		
	GPM6_ttTempC aUserCoolantPostStartEnrichmentSpread[17];	
	/* 4 */	
	sint32 aUserCoolantPostStartEnrichmentTable[17];		
	uint32 aUserCrankingAirflowSpread[17];	
	uint16 aUserCrankingAirflowTable[17];	
	uint32 aUserDwellSpread[17];
	uint16 aUserDwellTable[17];
	GPM6_ttTempC aUserAirTempCorrectionSpread[17];
	uint16 aUserAirTempCorrectionTable[17];
	uint32 aUserTPSMovementCorrectionSpread[17];
	uint16 aUserTPSMovementCorrectionTable[17];
	uint32 aUserInjectionPhasingSpread[17];
	/* 5 */	
	uint16 aUserInjectionPhasingTable[17];
	GPM6_ttTempC aUserCTSTimingCorrectionSpread[17];
	uint16 aUserCTSTimingCorrectionTable[17];
	GPM6_ttTempC aUserATSTimingCorrectionSpread[17];
	uint16 aUserATSTimingCorrectionTable[17];
	GPM6_ttTempC aUserISCSpeedTargetSpread[17];
	uint16 aUserISCSpeedTargetTable[17];
	uint8 aUserStepperCloseTable[4];
	uint8 aUserStepperOpenTable[4];
	uint32 u32UserStepperHomeSteps;
	/* 6 */	
	uint16 aUserPrimaryTriggerTable[80];
	uint16 aUserSecondaryTriggerTable[40];
	uint16 aUserSyncPointsTable[36];
	uint8 u8UserPrimaryEdgeSetup;
	uint8 u8UserSecondaryEdgeSetup;
	uint8 u8UserPrimaryVREnable;
	uint8 u8UserSecondaryVREnable;
	uint8 u8UserFirstEdgeRisingPrimary;
	uint8 u8UserFirstEdgeRisingSecondary;
	uint32 au32InjectionSequence[4];
	/* 7 */	
	uint32 au32IgnitionSequence[4];
	IOAPI_tenEHIOResource enSimpleCamSyncSource;
	Bool boCamSyncHighLate;
	uint32 u32CamSyncSampleToothCount;
	uint16 u16ManifoldVolumeCC;	
	uint8 u8CylCount;
	uint16 u16InjFlowRate;
	uint32 aUserBackupAirflowxSpread[11];	
	uint32 aUserBackupAirflowySpread[11];	
	uint32 aUserBackupAirflowMap[11][11];
	/* 8 */	
	uint8 u8ManifoldTimeConstantTrim;
	uint8 u8EnableBackupAirflowTransients;
	sint32 aUserPrimerSpread[11];
	uint32 aUserPrimerTable[11];
    uint32 aUserInjShortOpeningSpread[11];
    uint32 aUserInjShortOpeningTable[11];
	uint32 u32STTNegLimit;
	uint32 u32STTPosLimit;
	uint32 u32CLStepClosed;
	uint32 u32CLIncClosed;
	/* 9 */	
	uint32 u32CLStepOpen;
	uint32 u32CLIncOpen;
	uint16 u16TimingMainOffset;
	uint32 u32AFMTransientControlRPMLimit;
	uint8 u8WastedSparkEnable;
	uint16 u16AFRReinstate;
	uint16 u16AFRCutoff;
	uint32 u32TPSClosedLower;
	uint32 u32TPSClosedUpper;
	uint32 u32ISCESTTrimPos;
	/* 10 */	
	uint32 u32ISCESTTrimNeg;
	uint32 u32CLO2ESTTrimPos;
	uint32 u32CLO2ESTTrimNeg;
	uint16 u16CylinderCC;
	uint8 u8EnableAFMPrimaryInputOpen;
	uint32 u32SpeedDensityAFMTPSLim;
	uint16 au16SeqRPMLimit[8];
	uint16 u16SeqRPMLimitHyst;
	uint16 u16HighVacuumEnableKpa;
	uint16 u16HighVacuumDisableKpa;
	/* 11 */	
	uint16 u16OverrunCutEnableRPM;
	uint16 u16OverrunCutDisableRPM;
	uint16 u16RPMRunThreshold;
	uint8 u8SeqFuelAssign;
	uint16 u16ESTFilterClosed;
	uint16 u16ESTFilterOpen;
	uint16 u16ESTCLGain;
	uint8 u8EnableAFMPrimaryInputClosed;
	sint16 s16SequenceFuelOffset[4];
	uint16 u16SequenceFuelGain[4];
	/* 12 */	
	Bool boTPSCANPrimary;
	Bool boPPSCANPrimary;
	Bool boCTSCANPrimary;
	Bool boATSCANPrimary;
	Bool boMAPCANPrimary;
	uint32 au32PrioCANID[4];
	uint8 au8SensorCANDataOffset[8];
	uint8 au8SensorCANDataByteCount[8];
	uint32 u32SyncPhaseRepeats;
	sint32 s32MapSensorGain;
	/* 13 */	
	sint32 s32MapSensorOffset;
	uint8 u8InjDivide;
	uint8 u8CLO2Leftenable;
	uint8 u8CLO2RightEnable;
	uint8 u8ReturnlessEnable;
	uint16 u16ReturnlessPressureKPa;
	uint8 u8CrankingAirflowEnable;
	uint8 u8StepperIACEnable;
	uint16 u16IACStepsOpenHot;
	uint16 u16IACStepsOpenCold;
	/* 14 */	
	uint16 u16IdleEntryOffset;
	GPM6_ttTempC s32RadFanOnTemp;
	GPM6_ttTempC s32RadFanOffTemp;
	IOAPI_tenEHIOResource enFuelPumpRelay;
	IOAPI_tenEHIOResource enESTBypass;
	uint32 aUserAirSensorSpread[17];
	GPM6_ttTempC aUserAirSensorTable[17];
	uint16 aFuelIOResource[4];
	uint16 aESTIOResource[4];
	uint16 aIACIOResource[4];
	Bool boOBDISCADV;
	uint8 u8TriggerType;
	uint8 u8SyncType;
	uint8 u8TriggerPullStrength;
	uint8 u8SyncPullStrength;
	uint16 u16MinLinearFuelPulse;
	uint16 u16CTSADResource;
	uint16 u16MAPADResource;
	uint16 u16ATSADResource;
	uint16 u16AFMADResource;
	uint16 u16TPSADResource;
	uint32 aUserTipInCorrectionSpread[17];
	uint16 aUserTipInCorrectionTable[17];
	uint32 aLogicBlockVar[8];
	uint32 aLogicBlockOperand[8];
	uint16 aLogicBlockChainOutput[8];
	IOAPI_tenEHIOResource enThermoFanRelay;
	uint16 aESTIOMuxResource[4];
	Bool boUEGOCANPrimary;
	uint16 aPWM2DAxesSourceXIDX[8];
	uint16 aPWM3DAxesSourceXIDX[4];
	uint16 aPWM3DAxesSourceYIDX[4];
	uint16 aPWM2DIOOutputResource[8];
	uint16 aPWM3DIOOutputResource[4];
	sint32 aUserPWM2D_1SpreadX[17];
	sint32 aUserPWM2D_2SpreadX[17];
	sint32 aUserPWM2D_3SpreadX[17];
	sint32 aUserPWM2D_4SpreadX[17];
	sint32 aUserPWM2D_5SpreadX[17];
	sint32 aUserPWM2D_6SpreadX[17];
	sint32 aUserPWM2D_7SpreadX[17];
	sint32 aUserPWM2D_8SpreadX[17];
	uint16 aUserPWM2D_1Data[17];
	uint16 aUserPWM2D_2Data[17];
	uint16 aUserPWM2D_3Data[17];
	uint16 aUserPWM2D_4Data[17];
	uint16 aUserPWM2D_5Data[17];
	uint16 aUserPWM2D_6Data[17];
	uint16 aUserPWM2D_7Data[17];
	uint16 aUserPWM2D_8Data[17];
	sint32 aUserPWM3D_1SpreadX[17];
	sint32 aUserPWM3D_2SpreadX[17];
	sint32 aUserPWM3D_3SpreadX[17];
	sint32 aUserPWM3D_4SpreadX[17];	
	sint32 aUserPWM3D_1SpreadY[17];
	sint32 aUserPWM3D_2SpreadY[17];
	sint32 aUserPWM3D_3SpreadY[17];
	sint32 aUserPWM3D_4SpreadY[17];
	uint16 aUserPWM3D_1Data[17][17];
	uint16 aUserPWM3D_2Data[17][17];
	uint16 aUserPWM3D_3Data[17][17];
	uint16 aUserPWM3D_4Data[17][17];
	uint16 VVTInputResource;
	uint8 VVTInputType;
	uint16 u16CRC16;
	uint32* offsets;
} USERCAL_tstCalibration;


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
/* 	NOTE MUST ALWAYS INCLUDE CAL STRUCT ELEMENTS ONE FOR ONE AND IN ORDER */
EXTERN USERCAL_tstCalibration __attribute__ ((packed)) USERCAL_stRAMCAL;
//
//
#pragma GCC diagnostic pop//
//
// 0
//ASAM mode=writevalue name="AD RefH Voltage" parent="USERCAL_stRAMCAL" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=4.3 help="AD Converter Reference High Voltage"
//ASAM mode=writevalue name="User Cal VRef 445" parent="USERCAL_stRAMCAL" type=uint32 offset=4 min=0 max=5 m=0.001 b=0 units="TBC" format=4.3 help="TBC"
//ASAM mode=writevalue name="Cal VNernst Target" parent="USERCAL_stRAMCAL" type=uint32 offset=8 min=0 max=5 m=0.001 b=0 units="TBC" format=4.3 help="TBC"
//ASAM mode=writevalue name="Cal RNernst Ready" parent="USERCAL_stRAMCAL" type=uint32 offset=12 min=0 max=5 m=0.001 b=0 units="TBC" format=4.3 help="TBC"
//ASAM mode=writevalue name="Uego Heater Rth" parent="USERCAL_stRAMCAL" type=uint32 offset=16 min=0 max=10 m=0.001 b=0 units="Ohms" format=4.3 help="Uego Heater Resistance Threshold"
//ASAM mode=writevalue name="Uego1 Current P Cal" parent="USERCAL_stRAMCAL" type=uint32 offset=20 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Current P Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego1 Current M Cal" parent="USERCAL_stRAMCAL" type=uint32 offset=24 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Current M Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego1 Offset Cal" parent="USERCAL_stRAMCAL" type=sint32 offset=28 min=-5000000 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Offset Calibration (dimensionless)"
//ASAM mode=writeaxis_pts name="Uego Pump Current Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=32 min=-5000000 max=5000000 m=0.000001 b=0 units="mA" format=7.6 help="Uego Pump Current X Axis" xcount=17 xindexvar="Pump Current 1"
//ASAM mode=writecurve name="Uego Pump Current Table" parent="USERCAL_stRAMCAL" type=uint16 offset=100 min=0 max=5 m=0.001 b=0 units="dl" format=4.3 help="Uego Pump Current Curve" xcount=17 xindexvar="Pump Current 1"
// 1
//ASAM mode=writevalue name="Uego1 Sensor Trim" parent="USERCAL_stRAMCAL" type=uint8 offset=134 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Uego1 Sensor Trim"
//ASAM mode=writevalue name="Uego1 Stoich Cal" parent="USERCAL_stRAMCAL" type=uint32 offset=136 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="Uego1 Stoich Cal"
//ASAM mode=writevalue name="User Cal Pump Deadband" type=uint8 offset=140 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="User Cal Free Air Points" type=uint16 offset=142 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="UserHeaterAlarmCurrent" type=uint32 offset=180 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Warm Up Delay" type=uint8 offset=184 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Display Mode" type=uint8 offset=185 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Back Light Enable" type=uint8 offset=186 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="TPS Cal Min" type=uint32 offset=188 min=0 max=5000 m=1 b=0 units="V" format=3.0 help="TPS voltage at min throttle position or neutral ETC position"
//ASAM mode=writevalue name="TPS Cal Max" type=uint32 offset=192 min=0 max=5000 m=1 b=0 units="V" format=3.0 help="TPS voltage at maximum throttle position"
// 2
//ASAM mode=writeaxis_pts name="AfmTF Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=196 min=0 max=300000 m=0.001 b=0 units="V" format=7.6 help="Air Flow Meter calibration curve x-axis points array" xcount=17 xindexvar="AFM Sensor Voltage"
//ASAM mode=writecurve name="AfmTF Table" parent="USERCAL_stRAMCAL" type=uint32 offset=264 min=0 max=400 m=0.000001 b=0 units="g/s" format=4.3 help=Air Flow Meter calibration curve data points array" xcount=17 xindexvar="AFM Sensor Voltage"
//ASAM mode=writeaxis_pts name="Inj Response Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=332 min=0 max=20000 m=0.001 b=0 units="V" format=4.3 help="Injector response curve x-axis points array" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writecurve name="Inj Response Table" parent="USERCAL_stRAMCAL" type=uint16 offset=400 min=0 max=3 m=0.001 b=0 units="ms" format=4.3 help="Injector response curve data points array" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writeaxis_pts name="Coolant Sensor Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=436 min=0 max=5 m=1 b=0 units="V" format=7.0 help="Coolant sensor calibration curve x-axis points array" xcount=17 xindexvar="CTS Sensor Voltage"
//ASAM mode=writecurve name="Coolant Sensor Table" parent="USERCAL_stRAMCAL" type=sint32 offset=504 min=-40 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="Coolant sensor calibration curve data points array" xcount=17 xindexvar="CTS Sensor Voltage"
//ASAM mode=writeaxis_pts name="CTS Enrichment Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=572 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Enrichment X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Enrichment Table" parent="USERCAL_stRAMCAL" type=sint32 offset=640 min=-1 max=3 m=0.001 b=0 units="%" format=4.3 help="CTS Enrichment Curve" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="TAFR Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=708 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="TAFR Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=776 min=0 max=110000 m=0.001 b=0 units="kPa" format=3.1 help="VE MAP X Axis" xcount=17 xindexvar="MAP kPa"
// 3
//ASAM mode=writemap name="TAFR Map" parent="USERCAL_stRAMCAL" type=uint16 offset=844 min=0 max=20 m=0.01 b=0 units="dl" format=4.3 help="TAFR Map" xcount=17 xindexvar="Engine Speed Raw" ycount=17 yindexvar="Throttle Angle"
//ASAM mode=writeaxis_pts name="Timing Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=1424 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Timing Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=1492 min=0 max=110000 m=0.001 b=0 units="kPa" format=3.1 help="VE MAP X Axis" xcount=17 xindexvar="MAP kPa"
//ASAM mode=writemap name="Timing Map" parent="USERCAL_stRAMCAL" type=uint16 offset=1560 min=0 max=50 m=0.1 b=0 units="Degrees" format=3.1 help="Timing Map" xcount=17 xindexvar="Engine Speed Raw" ycount=17 yindexvar="MAP kPa"
//ASAM mode=writeaxis_pts name="VE Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=2140 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="VE Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=2208 min=0 max=110000 m=0.001 b=0 units="kPa" format=3.1 help="VE MAP X Axis" xcount=17 xindexvar="MAP kPa"
//ASAM mode=writemap name="VE Map" parent="USERCAL_stRAMCAL" type=uint16 offset=2276 min=0 max=1.5 m=0.001 b=0 units="dl" format=4.3 help="VE Map" xcount=17 xindexvar="Engine Speed Raw" ycount=17 yindexvar="MAP kPa"
//ASAM mode=writeaxis_pts name="CTS Start Enrichment Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=2856 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Start Enrichment X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Start Enrichment Table" parent="USERCAL_stRAMCAL" type=sint32 offset=2924 min=0 max=4 m=0.001 b=0 units="%" format=4.3 help="CTS Start Enrichment Curve" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="CTS Post Start Enrichment Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=2992 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Post Start Enrichment X Axis" xcount=17 xindexvar="Coolant Temperature"
// 4
//ASAM mode=writecurve name="CTS Post Start Enrichment Table" parent="USERCAL_stRAMCAL" type=sint32 offset=3060 min=0 max=4 m=0.001 b=0 units="%" format=4.3 help="CTS Post Start Enrichment Curve" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="Cranking Airflow Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=3128 min=0 max=8000 m=1 b=0 units="RPM" format=4.3 help="Cranking Airflow X Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="Cranking Airflow Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3196 min=0 max=65.535 m=0.001 b=0 units="g/s" format=4.3 help="Cranking Airflow Curve" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Dwell Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=3232 min=0 max=20000 m=0.001 b=0 units="V" format=4.3 help="Dwell X Axis" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writecurve name="Dwell Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3300 min=0 max=10 m=0.001 b=0 units="ms" format=4.3 help="Dwell Curve" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writeaxis_pts name="ATS Fuel Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=3336 min=0 max=20 m=0.001 b=0 units="V" format=4.3 help="ATS Fuel Correction X Axis" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writecurve name="ATS Fuel Correction Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3404 min=0 max=2 m=0.001 b=0 units="%" format=4.3 help="ATS Fuel Correction Curve" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writeaxis_pts name="TPS Movement Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=3440 min=-450 max=450 m=0.001 b=0 units="Degrees/s" format=4.3 help="TPS Movement Correction X Axis" xcount=17 xindexvar="TPS Movement Rate"
//ASAM mode=writecurve name="TPS Movement Correction Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3508 min=0 max=2 m=0.001 b=0 units="%" format=6.3 help="TPS Movement Correction Curve" xcount=17 xindexvar="TPS Movement Rate"
//ASAM mode=writeaxis_pts name="Injection Phasing Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=3544 min=0 max=20000 m=0.001 b=0 units="RPM" format=4.3 help="Injection Phasing X Axis" xcount=17 xindexvar="Engine Speed Raw"
// 5
//ASAM mode=writecurve name="Injection Phasing Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3612 min=0 max=20 m=0.001 b=0 units="ms" format=4.3 help="Injection Phasing Curve" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="CTS Timing Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=3648 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Timing Correction X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Timing Correction Table" parent="USERCAL_stRAMCAL" type=sint16 offset=3716 min=-10 max=10 m=0.1 b=0 units="Degrees" format=4.3 help="CTS Timing Correction Curve" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="ATS Timing Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=3752 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="ATS Timing Correction X Axis" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writecurve name="ATS Timing Correction Table" parent="USERCAL_stRAMCAL" type=sint16 offset=3820 min=-10 max=10 m=0.1 b=0 units="Degrees" format=4.3 help="ATS Timing Correction Curve" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writeaxis_pts name="ISC Target Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=3856 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="ISC Target X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="ISC Target Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3924 min=0 max=2000 m=1 b=0 units="RPM" format=4.3 help="ISC Target Curve" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writevalue name="User Stepper Close Table" type=uint32 offset=3958 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="User Stepper Open Table" type=uint32 offset=3962 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="User Stepper Home Steps" type=uint32 offset=3968 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
// 6
//ASAM mode=writeblob name="Primary Trigger Table" parent="USERCAL_stRAMCAL" type=uint16 offset=3972 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=80 help="Primary Trigger Tooth Setup"
//ASAM mode=writeblob name="Secondary Trigger Table" parent="USERCAL_stRAMCAL" type=uint16 offset=4132 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=40 help="Secondary Trigger Tooth Setup"
//ASAM mode=writeblob name="Sync Points Table" parent="USERCAL_stRAMCAL" type=uint16 offset=4212 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=36 help="Syncronisation Points Setup"
//ASAM mode=writevalue name="Primary Edge Setup" parent="USERCAL_stRAMCAL" type=uint8 offset=4284 min=0 max=255 m=1 b=0 units="dl" format=5.0 help="Primary Edge Setup"
//ASAM mode=writevalue name="Secondary Edge Setup" parent="USERCAL_stRAMCAL" type=uint8 offset=4285 min=0 max=255 m=1 b=0 units="dl" format=5.0 help="Secondary Edge Setup"
//ASAM mode=writevalue name="Primary VR Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=4286 min=0 max=255 m=1 b=0 units="bool" format=5.0 help="Primary VR Circuit Enable"
//ASAM mode=writevalue name="Secondary VR Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=4287 min=0 max=255 m=1 b=0 units="bool" format=5.0 help="Secondary VR Circuit Enable"
//ASAM mode=writevalue name="First Edge Rising Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=4288 min=0 max=255 m=1 b=0 units="bool" format=3.0 help="First Edge Rising Primary"
//ASAM mode=writevalue name="First Edge Rising Secondary" parent="USERCAL_stRAMCAL" type=uint8 offset=4289 min=0 max=255 m=1 b=0 units="bool" format=3.0 help="First Edge Rising Secondary"
//ASAM mode=writevalue name="Injection Sequence AE" type=uint32 offset=4292 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Injection Sequence BF" type=uint32 offset=4296 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Injection Sequence CG" type=uint32 offset=4300 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Injection Sequence DH" type=uint32 offset=4304 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC"

// 7
//ASAM mode=writevalue name="Ignition Sequence AE" type=uint32 offset=4308 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Ignition Sequence BF" type=uint32 offset=4312 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Ignition Sequence CG" type=uint32 offset=4316 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Ignition Sequence DH" type=uint32 offset=4320 min=0 max=65535 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Simple Cam Sync Source" Source type=uint8 offset=4324 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Cam Sync High Late" type=uint8 offset=4325 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Cam Sync Sample Tooth Count" type=uint32 offset=4328 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Manifold Volume" parent="USERCAL_stRAMCAL" type=uint16 offset=4332 min=0 max=65535 m=1 b=0 units="cc" format=5.0 help="Manifold Volume CC"
//ASAM mode=writevalue name="Cyl Count" type=uint8 offset=4334 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="Injector Flow Rate" type=uint32 offset=4336 min=0 max=20000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writeaxis_pts name="Airflow Map_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=4340 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="Airflow MAP Y Axis" xcount=11 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Airflow Map_YAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=4384 min=0 max=90000 m=0.001 b=0 units="Degrees" format=3.1 help="Airflow MAP X Axis" xcount=11 xindexvar="Throttle Angle"
//ASAM mode=writemap name="Airflow Map" parent="USERCAL_stRAMCAL" type=uint32 offset=4428 min=0 max=150 m=0.000001 b=0 units="g/s" format=9.6 help="Airflow Map" xcount=11 xindexvar="Engine Speed Raw" ycount=11 yindexvar="Throttle Angle"
// 8
//ASAM mode=writevalue name="Manifold Time Constant Cal" parent="USERCAL_stRAMCAL" type=uint8 offset=4912 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Manifold Time Constant Cal"
//ASAM mode=writevalue name="Airflow Transient Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=4913 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="Airflow Transients Enable"
//ASAM mode=writeaxis_pts name="Injector Start Primer Table_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=4916 min=0 max=2 m=0.001 b=0 units="ms" format=4.3 help="Injector Short Opening X Axis" xcount=11 xindexvar="Fuel Injector Predicted ms"
//ASAM mode=writecurve name="Injector Start Primer Table" parent="USERCAL_stRAMCAL" type=uint32 offset=4960 min=0 max=2 m=1 b=0 units="ms" format=4.3 help="Injector Short Opening Curve" xcount=11 xindexvar="Fuel Injector Predicted ms"
//ASAM mode=writeaxis_pts name="Injector Short Opening Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=5004 min=0 max=2 m=0.001 b=0 units="ms" format=4.3 help="Injector Short Opening X Axis" xcount=11 xindexvar="Fuel Injector Predicted ms"
//ASAM mode=writecurve name="Injector Short Opening Table" parent="USERCAL_stRAMCAL" type=uint32 offset=5048 min=0 max=2 m=0.001 b=0 units="ms" format=4.3 help="Injector Short Opening Curve" xcount=11 xindexvar="Fuel Injector Predicted ms"
//ASAM mode=writevalue name="STT Neg Limit" parent="USERCAL_stRAMCAL" type=uint32 offset=5092 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="STT Pos Limit" parent="USERCAL_stRAMCAL" type=uint32 offset=5096 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="TBC"
//ASAM mode=writevalue name="CLO2 Step Closed" parent="USERCAL_stRAMCAL" type=uint32 offset=5100 min=0 max=10 m=0.1 b=0 units="%" format=3.1 help="TBC"
//ASAM mode=writevalue name="CLO2 Inc Closed" parent="USERCAL_stRAMCAL" type=uint32 offset=5104 min=0 max=10 m=0.1 b=0 units="%" format=3.1 help="TBC"
// 9
//ASAM mode=writevalue name="CLO2 Step Open" parent="USERCAL_stRAMCAL" type=uint32 offset=5108 min=0 max=10 m=0.1 b=0 units="%" format=3.1 help="TBC"
//ASAM mode=writevalue name="CLO2 Inc Open" parent="USERCAL_stRAMCAL" type=uint32 offset=5112 min=0 max=10 m=0.1 b=0 units="%" format=3.1 help="TBC"
//ASAM mode=writevalue name="Timing Main Offset" parent="USERCAL_stRAMCAL" type=uint16 offset=5116 min=0 max=360 m=0.001 b=0 units="Degrees" format=1.0 help="TBC"
//ASAM mode=writevalue name="AFM Transient Control RPM Limit" parent="USERCAL_stRAMCAL" type=uint32 offset=5120 min=0 max=8000 m=1 b=0 units="RPM" format=1.0 help="TBC"
//ASAM mode=writevalue name="Wasted Spark Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5124 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="TBC"
//ASAM mode=writevalue name="AFR Reinstate" parent="USERCAL_stRAMCAL" type=uint16 offset=5126 min=0 max=1 m=0.001 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="AFR Cutoff" parent="USERCAL_stRAMCAL" type=uint16 offset=5128 min=0 max=1 m=0.001 b=0 units="Degrees" format=1.0 help="TBC"
//ASAM mode=writevalue name="TPS Closed Lower" parent="USERCAL_stRAMCAL" type=uint32 offset=5132 min=0 max=90 m=0.001 b=0 units="Degrees" format=1.0 help="TBC"
//ASAM mode=writevalue name="TPS Closed Upper" parent="USERCAL_stRAMCAL" type=uint32 offset=5136 min=0 max=90 m=0.001 b=0 units="Degrees" format=1.0 help="TBC"
//ASAM mode=writevalue name="ISC EST Trim Pos" parent="USERCAL_stRAMCAL" type=uint32 offset=5140 min=0 max=15 m=0.001 b=0 units="Degrees" format=5.3 help="ISC EST Trim Positive Max"
// 10
//ASAM mode=writevalue name="ISC EST Trim Neg" parent="USERCAL_stRAMCAL" type=uint32 offset=5144 min=0 max=15 m=0.001 b=0 units="Degrees" format=5.3 help="ISC EST Trim Negative Max"
//ASAM mode=writevalue name="CLO2 EST Trim Pos" parent="USERCAL_stRAMCAL" type=uint32 offset=5148 min=0 max=5 m=0.001 b=0 units="Degrees" format=5.3 help="Closed Loop Fuel EST Trim Positive Max"
//ASAM mode=writevalue name="CLO2 EST Trim Neg" parent="USERCAL_stRAMCAL" type=uint32 offset=5152 min=0 max=5 m=0.001 b=0 units="Degrees" format=5.3 help="Closed Loop Fuel EST Trim Negative Max"
//ASAM mode=writevalue name="Cylinder CC" parent="USERCAL_stRAMCAL" type=uint16 offset=5156 min=0 max=1 m=1 b=0 units="dl" format=1.0 help="Cylinder Capacity"
//ASAM mode=writevalue name="AFM Primary Input Enable Open" parent="USERCAL_stRAMCAL" type=uint8 offset=5158 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="AFM Primary Input Enable Open"
//ASAM mode=writevalue name="Speed Density AFM TPS Limit" parent="USERCAL_stRAMCAL" type=uint32 offset=5160 min=0 max=90 m=.001 b=0 units="degrees" format=5.3 help="TBC"
//ASAM mode=writevalue name="Seq RPM Limit AE" parent="USERCAL_stRAMCAL" type=uint16 offset=5164 min=0 max=10000 m=1 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="Seq RPM Limit BF" type=uint16 offset=5166 suboffset=2 min=0 max=10000 m=1 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="Seq RPM Limit CG" type=uint16 offset=5168 suboffset=4 min=0 max=10000 m=1 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="Seq RPM Limit DH" type=uint16 offset=5170 suboffset=6 min=0 max=10000 m=1 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="SeqRPMLimit5" type=uint16 offset=5172 suboffset=8 min=0 max=1 m=10000 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="SeqRPMLimit6" type=uint16 offset=5174 suboffset=10 min=0 max=1 m=10000 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="SeqRPMLimit7" type=uint16 offset=5176 suboffset=12 min=0 max=1 m=10000 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="SeqRPMLimit8" type=uint16 offset=5178 suboffset=14 min=0 max=1 m=10000 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="Seq RPM Limit Hyst" parent="USERCAL_stRAMCAL" type=uint16 offset=5180 min=0 max=500 m=1 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="HighVacuumEnableKpa" parent="USERCAL_stRAMCAL" type=uint16 offset=5182 min=0 max=101.3 m=0.001 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="HighVacuumDisableKpa" parent="USERCAL_stRAMCAL" type=uint16 offset=5184 min=0 max=101.3 m=0.001 b=0 units="dl" format=1.0 help="TBC"
// 11
//ASAM mode=writevalue name="OverrunCutEnableRPM" parent="USERCAL_stRAMCAL" type=uint16 offset=5186 min=0 max=2000 m=1 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="OverrunCutDisableRPM" parent="USERCAL_stRAMCAL" type=uint16 offset=5188 min=0 max=2000 m=1 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="RPMRunThreshold" parent="USERCAL_stRAMCAL" type=uint16 offset=5190 min=0 max=1 m=1 b=0 units="dl" format=1.0 help="TBC"
//ASAM mode=writevalue name="CLO2 Sequence Assignment" parent="USERCAL_stRAMCAL" type=uint8 offset=5192 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="CLO2 Sequence Assignment"
//ASAM mode=writevalue name="EST Filter Closed" parent="USERCAL_stRAMCAL" type=uint16 offset=5194 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="EST Filter Closed"
//ASAM mode=writevalue name="EST Filter Open" parent="USERCAL_stRAMCAL" type=uint16 offset=5196 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="EST Filter Open"
//ASAM mode=writevalue name="EST CL Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5198 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="EST CL Gain"
//ASAM mode=writevalue name="AFM Primary Input Enable Closed" parent="USERCAL_stRAMCAL" type=uint8 offset=5200 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="AFM Primary Input Enable Closed"
//ASAM mode=writevalue name="Sequence 1 Fuel Offset" parent="USERCAL_stRAMCAL" type=sint16 offset=5202 min=-1 max=1 m=.001 b=0 units="ms" format=4.3 help="Sequence 1 Fuel Offset"
//ASAM mode=writevalue name="Sequence 2 Fuel Offset" parent="USERCAL_stRAMCAL" type=sint16 offset=5204 min=-1 max=1 m=.001 b=0 units="ms" format=4.3 help="Sequence 2 Fuel Offset"
//ASAM mode=writevalue name="Sequence 3 Fuel Offset" parent="USERCAL_stRAMCAL" type=sint16 offset=5206 min=-1 max=1 m=.001 b=0 units="ms" format=4.3 help="Sequence 3 Fuel Offset"
//ASAM mode=writevalue name="Sequence 4 Fuel Offset" parent="USERCAL_stRAMCAL" type=sint16 offset=5208 min=-1 max=1 m=.001 b=0 units="ms" format=4.3 help="Sequence 4 Fuel Offset"
//ASAM mode=writevalue name="Sequence 1 Fuel Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5210 min=0 max=2 m=.001 b=0 units="dl" format=4.3 help="Sequence 1 Fuel Gain"
//ASAM mode=writevalue name="Sequence 2 Fuel Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5212 min=0 max=2 m=.001 b=0 units="dl" format=4.3 help="Sequence 2 Fuel Gain"
//ASAM mode=writevalue name="Sequence 3 Fuel Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5214 min=0 max=2 m=.001 b=0 units="dl" format=4.3 help="Sequence 3 Fuel Gain"
//ASAM mode=writevalue name="Sequence 4 Fuel Gain" parent="USERCAL_stRAMCAL" type=uint16 offset=5216 min=0 max=2 m=.001 b=0 units="dl" format=4.3 help="Sequence 4 Fuel Gain"
// 12
//ASAM mode=writevalue name="TPS CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5218 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="TPS CAN Enable"
//ASAM mode=writevalue name="PPS CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5219 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="PPS CAN Enable"
//ASAM mode=writevalue name="CTS CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5220 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="CTS CAN Enable"
//ASAM mode=writevalue name="ATS CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5221 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="ATS CAN Enable"
//ASAM mode=writevalue name="MAP CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5222 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="MAP CAN Enable"
//ASAM mode=writevalue name="Priority CAN ID 1" parent="USERCAL_stRAMCAL" type=uint32 offset=5224 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Priority CAN ID 1"
//ASAM mode=writevalue name="Sensor CAN Data Offset" parent="USERCAL_stRAMCAL" type=uint8 offset=5240 min=1 max=255 m=1 b=0 units="dl" format=4.0 help="Priority CAN ID 1"
//ASAM mode=writevalue name="Sensor CAN Data ByteCount" parent="USERCAL_stRAMCAL" type=uint8 offset=5248 min=1 max=255 m=1 b=0 units="dl" format=4.0 help="Priority CAN ID 1"
//ASAM mode=writevalue name="Sync Phase Repeats" parent="USERCAL_stRAMCAL" type=uint32 offset=5256 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="Map Sensor Gain" parent="USERCAL_stRAMCAL" type=sint32 offset=5260 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
// 13
//ASAM mode=writevalue name="Map Sensor Offset" parent="USERCAL_stRAMCAL" type=sint32 offset=5264 min=1 max=4095 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="Inj Divide" parent="USERCAL_stRAMCAL" type=uint8 offset=5268 min=1 max=4 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="CLO2 LeftEnable" parent="USERCAL_stRAMCAL" type=uint8 offset=5269 min=0 max=1 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="CLO2 RightEnable" parent="USERCAL_stRAMCAL" type=uint8 offset=5270 min=0 max=1 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="Returnless Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5271 min=1 max=1 m=1 b=0 units="kPa" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="Returnless Pressure kPa" parent="USERCAL_stRAMCAL" type=uint16 offset=5272 min=1 max=500 m=0.001 b=0 units="kPa" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="Cranking Airflow Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5274 min=1 max=1 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="Stepper IAC Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5275 min=1 max=1 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="IAC Steps Open Hot" parent="USERCAL_stRAMCAL" type=uint16 offset=5276 min=1 max=255 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
//ASAM mode=writevalue name="IAC Steps Open Cold" parent="USERCAL_stRAMCAL" type=uint16 offset=5278 min=1 max=255 m=1 b=0 units="dl" format=4.0 help="Sync Phase Repeats"
// 14
//ASAM mode=writevalue name="Idle Entry RPM Offset" parent="USERCAL_stRAMCAL" type=uint16 offset=5280 min=1 max=1000 m=1 b=0 units="RPM" format=4.0 help="Idle Entry RPM Offset"
//ASAM mode=writevalue name="Rad Fan On Temp" parent="USERCAL_stRAMCAL" type=sint32 offset=5284 min=-40 max=200 m=0.001 b=1 units="degC" format=4.0 help="Rad Fan On Temp"
//ASAM mode=writevalue name="Rad Fan Off Temp" parent="USERCAL_stRAMCAL" type=sint32 offset=5288 min=-40 max=200 m=0.001 b=1 units="degC" format=4.0 help="Rad Fan Off Temp"
//ASAM mode=writevalue name="Fuel Pump Relay" parent="USERCAL_stRAMCAL" type=uint8 offset=5292 min=1 max=100 m=1 b=0 units="dl" format=4.0 help="Fuel Pump Relay"
//ASAM mode=writevalue name="EST Bypass" parent="USERCAL_stRAMCAL" type=uint8 offset=5293 min=1 max=1000 m=1 b=0 units="dl" format=4.0 help="EST Bypass"
//ASAM mode=writeaxis_pts name="Air Sensor Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=5296 min=0 max=5 m=1 b=0 units="V" format=7.0 help="Air Sensor X Axis" xcount=17 xindexvar="ATS Sensor Voltage"
//ASAM mode=writecurve name="Air Sensor Table" parent="USERCAL_stRAMCAL" type=sint32 offset=5364 min=-40 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="Air Sensor Curve" xcount=17 xindexvar="ATS Sensor Voltage"
//ASAM mode=writevalue name="Fuel 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5432 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Fuel Sequence 1 IO Resource"
//ASAM mode=writevalue name="Fuel 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5434 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Fuel Sequence 2 IO Resource"
//ASAM mode=writevalue name="Fuel 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5436 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Fuel Sequence 3 IO Resource"
// 15
//ASAM mode=writevalue name="Fuel 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5438 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Fuel Sequence 4 IO Resource"
//ASAM mode=writevalue name="EST 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5440 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="EST Sequence 1 IO Resource"
//ASAM mode=writevalue name="EST 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5442 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="EST Sequence 2 IO Resource"
//ASAM mode=writevalue name="EST 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5444 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="EST Sequence 3 IO Resource"
//ASAM mode=writevalue name="EST 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5446 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="EST Sequence 4 IO Resource"
//ASAM mode=writevalue name="IAC 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5448 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="IAC 1 IO Resource"
//ASAM mode=writevalue name="IAC 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5450 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="IAC 2 IO Resource"
//ASAM mode=writevalue name="IAC 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5452 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="IAC 3 IO Resource"
//ASAM mode=writevalue name="IAC 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5454 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="IAC 4 IO Resource"
//ASAM mode=writevalue name="OBD ISC Advance Enable" parent="USERCAL_stRAMCAL" type=uint8 offset=5456 min=0 max=1 m=1 b=0 units="dl" format=1.0 help="IAC Enable OBD Idle Advance"

//ASAM mode=writevalue name="Trigger Type" parent="USERCAL_stRAMCAL" type=uint8 offset=5457 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="Trigger Type"
//ASAM mode=writevalue name="Sync Type" parent="USERCAL_stRAMCAL" type=uint8 offset=5458 min=0 max=255 m=1 b=0 units="dl" format=1.0 help="Sync Type"
//ASAM mode=writevalue name="Trigger Pull Strength" parent="USERCAL_stRAMCAL" type=uint8 offset=5459 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="Trigger Pull Strength"
//ASAM mode=writevalue name="Sync Pull Strength" parent="USERCAL_stRAMCAL" type=uint8 offset=5460 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="Sync Pull Strength"
//ASAM mode=writevalue name="Min Linear Fuel Pulse" parent="USERCAL_stRAMCAL" type=uint16 offset=5462 min=0 max=2 m=0.001 b=0 units="ms" format=1.0 help="Min Linear Fuel Pulse"

//ASAM mode=writevalue name="CTS AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5464 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="CTS AD Resource"
//ASAM mode=writevalue name="MAP AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5466 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="MAP AD Resource"
//ASAM mode=writevalue name="ATS AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5468 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="ATS AD Resource"
//ASAM mode=writevalue name="AFM AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5470 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="AFM AD Resource"
//ASAM mode=writevalue name="TPS AD Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5472 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="TPS AD Resource"


//ASAM mode=writeaxis_pts name="Tip In Correction Table_XAXIS" parent="USERCAL_stRAMCAL" type=uint32 offset=5476 min=0 max=8000 m=1 b=0 units="RPM" format=4.3 help="Tip In Correction X Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="Tip In Correction Table" parent="USERCAL_stRAMCAL" type=uint16 offset=5544 min=0 max=2 m=0.001 b=0 units="%" format=6.3 help="Tip In Correction Curve" xcount=17 xindexvar="Engine Speed Raw"


//ASAM mode=writevalue name="Logic Block Var A" parent="USERCAL_stRAMCAL" type=uint32 offset=5580 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 1"
//ASAM mode=writevalue name="Logic Block Var B" parent="USERCAL_stRAMCAL" type=uint32 offset=5584 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 2"
//ASAM mode=writevalue name="Logic Block Var C" parent="USERCAL_stRAMCAL" type=uint32 offset=5588 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 3"
//ASAM mode=writevalue name="Logic Block Var D" parent="USERCAL_stRAMCAL" type=uint32 offset=5592 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 4"
//ASAM mode=writevalue name="Logic Block Var E" parent="USERCAL_stRAMCAL" type=uint32 offset=5596 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 5"
//ASAM mode=writevalue name="Logic Block Var F" parent="USERCAL_stRAMCAL" type=uint32 offset=5600 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 6"
//ASAM mode=writevalue name="Logic Block Var G" parent="USERCAL_stRAMCAL" type=uint32 offset=5604 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 7"
//ASAM mode=writevalue name="Logic Block Var H" parent="USERCAL_stRAMCAL" type=uint32 offset=5608 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Variable 8"	

//ASAM mode=writevalue name="Logic Block Operand A" parent="USERCAL_stRAMCAL" type=uint32 offset=5612 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 1"
//ASAM mode=writevalue name="Logic Block Operand B" parent="USERCAL_stRAMCAL" type=uint32 offset=5616 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 2"
//ASAM mode=writevalue name="Logic Block Operand C" parent="USERCAL_stRAMCAL" type=uint32 offset=5620 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 3"
//ASAM mode=writevalue name="Logic Block Operand D" parent="USERCAL_stRAMCAL" type=uint32 offset=5624 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 4"
//ASAM mode=writevalue name="Logic Block Operand E" parent="USERCAL_stRAMCAL" type=uint32 offset=5628 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 5"
//ASAM mode=writevalue name="Logic Block Operand F" parent="USERCAL_stRAMCAL" type=uint32 offset=5632 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 6"
//ASAM mode=writevalue name="Logic Block Operand G" parent="USERCAL_stRAMCAL" type=uint32 offset=5636 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 7"
//ASAM mode=writevalue name="Logic Block Operand H" parent="USERCAL_stRAMCAL" type=uint32 offset=5640 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Operand 8"
	
//ASAM mode=writevalue name="Logic Block Chain Output A" parent="USERCAL_stRAMCAL" type=uint16 offset=5644 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 1"
//ASAM mode=writevalue name="Logic Block Chain Output B" parent="USERCAL_stRAMCAL" type=uint16 offset=5646 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 2"
//ASAM mode=writevalue name="Logic Block Chain Output C" parent="USERCAL_stRAMCAL" type=uint16 offset=5648 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 3"
//ASAM mode=writevalue name="Logic Block Chain Output D" parent="USERCAL_stRAMCAL" type=uint16 offset=5650 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 4"
//ASAM mode=writevalue name="Logic Block Chain Output E" parent="USERCAL_stRAMCAL" type=uint16 offset=5652 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 5"
//ASAM mode=writevalue name="Logic Block Chain Output F" parent="USERCAL_stRAMCAL" type=uint16 offset=5654 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 6"
//ASAM mode=writevalue name="Logic Block Chain Output G" parent="USERCAL_stRAMCAL" type=uint16 offset=5656 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 7"
//ASAM mode=writevalue name="Logic Block Chain Output H" parent="USERCAL_stRAMCAL" type=uint16 offset=5658 min=0 max=4294967296 m=1 b=0 units="dl" format=3.0 help="Logic Block Chain Output 8"

//ASAM mode=writevalue name="Thermofan Relay" parent="USERCAL_stRAMCAL" type=uint8 offset=5660 min=1 max=100 m=1 b=0 units="dl" format=4.0 help="Thermofan Relay"

//ASAM mode=writevalue name="EST 1 IO Mux Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5662 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="EST Sequence 1 IO Mux Resource"
//ASAM mode=writevalue name="EST 2 IO Mux Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5664 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="EST Sequence 2 IO Mux Resource"
//ASAM mode=writevalue name="EST 3 IO Mux Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5666 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="EST Sequence 3 IO Mux Resource"
//ASAM mode=writevalue name="EST 4 IO Mux Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5668 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="EST Sequence 4 IO Mux Resource"
//ASAM mode=writevalue name="UEGO CAN Primary" parent="USERCAL_stRAMCAL" type=uint8 offset=5670 min=0 max=2 m=1 b=0 units="dl" format=1.0 help="UEGO CAN Enable"

//ASAM mode=writevalue name="PWM 2D 1 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5672 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 1 Axis Source X"
//ASAM mode=writevalue name="PWM 2D 2 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5674 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 2 Axis Source X"
//ASAM mode=writevalue name="PWM 2D 3 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5676 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 3 Axis Source X"
//ASAM mode=writevalue name="PWM 2D 4 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5678 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 4 Axis Source X"
//ASAM mode=writevalue name="PWM 2D 5 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5680 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 5 Axis Source X"
//ASAM mode=writevalue name="PWM 2D 6 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5682 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 6 Axis Source X"
//ASAM mode=writevalue name="PWM 2D 7 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5684 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 7 Axis Source X"
//ASAM mode=writevalue name="PWM 2D 8 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5686 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 8 Axis Source X"
	
//ASAM mode=writevalue name="PWM 3D 1 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5688 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 1 Axis Source X"
//ASAM mode=writevalue name="PWM 3D 2 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5690 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 2 Axis Source X"
//ASAM mode=writevalue name="PWM 3D 3 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5692 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 3 Axis Source X"
//ASAM mode=writevalue name="PWM 3D 4 Axis Source X" parent="USERCAL_stRAMCAL" type=uint16 offset=5694 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 4 Axis Source X"	
	
//ASAM mode=writevalue name="PWM 3D 1 Axis Source Y" parent="USERCAL_stRAMCAL" type=uint16 offset=5696 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 1 Axis Source Y"
//ASAM mode=writevalue name="PWM 3D 2 Axis Source Y" parent="USERCAL_stRAMCAL" type=uint16 offset=5698 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 2 Axis Source Y"
//ASAM mode=writevalue name="PWM 3D 3 Axis Source Y" parent="USERCAL_stRAMCAL" type=uint16 offset=5700 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 3 Axis Source Y"
//ASAM mode=writevalue name="PWM 3D 4 Axis Source Y" parent="USERCAL_stRAMCAL" type=uint16 offset=5702 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 4 Axis Source Y"

//ASAM mode=writevalue name="PWM 2D 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5704 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 1 IO Resource"
//ASAM mode=writevalue name="PWM 2D 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5706 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 2 IO Resource"
//ASAM mode=writevalue name="PWM 2D 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5708 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 3 IO Resource"
//ASAM mode=writevalue name="PWM 2D 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5710 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 4 IO Resource"
//ASAM mode=writevalue name="PWM 2D 5 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5712 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 5 IO Resource"
//ASAM mode=writevalue name="PWM 2D 6 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5714 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 6 IO Resource"
//ASAM mode=writevalue name="PWM 2D 7 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5716 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 7 IO Resource"
//ASAM mode=writevalue name="PWM 2D 8 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5718 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 2D 8 IO Resource"

//ASAM mode=writevalue name="PWM 3D 1 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5720 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 1 IO Resource"
//ASAM mode=writevalue name="PWM 3D 2 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5722 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 2 IO Resource"
//ASAM mode=writevalue name="PWM 3D 3 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5724 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 3 IO Resource"
//ASAM mode=writevalue name="PWM 3D 4 IO Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=5726 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="PWM 3D 4 IO Resource"

//ASAM mode=writeaxis_pts name="PWM 2D Table 1_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5728 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 1 X Axis" xcount=17 xindexvar="Generic Source IDX1"
//ASAM mode=writeaxis_pts name="PWM 2D Table 2_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5796 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 2 X Axis" xcount=17 xindexvar="Generic Source IDX2"
//ASAM mode=writeaxis_pts name="PWM 2D Table 3_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5864 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 3 X Axis" xcount=17 xindexvar="Generic Source IDX3"
//ASAM mode=writeaxis_pts name="PWM 2D Table 4_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=5932 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 4 X Axis" xcount=17 xindexvar="Generic Source IDX4"
//ASAM mode=writeaxis_pts name="PWM 2D Table 5_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6000 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 5 X Axis" xcount=17 xindexvar="Generic Source IDX5"
//ASAM mode=writeaxis_pts name="PWM 2D Table 6_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6068 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 6 X Axis" xcount=17 xindexvar="Generic Source IDX6"
//ASAM mode=writeaxis_pts name="PWM 2D Table 7_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6136 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 7 X Axis" xcount=17 xindexvar="Generic Source IDX7"
//ASAM mode=writeaxis_pts name="PWM 2D Table 8_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6204 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 2D 8 X Axis" xcount=17 xindexvar="Generic Source IDX8"
		
//ASAM mode=writecurve name="PWM 2D Table 1" parent="USERCAL_stRAMCAL" type=uint16 offset=6272 min=0 max=100 m=0.1 b=0 units="%" format=4.3 help="PWM 2D Curve 1" xcount=17 xindexvar="Generic Source IDX1"
//ASAM mode=writecurve name="PWM 2D Table 2" parent="USERCAL_stRAMCAL" type=uint16 offset=6306 min=0 max=100 m=0.1 b=0 units="%" format=4.3 help="PWM 2D Curve 2" xcount=17 xindexvar="Generic Source IDX2"
//ASAM mode=writecurve name="PWM 2D Table 3" parent="USERCAL_stRAMCAL" type=uint16 offset=6340 min=0 max=100 m=0.1 b=0 units="%" format=4.3 help="PWM 2D Curve 3" xcount=17 xindexvar="Generic Source IDX3"
//ASAM mode=writecurve name="PWM 2D Table 4" parent="USERCAL_stRAMCAL" type=uint16 offset=6374 min=0 max=100 m=0.1 b=0 units="%" format=4.3 help="PWM 2D Curve 4" xcount=17 xindexvar="Generic Source IDX4"
//ASAM mode=writecurve name="PWM 2D Table 5" parent="USERCAL_stRAMCAL" type=uint16 offset=6408 min=0 max=100 m=0.1 b=0 units="%" format=4.3 help="PWM 2D Curve 5" xcount=17 xindexvar="Generic Source IDX5"
//ASAM mode=writecurve name="PWM 2D Table 6" parent="USERCAL_stRAMCAL" type=uint16 offset=6442 min=0 max=100 m=0.1 b=0 units="%" format=4.3 help="PWM 2D Curve 6" xcount=17 xindexvar="Generic Source IDX6"
//ASAM mode=writecurve name="PWM 2D Table 7" parent="USERCAL_stRAMCAL" type=uint16 offset=6476 min=0 max=100 m=0.1 b=0 units="%" format=4.3 help="PWM 2D Curve 7" xcount=17 xindexvar="Generic Source IDX7"
//ASAM mode=writecurve name="PWM 2D Table 8" parent="USERCAL_stRAMCAL" type=uint16 offset=6510 min=0 max=100 m=0.1 b=0 units="%" format=4.3 help="PWM 2D Curve 8" xcount=17 xindexvar="Generic Source IDX8"

//ASAM mode=writeaxis_pts name="PWM 3D Map 1_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6544 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 1 X Axis" xcount=17 xindexvar="Generic Source IDX9"
//ASAM mode=writeaxis_pts name="PWM 3D Map 2_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6612 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 2 X Axis" xcount=17 xindexvar="Generic Source IDX10"
//ASAM mode=writeaxis_pts name="PWM 3D Map 3_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6680 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 3 X Axis" xcount=17 xindexvar="Generic Source IDX11"
//ASAM mode=writeaxis_pts name="PWM 3D Map 4_XAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6748 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 4 X Axis" xcount=17 xindexvar="Generic Source IDX12"
//ASAM mode=writeaxis_pts name="PWM 3D Map 1_YAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6816 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 1 Y Axis" xcount=17 xindexvar="Generic Source IDX13"
//ASAM mode=writeaxis_pts name="PWM 3D Map 2_YAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6884 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 2 Y Axis" xcount=17 xindexvar="Generic Source IDX14"
//ASAM mode=writeaxis_pts name="PWM 3D Map 3_YAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=6952 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 3 Y Axis" xcount=17 xindexvar="Generic Source IDX15"
//ASAM mode=writeaxis_pts name="PWM 3D Map 4_YAXIS" parent="USERCAL_stRAMCAL" type=sint32 offset=7020 min=0 max=65535 m=1 b=0 units="%" format=4.3 help="PWM 3D 4 Y Axis" xcount=17 xindexvar="Generic Source IDX16"
	
//ASAM mode=writemap name="PWM 3D Map 1" parent="USERCAL_stRAMCAL" type=uint16 offset=7088 min=0 max=100 m=0.1 b=0 units="dl" format=4.3 help="PWM 3D Map 1" xcount=17 xindexvar="Generic Source IDX9" ycount=17 yindexvar="Generic Source IDX13"
//ASAM mode=writemap name="PWM 3D Map 2" parent="USERCAL_stRAMCAL" type=uint16 offset=7666 min=0 max=100 m=0.1 b=0 units="dl" format=4.3 help="PWM 3D Map 2" xcount=17 xindexvar="Generic Source IDX10" ycount=17 yindexvar="Generic Source IDX14"
//ASAM mode=writemap name="PWM 3D Map 3" parent="USERCAL_stRAMCAL" type=uint16 offset=8244 min=0 max=100 m=0.1 b=0 units="dl" format=4.3 help="PWM 3D Map 3" xcount=17 xindexvar="Generic Source IDX11" ycount=17 yindexvar="Generic Source IDX15"
//ASAM mode=writemap name="PWM 3D Map 4" parent="USERCAL_stRAMCAL" type=uint16 offset=8822 min=0 max=100 m=0.1 b=0 units="dl" format=4.3 help="PWM 3D Map 4" xcount=17 xindexvar="Generic Source IDX12" ycount=17 yindexvar="Generic Source IDX16"

//ASAM mode=writevalue name="VVT Input Resource" parent="USERCAL_stRAMCAL" type=uint16 offset=9400 min=1 max=100 m=1 b=0 units="dl" format=4.0 help="VVT Input Resource"
//ASAM mode=writevalue name="VVT Input Type" parent="USERCAL_stRAMCAL" type=uint8 offset=9402 min=1 max=100 m=1 b=0 units="dl" format=4.0 help="VVT Input Type"


//ASAM mode=writevalue name="CAL CRC" parent="USERCAL_stRAMCAL" type=uint8 offset=9404 min=0 max=255 m=1 b=0 units="counts" format=3.0 help="CAL CRC16"
/* 	NOTE MUST ALWAYS INCLUDE CAL STRUCT ELEMENTS ONE FOR ONE AND IN ORDER */

/* Testing offsets table for ASAM parsing tool here */
#ifdef _USERCAL_C

#define OFFSETS_DATA \
{ \
	offsetof(USERCAL_tstCalibration, userCalADRefH),\
	offsetof(USERCAL_tstCalibration, userCalVRef445),\
	offsetof(USERCAL_tstCalibration, userCalVNernstTarget),\
	offsetof(USERCAL_tstCalibration, userCalRNernstReady),\
	offsetof(USERCAL_tstCalibration, userCalRHeaterThreshold),\
	offsetof(USERCAL_tstCalibration, au32UserCalPumpCurrentPFactor),\
	offsetof(USERCAL_tstCalibration, au32UserCalPumpCurrentMFactor),\
	offsetof(USERCAL_tstCalibration, ai32UserCalPumpCurrentOffset),\
	offsetof(USERCAL_tstCalibration, aUserCalPumpSpread),\
	offsetof(USERCAL_tstCalibration, aUserCalPumpLambda),\
	offsetof(USERCAL_tstCalibration, au8UserCalPumpSensorTrim),\
	offsetof(USERCAL_tstCalibration, aUserStoichFuelCal),\
	offsetof(USERCAL_tstCalibration, au8UserCalPumpDeadband),\
	offsetof(USERCAL_tstCalibration, au16UserCalFreeAirPoints),\
	offsetof(USERCAL_tstCalibration, aUserHeaterAlarmCurrent),\
	offsetof(USERCAL_tstCalibration, u8WarmUpDelay),\
	offsetof(USERCAL_tstCalibration, u8DisplayMode),\
	offsetof(USERCAL_tstCalibration, u8BackLightEnable),\
	offsetof(USERCAL_tstCalibration, userCalTPSCalMin),\
	offsetof(USERCAL_tstCalibration, userCalTPSCalMax),\
	offsetof(USERCAL_tstCalibration, aUserCURVEAfmTFSpread),\
	offsetof(USERCAL_tstCalibration, aUserCURVEAfmTFTable),\
	offsetof(USERCAL_tstCalibration, aUserInjResponseSpread),\
	offsetof(USERCAL_tstCalibration, aUserInjResponseTable),\
	offsetof(USERCAL_tstCalibration, aUserCoolantSensorSpread),\
	offsetof(USERCAL_tstCalibration, aUserCoolantSensorTable),\
	offsetof(USERCAL_tstCalibration, aUserCoolantEnrichmentSpread),\
	offsetof(USERCAL_tstCalibration, aUserCoolantEnrichmentTable),\
	offsetof(USERCAL_tstCalibration, aUserTAFRxSpread),\
	offsetof(USERCAL_tstCalibration, aUserTAFRySpread),\
	offsetof(USERCAL_tstCalibration, aUserMAPTAFR),\
	offsetof(USERCAL_tstCalibration, aUserTimingxSpread),\
	offsetof(USERCAL_tstCalibration, aUserTimingySpread),\
	offsetof(USERCAL_tstCalibration, aUserTimingMap),\
	offsetof(USERCAL_tstCalibration, aUserVExSpread),\
	offsetof(USERCAL_tstCalibration, aUserVEySpread),\
	offsetof(USERCAL_tstCalibration, aUserMAPVE),\
	offsetof(USERCAL_tstCalibration, aUserCoolantStartEnrichmentSpread),\
	offsetof(USERCAL_tstCalibration, aUserCoolantStartEnrichmentTable),\
	offsetof(USERCAL_tstCalibration, aUserCoolantPostStartEnrichmentSpread),\
	offsetof(USERCAL_tstCalibration, aUserCoolantPostStartEnrichmentTable),\
	offsetof(USERCAL_tstCalibration, aUserCrankingAirflowSpread),\
	offsetof(USERCAL_tstCalibration, aUserCrankingAirflowTable),\
	offsetof(USERCAL_tstCalibration, aUserDwellSpread),\
	offsetof(USERCAL_tstCalibration, aUserDwellTable),\
	offsetof(USERCAL_tstCalibration, aUserAirTempCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserAirTempCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aUserTPSMovementCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserTPSMovementCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aUserInjectionPhasingSpread),\
	offsetof(USERCAL_tstCalibration, aUserInjectionPhasingTable),\
	offsetof(USERCAL_tstCalibration, aUserCTSTimingCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserCTSTimingCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aUserATSTimingCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserATSTimingCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aUserISCSpeedTargetSpread),\
	offsetof(USERCAL_tstCalibration, aUserISCSpeedTargetTable),\
	offsetof(USERCAL_tstCalibration, aUserStepperCloseTable),\
	offsetof(USERCAL_tstCalibration, aUserStepperOpenTable),\
	offsetof(USERCAL_tstCalibration, u32UserStepperHomeSteps),\
	offsetof(USERCAL_tstCalibration, aUserPrimaryTriggerTable),\
	offsetof(USERCAL_tstCalibration, aUserSecondaryTriggerTable),\
	offsetof(USERCAL_tstCalibration, aUserSyncPointsTable),\
	offsetof(USERCAL_tstCalibration, u8UserPrimaryEdgeSetup),\
	offsetof(USERCAL_tstCalibration, u8UserSecondaryEdgeSetup),\
	offsetof(USERCAL_tstCalibration, u8UserPrimaryVREnable),\
	offsetof(USERCAL_tstCalibration, u8UserSecondaryVREnable),\
	offsetof(USERCAL_tstCalibration, u8UserFirstEdgeRisingPrimary),\
	offsetof(USERCAL_tstCalibration, u8UserFirstEdgeRisingSecondary),\
	offsetof(USERCAL_tstCalibration, au32InjectionSequence),\
	offsetof(USERCAL_tstCalibration, au32IgnitionSequence),\
	offsetof(USERCAL_tstCalibration, enSimpleCamSyncSource),\
	offsetof(USERCAL_tstCalibration, boCamSyncHighLate),\
	offsetof(USERCAL_tstCalibration, u32CamSyncSampleToothCount),\
	offsetof(USERCAL_tstCalibration, u16ManifoldVolumeCC),\
	offsetof(USERCAL_tstCalibration, u8CylCount),\
	offsetof(USERCAL_tstCalibration, u16InjFlowRate),\
	offsetof(USERCAL_tstCalibration, aUserBackupAirflowxSpread),\
	offsetof(USERCAL_tstCalibration, aUserBackupAirflowySpread),\
	offsetof(USERCAL_tstCalibration, aUserBackupAirflowMap),\
	offsetof(USERCAL_tstCalibration, u8ManifoldTimeConstantTrim),\
	offsetof(USERCAL_tstCalibration, u8EnableBackupAirflowTransients),\
	offsetof(USERCAL_tstCalibration, aUserPrimerSpread),\
	offsetof(USERCAL_tstCalibration, aUserPrimerTable),\
	offsetof(USERCAL_tstCalibration, aUserInjShortOpeningSpread),\
	offsetof(USERCAL_tstCalibration, aUserInjShortOpeningTable),\
	offsetof(USERCAL_tstCalibration, u32STTNegLimit),\
	offsetof(USERCAL_tstCalibration, u32STTPosLimit),\
	offsetof(USERCAL_tstCalibration, u32CLStepClosed),\
	offsetof(USERCAL_tstCalibration, u32CLIncClosed),\
	offsetof(USERCAL_tstCalibration, u32CLStepOpen),\
	offsetof(USERCAL_tstCalibration, u32CLIncOpen),\
	offsetof(USERCAL_tstCalibration, u16TimingMainOffset),\
	offsetof(USERCAL_tstCalibration, u32AFMTransientControlRPMLimit),\
	offsetof(USERCAL_tstCalibration, u8WastedSparkEnable),\
	offsetof(USERCAL_tstCalibration, u16AFRReinstate),\
	offsetof(USERCAL_tstCalibration, u16AFRCutoff),\
	offsetof(USERCAL_tstCalibration, u32TPSClosedLower),\
	offsetof(USERCAL_tstCalibration, u32TPSClosedUpper),\
	offsetof(USERCAL_tstCalibration, u32ISCESTTrimPos),\
	offsetof(USERCAL_tstCalibration, u32ISCESTTrimNeg),\
	offsetof(USERCAL_tstCalibration, u32CLO2ESTTrimPos),\
	offsetof(USERCAL_tstCalibration, u32CLO2ESTTrimNeg),\
	offsetof(USERCAL_tstCalibration, u16CylinderCC),\
	offsetof(USERCAL_tstCalibration, u8EnableAFMPrimaryInputOpen),\
	offsetof(USERCAL_tstCalibration, u32SpeedDensityAFMTPSLim),\
	offsetof(USERCAL_tstCalibration, au16SeqRPMLimit),\
	offsetof(USERCAL_tstCalibration, u16SeqRPMLimitHyst),\
	offsetof(USERCAL_tstCalibration, u16HighVacuumEnableKpa),\
	offsetof(USERCAL_tstCalibration, u16HighVacuumDisableKpa),\
	offsetof(USERCAL_tstCalibration, u16OverrunCutEnableRPM),\
	offsetof(USERCAL_tstCalibration, u16OverrunCutDisableRPM),\
	offsetof(USERCAL_tstCalibration, u16RPMRunThreshold),\
	offsetof(USERCAL_tstCalibration, u8SeqFuelAssign),\
	offsetof(USERCAL_tstCalibration, u16ESTFilterClosed),\
	offsetof(USERCAL_tstCalibration, u16ESTFilterOpen),\
	offsetof(USERCAL_tstCalibration, u16ESTCLGain),\
	offsetof(USERCAL_tstCalibration, u8EnableAFMPrimaryInputClosed),\
	offsetof(USERCAL_tstCalibration, s16SequenceFuelOffset),\
	offsetof(USERCAL_tstCalibration, u16SequenceFuelGain),\
	offsetof(USERCAL_tstCalibration, boTPSCANPrimary),\
	offsetof(USERCAL_tstCalibration, boPPSCANPrimary),\
	offsetof(USERCAL_tstCalibration, boCTSCANPrimary),\
	offsetof(USERCAL_tstCalibration, boATSCANPrimary),\
	offsetof(USERCAL_tstCalibration, boMAPCANPrimary),\
	offsetof(USERCAL_tstCalibration, au32PrioCANID),\
	offsetof(USERCAL_tstCalibration, au8SensorCANDataOffset),\
	offsetof(USERCAL_tstCalibration, au8SensorCANDataByteCount),\
	offsetof(USERCAL_tstCalibration, u32SyncPhaseRepeats),\
	offsetof(USERCAL_tstCalibration, s32MapSensorGain),\
	offsetof(USERCAL_tstCalibration, s32MapSensorOffset),\
	offsetof(USERCAL_tstCalibration, u8InjDivide),\
	offsetof(USERCAL_tstCalibration, u8CLO2Leftenable),\
	offsetof(USERCAL_tstCalibration, u8CLO2RightEnable),\
	offsetof(USERCAL_tstCalibration, u8ReturnlessEnable),\
	offsetof(USERCAL_tstCalibration, u16ReturnlessPressureKPa),\
	offsetof(USERCAL_tstCalibration, u8CrankingAirflowEnable),\
	offsetof(USERCAL_tstCalibration, u8StepperIACEnable),\
	offsetof(USERCAL_tstCalibration, u16IACStepsOpenHot),\
	offsetof(USERCAL_tstCalibration, u16IACStepsOpenCold),\
	offsetof(USERCAL_tstCalibration, u16IdleEntryOffset),\
	offsetof(USERCAL_tstCalibration, s32RadFanOnTemp),\
	offsetof(USERCAL_tstCalibration, s32RadFanOffTemp),\
	offsetof(USERCAL_tstCalibration, enFuelPumpRelay),\
	offsetof(USERCAL_tstCalibration, enESTBypass),\
	offsetof(USERCAL_tstCalibration, aUserAirSensorSpread),\
	offsetof(USERCAL_tstCalibration, aUserAirSensorTable),\
	offsetof(USERCAL_tstCalibration, aFuelIOResource),\
	offsetof(USERCAL_tstCalibration, aESTIOResource),\
	offsetof(USERCAL_tstCalibration, aIACIOResource),\
	offsetof(USERCAL_tstCalibration, boOBDISCADV),\
	offsetof(USERCAL_tstCalibration, u8TriggerType),\
	offsetof(USERCAL_tstCalibration, u8SyncType),\
	offsetof(USERCAL_tstCalibration, u8TriggerPullStrength),\
	offsetof(USERCAL_tstCalibration, u8SyncPullStrength),\
	offsetof(USERCAL_tstCalibration, u16MinLinearFuelPulse),\
	offsetof(USERCAL_tstCalibration, u16CTSADResource),\
	offsetof(USERCAL_tstCalibration, u16MAPADResource),\
	offsetof(USERCAL_tstCalibration, u16ATSADResource),\
	offsetof(USERCAL_tstCalibration, u16AFMADResource),\
	offsetof(USERCAL_tstCalibration, u16TPSADResource),\
	offsetof(USERCAL_tstCalibration, aUserTipInCorrectionSpread),\
	offsetof(USERCAL_tstCalibration, aUserTipInCorrectionTable),\
	offsetof(USERCAL_tstCalibration, aLogicBlockVar),\
	offsetof(USERCAL_tstCalibration, aLogicBlockOperand),\
	offsetof(USERCAL_tstCalibration, aLogicBlockChainOutput),\
	offsetof(USERCAL_tstCalibration, enThermoFanRelay),\
	offsetof(USERCAL_tstCalibration, aESTIOMuxResource),\
	offsetof(USERCAL_tstCalibration, boUEGOCANPrimary),\
	offsetof(USERCAL_tstCalibration, aPWM2DAxesSourceXIDX),\
	offsetof(USERCAL_tstCalibration, aPWM3DAxesSourceXIDX),\
	offsetof(USERCAL_tstCalibration, aPWM3DAxesSourceYIDX),\
	offsetof(USERCAL_tstCalibration, aPWM2DIOOutputResource),\
	offsetof(USERCAL_tstCalibration, aPWM3DIOOutputResource),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_1SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_2SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_3SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_4SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_5SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_6SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_7SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_8SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_1Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_2Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_3Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_4Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_5Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_6Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_7Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM2D_8Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_1SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_2SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_3SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_4SpreadX),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_1SpreadY),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_2SpreadY),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_3SpreadY),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_4SpreadY),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_1Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_2Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_3Data),\
	offsetof(USERCAL_tstCalibration, aUserPWM3D_4Data),\
	offsetof(USERCAL_tstCalibration, VVTInputResource),\
	offsetof(USERCAL_tstCalibration, VVTInputType),\
	offsetof(USERCAL_tstCalibration, u16CRC16)}

const uint32 __attribute__((used)) au32Offsets[] = OFFSETS_DATA;
//ASAM mode=readvalue name="au32Offsets" type=uint32 offset=9408 min=0 max=65535 m=1 b=0 units="dl" format=8.0 help="Internal"
#endif

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void USERCAL_vStart(puint32 const pu32Arg);/*CR1_1*/
void USERCAL_vRun(puint32 const pu32Arg);/*CR1_1*/
void USERCAL_vTerminate(puint32 const pu32Arg);/*CR1_1*/


#endif // USERCAL_H

