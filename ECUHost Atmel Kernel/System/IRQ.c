/******************************************************************************/
/*    Copyright (c) 2016 MD Automotive Controls. Original Work.               */
/*    License: http://www.gnu.org/licenses/gpl.html GPL version 2 or higher   */
/******************************************************************************/
/* CONTEXT:KERNEL                                                             */                      
/* PACKAGE TITLE:      IRQ                                                    */
/* DESCRIPTION:        This code is interrupt service routines                */
/*                                                                            */
/* FILE NAME:          IRQ.c                                                  */
/* REVISION HISTORY:   28-03-2016 | 1.0 | Initial revision                    */
/*                                                                            */
/******************************************************************************/
#include "CPUAbstract.h"
#ifdef BUILD_MK60
#include <core_cm4.h>
#endif //BUILD_MK60
#ifdef BUILD_SAM3X8E
#include <core_cm3.h>
#endif //BUILD_SAM3X8E
#include <string.h>
#include "IRQ.h"
#include "timer.h"

MPU_Type IRQ_stMPU;

#ifdef BUILD_MK60
IRQRXCallBack IRQ_apfRXCallBack[100];
IRQTXCallBack IRQ_apfTXCallBack[100];
#endif

#ifdef BUILD_SAM3X8E
IRQRXCallBack IRQ_apfRXCallBack[PERIPH_COUNT_IRQn];
IRQTXCallBack IRQ_apfTXCallBack[PERIPH_COUNT_IRQn];
#endif

#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
extern uint32 OS_u32SysTickInterval;
extern uint32 OS_u32SysRunTimeHigh;
extern uint32 OS_u32SysRunTimeLow;
extern tq_struct* OS_pstBackgroundDispatcherTask;
#endif

extern void SYS_vAPISVC(void*);
DLL_tstRXDLLData CAN_stRXDLLData;
Bool IRQ_boEnableRTOS = TRUE;

static void IRQ_vCommonCAN(tstCANModule*, IOAPI_tenEHIOResource, IRQn_Type enIRQType);
void SVC_Handler_Main(void* svc_args);

#if defined(BUILD_MK60) || defined(BUILD_SAM3X8E)
void IRQ_vEnableIRQ(IRQn_Type enIRQType, IRQ_tenPRIO enPRIO, IRQRXCallBack pfRXCallBack, IRQTXCallBack pfTXCallBack)
{
	IRQ_apfRXCallBack[enIRQType] = pfRXCallBack;
	IRQ_apfTXCallBack[enIRQType] = pfTXCallBack;
	NVIC_ClearPendingIRQ(enIRQType);                     
	NVIC_SetPriority(enIRQType, enPRIO);              
	NVIC_EnableIRQ(enIRQType);                       
}
#endif

#if defined(BUILD_MK60) || defined(BUILD_SAM3X8E)
void IRQ_vDisableIRQ(IRQn_Type enIRQType)
{
	IRQ_apfRXCallBack[enIRQType] = NULL;
	IRQ_apfTXCallBack[enIRQType] = NULL;
	NVIC_DisableIRQ(enIRQType);
}
#endif

#ifdef BUILD_MK60
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
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
#endif //BUILD_KERNEL
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
task_queue IRQ_vSuspendThread(task_queue*);
task_queue IRQ_vSuspendThread(task_queue* stTaskQueueToSuspend)
{
	__asm(".EQU TASK_PC_OFFSET,	22");
	__asm(".EQU TASK_SP_OFFSET,	18");	
	__asm(".EQU STACK_FRAME_BASE_OFFSET, 8");
	__asm(".EQU STACK_FRAME_SIZE, 32");	
	__asm(".EQU FRAME_R0_OFFSET, 0");
	__asm(".EQU FRAME_R1_OFFSET, 4");
	__asm(".EQU FRAME_R2_OFFSET, 8");
	__asm(".EQU FRAME_R3_OFFSET, 12");
	__asm(".EQU FRAME_R12_OFFSET, 16");
	__asm(".EQU FRAME_LR_OFFSET, 20");
	__asm(".EQU FRAME_PC_OFFSET, 24");
	__asm(".EQU FRAME_XPSR_OFFSET, 28");	
	__asm(".EQU MASK_XPSR_THUMB, 16777216");
	
	//
	//
	//
	// get first task in R1
	__asm("LDR		R1, [R0]");
	
	// get next task in R2
	__asm("LDR		R2, [R1]");
	__asm("B 		TestQueueLast");
	__asm("FindQueueEndLoop:");
	__asm("MOV		R3, R2");
	__asm("LDR		R2,	[R2]");
	__asm("TestQueueLast:");
	__asm("CMP		R2, #0");
	__asm("BNE		FindQueueEndLoop");
	// queue pointer is in R0, current task R1, end task R3

	// change queue pointer to current task->next
	__asm("LDR		R2, [R1]");
	__asm("CMP		R1, R2");
	__asm("BEQ		ThreadSwapAbort");
	__asm("STR		R2, [R0]");

	//	set end task->next to current task
	__asm("STR		R1, [R3]");
	
	// set current task->next to NULL
	__asm("MOV		R3, #0");
	__asm("STR		R3, [R1]");
	
	// get thread address in R4 MATTHEW
	//		MOV		R4, #255
	
	// store current task program counter in task structure program counter
	//		STR		R4, [R1, #TASK_PC_OFFSET] MATTHEW
	
	// update the task saved stack pointer
	__asm("MRS		R0, PSP");
	// MATTHEW for R4 PUSH		NO!!!!!!!
	//		ADD		R0, #4
	__asm("STR		R0, [R1, #TASK_SP_OFFSET]");
	
	// get background dispatcher task stack pointer
	__asm("LDR		R1, =OS_pstBackgroundDispatcherTask");
	__asm("LDR		R1, [R1]");
	__asm("LDR		R1,	[R1, #TASK_SP_OFFSET]");
	
	// set	background dispatcher stack frame return address to
	// dispatcher entry point always
	__asm("LDR		R2, =OS_vBackgroundDispatcher");
	__asm("ORR		R2, #1");
	__asm("STR		R2,	[R1, #FRAME_PC_OFFSET]");
	
	// set XPSR thumb bit always
	//LDR		R2, [R1, #FRAME_XPSR_OFFSET]
	__asm("MOV		R2, #0");
	__asm("ORR		R2, #MASK_XPSR_THUMB");
	__asm("STR		R2, [R1, #FRAME_XPSR_OFFSET]");
	
	// store dispatcher stack pointer to PSP
	__asm("MSR		PSP, R1");
	
    __asm("ThreadSwapAbort:");
}
#endif //BUILD_KERNEL
#endif //BUILD_SAM3X8E

void IRQ_vReset(void)
{
	uint32 u32IDX;

	for (u32IDX = 0; u32IDX < (sizeof(IRQ_apfRXCallBack) / sizeof(IRQRXCallBack)); u32IDX++)
	{
		IRQ_apfRXCallBack[u32IDX] = NULL;
		IRQ_apfRXCallBack[u32IDX] = NULL;
	}
}


/*----------------------------------------------------------------------------
  SVC_Handler
 *----------------------------------------------------------------------------*/
 #ifdef BUILD_MK60
#if defined(BUILD_PBL) || defined (BUILD_SBL)
__asm void SVC_Handler(void) 
{}
#endif	
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
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
#endif //BUILD_KERNEL
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
#if defined(BUILD_PBL) || defined (BUILD_SBL)
void SVC_Handler(void)
{}
#endif
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
void SVC_Handler(void)
{
	//IMPORT 	SVC_Handler_Main
	//IMPORT 	OS_stSVCDataStruct
	__asm("TST   lr, #4");
	__asm("ITE   NE");
	__asm("MRSNE r0, PSP");
	__asm("MRSEQ r0, MSP");
	
	// load the link register containing user module return address into R0
	// module return address will be used as module call handle - UID which
	// infers privilege and is also unique
	__asm("LDR   R0, [R0, #24]");
	__asm("PUSH  {R1}");
	__asm("LDR   R1, =OS_stSVCDataStruct");
	__asm("STR   R0, [R1, #4]");
	__asm("POP   {R1}");
	
	__asm("TST   lr, #4");
	__asm("ITE   NE");
	__asm("MRSNE r0, PSP");
	__asm("MRSEQ r0, MSP");
	__asm("B     SVC_Handler_Main");
}
#endif //BUILD_KERNEL
#endif //BUILD_SAM3X8E

#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
void SVC_Handler_Main(void* svc_args)
{
	SYS_vAPISVC(svc_args);
}
#endif

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) 
{
#if defined(BUILD_PBL)
	PBL_vCyclicTask();
#endif	
#if defined(BUILD_SBL)
	SBL_vCyclicTask();	
#endif
#if defined(BUILD_KERNEL_OR_KERNEL_APP)
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
	
	if ((OS_enOSStateOSStarted == enOSState) &&
		(TRUE == IRQ_boEnableRTOS))
	{		
		/* check if kernel cyclic queue is active */
		if(NULL == stTaskQueue)
		{	
			pstTaskQueue = OS_stGetOSQueue(OS_enKERNELQueueCyclic);
			stTaskQueue = *pstTaskQueue;
			if (NULL != stTaskQueue)
			{			
				OS_vOverrunTaskReport(OS_enKERNELQueueCyclic);
				/* call schedule - no thread to swap out we are still co-op tasking */				
				//OS_vSchedule(OS_enOSSchedulerTick);
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
				//OS_vSchedule(OS_enOSSchedulerTick);
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
 #ifdef BUILD_MK60
void ENET_Transmit_IRQHandler(void){}
#endif  //BUILD_MK60

/*----------------------------------------------------------------------------
  ENET_RX_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_MK60
void ENET_Receive_IRQHandler(void)
{
	if(ENE_xIntActivated(ENET_EIMR_BABR_MASK))
	{		
		ENE_xIntClearFlag(ENET_EIMR_BABR_MASK);
	}
	
	if(ENE_xIntActivated(ENET_EIMR_RXF_MASK))
	{		
		ENE_xIntClearFlag(ENET_EIMR_RXF_MASK);
		IRQ_apfRXCallBack[EH_VIO_ENET_CH1](NULL, NULL);
	}
}
#endif  //BUILD_MK60

/*----------------------------------------------------------------------------
  MK60 ENET_ERR_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_MK60
void ENET_Error_IRQHandler(void){}
#endif //BUILD_MK60

/*----------------------------------------------------------------------------
  SAM3X8E ENET_ERR_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_SAM3X8E
void EMAC_Handler(void)
{
	IRQ_apfRXCallBack[EMAC_IRQn]((IOAPI_tenEHIOResource)NULL, NULL);
}
#endif //BUILD_SAM3X8E

/*----------------------------------------------------------------------------
  UART0_Handlers
 *----------------------------------------------------------------------------*/
 #ifdef BUILD_MK60
void UART0_RX_TX_IRQHandler(void)
{
	tstUARTModule* pstUART = UART0;
	IRQ_vCommonUART(pstUART, EH_VIO_UART1);
}

void UART0_ERR_IRQHandler(void){}
#endif //BUILD_MK60	

/*----------------------------------------------------------------------------
  UART1_Handlers
 *----------------------------------------------------------------------------*/
 #ifdef BUILD_MK60
void UART1_RX_TX_IRQHandler(void)
{
	tstUARTModule* pstUART = UART1;
	IRQ_vCommonUART(pstUART, EH_VIO_UART2);
}

void UART1_ERR_IRQHandler(void){}
#endif //BUILD_MK60	

/*----------------------------------------------------------------------------
  UART2_Handlers
 *----------------------------------------------------------------------------*/
 #ifdef BUILD_MK60
void UART2_RX_TX_IRQHandler(void)
{
	tstUARTModule* pstUART = UART2;
	IRQ_vCommonUART(pstUART, EH_VIO_UART3);
}

void UART2_ERR_IRQHandler(void){}
#endif //BUILD_MK60	
	
/*----------------------------------------------------------------------------
  UART5_Handlers
 *----------------------------------------------------------------------------*/
 #ifdef BUILD_MK60
void UART3_RX_TX_IRQHandler(void)
{
	tstUARTModule* pstUART = UART3;
	IRQ_vCommonUART(pstUART, EH_VIO_UART4);
}

void UART3_ERR_IRQHandler(void){}
#endif //BUILD_MK60	

/*----------------------------------------------------------------------------
  UART4_Handlers
 *----------------------------------------------------------------------------*/
 #ifdef BUILD_MK60
void UART4_RX_TX_IRQHandler(void)
{
	tstUARTModule* pstUART = UART4;
	IRQ_vCommonUART(pstUART, EH_VIO_UART5);
}

void UART4_ERR_IRQHandler(void){}
#endif //BUILD_MK60	

/*----------------------------------------------------------------------------
  UART5_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_MK60
void UART5_RX_TX_IRQHandler(void)
{
	tstUARTModule* pstUART = UART5;
	IRQ_vCommonUART(pstUART, EH_VIO_UART6);
}

void UART5_ERR_IRQHandler(void){}	
#endif //BUILD_MK60	
	
/*----------------------------------------------------------------------------
  MK60 SDHC_Handler
 *----------------------------------------------------------------------------*/
#ifdef BUILD_MK60
void SDHC_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	IRQ_apfRXCallBack[0](NULL, NULL);
#endif	
}
#endif

/*----------------------------------------------------------------------------
  SAM3X8E HSCMI_Handler
 *----------------------------------------------------------------------------*/
#ifdef BUILD_SAM3X8E
void HSMCI_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	IRQ_apfRXCallBack[HSMCI_IRQn]((IOAPI_tenEHIOResource)NULL, NULL);
#endif	
}
#endif

/*----------------------------------------------------------------------------
  ADCX_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_MK60
void ADC0_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	IRQ_apfRXCallBack[ADC0_IRQn](NULL, NULL);	
#endif	
}

void ADC1_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)	
	IRQ_apfRXCallBack[ADC1_IRQn](NULL, NULL);
#endif	
}

void ADC2_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	IRQ_apfRXCallBack[ADC2_IRQn](NULL, NULL);
#endif
}

void ADC3_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)	
	IRQ_apfRXCallBack[ADC3_IRQn](NULL, NULL);
#endif	
}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
void ADC_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	IRQ_apfRXCallBack[ADC_IRQn](EH_VIO_ADC0, NULL);
#endif
}
#endif //BUILD_SAM3X8E

/*----------------------------------------------------------------------------
  KM60 FTMX_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_MK60
void FTM0_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	FTM_Interrupt(FTM_enFTM0);
#endif	
}

void FTM1_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	FTM_Interrupt(FTM_enFTM1);	
#endif	
}

void FTM2_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	FTM_Interrupt(FTM_enFTM2);	
#endif	
}

void FTM3_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	FTM_Interrupt(FTM_enFTM3);	
#endif	
}
#endif //BUILD_MK60

/*----------------------------------------------------------------------------
  SAM3X8E TCX_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_SAM3X8E
void TC0_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 1;
	IRQ_apfRXCallBack[TC0_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC0, (void*)&u32Prio);
#endif	
}

void TC1_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 0;
	IRQ_apfRXCallBack[TC1_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC1, (void*)&u32Prio);
#endif
}

void TC2_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 0;
	IRQ_apfRXCallBack[TC2_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC2, (void*)&u32Prio);
#endif
}

void TC3_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 0;
	IRQ_apfRXCallBack[TC3_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC3, (void*)&u32Prio);
#endif
}

void TC4_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 0;
	IRQ_apfRXCallBack[TC4_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC4, (void*)&u32Prio);
#endif
}

void TC5_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 0;
	IRQ_apfRXCallBack[TC5_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC5, (void*)&u32Prio);
#endif
}

void TC6_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 0;
	IRQ_apfRXCallBack[TC6_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC6, (void*)&u32Prio);
#endif
}

void TC7_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 1;
	IRQ_apfRXCallBack[TC7_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC7, (void*)&u32Prio);
#endif
}

void TC8_Handler(void)
{
	#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	uint32 u32Prio = 0;
	IRQ_apfRXCallBack[TC8_IRQn]((IOAPI_tenEHIOResource)EH_VIO_TCC8, (void*)&u32Prio);
	#endif
}
#endif //BUILD_SAM3X8E

/*----------------------------------------------------------------------------
  MK60 IICX_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_MK60
void I2C0_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)	
	IRQ_apfRXCallBack[I2C0_IRQn](NULL, NULL);
#endif
}

void I2C1_IRQHandler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)	
	IRQ_apfRXCallBack[I2C1_IRQn](NULL, NULL);
#endif	
}
#endif //BUILD_MK60	

/*----------------------------------------------------------------------------
  SAM3X8E SPI_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_SAM3X8E
void SPI0_Handler(void)
{
#if (BUILD_KERNEL_OR_KERNEL_APP == 1)
	IRQ_apfRXCallBack[SPI0_IRQn]((IOAPI_tenEHIOResource)EH_VIO_SPI1, NULL);
#endif	
}

#endif //BUILD_SAM3X8E

/*----------------------------------------------------------------------------
  MK60 CANX_Handlers
 *----------------------------------------------------------------------------*/
#ifdef BUILD_MK60
void CAN0_ORed_Message_buffer_IRQHandler(void)
{
	tstCANModule* pstCAN = CAN0;

	IRQ_vCommonCAN(pstCAN, EH_VIO_CAN1);
}

void CAN1_ORed_Message_buffer_IRQHandler(void)
{
	tstCANModule* pstCAN = CAN1;
	
	IRQ_vCommonCAN(pstCAN, EH_VIO_CAN2);
}
#endif //BUILD_MK60

#ifdef BUILD_SAM3X8E
void CAN0_Handler(void)
{
	IRQ_vCommonCAN(CAN0, (IOAPI_tenEHIOResource)EH_VIO_CAN1, CAN0_IRQn);
}

void CAN1_Handler(void)
{
	IRQ_vCommonCAN(CAN1, (IOAPI_tenEHIOResource)EH_VIO_CAN2, CAN1_IRQn);
}
#endif //BUILD_SAM3X8E

void IRQ_vCommonCAN(tstCANModule* pstCAN, IOAPI_tenEHIOResource enEHIOResource, IRQn_Type enIRQType)
{
#if defined(BUILD_SAM3X8E)	
	IRQ_apfRXCallBack[enIRQType](enEHIOResource, NULL);
#endif //BUILD_SAM3X8E
	
	
#ifdef BUILD_MK60
	CANHA_tstCANMB* pstCANMB;
	uint32 u32IMask = 1;
	uint32 u32MBIDX;
	uint32 u32IMaskMax = MATH_u32IDXToMask(CAN_nCANMailboxCount);
	
	while(u32IMask < u32IMaskMax)
	{
		if((u32IMask & (pstCAN -> IFLAG1)) == u32IMask)
		{
			pstCANMB = (CANHA_tstCANMB*)&pstCAN -> MB[0];
			u32MBIDX = MATH_u32MaskToIDX(u32IMask);
			pstCANMB += u32MBIDX;
			
			while(((pstCANMB -> u32CS & CAN_CS_CODE_MASK)
				>> CAN_CS_CODE_SHIFT) == CAN_nCANMBXBusy);
					
			IRQ_apfRXCallBack[enIRQType](enEHIOResource, (void*)pstCANMB);			
			pstCAN -> IFLAG1 |= u32IMask;
		}
		u32IMask <<= 1;
		/* abort loop if all flags clear */
		if (pstCAN -> IFLAG1 == 0) 
		{
			u32IMask = u32IMaskMax;
		}
	}
#endif
}



/*----------------------------------------------------------------------------
  USBOTG Handler
 *----------------------------------------------------------------------------*/
void UOTGHS_Handler(void)
{
#ifdef BUILD_SAM3X8E
    udd_interrupt();
#endif //BUILD_SAM3X8E
}



void NMI_Handler        ( void ) {while(1) {}}
void HardFault_Handler  ( void ) {while(1) {}}
void BusFault_Handler   ( void ) {while(1) {}}
void UsageFault_Handler ( void ) {while(1) {}}
void DebugMon_Handler   ( void ) {while(1) {}}
void PendSV_Handler     ( void ) {while(1) {}}


/* Peripherals handlers */
void SUPC_Handler       ( void ) {while(1) {}}
void RSTC_Handler       ( void ) {while(1) {}}
void RTC_Handler        ( void ) {while(1) {}}
void RTT_Handler        ( void ) {while(1) {}}
void WDT_Handler        ( void ) {while(1) {}}
void PMC_Handler        ( void ) {while(1) {}}
void EFC0_Handler       ( void ) {while(1) {}}
void EFC1_Handler       ( void ) {while(1) {}}
void UART_Handler       ( void ) {while(1) {}}
#ifdef _SAM3XA_SMC_INSTANCE_
//void SMC_Handler        ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif /* _SAM3XA_SMC_INSTANCE_ */
#ifdef _SAM3XA_SDRAMC_INSTANCE_
//void SDRAMC_Handler     ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
#endif /* _SAM3XA_SDRAMC_INSTANCE_ */


#ifdef _SAM3XA_PIOE_INSTANCE_
void PIOE_Handler       ( void ) {while(1) {}}
#endif /* _SAM3XA_PIOE_INSTANCE_ */
#ifdef _SAM3XA_PIOF_INSTANCE_
void PIOF_Handler       ( void ) {while(1) {}}
#endif /* _SAM3XA_PIOF_INSTANCE_ */
void USART1_Handler     ( void ) {while(1) {}}
void USART2_Handler     ( void ) {while(1) {}}
#ifdef _SAM3XA_USART3_INSTANCE_
void USART3_Handler     ( void ) {while(1) {}}
#endif /* _SAM3XA_USART3_INSTANCE_ */
void TWI0_Handler       ( void ) {while(1) {}}
void TWI1_Handler       ( void ) {while(1) {}}
#ifdef _SAM3XA_SPI1_INSTANCE_
void SPI1_Handler       ( void ) {while(1) {}}
#endif /* _SAM3XA_SPI1_INSTANCE_ */
void SSC_Handler        ( void ) {while(1) {}}
void PWM_Handler        ( void ) {while(1) {}}
void DACC_Handler       ( void ) {while(1) {}}
void DMAC_Handler       ( void ) {while(1) {}}
void TRNG_Handler       ( void ) {while(1) {}}



void IRQ_vEnableRTOS(Bool boEnable)
{
	IRQ_boEnableRTOS = boEnable;
}






