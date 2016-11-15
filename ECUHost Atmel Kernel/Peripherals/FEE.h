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
#ifndef FEE_H
#define FEE_H

#include <string.h>
#include <stddef.h>
#include "COMMONNL.h"
#include "CRC16.h"
#include "sys.h"
#include "types.h"

#define M8(adr)    (*((volatile unsigned char  *) (adr)))
#define M16(adr)   (*((volatile unsigned short *) (adr)))
#define M32(adr)   (*((volatile unsigned long  *) (adr)))

// FEE programming interface addresse ranges
#define FEE_PFLASH_START 			0x00000000u    		// Program Flash base address
#define FEE_PFLASH_END	 			0x0007FFFFu				// Program Flash end address
#define FEE_DFLASH_START 			0x10000000u   		// Data    Flash base address
#define FEE_DFLASH_END	 			0x1007FFFFu				// Data		 Flash end address
#define FEE_EEPROM_START			0x14000000u				// EEprom	 Base address
#define FEE_EEPROM_END				0x14003FFFu				// EEprom  Base address
#define FEE_WORK_DATA_START		0x20000000u
#define FEE_WORK_DATA_END			0x2000FFFFu
#define FEE_WORK_DATA_MAX			0x4000u

// FEE sector sizes
#define FEE_PFLASH_SCTR_BYTES	4096u							// Program Flash bytes per sector
#define FEE_DFLASH_SCTR_BYTES	4096u							// Data 	 Flash bytes per sector
#define FEE_EEPROM_SCTR_BYTES	1024u							// EEprom        bytes per sector

#define FEE_PFLASH_BYTES_PHRS	0x10							// Program Flash bytes per phrase
#define FEE_DFLASH_BYTES_PHRS	0x10							// Data 	 Flash bytes per phrase
#define FEE_EEPROM_BYTES_PHRS	0x10							// EEprom        bytes per phrase

#define FEE_PFLASH_BYTES_WORD	0x08							// Program Flash bytes per word
#define FEE_DFLASH_BYTES_WORD	0x08							// Data 	 Flash bytes per word
#define FEE_EEPROM_BYTES_WORD	0x01							// EEprom        bytes per word

#define FEE_PFLASH_SCTR_WORDS	(FEE_PFLASH_SCTR_BYTES / FEE_PFLASH_BYTES_WORD)
#define FEE_DFLASH_SCTR_WORDS	(FEE_DFLASH_SCTR_BYTES / FEE_DFLASH_BYTES_WORD)
#define FEE_EEPROM_SCTR_WORDS	(FEE_EEPROM_SCTR_BYTES / FEE_EEPROM_BYTES_WORD)

#define FEE_PFLASH_SCTR_PHRS  (FEE_PFLASH_SCTR_BYTES / FEE_PFLASH_BYTES_PHRS)
#define FEE_DFLASH_SCTR_PHRS  (FEE_DFLASH_SCTR_BYTES / FEE_DFLASH_BYTES_PHRS)
#define FEE_EEPROM_SCTR_PHRS  (FEE_EEPROM_SCTR_BYTES / FEE_EEPROM_BYTES_PHRS)

// FEE Commands
#define FEE_CMD_READ1SB 0x00            				// Verify block erased
#define FEE_CMD_READ1SS 0x01            				// Verify section erased
#define FEE_CMD_PRGCHK	0x02										// Checks programmed phrases at margin read
#define FEE_CMD_READIFR	0x03										// READ IFR field
#define FEE_CMD_PRG			0x07										// Program 8 bytes
#define FEE_CMD_ERASEB	0x08										// Erase Flash block
#define FEE_CMD_ERASES	0x09										// Erase Flash sector
#define FEE_CMD_PRGS		0x0B										// program data from section program buffer
#define FEE_CMD_READ1SL	0x40										// Verify all blocks erased
#define FEE_CMD_ERASEL	0x44										// Erase all blocks

// FEE constants
#define FEE_PFLASH_PRT3	0xFF
#define FEE_PFLASH_PRT2 0xFF
#define FEE_PFLASH_PRT1 0xFF
#define FEE_PFLASH_PRT0 0xFE
#define FEE_DFLASH_PROT	0xFF
#define FEE_EFLASH_PROT	0xFF
#define FEE_SEC					0x02

// FEE records addresses
#define FEE_ADCREC_ADDRESS 0x14003FC0u

// FEE macros
#define FEE_xWaitCmdToComplete()	((pstFTFE -> FSTAT) & FTFE_FSTAT_CCIF_MASK) != FTFE_FSTAT_CCIF_MASK

// FEE MDAC error codes
typedef enum
{
	enErrNone,
	enErrPViol,
	enErrOOR,
	enErrAlign,
	enErrContig,
	enErrAccess,
	enErrBusy
} FEE_tenPgmErrCode;

typedef enum
{
	enMarginNormal,
	enMarginUser,
	enMarginFactory
} FEE_tenMarginChoice;

#define FEE_nReg8Set 																																																\
{																																																										\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT3)), (uint8)FEE_PFLASH_PRT3, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT2)), (uint8)FEE_PFLASH_PRT2, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT1)), (uint8)FEE_PFLASH_PRT1, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FPROT0)), (uint8)FEE_PFLASH_PRT0, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FDPROT)), (uint8)FEE_DFLASH_PROT, REGSET_enOverwrite }, 	\
	{ (volatile uint8*)(FTFE_BASE + offsetof(FTFE_Type, FEPROT)), (uint8)FEE_EFLASH_PROT, REGSET_enOverwrite }, 	\
	{ NULL, 0, REGSET_enOverwrite }																		\
}

typedef struct
{
	puint8 pu8WorkingData;
	sint16 s16WorkingDataCount;
} FEE_tstWorkingPage;

typedef struct
{
	COMMONNL_tstRXLargeBuffer* pstSourceBuffer;
	uint8* pu8TargetAddress;
	uint32 u32WriteCount;
	uint32 u32AccumulateCount;
	Bool boProgramming;
	Bool boProgErr;
} FEE_tstWriteControlBlock;

void FEE_vStart(uint32* const);
void FEE_vRun(uint32* const);
void FEE_vTerminate(uint32* const);
void FEE_vStartSBL(void);
Bool FEE_boCheckPartition(void);
Bool FEE_boSetWorkingData(puint8, uint16);
Bool FEE_boNVMWorkingCopy(Bool, Bool);
Bool FEE_boNVMClear(void);
Bool FEE_boPartition(void);
Bool FEE_boWriteControlBlock(COMMONNL_tstRXLargeBuffer* const, 
															 uint8* const, 
															 uint32);
Bool FEE_boUpdateControlBlock(uint32);
Bool FEE_boEraseForDownload(puint8, uint32);
Bool FEE_boWriteNVM(puint8, puint8, uint32);

#endif //FEE_H

