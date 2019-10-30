/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Diagnostics                                            */
/* DESCRIPTION:        This code module initialises the required comms        */
/*                     resources for and provides functions for diagnostics   */
/* FILE NAME:          DIAG.c                                                 */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#define _USERDIAG_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "diag.h"

/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
const USERDIAG_tstIdentifierIB USERDIAG_rastIdentifierIB[] = USER_DIAG_nCIDInfo;
DIAGAPI_tenDiagSecurityLevel USERDIAG_enSecLevel;
uint32 USERDIAG_u32GlobalTimeTick;
//ASAM mode=readvalue name="DIAGGlobalTimeTick" type=uint32 offset=0 min=0 max=10000 m=1 b=0 units="dl" format=6.0 help="Data Logging Global Time Tick"   
puint8 USERDIAG_pu8Mode1Data;
puint8 USERDIAG_pu8Mode2Data;
puint8 USERDIAG_pu8Mode3Data;
puint8 USERDIAG_pu8Mode4Data;
puint8 USERDIAG_pu8Mode6Data;
puint8 USERDIAG_pu8Mode7Data;
uint16 USERDIAG_rau16Mode1DataOffsets[MODE1_PIDS_COUNT] = USERDIAG_nMode1PIDOffsets;

/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : DIAG_vNTOH
*
* Implementation   : Network to Host data endianness conversion
*
* Parameter
*    Par1          : Pointer to the data
*    Par2          : Datacount
*    Par3          : Datasize in bytes
*
* Return Value     : Nil
*******************************************************************************/
static void	DIAG_vNTOH(puint8 pu8Data, uint8 u8DataCount, uint8 u8DataSize);
static SPREADAPI_ttSpreadIDX USERDIAG_stGetSpreadIDX(puint8 pu8Data);

/*******************************************************************************
* Interface        : USERDIAG_i16GetCIDParamAddress
*
* Implementation   : Serach for the index of a CID within a CID info table
*
* Parameter
*    Par1          : The CID being searched for
*
* Return Value     : Index of the found data, -1 for failed to find
*******************************************************************************/
static sint16 USERDIAG_i16GetCIDParamAddress(uint16);


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void USERDIAG_vStart(puint32 const pu32Arg)
{
	IOAPI_tenEHIOType enEHIOType;	
	IOAPI_tenEHIOResource enEHIOResource;
	IOAPI_tstPortConfigCB stPortConfigCB;	
	DIAGAPI_tenCallBackType enCallBackType;
	uint32 u32DiagSpreadsIDX;
	
	USERDIAG_u32GlobalTimeTick = 0;

	for (u32DiagSpreadsIDX = 0; u32DiagSpreadsIDX < DIAG_nSPREADSRECORDSMAX; u32DiagSpreadsIDX++)
	{
		DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].pData = NULL;
		DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].tSpreadIDX = ~0;
	}

	for (u32DiagSpreadsIDX = 0; u32DiagSpreadsIDX < DIAG_nSPREADSMAX; u32DiagSpreadsIDX++)
	{
		DIAG_apu8SpreadTableAddresses[DIAG_nSPREADSMAX] = NULL;
	}
	
		
#if defined(USERDIAG_nEnableUART)
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{		
		/* Setup the USERDIAG UART resources */
		enEHIOResource = EH_VIO_UART2;	
		
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	NULL, NULL);
	
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{	
			stPortConfigCB.enPortMode = IOAPI_enPortComms;
			stPortConfigCB.u32BaudRateHz = USERDIAG_nUARTFrequency;
			stPortConfigCB.enVIOResource = EH_VIO_UART2;
			stPortConfigCB.stPinConfig.uPinInfo.stUARTPinInfo.enRXPin = EH_IO_TMR14;
			stPortConfigCB.stPinConfig.uPinInfo.stUARTPinInfo.enTXPin = EH_IO_TMR13;
			stPortConfigCB.stPinConfig.uPinInfo.stUARTPinInfo.enRTSPin = EH_IO_Invalid;
			stPortConfigCB.stPinConfig.uPinInfo.stUARTPinInfo.enCTSPin = EH_IO_Invalid;		
			stPortConfigCB.enLLProtocol = PROT_enLLUARTPacked;
			stPortConfigCB.enNLProtocol = PROT_enISO15765;			
			stPortConfigCB.enTLProtocol = PROT_enTLUDS;
			stPortConfigCB.pfRXTransferCB = NULL;
			stPortConfigCB.pfTXTransferCB = NULL;	
			enEHIOType = IOAPI_enUARTBus;
			
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stPortConfigCB);	
			
			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{	
				enCallBackType = DIAGAPI_enDataWriteCB;
				USER_vSVC(SYSAPI_enSetDiagCallback, (void*)&enCallBackType, (void*)USERDIAG_boWriteDataCallBack, NULL);			
			}
		}
	}	
#endif
	
#if defined(USERDIAG_nEnableCAN)
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{		
		/* Setup the USERDIAG CAN resources */
		enEHIOResource = EH_VIO_CAN1;	
		
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	NULL, NULL);
	
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{	
			stPortConfigCB.enPortMode = IOAPI_enPortComms;
			stPortConfigCB.u32BaudRateHz = USERDIAG_nCAN1Frequency;
			stPortConfigCB.enVIOResource = EH_VIO_CAN1;
			stPortConfigCB.stPinConfig.uPinInfo.stCANPinInfo.enRXPin = EH_IO_CAN1R;
			stPortConfigCB.stPinConfig.uPinInfo.stCANPinInfo.enTXPin = EH_IO_CAN1T;	
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32GlobalCANDiagAddress = 0x7df;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANDiagAddress = 0x7e0;			
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[0] = 1217;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[1] = 401;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[2] = 1809;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[3] = 2024;					
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[0] = 0x7ff;				
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[1] = 0x7ff;	
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[2] = 0x7ff;	
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityMask[3] = 0x7ff;				
			stPortConfigCB.enLLProtocol = PROTAPI_enLLCAN11;
			stPortConfigCB.enNLProtocol = PROTAPI_enISO15765;			
			stPortConfigCB.enTLProtocol = PROTAPI_enTLUDS;
			stPortConfigCB.pfRXTransferCB = NULL;
			stPortConfigCB.pfTXTransferCB = NULL;	
			enEHIOType = IOAPI_enCANBus;
			
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stPortConfigCB);	
			
			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{	
				enCallBackType = DIAGAPI_enDataWriteCB;
				USER_vSVC(SYSAPI_enSetDiagCallback, (void*)&enCallBackType, (void*)USERDIAG_boWriteDataCallBack, NULL);			
			}
		}
	}	
#endif

#if defined(USERDIAG_nEnableENET)
	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{		
		/* Setup the USERDIAG ENET resources */
		enEHIOResource = EH_VIO_ENET_CH1;	
		
		USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource,	NULL, NULL);

		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{	
			stPortConfigCB.enPortMode = IOAPI_enPortComms;
			stPortConfigCB.enVIOResource = EH_VIO_ENET_CH1;
			stPortConfigCB.enLLProtocol = PROTAPI_enLL802_3;
			stPortConfigCB.enNLProtocol = PROTAPI_enISO15765_IP;			
			stPortConfigCB.enTLProtocol = PROTAPI_enTLUDS_UDP;
			stPortConfigCB.stNetConfig.uNetInfo.stLANNetInfo.u16RPCREQSourcePort = USERDIAG_nRPCUDSREQSourcePort;
			stPortConfigCB.stNetConfig.uNetInfo.stLANNetInfo.u16RPCREQDestPort = USERDIAG_nRPCUDSREQDestPort;		
			stPortConfigCB.pfRXTransferCB = NULL;
			stPortConfigCB.pfTXTransferCB = NULL;	
			enEHIOType = IOAPI_enENETChannel;
			
			USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
					(void*)&enEHIOType,	(void*)&stPortConfigCB);	
			
			if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
			{		
				enCallBackType = DIAGAPI_enDataWriteCB;
				USER_vSVC(SYSAPI_enSetDiagCallback, (void*)&enCallBackType, (void*)USERDIAG_boWriteDataCallBack, NULL);
			}			
		}
	}
#endif

#if defined(USERDIAG_nEnableUSB)
if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
{
	/* Setup the USERDIAG USB resources */
	enEHIOResource = EH_VIO_USB;
	
	USER_vSVC(SYSAPI_enRequestIOResource, (void*)&enEHIOResource, NULL, NULL);

	if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
	{
		stPortConfigCB.enPortMode = IOAPI_enPortComms;
		stPortConfigCB.enVIOResource = EH_VIO_USB;
		stPortConfigCB.enLLProtocol = PROTAPI_enUSB2;
		stPortConfigCB.enNLProtocol = PROTAPI_enISO15765;
		stPortConfigCB.enTLProtocol = PROTAPI_enTLUDS;
		stPortConfigCB.pfRXTransferCB = NULL;
		stPortConfigCB.pfTXTransferCB = NULL;
		enEHIOType = IOAPI_enUSBChannel;
		
		USER_vSVC(SYSAPI_enInitialiseIOResource, (void*)&enEHIOResource,
		(void*)&enEHIOType,	(void*)&stPortConfigCB);
		
		if (SYSAPI_enOK == pstSVCDataStruct->enSVCResult)
		{
			enCallBackType = DIAGAPI_enDataWriteCB;
			USER_vSVC(SYSAPI_enSetDiagCallback, (void*)&enCallBackType, (void*)USERDIAG_boWriteDataCallBack, NULL);
		}
	}

	USERDIAG_pu8Mode1Data = NULL;
	USERDIAG_pu8Mode2Data = NULL;
	USERDIAG_pu8Mode3Data = NULL;
	USERDIAG_pu8Mode4Data = NULL;
	USERDIAG_pu8Mode6Data = NULL;
	USERDIAG_pu8Mode7Data = NULL;

	USER_vSVC(SYSAPI_enGetMode1Buffer, NULL, NULL, NULL);
	USERDIAG_pu8Mode1Data = pstSVCDataStruct->pvArg1;

	if (NULL != USERDIAG_pu8Mode1Data)
	{
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Supported1]] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Supported1] + 1] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Supported1] + 2] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Supported1] + 3] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported2]] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported2] + 1] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported2] + 2] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported2] + 3] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported3]] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported3] + 1] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported3] + 2] = 0xff;
		USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_Mode1Supported3] + 3] = 0xff;
	}
}
#endif
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
void USERDIAG_vRun(puint32 const pu32Arg)
{
	static uint32 u32SpreadIDX;
	SPREADAPI_ttSpreadIDX tSpreadIDX;
	SPREADAPI_tstSpreadResult* pstSpreadResult;

	USERDIAG_u32GlobalTimeTick++;

	if (DIAG_nSPREADSMAX > u32SpreadIDX)
	{
		if (NULL != DIAG_apu8SpreadTableAddresses[u32SpreadIDX])
		{
			tSpreadIDX = USERDIAG_stGetSpreadIDX(DIAG_apu8SpreadTableAddresses[u32SpreadIDX]);

			if (-1 != tSpreadIDX)
			{
				pstSpreadResult = BOOSTED_pstGetSpread(tSpreadIDX);
				DIAG_astSpreadResult[u32SpreadIDX].uSpreadData.stSpreadResult.u16SpreadIndex = pstSpreadResult->uSpreadData.stSpreadResult.u16SpreadIndex;
				DIAG_astSpreadResult[u32SpreadIDX].uSpreadData.stSpreadResult.u16SpreadOffset = pstSpreadResult->uSpreadData.stSpreadResult.u16SpreadOffset;
			}
			else
			{
				/* Just copy the raw uint32 data because this is not a spread */
				DIAG_astSpreadResult[u32SpreadIDX].uSpreadData.u32Data = *(uint32*)DIAG_apu8SpreadTableAddresses[u32SpreadIDX];
			}
		}

		u32SpreadIDX++;
	}
	else
	{
		u32SpreadIDX = 0;
	}

	if (NULL != USERDIAG_pu8Mode1Data)
	{
		switch (USERDIAG_u32GlobalTimeTick % 10)
		{
			case 0:
			{


				break;
			}


			case 1:
			{
				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_RPM]] = (4 * CAM_u32RPMRaw) / 0x100;
				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_RPM] + 1] = (4 * CAM_u32RPMRaw) & 0xff;

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_IAT]] = (uint8)(((sint32)40000 + ATS_tTempCFiltered) / 1000);
				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_ECT]] = (uint8)(((sint32)40000 + CTS_tTempCFiltered) / 1000);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_TPS]] = (uint8)(TPS_tThetaFiltered / 352);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_MAP]] = (uint8)(MAP_tKiloPaFiltered / 1000);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_IgnAdvance]] = (uint8)((EST_tIgnitionAdvanceMtheta / 500) + 128);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_CalcEngineLoad]] = (uint8)(((MAP_tKiloPaFiltered / 1000) * CAM_u32RPMRaw) / 10000);

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_STT_Bank1]] = (uint8)(3 + (CLO2_u32STT[0] / 8));

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_STT_Bank2]] = (uint8)(3 + (CLO2_u32STT[1] / 8));

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_LTT_Bank1]] = 0x80;

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_LTT_Bank2]] = 0x80;

				USERDIAG_pu8Mode1Data[USERDIAG_rau16Mode1DataOffsets[MODE1_FuelPressure]] = (uint8)(USERCAL_stRAMCAL.u16ReturnlessPressureKPa / 33);

				break;
			}
		}

	}
}
#pragma GCC diagnostic pop


static SPREADAPI_ttSpreadIDX USERDIAG_stGetSpreadIDX(puint8 pu8Data)
{
	SPREADAPI_ttSpreadIDX tSpreadIDX = -1;
	uint32 u32AddressPairIDX;

	for (u32AddressPairIDX = 0; u32AddressPairIDX < DIAG_nSPREADSRECORDSMAX; u32AddressPairIDX++)
	{
		if (pu8Data == DIAG_astIDXAddressPairs[u32AddressPairIDX].pData)
		{
			tSpreadIDX = DIAG_astIDXAddressPairs[u32AddressPairIDX].tSpreadIDX;
			break;
		}
	}

	return tSpreadIDX;
}

void USERDIAG_vAddIDXAddressPair(USERDIAG_tstSpreadIDXAddressPair* pstSpreadIDXAddressPair)
{
    uint32 u32DiagSpreadsIDX;

	for (u32DiagSpreadsIDX = 0; u32DiagSpreadsIDX < DIAG_nSPREADSRECORDSMAX; u32DiagSpreadsIDX++)
	{
		if (NULL == DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].pData)
		{
			DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].pData = pstSpreadIDXAddressPair->pData;
			DIAG_astIDXAddressPairs[u32DiagSpreadsIDX].tSpreadIDX = pstSpreadIDXAddressPair->tSpreadIDX;
			break;
		}
	}
}


void USERDIAG_vTerminate(puint32 const pu32Arg)
{

}

void USERDIAG_vCallBack(uint32* const pu32Arg)
{

}

DIAGAPI_tenCBResult USERDIAG_boReadDataCallBack(uint16 u16CID, puint8 pu8Data, uint8 u8DataCount)
{
	return DIAGAPI_enCBOK;/*CR1_25*/
}

DIAGAPI_tenCBResult USERDIAG_boWriteDataCallBack(uint16 u16CID, puint8 pu8Data, uint8 u8DataCount)
{
	sint16 i16CIDIDX;
	DIAGAPI_tenCBResult enCBResult = DIAGAPI_enRWGenFail;
	
	i16CIDIDX = USERDIAG_i16GetCIDParamAddress(u16CID);
	
	if (-1 != i16CIDIDX)
	{
		if (USERDIAG_enSecLevel >= USERDIAG_rastIdentifierIB[i16CIDIDX].enSecLevel)
		{
			DIAG_vNTOH(pu8Data, u8DataCount, USERDIAG_rastIdentifierIB[i16CIDIDX].u8DataSize);
			memcpy((void*)USERDIAG_rastIdentifierIB[i16CIDIDX].pu8Data, 
					(void*)pu8Data, USERDIAG_rastIdentifierIB[i16CIDIDX].u8DataCount);
			enCBResult = DIAGAPI_enCBOK;
		}
		else
		{
			enCBResult = DIAGAPI_enRWSecurityError;
		}
	}
	else
	{
		enCBResult = DIAGAPI_enRWParamNotFound;
	}
	
	return enCBResult;
}

DIAGAPI_tenCBResult USERDIAG_boRoutineControl(DIAGAPI_tenRoutineAction enRoutineAction, puint8 pu8Data, uint8 u8DataCount)
{
	return DIAGAPI_enCBOK;/*CR1_25*/
}

static sint16 USERDIAG_i16GetCIDParamAddress(uint16 u16CID)
{
	uint16 u16IDX;
	sint16 i16CIDIDX = -1;
	
	for (u16IDX = 0; 
		u16IDX < (sizeof(USERDIAG_rastIdentifierIB) / sizeof(USERDIAG_tstIdentifierIB));
		u16IDX++)
	{
		if (u16CID == USERDIAG_rastIdentifierIB[u16IDX].u16CID)
		{
			i16CIDIDX = u16IDX;
			break;			
		}	
	}
	
	return i16CIDIDX;
}

static void	DIAG_vNTOH(puint8 pu8Data, uint8 u8DataCount, uint8 u8DataSize)
{
	uint32 u32Temp;
	uint16 u16Temp;
	
	if ((4 > u8DataSize) && (0 == (u8DataCount % u8DataSize)))/*CR1_28*//*CR1_29*/
	{
		while (0 < u8DataCount)
		{
			switch ((u8DataCount & 0x03) % u8DataSize)/*CR1_27*/
			{
				case 0:
				{
					memcpy((void*)&u32Temp, (void*)pu8Data, 4);
					*pu8Data++ = (uint8)((u32Temp & 0xff000000) >> 24);
					*pu8Data++ = (uint8)((u32Temp & 0xff0000) >> 16);				
					*pu8Data++ = (uint8)((u32Temp & 0xff00) >> 8);
					*pu8Data++ = (uint8)((u32Temp & 0xff) >> 0);	
					u8DataCount -= 4;
					break;
				}
				case 2:
				{
					memcpy((void*)&u16Temp, (void*)pu8Data, 2);
					*pu8Data++ = (uint8)((u16Temp & 0xff00) >> 8);
					*pu8Data++ = (uint8)((u16Temp & 0xff) >> 0);				
					u8DataCount -= 2;
					break;
				}	
				case 1:
				{
					u8DataCount = 0;
				}
			}	
		}	
	}
}

#endif //BUILD_USER


