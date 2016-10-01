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
#ifndef IO_H
#define IO_H

#include "CPUAbstract.h"
#include "ADC.h"
#include "ADCAPI.h"
#include "CAN.h"
#include "DAC.h"
#include "DACAPI.h"
#include "ENE.h"
#include "DECLARATIONS.h"
#include "IIC.h"
#include "IICAPI.h"
#include "IOAPI.h"
#include "OS.h"
#include "PIM.h"
#include "PIMAPI.h"
#include "UART.h"
#include "RESM.h"
#include "sys.h"	
#include "SYSAPI.h"
#include "TEPM.h"

#define KERNEL_RXD1							(IOAPI_tenEHIOResource)71
#define KERNEL_RXD0 						(IOAPI_tenEHIOResource)72
#define KERNEL_RXDV 						(IOAPI_tenEHIOResource)73
#define KERNEL_TXEN 						(IOAPI_tenEHIOResource)74
#define KERNEL_TXD0 						(IOAPI_tenEHIOResource)75
#define KERNEL_TXD1 						(IOAPI_tenEHIOResource)76
#define KERNEL_MDIO 						(IOAPI_tenEHIOResource)77
#define KERNEL_MDC 							(IOAPI_tenEHIOResource)78
#define KERNEL_CPURX 						(IOAPI_tenEHIOResource)79
#define KERNEL_CPUTX 						(IOAPI_tenEHIOResource)80
#define KERNEL_DISCRETE_HIDDEN 	(IOAPI_tenEHIOResource)11
	
typedef struct
{
	IOAPI_tenEHIOResource enEHIO;
	PIMAPI_tenPort enPort;
	uint32 u32PortBit;
	bool boIsADSE;	
	bool boIsADD;
	bool boIsDAC;
} IO_tstEHPadResource;

#if defined(BUILD_MK60) && defined(BUILD_ECUHOST_1_4)
#define IO_nPadResourceMap	{                                                           \
	{EH_IO_ADSE1,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_7, TRUE, FALSE, FALSE},       \
	{EH_IO_ADSE2,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_6, TRUE, FALSE, FALSE},       \
	{EH_IO_ADSE3,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_5, TRUE, FALSE, FALSE},       \
	{EH_IO_ADSE4,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_4, TRUE, FALSE, FALSE},       \
	{EH_IO_ADSE5,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_11, TRUE, FALSE, FALSE},      \
	{EH_IO_ADSE6,	PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_10, TRUE, FALSE, FALSE},      \
	{EH_IO_ADSE7,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE8,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE9,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE10,PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE11,PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_ADSE12,PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_GPSE1, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_3, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE2, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_2, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE5,	PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_27, TRUE, FALSE, FALSE},      \
	{EH_IO_GPSE6,	PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_28, TRUE, FALSE, FALSE},      \
	{EH_IO_GPSE7,	PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_10, TRUE, FALSE, FALSE},      \
	{EH_IO_GPSE8, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_9, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE9, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_8, TRUE, FALSE, FALSE},			\
	{EH_IO_GPSE10, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_7, TRUE, FALSE, FALSE},        \
	{EH_I_ADD1,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                           \
	{EH_I_ADD2,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                           \
	{EH_I_ADD3,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                           \
	{EH_I_ADD4,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                           \
	{EH_I_ADD5,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                           \
	{EH_I_ADD6,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                           \
	{EH_I_ADD7,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                           \
	{EH_I_ADD8,	PIMAPI_enPortInvalid, 0u, TRUE, TRUE, FALSE},                           \
	{EH_IO_CAN2T,	PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_24, TRUE, FALSE, FALSE},      \
	{EH_IO_CAN2R,	PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_25, TRUE, FALSE, FALSE},      \
	{EH_I_CMP1,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_I_CMP2,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_I_CMP3,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{IO_AD_Count,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_UART1_TX, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_19, TRUE, FALSE, FALSE},     \
	{EH_IO_UART1_RX, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_18, TRUE, FALSE, FALSE},     \
	{EH_IO_UART1_CTS, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_17, TRUE, FALSE, FALSE},	\
	{EH_IO_UART1_RTS, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_16, TRUE, FALSE, FALSE},	\
	{EH_IO_UART2_TX, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_15, TRUE, FALSE, FALSE},     \
	{EH_IO_UART2_RX, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_14, TRUE, FALSE, FALSE},     \
	{EH_IO_UART2_CTS, PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_13, TRUE, FALSE, FALSE},	\
	{EH_IO_UART2_RTS,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_12, TRUE, FALSE, FALSE},	\
	{EH_IO_CAN1T,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_CAN1R,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{EH_IO_TMR1, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_7, TRUE, FALSE, FALSE},          \
	{EH_IO_TMR2, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_6, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR3, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_5, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR4, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_4, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR5, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_3, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR6, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_2, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR7, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_1, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR8, PIMAPI_enPHYS_PORT_D, PIMAPI_PORT_BIT_0, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR9, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_9, TRUE, FALSE, FALSE},				\
	{EH_IO_TMR10, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_10, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR11, PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_11, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR12,	PIMAPI_enPHYS_PORT_E, PIMAPI_PORT_BIT_12, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR13,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_4, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR14,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_3, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR15,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_2, TRUE, FALSE, FALSE},			\
	{EH_IO_TMR16,	PIMAPI_enPHYS_PORT_C, PIMAPI_PORT_BIT_1, TRUE, FALSE, FALSE},			\
	{EH_IO_IIC1_SCL, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},					\
	{EH_IO_IIC1_SDA, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},					\
	{EH_I_CMP4,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_I_CMP5,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_O_DAC1,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, TRUE},								\
	{EH_O_DAC2,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, TRUE},								\
	{EH_IO_GP3, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_8, TRUE, FALSE, FALSE},				\
	{EH_IO_GP4, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_9, TRUE, FALSE, FALSE},				\
	{EH_IO_K33A, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},							\
	{EH_IO_USB5V,	PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},						\
	{IO_Total_Discrete_Count, PIMAPI_enPortInvalid, 0u, TRUE, FALSE, FALSE},	\
	{KERNEL_RXD1, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_12, FALSE, FALSE, FALSE},		\
	{KERNEL_RXD0, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_13, FALSE, FALSE, FALSE},		\
	{KERNEL_RXDV, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_14, FALSE, FALSE, FALSE},		\
	{KERNEL_TXEN, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_15, FALSE, FALSE, FALSE},		\
	{KERNEL_TXD0, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_16, FALSE, FALSE, FALSE},		\
	{KERNEL_TXD1, PIMAPI_enPHYS_PORT_A, PIMAPI_PORT_BIT_17, FALSE, FALSE, FALSE},		\
	{KERNEL_MDIO, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_0, FALSE, FALSE, FALSE},			\
	{KERNEL_MDC, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_1, FALSE, FALSE, FALSE},			\
	{KERNEL_CPURX, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_16, FALSE, FALSE, FALSE},		\
	{KERNEL_CPUTX, PIMAPI_enPHYS_PORT_B, PIMAPI_PORT_BIT_17, FALSE, FALSE, FALSE}		\
}	
#endif //BUILD_MK60 && BUILD_ECUHOST_1_4

#if defined(BUILD_SAM3X8E) && defined(BUILD_ARDUINO_DUE)
#define IO_nPadResourceMap	{                                                           \
	{(IOAPI_tenEHIOResource)NULL, (PIMAPI_tenPort)NULL, (uint32)NULL, TRUE, FALSE, FALSE} \
}
#endif //BUILD_SAM3X8E && BUILD ARDUINO_DUE
	
	
	



void IO_vInitDIOResource(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, IOAPI_tenDriveStrength);
void IO_vAssertDIOResource(IOAPI_tenEHIOResource, IOAPI_tenTriState);
void IO_vInitADCResource(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, ADCAPI_tstADCCB*);
void IO_vInitDACResource(IOAPI_tenEHIOResource, IOAPI_tenEHIOType, DACAPI_tstDACCB*);
IOAPI_tenTriState IO_stGetDIOResourceState(IOAPI_tenEHIOResource);
SYSAPI_tenSVCResult IO_enInitCommsResource(IOAPI_tenEHIOResource, IOAPI_tstPortConfigCB*);
SYSAPI_tenSVCResult IO_enInitTEPMChannel(IOAPI_tenEHIOResource, TEPMAPI_tstTEPMChannelCB*);
IOAPI_tenTriState IO_enGetDIOResourceState(IOAPI_tenEHIOResource);

#endif //IO_H

