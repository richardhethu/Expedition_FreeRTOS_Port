#ifndef MMAP_H_
#define MMAP_H_

// memory address range of ram buffer used for data communication between
// mbed and Cortex-M3
#define WRITEBUF_BEGIN    ( (volatile unsigned long *)0x24000100 )
#define WRITEBUF_END      ( (volatile unsigned long *)0x2400017C )
#define READBUF_BEGIN     ( (volatile unsigned long *)0x24000180 )
#define READBUF_END       ( (volatile unsigned long *)0x240001FC )

#define BUF_SIZE        ( (unsigned int)(0x7c - 4 )) 
// write to this address to indicate whether it is a print req or scan req
#define IO_TYPE         ( (volatile unsigned long *)0x2400017C )
#define ACK_TYPE        ( (volatile unsigned long *)0x240001FC )
#define ACK_RETURN_VAL  ( (volatile unsigned long *)0x240001F8)

#define EMPTY_REQ			 ( 16UL)
#define TERM_PRINT_REQ       ( 0UL )
#define TERM_SCAN_REQ        ( 1UL )
#define ENET_SEND_REQ    	 ( 2UL )
#define ENET_RECV_REQ     	 ( 3UL )
#define WIRELESS_SEND_REQ 	 ( 4UL )
#define WIRELESS_RECV_REQ 	 ( 5UL )
#define READ_POW_REQ      	 ( 6UL )
#define PANIC_REQ            ( 15UL )

#define PRINT_ACK    		( 0UL )
#define SCAN_ACK      		( 1UL )
#define ENET_SEND_ACK 		( 2UL )
#define ENET_RECV_ACK 	 	( 3UL )
#define WIRELESS_SEND_ACK 	( 4UL )
#define WIRELESS_RECV_ACK 	( 5UL )
#define READ_POW_ACK      	( 6UL )

#endif
