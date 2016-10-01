/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SDHC Header File                                       */
/* DESCRIPTION:        This code provides macros for setup and control of the */
/*                     SDHC hardware module                                   */	
/* FILE NAME:          SDHC.h                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef SDHCHA_H
#define SDHCHA_H

#include <stddef.h>
#include "sys.h"
#include "types.h"
#include "disk.h"
//#include "sdhc1.h"

#ifdef BUILD_MK60
#define SDHCHA_nReg32HWInitSet 																																																																										\
{																																																																																								\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 0])), (uint32)~(PORT_PCR_ISF_MASK  | PORT_PCR_MUX_MASK), REGSET_enAnd },																		\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 0])), (uint32)(PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK), REGSET_enOr }, 	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 1])), (uint32)~(PORT_PCR_ISF_MASK  | PORT_PCR_MUX_MASK), REGSET_enAnd }, 																	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 1])), (uint32)(PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK), REGSET_enOr }, 	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 2])), (uint32)~(PORT_PCR_ISF_MASK  | PORT_PCR_MUX_MASK), REGSET_enAnd }, 																	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 2])), (uint32)PORT_PCR_MUX(4), REGSET_enOr }, 																																\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 3])), (uint32)~(PORT_PCR_ISF_MASK  | PORT_PCR_MUX_MASK), REGSET_enAnd }, 																	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 3])), (uint32)(PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK), REGSET_enOr }, 	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 4])), (uint32)~(PORT_PCR_ISF_MASK  | PORT_PCR_MUX_MASK), REGSET_enAnd }, 																	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 4])), (uint32)(PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK), REGSET_enOr }, 	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 5])), (uint32)~(PORT_PCR_ISF_MASK  | PORT_PCR_MUX_MASK), REGSET_enAnd }, 																	\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 5])), (uint32)(PORT_PCR_MUX(4) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_DSE_MASK), REGSET_enOr }, 	\
		{ NULL, 0, REGSET_enOverwrite }																																																																							\
};
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define SDHCHA_nReg32HWInitSet      \
{									\
    { NULL, 0, REGSET_enOverwrite }	\
};
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
#define SDHCHA_nReg32HWDeInitSet 																																																			\
{																																																															\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 6])), (uint32)PORT_PCR_MUX(1), REGSET_enOverwrite }, 			\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 7])), (uint32)PORT_PCR_MUX(1), REGSET_enOverwrite }, 			\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[ 8])), (uint32)PORT_PCR_MUX(1), REGSET_enOverwrite }, 			\
	{ (volatile uint32*)(PORTE_BASE + offsetof(PORT_Type, PCR[26])), (uint32)PORT_PCR_MUX(1), REGSET_enOverwrite }, 			\
	{ NULL, 0, REGSET_enOverwrite }																																															\
};
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define SDHCHA_nReg32HWDeInitSet       \
{                                      \
	{ NULL, 0, REGSET_enOverwrite }																																															\
};
#endif //BUILD_SAM3X8E

#ifdef BUILD_MK60
/* command opcode macros */
#define SDHCHA_nCMD0_GO_IDLE_STATE            0u
#define SDHCHA_nCMD1_MMC_SEND_OP_COND         1u
#define SDHCHA_nCMD2_ALL_SEND_CID             2u
#define SDHCHA_nCMD3_SET_RELATIVE_ADDR        3u
#define SDHCHA_nCMD6_SWITCH                   6u
#define SDHCHA_nCMD7_SELECT_CARD              7u
#define SDHCHA_nCMD8_SEND_EXT_CSD             8u
#define SDHCHA_nCMD9_SEND_CSD                 9u
#define SDHCHA_nCMD12_STOP_TRANS              12u
#define SDHCHA_nCMD13_SEND_STATUS             13u
#define SDHCHA_nCMD14_BUS_TEST_READ           14u
#define SDHCHA_nCMD16_SET_BLOCKLEN            16u
#define SDHCHA_nCMD17_READ_SINGLE_BLOCK       17u
#define SDHCHA_nCMD18_READ_MULTIPLE_BLOCK     18u
#define SDHCHA_nCMD19_BUS_TEST_WRITE          19u
#define SDHCHA_nCMD24_WRITE_BLOCK             24u
#define SDHCHA_nCMD25_WRITE_MULTIPLE_BLOCK    25u
#define SDHCHA_nCMD27_PROGRAM_CSD             27u
#define SDHCHA_nCMD28_SET_WRITE_PROT          28u
#define SDHCHA_nCMD29_CLR_WRITE_PROT          29u
#define SDHCHA_nCMD30_SEND_WRITE_PROT         30u
#define SDHCHA_nCMD32_TAG_SECTOR_START        32u
#define SDHCHA_nCMD33_TAG_SECTOR_END          33u
#define SDHCHA_nCMD35_TAG_ERASE_GROUP_START   35u
#define SDHCHA_nCMD36_TAG_ERASE_GROUP_END     36u
#define SDHCHA_nCMD38_ERASE                   38u
#define SDHCHA_nCMD55_APP_CMD                 55u
/* application command opcode macros */
#define SDHCHA_nACMD6_SET_BUS_WIDTH           6u
#define SDHCHA_nACMD41_SEND_OP_COND           41u
#define SDHCHA_nACMD51_MMC_SEND_SCR           51u
/* XFERTYP bit macros */
#define SDHCHA_nRESPONSE_NIL                  0x0 << SDHC_XFERTYP_RSPTYP_SHIFT 
#define SDHCHA_nRESPONSE_136                  0x1 << SDHC_XFERTYP_RSPTYP_SHIFT 
#define SDHCHA_nRESPONSE_48                   0x2 << SDHC_XFERTYP_RSPTYP_SHIFT 
#define SDHCHA_nRESPONSE_48_BUSY              0x3 << SDHC_XFERTYP_RSPTYP_SHIFT 
#define SDHCHA_nENABLE_DMA                    SDHC_XFERTYP_DMAEN_MASK
#define SDHCHA_nDATA_PRESENT                  SDHC_XFERTYP_DPSEL_MASK
#define SDHCHA_nDATA_READ                     SDHC_XFERTYP_DTDSEL_MASK
#define SDHCHA_nDATA_WRITE                    0x0
#define SDHCHA_nENABLE_BLOCK_COUNT            SDHC_XFERTYP_BCEN_MASK
#define SDHCHA_nENABLE_AUTO_CMD12             SDHC_XFERTYP_AC12EN_MASK
#define SDHCHA_nMULTIPLE_BLOCK                SDHC_XFERTYP_MSBSEL_MASK
/* IRQSTAT bit macros */
#define SDHCHA_nDATA_TIMEOUT_ERROR_INT        SDHC_IRQSTAT_DTOE_MASK
#define SDHCHA_nDATA_END_BIT_ERROR_INT        SDHC_IRQSTAT_DEBE_MASK
#define SDHCHA_nDATA_CRC_ERROR_INT            SDHC_IRQSTAT_DCE_MASK
#define SDHCHA_nCOMMAND_TIMEOUT_ERROR_INT     SDHC_IRQSTAT_CTOE_MASK
#define SDHCHA_nCOMMAND_CRC_ERROR_INT         SDHC_IRQSTAT_CCE_MASK
#define SDHCHA_nCOMMAND_END_BIT_ERROR_INT     SDHC_IRQSTAT_CEBE_MASK
#define SDHCHA_nCOMMAND_INDEX_ERROR_INT       SDHC_IRQSTAT_CIE_MASK
#define SDHCHA_nDMA_ERROR_INT                 SDHC_IRQSTAT_DMAE_MASK
#define SDHCHA_nCOMMAND_COMPLETE_INT          SDHC_IRQSTAT_CC_MASK
#define SDHCHA_nTRANSFER_COMPLETE_INT         SDHC_IRQSTAT_TC_MASK
#define SDHCHA_nBUFFER_READ_READY_INT         SDHC_IRQSTAT_BRR_MASK
#define SDHCHA_nBUFFER_WRITE_READY_INT        SDHC_IRQSTAT_BWR_MASK
/* AC12ERR bit macros */
#define SDHCHA_nAUTO_CMD12_CRC_ERROR          SDHC_AC12ERR_AC12CE_MASK
#define SDHCHA_nAUTO_CMD12_END_BIT_ERROR      SDHC_AC12ERR_AC12EBE_MASK
#define SDHCHA_nAUTO_CMD12_INDEX_ERROR        SDHC_AC12ERR_AC12IE_MASK
#define SDHCHA_nAUTO_CMD12_ERROR_INT          SDHC_AC12ERR_AC12NE_MASK
#define SDHCHA_nAUTO_CMD12_TIMEOUT_ERROR      SDHC_AC12ERR_AC12TOE_MASK
/* redefines of sdhc1 macros */
#define SDHCHA_nSectorSize                    SDHC1_EXT_CSD_SIZE
#define SDHCHA_nTransferReadOperation         LDD_SDHC_READ
#define SDHCHA_tstBufferDesc                  LDD_SDHC_TBufferDesc
#define	SDHCHA_tenError                       LDD_TError
#define SDHCHA_tstSDData                      LDD_SDHC_TSDData
#define SDHCHA_tenSDHCBusClock12_5MHz         SDHC1_BUS_CLOCK_12_5MHz
#define SDHCHA_nCardDataWidth8Bit             LDD_SDHC_CARD_DATA_WIDTH_8_BIT
#define SDHCHA_nCardDataWidth4Bit             LDD_SDHC_CARD_DATA_WIDTH_4_BIT
#define SDHCHA_nCardDataWidth1Bit             LDD_SDHC_CARD_DATA_WIDTH_1_BIT
/**/
#define SDHCHA_n1_BIT_MODE                    0x0
#define SDHCHA_n4_BIT_MODE                    0x1										
#define SDHCHA_n8_BIT_MODE                    0x2
/* timeouts */
#define SDHCHA_nWaitTimeoutMs                 100u
#define SDHCHA_nTransferTimeoutExponent       14u
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#define SDHCHA_nSectorSize                    512//matthew should be IAW driver macro!
#define SDHCHA_tstSDData                      uint32//matthew should be...
#endif //BUILD_MK60

void SDHCHA_vStart(uint32* const);
void SDHCHA_vRun(uint32* const);
void SDHCHA_vTerminate(uint32* const);
void SDHCHA_vHWInit(void);
void SDHCHA_vHWDeInit(void);
uint8 SDHCHA_u8DiskRead(uint8* const, const uint32, const uint16);
uint8 SDHCHA_u8DiskWrite(uint8* const, const uint32, const uint16);
void SHDCHA_vWaitTimeoutCB(void);

#endif //SDHCHA_H

