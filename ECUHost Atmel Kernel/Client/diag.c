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
	
	USERDIAG_u32GlobalTimeTick = 0;
	
#if USERDIAG_nEnableUART	
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
	
#ifdef USERDIAG_nEnableCAN	
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
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANDiagAddress = 0x750;			
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[0] = 0x450;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[1] = 0x550;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[2] = 0x650;
			stPortConfigCB.stNetConfig.uNetInfo.stCANNetInfo.u32CANPriorityAddress[3] = 0x700;			
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

#if USERDIAG_nEnableENET	
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

#if USERDIAG_nEnableUSB
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
}
#endif
}

void USERDIAG_vRun(puint32 const pu32Arg)
{
	USERDIAG_u32GlobalTimeTick++;
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


