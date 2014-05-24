#include "FreeRTOS.h"
#include "gpio.h"

#define REQ_PIN  (0)
#define ACK_PIN   (1)

#define GPIO_MASK   	( (unsigned long *)0x44001008 )
#define GPIO_INT_BITS   ( (unsigned long *)0x4400100C )
#define GPIO_ACK_ENA    ( (unsigned long *)0x44001304 )
#define GPIO_REQ_DATA    ( (unsigned long *)0x44001100 )
#define GPIO_REQ_DIR    ( (unsigned long *)0x44001200 )
#define GPIO_ACK_DIR    ( (unsigned long *)0x44001204 )
#define GPIO_ACK_PRI    ( (unsigned char *)0xE000E401 )   //Priority-level external Interrupt #1

#define IRQ_ENA         ( (unsigned long *)0xE000E100 )   	// Enable for external Interrupt #0иC#31
															// bit[0] for Interrupt #0
															// bit[1] for Interrupt #1
															//бн
															// bit[31] for Interrupt #31

/* GPIO port initialization */
void init_gpio(void) {
    volatile unsigned long *mask = GPIO_MASK;
    *mask = ~0UL;
    volatile unsigned long *req_dir = GPIO_REQ_DIR;
    *req_dir = 1UL;
    volatile unsigned long *req_data = GPIO_REQ_DATA;
    *req_data = 0UL;
    // enable ack interrupt
    volatile unsigned long *ack_dir = GPIO_ACK_DIR;
    *ack_dir = 0UL;
    volatile unsigned long *intr_bits = GPIO_INT_BITS;
    *intr_bits = 1UL << ACK_PIN;
    volatile unsigned long *ack_enable = GPIO_ACK_ENA;
    *ack_enable = 1UL;
    // set ack interrupt priority
    volatile unsigned char *ack_pri = GPIO_ACK_PRI;
    *ack_pri = configMAX_SYSCALL_INTERRUPT_PRIORITY;
    // enable IRQ1
    volatile unsigned long *IRQ1_enable = IRQ_ENA;
    *IRQ1_enable = 1UL << 1;
}

void send_req(void) {
    volatile unsigned long *req = GPIO_REQ_DATA;
    // generate an interrupt signal
    *req = 1UL;
    *req = 0UL;
}
