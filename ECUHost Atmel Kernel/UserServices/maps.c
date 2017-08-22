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
#include <string.h>
#include "os.h"
#include "stddef.h"
#include "types.h"
#include "SPREADS.h"
#include "maps.h"
#include "SYSAPI.h"
#include "MATH.h"
#include "declarations.h"

/* Private data declarations
   -------------------------*/
MAPSAPI_tstMapCB MAP_astMapCB[MAP_nKernelMapCount];	 
MAPSAPI_tstMapCB* MAP_apstMapCB[MAP_nKernelMapCount + MAP_nUserMapCount];
uint32 MAP_au32Counter[MAP_nKernelMapCount + MAP_nUserMapCount];

/* Private function declarations
   ----------------------------*/
static uint16 MAP_u16Interp(void*, uint16, uint16);
static uint32 MAP_u32Interp(void*, uint16, uint16);

/* Public function definitions
   ---------------------------*/	 
void MAPS_vStart(puint32 const pu32Arg)
{
	uint32 u32IDX;
	
	for (u32IDX = 0; u32IDX < (MAP_nKernelMapCount + MAP_nUserMapCount); u32IDX++)
	{
		MAP_apstMapCB[u32IDX] = NULL;
	}
}

void MAPS_vRun(puint32 const pu32Arg)
{
	
	
}

void MAPS_vTerminate(puint32 const pu32Arg)
{
	
	
}

MAPSAPI_ttMapIDX MAP_tRequestKernelMap(MAPSAPI_tstMapCB* pstTableCBReq)
{
	MAPSAPI_tstMapCB* pstMapCB;
	uint32 u32MapIDX;
	MAPSAPI_ttMapIDX MapIDX = -1;
	
	/* Request a Kernel managed TABLE */
	for (u32MapIDX = 0; MAP_nKernelMapCount > u32MapIDX; u32MapIDX++)
	{
		if (NULL == MAP_apstMapCB[u32MapIDX])
		{
			pstMapCB = &MAP_astMapCB[u32MapIDX];
			MAP_apstMapCB[u32MapIDX] = pstMapCB;
			memcpy((void*)pstMapCB, (void*)pstTableCBReq, sizeof(MAPSAPI_tstMapCB));	
			MapIDX = u32MapIDX;
			break;
		}			
	}
	
	return MapIDX;	
}


Bool MAP_vCalculate(MAPSAPI_ttMapIDX tMapIDX)
{
	uint8 u8DataR; uint16 u16DataR; uint32 u32DataR; uint32 u32DataL;	
	sint8 s8DataL; sint16 s16DataL; sint32 s32DataL;	
	sint8 s8DataR; sint16 s16DataR; sint32 s32DataR;
	uint8 u8Data; uint16 u16Data; uint32 u32Data;
	sint8 s8Data; sint16 s16Data; sint32 s32Data;		
	puint8 pu8Data; puint16 pu16Data; puint32 pu32Data;
	psint8 ps8Data; psint16 ps16Data; psint32 ps32Data;		
	SPREADAPI_ttSpreadIDX tSpreadIDXX;
	SPREADAPI_ttSpreadIDX tSpreadIDXY;
	SPREADAPI_tstSpreadResult stSpreadResultX;
	SPREADAPI_tstSpreadResult stSpreadResultY;
	uint32 u32ShiftL = 0;
	uint32 u32ShiftR = 0;
	uint32 u32Factor;
	uint16 u16DataResult[2];
	uint32 u32DataResult[2];
	void* pvMapRowData;

	tSpreadIDXX = MAP_apstMapCB[tMapIDX]->tSpreadIDXX;
	tSpreadIDXY = MAP_apstMapCB[tMapIDX]->tSpreadIDXY;
	
	stSpreadResultX = SPREAD_stGetSpread(tSpreadIDXX);
	stSpreadResultY = SPREAD_stGetSpread(tSpreadIDXY);
	
	switch (MAP_astMapCB[tMapIDX].enDataType)
	{
		case TYPE_enUInt8:
		case TYPE_enUInt16:
		case TYPE_enInt8:
		case TYPE_enInt16:
		{
            pvMapRowData = MAP_astMapCB[tMapIDX].pvMapData;
			pvMapRowData += stSpreadResultX.u16SpreadIndex * MAP_astMapCB[tMapIDX].i16MapSizeX * sizeof(uint16);
            u16DataResult[0] = MAP_u16Interp(pvMapRowData, stSpreadResultY.u16SpreadIndex, stSpreadResultY.u16SpreadOffset);
			pvMapRowData +=MAP_astMapCB[tMapIDX].i16MapSizeX * sizeof(uint16);
            u16DataResult[1] = MAP_u16Interp(pvMapRowData, stSpreadResultY.u16SpreadIndex, stSpreadResultY.u16SpreadOffset);
			*(sint16*)MAP_astMapCB[tMapIDX].pvOutputData = MAP_u16Interp(&u16DataResult, 0, stSpreadResultX.u16SpreadOffset);
			break;
		}
		case TYPE_enInt32:
		{
			ps32Data = (sint32*)MAP_astMapCB[tMapIDX].pvMapData;
			ps32Data += stSpreadResultX.u16SpreadIndex;
		
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
			
			u32Data = (s32DataR * stSpreadResultX.u16SpreadOffset) +
								(s32DataL * (0xffff - stSpreadResultX.u16SpreadOffset));
			
			u32Data /= 0xffff;			//matthew check this look for a better way to hold precision
			u32Data *= u32Factor;
			
			*(sint32*)MAP_astMapCB[tMapIDX].pvOutputData = (sint32)u32Data;
			break;
		}		
		case TYPE_enUInt32:
		{
            pvMapRowData = MAP_astMapCB[tMapIDX].pvMapData;
			pvMapRowData += stSpreadResultX.u16SpreadIndex * MAP_astMapCB[tMapIDX].i16MapSizeX * sizeof(uint32);
            u32DataResult[0] = MAP_u32Interp(pvMapRowData, stSpreadResultY.u16SpreadIndex, stSpreadResultY.u16SpreadOffset);
			pvMapRowData +=MAP_astMapCB[tMapIDX].i16MapSizeX * sizeof(uint32);
            u32DataResult[1] = MAP_u32Interp(pvMapRowData, stSpreadResultY.u16SpreadIndex, stSpreadResultY.u16SpreadOffset);
			*(uint32*)MAP_astMapCB[tMapIDX].pvOutputData = MAP_u32Interp(&u32DataResult, 0, stSpreadResultX.u16SpreadOffset);
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
	(void)s8Data;
	(void)s32Data;
	(void)pu8Data;
	
	return true;
}

static uint16 MAP_u16Interp(void* pvMapData, uint16 u16SpreadIDX, uint16 u16SpreadOffset)
{
    puint16 ps16Data;
	sint16 s16DataL, s16DataR;
	uint32 u32ShiftL = 0;
	uint32 u32ShiftR = 0;
	uint32 u32Factor, u32Data;

	ps16Data = (sint16*)pvMapData;
	ps16Data += u16SpreadIDX;
		
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
			
	u32Data = (s16DataR * u16SpreadOffset) +
						(s16DataL * (0xffff - u16SpreadOffset));
			
	u32Data *= u32Factor;
	u32Data /= 0xffff;
			
	return (sint16)u32Data;
}

static uint32 MAP_u32Interp(void* pvMapData, uint16 u16SpreadIDX, uint16 u16SpreadOffset)
{
    puint32 pu32Data;
	uint32 u32DataL, u32DataR;
	uint32 u32ShiftL = 0;
	uint32 u32ShiftR = 0;
	uint32 u32Factor, u32Data;

	pu32Data = (sint32*)pvMapData;
	pu32Data += u16SpreadIDX;
		
	u32DataL = *pu32Data;
	u32DataR = *(pu32Data + 1);		
		
	while (0x4000 <= u32DataL)
	{
		u32ShiftL++;
		u32DataL /= 2;
	}	
			
	while (0x4000 <= u32DataR)
	{
		u32ShiftR++;
		u32DataR /= 2;
	}		

	u32ShiftL = (u32ShiftL > u32ShiftR) ? u32ShiftL : u32ShiftR;
	u32Factor = MATH_u32IDXToMask(u32ShiftL);
			
	u32DataL = (*pu32Data) / u32Factor;
	u32DataR = (*(pu32Data + 1)) / u32Factor;		
			
	u32Data = (u32DataR * u16SpreadOffset) +
						(u32DataL * (0x10000 - u16SpreadOffset));
			
	u32Data /= 0x10000;
	u32Data *= u32Factor;
			
	return u32Data;
}

