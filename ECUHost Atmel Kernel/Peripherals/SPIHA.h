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
#ifndef SPIHA_H
#define SPIHA_H

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
#include "spi.h"
#include "SRLTFR.h"
#include "sys.h"
#include "types.h"

#define SPIHA_nBitCount 8
#define SPIHA_nDeviceID 0
#define SPIHA_xSPIBaudRateDivider(b) spi_calc_baudrate_div(b, SYS_FREQ_BUS)

#ifdef BUILD_MK60
#define SPIHA_nReg8SetSPI0 																																												\
{																																																								\
	{ (volatile uint8*)(I2C0_BASE + offsetof(I2C_Type, C1)), (uint8)I2C_C1_SPIEN_MASK, REGSET_enOverwrite }, 	\
	{ NULL, 0, REGSET_enOverwrite }																																								\
};
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define SPIHA_nReg8SetSPI0           \
{                                    \
	{ NULL, 0, REGSET_enOverwrite }  \
};
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define SPIHA_nReg8SetSPI1                                                                                      \
{                                                                                                               \
	{ (volatile uint8*)(I2C1_BASE + offsetof(I2C_Type, C1)), (uint8)I2C_C1_SPIEN_MASK, REGSET_enOverwrite }, 	\
	{ NULL, 0, REGSET_enOverwrite }                                                                             \
};
#endif  //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define SPIHA_nReg8SetSPI1            \
{                                     \
	{ NULL, 0, REGSET_enOverwrite }   \
};
#endif  //BUILD_SAM3X8E

#define SPIHA_nDivisorMap   \
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

#define SPI_xRequestPortClock(x)	\
/* turn on  clock */                \
SIM_vSetReg32(SIM_SCGC4, x);        \
SPI_u32PortClockRequested |= x	

#define SPI_xCalcDivisor                                                                \
for (u32DivMapIDX = 0;                                                                  \
			 u32DivMapIDX < (sizeof(SPI_rastDivisorMap) / sizeof(SPI_tstDivisorMap));	\
			 u32DivMapIDX++)                                                            \
{                                                                                       \
	u32Div = u32DivMapIDX;                                                              \
	if (SPI_rastDivisorMap[u32DivMapIDX].u32SCLDivider > u32DivCalc) break;             \
}	

typedef struct
{
	uint32 u32SCLDivider;
	uint32 u32SDAHold;
} SPIHA_tstDivisorMap;

#ifdef BUILD_MK60
typedef SPI_Type tstSPIModule;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
typedef Spi tstSPIModule;
#endif //BUILD_SAM3X8E


void SPIHA_vStart(puint32 const);
void SPIHA_vRun(puint32 const);
void SPIHA_vTerminate(puint32 const);
uint32 SPIHA_u32InitBus(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
void SPIHA_vInitTransfer(IOAPI_tstTransferCB*);
void SPIHA_vInterrupt(IOAPI_tenEHIOResource);

#endif //SPIHA_H

