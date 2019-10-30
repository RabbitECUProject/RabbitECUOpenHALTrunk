/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      User Math                                              */
/* DESCRIPTION:        This code module provides mathematical data filtering  */
/*                     functions for the user application                     */
/* FILE NAME:          USERMATH.c                                             */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "USERMATH.H"

uint8 USERMATH_au8TimeConstantData[] = USERMATH_nTimeConstantData;
uint32 USERMATH_rau32SquareRoot[] = USERMATH_nSQRT_DATA;
uint16 USERMATH_rau16CRCData[] = CRC16_DATA;


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
uint8 USERMATH_u8GetFilterFromTimeConstant(uint32 u32CalcPeriodMs, uint32 u32TimeConstantMs)
{
    uint32 u32Temp;
	uint8 u8FilterVal;

    switch (u32CalcPeriodMs)
	{
	    case 1: u32Temp = 256; break;
	    case 2: u32Temp = 128; break;
	    case 4: u32Temp = 64; break;
	    case 8: u32Temp = 32; break;
	    case 16: u32Temp = 16; break;
	    case 32: u32Temp = 8; break;
	    case 64: u32Temp = 4; break;
	    case 128: u32Temp = 2; break;
	    case 256: u32Temp = 1; break;
		default: u32Temp = 32; break;
	}

	u32Temp *= u32TimeConstantMs;
	u32Temp /= 32;
	u32Temp = MIN(0x1ff, u32Temp);

	u8FilterVal = USERMATH_au8TimeConstantData[u32Temp];

	return u8FilterVal;
}



uint16 USERMATH_u16SinglePoleLowPassFilter16(uint16 u16NewVal, uint8 u8FilterWeight, puint32 pu32FilterVal)
{
	u16NewVal = (UINT16_MAX >= u16NewVal) ? u16NewVal : USHRT_MAX;/*CR1_78*/
	*pu32FilterVal = (0x100 - u8FilterWeight) * (*pu32FilterVal);/*CR1_79*/
	*pu32FilterVal /= 0x100;
	*pu32FilterVal += (u8FilterWeight * u16NewVal);/*CR1_79*/
	
	return (uint16)*pu32FilterVal;
}

uint32 USERMATH_u32SinglePoleLowPassFilter32(uint32 u32NewVal, uint8 u8FilterWeight, puint32 pu32FilterVal)
{
	uint32 u32Shift1 = 0;
	uint32 u32Shift2 = 0;
	uint32 u32Temp;

	while (0x1000000 <= u32NewVal)
	{
	    u32NewVal /= 2;
		u32Shift1++;
	}

	while (0x1000000 <= *pu32FilterVal)
	{
		*pu32FilterVal /= 2;
		u32Shift2++;
	}

	*pu32FilterVal = (0x100 - u8FilterWeight) * (*pu32FilterVal);
	*pu32FilterVal /= 0x100;
	*pu32FilterVal = *pu32FilterVal << u32Shift2;

	u32Temp = (u8FilterWeight * u32NewVal);
	u32Temp /= 0x100;
	u32Temp = u32Temp << u32Shift1;

	*pu32FilterVal += u32Temp;
	
	return *pu32FilterVal;
}

uint32 USERMATH_u32DiscardAndAverage32(puint32 pu32Samples, uint32 u32SampleCount, uint32 u32DiscardCount)
{
	uint32 u32Sum = 0;
	uint32 u32IDX;
	uint32 u32DiscardedCount = 0;
	uint32 u32PreviousWorstDelta;
	uint32 u32WorstDelta;
	uint32 u32WorstDeltaIndex = 0;
	uint32 u32Average = 0;/*CR1_80*/
	uint32 u32Delta;
	
	if (0 < u32SampleCount)/*CR1_80*/
	{
		for (u32IDX = 0; u32IDX < u32SampleCount; u32IDX++)
		{
			u32Sum += *(pu32Samples + u32IDX);
		}
		
		u32Average = u32Sum / u32SampleCount;
		u32PreviousWorstDelta = UINT_MAX;/*CR1_81*/
		
		while (u32DiscardedCount < u32DiscardCount)
		{
			u32WorstDelta = 0;
			
			for (u32IDX = 0; u32IDX < u32SampleCount; u32IDX++)
			{
				u32Delta = (*(pu32Samples + u32IDX) > u32Average) ? 
					*(pu32Samples + u32IDX) - u32Average : u32Average - *(pu32Samples + u32IDX);
			
				if ((u32Delta >= u32WorstDelta) && (u32Delta < u32PreviousWorstDelta))
				{
					u32WorstDelta = u32Delta;
					u32WorstDeltaIndex = u32IDX;
				}
			}
			
			u32Sum -= *(pu32Samples + u32WorstDeltaIndex);
			u32PreviousWorstDelta = u32WorstDelta;
			u32DiscardedCount++;
		}
		
		u32Average = u32Sum / (u32SampleCount - u32DiscardCount);
	}

	return u32Average;
}

sint32 USERMATH_s32DiscardAndAverage32(psint32 ps32Samples, uint32 u32SampleCount, uint32 u32DiscardCount)
{
	sint32 s32Sum = 0;
	uint32 u32IDX;
	uint32 u32DiscardedCount = 0;
	sint32 s32PreviousWorstDelta;
	sint32 s32WorstDelta;
	uint32 u32WorstDeltaIndex = 0;
	sint32 s32Average;
	sint32 s32Delta;
	
	for (u32IDX = 0; u32IDX < u32SampleCount; u32IDX++)
	{
		s32Sum += *(ps32Samples + u32IDX);
	}
	
	s32Average = s32Sum / (sint32)u32SampleCount;
	s32PreviousWorstDelta = INT_MAX;/*CR1_82*/
	
	while (u32DiscardedCount < u32DiscardCount)
	{
		s32WorstDelta = 0;
		
		for (u32IDX = 0; u32IDX < u32SampleCount; u32IDX++)
		{
			s32Delta = s32Average - *(ps32Samples + u32IDX);
		
			if ((ABS(s32Delta) >= s32WorstDelta) && (ABS(s32Delta) < s32PreviousWorstDelta))
			{
				s32WorstDelta = ABS(s32Delta);
				u32WorstDeltaIndex = u32IDX;
			}
		}
		
		s32Sum -= *(ps32Samples + u32WorstDeltaIndex);
		s32PreviousWorstDelta = s32WorstDelta;
		u32DiscardedCount++;
	}
	
	s32Average = s32Sum / (sint32)(u32SampleCount - u32DiscardCount);
	
	return s32Average;
}


uint32 USERMATH_u32GetSquareRoot(uint32 u32Fraction)
{
	uint32 u32Sqrt = 0;

	if (1000 > u32Fraction)
	{
		u32Sqrt = USERMATH_rau32SquareRoot[u32Fraction];
	}

	return u32Sqrt;
}


uint16 USERMATH_U16GetCRC(uint16 CRCSeed, puint8 pu8Data, uint32 u32Len)
{
	while (u32Len--)
	{
		CRCSeed = (CRCSeed << 8) ^ USERMATH_rau16CRCData[((CRCSeed >> 8) ^ *pu8Data++)];
	}

	return CRCSeed;
}

#endif //BUILD_USER



			
			
