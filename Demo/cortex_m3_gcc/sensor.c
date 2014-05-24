#include "FreeRTOS.h"
#include "task.h"
#include "locks.h"
#include "sensor.h"

#define SYN_EN          ( (volatile unsigned long *)0x44100028 )
#define INV_EN          ( (volatile unsigned long *)0x4410002C )

#define DDRO_REF_SRC    ( (volatile unsigned long *)0x44100030 )
#define DDRO_SAMPLE_SRC ( (volatile unsigned long *)0x44100034 )
#define DDRO_START      ( (volatile unsigned long *)0x44100038 )
#define DDRO_THRESHOLD  ( (volatile unsigned long *)0x4410003C )
#define DDRO_COUNT      ( (volatile long *)0x44100064 )

// enable interrupts and SET INTERRUPT PRIORITY
void init_sensor(void) {
    volatile unsigned long *sensor_intr = \
            (volatile unsigned long *)0xE000E104;	//Enable for external Interrupt #32¨C63
							//Write 1 to set bit to 1; write 0 has no effect
							//Read value indicates the current status
    *sensor_intr = 0x3;     // enable IRQ32 and IRQ33
    // WARNING: INTERRUPT PRIORITY LEVEL MUSH BE LESS THAN
    // or equal to configMAX_SYSCALL_INTERRUPT_PRIORITY;
    // set priority level for IRQ33
    volatile unsigned char *sensor_intr_priority = \
            (volatile unsigned char *)0xE000E421;	//Priority-level external Interrupt #33
    *sensor_intr_priority = configMAX_SYSCALL_INTERRUPT_PRIORITY;
}

long sensor_read(long type, long id) {
    // since ddro is shared resource, we need mutex to protect it.
    xSemaphoreTake(xDDRO_Mutex, portMAX_DELAY);

    // enable corresponding sensor
    if (type < SYN_DDRO || type > TEMP_CLK) {
        // invalid sensor
        return -1;	// dead_lock risk
    } else if (type == SYN_DDRO && (id < 0 || id > 8)) {
        // invalid id
        return -1;
    } else if (type == INV_DDRO && (id < 0 || id > 9)) {
        // invalid id
        return -1;
    }
    volatile unsigned long *enable = 0x0;
    long index = type;
    if (type == SYN_DDRO) {
        enable = SYN_EN;
        *enable = 1 << id;
        index = type + id;
    } else if (type == INV_DDRO) {
        enable = INV_EN;
        *enable = 1 << id;
        index = type + id;
    }

    // set ref_clk
    volatile unsigned long *ref_src = DDRO_REF_SRC;
    *ref_src = 0x2;

    // set threshold
    volatile unsigned long *threshold = DDRO_THRESHOLD;
    *threshold = 100000;

    // set sample clk
    volatile unsigned long *sample_src = DDRO_SAMPLE_SRC;
    *sample_src = (unsigned long)index;

    // start ddro
    volatile unsigned long *start = DDRO_START;
    *start = 0x1;

    // wait for ddro_done signal
    xSemaphoreTake(xDDRO_BinarySemphr, portMAX_DELAY);

    // read counter value and clear counter value
    *start = 0x0;
    if (enable)
        *enable = 0x0;
    volatile long *counter = DDRO_COUNT;
    long result = *counter;
    *counter = 0x0;

    // release mutex
    xSemaphoreGive(xDDRO_Mutex);
    return result;
}
