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
#include "SYSAPI.h"
#include "declarations.h"

/* Private data declarations
   -------------------------*/
SPREADAPI_tstSpreadCB SPREAD_astSpreadCB[SPREAD_nKernelSpreadCount];	 
SPREADAPI_tstSpreadCB* SPREAD_apstSpreadCB[SPREAD_nKernelSpreadCount + SPREAD_nUserSpreadCount];
uint32 SPREAD_au32Counter[SPREAD_nKernelSpreadCount + SPREAD_nUserSpreadCount];
SPREADAPI_tstSpreadResult SPREAD_astSpreadResult[SPREAD_nKernelSpreadCount];	

/* Private function declarations
   ----------------------------*/

/* Public function definitions
   ---------------------------*/	 
void SPREADS_vStart(puint32 const pu32Arg)
{
	uint32 u32IDX;
	
	for (u32IDX = 0; u32IDX < (SPREAD_nKernelSpreadCount + SPREAD_nUserSpreadCount); u32IDX++)
	{
		SPREAD_apstSpreadCB[u32IDX] = NULL;
	}
}

void SPREADS_vRun(puint32 const pu32Arg)
{
	
	
}

void SPREADS_vTerminate(puint32 const pu32Arg)
{
	
	
}

SPREADAPI_ttSpreadIDX SPREAD_tRequestKernelSpread(SPREADAPI_tstSpreadCB* pstSpreadCBReq)
{
	SPREADAPI_tstSpreadCB* pstSpreadCB;
	uint32 u32SpreadIDX;
	SPREADAPI_ttSpreadIDX tSpreadIDX = -1;
	
	/* Request a Kernel managed spread */
	for (u32SpreadIDX = 0; SPREAD_nKernelSpreadCount > u32SpreadIDX; u32SpreadIDX++)
	{
		if (NULL == SPREAD_apstSpreadCB[u32SpreadIDX])
		{
			pstSpreadCB = &SPREAD_astSpreadCB[u32SpreadIDX];
			SPREAD_apstSpreadCB[u32SpreadIDX] = pstSpreadCB;
			memcpy((void*)pstSpreadCB, (void*)pstSpreadCBReq, sizeof(SPREADAPI_tstSpreadCB));	
			tSpreadIDX = u32SpreadIDX;
			break;
		}			
	}
	
	return tSpreadIDX;	
}


Bool SPREAD_vCalculate(SPREADAPI_ttSpreadIDX tSpreadIDX)
{
	SPREADAPI_tstSpreadCB* pstSpreadCB;
	puint32 pu32Spread; puint16 pu16Spread; puint8 pu8Spread;
	psint32 ps32Spread; psint16 ps16Spread; psint8 ps8Spread;
	uint16 u16SpreadIDX; uint16 u16OldSpreadIDX; uint16 u16TempSpreadIDX;
	uint32 u32Source;
	uint16 u16Source;
	sint32 s32Source; sint16 s16Source;
	sint32 s32Step;
	sint32 s32Increment;
	uint32 u32Shift = 0;
	Bool boResultFound = false;

	if ((0 <= tSpreadIDX) &&
	   ((SPREAD_nKernelSpreadCount + SPREAD_nUserSpreadCount) > tSpreadIDX))
    {
		pstSpreadCB = SPREAD_apstSpreadCB[tSpreadIDX];
	
		switch (pstSpreadCB->enDataType)
		{
			{
			case TYPE_enUInt8:
				u32Source = *(puint32)(pstSpreadCB->pvSourceData);
				pu8Spread = (puint8)pstSpreadCB->pvSpreadData;
				break;			
			}
			case TYPE_enUInt16:
			{
				u16Source = *(puint16)(pstSpreadCB->pvSourceData);
				pu16Spread = (puint16)pstSpreadCB->pvSpreadData;	

				if (u16Source < *pu16Spread)
				{
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = 0;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0;				
				}
				else if (u16Source >= *(pu16Spread + (pstSpreadCB->s16SpreadSize - 1)))
				{
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = pstSpreadCB->s16SpreadSize - 2;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0xffff;					
				}
				else
				{				
					/* Start bisection search */
					u16SpreadIDX = (pstSpreadCB->s16SpreadSize) / 2;
					u16OldSpreadIDX = pstSpreadCB->s16SpreadSize - 1;
				
					while (false == boResultFound)
					{
						if (u16Source == *(pu16Spread + u16SpreadIDX))
						{
							SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX;
							SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0;	
							boResultFound = true;
						}
						else if (u16Source < *(pu16Spread + u16SpreadIDX))
						{						
							if (u16Source >= *(pu16Spread + u16SpreadIDX - 1))
							{
								SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX - 1;							
								boResultFound = true;
							}
							else
							{
								/* Move left */
								if (u16OldSpreadIDX < u16SpreadIDX)
								{
									/* Was moving right */
									u16TempSpreadIDX = u16OldSpreadIDX;
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + u16TempSpreadIDX) / 2;	
								}	
								else
								{
									/* Was moving left */
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = u16SpreadIDX / 2;	
								}	
							}
						}
						else if (u16Source >= *(pu16Spread + u16SpreadIDX))
						{						
							if (u16Source <= *(pu16Spread + u16SpreadIDX + 1))							
							{
								SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX;							
								boResultFound = true;
							}	
							else
							{
								/* Move right */
								if (u16OldSpreadIDX > u16SpreadIDX)
								{					
									/*	Was moving left */					
									u16TempSpreadIDX = u16OldSpreadIDX;
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + u16TempSpreadIDX) / 2;	
								}
								else
								{
									/* Was moving right */
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + pstSpreadCB->s16SpreadSize) / 2;
								}	
							}					
						}
					}
				
					s32Step = *(ps32Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex + 1) 
									- *(ps32Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex);

					while (0xffff < s32Step)
					{
						u32Shift++;
						s32Step = s32Step >> 1;
					}
				
					s32Increment = s32Source - *(pu16Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex);
					s32Increment = s32Increment >> u32Shift;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = (0x10000 * (uint32)s32Increment) / (uint32)s32Step;				
				}		
				break;		
			}
			case TYPE_enUInt32:
			{
				u32Source = *(puint32)(pstSpreadCB->pvSourceData);
				pu32Spread = (puint32)pstSpreadCB->pvSpreadData;		
			
				if (u32Source < *pu32Spread)
				{
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = 0;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0;				
				}
				else if (u32Source >= *(pu32Spread + (pstSpreadCB->s16SpreadSize - 1)))
				{
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = pstSpreadCB->s16SpreadSize - 2;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0xffff;					
				}
				else
				{				
					/* Start bisection search */
					u16SpreadIDX = (pstSpreadCB->s16SpreadSize) / 2;
					u16OldSpreadIDX = pstSpreadCB->s16SpreadSize - 1;
				
					while (false == boResultFound)
					{
						if ((u32Source == *(pu32Spread + u16SpreadIDX)))
						{
							SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX;
							SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0;	
							boResultFound = true;
						}
						else if (u32Source < *(pu32Spread + u16SpreadIDX))
						{						
							if (u32Source >= *(pu32Spread + u16SpreadIDX - 1))
							{
								SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX - 1;							
								boResultFound = true;
							}
							else
							{
								/* Move left */
								if (u16OldSpreadIDX < u16SpreadIDX)
								{
									/* Was moving right */
									u16TempSpreadIDX = u16OldSpreadIDX;
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + u16TempSpreadIDX) / 2;	
								}	
								else
								{
									/* Was moving left */
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = u16SpreadIDX / 2;	
								}	
							}
						}
						else if (u32Source >= *(pu32Spread + u16SpreadIDX))
						{						
							if (u32Source <= *(pu32Spread + u16SpreadIDX + 1))							
							{
								SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX;							
								boResultFound = true;
							}	
							else
							{
								/* Move right */
								if (u16OldSpreadIDX > u16SpreadIDX)
								{					
									/*	Was moving left */					
									u16TempSpreadIDX = u16OldSpreadIDX;
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + u16TempSpreadIDX) / 2;	
								}
								else
								{
									/* Was moving right */
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + pstSpreadCB->s16SpreadSize) / 2;
								}	
							}					
						}
					}
				
					s32Step = *(pu32Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex + 1) 
									- *(pu32Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex);

					while (0xffff < s32Step)
					{
						u32Shift++;
						s32Step = s32Step >> 1;
					}
				
					s32Increment = u32Source - *(pu32Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex);
					s32Increment = s32Increment >> u32Shift;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = (0x10000 * (uint32)s32Increment) / (uint32)s32Step;				
				}
				break;	
			}
		
			case TYPE_enInt8:
			{
				s32Source = *(psint8)(pstSpreadCB->pvSourceData);	
				ps8Spread = (psint8)pstSpreadCB->pvSpreadData;
				break;	
			}
		
			case TYPE_enInt16:
			{
				s16Source = *(psint16)(pstSpreadCB->pvSourceData);	
				ps16Spread = (psint16)pstSpreadCB->pvSpreadData;		
				break;	
			}
		
			case TYPE_enInt32:
			{
				s32Source = *(psint32)(pstSpreadCB->pvSourceData);	
				ps32Spread = (psint32)pstSpreadCB->pvSpreadData;		
		
				if (s32Source < *ps32Spread)
				{
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = 0;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0;				
				}
				else if (s32Source >= *(ps32Spread + (pstSpreadCB->s16SpreadSize - 1)))
				{
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = pstSpreadCB->s16SpreadSize - 2;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0xffff;					
				}
				else
				{				
					/* Start bisection search */
					u16SpreadIDX = (pstSpreadCB->s16SpreadSize) / 2;
					u16OldSpreadIDX = pstSpreadCB->s16SpreadSize - 1;
				
					while (false == boResultFound)
					{
						if ((s32Source == *(ps32Spread + u16SpreadIDX)))
						{
							SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX;
							SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = 0;	
							boResultFound = true;
						}
						else if (s32Source < *(ps32Spread + u16SpreadIDX))
						{						
							if (s32Source >= *(ps32Spread + u16SpreadIDX - 1))
							{
								SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX - 1;							
								boResultFound = true;
							}
							else
							{
								/* Move left */
								if (u16OldSpreadIDX < u16SpreadIDX)
								{
									/* Was moving right */
									u16TempSpreadIDX = u16OldSpreadIDX;
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + u16TempSpreadIDX) / 2;	
								}	
								else
								{
									/* Was moving left */
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = u16SpreadIDX / 2;	
								}	
							}
						}
						else if (s32Source >= *(ps32Spread + u16SpreadIDX))
						{						
							if (s32Source <= *(ps32Spread + u16SpreadIDX + 1))							
							{
								SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex = u16SpreadIDX;							
								boResultFound = true;
							}	
							else
							{
								/* Move right */
								if (u16OldSpreadIDX > u16SpreadIDX)
								{					
									/*	Was moving left */					
									u16TempSpreadIDX = u16OldSpreadIDX;
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + u16TempSpreadIDX) / 2;	
								}
								else
								{
									/* Was moving right */
									u16OldSpreadIDX = u16SpreadIDX;
									u16SpreadIDX = (u16SpreadIDX + pstSpreadCB->s16SpreadSize) / 2;
								}	
							}					
						}
					}
				
					s32Step = *(ps32Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex + 1) 
									- *(ps32Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex);

					while (0xffff < s32Step)
					{
						u32Shift++;
						s32Step = s32Step >> 1;
					}
				
					s32Increment = s32Source - *(ps32Spread + SPREAD_astSpreadResult[tSpreadIDX].u16SpreadIndex);
					s32Increment = s32Increment >> u32Shift;
					SPREAD_astSpreadResult[tSpreadIDX].u16SpreadOffset = (0x10000 * (uint32)s32Increment) / (uint32)s32Step;				
				}
				break;	
			}
		}
	}

	/* TODO suppress warnings */
	(void)pu8Spread;
	(void)ps16Spread;
	(void)ps8Spread;
	(void)s16Source;
		
	OS_stSVCDataStruct.pvData = (void*)&SPREAD_astSpreadResult;
	
	return boResultFound;
}

SPREADAPI_tstSpreadResult SPREAD_stGetSpread(SPREADAPI_ttSpreadIDX tSpreadIDX)
{
	return SPREAD_astSpreadResult[tSpreadIDX];
}

SPREADAPI_tstSpreadResult* SPREAD_pstGetSpread(SPREADAPI_ttSpreadIDX tSpreadIDX)
{
	return &SPREAD_astSpreadResult[tSpreadIDX];
}
