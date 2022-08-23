/*
 * fos_mutex.h
 *
 *  Created on: Aug 23, 2022
 *      Author: Felix
 */

#ifndef INC_FOS_MUTEX_H_
#define INC_FOS_MUTEX_H_

#include "fos_def.h"
#include "fos_task.h"
#include "fos_queue.h"

typedef struct _FOS_Mutex_ {
	uint8_t ref;
	FOS_QueueNode queue;
} FOS_Mutex;

void FOS_MutexInit(FOS_Mutex *mutex, uint8_t threshold);
void FOS_MutexLock(FOS_Mutex *mutex);
void FOS_MutexUnlock(FOS_Mutex *mutex);

#endif /* INC_FOS_MUTEX_H_ */
