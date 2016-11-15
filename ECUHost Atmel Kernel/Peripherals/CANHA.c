/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CAN Hardware Abstraction                               */
/* DESCRIPTION:        This code provides functions for interacting with      */
/*                     the CAN hardware module/s                              */
/* FILE NAME:          CANHA.c                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <string.h>
#include "can.h"
#include "PERCAN.h"

const CANHA_tstTimingSettings CANHA_astTimingSettings[] = CANHA_nSetTimingData;

static sint32 CANHA_u32GetCANIndex(IOAPI_tenEHIOResource);

#define CANHA_nTimingOptionsCount	(sizeof(CANHA_astTimingSettings) / sizeof(CANHA_tstTimingSettings))

void CANHA_vStart(uint32* const u32Stat)
{

	
}

SYSAPI_tenSVCResult CANHA_enInitBus(IOAPI_tenEHIOResource enEHIOResource, IOAPI_tstPortConfigCB* pstPortConfigCB)
{
	tstCANModule* pstCAN;
	SYSAPI_tenSVCResult enSVCResult = SYSAPI_enBadResource;
	IRQn_Type	nCANIRQ;
	sint32 i32IDX = CANHA_u32GetCANIndex(enEHIOResource);

#ifdef BUILD_MK60
	REGSET_tstReg32Val astCANReg32Val[54];	
	uint8 u8RegIDX = 0;
	uint8 u8TimingIDX;
	uint8 u8PRESDIV;
	uint8 u8SJW = 3;
	uint8 u8TSEG1 = 15;
	uint8 u8TSEG2 = 7;
	uint8 u8MBXIDX;
	

	
	if ((-1 != i32IDX) && (TRUE == DLL_boInitDLLChannel(enEHIOResource, pstPortConfigCB)))	
	{
		switch (enEHIOResource)
		{
			case EH_VIO_CAN1:
			{
				pstCAN = (CAN_Type*)CAN0_BASE;
				nCANIRQ = CAN0_ORed_Message_buffer_IRQn;
				/* turn on FLEXCAN1 clock */
				SIM_vSetReg32(SIM_SCGC6, SIM_SCGC6_FLEXCAN0_MASK);						
				break;
			}
			case EH_VIO_CAN2:
			{
				pstCAN = (CAN_Type*)CAN1_BASE;
				nCANIRQ = CAN1_ORed_Message_buffer_IRQn;			
				/* turn on FLEXCAN2 clock */
				SIM_vSetReg32(SIM_SCGC3, SIM_SCGC3_FLEXCAN1_MASK);			
				break;
			}		
			default:
			{
				pstCAN = NULL;
				break;
			}
		}
			
		if (NULL != pstCAN)
		{			
			for (u8TimingIDX = 0; u8TimingIDX < CAN_nTimingOptionsCount; u8TimingIDX++)
			{
				if (CAN_astTimingSettings[u8TimingIDX].u32BitRate == pstPortConfigCB->u32BaudRateHz)
				{
					u8PRESDIV = CAN_astTimingSettings[u8TimingIDX].u8PRESDIV;					
					u8SJW = CAN_astTimingSettings[u8TimingIDX].u8SJW;
					u8TSEG1 = CAN_astTimingSettings[u8TimingIDX].u8TSEG1;			
					u8TSEG2 = CAN_astTimingSettings[u8TimingIDX].u8TSEG2;			
				}
			}
			
			pstCAN -> MCR &= ~CAN_MCR_MDIS_MASK;
			pstCAN -> MCR |= CAN_MCR_SOFTRST_MASK;
			while(pstCAN -> MCR & CAN_MCR_SOFTRST_MASK);
			
			pstCAN -> MCR |= CAN_MCR_FRZ_MASK;
			pstCAN -> MCR |= CAN_MCR_HALT_MASK;		
			pstCAN -> MCR |= CAN_MCR_IRMQ_MASK;	
			while(!(pstCAN -> MCR & CAN_MCR_FRZACK_MASK));			
			
			astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, CTRL1));
			astCANReg32Val[u8RegIDX].val = CAN_CTRL1_PRESDIV(u8PRESDIV)
															+ CAN_CTRL1_PSEG1(u8TSEG1)
															+ CAN_CTRL1_PSEG2(u8TSEG2)
															+ CAN_CTRL1_RJW(u8SJW);			
			astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;	
			
			/* Set the MBX RX CS fields */
			for (u8MBXIDX = 0; u8MBXIDX < CAN_nCANRXMailboxCount; u8MBXIDX++)
			{
				astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, MB[u8MBXIDX].CS));	
				astCANReg32Val[u8RegIDX].val = CAN_CS_CODE(CAN_nCANMBXEmpty);	
				astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;	
			}
			
			/* Set the MBX TX CS fields */
			for (u8MBXIDX = CAN_nCANRXMailboxCount; u8MBXIDX < CAN_nCANMailboxCount; u8MBXIDX++)
			{
				astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, MB[u8MBXIDX].CS));	
				astCANReg32Val[u8RegIDX].val = CAN_CS_CODE(CAN_nCANMBXInactive2);	
				astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;	
			}						
			
			/* Set the global diag and diag MB ID fields */
			astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, MB[0].ID));	
			astCANReg32Val[u8RegIDX].val = CAN_ID_STD(pstPortConfigCB->stNetConfig.uNetInfo.stCANNetInfo.u32GlobalCANDiagAddress)
							| CAN_ID_PRIO(0);			
			astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;		
			astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, MB[1].ID));	
			astCANReg32Val[u8RegIDX].val = CAN_ID_STD(pstPortConfigCB->stNetConfig.uNetInfo.stCANNetInfo.u32CANDiagAddress)
							| CAN_ID_PRIO(0);			
			astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;	

			/* Set the global diag and diag ID mask fields */
			astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, RXIMR[0]));	
			astCANReg32Val[u8RegIDX].val = 0x7ff << CAN_ID_STD_SHIFT;	
			astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;				
			astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, RXIMR[1]));	
			astCANReg32Val[u8RegIDX].val = 0x7ff << CAN_ID_STD_SHIFT;		
			astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;				

			/* Set the priority MB ID fields */			
			for (u8MBXIDX = 2; u8MBXIDX < 6; u8MBXIDX++)
			{			
				astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, MB[u8MBXIDX].ID));	
				
				if (0x800u > pstPortConfigCB->stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[u8MBXIDX - 2])
				{
					astCANReg32Val[u8RegIDX].val = CAN_ID_STD(pstPortConfigCB->stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[u8MBXIDX - 2])
								| CAN_ID_PRIO(0);
				}
				else
				{
					astCANReg32Val[u8RegIDX].val = CAN_ID_EXT(pstPortConfigCB->stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[u8MBXIDX - 2])
								| CAN_ID_PRIO(0);
				}					
				astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;	
			}
			
			/* Set the priority ID mask fields */
			for (u8MBXIDX = 2; u8MBXIDX < 6; u8MBXIDX++)
			{			
				astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, RXIMR[u8MBXIDX]));	
				
				if (0x800u > pstPortConfigCB->stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[u8MBXIDX - 2])
				{
					astCANReg32Val[u8RegIDX].val = pstPortConfigCB->stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[u8MBXIDX - 2] << CAN_ID_STD_SHIFT;
				}
				else
				{
					astCANReg32Val[u8RegIDX].val = pstPortConfigCB->stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[u8MBXIDX - 2] << CAN_ID_EXT_SHIFT;
				}			
				astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;	
			}		

			/* Clear remaining MB ID fields */
			for (u8MBXIDX = 6; u8MBXIDX < 16; u8MBXIDX++)
			{			
				astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, MB[u8MBXIDX].ID));	
				astCANReg32Val[u8RegIDX].val = 0u;	
				astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;	
			}	

			/* Clear the remaining ID mask fields */
			for (u8MBXIDX = 6; u8MBXIDX < 16; u8MBXIDX++)
			{			
				astCANReg32Val[u8RegIDX].reg = (volatile uint32*)((uint32)pstCAN + (uint32)offsetof(CAN_Type, RXIMR[u8MBXIDX]));	
				astCANReg32Val[u8RegIDX].val = 0u;		
				astCANReg32Val[u8RegIDX++].writeMode =	REGSET_enOverwrite;				
			}	

			astCANReg32Val[u8RegIDX].reg = NULL;							
			REGSET_vInitReg32(&astCANReg32Val[0]);													
		
			pstCAN -> MCR &= ~CAN_MCR_HALT_MASK;
			while((pstCAN -> MCR) & CAN_MCR_FRZACK_MASK);			
			
			IRQ_vEnableIRQ(nCANIRQ);
			pstCAN -> IMASK1 = 0x0000FFFF;				
			
			enSVCResult = SYSAPI_enOK;
		}	
	}
#endif

#ifdef BUILD_SAM3X8E

	if ((-1 != i32IDX) && (TRUE == DLL_boInitDLLChannel(enEHIOResource, pstPortConfigCB)))
	{
		switch (enEHIOResource)
		{
			case EH_VIO_CAN1:
			{
				pstCAN = (tstCANModule*)CAN0;
				nCANIRQ = CAN0_IRQn;
				break;
			}
			case EH_VIO_CAN2:
			{
				pstCAN = (tstCANModule*)CAN1;
				nCANIRQ = CAN1_IRQn;
				break;
			}
			default:
			{
				pstCAN = NULL;
				break;
			}
		}
		
		if (NULL != pstCAN)
		{
			/* turn on peripheral clock */
			SIM_boEnablePeripheralClock(nCANIRQ);
            can_init(pstCAN, SYS_FREQ_BUS, pstPortConfigCB->u32BaudRateHz);
		}
	}
#endif //BUILD SAM3X8E
	
	return enSVCResult;
}

void CANHA_vRun(uint32* const u32Stat)
{

}

void CANHA_vInitTransfer(IOAPI_tstTransferCB* pstTransferCB)
{
	tstCANModule* pstCAN = NULL;
	PROTAPI_tstCANMsg* pstCANMsg;

	
#ifdef BUILD_MK60		
	CANHA_tstCANMB* pstCANMB;

	uint32 u32MBIDX;
	uint32 u32IMaskMax = MATH_u32IDXToMask(CAN_nCANMailboxCount);
	uint32 u32IMask = MATH_u32IDXToMask(CAN_nCANRXMailboxCount);	
	
	switch (pstTransferCB->enEHIOResource)
	{
		case EH_VIO_CAN1:
		{			
			pstCAN = (CAN_Type*)CAN0_BASE;
			break;
		}
		case EH_VIO_CAN2:
		{			
			pstCAN = (CAN_Type*)CAN1_BASE;
			break;
		}		
		default:
		{
			break;
		}
	}
	
	if (NULL != pstCAN)
	{
		while(u32IMask < u32IMaskMax)
		{
			if (0 == (u32IMask & (pstCAN -> IFLAG1)))
			{
				pstCANMB = (CAN_tstCANMB*)&pstCAN ->MB[0];
				u32MBIDX = MATH_u32MaskToIDX(u32IMask);
				pstCANMB += u32MBIDX;
				
				/* Data must be CAN frame so cast the pointer */
				pstCANMsg = (PROTAPI_tstCANMsg*)pstTransferCB->pvData;				
				
				if (0x800u > pstCANMsg->u32ID)
				{
					pstCANMB->u32ID = CAN_ID_STD(pstCANMsg->u32ID)
								| CAN_ID_PRIO(0);
				}
				else
				{
					pstCANMB->u32ID = CAN_ID_EXT(pstCANMsg->u32ID)
								| CAN_ID_PRIO(0);
				}					
				
				pstCANMB->u32DWH = pstCANMsg->u32DWH;
				pstCANMB->u32DWL = pstCANMsg->u32DWL;		
				pstCANMB->u32CS = CAN_CS_CODE(CAN_nCANMBXData) | 
														CAN_CS_DLC(pstCANMsg->u8DLC);
				break;				
			}
		}
	}	
#endif
}

void CANHA_vTerminate(uint32* const u32Stat)
{

}

static sint32 CANHA_u32GetCANIndex(IOAPI_tenEHIOResource enEHIOResource)
{
	sint32 i32IDX = -1;
	
#ifdef BUILD_MK60
	if ((EH_VIO_CAN1 <= enEHIOResource) && (EH_VIO_CAN2 >= enEHIOResource)) 
	{
		i32IDX = enEHIOResource - EH_VIO_CAN1;
	}
#endif
		
	return i32IDX;
}


			
			
