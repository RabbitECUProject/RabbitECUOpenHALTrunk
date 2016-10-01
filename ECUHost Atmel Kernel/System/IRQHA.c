/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                                   */                      
/* PACKAGE TITLE:      IRQ                      															*/
/* DESCRIPTION:        This code is interrupt service routines								*/
/*										 																												*/
/* FILE NAME:          IRQ.c                                          				*/
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#ifndef IRQHA_H
#define IRQHA_H

#include "CPUAbstract.h"
#include <core_cm4.h>
#include <string.h>


MPU_Type IRQ_stMPU;

#if BUILD_KERNEL_OR_KERNEL_APP
extern uint32 OS_u32SysTickInterval;
extern uint32 OS_u32SysRunTimeHigh;
extern uint32 OS_u32SysRunTimeLow;
extern tq_struct* OS_pstBackgroundDispatcherTask;
extern SDHC1_Interrupt(void);
extern void OS_vBackgroundDispatcher(void);
#endif

extern void NVIC_EnableIRQ(IRQn_Type IRQn);
DLL_tstRXDLLData CAN_stRXDLLData;

void IRQ_vEnableIRQ(IRQn_Type enIRQType)
{
	NVIC_EnableIRQ(enIRQType);
}

void IRQ_vDisableIRQ(IRQn_Type enIRQType)
{
	NVIC_DisableIRQ(enIRQType);
}

#if BUILD_KERNEL_OR_KERNEL_APP
__asm task_queue IRQ_vSuspendThread(task_queue* stTaskQueueToSuspend)
{
TASK_PC_OFFSET	EQU	22
TASK_SP_OFFSET	EQU	18
	
STACK_FRAME_BASE_OFFSET EQU	8
STACK_FRAME_SIZE				EQU	32
	
FRAME_R0_OFFSET					EQU	0
FRAME_R1_OFFSET					EQU	4
FRAME_R2_OFFSET					EQU	8
FRAME_R3_OFFSET					EQU	12
FRAME_R12_OFFSET				EQU	16
FRAME_LR_OFFSET					EQU	20
FRAME_PC_OFFSET					EQU	24
FRAME_XPSR_OFFSET				EQU	28
	
MASK_XPSR_THUMB					EQU	16777216
	
;
;	
;	
; get first task in R1
		LDR		R1, [R0]	
	
; get next task in R2
		LDR		R2, [R1]
		B 		TestQueueLast
FindQueueEndLoop
		MOV		R3, R2
		LDR		R2,	[R2]
TestQueueLast
		CMP		R2, #0	
		BNE		FindQueueEndLoop
; queue pointer is in R0, current task R1, end task R3

; change queue pointer to current task->next
		LDR		R2, [R1]
		CMP		R1, R2
		BEQ		ThreadSwapAbort
		STR		R2, [R0]

;	set end task->next to current task
		STR		R1, [R3]
		
; set current task->next to NULL
		MOV		R3, #0
		STR		R3, [R1]
	
; get thread address in R4 MATTHEW
;		MOV		R4, #255
	
; store current task program counter in task structure program counter
;		STR		R4, [R1, #TASK_PC_OFFSET] MATTHEW
	
; update the task saved stack pointer
		MRS		R0, PSP
; MATTHEW for R4 PUSH		NO!!!!!!!
;		ADD		R0, #4
		STR		R0, [R1, #TASK_SP_OFFSET]
	
; get background dispatcher task stack pointer
		LDR		R1, =__cpp(&OS_pstBackgroundDispatcherTask)
		LDR		R1, [R1]
		LDR		R1,	[R1, #TASK_SP_OFFSET]
				
; set	background dispatcher stack frame return address to
; dispatcher entry point always
		LDR		R2, =__cpp(OS_vBackgroundDispatcher)
		ORR		R2, #1
		STR		R2,	[R1, #FRAME_PC_OFFSET]
		
; set XPSR thumb bit always		
		;LDR		R2, [R1, #FRAME_XPSR_OFFSET]
		MOV		R2, #0
		ORR		R2, #MASK_XPSR_THUMB
		STR		R2, [R1, #FRAME_XPSR_OFFSET]
		
; store dispatcher stack pointer to PSP		
		MSR		PSP, R1
		
ThreadSwapAbort		
}
#endif


/*----------------------------------------------------------------------------
  SVC_Handler
 *----------------------------------------------------------------------------*/
#if BUILD_PBL
__asm void SVC_Handler(void) 
{}
#endif	
#if BUILD_SBL
__asm void SVC_Handler(void) 
{}	
#endif	
#if BUILD_KERNEL_OR_KERNEL_APP
__asm void SVC_Handler(void) 
{
	PRESERVE8
	
	IMPORT 	SVC_Handler_Main
	IMPORT 	OS_stSVCDataStruct
	TST			lr, #4
	MRSEQ		r0, MSP
	MRSNE		r0, PSP
	
; load the link register containing user module return address into R0 
; module return address will be used as module call handle - UID which
; infers privilege and is also unique
	LDR			R0, [R0, #24]
	PUSH		{R1}
	LDR			R1, =OS_stSVCDataStruct
	STR			R0, [R1, #4]
	POP			{R1}
	
	TST			lr, #4
	MRSEQ		r0, MSP
	MRSNE		r0, PSP
	B 			SVC_Handler_Main
}
#endif

#if BUILD_KERNEL_OR_KERNEL_APP
SVC_Handler_Main(void* svc_args)
{
	SYS_vAPISVC(svc_args);
}
#endif

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) 
{
#if BUILD_PBL
	PBL_vCyclicTask();
#endif	
#if BUILD_SBL
	SBL_vCyclicTask();	
#endif
#if BUILD_KERNEL_OR_KERNEL_APP
	/* declare data static because are usually consuming
		 a thread stack here for temp data! */
	static uint32 u32SysRunTimeLowOld;
 	static OS_tenOSState enOSState;		
	static uint32 u32Stat;
	static task_queue* pstTaskQueue;	
	static task_queue* pstTaskQueueToSuspend;
	static task_queue stTaskQueue;	
	//static TASK_tStackPointer pStackPointer;
	
	stTaskQueue = NULL;
	pstTaskQueueToSuspend = NULL;	
	
	OS_u32SysRunTimeLow += OS_u32SysTickInterval;
	OS_u32SysRunTimeHigh = (u32SysRunTimeLowOld > OS_u32SysRunTimeLow) ?
													OS_u32SysRunTimeHigh + 1 : OS_u32SysRunTimeHigh;
	u32SysRunTimeLowOld = OS_u32SysRunTimeLow;
	
	/* call this OS module run function from the interrupt context although
	   normally OS module runs are called in co-operative multitask context */
	TIMER_vRun(&u32Stat);

 	enOSState = OS_enGetState();
	
	if (OS_enOSStateOSStarted == enOSState)
	{		
		/* check if kernel cyclic queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enKERNELQueueCyclic);
			stTaskQueue = *pstTaskQueue;
			if(NULL != stTaskQueue)
			{			
				OS_vOverrunTaskReport(OS_enKERNELQueueCyclic);
				/* call schedule - no thread to swap out we are still co-op tasking */				
				OS_vSchedule(OS_enOSSchedulerTick);
			}
		}	
		
		/* check if client cyclic queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enCLIENTQueueCyclic);
			stTaskQueue = *pstTaskQueue;
			if(NULL != stTaskQueue)
			{			
				OS_vOverrunTaskReport(OS_enKERNELQueueCyclic);
				/* call schedule - no thread to swap out we are still co-op tasking */				
				OS_vSchedule(OS_enOSSchedulerTick);
			}
		}			

		/* check if kernel thread queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enKERNELQueueThread);
			stTaskQueue = *pstTaskQueue;
			if(NULL != stTaskQueue)
			{			
				/* swap out thread before calling schedule */
				pstTaskQueueToSuspend = pstTaskQueue;
				OS_vSchedule(OS_enOSSchedulerTick);
			}
		}	

		/* check if client thread queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enCLIENTQueueThread);
			stTaskQueue = *pstTaskQueue;
			if(NULL != stTaskQueue)
			{			
				/* swap out thread before calling schedule */
				pstTaskQueueToSuspend = pstTaskQueue;
				OS_vSchedule(OS_enOSSchedulerTick);
			}
		}
		
		if(NULL != pstTaskQueueToSuspend)
		{
			stTaskQueue = IRQ_vSuspendThread(pstTaskQueueToSuspend);
		}
		
		stTaskQueue = NULL;		
	}
	else if (OS_enOSStartRequest == enOSState)
	{
		OS_vSetState(OS_enOSStateOSStarted);	
	}
#endif	
}


/*----------------------------------------------------------------------------
  MemManage_Handler
 *----------------------------------------------------------------------------*/
void MemManage_Handler(void)
{
	memcpy((void*)&IRQ_stMPU, (void*)MPU_BASE, sizeof(MPU_Type));
	while(1);
}

/*----------------------------------------------------------------------------
  ENET_TX_Handlers
 *----------------------------------------------------------------------------*/
void ENET_Transmit_IRQHandler(void)
{
	
}

/*----------------------------------------------------------------------------
  ENET_RX_Handlers
 *----------------------------------------------------------------------------*/
void ENET_Receive_IRQHandler(void)
{
	
	if(ENE_xIntActivated(ENET_EIMR_BABR_MASK))
	{		
		ENE_xIntClearFlag(ENET_EIMR_BABR_MASK);
	}
	
	if(ENE_xIntActivated(ENET_EIMR_RXF_MASK))
	{		
		ENE_xIntClearFlag(ENET_EIMR_RXF_MASK);
		ENE_vReceiveFrame();		
	}
}

/*----------------------------------------------------------------------------
  ENET_ERR_Handlers
 *----------------------------------------------------------------------------*/
void ENET_Error_IRQHandler(void)
{
	
	
}









/*----------------------------------------------------------------------------
  UART0_Handlers
 *----------------------------------------------------------------------------*/
void UART0_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART0;
	IRQ_vCommonUART(pstUART, EH_VIO_UART1);
}

void UART0_ERR_IRQHandler(void){}
	
/*----------------------------------------------------------------------------
  UART1_Handlers
 *----------------------------------------------------------------------------*/
void UART1_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART1;
	IRQ_vCommonUART(pstUART, EH_VIO_UART2);
}

void UART1_ERR_IRQHandler(void){}

/*----------------------------------------------------------------------------
  UART2_Handlers
 *----------------------------------------------------------------------------*/
void UART2_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART2;
	IRQ_vCommonUART(pstUART, EH_VIO_UART3);
}

void UART2_ERR_IRQHandler(void){}
	
/*----------------------------------------------------------------------------
  UART5_Handlers
 *----------------------------------------------------------------------------*/
void UART3_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART3;
	IRQ_vCommonUART(pstUART, EH_VIO_UART4);
}

void UART3_ERR_IRQHandler(void){}

/*----------------------------------------------------------------------------
  UART4_Handlers
 *----------------------------------------------------------------------------*/
void UART4_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART4;
	IRQ_vCommonUART(pstUART, EH_VIO_UART5);
}

void UART4_ERR_IRQHandler(void){}

/*----------------------------------------------------------------------------
  UART5_Handlers
 *----------------------------------------------------------------------------*/
void UART5_RX_TX_IRQHandler(void)
{
	UART_Type* pstUART = UART5;
	IRQ_vCommonUART(pstUART, EH_VIO_UART6);
}

void UART5_ERR_IRQHandler(void){}	
	
	
/*----------------------------------------------------------------------------
  SDHC_Handler
 *----------------------------------------------------------------------------*/
void SDHC_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP
	SDHC1_Interrupt();
#endif	
}

/*----------------------------------------------------------------------------
  ADCX_Handlers
 *----------------------------------------------------------------------------*/
void ADC0_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP
	ADC_Interrupt(ADC_enADC0);	
#endif	
}

void ADC1_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP	
	ADC_Interrupt(ADC_enADC1);	
#endif	
}

void ADC2_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP
	ADC_Interrupt(ADC_enADC2);
	#endif
}

void ADC3_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP	
	ADC_Interrupt(ADC_enADC3);	
#endif	
}


/*----------------------------------------------------------------------------
  FTMX_Handlers
 *----------------------------------------------------------------------------*/
void FTM0_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP
	FTM_Interrupt(FTM_enFTM0);
#endif	
}

void FTM1_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP
	FTM_Interrupt(FTM_enFTM1);	
#endif	
}

void FTM2_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP
	FTM_Interrupt(FTM_enFTM2);	
#endif	
}

void FTM3_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP
	FTM_Interrupt(FTM_enFTM3);	
#endif	
}

/*----------------------------------------------------------------------------
  IICX_Handlers
 *----------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP	
	IIC_Interrupt(EH_VIO_IIC1);	
#endif
}

void I2C1_IRQHandler(void)
{
#if BUILD_KERNEL_OR_KERNEL_APP	
	IIC_Interrupt(EH_VIO_IIC1);	
#endif	
}
	

void CAN0_ORed_Message_buffer_IRQHandler(void)
{
	IRQ_vCommonCAN(CAN0, EH_VIO_CAN1);
}

void CAN1_ORed_Message_buffer_IRQHandler(void)
{
	CAN_Type* pstCAN = CAN1;
	
	IRQ_vCommonCAN(pstCAN, EH_VIO_CAN2);
}

void 
IRQ_vCommonCAN(CAN_Type* pstCAN, IOAPI_tenEHIOResource enEHIOResource)
{
	CAN_tstCANMB* pstCANMB;
	uint32 u32IMask = 1;
	uint32 u32MBIDX;
	uint32 u32IMaskMax = MATH_u32IDXToMask(CAN_nCANMailboxCount);
	
	while(u32IMask < u32IMaskMax)
	{
		if((u32IMask & (pstCAN -> IFLAG1)) == u32IMask)
		{
			pstCANMB = (CAN_tstCANMB*)&pstCAN -> MB[0];
			u32MBIDX = MATH_u32MaskToIDX(u32IMask);
			pstCANMB += u32MBIDX;
			
			while(((pstCANMB -> u32CS & CAN_CS_CODE_MASK)
				>> CAN_CS_CODE_SHIFT) == CAN_nCANMBXBusy);

			//CAN_ICvQueue(pstCANMB, enEHIOResource);
					
			DLL_vFrameRXCB(enEHIOResource, (puint8)pstCANMB);
			
			//u32Temp = pstCAN -> TIMER;				
			pstCAN -> IFLAG1 |= u32IMask;
		}
		u32IMask <<= 1;
		/* abort loop if all flags clear */
		if (pstCAN -> IFLAG1 == 0) 
		{
			u32IMask = u32IMaskMax;
		}
	}
}

void 
IRQ_vCommonUART(UART_Type* pstUART, IOAPI_tenEHIOResource enEHIOResource)
{
	DLL_tstRXDLLData stRXDLLData;
	
	if(((pstUART -> S1) & UART_S1_TDRE_MASK) == UART_S1_TDRE_MASK)
	{
		if(((pstUART -> C2) & UART_C2_TIE_MASK) == UART_C2_TIE_MASK)
		
		DLL_vTransmitUARTBytesCB(enEHIOResource);
	}
	
	if(((pstUART -> S1) & UART_S1_RDRF_MASK) == UART_S1_RDRF_MASK)
	{
		stRXDLLData.u8Data[0] = UART_u8GetChar(enEHIOResource);
		stRXDLLData.u8DataCount = 1;
		stRXDLLData.u8RXByteIDX = 0;		
		DLL_vReceiveUARTBytes(enEHIOResource, &stRXDLLData);
	}
}

#endif //IRQHA_H



