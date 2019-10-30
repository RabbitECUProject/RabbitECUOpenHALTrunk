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
#include "CPUAbstract.h"
#include <string.h>
#include "FEE.h"
#include "FEEHA.h"
#include "FLASHHA.h"
#include "TEPM.h"

tstFTFEConfig FEE_stFlashSSDConfig;

uint32 returnCode;          /* Return code from each SSD function */
uint32 destination;         /* Address of the target location */
uint32 size;
uint32 source;
uint32 protectStatus;       /* Store Protection Status Value of PFLASH */
uint8  protectStatus1;      /* Store Protection Status Value of DFLASH or EEPROM */
uint32 FailAddr;
uint8  FailData;
uint8  marginLevel;
uint16 phraseNumber;        /* Number of phrase to be verified*/
uint8  EEEDataSizeCode;
uint8  DEPartitionCode;
uint32 Sum;
uint32 unsecure_key;
uint32 i;
uint8 u8WriteCount;
uint32 FEE_u32RunCount;

FEE_tstWriteControlBlock FEE_stWriteControlBlock;
FEE_tenPgmErrCode FEE_enPgmErrCode;
FEE_tstWorkingPage FEE_stWorkingPage;

#if	defined(BUILD_SBL)
	uint8 FEE_au8ProgBuff[FEE_PFLASH_SCTR_BYTES];
#else
	uint8 FEE_au8ProgBuff[1];
#endif

const REGSET_tstReg8Val FEEHA_rastFTFEReg8Val[] = FEEHA_nReg8Set;

static Bool FEEHA_boPartitionDFlash(void);

#if defined(BUILD_MK60)
static void FEEHA_vInitFlashConfig(void);
#endif


void FEEHA_vStartSBL(void)
{
#if defined(BUILD_SBL)
	FTFE_Type* pstFTFE = FTFE;	
	
	if (((pstFTFE -> FCNFG) & FTFE_FCNFG_RAMRDY_MASK) != FTFE_FCNFG_RAMRDY_MASK)	
	{
		/* Make FlexRAM available as RAM for programming */
		pSetEEEEnable(&FEE_stFlashSSDConfig, 0xff);
	}
#endif	
}

Bool FEEHA_boCheckPartition(void)
{
	Bool boPartitionOK = false;
	
#ifdef BUILD_MK60
	tstFTFEModule* pstFTFE;	
    pstFTFE = FTFE;

	if (((pstFTFE -> FCNFG) & FTFE_FCNFG_EEERDY_MASK) != FTFE_FCNFG_EEERDY_MASK)
	/* If DFlash not already partitioned */
	{	
		boPartitionOK = FALSE;
	}
	else
	{
		boPartitionOK = TRUE;
	}	
#endif //BUILD_MK60
	
	return boPartitionOK;
}

Bool FEEHA_boNVMWorkingCopy(Bool boNVMToWorking, Bool boCheckCRC16MakeCRC16)
{		
	static uint32 u32LastWriteCount;
	Bool boCopyOK = false;
	puint16 pu16CRC16Computed;
	puint16 pu16CRC16Stored;

#ifdef BUILD_MK60
	tstFTFEModule* pstFTFE;
	boCopyOK = FEEHA_boCheckPartition();
	u32RetCode = FTFx_OK;
    pstFTFE = FTFE;
	uint32 u32NVMWordCount;
	uint32 u32NVMWordIDX;
	uint32 u32SourceWord;
	uint32 u32DestinationWord;
	uint32 u32RetCode;

	if ((TRUE == boCopyOK) && 
			(0 < FEE_stWorkingPage.s16WorkingDataCount) &&
			(NULL != FEE_stWorkingPage.pu8WorkingData))
	{	
		if (TRUE == boNVMToWorking)
		/* Copy NVM page to working page */
		{
			pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, (puint8)FEE_EEPROM_START, 
				FEE_stWorkingPage.s16WorkingDataCount - 2));
			
			pu16CRC16Stored = (puint16)((uint32)FEE_EEPROM_START + 
				(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);
			
			if (TRUE == boCheckCRC16MakeCRC16)
			{
				if (*pu16CRC16Stored == *pu16CRC16Computed)
				{
					memcpy((void*)FEE_stWorkingPage.pu8WorkingData,
						(void*)FEE_EEPROM_START, FEE_stWorkingPage.s16WorkingDataCount);	
					boCopyOK = TRUE;
				}
				else
				{
					boCopyOK = FALSE;
				}
			}
			else
			{
				memcpy((void*)FEE_stWorkingPage.pu8WorkingData,
					(void*)FEE_EEPROM_START, FEE_stWorkingPage.s16WorkingDataCount);	
				boCopyOK = TRUE;
			}	
		}	
		else
		/* Copy working page to NVM page */
		{
			if (TRUE == boCheckCRC16MakeCRC16)
			{
				pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, FEE_stWorkingPage.pu8WorkingData, 
					FEE_stWorkingPage.s16WorkingDataCount - 2));
				
				pu16CRC16Stored = (puint16)((uint32)FEE_stWorkingPage.pu8WorkingData + 
					(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);
				
				/* Copy the computed CRC into the working page */
				*pu16CRC16Stored = *pu16CRC16Computed;
			}
			
			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
			{
				pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
			}
			
			u32NVMWordCount = FEE_stWorkingPage.s16WorkingDataCount / 4;
			u32NVMWordCount = (0 == FEE_stWorkingPage.s16WorkingDataCount % 4) ?
										u32NVMWordCount : u32NVMWordCount + 1;			
			boCopyOK = TRUE;
			
			for (u32NVMWordIDX = 0; u32NVMWordIDX < u32NVMWordCount; u32NVMWordIDX++)
			{				
				u32SourceWord = (uint32)FEE_stWorkingPage.pu8WorkingData + 4 * u32NVMWordIDX;
				u32DestinationWord = (uint32)FEE_EEPROM_START + 4 * u32NVMWordIDX;				
				
				if (*(puint32)u32SourceWord != *(puint32)u32DestinationWord)
				/* Only call write if words are different */
				{
					if (FTFx_OK == u32RetCode)
					{
						u32RetCode = pEEEWrite(&FEE_stFlashSSDConfig, 
													u32DestinationWord,	4, u32SourceWord);
					}
					else
					{
						boCopyOK = false;
					}
				}
			}
		}			
	}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
	if (TRUE == boNVMToWorking)
	/* Copy NVM page to working page */
	{
		if (TRUE == boCheckCRC16MakeCRC16)
		{
			pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, (puint8)FEEHA_EEPROM_START,
			FEE_stWorkingPage.s16WorkingDataCount - 2));
			
			pu16CRC16Stored = (puint16)((uint32)FEEHA_EEPROM_START +
			(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);
				
			if ((NULL != FEE_stWorkingPage.pu8WorkingData) && (*pu16CRC16Stored == *pu16CRC16Computed))
			{
				memcpy(FEE_stWorkingPage.pu8WorkingData, (void*)0x000c0000, FEE_stWorkingPage.s16WorkingDataCount);
				boCopyOK = true;
			}
		}
	}
	else
	/* Copy working page to NVM page */
	{
		if ((NULL != FEE_stWorkingPage.pu8WorkingData) && (10000 < (FEE_u32RunCount - u32LastWriteCount)))
		{
			pu16CRC16Computed = (puint16)(CRC16_pu16CalcCRC(0xffff, FEE_stWorkingPage.pu8WorkingData,
			FEE_stWorkingPage.s16WorkingDataCount - 2));
			
			pu16CRC16Stored = (puint16)((uint32)FEE_stWorkingPage.pu8WorkingData +
			(uint32)FEE_stWorkingPage.s16WorkingDataCount - 2);

			/* Copy the computed CRC into the working page */
			*pu16CRC16Stored = *pu16CRC16Computed;

			uint32_t u32LoopDelay = 0x1800000;

			TEPM_vEnableSequences(FALSE);
			IRQ_vEnableRTOS(FALSE);

			while (0 != u32LoopDelay)
			{
				u32LoopDelay--;
			}

			/* Erase flash */
			flash_erase_all(0x00c0000);

			/* Write working page to flash */
			flash_write(0x00c0000, FEE_stWorkingPage.pu8WorkingData, FEE_stWorkingPage.s16WorkingDataCount , 1u);
			u32LastWriteCount = FEE_u32RunCount;

			u32LoopDelay = ~0;

			while (0 != u32LoopDelay)
			{
				u32LoopDelay--;
			}
		}
	}
#endif //BUILD_SAM3X8E

	return boCopyOK;
}

Bool FEEHA_boWriteNVM(puint8 pu8SourceData, puint8 pu8DestData, uint32 u32DataByteCount)
{
	Bool boCopyOK = TRUE;		
	uint32 u32RetCode;


#ifdef BUILD_MK60
	tstFTFEModule* pstFTFE;	
	uint32 u32SourceWord = (uint32)pu8SourceData;
	uint32 u32DestinationWord = (uint32)pu8DestData;
	tstFTFEModule* pstFTFE;	
    pstFTFE = FTFE;
	u32RetCode = FTFx_OK;

	if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
	{
		pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
	}	
	
	if (TRUE == FEE_boGetSem(TRUE))
	{
		while (4 < u32DataByteCount)
		{
			if (*(puint32)u32SourceWord != *(puint32)u32DestinationWord)
			/* Only call write if words are different */
			{
				if (FTFx_OK == u32RetCode)
				{
					u32RetCode = pEEEWrite(&FEE_stFlashSSDConfig, 
												u32DestinationWord,	4, u32SourceWord);
				}
				else
				{
					boCopyOK = false;
				}
			}
			
			u32DestinationWord += sizeof(uint32);
			u32SourceWord += sizeof(uint32);
			u32DataByteCount -= sizeof(uint32);		
		}
		
		if (0 < u32DataByteCount)
		{
			if (0 != memcmp((void*)u32SourceWord, (void*)u32DestinationWord, u32DataByteCount))
			/* Only call write if bytes are different */
			{
				if (FTFx_OK == u32RetCode)
				{
					u32RetCode = pEEEWrite(&FEE_stFlashSSDConfig, 
												u32DestinationWord,	u32DataByteCount, u32SourceWord);
				}
				else
				{
					boCopyOK = FALSE;
				}
			}
		}

		FEE_boGetSem(FALSE);
	}
	else
	{
		boCopyOK = FALSE;
	}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
	/* Write working page to flash */
	u32RetCode = flash_write(0x00c0000 + (uint32)pu8DestData, (const void*)pu8SourceData, u32DataByteCount, 0u);

	boCopyOK = 0 == u32RetCode;
#endif
	
	return boCopyOK;
}	


Bool FEEHA_boNVMClear(void)
{	
	Bool boClearOK = false;

#ifdef BUILD_MK60
	uint32 u32DestinationWord;
	uint32 u32RetCode;
	uint32 u32NVMWordCount;
	uint32 u32NVMWordIDX;
	uint32 u32SourceWord = 0;
	tstFTFEModule* pstFTFE;	
    pstFTFE = FTFE;
    u32RetCode = FTFx_OK;
		
	boClearOK = FEE_boCheckPartition();
	
	if ((TRUE == boClearOK) && 
			(0 < FEE_stWorkingPage.s16WorkingDataCount) &&
			(NULL != FEE_stWorkingPage.pu8WorkingData))
	{	
		/* Clear NVM page */
		{
			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
			{
				pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
			}
			
			u32NVMWordCount = FEE_stWorkingPage.s16WorkingDataCount / 4;
			u32NVMWordCount = (0 == FEE_stWorkingPage.s16WorkingDataCount % 4) ?
										u32NVMWordCount : u32NVMWordCount + 1;			
			boClearOK = TRUE;
			
			for (u32NVMWordIDX = 0; u32NVMWordIDX < u32NVMWordCount; u32NVMWordIDX++)
			{				
				u32DestinationWord = (uint32)FEE_EEPROM_START + 4 * u32NVMWordIDX;				
				
				if (FTFx_OK == u32RetCode)
				{
					u32RetCode = pEEEWrite(&FEE_stFlashSSDConfig, 
												u32DestinationWord,	4, (uint32)&u32SourceWord);
				}
				else
				{
					boClearOK = false;
				}
			}
		}			
	}
#endif //BUILD_MK60

	return boClearOK;
}

Bool FEEHA_boEraseForDownload(puint8 pu8TargetAddress, uint32 u32EraseCount)
{
	Bool boEraseErr = false;
	
#ifdef BUILD_MK60
	FEE_enPgmErrCode = enErrNone;	
	uint32* pu32SectorWord;	
	puint32 pu32TargetAddress;
	uint16 u16WordCount;
	uint16 u16SectorEraseCount;
	uint32 u32ReturnCode;
	tstFTFEModule* pstFTFE;
    pstFTFE = FTFE;
	if (((pstFTFE -> FSTAT) & FTFE_FSTAT_CCIF_MASK) == FTFE_FSTAT_CCIF_MASK)
	{
		if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
		{
			pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
		}

		if (((pstFTFE -> FSTAT) & FTFE_FSTAT_FPVIOL_MASK) == 0)
		{
			pstFTFE -> FSTAT &= FTFE_FSTAT_FPVIOL_MASK;			
		}	

		if (0 == (uint32)pu8TargetAddress % 4)
		{
			pu32TargetAddress = (puint32)(uint32)pu8TargetAddress;
		}
		else
		{
			pu32TargetAddress = (puint32)0xffffffff;
		}		
			
		/* PFLASH erase */
#if (0 < FEE_PFLASH_START)		
		if (((uint32)pu32TargetAddress >= FEE_PFLASH_START) &&
				(((uint32)pu32TargetAddress + u32EraseCount <= FEE_PFLASH_END)))
#else
		if ((uint32)pu32TargetAddress + u32EraseCount <= FEE_PFLASH_END)
#endif		
		{
			u16WordCount = u32EraseCount / FEE_PFLASH_BYTES_WORD;			
			
			if (((uint32)pu32TargetAddress % FEE_PFLASH_BYTES_WORD) != 0)
			{	
				/* abort on non-word boundary */
				boEraseErr = true;
			}				
			else
			{
				/* calculate target sector count */
				if (u16WordCount > FEE_PFLASH_SCTR_WORDS)
				{
					if ((u16WordCount % FEE_PFLASH_SCTR_WORDS) == 0)
					{
						u16SectorEraseCount = u16WordCount / FEE_PFLASH_SCTR_WORDS;
					}
					else
					{
						u16SectorEraseCount = u16WordCount / FEE_PFLASH_SCTR_WORDS + 1;					
					}
				}
				else
				{
					u16SectorEraseCount = 1;
				}
										
				pu32SectorWord = (uint32*)((uint32)pu32TargetAddress - ((uint32)pu32TargetAddress % FEE_PFLASH_SCTR_WORDS));
				
				while(u16SectorEraseCount-- > 0)
				/* erase any target sectors that are not erased */
				{
					u32ReturnCode = FlashVerifySection(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, u16WordCount, enMarginNormal, pFlashCommandSequence);
					
					if (((pstFTFE -> FSTAT) & FTFE_FSTAT_MGSTAT0_MASK) == FTFE_FSTAT_MGSTAT0_MASK)
					/* erase sector not blank */
					{
						u32ReturnCode = FlashEraseSector(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, FTFx_PSECTOR_SIZE, pFlashCommandSequence);
						boEraseErr = 0 != u32ReturnCode	? true : boEraseErr;	
					}
					
					/* update the sector base pointer to next sector */
					pu32SectorWord = (uint32*)((uint32)pu32SectorWord + FEE_PFLASH_SCTR_BYTES);					
				}	
			}
		}
		/* DFLASH erase */
		else if (((uint32)pu32TargetAddress >= FEE_DFLASH_START) &&
				(((uint32)pu32TargetAddress + u32EraseCount <= FEE_DFLASH_END)))
		{
			u16WordCount = u32EraseCount / FEE_DFLASH_BYTES_WORD;			
			
			if (((uint32)pu32TargetAddress % FEE_DFLASH_BYTES_WORD) != 0)
			{	
				/* abort on non-word boundary */
				boEraseErr = true;
			}				
			else
			{
				/* calculate target sector count */
				if (u16WordCount > FEE_DFLASH_SCTR_WORDS)
				{
					if ((u16WordCount % FEE_DFLASH_SCTR_WORDS) == 0)
					{
						u16SectorEraseCount = u16WordCount / FEE_DFLASH_SCTR_WORDS;
					}
					else
					{
						u16SectorEraseCount = u16WordCount / FEE_DFLASH_SCTR_WORDS + 1;					
					}
				}
				else
				{
					u16SectorEraseCount = 1;
				}
										
				pu32SectorWord = (uint32*)((uint32)pu32TargetAddress - ((uint32)pu32TargetAddress % FEE_DFLASH_SCTR_WORDS));
				
				while(u16SectorEraseCount-- > 0)
				/* erase any target sectors that are not erased */
				{
					u32ReturnCode = FlashVerifySection(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, u16WordCount, enMarginNormal, pFlashCommandSequence);
					
					if (((pstFTFE -> FSTAT) & FTFE_FSTAT_MGSTAT0_MASK) == FTFE_FSTAT_MGSTAT0_MASK)
					/* erase sector not blank */
					{
						u32ReturnCode = FlashEraseSector(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, FTFx_DSECTOR_SIZE, pFlashCommandSequence);
						boEraseErr = 0 != u32ReturnCode	? true : boEraseErr;	
					}				
				}	
			}
		}		
		else
		{
			boEraseErr = true;
		}	
	}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E


#endif //BUILD_SAM3X8E
	
	return boEraseErr;
}

static Bool FEEHA_boPartitionDFlash(void)
{
	Bool boWriteErr = false;
	
#ifdef BUILD_MK60  
	FEE_enPgmErrCode = enErrNone;  
	uint32* pu32SectorWord;
	tstFTFEModule* pstFTFE;
	pstFTFE = FTFE;
	
	if (((pstFTFE -> FSTAT) & FTFE_FSTAT_CCIF_MASK) == FTFE_FSTAT_CCIF_MASK)
	{
		if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
		{
			pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
		}

		if (((pstFTFE -> FSTAT) & FTFE_FSTAT_FPVIOL_MASK) == 0)
		{
			pstFTFE -> FSTAT &= FTFE_FSTAT_FPVIOL_MASK;			
		}					
			
		if (((pstFTFE -> FCNFG) & FTFE_FCNFG_EEERDY_MASK) != FTFE_FCNFG_EEERDY_MASK)
		/* If DFlash not already partitioned */
		{		
			pu32SectorWord = (uint32*)FEE_DFLASH_START;	
			returnCode = FlashVerifyBlock(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, enMarginNormal, pFlashCommandSequence);
					
			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_MGSTAT0_MASK) == FTFE_FSTAT_MGSTAT0_MASK)
			/* Erase sector not blank */
			{
				FEE_boEraseForDownload((puint8)FEE_DFLASH_START, FEE_DFLASH_END - FEE_DFLASH_START + 1);
			}
							
			/* Partition DFlash */
			returnCode = pDEFlashPartition(&FEE_stFlashSSDConfig, 0x00, 0x08, pFlashCommandSequence);

			while(FEE_xWaitCmdToComplete()) {}
				
			//pSetEEEEnable(&FEE_stFlashSSDConfig, 0x00);
		}
	}	
#endif //BUILD_MK60
	
	return boWriteErr;
}

Bool FEEHA_boWriteSector(void)
{
	Bool boWriteErr = false;
	
#ifdef BUILD_MK60
	FEE_enPgmErrCode = enErrNone;
	uint32* pu32ProgramWord;
	uint32* pu32SourceWord;
	uint32* pu32SectorWord;
	uint16 u16WordCount;	
	uint16 u16ByteCount;
	tstFTFEModule* pstFTFE;
	pstFTFE = FTFE;
	if (((pstFTFE -> FSTAT) & FTFE_FSTAT_CCIF_MASK) == FTFE_FSTAT_CCIF_MASK)
	{
		if (((pstFTFE -> FSTAT) & FTFE_FSTAT_ACCERR_MASK) == FTFE_FSTAT_ACCERR_MASK)
		{
			pstFTFE -> FSTAT &= FTFE_FSTAT_ACCERR_MASK;
		}

		if (((pstFTFE -> FSTAT) & FTFE_FSTAT_FPVIOL_MASK) == 0)
		{
			pstFTFE -> FSTAT &= FTFE_FSTAT_FPVIOL_MASK;			
		}		
			
		/* PFLASH program */
#if (0 < FEE_PFLASH_START)		
		if (((uint32)FEE_stWriteControlBlock.pu8TargetAddress >= FEE_PFLASH_START) &&
				(((uint32)FEE_stWriteControlBlock.pu8TargetAddress +
					(FEE_PFLASH_SCTR_BYTES - 1) <= (uint32)FEE_PFLASH_END)))
#else
		if (((uint32)FEE_stWriteControlBlock.pu8TargetAddress +
					(FEE_PFLASH_SCTR_BYTES - 1) <= (uint32)FEE_PFLASH_END))
#endif
		{
			if (FEE_PFLASH_SCTR_BYTES < FEE_stWriteControlBlock.u32WriteCount)
			{
				u16WordCount = FEE_PFLASH_SCTR_BYTES / FEE_PFLASH_BYTES_WORD;
				u16ByteCount = FEE_PFLASH_SCTR_BYTES;
			}
			else
			{
				if (0 == (FEE_stWriteControlBlock.u32WriteCount % FEE_PFLASH_BYTES_WORD))
				{
					u16WordCount = FEE_stWriteControlBlock.u32WriteCount / FEE_PFLASH_BYTES_WORD;
				}
				else
				{
					u16WordCount = (FEE_stWriteControlBlock.u32WriteCount / FEE_PFLASH_BYTES_WORD) + 1;
				}
				
				u16ByteCount = u16WordCount * FEE_PFLASH_BYTES_WORD;
			}
			
			pu32ProgramWord = (uint32*)FEE_stWriteControlBlock.pu8TargetAddress;
			pu32SourceWord = (uint32*)&FEE_au8ProgBuff[0];
			
			if (((uint32)pu32ProgramWord % FEE_PFLASH_BYTES_WORD) != 0)
			{	
				/* abort on non-word boundary */
				u16WordCount = 0;
			}
			
			pu32SectorWord = (uint32*)((uint32)pu32ProgramWord - ((uint32)pu32ProgramWord % FEE_PFLASH_SCTR_WORDS));
			returnCode = FlashVerifySection(&FEE_stFlashSSDConfig, (uint32)pu32SectorWord, u16WordCount, enMarginNormal, pFlashCommandSequence);
				
			if (((pstFTFE -> FSTAT) & FTFE_FSTAT_MGSTAT0_MASK) == FTFE_FSTAT_MGSTAT0_MASK)
			/* erase sector not blank */
			{
				boWriteErr = true;
				u16WordCount = 0;
				u8WriteCount = 0x34;
			}
						
			/* program PFLASH words */
			if (u16WordCount > 0)
			{
				/* copy large diag buffer to RAM accelaration buffer */
				memcpy((void*)EERAM_BLOCK_BASE, (void*)pu32SourceWord, (uint32)u16ByteCount);

				/* program the values to PFLASH */
				if (0 == (u16ByteCount % FEE_PFLASH_BYTES_PHRS))
				{
					phraseNumber = u16ByteCount / FEE_PFLASH_BYTES_PHRS;
				}
				else
				{
					phraseNumber = u16ByteCount / FEE_PFLASH_BYTES_PHRS + 1;					
				}
				
				destination = (uint32)pu32ProgramWord;
				returnCode = pFlashProgramSection(&FEE_stFlashSSDConfig, destination, phraseNumber, pFlashCommandSequence);
				if (FTFx_OK != returnCode)
				{
						u16WordCount = 0;
				}
				
				u8WriteCount++;

				/* update write address and decrement write count */
				FEE_stWriteControlBlock.pu8TargetAddress += FEE_PFLASH_SCTR_BYTES;
				
				if (FEE_PFLASH_SCTR_BYTES < FEE_stWriteControlBlock.u32WriteCount)
				{
					FEE_stWriteControlBlock.u32WriteCount -= FEE_PFLASH_SCTR_BYTES;						
				}
				else
				{
					FEE_stWriteControlBlock.u32WriteCount = 0;
					FEE_stWriteControlBlock.boProgramming = false;				
				}
			}
		}
		else
		{
			boWriteErr = true;
			u8WriteCount = 0x55;
		}
		
		while(FEE_xWaitCmdToComplete()) {}
	}	
#endif //BUILD_MK60
	
	return boWriteErr;
}


Bool FEEHA_boStart(uint32* const u32Stat)
{
    Bool boStartOK = false;

#ifdef BUILD_MK60
	u8WriteCount = 0;
	unsecure_key = 0xFFFFFFFE;

	REGSET_vInitReg8(&FEEHA_rastFTFEReg8Val[0]);		
	FEEHA_vInitFlashConfig();

	returnCode = pFlashInit(&FEE_stFlashSSDConfig);
	
	if (FTFx_OK == returnCode)
	{
		FEE_stWriteControlBlock.boProgramming = false;
		FEE_stWriteControlBlock.boProgErr = false;
		boStartOK = true;
	}
#endif  //BUILD_MK60

#ifdef BUILD_SAM3X8E
    uint32 u32Err;

    u32Err = flash_init(128u, 10u);

	if (FLASH_RC_OK == u32Err)
	{
		FEE_stWriteControlBlock.boProgramming = false;
		FEE_stWriteControlBlock.boProgErr = false;
		boStartOK = true;
	}
#endif

    return boStartOK;
}

void FEEHA_vRun(uint32* const u32Stat)
{	
	FEE_u32RunCount++;
}

void FEEHA_vTerminate(uint32* const u32Stat)
{

}

Bool FEEHA_boPartition(void)
{
	Bool boRetVal;
	
	boRetVal = FEEHA_boPartitionDFlash();
	
	return boRetVal;
}

#ifdef BUILD_MK60
static void FEEHA_vInitFlashConfig(void)
{

	FEE_stFlashSSDConfig.ftfxRegBase = FTFx_REG_BASE;
	FEE_stFlashSSDConfig.PFlashBlockBase = PFLASH_BLOCK_BASE;
	FEE_stFlashSSDConfig.PFlashBlockSize = PBLOCK_SIZE;
	FEE_stFlashSSDConfig.DFlashBlockBase = DEFLASH_BLOCK_BASE;
	FEE_stFlashSSDConfig.DFlashBlockSize = 0;
	FEE_stFlashSSDConfig.EERAMBlockBase = EERAM_BLOCK_BASE;
	FEE_stFlashSSDConfig.EERAMBlockSize = EERAM_BLOCK_SIZE;
	FEE_stFlashSSDConfig.EEEBlockSize = 0;
	FEE_stFlashSSDConfig.DebugEnable = DEBUGENABLE;
	FEE_stFlashSSDConfig.CallBack = NULL_CALLBACK;
}
#endif //BUILD_MK60



