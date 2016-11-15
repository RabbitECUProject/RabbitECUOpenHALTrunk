/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      SDHC                                                   */
/* DESCRIPTION:        This code provides setup and functions for the SDHC    */
/*                     hardware module                                        */	
/* FILE NAME:          SDHC.c                                                 */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "types.h"
#include "CPUAbstract.h"
#include "regset.h"
#include "declarations.h"
#include "OS.h"
#include "disk.h"
#include "sdhc.h"
#include "SDHCHA.h"
#include "timer.h"
#include "sim.h"

#ifdef BUILD_MK60
#include "sdhc1.h"
#endif //BUILD_MK60
		
const REGSET_tstReg32Val SDHCHA_rastSDHCReg32HWInitVal[] = SDHCHA_nReg32HWInitSet; 
const REGSET_tstReg32Val SDHCHA_rastSDHCReg32HWDeInitVal[] = SDHCHA_nReg32HWDeInitSet; 
Bool SDHCHA_boTimeout;
uint8 SDHCHA_au8SectorBuffer[SDHCHA_nSectorSize];

SDHCHA_tstSDData stSDData;

#ifdef BUILD_MK60
static void SDHC_vWait(LDD_SDHC_TSDData*, Bool*);
#endif //BUILD_MK60

void SDHCHA_vStart(uint32* const u32Stat)
{
	//REGSET_vInitReg32(&SDHC_rastSDHCReg32HWInitVal[0]);	
	//stSDData.SDHCPtr = SDHC1_Init(NULL);
	OS_xModuleStartOK(*u32Stat);
	//SDHCHA_vCardInitialise();
}

void SDHCHA_vRun(uint32* const u32Stat)
{
#ifdef BUILD_MK60
	SDHCHA_tstBufferDesc stBufferDesc;
	static uint32 u32Sector = 0x10a00;
	
	stBufferDesc.DataPtr = (uint8*)&SDHC_au8SectorBuffer;
	stBufferDesc.Size = SDHCHA_nSectorSize;
	
	//enError = SDHC1_TransferBlocks(stSDData.SDHCPtr, SDHC_nTransferReadOperation, u32Sector, &stBufferDesc, 1);
	
	if(0 == SDHC_au8SectorBuffer[511])
	{
		u32Sector += 0x200;
	}
#endif //BUILD_MK60	
	/* TODO suppress warning */
	//(void)stBufferDesc;
}

void SDHCHA_vTerminate(uint32* const u32Stat)
{

}

void SDHCHA_vHWInit(void)
{
	REGSET_vInitReg32(&SDHCHA_rastSDHCReg32HWInitVal[0]);	
}

void SDHCHA_vHWDeInit(void)
{
	REGSET_vInitReg32(&SDHCHA_rastSDHCReg32HWDeInitVal[0]);	
}

uint8 SDHCHA_u8DiskRead(uint8* const pu8Buf, const uint32 u32Sector, const uint16 u16SectorCount)
{
#ifdef BUILD_MK60
	SDHCHA_tenError enError;
	SDHCHA_tstBufferDesc stBufferDesc;
	
	stBufferDesc.DataPtr = pu8Buf;
	stBufferDesc.Size = SDHCHA_nSectorSize * u16SectorCount;	
	
	enError = SDHC1_TransferBlocks(stSDData.SDHCPtr, SDHCHA_nTransferReadOperation, u32Sector, &stBufferDesc, 1);

	while(LDD_SDHC_TRANSFER == stSDData.SDHCPtr->State){};
#endif //BUILD_MK60
		
	/* TODO suppress warning */
	//(void)enError;
	return 0;
}

uint8 SDHCHA_u8DiskWrite(uint8* const pu8Buf, const uint32 u32Sector, const uint16 u16SectorCount)
{
	return 0;
}

#ifdef BUILD_MK60
static void SDHCHA_vWait(LDD_SDHC_TSDData* pstSDData, Bool* pboErrorFlag)
{
  LDD_SDHC_TError enError;    

	(void)TIMER_vConfigureAlarm(SDHC_nWaitTimeoutMs, SHDC_vWaitTimeoutCB);	

  while(!pstSDData->Finished && !SDHC_boTimeout);

  pstSDData->Finished = FALSE;

  enError = SDHC1_GetError(pstSDData->SDHCPtr, NULL);
	
  if (enError != LDD_SDHC_ERR_OK) {
    //SD_ErrorHandler(SD, Error);
    *pboErrorFlag = TRUE;
  }
}
#endif //BUILD_MK60

void SHDC_vWaitTimeoutCB(void)
{
	SDHCHA_boTimeout = true;
}

static void SDHCHA_vCardInitialise(void)
{
#ifdef BUILD_MK60
  Bool boError = FALSE;
	MPU_Type* pstMPU = MPU;

  /* disable memory protection - DMA is used in SD Card init */
	pstMPU -> CESR = 0;	
	
	//MATTHEW
	stSDData.Inserted = true;
	
	/* turn on DMAMUX clock */
	SIM_vSetReg32(SIM_SCGC6, SIM_SCGC6_DMAMUX0_MASK);	
	
  if(!boError && stSDData.Inserted) 
	{
//     if(NULL != stSDData.SDHCPtr) 
// 		{
// 			SDHC1_Deinit(stSDData.SDHCPtr);
// 			stSDData.SDHCPtr = SDHC1_Init(NULL);
// 			SDHC_vWait(&stSDData, &boError);
// 		}
// 		else
		{
			stSDData.SDHCPtr = SDHC1_Init(&stSDData);
			SDHC_vWait(&stSDData, &boError);
		}
		
    if(SDHC1_DetectCards(stSDData.SDHCPtr) != ERR_OK) 
		{
      boError = TRUE;
    }
		
    SDHC_vWait(&stSDData, &boError);
		
    if (!boError && stSDData.CardId != SDHC1_NO_CARD) 
		{
      /* card detected - selecting card... */
      if (SDHC1_SelectCard(stSDData.SDHCPtr, stSDData.CardId) != ERR_OK) 
			{
        boError = TRUE;
      }
      SDHC_vWait(&stSDData, &boError);
    }
		
    if (!boError) 
		{
			/* card selected - requesting card info... */
			if (SDHC1_GetCardInfo(stSDData.SDHCPtr, &stSDData.CardInfo) != ERR_OK) 
			{
				boError = TRUE;
			}
			SDHC_vWait(&stSDData, &boError);
		}

    if (!boError) 
		{
			/* card info obtained - set faster bus speed */
			if (SDHC1_SelectBusClock(stSDData.SDHCPtr, SDHC_tenSDHCBusClock12_5MHz) != ERR_OK) 
			{
				boError = TRUE;
			}
			SDHC_vWait(&stSDData, &boError);
			}
		}
    if (!boError) {
      if (stSDData.CardInfo.Caps.DataWidths&SDHC_nCardDataWidth8Bit) {
        boError = SDHC1_SetDataWidth(stSDData.SDHCPtr, SDHC_nCardDataWidth8Bit)!=ERR_OK;
      } else if (stSDData.CardInfo.Caps.DataWidths&SDHC_nCardDataWidth4Bit) {
        boError = SDHC1_SetDataWidth(stSDData.SDHCPtr, SDHC_nCardDataWidth4Bit)!=ERR_OK;
      } else if (stSDData.CardInfo.Caps.DataWidths&SDHC_nCardDataWidth1Bit) {
        boError = SDHC1_SetDataWidth(stSDData.SDHCPtr, SDHC_nCardDataWidth1Bit)!=ERR_OK;
      }
      SDHC_vWait(&stSDData, &boError);
      /* selected highest possible data width */
    }	
#endif //BUILD_MK60		
}


