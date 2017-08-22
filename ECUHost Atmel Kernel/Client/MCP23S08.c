/******************************************************************************/
/*                Copyright (c) 2017 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      MCP23S08                                               */
/* DESCRIPTION:        This code module provides functions to invoke the SPI  */
/*                     data transfers to/from the MCP23S08 device             */
/*                                                                            */
/* FILE NAME:          MCP23S08.c                                             */
/* REVISION HISTORY:   11-01-2017 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER

#include "MCP23S08.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/
uint8 MCP23S08_au8TXData[MCP23S08_TXRXBufferSize];/*CR1_71*/
uint8 MCP23S08_au8RXData[MCP23S08_TXRXBufferSize];/*CR1_71*/
IOAPI_tenEHIOResource MCP23S08_enIOResource;
bool MCP23S08_boBusy;
MCP23S08_tpfCB MCP23S08_pfCallBack;


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/
/*******************************************************************************
* Interface        : MCP23S08_vNotifyCB
*
* Implementation   : Function called when the MCP23S08 data transfer is complete
*
* Parameter
*    Par1          : enEHIOResource enum of the IIC resource used
*    Par2          : enTransferType enum of the R/W mode
*    Par3          : pvData pointer to data read
*
* Return Value     : NIL
*******************************************************************************/
static void MCP23S08_vNotifyCB(IOAPI_tenEHIOResource enEHIOResource, void* pvData, uint32 u32ByteCount, IOAPI_enTransferType enTransferType);/*CR1_74*/

/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void MCP23S08_vStart(puint32 const pu32Arg)
{
	MCP23S08_enIOResource = EH_VIO_SPI1;
	MCP23S08_boBusy = FALSE;
	MCP23S08_pfCallBack = NULL;
}

void MCP23S08_vRun(puint32 const pu32Arg)
{

}

void MCP23S08_vTerminate(puint32 const pu32Arg)
{

}

Bool MCP23S08_boSetCallback(MCP23S08_tpfCB pfCB)
{
    Bool boRetVal = FALSE;

	if (NULL == MCP23S08_pfCallBack)
	{
	    MCP23S08_pfCallBack = pfCB;
		boRetVal = TRUE;
	}
}

Bool MCP23S08_boTransferData(IOAPI_enTransferType enTransferType, void* pvData, uint32 u32DataCount)
{
	IOAPI_tstTransferCB stTransferCB;
	bool boRetVal = true;
	
	if ((FALSE == MCP23S08_boBusy) && ((MCP23S08_TXRXBufferSize - 2) >= u32DataCount))
	{
		if (IOAPI_IO_TX == enTransferType)
		{
			if (NULL != pvData)/*CR1_72*/
			{
				memcpy((void*)&MCP23S08_au8TXData[2], pvData, u32DataCount);
				MCP23S08_au8TXData[0] = MCP23S08_nCmdWrite;
				MCP23S08_au8TXData[1] = MCP23S08_nRegOLAT;		
			}				
		}
		
		stTransferCB.enEHIOResource = MCP23S08_enIOResource;
		stTransferCB.enTransferType = enTransferType;
		stTransferCB.pfCB = &MCP23S08_vNotifyCB;
		stTransferCB.pvData	= (void*)&MCP23S08_au8TXData[0];
		stTransferCB.u32ByteCount = u32DataCount + 2;
		
		USER_vSVC(SYSAPI_enRequestIOBusTransfer, (void*)EH_VIO_SPI1, (void*)&stTransferCB, NULL);	
	    MCP23S08_boBusy = TRUE;					
	}
	else
	{
		boRetVal = false;
	}
	
	return boRetVal;
}

void MCP23S08_vSetDataDirection(uint8 u8DirMask1)
{
	IOAPI_tstTransferCB stTransferCB;
	bool boRetVal = true;
	
	if (FALSE == MCP23S08_boBusy)
	{
		MCP23S08_au8TXData[0] = MCP23S08_nCmdWrite;
		MCP23S08_au8TXData[1] = MCP23S08_nRegIODIR;	
		MCP23S08_au8TXData[2] = u8DirMask1;			
		
		stTransferCB.enEHIOResource = MCP23S08_enIOResource;
		stTransferCB.enTransferType = IOAPI_IO_TX;
		stTransferCB.pfCB = &MCP23S08_vNotifyCB;
		stTransferCB.pvData	= (void*)&MCP23S08_au8TXData[0];
		stTransferCB.u32ByteCount = 3;	
		
		USER_vSVC(SYSAPI_enRequestIOBusTransfer, (void*)EH_VIO_SPI1, (void*)&stTransferCB, NULL);	
	    MCP23S08_boBusy = TRUE;
	}
}

void MCP23S08_vThread(void)
{
	while(1);	
}

void MCP23S08_vCallBack(puint32 const pu32Arg)
{

}

static void MCP23S08_vNotifyCB(IOAPI_tenEHIOResource enEHIOResource, void* pvData, uint32 u32ByteCount, IOAPI_enTransferType enTransferType)
{
    uint32 u32Flags;

	if (MCP23S08_enIOResource == enEHIOResource)
	{
		MCP23S08_boBusy = FALSE;

		if (NULL != MCP23S08_pfCallBack)
		{
		    MCP23S08_pfCallBack(&u32Flags);
		}
	}
}

#endif //BUILD_USER