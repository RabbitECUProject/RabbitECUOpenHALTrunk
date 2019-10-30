/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      ADCHA Header File                                      */
/* DESCRIPTION:        ADC Hardware Adaption                                  */
/* FILE NAME:          ADC.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef ADCHA_H
#define ADCHA_H

#include "adc.h"
#include "CPUAbstract.h"
#include "declarations.h"
#include "IOAPI.h"
#include "IRQ.h"
#include "pmc.h"
#include "SIM.h"

#ifdef BUILD_MK60
typedef ADC_Type tstADCModule;
#endif
#ifdef BUILD_SAM3X8E
typedef Adc tstADCModule;
#endif

#ifdef BUILD_SAM3X8E
#define ADCHA_nADCCLKFREQ 5000000u
#define ADCHA_nADCTRSTTIME 8u
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define ADCHA_nReg32Set              \
{  								     \
	{(volatile uint32*)(ADC0_BASE + offsetof(ADC_Type, CFG1)), (uint32)(ADC_CFG1_MODE(1) + ADC_CFG1_ADICLK(1)+ ADC_CFG1_ADIV(1) + ADC_CFG1_ADLSMP_MASK), REGSET_enOverwrite},		\
	{(volatile uint32*)(ADC1_BASE + offsetof(ADC_Type, CFG1)), (uint32)(ADC_CFG1_MODE(1) + ADC_CFG1_ADICLK(1)+ ADC_CFG1_ADIV(1) + ADC_CFG1_ADLSMP_MASK), REGSET_enOverwrite},		\
	{(volatile uint32*)(ADC2_BASE + offsetof(ADC_Type, CFG1)), (uint32)(ADC_CFG1_MODE(1) + ADC_CFG1_ADICLK(1)+ ADC_CFG1_ADIV(1) + ADC_CFG1_ADLSMP_MASK), REGSET_enOverwrite},		\
	{(volatile uint32*)(ADC3_BASE + offsetof(ADC_Type, CFG1)), (uint32)(ADC_CFG1_MODE(1) + ADC_CFG1_ADICLK(1)+ ADC_CFG1_ADIV(1) + ADC_CFG1_ADLSMP_MASK), REGSET_enOverwrite},		\
	{ NULL, 0, REGSET_enOverwrite }  \
}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define ADCHA_nReg32Set              \
{  								     \
	NULL, 0, REGSET_enOverwrite  \
}
#endif //BUILD_MK60

#ifdef BUILD_MK60
#define ADCHA_nChannelInfo                                              \
{                                                                       \
	{EH_IO_ADSE1, 	ADC_enADC1, 13, 	0, ADC_enChannelA, false},      \
	{EH_IO_ADSE2, 	ADC_enADC1, 12, 	0, ADC_enChannelA, false},      \
	{EH_IO_ADSE3, 	ADC_enADC1, 11, 	0, ADC_enChannelA, false},		\
	{EH_IO_ADSE4, 	ADC_enADC1, 10, 	0, ADC_enChannelA, false},		\
	{EH_IO_ADSE5, 	ADC_enADC1, 15, 	0, ADC_enChannelA, false},		\
	{EH_IO_ADSE6, 	ADC_enADC1, 14, 	0, ADC_enChannelA, false},		\
	{EH_IO_ADSE7, 	ADC_enADC1, 4, 		0, ADC_enChannelB, false},		\
	{EH_IO_ADSE8, 	ADC_enADC1, 5, 		0, ADC_enChannelB, false},		\
	{EH_IO_ADSE9, 	ADC_enADC1, 6, 		0, ADC_enChannelB, false},		\
	{EH_IO_ADSE10, 	ADC_enADC1, 7, 		0, ADC_enChannelB, false},		\
	{EH_IO_ADSE11, 	ADC_enADC2, 14, 	0, ADC_enChannelA, false},		\
	{EH_IO_ADSE12, 	ADC_enADC2, 15, 	0, ADC_enChannelA, false},		\
	{EH_IO_GPSE1, 	ADC_enADC0, 13, 	0, ADC_enChannelA, false},		\
	{EH_IO_GPSE2, 	ADC_enADC0, 12, 	0, ADC_enChannelA, false},		\
	{EH_IO_GPSE5, 	ADC_enADC3, 4, 		0, ADC_enChannelB, false},		\
	{EH_IO_GPSE6, 	ADC_enADC3, 7, 		0, ADC_enChannelB, false},		\
	{EH_IO_GPSE7, 	ADC_enADC3, 4, 		0, ADC_enChannelA, false},		\
	{EH_IO_GPSE8, 	ADC_enADC3, 5, 		0, ADC_enChannelA, false},		\
	{EH_IO_GPSE9, 	ADC_enADC0, 11, 	0, ADC_enChannelA, false},		\
	{EH_IO_GPSE10, 	ADC_enADC0, 10, 	0, ADC_enChannelA, false},		\
	{EH_I_ADD1, 		ADC_enADC2, 2, 		0, ADC_enChannelA, true},			\
	{EH_I_ADD2, 		ADC_enADC2, 2, 		0, ADC_enChannelA, true},			\
	{EH_I_ADD3, 		ADC_enADC3, 2, 		0, ADC_enChannelA, true},			\
	{EH_I_ADD4, 		ADC_enADC3, 2, 		0, ADC_enChannelA, true},			\
	{EH_I_ADD5, 		ADC_enADC0, 2, 		0, ADC_enChannelA, true},			\
	{EH_I_ADD6, 		ADC_enADC0, 2, 		0, ADC_enChannelA, true},			\
	{EH_I_ADD7, 		ADC_enADC1, 2, 		0, ADC_enChannelA, true},			\
	{EH_I_ADD8, 		ADC_enADC1, 2, 		0, ADC_enChannelA, true},			\
	{EH_IO_CAN2T,		ADC_enADC0, 17,  	0, ADC_enChannelA, false},		\
	{EH_IO_CAN2R,		ADC_enADC0, 18,		0, ADC_enChannelA, false},		\
	{EH_I_CMP1,			ADC_enADC1, 16, 	0, ADC_enChannelA, false},		\
	{EH_I_CMP2,			ADC_enADC0, 16,		0, ADC_enChannelA, false},		\
	{EH_I_CMP3,			ADC_enADC1, 18,		0, ADC_enChannelA, false}			\
}
#endif //BUILD_MK60

#if defined(BUILD_SAM3X8E) && defined(BUILD_ARDUINO_DUE)
#define ADCHA_nChannelInfo                                                  \
{                                                                           \
    {EH_IO_ADD1, ADCHA_enADC0, 7,  true},                   \
    {EH_IO_ADD2, ADCHA_enADC0, 6,  true},                   \
    {EH_IO_ADD3, ADCHA_enADC0, 5,  true},                   \
    {EH_IO_ADD4, ADCHA_enADC0, 4,  true},                   \
    {EH_IO_ADD5, ADCHA_enADC0, 3,  true},                   \
    {EH_IO_ADD6, ADCHA_enADC0, 2,  true},                   \
    {EH_IO_ADD7, ADCHA_enADC0, 1,  true},                   \
    {EH_IO_ADD8, ADCHA_enADC0, 0,  true},                   \
    {EH_IO_ADD9, ADCHA_enADC0, 10,  true},                  \
    {EH_IO_ADD10, ADCHA_enADC0, 11,  true},                 \
    {EH_IO_ADD11, ADCHA_enADC0, 12,  true},                 \
    {EH_IO_ADD12, ADCHA_enADC0, 13,  true},                 \
    {EH_IO_DAC1, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_DAC2, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_CAN1R, ADCHA_enADCModuleInvalid, 0,  false},    \
    {EH_IO_CAN1T, ADCHA_enADCModuleInvalid, 0,  false},    \
    {EH_IO_IO22, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO23, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO24, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO25, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO26, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO27, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO28, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO29, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO30, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO31, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO32, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO33, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO34, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO35, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO36, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO37, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO38, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO39, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO40, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO41, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO42, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO43, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO44, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO45, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO46, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO47, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO48, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO49, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO50, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO51, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IO52, ADCHA_enADC0, 14,  true},                 \
    {EH_IO_IO53, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_IIC1_SCL, ADCHA_enADC0, 9,  true},              \
    {EH_IO_IIC1_SDA, ADCHA_enADC0, 8,  true},              \
    {EH_IO_UART1_RX, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_UART1_TX, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_UART2_RX, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_UART2_TX, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_UART3_RX, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_UART3_TX, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_UART5_RX, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_UART5_TX, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_TMR1, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR2, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR3, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR4, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR5, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR6, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR7, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR8, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR9, ADCHA_enADCModuleInvalid, 0,  false},     \
    {EH_IO_TMR10, ADCHA_enADCModuleInvalid, 0,  false},    \
    {EH_IO_TMR11, ADCHA_enADCModuleInvalid, 0,  false},    \
    {EH_IO_TMR12, ADCHA_enADCModuleInvalid, 0,  false},    \
    {EH_IO_IIC2_SDA, ADCHA_enADCModuleInvalid, 0,  false}, \
    {EH_IO_IIC2_SCL, ADCHA_enADCModuleInvalid, 0,  false}  \
}
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define ADCHA_nIOResourceMap	\
{														\
	EH_I_ADD5,			\
	IO_Total_Count,	\
	EH_I_ADD5,			\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	EH_IO_GPSE10,		\
	EH_IO_GPSE9,		\
	EH_IO_GPSE2,		\
	EH_IO_GPSE1,		\
	IO_Total_Count,	\
	IO_Total_Count,	\
	EH_I_CMP2,			\
	EH_IO_CAN2T,		\
	EH_IO_CAN2R,		\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	EH_I_ADD7,			\
	IO_Total_Count,	\
	EH_I_ADD7,			\
	IO_Total_Count,	\
	EH_IO_ADSE7,		\
	EH_IO_ADSE8,		\
	EH_IO_ADSE9,		\
	EH_IO_ADSE10,		\
	IO_Total_Count,	\
	IO_Total_Count,	\
	EH_IO_ADSE4,		\
	EH_IO_ADSE3,		\
	EH_IO_ADSE2,		\
	EH_IO_ADSE1,		\
	EH_IO_ADSE6,		\
	EH_IO_ADSE5,		\
	EH_I_CMP1,			\
	IO_Total_Count,	\
	EH_I_CMP3,			\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	EH_I_ADD1,			\
	IO_Total_Count,	\
	EH_I_ADD1,			\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	EH_IO_ADSE11,	\
	EH_IO_ADSE12,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	IO_Total_Count,	\
	EH_I_ADD3,	\
	IO_Total_Count,	\
	EH_I_ADD3,	\
	EH_I_ADD2,	\
	EH_IO_GPSE7,	\
	EH_IO_GPSE8,	\
	IO_Total_Count,	\
	EH_IO_GPSE6	\
}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define ADCHA_nIOResourceMap   \
{                              \
	EH_IO_ADD8,                \
	EH_IO_ADD7,                \
	EH_IO_ADD6,                \
	EH_IO_ADD5,                \
	EH_IO_ADD4,                \
	EH_IO_ADD3,                \
	EH_IO_ADD2,                \
	EH_IO_ADD1,                \
	EH_IO_IIC1_SDA,	           \
	EH_IO_IIC1_SCL,	           \
	EH_IO_ADD9,                \
	EH_IO_ADD10,               \
	EH_IO_ADD11,               \
	EH_IO_ADD12,               \
	EH_IO_IO52,                \
	IO_Total_Count             \
}
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define ADCHA_xInitialise(x, y)    \
ADCHA_xRequestPortClock(x);        \
ADHAC_xCalibrate(x);               \
ADCHA_xInitInterrupts(x, y);
#define ADC_xRequestPortClock(x)                                                                   \
if (ADC0 == x){SIM_vSetReg32(SIM_SCGC6, SIM_SCGC6_ADC0_MASK);ADCHA_u32PortClockRequested |= 1;}    \
if (ADC1 == x){SIM_vSetReg32(SIM_SCGC3, SIM_SCGC3_ADC1_MASK);ADCHA_u32PortClockRequested |= 2;}    \
if (ADC2 == x){SIM_vSetReg32(SIM_SCGC6, SIM_SCGC6_ADC2_MASK);ADCHA_u32PortClockRequested |= 4;}    \
if (ADC3 == x){SIM_vSetReg32(SIM_SCGC3, SIM_SCGC3_ADC3_MASK);ADCHA_u32PortClockRequested |= 8;}    \
#define ADC_xCalibrate(x)                      \
if (ADC0 == x){ADC_vCalibrate(ADC0, 0, 1);}    \
if (ADC1 == x){ADC_vCalibrate(ADC1, 1, 2);}    \
if (ADC2 == x){ADC_vCalibrate(ADC2, 2, 4);}    \
if (ADC3 == x){ADC_vCalibrate(ADC3, 3, 8);}    \
#define ADC_xInitInterrupts(x, y)          \
if (ADC0 == x){ADC_vInitInterrupts(y);}    \
if (ADC1 == x){ADC_vInitInterrupts(y);}    \
if (ADC2 == x){ADC_vInitInterrupts(y);}    \
if (ADC3 == x){ADC_vInitInterrupts(y);}    
#endif

#ifdef BUILD_SAM3X8E
#define ADCHA_xInitialise(x, y)  
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
typedef enum
{
	ADCHA_enADC0,
	ADCHA_enADC1,
	ADCHA_enADC2,
	ADCHA_enADC3,
	ADCHA_enADCModuleCount
} ADCHA_tenADCModule;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
typedef enum
{
	ADCHA_enADC0,
	ADCHA_enADCModuleCount,
	ADCHA_enADCModuleInvalid
} ADCHA_tenADCModule;
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
typedef enum
{
	ADCHA_enChannelA = 0,
	ADCHA_enChannelB = 1,
	ADCHA_enChannelCount
} ADCHA_tenChannelAB;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
typedef enum
{
	ADCHA_enChannelA = 0,
	ADCHA_enChannelCount
} ADCHA_tenChannelAB;
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	ADCHA_tenADCModule enADCModule;
	uint32 u32ADChannel;
	uint32 u32Samples;
	ADCHA_tenChannelAB enChannelAB;
	Bool boIsDiff;
} ADCHA_tstADCChannel;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	ADCHA_tenADCModule enADCModule;
	uint32 u32ADChannel;
	Bool boIsDiff;
} ADCHA_tstADCChannel;
#endif//BUILD_SAM3X8E

#ifdef BUILD_MK60
typedef struct
{
	uint32 u32OFS;
	uint32 u32PG;
	uint32 u32MG;
	uint32 u32CRC;
} ADCHA_tstADCCalStruct;
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
typedef struct
{
	uint32 u32Dummy;
} ADCHA_tstADCCalStruct;
#endif //BUILD_SAM3X8E

typedef struct
{
	uint32 u32ResetCount;
	uint32 u32ControlCount;
	ADCHA_tstADCChannel stADCChannel;
	ADCAPI_tpfResultCB pfResultCB;
} ADCHA_tstADCConversion;

typedef enum
{
	ADCHA_enQueueCyclic,
	ADCHA_enQueueTriggered1,
	ADCHA_enQueueTriggered2,
	ADCHA_enQueueTriggered3,
	ADCHA_enQueueTriggered4,
	ADCHA_enQueueCount
} ADCHA_tenQueue;

void ADCHA_vStart(uint32* const);
Bool ADCHA_boBackupCalibrations(void);
void ADCHA_vInitADCResourcePGA(ADCHA_tstADCConversion*);
void ADCHA_vTerminate(uint32* const);
Bool ADCHA_boInitiateConversion(ADCHA_tstADCConversion*, ADCHA_tenQueue, uint32, Bool);
IOAPI_tenEHIOResource ADCHA_enGetResourceAndResult(ADCHA_tenADCModule, tstADCModule*, uint32, puint32);
void ADCHA_vCalibrate(tstADCModule*, uint32, uint32);
void ADCHA_vInitConversion(IOAPI_tenEHIOResource, ADCHA_tstADCConversion*, ADCAPI_tstADCCB*, uint32);
Bool ADCHA_boGetModuleBusy(ADCHA_tenADCModule);
void ADCHA_vInitChannel(IOAPI_tenEHIOResource);
tstADCModule* ADCHA_pstGetADCModule(ADCHA_tenADCModule);
uint32 ADCHA_u32GetAvailableResultCount(tstADCModule*);
void ADCHA_vClearModuleBusy(ADCHA_tenADCModule);
void ADCHA_vReset(ADCHA_tenADCModule enADCModule);

#endif //ADCHA_H

