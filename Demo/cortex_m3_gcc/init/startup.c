#include "../locks.h"
#include "mmap.h"
#include "basic_io.h"

//*****************************************************************************
//
// startup.c - Boot code for Ferrari testboard.
//
//********************************************************************

//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void NmiSR(void);
void FaultISR(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);
extern void vPortSVCHandler( void );
void vAcknowledge_ISR(void);
void vCount_ISR(void);
void vDDRO_ISR(void);

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern void entry(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
#ifndef STACK_BEGIN
#define STACK_BEGIN (0x2002C000)
#endif

//*****************************************************************************
//
// The minimal vector table for a Cortex-M3.  Note that the proper constructs
// must be placed on this to ensure that it ends up at physical address
// 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".isr_vectors")))
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))STACK_BEGIN,
    ResetISR,
    NmiSR,
    FaultISR,								//FAULT
    0,                                      // The MPU fault handler
    0,                                      // The bus fault handler
    0,                                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    vPortSVCHandler,						// SVCall handler
    0,                                      // Debug monitor handler
    0,                                      // Reserved
    xPortPendSVHandler,                     // The PendSV handler
    xPortSysTickHandler,                    // The SysTick handler
    vAcknowledge_ISR,                      // IRQ0 handler    //why?
    vAcknowledge_ISR,                       // IRQ1 handler
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    vCount_ISR,
    vDDRO_ISR		//why 33, since IRQ33 corresponds to exception 33+16=47
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void
ResetISR(void)
{
    unsigned long *pulSrc, *pulDest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = &_etext;
    for(pulDest = &_data; pulDest < &_edata; )
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.
    //
    for(pulDest = &_bss; pulDest < &_ebss; )
    {
        *pulDest++ = 0;
    }

    //
    // Call the application's entry point.
    //
    main();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
NmiSR(void)
{
    // write error code to a certain address
    panic("Enter NMI handler.\r\n");
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
void
FaultISR(void)
{
    // write message
    panic("Enter hard fault handler.\r\n");
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}


//************************************************************************
//
// This is the code that gets called when the processor receives an
// acknowledge interrupt. This handler then wakes up tasks waiting for this
// acknowledge signal by releasing a binary semaphore.
//
//************************************************************************
void vAcknowledge_ISR(void) {
    volatile unsigned long *ack_type = ACK_TYPE;
	volatile unsigned long *rtn_val = ACK_RETURN_VAL;
	unsigned long type = *ack_type;
	unsigned long val = *rtn_val;    
    xSemaphoreHandle xBinarySemaphore;
    switch(type) {
    case PRINT_ACK:
	case ENET_SEND_ACK:
	case WIRELESS_SEND_ACK:
        // print acknowledgement
        xBinarySemaphore = xSendACK_BinarySemphr;
		send_rtn_val = val;
        break;
    case SCAN_ACK:
	case ENET_RECV_ACK:
	case WIRELESS_RECV_ACK:
	case READ_POW_ACK:
        // scan acknowledgement     
        xBinarySemaphore = xRecvACK_BinarySemphr;
		recv_rtn_val = val;
        break;
    default:
        panic("Unsupported ack type: %u.\r\n", type);
    }
    portBASE_TYPE xHigherPriorityTasksWoken = pdFALSE;
    xSemaphoreGiveFromISR(xBinarySemaphore, \
            &xHigherPriorityTasksWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTasksWoken);
}

void vCount_ISR(void) {
    panic("Enter vCountISR.\r\n");
}

//************************************************************************
//
// This is the code that gets called when the processor receives an
// ddro done interrupt. This handler then wakes up tasks waiting for this
// signal by releasing a binary semaphore.
//
//************************************************************************
void vDDRO_ISR(void) {
    portBASE_TYPE xHigherPriorityTasksWoken = pdFALSE;
    xSemaphoreGiveFromISR(xDDRO_BinarySemphr, \
            &xHigherPriorityTasksWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTasksWoken);
}
