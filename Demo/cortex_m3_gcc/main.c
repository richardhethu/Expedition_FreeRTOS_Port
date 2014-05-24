/*
    FreeRTOS V7.5.3 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* 
 * This demo application creates six co-routines and two tasks (three including 
 * the idle task).  The co-routines execute as part of the idle task hook.
 *
 * Five of the created co-routines are the standard 'co-routine flash' 
 * co-routines contained within the Demo/Common/Minimal/crflash.c file and 
 * documented on the FreeRTOS.org WEB site.  
 *
 * The 'LCD Task' rotates a string on the LCD, delaying between each character
 * as necessitated by the slow interface, and delaying between each string just
 * long enough to enable the text to be read.
 *
 * The sixth co-routine and final task control the transmission and reception
 * of a string to UART 0.  The co-routine periodically sends the first 
 * character of the string to the UART, with the UART's TxEnd interrupt being
 * used to transmit the remaining characters.  The UART's RxEnd interrupt 
 * receives the characters and places them on a queue to be processed by the 
 * 'COMs Rx' task.  An error is latched should an unexpected character be 
 * received, or any character be received out of sequence.  
 *
 * A loopback connector is required to ensure that each character transmitted 
 * on the UART is also received on the same UART.  For test purposes the UART
 * FIFO's are not utalised in order to maximise the interrupt overhead.  Also
 * a pseudo random interval is used between the start of each transmission in 
 * order that the resultant interrupts are more randomly distributed and 
 * therefore more likely to highlight any problems.
 *
 * The flash co-routines control LED's zero to four.  LED five is toggled each
 * time the string is transmitted on the UART.  LED six is toggled each time
 * the string is CORRECTLY received on the UART.  LED seven is latched on should
 * an error be detected in any task or co-routine.
 *
 * In addition the idle task makes repetative calls to 
 * prvSetAndCheckRegisters().  This simply loads the general purpose registers 
 * with a known value, then checks each register to ensure the held value is 
 * still correct.  As a low priority task this checking routine is likely to 
 * get repeatedly swapped in and out.  A register being found to contain an 
 * incorrect value is therefore indicative of an error in the task switching 
 * mechansim.
 *
 */
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "mmap.h"
#include "basic_io.h"
#include "gpio.h"
#include "locks.h"
#include "sensor.h"

void prvHardwareSetup(void);
/* Task functions */
void vTestKernel(void *pvAddress);
void vTestKernel2(void *pvAddress);
void vTestKernel3(void *pvAddress);
void vTestKernel4(void *pvAddress);

/*-----------------------------------------------------------*/

void main( void )
{
	prvHardwareSetup();
	/* create 4 tasks */
	xTaskCreate(vTestKernel, "Task1", 512, NULL, 1, NULL);
	xTaskCreate(vTestKernel2, "Task2", 512, NULL, 1, NULL);
	xTaskCreate(vTestKernel3, "Task3", 512, NULL, 1, NULL);
	xTaskCreate(vTestKernel4, "Task4", 512, NULL, 1, NULL);

	/* Start the scheduler running the tasks and co-routines just created. */
	vTaskStartScheduler();

	/* Should not get here unless we did not have enough memory to start the
	scheduler. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vTestKernel(void *pvAddress)
{
    	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	unsigned int rtn;
	char ch = 'A';
    	while (1) {
		send_buf[0] = ch;
		send_buf[1] = '\0';
		ch = (char)((unsigned int)ch + 1);
		if(ch > 'Z'){
			ch = 'A';
		}
		rtn = enet_send(send_buf);
		if (rtn > 0) {

			term_printf("<<<Task 1>>> sent by enet: %s\r\n", send_buf);
			vTaskDelay(500 / portTICK_RATE_MS);
			rtn = enet_recv(recv_buf, 1);
			if(rtn > 0){
				term_printf("<<<Task 1>>> received by enet: %s\r\n", recv_buf);
			}
			else{
				term_printf("<<<Task 1>>> received nothing by enet!\r\n");
			}
		}
		else {
			term_printf("<<<Task 1>>> failed to send data by enet!\r\n");
		}	
    	}
}

void vTestKernel2(void *pvAddress)
{
    while (1) {
        term_printf("<<<Task 2>>> is running.\r\n");
        vTaskDelay(6000 / portTICK_RATE_MS);
    }
}

void vTestKernel3(void *pvAddress)
{
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	unsigned int rtn;
	char ch = 'A';
    	while (1) {
		send_buf[0] = ch;
		send_buf[1] = '\0';
		ch = (char)((unsigned int)ch + 1);
		if(ch > 'Z'){
			ch = 'A';
		}
		rtn = wireless_send(send_buf);
		if (rtn > 0) {
			term_printf("<<<Task 3>>> sent by wifi: %s\r\n", send_buf);
			vTaskDelay(500 / portTICK_RATE_MS);
			rtn = wireless_recv(recv_buf, 1);
			if(rtn > 0){
				term_printf("<<<Task 3>>> received: %s\r\n", recv_buf);
			}
			else{
				term_printf("<<<Task 3>>> received nothing by wifi!\r\n");
			}
		}
		else {
			term_printf("<<<Task 3>>> failed to send data by wifi!\r\n");
		}	
    	}
}

void vTestKernel4(void *pvAddress)
{
	float sen, mem1, mem2, core;
	int gain_ctrl = 0;
	int rtn;
	while (1) {
        	rtn = read_pow(gain_ctrl, &sen, &mem1, &mem2, &core);
		if (rtn == 0) {
			term_printf("<<<Task 4>>> failed to read power sensors!\r\n");
		}
		else{
			term_printf("<<<Task 4>>> power_sensor reading ---gain_ctrl: %d, sen: %f, mem1: %f, mem2: %f, core: %f\r\n", gain_ctrl, sen, mem1, mem2, core);
		}
		vTaskDelay(5000 / portTICK_RATE_MS);
		gain_ctrl = 1 - gain_ctrl;
    	}
}

void prvHardwareSetup(void) {
	// init shard_buf
	volatile unsigned long *addr = IO_TYPE;
	*addr = EMPTY_REQ;
	
    // set GPIO direction
    init_gpio();
    // create mutex and semaphore
    init_locks();
    // enable interrupt for sensors
    init_sensor();
}
