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
#include "FEE.h"
#include "FEEHA.h"

FEE_tstWriteControlBlock FEE_stWriteControlBlock;
FEE_tenPgmErrCode FEE_enPgmErrCode;
FEE_tstWorkingPage FEE_stWorkingPage;

#if	BUILD_SBL
	uint8 FEE_au8ProgBuff[FEE_PFLASH_SCTR_BYTES];
#else
	uint8 FEE_au8ProgBuff[1];
#endif


void FEE_vStartSBL(void)
{
#if BUILD_SBL
    FEEHA_vStartSBL();
#endif	
}

Bool FEE_boCheckPartition(void)
{
	Bool boRetVal = false;

#ifdef BUILD_MK60
	return FEEHA_boCheckPartition();
#endif

#ifdef BUILD_SAM3X8E
    boRetVal = true;
#endif //BUILD_SAM3X8E			

    return boRetVal;
}

Bool FEE_boSetWorkingData(puint8 pu8WorkingAddress, uint16 u16WorkingDataCount)
{
	Bool boWorkingDataOK = FALSE;
	
	if (((puint8)FEEHA_WORK_DATA_START <= pu8WorkingAddress) &&
			((puint8)FEEHA_WORK_DATA_END >= pu8WorkingAddress) &&
			((uint16)FEEHA_WORK_DATA_MAX > u16WorkingDataCount) &&
			(2 <= u16WorkingDataCount))
	{
		FEE_stWorkingPage.pu8WorkingData = pu8WorkingAddress;
		FEE_stWorkingPage.s16WorkingDataCount = (sint16)u16WorkingDataCount;		
		boWorkingDataOK = TRUE;		
	}
		
	return boWorkingDataOK;
}

Bool FEE_boNVMWorkingCopy(Bool boNVMToWorking, Bool boCheckCRC16MakeCRC16)
{
	Bool boCopyOK;

	boCopyOK = FEEHA_boNVMWorkingCopy(boNVMToWorking, boCheckCRC16MakeCRC16);

	return boCopyOK;
}

Bool FEE_boWriteNVM(puint8 pu8SourceData, puint8 pu8DestData, uint32 u32DataByteCount)
{
	Bool boCopyOK = TRUE;		
	
	boCopyOK = FEEHA_boWriteNVM(pu8SourceData, pu8DestData, u32DataByteCount);
	
	return boCopyOK;
}	


Bool FEE_boNVMClear(void)
{	
	Bool boClearOK;

	boClearOK = FEEHA_boNVMClear();

	return boClearOK;
}

Bool FEE_boEraseForDownload(puint8 pu8TargetAddress, uint32 u32EraseCount)
{
	Bool boEraseErr = false;
	
	boEraseErr = FEEHA_boEraseForDownload(pu8TargetAddress, u32EraseCount);
	
	return boEraseErr;
}


void FEE_vStart(uint32* const pu32Stat)
{
    Bool boStartOK;

	boStartOK = FEEHA_boStart(pu32Stat);
	
	if (false == boStartOK)
	{
		*pu32Stat = false;
	}	
	else
	{	
		FEE_stWriteControlBlock.boProgramming = false;	
		FEE_stWriteControlBlock.boProgErr = false;	
		*pu32Stat = true;
	}
	
	FEE_stWorkingPage.pu8WorkingData = NULL;
	FEE_stWorkingPage.s16WorkingDataCount = -1;

	OS_xModuleStartOK(*pu32Stat);
}

void FEE_vRun(uint32* const u32Stat)
{	
    FEEHA_vRun(u32Stat);
}


void FEE_vTerminate(uint32* const u32Stat)
{

}

Bool FEE_boPartition(void)
{
	Bool boRetVal;
	
	boRetVal = FEEHA_boPartition();
	
	return boRetVal;
}

Bool FEE_boWriteControlBlock(COMMONNL_tstRXLargeBuffer* const pstParamSourceBuffer, 
															 uint8* const pu8ParamTargetAddress, 
															 uint32 u32ParamWriteCount)
{
	if ((false == FEE_stWriteControlBlock.boProgramming) &&
			(false == FEE_stWriteControlBlock.boProgErr))
	{
		FEE_stWriteControlBlock.pstSourceBuffer = pstParamSourceBuffer;
		FEE_stWriteControlBlock.pu8TargetAddress = pu8ParamTargetAddress;
		FEE_stWriteControlBlock.u32WriteCount = u32ParamWriteCount;
		FEE_stWriteControlBlock.u32AccumulateCount = 0;		
		FEE_stWriteControlBlock.boProgramming = true;
	}
	else
	{
		FEE_stWriteControlBlock.boProgramming = false;
	}
	
	return FEE_stWriteControlBlock.boProgramming;
}

Bool FEE_boUpdateControlBlock(uint32 u32BlockWriteCount)
{		
	puint8 pu8SourceData;
	puint8 pu8BufferData;
	
	if (FEE_stWriteControlBlock.boProgramming == true)
	{
		if ((FEEHA_PFLASH_SCTR_BYTES >= 
				(u32BlockWriteCount + FEE_stWriteControlBlock.u32AccumulateCount)) &&
				(0 != u32BlockWriteCount))
		{			
			pu8SourceData = (puint8)&FEE_stWriteControlBlock.pstSourceBuffer->u8Data[0];
			pu8BufferData = (puint8)((uint32)&FEE_au8ProgBuff + FEE_stWriteControlBlock.u32AccumulateCount);
			
			memcpy((void*)pu8BufferData, (void*)pu8SourceData, u32BlockWriteCount);				
		
			FEE_stWriteControlBlock.u32AccumulateCount += u32BlockWriteCount;
			
			if (FEEHA_PFLASH_SCTR_BYTES == FEE_stWriteControlBlock.u32AccumulateCount)
			{
				FEE_stWriteControlBlock.u32AccumulateCount = 0;
				FEEHA_boWriteSector();
			}
			else if (FEEHA_PFLASH_SCTR_BYTES < FEE_stWriteControlBlock.u32AccumulateCount)
			{
				/* Uh oh too many bytes received - ran past the sector size */
				/* matthew deal with this */
				FEE_stWriteControlBlock.boProgramming = false;	
			}					
			else if (FEE_stWriteControlBlock.u32WriteCount <= FEE_stWriteControlBlock.u32AccumulateCount)
			{
				FEE_stWriteControlBlock.u32AccumulateCount = 0;
				FEEHA_boWriteSector();				
			}
		}
		else
		{
			FEE_stWriteControlBlock.boProgramming = false;
		}
	}
	
	return FEE_stWriteControlBlock.boProgramming;	
}
