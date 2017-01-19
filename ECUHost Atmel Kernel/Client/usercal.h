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

#define USERCAL_nROMCAL		\
{													\
	3290u,									\
	4530u,									\
	4450u,									\
	300000u,								\
	6500u,									\
	{9300u, 								\
	9300u},									\
	{9300u, 								\
	9300u},									\
	{2u, 										\
	2u},										\
	{-1850000u,							\
  -1465000u,							\
	-1080000u,							\
	-920000u,								\
	-760000u,								\
	-615000u,								\
	-470000u,								\
	-235000u,								\
	0,											\
	170000u,								\
	340000u,								\
	510000u,								\
	680000u,								\
	815000u,								\
	950000u,								\
	1400000u,								\
	2550000u},							\
	{700u,									\
	750u,										\
	800u,										\
	825,										\
	850u,										\
	875u,										\
	900u,										\
	955u,										\
	1000u,									\
	1094u,									\
	1180u,									\
	1305u,									\
	1430u,									\
	1565u,									\
	1700u,									\
	2350u,									\
	4160u},									\
	74u,										\
	74u,										\
	1470u,									\
	2u,											\
	2u,											\
	3310u,									\
	3320u,									\
	3330u,									\
	3340u,									\
	3350u,									\
	3360u,									\
	3370u,									\
	3380u,									\
	3390u,									\
	3400u,									\
	3410u,									\
	3420u,									\
	3430u,									\
	3440u,									\
	3450u,									\
	3460u,									\
	3470u,									\
	3480u,									\
	3490u,									\
	2600u,									\
	2600u,									\
	10u,										\
	1u,											\
	1u,											\
	400u,										\
	2500u,									\
	{0u,										\
  10000u,									\
	20000u,									\
	30000u,									\
	40000u,									\
	50000u,									\
	60000u,									\
	70000u,									\
	80000u,									\
	90000u,									\
	100000u,								\
	110000u,								\
	120000u,								\
	130000u,								\
	140000u,								\
	150000u,								\
	160000u},								\
	{620u,									\
  620u,										\
	625u,										\
	630u,										\
	635u,										\
	640u,										\
	645u,										\
	645u,										\
	645u,										\
	645u,										\
	645u,										\
	645u,										\
	645u,										\
	645u,										\
	645u,										\
	645u,										\
	645u},									\
	{7500u,									\
  8000u,									\
	8500u,									\
	9000u,									\
	9500u,									\
	10000u,									\
	10500u,									\
	11000u,									\
	11500u,									\
	12000u,									\
	12500u,									\
	13000u,									\
	13500u,									\
	14000u,									\
	14500u,									\
	15000u,									\
	15500u},								\
	{400u,									\
  380u,										\
	360u,										\
	340u,										\
	320u,										\
	300u,										\
	290u,										\
	280u,										\
	270u,										\
	260u,										\
	250u,										\
	240u,										\
	240u,										\
	240u,										\
	240u,										\
	240u,										\
	240u},									\
	{204u,									\
	300u,										\
	392u,										\
	607u,										\
	803u,										\
	1016u,									\
	1412u,									\
	1780u,									\
	2120u,									\
	2445u,									\
	2965u,									\
	3592u,									\
	4103u,									\
	4705u,									\
	4831u,									\
  4907u,									\
	4976u},									\
	{200000,								\
	170000,									\
	140000,									\
	132000,									\
	122000,									\
	110000,									\
	100000,									\
	87000,									\
	79000,									\
	71000,									\
	60000,									\
	43000,									\
	33000,									\
	5000,										\
	-6000,									\
  -18000,									\
	-40000},								\
	{-40000,								\
  -30000,									\
	-20000,									\
	-10000,									\
	0,											\
	10000,									\
	20000,									\
	30000,									\
	40000,									\
	50000,									\
	60000,									\
	70000,									\
	80000,									\
	90000,									\
	100000,									\
	110000,									\
	120000},								\
	{5000u,									\
  4000u,									\
	2000u,									\
	1800u,									\
	1600u,									\
	1500u,									\
	1400u,									\
	1300u,									\
	1200u,									\
	1100u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u},									\
	{0u,										\
  500u,										\
	1000u,									\
	1500u,									\
	2000u,									\
	2500u,									\
	3000u,									\
	3500u,									\
	4000u,									\
	4500u,									\
	5000u,									\
	6500u,									\
	7000u,									\
	7500u,									\
	8000u,									\
	8500u,									\
	9000u},									\
	{1000u,									\
  1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1015u,									\
	1030u,									\
	1045u,									\
	1035u,									\
	1035u,									\
	1030u,									\
	1035u,									\
	1040u,									\
	1045u,									\
	1050u,									\
	1055u,									\
	1060u},									\
	{0u,										\
  10000u,									\
	20000u,									\
	30000u,									\
	40000u,									\
	50000u,									\
	60000u,									\
	70000u,									\
	80000u,									\
	90000u,									\
	100000u,								\
	110000u,								\
	120000u,								\
	130000u,								\
	140000u,								\
	150000u,								\
	160000u},								\
	{0u,										\
  0u,											\
	50u,										\
	100u,										\
	120u,										\
	90u,										\
	70u,										\
	50u,										\
	30u,										\
	15u,										\
	0u,											\
	0u,											\
	0u,											\
	0u,											\
	0u,											\
	0u,											\
	0u},										\
	{0u,										\
  500u,										\
	1000u,									\
	1500u,									\
	2000u,									\
	2500u,									\
	3000u,									\
	3500u,									\
	4000u,									\
	4500u,									\
	5000u,									\
	6500u,									\
	7000u,									\
	7500u,									\
	8000u,									\
	8500u,									\
	9000u},									\
	{0u,										\
  0u,											\
	0u,											\
	0u,											\
	20u,										\
	50u,										\
	60u,										\
	80u,										\
	80u,										\
	80u,										\
	90u,										\
	90u,										\
	90u,										\
	90u,										\
	90u,										\
	90u,										\
	90u},										\
	{-40000,								\
  -30000,									\
	-20000,									\
	-10000,									\
	0,											\
	10000,									\
	20000,									\
	30000,									\
	40000,									\
	50000,									\
	60000,									\
	70000,									\
	80000,									\
	90000,									\
	100000,									\
	110000,									\
	120000},								\
	{6000u,									\
  5000u,									\
	3200u,									\
	2500u,									\
	2000u,									\
	1600u,									\
	1400u,									\
	1300u,									\
	1200u,									\
	1200u,									\
	1200u,									\
	1200u,									\
	1200u,									\
	1200u,									\
	1200u,									\
	1200u,									\
	1200u},									\
	{-40000,								\
  -30000,									\
	-20000,									\
	-10000,									\
	0,											\
	10000,									\
	20000,									\
	30000,									\
	40000,									\
	50000,									\
	60000,									\
	70000,									\
	80000,									\
	90000,									\
	100000,									\
	110000,									\
	120000},								\
	{5000u,									\
  4000u,									\
	2000u,									\
	1800u,									\
	1600u,									\
	1500u,									\
	1400u,									\
	1300u,									\
	1200u,									\
	1200u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u},									\
	{7500u,									\
  8000u,									\
	8500u,									\
	9000u,									\
	9500u,									\
	10000u,									\
	10500u,									\
	11000u,									\
	11500u,									\
	12000u,									\
	12500u,									\
	13000u,									\
	13500u,									\
	14000u,									\
	14500u,									\
	15000u,									\
	15500u},								\
	{3000u,									\
  2750u,									\
	2500u,									\
	2250u,									\
	2000u,									\
	1750u,									\
	1500u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u},									\
	{7500u,									\
		8000u,									\
		8500u,									\
		9000u,									\
		9500u,									\
		10000u,									\
		10500u,									\
		11000u,									\
		11500u,									\
		12000u,									\
		12500u,									\
		13000u,									\
		13500u,									\
		14000u,									\
		14500u,									\
		15000u,									\
	15500u},								\
	{10000u,									\
		9000u,									\
		8000u,									\
		7000u,									\
		6000u,									\
		5000u,									\
		4500u,									\
		4000u,									\
		4000u,									\
		4000u,									\
		4000u,									\
		4000u,									\
		4000u,									\
		4000u,									\
		4000u,									\
		4000u,									\
	4000u},									\
	{5,											\
	9,											\
	10,											\
	6},											\
	{6,											\
	10,											\
	9,											\
	5},											\
	12,											\
	2000,										\
	0x1234									\
}			

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
typedef __packed struct
/* The usercal struct MUST be single and packed with CRC16 last two bytes
	 for the Kernel CRC16 NVM store and restore to function */
{
	GPM6_ttVolts userCalADRefH;
	GPM6_ttVolts userCalVRef445;	
	GPM6_ttVolts userCalVNernstTarget;
	GPM6_ttOhms	userCalRNernstReady;
	GPM6_ttOhms	userCalRHeaterThreshold;	
	uint32 au32UserCalPumpCurrentPFactor[2];	
	uint32 au32UserCalPumpCurrentMFactor[2];		
	sint32 ai32UserCalPumpCurrentOffset[2];		
	GPM6_ttMicroAmps aUserCalPumpSpread[17];	
	GPM6_ttLambda aUserCalPumpLambda[17];
	uint8 au8UserCalPumpSensorTrim[2];
	uint32 aUserStoichFuelCal;
	uint8 au8UserCalPumpDeadband[2];	
	uint16 au16UserCalFreeAirPoints[19];
	GPM6_ttAmps aUserHeaterAlarmCurrent[2];	
	uint8 u8WarmUpDelay;
	uint8 u8DisplayMode;
	uint8 u8BackLightEnable;
	GPM6_ttVolts userCalTPSCalMin;
	GPM6_ttVolts userCalTPSCalMax;	
	GPM6_ttPa aUserMAPVExSpread[17];	
	uint16 aUserMAPVExTable[17];	
	GPM6_ttVolts aUserInjResponseSpread[17];	
	uint16 aUserInjResponseTable[17];		
	uint32 aUserCoolantSensorSpread[17];	
	GPM6_ttTempC aUserCoolantSensorTable[17];			
	GPM6_ttTempC aUserCoolantEnrichmentSpread[17];	
	sint32 aUserCoolantEnrichmentTable[17];	
	uint32 aUserMAPVEySpread[17];	
	uint16 aUserMAPVEyTable[17];	
	GPM6_ttPa aUserTimingxSpread[17];	
	uint16 aUserTimingxTable[17];	
	uint32 aUserTimingySpread[17];	
	uint16 aUserTimingyTable[17];	
	GPM6_ttTempC aUserCoolantStartEnrichmentSpread[17];	
	sint32 aUserCoolantStartEnrichmentTable[17];		
	GPM6_ttTempC aUserCoolantPostStartEnrichmentSpread[17];	
	sint32 aUserCoolantPostStartEnrichmentTable[17];		
	GPM6_ttVolts aUserInjPeakTimeSpread[17];	
	uint16 aUserInjPeakTimeTable[17];	
	uint32 aUserDwellSpread[17];
	uint16 aUserDwellTable[17];
	uint8 aUserStepperCloseTable[4];
	uint8 aUserStepperOpenTable[4];
	uint32 u32UserStepperHomeSteps;
	uint16 u16ManifoldVolumeCC;	
	uint16 u16CRC16;
} USERCAL_tstCalibration;


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
/* 	NOTE MUST ALWAYS INCLUDE AND ASAM TAG FOR FIRST AND LAST VAR IN CAL STRUCT */
EXTERN USERCAL_tstCalibration USERCAL_stRAMCAL;
//ASAM mode=writevalue name="AD RefH Voltage" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=4.3 help="AD Converter Reference High Voltage"
//ASAM mode=writevalue name="Uego Heater Rth" type=uint32 offset=16 min=0 max=10 m=0.001 b=0 units="Ohms" format=4.3 help="Uego Heater Resistance Threshold"
//ASAM mode=writevalue name="Uego1 Current P Cal" type=uint32 offset=20 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Current P Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego2 Current P Cal" type=uint32 offset=24 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego2 Current P Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego1 Current M Cal" type=uint32 offset=28 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Current M Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego2 Current M Cal" type=uint32 offset=32 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego2 Current M Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego1 Offset Cal" type=sint32 offset=36 min=-5000000 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Offset Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego2 Offset Cal" type=sint32 offset=40 min=-5000000 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego2 Offset Calibration (dimensionless)"
//ASAM mode=writeaxis_pts name="Uego Pump Current Table_XAXIS" type=sint32 offset=44 min=-5000000 max=5000000 m=0.000001 b=0 units="mA" format=7.6 help="Uego Pump Current X Axis" xcount=17 xindexvar="Pump Current 1"
//ASAM mode=writecurve name="Uego Pump Current Table" type=uint16 offset=112 min=0 max=5 m=0.001 b=0 units="dl" format=4.3 help="Uego Pump Current Curve" xcount=17 xindexvar="Pump Current 1"
//ASAM mode=writevalue name="Uego1 Sensor Trim" type=uint8 offset=146 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Uego1 Sensor Trim"
//ASAM mode=writevalue name="Uego2 Sensor Trim" type=uint8 offset=147 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Uego2 Sensor Trim"
//ASAM mode=writevalue name="Uego1 Stoich Cal" type=uint32 offset=148 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="Uego1 Stoich Cal"
//ASAM mode=writeaxis_pts name="VEx Table_XAXIS" type=uint32 offset=211 min=0 max=300000 m=0.001 b=0 units="kPa" format=7.6 help="VEx X Axis" xcount=17 xindexvar="MAP kPa"
//ASAM mode=writecurve name="VEx Table" type=uint16 offset=279 min=0 max=3000 m=0.001 b=0 units="dl" format=4.3 help="VEx Curve" xcount=17 xindexvar="MAP kPa"
//ASAM mode=writeaxis_pts name="Inj Response Table_XAXIS" type=uint32 offset=313 min=0 max=20000 m=0.001 b=0 units="V" format=4.3 help="Inj Response X Axis" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writecurve name="Inj Response Table" type=uint16 offset=381 min=0 max=3000 m=0.001 b=0 units="ms" format=4.3 help="Inj Response Curve" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writeaxis_pts name="Coolant Sensor Table_XAXIS" type=uint32 offset=415 min=0 max=5 m=1 b=0 units="V" format=7.0 help="Coolant Sensor X Axis" xcount=17 xindexvar="CTS Sensor Voltage"
//ASAM mode=writecurve name="Coolant Sensor Table" type=sint32 offset=483 min=-40 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="Coolant Sensor Curve" xcount=17 xindexvar="CTS Sensor Voltage"
//ASAM mode=writeaxis_pts name="CTS Enrichment Table_XAXIS" type=sint32 offset=551 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Enrichment X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Enrichment Table" type=sint32 offset=619 min=-100 max=200 m=0.001 b=0 units="%" format=4.3 help="CTS Enrichment Curve" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writeaxis_pts name="VEy Table_XAXIS" type=uint32 offset=687 min=0 max=300000 m=0.001 b=0 units="kPa" format=7.6 help="VEy X Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="VEy Table" type=uint16 offset=755 min=0 max=3000 m=0.001 b=0 units="dl" format=4.3 help="VEx Curve" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Timingx Table_XAXIS" type=uint32 offset=789 min=0 max=300000 m=0.001 b=0 units="kPa" format=7.6 help="Timingx X Axis" xcount=17 xindexvar="MAP kPa"
//ASAM mode=writecurve name="Timingx Table" type=uint16 offset=857 min=0 max=50 m=0.1 b=0 units="degrees" format=4.3 help="Timingx Curve" xcount=17 xindexvar="MAP kPa"
//ASAM mode=writeaxis_pts name="Timingy Table_XAXIS" type=uint32 offset=891 min=0 max=300000 m=0.001 b=0 units="kPa" format=7.6 help="Timingy X Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="Timingy Table" type=uint16 offset=959 min=0 max=50 m=0.1 b=0 units="degress" format=4.3 help="Timingx Curve" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writevalue name="CAL CRC HB" type=uint8 offset=789 min=0 max=255 m=1 b=0 units="counts" format=3.0 help="CAL CRC HB"
//ASAM mode=writevalue name="CAL CRC LB" type=uint8 offset=790 min=0 max=255 m=1 b=0 units="counts" format=3.0 help="CAL CRC LB"
/* 	NOTE MUST ALWAYS INCLUDE AND ASAM TAG FOR FIRST AND LAST VAR IN CAL STRUCT */

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void USERCAL_vStart(puint32 const pu32Arg);/*CR1_1*/
void USERCAL_vRun(puint32 const pu32Arg);/*CR1_1*/
void USERCAL_vTerminate(puint32 const pu32Arg);/*CR1_1*/


#endif // USERCAL_H

