/*
 * fos_mutex.c
 *
 *  Created on: Aug 23, 2022
 *      Author: Felix
 */

#include "fos_mutex.h"

/*
 * When lock,
 * Queue: node(1)
 * Queue: node(1) node(2)
 * Queue: node(1) node(2) node(3)
 *
 * When unlock,
 * Queue: none
 * All tasks will be ready at the same time.
 */

extern FOS_QueueNode *cur_task_node;
extern uint8_t last_priority;
extern FOS_QueueNode _task_queue_ready_priority[PRIORITY_MAX];
extern FOS_QueueNode _task_queue_sleep;

void FOS_MutexInit(FOS_Mutex *mutex, uint8_t threshold) {
	mutex->ref = threshold;
	FOS_QueueNodeInit(&mutex->queue, 0);
}

void FOS_MutexLock(FOS_Mutex *mutex) {
	FOS_DISABLE_IRQ();
	if (mutex->ref == 0) {
		FOS_Task *_task = (FOS_Task*) cur_task_node->value;
		uint8_t _priority = _task->priority_state >> 2;
		FOS_QueueNode *_cur_priority_node = &_task_queue_ready_priority[_priority];
		_task->priority_state = (_priority << 2) + TaskState_Suspend;
		if (cur_task_node->nextNode == cur_task_node) {
			_cur_priority_node->value = 0;
		} else {
			_cur_priority_node->value = cur_task_node->nextNode;
		}
		FOS_QueueRemoveNode(cur_task_node);
		FOS_QueueAddNext(&mutex->queue, cur_task_node);
		last_priority = PRIORITY_MAX + 1;
		FOS_TaskSchedule();
	} else {
		mutex->ref--;
		FOS_ENABLE_IRQ();
	}
}

void FOS_MutexUnlock(FOS_Mutex *mutex) {
	FOS_DISABLE_IRQ();
	mutex->ref++;
	FOS_QueueNode *_node = mutex->queue.nextNode;
	if (_node != &mutex->queue) {
		while (1) {
			FOS_QueueNode *_next_node = _node->nextNode;
			//Recover
			FOS_Task *_task = (FOS_Task*) _node->value;
			uint8_t _priority = _task->priority_state >> 2;
			FOS_QueueNode *_cur_priority_node = &_task_queue_ready_priority[_priority];
			FOS_QueueRemoveNode(_node);
			/*
			 * The tasks that can get into Mutex Queue must be Ready State.
			 * Just make them resume to Ready State.
			 */
			_task->priority_state = (_priority << 2) + TaskState_Ready;
			if (_cur_priority_node->value == 0) {
				_cur_priority_node->value = _node;
			} else {
				FOS_QueueAddRoundTail(_cur_priority_node->value, _node);
			}
			//The next
			if (_next_node != _node) {
				_node = _next_node;
			} else {
				break;
			}
		}
		last_priority = PRIORITY_MAX + 1;
		FOS_TaskSchedule();
	} else {
		FOS_ENABLE_IRQ();
	}
}
