/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      XXX                                                    */
/* DESCRIPTION:        XXX																										*/
/* FILE NAME:          XXX.c                                          				*/
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include <string.h>
#include "os.h"
#include "stddef.h"
#include "types.h"
#include "SPREADS.h"
#include "TABLES.h"
#include "SYSAPI.h"
#include "MATH.h"
#include "declarations.h"

/* Private data declarations
   -------------------------*/
TABLEAPI_tstTableCB TABLE_astTableCB[TABLE_nKernelTableCount];	 
TABLEAPI_tstTableCB* TABLE_apstTableCB[TABLE_nKernelTableCount + TABLE_nUserTableCount];
uint32 TABLE_au32Counter[TABLE_nKernelTableCount + TABLE_nUserTableCount];

/* Private function declarations
   ----------------------------*/

/* Public function definitions
   ---------------------------*/	 
void TABLES_vStart(puint32 const pu32Arg)
{
	uint32 u32IDX;
	
	for (u32IDX = 0; u32IDX < (TABLE_nKernelTableCount + TABLE_nUserTableCount); u32IDX++)
	{
		TABLE_apstTableCB[u32IDX] = NULL;
	}
}

void TABLES_vRun(puint32 const pu32Arg)
{
	
	
}

void TABLES_vTerminate(puint32 const pu32Arg)
{
	
	
}

TABLEAPI_ttTableIDX TABLE_tRequestKernelTable(TABLEAPI_tstTableCB* pstTableCBReq)
{
	TABLEAPI_tstTableCB* pstTableCB;
	uint32 u32TableIDX;
	TABLEAPI_ttTableIDX TableIDX = -1;
	
	/* Request a Kernel managed TABLE */
	for (u32TableIDX = 0; TABLE_nKernelTableCount > u32TableIDX; u32TableIDX++)
	{
		if (NULL == TABLE_apstTableCB[u32TableIDX])
		{
			pstTableCB = &TABLE_astTableCB[u32TableIDX];
			TABLE_apstTableCB[u32TableIDX] = pstTableCB;
			memcpy((void*)pstTableCB, (void*)pstTableCBReq, sizeof(TABLEAPI_tstTableCB));	
			TableIDX = u32TableIDX;
			break;
		}			
	}
	
	return TableIDX;	
}


bool TABLE_vCalculate(TABLEAPI_ttTableIDX tTableIDX)
{
	uint8 u8DataR; uint16 u16DataR; uint32 u32DataR;	
	sint8 s8DataL; sint16 s16DataL; sint32 s32DataL;	
	sint8 s8DataR; sint16 s16DataR; sint32 s32DataR;
	uint8 u8Data; uint16 u16Data; uint32 u32Data;
	sint8 s8Data; sint16 s16Data; sint32 s32Data;		
	puint8 pu8Data; puint16 pu16Data; puint32 pu32Data;
	psint8 ps8Data; psint16 ps16Data; psint32 ps32Data;		
	SPREADAPI_ttSpreadIDX tSpreadIDX;
	SPREADAPI_tstSpreadResult stSpreadResult;
	uint32 u32ShiftL = 0;
	uint32 u32ShiftR = 0;
	uint32 u32Factor;
	tSpreadIDX = TABLE_astTableCB[tTableIDX].tSpreadIDX;
	
	stSpreadResult = SPREAD_stGetSpread(tSpreadIDX);
	
	switch (TABLE_astTableCB[tTableIDX].enDataType)
	{
		case TYPE_enUInt8:
		case TYPE_enUInt16:
		case TYPE_enUInt32:
		case TYPE_enInt8:
		case TYPE_enInt16:
		{
			ps16Data = (sint16*)TABLE_astTableCB[tTableIDX].pvTableData;
			ps16Data += stSpreadResult.u16SpreadIndex;
		
			s16DataL = *ps16Data;
			s16DataR = *(ps16Data + 1);		
		
			while ((0x4000 <= s16DataL) || (-0x4000 >= s16DataL))
			{
				u32ShiftL++;
				s16DataL /= 2;
			}	
			
			while ((0x4000 <= s16DataR) || (-0x4000 >= s16DataR))
			{
				u32ShiftR++;
				s16DataR /= 2;
			}		

			u32ShiftL = (u32ShiftL > u32ShiftR) ? u32ShiftL : u32ShiftR;
			u32Factor = MATH_u32IDXToMask(u32ShiftL);
			
			s16DataL = (*ps16Data) / u32Factor;
			s16DataR = (*(ps16Data + 1)) / u32Factor;		
			
			u32Data = (s16DataR * stSpreadResult.u16SpreadOffset) +
								(s16DataL * (0xffff - stSpreadResult.u16SpreadOffset));
			
			u32Data *= u32Factor;
			u32Data /= 0xffff;
			
			*(sint16*)TABLE_astTableCB[tTableIDX].pvOutputData = (sint16)u32Data;
			break;
		}
		case TYPE_enInt32:
		{
			ps32Data = (sint32*)TABLE_astTableCB[tTableIDX].pvTableData;
			ps32Data += stSpreadResult.u16SpreadIndex;
		
			s32DataL = *ps32Data;
			s32DataR = *(ps32Data + 1);		
		
			while ((0x4000 <= s32DataL) || (-0x4000 >= s32DataL))
			{
				u32ShiftL++;
				s32DataL /= 2;
			}	
			
			while ((0x4000 <= s32DataR) || (-0x4000 >= s32DataR))
			{
				u32ShiftR++;
				s32DataR /= 2;
			}		

			u32ShiftL = (u32ShiftL > u32ShiftR) ? u32ShiftL : u32ShiftR;
			u32Factor = MATH_u32IDXToMask(u32ShiftL);
			
			s32DataL = (*ps32Data) / u32Factor;
			s32DataR = (*(ps32Data + 1)) / u32Factor;		
			
			u32Data = (s32DataR * stSpreadResult.u16SpreadOffset) +
								(s32DataL * (0xffff - stSpreadResult.u16SpreadOffset));
			
			u32Data /= 0xffff;			//matthew check this look for a better way to hold precision
			u32Data *= u32Factor;
			
			*(sint32*)TABLE_astTableCB[tTableIDX].pvOutputData = (sint32)u32Data;
			break;
		}		
			
	}			
	
	OS_stSVCDataStruct.pvData = NULL;
	
	/* TODO suppress warning */
	(void)u8DataR;
	(void)u16DataR;
	(void)s8DataL;
	(void)s8DataR;
	(void)u8Data;
	(void)u16Data;
	(void)s16Data;
	(void)pu16Data;
	(void)ps8Data;
	(void)pu32Data;
	(void)u32DataR;
	(void)s8Data;
	(void)s32Data;
	(void)pu8Data;
	
	return true;
}

