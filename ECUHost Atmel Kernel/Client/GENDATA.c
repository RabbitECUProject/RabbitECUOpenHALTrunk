/******************************************************************************/
/*    Copyright (c) 2019 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:USER_APP                                                           */
/* PACKAGE TITLE:      Generic Data Code File                                 */
/* DESCRIPTION:        This code module contains generic data transforms      */
/*                                                                            */
/* FILE NAME:          gendata.c                                              */
/* REVISION HISTORY:   28-06-2019 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/

#define _GENDATA_C

/******************************************************************************/
/* HEADER FILES                                                               */
/******************************************************************************/
#include "build.h"

#if BUILD_USER
#include "GENDATA.h"
#include "AFM.h"
#include "MAP.h"


/* LOCAL VARIABLE DEFINITIONS (STATIC) ****************************************/


/* LOCAL FUNCTION PROTOTYPES (STATIC) *****************************************/

SPREADAPI_ttSpreadIDX GENDATA_tSpreadPWM2DxIDX[GENDATA_nPWM2DTableCount];
SPREADAPI_ttSpreadIDX GENDATA_tSpreadPWM3DxIDX[GENDATA_nPWM3DTableCount];
SPREADAPI_ttSpreadIDX GENDATA_tSpreadPWM3DyIDX[GENDATA_nPWM3DTableCount];

TABLEAPI_ttTableIDX GENDATA_tTablePWMIDX[GENDATA_nPWM2DTableCount];
MAPSAPI_ttMapIDX GENDATA_tMapPWMIDX[GENDATA_nPWM3DTableCount];


/* GLOBAL FUNCTION DEFINITIONS ************************************************/
void GENDATA_vStart(puint32 const pu32Arg)
{
	//uint32 u32TableMapIDX;
	//IOAPI_tenEHIOResource enEHIOResource;
	//IOAPI_tenEHIOType enEHIOType;
	
	/* Request and initialise required Kernel managed spread for PWM table 1 x */
	GENDATA_tSpreadPWM2DxIDX[0] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[0], (void*)&USERCAL_stRAMCAL.aUserPWM2D_1SpreadX, TYPE_enUInt16, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM table 2 x */
	GENDATA_tSpreadPWM2DxIDX[1] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[1], (void*)&USERCAL_stRAMCAL.aUserPWM2D_2SpreadX, TYPE_enUInt16, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 3 x */
	GENDATA_tSpreadPWM2DxIDX[2] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[2], (void*)&USERCAL_stRAMCAL.aUserPWM2D_3SpreadX, TYPE_enUInt16, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 4 x */
	GENDATA_tSpreadPWM2DxIDX[3] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[3], (void*)&USERCAL_stRAMCAL.aUserPWM2D_4SpreadX, TYPE_enUInt16, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 5 x */
	GENDATA_tSpreadPWM2DxIDX[4] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[4], (void*)&USERCAL_stRAMCAL.aUserPWM2D_5SpreadX, TYPE_enUInt16, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 6 x */
	GENDATA_tSpreadPWM2DxIDX[5] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[5], (void*)&USERCAL_stRAMCAL.aUserPWM2D_6SpreadX, TYPE_enUInt16, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 7 x */
	GENDATA_tSpreadPWM2DxIDX[6] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[6], (void*)&USERCAL_stRAMCAL.aUserPWM2D_7SpreadX, TYPE_enUInt16, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);
	
	/* Request and initialise required Kernel managed spread for PWM table 8 x */
	GENDATA_tSpreadPWM2DxIDX[7] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[7], (void*)&USERCAL_stRAMCAL.aUserPWM2D_8SpreadX, TYPE_enUInt16, GENDATA_nPWM2DTableDimX, SPREADAPI_enSpread4ms, NULL);


	/* 2D PWM maps data */
	/* Request and initialise required Kernel managed map for 2D PWM 1 */
	GENDATA_tTablePWMIDX[0] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_1Data, (void*)&GENDATA_u16PWMOutput[0], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[0], NULL);

	/* Request and initialise required Kernel managed map for 2D PWM 2 */
	GENDATA_tTablePWMIDX[1] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_2Data, (void*)&GENDATA_u16PWMOutput[1], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[1], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 3 */
	GENDATA_tTablePWMIDX[2] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_3Data, (void*)&GENDATA_u16PWMOutput[2], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[2], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 4 */
	GENDATA_tTablePWMIDX[3] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_4Data, (void*)&GENDATA_u16PWMOutput[3], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[3], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 5 */
	GENDATA_tTablePWMIDX[4] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_5Data, (void*)&GENDATA_u16PWMOutput[4], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[4], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 6 */
	GENDATA_tTablePWMIDX[5] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_6Data, (void*)&GENDATA_u16PWMOutput[5], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[5], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 7 */
	GENDATA_tTablePWMIDX[6] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_7Data, (void*)&GENDATA_u16PWMOutput[6], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[6], NULL);
	
	/* Request and initialise required Kernel managed map for 2D PWM 8 */
	GENDATA_tTablePWMIDX[7] = SETUP_tSetupTable((void*)&USERCAL_stRAMCAL.aUserPWM2D_8Data, (void*)&GENDATA_u16PWMOutput[7], TYPE_enUInt16, GENDATA_nPWM2DTableDimX, GENDATA_tSpreadPWM2DxIDX[7], NULL);
	

	/* 3D PWM maps X axes */
	/* Request and initialise required Kernel managed spread for PWM map 1 x */
	GENDATA_tSpreadPWM3DxIDX[0] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[8], (void*)&USERCAL_stRAMCAL.aUserPWM3D_1SpreadX, TYPE_enUInt16, GENDATA_nPWM3DTableDimX, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 2 x */
	GENDATA_tSpreadPWM3DxIDX[1] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[9], (void*)&USERCAL_stRAMCAL.aUserPWM3D_2SpreadX, TYPE_enUInt16, GENDATA_nPWM3DTableDimX, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 3 x */
	GENDATA_tSpreadPWM3DxIDX[2] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[10], (void*)&USERCAL_stRAMCAL.aUserPWM3D_3SpreadX, TYPE_enUInt16, GENDATA_nPWM3DTableDimX, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 4 x */
	GENDATA_tSpreadPWM3DxIDX[3] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[11], (void*)&USERCAL_stRAMCAL.aUserPWM3D_4SpreadX, TYPE_enUInt16, GENDATA_nPWM3DTableDimX, SPREADAPI_enSpread4ms, NULL);



	/* 3D PWM maps Y axes */
	/* Request and initialise required Kernel managed spread for PWM map 1 x */
	GENDATA_tSpreadPWM3DyIDX[0] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[12], (void*)&USERCAL_stRAMCAL.aUserPWM3D_1SpreadY, TYPE_enUInt16, GENDATA_nPWM3DTableDimY, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 2 x */
	GENDATA_tSpreadPWM3DyIDX[1] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[13], (void*)&USERCAL_stRAMCAL.aUserPWM3D_2SpreadY, TYPE_enUInt16, GENDATA_nPWM3DTableDimY, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 3 x */
	GENDATA_tSpreadPWM3DyIDX[2] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[14], (void*)&USERCAL_stRAMCAL.aUserPWM3D_3SpreadY, TYPE_enUInt16, GENDATA_nPWM3DTableDimY, SPREADAPI_enSpread4ms, NULL);

	/* Request and initialise required Kernel managed spread for PWM map 4 x */
	GENDATA_tSpreadPWM3DyIDX[3] = SETUP_tSetupSpread((void*)&GENDATA_u16SOURCE[15], (void*)&USERCAL_stRAMCAL.aUserPWM3D_4SpreadY, TYPE_enUInt16, GENDATA_nPWM3DTableDimY, SPREADAPI_enSpread4ms, NULL);



	/* 3D PWM maps data */
	/* Request and initialise required Kernel managed map for 3D PWM 1 */
	GENDATA_tMapPWMIDX[0] = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPWM3D_1Data, (void*)&GENDATA_u16PWMOutput[8], TYPE_enUInt16, 17, 17, GENDATA_tSpreadPWM3DxIDX[0], GENDATA_tSpreadPWM3DyIDX[0], NULL);	
	
	/* Request and initialise required Kernel managed map for 3D PWM 2 */
	GENDATA_tMapPWMIDX[1] = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPWM3D_2Data, (void*)&GENDATA_u16PWMOutput[9], TYPE_enUInt16, 17, 17, GENDATA_tSpreadPWM3DxIDX[1], GENDATA_tSpreadPWM3DyIDX[1], NULL);
	
	/* Request and initialise required Kernel managed map for 3D PWM 3 */
	GENDATA_tMapPWMIDX[2] = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPWM3D_3Data, (void*)&GENDATA_u16PWMOutput[10], TYPE_enUInt16, 17, 17, GENDATA_tSpreadPWM3DxIDX[2], GENDATA_tSpreadPWM3DyIDX[2], NULL);
	
	/* Request and initialise required Kernel managed map for 3D PWM 4 */
	GENDATA_tMapPWMIDX[3] = SETUP_tSetupMap((void*)&USERCAL_stRAMCAL.aUserPWM3D_4Data, (void*)&GENDATA_u16PWMOutput[11], TYPE_enUInt16, 17, 17, GENDATA_tSpreadPWM3DxIDX[3], GENDATA_tSpreadPWM3DyIDX[3], NULL);
}

void GENDATA_vRun(puint32 const pu32Arg)
{


}

void GENDATA_vTerminate(puint32 const pu32Arg)
{

}



#endif //BUILD_USER
