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
#include "IICHA.h"


const REGSET_tstReg8Val IICHA_rastPIMReg8ValIIC0[] = IICHA_nReg8SetIIC0;
const REGSET_tstReg8Val IICHA_rastPIMReg8ValIIC1[] = IICHA_nReg8SetIIC1;
const IICHA_tstDivisorMap IICHA_rastDivisorMap[] = IICHA_nDivisorMap;
IOAPI_tstTransferCB* IICHA_pstTransferCB;
uint32 IICHA_u32BytesToTransfer;
void* IICHA_pvData;
IOAPI_tpfTransferCB IICHA_pfCB;
tstI2CModule* IICHA_pstIIC;	

uint32 IIC_u32PortClockRequested;

#if defined(BUILD_MK60)
static sint32 IICHA_u32GetIICIndex(IOAPI_tenEHIOResource);
#endif //BUILD_MK60

void IICHA_vStart(puint32 const u32Stat)
{
	IICHA_pfCB = NULL;
	IICHA_pstIIC = NULL;
	IICHA_u32BytesToTransfer = 0;
	OS_xModuleStartOK(*u32Stat);
}

void IICHA_vRun(puint32 const pu32Stat)
{
	
}

void IICHA_vTerminate(puint32 const pu32Stat)
{
	
}

SYSAPI_tenSVCResult IICHA_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enResourceAlreadyAllocated;	
#ifdef BUILD_MK60	
	uint32 u32Mul;
	uint32 u32DivCalc;
	uint32 u32Div;
	uint32 u32DivMapIDX;
	tstI2CModule* pstIIC;
	IRQn_Type enIRQType;

	REGSET_tstReg32Val IIC_astIICReg32Val[3];
	sint32 i32IDX = IICHA_u32GetIICIndex(enEHIOResource);	
	
	if ((-1 != i32IDX) && (TRUE == DLL_boInitDLLChannel(enEHIOResource, pstPortConfigCB)))	
	
	switch (enEHIOResource)
	{
		case EH_VIO_IIC1:
		{				
			IIC_xRequestPortClock(SIM_SCGC4_IIC0_MASK);
				
			IIC_astIICReg32Val[0].reg = (vpuint32)(PORTD_BASE + offsetof(PORT_Type, PCR[8]));
			IIC_astIICReg32Val[0].val = (uint32)(PORT_PCR_MUX(2) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
			IIC_astIICReg32Val[0].writeMode = REGSET_enOverwrite;
			
			IIC_astIICReg32Val[1].reg = (vpuint32)(PORTD_BASE + offsetof(PORT_Type, PCR[9]));
			IIC_astIICReg32Val[1].val = (uint32)(PORT_PCR_MUX(2) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);
			IIC_astIICReg32Val[1].writeMode = REGSET_enOverwrite;			
			
			IIC_astIICReg32Val[2].reg = NULL;			
			REGSET_vInitReg32(&IIC_astIICReg32Val[0]);			
			
			enSVCResult = SYSAPI_enBaudRateUnavailable;
			pstIIC = I2C0;
			
			IIC_xRequestPortClock(SIM_SCGC4_IIC0_MASK);
			u32Mul = pstPortConfigCB->u32BaudRateHz / (SYS_FREQ_BUS / IIC_rastDivisorMap[0].u32SCLDivider);
			u32Mul = (4 < u32Mul) ? 4 : u32Mul;
			
			switch (u32Mul)
			{
				case 0: u32Mul = 1; break;
				case 3: u32Mul = 2; break;
				default: u32Mul = 4; break;
			}
			
			u32DivCalc = SYS_FREQ_BUS / (u32Mul * pstPortConfigCB->u32BaudRateHz);
			
			if (IIC_rastDivisorMap[63].u32SCLDivider < u32DivCalc) break;
			enSVCResult = SYSAPI_enOK;			
			
			IIC_xCalcDivisor;
			enIRQType = I2C0_IRQn;
			break;			
		}
		case EH_VIO_IIC2:
		{
			IIC_xRequestPortClock(SIM_SCGC4_IIC1_MASK);			
			break;
		}
		default:
		{
			break;
		}
	}	
	
	if (SYSAPI_enOK	== enSVCResult)
	{
		pstIIC->F = (I2C_F_MULT(u32Mul) | I2C_F_ICR(u32Div));	
		pstIIC->C1 |= I2C_C1_IICEN_MASK;		
		IRQ_vEnableIRQ(enIRQType);
	}
#endif //BUILD_MK60	
	return enSVCResult;
}

void IICHA_vInitTransfer(IOAPI_tstTransferCB* pstTransferCB)
{
#ifdef BUILD_MK60	
	IICHA_pstTransferCB = pstTransferCB;

	switch (pstTransferCB->enEHIOResource)
	{

		case EH_VIO_IIC1:
		{
			IICHA_pstIIC = I2C0;
			IICHA_u32BytesToTransfer = pstTransferCB->u32ByteCount - 1;
			IICHA_pfCB = pstTransferCB->pfCB;
			IICHA_pvData = pstTransferCB->pvData;
			IICHA_pstIIC->C1 |= (I2C_C1_TX_MASK | I2C_C1_IICIE_MASK | I2C_C1_MST_MASK);
			IICHA_pstIIC->D = *(uint8*)IIC_pvData;
			IICHA_pvData = (void*)((uint32)IIC_pvData + 1);
		}
		default:
        {
		    break;
		}
	}
#endif  //BUILD_MK60
}

void IICHA_vInterrupt(IOAPI_tenEHIOResource enEHIOResource)
{
	if (NULL != IICHA_pstIIC)
	{
#ifdef BUILD_MK60
		IICHA_pstIIC->S |= I2C_S_IICIF_MASK;
		
		if (0 < IICHA_u32BytesToTransfer)
		{
			IICHA_pstIIC->D = *(uint8*)IIC_pvData;
			IICHA_pvData = (void*)((uint32)IIC_pvData + 1);
			IICHA_u32BytesToTransfer -= 1;
		}	
		else
		{
			IICHA_pstIIC->C1 &= ~(I2C_C1_IICIE_MASK | I2C_C1_MST_MASK);	

			enMode = DLL_enGetChannelMode(enEHIOResource);
			
			if (IOAPI_enPortSerialTransfer == enMode)
			{
				SRLTFR_vNotifyCB(EH_VIO_IIC1);			
			}
			else if (IOAPI_enPortComms == enMode)
			{			
				//DLL_vFrameRXCB(enEHIOResource, &CAN_stRXDLLData);						
			}				
		}
#endif  //BUILD_MK60
	}
}

#if defined(BUILD_MK60)
static sint32 IICHA_u32GetIICIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	sint32 i32IDX = -1;
	
	if ((EH_VIO_IIC1 <= enEHIOResource) && (EH_VIO_IIC2 >= enEHIOResource)) 
	{
		i32IDX = enEHIOResource - EH_VIO_IIC1;
	}
		
	return i32IDX;
}
#endif //BUILD_MK60



			
			
