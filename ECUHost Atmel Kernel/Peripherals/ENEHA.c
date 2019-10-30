/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.c                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _ENE_C
#include <string.h>
#include "ENEHA.h"

const REGSET_tstReg32Val ENEHA_rastENEReg32Val[] = ENEHA_nReg32Set;


extern uint8_t *TxBuffer;
extern tstNBUF *TxNBUF;
extern tstNBUF *RxNBUF;
tstNBUF ENEHA_stDescriptorTypeTXBuffer;

static void ENEHA_vInit(void);

void ENEHA_vStart(uint32* const u32Stat)
{
#ifdef BUILD_MK60
    OSC0->CR    |= OSC_CR_ERCLKEN_MASK;    /* Enable external reference clock    */
	
	/* turn on ENET clock */
	SIM_vSetReg32(SIM_SCGC2, SIM_SCGC2_ENET_MASK);	
#endif //BUILD_MK60
	
	ENEHA_vInit();
	OS_xModuleStartOK(*u32Stat);
}

void ENEHA_vRun(uint32* const u32Stat)
{

}

void ENEHA_vTerminate(uint32* const u32Stat)
{
	
}

static void ENEHA_vInit(void)
{
#ifdef BUILD_MK60	
	tstENETConfig stEnetConfig;
	MPU_Type* pstMPU = MPU;
	tstENETModule* pstENET;	
	sint32 i32NegotiatedSettings;
	
	REGSET_vInitReg32(&ENEHA_rastENEReg32Val[0]);	

	pstENET = ENET;
    stEnetConfig.ch = 1;            
	stEnetConfig.interface = mac_rmii;   
	stEnetConfig.neg = autoneg_on;                 
	stEnetConfig.loopback = NO_LOOPBACK;     
	stEnetConfig.external = EXTERNAL_NONE;
	stEnetConfig.speed = MII_100BASET;	/* MATTHEW these were swapped! */
	stEnetConfig.duplex = MII_FDX;
	stEnetConfig.prom = 1;     
	memcpy((uint8*)&stEnetConfig.mac[0], (uint8*)&ENE_rau8SourceMACAddress[0], sizeof(ENE_rau8SourceMACAddress));
  
	pstMPU -> CESR = 0;

	enet_reset(stEnetConfig.ch);
	mii_init(stEnetConfig.ch, 50u);	
	
	enet_mib_init(stEnetConfig.ch);
	pstENET -> TACC = ENET_TACC_IPCHK_MASK | ENET_TACC_PROCHK_MASK;
	
	enet_init(&stEnetConfig);	
	
	IRQ_vEnableIRQ(ENET_Transmit_IRQn);   // todo IRQ prio
	IRQ_vEnableIRQ(ENET_Receive_IRQn);
	IRQ_vEnableIRQ(ENET_Error_IRQn);

    if((true == ETH_nAutoNegotiateSpeed) || (true == ETH_nAutoNegotiateDuplex))
	{
		//todo here 4 options
		eth_phy_autoneg(stEnetConfig.ch, ENE_nPHYAddress, MII_10BASET, MII_FDX);
	}
	
    if(true == ETH_nAutoNegotiateSpeed)
	{	
		eth_phy_get_speed(stEnetConfig.ch, ENE_nPHYAddress, &i32NegotiatedSettings);
		stEnetConfig.speed = (ENET_SPEED)i32NegotiatedSettings;
	}
	
    if(true == ETH_nAutoNegotiateDuplex)
	{	
		eth_phy_get_duplex(stEnetConfig.ch, ENE_nPHYAddress, &i32NegotiatedSettings);
		stEnetConfig.duplex = (ENET_DUPLEX)i32NegotiatedSettings;
		enet_duplex(0, stEnetConfig.duplex);    
	}
	
	nbuf_alloc(stEnetConfig.ch);
	nbuf_init(stEnetConfig.ch);	
	
	eth_phy_reg_dump(stEnetConfig.ch, ENE_nPHYAddress);		
	enet_start(stEnetConfig.ch);
	nbuf_start_rx(stEnetConfig.ch);
#endif //BUILD_MK60
}



ENE_tstETHUnionFrame* ENEHA_pstReceiveFrame(void)
{
	ENE_tstETHUnionFrame* pstETHUnionFrame = NULL;

#ifdef BUILD_MK60
	NBUF ENE_stReceiveBD;	
	enet_get_received_packet(0, &ENE_stReceiveBD);	
	pstETHUnionFrame = (ENE_tstETHUnionFrame*)ENE_stReceiveBD.data;
#endif //BUILD_MK60
	
	return pstETHUnionFrame;
}

void ENEHA_vTransmitFrame(puint8 pu8Buff, uint16 u16Length)
{
#ifdef BUILD_MK60
	ENE_stDescriptorTypeTXBuffer.length = u16FrameLength;
	enet_transmit_packet(0, &ENE_stDescriptorTypeTXBuffer);	
#endif		
}

sint32 ENEHA_s32GetENEIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	sint32 i32IDX = -1;
	
#ifdef BUILD_MK60
	if ((EH_VIO_ENET_CH1 <= enEHIOResource) && (EH_VIO_ENET_CH4 >= enEHIOResource)) 
	{
		i32IDX = enEHIOResource - EH_VIO_ENET_CH1;
	}
#endif //BUILD_MK60
		
	return i32IDX;
}




			
