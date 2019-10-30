/******************************************************************************/
/*                Copyright (c) 2016 MD Automotive Controls.                  */
/*                         All rights reserved.                               */
/******************************************************************************/
/*                                                                            */
/* PACKAGE TITLE:      Build Header File                                      */
/* DESCRIPTION:        This code module provides build options for compiling  */
/*                     the project to PBL, SBL, Flash Kernel, RAM Kernel+APP  */
/*                     Debugging or Flash User APP and WIFI/ETHERNET switch   */
/* FILE NAME:          BUILD.h                                                */
/* REVISION HISTORY:   07-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
#ifndef BUILD_H
#define BUILD_H


/* GLOBAL MACRO DEFINITIONS ***************************************************/
#ifdef BUILD_PBL
/* Build the Primary BootLoader to be resident in FLASH @ 0x00000000 */
	#define BUILD_DL_ADDR_MIN 0x20000000
	#define BUILD_DL_ADDR_MAX 0x2000ffff
	#define BUILD_KERNEL_OR_KERNEL_APP 0	
	#define BUILD_USER 0	
#endif	

#ifdef BUILD_SBL
/* Build the Secondary BootLoader to be resident in RAM @ 0x20000000 */
	#define BUILD_DL_ADDR_MIN 0x00010000
	#define BUILD_DL_ADDR_MAX 0x0007ffff
	#define BUILD_KERNEL_OR_KERNEL_APP 0
	#define BUILD_USER 0	
#endif	

#ifdef BUILD_UBL
/* Build the User BootLoader to be resident in RAM @ 0x20000000 */
	#define BUILD_DL_ADDR_MIN 0x00020000
	#define BUILD_DL_ADDR_MAX 0x0002ffff
	#define BUILD_KERNEL_OR_KERNEL_APP 0	
	#define BUILD_USER 0	
#endif	

#ifdef BUILD_KERNEL
/* Build the KERNEL for release to be resident in FLASH @ 0x00010000 */
	#define BUILD_DL_ADDR_MIN 0x00070000
	#define BUILD_DL_ADDR_MAX 0x0007ffff
	#define BUILD_KERNEL_OR_KERNEL_APP 1
	#define BUILD_USER 0	
#endif	

#ifdef BUILD_KERNEL_APP
/* Build the KERNEL and APP for testing to be resident in RAM @ 0x20000000 */
	#define BUILD_DL_ADDR_MIN 0x00070000
	#define BUILD_DL_ADDR_MAX 0x0007ffff
	#define BUILD_KERNEL_OR_KERNEL_APP 1	
	#define BUILD_USER 1	
#endif	

#ifdef BUILD_USER_APP
/* Build the User APP for release be resident in FLASH @ 0x00050000 */
	#define BUILD_DL_ADDR_MIN 0x00070000
	#define BUILD_DL_ADDR_MAX 0x0007ffff
	#define BUILD_KERNEL_OR_KERNEL_APP 0	
	#define BUILD_USER 1	
#endif	

#define PBL_WORD_ADDR		            0x2000fffc
#define RAM_START_ADDR					0x20000000
#define RAM_END_ADDR                    0x2008ffff

#define COMMS_ETH_WIRED 0
#define COMMS_ETH_WIFI 1
#define LSU_DEVICE_COUNT				1u
#define BUILD_ALLOW_CRITICAL			1u

/* Board support package options */

/* Defined sensor types */
#define BUILD_BSP_CTS_CAN
#define BUILD_BSP_ATS_CAN
#define BUILD_BSP_TPS_CAN
#define BUILD_BSP_PPS_CAN
#define BUILD_BSP_VSS_CAN
#define BUILD_BSP_TPS_ANALOG
#define BUILD_BSP_MAP_ANALOG
#define BUILD_BSP_AFM_FREQ
#define BUILD_BSP_STT_CAN
#define BUILD_BSP_EST_ADV_OBD 

/* Undefined sensor types */
#define BUILD_BSP_CTS_ANALOG
#define BUILD_BSP_ATS_ANALOG
//#define BUILD_BSP_CTS_PULLUP_SENSE
#define BUILD_BSP_IAC_STEPPER

#endif //BUILD_H

