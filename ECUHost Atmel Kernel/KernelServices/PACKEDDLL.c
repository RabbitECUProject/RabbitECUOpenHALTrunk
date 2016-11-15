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

#include "DLL.h"

void PACKEDDLL_vUnpack(DLL_tstRXDLLData* pRXFrame)
{
	pRXFrame->u8Data[0] = pRXFrame->u8Data[0] << 4 | pRXFrame->u8Data[1] >> 3;
	pRXFrame->u8Data[1] = pRXFrame->u8Data[1] << 5 | pRXFrame->u8Data[2] >> 2;
	pRXFrame->u8Data[2] = pRXFrame->u8Data[2] << 6 | pRXFrame->u8Data[3] >> 1;
	pRXFrame->u8Data[3] = pRXFrame->u8Data[3] << 7 | pRXFrame->u8Data[4];
	pRXFrame->u8Data[4] = pRXFrame->u8Data[5] << 1 | pRXFrame->u8Data[6] >> 6;
	pRXFrame->u8Data[5] = pRXFrame->u8Data[6] << 2 | pRXFrame->u8Data[7] >> 5;
	pRXFrame->u8Data[6] = pRXFrame->u8Data[7] << 3 | pRXFrame->u8Data[8] >> 4;
	pRXFrame->u8Data[7] = pRXFrame->u8Data[8] << 4 | pRXFrame->u8Data[9] >> 3;
	pRXFrame->u8Data[8] = pRXFrame->u8Data[9] << 5 | pRXFrame->u8Data[10] >> 2;	
	pRXFrame->u8Data[9] = 0x7f & (pRXFrame->u8Data[10] << 6 | pRXFrame->u8Data[11]);	
	pRXFrame->u8DataCount = 10;
}

void PACKEDDLL_vPack(puint8 pu8TXData)
{
	uint8 u8IDX;
	
	for(u8IDX = 11; u8IDX > 1; u8IDX--)
	{
		pu8TXData[u8IDX] = pu8TXData[u8IDX - 2];
	}
	
	pu8TXData[0] = 0xc0 | pu8TXData[2] >> 4;
	pu8TXData[1] = 0x7f & (pu8TXData[2] << 3 | pu8TXData[3] >> 5);
	pu8TXData[2] = 0x7f & (pu8TXData[3] << 2 | pu8TXData[4] >> 6);
	pu8TXData[3] = 0x7f & (pu8TXData[4] << 1 | pu8TXData[5] >> 7);
	pu8TXData[4] = 0x7f & (pu8TXData[5]);
	pu8TXData[5] = 0x7f & (pu8TXData[6] >> 1);
	pu8TXData[6] = 0x7f & (pu8TXData[6] << 6 | pu8TXData[7] >> 2);
	pu8TXData[7] = 0x7f & (pu8TXData[7] << 5 | pu8TXData[8] >> 3);
	pu8TXData[8] = 0x7f & (pu8TXData[8] << 4 | pu8TXData[9] >> 4);	
	pu8TXData[9] = 0x7f & (pu8TXData[9] << 3 | pu8TXData[10] >> 5);	
	pu8TXData[10] = 0x7f & (pu8TXData[10] << 2 | pu8TXData[11] >> 6);	
	pu8TXData[11] = 0xbf & (pu8TXData[11] | 0x80);	

}

