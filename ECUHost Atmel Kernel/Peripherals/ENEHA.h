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
#ifndef ENEHA_H
#define ENEHA_H

#include "build.h"
#include "CPUAbstract.h"
//#include "declarations.h"
//#include "eth.h"
#include "ENE.h"
//#include "IOAPI.h"
//#include "irq.h"
//#include "macros.h"
//#include "nbuf.h"
//#include "os.h"
#include "regset.h"
//#include "sys.h"
//#include "SIM.h"
//#include "SYSAPI.h"
#include "types.h"
//#include "WIFI.h"
//#include "TIMER.h"

#ifdef BUILD_MK60
#include "FreescaleDrivers/include/enet.h"
#include "FreescaleDrivers/include/eth.h"
#include "FreescaleDrivers/include/eth_phy.h"
#include "FreescaleDrivers/include/mii.h"
#include "FreescaleDrivers/include/nbuf.h"
#endif

#ifdef BUILD_MK60
#define ENEHA_nReg32Set 																																																										\
{																											    \
	{ (volatile uint32*)(PORTA_BASE + offsetof(PORT_Type, PCR[5])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },     \
	{ (volatile uint32*)(PORTA_BASE + offsetof(PORT_Type, PCR[12])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },    \
	{ (volatile uint32*)(PORTA_BASE + offsetof(PORT_Type, PCR[13])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },    \
	{ (volatile uint32*)(PORTA_BASE + offsetof(PORT_Type, PCR[14])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },    \
	{ (volatile uint32*)(PORTA_BASE + offsetof(PORT_Type, PCR[15])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },    \
	{ (volatile uint32*)(PORTA_BASE + offsetof(PORT_Type, PCR[16])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },    \
	{ (volatile uint32*)(PORTA_BASE + offsetof(PORT_Type, PCR[17])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },    \
	{ (volatile uint32*)(PORTB_BASE + offsetof(PORT_Type, PCR[0])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },     \
	{ (volatile uint32*)(PORTB_BASE + offsetof(PORT_Type, PCR[1])), (uint32)PORT_PCR_MUX(4), REGSET_enOr },     \
    { NULL, 0, REGSET_enOverwrite }
};
#endif

#ifdef BUILD_SAM3X8E	
#define ENEHA_nReg32Set             \
{	                                \
    { NULL, 0, REGSET_enOverwrite } \
};
#endif

#ifdef BUILD_MK60
typedef NBUF tstNBUF;
typedef ENET_CONFIG tstENETConfig;
#endif

#ifdef BUILD_SAM3X8E
typedef struct
{
	uint8_t  ch;            /* FEC channel       */
	uint8_t  interface;     /* Transceiver mode  */
	uint8_t  neg;           /* FEC autoneg */
	uint8_t  speed;         /* Ethernet Speed           */
	uint8_t  duplex;        /* Ethernet Duplex          */
	uint8_t  loopback;      /* Loopback Mode */
	uint8_t  external;      /* outside test? */
	uint8_t  prom;          /* Promiscuous Mode?        */
	uint8_t  mac[6];        /* Ethernet Address         */
} tstENETConfig;//todo struct def will be found in driver folder
#endif

#ifdef BUILD_SAM3X8E
  typedef struct
  {
	  uint16_t status;	            /* control and status */
	  uint16_t length;	            /* transfer length */
	  uint8_t  *data;	                /* buffer address */
	  uint32_t ebd_status;
	  uint16_t length_proto_type;
	  uint16_t payload_checksum;
	  uint32_t bdu;
	  uint32_t timestamp;
	  uint32_t reserverd_word1;
	  uint32_t reserverd_word2;
  } tstNBUF;
#endif //BUILD_SAM3X8E//matthew placeholder

#ifdef BUILD_MK60
typedef ENET_Type tstENETModule;
#endif

#ifdef BUILD_SAM3X8E
typedef Emac tstENETModule;
#endif


#define ENEHA_nAutoNegotiateDuplex     false
#define ENEHA_nAutoNegotiateSpeed      false
#define	ENEHA_nDefaultDuplex           MII_FDX
#define ENEHA_nDefaultSpeed            MII_100BASET
#define ENEHA_nBMOUPollTimeoutMs       1000u
#define ENEHA_nENETChannels            (EH_VIO_FTM0 - EH_VIO_ENET_CH1)


#define ENE_xIntActivated(x)	((ENET -> EIR & (x)) == (x))
#define ENE_xIntClearFlag(x)	(ENET -> EIR |= (x))


void ENEHA_vStart(uint32* const);
void ENEHA_vRun(uint32* const);
void ENEHA_vTerminate(uint32* const);
ENE_tstETHUnionFrame* ENEHA_pstReceiveFrame(void);
void ENEHA_vTransmitFrame(puint8, uint16);
sint32 ENEHA_s32GetENEIndex(IOAPI_tenEHIOResource enEHIOResource);

#endif //ENEHA_H

