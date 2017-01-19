/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      User Math                               								*/
/* DESCRIPTION:        This code module provides mathematical data filtering  */
/*                     functions for the user application   									*/
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

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
uint16 USERMATH_u32SinglePoleLowPassFilter(uint16 u16NewVal, uint8 u8FilterWeight, puint32 pu32FilterVal)
{
	u16NewVal = (UINT16_MAX >= u16NewVal) ? u16NewVal : USHRT_MAX;/*CR1_78*/
	*pu32FilterVal = (0x100 - u8FilterWeight) * (*pu32FilterVal);/*CR1_79*/
	*pu32FilterVal /= 0x100;
	*pu32FilterVal += (u8FilterWeight * u16NewVal);/*CR1_79*/
	
	return (uint16)*pu32FilterVal;
}

uint32 USERMATH_u32DiscardAndAverage32(puint32 pu32Samples, uint32 u32SampleCount, uint32 u32DiscardCount)
{
	uint32 u32Sum = 0;
	uint32 u32IDX;
	uint32 u32DiscardedCount = 0;
	uint32 u32PreviousWorstDelta;
	uint32 u32WorstDelta;
	uint32 u32WorstDeltaIndex;
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
	uint32 u32WorstDeltaIndex;
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

#endif //BUILD_USER



			
			
