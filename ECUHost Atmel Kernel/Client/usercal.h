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
	{9300u}, 								\
	{9300u}, 								\
	{2u}, 							    	\
	{-1850000u,					    		\
  -1465000u,					    		\
	-1080000u,						    	\
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
	1470u,									\
	{2u},											\
	{3310u,									\
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
	3490u},									\
	2600u,									\
	10u,									\
	1u,										\
	1u,										\
	400u,									\
	2500u,	 								\
	{500u,	 								\
	750u,	 								\
	1000u,	 								\
	1250u,	 								\
	1500u,	 								\
	1750u,	 								\
	2000u,	 								\
	2250u,	 								\
	2500u,	 								\
	2750u,	 								\
	3000u,	 								\
	3250u,	 								\
	3500u,	 								\
	3750u,	 								\
	4000u,	 								\
	4250u,	 								\
	4500u},  								\
    {0x2A4C20,								\
    0x35D53F,								\
    0x4722F0,								\
    0x63F9C0,								\
    0x82BCC0,								\
    0xAA2698,								\
    0xD84B18,								\
    0x10D2A40,								\
    0x1544D30,								\
    0x1A50D0F,								\
    0x2062488,								\
    0x2797FC8,								\
    0x308BBC0,								\
    0x3AD1DC8,								\
    0x4679BF8,								\
    0x53C0EB0,								\
    0x6437460},								\
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
	{1400u,									\
    1300u,										\
	1200u,										\
	1100u,										\
	1000u,										\
	900u,										\
	800u,										\
	700u,										\
	600u,										\
	500u,										\
	480u,										\
	460u,										\
	440u,										\
	430u,										\
	420u,										\
	410u,										\
	400u},									\
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
	{2500u,									\
    2200u,									\
	1800u,									\
	1600u,									\
	1500u,									\
	1400u,									\
	1300u,									\
	1200u,									\
	1150u,									\
	1100u,									\
	1050u,									\
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
	    5500u,									\
	    6000u,									\
	    6500u,									\
	    7000u,									\
	    7500u,									\
    8000u},									\
    {0u,\
	    5625u,\
	    11250u,\
	    16875u,\
	    22500u,\
	    28125u,\
	    33750u,\
	    39375u,\
	    45000u,\
	    50625u,\
	    56250u,\
	    61875u,\
	    67500u,\
	    73125u,\
	    78750u,\
	    84375u,\
    90000u},					\
	{{1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1400},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1190,1190,1190},\
	{1550,1470,1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190},\
	{1550,1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190},\
	{1470,1470,1470,1470,1470,1470,1470,1470,1400,1330,1260,1190,1190,1190,1190,1190,1190}},\
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
		5500u,									\
		6000u,									\
		6500u,									\
		7000u,									\
		7500u,									\
	8000u},									\
    {0u,\
	5625u,\
	11250u,\
	16875u,\
	22500u,\
	28125u,\
	33750u,\
	39375u,\
	45000u,\
	50625u,\
	56250u,\
	61875u,\
	67500u,\
	73125u,\
	78750u,\
	84375u,\
    90000u},					\
	{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},\
	{120,120,120, 80, 80, 80, 80, 80, 80, 80, 80, 70, 70, 70, 70, 70, 70},\
	{120,120,120, 90, 90, 90, 90, 90, 90, 90, 90, 80, 80, 80, 80, 80, 80},\
	{ 50, 80,120,120,120,120,120,110,110,110,110,110,110,110,110,110,110},\
	{100,170,170,140,130,130,130,130,130,130,130,130,130,120,120,120,120},\
	{140,200,220,150,130,130,130,130,130,130,130,130,130,140,140,140,140},\
	{140,200,220,150,130,130,130,130,130,130,130,130,130,140,140,140,140},\
	{140,200,220,150,130,130,130,130,130,130,130,130,130,140,140,140,140},\
	{140,200,220,150,130,130,130,130,130,130,130,130,130,140,140,140,140},\
	{140,200,200,150,130,130,130,130,130,130,130,130,130,140,140,140,140},\
	{180,190,220,240,240,240,250,245,241,230,220,210,200,190,180,170,160},\
	{130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130},\
	{140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140},\
	{150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150},\
	{160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160},\
	{130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130},\
	{90,90,90,90,90,90,90,90,90,90,90,80,80,80,80,80,80}},\
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
	    5500u,									\
	    6000u,									\
	    6500u,									\
	    7000u,									\
	    7500u,									\
    8000u},									\
    {0u,\
	    5625u,\
	    11250u,\
	    16875u,\
	    22500u,\
	    28125u,\
	    33750u,\
	    39375u,\
	    45000u,\
	    50625u,\
	    56250u,\
	    61875u,\
	    67500u,\
	    73125u,\
	    78750u,\
	    84375u,\
    90000u},					\
	{{500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500},       \
	{510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510,510},       \
	{520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520,520},       \
	{530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530,530},       \
	{540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540,540},       \
	{550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550,550},       \
	{560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560,560},       \
	{570,570,570,570,570,570,570,570,570,570,570,570,570,570,570,570,570},       \
	{580,580,580,580,580,580,580,580,580,580,580,580,580,580,580,580,580},       \
	{590,590,590,590,590,590,590,590,590,590,590,590,590,590,590,590,590},       \
	{600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600},       \
	{600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600},       \
	{600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600},       \
	{600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600},       \
	{600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600},       \
	{600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600},       \
	{600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600}},       \
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
	{1500u,									\
    1450u,									\
	1400u,									\
	1350u,									\
	1300u,									\
	1250u,									\
	1200u,									\
	1150u,									\
	1100u,									\
	1050u,									\
	1030u,									\
	1010u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u},									\
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
	{2000u,									\
    1800u,									\
	1600u,									\
	1400u,									\
	1150u,									\
	1130u,									\
	1110u,									\
	1090u,									\
	1070u,									\
	1050u,									\
	1030u,									\
	1030u,									\
	1030u,									\
	1030u,									\
	1030u,									\
	1030u,									\
	1030u},									\
	{150u,									\
    160u,									\
	170u,									\
	180u,									\
	190u,									\
	200u,									\
	210u,									\
	220u,									\
	230u,									\
	240u,									\
	250u,									\
	260u,									\
	270u,									\
	280u,									\
	290u,									\
	300u,									\
	400u},								    \
	{5000u,									\
    5000u,									\
	6000u,									\
	6500u,									\
	7000u,									\
	7500u,									\
	8000u,									\
	8500u,									\
	9000u,									\
	9500u,									\
	10000u,									\
	10000u,									\
	10000u,									\
	10000u,									\
	10000u,									\
	10000u,									\
	10000u},									\
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
		4000u,									\
		3000u,									\
		3000u,									\
		3000u,									\
		3000u,									\
		3000u,									\
		3000u,									\
		3000u,									\
		3000u,									\
		3000u,									\
	3000u},									\
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
	{1000u,									\
    1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
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
	{1000u,									\
    1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
	1000u,									\
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
	100u,									\
	200u,									\
	300u,									\
	400u,									\
	500u,									\
	600u,									\
	700u,									\
	800u,									\
	900u,									\
	1000u,								\
	1100u,								\
	1200u,								\
	1300u,								\
	1400u,								\
	1500u,								\
	1600u},\
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
	{31u,									\
    25u,									\
	22u,									\
	18u,									\
	15u,									\
	14u,									\
	13u,									\
	12u,									\
	11u,									\
	10u,									\
	10u,									\
	10u,									\
	10u,									\
	10u,									\
	10u,									\
	10u,									\
	10u},									\
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
	{31u,									\
    25u,									\
	22u,									\
	18u,									\
	15u,									\
	14u,									\
	13u,									\
	12u,									\
	11u,									\
	10u,									\
	10u,									\
	10u,									\
	10u,									\
	10u,									\
	10u,									\
	10u,									\
	10u},									\
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
	{1500u,									\
    1250u,									\
	1220u,									\
	1180u,									\
	1150u,									\
	1140u,									\
	1130u,									\
	1120u,									\
	1110u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u,									\
	1100u},									\
	{5,											\
	9,											\
	10,											\
	6},											\
	{6,											\
	10,											\
	9,											\
	5},											\
	24,\
	{0x0,\
	0x444,\
	0x888,\
	0xCCC,\
	0x1999,\
	0x1DDD,\
	0x2222,\
	0x2666,\
	0x2AAA,\
	0x2EEE,\
	0x3333,\
	0x3777,\
	0x3BBB,\
	0x4000,\
	0x4444,\
	0x4888,\
	0x4CCC,\
	0x5111,\
	0x5555,\
	0x5999,\
	0x5DDD,\
	0x6222,\
	0x6666,\
	0x6AAA,\
	0x6EEE,\
	0x7333,\
	0x7777,\
	0x7BBB,\
	0x8000,\
	0x8444,\
	0x8888,\
	0x8CCC,\
	0x9111,\
	0x9555,\
	0x9999,\
	0x9DDD,\
	0xA222,\
	0xA666,\
	0xAAAA,\
	0xAEEE,\
	0xB333,\
	0xB777,\
	0xBBBB,\
	0xC000,\
	0xC444,\
	0xC888,\
	0xCCCC,\
	0xD111,\
	0xD555,\
	0xD999,\
	0xDDDD,\
	0xE222,\
	0xE666,\
	0xEAAA,\
	0xEEEE,\
	0xF333,\
	0xF777,\
	0xFBBB,\
    0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0x0000,\
	0}, \
	{0x0,\
		0x444,\
		0x888,\
		0xCCC,\
		0x1111,\
		0x1555,\
		0x1999,\
		0x1DDD,\
		0x2AAA,\
		0x2EEE,\
		0x3333,\
		0x3777,\
		0x3BBB,\
		0x4000,\
		0x4444,\
		0x4888,\
		0x4CCC,\
		0x5111,\
		0x5555,\
		0x5999,\
		0x5DDD,\
		0x6222,\
		0x6666,\
		0x6AAA,\
		0x6EEE,\
		0x7333,\
		0x7777,\
		0x7BBB,\
		0x8000,\
		0x8444,\
		0x8888,\
		0x8CCC,\
		0x9111,\
		0x9555,\
		0x9999,\
		0x9DDD,\
		0xA222,\
		0xA666,\
		0xAAAA,\
		0xAEEE},\	
	{0x0, 0x4000, 0x8000, 0xc000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},\
	0,\
	0,\
	0,\
	0,\
	0,\
	12,											\
	2000,										\
	4, \
	2530,  \
{0,\
500,\
1000,\
1500,\
2000,\
2500,\
3000,\
3500,\
4000,\
4500,\
5000},\
	{0x0,\
	0xB32,\
	0x1664,\
	0x2196,\
	0x2CC8,\
	0x37FB,\
	0x432D,\
	0x5991,\
	0x865A,\
	0xDFEC,\
	0x15F90},\
{{0x16E360,	0x19F0A0,	0x19F0A0,	0x19F0A0,	0x19F0A0,	0x19F0A0,	0x19F0A0,	0x19F0A0,	0x19F0A0,	0x19F0A0,	0x19F0A0},\
{0x2191C0,	0x2932E0,	0x3567E0,	0x4C4B40,	0x5B8D80,	0x6ACFC0,	0x6ACFC0,	0x6ACFC0,	0x6ACFC0,	0x6ACFC0,	0x6ACFC0},\
{0x2191C0,	0x200B20,	0x3037C0,	0x3B8260,	0x6E2B20,	0xA1BE40,	0xAC6CA0,	0xD29240,	0xE4E1C0,	0xE4E1C0,	0xE4E1C0},\
{0x200B20,	0x231860,	0x2EB120,	0x4853A0,	0x755710,	0x989680,	0xDD40A0,	0x116C730,	0x15752A0,	0x15EF3C0,	0x15EF3C0},\
{0x1E8480,	0x231860,	0x367950,	0x49B330,	0x753000,	0xA037A0,	0xCAF120,	0x13376F0,	0x19BFCC0,	0x1C9C380,	0x1D905C0},\
{0x1E8480,	0x231860,	0x325AA0,	0x4C4B40,	0x7A1200,	0x890620,	0xCAF120,	0x1658370,	0x1F0AC6F,	0x2255100,	0x2625A00},\
{0x1E8480,	0x231860,	0x325AA0,	0x4C4B40,	0x7A1200,	0xBEBC20,	0x10B0760,	0x158B230,	0x1F78A40,	0x29020C0,	0x2FAF080},\
{0x493E0,	0x231860,	0x325AA0,	0x4C4B40,	0x7A1200,	0xBEBC20,	0x112A880,	0x15EF3C0,	0x1E84800,	0x2BDE780,	0x38444C0},\
{0x493E0,	0x231860,	0x325AA0,	0x4C4B40,	0x7A1200,	0xBEBC20,	0x11A49A0,	0x16E3600,	0x1F78A40,	0x2FAF080,	0x3FE56C0},\
{0x493E0,	0x231860,	0x325AA0,	0x4C4B40,	0x7A1200,	0xBEBC20,	0x11A49A0,	0x16E3600,	0x1F78A40,	0x3473BC0,	0x47868C0},\
{0x493E0,	0x231860,	0x325AA0,	0x4C4B40,	0x7A1200,	0xBEBC20,	0x11A49A0,	0x16E3600,	0x206CC80,	0x3938700,	0x4F27AC0}},\
0xff,\
0x01,\
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
	 /* CAVEAT!!! Make first element uint32 for %4 alignment!!! */
{
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
	uint16 aUserMAPTAFR[17][17];
	uint32 aUserTimingxSpread[17];	
	GPM6_ttMTheta aUserTimingySpread[17];	
	uint16 aUserTimingMap[17][17];	
	uint32 aUserVExSpread[17];	
	GPM6_ttMTheta aUserVEySpread[17];	
	uint16 aUserMAPVE[17][17];
	GPM6_ttTempC aUserCoolantStartEnrichmentSpread[17];	
	sint32 aUserCoolantStartEnrichmentTable[17];		
	GPM6_ttTempC aUserCoolantPostStartEnrichmentSpread[17];	
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
	uint16 aUserPrimaryTriggerTable[80];
	uint16 aUserSecondaryTriggerTable[40];
	uint16 aUserSyncPointsTable[36];
	uint8 u8UserPrimaryEdgeSetup;
	uint8 u8UserSecondaryEdgeSetup;
	uint8 u8UserPrimaryVREnable;
	uint8 u8UserSecondaryVREnable;
	uint8 u8UserFirstEdgeRisingPrimary;
	uint8 u8UserFirstEdgeRisingSecondary;
	uint16 u16ManifoldVolumeCC;	
	uint8 u8CylCount;
	uint16 u16InjFlowRate;
	uint32 aUserBackupAirflowxSpread[11];	
	uint32 aUserBackupAirflowySpread[11];	
	uint32 aUserBackupAirflowMap[11][11];
	uint8 u8ManifoldTimeConstantTrim;
	uint8 u8EnableBackupAirflowTransients;
	uint16 u16CRC16;
} USERCAL_tstCalibration;


/* GLOBAL VARIABLE DECLARATIONS ***********************************************/
/* 	NOTE MUST ALWAYS INCLUDE AND ASAM TAG FOR FIRST AND LAST VAR IN CAL STRUCT */
EXTERN USERCAL_tstCalibration USERCAL_stRAMCAL;
//ASAM mode=writevalue name="AD RefH Voltage" type=uint32 offset=0 min=0 max=5 m=0.001 b=0 units="V" format=4.3 help="AD Converter Reference High Voltage"
//ASAM mode=writevalue name="Uego Heater Rth" type=uint32 offset=16 min=0 max=10 m=0.001 b=0 units="Ohms" format=4.3 help="Uego Heater Resistance Threshold"
//ASAM mode=writevalue name="Uego1 Current P Cal" type=uint32 offset=20 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Current P Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego1 Current M Cal" type=uint32 offset=24 min=0 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Current M Calibration (dimensionless)"
//ASAM mode=writevalue name="Uego1 Offset Cal" type=sint32 offset=28 min=-5000000 max=5000000 m=1 b=0 units="dl" format=6.0 help="Uego1 Offset Calibration (dimensionless)"
//ASAM mode=writeaxis_pts name="Uego Pump Current Table_XAXIS" type=sint32 offset=32 min=-5000000 max=5000000 m=0.000001 b=0 units="mA" format=7.6 help="Uego Pump Current X Axis" xcount=17 xindexvar="Pump Current 1"
//ASAM mode=writecurve name="Uego Pump Current Table" type=uint16 offset=100 min=0 max=5 m=0.001 b=0 units="dl" format=4.3 help="Uego Pump Current Curve" xcount=17 xindexvar="Pump Current 1"
//ASAM mode=writevalue name="Uego1 Sensor Trim" type=uint8 offset=134 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Uego1 Sensor Trim"
//ASAM mode=writevalue name="Uego1 Stoich Cal" type=uint32 offset=136 min=0 max=2000 m=1 b=0 units="dl" format=3.0 help="Uego1 Stoich Cal"

//ASAM mode=writeaxis_pts name="AfmTF Table_XAXIS" type=uint32 offset=196 min=0 max=300000 m=0.001 b=0 units="V" format=7.6 help="AfmTF X Axis" xcount=17 xindexvar="AFM Sensor Voltage"
//ASAM mode=writecurve name="AfmTF Table" type=uint32 offset=264 min=0 max=150 m=0.000001 b=0 units="g/s" format=4.3 help="AfmTF Curve" xcount=17 xindexvar="AFM Sensor Voltage"

//ASAM mode=writeaxis_pts name="Inj Response Table_XAXIS" type=uint32 offset=332 min=0 max=20000 m=0.001 b=0 units="V" format=4.3 help="Inj Response X Axis" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writecurve name="Inj Response Table" type=uint16 offset=400 min=0 max=3 m=0.001 b=0 units="ms" format=4.3 help="Inj Response Curve" xcount=17 xindexvar="Battery Voltage"

//ASAM mode=writeaxis_pts name="Coolant Sensor Table_XAXIS" type=uint32 offset=436 min=0 max=5 m=1 b=0 units="V" format=7.0 help="Coolant Sensor X Axis" xcount=17 xindexvar="CTS Sensor Voltage"
//ASAM mode=writecurve name="Coolant Sensor Table" type=sint32 offset=504 min=-40 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="Coolant Sensor Curve" xcount=17 xindexvar="CTS Sensor Voltage"

//ASAM mode=writeaxis_pts name="CTS Enrichment Table_XAXIS" type=sint32 offset=572 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Enrichment X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Enrichment Table" type=sint32 offset=640 min=-1 max=3 m=0.001 b=0 units="%" format=4.3 help="CTS Enrichment Curve" xcount=17 xindexvar="Coolant Temperature"

//ASAM mode=writeaxis_pts name="TAFR Map_XAXIS" type=uint32 offset=708 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="TAFR Map_YAXIS" type=uint32 offset=776 min=0 max=90000 m=0.001 b=0 units="Degrees" format=3.1 help="VE MAP X Axis" xcount=17 xindexvar="Throttle Angle"
//ASAM mode=writemap name="TAFR Map" type=uint16 offset=844 min=0 max=20 m=0.01 b=0 units="dl" format=4.3 help="TAFR Map" xcount=17 xindexvar="Throttle Angle" ycount=17 yindexvar="Engine Speed Raw"

//ASAM mode=writeaxis_pts name="Timing Map_XAXIS" type=uint32 offset=1424 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Timing Map_YAXIS" type=uint32 offset=1492 min=0 max=90000 m=0.001 b=0 units="Degrees" format=3.1 help="VE MAP X Axis" xcount=17 xindexvar="Throttle Angle"
//ASAM mode=writemap name="Timing Map" type=uint16 offset=1560 min=0 max=50 m=0.1 b=0 units="Degrees" format=3.1 help="Timing Map" xcount=17 xindexvar="Throttle Angle" ycount=17 yindexvar="Engine Speed Raw"

//ASAM mode=writeaxis_pts name="VE Map_XAXIS" type=uint32 offset=2140 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="VE MAP Y Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="VE Map_YAXIS" type=uint32 offset=2208 min=0 max=90000 m=1 b=0 units="Degrees" format=3.1 help="VE MAP X Axis" xcount=17 xindexvar="Throttle Angle"
//ASAM mode=writemap name="VE Map" type=uint16 offset=2276 min=0 max=1.5 m=0.001 b=0 units="dl" format=4.3 help="VE Map" xcount=17 xindexvar="Throttle Angle" ycount=17 yindexvar="Engine Speed Raw"

//ASAM mode=writeaxis_pts name="CTS Start Enrichment Table_XAXIS" type=sint32 offset=2856 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Start Enrichment X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Start Enrichment Table" type=sint32 offset=2924 min=0 max=4 m=0.001 b=0 units="%" format=4.3 help="CTS Start Enrichment Curve" xcount=17 xindexvar="Coolant Temperature"

//ASAM mode=writeaxis_pts name="CTS Post Start Enrichment Table_XAXIS" type=sint32 offset=2992 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Post Start Enrichment X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Post Start Enrichment Table" type=sint32 offset=3060 min=0 max=4 m=0.001 b=0 units="%" format=4.3 help="CTS Post Start Enrichment Curve" xcount=17 xindexvar="Coolant Temperature"

//ASAM mode=writeaxis_pts name="Cranking Airflow Table_XAXIS" type=uint32 offset=3128 min=0 max=8000 m=1 b=0 units="RPM" format=4.3 help="Cranking Airflow X Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="Cranking Airflow Table" type=uint16 offset=3196 min=0 max=65.535 m=0.001 b=0 units="g/s" format=4.3 help="Cranking Airflow Curve" xcount=17 xindexvar="Engine Speed Raw"

//ASAM mode=writeaxis_pts name="Dwell Table_XAXIS" type=uint32 offset=3232 min=0 max=20000 m=0.001 b=0 units="V" format=4.3 help="Dwell X Axis" xcount=17 xindexvar="Battery Voltage"
//ASAM mode=writecurve name="Dwell Table" type=uint16 offset=3300 min=0 max=10 m=0.001 b=0 units="ms" format=4.3 help="Dwell Curve" xcount=17 xindexvar="Battery Voltage"

//ASAM mode=writeaxis_pts name="ATS Fuel Correction Table_XAXIS" type=sint32 offset=3336 min=0 max=20 m=0.001 b=0 units="V" format=4.3 help="ATS Fuel Correction X Axis" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writecurve name="ATS Fuel Correction Table" type=uint16 offset=3404 min=0 max=2 m=0.001 b=0 units="%" format=4.3 help="ATS Fuel Correction Curve" xcount=17 xindexvar="Air Temperature"

//ASAM mode=writeaxis_pts name="TPS Movement Correction Table_XAXIS" type=sint32 offset=3440 min=-450 max=450 m=0.001 b=0 units="Degrees/s" format=4.3 help="TPS Movement Correction X Axis" xcount=17 xindexvar="TPS Movement Rate"
//ASAM mode=writecurve name="TPS Movement Correction Table" type=uint16 offset=3508 min=0 max=2 m=0.001 b=0 units="%" format=6.3 help="TPS Movement Correction Curve" xcount=17 xindexvar="TPS Movement Rate"

//ASAM mode=writeaxis_pts name="Injection Phasing Table_XAXIS" type=uint32 offset=3544 min=0 max=20000 m=0.001 b=0 units="RPM" format=4.3 help="Injection Phasing X Axis" xcount=17 xindexvar="Engine Speed Raw"
//ASAM mode=writecurve name="Dwell Table" type=uint16 offset=3612 min=0 max=20 m=0.001 b=0 units="ms" format=4.3 help="Injection Phasing Curve" xcount=17 xindexvar="Engine Speed Raw"

//ASAM mode=writeaxis_pts name="CTS Timing Correction Table_XAXIS" type=sint32 offset=3648 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="CTS Timing Correction X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="CTS Timing Correction Table" type=sint16 offset=3716 min=-10 max=10 m=0.1 b=0 units="Degrees" format=4.3 help="CTS Timing Correction Curve" xcount=17 xindexvar="Coolant Temperature"

//ASAM mode=writeaxis_pts name="ATS Timing Correction Table_XAXIS" type=sint32 offset=3752 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="ATS Timing Correction X Axis" xcount=17 xindexvar="Air Temperature"
//ASAM mode=writecurve name="ATS Timing Correction Table" type=sint16 offset=3820 min=-10 max=10 m=0.1 b=0 units="Degrees" format=4.3 help="ATS Timing Correction Curve" xcount=17 xindexvar="Air Temperature"

//ASAM mode=writeaxis_pts name="ISC Target Table_XAXIS" type=sint32 offset=3856 min=0 max=200 m=0.001 b=0 units="degrees C" format=4.3 help="ISC Target X Axis" xcount=17 xindexvar="Coolant Temperature"
//ASAM mode=writecurve name="ISC Target Table" type=uint16 offset=3924 min=0 max=2000 m=1 b=0 units="RPM" format=4.3 help="ISC Target Curve" xcount=17 xindexvar="Coolant Temperature"

//ASAM mode=writeblob name="Primary Trigger Table" type=uint16 offset=3974 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=80 help="Primary Trigger Tooth Setup"
//ASAM mode=writeblob name="Secondary Trigger Table" type=uint16 offset=4134 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=40 help="Secondary Trigger Tooth Setup"
//ASAM mode=writeblob name="Sync Points Table" type=uint16 offset=4214 min=1 max=65535 m=1 b=0 units="dl" format=5.0 pointcount=36 help="Syncronisation Points Setup"

//ASAM mode=writevalue name="Primary Edge Setup" type=uint8 offset=4286 min=0 max=255 m=1 b=0 units="dl" format=5.0 help="Primary Edge Setup"
//ASAM mode=writevalue name="Secondary Edge Setup" type=uint8 offset=4287 min=0 max=255 m=1 b=0 units="dl" format=5.0 help="Secondary Edge Setup"

//ASAM mode=writevalue name="Primary VR Enable" type=uint8 offset=4288 min=0 max=255 m=1 b=0 units="bool" format=5.0 help="Primary VR Circuit Enable"
//ASAM mode=writevalue name="Secondary VR Enable" type=uint8 offset=4289 min=0 max=255 m=1 b=0 units="bool" format=5.0 help="Secondary VR Circuit Enable"

//ASAM mode=writevalue name="First Edge Rising Primary" type=uint8 offset=4290 min=0 max=255 m=1 b=0 units="bool" format=3.0 help="First Edge Rising Primary"
//ASAM mode=writevalue name="First Edge Rising Secondary" type=uint8 offset=4291 min=0 max=255 m=1 b=0 units="bool" format=3.0 help="First Edge Rising Secondary"
//ASAM mode=writevalue name="Manifold Volume CC" type=uint16 offset=4292 min=0 max=65535 m=1 b=0 units="cc" format=5.0 help="Manifold Volume CC"

//ASAM mode=writevalue name="First Edge Rising Primary Enable" type=uint8 offset=4290 min=0 max=255 m=1 b=0 units="dl" format=5.0 help="First Edge Rising Primary Enable"
//ASAM mode=writevalue name="First Edge Rising Secondary Enable" type=uint8 offset=4291 min=0 max=255 m=1 b=0 units="dl" format=5.0 help="First Edge Rising Secondary Enable Enable"

//ASAM mode=writeaxis_pts name="Airflow Map_XAXIS" type=uint32 offset=4296 min=0 max=8000 m=1 b=0 units="RPM" format=4.0 help="Airflow Map Y Axis" xcount=11 xindexvar="Engine Speed Raw"
//ASAM mode=writeaxis_pts name="Airflow Map_YAXIS" type=uint32 offset=4340 min=0 max=90000 m=1 b=0 units="Degrees" format=3.1 help="Airflow Map X Axis" xcount=11 xindexvar="Throttle Angle"
//ASAM mode=writemap name="Airflow Map" type=uint32 offset=4384 min=0 max=150 m=0.000001 b=0 units="g/s" format=9.6 help="Airflow Map" xcount=11 xindexvar="Throttle Angle" ycount=11 yindexvar="Engine Speed Raw"

//ASAM mode=writevalue name="Manifold Time Constant Cal" type=uint8 offset=4868 min=0 max=255 m=1 b=0 units="dl" format=3.0 help="Manifold Time Constant Cal"
//ASAM mode=writevalue name="Airflow Transient Enable" type=uint8 offset=4869 min=0 max=1 m=1 b=0 units="bool" format=1.0 help="Airflow Transients Enable"

//ASAM mode=writevalue name="CAL CRC HB" type=uint8 offset=4868 min=0 max=255 m=1 b=0 units="counts" format=3.0 help="CAL CRC HB"
//ASAM mode=writevalue name="CAL CRC LB" type=uint8 offset=4869 min=0 max=255 m=1 b=0 units="counts" format=3.0 help="CAL CRC LB"
/* 	NOTE MUST ALWAYS INCLUDE AND ASAM TAG FOR FIRST AND LAST VAR IN CAL STRUCT */

/* GLOBAL FUNCTION DECLARATIONS ***********************************************/
void USERCAL_vStart(puint32 const pu32Arg);/*CR1_1*/
void USERCAL_vRun(puint32 const pu32Arg);/*CR1_1*/
void USERCAL_vTerminate(puint32 const pu32Arg);/*CR1_1*/


#endif // USERCAL_H

