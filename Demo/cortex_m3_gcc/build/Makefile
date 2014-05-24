#******************************************************************************
#
# Makefile - Rules for building the driver library and examples.
#
# Copyright (c) 2005,2006 Luminary Micro, Inc.  All rights reserved.
#
# Software License Agreement
#
# Luminary Micro, Inc. (LMI) is supplying this software for use solely and
# exclusively on LMI's Stellaris Family of microcontroller products.
#
# The software is owned by LMI and/or its suppliers, and is protected under
# applicable copyright laws.  All rights are reserved.  Any use in violation
# of the foregoing restrictions may subject the user to criminal sanctions
# under applicable laws, as well as to civil liability for the breach of the
# terms and conditions of this license.  #
# THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
# OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
# LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
#
#******************************************************************************

RTOS_SOURCE_DIR=../../../Source

CCPATH=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
LDSCRIPT=standalone.ld

ARCH=-mthumb -mcpu=cortex-m3
OPTIM=-O0
INCLUDE=-I ../ -I ${RTOS_SOURCE_DIR}/include -I ${RTOS_SOURCE_DIR}/portable/GCC/ARM_CM3
CFLAGS=${INCLUDE} ${ARCH} ${OPTIM}

LDFLAG=-nostartfiles -T ${LDSCRIPT}

TARGET=system
SOURCES=main.c	\
	startup.c	\
	list.c    \
      	queue.c   \
      	tasks.c   \
      	port.c	\
	heap_1.c \
	basic_io.c	\
	syscalls.c \
	assert.c	\
	gpio.c	\
	locks.c	\
	sensor.c

OBJS=${SOURCES:.c=.o}

VPATH=../:${RTOS_SOURCE_DIR}:${RTOS_SOURCE_DIR}/portable/GCC/ARM_CM3:${RTOS_SOURCE_DIR}/portable/MemMang:../init

.PHONY: all clean cleanobj
#
# The default rule, which causes init to be built.
#
all: ${TARGET}.hex

${TARGET}.hex: %.hex: %.bin
	hexdump -v -e '1/4 "%08X""\n"' $< >$@

${TARGET}.bin: ${TARGET}
	${OBJCOPY} $< -O binary $@

${TARGET}: ${OBJS}
	${CCPATH} ${ARCH} ${OPTIM} ${LDFLAG} -o $@ $^

${OBJS}: %.o: %.c
	${CCPATH} ${CFLAGS} -c $<

-include ${SOURCES:.c=.d}

# automatic header files dependencies
%.d: %.c
	@set -e; rm -rf $@; \
	${CCPATH} ${INCLUDE} -MM $< >$@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' <$@.$$$$ >$@;\
	rm -rf $@.$$$$

#
# The rule to clean out all the build products
#

clean: cleanobj
	rm -rf ${SOURCES:.c=.d}

cleanobj:
	rm -rf ${TARGET} ${OBJS} ${TARGET}.bin ${TARGET}.hex
