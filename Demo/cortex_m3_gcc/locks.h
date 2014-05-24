#ifndef LOCKS_H_
#define LOCKS_H_

#include "FreeRTOS.h"
#include "../../Source/include/semphr.h"

// binary semaphores to synchronize tasks
extern xSemaphoreHandle xSendACK_BinarySemphr;
extern xSemaphoreHandle xRecvACK_BinarySemphr;
extern xSemaphoreHandle xDDRO_BinarySemphr;

// mutexes to lock resource
extern xSemaphoreHandle xPrint_Mutex;
extern xSemaphoreHandle xScan_Mutex;
extern xSemaphoreHandle xDDRO_Mutex;
extern xSemaphoreHandle xREQ_Mutex;


extern void init_locks(void);

#endif
