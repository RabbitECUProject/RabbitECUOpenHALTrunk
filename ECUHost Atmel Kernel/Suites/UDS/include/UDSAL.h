/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      XXX Header File                                        */
/* DESCRIPTION:        XXX                                                    */
/* FILE NAME:          XXX.h                                                  */
/* REVISION HISTORY:   19-08-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef UDSAL_H
#define UDSAL_H

#include "ADC.h"
#include "build.h"
#include "client.h"
#include "COMMONNL.h"
#include "CQUEUE.h"
#include "declarations.h"
//#include "DIAG.h"
#include "DIAGAPI.h"
#include "DLL.h"
#include "FEE.h"
#include "KERNELDIAG.h"
#include "CPUAbstract.h"
#include "MSG.h"
#include "OS.h"
#include "types.h"
#include "UDSNL.h"
#include "UDSSL.h"


#define UDSAL_COMMON_BUFF_SIZE 		21
#define UDSAL_READ_TRANSFER_SIZE	32
#define UDSAL_WRITE_TRANSFER_SIZE	32

#if	defined(BUILD_PBL)
	#define UDSAL_DL_BUFF_SIZE 				32768
#else
	#define UDSAL_DL_BUFF_SIZE 				1
#endif


#define UDSAL_DDDI_CID_START	0x9000
#define UDSAL_DDDI_CID_END		0x9FFF

#if BUILD_KERNEL_APP
	#define UDSAL_nDDDICacheCount	4
	#define UDSAL_nDDDICacheSize	0x100	
#elif BUILD_SBL
	#define UDSAL_nDDDICacheCount 1
	#define UDSAL_nDDDICacheSize	0x150
#elif BUILD_PBL
	#define UDSAL_nDDDICacheCount 1
	#define UDSAL_nDDDICacheSize	0x80
#endif	

//UDS supported services
#define UDSAL_SID_MODE1				0x01
#define UDSAL_SID_MODE2				0x02
#define UDSAL_SID_MODE3				0x03
#define UDSAL_SID_MODE4				0x04
#define UDSAL_SID_MODE6				0x06
#define UDSAL_SID_MODE7				0x07
#define UDSAL_SID_DSC 				0x10
#define UDSAL_SID_ER					0x11
#define UDSAL_SID_RDTCREC			0x12
#define UDSAL_SID_CLRDTCINF		0x14
#define UDSAL_SID_RDTCSTAT		0x17
#define UDSAL_SID_RSTATBYDTC	0x18
#define UDSAL_SID_RECUID			0x1A
#define UDSAL_SID_STPUDSAL		0x20
#define UDSAL_SID_RDBI				0x22
#define UDSAL_SID_RMBA				0x23
#define UDSAL_SID_SA					0x27
#define UDSAL_SID_CC					0x28
#define UDSAL_SID_DDDI				0x2C
#define UDSAL_SID_WDBI				0x2E
#define UDSAL_SID_IOBCID			0x2F
#define UDSAL_SID_IOBLID			0x30
#define UDSAL_SID_RC					0x31
#define UDSAL_SID_RD					0x34
#define UDSAL_SID_RU					0x35
#define UDSAL_SID_TD					0x36
#define	UDSAL_SID_RTE					0x37
#define UDSAL_SID_WMBA				0x3D
#define UDSAL_SID_TP					0x3E
#define UDSAL_SID_CNTRDTC			0x85
#define UDSAL_SID_INSTRSTAT		0xA2
#define UDSAL_SID_INSTRBLID 	0xA5
#define UDSAL_SID_RBDYNLID		0xAA
#define UDSAL_SID_INSTRBCID		0xB2
#define UDSAL_SID_COUNT				34

//UDS subservice
#define UDSAL_SSID_RC_STR			0x01
#define UDSAL_SSID_RC_STPR		0x02
#define UDSAL_SSID_RC_RRR			0x03

#define UDSAL_DDDI_SSID_DMBA	0x02

//UDS routine user IDs
#define UDSAL_RCUID_RUN_DL			0x9000
#define UDSAL_RCUID_PARTITION		0x9001
#define UDSAL_RCUID_WORK_TO_NVM	0x9002
#define UDSAL_RCUID_CLEAR_NVM		0x9003
#define UDSAL_RCUID_BCKP_ADCCAL	0x9080

//UDS response code
#define UDSAL_RSP_GR					0x10
#define UDSAL_RSP_SNS					0x11
#define UDSAL_RSP_SFNS				0x12
#define UDSAL_RSP_IMLOIF			0x13
#define UDSAL_RSP_RTL					0x14
#define UDSAL_RSP_BRR					0x21
#define UDSAL_RSP_CNC					0x22
#define UDSAL_RSP_RSE					0x24
#define UDSAL_RSP_NRFSC				0x25
#define UDSAL_RSP_FPEORA			0x26
#define UDSAL_RSP_ROOR				0x31
#define UDSAL_RSP_SAD					0x33
#define UDSAL_RSP_IK					0x35
#define UDSAL_RSP_ENOA				0x36
#define UDSAL_RSP_RTDNE				0x37
#define UDSAL_RSP_TDS					0x71
#define UDSAL_RSP_GPF					0x72
#define UDSAL_RSP_WBSC				0x73
#define UDSAL_RSP_RCRRP				0x78
#define UDSAL_RSP_SFNSIAS			0x7E
#define UDSAL_RSP_SNSIAS			0x7F
#define UDSAL_RSP_NSUPINSESS	0x80
#define UDSAL_RSP_SUPPRESS		0xFD
#define UDSAL_RSP_FLOW				0xFE
#define UDSAL_RSP_OK					0xFF

//buffer index breakdown
#define UDSAL_IDX_LEN				0x00
#define UDSAL_IDX_SID				0x02
#define UDSAL_IDX_SUBSID		0x03
#define UDSAL_IDX_RW_CID		0x04
#define UDSAL_IDX_RW_DATA		0x05

//diag MSG IDs
#define UDSAL_ID_UDSAL				0x0C6
#define UDSAL_ID_UDSAL_EXT		false
#define UDSAL_ID_FUNC				0x7DF

typedef uint8 (*pfUDSALProcess)(puint8, uint32, puint8, puint32, uint32);

typedef enum
{
	UDSAL_enPBLContext,
	UDSAL_enSBLContext,
	UDSAL_enAppContext
} UDSAL_tenContext;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

typedef struct
{
	uint8 u8Service;
	uint8 u8BytesMin;
	uint8 u8BytesMax;
	uint8 u8BytesResp;
	UDSAL_tenContext enContext;
	pfUDSALProcess pfProcess;
} UDSAL_tstALProcessInfo;

#pragma GCC diagnostic pop

typedef struct
{
	COMMONNL_tstRXLargeBuffer* pstSourceBuffer;
	uint8* pu8TargetAddress;
	uint32 u32AccumulateCount;
	uint32 u32WriteCount;	
	bool boPending;
	bool boComplete;
} UDSAL_tstDLControlBlock;


#define UDSAL_nALProcessInfo																															\
{																																													\
	{ UDSAL_SID_MODE1 				, 0, 0, 6, 		UDSAL_enPBLContext,	UDSAL_u8MODE1 },						\
	{ UDSAL_SID_MODE2 				, 0, 0, 6, 		UDSAL_enPBLContext,	UDSAL_u8MODE2 },						\
	{ UDSAL_SID_MODE3 				, 0, 0, 6, 		UDSAL_enPBLContext,	UDSAL_u8MODE3 },						\
	{ UDSAL_SID_MODE4 				, 0, 0, 0, 		UDSAL_enPBLContext,	UDSAL_u8MODE4 },						\
	{ UDSAL_SID_MODE6 				, 0, 0, 0, 		UDSAL_enPBLContext,	UDSAL_u8MODE6 },						\
	{ UDSAL_SID_MODE7 				, 0, 0, 0, 		UDSAL_enPBLContext,	UDSAL_u8MODE7 },						\
	{ UDSAL_SID_DSC 				, 0, 0, 0, 		UDSAL_enPBLContext,	UDSAL_u8SESSION },						\
	{ UDSAL_SID_ER					, 0, 0, 0, 		UDSAL_enPBLContext,	UDSAL_u8RESET },							\
	{ UDSAL_SID_RDTCREC			, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_CLRDTCINF		, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_RDTCSTAT		, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_RSTATBYDTC	, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_RECUID			, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_STPUDSAL		, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_RDBI				, 0, 0, 0xff, UDSAL_enPBLContext,	UDSAL_u8RDBI },								\
	{ UDSAL_SID_RMBA				, 0, 0, 0xff, UDSAL_enPBLContext,	UDSAL_u8RMBA },								\
	{ UDSAL_SID_WMBA				, 0, 0, 0xff, UDSAL_enPBLContext,	UDSAL_u8WMBA },								\
	{ UDSAL_SID_SA					, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_CC					, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_DDDI				, 0, 0, 4, 		UDSAL_enAppContext,	UDSAL_u8DDDI },								\
	{ UDSAL_SID_WDBI				, 0, 0, 3, 		UDSAL_enPBLContext,	UDSAL_u8WDBI },								\
	{ UDSAL_SID_IOBCID			, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_IOBLID			, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_RC					, 0, 0, 5, 		UDSAL_enPBLContext,	UDSAL_u8RC },									\
	{ UDSAL_SID_RD					, 0, 0, 4, 		UDSAL_enPBLContext,	UDSAL_u8DLREQ },							\
	{ UDSAL_SID_RU					, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_TD					, 0, 0, 2, 		UDSAL_enPBLContext,	UDSAL_u8TFREQ },							\
	{	UDSAL_SID_RTE					, 0, 0, 3, 		UDSAL_enPBLContext,	UDSAL_u8TFEXREQ },						\
	{ UDSAL_SID_TP					, 0, 0, 2, 		UDSAL_enPBLContext,	UDSAL_u8TP },									\
	{ UDSAL_SID_CNTRDTC			, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_INSTRSTAT		, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_INSTRBLID 	, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_RBDYNLID		, 0, 0, 0, 		UDSAL_enPBLContext,	NULL },												\
	{ UDSAL_SID_INSTRBCID		, 0, 0, 0, 		UDSAL_enPBLContext,	NULL }												\
}

#define UDSAL_nMode1InfoByteOffset	\
{\
	0,\
	4,\
	8,\
	10,\
	12,\
	13,\
	14,\
	15,\
	16,\
	17,\
	18,\
	19,\
	20,\
	22,\
	23,\
	24,\
	25,\
	27,\
	28,\
	29,\
	30,\
	32,\
	34,\
	36,\
	38,\
	40,\
	42,\
	44,\
	46,\
	47,\
	48,\
	49,\
	51,\
	55,\
	57,\
	59,\
	61,\
	65,\
	69,\
	73,\
	77,\
	81,\
	85,\
	89,\
	93,\
	94,\
	95,\
	96,\
	97,\
	98,\
	100,\
	102,\
	103,\
	107\
}

#define UDSAL_xCalcBytesFree()																				\
u32TXCap - *pu32TXByteCount - u32TransferByteCount


//public functions
void UDSAL_vProcessBufferCB(puint8, uint32, puint8, puint32, uint32);
void UDSAL_vStart(puint32 const);
void UDSAL_vRun(puint32 const);
void UDSAL_vTerminate(puint32 const);

#endif //UDSAL_H

