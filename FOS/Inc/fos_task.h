/*
 * fos_task.h
 *
 *  Created on: Aug 23, 2022
 *      Author: Felix
 */

#ifndef INC_FOS_TASK_H_
#define INC_FOS_TASK_H_

#include "fos_def.h"
#include "fos_queue.h"

#define FOS_GetStackTop(task_stack) task_stack + sizeof(task_stack) / 4

typedef struct _FOS_Task {
	addr_t sp;                   //Stack Pointer must be the first
	TaskFun entry;               //The function entry
	uint8_t priority_state;      //[7:2]: priority   [1:0]:state
	addr_t param;                //The parameter passed to task
	uint16_t timeout;            //Decreasing counter to wake  range in [0, 65535]
} FOS_Task, *pFOS_Task;

#define TaskState_Ready 0x0
#define TaskState_Suspend 0x1
#define TaskState_Sleep 0x2

void FOS_TaskInit();
void FOS_TaskCreate(FOS_QueueNode *node, FOS_Task *task, TaskFun entry, addr_t param, uint8_t priority, addr_t stack);
void FOS_TaskDelay(FOS_QueueNode *node, uint16_t timeout);
void FOS_TaskSuspend(FOS_QueueNode *node);
void FOS_TaskResume(FOS_QueueNode *node);
void FOS_TaskEnableSchedule();
void FOS_TaskDisableSchedule();
void FOS_TaskSchedule();

#endif //INC_FOS_TASK_H_
