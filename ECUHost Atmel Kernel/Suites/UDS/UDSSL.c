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
#include "UDSSL.h"

static void UDSSL_vSessionControl(puint8 pu8Buffer);

DIAGAPI_tenSession UDSSL_enSession;
sint32 UDSSL_i32ResourceTimerIDX;

void UDSSL_vStart(void)
{
	UDSNL_vStart();
	UDSSL_i32ResourceTimerIDX = -1;	
}

void UDSSL_vProcessBufferCB(puint8 pu8RXBuffer, uint32 u32RXDataCount, puint8 pu8TXBuffer, puint32 pu32TXByteCount, uint32 u32TXCap)
{
	UDSSL_vSessionControl(pu8RXBuffer);
	UDSAL_vProcessBufferCB(pu8RXBuffer, u32RXDataCount, pu8TXBuffer, pu32TXByteCount, u32TXCap);
}

DIAGAPI_tenSession UDSSL_enGetSession(void)
{
	return UDSSL_enSession;
}

void UDSSL_vResetSessionTimer(void)
{
	if (-1 == UDSSL_i32ResourceTimerIDX)
	{
		UDSSL_i32ResourceTimerIDX = TIMER_vConfigureAlarm(UDSSL_nSessionTimeout, UDSSL_vSessionTimeout);	
	}
	else
	{
		TIMER_vSetAlarm(UDSSL_nSessionTimeout, UDSSL_i32ResourceTimerIDX);
	}
}

static void UDSSL_vSessionControl(puint8 pu8Buffer)
{
	if (UDSAL_SID_DSC == pu8Buffer[UDSSL_IDX_SUBSID])
	{	
		switch(pu8Buffer[UDSSL_IDX_SID])
		{
// 			case UDSSL_SESS_DFLT:
// 				UDSSL_enSession = UDSSL_enSessionDefault;
// 				break;
// 			
// 			case UDSSL_SESS_PGM:
// 				if ((UDSSL_enSecurityLevel == UDSSL_enSecLevelOEM) ||
// 						(UDSSL_enSecurityLevel == UDSSL_enSecLevelMDAC))
// 				{
// 					UDSSL_enSession = UDSSL_enSessionProgramming;
// 					UDSSL_vResponse(UDS_RSP_OK);
// 				}
// 				else
// 				{
// 					UDSSL_enSession = UDSSL_enSessionInactive;
// 					UDSSL_vResponse(UDS_RSP_SECRQRD);
// 				}			
// 				break;	
// 				
// 			case UDS_SESS_EXT:
// 				if ((UDSSL_enSecurityLevel == UDSSL_enSecLevelOEM) ||
// 						(UDSSL_enSecurityLevel == UDSSL_enSecLevelMDAC))
// 				{
// 					UDSSL_enSession = UDSSL_enSessionProgramming;
// 					UDSSL_vResponse(UDS_RSP_OK);
// 				}
// 				else
// 				{
// 					UDSSL_enSession = UDSSL_enSessionInactive;
// 					UDSSL_vResponse(UDS_RSP_SECRQRD);
// 				}			
// 				break;	
		}
	}
}

void UDSSL_vSessionTimeout(void)
{
	UDSSL_enSession = DIAGAPI_enNoSession;
}
