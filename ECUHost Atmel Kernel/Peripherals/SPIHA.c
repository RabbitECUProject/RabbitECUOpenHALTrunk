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
#include "SPIHA.h"


const REGSET_tstReg8Val SPIHA_rastPIMReg8ValSPI0[] = SPIHA_nReg8SetSPI0;
const REGSET_tstReg8Val SPIHA_rastPIMReg8ValSPI1[] = SPIHA_nReg8SetSPI1;
const SPIHA_tstDivisorMap SPIHA_rastDivisorMap[] = SPIHA_nDivisorMap;
IOAPI_tstTransferCB* SPIHA_pstTransferCB;
uint32 SPIHA_u32BytesToTransfer;
void* SPIHA_pvData;
IOAPI_tpfTransferCB SPIHA_pfCB;
tstSPIModule* SPIHA_pstSPI;	

uint32 SPI_u32PortClockRequested;

static sint32 SPIHA_u32GetSPIIndex(IOAPI_tenEHIOResource);

void SPIHA_vStart(puint32 const u32Stat)
{
	SPIHA_pfCB = NULL;
	SPIHA_pstSPI = NULL;
	SPIHA_u32BytesToTransfer = 0;
	OS_xModuleStartOK(*u32Stat);
}

void SPIHA_vRun(puint32 const pu32Stat)
{
	
}

void SPIHA_vTerminate(puint32 const pu32Stat)
{
	
}

uint32 SPIHA_u32InitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
    uint32 u32MuxSel = ~0;

	sint32 i32IDX = SPIHA_u32GetSPIIndex(enEHIOResource);	
	
	if ((-1 != i32IDX) && (TRUE == DLL_boInitDLLChannel(enEHIOResource, pstPortConfigCB)))	
	{
#ifdef BUILD_MK60
	REGSET_tstReg32Val SPI_astSPIReg32Val[3];
		IRQn_Type enIRQType;
		tstSPIModule* pstSPI;
		uint32 u32Mul;
		uint32 u32DivCalc;
		uint32 u32Div;
		uint32 u32DivMapIDX;

		switch (enEHIOResource)
		{
			case EH_VIO_SPI1:
			{				
				SPI_xRequestPortClock(SIM_SCGC4_SPI0_MASK);
				
				SPI_astSPIReg32Val[0].reg = (vpuint32)(PORTD_BASE + offsetof(PORT_Type, PCR[8]));
				SPI_astSPIReg32Val[0].val = (uint32)(PORT_PCR_MUX(2) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
				SPI_astSPIReg32Val[0].writeMode = REGSET_enOverwrite;
			
				SPI_astSPIReg32Val[1].reg = (vpuint32)(PORTD_BASE + offsetof(PORT_Type, PCR[9]));
				SPI_astSPIReg32Val[1].val = (uint32)(PORT_PCR_MUX(2) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
				SPI_astSPIReg32Val[1].writeMode = REGSET_enOverwrite;			
			
				SPI_astSPIReg32Val[2].reg = NULL;			
				REGSET_vInitReg32(&SPI_astSPIReg32Val[0]);			
			
				enSVCResult = SYSAPI_enBaudRateUnavailable;
				pstSPI = I2C0;
			
				SPI_xRequestPortClock(SIM_SCGC4_SPI0_MASK);
				u32Mul = pstPortConfigCB->u32BaudRateHz / (SYS_FREQ_BUS / SPI_rastDivisorMap[0].u32SCLDivider);
				u32Mul = (4 < u32Mul) ? 4 : u32Mul;
			
				switch (u32Mul)
				{
					case 0: u32Mul = 1; break;
					case 3: u32Mul = 2; break;
					default: u32Mul = 4; break;
				}
			
				u32DivCalc = SYS_FREQ_BUS / (u32Mul * pstPortConfigCB->u32BaudRateHz);
			
				if (SPI_rastDivisorMap[63].u32SCLDivider < u32DivCalc) break;
				enSVCResult = SYSAPI_enOK;			
			
				SPI_xCalcDivisor;
				enIRQType = I2C0_IRQn;

		        pstSPI->F = (I2C_F_MULT(u32Mul) | I2C_F_ICR(u32Div));	
	            pstSPI->C1 |= I2C_C1_SPIEN_MASK;		
	            IRQ_vEnableIRQ(enIRQType);
				break;			
			}
			case EH_VIO_SPI2:
			{
				SPI_xRequestPortClock(SIM_SCGC4_SPI1_MASK);			
				break;
			}
			default:
			{
				break;
			}
		}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
		Spi* pstSPIModule = NULL;
		u32MuxSel = 0;

		switch (pstPortConfigCB->enVIOResource)
		{
			case EH_VIO_SPI1:
			{
				pstSPIModule = SPI0;
				break;
			}
			default:
			{
				break;
			}
		}

		if (NULL != pstPortConfigCB)
		{
			spi_enable_clock(pstSPIModule);//matthew non standard clock setup
			spi_reset(pstSPIModule);
			spi_set_master_mode(pstSPIModule);
			spi_disable_mode_fault_detect(pstSPIModule);
			spi_disable_loopback(pstSPIModule);
			spi_set_peripheral_chip_select_value(pstSPIModule, spi_get_pcs(SPIHA_nDeviceID));
			spi_set_fixed_peripheral_select(pstSPIModule);
			spi_disable_peripheral_select_decode(pstSPIModule);
			spi_set_delay_between_chip_select(pstSPIModule, 0);

			spi_set_transfer_delay(pstSPIModule, SPIHA_nDeviceID, 0, 0);
			spi_set_bits_per_transfer(pstSPIModule, SPIHA_nDeviceID, SPIHA_nBitCount);
			spi_set_baudrate_div(pstSPIModule, SPIHA_nDeviceID, SPIHA_xSPIBaudRateDivider(pstPortConfigCB->u32BaudRateHz));
			spi_configure_cs_behavior(pstSPIModule, SPIHA_nDeviceID, SPI_CS_KEEP_LOW);

			spi_set_clock_polarity(pstSPIModule, SPIHA_nDeviceID, pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.boClockIdleHigh);
			spi_set_clock_phase(pstSPIModule, SPIHA_nDeviceID, pstPortConfigCB->stPinConfig.uPinInfo.stSPIPinInfo.boShiftRising);
			spi_enable(pstSPIModule);
            spi_enable_interrupt(pstSPIModule, SPI_IER_TDRE);
		    IRQ_vEnableIRQ(SPI0_IRQn, IRQ_enPRIO_15, &SPI_vInterrupt, NULL);//matthew this could be moved out of HAL is generic CMx!
		}
#endif //BUILD_SAM3X8E
	}	
	return u32MuxSel;
}

void SPIHA_vInitTransfer(IOAPI_tstTransferCB* pstTransferCB)
{
	SPIHA_pstTransferCB = pstTransferCB;
	tstSPIModule* pstSPIModule = NULL;
	volatile uint32 u32Count;

#ifdef BUILD_MK60
	switch (pstTransferCB->enEHIOResource)
	{
		case EH_VIO_SPI1:
		{
			//SPIHA_pstSPI = I2C0;
			//SPIHA_u32BytesToTransfer = pstTransferCB->u32ByteCount - 1;
			//SPIHA_pfCB = pstTransferCB->pfCB;
			//SPIHA_pvData = pstTransferCB->pvData;
			//SPIHA_pstSPI->C1 |= (I2C_C1_TX_MASK | I2C_C1_SPIIE_MASK | I2C_C1_MST_MASK);
			//SPIHA_pstSPI->D = *(uint8*)SPI_pvData;
			//SPIHA_pvData = (void*)((uint32)SPI_pvData + 1);
		}
		default:
		{
		    break;
		}
	}
#endif  //BUILD_MK60

#ifdef BUILD_SAM3X8E
    switch (pstTransferCB->enEHIOResource)
	{
        case EH_VIO_SPI1:
        {
		    pstSPIModule = SPI0;
        }
		default:
        {
		    break;
		}
	}

    if (NULL != pstSPIModule)
    {
		SPIHA_pstSPI = SPI0;
		SPIHA_u32BytesToTransfer = pstTransferCB->u32ByteCount - 1;
		SPIHA_pfCB = pstTransferCB->pfCB;
		SPIHA_pvData = pstTransferCB->pvData;

		if (TRUE == pstTransferCB->boBlockingMode)
		{
			for (u32Count = 0; u32Count < 250; u32Count++)
			{
				__asm("nop");
			}
		}

        spi_write(pstSPIModule, (uint16)*(puint8)pstTransferCB->pvData, 0, 0);

		if (TRUE == pstTransferCB->boBlockingMode)
		{
			while (0 < SPIHA_u32BytesToTransfer)
			{
				for (u32Count = 0; u32Count < 250; u32Count++)
				{
					__asm("nop");
				}

				SPIHA_pvData = (void*)((uint32)SPIHA_pvData + 1);
				spi_write(pstSPIModule, (uint16)*(puint8)SPIHA_pvData, 0, 0);
				SPIHA_u32BytesToTransfer--;
			}

			for (u32Count = 0; u32Count < 250; u32Count++)
			{
				__asm("nop");
			}
		}
		else
		{
			SPIHA_pvData = (void*)((uint32)SPIHA_pvData + 1);
			spi_enable_interrupt(pstSPIModule, SPI_SR_TDRE);
		}
    }
#endif //BUILD_SAM3X8E
}

void SPIHA_vInterruptHandler(IOAPI_tenEHIOResource enEHIOResource)
{
	IOAPI_tenPortMode enMode;
	tstSPIModule* pstSPI = NULL;	
	volatile uint32 u32Counter;

#ifdef BUILD_MK60
	SPIHA_pstSPI->S |= I2C_S_SPIIF_MASK;
		
	if (0 < SPIHA_u32BytesToTransfer)
	{
		SPIHA_pstSPI->D = *(uint8*)SPI_pvData;
		SPIHA_pvData = (void*)((uint32)SPI_pvData + 1);
		SPIHA_u32BytesToTransfer -= 1;
	}	
	else
	{
		SPIHA_pstSPI->C1 &= ~(I2C_C1_SPIIE_MASK | I2C_C1_MST_MASK);	

		enMode = DLL_enGetChannelMode(enEHIOResource);
			
		if (IOAPI_enPortSerialTransfer == enMode)
		{
			SRLTFR_vNotifyCB(EH_VIO_SPI1);			
		}
		else if (IOAPI_enPortComms == enMode)
		{			
			//DLL_vFrameRXCB(enEHIOResource, &CAN_stRXDLLData);						
		}		
	}
#endif  //BUILD_MK60

#ifdef BUILD_SAM3X8E
    uint16 u16Data;

    switch(enEHIOResource)
	{
	    case EH_VIO_SPI1:
		{
		    pstSPI = SPI0;
			break;
		}
		default:
		{
		    break;
		}
	}

	if (((NULL != SPIHA_pstSPI) && (NULL != pstSPI)) &&
	   (SPIHA_pstSPI == pstSPI))
	{
		if (0 < SPIHA_u32BytesToTransfer)
		{
		    u16Data = (uint16)*(puint8)SPIHA_pvData;
			spi_write(SPIHA_pstSPI, u16Data, 0, 0);
			SPIHA_pvData = (void*)((uint32)SPIHA_pvData + 1);
			SPIHA_u32BytesToTransfer -= 1;

			for (u32Counter = 0; u32Counter < 199; u32Counter++)
			{
				__asm("nop");
			}
		}
		else
		{
			enMode = DLL_enGetChannelMode(enEHIOResource);
	
			if (IOAPI_enPortSerialTransfer == enMode)
			{
				SRLTFR_vNotifyCB(EH_VIO_SPI1);
			}
			else if (IOAPI_enPortComms == enMode)
			{
				//DLL_vFrameRXCB(enEHIOResource, &CAN_stRXDLLData);
			}

            spi_disable_interrupt(pstSPI, SPI_SR_TDRE);
        }
    }
	else if (NULL != pstSPI)
	{
        spi_disable_interrupt(pstSPI, SPI_SR_TDRE);
	}
#endif //BUILD_SAM3X8E
}

static sint32 SPIHA_u32GetSPIIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	sint32 i32IDX = -1;
	
	if ((EH_VIO_SPI1 <= enEHIOResource) && (EH_VIO_SPI2 >= enEHIOResource)) 
	{
		i32IDX = enEHIOResource - EH_VIO_SPI1;
	}
		
	return i32IDX;
}




			
			
