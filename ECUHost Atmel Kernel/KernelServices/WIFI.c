/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                               							*/
/* PACKAGE TITLE:      XXX                  																	*/
/* DESCRIPTION:        XXX																										*/
/* FILE NAME:          XXX.c                                          				*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#include <string.h>

#include "mk60f12.h"
#include "regset.h"
#include "sys.h"
#include "declarations.h"
#include "os.h"
#include "types.h"
#include "eth_phy.h"
#include "mii.h"
#include "nbuf.h"
#include "eth.h"
#include "ENE.h"
#include "SIM.h"
#include "irq.h"
#include "CAN.h"
#include "DACAPI.h"
#include "TEPMAPI.h"
#include "IO.h"
#include "WIFI.h"
#include "DLL.h"
#include "macros.h"
#include "build.h"



const REGSET_tstReg32Val WIFI_rastWIFIReg32Val[] = WIFI_nReg32Set;
WIFI_tstWLAN WIFI_stWLAN;
const IOAPI_tstPortConfigCB WIFI_rstPortConfigCB = WIFI_nUARTConfigCB;
const char WIFI_rau8IOK[] = WIFI_nATPLUSi_IOK;
const char WIFI_rau8IERROR[] = WIFI_nATPLUSi_IERROR;
const char WIFI_rau8SSIDWLAN[] = WIFI_nSSIDECUHOSTWLAN;
const char WIFI_rau8RPx[WIFI_REP_COUNT][11] = WIFI_nATPLUSi_REPORTS;
const char WIFI_rau8CFx[WIFI_CFG_COUNT][30] = WIFI_nATPLUSi_CONFIG;
char WIFI_au8Report[WIFI_nReportCount][WIFI_nReportLengthMax];
uint8 WIFI_u8ReportIndex;
uint8 WIFI_u8ReportResponseIndex;
uint8 WIFI_u8ConfigIndex;
uint8 WIFI_u8CallBackCount;
uint8 WIFI_u8RunCount;

WIFI_tenState WIFI_enState;

bool WIFI_boIOK;
bool WIFI_boIOERROR;
bool WIFI_boWLANFound;
bool WIFI_boIOOnline;

void WIFI_vStart(uint32* const u32Stat)
{
	SYSAPI_ttClientHandle tClientHandle;
	
	REGSET_vInitReg32(&WIFI_rastWIFIReg32Val[0]);
	
	/* Setup the WIFI UART resources */
	tClientHandle = WIFI_xGetClientHandle();
	
	RESM_RequestEHIOResource(EH_VIO_UART1, tClientHandle);			

	if ((WIFI_xGetClientHandle()) == tClientHandle)
	{				
		IO_enInitCommsResource(EH_VIO_UART1, (IOAPI_tstPortConfigCB*)&WIFI_rstPortConfigCB);		
	}	

	OS_xModuleStartOK(*u32Stat);

	WIFI_boIOERROR = false;	
	WIFI_boIOK = true;	
	WIFI_boIOOnline = false;
	WIFI_boWLANFound = false;
	WIFI_u8ReportIndex = 0;
	WIFI_u8ReportResponseIndex = 0;	
	WIFI_u8ConfigIndex = 0;
	WIFI_u8CallBackCount = 0;
	WIFI_enState = WIFI_enREPS;
	WIFI_enState = WIFI_enRST;
	WIFI_u8RunCount = 0;
	
	memset((void*)&WIFI_au8Report[0], 0, WIFI_nReportCount * WIFI_nReportLengthMax);
}

void WIFI_vRun(uint32* const u32Stat)
{
	uint8 au8TXData[WIFI_nPromptStringSize];
	uint32 u32TXDataCount;
	
	if(((++WIFI_u8RunCount % 4) != 0) || (FALSE == ENE_boResetComplete)) return;
	
		//if((true == WIFI_boBRMOK) && (true == WIFI_boIOK) && (false == WIFI_boIOERROR))
	
	switch(WIFI_enState)
	{
		case WIFI_enRST:
		{
			WIFI_enState = WIFI_enCFG;
			break;
		}		
		case WIFI_enCFG:
		{
			if (WIFI_CFG_COUNT <= WIFI_u8ConfigIndex)
			{
				WIFI_enState = WIFI_enREPS;
			}
			else
			{
				//if ((true == WIFI_boIOK) && (0 == WIFI_u8CallBackCount))
				if (0 == WIFI_u8CallBackCount)					
					/* check callbackcount is zero - no callbacks received last 
						 iteration means the callbacks have stopped */
				{
					u32TXDataCount = 0;
					
					while ((0 != WIFI_rau8CFx[WIFI_u8ConfigIndex][u32TXDataCount]) 
										&& (30 > u32TXDataCount)) 
					{
						u32TXDataCount++;
					}
						
					memcpy(&au8TXData[0], &WIFI_rau8CFx[WIFI_u8ConfigIndex++][0], u32TXDataCount);	
					DLL_boQueueMessage(WIFI_rstPortConfigCB.enVIOResource, &au8TXData[0], u32TXDataCount);
					WIFI_boIOK = false;
					WIFI_boIOERROR = false;	
				}					
			}			
			break;			
		}
		case WIFI_enREPS:
		{
			if (WIFI_REP_COUNT <= WIFI_u8ReportIndex)
			{
				WIFI_enState = WIFI_enCHK;
			}
			else
			{
				if(0 == WIFI_u8CallBackCount)
					/* check callbackcount is zero - no callbacks received last 
						 iteration means the callbacks have stopped */
				{
					u32TXDataCount = 0;
					
					while ((0 != WIFI_rau8RPx[WIFI_u8ReportIndex][u32TXDataCount]) 
										&& (11 > u32TXDataCount)) 
					{
						u32TXDataCount++;
					}
						
					memcpy(&au8TXData[0], &WIFI_rau8RPx[WIFI_u8ReportIndex++][0], u32TXDataCount);	
					DLL_boQueueMessage(WIFI_rstPortConfigCB.enVIOResource, &au8TXData[0], u32TXDataCount);
					WIFI_boIOK = false;
					WIFI_boIOERROR = false;	
				}					
			}
			break;			
		}	
		case WIFI_enCHK:
		{
			if (false == WIFI_boWLANFound)
			{
				WIFI_boWLANFound = WIFI_boConfirmECUHOSTLAN();	
			}
			break;
		}
	}
	
	WIFI_u8CallBackCount = 0;
}

void WIFI_vTerminate(uint32* const u32Stat)
{

}

/* TODO this callback does not comply with the callback typedef */
void WIFI_vReceive(IOAPI_tenEHIOResource enEHIOResource, puint8 pu8Data, uint32 u32DataCount)
{
	WIFI_u8CallBackCount++;
	
	if(0 == strcmp((const char*)pu8Data, WIFI_rau8IOK))
	{
		WIFI_boIOK = true;
	}	
	
	if(0 == strcmp((const char*)pu8Data, WIFI_rau8IERROR))
	{
		WIFI_boIOERROR = true;
	}

// 	else if(0 == strcmp((const char*)pu8Data, WIFI_nATPLUSi_IONLINE))
// 	{
// 		WIFI_boIOOnline = true;	
// 	}
// 		
// 	else
// 	{
#if BUILD_KERNEL_OR_KERNEL_APP
	memcpy((uint8*)&WIFI_au8Report[WIFI_u8ReportResponseIndex++][0], pu8Data, 64);	
#endif	
	WIFI_boIOK = true;
	
	if(98 < WIFI_u8ReportResponseIndex) WIFI_u8ReportResponseIndex = 98;
	//}
}

bool WIFI_boConfirmECUHOSTLAN(void)
{
	bool boNetworksResponseFound = false;
	bool boECUHOSTWLANFound = false;
	char u8SearchString[64];
	char* pu8SearchString = &u8SearchString[0];
	char* pu8CharPos;
	uint8 u8Index;
	uint32 u32SearchIndex = 0;
	
#if BUILD_PBL
	boECUHOSTWLANFound = true;	
#endif
	
	while((false == boECUHOSTWLANFound) && (WIFI_nReportCount > u32SearchIndex))
	{
#if BUILD_KERNEL_OR_KERNEL_APP
		memcpy(pu8SearchString, (uint8*)&WIFI_au8Report[u32SearchIndex++][0], sizeof(u8SearchString));		
#endif		
		if(false == boNetworksResponseFound)
		{
			if(0 == strcmp(pu8SearchString, "AT+iRP20\r"))
			{
				boNetworksResponseFound = true;
			}				
		}
		else
		{
			pu8CharPos = NULL;
			/* find the first appearing comma */
			pu8CharPos = strchr(pu8SearchString, ',');
			
			if(NULL != pu8CharPos)
			{
				*pu8CharPos = '\0';
				
				if(0 == strcmp(pu8SearchString, WIFI_rau8SSIDWLAN))
				{
					boECUHOSTWLANFound = true;
					pu8SearchString = pu8CharPos + 1;
					pu8CharPos = strchr(pu8SearchString, ',');
					pu8SearchString = pu8CharPos + 1;
					pu8CharPos = strchr(pu8SearchString, ',');
					*pu8CharPos = '\0';
					
					memcpy(WIFI_stWLAN.szRemoteMAC, pu8SearchString, sizeof(WIFI_stWLAN.szRemoteMAC) - 1);
					
					/* parse the MAC string and convert to byte array */
					for(u8Index = 0; u8Index < 6; u8Index++)
					{
						WIFI_stWLAN.au8RemoteMAC[u8Index] = ((*pu8SearchString - '0') << 4) + ((*(pu8SearchString + 1)) - '0');
						pu8SearchString += 3;
					}			
				}				
			}			
		}
	}
	
	return boECUHOSTWLANFound;
}


			
