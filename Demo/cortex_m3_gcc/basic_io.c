#include <stdarg.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "mmap.h"
#include "basic_io.h"
#include "semphr.h"
#include "locks.h"
#include "gpio.h"

static float atof(char *str);
unsigned int send_rtn_val;	// send_out return value
unsigned int recv_rtn_val;	// receive return value

static union byte_chunk_union {
    unsigned long word;
    char bytes[4];
} byte_chunk;

static char out_line[BUF_SIZE];
static char in_line[BUF_SIZE];
// retargeted stdio functions
// This function is not thread-safe. It can only be called after acquiring
// a mutex lock.
static int impl_write(unsigned long req_type, const char *format, \
        va_list args) {
    volatile unsigned long *addr;
    char *str;
    int i;
    int len = 0;
    unsigned long ack_val;
    // acquire a mutex to protect out_line and WRITEBUF
if (req_type != PANIC_REQ) {
    xSemaphoreTake(xPrint_Mutex, portMAX_DELAY);
}
  //{
        len = vsnprintf(out_line, BUF_SIZE, format, args);

        addr = WRITEBUF_BEGIN;
        str = out_line;
        do {
            for (i = 0; i < 4; ++i) {
                if (*str) {
                    byte_chunk.bytes[i] = *str++;
                    ++len;
                } else {
                    byte_chunk.bytes[i] = '\0';
                }
            }
            *addr++ = byte_chunk.word;
        } while (*str && addr < WRITEBUF_END);
        // add trailing \0
        if (addr < WRITEBUF_END)
            *addr = 0x0;
        // since IO_TYPE is in critical section, we need a lock to protect it.
        xSemaphoreTake(xREQ_Mutex, portMAX_DELAY);
        {
            // send a request signal to mbed
            addr = IO_TYPE;
			while(*addr != EMPTY_REQ); //wait MBED 
            *addr = req_type;
            // send a request signal to mbed
            send_req();
        }
        xSemaphoreGive(xREQ_Mutex);

        // wait for acknowledge signal from mbed
		ack_val = send_rtn_val;
        xSemaphoreTake(xSendACK_BinarySemphr, portMAX_DELAY);
  //}
if (req_type != PANIC_REQ) {
    // Release a mutex lock
    xSemaphoreGive(xPrint_Mutex);
}

    return ack_val;
}

static int impl_read(unsigned long req_type, char *buffer, unsigned long size) {
    volatile unsigned long *addr;
    char *str;
    int len;
	unsigned long ack_val;
	
    // acquire a mutex to protect READBUF
    xSemaphoreTake(xScan_Mutex, portMAX_DELAY);
    {
        // IO_TYPE is in critical section, need mutex to protect it.
        xSemaphoreTake(xREQ_Mutex, portMAX_DELAY);
        // write to IO_TYPE
        addr = IO_TYPE;
		while(*addr != EMPTY_REQ); //wait MBED 
        *addr = req_type;
	addr = READBUF_BEGIN;
	*addr = size;
        // send a request signal to mbed
        send_req();
        xSemaphoreGive(xREQ_Mutex);

        // wait for acknowledge signal from mbed
        xSemaphoreTake(xRecvACK_BinarySemphr, portMAX_DELAY);
	ack_val = recv_rtn_val;

	if(ack_val > 0){			
	// read from ram buffer
	volatile char *str = (char *)READBUF_BEGIN;
	int i = 0;
	while (*str && i < ack_val) {
		buffer[i++] = *str++;
	}
	buffer[i] = '\0';
	//len = vsscanf(in_line, format, args);
	}
    }
    // Release a mutex lock
    xSemaphoreGive(xScan_Mutex);

    return ack_val;
}

int term_printf(const char *format, ...) {
    int rtn;

    va_list args;
    va_start(args, format);

    rtn = impl_write(TERM_PRINT_REQ, format, args);

    va_end(args);
    return rtn;
}

/* Zimin: I should have implemented term_scanf just like scanf in 
 * libc, but my implementation needs vsscanf which is problematic in this demo.
 * Calling vsscanf would result in a hard fault. 
 *
 * So the user has to read in a string firstly and then parse the string
 * by themselves.
 */
int term_gets(char *str, size_t size) {
    int rtn;
    rtn = impl_read(TERM_SCAN_REQ, str, size);
    return rtn;
}

int enet_send(const char* str) {
	va_list args;
	int rtn = impl_write(ENET_SEND_REQ, str, args);
	return rtn;
}

int enet_recv(char *str, size_t size) {
	int rtn;
    	rtn = impl_read(ENET_RECV_REQ, str, size);
    	return rtn;
}

int wireless_send(const char* str) {
	va_list args;
	int rtn = impl_write(WIRELESS_SEND_REQ, str, args);
	return rtn;
}

int wireless_recv(char *str, size_t size) {
	int rtn;
	rtn = impl_read(WIRELESS_RECV_REQ, str, size);
	return rtn;
}

int read_pow(int gain_ctrl, float *sen, float *mem1, float *mem2, float *core) {
	int rtn;
	static char buf[BUF_SIZE];
	static char buf2[BUF_SIZE];
	rtn = impl_read(READ_POW_REQ, buf, gain_ctrl);
	if (rtn == 0)
		return 0;
	int i =0;
	int j =0;
	while(buf[i] != ',') {
		buf2[j] = buf[i];		
		i ++;
		j ++;
	}
	buf2[j] = '\0';
	*sen = atof(buf2);
	
	j =0;
	i ++;
	while(buf[i] != ',') {
		buf2[j] = buf[i];		
		i ++;
		j ++;
	}
	buf2[j] = '\0';
	*mem1 = atof(buf2);

	j =0;
	i ++;
	while(buf[i] != ',') {
		buf2[j] = buf[i];		
		i ++;
		j ++;
	}
	buf2[j] = '\0';
	*mem2 = atof(buf2);
	
	j =0;
	i ++;
	while(buf[i] != '\0') {
		buf2[j] = buf[i];		
		i ++;
		j ++;
	}
	buf2[j] = '\0';
	*core = atof(buf2);
	return 1;
}

static float atof(char *str) {
	float inte = str[0] - '0';
	float poi;
	float wt;
	int i=1;
	int flag = 0;
	while(str[i] != '\0') {
		if (str[i] == '.') {
			wt = 1;
			poi = 0;
			flag = 1;
			i ++;	
		}
		else {
			if(flag == 0) {
				inte = inte * 10 + str[i] - '0';
				i ++;
			}
			else {
				wt /= 10;
				poi = poi + (str[i] - '0') * wt;
				i ++;
			}
		}
	}
	return inte + poi;
}

// This function is called in fault handler to print information to ram
// buffer to debug use.
void panic(const char *format, ...) {
    // suspend all tasks and print error message
    vTaskSuspendAll();
    va_list args;

    va_start(args, format);
    impl_write(PANIC_REQ, format, args);
    va_end(args);

    while (1)
        ;
}
