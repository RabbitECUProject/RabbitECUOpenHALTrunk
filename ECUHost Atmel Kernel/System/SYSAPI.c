/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                               							*/
/* PACKAGE TITLE:      SYSAPI                     														*/
/* DESCRIPTION:        This code provides functions for running system calls	*/
/*										 																												*/
/* FILE NAME:          SYSAPI.c                                             	*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include "sys.h"

#include "ADC.h"
#include "ADCAPI.h"
#include "CEM.h"
#include "CLIENT.h"
#include "CQUEUE.h"
#include "CRC16.h"
#include "CTRL.h"
#include "CTRLAPI.h"
#include "DAC.h"
#include "DACAPI.h"
#include "DLL.h"
#include "DIAGAPI.h"
#include "FEE.h"
#include "IO.h"
#include "IOAPI.h"
#include "KERNELDIAG.h"
#include "MSG.h"
#include "os.h"
#include "PROTAPI.h"
#include "RESM.h"
#include "SPREADS.h"
#include "SPREADSAPI.h"
#include "SRLTFR.h"
#include "SYSAPI.h"
#include "TABLES.h"
#include "TABLESAPI.h"
#include "TEPM.h"
#include "types.h"

void SYS_vAPISVC(void* svc_args)
{
	puint32 pu32Arg = (puint32)svc_args;
	uint32 u32Temp;
	puint16 pu16Temp;
	CTRLAPI_ttPIDIDX PIDIDX;
	SPREADAPI_ttSpreadIDX tSpreadIDX;	
	TABLEAPI_ttTableIDX tTableIDX;		
	bool boResult;
	
	switch((SYSAPI_tenSVCID)*pu32Arg)
	{
		case SYSAPI_enSetClientStartTask:
		{	
			OS_stSVCDataStruct.enSVCResult =			
			CLIENT_enAddTask(OS_enCLIENTQueueStart, (SYSAPI_tpfUserTaskFunction)*(puint32)*(pu32Arg + 1), (TASKAPI_tenPriority)*(puint32)*(pu32Arg + 2), (TASKAPI_tenRateMs)*(puint32)*(pu32Arg + 3));						
			break;
		}
		
		case SYSAPI_enSetClientCyclicTask:
		{	
			OS_stSVCDataStruct.enSVCResult =			
			CLIENT_enAddTask(OS_enCLIENTQueueCyclic, (SYSAPI_tpfUserTaskFunction)*(puint32)*(pu32Arg + 1), (TASKAPI_tenPriority)*(puint32)*(pu32Arg + 2), (TASKAPI_tenRateMs)*(puint32)*(pu32Arg + 3));						
			break;
		}		
		
		case SYSAPI_enSetClientTerminateTask:
		{	
			OS_stSVCDataStruct.enSVCResult =
			CLIENT_enAddTask(OS_enCLIENTQueueTerminate, (SYSAPI_tpfUserTaskFunction)*(puint32)*(pu32Arg + 1), (TASKAPI_tenPriority)*(puint32)*(pu32Arg + 2), (TASKAPI_tenRateMs)*(puint32)*(pu32Arg + 3));								
			break;
		}		

		case SYSAPI_enRequestIOResource:
		{						
			u32Temp = (SYSAPI_ttClientHandle)RESM_RequestEHIOResource((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), OS_stSVCDataStruct.tClientHandle);			
			OS_stSVCDataStruct.enSVCResult = (u32Temp == OS_stSVCDataStruct.tClientHandle) ? SYSAPI_enOK : SYSAPI_enFail;
			break;
		}		
		
		case SYSAPI_enInitialiseIOResource:
		{				
			if (IOAPI_enDIO == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2))
			{
				IO_vInitDIOResource((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2), (IOAPI_tenDriveStrength)*(puint32)*(pu32Arg + 3));
			}
			
			else if ((IOAPI_enADSE == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2)) || (IOAPI_enADD == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2)))
			{
				IO_vInitADCResource((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2), (ADCAPI_tstADCCB*)*(pu32Arg + 3));
			}	
			
			else if (IOAPI_enDAC == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2))
			{
				IO_vInitDACResource((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2), (DACAPI_tstDACCB*)*(pu32Arg + 3));
			}				
			
			else if ((IOAPI_enIICBus == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2)) 
								|| (IOAPI_enUARTBus == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2)) 
								|| (IOAPI_enCANBus == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2)) 			
								|| (IOAPI_enENETChannel == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2)))
			{
				OS_stSVCDataStruct.enSVCResult = IO_enInitCommsResource((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (IOAPI_tstPortConfigCB*)*(pu32Arg + 3));			
			}
			
			else if (IOAPI_enTEPM == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2))
			{
				OS_stSVCDataStruct.enSVCResult = TEPM_vInitTEPMResource((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (TEPMAPI_tstTEPMResourceCB*)*(pu32Arg + 3));
			}
			
			else if (IOAPI_enCaptureCompare == (IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2))
			{
				OS_stSVCDataStruct.enSVCResult = IO_enInitTEPMChannel((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (TEPMAPI_tstTEPMChannelCB*)*(pu32Arg + 3));
			}						
			
			break;			
		}
		
		case SYSAPI_enInitialiseCTRLResource:
		{
			if (CTRLAPI_enPID == (CTRLAPI_tenCTRLType)*(puint32)*(pu32Arg + 1))
			{
				PIDIDX = CTRL_tRequestKernelPIDController((CTRLAPI_tstPIDCB*)*(pu32Arg + 2));
				OS_stSVCDataStruct.tClientHandle = (uint32)PIDIDX;
				OS_stSVCDataStruct.enSVCResult = (-1 < PIDIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
				break;
			}
		}
		
		case SYSAPI_enIterateCTRLResource:
		{
			if (CTRLAPI_enPID == (CTRLAPI_tenCTRLType)*(puint32)*(pu32Arg + 1))
			{
				PIDIDX = (CTRLAPI_tenCTRLType)*(puint32)*(pu32Arg + 2);
				CTRL_vIteratePID(PIDIDX);
				OS_stSVCDataStruct.enSVCResult = (-1 < PIDIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
				break;
			}
		}		
		
		case SYSAPI_enInitialiseSpreadResource:
		{
			tSpreadIDX = SPREAD_tRequestKernelSpread((SPREADAPI_tstSpreadCB*)*(pu32Arg + 1));
			OS_stSVCDataStruct.tClientHandle = (uint32)tSpreadIDX;
			OS_stSVCDataStruct.enSVCResult = (-1 < tSpreadIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
			break;
		}		
		
		case SYSAPI_enInitialiseTableResource:
		{
			tTableIDX = TABLE_tRequestKernelTable((TABLEAPI_tstTableCB*)*(pu32Arg + 1));
			OS_stSVCDataStruct.tClientHandle = (uint32)tTableIDX;
			OS_stSVCDataStruct.enSVCResult = (-1 < tSpreadIDX) ? SYSAPI_enOK : SYSAPI_enResourceUnavailable;
			break;
		}	
		
		case SYSAPI_enAssertDIOResource:
		{
			IO_vAssertDIOResource((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (IOAPI_tenTriState)*(puint32)*(pu32Arg + 2));			
			break;
		}
		
		case SYSAPI_enSetSVCDataPointer:
		{
			*(puint32)*(pu32Arg + 1) = (uint32)&OS_stSVCDataStruct;
			break;
		}
		
		case SYSAPI_enTriggerADQueue:
		{
			boResult = ADC_vTriggerQueue((ADCAPI_tenTrigger)*(puint32)*(pu32Arg + 1));
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}
		
		case SYSAPI_enRequestIOBusTransfer:
		{
			switch ((IOAPI_tenEHIOType)*(puint32)*(pu32Arg + 2))
			{				
				case EH_VIO_IIC1:
				case EH_VIO_IIC2:
				{				
					OS_stSVCDataStruct.enSVCResult = SRLTFR_enEnqueue((IOAPI_tstTransferCB*)*(pu32Arg + 2));
					break;
				}
				default:
				{
					OS_stSVCDataStruct.enSVCResult = SYSAPI_enBadResource;
					break;
				}
			}
			break;
		}	

		case SYSAPI_enAppendTEPMQueue:
		{
			TEPM_vAppendTEPMQueue((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (TEPMAPI_tstTimedUserEvent*)*(pu32Arg + 2), (TEPMAPI_ttEventCount)*(puint32)*(pu32Arg + 3));	
			break;
		}
		
		case SYSAPI_enConfigureUserTEPMInput:
		{
			TEPM_vConfigureUserTEPMInput((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (TEPMAPI_tstTimedUserEvent*)*(pu32Arg + 2));	
			break;
		}			
		
		case SYSAPI_enConfigureKernelTEPMOutput:
		{
			TEPM_vConfigureKernelTEPMOutput((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (TEPMAPI_tstTimedKernelEvent*)*(pu32Arg + 2), (TEPMAPI_ttEventCount)*(puint32)*(pu32Arg + 3));	
			break;
		}				
		
		case SYSAPI_enWriteDACQueue:
		{
			DAC_vWriteDACQueue((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (DACAPI_ttOutputVoltage*)*(pu32Arg + 2));	
			break;
		}
		
		case SYSAPI_enCalculateSpread:
		{
			boResult = SPREAD_vCalculate((SPREADAPI_ttSpreadIDX)*(puint32)*(pu32Arg + 1));
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}
		
		case SYSAPI_enCalculateTable:
		{
			boResult = TABLE_vCalculate((TABLEAPI_ttTableIDX)*(puint32)*(pu32Arg + 1));
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}		
		
		case SYSAPI_enSetDiagCallback:
		{
			DIAG_vSetCallBack((DIAGAPI_tenCallBackType)*(puint32)*(pu32Arg + 1), (void*)(uint32*)*(pu32Arg + 2));
			break;
		}	
		
		case SYSAPI_enGetTimerValue:
		{
			if ((EH_FIRST_TMR <= (IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1)) &&
			(EH_LAST_TMR >= (IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1)))
			{
				TEPM_u32GetTimerVal((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (puint32)*(pu32Arg + 2));
			}		
			break;			
		}
		
		case SYSAPI_enGetCRC16:
		{
			pu16Temp = (puint16)CRC16_pu16CalcCRC((uint16)*(puint32)*(pu32Arg + 1), (puint8)*(puint32)*(pu32Arg + 2), (uint16)*(puint32)*(pu32Arg + 3));
			OS_stSVCDataStruct.pvData = (void*)pu16Temp;
			OS_stSVCDataStruct.enSVCResult = SYSAPI_enOK;
			break;
		}		
		
		case SYSAPI_enCheckPartition:
		{
			boResult = FEE_boCheckPartition();
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadPartition;
			break;
		}	

		case SYSAPI_enNVMWorkingCopy:
		{
			boResult = FEE_boNVMWorkingCopy((bool)*(puint32)*(pu32Arg + 1), (bool)*(puint32)*(pu32Arg + 2));
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadNVMWorkingCopy;			
			break;
		}		
		
		case SYSAPI_enSetupWorkingPage:
		{		
			boResult = FEE_boSetWorkingData((puint8)*(puint32)*(pu32Arg + 1), (uint16)*(puint32)*(pu32Arg + 2));	
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;
			break;
		}
		
		case SYSAPI_enQueueCANMessage:
		{
			boResult = DLL_vQueueCANMessage((IOAPI_tenEHIOResource)*(puint32)*(pu32Arg + 1), (PROTAPI_tstCANMsg*)*(pu32Arg + 2));	
			break;
		}		

		case SYSAPI_enSetupCrankTriggerEdgePattern:
		{
			boResult = CEM_boPopulateCrankEdgeArrays((puint16)*(pu32Arg + 1), (bool)*(puint32)*(pu32Arg + 2), (uint8)*(puint32)*(pu32Arg + 3));
			OS_stSVCDataStruct.enSVCResult = (TRUE == boResult) ? SYSAPI_enOK : SYSAPI_enBadArgument;			
			break;
		}
	}
}




