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
#include "CPUAbstract.h"
#include "regset.h"
#include "sys.h"
#include "declarations.h"
#include "os.h"
#include "PIMHA.h"
#include "SIM.h"
#include "PIMAPI.h"
#include "regset.h"
#include "MATH.h"
#include "IOAPI.h"

const REGSET_tstReg32Val PIMHA_rastPIMReg32Val[] = PIMHA_nReg32Set;
const tstPortModule* const PIMHA_rapstPIMPortTypeMap[] = PIMHA_nPortPointerMap;

#ifdef BUILD_MK60
const tstGPIOModule* const PIMHA_rapstPIMGPIOTypeMap[] = PIMHA_nGPIOPointerMap;
#endif //BUILD_MK60

const uint32 PIMHA_rau32PIMClockMasks[] = PIMHA_nPortClockMasks;

uint32 PIM_u32PortClockRequested;

void PIMHA_vStart(uint32* const u32Stat)
{
#ifdef BUILD_MK60
	/* Request Kernel required port clocks */
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTA_MASK);
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTB_MASK);
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTC_MASK);
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTD_MASK);	/* TODO not kernel required! */
	PIMHA_xRequestPortClock(SIM_SCGC5_PORTE_MASK);	
#endif

#ifdef BUILD_SAM3X8E
	PIMHA_xRequestPortClock(PIOA_IRQn);
	PIMHA_xRequestPortClock(PIOB_IRQn);
	PIMHA_xRequestPortClock(PIOC_IRQn);
	PIMHA_xRequestPortClock(PIOD_IRQn);
#endif //BUILD_SAM3X8E
	
	REGSET_vInitReg32(&PIMHA_rastPIMReg32Val[0]);
}


void PIMHA_vInitPortBit(PIMAPI_tenPort enPort, uint32 u32PortBit)
{
	uint32 u32Temp = MATH_u32MaskToIDX(u32PortBit);
	
	if (0 == (PIMHA_rau32PIMClockMasks[enPort] & PIM_u32PortClockRequested))
	/* Request the port clock if not yet requested */
	{
		PIMHA_xRequestPortClock(PIM_rau32PIMClockMasks[enPort]);
	}
	
	PIMHA_vSetPortMux(enPort, u32PortBit, 1);
}

void PIMHA_vAssertPortBit(PIMAPI_tenPort enPort, uint32 u32PortBit, IOAPI_tenTriState enTriState)
{	
	switch (enTriState)
	{
		case IOAPI_enLow:
		{
#ifdef BUILD_MK60
			volatile tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PDDR |= u32PortBit;
			pstGPIO->PCOR = u32PortBit;
#endif //BUILD_MK60	
			break;			
		}
		case IOAPI_enHigh:
		{
#ifdef BUILD_MK60
			volatile tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PDDR |= u32PortBit;
			pstGPIO->PSOR = u32PortBit;
#endif //BUILD_MK60	
			break;			
		}		
		case IOAPI_enToggle:
		{
#ifdef BUILD_MK60
			volatile tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PDDR |= u32PortBit;
			pstGPIO->PTOR = u32PortBit;
#endif //BUILD_MK60	
			break;			
		}	
		case IOAPI_enHiZ:
		{
#ifdef BUILD_MK60
			volatile tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];
			pstGPIO->PDDR &= ~u32PortBit;
			pstGPIO->PCOR = u32PortBit;
#endif //BUILD_MK60	
			break;					
		}				
	}
}

bool PIMHA_boGetPortBitState(PIMAPI_tenPort enPort, uint32 u32PortBit)
{
	bool boBitHigh = false;

#ifdef BUILD_MK60
	volatile tstGPIOModule* pstGPIO = (tstGPIOModule*)PIMHA_rapstPIMGPIOTypeMap[enPort];	
	boBitHigh = 0 < (pstGPIO->PDIR & u32PortBit);
#endif //BUILD_MK60	

	return boBitHigh;	
}

void PIMHA_vSetPortMux(PIMAPI_tenPort enPort, uint32 u32PortBit, uint32 u32MuxSel)
{
	//uint32 u32Temp = MATH_u32MaskToIDX(u32PortBit);	

#ifdef BUILD_MK60
	volatile tstPortModule* pstPort = (tstPortModule*)PIMHA_rapstPIMPortTypeMap[enPort];
	uint32 u32RegOffset = offsetof(tstPortModule, PCR[u32Temp]);
	*(puint32)((uint32)pstPort + (uint32)u32RegOffset) &= ~PORT_PCR_MUX_MASK;
	*(puint32)((uint32)pstPort + (uint32)u32RegOffset) |= PORT_PCR_MUX(u32MuxSel);	
#endif //BUILD_MK60		
}




			
			
