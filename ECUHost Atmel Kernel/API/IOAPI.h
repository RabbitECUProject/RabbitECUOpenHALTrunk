/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* API TITLE:          IOAPI                                                  */
/* DESCRIPTION:        This API provides type definitions for objects and     */
/*                     functions required by Kernel managed I/O               */
/*                     initialisation, functions and events                   */
/*                                                                            */
/* FILE NAME:          IOAPI.h                                                */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef IOAPI_H
#define IOAPI_H

#include "PROTAPI.h"
#include "types.h"


/* GLOBAL TYPE DEFINITIONS ****************************************************/
typedef enum
{
	IOAPI_IO_RX = 0,
	IOAPI_IO_TX = 1,
	IOAPI_IO_MEM_READ = 2,
	IOAPI_IO_MEM_WRITE = 3
} IOAPI_enTransferType;
/*CR1_103*/

typedef enum
{
	IOAPI_enPortComms,
	IOAPI_enPortSerialTransfer,
	IOAPI_enPortNone
} IOAPI_tenPortMode;

#if defined(BUILD_MK60) && defined(BUILD_ECUHOST_1_4)
typedef enum
{
	EH_IO_ADSE1 = 0,
	EH_IO_ADSE2 = 1,
	EH_IO_ADSE3 = 2,
	EH_IO_ADSE4 = 3,
	EH_IO_ADSE5 = 4,
	EH_IO_ADSE6 = 5,
	EH_IO_ADSE7 = 6,
	EH_IO_ADSE8 = 7,
	EH_IO_ADSE9 = 8,
	EH_IO_ADSE10 = 9,
	EH_IO_ADSE11 = 10,
	EH_IO_ADSE12 = 11,	
	EH_IO_GPSE1 = 12,
	EH_IO_GPSE2 = 13,
	EH_IO_GPSE5 = 14,
	EH_IO_GPSE6 = 15,
	EH_IO_GPSE7 = 16,
	EH_IO_GPSE8 = 17,
	EH_IO_GPSE9 = 18,
	EH_IO_GPSE10 = 19,
	EH_I_ADD1 = 20,
	EH_I_ADD2 = 21,
	EH_I_ADD3 = 22,
	EH_I_ADD4 = 23,
	EH_I_ADD5 = 24,
	EH_I_ADD6 = 25,
	EH_I_ADD7 = 26,
	EH_I_ADD8 = 27,	
	EH_IO_CAN2T = 28,
	EH_IO_CAN2R = 29,		
	EH_I_CMP1 = 30,
	EH_I_CMP2 = 31,
	EH_I_CMP3 = 32,	
	IO_AD_Count = 33,
	EH_IO_UART1_TX = 34,
	EH_IO_UART1_RX = 35,
	EH_IO_UART1_CTS = 36,
	EH_IO_UART1_RTS = 37,
	EH_IO_UART2_TX = 38,
	EH_IO_UART2_RX = 39,
	EH_IO_UART2_CTS = 40,
	EH_IO_UART2_RTS = 41,	
	EH_IO_CAN1T = 42,
	EH_IO_CAN1R = 43,
	EH_IO_TMR1 = 44,
	EH_IO_TMR2 = 45,
	EH_IO_TMR3 = 46,
	EH_IO_TMR4 = 47,
	EH_IO_TMR5 = 48,
	EH_IO_TMR6 = 49,
	EH_IO_TMR7 = 50,
	EH_IO_TMR8 = 51,
	EH_IO_TMR9 = 52,
	EH_IO_TMR10 = 53,
	EH_IO_TMR11 = 54,
	EH_IO_TMR12 = 55,
	EH_IO_TMR13 = 56,
	EH_IO_TMR14 = 57,
	EH_IO_TMR15 = 58,
	EH_IO_TMR16 = 59,	
	EH_IO_IIC1_SCL = 60,
	EH_IO_IIC1_SDA = 61,	
	EH_I_CMP4 = 62,
	EH_I_CMP5 = 63,	
	EH_O_DAC1 = 64,
	EH_O_DAC2 = 65,
	EH_IO_GP3 = 66,
	EH_IO_GP4 = 67,	
	EH_IO_K33A = 68,
	EH_IO_USB5V = 69,
	IO_Total_Discrete_Count = 70,
	EH_VIO_IIC1 = 100,
	EH_VIO_IIC2 = 101,
	EH_VIO_SPI1 = 102,
	EH_VIO_SPI2 = 103,		
	EH_VIO_UART1 = 104,
	EH_VIO_UART2 = 105,
	EH_VIO_UART3 = 106,
	EH_VIO_UART4 = 107,
	EH_VIO_UART5 = 108,
	EH_VIO_UART6 = 109,
	EH_VIO_CAN1 = 110,
	EH_VIO_CAN2 = 111,
	EH_VIO_ENET_CH1 = 112,
	EH_VIO_ENET_CH2 = 113,
	EH_VIO_ENET_CH3 = 114,
	EH_VIO_ENET_CH4 = 115,
	EH_VIO_FTM0 = 116,
	EH_VIO_FTM1 = 117,
	EH_VIO_FTM2 = 118,
	EH_VIO_FTM3 = 119, 
	IO_Total_Count,
	EH_IO_Invalid
} IOAPI_tenEHIOResource;	

#define EH_FIRST_TMR      EH_IO_TMR1
#define EH_LAST_TMR	      EH_IO_TMR16
#endif //BUILD_MK60

#if defined(BUILD_SAM3X8E) && defined(BUILD_ARDUINO_DUE)
typedef enum
{
	EH_IO_ADD1 = 0,
	EH_IO_ADD2 = 1,
	EH_IO_ADD3 = 2,
	EH_IO_ADD4 = 3,
	EH_IO_ADD5 = 4,
	EH_IO_ADD6 = 5,
	EH_IO_ADD7 = 6,
	EH_IO_ADD8 = 7,
	EH_IO_ADD9 = 8,
	EH_IO_ADD10 = 9,
	EH_IO_ADD11 = 10,
	EH_IO_ADD12 = 11,
    EH_IO_DAC1 = 12,
	EH_IO_DAC2 = 13,
	EH_IO_CAN1R = 14,
	EH_IO_CAN1T = 15,
	EH_IO_IO22 = 16,
	EH_IO_IO23 = 17,
	EH_IO_IO24 = 18,
	EH_IO_IO25 = 19,
	EH_IO_IO26 = 20,
	EH_IO_IO27 = 21,
	EH_IO_IO28 = 22,
	EH_IO_IO29 = 23,
	EH_IO_IO30 = 24,
	EH_IO_IO31 = 25,
	EH_IO_IO32 = 26,
	EH_IO_IO33 = 27,
	EH_IO_IO34 = 28,
	EH_IO_IO35 = 29,
	EH_IO_IO36 = 30,
	EH_IO_IO37 = 31,
	EH_IO_IO38 = 32,
	EH_IO_IO39 = 33,
	EH_IO_IO40 = 34,
	EH_IO_IO41 = 35,
	EH_IO_IO42 = 36,
	EH_IO_IO43 = 37,
	EH_IO_IO44 = 38,
	EH_IO_IO45 = 39,
	EH_IO_IO46 = 40,
	EH_IO_IO47 = 41,
	EH_IO_IO48 = 42,
	EH_IO_IO49 = 43,
	EH_IO_IO50 = 44,
	EH_IO_IO51 = 45,
	EH_IO_IO52 = 46,
	EH_IO_IO53 = 47,
	EH_IO_IIC1_SCL = 48,
	EH_IO_IIC1_SDA = 49,
	EH_IO_UART1_RX = 50,
	EH_IO_UART1_TX = 51,
	EH_IO_UART2_RX = 52,
	EH_IO_UART2_TX = 53,
	EH_IO_UART3_RX = 54,
	EH_IO_UART3_TX = 55,
	EH_IO_UART5_RX = 56,
	EH_IO_UART5_TX = 57,
	EH_IO_TMR1 = 58,
	EH_IO_TMR2 = 59,
	EH_IO_TMR3 = 60,
	EH_IO_TMR4 = 61,
	EH_IO_TMR5 = 62,
	EH_IO_TMR6 = 63,
	EH_IO_TMR7 = 64,
	EH_IO_TMR8 = 65,
	EH_IO_TMR9 = 66,
	EH_IO_TMR10 = 67,
	EH_IO_TMR11 = 68,
	EH_IO_TMR12 = 69,
	EH_IO_IIC2_SDA = 70,
	EH_IO_IIC2_SCL = 71,
	EH_IO_SPI1_MOSI = 72,
	EH_IO_SPI1_MISO = 73,
	EH_IO_SPI1_SCK = 74,
	IO_Total_Discrete_Count,
	EH_VIO_TC0,
	EH_VIO_TC1,
	EH_VIO_TC2,
	EH_VIO_TCC0,
	EH_VIO_TCC1,
	EH_VIO_TCC2,
	EH_VIO_TCC3,
	EH_VIO_TCC4,
	EH_VIO_TCC5,
	EH_VIO_TCC6,
	EH_VIO_TCC7,
	EH_VIO_TCC8,
	EH_VIO_IIC1,	/* Recommended to have contiguous virtual comms devices/channels here */
	EH_VIO_IIC2,    /* Don't change the IIC, SPI, UART, CAN, ENET order */
	EH_VIO_SPI1,
	EH_VIO_SPI2,
	EH_VIO_UART1,
	EH_VIO_UART2,
	EH_VIO_CAN1,
	EH_VIO_CAN2,
    EH_VIO_ENET_CH1,
    EH_VIO_ENET_CH2,
	EH_VIO_ADC0,
	EH_VIO_PWM,
	IO_Total_Count,
	EH_IO_Invalid
} IOAPI_tenEHIOResource;

#define EH_FIRST_TMR        EH_IO_TMR1
#define EH_LAST_TMR	        EH_IO_TMR12
#define EH_ENET_FIRST_CH    EH_VIO_ENET_CH1
#define EH_ENET_LAST_CH	    EH_VIO_ENET_CH2
#define EH_FIRST_UART       EH_VIO_UART1
#define EH_LAST_UART        EH_VIO_UART2
#define EH_FIRST_IIC        EH_VIO_IIC1
#define EH_LAST_IIC         EH_VIO_IIC2
#define EH_FIRST_SPI        EH_VIO_SPI1
#define EH_LAST_SPI         EH_VIO_SPI2
#define EH_FIRST_CAN        EH_VIO_CAN1
#define EH_LAST_CAN         EH_VIO_CAN2
#define EH_FIRST_VIO_COMMS  EH_VIO_IIC1
#define EH_LAST_VIO_COMMS   EH_VIO_ENET_CH2

#endif //BUILD_SAM3X8E

typedef enum
{
	IOAPI_enDIOOutput,
	IOAPI_enDIOInput,
	IOAPI_enADSE,
	IOAPI_enADD,
	IOAPI_enDAC,
	IOAPI_enCaptureCompare,
	IOAPI_enPWM,
	IOAPI_enIICBus,
	IOAPI_enIISBus,
	IOAPI_enSPIBus,
	IOAPI_enUARTBus,
	IOAPI_enCANBus,
	IOAPI_enENETChannel,
	IOAPI_enTEPM
} IOAPI_tenEHIOType;

typedef enum
{
    IOAPI_enEdgeRising,
	IOAPI_enEdgeFalling,
	IOAPI_enEdgeBoth
} IOAPI_tenEdgePolarity;

typedef enum
{
	IOAPI_enLow,
	IOAPI_enHigh,
	IOAPI_enToggle,
	IOAPI_enHiZ,
	IOAPI_enError
} IOAPI_tenTriState;

typedef enum
{
	IOAPI_enWeak,
	IOAPI_enStrong
} IOAPI_tenDriveStrength;

typedef void (*IOAPI_tpfTransferCB)(IOAPI_tenEHIOResource, puint8, uint32, IOAPI_enTransferType);
	
typedef struct
{
	uint32 u32MasterAddress;
	uint32 u32SlaveAddress;	
} tstIICNetInfo;
		
typedef struct
{
	uint32 u32GlobalCANDiagAddress;
	uint32 u32CANDiagAddress;
	uint32 u32CANPriorityAddress[4];
	uint32 u32CANPriorityMask[4];
} tstCANNetInfo;	

typedef struct
{
	uint16 u16RPCREQSourcePort;
	uint16 u16RPCREQDestPort;
} tstLANNetInfo;

typedef struct
{
	union
	{
		tstIICNetInfo stIICNetInfo;
		tstCANNetInfo stCANNetInfo;
		tstLANNetInfo stLANNetInfo;
	} uNetInfo;
} IOAPI_tstNetConfig;

typedef struct
{
	IOAPI_tenEHIOResource enRXPin;
	IOAPI_tenEHIOResource enTXPin;
	IOAPI_tenEHIOResource enRTSPin;
	IOAPI_tenEHIOResource enCTSPin;
} IOAPI_tstUARTPinInfo;

typedef struct
{
	IOAPI_tenEHIOResource enRXPin;
	IOAPI_tenEHIOResource enTXPin;
} IOAPI_tstCANPinInfo;
/*CR1_104*/

typedef struct
{
	IOAPI_tenEHIOResource enSDAPin;
	IOAPI_tenEHIOResource enSCLPin;
} IOAPI_tstIICPinInfo;

typedef struct
{
	IOAPI_tenEHIOResource enMOSIPin;
	IOAPI_tenEHIOResource enMISOPin;
	IOAPI_tenEHIOResource enSCKPin;
	Bool boCaptureRising;
	Bool boShiftRising;
	Bool boIdleHigh;
} IOAPI_tstSPIPinInfo;

typedef struct
{ 
	union
	{
		IOAPI_tstUARTPinInfo stUARTPinInfo;
		IOAPI_tstIICPinInfo stIICPinInfo;
		IOAPI_tstSPIPinInfo stSPIPinInfo;
		IOAPI_tstCANPinInfo stCANPinInfo;
	} uPinInfo;
} IOAPI_tstPinConfig;

typedef struct
{
	IOAPI_tstNetConfig stNetConfig;
	IOAPI_tstPinConfig stPinConfig;
	IOAPI_tenPortMode enPortMode;
	uint32 u32BaudRateHz;
	uint32 u32TXPacketRateMs;
	IOAPI_tenEHIOResource enVIOResource;
	PROTAPI_tenLLProtocol enLLProtocol;
	PROTAPI_tenNLProtocol enNLProtocol;	
	PROTAPI_tenTLProtocol enTLProtocol;
	IOAPI_tpfTransferCB pfRXTransferCB;
	IOAPI_tpfTransferCB pfTXTransferCB;	
} IOAPI_tstPortConfigCB;


typedef struct
{
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_enTransferType enTransferType;
	IOAPI_tpfTransferCB pfCB;
	void* pvData;
	uint32 u32ByteCount;
} IOAPI_tstTransferCB;

#endif //IOAPI_H

