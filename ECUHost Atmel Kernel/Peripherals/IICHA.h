/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef IICHA_H
#define IICHA_H

#include <string.h>
#include "CPUAbstract.h"
#include "declarations.h"
#include "DLL.h"
#include "IOAPI.h"
#include "MATH.h"
#include "OS.h"
#include "PIM.h"
#include "PIMAPI.h"
#include "regset.h"
#include "RESM.h"
#include "SIM.h"
#include "SRLTFR.h"
#include "sys.h"
#include "types.h"

#ifdef BUILD_MK60
#define IICHA_nReg8SetIIC0 																																												\
{																																																								\
	{ (volatile uint8*)(I2C0_BASE + offsetof(I2C_Type, C1)), (uint8)I2C_C1_IICEN_MASK, REGSET_enOverwrite }, 	\
	{ NULL, 0, REGSET_enOverwrite }																																								\
};
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define IICHA_nReg8SetIIC0           \
{                                    \
	{ NULL, 0, REGSET_enOverwrite }  \
};
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define IICHA_nReg8SetIIC1                                                                                      \
{                                                                                                               \
	{ (volatile uint8*)(I2C1_BASE + offsetof(I2C_Type, C1)), (uint8)I2C_C1_IICEN_MASK, REGSET_enOverwrite }, 	\
	{ NULL, 0, REGSET_enOverwrite }                                                                             \
};
#endif  //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define IICHA_nReg8SetIIC1            \
{                                     \
	{ NULL, 0, REGSET_enOverwrite }   \
};
#endif  //BUILD_SAM3X8E

#define IICHA_nDivisorMap   \
{                           \
	{20, 7},                \
	{22, 7},                \
	{24, 8},                \
	{26, 8},                \
	{28, 9},                \
	{30, 9},                \
	{34, 10},               \
	{40, 10},               \
	{28, 7},                \
	{32, 7},                \
	{36, 9},                \
	{40, 9},							\
	{44, 11},							\
	{48, 11},							\
	{56, 13},							\
	{68, 13},							\
	{48, 9},							\
	{56, 9},							\
	{64, 13},							\
	{72, 13},							\
	{80, 17},							\
	{88, 17},							\
	{104, 21},						\
	{128, 21},						\
	{80, 9},							\
	{96, 9},							\
	{112, 17},						\
	{128, 17},						\
	{144, 25},						\
	{160, 25},						\
	{192, 33},						\
	{240, 33},						\
	{160, 17},						\
	{192, 17},						\
	{224, 33},						\
	{256, 33},						\
	{288, 49},						\
	{320, 49},						\
	{384, 65},						\
	{480, 65},						\
	{320, 33},						\
	{384, 33},						\
	{448, 65},						\
	{512, 65},						\
	{576, 97},						\
	{640, 97},						\
	{768, 129},						\
	{960, 129},						\
	{640, 65},						\
	{768, 65},						\
	{896, 129},						\
	{1024, 129},					\
	{1152, 193},					\
	{1280, 193},					\
	{1536, 257},					\
	{1920, 257},					\
	{1280, 129},					\
	{1536, 129},					\
	{1792, 257},					\
	{2048, 257},					\
	{2304, 385},					\
	{2560, 385},					\
	{3072, 513},					\
	{3840, 513}						\
}

#define IIC_xRequestPortClock(x)	\
/* turn on  clock */                \
SIM_vSetReg32(SIM_SCGC4, x);        \
IIC_u32PortClockRequested |= x	

#define IIC_xCalcDivisor                                                                \
for (u32DivMapIDX = 0;                                                                  \
			 u32DivMapIDX < (sizeof(IIC_rastDivisorMap) / sizeof(IIC_tstDivisorMap));	\
			 u32DivMapIDX++)                                                            \
{                                                                                       \
	u32Div = u32DivMapIDX;                                                              \
	if (IIC_rastDivisorMap[u32DivMapIDX].u32SCLDivider > u32DivCalc) break;             \
}	

typedef struct
{
	uint32 u32SCLDivider;
	uint32 u32SDAHold;
} IICHA_tstDivisorMap;

#ifdef BUILD_MK60
typedef I2C_Type tstI2CModule;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
typedef Twi tstI2CModule;
#endif //BUILD_SAM3X8E


void IICHA_vStart(puint32 const);
void IICHA_vRun(puint32 const);
void IICHA_vTerminate(puint32 const);
SYSAPI_tenSVCResult IICHA_enInitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
void IICHA_vInitTransfer(IOAPI_tstTransferCB*);
void IICHA_vInterrupt(IOAPI_tenEHIOResource);

#endif //IICHA_H

