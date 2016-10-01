/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      CRC16                                                  */
/* DESCRIPTION:        This code provides CRC16 functions                     */
/*                                                                            */
/* FILE NAME:          CRC16.c                                                */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "CRC16.h"

uint16 CRC16_u16CRC;

void CRC16_vStart(uint32* const u32Stat)
{
	CRC16HA_vStart(u32Stat);
}

void CRC16_vRun(uint32* const u32Stat)
{
	
}

void CRC16_vTerminate(uint32* const u32Stat)
{
	
}

uint16 CRC16_u16CalcCRC(uint16 u16CRC, uint8* u8Data, uint16 u16Len)
{
	return CRC16HA_u16CalcCRC(u16CRC, u8Data, u16Len);
}	

puint16 CRC16_pu16CalcCRC(uint16 u16CRC, uint8* u8Data, uint16 u16Len)
{
	return CRC16HA_pu16CalcCRC(u16CRC, u8Data, u16Len);
}	



